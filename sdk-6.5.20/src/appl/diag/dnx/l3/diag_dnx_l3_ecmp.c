/** \file diag_dnx_l3_ecmp.c
 *
 * file for l3 intf diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** bcm */
#include <bcm/l3.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <bcm/switch.h>
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_l3.h"

/*
 * }
 */

/*
 * This is the number of columns to use when displaying the ECMP consistent members.
 */
#define DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY     32
/*
 * MAX command string length (can be changed if more chars are needed)
 */
#define DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH                     70
/*
 * The NOF FECs that will be used for the example testing
 */
#define DIAG_DNX_PP_EXAMPLE_NOF_FEC                         10
/*
 * The most NOF ECMP groups that will be used for the example testing
 */
#define DIAG_DNX_PP_EXAMPLE_MAX_NOF_ECMPS_CREATED           4
/*
 * The ECMP group ID that of the group that created and the example tests init
 */
#define DIAG_DNX_PP_EXAMPLE_ECMP_ID                         51
/*
 * The size of the ECMP group that created and the example tests init
 */
#define DIAG_DNX_PP_EXAMPLE_ECMP_GROUP_SIZE                 3

/*
 * \brief
 * Generate dynamically "ECMP create" example as the FEC IDs of the ECMP group depends on the device and the configuration.
 */
static shr_error_e
ecmp_create_example_generate(
    int unit,
    int flags,
    struct sh_sand_cmd_s *sh_sand_cmd,
    rhlist_t * examples_list)
{

    int fec_id;
    bcm_switch_fec_property_config_t fec_config;
    char cmd[DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH];
    char fec_str[10];
    int iter;
    SHR_FUNC_INIT_VARS(unit);
    /** Get the first FEC ID of the first hierarchy */
    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    /** Create and ECMP create example with ID */
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(fec_config.start));
    sal_memset(&cmd, 0, sizeof(cmd));
    sal_strncpy(cmd, "ID=50 size=3 fec=0x", DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH - 1);
    for (iter = 0; iter < 3; iter++)
    {
        sal_memset(&fec_str, 0, sizeof(fec_str));
        sal_sprintf(fec_str, "%08x", fec_id++);
        sal_strncat_s(cmd, fec_str, sizeof(cmd));
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_example_add(unit, flags, sh_sand_cmd, examples_list, cmd), "");

    /** Create and ECMP create example without an ID */
    sal_memset(&cmd, 0, sizeof(cmd));
    sal_strncpy(cmd, "size=2 fec=0x", DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH - 1);
    for (iter = 0; iter < 2; iter++)
    {
        sal_memset(&fec_str, 0, sizeof(fec_str));
        sal_sprintf(fec_str, "%08x", fec_id++);
        sal_strncat_s(cmd, fec_str, sizeof(cmd));
    }
    SHR_CLI_EXIT_IF_ERR(sh_sand_example_add(unit, flags, sh_sand_cmd, examples_list, cmd), "");

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Generate dynamically "ECMP add" example as the FEC IDs of the ECMP group depends on the device and the configuration.
 */
