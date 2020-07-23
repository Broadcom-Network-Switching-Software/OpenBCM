/** \file appl_ref_sys_db.c
 * 
 *
 * System data base application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/** Include files. */
/** { */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <soc/drv.h>
#include "appl_ref_sys_db.h"
#include "appl_ref_sys_db_utils.h"
/** } */

/** DEFINEs */
/** { */

#define INVALID_ENTRY_INDEX -1

/** each entry is composed of mapping info and db management objects */
typedef struct
{
    /** system port mapping info */
    system_port_mapping_t sys_port_map_info;

    /** index of next entry */
    int next_entry_index;

    /** index of previous entry */
    int prev_entry_index;

    /** indication that entry is used */
    int valid_entry;
} system_port_db_entry_t;

/** system port data base */
struct system_port_db_t
{
    /** array containing the actual mapping */
    system_port_db_entry_t entries[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_SYSTEM_PORTS];

    /** number of entries in array */
    int count;

    /** index of the first entry valid entry */
    int first_entry_index;
};

/** } */

/** MACROs */
/** { */
/** } */

/** allocate and init values of system port DB - see header */
shr_error_e
appl_dnx_system_port_db_create(
    int unit,
    system_port_db_t ** system_port_db_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(system_port_db_ptr, _SHR_E_PARAM, "system_port_db_ptr");

    /** allocate DB */
    SHR_ALLOC_SET_ZERO(*system_port_db_ptr, sizeof(system_port_db_t), "system_port_db", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** set initial values */
    (*system_port_db_ptr)->first_entry_index = INVALID_ENTRY_INDEX;

exit:
    SHR_FUNC_EXIT;
}

/** free system port DB - see header */
shr_error_e
appl_dnx_system_port_db_destroy(
    int unit,
    system_port_db_t ** system_port_db_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(system_port_db_ptr, _SHR_E_PARAM, "system_port_db_ptr");

    /** free the data base */
    SHR_FREE(*system_port_db_ptr);

exit:
    SHR_FUNC_EXIT;
}

/** add a single new entry - see header */
shr_error_e
appl_dnx_system_port_db_add(
    int unit,
    system_port_db_t * system_port_db,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info)
{
    system_port_db_entry_t entry_to_add;
    SHR_FUNC_INIT_VARS(unit);

    /** Check valid data base pointer */
    SHR_NULL_CHECK(system_port_db, _SHR_E_PARAM, "system_port_db");
    SHR_NULL_CHECK(sys_port_map_info, _SHR_E_PARAM, "sys_port_map_info");

    /** Check valid system port ID */
    if ((system_port_id < 0) || (system_port_id >= DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_SYSTEM_PORTS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port %d is not in valid range", system_port_id);
    }

    /** Check that entry is not used */
    if (system_port_db->entries[system_port_id].valid_entry != 0)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "system port %d is already in use", system_port_id);
    }

    /** Update entry info - corner case of first entry is covered by init it to INVALID_ENTRY_INDEX during creation */
    /** each new entry is added at the beginning of the linked list, first entry's next is entry is pointing to INVALID index */
    entry_to_add.sys_port_map_info = *sys_port_map_info;
    entry_to_add.valid_entry = 1;
    entry_to_add.next_entry_index = system_port_db->first_entry_index;
    entry_to_add.prev_entry_index = INVALID_ENTRY_INDEX;

    /** update system port id to mapping info */
    entry_to_add.sys_port_map_info.system_port_id = system_port_id;

    /** Set entry to DB */
    system_port_db->entries[system_port_id] = entry_to_add;

    /** update previous member of previous first to new first if previous member exists */
    if (system_port_db->count > 0)
    {
        system_port_db->entries[system_port_db->first_entry_index].prev_entry_index = system_port_id;
    }

    /** Change first to last added entry */
    system_port_db->first_entry_index = system_port_id;

    /** Add 1 to count */
    ++(system_port_db->count);

exit:
    SHR_FUNC_EXIT;
}

