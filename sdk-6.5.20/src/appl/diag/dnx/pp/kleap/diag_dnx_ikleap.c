/** \file diag_dnx_ikleap.c
 *
 * Main diagnostics for dbal applications All CLI commands, that
 * are related to KLEAP, are gathered in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGKLEAPDNX

/*************
 * INCLUDES  *
 */
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include "../../../../../bcm/dnx/kbp/kbp_mngr_internal.h"
#include "diag_dnx_ikleap.h"
#include "../diag_dnx_pp.h"
#include <sal/core/boot.h>
/** allow drv.h include explicitly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc_block_any*/

extern stage_t ikleap_stages[KLEAP_NOF_STAGES];
extern fixed_key_lookup_t fixed_key_lookup[NOF_FIXED_KEY_LOOKUP];
extern shr_error_e dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id);

/*
 * \brief Mapping of Key[0..3] signals containers of up to 160bits to corresponding KBR index, in KBP case, in the FWD12 stage
 */
kbr_value_e key_sig_fwd12_2kbp_kbr[4] = {
    KBR_4,
    KBR_5,
    KBR_16,
    KBR_17
};

/*************
 * DEFINES   *
 */


#define STAGE_INFO ikleap_stages[stage_index]

#define DIAG_DNX_IKLEAP_DUMMY_DBAL_TABLE_ALLOC(__local_handle_id__) \
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &__local_handle_id__))

#define DIAG_IKLEAP_SKIP_STAGE                                                     \
    (all_stages_flag && (stage_index > KLEAP_STAGE_FWD12)) /* skip PMF */ ||       \
    (!all_stages_flag && ((matched_stage_idx != stage_index) || (matched_sub_stage_idx != sub_stage)))

#define DIAG_IKLEAP_SKIP_KBR_CHECK                                                                  \
{                                                                                                   \
    uint32 kbr_stage = 0;                                                                           \
    if (STAGE_INFO.stage_type != SINGLE_STAGE_TYPE)                                                 \
    {                                                                                               \
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get                                         \
                        (unit, rsrcs_map_handle_id, DBAL_FIELD_KBR_SELECTOR, kbr_idx, &kbr_stage)); \
    }                                                                                               \
    if (kbr_stage != sub_stage)                                                                     \
    {                                                                                               \
        continue;                                                                                   \
    }                                                                                               \
    if ((phy_db_id != DBAL_PHYSICAL_TABLE_NONE) && (kbr2phy_mapping[kbr_idx] != phy_db_id))         \
    {                                                                                               \
        continue;                                                                                   \
    }                                                                                               \
}

#define DIAG_IKLEAP_SKIP_KBR_CHECK_2                                        \
{                                                                           \
    uint8 kbr_skip = FALSE, fixed_key_i = 0;                                \
    if (!utilex_bitstream_have_one_in_range(ffc_bitmap, 0, nof_ffc - 1))    \
    {                                                                       \
        has_ffc = FALSE;                                                    \
        kbr_skip = TRUE;                                                    \
    }                                                                       \
    for(fixed_key_i = 0; fixed_key_i < NOF_FIXED_KEY_LOOKUP; fixed_key_i++) \
    {                                                                       \
        int kbr_i;                                                          \
        for(kbr_i = 0; kbr_i < fixed_key_lookup[fixed_key_i].nof_interfaces; kbr_i++) \
        {                                                                   \
            if ((stage_index == fixed_key_lookup[fixed_key_i].stage) &&     \
                (sub_stage == fixed_key_lookup[fixed_key_i].sub_stage) &&   \
                (kbr_idx == fixed_key_lookup[fixed_key_i].kbr_idx[kbr_i]))  \
            {                                                               \
                fixed_lookup_key_indication = TRUE;                         \
                fixed_lookup_key_index = fixed_key_i;                       \
                kbr_skip = FALSE;                                           \
            }                                                               \
        }                                                                   \
    }                                                                       \
    if(kbr_skip)                                                            \
    {                                                                       \
        continue;                                                           \
    }                                                                       \
}

#define DIAG_IKLEAP_SKIP_KBR_CHECK_3                                        \
{                                                                           \
    uint8 kbr_skip = FALSE, fixed_key_i = 0;                                \
    if (!utilex_bitstream_have_one_in_range(ffc_bitmap, 0, nof_ffc - 1))    \
    {                                                                       \
        kbr_skip = TRUE;                                                    \
    }                                                                       \
    for(fixed_key_i = 0; fixed_key_i < NOF_FIXED_KEY_LOOKUP; fixed_key_i++) \
    {                                                                       \
        int kbr_i;                                                          \
        for(kbr_i = 0; kbr_i < fixed_key_lookup[fixed_key_i].nof_interfaces; kbr_i++) \
        {                                                                   \
            if ((stage_index == fixed_key_lookup[fixed_key_i].stage) &&     \
                (sub_stage == fixed_key_lookup[fixed_key_i].sub_stage) &&   \
                (kbr_idx == fixed_key_lookup[fixed_key_i].kbr_idx[kbr_i]))  \
            {                                                               \
                fixed_lookup_key_indication = TRUE;                         \
                fixed_lookup_key_index = fixed_key_i;                       \
                kbr_skip = FALSE;                                           \
            }                                                               \
        }                                                                   \
    }                                                                       \
    if(kbr_skip)                                                            \
    {                                                                       \
        continue;                                                           \
    }                                                                       \
}

#define DIAG_IKLEAP_CHECK_APP_DB_ID(____kbr_has_app_db_id____)                              \
{                                                                                           \
    dbal_table_field_info_t field_info;                                                     \
    rv = dbal_tables_field_info_get_no_err(unit, STAGE_INFO.dbal_table_kbr_info,            \
                                           DBAL_FIELD_APP_DB_ID, FALSE, 0, 0, &field_info); \
    if (rv == _SHR_E_NONE)                                                                  \
    {                                                                                       \
        ____kbr_has_app_db_id____ = TRUE;                                                   \
    }                                                                                       \
    else if (rv == _SHR_E_NOT_FOUND)                                                        \
    {                                                                                       \
        ____kbr_has_app_db_id____ = FALSE;                                                  \
    }                                                                                       \
    else                                                                                    \
    {                                                                                       \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected Error from DBAL, searching for APP_DB_ID in table\n"); \
    }                                                                                       \
    if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)                                \
    {                                                                                       \
        ____kbr_has_app_db_id____ = FALSE;                                                  \
    }                                                                                       \
}

#define DIAG_IKLEAP_PRINT_LOGICAL_TABLE_TO_PRT                                  \
{                                                                               \
    if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_NONE)                   \
    {                                                                           \
        PRT_CELL_SET("%s", "Direct Extraction(-)");                             \
    }                                                                           \
    else if (diag_dnx_kleap_stage_is_pmf(unit, stage_index))                    \
    {                                                                           \
        if (kbr_has_app_db_id)                                                  \
        {                                                                       \
            PRT_CELL_SET("%s%d", "PMF,APP_DB_ID = ", app_db_id);                \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            PRT_CELL_SET("%s", "PMF,No APP_DB_ID");                             \
        }                                                                       \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        /**  Here if it's non PMF stage */                                      \
        if (kbr_has_app_db_id)                                                  \
        {                                                                       \
            /** this case in this case we provide the logical table using app_db_id */ \
            SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get                    \
                            (unit, kbr2phy_mapping[kbr_idx], app_db_id, &logical_table_for_kbr));\
            PRT_CELL_SET("%s(%s,%d)", dbal_logical_table_to_string(unit, logical_table_for_kbr),\
                         phy_db_name, app_db_id);                               \
        }                                                                       \
        else if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)           \
        {                                                                       \
            uint8 opcode_id;                                                    \
            char  opcode_name[DBAL_MAX_STRING_LENGTH];                          \
            SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_get(unit, context_hw_val, acl_context, &opcode_id, opcode_name)); \
            PRT_CELL_SET("KBP,\n opcode %s (%d)", opcode_name, opcode_id);        \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            SHR_ERR_EXIT(_SHR_E_CONFIG,                                         \
                         "not a PMF stage & non KBP KBR & app_db_id is non present - non legal state\n"\
                         "sub-stage: %s, context: %d, kbr: %d\n", stage_name, context_enum_val, kbr_idx);\
        }                                                                       \
    }                                                                           \
}

#define DIAG_IKLEAP_VIS_PER_STAGE_GET_HW_CONTEXT \
{                                                                           \
    if (stage_index == KLEAP_STAGE_VT1)\
    {\
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_VT1_CONTEXT_ID, context_enum_val, &context_hw_val));\
    }\
    else if (stage_index == KLEAP_STAGE_VT23)\
    {\
        if (sub_stage == 0)\
        {\
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_VT2_CONTEXT_ID, context_enum_val, &context_hw_val));\
        }\
        else\
        {\
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_VT3_CONTEXT_ID, context_enum_val, &context_hw_val));\
        }\
    }\
    else if (stage_index == KLEAP_STAGE_VT45)\
    {\
        if (sub_stage == 0)\
        {\
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_VT4_CONTEXT_ID, context_enum_val, &context_hw_val));\
        }\
        else\
        {\
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_VT5_CONTEXT_ID, context_enum_val, &context_hw_val));\
        }\
    }\
    else if (stage_index == KLEAP_STAGE_FWD12)\
    {\
        if (sub_stage == 0)\
        {\
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_FWD1_CONTEXT_ID, context_enum_val, &context_hw_val));\
        }\
        else\
        {\
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_FWD2_CONTEXT_ID, context_enum_val, &context_hw_val));\
        }\
    }\
}

