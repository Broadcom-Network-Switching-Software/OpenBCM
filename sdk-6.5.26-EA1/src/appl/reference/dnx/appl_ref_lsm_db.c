/** \file appl_ref_lsm_db.c
 *
 * This file is an implemantation of the database that holds all qualifiers and actions
 * that are configured dynamically and used by the lsm diagnostic tool.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LSM

 /*
  * Include files.
  * {
  */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/error.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/rx/rx_trap_map.h>
#include <appl/reference/dnx/appl_ref_lsm.h>
/*
 * }
 */

/**
 * \brief
 *  pointer to qualifier table (qualifier database).
 */
static lsm_db_qualifier_table_t *Qualifier_table = NULL;
/**
 * \brief
 *  pointer to a list of action tables (action database).
 */
static lsm_db_action_table_t *Action_tables = NULL;
/**
 * \brief
 *  pointer to a list of qulifier fields. For memory optimization qualifier fields are uniqe and
 *  are being pointed at br qualifier info objects.
 */
static lsm_qualifiers_fields_t *Qualifier_fields = NULL;
/**
 * \brief
 *  number of qualifier fields currently in database.
 */
static uint32 Nof_qualifier_fields = 0;

/**
 * \brief
 *  pointer to a list of traps configured on init.
 */
static lsm_init_trap_t *Lsm_traps = NULL;
/**
 * \brief
 *  number of traps currently in database.
 */
static uint32 Lsm_nof_traps;

/**
 * \brief
 *  pointer to a list of interrupts configured on init that is dynamically allocated.
 */
static lsm_init_interrupt_t *Lsm_interrupts = NULL;
/**
 * \brief
 *  number of interrupts currently in database.
 */
static uint32 Lsm_nof_interrupts = 0;

