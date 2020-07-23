/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_fec.c
 *
 * FEC PP Diagnostic Command - Retrieves either all FECs in the system,
 * or FECs that were hit in last packet traverse.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include "diag_dnx_pp.h"
#include <bcm_int/dnx/l3/l3_fec.h>
#include <soc/dnx/dbal/dbal_string_apis.h>

/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

dbal_tables_e fec_hierarchy_tables[] =
    { DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY, DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY, DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY };

/** this array corresponds to the SUPER_FEC result type and describes which gerenral FEC fields is used */
dbal_fields_e fec_general_entry_dbal_fields[] =
    { DBAL_FIELD_FEC_ENTRY_NO_PROTECTION_NO_STATS, DBAL_FIELD_FEC_ENTRY_W_PROTECTION_W_1_STATS,
    DBAL_FIELD_FEC_ENTRY_W_PROTECTION_NO_STATS, DBAL_FIELD_FEC_ENTRY_NO_PROTECTION_W_2_STATS
};

/*************
 *  MACROS  *
 */

#define L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(sup_fec_id, instance) ((sup_fec_id << 1) | instance)

/*************
 * TYPEDEFS  *
 */
extern shr_error_e dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info);
/** define the table which represents fec's inner struct values */
typedef struct table_fec_structs
{
    dbal_fields_e struct_id;
    uint32 struct_value;
    uint8 struct_present;
} table_fec_structs_t;

