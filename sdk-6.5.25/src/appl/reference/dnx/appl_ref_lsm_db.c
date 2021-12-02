/** \file appl_ref_lsm_db.c
 *
 * This file is an implemantation of the database that holds all qualifiers and actions
 * that are configured dynamically and used by the lsm diagnostic tool.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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

#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/error.h>
#include <appl/reference/dnx/appl_ref_lsm.h>
/*
 * }
 */

/**
 * \brief a list of qualifiers which signify a single qualifier entry
 */
typedef struct
{
    /**
     * a list of qualifier fields pointers
     */
    lsm_qualifiers_fields_t *qualifiers[MAX_NOF_QUAL];
    /**
     * the number of qualifiers in the list
     */
    uint32 nof_qualifiers;
} lsm_db_qualifiers_entry_t;

/**
 * \brief structure that holds information regarding a single qualifer and its value
 */
typedef struct
{
    /**
     * a single qualifier's fields information
     */
    lsm_qualifiers_fields_t *qualifer_field;
    /**
     * the value of a single qualifier
     */
    int qualifier_value;
    /**
     * the mask of a single qualifier
     */
    uint32 qualifier_mask;
} lsm_db_qualifier_info_t;

/**
 * \brief The qualifier table (only one) is a list of all qualifiers entries.
 *  It is allocated on init.
 */
typedef struct
{
    /**
     * a list of MAX_NOF_QUAL_ENTRIES qualifiers entries.
     */
    lsm_db_qualifiers_entry_t qualifier_entries[MAX_NOF_QUAL_ENTRIES];
    /**
     * the number of qualifier entries currently in the table.
     */
    uint32 nof_entries;
} lsm_db_qualifier_table_t;

/**
 * \brief an action entry is a mapping between a list of qualifiers and a list of actions.
 */
typedef struct
{
    /**
     * a list of qualifier elements and thier values.
     */
    lsm_db_qualifier_info_t qualifiers[MAX_NOF_QUAL];
    /**
     * the number of qualifiers in the qualifier list.
     */
    uint32 nof_qualifiers;
    /**
     * a list of action to executes in case of hit on all qualifiers in the list.
     */
    lsm_action_results_t list_of_action_results[MAX_NOF_ACTIONS];
    /**
     * the number of actions in the action list.
     */
    uint32 nof_actions;
    int priority_id;
} lsm_db_actions_entry_t;

/**
 * \brief An action table is a list of action entries.
 *  MAX_NOF_QUAL actions table are allocated on init.
 */
typedef struct
{
    /**
     * a list of MAX_NOF_ACTIONS_ENTRIES qualifiers entries.
     */
    lsm_db_actions_entry_t actions_entries[MAX_NOF_ACTIONS_ENTRIES];
    /**
     * the number of action entries currently in the table.
     */
    uint32 nof_entries;
} lsm_db_action_table_t;

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
 * \brief -
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
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(Qualifier_fields, BCM_E_PARAM, "Qualifier_fields is null");

    sal_printf
        ("|--------------------------------------------------------------------------------------------------------------|\n");
    sal_printf("| %-25s || %-5s || %-25s || %-11s || %-11s || %-11s |\n", "Name", "Type", "Signal", "Block", "From",
               "To");
    sal_printf
        ("|--------------------------------------------------------------------------------------------------------------|\n");
    for (int i = 0; i < Nof_qualifier_fields; ++i)
    {
        sal_printf("| %-25s || %-5d || %-25s || %-11s || %-11s || %-11s |\n",
                   Qualifier_fields[i].name, Qualifier_fields[i].qualifier_type,
                   Qualifier_fields[i].qualifier_command.signal_name,
                   Qualifier_fields[i].qualifier_command.signal_block,
                   Qualifier_fields[i].qualifier_command.signal_from, Qualifier_fields[i].qualifier_command.signal_to);
    }
    sal_printf
        ("|--------------------------------------------------------------------------------------------------------------|\n");

exit:
    SHR_FUNC_EXIT;
}

void
appl_ref_lsm_show_actions(
    int unit,
    uint32 entry_id)
{
    sal_printf("Not supported yet.\n");
}

void
appl_ref_lsm_show_traps(
    int unit)
{
    sal_printf("Not supported yet.\n");
}

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
    if (action_table_ptr->nof_entries >= MAX_NOF_ACTIONS)
    {
        SHR_ERR_EXIT(BCM_E_FULL, "action table %d is full.", entry_id);
    }

    action_entry_id = action_table_ptr->nof_entries;

    action_table_ptr->actions_entries[action_entry_id].nof_qualifiers = action->qualifiers.nof_qualifiers;

    for (element_index = 0; element_index < action->qualifiers.nof_qualifiers; ++element_index)
    {
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

        action_table_ptr->actions_entries[action_entry_id].qualifiers[element_index].qualifer_field =
            &Qualifier_fields[qual_index];

        action_table_ptr->actions_entries[action_entry_id].qualifiers[element_index].qualifier_value =
            action->qualifiers.list_of_qualifiers[element_index].qualifier_value;

        action_table_ptr->actions_entries[action_entry_id].qualifiers[element_index].qualifier_mask =
            action->qualifiers.list_of_qualifiers[element_index].qualifier_mask;
    }

    action_table_ptr->actions_entries[action_entry_id].nof_actions = action->nof_actions;
    action_table_ptr->actions_entries[action_entry_id].priority_id = action->priority_id;
    for (element_index = 0; element_index < action->nof_actions; ++element_index)
    {
        sal_strncpy_s(action_table_ptr->actions_entries[action_entry_id].list_of_action_results[element_index].diags,
                      action->list_of_action_results[element_index].diags, MAX_SIZE_DIAG);

        sal_strncpy_s(action_table_ptr->actions_entries[action_entry_id].list_of_action_results[element_index].prt_info,
                      action->list_of_action_results[element_index].prt_info, MAX_SIZE_PRINT);
    }

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
     * a_index - action index in action list
     * uq_index - user qualifier index
     * dbq_index - database qualifier index
     */
    int ae_index, uq_index, dbq_index, a_index;
    int is_qual_match;
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

                /**
                 * if the qual value matches this qual is verified, verify next qual in user list.
                 * setting is_qual_match=TRUE will break inner loop condition and will cuase a
                 * new iteration in outer loop.
                 */
                }
                else if (((uint32) user_qulifier->qualifier_value & (uint32) db_qualifier->qualifier_mask) ==
                         ((uint32) db_qualifier->qualifier_value & (uint32) db_qualifier->qualifier_mask))
                {
                    is_qual_match = TRUE;

                /**
                 * if the qual value does not match this qual entry is not a match, check next qual ebtry in db
                 * becuase is_qual_match=FALSE right now outer loop condition will not be met and we will get to
                 * qual entry loop.
                 */
                }
                else
                {
                    break;
                }
            }
        }

        /**
         * the algorithem must verify there is no other match with higher priority. each found match's priority will
         * be compared to the match with the highest priority until that moment.
         *
         */
        if (is_qual_match)
        {
            if (Action_tables[entry_id].actions_entries[ae_index].priority_id > priority)
            {
                priority = Action_tables[entry_id].actions_entries[ae_index].priority_id;
                qualifier->nof_actions = Action_tables[entry_id].actions_entries[ae_index].nof_actions;
                for (a_index = 0; a_index < qualifier->nof_actions; ++a_index)
                {
                    sal_memcpy(&qualifier->list_of_action_results[a_index],
                               &Action_tables[entry_id].actions_entries[ae_index].list_of_action_results[a_index],
                               sizeof(lsm_action_results_t));
                }
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