#define DIAG_IKLEAP_VIS_SET_CONTEXT_AND_NASID                                   \
{                                                                               \
    uint8 to_skip = FALSE;                                                      \
    uint32 current_stage_index;                                                 \
    current_stage_index = STAGE_INFO.stage_index_in_pipe[sub_stage];            \
    context_enum_val = 0;                                                       \
    context_hw_val = 0;                                                         \
    nasid = 0;                                                                  \
    if (stage_index != KLEAP_STAGE_VT1)                                         \
    {                                                                           \
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "", stage_name, "Nasid", &nasid, 1));\
    }                                                                           \
    if ((current_stage_index != 0) && (nasid > current_stage_index))            \
    {                                                                           \
        LOG_CLI((BSL_META("NASID is set to %d, stage %s(%d) is skipped\n"), nasid, stage_name, current_stage_index));\
        to_skip = TRUE;                                                         \
    }                                                                           \
    if(!to_skip)                                                                \
    {                                                                           \
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "", stage_name, "Context", &context_hw_val, 1));\
        if (stage_index == KLEAP_STAGE_VT1)\
        {\
            SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_VT1_CONTEXT_ID, context_hw_val, &context_enum_val));\
        }\
        else if (stage_index == KLEAP_STAGE_VT23)\
        {\
            if (sub_stage == 0)\
            {\
                SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_VT2_CONTEXT_ID, context_hw_val, &context_enum_val));\
            }\
            else\
            {\
                SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_VT3_CONTEXT_ID, context_hw_val, &context_enum_val));\
            }\
        }\
        else if (stage_index == KLEAP_STAGE_VT45)\
        {\
            if (sub_stage == 0)\
            {\
                SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_VT4_CONTEXT_ID, context_hw_val, &context_enum_val));\
            }\
            else\
            {\
                SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_VT5_CONTEXT_ID, context_hw_val, &context_enum_val));\
            }\
        }\
        else if (stage_index == KLEAP_STAGE_FWD12)\
        {\
            if (sub_stage == 0)\
            {\
                SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_FWD1_CONTEXT_ID, context_hw_val, &context_enum_val));\
            }\
            else\
            {\
                SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_FWD2_CONTEXT_ID, context_hw_val, &context_enum_val));\
            }\
        }\
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FWD2", "", "ACL_Context", &acl_context, 1));\
    }                                                                           \
    else                                                                        \
    {                                                                           \
        continue;                                                               \
    }                                                                           \
}

#define DIAG_IKELAP_COPY_STAGE_INFO_TO_LOCAL_PARAMS                             \
do                                                                              \
{                                                                               \
    context_profile_field = STAGE_INFO.dbal_context_profile_field_per_stage;    \
    kbr_idx_field = STAGE_INFO.dbal_kbr_idx_field_per_stage;                    \
    kbr2phy_mapping = STAGE_INFO.kbr2physical;                                  \
    nof_kbr = STAGE_INFO.nof_kbrs;                                              \
    nof_ffc = STAGE_INFO.nof_ffc;                                               \
}while (0)

/*************
 * TYPEDEFS  *
 */
static char key_signal_str[DNX_KBP_MAX_NOF_LOOKUPS + 1][DIAG_DNX_KLEAP_MAX_STRING_LENGTH];
static char res_signal_str[DNX_KBP_MAX_NOF_LOOKUPS + 1][DIAG_DNX_KLEAP_MAX_STRING_LENGTH];


#define IKEAP_KAPS_STRENGTH_WIDTH   3
/*************
 * FUNCTIONS *
 */
/*
 * \brief - check if stage is a PMF stage or not. return TRUE if PMF stage, otherwise FALSE
 */
static uint8
diag_dnx_kleap_stage_is_pmf(
    int unit,
    kleap_stages_e stage_name)
{
    if ((stage_name == KLEAP_STAGE_IPMF1) || (stage_name == KLEAP_STAGE_IPMF2)
        || (stage_name == KLEAP_STAGE_IPMF3)
        || (stage_name == KLEAP_STAGE_EPMF) || (stage_name == KLEAP_STAGE_IPMF1_INITIAL))
    {
        return TRUE;
    }
    return FALSE;
}


/*
 * \brief - update stage parameters. decide whether to run on all stages or specific stage
 */
