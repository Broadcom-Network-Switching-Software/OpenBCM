
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    diag_sand_dsig.c
 * Purpose:    Routines for handling debug and internal signals
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_actions.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "diag_dnx_field_key.h"
#include "diag_dnx_field_group.h"
#include "diag_dnx_field_context.h"
#include "diag_dnx_field_qualifier.h"
#include "diag_dnx_field_action.h"
#include "diag_dnx_field_attach.h"
#include "diag_dnx_field_entry.h"
#include "diag_dnx_field_tcam.h"
#include "diag_dnx_field_ace.h"
#include "diag_dnx_field_efes.h"
#include "diag_dnx_field_last.h"
#include "diag_dnx_field_range.h"
#include "diag_dnx_field_virtualwire.h"

/*
 * Globals
 * {
 */

static sh_sand_man_t Sh_dnx_field_key_man = {
    .brief = "Presenting operations related to keys (Currently display of configuration and allocation)",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_fem_base_man = {
    .brief = "Presenting operations related to FEMs (Currently display of configuration and allocation)",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_group_base_man = {
    .brief = "Presenting operations related to Field Groups (Currently display of configuration and allocation)",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_context_base_man = {
    .brief = "Presenting operations related to Field Context (Currently display of configuration and allocation)",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_attach_base_man = {
    .brief = "Presenting operations related to Field Group Attach info.",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_qualifier_base_man = {
    .brief = "Presenting operations related to Field User-Defined Qualifiers info.",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_action_base_man = {
    .brief = "Presenting operations related to Field User-Defined Actions info.",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_entry_base_man = {
    .brief = "Presenting operations related to Entries",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_tcam_base_man = {
    .brief = "Presenting operations related to TCAM (banks, management)",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_ace_base_man = {
    .brief = "Presenting operations related to ACE Format and ACE Entry",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_efes_base_man = {
    .brief = "Presenting operations related to EFESs",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_last_base_man = {
    .brief = "Presenting operations related to Last Packet.",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_range_base_man = {
    .brief = "Presenting operations related to Range Qualifier.",
    .full = NULL
};

static sh_sand_man_t Sh_dnx_field_virtualwire_base_man = {
    .brief = "Presenting operations related to Range Qualifier.",
    .full = NULL
};

sh_sand_man_t Sh_dnx_field_man = {
    .brief = "Field Processor diagnostic commands",
    .full = NULL
};

/*
 * Globals for init functions
 */
bcm_field_context_t Sh_dnx_field_init_context_ipmf1 = 15;
bcm_field_context_t Sh_dnx_field_init_context_ipmf1_st = 11;
bcm_field_context_t Sh_dnx_field_init_context_ipmf2 = 12;
bcm_field_context_t Sh_dnx_field_init_context_ipmf2_based_default_1 = 18;
bcm_field_context_t Sh_dnx_field_init_context_ipmf2_based_default_2 = 17;
bcm_field_context_t Sh_dnx_field_init_context_ipmf3 = 16;
bcm_field_context_t Sh_dnx_field_init_context_epmf = 12;
bcm_field_group_t Sh_dnx_field_init_fg_id_tcam_half = 70;
bcm_field_group_t Sh_dnx_field_init_fg_id_tcam_single = 72;
bcm_field_group_t Sh_dnx_field_init_fg_id_tcam_double = 73;
bcm_field_group_t Sh_dnx_field_init_fg_id_const_1 = 71;
bcm_field_group_t Sh_dnx_field_init_fg_id_state_table = 74;
bcm_field_group_t Sh_dnx_field_init_fg_id_exem = 75;
bcm_field_group_t Sh_dnx_field_init_fg_id_direct_extraction = 76;
bcm_field_group_t Sh_dnx_field_init_fg_id_tcam_direct = 77;
bcm_field_group_t Sh_dnx_field_init_fg_id_mdb_direct = 78;
bcm_field_group_t Sh_dnx_field_init_fg_id_const_2 = 79;

/* *INDENT-OFF* */
sh_sand_cmd_t Sh_dnx_field_cmds[] = {
    {"key",           NULL,                    Sh_dnx_field_key_cmds,           NULL, &Sh_dnx_field_key_man             , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"fem",           NULL,                    Sh_dnx_field_fem_base_cmds,      NULL, &Sh_dnx_field_fem_base_man        , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"group",         NULL,                    Sh_dnx_field_group_cmds,         NULL, &Sh_dnx_field_group_base_man      , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"context",       NULL,                    Sh_dnx_field_context_cmds,       NULL, &Sh_dnx_field_context_base_man    , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"attach",        NULL,                    Sh_dnx_field_attach_cmds,        NULL, &Sh_dnx_field_attach_base_man     , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"qualifier",     NULL,                    Sh_dnx_field_qualifier_cmds,     NULL, &Sh_dnx_field_qualifier_base_man  , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"action",        NULL,                    Sh_dnx_field_action_cmds,        NULL, &Sh_dnx_field_action_base_man     , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"entry",         NULL,                    Sh_dnx_field_entry_cmds,         NULL, &Sh_dnx_field_entry_base_man      , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"tcam",          NULL,                    Sh_dnx_field_tcam_cmds,          NULL, &Sh_dnx_field_tcam_base_man       , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"ace",           NULL,                    Sh_dnx_field_ace_cmds,           NULL, &Sh_dnx_field_ace_base_man        , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"efes",          NULL,                    Sh_dnx_field_efes_cmds,          NULL, &Sh_dnx_field_efes_base_man       , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"last",          NULL,                    Sh_dnx_field_last_cmds,          NULL, &Sh_dnx_field_last_base_man       , NULL, NULL, SH_CMD_SKIP_MEMORY_CHECK},
    {"range",         NULL,                    Sh_dnx_field_range_cmds,         NULL, &Sh_dnx_field_range_base_man      , NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"virtualwire",   NULL,                    Sh_dnx_field_virtualwire_cmds,   NULL, &Sh_dnx_field_virtualwire_base_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {NULL}
};

/* *INDENT-ON* */
/**
 * Enum for option minimal and maximal value of fem_program id for display
 */
sh_sand_enum_t Field_fem_program_enum_table_for_display[] = {
    {"FEM_PROGRAM_LOWEST", 0, "Lowest value for FEM program"},
    {"FEM_PROGRAM_HIGHEST", 0, "Highest value for FEM program",
     "DNX_DATA.field.stage.stage_info.nof_fem_programs(DNX_FIELD_STAGE_IPMF1)-1"},
    {NULL}
};

/**
 * Enum for option requiring all 'pmf' and none 'pmf' stages
 */
sh_sand_enum_t Field_stage_enum_table[] = {
    {"IPMF1", DNX_FIELD_STAGE_IPMF1, "ingress PMF1 stage"},
    {"IPMF2", DNX_FIELD_STAGE_IPMF2, "ingress PMF2 stage"},
    {"IPMF3", DNX_FIELD_STAGE_IPMF3, "ingress PMF3 stage"},
    {"EPMF", DNX_FIELD_STAGE_EPMF, "Egress PMF stage"},
    {"ACE", DNX_FIELD_STAGE_ACE, "ACE (Egress PMF Extension) stage"},
    {"EXTERNAL", DNX_FIELD_STAGE_EXTERNAL, "External stage"},
    {"STAGE_LOWEST", DNX_FIELD_STAGE_FIRST, "Minimal value for field stage"},
    {"STAGE_HIGHEST", (DNX_FIELD_STAGE_NOF - 1), "Maximal value for field stage"},
    {"all", DNX_FIELD_STAGE_NOF, "All PMF stages"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'context' diagnostic.
 */
sh_sand_enum_t Field_context_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EPMF", 3, "Egress PMF stage"},
    {"EXTERNAL", 4, "External stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 4, "Maximal value for field stage"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'efes' diagnostic.
 */
sh_sand_enum_t Field_efes_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EPMF", 3, "Egress PMF stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 3, "Maximal value for field stage"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'group' diagnostic.
 */
sh_sand_enum_t Field_group_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EPMF", 3, "Egress PMF stage"},
    {"EXTERNAL", 4, "External stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 4, "Maximal value for field stage"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'last' diagnostic.
 */
sh_sand_enum_t Field_last_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EPMF", 3, "Egress PMF stage"},
    {"EXTERNAL", 4, "External stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 4, "Maximal value for field stage"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'qualifier' diagnostic.
 */
sh_sand_enum_t Field_qualifier_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EPMF", 3, "Egress PMF stage"},
    {"EXTERNAL", 4, "External stage"},
    {"L4_OPS", 5, "L4OPs stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 5, "Maximal value for field stage"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'action' diagnostic.
 */
sh_sand_enum_t Field_action_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EPMF", 3, "Egress PMF stage"},
    {"ACE", 4, "ACE stage"},
    {"EXTERNAL", 5, "External stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 5, "Maximal value for field stage"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'range' diagnostic.
 */
sh_sand_enum_t Field_range_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EPMF", 3, "Egress PMF stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 3, "Maximal value for field stage"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'virtualwire' diagnostic.
 */
sh_sand_enum_t Field_virtualwire_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EXTERNAL", 3, "External stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 3, "Maximal value for field stage"},
    {NULL}
};

/**
 * Enum for option requiring all 'pmf' qualifier classes
 */
sh_sand_enum_t Field_qual_class_enum_table[] = {
    {"META", DNX_FIELD_QUAL_CLASS_META, "Meta Qual Class"},
    {"LAYER", DNX_FIELD_QUAL_CLASS_LAYER_RECORD, "LR Qual Class"},
    {"HEADER", DNX_FIELD_QUAL_CLASS_HEADER, "Header Qual class"},
    {"all", DNX_FIELD_QUAL_CLASS_NOF, "All Qual classes"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' field groups
 */
sh_sand_enum_t Field_group_enum_table[] = {
    {"MIN", 0, "Minimal value for field group id"},
    {"MAX", 0, "Maximal value for field group id", "DNX_DATA.field.group.nof_fgs-1"},
    {NULL}
};
/**
 * Enum for option requiring 'pmf' ACE Format IDs
 */
sh_sand_enum_t Field_ace_format_id_enum_table[] = {
    {"MIN", 1, "Minimal value for ace format id"},
    {"MAX", 0, "Maximal value for ace format id", "DNX_DATA.field.ace.nof_ace_id-1"},
    {NULL}
};
/**
 * Enum for option requiring 'pmf' ACE Entry IDs
 */
sh_sand_enum_t Field_ace_entry_id_enum_table[] = {
    {"MIN", 0, "Minimal value for ace entry id", "DNX_DATA.field.ace.min_key_range_pmf"},
    {"MAX", 0, "Maximal value for ace entry id", "DNX_DATA.field.ace.max_key_range_pmf-1"},
    {NULL}
};
/**
 * Enum for option requiring 'pmf' TCAM Handler IDs
 */
sh_sand_enum_t Field_tcam_handler_id_enum_table[] = {
    {"MIN", 0, "Minimal value for tcam handler id"},
    {"MAX", 0, "Maximal value for tcam handler id", "DNX_DATA.field.tcam.nof_tcam_handlers-1"},
    {NULL}
};
/**
 * Enum for option requiring 'pmf' TCAM Bank IDs
 */
sh_sand_enum_t Field_tcam_bank_id_enum_table[] = {
    {"MIN", 0, "Minimal value for tcam bank id"},
    {"MAX", 0, "Maximal value for tcam bank id", "DNX_DATA.field.tcam.nof_banks-1"},
    {NULL}
};
/**
 * Enum for option requiring 'pmf' field qualifier
 */
sh_sand_enum_t Field_qualifier_enum_table[] = {
    {"MIN", 0, "Minimal value for qualifier id"},
    {"MAX", 0, "Maximal value for qualifier id", "DNX_DATA.field.qual.user_nof-1"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' field virtualwire qualifier
 */
sh_sand_enum_t Field_qualifier_vw_enum_table[] = {
    {"MIN", 0, "Minimal value for virtualwire qualifier id", "DNX_DATA.field.qual.vw_1st"},
    {"MAX", 0, "Maximal value for virtualwire qualifier id", "DNX_DATA.field.qual.vw_1st+DNX_DATA.field.qual.vw_nof-1"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' field action
 */
sh_sand_enum_t Field_action_enum_table[] = {
    {"MIN", 0, "Minimal value for action id"},
    {"MAX", 0, "Maximal value for action id", "DNX_DATA.field.action.user_nof-1"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' field virtual wire action
 */
sh_sand_enum_t Field_action_vw_enum_table[] = {
    {"MIN", 0, "Minimal value for virtualwire action id", "DNX_DATA.field.action.vw_1st"},
    {"MAX", 0, "Maximal value for virtualwire action id",
     "DNX_DATA.field.action.vw_1st+DNX_DATA.field.action.vw_nof-1"},
    {NULL}
};

/**
 * Enum for option requiring 'pmf' FEMs
 */
sh_sand_enum_t Field_fem_enum_table[] = {
    {"MIN", 0, "Minimal value for FEM id"},
    {"MAX", 0, "Minimal value for FEM id", "DNX_DATA.field.stage.stage_info.nof_fem_id(DNX_FIELD_STAGE_IPMF1)-1"},
    {NULL}
};
/**
 * Enum for option requiring 'pmf' active actions (for FEM)
 */
sh_sand_enum_t Field_fem_active_actions_enum_table[] = {
    {"IGNORE", DNX_FIELD_IGNORE_ALL_ACTIONS, "Ignore this input fem active action variable"},
    {NULL}
};
/**
 * Enum for option requiring all 'group types'
 */
sh_sand_enum_t Group_type_enum_table[] = {
    {"GROUP_TYPE_LOWEST", DNX_FIELD_GROUP_TYPE_FIRST, "First (lowest in range) group type"},
    {"TCAM", DNX_FIELD_GROUP_TYPE_TCAM, "TCAM group type"},
    {"DIRECT_TABLE_TCAM", DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM, "Direct-table on TCAM group type"},
    {"DIRECT_TABLE_MDB", DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB, "Direct-table on MDB group type"},
    {"EXEM", DNX_FIELD_GROUP_TYPE_EXEM, "EXEM group type"},
    {"STATE_TABLE", DNX_FIELD_GROUP_TYPE_STATE_TABLE, "State-table group type"},
    {"DIRECT_EXTRACTION", DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION, "Direct-extraction group type"},
    {"CONST", DNX_FIELD_GROUP_TYPE_CONST, "Group type without key, only actions with zero size"},
    {"KBP", DNX_FIELD_GROUP_TYPE_EXTERNAL_TCAM, "KBP Field Group type. ACL group in the FLP block"},
    {"GROUP_TYPE_HIGHEST", (DNX_FIELD_GROUP_TYPE_NOF - 1), "Last (Highest in range) group type"},
    {NULL}
};

/**
 * Enum for option requiring all 'range types' for range diagnostic.
 */
sh_sand_enum_t Field_range_list_range_type_enum_table[] = {
    {"L4_SRC", 0, "L4 source port range type"},
    {"L4_DST", 1, "L4 destination range type"},
    {"OUT_LIF", 2, "Outlif range type"},
    {"PACKET_HEADER_SIZE", 3, "Packet header size range type"},
    {"RANGE_TYPE_LOWEST", 0, "Minimal value for range type"},
    {"RANGE_TYPE_HIGHEST", 3, "Maximal value for range type"},
    {NULL}
};

/**
 * Enum for option requiring all 'range types' for range extended diagnostic.
 */
sh_sand_enum_t Field_range_extended_range_type_enum_table[] = {
    {"L4_SRC", 0, "L4 source port range type"},
    {"L4_DST", 1, "L4 destination range type"},
    {"L4OPS_PKT_HDR_SIZE", 2, "Extended L4 OPS Packet heder size range type"},
    {"IN_TTL", 3, "In TTL range type"},
    {"FFC1_LOW", 4, "FFC 1 Low range type"},
    {"FFC1_HIGH", 5, "FFC 1 High range type"},
    {"FFC2_LOW", 6, "FFC 2 Low range type"},
    {"FFC2_HIGH", 7, "FFC 2 High range type"},
    {"RANGE_TYPE_LOWEST", 0, "Minimal value for range type"},
    {"RANGE_TYPE_HIGHEST", 7, "Maximal value for range type"},
    {NULL}
};

/**
 * Enum for option requiring all 'range config types' for range extended diagnostic.
 */
sh_sand_enum_t Field_range_extended_range_config_type_enum_table[] = {
    {"RANGE_CONFIG_TYPE_LOWEST", 0, "Minimal value for range config type"},
    {"RANGE_CONFIG_TYPE_HIGHEST", 1, "Maximal value for range config type"},
    {NULL}
};

/**
 * Enum for option requiring 'type' of test (BCM or DNX)
 */
sh_sand_enum_t Field_level_enum_table[] = {
    {"BCM", 0},
    {"DNX", 1},
    {NULL}
};

sh_sand_enum_t Field_init_key_enum_table[] = {
    {"MIN", DBAL_ENUM_FVAL_FIELD_KEY_F, "Minimal initial key"},
    {"F", DBAL_ENUM_FVAL_FIELD_KEY_F, "Initial key 'F' (IPMF1)"},
    {"G", DBAL_ENUM_FVAL_FIELD_KEY_G, "Initial key 'G' (IPMF1)"},
    {"H", DBAL_ENUM_FVAL_FIELD_KEY_H, "Initial key 'H' (IPMF1)"},
    {"I", DBAL_ENUM_FVAL_FIELD_KEY_I, "Initial key 'I' (IPMF1)"},
    {"J", DBAL_ENUM_FVAL_FIELD_KEY_J, "Initial key 'J' (IPMF1)"},
    {"MAX", DBAL_ENUM_FVAL_FIELD_KEY_J, "Maximal initial key"},
    {NULL}
};

/**
 * Enum for option requiring all 'kbp opcode types'
 */
sh_sand_enum_t Field_kbp_opcode_type_enum_table[] = {
    {"STATIC", 0, "Static allocation of kbp opcode"},
    {"DYNAMIC", 1, "Dynamic allocation of kbp opcode"},
    {NULL}
};

/*
 * }
 */
/*
 * Diagnostics procedures to be used by, say, diag_dnx_field_* and ctest_dnx_field_*
 * {
 */

/*
 * Utility procedures, related to FEMs, to be used for, say, display of FEMs (See ctest_dnx_field_action.c)
 * {
 */

/**
 * See header in diag_dnx_field.h
 */
shr_error_e
appl_dnx_action_swstate_fem_prg_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    unsigned int num_skips_per_row,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr)
{
    dnx_field_fg_id_info_t fg_id_info;
    dnx_field_fem_map_index_t fem_map_index, fem_map_index_max;
    dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id, fem_program, &(fg_id_info.fg_id)));
    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.input_offset.get(unit, fem_id, fem_program, &(fg_id_info.input_offset)));
    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.ignore_actions.get(unit, fem_id, fem_program, &(fg_id_info.ignore_actions)));
    /*
     * For 'invalid' field groups, just display '---'
     */
    if (fg_id_info.fg_id == DNX_FIELD_GROUP_INVALID)
    {
        PRT_CELL_SET("%s", "---");
    }
    else
    {
        PRT_CELL_SET("%d", fg_id_info.fg_id);
    }
    PRT_CELL_SET("%d", fg_id_info.input_offset);
    PRT_CELL_SET("0x%02X", fg_id_info.ignore_actions);
    PRT_CELL_SET("%d", fem_program);
    /*
     * If enry is marked as 'empty' of actions, then print no info on 'map index' and 'encoded actions'
     */
    if (fg_id_info.ignore_actions != DNX_FIELD_IGNORE_ALL_ACTIONS)
    {
        fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            /*
             * Skip the above cells since we start an array display here
             */
            PRT_CELL_SKIP(num_skips_per_row);
            SHR_IF_ERR_EXIT(FEM_INFO_FEM_ENCODED_ACTIONS.get
                            (unit, fem_id, fem_program, fem_map_index, &(fem_encoded_actions[fem_map_index])));
            PRT_CELL_SET("%d", fem_map_index);
            if (fem_encoded_actions[fem_map_index] == DNX_FIELD_ACTION_INVALID)
            {
                PRT_CELL_SET("%s", "---");
            }
            else
            {
                PRT_CELL_SET("0x%08X", fem_encoded_actions[fem_map_index]);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Check HW contents of all 'conditions' for a specified 'fem id, fem program'
 *   pair and indicate whether specified 'action' is used any condition that
 *   is valid.
 *   Note that 'action' index is referred to below as 'map_index'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *   Indicator for which 'FEM program' this operation is intended.
 * \param [in] fem_map_index -
 *   Indicator for which 'action index' this operation is intended.
 * \param [out] action_is_valid_p -
 *   Pointer to int. This procedure loads pointed memory by a non-zero
 *   value if specified action index is found on a valid condition for
 *   specified 'fem_id,fem_program'.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Usaed for display of 'fem programs' that point to a specific 'action'.
 * \see
 *   * field_fem_display_actions_cb
 */
static shr_error_e
appl_dnx_action_hw_fem_program_is_action_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_fem_map_index_t fem_map_index,
    int *action_is_valid_p)
{
    dnx_field_fem_condition_t fem_condition_index, fem_condition_index_max;
    dnx_field_fem_map_index_entry_t dnx_field_fem_map_index_entry;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);
    DNX_FIELD_FEM_MAP_INDEX_VERIFY(unit, fem_map_index);
    fem_condition_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
    *action_is_valid_p = FALSE;
    for (fem_condition_index = 0; fem_condition_index < fem_condition_index_max; fem_condition_index++)
    {
        dnx_field_fem_map_index_entry.fem_id = fem_id;
        dnx_field_fem_map_index_entry.fem_program = fem_program;
        dnx_field_fem_map_index_entry.fem_condition = fem_condition_index;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_map_index_hw_get(unit, &dnx_field_fem_map_index_entry));
        if (dnx_field_fem_map_index_entry.fem_condition_entry.fem_action_valid)
        {
            /*
             * Enter if one 'valid' condition was found.
             * If the 'action' it points to is the same as on input,
             * the return and specify 'is_valid'.
             */
            if (dnx_field_fem_map_index_entry.fem_condition_entry.fem_map_index == fem_map_index)
            {
                *action_is_valid_p = TRUE;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_field.h
 */
shr_error_e
appl_dnx_action_hw_fem_is_any_action_on_condition_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    int *valid_action_found_p)
{
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    int valid_condition_found;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);

    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    *valid_action_found_p = FALSE;
    for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_is_any_condition_valid
                        (unit, fem_id, fem_program_index, &valid_condition_found));
        if (valid_condition_found != FALSE)
        {
            /*
             * If a valid 'condition' is found then it must point to one of the
             * 4 available actions a valid action was found.
             * Note that 'action type' on that 'action' may be 'invalid' but we still
             * consider it worthwhile to display.
             */
            *valid_action_found_p = TRUE;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Check HW contents of all 'conditions' for a specified 'FEM' and
 *   indicate whether there is any condition that is both valid and
 *   points to specified 'action' (indicated as 'fem_map_index').
 *   Indicates, then, whether at specified action is 'valid' for this FEM.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_map_index -
 *   Indicator for which 'action index' this operation is intended.
 * \param [out] valid_action_found_p -
 *   Pointer to int. This procedure loads pointed memory by a non-zero value if
 *   at least one action is 'valid' for specified 'fem_id'.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Used to verify whether specified action should be displayed for
 *      specified 'fem_id'. If there is no match, specified action
 *      will, in most cases, not be displayed for that 'fem_id'.
 * \see
 *   * field_fem_display_actions_cb
 */
static shr_error_e
appl_dnx_action_hw_fem_is_action_on_condition_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_map_index_t fem_map_index,
    int *valid_action_found_p)
{
    dnx_field_fem_condition_t fem_condition_index, fem_condition_index_max;
    dnx_field_fem_map_index_entry_t dnx_field_fem_map_index_entry;
    dnx_field_fem_program_t fem_program_index, fem_program_max;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);

    fem_condition_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    *valid_action_found_p = FALSE;
    dnx_field_fem_map_index_entry.fem_id = fem_id;
    for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
    {
        dnx_field_fem_map_index_entry.fem_program = fem_program_index;
        for (fem_condition_index = 0; fem_condition_index < fem_condition_index_max; fem_condition_index++)
        {
            dnx_field_fem_map_index_entry.fem_condition = fem_condition_index;
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_map_index_hw_get(unit, &dnx_field_fem_map_index_entry));
            if (dnx_field_fem_map_index_entry.fem_condition_entry.fem_action_valid)
            {
                /*
                 * Enter if a 'valid' condition was found.
                 */
                if (dnx_field_fem_map_index_entry.fem_condition_entry.fem_map_index == fem_map_index)
                {
                    /*
                     * Enter if 'action', on that 'valid' condition, matches input.
                     */
                    *valid_action_found_p = TRUE;
                    break;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_field.h
 */
shr_error_e
appl_dnx_action_hw_fem_is_any_condition_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    int *valid_condition_found_p)
{
    dnx_field_fem_condition_t fem_condition_index, fem_condition_index_max;
    dnx_field_fem_map_index_entry_t dnx_field_fem_map_index_entry;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);

    fem_condition_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
    *valid_condition_found_p = FALSE;
    for (fem_condition_index = 0; fem_condition_index < fem_condition_index_max; fem_condition_index++)
    {
        dnx_field_fem_map_index_entry.fem_id = fem_id;
        dnx_field_fem_map_index_entry.fem_program = fem_program;
        dnx_field_fem_map_index_entry.fem_condition = fem_condition_index;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_map_index_hw_get(unit, &dnx_field_fem_map_index_entry));
        if (dnx_field_fem_map_index_entry.fem_condition_entry.fem_action_valid)
        {
            /*
             * Enter if one 'valid' condition was found.
             * Caller requires to see all condition entries on fem programs provided at least one condition is 'valid'.
             * So, do print.
             */
            *valid_condition_found_p = TRUE;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Display HW contents of all info related to an 'action index' ('fem_map_index') on
 *   a specified 'fem_id'. This info is: 'action type', 'adder', all 'bit descriptors'
 *   and indication of the 'fem_programs' that point to this 'action index'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_map_index -
 *   Indicator for which 'action index' this operation is intended.
 * \param [in] num_skips_per_row -
 *   Print formatting instruction. Number of columns to skip before getting to the
 *   "action specific info" columns. Since some users add columns before calling this
 *   procedure, this input enables internal control over these added columns.
 * \param [in] non_empty_only -
 *    Flag. Boolean. If non-zero then display only actions which are pointed by at least one 'valid'
 *    'condition'. Otherwise, display HW contents regardless of 'valid' state.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \param [in] prt_ctr -
 *   Control information related PRT tables. Required for procedures which, for example, add
 *   cells to an existing table.
 * \param [out] is_action_user_defined_p -
 *   Indication if the presented action was User Defined or no.
 *   In case yes value will be set to TRUE, otherwise FALSE.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   In the standard case (non_empty_only = TRUE), the caller of this procedure is assumed
 *   to have verified that this action is pointed by at least one 'condition' on one of the
 *   'fem_programs' that are assigned to this 'fem_id'.
 * \see
 *   * field_fem_display_conditions_cb
 */
static shr_error_e
appl_dnx_action_hw_fem_single_action_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_map_index_t fem_map_index,
    unsigned int num_skips_per_row,
    int non_empty_only,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr,
    uint8 *is_action_user_defined_p)
{
    dnx_field_fem_action_info_t dnx_field_fem_action_info;
    dnx_field_fem_action_entry_t *dnx_field_fem_action_entry_p;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    dnx_field_action_t fem_encoded_action;
    int action_is_valid;
    unsigned int num_skips_before_bit_descriptor;
    dnx_field_action_type_t dnx_action_type;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_MAP_INDEX_VERIFY(unit, fem_map_index);

    /*
     * Note: This procedure is invoked after caller has verified that specified action index
     * ('fem_map_index') is valis on specified 'fem_id'. Therefore, specified action must
     * be valid on at least one 'fem_program' under this 'fem_id'.
     */
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    dnx_field_fem_action_info.fem_id = fem_id;
    dnx_field_fem_action_info.fem_map_index = fem_map_index;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_action_info_hw_get(unit, &dnx_field_fem_action_info));
    dnx_field_fem_action_entry_p = &(dnx_field_fem_action_info.fem_action_entry);
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(num_skips_per_row);
    /*
     * 'num_skips_before_bit_descriptor' indicates the number of skips required before
     * printing each bit descriptor.
     * It is calculated by summing up all fields up to the point where bit descriptors
     * are printed.
     * Note that bit descriptors are all printed on the same column.
     */
    num_skips_before_bit_descriptor = 0;
    PRT_CELL_SET("%d", (unsigned int) fem_map_index);
    num_skips_before_bit_descriptor++;
    /*
     * Display 'action type' as a number.
     */
    PRT_CELL_SET("%d", (unsigned int) (dnx_field_fem_action_entry_p->fem_action));
    num_skips_before_bit_descriptor++;
    {
        /*
         * Find 'encoded action', corresponding to this action, on at least one 'fem_program'.
         * There must be one. See remark above.
         */
        int found;
        found = FALSE;
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_program_is_action_valid(unit,
                                                                           fem_id, fem_program_index, fem_map_index,
                                                                           &action_is_valid));
            if (action_is_valid == TRUE)
            {
                /*
                 * Get, from SWSTATE, the encoded value for this action type.
                 */
                SHR_IF_ERR_EXIT(FEM_INFO_FEM_ENCODED_ACTIONS.get(unit,
                                                                 fem_id, fem_program_index, fem_map_index,
                                                                 &fem_encoded_action));
                dnx_field_stage = DNX_ACTION_STAGE(fem_encoded_action);
                found = TRUE;
                break;
            }
        }
        if (found != TRUE)
        {
            if (non_empty_only == TRUE)
            {
                /*
                 * Reach here if only 'valid' action indices are to be displayed.
                 */
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Could not find 'encoded action' for action type %d (index %d on fem_id %d).\r\n"
                             "==> Inconsistency between HW and SWSTATE. Probably internal error.\r\n",
                             dnx_field_fem_action_entry_p->fem_action, fem_map_index, fem_id);
            }
            else
            {
                /*
                 * Reach here if all action indices are to be displayed.
                 * If no 'fem_program' is found to be pointing to this 'action index' then it
                 * is not in use. Prepare a fake 'encoded_action' for it.
                 */
                SHR_IF_ERR_EXIT(dnx_field_map_action_type_to_predef_dnx_action
                                (unit, DNX_FIELD_STAGE_IPMF2, dnx_field_fem_action_entry_p->fem_action,
                                 &fem_encoded_action));
                dnx_field_stage = DNX_FIELD_STAGE_IPMF2;
            }
        }

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type(unit, dnx_field_stage, fem_encoded_action, &dnx_action_type));
        if (dnx_action_type != dnx_field_fem_action_entry_p->fem_action)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action type extracted from 'encoded action' (%d) is not the same as action type on hw (%d) (index %d on fem_id %d).\r\n"
                         "==> Inconsistency between HW and SWSTATE. Probably internal error.\r\n",
                         dnx_action_type, dnx_field_fem_action_entry_p->fem_action, fem_map_index, fem_id);
        }
    }

    if (DNX_ACTION_CLASS(fem_encoded_action) == DNX_FIELD_ACTION_CLASS_USER)
    {
        *is_action_user_defined_p = TRUE;
    }
    else
    {
        *is_action_user_defined_p = FALSE;
    }

    PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, fem_encoded_action));

    num_skips_before_bit_descriptor++;
    if (fem_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
    {
        PRT_CELL_SET("%s", "--");
    }
    else
    {
        PRT_CELL_SET("%d", (unsigned int) (dnx_field_fem_action_entry_p->fem_adder));
    }
    num_skips_before_bit_descriptor++;
    {
        /*
         * Now display indication, per fem_program, on whether it uses this 'action'
         */
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_program_is_action_valid(unit,
                                                                           fem_id, fem_program_index, fem_map_index,
                                                                           &action_is_valid));
            if (action_is_valid == TRUE)
            {
                /*
                 * Specify this fem_program points to this 'action'.
                 */
                PRT_CELL_SET("%s", "V ");
            }
            else
            {
                /*
                 * Specify this fem_program does not point to this 'action'.
                 */
                PRT_CELL_SET("%s", "--");
            }
            num_skips_before_bit_descriptor++;
        }
    }
    {
        /*
         * Now display all bit descriptors.
         * The number of descriptors is DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION.
         */
        unsigned int bit_in_fem_action, nof_bits_in_fem_action;

        nof_bits_in_fem_action =
            sizeof(dnx_field_fem_action_entry_p->fem_bit_info) / sizeof(dnx_field_fem_action_entry_p->fem_bit_info[0]);
        for (bit_in_fem_action = 0; bit_in_fem_action < nof_bits_in_fem_action; bit_in_fem_action++)
        {
            dnx_field_fem_bit_info_t *fem_bit_info_p;
            fem_bit_info_p = &(dnx_field_fem_action_entry_p->fem_bit_info[bit_in_fem_action]);
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(num_skips_per_row);
            PRT_CELL_SKIP(num_skips_before_bit_descriptor);
            PRT_CELL_SET("%d", bit_in_fem_action);
            switch (fem_bit_info_p->fem_bit_format)
            {
                case DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT:
                {
                    PRT_CELL_SET("%s%d", "I", fem_bit_info_p->fem_bit_value.bit_on_key_select);
                    break;
                }
                case DNX_FIELD_FEM_BIT_FORMAT_FROM_MAP_DATA:
                {
                    PRT_CELL_SET("%s%d", "D", fem_bit_info_p->fem_bit_value.bit_on_map_data);
                    break;
                }
                case DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD:
                {
                    PRT_CELL_SET("%s%d", "C", fem_bit_info_p->fem_bit_value.bit_value);
                    break;
                }
                case DNX_FIELD_FEM_BIT_FORMAT_INVALID:
                {
                    PRT_CELL_SET("%s", "--");
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Illegal 'bit descriptor format' (%d) found. (index %d on fem_id %d).\r\n"
                                 "==> Probably internal sw error.\r\n",
                                 fem_bit_info_p->fem_bit_format, fem_map_index, fem_id);
                    break;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_field.h
 */
shr_error_e
appl_dnx_action_hw_fem_all_action_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    unsigned int num_skips_per_row,
    int non_empty_only,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr,
    uint8 *is_any_action_user_defined_p)
{
    dnx_field_fem_map_index_t fem_map_index, fem_map_index_max;
    int valid_action_found;
    uint8 is_action_user_defined;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);

    fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
    {
        if (non_empty_only == TRUE)
        {
            /*
             * Just to silence compiler's warning.
             */
            valid_action_found = FALSE;
            SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_is_action_on_condition_valid
                            (unit, fem_id, fem_map_index, &valid_action_found));
            if (valid_action_found != TRUE)
            {
                continue;
            }
        }
        /*
         * At least one 'fem_program' points to this action.
         */
        SHR_IF_ERR_EXIT(appl_dnx_action_hw_fem_single_action_display(unit, fem_id, fem_map_index,
                                                                     num_skips_per_row, non_empty_only, sand_control,
                                                                     prt_ctr, &is_action_user_defined));
        if (is_action_user_defined)
        {
            *is_any_action_user_defined_p = TRUE;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_field.h
 */
shr_error_e
appl_dnx_action_hw_fem_condition_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    unsigned int num_skips_per_row,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr)
{
    dnx_field_fem_condition_t fem_condition_index, fem_condition_index_max;
    dnx_field_fem_map_index_entry_t dnx_field_fem_map_index_entry;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);

    fem_condition_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
    for (fem_condition_index = 0; fem_condition_index < fem_condition_index_max; fem_condition_index++)
    {
        dnx_field_fem_map_index_entry.fem_id = fem_id;
        dnx_field_fem_map_index_entry.fem_program = fem_program;
        dnx_field_fem_map_index_entry.fem_condition = fem_condition_index;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_map_index_hw_get(unit, &dnx_field_fem_map_index_entry));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(num_skips_per_row);
        PRT_CELL_SET("%d", (unsigned int) fem_condition_index);
        if (dnx_field_fem_map_index_entry.fem_condition_entry.fem_action_valid)
        {
            PRT_CELL_SET("%s", "YES");
        }
        else
        {
            PRT_CELL_SET("%s", "NO ");
        }
        PRT_CELL_SET("0x%01X", (unsigned int) (dnx_field_fem_map_index_entry.fem_condition_entry.fem_map_data));
        PRT_CELL_SET("%d", (unsigned int) (dnx_field_fem_map_index_entry.fem_condition_entry.fem_map_index));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_field.h
 */
shr_error_e
appl_dnx_action_full_fem_context_display(
    int unit,
    dnx_field_context_t context_id,
    int non_empty_only,
    dnx_field_fem_id_t fem_id_lower,
    dnx_field_fem_id_t fem_id_upper,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr,
    int *context_was_loaded_p)
{
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID];
    dnx_field_fem_id_t fem_id_index;
    int do_print;

    SHR_FUNC_INIT_VARS(unit);
    *context_was_loaded_p = FALSE;
    for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
    {
        dnx_field_pmf_fem_program_entry[fem_id_index].context_id = context_id;
        dnx_field_pmf_fem_program_entry[fem_id_index].fem_id = fem_id_index;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry[fem_id_index]));
        if (dnx_field_pmf_fem_program_entry[fem_id_index].pmf_fem_program.fem_key_select !=
            DNX_FIELD_INVALID_FEM_KEY_SELECT)
        {
            /*
             * Indicate that at least one context was loaded by valid FEM data.
             */
            *context_was_loaded_p = TRUE;
        }
    }
    /*
     * If no FEM, with 'valid' info, was found and it was required to show 'empty' entries
     * then skip the printing of this line altogether.
     */
    do_print = TRUE;
    if (*context_was_loaded_p != TRUE)
    {
        if (non_empty_only == TRUE)
        {
            do_print = FALSE;
        }
        else
        {
            /*
             * Since caller requested to see all entries, valid or not, indicate, to the caller,
             * that info for display was loaded.
             */
            *context_was_loaded_p = TRUE;
        }
    }
    if (do_print)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        /*
         * A full line with id per each FEM
         */
        PRT_CELL_SET("%d", context_id);
        PRT_CELL_SET("%s", "ID");
        for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
        {
            PRT_CELL_SET("%d", dnx_field_pmf_fem_program_entry[fem_id_index].fem_id);
        }
        /*
         * A full line with 'fem key select' per each FEM. If it is DNX_FIELD_INVALID_FEM_KEY_SELECT
         * then there is no action for this 'fem id' on this context!
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "KSL");
        for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
        {
            if (dnx_field_pmf_fem_program_entry[fem_id_index].pmf_fem_program.fem_key_select ==
                DNX_FIELD_INVALID_FEM_KEY_SELECT)
            {
                PRT_CELL_SET("%s", "--");
            }
            else
            {
                PRT_CELL_SET("%d", dnx_field_pmf_fem_program_entry[fem_id_index].pmf_fem_program.fem_key_select);
            }
        }
        /*
         * A full line with 'fem program' per each FEM
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "PRG");
        for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
        {
            PRT_CELL_SET("%d", dnx_field_pmf_fem_program_entry[fem_id_index].pmf_fem_program.fem_program);
        }
        /*
         * A full line with 'replace_lsb_flag' per each FEM. If set to a non zero value then the 'replace' ('override')
         * feature is enabled and the 'replace_lsb_select' field is meaningful and indicates which of
         * the TCAM results is to be used for replacing the LS 16 bits.
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "RFLG");
        for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
        {
            PRT_CELL_SET("%d", dnx_field_pmf_fem_program_entry[fem_id_index].pmf_fem_program.replace_lsb_flag);
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "TCAM");
        for (fem_id_index = fem_id_lower; fem_id_index <= fem_id_upper; fem_id_index++)
        {
            PRT_CELL_SET("%d", dnx_field_pmf_fem_program_entry[fem_id_index].pmf_fem_program.replace_lsb_select);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in diag_dnx_field.h
 */
shr_error_e
diag_dnx_field_generic_init(
    int unit,
    int add_entries)
{
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_context_hash_info_t hash_info;
    bcm_field_fem_action_info_t fem_action_info;
    bcm_field_efes_action_info_t efes_action_info;
    bcm_core_t core;
    bcm_field_entry_t entry_handle;
    bcm_field_action_t uda_epmf_tc;
    bcm_field_action_t uda_epmf_ace_key;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    int ii;
    int entry_index;
    int fem_conditions_index;
    int fem_mapping_bits_index;
    bcm_field_action_priority_t action_position;
    int offset_on_fg;
    bcm_field_context_t context_ipmf3_default = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create contexts
     */
    bcm_field_context_info_t_init(&context_info);
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    context_info.hashing_enabled = TRUE;
    sal_strncpy_s((char *) context_info.name, "ipmf1_context", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info,
                     &Sh_dnx_field_init_context_ipmf1));
    bcm_field_context_info_t_init(&context_info);
    context_info.state_table_enabled = TRUE;
    sal_strncpy_s((char *) context_info.name, "ipmf1_context_st", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info,
                     &Sh_dnx_field_init_context_ipmf1_st));
    bcm_field_context_info_t_init(&context_info);
    context_info.cascaded_from = Sh_dnx_field_init_context_ipmf1;
    sal_strncpy_s((char *) context_info.name, "ipmf2_derived_context", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info,
                     &Sh_dnx_field_init_context_ipmf2));
    bcm_field_context_info_t_init(&context_info);
    context_info.cascaded_from = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
    sal_strncpy_s((char *) context_info.name, "ipmf2_derived_context_from_default_1", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info,
                     &Sh_dnx_field_init_context_ipmf2_based_default_1));
    bcm_field_context_info_t_init(&context_info);
    context_info.cascaded_from = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
    sal_strncpy_s((char *) context_info.name, "ipmf2_derived_context_from_default_2", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info,
                     &Sh_dnx_field_init_context_ipmf2_based_default_2));
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) context_info.name, "ipmf3_context", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF3, &context_info,
                     &Sh_dnx_field_init_context_ipmf3));
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) context_info.name, "epmf_context", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageEgress, &context_info,
                     &Sh_dnx_field_init_context_epmf));

    /*
     * Configure compare
     */
    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 3;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyL4DstPort;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 2;
    compare_info.first_key_info.qual_info[0].offset = 0;
    compare_info.first_key_info.qual_types[1] = bcmFieldQualifyColor;
    compare_info.first_key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    compare_info.first_key_info.qual_types[2] = bcmFieldQualifyIpProtocol;
    compare_info.first_key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[2].input_arg = 1;
    compare_info.first_key_info.qual_info[2].offset = 0;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = bcmFieldQualifyL4SrcPort;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.second_key_info.qual_info[0].input_arg = 2;
    compare_info.second_key_info.qual_info[0].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                    (unit, 0, bcmFieldStageIngressPMF1, Sh_dnx_field_init_context_ipmf1, 0, &compare_info));

    /*
     * Configure hashing
     */
    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
    hash_info.order = TRUE;
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
    hash_info.key_info.nof_quals = 5;
    hash_info.key_info.qual_types[0] = bcmFieldQualifySrcIp;
    hash_info.key_info.qual_types[1] = bcmFieldQualifyDstIp;
    hash_info.key_info.qual_types[2] = bcmFieldQualifyIpProtocol;
    hash_info.key_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    hash_info.key_info.qual_types[4] = bcmFieldQualifyL4DstPort;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;
    hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[1].input_arg = 1;
    hash_info.key_info.qual_info[1].offset = 0;
    hash_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[2].input_arg = 1;
    hash_info.key_info.qual_info[2].offset = 0;
    hash_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[3].input_arg = 2;
    hash_info.key_info.qual_info[3].offset = 0;
    hash_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[4].input_arg = 2;
    hash_info.key_info.qual_info[4].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_hash_create
                    (unit, 0, bcmFieldStageIngressPMF1, Sh_dnx_field_init_context_ipmf1, &hash_info));

    /*
     * Create presels
     */
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_id.presel_id = 15;
    presel_entry_id.stage = bcmFieldStageIngressPMF2;
    presel_entry_data.context_id = Sh_dnx_field_init_context_ipmf2_based_default_1;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyStateTableData;
    presel_entry_data.qual_data[0].qual_value = 4;
    presel_entry_data.qual_data[0].qual_mask = -1;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_id.presel_id = 15;
    presel_entry_id.stage = bcmFieldStageEgress;
    presel_entry_data.context_id = Sh_dnx_field_init_context_epmf;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 3;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyOutVportClass;
    presel_entry_data.qual_data[0].qual_value = 1;
    presel_entry_data.qual_data[0].qual_mask = -1;
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 1;
    presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeMpls;
    presel_entry_data.qual_data[1].qual_mask = -1;
    presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[2].qual_arg = 2;
    presel_entry_data.qual_data[2].qual_value = bcmFieldLayerTypeIp6;
    presel_entry_data.qual_data[2].qual_mask = -1;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_id.presel_id = 70;
    presel_entry_id.stage = bcmFieldStageEgress;
    presel_entry_data.context_id = Sh_dnx_field_init_context_epmf;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 0;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*
     * Add TCAM field group with half key size
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_tcam_half", sizeof(fg_info.name));
    fg_info.nof_quals = 3;
    fg_info.nof_actions = 3;
    fg_info.qual_types[0] = bcmFieldQualifyGeneratedTtl;
    fg_info.qual_types[1] = bcmFieldQualifyIntPriority;
    fg_info.qual_types[2] = bcmFieldQualifyPacketIsIEEE1588;
    fg_info.action_types[0] = bcmFieldActionAdmitProfile;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[1] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[1] = TRUE;
    fg_info.action_types[2] = bcmFieldActionNetworkQos;
    fg_info.action_with_valid_bit[2] = TRUE;
    fg_id = Sh_dnx_field_init_fg_id_tcam_half;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    /*
     * Add FEM to 4 bit FEM
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;
    for (fem_conditions_index = 0; fem_conditions_index < 16; fem_conditions_index++)
    {
        fem_action_info.fem_condition[fem_conditions_index].extraction_id = 0;
        fem_action_info.fem_condition[fem_conditions_index].is_action_valid = 1;
    }
    fem_action_info.fem_condition[4].extraction_id = 1;
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionOutVport3Raw;
    for (fem_mapping_bits_index = 0; fem_mapping_bits_index < 3; fem_mapping_bits_index++)
    {
        fem_action_info.fem_extraction[0].output_bit[fem_mapping_bits_index].source_type =
            bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[fem_mapping_bits_index].offset = fem_mapping_bits_index + 1;
    }
    fem_action_info.fem_extraction[0].output_bit[fem_mapping_bits_index].source_type =
        bcmFieldFemExtractionOutputSourceTypeForce;
    fem_action_info.fem_extraction[0].output_bit[fem_mapping_bits_index].forced_value = 1;
    fem_action_info.fem_extraction[1].action_type = bcmFieldActionOutVport3Raw;
    for (fem_mapping_bits_index = 0; fem_mapping_bits_index < 3; fem_mapping_bits_index++)
    {
        fem_action_info.fem_extraction[1].output_bit[fem_mapping_bits_index].source_type =
            bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[1].output_bit[fem_mapping_bits_index].forced_value = 0;
    }
    action_position = BCM_FIELD_ACTION_POSITION(1, 1);
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, fg_id, action_position, &fem_action_info));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    attach_info.payload_info.action_info[1].priority = BCM_FIELD_ACTION_POSITION(0, 7);
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, Sh_dnx_field_init_context_ipmf1, &attach_info));

    if (add_entries)
    {
        /*
         * Add entries differently by core
         */
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.nof_entry_quals = 2;
        entry_info.nof_entry_actions = fg_info.nof_actions;
        fg_info.qual_types[0] = bcmFieldQualifyGeneratedTtl;
        fg_info.qual_types[1] = bcmFieldQualifyIntPriority;
        fg_info.qual_types[2] = bcmFieldQualifyPacketIsIEEE1588;
        entry_info.entry_qual[0].type = bcmFieldQualifyIntPriority;
        entry_info.entry_qual[0].value[0] = 2;
        entry_info.entry_qual[0].mask[0] = 0x7;
        entry_info.entry_qual[1].type = bcmFieldQualifyGeneratedTtl;
        entry_info.entry_qual[1].mask[0] = 0xFF;
        entry_info.entry_action[0].type = bcmFieldActionAdmitProfile;
        entry_info.entry_action[0].value[0] = 3;
        entry_info.entry_action[1].type = bcmFieldActionPrioIntNew;
        entry_info.entry_action[1].value[0] = 4;
        entry_info.entry_action[2].type = bcmFieldActionNetworkQos;
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            entry_info.entry_qual[1].type = bcmFieldQualifyGeneratedTtl;
            entry_info.entry_qual[1].mask[0] = 0xFF;
            entry_info.core = core;
            for (entry_index = 0; entry_index < (4 + (5 * core)); entry_index++)
            {
                entry_info.entry_qual[1].value[0] = entry_index;
                entry_info.entry_action[2].value[0] = entry_index + 1;
                SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle));
            }
        }
    }
    /*
     * EFES add
     */
    SHR_IF_ERR_EXIT(bcm_field_group_action_offset_get(unit, 0, fg_id, fg_info.action_types[0], &offset_on_fg));
    bcm_field_efes_action_info_t_init(&efes_action_info);
    efes_action_info.is_condition_valid = FALSE;
    efes_action_info.efes_condition_conf[0].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[0].action_type = bcmFieldActionUDHBase1;
    efes_action_info.efes_condition_conf[0].action_lsb = offset_on_fg;
    efes_action_info.efes_condition_conf[0].action_nof_bits = 2;
    efes_action_info.efes_condition_conf[0].action_with_valid_bit = FALSE;
    action_position = BCM_FIELD_ACTION_POSITION(0, 1);
    SHR_IF_ERR_EXIT(bcm_field_efes_action_add
                    (unit, 0, fg_id, Sh_dnx_field_init_context_ipmf1, &action_position, &efes_action_info));
    bcm_field_efes_action_info_t_init(&efes_action_info);
    efes_action_info.is_condition_valid = TRUE;
    efes_action_info.condition_msb = 31;
    efes_action_info.efes_condition_conf[0].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[0].action_type = bcmFieldActionUDHBase2;
    efes_action_info.efes_condition_conf[0].action_lsb = offset_on_fg;
    efes_action_info.efes_condition_conf[0].action_nof_bits = 2;
    efes_action_info.efes_condition_conf[0].action_with_valid_bit = TRUE;
    efes_action_info.efes_condition_conf[1].is_action_valid = TRUE;
    efes_action_info.efes_condition_conf[1].action_type = bcmFieldActionUDHBase3;
    efes_action_info.efes_condition_conf[1].action_lsb = offset_on_fg;
    efes_action_info.efes_condition_conf[1].action_nof_bits = 1;
    efes_action_info.efes_condition_conf[1].action_with_valid_bit = FALSE;
    action_position = BCM_FIELD_ACTION_POSITION(0, 2);
    SHR_IF_ERR_EXIT(bcm_field_efes_action_add(unit, 0, fg_id, Sh_dnx_field_init_context_ipmf1, &action_position,
                                              &efes_action_info));

    /*
     * Add 2 CONST field groups. attach one.
     */
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionPrioIntNew;
    action_info.prefix_size = 3;
    action_info.prefix_value = 0x1;
    action_info.size = 0;
    action_info.stage = bcmFieldStageEgress;
    sal_strncpy_s((char *) action_info.name, "uda_epmf_tc", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &uda_epmf_tc));
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionAceEntryId;
    action_info.prefix_size = 22;
    action_info.prefix_value = 0x2;
    action_info.size = 0;
    action_info.stage = bcmFieldStageEgress;
    sal_strncpy_s((char *) action_info.name, "uda_epmf_ace_key", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &uda_epmf_ace_key));
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.nof_quals = 0;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = uda_epmf_tc;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = uda_epmf_ace_key;
    fg_info.action_with_valid_bit[1] = FALSE;
    sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_const", sizeof(fg_info.name));
    fg_id = Sh_dnx_field_init_fg_id_const_1;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    fg_id = Sh_dnx_field_init_fg_id_const_2;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, Sh_dnx_field_init_context_epmf, &attach_info));

    /*
     * Add TCAM field group with full key size
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_tcam_single", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = bcmFieldQualifyDstIp6;
    fg_info.action_types[0] = bcmFieldActionUDHData1;
    fg_id = Sh_dnx_field_init_fg_id_tcam_single;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 7;
    attach_info.key_info.qual_info[0].offset = 5;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, Sh_dnx_field_init_context_ipmf2, &attach_info));

    /*
     * Add TCAM field group with double key size
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_tcam_double", sizeof(fg_info.name));
    fg_info.nof_quals = 2;
    fg_info.nof_actions = 2;
    fg_info.qual_types[0] = bcmFieldQualifyDstIp6;
    fg_info.qual_types[1] = bcmFieldQualifySrcIp6;
    fg_info.action_types[0] = bcmFieldActionUDHData2;
    fg_info.action_types[1] = bcmFieldActionUDHData3;
    fg_id = Sh_dnx_field_init_fg_id_tcam_double;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 7;
    attach_info.key_info.qual_info[0].offset = 5;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 7;
    attach_info.key_info.qual_info[1].offset = 5;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, Sh_dnx_field_init_context_ipmf2, &attach_info));

    if (add_entries)
    {
        /*
         * Add entries for multiple banks
         */
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.nof_entry_quals = 1;
        entry_info.nof_entry_actions = 1;
        fg_info.qual_types[0] = bcmFieldQualifyDstIp6;
        entry_info.entry_qual[0].type = bcmFieldQualifyDstIp6;
        entry_info.entry_qual[0].mask[0] = 0xFFFFFFFF;
        entry_info.entry_action[0].type = bcmFieldActionUDHData2;
        for (entry_index = 0; entry_index < 3000; entry_index++)
        {
            entry_info.entry_qual[0].value[0] = entry_index;
            entry_info.entry_action[0].value[0] = entry_index + 1;
            SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle));
        }
    }

    /*
     * Add state table field group
     */
    if (dnx_data_field.state_table.supported_get(unit))
    {
        int state_table_supports_rmw = dnx_data_field.features.state_table_atomic_rmw_get(unit);
        int addresss_size = dnx_data_field.state_table.address_size_get(unit);
        int data_size = dnx_data_field.state_table.data_size_get(unit);
        int opcode_size;
        int address_position;
        int data_position;
        bcm_field_qualify_t udq_st_address;
        bcm_field_qualify_t udq_st_data;
        bcm_field_qualify_t udq_st_opcode;
        bcm_field_action_t uda_st;

        if (state_table_supports_rmw)
        {
            opcode_size = dnx_data_field.state_table.opcode_size_rmw_get(unit);
            address_position = 1;
            data_position = 0;
        }
        else
        {
            opcode_size = dnx_data_field.state_table.wr_bit_size_rw_get(unit);
            address_position = 0;
            data_position = 1;
        }

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = addresss_size;
        sal_strncpy_s((char *) qual_info.name, "udq_st_address", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_st_address));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = data_size;
        sal_strncpy_s((char *) qual_info.name, "udq_st_data", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_st_data));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = opcode_size;
        sal_strncpy_s((char *) qual_info.name, "udq_st_opcode", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_st_opcode));
        bcm_field_action_info_t_init(&action_info);
        action_info.stage = bcmFieldStageIngressPMF1;
        action_info.action_type = bcmFieldActionVoid;
        action_info.size = 8;
        sal_strncpy_s((char *) action_info.name, "uda_st", sizeof(action_info.name));
        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &uda_st));

        bcm_field_group_info_t_init(&fg_info);
        fg_info.fg_type = bcmFieldGroupTypeStateTable;
        fg_info.stage = bcmFieldStageIngressPMF1;
        sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_state_table", sizeof(fg_info.name));
        fg_info.nof_quals = 3;
        fg_info.qual_types[address_position] = udq_st_address;
        fg_info.qual_types[data_position] = udq_st_data;
        fg_info.qual_types[2] = udq_st_opcode;
        fg_info.nof_actions = 0;
        if (state_table_supports_rmw)
        {
            fg_info.nof_actions = 1;
            fg_info.action_types[0] = uda_st;
            fg_info.action_with_valid_bit[0] = FALSE;
        }
        fg_id = Sh_dnx_field_init_fg_id_state_table;
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        for (ii = 0; ii < fg_info.nof_quals; ii++)
        {
            attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
            attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
            attach_info.key_info.qual_info[ii].offset = 0;
        }
        for (ii = 0; ii < fg_info.nof_actions; ii++)
        {
            attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
        }
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                        (unit, 0, fg_id, Sh_dnx_field_init_context_ipmf1_st, &attach_info));
    }

    /*
     * Add EXEM
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_EXEM", sizeof(fg_info.name));
    fg_info.nof_quals = 3;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = bcmFieldQualifyKeyGenVar;
    fg_info.qual_types[1] = bcmFieldQualifySrcPort;
    fg_info.qual_types[2] = bcmFieldQualifyInPort;
    fg_info.action_types[0] = bcmFieldActionOutInterface0;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_id = Sh_dnx_field_init_fg_id_exem;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    attach_info.payload_info.action_info[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 2000);
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_ipmf3_default, &attach_info));

    /*
     * Add direct extraction
     */
    {
        bcm_field_qualify_t udq_de_metadata;
        bcm_field_qualify_t udq_de_metadata_native;
        bcm_field_qualify_t udq_de_header_relative;
        bcm_field_qualify_t udq_de_header_absolute;
        bcm_field_qualify_t udq_de_lr_relative;
        bcm_field_qualify_t udq_de_lr_absolute;
        bcm_field_qualify_t udq_de_const;
        bcm_field_qualify_t udq_de_cascaded;
        bcm_field_action_t uda_de_void;
        bcm_field_action_t uda_de_w_valid;
        bcm_field_action_t uda_de_wo_valid;
        bcm_field_qualifier_info_get_t qual_info_get;
        int tc_pbus_offset;
        int hash_value_native_pbus_offset;

        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                        (unit, bcmFieldQualifyIntPriority, bcmFieldStageIngressPMF2, &qual_info_get));
        tc_pbus_offset = qual_info_get.offset;
        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                        (unit, bcmFieldQualifyHashValue, bcmFieldStageIngressPMF2, &qual_info_get));
        hash_value_native_pbus_offset = qual_info_get.offset;
        SHR_IF_ERR_EXIT(bcm_field_group_action_offset_get
                        (unit, 0, Sh_dnx_field_init_fg_id_tcam_half, bcmFieldActionAdmitProfile, &offset_on_fg));

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 4;
        sal_strncpy_s((char *) qual_info.name, "udq_de_metadata", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_de_metadata));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 4;
        sal_strncpy_s((char *) qual_info.name, "udq_de_metadata_native", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_de_metadata_native));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 4;
        sal_strncpy_s((char *) qual_info.name, "udq_de_header_relative", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_de_header_relative));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 4;
        sal_strncpy_s((char *) qual_info.name, "udq_de_header_absolute", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_de_header_absolute));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 4;
        sal_strncpy_s((char *) qual_info.name, "udq_de_lr_relative", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_de_lr_relative));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 4;
        sal_strncpy_s((char *) qual_info.name, "udq_de_lr_absolute", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_de_lr_absolute));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 4;
        sal_strncpy_s((char *) qual_info.name, "udq_de_const", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_de_const));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 4;
        sal_strncpy_s((char *) qual_info.name, "udq_de_cascaded", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udq_de_cascaded));
        bcm_field_action_info_t_init(&action_info);
        action_info.stage = bcmFieldStageIngressPMF2;
        action_info.action_type = bcmFieldActionVoid;
        action_info.size = 25;
        sal_strncpy_s((char *) action_info.name, "uda_de_void", sizeof(action_info.name));
        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &uda_de_void));
        bcm_field_action_info_t_init(&action_info);
        action_info.stage = bcmFieldStageIngressPMF2;
        action_info.action_type = bcmFieldActionTtlSet;
        action_info.size = 4;
        action_info.prefix_size = 4;
        action_info.prefix_value = 1;
        sal_strncpy_s((char *) action_info.name, "uda_de_w_valid", sizeof(action_info.name));
        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &uda_de_w_valid));
        bcm_field_action_info_t_init(&action_info);
        action_info.stage = bcmFieldStageIngressPMF2;
        action_info.action_type = bcmFieldActionUDHBase0;
        action_info.size = 2;
        sal_strncpy_s((char *) action_info.name, "uda_de_wo_valid", sizeof(action_info.name));
        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &uda_de_wo_valid));
        bcm_field_group_info_t_init(&fg_info);
        fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
        fg_info.stage = bcmFieldStageIngressPMF2;
        sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_DE", sizeof(fg_info.name));
        fg_info.nof_quals = 2;
        fg_info.nof_actions = 2;
        ii = 0;
        fg_info.qual_types[ii++] = udq_de_metadata;
        fg_info.qual_types[ii++] = udq_de_metadata_native;
        fg_info.qual_types[ii++] = udq_de_header_relative;
        fg_info.qual_types[ii++] = udq_de_header_absolute;
        fg_info.qual_types[ii++] = udq_de_lr_relative;
        fg_info.qual_types[ii++] = udq_de_lr_absolute;
        fg_info.qual_types[ii++] = udq_de_const;
        fg_info.qual_types[ii++] = udq_de_cascaded;
        fg_info.nof_quals = ii;
        ii = 0;
        fg_info.action_with_valid_bit[ii] = FALSE;
        fg_info.action_types[ii++] = uda_de_void;
        fg_info.action_with_valid_bit[ii] = TRUE;
        fg_info.action_types[ii++] = uda_de_w_valid;
        fg_info.action_with_valid_bit[ii] = FALSE;
        fg_info.action_types[ii++] = uda_de_wo_valid;
        fg_info.nof_actions = ii;
        fg_id = Sh_dnx_field_init_fg_id_direct_extraction;
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
        /*
         * Add FEM to 24 bit FEM
         */
        bcm_field_fem_action_info_t_init(&fem_action_info);
        fem_action_info.fem_input.input_offset = 0;
        fem_action_info.condition_msb = 3;
        for (fem_conditions_index = 0; fem_conditions_index < 2; fem_conditions_index++)
        {
            fem_action_info.fem_condition[fem_conditions_index].extraction_id = 0;
            fem_action_info.fem_condition[fem_conditions_index].is_action_valid = 1;
        }
        fem_action_info.fem_extraction[0].action_type = bcmFieldActionOutVport3Raw;
        fem_action_info.fem_extraction[0].increment = 6;
        for (fem_mapping_bits_index = 0; fem_mapping_bits_index < 8; fem_mapping_bits_index++)
        {
            fem_action_info.fem_extraction[0].output_bit[fem_mapping_bits_index].source_type =
                bcmFieldFemExtractionOutputSourceTypeForce;
            fem_action_info.fem_extraction[0].output_bit[fem_mapping_bits_index].offset = 1;
        }
        action_position = BCM_FIELD_ACTION_POSITION(3, 5);
        SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, fg_id, action_position, &fem_action_info));
        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        ii = 0;
        attach_info.key_info.qual_types[ii] = udq_de_metadata;
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[ii].offset = tc_pbus_offset + 1;
        ii++;
        attach_info.key_info.qual_types[ii] = udq_de_metadata_native;
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[ii].offset = hash_value_native_pbus_offset + 2;
        ii++;
        attach_info.key_info.qual_types[ii] = udq_de_header_relative;
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeLayerFwd;
        attach_info.key_info.qual_info[ii].input_arg = 5;
        attach_info.key_info.qual_info[ii].offset = 0;
        ii++;
        attach_info.key_info.qual_types[ii] = udq_de_header_absolute;
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[ii].input_arg = 4;
        attach_info.key_info.qual_info[ii].offset = 2;
        ii++;
        attach_info.key_info.qual_types[ii] = udq_de_lr_relative;
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeLayerRecordsFwd;
        attach_info.key_info.qual_info[ii].input_arg = 2;
        attach_info.key_info.qual_info[ii].offset = 1;
        ii++;
        attach_info.key_info.qual_types[ii] = udq_de_lr_absolute;
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
        attach_info.key_info.qual_info[ii].input_arg = 0;
        attach_info.key_info.qual_info[ii].offset = 8;
        ii++;
        attach_info.key_info.qual_types[ii] = udq_de_const;
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeConst;
        attach_info.key_info.qual_info[ii].input_arg = 7;
        ii++;
        attach_info.key_info.qual_types[ii] = udq_de_cascaded;
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeCascaded;
        attach_info.key_info.qual_info[ii].input_arg = Sh_dnx_field_init_fg_id_tcam_half;
        attach_info.key_info.qual_info[ii].offset = offset_on_fg + 1;
        ii++;
        for (ii = 0; ii < fg_info.nof_actions; ii++)
        {
            attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
        }
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, Sh_dnx_field_init_context_ipmf2, &attach_info));
    }

    /*
     * Add TCAM direct field group
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_tcam_direct", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = bcmFieldQualifyGeneratedTtl;
    fg_info.action_types[0] = bcmFieldActionOutInterface1;
    fg_id = Sh_dnx_field_init_fg_id_tcam_direct;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, Sh_dnx_field_init_context_ipmf1, &attach_info));

    /*
     * Add MDB direct field group
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectMdb;
    fg_info.stage = bcmFieldStageIngressPMF1;
    sal_strncpy_s((char *) fg_info.name, "field_group_info_diag_MDB_direct", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = bcmFieldQualifyGeneratedTtl;
    fg_info.action_types[0] = bcmFieldActionOutInterface0;
    fg_id = Sh_dnx_field_init_fg_id_mdb_direct;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &fg_id));
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
        attach_info.key_info.qual_info[ii].input_type = bcmFieldInputTypeMetaData;
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, Sh_dnx_field_init_context_ipmf1, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * }
 */
