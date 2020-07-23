
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include "diag_dnx_field_key.h"
#include "diag_dnx_field_group.h"
#include "diag_dnx_field_context.h"
#include "diag_dnx_field_qualifier.h"
#include "diag_dnx_field_port.h"
#include "diag_dnx_field_action.h"
#include "diag_dnx_field_attach.h"
#include "diag_dnx_field_entry.h"
#include "diag_dnx_field_tcam.h"
#include "diag_dnx_field_system.h"
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

static sh_sand_man_t Sh_dnx_field_system_base_man = {
    .brief =
        "Info - Presenting operations related to system view of the entire lookup process on a certain FP stage.\r\n"
        "Header - Presenting operations related to system header profiles",
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

static sh_sand_man_t Sh_dnx_field_port_base_man = {
    .brief = "Presenting operations related to Field Ports",
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

/* *INDENT-OFF* */
sh_sand_cmd_t Sh_dnx_field_cmds[] = {
    {"key",           NULL,                    Sh_dnx_field_key_cmds,           NULL, &Sh_dnx_field_key_man},
    {"fem",           NULL,                    Sh_dnx_field_fem_base_cmds,      NULL, &Sh_dnx_field_fem_base_man},
    {"group",         NULL,                    Sh_dnx_field_group_cmds,         NULL, &Sh_dnx_field_group_base_man},
    {"context",       NULL,                    Sh_dnx_field_context_cmds,       NULL, &Sh_dnx_field_context_base_man},
    {"attach",        NULL,                    Sh_dnx_field_attach_cmds,        NULL, &Sh_dnx_field_attach_base_man},
    {"qualifier",     NULL,                    Sh_dnx_field_qualifier_cmds,     NULL, &Sh_dnx_field_qualifier_base_man},
    {"action",        NULL,                    Sh_dnx_field_action_cmds,        NULL, &Sh_dnx_field_action_base_man},
    {"port",          NULL,                    Sh_dnx_field_port_cmds,          NULL, &Sh_dnx_field_port_base_man},
    {"entry",         NULL,                    Sh_dnx_field_entry_cmds,         NULL, &Sh_dnx_field_entry_base_man},
    {"tcam",          NULL,                    Sh_dnx_field_tcam_cmds,          NULL, &Sh_dnx_field_tcam_base_man},
    {"system",        NULL,                    Sh_dnx_field_system_cmds,        NULL, &Sh_dnx_field_system_base_man},
    {"ace",           NULL,                    Sh_dnx_field_ace_cmds,           NULL, &Sh_dnx_field_ace_base_man},
    {"efes",          NULL,                    Sh_dnx_field_efes_cmds,          NULL, &Sh_dnx_field_efes_base_man},
    {"last",          NULL,                    Sh_dnx_field_last_cmds,          NULL, &Sh_dnx_field_last_base_man},
    {"range",         NULL,                    Sh_dnx_field_range_cmds,         NULL, &Sh_dnx_field_range_base_man},
    {"virtualwire",   NULL,                    Sh_dnx_field_virtualwire_cmds,   NULL, &Sh_dnx_field_virtualwire_base_man},
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
 * Enum for option requiring all 'port types'
 */
sh_sand_enum_t Field_port_type_enum_table[] = {
    {"LIF", 0, "LIF port Id"},
    {"PP", 1, "PP-port Id"},
    {"TM", 2, "TM-port Id"},
    {"PORT_TYPE_NOF", 3, "Number of port types"},
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
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'key' diagnostic.
 */
sh_sand_enum_t Field_key_diag_stage_enum_table[] = {
    {"IPMF1", 0, "ingress PMF1 stage"},
    {"IPMF2", 1, "ingress PMF2 stage"},
    {"IPMF3", 2, "ingress PMF3 stage"},
    {"EPMF", 3, "Egress PMF stage"},
    {"STAGE_LOWEST", 0, "Minimal value for field stage"},
    {"STAGE_HIGHEST", 3, "Maximal value for field stage"},
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
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'port' diagnostic.
 */
sh_sand_enum_t Field_port_diag_stage_enum_table[] = {
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
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'system' diagnostic.
 */
sh_sand_enum_t Field_system_diag_stage_enum_table[] = {
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
    {"KBP", DNX_FIELD_GROUP_TYPE_KBP, "KBP Field Group type. ACL group in the FLP block"},
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
/**
 * Enum for option requiring 'offset type' of test (PBUS or KEY_TEMPLATE)
 */
sh_sand_enum_t Field_offset_enum_table[] = {
    {"P_BUS", 0},
    {"KEY_TEMPLATE", 1},
    {NULL}
};
/**
 * Enum for option requiring 'direct extraction type' of test (SINGLE, MULTIPLE or FOR_CINT)
 * 'MULTIPLE' stands for 'multiple direct extractions' (i.e., dynamic allocation
 * of bit-ranges on the same key for a few field groups).
 * FOR_CINT is setup for AT_Dnx_Cint_field_dir_ext()
 *
 * The various values used for selecting direct extraction test type:
 * SINGLE, MULTIPLE or FOR_CINT.
 * SINGLE stands for setting a single 'bit-range' on key (using, for DNX type,
 * dnx_field_group_add(), dnx_field_group_context_attach())
 * MULTIPLE stands for setting a multiple 'bit-range's on key(s) for various
 * contexts (using, for DNX type, dnx_field_group_add() and
 * dnx_field_group_context_attach())
 * FOR_CINT is the same as SINGLE but setting a qualifier and an action for
 * the test on
 */
sh_sand_enum_t Field_de_type_enum_table[] = {
    {"MULTIPLE", 0},
    {"SINGLE", 1},
    {"FOR_CINT", 2},
    {NULL}
};

sh_sand_enum_t Field_key_enum_table[] = {
    {"A", DBAL_ENUM_FVAL_FIELD_KEY_A, "Key-A (used for TCAM)"},
    {"B", DBAL_ENUM_FVAL_FIELD_KEY_B, "Key-B (used for TCAM)"},
    {"C", DBAL_ENUM_FVAL_FIELD_KEY_C, "Key-C (used for TCAM)"},
    {"D", DBAL_ENUM_FVAL_FIELD_KEY_D, "Key-D (used for TCAM)"},
    {"E", DBAL_ENUM_FVAL_FIELD_KEY_E, "Key-E (used for EXEM)"},
    {"F", DBAL_ENUM_FVAL_FIELD_KEY_F, "Key-F (used for TCAM)"},
    {"G", DBAL_ENUM_FVAL_FIELD_KEY_G, "Key-G (used for TCAM)"},
    {"H", DBAL_ENUM_FVAL_FIELD_KEY_H, "Key-H (used for TCAM)"},
    {"I", DBAL_ENUM_FVAL_FIELD_KEY_I, "Key-I (used for TCAM/DE)"},
    {"J", DBAL_ENUM_FVAL_FIELD_KEY_J, "Key-J (used for DE)"},
    {"ALL", DNX_FIELD_KEY_ALL, "All keys"},
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
 * Enum for option requiring all 'key diag display types'
 */
sh_sand_enum_t Field_key_display_type_enum_table[] = {
    {"BY_HALF_KEY", 0, "Display of allocated bit-ranges"},
    {"BY_BIT_RANGE", 1, "Display for a range of keys"},
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
appl_dnx_action_swstate_fem_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    char *test_text,
    sh_sand_control_t * sand_control)
{
    dnx_field_fem_program_t fem_program_index, fem_program_max;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * If current severity is higher than 'info' then display nothing.
     */
    {
        PRT_TITLE_SET("%s - SWSTATE for FEM id %d", test_text, fem_id);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "fg_id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "input offset");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "second fg_id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ignore actions");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "map index");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "encoded actions");

        fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            /*
             * Skip 4 columns to get to the "map index" column
             */
            SHR_IF_ERR_EXIT(appl_dnx_action_swstate_fem_prg_display
                            (unit, fem_id, fem_program_index, 4, sand_control, prt_ctr));
        }
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}
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
    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.get(unit, fem_id, fem_program, &(fg_id_info.second_fg_id)));
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
    if (fg_id_info.second_fg_id == DNX_FIELD_GROUP_INVALID)
    {
        PRT_CELL_SET("%s", "---");
    }
    else
    {
        PRT_CELL_SET("%d", fg_id_info.second_fg_id);
    }
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
                fem_encoded_action =
                    DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                               DNX_FIELD_STAGE_IPMF2, dnx_field_fem_action_entry_p->fem_action);

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

/*
 * }
 */

/*
 * }
 */