static shr_error_e
diag_kleap_decide_stage(
    int unit,
    char *stage_name,
    int *matched_stage_idx,
    int *matched_sub_stage_idx,
    int *all_stages_flag)
{
    int stage_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!sal_strncmp(stage_name, "", DBAL_MAX_STRING_LENGTH))
    {
        *all_stages_flag = 1;
        SHR_EXIT();
    }

    for (stage_index = 0; stage_index < KLEAP_NOF_STAGES; stage_index++)
    {
        int sub_stage;
        for (sub_stage = 0; sub_stage <= STAGE_INFO.stage_type; sub_stage++)
        {
            if (!sal_strncasecmp(STAGE_INFO.kleap_sub_stages_names[sub_stage], stage_name, DBAL_MAX_STRING_LENGTH))
            {
                *matched_stage_idx = stage_index;
                *matched_sub_stage_idx = sub_stage;
                SHR_EXIT();
            }
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal Stage %s\n", stage_name);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - update phydb id parameters. decide whether to run on all physical DBs or specific DB
 */
static shr_error_e
diag_kleap_decide_phy_db_id(
    int unit,
    char *phy_db_name,
    dbal_physical_tables_e * phy_db_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sal_strncmp(phy_db_name, "", DBAL_MAX_STRING_LENGTH))
    {
        *phy_db_id = DBAL_PHYSICAL_TABLE_NONE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_physical_table_string_to_id(unit, phy_db_name, phy_db_id));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
diag_dnx_ikleap_find_last_local_lif(
    int unit,
    kleap_stages_e stage_index,
    int sub_stage,
    int core_id,
    int context,
    uint8 first_kbr_printed,
    uint32 *local_in_lif,
    uint32 *lif_dbal_result_type,
    dbal_tables_e * lif_dbal_table,
    int *is_default_lif)
{
    dbal_tables_e pd_info_table;
    uint32 pd_info_handle;
    uint32 tt_pd_idx;
    uint32 kbr_idx, nof_kbrs;
    uint32 order = 0, const_strength = 0;
    int max_order_value = 0;
    uint8 default_supported;
    char *stage_name;
    int ii, lif_strength_idx;
    uint8 local_lif_found = FALSE;
    dbal_physical_tables_e dbal_phy_db;
    signal_output_t *signal_output_result = NULL;
    signal_output_t *signal_output_port = NULL;

    uint32 chosen_in_lif;
    /**max number of inlifs, assuming their size is 60b*/
    int max_inlif_capacity_60b;
    /** the actual capacity of out IN_LIFs, assuming their actual size*/
    int max_inlif_capacity;
    dbal_enum_value_field_mdb_direct_payload_e inlif_table_payload_type;
    uint32 inlif_table_entry_size;

/* *INDENT-OFF* */
    #define LIF_ELEMENT 0
    #define KBR_ELEMENT 1
    #define LIF_HITBIT_ELEMENT 2
    #define NOF_ELEMENTS 3
    #define MAX_SUPPORTED_STRENGTH 100
    uint32 *lif_arr_by_strength = NULL;
/* *INDENT-ON* */
    char *res_sig_name = NULL;
    char *hit_sig_name = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(local_in_lif, _SHR_E_PARAM, "local_in_lif");
    SHR_NULL_CHECK(lif_dbal_table, _SHR_E_PARAM, "lif_dbal_table");

    SHR_ALLOC_SET_ZERO(lif_arr_by_strength, MAX_SUPPORTED_STRENGTH * NOF_ELEMENTS * sizeof(uint32),
                       "entry_print_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(res_sig_name, SIGNALS_MAX_NAME_LENGTH + 1, "res_sig_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(hit_sig_name, SIGNALS_MAX_NAME_LENGTH + 1, "hit_sig_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if ((stage_index != KLEAP_STAGE_VT1) && (stage_index != KLEAP_STAGE_VT23) && (stage_index != KLEAP_STAGE_VT45))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "last local lif is valid for VT stages only. given ikleap stage %d is not supported",
                     stage_index);
    }
    else if (stage_index == KLEAP_STAGE_VT1)
    {
        tt_pd_idx = 0;
        default_supported = TRUE;
        dbal_phy_db = DBAL_PHYSICAL_TABLE_INLIF_1;
    }
    else
    {
        default_supported = FALSE;
        dbal_phy_db = DBAL_PHYSICAL_TABLE_INLIF_2;
        if (sub_stage == 0)
        {
            tt_pd_idx = 0;
        }
        else if (sub_stage == 1)
        {
            tt_pd_idx = 2;
            /*
             * FIXME ARCHAD-135 support VTT5 inner eth default lif
             * if(stage_index == KLEAP_STAGE_VT45)
             * {
             *     default_supported = TRUE;
             * }
 */
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "sub_stage %d is invalid for ikleap stage %d", sub_stage, stage_index);
        }
    }

    DNX_PP_BLOCK_IS_READY_EXIT(core_id, DNX_PP_BLOCK_IRPP, TRUE);

    pd_info_table = STAGE_INFO.dbal_table_pd_info;
    nof_kbrs = STAGE_INFO.nof_kbrs;

    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, pd_info_table, DBAL_FIELD_PD_ORDER, FALSE, 0, 0,
                                                    &max_order_value));

    stage_name = STAGE_INFO.kleap_sub_stages_names[sub_stage];

    /*
     * First, find the KBR which performed an TT lookup with valid results, and sort by strength
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, pd_info_table, &pd_info_handle));
    dbal_entry_key_field32_set(unit, pd_info_handle, DBAL_FIELD_CONTEXT_PROFILE, context);
    dbal_entry_key_field32_set(unit, pd_info_handle, DBAL_FIELD_PD_IDX, tt_pd_idx);
    if (first_kbr_printed)
    {
        for (kbr_idx = 2; kbr_idx < nof_kbrs; kbr_idx++)
        {
            uint32 lif_strength = 0, local_lif_value = 0;
            char *phy_db_name;
            char *index_as_str[MAX_NUM_INTERFACES_PER_PHY_DB] = { "0", "1", "2", "3" };

            uint32 *hit_sig_value = NULL;

            sal_memset(res_sig_name, 0, SIGNALS_MAX_NAME_LENGTH + 1);
            sal_memset(hit_sig_name, 0, SIGNALS_MAX_NAME_LENGTH + 1);

            dbal_entry_key_field32_set(unit, pd_info_handle, DBAL_FIELD_KBR_IDX, kbr_idx);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, pd_info_handle, DBAL_GET_ALL_FIELDS));

            /*
             * Consider adding strength width to the calculation
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, pd_info_handle,
                                                                DBAL_FIELD_PD_CONST_STRENGTH,
                                                                INST_SINGLE, &const_strength));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, pd_info_handle, DBAL_FIELD_PD_ORDER, INST_SINGLE, &order));

            /** order != 0 means KBR lookup is valid and the result is of  IN_LIF type - cause Result goes to VTT Priority Decoder */
            if (order == 0)
            {
                continue;
            }

            lif_strength = max_order_value * const_strength + order;
            if (lif_strength >= MAX_SUPPORTED_STRENGTH)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "lif strength %d is larger than supported. should be updated",
                             lif_strength);
            }

            if (lif_arr_by_strength[lif_strength * NOF_ELEMENTS + LIF_ELEMENT] != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Two KBRs (%d and %d) with the same strength. cannot resolve priority",
                             kbr_idx, lif_arr_by_strength[lif_strength * NOF_ELEMENTS + KBR_ELEMENT]);
            }
            phy_db_name = dbal_physical_table_to_string(unit, STAGE_INFO.kbr2physical[kbr_idx]);

            sal_strncpy_s(res_sig_name, "Result", SIGNALS_MAX_NAME_LENGTH);
            sal_strncpy_s(hit_sig_name, "Hit", SIGNALS_MAX_NAME_LENGTH);
            if (STAGE_INFO.kbr_if_id[kbr_idx] >= IF_0)
            {
                sal_strncat(res_sig_name, "_", 2);
                sal_strncat(res_sig_name, index_as_str[STAGE_INFO.kbr_if_id[kbr_idx]], 2);

                sal_strncat(hit_sig_name, "_", 2);
                sal_strncat(hit_sig_name, index_as_str[STAGE_INFO.kbr_if_id[kbr_idx]], 2);
            }
            /*
             * Free signal_output - if yet or already NULL - nothing happens
             */
            SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id,
                                                    SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name, res_sig_name,
                                                    &signal_output_result));
            local_lif_value = signal_output_result->value[0] >> (signal_output_result->size - 20);

            /*
             * Get the HIT sent signal
             */
            SHR_IF_ERR_EXIT(sand_signal_output_find
                            (unit, core_id, SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name, hit_sig_name,
                             &signal_output_result));
            hit_sig_value = signal_output_result->value;

            lif_arr_by_strength[lif_strength * NOF_ELEMENTS + KBR_ELEMENT] = kbr_idx;
            lif_arr_by_strength[lif_strength * NOF_ELEMENTS + LIF_ELEMENT] = local_lif_value;
            lif_arr_by_strength[lif_strength * NOF_ELEMENTS + LIF_HITBIT_ELEMENT] = hit_sig_value[0];
        }
    }
    DBAL_HANDLE_FREE(unit, pd_info_handle);

    if (default_supported)
    {
        uint32 in_pp_port;
        uint32 ing_pp_port_handle;
        uint32 default_lif_value;

        /*
         * VTT1
         * IppfPhysicalCfg::VttInPpPortConfig0 vtt_in_pp_port_cfg(IppfPhysicalCfg::get_instance()->read_vtt_in_pp_port_config0(packet.get_in_port()));
         * packet.set_default_port_lif(vtt_in_pp_port_cfg.default_port_lif);
         */
        SHR_IF_ERR_EXIT(sand_signal_output_find
                        (unit, core_id, SIGNALS_MATCH_ASIC, "", "vtt5", "", "In_Port", &signal_output_port));
        in_pp_port = signal_output_port->value[0];
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &ing_pp_port_handle));
        dbal_entry_key_field32_set(unit, ing_pp_port_handle, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, ing_pp_port_handle, DBAL_FIELD_PP_PORT, in_pp_port);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, ing_pp_port_handle, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, ing_pp_port_handle,
                                                            DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, &default_lif_value));
        DBAL_HANDLE_FREE(unit, ing_pp_port_handle);
        lif_arr_by_strength[0 * NOF_ELEMENTS + LIF_ELEMENT] = default_lif_value;
        /** setting in this case the hitbit to 1 for a default LIF case, for devices that support hitbit */
        lif_arr_by_strength[0 * NOF_ELEMENTS + LIF_HITBIT_ELEMENT] = 1;

        
    }

    /*
     * count the maximum number that an IN_LIF can get
     */
    SHR_IF_ERR_EXIT(mdb_get_capacity(unit, dbal_phy_db, &max_inlif_capacity_60b));

    SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_phy_db, &inlif_table_payload_type));
    inlif_table_entry_size = (MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(inlif_table_payload_type));
    max_inlif_capacity = max_inlif_capacity_60b * 60 / inlif_table_entry_size;

    /** go over all found lifs and choose the one with the highest priority */
    for (ii = MAX_SUPPORTED_STRENGTH - 1; ii >= 0; ii--)
    {
        uint8 lif_is_valid = 0;

        chosen_in_lif = lif_arr_by_strength[ii * NOF_ELEMENTS + LIF_ELEMENT];

        /** check if in this device the hit_bit indication is working in substage0  - then use the lif hit-bit instead*/
        if ((sub_stage == 1)
            || (!dnx_data_debug.feature.feature_get(unit, dnx_data_debug_feature_no_hit_bit_on_mdb_access)))
        {
            lif_is_valid = lif_arr_by_strength[ii * NOF_ELEMENTS + LIF_HITBIT_ELEMENT];
        }
        else
        {
            /*
             * in case don't support hit_bit - search if IN_LIF allocated -treat only IN_LIFs with 0 to max allowed
             * IN_LIF value in res_mangr
             */
            if ((chosen_in_lif != 0) && (chosen_in_lif < max_inlif_capacity))
            {
                uint8 is_allocated = 0;

                /** check if IN_LIF was allocated (to prevent taking 'garbage' IN_LIF') */
                {
                    if (dbal_phy_db == DBAL_PHYSICAL_TABLE_INLIF_1)
                    {
                    /** if PhyDB is INLIF_1 then check in shared by Core DB */
                        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                                        inlif_table_shared_by_cores.is_allocated(unit, chosen_in_lif, &is_allocated));
                    }
                    else if (dbal_phy_db == DBAL_PHYSICAL_TABLE_INLIF_2)
                    {

                    /** if PhyDB is INLIF_2 then check in shared by All-Core DB or per Single-Core DB */
                        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                                        inlif_table_dpc_all_cores.is_allocated(unit, chosen_in_lif, &is_allocated));

                        if (!is_allocated)
                        {
                            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                                            inlif_table_dedicated_per_core.is_allocated(unit, core_id, chosen_in_lif,
                                                                                        &is_allocated));
                        }
                    }
                }
                lif_is_valid = is_allocated;
            }

        } /** of else of hit bit support */

        /** at this point we know the the IN_LIF is non-zero and was actually either allocated or returned */
        if (lif_is_valid)
        {
            lif_strength_idx = ii;
            local_lif_found = TRUE;
            break;
        }
    } /** of for loop */

    /*
     * Read the founded lif information
     */
    if (local_lif_found)
    {
        shr_error_e error_code;
        *local_in_lif = chosen_in_lif;
        error_code = dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(unit,
                                                                             lif_arr_by_strength[lif_strength_idx *
                                                                                                 NOF_ELEMENTS +
                                                                                                 LIF_ELEMENT], core_id,
                                                                             dbal_phy_db, lif_dbal_table,
                                                                             lif_dbal_result_type, NULL);
        SHR_IF_ERR_EXIT_EXCEPT_IF(error_code, _SHR_E_NOT_FOUND);
        if (error_code == _SHR_E_NOT_FOUND)
        {
            /*
             * if lif information is missing handle as if lif not found 
             */
            *local_in_lif = 0;
        }
        else
        {
            *is_default_lif = (lif_strength_idx == 0);
        }
    }
    else
    {
        *local_in_lif = 0;
        *lif_dbal_result_type = 0;
        *lif_dbal_table = DBAL_TABLE_EMPTY;
    }