static shr_error_e
ecmp_add_example_generate(
    int unit,
    int flags,
    struct sh_sand_cmd_s *sh_sand_cmd,
    rhlist_t * examples_list)
{

    int fec_id;
    bcm_switch_fec_property_config_t fec_config;
    char cmd[DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH];
    char tmp_str[DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    /** Get the next FEC_ID of the ECMP group to add it */
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC,
                   BCM_L3_ITF_VAL_GET(fec_config.start + DIAG_DNX_PP_EXAMPLE_ECMP_GROUP_SIZE));
    sal_memset(&cmd, 0, sizeof(cmd));
    sal_memset(&tmp_str, 0, sizeof(tmp_str));
    sal_strncpy(cmd, "ID=", DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH - 1);
    /** Add the ECMP group ID  */
    sal_sprintf(tmp_str, "%d", DIAG_DNX_PP_EXAMPLE_ECMP_ID);
    sal_strncat_s(cmd, tmp_str, sizeof(cmd));
    sal_strncat_s(cmd, " intf=0x", sizeof(cmd));
    /** Add the new interface */
    sal_memset(&tmp_str, 0, sizeof(tmp_str));
    sal_sprintf(tmp_str, "%08x", fec_id);
    sal_strncat_s(cmd, tmp_str, sizeof(cmd));
    SHR_CLI_EXIT_IF_ERR(sh_sand_example_add(unit, flags, sh_sand_cmd, examples_list, cmd), "");

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Generate dynamically "ECMP delete" example as the FEC IDs of the ECMP group depends on the device and the configuration.
 */
static shr_error_e
ecmp_delete_example_generate(
    int unit,
    int flags,
    struct sh_sand_cmd_s *sh_sand_cmd,
    rhlist_t * examples_list)
{

    int fec_id;
    bcm_switch_fec_property_config_t fec_config;
    char cmd[DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH];
    char tmp_str[DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    /** Get the last FEC_ID of the ECMP group to delete it */
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC,
                   BCM_L3_ITF_VAL_GET(fec_config.start + DIAG_DNX_PP_EXAMPLE_ECMP_GROUP_SIZE - 1));
    sal_memset(&cmd, 0, sizeof(cmd));
    sal_memset(&tmp_str, 0, sizeof(tmp_str));
    sal_strncpy(cmd, "ID=", DIAG_DNX_PP_ECMP_CMD_MAX_LENGTH - 1);
    /** Add the ECMP group ID  */
    sal_sprintf(tmp_str, "%d", DIAG_DNX_PP_EXAMPLE_ECMP_ID);
    sal_strncat_s(cmd, tmp_str, sizeof(cmd));
    sal_strncat_s(cmd, " intf=0x", sizeof(cmd));
    /** Add the new interface */
    sal_memset(&tmp_str, 0, sizeof(tmp_str));
    sal_sprintf(tmp_str, "%08x", fec_id);
    sal_strncat_s(cmd, tmp_str, sizeof(cmd));
    SHR_CLI_EXIT_IF_ERR(sh_sand_example_add(unit, flags, sh_sand_cmd, examples_list, cmd), "");

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief
 * Run before the testing of each ECMP example test.
 * The init will create FECs that will be used for the ECMP groups that will be tested and also an ECMP group to
 * test for all the examples besides the "create" which creates its own ECMP group.
 */
shr_error_e
ecmp_init(
    int unit)
{

    int fec_id;
    bcm_switch_fec_property_config_t fec_config;
    int iter;
    uint32 egr_intf = 0x40003000;
    bcm_l3_egress_t egr_entry;
    uint32 flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    bcm_l3_egress_ecmp_t ecmp_entry, ecmp;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the first FEC of the first hierarchy
     */
    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    /*
     * Create FECs that will be used for the ECMP groups in the example testing
     */
    bcm_l3_egress_t_init(&egr_entry);
    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(fec_config.start));
    for (iter = 0; iter < DIAG_DNX_PP_EXAMPLE_NOF_FEC; iter++)
    {
        egr_entry.intf = egr_intf;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_entry, &fec_id));
        intf_array[iter] = fec_id++;
    }
    /*
     * Create ECMP group that will be used to the add, delete and destroy options
     */
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.flags = BCM_L3_WITH_ID;
    ecmp_entry.ecmp_intf = DIAG_DNX_PP_EXAMPLE_ECMP_ID;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_entry, DIAG_DNX_PP_EXAMPLE_ECMP_GROUP_SIZE, intf_array));

    if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        bcm_l3_ecmp_tunnel_priority_map_info_t map_info;
        /*
         *  Create tunnel priority map for the ECMP group
         */
        map_info.l3_flags = BCM_L3_WITH_ID;
        map_info.map_profile = 1;
        SHR_IF_ERR_EXIT(bcm_l3_ecmp_tunnel_priority_map_create(unit, &map_info));

        map_info.tunnel_priority = 0;
        map_info.index = 0;
        map_info.l3_flags = 0;
        SHR_IF_ERR_EXIT(bcm_l3_ecmp_tunnel_priority_map_set(unit, &map_info));

        /*
         * Create a TP ECMP group
         */
        bcm_l3_egress_ecmp_t_init(&ecmp);
        ecmp.flags = BCM_L3_WITH_ID;
        ecmp.ecmp_intf = DIAG_DNX_PP_EXAMPLE_ECMP_ID + 1;
        ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        ecmp.max_paths = DIAG_DNX_PP_EXAMPLE_ECMP_GROUP_SIZE;
        ecmp.tunnel_priority.index = 0;
        ecmp.tunnel_priority.map_profile = map_info.map_profile;
        ecmp.tunnel_priority.mode = bcmL3EcmpTunnelPriorityModeTwoPriorities;
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, 1, intf_array));
    }
    else
    {
        /*
         * In case the tunnel priority is not supported.
         */
        bcm_l3_egress_ecmp_t_init(&ecmp);
        ecmp.flags = BCM_L3_WITH_ID;
        ecmp.ecmp_intf = DIAG_DNX_PP_EXAMPLE_ECMP_ID + 1;
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, 1, intf_array));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Run after each one of the ECMP example tests
 * Destroy all the ECMP group that were created at init and in the create example if it was used and all the
 * FECs which were created at the init stage
 */
