/*! \file bcmltm_ha_init.c
 *
 * Logical Table Manager
 *
 * This module contains an implementation sufficient to permit management
 * of a direct-indexed table.  Most operations of the LTM are stubs.
 *
 * Translate LT (field,value) entry components to PT fields in HW entry format
 * Provide Logical resource manager stub
 * Provide LTM state update stub
 * Invoke PTM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bsl/bsl.h>

#include <bcmcfg/comp/bcmcfg_ltm_resources.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd_config.h>
#include <bcmptm/bcmptm.h>

#include <bcmltm/bcmltm_ha_internal.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_dump.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_HA

static bcmltm_ha_block_info_t
ha_block_info[BCMDRD_CONFIG_MAX_UNITS][BCMLTM_HA_BLOCK_TYPE_COUNT];

static bool issu_enabled[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */


/*******************************************************************************
 * Public functions
 */

void
bcmltm_issu_enable(int unit)
{
    issu_enabled[unit] = true;
}

uint8_t
bcmltm_ha_transaction_max(int unit)
{
    uint8_t max_tables_per_transaction = BCMLTM_TRANSACTION_LT_MAX;
    bcmcfg_feature_ctl_config_t feature_conf;
    bcmcfg_ltm_resources_config_t ltm_conf;
    int rv;

    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_atomic_trans == 0)) {
        /* Atomic transactions disabled, no transaction records kept. */
        max_tables_per_transaction = 0;
    } else {
        rv = bcmcfg_ltm_resources_config_get(&ltm_conf);
        if (SHR_FAILURE(rv) && ltm_conf.max_tables_transaction != 0) {
            max_tables_per_transaction = ltm_conf.max_tables_transaction;
        }
    }

    return max_tables_per_transaction;
}

int
bcmltm_ha_init(int unit,
               bool warm,
               bcmltm_ha_state_sizes_t *ha_state_sizes)
{
    uint32_t block_size, block_size_alloc;
    uint8_t trans_lt_max;
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_info_t *block_info;
    bcmltm_state_mgmt_t *state_mgmt;
    bcmltm_ha_block_header_t *block_header;
    bool no_atomic = FALSE;
    char *ha_block_id = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Prepare High Availability records of Logical Table state
     * for use by the LT metadata creation system.
     */

    LOG_VERBOSE(BSL_LS_BCMLTM_HA,
                (BSL_META_U(unit,
                            "Logical Table Manager High Availability initialization\n")));

    trans_lt_max = bcmltm_ha_transaction_max(unit);
    no_atomic = (trans_lt_max == 0);

    state_mgmt = bcmltm_state_mgmt_get(unit);

    state_mgmt->maximum_state_size = ha_state_sizes->maximum_state_size;
    state_mgmt->maximum_state_data_size =
        ha_state_sizes->maximum_state_data_size;
    state_mgmt->lt_trans_max = trans_lt_max;

    if (warm && issu_enabled[unit]) {
        SHR_IF_ERR_EXIT(bcmltm_unit_state_upgrade(unit, ha_state_sizes));
    }

    /* Cold Boot */
    for (block_type = BCMLTM_HA_BLOCK_TYPE_STATE;
         block_type < BCMLTM_HA_BLOCK_TYPE_COUNT;
         block_type++) {
        block_info = &(ha_block_info[unit][block_type]);

        /* Redundant for now, but this record may be useful later. */
        block_info->block_type = block_type;
        block_size = 0;

        switch (block_type) {
        case BCMLTM_HA_BLOCK_TYPE_STATE:
            block_size =
                ha_state_sizes->total_state_size;
            ha_block_id = "ltmState";
            break;

        case BCMLTM_HA_BLOCK_TYPE_STATE_DATA:
            block_size =
                ha_state_sizes->total_state_data_size;
            ha_block_id = "ltmStateData";
            break;

        case BCMLTM_HA_BLOCK_TYPE_TRANS_STATUS:
            if (no_atomic) {
                /* Transaction record not used */
                continue;
            }
            block_size =
                (trans_lt_max * sizeof(uint32_t)) +
                sizeof(bcmltm_transaction_status_t);
            ha_block_id = "ltmTransStatus";
            break;

        case BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE:
            if (no_atomic) {
                /* Rollback state not used */
                continue;
            }
            block_size =
                trans_lt_max *
                ha_state_sizes->maximum_state_size;
            ha_block_id = "ltmRbState";
            break;

        case BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE_DATA:
            if (no_atomic) {
                /* Rollback state not used */
                continue;
            }
            block_size =
                trans_lt_max *
                ha_state_sizes->maximum_state_data_size;
            ha_block_id = "ltmRbStateData";
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Add the block header for HA recovery identification */
        block_size += sizeof(bcmltm_ha_block_header_t);
        block_size_alloc = block_size;
        block_info->ha_block_ptr =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_LTM_COMP_ID,
                             block_type,
                             ha_block_id,
                             &block_size_alloc);
        SHR_NULL_CHECK(block_info->ha_block_ptr, SHR_E_MEMORY);
        if (block_size_alloc < block_size) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "LTM block %d: "
                                  "requested bytes = %0u, "
                                  "allocated bytes = %0u\n"),
                       block_type,
                       (uint32_t)block_size,
                       (uint32_t)block_size_alloc));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        /* Block header init */
        block_header = (bcmltm_ha_block_header_t *)(block_info->ha_block_ptr);

        if (!warm ||
            (block_header->signature != BCMLTM_BLOCK_SIGNATURE(block_type))) {
            /* Cold Boot (or corrupt HA) */
            sal_memset(block_header, 0, block_size_alloc);
            block_header->block_size = block_size_alloc;
            block_header->signature = BCMLTM_BLOCK_SIGNATURE(block_type);
            block_header->allocated_size = sizeof(bcmltm_ha_block_header_t);
        } else {
            /* Recovery */
            if (block_size_alloc < block_header->block_size) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "LTM block %d: "
                                      "expected bytes = %0u, "
                                      "recovered bytes = %0u\n"),
                           block_type,
                           (uint32_t)block_header->block_size,
                           (uint32_t)block_size_alloc));
            }
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_ha_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}