/** remove a single entry and return its mapping info to user - see header */
shr_error_e
appl_dnx_system_port_db_remove(
    int unit,
    system_port_db_t * system_port_db,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info)
{
    static const system_port_db_entry_t empty_entry = {.valid_entry = 0 };
    system_port_db_entry_t *removed_entry;
    SHR_FUNC_INIT_VARS(unit);

    /** Check valid data base pointer */
    SHR_NULL_CHECK(system_port_db, _SHR_E_PARAM, "system_port_db");
    SHR_NULL_CHECK(sys_port_map_info, _SHR_E_PARAM, "sys_port_map_info");

    /** Check valid system port ID */
    if ((system_port_id < 0) || (system_port_id >= DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_SYSTEM_PORTS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port %d is not in valid range", system_port_id);
    }

    /** Check that entry is used */
    if (system_port_db->entries[system_port_id].valid_entry == 0)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "system port %d is not in use", system_port_id);
    }

    /** get pointer to removed entry */
    removed_entry = &system_port_db->entries[system_port_id];

    /** get removed data to return to user */
    *sys_port_map_info = removed_entry->sys_port_map_info;

    /** handle corner case of single member */
    /** member is both first and last */
    if (system_port_db->count == 1)
    {
        system_port_db->first_entry_index = INVALID_ENTRY_INDEX;
    }
    else
    {
        /** removed is not last */
        if (removed_entry->next_entry_index != INVALID_ENTRY_INDEX)
        {
            system_port_db->entries[removed_entry->next_entry_index].prev_entry_index = removed_entry->prev_entry_index;
        }

        /** removed is not first */
        if (removed_entry->prev_entry_index != INVALID_ENTRY_INDEX)
        {
            system_port_db->entries[removed_entry->prev_entry_index].next_entry_index = removed_entry->next_entry_index;
        }
    }

    /** Remove entry from DB by overriding it with an empty entry */
    *removed_entry = empty_entry;

    /** remove 1 from count */
    --(system_port_db->count);

exit:
    SHR_FUNC_EXIT;
}

/** get a single entry - see header */
shr_error_e
appl_dnx_system_port_db_get(
    int unit,
    system_port_db_t * system_port_db,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check valid data base pointer */
    SHR_NULL_CHECK(system_port_db, _SHR_E_PARAM, "system_port_db");
    SHR_NULL_CHECK(sys_port_map_info, _SHR_E_PARAM, "sys_port_map_info");

    /** Check valid system port ID */
    if ((system_port_id < 0) || (system_port_id >= DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_SYSTEM_PORTS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port %d is not in valid range", system_port_id);
    }

    /** Check that entry is used */
    if (system_port_db->entries[system_port_id].valid_entry == 0)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "system port %d is not in use", system_port_id);
    }

    *sys_port_map_info = system_port_db->entries[system_port_id].sys_port_map_info;

exit:
    SHR_FUNC_EXIT;
}

/** iterate over entries and use callback - see header */
shr_error_e
appl_dnx_system_port_db_iterate(
    int unit,
    system_port_db_t * system_port_db,
    appl_dnx_system_port_db_iter_cb callback_function,
    void *opaque_data_pointer)
{
    int current_system_port_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Check valid data base pointer */
    SHR_NULL_CHECK(system_port_db, _SHR_E_PARAM, "system_port_db");

    /** get first system port id */
    current_system_port_id = system_port_db->first_entry_index;

    /** iterate over all system ports */
    while (current_system_port_id != INVALID_ENTRY_INDEX)
    {
        /** get pointer to current entry in DB */
        system_port_db_entry_t *current_entry = &system_port_db->entries[current_system_port_id];

        /** call callback function with current system port */
        SHR_IF_ERR_EXIT(callback_function
                        (unit, current_system_port_id, &current_entry->sys_port_map_info, opaque_data_pointer));

        /** move current system port to next one */
        current_system_port_id = current_entry->next_entry_index;
    }

exit:
    SHR_FUNC_EXIT;
}