shr_error_e
ecmp_deinit(
    int unit)
{

    int fec_id;
    bcm_switch_fec_property_config_t fec_config;
    int iter;
    int rv;
    int count;
    uint32 ecmp_id;
    int size = DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE;
    bcm_l3_egress_ecmp_t ecmp_entry;
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    bcm_l3_ecmp_tunnel_priority_map_info_t map_info;

    /** These are the ECMP groups IDs that can be created during the example testing, release them once the testing is done */
    int ecmp_ids_to_delete[DIAG_DNX_PP_EXAMPLE_MAX_NOF_ECMPS_CREATED] = { 1, 50, 51, 52 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Destroy all the ECMP groups that can be created during the example (not all of them were necessarily created)
     */
    for (iter = 0; iter < DIAG_DNX_PP_EXAMPLE_MAX_NOF_ECMPS_CREATED; iter++)
    {
        bcm_l3_egress_ecmp_t_init(&ecmp_entry);
        BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_ids_to_delete[iter]));
        ecmp_entry.ecmp_intf = ecmp_id;
        rv = bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count);
        if (rv == BCM_E_NONE)
        {
            bcm_l3_egress_ecmp_destroy(unit, &ecmp_entry);
        }
    }

    /*
     * Destroy all the FECs that were created on init
     */
    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(fec_config.start));

    for (iter = 0; iter < DIAG_DNX_PP_EXAMPLE_NOF_FEC; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fec_id));
        fec_id++;
    }
    /*
     *Destroy a TP map profile
     */
    if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        map_info.tunnel_priority = 0;
        map_info.index = 0;
        map_info.map_profile = 1;
        map_info.l3_flags = 0;
        rv = bcm_l3_ecmp_tunnel_priority_map_get(unit, &map_info);
        if (rv == BCM_E_NONE)
        {
            SHR_IF_ERR_EXIT(bcm_l3_ecmp_tunnel_priority_map_destroy(unit, &map_info));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_l3_ecmp_dump_man = {
    .brief = "Dump information about the existing ECMP groups regarding their ID, mode, protection, etc.",
    .full = "Print information regarding all existing ECMP groups in a table.\n"
        "If a particular ECMP group ID has been provided and if its mode is consistent, "
        "then its members will be printed as well.",
    .synopsis = "[id=<ECMP_GROUP_ID>]",
    .examples = "\n" "ID=51",
    .init_cb = ecmp_init,
    .deinit_cb = ecmp_deinit
};

static sh_sand_man_t dnx_l3_ecmp_create_man = {
    .brief = "Create an ECMP group",
    .full = "Create an ECMP group by providing an ECMP ID, size of the group and its members.",
    .synopsis = "[ID=<ECMP ID>] size=<group size> fec=\"<FEC members>\"",
    .example_generate_cb = ecmp_create_example_generate,
    .init_cb = ecmp_init,
    .deinit_cb = ecmp_deinit
};

static sh_sand_man_t dnx_l3_ecmp_destroy_man = {
    .brief = "Destroy an existing ECMP group",
    .full = "Destroy an existing ECMP group by providing its ID",
    .synopsis = "ID=<ECMP ID>",
    .examples = "ID=51",
    .init_cb = ecmp_init,
    .deinit_cb = ecmp_deinit
};

static sh_sand_man_t dnx_l3_ecmp_add_man = {
    .brief = "Add a member to the ECMP group",
    .full = "Add a member to the ECMP group by providing the ECMP group ID and the new member ID",
    .synopsis = "ID=<ECMP ID> intf=<FEC ID>",
    .example_generate_cb = ecmp_add_example_generate,
    .init_cb = ecmp_init,
    .deinit_cb = ecmp_deinit
};

static sh_sand_man_t dnx_l3_ecmp_get_man = {
    .brief = "Receive the data for an ECMP group",
    .full = "Receive the data for an ECMP group by providing the ECMP group ID",
    .synopsis = "ID=<ECMP ID>",
    .examples = "ID=51",
    .init_cb = ecmp_init,
    .deinit_cb = ecmp_deinit
};

static sh_sand_man_t dnx_l3_ecmp_delete_man = {
    .brief = "Delete a member to the ECMP group",
    .full = "Delete a member to the ECMP group by providing the ECMP group ID and the member ID",
    .synopsis = "ID=<ECMP ID> intf=<FEC ID>",
    .example_generate_cb = ecmp_delete_example_generate,
    .init_cb = ecmp_init,
    .deinit_cb = ecmp_deinit
};

static sh_sand_man_t dnx_l3_ecmp_allocation_man = {
    .brief = "Show the bank allocation information",
    .full = "Present information for each ECMP bank including its hierarchy and availability.",
    .synopsis = "[bank=<Bank ID>]",
    .examples = "\n" "bank=0",
    .init_cb = ecmp_init,
    .deinit_cb = ecmp_deinit
};

static sh_sand_option_t dnx_l3_ecmp_dump_options[] = {
    {"ID", SAL_FIELD_TYPE_INT32, "The ECMP group index", "-1"},
    {"TP_Index", SAL_FIELD_TYPE_INT32, "The ECMP TP index", "-1", NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_create_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", "1", NULL, NULL, SH_SAND_ARG_QUIET},
    {"Size", SAL_FIELD_TYPE_INT32, "The ECMP group size", NULL, NULL, NULL, SH_SAND_ARG_QUIET},
    {"fec", SAL_FIELD_TYPE_ARRAY32, "The ECMP group members", "0x20010000", NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_add_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", NULL, NULL, NULL, SH_SAND_ARG_QUIET},
    {"INTerFace", SAL_FIELD_TYPE_INT32, "Intf index", NULL, NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_delete_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", NULL, NULL, NULL, SH_SAND_ARG_QUIET},
    {"INTerFace", SAL_FIELD_TYPE_INT32, "Intf index", NULL, NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_get_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", NULL},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_destroy_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", NULL},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_allocation_options[] = {
    {"bank", SAL_FIELD_TYPE_UINT32, "The ECMP bank index", "20"},
    {NULL}
};

/**
 * \brief
 *    A function that prints the accessed ECMP group and its FEC members.
 *    It is valid for create/add/delete/get/destroy commands.
 * \param [in] unit - unit ID
 * \param [in] ecmp_id - The ID of the ECMP group.
 * \param [in] size - The size of the ECMP group.
 * \param [in] fec_members - The list of members of this ECMP group
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] action - string indicatig what the action to the ECMP group was:
 *          * create
 *          * add
 *          * delete
 *          * destroy
 *          * get
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_ecmp_entry_print(
    int unit,
    uint32 ecmp_id,
    int size,
    int *fec_members,
    sh_sand_control_t * sand_control,
    char *action)
{
    int idx;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ECMP entry %s", action);

    PRT_COLUMN_ADD("ECMP ID");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("FEC members");
    for (idx = 0; idx < 7 && idx < size - 1; idx++)
    {
        PRT_COLUMN_ADD("");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", ecmp_id);
    PRT_CELL_SET("%d", size);
    for (idx = 0; idx < size; idx++)
    {
        PRT_CELL_SET("0x%04x", fec_members[idx]);
        if ((idx + 1) % 8 == 0 && idx + 1 < size)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SKIP(2);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that dumps all ECMP groups that have been saved.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ecmp_gr_id;
    uint32 field_result;
    uint32 entry_handle_id_ecmp;
    uint32 nof_row_members = 0;
    uint32 member_row = 0;
    uint32 row_mem_idx = 0;
    uint32 entry_handle_id;
    uint32 max_nof_members;
    uint32 col_idx;
    uint32 nof_rows = 0;
    uint32 row_idx = 0;
    uint32 ecmp_group_profile = 0;
    uint32 member_base_address = 0;
    uint8 fec_member = 0;
    int is_end, entry_counter = 0;
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int size = DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE;
    int nof_tables = 1;
    int tp_index = 0;
    int table_index;
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    dnx_l3_ecmp_profile_t ecmp_profile;
    dbal_enum_value_field_hierarchy_level_e hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", ecmp_gr_id);
    SH_SAND_GET_INT32("TP_Index", tp_index);

    PRT_TITLE_SET("ECMP Groups Data");

    PRT_COLUMN_ADD("ECMP Group ID");
    PRT_COLUMN_ADD("Base FEC address");
    PRT_COLUMN_ADD("Hierarchy Level");
    PRT_COLUMN_ADD("Profile ID");
    PRT_COLUMN_ADD("ECMP Mode");
    PRT_COLUMN_ADD("Group Size");
    PRT_COLUMN_ADD("Protection");
    PRT_COLUMN_ADD("RPFmode");
    if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        PRT_COLUMN_ADD("TP Profile");
        PRT_COLUMN_ADD("TP Mode");
    }

    /*
     * Allocate handle to the main ECMP table.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id_ecmp));
    /*
     * If a particular ECMP group ID has been entered, then only the information regarding it will be printed.
     * If no ID has been provided, then all ECMP groups will be dumped.
     */
    if (ecmp_gr_id != -1)
    {
        ecmp_gr_id = BCM_L3_ITF_VAL_GET(ecmp_gr_id);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        dbal_entry_key_field32_set(unit, entry_handle_id_ecmp, DBAL_FIELD_ECMP_ID, ecmp_gr_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_ecmp, DBAL_GET_ALL_FIELDS));
        /** ECMP ID  */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_ECMP_ID,
                                                              (uint32 *) &ecmp_gr_id, NULL, 0, TRUE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** Base FEC address */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id_ecmp, DBAL_FIELD_FEC_POINTER_BASE,
                                                            INST_SINGLE, &field_result));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_FEC_POINTER_BASE,
                                                              &field_result, NULL, 0, FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** Hierarchy level */
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_gr_id, (uint32 *) &hierarchy));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, DBAL_FIELD_HIERARCHY_LEVEL, &hierarchy, NULL, 0,
                         TRUE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** ECMP group profile ID */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id_ecmp,
                                                            DBAL_FIELD_ECMP_GROUP_PROFILE, INST_SINGLE,
                                                            &ecmp_group_profile));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_ECMP_GROUP_PROFILE, &ecmp_group_profile, NULL, 0,
                         FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_ecmp_group_info(unit, ecmp_gr_id, 0, &size, intf_array, &ecmp_profile));

        /** ECMP mode */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                              DBAL_FIELD_ECMP_MODE, &ecmp_profile.ecmp_mode, NULL, 0,
                                                              FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** ECMP group size */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                              DBAL_FIELD_MAX_GROUP_SIZE_MINUS_ONE,
                                                              (uint32 *) &ecmp_profile.group_size, NULL,
                                                              0, FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** Protection */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, DBAL_FIELD_PROTECTION_ENABLE,
                         (uint32 *) &ecmp_profile.protection_flag, NULL, 0, FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** RPF mode */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                              DBAL_FIELD_ECMP_RPF_MODE,
                                                              (uint32 *) &ecmp_profile.rpf_mode, NULL, 0, FALSE,
                                                              buffer));
        PRT_CELL_SET("%s", buffer);
        if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
        {
            /** TP Profile */
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_ECMP_TUNNEL_PRIORITY_MAP_PROFILE,
                                                                  (uint32 *) &ecmp_profile.tunnel_priority_map_profile,
                                                                  NULL, 0, FALSE, buffer));
            PRT_CELL_SET("%s", buffer);

            /** TP Mode */
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_ECMP_TUNNEL_PRIORITY_MODE,
                                                                  &ecmp_profile.tunnel_priority_mode, NULL, 0, FALSE,
                                                                  buffer));
            PRT_CELL_SET("%s", buffer);
        }

        PRT_COMMITX;

        /** Print members table */
        if (ecmp_profile.ecmp_mode != DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
        {
            dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;

            /*
             * Get the members base address (relative to table address)
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_group_profile);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, hierarchy);

            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE,
                                       &member_base_address);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                            (unit, ecmp_profile.ecmp_mode, &consistent_table_type));

            if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
            {
                /**Display all tables*/
                if (tp_index == -1)
                {
                    nof_tables = L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(ecmp_profile.tunnel_priority_mode);
                    tp_index = 0;
                    member_row =
                        member_base_address * member_base_address * L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit,
                                                                                                                consistent_table_type);
                /**Display a single table*/
                }
                else
                {
                    nof_tables = 1;
                    member_row =
                        (member_base_address + tp_index) * L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit,
                                                                                                       consistent_table_type);
                    /**Verify that the tp_index is in range*/
                    if ((tp_index < 0)
                        || (tp_index >=
                            L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(ecmp_profile.tunnel_priority_mode)))
                    {
                        nof_tables = 0;
                    }
                }
            }
            else
            {
                member_row =
                    member_base_address * L3_ECMP_CONSISTENT_NOF_ROWS_IN_MEMBER_TABLE(unit, consistent_table_type);
            }
            /**If the tp index is out of range do not display table*/
            if (nof_tables == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong TP index selected.\n");
            }

            for (table_index = 0; table_index < nof_tables; table_index++)
            {
                PRT_TITLE_SET("Members offsets table address 0x%x (tbl type relative addr %d)", member_row,
                              member_base_address);
                for (col_idx = 0; col_idx < DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY; col_idx++)
                {
                    PRT_COLUMN_ADD("%d", (int) col_idx);
                }

                nof_row_members = L3_ECMP_CONSISTENT_NOF_MEMBERS_IN_MEM_ROW(unit, consistent_table_type);

                max_nof_members = dnx_data_l3.ecmp.consistent_tables_info_get(unit, consistent_table_type)->nof_entries;

                /** Determine the number of iterations */
                nof_rows = max_nof_members / DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY;
                if (max_nof_members % DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY != 0)
                {
                    nof_rows++;
                }

                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, &entry_handle_id));

                for (row_idx = 0, entry_counter = 0; row_idx < nof_rows * 2 - 1; row_idx++)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    for (col_idx = 0; col_idx < DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY; col_idx++)
                    {
                        if (row_idx != 0 && row_idx % 2 == 1)
                        {
                            PRT_CELL_SET("%d", (int) entry_counter);
                            entry_counter++;
                        }
                        else
                        {
                            if (row_mem_idx == 0)
                            {
                                /** Keys */
                                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_ROW_ID,
                                                           member_row);
                                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
                            }
                            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                            (unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_VAL, row_mem_idx,
                                             &fec_member));
                            PRT_CELL_SET("%02x", fec_member);
                            if (row_mem_idx + 1 == nof_row_members)
                            {
                                DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, entry_handle_id);
                                member_row++;
                                row_mem_idx = 0;
                            }
                            else
                            {
                                row_mem_idx++;
                            }
                            entry_counter++;
                        }
                    }
                    if (row_idx % 2 == 1)
                    {
                        entry_counter -= DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY;
                    }
                }
                PRT_COMMITX;
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id_ecmp, DBAL_ITER_MODE_ALL));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ecmp, &is_end));

        /** Iterate over all entries in the ECMP_TABLE */
        while (!is_end)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            /** ECMP ID  */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id_ecmp, DBAL_FIELD_ECMP_ID,
                                                                  (uint32 *) &ecmp_gr_id));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_ECMP_ID,
                                                                  (uint32 *) &ecmp_gr_id, NULL, 0, TRUE, buffer));
            PRT_CELL_SET("%s", buffer);

            /** Base FEC address */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id_ecmp, DBAL_FIELD_FEC_POINTER_BASE, INST_SINGLE, &field_result));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                            (unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_FEC_POINTER_BASE, &field_result, NULL, 0, FALSE,
                             buffer));
            PRT_CELL_SET("%s", buffer);

            /** Hierarchy level */
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_gr_id, (uint32 *) &hierarchy));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_HIERARCHY_LEVEL, &hierarchy, NULL, 0, TRUE,
                                                                  buffer));
            PRT_CELL_SET("%s", buffer);

            /** ECMP group profile ID */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id_ecmp,
                                                                DBAL_FIELD_ECMP_GROUP_PROFILE, INST_SINGLE,
                                                                &field_result));

            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE,
                                                                  DBAL_FIELD_ECMP_GROUP_PROFILE, &field_result, NULL,
                                                                  0, FALSE, buffer));
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_ecmp_group_info
                            (unit, ecmp_gr_id, 0, &size, intf_array, &ecmp_profile));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                            (unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, DBAL_FIELD_ECMP_MODE, &ecmp_profile.ecmp_mode,
                             NULL, 0, FALSE, buffer));
            /** ECMP mode */
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_MAX_GROUP_SIZE_MINUS_ONE,
                                                                  (uint32 *) &ecmp_profile.group_size, NULL,
                                                                  0, FALSE, buffer));
            /** ECMP group size */
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_PROTECTION_ENABLE,
                                                                  (uint32 *) &ecmp_profile.protection_flag, NULL,
                                                                  0, FALSE, buffer));
            /** Protection */
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_ECMP_RPF_MODE,
                                                                  (uint32 *) &ecmp_profile.rpf_mode, NULL,
                                                                  0, FALSE, buffer));
            /** RPF mode */
            PRT_CELL_SET("%s", buffer);
            if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
            {
                /** TP Profile */
                SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                      DBAL_FIELD_ECMP_TUNNEL_PRIORITY_MAP_PROFILE,
                                                                      (uint32 *)
                                                                      &ecmp_profile.tunnel_priority_map_profile, NULL,
                                                                      0, FALSE, buffer));
                PRT_CELL_SET("%s", buffer);

                /** TP Mode */
                SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                      DBAL_FIELD_ECMP_TUNNEL_PRIORITY_MODE,
                                                                      &ecmp_profile.tunnel_priority_mode, NULL, 0,
                                                                      FALSE, buffer));
                PRT_CELL_SET("%s", buffer);
            }

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ecmp, &is_end));
            entry_counter++;
        }
        if (entry_counter > 0)
        {
            PRT_INFO_ADD("Total Entries: %d", entry_counter);
            PRT_COMMITX;
        }
        else
        {
            printf("No ECMP group entries have been found\n\n");
        }
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that creates an ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_create_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    uint32 *fec_members;
    uint32 ecmp_id;
    int size;
    int count;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("ID", ecmp_id);
    SH_SAND_GET_INT32("Size", size);
    SH_SAND_GET_ARRAY32("fec", fec_members);

    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.flags = BCM_L3_WITH_ID;
    ecmp_entry.ecmp_intf = ecmp_id;

    /** If the ECMP group is in the extended range, add the needed flag. */
    if (ecmp_id >= dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit))
    {
        ecmp_entry.ecmp_group_flags = BCM_L3_ECMP_EXTENDED;
    }

    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_entry, size, (int *) fec_members));
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, (int *) fec_members, &count));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, (int *) fec_members, sand_control, "create"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that adds a FEC member to an existing ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    uint32 ecmp_id;
    int intf;
    int count;
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int size = DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("ID", ecmp_id);
    SH_SAND_GET_INT32("INTerFace", intf);
    BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_id));
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.ecmp_intf = ecmp_id;

    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp_entry, intf));
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, intf_array, sand_control, "add"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that deletes a FEC member from an existing ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    uint32 ecmp_id;
    int intf;
    int count;
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int size = DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("ID", ecmp_id);
    SH_SAND_GET_INT32("INTerFace", intf);
    BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_id));
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.ecmp_intf = ecmp_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp_entry, intf));
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, intf_array, sand_control, "delete"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that destroys an existing ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_destroy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    uint32 ecmp_id;
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int size = DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE;
    int count;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("ID", ecmp_id);
    BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_id));
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.ecmp_intf = ecmp_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count));
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp_entry));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, intf_array, sand_control, "destroy"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that present the ECMP bank allocation data
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_allocation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 bank_id = 0;
    uint32 hierarchy;
    uint32 first_in_bank;
    uint32 nof_banks =
        dnx_data_l3.ecmp.nof_ecmp_banks_basic_get(unit) + dnx_data_l3.ecmp.nof_ecmp_banks_extended_get(unit);
    uint32 nof_elements;
    uint32 bank_size;
    char *hierarchies[4] = { "1ST hierarchy", "2ND hierarchy", "3RD hierarchy", "NO hierarchy" };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("bank", bank_id);
    PRT_TITLE_SET("ECMP bank allocation");

    PRT_COLUMN_ADD("Bank ID");
    PRT_COLUMN_ADD("Hierarchy");
    PRT_COLUMN_ADD("Allocated/Total");
    PRT_COLUMN_ADD("Reserved for failover");
    if (bank_id < nof_banks)
    {
        nof_banks = bank_id + 1;
    }
    else
    {
        bank_id = 0;
    }
    for (; bank_id < nof_banks; bank_id++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        bank_size = dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);
        first_in_bank = bank_id * bank_size;
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, first_in_bank, &hierarchy));
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.get(unit, bank_id, &nof_elements));
        if (bank_id == 0)
        {
            nof_elements--;
            bank_size--;
        }
        PRT_CELL_SET("%u", bank_id);
        if (nof_elements == 0)
        {
            PRT_CELL_SET("NO hierarchy");
        }
        else
        {
            PRT_CELL_SET("%s", hierarchies[hierarchy]);
        }

        PRT_CELL_SET("%u/%u", nof_elements, bank_size);
        if (bank_id >= dnx_data_l3.ecmp.nof_ecmp_banks_basic_get(unit))
        {
            if (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
            {
                PRT_CELL_SET("Yes");
            }
            else
            {
                PRT_CELL_SET("No");
            }
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that receives the data for an existing ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    uint32 ecmp_id;
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int size = DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE;
    int count = 0;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("ID", ecmp_id);
    BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_id));
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.ecmp_intf = ecmp_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, intf_array, sand_control, "get"));
exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_l3_ecmp_cmds[] = {

    /*
     * keyword, action, command, options, man
     */
    {
     "create", sh_dnx_l3_ecmp_create_cmd, NULL, dnx_l3_ecmp_create_options, &dnx_l3_ecmp_create_man}
    ,
    {
     "dump", sh_dnx_l3_ecmp_dump_cmd, NULL, dnx_l3_ecmp_dump_options, &dnx_l3_ecmp_dump_man}
    ,
    {
     "add", sh_dnx_l3_ecmp_add_cmd, NULL, dnx_l3_ecmp_add_options, &dnx_l3_ecmp_add_man}
    ,
    {
     "delete", sh_dnx_l3_ecmp_delete_cmd, NULL, dnx_l3_ecmp_delete_options, &dnx_l3_ecmp_delete_man}
    ,
    {
     "get", sh_dnx_l3_ecmp_get_cmd, NULL, dnx_l3_ecmp_get_options, &dnx_l3_ecmp_get_man}
    ,
    {
     "destroy", sh_dnx_l3_ecmp_destroy_cmd, NULL, dnx_l3_ecmp_destroy_options, &dnx_l3_ecmp_destroy_man}
    ,
    {
     "allocation", sh_dnx_l3_ecmp_allocation_cmd, NULL, dnx_l3_ecmp_allocation_options, &dnx_l3_ecmp_allocation_man}
    ,
    {
     NULL}
};
