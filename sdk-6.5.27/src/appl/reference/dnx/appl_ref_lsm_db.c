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
#include <soc/dnx/intr/dnx_intr.h>
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
static lsm_interrupt_t *Lsm_interrupts = NULL;
/**
 * \brief
 *  number of interrupts currently in database.
 */
static uint32 Lsm_nof_interrupts = 0;

/**
 * \brief - logic to determine if given set of parameters should result in a 'hit' or 'miss' in an lsm_action_table lookup.
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

/**
 * \brief
 *   This function verifies appl_ref_lsm_db_trap_table_create function's inputs.
 * \param [in] unit - The unit number.
 * \param [in] nof_traps - The number of traps to expect in traps array.
 * \param [in] traps - array of traps to store in database
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_db_trap_table_create_verify(
    int unit,
    uint32 nof_traps,
    lsm_init_trap_t * traps)
{
    uint32 index;
    uint32 trap_index;
    dnx_rx_trap_map_type_t trap_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(traps, BCM_E_PARAM, "traps");

    SHR_MAX_VERIFY(nof_traps, LSM_MAX_NOF_TRAPS, BCM_E_PARAM, "too many traps, max number of traps=%d.\n",
                   LSM_MAX_NOF_TRAPS);

    /**
     * Verify no duplicate traps
     */
    for (index = 1; index < nof_traps; ++index)
    {
        for (trap_index = 0; trap_index < index; ++trap_index)
        {
            if (traps[index].trap_type == traps[trap_index].trap_type)
            {
                SHR_IF_ERR_EXIT(dnx_rx_trap_type_map_info_get(unit, traps[index].trap_type, &trap_info));
                SHR_ERR_EXIT(BCM_E_PARAM, "duplicate trap %s, cannot configue.\n", trap_info.trap_name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies appl_ref_lsm_db_trap_table_update function's inputs.
 * \param [in] unit - The unit number.
 * \param [in] nof_traps - The number of traps to expect in traps array.
 * \param [in] traps - array of traps to store in database
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_db_trap_table_update_verify(
    int unit,
    uint32 nof_traps,
    lsm_init_trap_t * traps)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(traps, BCM_E_PARAM, "traps");

    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_db_trap_table_create_verify(unit, nof_traps, traps));

    SHR_NULL_CHECK(Lsm_traps, BCM_E_PARAM,
                   "appl_ref_lsm_db_trap_table_update called before appl_ref_lsm_db_trap_table_create.");
    SHR_ASSERT_EQ(nof_traps, Lsm_nof_traps);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies appl_ref_lsm_db_interrupt_table_create function's inputs.
 * \param [in] unit - The unit number.
 * \param [in] nof_interrupts - The number of interrupts to expect in interrupts array.
 * \param [in] interrupts - array of interrupts to store in database
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_ref_lsm_db_interrupt_table_create_verify(
    int unit,
    uint32 nof_interrupts,
    lsm_interrupt_t * interrupts)
{
    int interrupt_id;
    uint32 index;
    uint32 int_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(interrupts, BCM_E_PARAM, "interrupts");

    SHR_MAX_VERIFY(nof_interrupts, LSM_MAX_NOF_INTERRUPTS, BCM_E_PARAM,
                   "too many interrupts, max number of interrupts=%d.\n", LSM_MAX_NOF_INTERRUPTS);

    /**
     * Verify no duplicate interrupts
     */
    for (index = 0; index < nof_interrupts; ++index)
    {
        interrupts[index].interrupt_id = soc_dnx_int_name_to_id(unit, interrupts[index].name);
        if (interrupts[index].interrupt_id == -1)
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "interrupt %s does not exsist, remove or fix in config.xml.\n",
                         interrupts[index].name);
        }

        for (int_index = index + 1; int_index < nof_interrupts; ++int_index)
        {
            interrupt_id = soc_dnx_int_name_to_id(unit, interrupts[int_index].name);
            if (interrupt_id == interrupts[index].interrupt_id)
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "duplicate interrupt %s, cannot configue.\n", interrupts[index].name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies appl_ref_lsm_db_interrupt_table_update function's inputs.
 * \param [in] unit - The unit number.
 * \param [in] nof_interrupts - The number of interrupts to expect in interrupts array.
 * \param [in] interrupts - array of interrupts to store in database
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_db_interrupt_table_update_verify(
    int unit,
    uint32 nof_interrupts,
    lsm_interrupt_t * interrupts)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_db_interrupt_table_create_verify(unit, nof_interrupts, interrupts));

    SHR_NULL_CHECK(Lsm_interrupts, BCM_E_PARAM,
                   "appl_ref_lsm_db_trap_table_update called before appl_ref_lsm_db_trap_table_create.");
    SHR_ASSERT_EQ(nof_interrupts, Lsm_nof_interrupts);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies appl_ref_lsm_db_qualifiers_info_add function's inputs.
 * \param [in] unit - The unit number.
 * \param [in] nof_qual_info - The number of qualifiers to expect in the qualifiers array.
 * \param [in] qualifier - array of qualifiers to store in database
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_db_qualifiers_info_add_verify(
    int unit,
    int nof_qual_info,
    lsm_qualifiers_fields_t * qualifier)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qualifier, BCM_E_PARAM, "qualifier");

    SHR_MAX_VERIFY(nof_qual_info, LSM_MAX_NOF_QUAL_ENTRIES, BCM_E_PARAM,
                   "too many qualifier entries, max number of different qualifier entries is %d.\n",
                   LSM_MAX_NOF_QUAL_ENTRIES);

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
    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_db_trap_table_create_verify(unit, nof_traps, traps));

    /**
     * LSM allocated/deallocates memory according to init/deinit commands repectivly. this does not suite autogenerated
     * ctests framwork as they provide a false possitive for memory leaks.
     */
    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT(Lsm_traps, sizeof(lsm_init_trap_t) * nof_traps,
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
appl_ref_lsm_db_trap_table_update(
    int unit,
    uint32 nof_traps,
    lsm_init_trap_t * traps)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_db_trap_table_update_verify(unit, nof_traps, traps));

    sal_memcpy(Lsm_traps, traps, sizeof(lsm_init_trap_t) * Lsm_nof_traps);

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
appl_ref_lsm_db_trap_table_destroy(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE_IGNORE_COUNTERS(Lsm_traps);

    Lsm_nof_traps = 0;

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
appl_ref_lsm_db_interrupt_table_create(
    int unit,
    uint32 nof_interrupts,
    lsm_interrupt_t * interrupts)
{
    uint32 index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_db_interrupt_table_create_verify(unit, nof_interrupts, interrupts));

    /**
     * LSM allocated/deallocates memory according to init/deinit commands repectivly. this does not suite autogenerated
     * ctests framwork as they provide a false possitive for memory leaks.
     */
    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT(Lsm_interrupts, sizeof(lsm_interrupt_t) * nof_interrupts,
                                                "interrupts", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (index = 0; index < nof_interrupts; ++index)
    {
        /**
         * no need to store all struct.
         */
        Lsm_interrupts[index].interrupt_id = interrupts[index].interrupt_id;
        sal_strncpy_s(Lsm_interrupts[index].name, interrupts[index].name, LSM_MAX_SIZE_STR);
    }

    Lsm_nof_interrupts = nof_interrupts;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_interrupt_table_update(
    int unit,
    uint32 nof_interrupts,
    lsm_interrupt_t * interrupts)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_db_interrupt_table_update_verify(unit, nof_interrupts, interrupts));
    sal_memcpy(Lsm_interrupts, interrupts, sizeof(lsm_interrupt_t) * Lsm_nof_interrupts);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_interrupt_table_get(
    int unit,
    uint32 *nof_interrupts,
    lsm_interrupt_t * interrupts)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(interrupts, BCM_E_PARAM, "interrupts");
    SHR_NULL_CHECK(nof_interrupts, BCM_E_PARAM, "nof_interrupts");

    sal_memcpy(interrupts, Lsm_interrupts, sizeof(lsm_interrupt_t) * Lsm_nof_interrupts);
    *nof_interrupts = Lsm_nof_interrupts;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_interrupt_table_destroy(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE_IGNORE_COUNTERS(Lsm_interrupts);

    Lsm_nof_interrupts = 0;

    SHR_EXIT();
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
appl_ref_lsm_db_qualifiers_table_create(
    int unit,
    uint32 nof_qualifiers)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(nof_qualifiers, LSM_MAX_NOF_QUAL_ENTRIES, BCM_E_PARAM,
                   "max number of different qualifiers is %d. Remove some from config.xml\n", LSM_MAX_NOF_QUAL_ENTRIES);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT(Qualifier_fields,
                                                sizeof(lsm_qualifiers_fields_t) * nof_qualifiers,
                                                "Qualifier_fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_qualifiers_table_destroy(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE_IGNORE_COUNTERS(Qualifier_fields);

    Qualifier_fields = NULL;
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
appl_ref_lsm_db_qualifier_field_get(
    int unit,
    char *qualifier_name,
    uint32 *qualifier_index)
{
    uint32 q_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_name, BCM_E_PARAM, "qualifier_name is null");
    SHR_NULL_CHECK(qualifier_index, BCM_E_PARAM, "qualifier_index is null");

    for (q_index = 0; q_index < Nof_qualifier_fields; q_index++)
    {
        if (sal_strncmp(qualifier_name, Qualifier_fields[q_index].name, LSM_MAX_SIZE_STR) == 0)
        {
            *qualifier_index = q_index;
            SHR_EXIT();
        }
    }

    SHR_SET_CURRENT_ERR(BCM_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_qualifiers_info_add(
    int unit,
    int nof_qual_info,
    lsm_qualifiers_fields_t * qualifier)
{
    uint32 qualifier_field_rcv_index;
    uint32 qualifier_field_db_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_db_qualifiers_info_add_verify(unit, nof_qual_info, qualifier));

    for (qualifier_field_rcv_index = 0; qualifier_field_rcv_index < nof_qual_info; qualifier_field_rcv_index++)
    {
        /**
         * for optimization puposes, first verify if qualifier field is already in the database. if already exists no
         * need to do anything. The counter shouldn't incress as the search algorithem will handle the rest.
         */
        SHR_SET_CURRENT_ERR(appl_ref_lsm_db_qualifier_field_get
                            (unit, qualifier[qualifier_field_rcv_index].name, &qualifier_field_db_index));
        if (SHR_GET_CURRENT_ERR() == BCM_E_NOT_FOUND)
        {
            sal_memcpy(&Qualifier_fields[Nof_qualifier_fields], &qualifier[qualifier_field_rcv_index],
                       sizeof(Qualifier_fields[Nof_qualifier_fields]));
            Nof_qualifier_fields++;
            SHR_SET_CURRENT_ERR(BCM_E_NONE);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_db_qualifiers_info_get(
    int unit,
    char *qualifier_name,
    lsm_qualifiers_fields_t * qualifier_info_p)
{
    uint32 qual_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_info_p, BCM_E_INTERNAL, "qualifier_info");
    SHR_IF_ERR_EXIT(appl_ref_lsm_db_qualifier_field_get(unit, qualifier_name, &qual_index));

    sal_memcpy(qualifier_info_p, &Qualifier_fields[qual_index], sizeof(*qualifier_info_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_qualifiers_db_entry_update(
    int unit,
    uint32 entry_id,
    lsm_qualifiers_entry_t * qualifier)
{
    uint32 qualifier_field_index, qualifier_element_index, qualifier_db_index;
    uint8 is_qualifier_exists;
    lsm_db_qualifiers_entry_t *entry_ptr;

    SHR_FUNC_INIT_VARS(unit);

    entry_ptr = &Qualifier_table->qualifier_entries[entry_id];

    for (qualifier_element_index = 0; qualifier_element_index < qualifier->nof_qualifiers; qualifier_element_index++)
    {
        SHR_IF_ERR_EXIT(appl_ref_lsm_db_qualifier_field_get(unit,
                                                            qualifier->
                                                            list_of_qualifiers[qualifier_element_index].qualifer_field.
                                                            name, &qualifier_field_index));
        /*
         * check if qualifier already in qualifier table and update only if not.
         */
        is_qualifier_exists = FALSE;
        for (qualifier_db_index = 0; qualifier_db_index < entry_ptr->nof_qualifiers; qualifier_db_index++)
        {
            if (sal_strncmp
                (entry_ptr->qualifiers[qualifier_db_index]->name,
                 qualifier->list_of_qualifiers[qualifier_element_index].qualifer_field.name, LSM_MAX_SIZE_STR) == 0)
            {
                is_qualifier_exists = TRUE;
                break;
            }
        }

        if (is_qualifier_exists == FALSE)
        {
            entry_ptr->qualifiers[entry_ptr->nof_qualifiers] = &Qualifier_fields[qualifier_field_index];
            entry_ptr->nof_qualifiers++;
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
appl_ref_lsm_action_tables_create(
    int unit,
    uint32 nof_action_tables)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(nof_action_tables, LSM_MAX_NOF_QUAL_ENTRIES, BCM_E_PARAM,
                   "Maximum action table supported is %d, got %d.\n", LSM_MAX_NOF_QUAL_ENTRIES, nof_action_tables);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT(Action_tables, sizeof(lsm_db_action_table_t) * nof_action_tables,
                                                "actions_tables", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT(Qualifier_table, sizeof(lsm_db_qualifier_table_t), "Qualifier_table",
                                                "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    Qualifier_table->nof_entries = nof_action_tables;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See appl_ref_lsm.h file
 */
shr_error_e
appl_ref_lsm_action_table_create(
    int unit,
    uint32 action_table_id,
    uint32 nof_action_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(action_table_id, Qualifier_table->nof_entries, BCM_E_PARAM,
                   "action_table_id=%d is larger then allocated number of action tables %d, use valid id.\n",
                   action_table_id, Qualifier_table->nof_entries);

    SHR_RANGE_VERIFY(nof_action_entries, 1, LSM_MAX_NOF_ACTION_ENTRIES, BCM_E_PARAM,
                     "nof_action_entries=%d is not in supported range 1 - %d, remove excess entries.\n",
                     nof_action_entries, LSM_MAX_NOF_ACTION_ENTRIES);

    SHR_ALLOC_SET_ZERO_IGNORE_COUNTERS_ERR_EXIT(Action_tables[action_table_id].actions_entries,
                                                sizeof(lsm_db_actions_entry_t) * nof_action_entries,
                                                "Action_tables[action_table_id]", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    Action_tables[action_table_id].nof_entries = 0;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See appl_ref_lsm.h file
 */
shr_error_e
appl_ref_lsm_action_table_destroy_all(
    int unit)
{
    int action_table_index;

    SHR_FUNC_INIT_VARS(unit);

    /**
     *  Qualifier_table and Action_tables are higly dependent on each other,
     *  as such they are created/destroyed together.
     *
     *  Note that Qualifier_fields are independant of these tables.
     */
    if (Qualifier_table && Action_tables)
    {
        /**
         * Qualifier_table->nof_entries signifies the number of entries in the qualifier table which also signifies the total
         * number of action tables.
         */
        for (action_table_index = 0; action_table_index < Qualifier_table->nof_entries; ++action_table_index)
        {
            SHR_FREE_IGNORE_COUNTERS(Action_tables[action_table_index].actions_entries);
        }
    }

    SHR_FREE_IGNORE_COUNTERS(Action_tables);
    SHR_FREE_IGNORE_COUNTERS(Qualifier_table);
    Action_tables = NULL;
    Qualifier_table = NULL;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * See appl_ref_lsm.h file
 */
shr_error_e
appl_ref_lsm_action_table_entry_add(
    int unit,
    uint32 action_table_id,
    lsm_actions_entry_t * action)
{
    uint32 qual_index, element_index;
    /**
     * A pointer varible to a freqently accessed element inside the Action_tables
     * database structure used to reduce access computing.
     * e.g.
     *      action_table_entry->nof_qualifiers
     *              instead of
     *      Action_tables[entry_id].actions_entries[Action_tables[entry_id].nof_entries].nof_qualifiers
     */
    lsm_db_actions_entry_t *action_table_entry;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action, BCM_E_PARAM, "action is null");
    SHR_NULL_CHECK(Action_tables, BCM_E_PARAM, "Action_tables");

    SHR_MAX_VERIFY(action->qualifiers.nof_qualifiers, LSM_MAX_NOF_QUAL, BCM_E_PARAM,
                   "action table entry can have a maximum of %d keys, given %d.\n", LSM_MAX_NOF_QUAL,
                   action->qualifiers.nof_qualifiers);
    SHR_MAX_VERIFY(action_table_id, LSM_MAX_NOF_QUAL, BCM_E_PARAM,
                   "highest action table entry id allowd is %d, given %d.\n", LSM_MAX_NOF_QUAL, action_table_id);
    SHR_MAX_VERIFY(Action_tables[action_table_id].nof_entries, LSM_MAX_NOF_ACTION_ENTRIES - 1, BCM_E_PARAM,
                   "action table %d is full.\n", action_table_id);

    action_table_entry = &Action_tables[action_table_id].actions_entries[Action_tables[action_table_id].nof_entries];
    action_table_entry->nof_qualifiers = action->qualifiers.nof_qualifiers;

    /**
     * loops over qualifiers in entry.
     */
    for (element_index = 0; element_index < action->qualifiers.nof_qualifiers; ++element_index)
    {
        /**
         * get index of qualifier in database according to qualifier name.
         */
        SHR_IF_ERR_EXIT(appl_ref_lsm_db_qualifier_field_get(unit,
                                                            action->qualifiers.
                                                            list_of_qualifiers[element_index].qualifer_field.name,
                                                            &qual_index));
        /**
         * update entry with missing qualifier values.
         */
        action_table_entry->qualifiers[element_index].qualifer_field = &Qualifier_fields[qual_index];

        action_table_entry->qualifiers[element_index].qualifier_value =
            action->qualifiers.list_of_qualifiers[element_index].qualifier_value;

        action_table_entry->qualifiers[element_index].qualifier_mask =
            action->qualifiers.list_of_qualifiers[element_index].qualifier_mask;

        action_table_entry->qualifiers[element_index].lookup_operator =
            action->qualifiers.list_of_qualifiers[element_index].lookup_operator;
    }

    /**
     * update entry with missing action values.
     */
    action_table_entry->nof_actions = action->nof_actions;
    action_table_entry->priority = action->priority;
    sal_memcpy(&action_table_entry->list_of_action_results, &action->list_of_action_results,
               sizeof(action->list_of_action_results));

    Action_tables[action_table_id].nof_entries++;

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
    uint8 is_qual_element_match;
    uint8 is_qual_list_match;
    uint32 priority = 0;
    lsm_qualifier_info_t *user_qulifier;
    lsm_db_qualifier_info_t *db_qualifier;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qualifier, BCM_E_PARAM, "qualifier is null");

    /**
     * init number of action to 0 until a match is found.
     */
    qualifier->nof_actions = 0;

    for (ae_index = 0; ae_index < Action_tables[entry_id].nof_entries; ++ae_index)
    {
        if (Action_tables[entry_id].actions_entries[ae_index].nof_qualifiers == 0)
        {
            continue;
        }

        if (qualifier->qualifiers.nof_qualifiers < Action_tables[entry_id].actions_entries[ae_index].nof_qualifiers)
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "number of qualifier elements in each action table must be consistent\n");
        }

        is_qual_list_match = TRUE;
        for (dbq_index = 0;
             is_qual_list_match && dbq_index < Action_tables[entry_id].actions_entries[ae_index].nof_qualifiers;
             ++dbq_index)
        {
            is_qual_element_match = FALSE;
            db_qualifier = &Action_tables[entry_id].actions_entries[ae_index].qualifiers[dbq_index];

            for (uq_index = 0; !is_qual_element_match && uq_index < qualifier->qualifiers.nof_qualifiers; ++uq_index)
            {
                user_qulifier = &qualifier->qualifiers.list_of_qualifiers[uq_index];

                /**
                 * if the user qual name does not match the db qual name, check next qual in db list.
                 */
                if (sal_strncmp
                    (user_qulifier->qualifer_field.name, db_qualifier->qualifer_field->name, LSM_MAX_SIZE_STR))
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
                                                               db_qualifier->lookup_operator, &is_qual_element_match));
                }
            }

            is_qual_list_match = is_qual_element_match;
        }

        /**
         * the algorithem must verify there is no other match with higher priority. each found match's priority will
         * be compared to the match with the highest priority until that moment.
         * returned match will be the last hit with highest priority.
         */
        if (is_qual_list_match)
        {
            if (Action_tables[entry_id].actions_entries[ae_index].priority >= priority)
            {
                priority = Action_tables[entry_id].actions_entries[ae_index].priority;
                qualifier->nof_actions = Action_tables[entry_id].actions_entries[ae_index].nof_actions;

                sal_memcpy(&qualifier->list_of_action_results,
                           &Action_tables[entry_id].actions_entries[ae_index].list_of_action_results,
                           sizeof(qualifier->list_of_action_results));
            }
        }
    }

    if (!qualifier->nof_actions)
    {
        SHR_ERR_EXIT(BCM_E_NOT_FOUND, "qualifier not found\n");
    }

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

    PRT_TITLE_SET("Qualifiers of type 'interrupt', configued by LSM");
    PRT_COLUMN_ADD("Name");

    for (qual_index = 0; qual_index < Nof_qualifier_fields; ++qual_index)
    {
        qualifier_ptr = &Qualifier_fields[qual_index];
        if (qualifier_ptr->qualifier_type == lsm_qualifier_interrupt)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", qualifier_ptr->name);
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
    char qual_op[LSM_OPERATOR_STRING_SIZE];

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