exit:
    SHR_FREE(res_sig_name);
    SHR_FREE(hit_sig_name);
    SHR_FREE(lif_arr_by_strength);
    sand_signal_output_free(signal_output_result);
    sand_signal_output_free(signal_output_port);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_ikleap_vis_build_key_and_result_strings(
    int unit,
    int context_hw_val,
    int acl_context,
    uint32 *key_sig_value,
    uint32 *res_sig_value,
    int key_sig_size,
    int res_sig_size,
    char *head_key_string,
    char *head_res_string,
    dbal_physical_tables_e phyDb,
    dbal_tables_e logical_table_for_kbr,
    int core_id,
    uint8 *nof_results,
    char **updated_res_string)
{
    int word_idx, char_count;
    char *key_string = head_key_string;
    char *res_string = head_res_string;
    int meaningful_lookup_type_string_offset = sal_strlen("KBP_LOOKUP_TYPE_");
    dbal_printable_entry_t *entry_print_info = NULL;
    kbp_printable_entry_t *entry_print_info_kbp = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** In case phy DB is KAPS, 3b strength are added to MSB, so for DBAL parsing the result size is 3b shorter */
    if ((phyDb == DBAL_PHYSICAL_TABLE_KAPS_1) || (phyDb == DBAL_PHYSICAL_TABLE_KAPS_2))
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(res_sig_value,
                                                         res_sig_size - IKEAP_KAPS_STRENGTH_WIDTH, res_sig_size - 1));
        res_sig_size -= IKEAP_KAPS_STRENGTH_WIDTH;
    }

    char_count = sal_snprintf(key_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "KEY(%db)=0x", key_sig_size);
    key_string += char_count;
    for (word_idx = BITS2WORDS(key_sig_size) - 1; word_idx >= 0; word_idx--)
    {
        if (word_idx == (BITS2WORDS(key_sig_size) - 1))
        {
            int print_width = (3 + (key_sig_size % 32)) / 4;

            if (print_width == 0)
            {
                print_width = 8;
            }
            char_count = sal_snprintf(key_string,
                                      DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "%0*x", print_width, key_sig_value[word_idx]);
            key_string += char_count;
        }
        else
        {
            char_count = sal_snprintf(key_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "%08x", key_sig_value[word_idx]);
            key_string += char_count;
        }
    }
    char_count = sal_snprintf(key_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "\n");
    key_string += char_count;

    /** Print Res as buffer */
    char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "RES(%db)=0x", res_sig_size);
    res_string += char_count;
    for (word_idx = BITS2WORDS(res_sig_size) - 1; word_idx >= 0; word_idx--)
    {
        if (word_idx == (BITS2WORDS(res_sig_size) - 1))
        {
            int print_width = (3 + (res_sig_size % 32)) / 4;

            if (print_width == 0)
            {
                print_width = 8;
            }
            char_count = sal_snprintf(res_string,
                                      DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "%0*x", print_width, res_sig_value[word_idx]);
            res_string += char_count;
        }
        else
        {
            char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "%08x", res_sig_value[word_idx]);
            res_string += char_count;
        }
    }
    char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "\n");
    res_string += char_count;

    /** Start key-result parsing using DBAL */
    {
        int key_idx, res_idx;

        SHR_ALLOC_SET_ZERO(entry_print_info, sizeof(dbal_printable_entry_t),
                           "entry_print_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC_SET_ZERO(entry_print_info_kbp, 8 * sizeof(kbp_printable_entry_t),
                           "entry_print_info_kbp", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        if (phyDb != DBAL_PHYSICAL_TABLE_KBP)
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_table_printable_entry_get(unit, logical_table_for_kbr,
                                                                     key_sig_value, core_id,
                                                                     res_sig_size, res_sig_value,
                                                                     entry_print_info), _SHR_E_NOT_FOUND);
        }
        else
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(kbp_mngr_opcode_printable_entry_get(unit, 0, context_hw_val, acl_context,
                                                                          key_sig_value, key_sig_size, res_sig_value,
                                                                          res_sig_size, nof_results,
                                                                          entry_print_info_kbp), _SHR_E_NOT_FOUND);
        }
        if (phyDb != DBAL_PHYSICAL_TABLE_KBP)
        {
            for (key_idx = 0; key_idx < entry_print_info->nof_key_fields; key_idx++)
            {
                char_count =
                    sal_snprintf(key_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s ", entry_print_info->key_fields_info[key_idx].field_name);
                key_string += char_count;
                char_count =
                    sal_snprintf(key_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s\n", entry_print_info->key_fields_info[key_idx].field_print_value);
                key_string += char_count;
            }

            for (res_idx = 0; res_idx < entry_print_info->nof_res_fields; res_idx++)
            {
                char_count =
                    sal_snprintf(res_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s ", entry_print_info->res_fields_info[res_idx].field_name);
                res_string += char_count;
                char_count =
                    sal_snprintf(res_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s\n", entry_print_info->res_fields_info[res_idx].field_print_value);
                res_string += char_count;
            }
        }
        else
        {
            int kbp_lkp_index = 0;

            for (kbp_lkp_index = 0; kbp_lkp_index < *nof_results; kbp_lkp_index++)
            {
                key_string = head_key_string + ((kbp_lkp_index + 1) * DIAG_DNX_KLEAP_MAX_STRING_LENGTH);
                res_string = head_res_string + ((kbp_lkp_index + 1) * DIAG_DNX_KLEAP_MAX_STRING_LENGTH);

                /** print KBP table name*/
                char_count = sal_snprintf(key_string,
                                          DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                          "table: %s\n", dbal_logical_table_to_string(unit,
                                                                                      entry_print_info_kbp
                                                                                      [kbp_lkp_index].table_id));
                key_string += char_count;
                for (key_idx = 0; key_idx < entry_print_info_kbp[kbp_lkp_index].entry_print_info.nof_key_fields;
                     key_idx++)
                {
                    char_count =
                        sal_snprintf(key_string,
                                     DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                     "    %s ",
                                     entry_print_info_kbp[kbp_lkp_index].entry_print_info.
                                     key_fields_info[key_idx].field_name);
                    key_string += char_count;
                    char_count =
                        sal_snprintf(key_string,
                                     DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                     "%s\n",
                                     entry_print_info_kbp[kbp_lkp_index].entry_print_info.
                                     key_fields_info[key_idx].field_print_value);
                    key_string += char_count;
                }

                if (entry_print_info_kbp[kbp_lkp_index].entry_print_info.nof_res_fields > 0)
                {
                    char_count =
                        sal_snprintf(res_string,
                                     DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                     "%s%s:\n",
                                     (entry_print_info_kbp[kbp_lkp_index].lookup_type_str +
                                      meaningful_lookup_type_string_offset), " LOOKUP");
                    res_string += char_count;
                    for (res_idx = 0; res_idx < entry_print_info_kbp[kbp_lkp_index].entry_print_info.nof_res_fields;
                         res_idx++)
                    {
                        char_count =
                            sal_snprintf(res_string,
                                         DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                         "%s ",
                                         entry_print_info_kbp[kbp_lkp_index].entry_print_info.
                                         res_fields_info[res_idx].field_name);
                        res_string += char_count;
                        char_count =
                            sal_snprintf(res_string,
                                         DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                         "%s\n",
                                         entry_print_info_kbp[kbp_lkp_index].entry_print_info.
                                         res_fields_info[res_idx].field_print_value);
                        res_string += char_count;
                    }
                }
                else
                {
                    char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "No Match\n");
                    res_string += char_count;
                }
            }
        }

    }
    *updated_res_string = res_string;

exit:
    SHR_FREE(entry_print_info);
    SHR_FREE(entry_print_info_kbp);
    SHR_FUNC_EXIT;
}

static shr_error_e
ikleap_kbp_aligner_check(
    int unit,
    uint32 kbr_idx,
    dbal_fields_e context_profile_field,
    int context,
    stage_t ikleap_stage,
    uint32 key_num,
    uint8 kbr_is_acl,
    uint32 *kbp_aligner_key_size)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    uint32 fwd12_aligner_mapper_handle_id;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &fwd12_aligner_mapper_handle_id));
    dbal_entry_key_field32_set(unit, fwd12_aligner_mapper_handle_id, DBAL_FIELD_KBP_KBR_IDX, key_num);
    dbal_entry_key_field32_set(unit, fwd12_aligner_mapper_handle_id, context_profile_field, context);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, fwd12_aligner_mapper_handle_id, DBAL_GET_ALL_FIELDS));

    if (!kbr_is_acl)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, fwd12_aligner_mapper_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE,
                         kbp_aligner_key_size));

    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, fwd12_aligner_mapper_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE,
                         kbp_aligner_key_size));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, fwd12_aligner_mapper_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}


sh_sand_man_t dnx_ikleap_visib_man = {.brief = "Show last packet key and payload per lookup",
    .full = "Show last packet key and payload per lookup",
    .init_cb = diag_sand_packet_send_simple,
    .synopsis = "[stage=<fwd1(2)/vtt1(2,3,4,5)>] [PhyDB=<phyDbName>]",
    .examples = "stage=fwd1 phyDB=LEM\n" "stage=vtt1 silence=Yes\n" "stage=fwd1 SHORT",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};
sh_sand_option_t dnx_ikleap_visib_options[] = {
    {"STAGE", SAL_FIELD_TYPE_STR, "KLEAP stage", ""}
    ,
    {"PhyDb", SAL_FIELD_TYPE_STR, "Physical DB", ""}
    ,
    {"silence", SAL_FIELD_TYPE_BOOL, NULL, "Yes"}
    ,
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};

