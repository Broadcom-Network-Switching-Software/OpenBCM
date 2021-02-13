/** \file dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h
 *
 * sw state types (structs/enums/typedefs)
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_LOCATION_TYPES_H__
#define __DNX_FIELD_TCAM_LOCATION_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_bank_types.h>
/*
 * MACROs
 */

/**
 * Returns the absolute address for the given (bank_id, bank_offset) pair
 */
#define DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset) (((bank_id) * dnx_data_field.tcam.hw_bank_size_get(unit)) + bank_offset)

/**
 * Returns the bank id for the given absolute address
 */
#define DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(absolute_address) (absolute_address / dnx_data_field.tcam.hw_bank_size_get(unit))

/**
 * Returns the bank offset for the given absolute address
 */
#define DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(absolute_address) (absolute_address % dnx_data_field.tcam.hw_bank_size_get(unit))

#define FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_id) ((bank_id)*dnx_data_field.tcam.nof_big_bank_lines_get(unit))

#define FIELD_TCAM_LOCATION_ALGO_MAX_BANK_INDEX(bank_id) (FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_id) + FIELD_TCAM_BANK_NOF_ENTRIES(bank_id) - 1)

#define FIELD_TCAM_LOCATION_IS_VALID(location) (FIELD_TCAM_BANK_IS_VALID(DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location)) && DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(location)<FIELD_TCAM_BANK_NOF_ENTRIES(DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location)))

#define FIELD_TCAM_LOCATION_INVALID -1

/*
 * STRUCTs
 */

/**
 * Used to describe a tcam location which is composed of bank id and bank offset
 */
typedef struct {
    /**
     * The id of the TCAM bank
     */
    int bank_id;
    /**
     * The offset inside the TCAM bank
     */
    int bank_offset;
} dnx_field_tcam_location_t;

/**
 * This struct represents a range consisting of min and max uint32 values as the range limits
 */
typedef struct {
    /**
     * Min limit
     */
    uint32 min;
    /**
     * Max limit
     */
    uint32 max;
} dnx_field_tcam_location_range_t;

/**
 * This struct represents a key for the priority list
 */
typedef struct {
    /**
     * Entry Priority
     */
    uint32 priority;
    /**
     * TCAM Handler ID
     */
    uint8 handler_id;
} dnx_field_tcam_location_prio_list_key_t;

/**
 * Contains information per handler that are relevant for the TCAM Location Manager
 */
typedef struct {
    /**
     * Occupation bitmap for this handler's entries
     */
    sw_state_occ_bm_t* entries_occupation_bitmap;
    /**
     * handler's first iterator in priority list
     */
    uint32* prio_list_first_iter;
    /**
     * handler's last iterator in priority list
     */
    uint32* prio_list_last_iter;
} dnx_field_tcam_location_handler_info_t;

/**
 * Holds all information needed for the TCAM Location Manager
 */
typedef struct {
    /**
     * This linked list enlists the currently added priorites, along with the range of each priority's entries in TCAM.
     */
    sw_state_sorted_ll_t* priorities_range;
    /**
     * Information for each tcam handler relevant to TCAM Location Manager
     */
    dnx_field_tcam_location_handler_info_t* tcam_handlers_info;
    /**
     * Array that saves for each TCAM location the priority of the entry at the given location
     */
    uint32* location_priority_arr[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    /**
     * Occupation bitmap per half entry that represents all the TCAM banks
     */
    sw_state_occ_bm_t* tcam_banks_occupation_bitmap;
} dnx_field_tcam_location_sw_t;


#endif /* __DNX_FIELD_TCAM_LOCATION_TYPES_H__ */