/**
 * \brief - logic to determine if given set of parameters should result in a 'hit' or 'miss'.
 *
 * \param [in] unit - Unit ID
 * \param [in] user_qual_value - qualifier value retrieved from device data.
 * \param [in] db_qual_value - qualifier value retrieved from lsm database.
 * \param [in] qual_mask - qualifier mask retrieved from lsm database.
 * \param [in] lookup_operator - lookup operator to use in between two masked qualifier value to determine hit/miss.
 * \param [out] is_hit - return value to to mark if there was a hit.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
appl_ref_lsm_db_comperator(
    int unit,
    uint32 user_qual_value,
    uint32 db_qual_value,
    uint32 qual_mask,
    lsm_operator_e lookup_operator,
    uint8 *is_hit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_hit, BCM_E_INTERNAL, "is_hit");

    *is_hit = FALSE;
    switch (lookup_operator)
    {
        case lsm_operator_equals:
            *is_hit = (user_qual_value & qual_mask) == (db_qual_value & qual_mask);
            break;

        case lsm_operator_not_equals:
            *is_hit = (user_qual_value & qual_mask) != (db_qual_value & qual_mask);
            break;

        case lsm_operator_lower:
            *is_hit = (user_qual_value & qual_mask) < (db_qual_value & qual_mask);
            break;

        case lsm_operator_greater:
            *is_hit = (user_qual_value & qual_mask) > (db_qual_value & qual_mask);
            break;

        case lsm_operator_lower_equals:
            *is_hit = (user_qual_value & qual_mask) <= (db_qual_value & qual_mask);
            break;

        case lsm_operator_greater_equals:
            *is_hit = (user_qual_value & qual_mask) >= (db_qual_value & qual_mask);
            break;

        default:
            SHR_ERR_EXIT(BCM_E_INTERNAL, "invalid operator=%d.\n", LSM_MAX_NOF_TRAPS);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_trap_table_create(
    int unit,
    uint32 nof_traps,
    lsm_init_trap_t * traps)
{
    uint32 index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(traps, BCM_E_PARAM, "traps");

    if (nof_traps > LSM_MAX_NOF_TRAPS)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "too many traps, max number of traps=%d but found %d.\n", LSM_MAX_NOF_TRAPS,
                     nof_traps);
    }

    SHR_ALLOC_SET_ZERO_ERR_EXIT(Lsm_traps, sizeof(lsm_init_trap_t) * nof_traps,
                                "Traps", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (index = 0; index < nof_traps; ++index)
    {
        Lsm_traps[index].trap_type = traps[index].trap_type;
    }

    Lsm_nof_traps = nof_traps;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_interrupt_table_create(
    int unit,
    uint32 nof_interrupts,
    lsm_init_interrupt_t * interrupts)
{
    uint32 index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(interrupts, BCM_E_PARAM, "interrupts");

    if (nof_interrupts > LSM_MAX_NOF_INTERRUPTS)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "too many interrupts, max number of interrupt=%d but found %d.\n", nof_interrupts,
                     LSM_MAX_NOF_INTERRUPTS);
    }

    SHR_ALLOC_SET_ZERO_ERR_EXIT(Lsm_interrupts, sizeof(lsm_init_interrupt_t) * nof_interrupts,
                                "interrupts", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (index = 0; index < nof_interrupts; ++index)
    {
        Lsm_interrupts[index].interrupt_id = interrupts[index].interrupt_id;
        sal_strncpy_s(Lsm_interrupts[index].name, interrupts[index].name, MAX_SIZE_STR);
    }

    Lsm_nof_interrupts = nof_interrupts;

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
appl_ref_lsm_db_trap_table_update(
    int unit,
    uint32 nof_traps,
    lsm_init_trap_t * traps)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(traps, BCM_E_PARAM, "traps");

    if (Lsm_interrupts == NULL)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL,
                     "appl_ref_lsm_db_trap_table_update called before appl_ref_lsm_db_trap_table_create.\n");
    }
    if (nof_traps > LSM_MAX_NOF_TRAPS)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "too many traps, max number of traps=%d but found %d.\n", nof_traps,
                     LSM_MAX_NOF_TRAPS);
    }

    if (nof_traps > LSM_MAX_NOF_TRAPS)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "too many traps, number of traps configured in create is=%d but found %d.\n",
                     nof_traps, Lsm_nof_interrupts);
    }

    sal_memcpy(Lsm_traps, traps, sizeof(lsm_init_trap_t) * Lsm_nof_traps);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_interrupt_table_update(
    int unit,
    uint32 nof_interrupts,
    lsm_init_interrupt_t * interrupts)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(interrupts, BCM_E_PARAM, "interrupts");

    if (nof_interrupts > LSM_MAX_NOF_INTERRUPTS)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "too many interrupts, max number of interrupts=%d but found %d.\n", nof_interrupts,
                     LSM_MAX_NOF_INTERRUPTS);
    }

    sal_memcpy(Lsm_interrupts, interrupts, sizeof(lsm_init_interrupt_t) * Lsm_nof_interrupts);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_trap_table_get(
    int unit,
    uint32 *nof_traps,
    lsm_init_trap_t * traps)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(traps, BCM_E_PARAM, "traps");
    SHR_NULL_CHECK(nof_traps, BCM_E_PARAM, "nof_traps");

    sal_memcpy(traps, Lsm_traps, sizeof(lsm_init_trap_t) * Lsm_nof_traps);
    *nof_traps = Lsm_nof_traps;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_interrupt_table_get(
    int unit,
    uint32 *nof_interrupts,
    lsm_init_interrupt_t * interrupts)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(interrupts, BCM_E_PARAM, "interrupts");
    SHR_NULL_CHECK(nof_interrupts, BCM_E_PARAM, "nof_interrupts");

    sal_memcpy(interrupts, Lsm_interrupts, sizeof(lsm_init_interrupt_t) * Lsm_nof_interrupts);
    *nof_interrupts = Lsm_nof_interrupts;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_trap_table_destroy(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE(Lsm_traps);

    Lsm_nof_traps = 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_interrupt_table_destroy(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE(Lsm_interrupts);

    Lsm_nof_interrupts = 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_nof_traps_get(
    int unit,
    uint32 *nof_traps)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_traps, BCM_E_PARAM, "nof_traps");

    *nof_traps = Lsm_nof_traps;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_nof_interrupts_get(
    int unit,
    uint32 *nof_interrupts)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_interrupts, BCM_E_PARAM, "nof_interrupts");

    *nof_interrupts = Lsm_nof_interrupts;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_qualifiers_db_table_create(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(Qualifier_fields,
                                sizeof(lsm_qualifiers_fields_t) * MAX_NOF_QUAL_ENTRIES * MAX_NOF_QUAL,
                                "Qualifier_fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(Qualifier_table, sizeof(lsm_db_qualifier_table_t), "Qualifier_table", "%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_qualifiers_db_table_destroy(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE(Qualifier_fields);
    SHR_FREE(Qualifier_table);

    Qualifier_fields = NULL;
    Qualifier_table = NULL;
    Nof_qualifier_fields = 0;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get index of qualifier in database according to qualifier name.
 *
 * \param [in] unit - Unit ID
 * \param [in] qualifier_name - qualifier name is unique in database
 * \param [out] qualifier_index  - the index of qualifier field in the database, if not found
 *                                   will have BCM_E_NOT_FOUND value.
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
appl_ref_lsm_qualifier_field_get(
    int unit,
    char *qualifier_name,
    int *qualifier_index)
{
    int q_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_name, BCM_E_PARAM, "qualifier_name is null");
    SHR_NULL_CHECK(qualifier_index, BCM_E_PARAM, "qualifier_index is null");

    *qualifier_index = BCM_E_NOT_FOUND;
    for (q_index = 0; q_index < Nof_qualifier_fields; q_index++)
    {
        if (sal_strncmp(qualifier_name, Qualifier_fields[q_index].name, MAX_SIZE_STR) == 0)
        {
            *qualifier_index = q_index;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_qualifiers_db_info_add(
    int unit,
    int nof_qual_info,
    lsm_qualifiers_fields_t * qualifier)
{
    int qualifier_field_rcv_index;
    int qualifier_field_db_index;
    SHR_FUNC_INIT_VARS(unit);

    for (qualifier_field_rcv_index = 0; qualifier_field_rcv_index < nof_qual_info; qualifier_field_rcv_index++)
    {
        /**
         * for optimization puposes, first verify if qualifier field is already in the database. if already exists no
         * need to do anything. The counter shouldn't incress as the search algorithem will handle the rest.
         */
        SHR_IF_ERR_EXIT(appl_ref_lsm_qualifier_field_get
                        (unit, qualifier[qualifier_field_rcv_index].name, &qualifier_field_db_index));
        if (qualifier_field_db_index == BCM_E_NOT_FOUND)
        {
            sal_memcpy(&Qualifier_fields[Nof_qualifier_fields], &qualifier[qualifier_field_rcv_index],
                       sizeof(Qualifier_fields[Nof_qualifier_fields]));
            Nof_qualifier_fields++;
        }

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_qualifiers_db_entry_update(
    int unit,
    uint32 entry_id,
    lsm_qualifiers_entry_t * qualifier)
{
    int qualifier_field_index, qualifier_element_index, qualifier_db_index;
    uint8 is_qualifier_exists;

    SHR_FUNC_INIT_VARS(unit);

    for (qualifier_element_index = 0; qualifier_element_index < qualifier->nof_qualifiers; qualifier_element_index++)
    {
        SHR_IF_ERR_EXIT(appl_ref_lsm_qualifier_field_get(unit,
                                                         qualifier->
                                                         list_of_qualifiers[qualifier_element_index].qualifer_field.
                                                         name, &qualifier_field_index));
        /**
         * appl_ref_lsm_qualifier_field_get will return the Nof_qualifier_fields value if the qualifier field was not found.
         */
        if (qualifier_field_index == BCM_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(BCM_E_NOT_FOUND,
                         "qualifier %s is not in field database make sure is was add through xml or other means first.\n",
                         qualifier->list_of_qualifiers[qualifier_element_index].qualifer_field.name);
        }

        /*
         * check if qualifier already in qualifier table and update only if not.
         */
        is_qualifier_exists = FALSE;
        for (qualifier_db_index = 0; qualifier_db_index < Qualifier_table->qualifier_entries[entry_id].nof_qualifiers;
             qualifier_db_index++)
        {
            if (sal_strncmp(Qualifier_table->qualifier_entries[entry_id].qualifiers[qualifier_db_index]->name,
                            qualifier->list_of_qualifiers[qualifier_element_index].qualifer_field.name,
                            MAX_SIZE_STR) == 0)
            {
                is_qualifier_exists = TRUE;
                break;
            }
        }

        if (is_qualifier_exists == FALSE)
        {
            Qualifier_table->qualifier_entries[entry_id].qualifiers[Qualifier_table->
                                                                    qualifier_entries[entry_id].nof_qualifiers] =
                &Qualifier_fields[qualifier_field_index];
            Qualifier_table->qualifier_entries[entry_id].nof_qualifiers++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_qualifiers_db_entry_get(
    int unit,
    uint32 entry_id,
    lsm_qualifiers_entry_t * qualifier)
{
    int qualifier_element_index;
    SHR_FUNC_INIT_VARS(unit);

    qualifier->nof_qualifiers = Qualifier_table->qualifier_entries[entry_id].nof_qualifiers;
    for (qualifier_element_index = 0; qualifier_element_index < qualifier->nof_qualifiers; ++qualifier_element_index)
    {
        sal_memcpy(&qualifier->list_of_qualifiers[qualifier_element_index].qualifer_field,
                   Qualifier_table->qualifier_entries[entry_id].qualifiers[qualifier_element_index],
                   sizeof(lsm_qualifiers_fields_t));
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_action_table_create(
    int unit,
    uint32 action_table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (Action_tables == NULL)
    {
        SHR_ALLOC_SET_ZERO_ERR_EXIT(Action_tables, sizeof(lsm_db_action_table_t) * MAX_NOF_QUAL_ENTRIES,
                                    "actions_tables", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        if (action_table_id)
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "first action table IDs must be 0 in the action xml file, got %d.\n",
                         action_table_id);
        }
    }
    else if (action_table_id != Qualifier_table->nof_entries)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "action table IDs must be ordered in the action xml file, got %d expected %d.\n",
                     action_table_id, Qualifier_table->nof_entries);
    }

    Qualifier_table->nof_entries++;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_action_table_destroy_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE(Action_tables);
    Action_tables = NULL;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_show_qualifiers(
    int unit)
{
    uint32 qual_index;
    lsm_qualifiers_fields_t *qualifier_ptr;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(Qualifier_fields, BCM_E_PARAM, "Qualifier_fields");

    PRT_TITLE_SET("Qualifiers of type 'signal', configued by LSM");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Signal");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("From");
    PRT_COLUMN_ADD("To");

    for (qual_index = 0; qual_index < Nof_qualifier_fields; ++qual_index)
    {
        qualifier_ptr = &Qualifier_fields[qual_index];
        if (qualifier_ptr->qualifier_type == lsm_qualifier_signal)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", qualifier_ptr->name);
            PRT_CELL_SET("%s", qualifier_ptr->qualifier_command.sig.signal_name);
            PRT_CELL_SET("%s", qualifier_ptr->qualifier_command.sig.signal_block);
            PRT_CELL_SET("%s", qualifier_ptr->qualifier_command.sig.signal_from);
            PRT_CELL_SET("%s", qualifier_ptr->qualifier_command.sig.signal_to);
        }
    }
    PRT_COMMIT;

    PRT_TITLE_SET("Qualifiers of type 'register', configued by LSM");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Register");
    PRT_COLUMN_ADD("Field");

    for (qual_index = 0; qual_index < Nof_qualifier_fields; ++qual_index)
    {
        qualifier_ptr = &Qualifier_fields[qual_index];
        if (qualifier_ptr->qualifier_type == lsm_qualifier_register)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", qualifier_ptr->name);
            PRT_CELL_SET("%s", qualifier_ptr->qualifier_command.reg.register_name);
            PRT_CELL_SET("%s", qualifier_ptr->qualifier_command.reg.field_name);
        }
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_show_actions(
    int unit,
    uint32 entry_id)
{
    /**
     * loop indeices
     */
    uint32 action_index;
    uint32 qual_index;

    lsm_db_actions_entry_t *actions_entry;
    lsm_db_qualifier_info_t *qualifier_info;
    uint32 max_elements;

    /**
     * lsm operator string value
     */
    char qual_op[LSM_OPERATOR_MAX_STRING_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(Action_tables, BCM_E_PARAM, "Action_tables");

    if (entry_id >= Action_tables->nof_entries)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "action table id %d does not exist, mad table ID is %d.", entry_id,
                     Action_tables->nof_entries - 1);
    }

    PRT_TITLE_SET("Action entries configued by LSM");
    PRT_COLUMN_ADD("#");
    PRT_COLUMN_ADD("Keys");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Mask");
    PRT_COLUMN_ADD("Op");
    PRT_COLUMN_ADD("Prio");
    PRT_COLUMN_ADD("Actions");

    for (action_index = 0; action_index < Action_tables[entry_id].nof_entries; ++action_index)
    {
        actions_entry = &(Action_tables[entry_id].actions_entries[action_index]);
        max_elements =
            actions_entry->nof_actions >= actions_entry->nof_qualifiers ?
            actions_entry->nof_actions : actions_entry->nof_qualifiers;

        for (qual_index = 0; qual_index < max_elements; ++qual_index)
        {
            qualifier_info = &(actions_entry->qualifiers[qual_index]);

            SHR_IF_ERR_EXIT(appl_ref_lsm_init_operator_enum_string_get(unit, qualifier_info->lookup_operator, qual_op));
            if (qual_index == 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
                PRT_CELL_SET("%d", action_index);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }

            if (qual_index < actions_entry->nof_qualifiers)
            {
                PRT_CELL_SET("%s", qualifier_info->qualifer_field->name);
                PRT_CELL_SET("%d", qualifier_info->qualifier_value);
                PRT_CELL_SET("0x%x", qualifier_info->qualifier_mask);
                PRT_CELL_SET("%s", qual_op);
            }
            else
            {
                PRT_CELL_SKIP(4);
            }

            if (qual_index == 0)
            {
                PRT_CELL_SET("%d", actions_entry->priority);
            }
            else
            {
                PRT_CELL_SKIP(1);
            }

            if (qual_index < actions_entry->nof_actions)
            {
                PRT_CELL_SET("%s", actions_entry->list_of_action_results[qual_index].action);
            }
            else
            {
                PRT_CELL_SKIP(1);
            }
        }
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_show_traps(
    int unit)
{
    dnx_rx_trap_map_type_t trap_type_info;
    uint32 trap_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(Lsm_traps, BCM_E_PARAM, "Lsm_traps");
    PRT_TITLE_SET("Traps configured by LSM");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Trap id (HW code)");

    for (trap_index = 0; trap_index < Lsm_nof_traps; ++trap_index)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_map_info_get(unit, Lsm_traps[trap_index].trap_type, &trap_type_info));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", trap_type_info.trap_name);
        PRT_CELL_SET("%d", trap_type_info.trap_id);
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_show_interrupts(
    int unit)
{
    uint32 interrupt_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(Lsm_interrupts, BCM_E_PARAM, "Lsm_interrupts");
    PRT_TITLE_SET("Interrupts configured by LSM");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Interrupt id");

    for (interrupt_index = 0; interrupt_index < Lsm_nof_interrupts; ++interrupt_index)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", Lsm_interrupts[interrupt_index].name);
        PRT_CELL_SET("%d", Lsm_interrupts[interrupt_index].interrupt_id);
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
appl_ref_lsm_action_table_entry_add(
    int unit,
    uint32 entry_id,
    lsm_actions_entry_t * action)
{
    int qual_index, action_entry_id, element_index;
    lsm_db_action_table_t *action_table_ptr;
    SHR_FUNC_INIT_VARS(unit);

    action_table_ptr = &Action_tables[entry_id];
    if (action_table_ptr->nof_entries >= MAX_NOF_ACTIONS_ENTRIES)
    {
        SHR_ERR_EXIT(BCM_E_FULL, "action table %d is full.", entry_id);
    }

    action_entry_id = action_table_ptr->nof_entries;

    action_table_ptr->actions_entries[action_entry_id].nof_qualifiers = action->qualifiers.nof_qualifiers;

    /**
     * loops over qualifiers in entry.
     */
    for (element_index = 0; element_index < action->qualifiers.nof_qualifiers; ++element_index)
    {
        /**
         * get index of qualifier in database according to qualifier name.
         */
        SHR_IF_ERR_EXIT(appl_ref_lsm_qualifier_field_get(unit,
                                                         action->qualifiers.
                                                         list_of_qualifiers[element_index].qualifer_field.name,
                                                         &qual_index));

        if (qual_index == BCM_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(BCM_E_NOT_FOUND,
                         "qualifier %s is not in database make sure the lsm_qualifier.xml is correct.\n",
                         action->qualifiers.list_of_qualifiers[element_index].qualifer_field.name);
        }

        /**
         * update entry with missing qualifier values.
         */
        action_table_ptr->actions_entries[action_entry_id].qualifiers[element_index].qualifer_field =
            &Qualifier_fields[qual_index];

        action_table_ptr->actions_entries[action_entry_id].qualifiers[element_index].qualifier_value =
            action->qualifiers.list_of_qualifiers[element_index].qualifier_value;

        action_table_ptr->actions_entries[action_entry_id].qualifiers[element_index].qualifier_mask =
            action->qualifiers.list_of_qualifiers[element_index].qualifier_mask;

        action_table_ptr->actions_entries[action_entry_id].qualifiers[element_index].lookup_operator =
            action->qualifiers.list_of_qualifiers[element_index].lookup_operator;
    }

    /**
     * update entry with missing action values.
     */
    action_table_ptr->actions_entries[action_entry_id].nof_actions = action->nof_actions;
    action_table_ptr->actions_entries[action_entry_id].priority = action->priority;
    sal_memcpy(&action_table_ptr->actions_entries[action_entry_id].list_of_action_results,
               &action->list_of_action_results, sizeof(action->list_of_action_results));

    action_table_ptr->nof_entries++;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_action_table_find_match(
    int unit,
    uint32 entry_id,
    lsm_actions_entry_t * qualifier)
{
    /**
     * this function will loop over all action entries(ae_index) in the database.
     * each action entry holds several actions (a_index).
     * it will than comapare each of the database's action's qualifier list (dbq_index)
     * to recived qualifer list (uq_index).
     *
     * ae_index - action entry index
     * uq_index - user qualifier index
     * dbq_index - database qualifier index
     */
    int ae_index, uq_index, dbq_index;
    uint8 is_qual_match;
    /**
     * assign a value lower than any possible value to priority.
     */
    int priority = -1;
    lsm_qualifier_info_t *user_qulifier;
    lsm_db_qualifier_info_t *db_qualifier;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qualifier, BCM_E_PARAM, "qualifier is null");

    for (ae_index = 0; ae_index < Action_tables[entry_id].nof_entries; ++ae_index)
    {
        if (qualifier->qualifiers.nof_qualifiers < Action_tables[entry_id].actions_entries[ae_index].nof_qualifiers)
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "number of qualifier elements in each action table must be consistent\n");
        }

        is_qual_match = TRUE;
        for (dbq_index = 0;
             is_qual_match && dbq_index < Action_tables[entry_id].actions_entries[ae_index].nof_qualifiers; ++dbq_index)
        {

            is_qual_match = FALSE;
            for (uq_index = 0; !is_qual_match && uq_index < qualifier->qualifiers.nof_qualifiers; ++uq_index)
            {
                user_qulifier = &qualifier->qualifiers.list_of_qualifiers[uq_index];
                db_qualifier = &Action_tables[entry_id].actions_entries[ae_index].qualifiers[dbq_index];

                /**
                 * if the user qual name does not match the db qual name, check next qual in db list.
                 */
                if (sal_strncmp(user_qulifier->qualifer_field.name, db_qualifier->qualifer_field->name, MAX_SIZE_STR))
                {
                    continue;
                }

                /**
                 * if the qual value matches this qual is verified, verify next qual in user list.
                 * setting is_qual_match=TRUE will break inner loop condition and will cuase a
                 * new iteration in outer loop.
                 * if the qual value does not match then this qual entry is not a match, check next qual entry in db
                 * becuase is_qual_match=FALSE right now outer loop condition will not be met and we will go to
                 * next qual entry in the loop.
                 */
                else
                {
                    SHR_IF_ERR_EXIT(appl_ref_lsm_db_comperator(unit, (uint32) user_qulifier->qualifier_value,
                                                               (uint32) db_qualifier->qualifier_value,
                                                               (uint32) db_qualifier->qualifier_mask,
                                                               db_qualifier->lookup_operator, &is_qual_match));
                }
            }
        }

        /**
         * the algorithem must verify there is no other match with higher priority. each found match's priority will
         * be compared to the match with the highest priority until that moment.
         * returned match will be the first hit with highest priority.
         */
        if (is_qual_match)
        {
            if (Action_tables[entry_id].actions_entries[ae_index].priority > priority)
            {
                priority = Action_tables[entry_id].actions_entries[ae_index].priority;
                qualifier->nof_actions = Action_tables[entry_id].actions_entries[ae_index].nof_actions;

                sal_memcpy(&qualifier->list_of_action_results,
                           &Action_tables[entry_id].actions_entries[ae_index].list_of_action_results,
                           sizeof(qualifier->list_of_action_results));
            }
        }
    }

    if (priority == -1)
    {
        SHR_ERR_EXIT(BCM_E_NOT_FOUND, "qualifier not found\n");
    }

exit:
    SHR_FUNC_EXIT;
}