/*
* \brief - get FODO and VID masks based on fodo_assignment_mode
*/
static shr_error_e
diag_ikleap_fodo_pd_mask_length(
    int unit,
    uint32 fodo_assignment_mode,
    uint32 *fodo_mask,
    uint32 *pd_mask)
{
    uint32 entry_handle_id;
    uint32 fodo_assignment_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get profile_id base on mode_id (PD_id is masked out)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FODO_ASSIGNMENT_MODE_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, fodo_assignment_mode);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_PD_RESULT_TYPE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FORWARD_DOMAIN_MASK_PROFILE, INST_SINGLE,
                     &fodo_assignment_profile));

    /*
     * get fodo and pd mask lengthes
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FODO_ASSIGNMENT_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_DOMAIN_MASK_PROFILE, fodo_assignment_profile);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FODO_BASE_MASK, INST_SINGLE, fodo_mask));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PD_RESULT_MASK, INST_SINGLE, pd_mask));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
* \brief - get VID form packet (outer VID) or default VID if packet untagged
*/
static shr_error_e
diag_ikleap_get_vid(
    int unit,
    int core_id,
    uint32 *vid)
{
    uint32 entry_handle_id;
    uint32 in_port, vt1_context;
    signal_output_t *signal_output_header = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "", "VTT1", "Context", &vt1_context, 1));
    if (((vt1_context == DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED)
         || (vt1_context == DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED_FRR)))
    {
        /*
         * get default VID based on in_port signal
         */
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "", "LLR", "In_Port", &in_port, 1));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, in_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, vid);
    }
    else
    {
        uint8 found_eth = FALSE;
        signal_output_t *first_header_sig = NULL;
        signal_output_t *second_header_sig = NULL;
        signal_output_t *eth_header_sig = NULL;
        /*
         * get VID from outer ETH on packet header
         * The signal structure is HEADER->ETH1/2->VLAN->VID (assuming PTchCH optional before ETH)
         * If layers 1/2 are not ETH, assuming VID=0
         */
        SHR_IF_ERR_EXIT(dnx_pp_ingress_header_sig_get(unit, core_id, &signal_output_header));
        first_header_sig = (signal_output_t *) (signal_output_header->field_list->top);

        if (sal_strstr(first_header_sig->entry.name, "PTCH"))
        {
            second_header_sig = (signal_output_t *) (first_header_sig->entry.next);
            if (sal_strstr(second_header_sig->entry.name, "ETH"))
            {
                found_eth = TRUE;
                eth_header_sig = second_header_sig;
            }
        }
        else if (sal_strstr(first_header_sig->entry.name, "ETH"))
        {
            found_eth = TRUE;
            eth_header_sig = first_header_sig;
        }
        else
        {
            *vid = 0;
            SHR_EXIT();
        }

        if (found_eth)
        {
            uint8 found_vlan = FALSE;
            signal_output_t *vlan_header_sig;
            rhentry_t *entry;

            if (eth_header_sig == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_NONE,
                             "found_eth flag is TRUE, but eth_header_sig is still NULL, something wrong in parsing\n");
            }

            for (entry = eth_header_sig->field_list->top; entry; entry = entry->next)
            {
                if (!sal_strcasecmp(entry->name, "VLAN"))
                {
                    found_vlan = TRUE;
                    vlan_header_sig = (signal_output_t *) entry;
                    break;
                }
            }
            if (found_vlan)
            {
                for (entry = vlan_header_sig->field_list->top; entry; entry = entry->next)
                {
                    if (!sal_strcasecmp(entry->name, "VID"))
                    {
                        *vid = ((signal_output_t *) entry)->value[0];
                        break;
                    }
                }
            }
            else
            {
                *vid = 0;
                SHR_EXIT();
            }
        }
        else
        {
            *vid = 0;
            SHR_EXIT();
        }
    }

exit:
    sand_signal_output_free(signal_output_header);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
* \brief - get fodo_value (FODO or VSI encoded) and fodo_assignement_mode from the LIF
*/
static shr_error_e
diag_ikleap_get_fodo_from_lif(
    int unit,
    int core_id,
    char *stage_name,
    uint32 stage_index,
    uint32 lif_handle_id,
    dbal_tables_e lif_dbal_table,
    uint32 lif_dbal_result_type,
    uint32 *fodo_assignement_mode,
    uint32 *fodo_value)
{
    int rv;
    uint32 lif_fodo_value = 0;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get FODO from previous stage
     */
    if (stage_index == KLEAP_STAGE_VT1)
    {
        *fodo_value = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "", stage_name, "Fwd_Domain_ID", fodo_value, 1));
    }

    rv = dbal_tables_field_info_get_no_err(unit, lif_dbal_table, DBAL_FIELD_FODO_ASSIGNMENT_MODE, 0,
                                           lif_dbal_result_type, 0, &field_info);
    {
        if (rv == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, lif_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, 0, fodo_assignement_mode));
        }
        else if (rv == _SHR_E_NOT_FOUND)
        {
            /*
             * set default value
             */
            *fodo_assignement_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    /** check if FODO/VSI/VRF exists on lif */
    rv = dbal_tables_field_info_get_no_err(unit, lif_dbal_table, DBAL_FIELD_FODO, 0, lif_dbal_result_type, 0,
                                           &field_info);
    if (rv == _SHR_E_NOT_FOUND)
    {
        rv = dbal_tables_field_info_get_no_err(unit, lif_dbal_table, DBAL_FIELD_VSI, 0, lif_dbal_result_type, 0,
                                               &field_info);
        if (rv == _SHR_E_NOT_FOUND)
        {
            rv = dbal_tables_field_info_get_no_err(unit, lif_dbal_table, DBAL_FIELD_VRF, 0, lif_dbal_result_type, 0,
                                                   &field_info);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(rv);
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, lif_handle_id, DBAL_FIELD_VRF, 0, &lif_fodo_value));
                /** convert VRF to FODO */
                SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                (unit, DBAL_FIELD_FODO, DBAL_FIELD_VRF, &lif_fodo_value, fodo_value));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, lif_handle_id, DBAL_FIELD_VSI, 0, &lif_fodo_value));
            /** convert VSI to FODO */
            SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                            (unit, DBAL_FIELD_FODO, DBAL_FIELD_VSI, &lif_fodo_value, fodo_value));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, lif_handle_id, DBAL_FIELD_FODO, 0, fodo_value));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Diagnostic to see the key build instruction for ingress KLEAP lookups.
 * Shows key, result, hit indication
 */