/** \brief List of Table types per FEC hierarchy */
typedef enum
{
    NO_PROTECTION_TABLE,
    W_PROTECTION_TABLE,

    NOF_FEC_TABLE_TYPES
} table_ids_t;

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief - Function prints the columns of 2 tables - FECs with Protection and FECs with no Protection
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] prt_ctr
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
print_fec_columns(
    int unit,
    prt_control_t * prt_ctr[])
{

    table_ids_t table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < NOF_FEC_TABLE_TYPES; table_id++)
    {
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "FEC_Id");

        /** print these columns only in the protection FEC table */
        if (table_id == W_PROTECTION_TABLE)
        {
            PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Inst");
            PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Protect");
        }

        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Stat_Id");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Stat_Cmd");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Dest");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "MC_RPF_Mode");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "EEI");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Hierarch_TM_Flow");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Glob_Out_LIF");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Glob_Out_LIF_2nd");
        PRT_COLUMN_ADD_FLEX_PL(table_id, PRT_FLEX_ASCII, "Hit bit");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function fills the fec tables with its values per a SUPER_FEC - for
 *          the 2 FEC instances in there, in parallel to 2 tables - FECs with Protections and FECs without
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] super_fec_id
 *   \param [in] fec_hierarchy
 *   \param [in] prt_ctr
 *   \param [in] first_row_separator - represents which separator to use before the first row
 *   \param [in] instance_id - if '-1' print both, otherwise print only one
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
fill_fec_table(
    int unit,
    uint32 super_fec_id,
    uint32 fec_hierarchy,
    prt_control_t * prt_ctr[],
    PRT_ROW_SEP_MODE first_row_separator,
    int instance_id)
{

#define PRIMARY_FEC_INST 0

    int fec_instance;

    uint32 entry_handle_id;
    uint32 attr_val;
    uint32 attr_type;

    /** array of pointers to Boolean for Primary and Secondary FECs */
    uint8 is_allocated_arr[2];
    uint8 is_allocated;
    uint8 prime_protected = FALSE;
    int hitbit_raised = -1;

    uint32 protection_pointer = 0;

    int low_instance, high_instance;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** gather information on allocated and protected (only prime has protection indication) */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated
                    (unit, L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(super_fec_id, 0), &is_allocated));
    is_allocated_arr[0] = is_allocated;
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated
                    (unit, L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(super_fec_id, 1), &is_allocated));
    is_allocated_arr[1] = is_allocated;
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get
                    (unit, L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(super_fec_id, 0), &prime_protected));

    /** if both of the FEC instances aren't allocated there's nothing to do here (as in case where a Core isn't used) */
    if ((!is_allocated_arr[0]) && (!is_allocated_arr[1]))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, fec_hierarchy_tables[fec_hierarchy], &entry_handle_id));

    /** use SUPER_FEC of the first 19bits from FEC_ID as key to access the table */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, super_fec_id);

    if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit))
    {
        attr_type = DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_SECONDARY | DBAL_ENTRY_ATTR_HIT_PRIMARY;
        dbal_entry_attribute_request(unit, entry_handle_id, attr_type, &attr_val);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (instance_id >= 0 && instance_id <= 1)
    {
        low_instance = instance_id;
        high_instance = instance_id;
    }
    else if (instance_id == -1)
    {
        low_instance = 0;
        high_instance = 1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEC instance: %d is not within 0-1 !\n", instance_id);
    }

    /** iterate print for both FEC instances in the SUPER_FEC - 1 is the Primary, 0 is secondary  */
    for (fec_instance = low_instance; fec_instance <= high_instance; fec_instance++)
    {

        uint32 fec_general_entry_result[3] = { 0 };
        uint32 fec_entry_value[3];

        dbal_fields_e fec_entry_type;

        /** this array corresponds to fec_general_entry_dbal_fields and describes for each in SUPER_FEC how many satistics supported */
        int statistics_support_arr[] = { 0, 1, 0, 2 };

        uint32 result_type;

        int low_level_field_idx;

        /** this is actually a simple uint32, but due to coverity, this variable is used as array in dbal function
         *  so defining as an array
         */
        uint32 struct_field_value[1];

        dbal_field_types_basic_info_t *low_level_fec_struct;

        /** define the table which represents fec's inner struct values */
        table_fec_structs_t table_fec_values[] = {
            {DBAL_FIELD_DESTINATION, 0, FALSE},
            {DBAL_FIELD_MC_RPF_MODE, 0, FALSE},
            {DBAL_FIELD_EEI, 0, FALSE},
            {DBAL_FIELD_HIERARCHICAL_TM_FLOW, 0, FALSE},
            {DBAL_FIELD_GLOB_OUT_LIF, 0, FALSE},
            {DBAL_FIELD_GLOB_OUT_LIF_2ND, 0, FALSE},
        };

        int table_struct_size = sizeof(table_fec_values) / sizeof(table_fec_values[0]);

        int table_struct_idx;

        int fec_id = L3_FEC_ID_FROM_SUPER_FEC_INSTANCE(super_fec_id, fec_instance);

        /** decide which table id, default is no protection table */
        table_ids_t table_id = NO_PROTECTION_TABLE;

        /** if the instance hasn't been allocated skip it */
        if (is_allocated_arr[fec_instance] == FALSE)
        {
            continue;
        }

        if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit))
        {
            uint32 hitbit_flags =
                (fec_instance ==
                 low_instance ? DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED :
                 DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED);
            if ((attr_val & hitbit_flags) != 0)
            {
                hitbit_raised = 1;
            }
            else
            {
                hitbit_raised = 0;
            }
        }
        /*
         *  Direct the table in which to print according to allocation and protection flags
         *  at this point we know the FEC is allocated
         */

        if (prime_protected)
        {
            /** no matter if we're at primary or secondary - if the primary protected both be printed in protection table */
            table_id = W_PROTECTION_TABLE;
        }

        /** decide which separator to use before the first line - allow a line only in a protected table */
        if ((fec_instance == 0) && (prime_protected))
        {
            PRT_ROW_ADD_PL(table_id, first_row_separator);
        }
        else
        {
            PRT_ROW_ADD_PL(table_id, PRT_ROW_SEP_NONE);
        }

         /** retrive the result type */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

        /** print the FEC_ID - this is the SUPER_FEC + fec_instance bit */
        PRT_CELL_SET_PL(table_id, "%d", fec_id);

        if (prime_protected)
        {

            /** print the Instance - Primary/Secondary */
            PRT_CELL_SET_PL(table_id, "%s", (fec_instance == PRIMARY_FEC_INST) ? "Primary" : "Secondary");

            /** only the Primary contains the protection */
            if (fec_instance == PRIMARY_FEC_INST)
            {
                /** get the FEC Protection entry result */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, fec_instance,
                                 &protection_pointer));
            }

            PRT_CELL_SET_PL(table_id, "%d", protection_pointer);
        }

        /** print the statistics according to support of the result type - 0/1/2 statistics availability */
        if ((statistics_support_arr[result_type] == 2) ||
            (statistics_support_arr[result_type] == 1 && fec_instance == PRIMARY_FEC_INST))
        {
            uint32 stat_id, stat_cmd;

           /** get the FEC Statistics entry result */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, fec_instance, &stat_id));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, fec_instance, &stat_cmd));

            PRT_CELL_SET_PL(table_id, "%d", stat_id);
            PRT_CELL_SET_PL(table_id, "%d", stat_cmd);
        }
        else
        {
            PRT_CELL_SET_PL(table_id, "-");
            PRT_CELL_SET_PL(table_id, "-");
        }

        /*
         **************** PRINT THE FEC INNER STRUCTURES ***********
         */

        /*
         * according to each result_type, choose the correct field where fec_id is present
         * each result contains x2 FECs, so the LSB bit 0 will decide which FEC instance to read
         */

        /** get the fec general entry result - which can contain multiple options */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, fec_general_entry_dbal_fields[result_type], fec_instance,
                         fec_general_entry_result));

        /** understand the specific sub fec entry type from the general fec entry */
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                        (unit, fec_general_entry_dbal_fields[result_type], fec_general_entry_result, &fec_entry_type,
                         fec_entry_value));

        /** get the inner low level fields of fecs by telling its type (given in array form) */
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, fec_entry_type, &low_level_fec_struct));

        /**  iterate through the inner structure array */
        for (low_level_field_idx = 0; low_level_field_idx < low_level_fec_struct->nof_struct_fields;
             low_level_field_idx++)
        {

            /** get the value of the lower level struct */
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type,
                                                            low_level_fec_struct->struct_field_info
                                                            [low_level_field_idx].struct_field_id, struct_field_value,
                                                            fec_entry_value));
            /*
             * build the array that represents the table
             */

            /** iterate through the possible struct field types and compare to above type */
            for (table_struct_idx = 0; table_struct_idx < table_struct_size; table_struct_idx++)
            {
                if (table_fec_values[table_struct_idx].struct_id ==
                    low_level_fec_struct->struct_field_info[low_level_field_idx].struct_field_id)
                {
                    table_fec_values[table_struct_idx].struct_value = struct_field_value[0];
                    table_fec_values[table_struct_idx].struct_present = TRUE;
                    break;
                }
            } /** of for on table values index */

        } /** of for on inner structure of the fec */

        /** iterate throught the table struct values and print out */
        for (table_struct_idx = 0; table_struct_idx < table_struct_size; table_struct_idx++)
        {
            if (table_fec_values[table_struct_idx].struct_present)
            {
                /** destination field requires a parsing by DBAL */
                if (table_fec_values[table_struct_idx].struct_id == DBAL_FIELD_DESTINATION)
                {
                    char parsed_dest_print[DSIG_MAX_SIZE_STR];
                    /** setting a pointer to get a const str from the function, to be used in dbal from hw value get */
                    char dest_field_str[RHNAME_MAX_SIZE];

                    sal_strncpy(dest_field_str,
                                dbal_field_to_string(unit, table_fec_values[table_struct_idx].struct_id),
                                RHNAME_MAX_SIZE - 1);

                    SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get(unit, dest_field_str,
                                                                             table_fec_values
                                                                             [table_struct_idx].struct_value,
                                                                             parsed_dest_print));

                    PRT_CELL_SET_PL(table_id, "%s", parsed_dest_print);
                }
                else
                {
                    PRT_CELL_SET_PL(table_id, "%d", table_fec_values[table_struct_idx].struct_value);
                }
            }
            else
            {
                PRT_CELL_SET_PL(table_id, "-");
            } /** of if struct is present */

        } /** of for on table values index */
        if (hitbit_raised == -1)
        {
            PRT_CELL_SET_PL(table_id, "-");
        }
        else if (hitbit_raised == 1)
        {
            PRT_CELL_SET_PL(table_id, "Yes");
        }
        else
        {
            PRT_CELL_SET_PL(table_id, "No");
        }

    } /** of for fec_instance */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Shows the all defined FEC entries in all the FEC hierarchies (default) or in a given hierearchie
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_in - core id
 *   \param [in] usr_hrc_idx - FECs Hierarchy of interest
 *   \param [in] short_mode - flag to indicate that SHORT flag appeared in the BCM shell
 *   \param [in] sand_control
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_fec_cmds(
    int unit,
    int core_in,
    int usr_hrc_idx,
    uint8 short_mode,
    sh_sand_control_t * sand_control)
{
    int core_id;

    uint32 entry_handle_id;

    /** There's no need to print twice for each core, it's same information */
    uint8 all_printed = FALSE;

    uint8 freed_tables[NOF_FEC_TABLE_TYPES] = { TRUE, TRUE };

    /** would indicate if there was at least one print */
    uint8 was_print = 0;

    /** Allocate x2 Protection and Non Protection Table per each Hierarchy*/
    PRT_INIT_VARS_PL(NOF_FEC_TABLE_TYPES);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** initial allocation for entry_handle_id so that re-use can be done on this handle with DBAL_HANDLE_CLEAR */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY, &entry_handle_id));

    /** print per each core or the core that user has set */
    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        table_ids_t table_id;
        int hrc_low, hrc_high, loop_hrc_idx;

        /** The information is same for each Core_ID, no need to do on both */
        if (all_printed)
        {
            continue;
        }

        all_printed = TRUE;

        /** check validity of user input for FEC hierarchy to present for ALL FECs print */
        if ((usr_hrc_idx >= 0) && (usr_hrc_idx <= 2))
        {
            hrc_low = usr_hrc_idx;
            hrc_high = usr_hrc_idx;
        }
        else
        {
            hrc_low = 0;
            hrc_high = 2;
        }

        /** if user doesn't insert the usr_hrc_idx, it's value is 4, so loop will iterate on all 1-3 FEC hierarchies */
        for (loop_hrc_idx = hrc_low; loop_hrc_idx <= hrc_high; loop_hrc_idx++)
        {
            int is_end = 0;
            int loop_count = 0;

            /** start printing the table per each Hiararchy - with and without protection */
            PRT_TITLE_SET_PL(NO_PROTECTION_TABLE, "No Protection FECs Hierarchy %d, (Both Cores)", loop_hrc_idx + 1);
            freed_tables[NO_PROTECTION_TABLE] = FALSE;

            PRT_TITLE_SET_PL(W_PROTECTION_TABLE, "Protection FECs Hierarchy %d, (Both Cores)", loop_hrc_idx + 1);
            freed_tables[W_PROTECTION_TABLE] = FALSE;

            SHR_IF_ERR_EXIT(print_fec_columns(unit, prt_ctr));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, fec_hierarchy_tables[loop_hrc_idx], entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

            /** iterate on all valid entries of the table of this Hierarchy */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            while (!is_end)
            {

                uint32 super_fec_id;

                /** get the current key of active entry - super_fec_id */
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, &super_fec_id));

                /** fill fec table - for first FEC entry print use no line, for next use a line before */
                SHR_IF_ERR_EXIT(fill_fec_table(unit, super_fec_id, loop_hrc_idx, prt_ctr,
                                               (loop_count ==
                                                0) ? PRT_ROW_SEP_NONE : PRT_ROW_SEP_UNDERSCORE_BEFORE, -1));

                /** move to the next valid entry in FEC */
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

                loop_count++;

            } /** of while is_end */

            /** close the table per FEC only which is not empty */
            for (table_id = 0; table_id < NOF_FEC_TABLE_TYPES; table_id++)
            {
                if (PRT_ROW_NUM_PL(table_id) > 0)
                {
                    PRT_COMMITX_PL(table_id);

                    /** mark that there was at least some print */
                    was_print = 1;
                }
                else
                {
                    PRT_FREE_PL(table_id);
                    /** mark this table as freed */
                    freed_tables[table_id] = TRUE;
                }
            }

        } /** of for on hierarchy index */

    }   /** of core id for loop*/

    if (!was_print)
    {
        LOG_CLI(("No FEC entries defined\n"));
    }