void
bcmltm_ha_cleanup(int unit)
{
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_info_t *block_info;

    for (block_type = BCMLTM_HA_BLOCK_TYPE_STATE;
         block_type < BCMLTM_HA_BLOCK_TYPE_COUNT;
         block_type++) {
        block_info = &(ha_block_info[unit][block_type]);
        block_info->ha_block_ptr = NULL;
        /* We do not free HA memory.
         * It is either discarded by the application or used
         * for recovery.
         */
    }

    return;
}

bcmltm_ha_ptr_t
bcmltm_ha_blk_get(int unit,
                  bcmltm_ha_block_type_t block_type)
{
    bcmltm_ha_block_info_t *block_info;
    bcmltm_ha_ptr_t segment_ptr;

    block_info = &(ha_block_info[unit][block_type]);
    if (!block_info->ha_block_ptr) {
       return BCMLTM_HA_PTR_INVALID;
    }
    segment_ptr = BCMLTM_HA_PTR(block_type, sizeof(bcmltm_ha_block_header_t));
    return segment_ptr;
}

bcmltm_ha_ptr_t
bcmltm_ha_alloc(int unit,
                bcmltm_ha_block_type_t block_type,
                uint32_t size)
{
    uint32_t aligned_size;
    bcmltm_ha_ptr_t segment_ptr = BCMLTM_HA_PTR_INVALID;
    bcmltm_ha_block_info_t *block_info;
    bcmltm_ha_block_header_t *block_header;

    BCMLTM_HA_PTR_INVALID_SET(segment_ptr);
    aligned_size = BCMLTM_WORDSIZEBYTES(size);

    block_info = &(ha_block_info[unit][block_type]);
    block_header = block_info->ha_block_ptr;

    if ((block_header->block_size - block_header->allocated_size) <
        aligned_size) {
        LOG_ERROR(BSL_LS_BCMLTM_HA,
                  (BSL_META_U(unit,
                              "Logical Table High Availability -\n"
                              "\tinsufficient space for LT state in block %d\n"
                              "Sizes:  block=%d allocated=%d aligned=%d\n"),
                   block_type,
                   block_header->block_size,
                   block_header->allocated_size,
                   aligned_size));
    } else {
        /* Find start of free space. */
        segment_ptr = BCMLTM_HA_PTR(block_type, block_header->allocated_size);
        /* Update allocated size. */
        block_header->allocated_size += aligned_size;
    }

    return segment_ptr;
}