shr_error_e
sh_dnx_ikleap_visib_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /*
     *  Diagnostic inputs
     */
    char *stage_name = NULL;
    char *phy_db_name = NULL;

    /*
     * Diagnostic flow control parameters, resolved from inputs
     */
    uint32 stage_index, sub_stage;
    int all_stages_flag = 0, matched_stage_idx = 0, matched_sub_stage_idx = 0;
    int core_in, core_id;
    dbal_physical_tables_e phy_db_id;

    /*
     * DBAL handlers to hard logic tables, used to read info
     */
    uint32 rsrcs_map_handle_id;
    uint32 kbr_info_handle_id;

    /*
     * HW parameters per stage
     */
    int nof_kbr;
    int nof_ffc;
    dbal_fields_e kbr_idx_field;
    dbal_fields_e context_profile_field;
    dbal_physical_tables_e *kbr2phy_mapping;

    /** Local parameters for saving multiple data per KBR */
    int rv = 0, ii;
    uint32 kbr_idx = 0;
    uint8 lkp_index;
    uint8 nof_results;

    /** usually used for intergration in a larger diagnostics commands such as 'diag pp vis last' */
    uint8 short_mode = FALSE;

    uint32 *temp_key_sig_value = NULL;
    uint32 *key_sig_value = NULL, *res_sig_value = NULL, *app_sig_value = NULL, *hit_sig_value = NULL, *err_sig_value =
        NULL;
    char *key_sig_name = NULL;
    char *res_sig_name = NULL;
    char *app_sig_name = NULL;
    char *hit_sig_name = NULL;
    char *err_sig_name = NULL;
    char *error_sig_name = NULL;
    char *context_name = NULL;
    uint8 bare_metal_disabled = TRUE;
    uint8 substage0_hit_bit_supported = TRUE;

    signal_output_t *signal_output_key = NULL;
    signal_output_t *signal_output_result = NULL;
    signal_output_t *signal_output_err = NULL;
    signal_output_t *signal_output_hit = NULL;
    signal_output_t *signal_output_app_db = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_STR("STaGe", stage_name);
    SH_SAND_GET_STR("PhyDb", phy_db_name);
    SH_SAND_GET_INT32("core", core_in);
    SH_SAND_GET_BOOL("SHORT", short_mode);


    /** check if in this device the hit_bit indication is working in substage0 */
    if (dnx_data_debug.feature.feature_get(unit, dnx_data_debug_feature_no_hit_bit_on_mdb_access))
    {
        substage0_hit_bit_supported = FALSE;
    }

    SHR_ALLOC_SET_ZERO(temp_key_sig_value, DSIG_MAX_SIZE_UINT32 * sizeof(uint32),
                       "temp_key_sig_value value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(key_sig_value, DSIG_MAX_SIZE_UINT32 * sizeof(uint32),
                       "key sig value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(key_sig_name, SIGNALS_MAX_NAME_LENGTH + 1, "key_sig_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(res_sig_name, SIGNALS_MAX_NAME_LENGTH + 1, "res_sig_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(app_sig_name, SIGNALS_MAX_NAME_LENGTH + 1, "app_sig_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(hit_sig_name, SIGNALS_MAX_NAME_LENGTH + 1, "hit_sig_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(err_sig_name, SIGNALS_MAX_NAME_LENGTH + 1, "err_sig_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(error_sig_name, SIGNALS_MAX_NAME_LENGTH + 1,
                       "error_sig_name", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(context_name, DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                       "context_name value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &bare_metal_disabled));

    /*
     * Update diagnostic control parameters according to inputs
     */
    SHR_IF_ERR_EXIT(diag_kleap_decide_stage(unit, stage_name, &matched_stage_idx,
                                            &matched_sub_stage_idx, &all_stages_flag));
    SHR_IF_ERR_EXIT(diag_kleap_decide_phy_db_id(unit, phy_db_name, &phy_db_id));

    DIAG_DNX_IKLEAP_DUMMY_DBAL_TABLE_ALLOC(kbr_info_handle_id);
    DIAG_DNX_IKLEAP_DUMMY_DBAL_TABLE_ALLOC(rsrcs_map_handle_id);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, !short_mode);

        for (stage_index = 0; stage_index < KLEAP_NOF_STAGES; stage_index++)
        {
            for (sub_stage = 0; sub_stage <= STAGE_INFO.stage_type; sub_stage++)
            {
                uint32 context_enum_val, context_hw_val, acl_context;
                uint32 nasid;
                uint8 first_kbr_printed = FALSE;
                dbal_field_types_defs_e context_enum;
                CONST dbal_field_types_basic_info_t *context_field_type;

                if (DIAG_IKLEAP_SKIP_STAGE)
                {
                    continue;
                }
                stage_name = STAGE_INFO.kleap_sub_stages_names[sub_stage];

                            /** Init Hw parameters (per stage) */
                DIAG_IKELAP_COPY_STAGE_INFO_TO_LOCAL_PARAMS;

                            /** Alloc handles for DBAL tables */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, STAGE_INFO.dbal_table_kbr_info, kbr_info_handle_id));

                            /** Get the resources configuration of the stage */
                if (STAGE_INFO.stage_type != SINGLE_STAGE_TYPE)
                {
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                                    (unit, STAGE_INFO.dbal_table_resource_mapping, rsrcs_map_handle_id));
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, rsrcs_map_handle_id, DBAL_GET_ALL_FIELDS));
                }

                DIAG_IKLEAP_VIS_SET_CONTEXT_AND_NASID;

                context_enum = STAGE_INFO.dbal_context_enum[sub_stage];
                SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, context_enum, &context_field_type));

                if ((context_enum_val >= context_field_type->nof_enum_values)
                    || (context_field_type->enum_val_info[context_enum_val].is_invalid))
                {
                    sal_strncpy_s(context_name, "unknown context", DIAG_DNX_KLEAP_MAX_STRING_LENGTH - 1);
                }
                else
                {
                    sal_strncpy_s(context_name, context_field_type->enum_val_info[context_enum_val].name,
                                  DIAG_DNX_KLEAP_MAX_STRING_LENGTH - 1);
                }

                /** Set the tables columns  */
                PRT_TITLE_SET("Context %s(%d), Core %d", context_name, context_enum_val, core_id);
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Logical DB (phyDB, AppDbId)");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key values");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result values");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Error");

                /*
                 *  Loop over all KBR of the stage
                 *  only the valid ones should be printed
                 */
                for (kbr_idx = 2; kbr_idx < nof_kbr; kbr_idx++)
                {
                    int nof_key_signals_to_concatanate = 1, first_interface_in_key = 0;
                    uint8 kbr_has_app_db_id;
                    uint8 fixed_lookup_key_indication = FALSE;
                    uint8 fixed_lookup_key_index = 0;
                    uint32 app_db_id = 0;
                    uint8 has_ffc = TRUE;
                    uint32 ffc_bitmap[DIAG_DNX_KLEAP_FFC_U32_BITMAP_SIZE] = { 0 };
                    dbal_tables_e logical_table_for_kbr = DBAL_NOF_TABLES;
                    uint32 kbp_aligner_key_size = 0;

                    /*
                     * Signals call variables
                     */
                    char *key_string = NULL, *res_string = NULL;
                    uint32 key_sig_size = 0, res_sig_size = 0;

                    char *index_as_str[MAX_NUM_INTERFACES_PER_PHY_DB] = { "0", "1", "2", "3" };
                    char signal_prefix[10] = { "" };
                    char *phy_db_name = NULL;

                    sal_memset(key_sig_value, 0, DSIG_MAX_SIZE_UINT32 * sizeof(uint32));
                    sal_memset(key_sig_name, 0, SIGNALS_MAX_NAME_LENGTH + 1);
                    sal_memset(res_sig_name, 0, SIGNALS_MAX_NAME_LENGTH + 1);
                    sal_memset(app_sig_name, 0, SIGNALS_MAX_NAME_LENGTH + 1);
                    sal_memset(hit_sig_name, 0, SIGNALS_MAX_NAME_LENGTH + 1);
                    sal_memset(err_sig_name, 0, SIGNALS_MAX_NAME_LENGTH + 1);
                    sal_memset(error_sig_name, 0, SIGNALS_MAX_NAME_LENGTH + 1);

                    DIAG_IKLEAP_SKIP_KBR_CHECK;
                    DIAG_IKLEAP_CHECK_APP_DB_ID(kbr_has_app_db_id);

                    phy_db_name = dbal_physical_table_to_string(unit, kbr2phy_mapping[kbr_idx]);

                    /*
                     * Get the KBR info, AppDbId and FFC bitmap
                     */
                    dbal_entry_key_field32_set(unit, kbr_info_handle_id, context_profile_field, context_hw_val);
                    dbal_entry_key_field32_set(unit, kbr_info_handle_id, kbr_idx_field, kbr_idx);

                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, kbr_info_handle_id, DBAL_GET_ALL_FIELDS));
                    if (kbr_has_app_db_id)
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, kbr_info_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, &app_db_id));
                    }
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                    (unit, kbr_info_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap));

                    DIAG_IKLEAP_SKIP_KBR_CHECK_2;

                    first_kbr_printed = TRUE;
                    if (fixed_lookup_key_indication)
                    {
                        /*
                         * check if the lookup is enabled per context
                         */
                        uint32 context_handle_id;
                        uint32 enable_bit = 0;
                        dbal_tables_e table = fixed_key_lookup[fixed_lookup_key_index].context_enablers_table;
                        dbal_fields_e key_field = fixed_key_lookup[fixed_lookup_key_index].key_field;
                        dbal_fields_e enabler_field = fixed_key_lookup[fixed_lookup_key_index].context_enablers_field;

                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &context_handle_id));
                        dbal_entry_key_field32_set(unit, context_handle_id, key_field, context_enum_val);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, context_handle_id, DBAL_GET_ALL_FIELDS));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, context_handle_id, enabler_field, INST_SINGLE, &enable_bit));
                        DBAL_HANDLE_FREE(unit, context_handle_id);
                        if (enable_bit)
                        {
                            nof_key_signals_to_concatanate = fixed_key_lookup[fixed_lookup_key_index].nof_interfaces;
                            first_interface_in_key =
                                STAGE_INFO.kbr_if_id[fixed_key_lookup[fixed_lookup_key_index].kbr_idx
                                                     [nof_key_signals_to_concatanate - 1]];
                            sal_strncpy_s(key_sig_name, fixed_key_lookup[fixed_lookup_key_index].key_sig_name,
                                          SIGNALS_MAX_NAME_LENGTH);
                            sal_strncpy_s(app_sig_name, fixed_key_lookup[fixed_lookup_key_index].app_sig_name,
                                          SIGNALS_MAX_NAME_LENGTH);
                        }
                        else
                        {
                            fixed_lookup_key_indication = FALSE;
                            if (!has_ffc)
                            {
                                continue;
                            }
                        }
                    }
                    else
                    {
                        if (!has_ffc)
                        {
                            continue;
                        }
                    }

                    if (!fixed_lookup_key_indication)
                    {
                        sal_strncpy_s(key_sig_name, "Key", SIGNALS_MAX_NAME_LENGTH);
                        sal_strncpy_s(app_sig_name, "AppDb", SIGNALS_MAX_NAME_LENGTH);
                    }

                    if (!fixed_lookup_key_indication)
                    {
                        if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_TCAM)
                        {
                            CONST dbal_logical_table_t *dbal_table;
                            SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get
                                            (unit, kbr2phy_mapping[kbr_idx], app_db_id, &logical_table_for_kbr));

                            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, logical_table_for_kbr, &dbal_table));
                            if (dbal_table->key_size > 160)
                            {
                                /*
                                 *  Need to:
                                 *  1. read two keys and concatenate
                                 *  2. check which interface holds the result/hit/error
                                 */
                                if ((kbr_idx % 2) == 1)
                                {
                                    continue;
                                }
                                nof_key_signals_to_concatanate = 2;
                                first_interface_in_key = STAGE_INFO.kbr_if_id[kbr_idx + 1];
                            }
                            else
                            {
                                first_interface_in_key = STAGE_INFO.kbr_if_id[kbr_idx];
                            }
                        }
                        else if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                        {
                            /*
                             *  Need to:
                             *  1. read X keys and concatenate - according to aligner
                             *  2. check which interface holds the result/hit/error
                             */

                            if (kbr_idx != 4)
                            {
                                continue;
                            }
                            nof_key_signals_to_concatanate = 4;
                            first_interface_in_key = 0;
                        }
                        else
                        {
                            first_interface_in_key = STAGE_INFO.kbr_if_id[kbr_idx];
                        }
                    }

                    sal_strncpy_s(signal_prefix, "_", sizeof(signal_prefix));
                    sal_strncpy_s(res_sig_name, "Result", SIGNALS_MAX_NAME_LENGTH);
                    sal_strncpy_s(err_sig_name, "Err", SIGNALS_MAX_NAME_LENGTH);
                    sal_strncpy_s(error_sig_name, "Error", SIGNALS_MAX_NAME_LENGTH);
                    sal_strncpy_s(hit_sig_name, "Hit", SIGNALS_MAX_NAME_LENGTH);

                    if (first_interface_in_key == INVALID_IF)
                    {
                        continue;
                    }

                    if (first_interface_in_key >= IF_0)
                    {
                        sal_strncat(signal_prefix, index_as_str[first_interface_in_key], 1);
                        sal_strncat_s(key_sig_name, signal_prefix, SIGNALS_MAX_NAME_LENGTH);
                        sal_strncat_s(res_sig_name, signal_prefix, SIGNALS_MAX_NAME_LENGTH);
                        sal_strncat_s(hit_sig_name, signal_prefix, SIGNALS_MAX_NAME_LENGTH);
                        sal_strncat_s(err_sig_name, signal_prefix, SIGNALS_MAX_NAME_LENGTH);
                        sal_strncat_s(error_sig_name, signal_prefix, SIGNALS_MAX_NAME_LENGTH);
                        sal_strncat_s(app_sig_name, signal_prefix, SIGNALS_MAX_NAME_LENGTH);
                    }

                    if (kbr_has_app_db_id)
                    {
                        /*
                         * Get the APP_DB_ID sent signal
                         */
                        SHR_IF_ERR_EXIT(sand_signal_output_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", stage_name, phy_db_name,
                                         app_sig_name, &signal_output_app_db));
                        app_sig_value = signal_output_app_db->value;
                        if ((app_sig_value[0] != app_db_id) && (nof_key_signals_to_concatanate == 1))
                        {
                            if (app_sig_value[0] == 0)
                            {
                                /*
                                 * Probably not a real lookup, print info message and continue
                                 */
                                LOG_CLI((BSL_META
                                         ("The sent App Db Id (%d) is not equal to the App Db Id (%d) on the KBR. stage  %s, kbr idx = %d\n"),
                                         app_sig_value[0], app_db_id, stage_name, kbr_idx));
                                continue;
                            }
                            else
                            {
                                SHR_ERR_EXIT(_SHR_E_CONFIG,
                                             "The sent App Db Id (%d) is not equal to the App Db Id (%d) on the KBR. stage  %s, kbr idx = %d\n",
                                             app_sig_value[0], app_db_id, stage_name, kbr_idx);
                            }
                        }
                    }

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    DIAG_IKLEAP_PRINT_LOGICAL_TABLE_TO_PRT;

                    for (lkp_index = 0; lkp_index < DNX_KBP_MAX_NOF_LOOKUPS + 1; lkp_index++)
                    {
                        sal_strncpy_s(key_signal_str[lkp_index], EMPTY, 1);
                        sal_strncpy_s(res_signal_str[lkp_index], EMPTY, 1);
                    }

                    key_string = &key_signal_str[0][0];
                    res_string = &res_signal_str[0][0];

                    /*
                     * Get the KEY sent signal
                     */
                    for (ii = 0; ii < nof_key_signals_to_concatanate; ii++)
                    {
                        uint8 kbr_is_acl = FALSE;

                        /** Check if the key is ACL or FWD (key is mapped to KBR) */
                        dbal_entry_key_field32_set(unit, kbr_info_handle_id, context_profile_field, context_hw_val);
                        dbal_entry_key_field32_set(unit, kbr_info_handle_id, kbr_idx_field, key_sig_fwd12_2kbp_kbr[ii]);

                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, kbr_info_handle_id, DBAL_GET_ALL_FIELDS));

                        if (STAGE_INFO.stage_type != SINGLE_STAGE_TYPE)
                        {
                            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                            (unit, kbr_info_handle_id, DBAL_FIELD_KBR_IS_ACL, INST_SINGLE,
                                             &kbr_is_acl));
                        }

                        if (ii != 0)
                        {
                            if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                            {

                                key_sig_name[sal_strlen(key_sig_name) - 1] = '0' + ii;
                            }
                            else
                            {
                                key_sig_name[sal_strlen(key_sig_name) - 1] = '0' + first_interface_in_key - ii;
                            }
                        }
                        SHR_IF_ERR_EXIT(sand_signal_output_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", stage_name, phy_db_name, key_sig_name,
                                         &signal_output_key));

                        if (ii == 0)
                        {
                            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                            (signal_output_key->value, 0, signal_output_key->size, key_sig_value));

                            key_sig_size = signal_output_key->size;

                            if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                            {

                                /** Get the aligner size for the ACL or FWD key */
                                SHR_IF_ERR_EXIT(ikleap_kbp_aligner_check(unit, kbr_idx, context_profile_field,
                                                                         context_hw_val, STAGE_INFO, ii, kbr_is_acl,
                                                                         &kbp_aligner_key_size));

                                key_sig_size = BYTES2BITS(kbp_aligner_key_size);

                                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                                (signal_output_key->value, signal_output_key->size - key_sig_size,
                                                 key_sig_size, key_sig_value));

                            }
                        }
                        else
                        {
                            sal_memset(temp_key_sig_value, 0, DSIG_MAX_SIZE_UINT32 * sizeof(uint32));

                            if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                            {
                                /** in case of KBP with x4 possible container keys - concatenate them into one big key */
                                uint32 aligner_size_in_bits;

                                SHR_IF_ERR_EXIT(ikleap_kbp_aligner_check(unit, kbr_idx, context_profile_field,
                                                                         context_hw_val, STAGE_INFO, ii, kbr_is_acl,
                                                                         &kbp_aligner_key_size));
                                aligner_size_in_bits = BYTES2BITS(kbp_aligner_key_size);

                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (key_sig_value, 0, key_sig_size, temp_key_sig_value));
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (temp_key_sig_value, aligner_size_in_bits, key_sig_size,
                                                 key_sig_value));

                                /** utilex_bitstream_get_any_field zeros the whole UINT32 if it writes to it, therefore using a temp variable */
                                sal_memset(temp_key_sig_value, 0, DSIG_MAX_SIZE_UINT32 * sizeof(uint32));

                                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                                (signal_output_key->value,
                                                 signal_output_key->size - aligner_size_in_bits, aligner_size_in_bits,
                                                 temp_key_sig_value));

                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (temp_key_sig_value, 0, aligner_size_in_bits, key_sig_value));

                                key_sig_size += aligner_size_in_bits;
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (key_sig_value, 0, key_sig_size, temp_key_sig_value));
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (temp_key_sig_value, signal_output_key->size, key_sig_size,
                                                 key_sig_value));
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (signal_output_key->value, 0, signal_output_key->size, key_sig_value));
                                key_sig_size += signal_output_key->size;
                            }
                        }
                    } /** of for (ii < nof_key_signals_to_concatanate) */

                    /*
                     * Get the RESULT sent signal
                     */
                    if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                    {
                        SHR_IF_ERR_EXIT(sand_signal_output_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", "FWD2", "IPMF1", "Elk_Lkp_Payload",
                                         &signal_output_result));
                        res_sig_value = signal_output_result->value;
                        res_sig_size = signal_output_result->size;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(sand_signal_output_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name, res_sig_name,
                                         &signal_output_result));
                        res_sig_value = signal_output_result->value;
                        res_sig_size = signal_output_result->size;

                        /*
                         * Get the HIT sent signal
                         */
                        SHR_IF_ERR_EXIT(sand_signal_output_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name, hit_sig_name,
                                         &signal_output_hit));
                        hit_sig_value = signal_output_hit->value;

                        /*
                         * Get the ERROR sent signal
                         */
                        rv = sand_signal_output_find(unit, core_id, SIGNALS_MATCH_ASIC, "",
                                                     phy_db_name, stage_name, err_sig_name, &signal_output_err);
                        if (rv == _SHR_E_NOT_FOUND)
                        {
                            SHR_IF_ERR_EXIT(sand_signal_output_find
                                            (unit, core_id, SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name,
                                             error_sig_name, &signal_output_err));
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(rv);
                        }
                        err_sig_value = signal_output_err->value;
                    }

                    SHR_IF_ERR_EXIT(diag_ikleap_vis_build_key_and_result_strings
                                    (unit, context_hw_val, acl_context, key_sig_value, res_sig_value, key_sig_size,
                                     res_sig_size, key_string, res_string, kbr2phy_mapping[kbr_idx],
                                     logical_table_for_kbr, core_id, &nof_results, &res_string));

                    /*
                     *  Print info to table:
                     *  KEY - always printed
                     *  RESULT -
                     *      if hit indication is valid, print according to hit indication
                     *      if hit indication is invalid (sub stage == 0), print result with warning
                     *  ERR - printed only if exists
                     */
                    if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                    {
                        uint8 kbp_lkp_index;

                        for (kbp_lkp_index = 0; kbp_lkp_index <= nof_results; kbp_lkp_index++)
                        {
                            PRT_CELL_SET("%s", key_signal_str[kbp_lkp_index]);
                            PRT_CELL_SET("%s", res_signal_str[kbp_lkp_index]);
                            if ((kbp_lkp_index + 1) <= nof_results)
                            {
                                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                                PRT_CELL_SET("%s", "");
                            }
                        }
                    }
                    else
                    {
                        PRT_CELL_SET("%s", &key_signal_str[0][0]);
                        if (substage0_hit_bit_supported || sub_stage == 1)
                        {
                            if (hit_sig_value[0] != 0)
                            {
                                PRT_CELL_SET("%s", &res_signal_str[0][0]);
                            }
                            else
                            {
                                PRT_CELL_SET("%s", "No Match");
                            }
                            PRT_CELL_SET("%s", err_sig_value[0] == 0 ? " " : "Err!");
                        }
                        else
                        {
                            int char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                                          "\n%s\n", "Unknown Match");
                            res_string += char_count;
                            PRT_CELL_SET("%s", &res_signal_str[0][0]);
                            PRT_CELL_SET("%s", err_sig_value[0] == 0 ? " " : "Err!");
                        }
                    }
                }/** of for on KBR*/

                /*
                 * Additional fixed lookups: Lif and VSI
                 * VT stages only
                 */
                {
                    uint8 is_vt_stage = FALSE;
                    uint32 context_handle_id;
                    dbal_tables_e context_table;
                    dbal_fields_e context_field;
                    uint32 vsi_enabler = 0, lif_enabler = 0;
                    {
                        /*
                         * check that the vsi lookup enabler is enable
                         */
                        if (stage_index == KLEAP_STAGE_VT1)
                        {
                            context_table = DBAL_TABLE_INGRESS_IRPP_VT1_CONTEXT_PROPERTIES;
                            context_field = DBAL_FIELD_VT1_CONTEXT_ID;
                            is_vt_stage = TRUE;
                        }
                        else if (stage_index == KLEAP_STAGE_VT23)
                        {
                            if (sub_stage == 0)
                            {
                                context_table = DBAL_TABLE_INGRESS_IRPP_VT2_CONTEXT_PROPERTIES;
                                context_field = DBAL_FIELD_VT2_CONTEXT_ID;
                            }
                            else
                            {
                                context_table = DBAL_TABLE_INGRESS_IRPP_VT3_CONTEXT_PROPERTIES;
                                context_field = DBAL_FIELD_VT3_CONTEXT_ID;
                            }
                            is_vt_stage = TRUE;
                        }
                        else if (stage_index == KLEAP_STAGE_VT45)
                        {
                            if (sub_stage == 0)
                            {
                                context_table = DBAL_TABLE_INGRESS_IRPP_VT4_CONTEXT_PROPERTIES;
                                context_field = DBAL_FIELD_VT4_CONTEXT_ID;
                            }
                            else
                            {
                                context_table = DBAL_TABLE_INGRESS_IRPP_VT5_CONTEXT_PROPERTIES;
                                context_field = DBAL_FIELD_VT5_CONTEXT_ID;
                            }
                            is_vt_stage = TRUE;
                        }
                    }
                    if (is_vt_stage)
                    {
                        uint8 has_vsi = FALSE;
                        char *key_string = NULL, *res_string = NULL;
                        uint32 local_in_lif = 0;
                        uint32 vsi_value = 0, fodo_value = 0;
                        dbal_tables_e lif_dbal_table = DBAL_TABLE_EMPTY;
                        uint32 lif_dbal_result_type = 0;
                        uint32 lif_handle_id, vsi_handle_id;
                        dbal_entry_handle_t *entry_handle;
                        int is_default_lif = 0;

                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, context_table, &context_handle_id));
                        dbal_entry_key_field32_set(unit, context_handle_id, context_field, context_enum_val);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, context_handle_id, DBAL_GET_ALL_FIELDS));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, context_handle_id, DBAL_FIELD_VSI_LOOKUP_ENABLE, INST_SINGLE,
                                         &vsi_enabler));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, context_handle_id, DBAL_FIELD_LIF_ARR_RESOLUTION_ENABLE, INST_SINGLE,
                                         &lif_enabler));
                        DBAL_HANDLE_FREE(unit, context_handle_id);

                        if (bare_metal_disabled && lif_enabler)
                        {
                            SHR_IF_ERR_EXIT(diag_dnx_ikleap_find_last_local_lif
                                            (unit, stage_index, sub_stage, core_id, context_enum_val, first_kbr_printed,
                                             &local_in_lif, &lif_dbal_result_type, &lif_dbal_table, &is_default_lif));
                            if (local_in_lif != 0)
                            {
                                uint32 fodo_assignement_mode = 0;
                                uint32 fodo_mask;
                                uint32 pd_mask;
                                uint32 fodo_vsi_value = 0;
                                uint32 pd_vid;
                                dbal_fields_e sub_field_id = DBAL_FIELD_EMPTY;

                                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, lif_dbal_table, &lif_handle_id));
                                dbal_entry_key_field32_set(unit, lif_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
                                dbal_entry_value_field32_set(unit, lif_handle_id, DBAL_FIELD_RESULT_TYPE, 0,
                                                             lif_dbal_result_type);
                                SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_handle_id, DBAL_GET_ALL_FIELDS));
                                SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, lif_handle_id, &entry_handle));

                                sal_strncpy_s(key_signal_str[0], EMPTY, 1);
                                sal_strncpy_s(res_signal_str[0], EMPTY, 1);
                                key_string = &key_signal_str[0][0];
                                res_string = &res_signal_str[0][0];

                                SHR_IF_ERR_EXIT(diag_ikleap_vis_build_key_and_result_strings
                                                (unit, context_hw_val, 0, entry_handle->phy_entry.key,
                                                 entry_handle->phy_entry.payload, entry_handle->phy_entry.key_size,
                                                 entry_handle->phy_entry.payload_size, key_string, res_string,
                                                 DBAL_PHYSICAL_TABLE_INLIF_1, lif_dbal_table, core_id, &nof_results,
                                                 &res_string));

                                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                                PRT_CELL_SET("%s(%s%s)", dbal_logical_table_to_string(unit, lif_dbal_table), "INLIF_",
                                             stage_index == KLEAP_STAGE_VT1 ? "1" : "2");
                                PRT_CELL_SET("%s%s", &key_signal_str[0][0], is_default_lif ? "(default)\n" : "\n");
                                PRT_CELL_SET("%s", &res_signal_str[0][0]);

                                /*
                                 * get PD VID from packet or default
                                 */
                                SHR_IF_ERR_EXIT(diag_ikleap_get_vid(unit, core_id, &pd_vid));

                                /*
                                 * get FODO from lif
                                 */
                                SHR_IF_ERR_EXIT(diag_ikleap_get_fodo_from_lif(unit, core_id, stage_name, stage_index,
                                                                              lif_handle_id, lif_dbal_table,
                                                                              lif_dbal_result_type,
                                                                              &fodo_assignement_mode, &fodo_value));
                                /*
                                 * get PD and FODO masks and calculate final FODO value
                                 */
                                SHR_IF_ERR_EXIT(diag_ikleap_fodo_pd_mask_length
                                                (unit, fodo_assignement_mode, &fodo_mask, &pd_mask));
                                fodo_vsi_value = (pd_vid & pd_mask) + (fodo_value & fodo_mask);

                                /*
                                 * extract the VSI from the calculated FODO
                                 */
                                sub_field_id = DBAL_FIELD_EMPTY;
                                SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO,
                                                                                      fodo_vsi_value, &sub_field_id,
                                                                                      &vsi_value));
                                if (sub_field_id == DBAL_FIELD_VSI)
                                {
                                    has_vsi = TRUE;
                                }

                                if (vsi_enabler && has_vsi)
                                {
                                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                                    (unit, DBAL_TABLE_ING_VSI_INFO_DB, &vsi_handle_id));
                                    dbal_entry_key_field32_set(unit, vsi_handle_id, DBAL_FIELD_VSI, vsi_value);
                                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, vsi_handle_id, DBAL_GET_ALL_FIELDS));
                                    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, vsi_handle_id, &entry_handle));

                                    sal_strncpy_s(key_signal_str[0], EMPTY, 1);
                                    sal_strncpy_s(res_signal_str[0], EMPTY, 1);
                                    key_string = &key_signal_str[0][0];
                                    res_string = &res_signal_str[0][0];

                                    SHR_IF_ERR_EXIT(diag_ikleap_vis_build_key_and_result_strings
                                                    (unit, context_hw_val, 0, entry_handle->phy_entry.key,
                                                     entry_handle->phy_entry.payload, entry_handle->phy_entry.key_size,
                                                     entry_handle->phy_entry.payload_size, key_string, res_string,
                                                     DBAL_PHYSICAL_TABLE_IVSI, DBAL_TABLE_ING_VSI_INFO_DB, core_id,
                                                     &nof_results, &res_string));

                                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                                    PRT_CELL_SET("%s(%s)", "ING_VSI_INFO_DB", "IVSI");
                                    PRT_CELL_SET("%s", &key_signal_str[0][0]);
                                    PRT_CELL_SET("%s", &res_signal_str[0][0]);
                                }
                            }
                        }
                    }
                }
                PRT_COMMITX;
            }/** of for of sub_stage */
        } /** of for of stage */
    }     /** of for on Core ID*/

exit:
    sand_signal_output_free(signal_output_key);
    sand_signal_output_free(signal_output_result);
    sand_signal_output_free(signal_output_err);
    sand_signal_output_free(signal_output_hit);
    sand_signal_output_free(signal_output_app_db);
    SHR_FREE(key_sig_value);
    SHR_FREE(key_sig_name);
    SHR_FREE(res_sig_name);
    SHR_FREE(app_sig_name);
    SHR_FREE(hit_sig_name);
    SHR_FREE(err_sig_name);
    SHR_FREE(error_sig_name);
    SHR_FREE(temp_key_sig_value);
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

/**
 * \brief DNX ING KLEAP diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for KLEAP diagnostic commands
 */

sh_sand_man_t sh_dnx_ikleap_info_man = {
#if 1
    .brief = "KLEAP Diagnostic - Show contexts configurations per stage",
#endif
    .full = NULL,
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};

sh_sand_man_t sh_dnx_ikleap_man = {
    .brief = "iKLEAP Diagnostic - Show lookups information for Ingress PP pipeline",
    .full = NULL
};
/* *INDENT-ON* */