exit:

    /** free only non free tables */
    {
        table_ids_t table_id;

        for (table_id = 0; table_id < NOF_FEC_TABLE_TYPES; table_id++)
        {
            if (!freed_tables[table_id])
            {
                PRT_FREE_PL(table_id);
            }
        }

    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - an Adaptor for sh_dnx_pp_fec_cmds function
 *          In case of Info FEC command, which supports ALL and HRC flag
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_info_fec_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int usr_hrc_idx = -1;

    int core = -1;
    uint8 short_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_BOOL("SHORT", short_mode);

    /** the hrc is set to accept only values from 0-2 from user */
    SH_SAND_GET_INT32("HieRarChy", usr_hrc_idx);

    SHR_IF_ERR_EXIT(sh_dnx_pp_fec_cmds(unit, core, usr_hrc_idx, short_mode, sand_control));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_pp_info_fec_table_set_cb(
    int unit)
{
    int hrc_idx;
    bcm_if_t fec_id[3];
    bcm_l3_egress_t fec_intf[3];
    bcm_gport_t gport = 0;
    int out_port = 203;

    bcm_switch_fec_property_config_t fec_config;

    uint32 iter_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** clear all FEC Hierarchy tables to start fresh */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, fec_hierarchy_tables[0], &iter_handle_id));

    for (hrc_idx = 0; hrc_idx <= 2; hrc_idx++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, fec_hierarchy_tables[hrc_idx], iter_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, iter_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, iter_handle_id, DBAL_ITER_ACTION_DELETE));
        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, iter_handle_id));
    }

    /** get first FEC_ID per each FEC hierarchy of allowed range */
    for (hrc_idx = 0; hrc_idx <= 2; hrc_idx++)
    {
        if (hrc_idx == 0)
        {
            fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
        }
        else if (hrc_idx == 1)
        {
            fec_config.flags = BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY;
        }
        else
        {
            fec_config.flags = BCM_SWITCH_FEC_PROPERTY_3RD_HIERARCHY;
        }

        SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));
        fec_id[hrc_idx] = fec_config.start;
    }

    /** 1st FEC hierarchy */
    bcm_l3_egress_t_init(&fec_intf[0]);
    fec_intf[0].flags = 0;                          /** no flags in order to use FEC of 1st hierarchy */
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_id[1]);
    fec_intf[0].port = gport;                       /** destination - the FEC in 2nd hierarchy */
    fec_intf[0].intf = 100;                         /** global lif-1 */
    fec_intf[0].encap_id = 0;                       /** global lif-2 */

    /** 2nd FEC hierarchy */
    bcm_l3_egress_t_init(&fec_intf[1]);
    fec_intf[1].flags = BCM_L3_2ND_HIERARCHY;                      /** flags in order to use FEC of 2nd hierarchy */
    BCM_GPORT_FORWARD_PORT_SET(gport, fec_id[2]);
    fec_intf[1].port = gport;                                      /** destination - the FEC in 3rd hierarchy */
    fec_intf[1].intf = 0x1384;                                     /** global lif-1 - add LIF encoding because above 0x1000 */
    BCM_L3_ITF_SET(fec_intf[1].intf, _SHR_L3_ITF_TYPE_LIF, fec_intf[1].intf);
    fec_intf[1].encap_id = 0;                                      /** global lif-2 */

    /** 3rd FEC hierarchy */
    bcm_l3_egress_t_init(&fec_intf[2]);
    fec_intf[2].flags = BCM_L3_3RD_HIERARCHY;       /** flags in order to use FEC of 3rd hierarchy */
    BCM_GPORT_FORWARD_PORT_SET(gport, out_port);
    fec_intf[2].port = gport;                       /** destination - out-port */
    fec_intf[2].intf = 15;                          /** global lif-1 */
    fec_intf[2].encap_id = 0x1386;                  /** global lif-2 */

    /** add one entry per each FEC Hierarchy table */
    for (hrc_idx = 0; hrc_idx <= 2; hrc_idx++)
    {
        SHR_IF_ERR_EXIT(bcm_l3_egress_create
                        (unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &fec_intf[hrc_idx], &fec_id[hrc_idx]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_info_fec_man = {
    .brief = "PP FEC Information",
    .full =
        "Shows the FEC entries that are configured in the device, if HieRarchy is not specified - show all FEC hierarchies entries",
    .init_cb = diag_dnx_pp_info_fec_table_set_cb,
    .examples = "core=0\n" "HRC=0\n" "SHORT"
};

sh_sand_option_t dnx_pp_info_fec_man_options[] = {
    {"HieRarChy", SAL_FIELD_TYPE_INT32, "Dump information for specific FEC hierarchy(0-2)", "-1", NULL,
     "0-2"}
    ,
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};