/** check if mapping in criteria is matching mapping in input  */
/** currently only supporting port_type as a matching criteria */
static shr_error_e
appl_dnx_system_port_db_port_mapping_is_match(
    int unit,
    const system_port_mapping_t * matching_criteria,
    const system_port_mapping_t * input_mapping,
    int *is_match)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_match, _SHR_E_PARAM, "is_match");

    *is_match = 1;
    if ((matching_criteria->port_type != APPL_PORT_TYPE_INVALID) &&
        (matching_criteria->port_type != input_mapping->port_type))
    {
        *is_match = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/** Get multiple system port mappings info from DB. according to matching criteria - see header */
shr_error_e
appl_dnx_system_port_db_get_multiple(
    int unit,
    system_port_db_t * system_port_db,
    const system_port_mapping_t * matching_criteria,
    int max_nof_ports,
    system_port_mapping_t * sys_port_map_array,
    int *nof_ports)
{
    int current_system_port_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Check valid data base pointer */
    SHR_NULL_CHECK(system_port_db, _SHR_E_PARAM, "system_port_db");
    SHR_NULL_CHECK(matching_criteria, _SHR_E_PARAM, "matching_criteria");
    SHR_NULL_CHECK(sys_port_map_array, _SHR_E_PARAM, "sys_port_map_array");
    SHR_NULL_CHECK(nof_ports, _SHR_E_PARAM, "nof_ports");

    /** get first system port id */
    current_system_port_id = system_port_db->first_entry_index;

    /** set count to 0 */
    *nof_ports = 0;

    /** iterate over all system ports */
    while (current_system_port_id != INVALID_ENTRY_INDEX)
    {
        int is_match = 0;

        /** get pointer to current entry in DB */
        system_port_db_entry_t *current_entry = &system_port_db->entries[current_system_port_id];

        /** Check if entry is matching to search criteria */
        SHR_IF_ERR_EXIT(appl_dnx_system_port_db_port_mapping_is_match
                        (unit, matching_criteria, &current_entry->sys_port_map_info, &is_match));

        /** if match was found - get entry if there's remaining place */
        if (is_match)
        {
            if (*nof_ports < max_nof_ports)
            {
                sys_port_map_array[*nof_ports] = current_entry->sys_port_map_info;
            }
            ++(*nof_ports);
        }

        /** move current system port to next one */
        current_system_port_id = current_entry->next_entry_index;
    }

exit:
    SHR_FUNC_EXIT;
}

/** populate with entries at init based on soc properties - see header */
shr_error_e
appl_dnx_system_port_db_init(
    int unit,
    system_port_db_t * system_port_db,
    int nof_devices)
{
    SHR_FUNC_INIT_VARS(unit);

    /** parse all possible system ports properties and see if exist */
    for (int sys_port_iter = 0; sys_port_iter < DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_SYSTEM_PORTS; ++sys_port_iter)
    {
        char *sys_port_prop;
        system_port_mapping_t sys_port_map;

        /** read soc prop string */
        sys_port_prop = soc_property_suffix_num_only_suffix_str_get(unit, sys_port_iter, spn_APPL_PARAM, "sys_port");

        /** if not found continue to next iteration */
        if (sys_port_prop == NULL)
        {
            continue;
        }

        /** read tokens verify input and fill struct */
        SHR_IF_ERR_EXIT(appl_dnx_sys_db_utils_prop_interpret(unit, sys_port_prop, &sys_port_map));
        sys_port_map.system_port_id = sys_port_iter;

        /** set mapping to sys_params after a sanity check, a more extensive test is done for each device on its own ports */
        if (sys_port_map.device_index < nof_devices)
        {
            SHR_IF_ERR_EXIT(appl_dnx_system_port_db_add(unit, system_port_db, sys_port_iter, &sys_port_map));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Device index (%d) is invalid, "
                         "there are only %d devices in the system, "
                         "index has to be (0-%d)\n", sys_port_map.device_index, nof_devices, nof_devices - 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
