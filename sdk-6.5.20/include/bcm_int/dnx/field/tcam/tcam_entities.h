/**
* \file        tcam_entities.h
*
* DESCRIPTION :
*       This file includes a representation for the TCAM database struct used as an input
*       by the TCAM module to create a new tcam handler.
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef TCAM_DATABASE_H_INCLUDED
/** { */
#define TCAM_DATABASE_H_INCLUDED

#include <include/bcm_int/dnx/field/field.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>

/**
 * This struct contains the information needed by the TCAM in order
 * to add an entry.
 */
typedef struct
{
    /**
     * The id of this entry
     */
    uint32 id;
    /**
     * the priority of this entry (determines the location of this entry
     * in TCAM relatively to other entries in the same database)
     */
    uint32 priority;
    /**
     * Specifies whether entry is valid.
     * (Non-valid entries exist in SW/HW but are not active
     */
    uint8 valid_bit;
} dnx_field_tcam_entry_t;

/**
 * This struct holds information about database that is needed by TCAM module.
 * It's used as an input parameter when creating a new handler.
 */
typedef struct
{
    /**
     * The stage of this database
     */
    dnx_field_tcam_stage_e stage;
    /**
     * specifies whether this database is direct_extraction/direct_TCAM or normal
     * TCAM database
     */
    uint8 is_direct;
    /**
     * The key size of this database either 80/160/320b
     */
    dnx_field_key_length_type_e key_size;
    /**
     * The action size of this database either 32(lsb)/32(msb)/64/128b
     */
    dnx_field_action_length_type_e action_size;
    /**
     * The total actual number of bits used by the database as action.
     */
    uint32 actual_action_size;
    /**
     * Restricted TCAM handlers ids which are not allowed to share banks with the
     * current TCAM handler to be created.
     */
    uint32 restricted_tcam_handlers_ids[DNX_DATA_MAX_FIELD_TCAM_NOF_TCAM_HANDLERS];
    /**
     * Number of restricted TCAM handlers
     */
    uint32 nof_restricted_tcam_handlers;
    /**
     * Prefix size
     */
    uint32 prefix_size;
    /**
     * Prefix value
     */
    uint32 prefix_value;
    /**
     * Bank allocation mode
     */
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;
    /*
     * Number of banks to allocate (for Select mode)
     */
    int nof_banks_to_alloc;
    /**
     * Bank IDs to allocate (For "Select" mode)
     */
    int banks_to_alloc[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
} dnx_field_tcam_database_t;

/**
 * \brief
 *  Inits the TCAM database info, which is used as an input to
 *  "dnx_field_tcam_handler_create()"
 *
 * \param[in]     unit - Device ID
 * \param[in,out] db   - Database to Init (should be allocated by user)
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_database_t_init(
    int unit,
    dnx_field_tcam_database_t * db);

/** } */
#endif