void
bcmltm_ha_reset(int unit)
{
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_info_t *block_info;
    void *clear_ptr;
    uint32_t clear_size;
    bcmltm_ha_block_header_t *block_header;

    for (block_type = BCMLTM_HA_BLOCK_TYPE_STATE;
         block_type < BCMLTM_HA_BLOCK_TYPE_COUNT;
         block_type++) {
        block_info = &(ha_block_info[unit][block_type]);
        block_header = block_info->ha_block_ptr;

        block_header->allocated_size = sizeof(bcmltm_ha_block_header_t);
        clear_ptr = ((char *)block_header) + block_header->allocated_size;
        clear_size = block_header->block_size - block_header->allocated_size;
        sal_memset(clear_ptr, 0, clear_size);
    }
}

void *
bcmltm_ha_ptr_to_mem_ptr(int unit,
                         bcmltm_ha_ptr_t ha_ptr)
{
    uint8_t *ptr_math;
    bcmltm_ha_block_info_t *block_info;

    if (BCMLTM_HA_PTR_IS_INVALID(ha_ptr)) {
        return NULL;
    }

    block_info =
        &(ha_block_info[unit][BCMLTM_HA_PTR_TO_BLOCK_TYPE(ha_ptr)]);
    ptr_math = (uint8_t *)block_info->ha_block_ptr +
        BCMLTM_HA_PTR_OFFSET(ha_ptr);
    return ptr_math;
}

bcmltm_ha_ptr_t
bcmltm_ha_ptr_increment(int unit,
                        bcmltm_ha_ptr_t ha_ptr,
                        uint32_t increment)
{
    bcmltm_ha_ptr_t new_ha_ptr;
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_info_t *block_info;
    bcmltm_ha_block_header_t *block_header;
    uint32_t offset;

    if (BCMLTM_HA_PTR_IS_INVALID(ha_ptr)) {
        return BCMLTM_HA_PTR_INVALID;
    }

    block_type = BCMLTM_HA_PTR_TO_BLOCK_TYPE(ha_ptr);
    offset = BCMLTM_HA_PTR_OFFSET(ha_ptr);
    block_info = &(ha_block_info[unit][block_type]);
    block_header = block_info->ha_block_ptr;

    /* Increment, word-aligned. */
    offset += BCMLTM_WORDSIZEBYTES(increment);
    if (offset >= block_header->block_size) {
        return BCMLTM_HA_PTR_INVALID;
    }

    new_ha_ptr = BCMLTM_HA_PTR(block_type, offset);
    return new_ha_ptr;
}

void
bcmltm_ha_blocks_dump(int unit,
                      shr_pb_t *pb)
{
    bcmltm_ha_block_info_t *block_info;
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_header_t *block_header;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "=== HA block info for Unit %d ===\n", unit);
    for (block_type = BCMLTM_HA_BLOCK_TYPE_STATE;
         block_type < BCMLTM_HA_BLOCK_TYPE_COUNT;
         block_type++) {
        block_info = &(ha_block_info[unit][block_type]);
        block_header = block_info->ha_block_ptr;

        if (block_header != NULL) {
            shr_pb_printf(pb, "    Block type %d\n", block_type);
            shr_pb_printf(pb, "        Block size %d, Allocated %d\n",
                          block_header->block_size,
                          block_header->allocated_size);
        }
    }

    SHR_PB_LOCAL_DONE();
}
