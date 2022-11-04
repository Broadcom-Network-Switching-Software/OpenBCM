
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_LOCATION_TYPES_H__
#define __DNX_FIELD_TCAM_LOCATION_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_bank_types.h>


#define DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset) (((bank_id) * dnx_data_field.tcam.hw_bank_size_get(unit)) + bank_offset)

#define DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(absolute_address) (absolute_address / dnx_data_field.tcam.hw_bank_size_get(unit))

#define DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(absolute_address) (absolute_address % dnx_data_field.tcam.hw_bank_size_get(unit))

#define DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_id) ((bank_id)*dnx_data_field.tcam.nof_big_bank_lines_get(unit))

#define DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_id) (DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_id) + FIELD_TCAM_BANK_NOF_ENTRIES(bank_id) - 1)

#define DNX_FIELD_TCAM_LOCATION_IS_VALID(location) (FIELD_TCAM_BANK_IS_VALID(DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location)) && DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(location)<FIELD_TCAM_BANK_NOF_ENTRIES(DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location)))

#define DNX_FIELD_TCAM_LOCATION_COMPLEMENTARY_GET(location) ((location) + 1 - (2 * ((location) % 2)))

#define DNX_FIELD_TCAM_LOCATION_LINE_CONTAINS(line, location) (((line) == (location)) || (DNX_FIELD_TCAM_LOCATION_COMPLEMENTARY_GET((line)) == location))

#define DNX_FIELD_TCAM_LOCATION_LINE_GET(location) ((location) - ((location) % 2))

#define DNX_FIELD_TCAM_LOCATION_POLARITY(location1, location2) (DNX_FIELD_TCAM_LOCATION_LINE_GET((location1)) + ((location2) % 2))

#define DNX_FIELD_TCAM_LOCATION_INVALID -1



typedef struct {
    int bank_id;
    int bank_offset;
} dnx_field_tcam_location_t;

typedef struct {
    uint32 min;
    uint32 max;
} dnx_field_tcam_location_range_t;

typedef struct {
    uint32 priority;
    uint8 handler_id;
} dnx_field_tcam_location_prio_list_key_t;

typedef struct {
    sw_state_occ_bm_t* entries_occupation_bitmap;
    uint32* prio_list_first_iter;
    uint32* prio_list_last_iter;
} dnx_field_tcam_location_handler_info_t;

typedef struct {
    sw_state_sorted_ll_t* priorities_range;
    dnx_field_tcam_location_handler_info_t* tcam_handlers_info;
    uint32* location_priority_arr[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    sw_state_occ_bm_t* tcam_banks_occupation_bitmap;
} dnx_field_tcam_location_sw_t;


#endif 
