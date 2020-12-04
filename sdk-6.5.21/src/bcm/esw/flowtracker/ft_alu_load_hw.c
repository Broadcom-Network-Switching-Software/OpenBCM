/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      ft_alu_load_hw.c
 * Purpose:   The purpose of this file is to set flow tracker hardware methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_drop.h>

#ifdef BCM_FLOWTRACKER_SUPPORT

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)

/* BS2.0 in HR4 resource allocation encodings */
#define BCMI_FTv2_STAGE_IFT                  0
#define BCMI_FTv2_STAGE_MFT                  1
#define BCMI_FTv2_STAGE_EFT                  2

/* No of BS2.0 resources shared across FT */
#define BCMI_FTv2_RESOURCE_NUM               6

/* Shared BS2.0 resources */
#define BCMI_FTv2_RESOURCE_SESSION_BANK      0
#define BCMI_FTv2_RESOURCE_ALU16             1
#define BCMI_FTv2_RESOURCE_ALU32             2
#define BCMI_FTv2_RESOURCE_LOAD8             3
#define BCMI_FTv2_RESOURCE_LOAD16            4
#define BCMI_FTv2_RESOURCE_TIMESTAMP_ENGINE  5

#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)

/* Number of banks for Aggregate Session Data */
#define BCMI_FTv3_AGG_SESSION_DATA_BANKS     4

/* Number of aggregate ALU32 per session data bank */
#define BCMI_FTv3_AGG_ALU32_PER_BANK         4

/* Hardware Encodings for Aggregate State */
#define BCMI_FTv3_STAGE_AIFT                 0
#define BCMI_FTv3_STAGE_AMFT                 1
#define BCMI_FTv3_STAGE_AEFT                 2

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

/*
 * 1. Allocate resource for LOAD/ALU based on element.
 * 2. Add the ALU information into the list group sw state.
 * 3. when the entry is installed then add the entry into hw.
 * 4. Set all the checks in hardware.
*/

/*|************************************************************|
  |********         ALU Software Install Methods.      ********|
  |************************************************************|
  |*******                 Start                       ********|
  |************************************************************|*/
/* Alu/Load vectors. */
bcmi_ft_alu_load_method_t alu32_methods[] =
  {{bcmi_ft_alu_entry_match,
    bcmi_ft_alu32_entry_install,
    bcmi_ft_alu32_entry_uninstall}};

/* Alu/Load vectors. */
bcmi_ft_alu_load_method_t alu16_methods[] =
  {{bcmi_ft_alu_entry_match,
    bcmi_ft_alu16_entry_install,
    bcmi_ft_alu16_entry_uninstall}};

/* Alu/Load vectors. */
bcmi_ft_alu_load_method_t load16_methods[] =
  {{bcmi_ft_load16_entry_match,
    bcmi_ft_load16_entry_install,
    bcmi_ft_load16_entry_uninstall}};

/* Alu/Load vectors. */
bcmi_ft_alu_load_method_t load8_methods[] =
  {{bcmi_ft_load8_entry_match,
    bcmi_ft_load8_entry_install,
    bcmi_ft_load8_entry_uninstall}};

/* ALU/LOAD hw memory databases. */
bcmi_ft_group_pdd_bus_info_t bcmi_load8_pdd_info[TOTAL_GROUP_LOAD8_DATA_NUM] =
    {{BSD_FLEX_LOAD8_TRACKING_PARAM_0f, 71, 64, 1},
     {BSD_FLEX_LOAD8_TRACKING_PARAM_1f, 63, 56, 1},
     {BSD_FLEX_LOAD8_TRACKING_PARAM_2f, 55, 48, 1},
     {BSD_FLEX_LOAD8_TRACKING_PARAM_3f, 47, 40, 1},
     {BSD_FLEX_LOAD8_TRACKING_PARAM_4f, 39, 32, 1},
     {BSD_FLEX_LOAD8_TRACKING_PARAM_5f, 31, 24, 1},
     {BSD_FLEX_LOAD8_TRACKING_PARAM_6f, 23, 16, 1},
     {BSD_FLEX_LOAD8_TRACKING_PARAM_7f, 15, 8, 1}};

bcmi_ft_group_pdd_bus_info_t bcmi_alu16_pdd_info[TOTAL_GROUP_ALU16_MEM] =
    {{BSD_FLEX_ALU16_TRACKING_PARAM_0f, 215, 200, 2},
     {BSD_FLEX_ALU16_TRACKING_PARAM_1f, 199, 184, 2},
     {BSD_FLEX_ALU16_TRACKING_PARAM_2f, 183, 168, 2},
     {BSD_FLEX_ALU16_TRACKING_PARAM_3f, 167, 152, 2},
     {BSD_FLEX_ALU16_TRACKING_PARAM_4f, 151, 136, 2},
     {BSD_FLEX_ALU16_TRACKING_PARAM_5f, 135, 120, 2},
     {BSD_FLEX_ALU16_TRACKING_PARAM_6f, 119, 104, 2},
     {BSD_FLEX_ALU16_TRACKING_PARAM_7f, 103, 88, 2}};

bcmi_ft_group_pdd_bus_info_t bcmi_load16_pdd_info[TOTAL_GROUP_LOAD16_DATA_NUM] =
    {{BSD_FLEX_LOAD16_TRACKING_PARAM_0f, 471, 456, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_1f, 455, 440, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_2f, 439, 424, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_3f, 423, 408, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_4f, 407, 392, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_5f, 391, 376, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_6f, 375, 360, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_7f, 359, 344, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_8f, 343, 328, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_9f, 327, 312, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_10f, 311, 296, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_11f, 295, 280, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_12f, 279, 264, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_13f, 263, 248, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_14f, 247, 232, 2},
     {BSD_FLEX_LOAD16_TRACKING_PARAM_15f, 231, 216, 2}};

bcmi_ft_group_pdd_bus_info_t bcmi_alu32_pdd_info[TOTAL_GROUP_ALU32_MEM] =
    {{BSD_FLEX_ALU32_TRACKING_PARAM_0f, 855, 824, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_1f, 823, 792, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_2f, 791, 760, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_3f, 759, 728, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_4f, 727, 696, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_5f, 695, 664, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_6f, 663, 632, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_7f, 631, 600, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_8f, 599, 568, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_9f, 567, 536, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_10f, 535, 504, 4},
     {BSD_FLEX_ALU32_TRACKING_PARAM_11f, 503, 472, 4}};

alu_load_info_t bcmi_alu32_mem_info[TOTAL_GROUP_ALU32_MEM] =
    {{BSC_DG_GROUP_ALU32_PROFILE_0m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_1m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_2m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_3m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_4m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_5m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_6m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_7m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_8m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_9m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_10m, -1, NULL},
     {BSC_DG_GROUP_ALU32_PROFILE_11m, -1, NULL}};

alu_load_info_t bcmi_alu16_mem_info[TOTAL_GROUP_ALU16_MEM] =
    {{BSC_DG_GROUP_ALU16_PROFILE_0m, -1, NULL},
     {BSC_DG_GROUP_ALU16_PROFILE_1m, -1, NULL},
     {BSC_DG_GROUP_ALU16_PROFILE_2m, -1, NULL},
     {BSC_DG_GROUP_ALU16_PROFILE_3m, -1, NULL},
     {BSC_DG_GROUP_ALU16_PROFILE_4m, -1, NULL},
     {BSC_DG_GROUP_ALU16_PROFILE_5m, -1, NULL},
     {BSC_DG_GROUP_ALU16_PROFILE_6m, -1, NULL},
     {BSC_DG_GROUP_ALU16_PROFILE_7m, -1, NULL}};

alu_load_info_t bcmi_load8_mem_info[TOTAL_GROUP_LOAD8_MEM] =
     {{BSC_DG_GROUP_LOAD8_PROFILEm, -1, NULL}};

alu_load_info_t bcmi_load16_mem_info[TOTAL_GROUP_LOAD16_MEM] =
     {{BSC_DG_GROUP_LOAD16_PROFILEm, -1, NULL}};

uint32 bcmi_dg_group_alu32_fid[TOTAL_GROUP_ALU32_MEM] =
    {GROUP_ALU32_PROFILE_0_IDXf,
     GROUP_ALU32_PROFILE_1_IDXf,
     GROUP_ALU32_PROFILE_2_IDXf,
     GROUP_ALU32_PROFILE_3_IDXf,
     GROUP_ALU32_PROFILE_4_IDXf,
     GROUP_ALU32_PROFILE_5_IDXf,
     GROUP_ALU32_PROFILE_6_IDXf,
     GROUP_ALU32_PROFILE_7_IDXf,
     GROUP_ALU32_PROFILE_8_IDXf,
     GROUP_ALU32_PROFILE_9_IDXf,
     GROUP_ALU32_PROFILE_10_IDXf,
     GROUP_ALU32_PROFILE_11_IDXf};

uint32 bcmi_dg_group_alu16_fid[TOTAL_GROUP_ALU16_MEM] =
    {GROUP_ALU16_PROFILE_0_IDXf,
     GROUP_ALU16_PROFILE_1_IDXf,
     GROUP_ALU16_PROFILE_2_IDXf,
     GROUP_ALU16_PROFILE_3_IDXf,
     GROUP_ALU16_PROFILE_4_IDXf,
     GROUP_ALU16_PROFILE_5_IDXf,
     GROUP_ALU16_PROFILE_6_IDXf,
     GROUP_ALU16_PROFILE_7_IDXf};

uint32 bcmi_dg_group_load8_fid[TOTAL_GROUP_LOAD8_MEM] =
    {GROUP_LOAD8_PROFILE_IDXf};

uint32 bcmi_dg_group_load16_fid[TOTAL_GROUP_LOAD16_MEM] =
    {GROUP_LOAD16_PROFILE_IDXf};

/* Debug Counters information. */
uint32 bcmi_ft_alu32_debug_counter[TOTAL_GROUP_ALU32_MEM] =
    {BSC_DG_ALU32_0_EVENT_COUNTERm,
     BSC_DG_ALU32_1_EVENT_COUNTERm,
     BSC_DG_ALU32_2_EVENT_COUNTERm,
     BSC_DG_ALU32_3_EVENT_COUNTERm,
     BSC_DG_ALU32_4_EVENT_COUNTERm,
     BSC_DG_ALU32_5_EVENT_COUNTERm,
     BSC_DG_ALU32_6_EVENT_COUNTERm,
     BSC_DG_ALU32_7_EVENT_COUNTERm,
     BSC_DG_ALU32_8_EVENT_COUNTERm,
     BSC_DG_ALU32_9_EVENT_COUNTERm,
     BSC_DG_ALU32_10_EVENT_COUNTERm,
     BSC_DG_ALU32_11_EVENT_COUNTERm};

uint32 bcmi_ft_alu16_debug_counter[TOTAL_GROUP_ALU16_MEM] =
    {BSC_DG_ALU16_0_EVENT_COUNTERm,
     BSC_DG_ALU16_1_EVENT_COUNTERm,
     BSC_DG_ALU16_2_EVENT_COUNTERm,
     BSC_DG_ALU16_3_EVENT_COUNTERm,
     BSC_DG_ALU16_4_EVENT_COUNTERm,
     BSC_DG_ALU16_5_EVENT_COUNTERm,
     BSC_DG_ALU16_6_EVENT_COUNTERm,
     BSC_DG_ALU16_7_EVENT_COUNTERm};

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
alu_load_info_t bcmi_session_data_bank_info[TOTAL_GROUP_SESSION_DATA_BANK_MEM] =
    {{BSC_DT_FLEX_SESSION_DATA_DOUBLEm, -1, NULL}};
alu_load_info_t bcmi_timestamp_engine_info[TOTAL_GROUP_TIMESTAMP_ENGINE_MEM] =
    {{BSC_DG_GROUP_TIMESTAMP_PROFILEm, -1, NULL}};
#endif

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
alu_load_info_t
bcmi_agg_session_data_bank_info[TOTAL_GROUP_AGG_SESSION_DATA_BANK_MEM] =
{{BSC_DT_FLEX_AFT_SESSION_DATA_BANK_0m, -1, NULL},
    {BSC_DT_FLEX_AFT_SESSION_DATA_BANK_1m, -1, NULL},
    {BSC_DT_FLEX_AFT_SESSION_DATA_BANK_2m, -1, NULL},
    {BSC_DT_FLEX_AFT_SESSION_DATA_BANK_3m, -1, NULL}};

alu_load_info_t bcmi_agg_alu32_mem_info[TOTAL_GROUP_AGG_ALU32_MEM] =
    {{BSC_DG_AGG_GROUP_ALU32_PROFILE_0m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_1m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_2m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_3m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_4m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_5m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_6m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_7m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_8m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_9m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_10m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_11m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_12m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_13m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_14m, -1, NULL},
     {BSC_DG_AGG_GROUP_ALU32_PROFILE_15m, -1, NULL}};

uint32 bcmi_group_agg_alu32_fid[TOTAL_GROUP_AGG_ALU32_MEM] =
    {AGG_GROUP_ALU32_PROFILE_0_IDXf,
     AGG_GROUP_ALU32_PROFILE_1_IDXf,
     AGG_GROUP_ALU32_PROFILE_2_IDXf,
     AGG_GROUP_ALU32_PROFILE_3_IDXf,
     AGG_GROUP_ALU32_PROFILE_4_IDXf,
     AGG_GROUP_ALU32_PROFILE_5_IDXf,
     AGG_GROUP_ALU32_PROFILE_6_IDXf,
     AGG_GROUP_ALU32_PROFILE_7_IDXf,
     AGG_GROUP_ALU32_PROFILE_8_IDXf,
     AGG_GROUP_ALU32_PROFILE_9_IDXf,
     AGG_GROUP_ALU32_PROFILE_10_IDXf,
     AGG_GROUP_ALU32_PROFILE_11_IDXf,
     AGG_GROUP_ALU32_PROFILE_12_IDXf,
     AGG_GROUP_ALU32_PROFILE_13_IDXf,
     AGG_GROUP_ALU32_PROFILE_14_IDXf,
     AGG_GROUP_ALU32_PROFILE_15_IDXf};

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

/* ALU/LOAD vectors. */
bcmi_ft_alu_load_method_t
    *alu_load_methods[BCMI_FT_ALU_LOAD_MEMS];

/* Global structures for resource tracking. */
alu_load_index_info_t
    alu_load_index_info[BCM_MAX_NUM_UNITS][BCMI_FT_ALU_LOAD_MEMS];
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
alu_load_info_t
    *session_data_bank_info[BCM_MAX_NUM_UNITS];
alu_load_info_t
    *timestamp_engine_info[BCM_MAX_NUM_UNITS];
bcmi_ft_resource_sw_info_t
    bcmi_ft_resource_sw_state[BCM_MAX_NUM_UNITS];
STATIC int
bcmi_ftv2_alu_kmap_opr_result_get(int unit,
                bcmi_flowtracker_flowchecker_info_t *fc_info,
                int *opr_result);
#endif

/* Static Function prototypes */
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
alu_load_info_t
    *agg_session_data_bank_info[BCM_MAX_NUM_UNITS];

STATIC int
bcmi_ftv3_flow_transition_trigger_update(int unit,
                bcmi_ft_alu_load_type_t alu_load_type,
                int index, bcmi_ft_group_alu_info_t *info,
                int set);
#endif

extern int bcmi_alu_action_conversion[9];
/*
 * Function:
 *   bcmi_ft_load_pdd_entry_set
 * Purpose:
 *   Set load PDD actions.
 * Parameters:
 *   unit      - (IN) BCM device id
 *   id        - (IN) FT group id.
 *   load_type - (IN) Load memory type.
 *   index     - (IN) memory index.
 *   clear     - (IN) set/reset
 * Returns:
 *   NONE
 */
STATIC int
bcmi_ft_load_pdd_entry_set(int unit,
                           bcm_flowtracker_group_t id,
                           bcmi_ft_alu_load_type_t load_type,
                           int index, int clear)
{
    uint32 ctrlfmt[1];
    uint32 trigger = 0;
    soc_mem_t mem, mem1;
    uint32 *entry = NULL;
    int i = 0, fmt = 0, end_index = 0;
    bsc_dg_group_load8_profile_entry_t load8_entry;
    bsc_dg_group_load16_profile_entry_t load16_entry;
    int data[TOTAL_GROUP_LOAD16_DATA_NUM] =
                   {DATA_0f,
                    DATA_1f,
                    DATA_2f,
                    DATA_3f,
                    DATA_4f,
                    DATA_5f,
                    DATA_6f,
                    DATA_7f,
                    DATA_8f,
                    DATA_9f,
                    DATA_10f,
                    DATA_11f,
                    DATA_12f,
                    DATA_13f,
                    DATA_14f,
                    DATA_15f};

    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));
    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));

    if (load_type == bcmiFtAluLoadTypeLoad16) {
        mem = BSC_DG_GROUP_LOAD16_PROFILEm;
        entry = (uint32 *) (&load16_entry);
        fmt = BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt;
        end_index = TOTAL_GROUP_LOAD16_DATA_NUM;
    } else {
        mem = BSC_DG_GROUP_LOAD8_PROFILEm;
        entry = (uint32 *) (&load8_entry);
        fmt = BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt;
        end_index = TOTAL_GROUP_LOAD8_DATA_NUM;
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry));

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem1);

    for (i = 0; i < end_index; i++) {

        soc_mem_field_get(unit, mem, entry, data[i], ctrlfmt);

        trigger = soc_format_field32_get(unit, fmt, ctrlfmt, LOAD_TRIGGERf);

        if (trigger && (!clear)) {
            /* set pdd profile entry for this load entry */
            soc_mem_field32_set(unit, mem1,
                (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                (BCMI_FT_ALU_LOAD_PDD_INFO(unit, load_type)[i]).param_id, 1);
        } else {
            /* clear pdd profile entry for this load entry */
            soc_mem_field32_set(unit, mem1,
                (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                (BCMI_FT_ALU_LOAD_PDD_INFO(unit, load_type)[i]).param_id, 0);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_element_alu_action_get
 * Purpose:
 *   Get ALU actions.
 * Parameters:
 *   uint    - (IN) BCM device number.
 *   id      - (IN) Flowtracker Group
 *   element - (IN) FT element type.
 *   action  - (OUT) Action for element.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_element_alu_action_get(int unit,
                               bcm_flowtracker_group_t id,
                               bcm_flowtracker_tracking_param_type_t element,
                               bcm_flowtracker_check_action_t *action)
{
    bcm_flowtracker_check_action_t alu_action;

    switch(element) {
        case bcmFlowtrackerTrackingParamTypePktCount:
            alu_action = bcmFlowtrackerCheckActionCounterIncr;
            break;
        case bcmFlowtrackerTrackingParamTypeByteCount:
            alu_action = bcmFlowtrackerCheckActionCounterIncrByPktBytes;
            break;
        case bcmFlowtrackerTrackingParamTypeDosAttack:
        case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
            if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
                alu_action = bcmFlowtrackerCheckActionUpdateValue;
            } else {
                alu_action = bcmFlowtrackerCheckActionCounterIncr;
            }
            break;
        default:
            alu_action = bcmFlowtrackerCheckActionUpdateValue;
            break;
    }

    if (action != NULL) {
        *action = alu_action;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_get_alu32_mem
 * Purpose:
 *   Get alu32 enable memory banks.
 * Parameters:
 *   unit - (IN) BCM device id.
 * Returns:
 *   enabled memory banks
 */
STATIC int
bcmi_ft_get_alu32_mem(int unit)
{
    if (soc_feature(unit, soc_feature_flowtracker_ver_1_alu32_0_1_banks)) {
        /* Internally SDK will only pick first 2. */
        return 2;
    }

    return TOTAL_GROUP_ALU32_MEM;
}

/*
 * Function:
 *   bcmi_ft_cleanup_mem_resource_tracking
 * Purpose:
 *   Allocate resource for FT Type Tracking.
 * Parameters:
 *   unit - (IN) BCM device id.
 * Returns:
 */
STATIC void
bcmi_ft_cleanup_mem_resource_tracking(int unit)
{
    uint32 mem_idx = 0;
    alu_load_info_t *res_tmp = NULL;

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

        /* Cleanup ft type tracking for aggregate session data banks */
        if (BCMI_FT_AGG_SESSION_DATA_BANK_INFO(unit)) {
            for (mem_idx = 0;
                    mem_idx < TOTAL_GROUP_AGG_SESSION_DATA_BANK_MEM;
                    mem_idx++) {
                res_tmp = &BCMI_FT_AGG_SESSION_DATA_BANK_INFO(unit)[mem_idx];
                if ((res_tmp != NULL) && (res_tmp->resource_ft_type != NULL)) {
                    sal_free(res_tmp->resource_ft_type);
                    res_tmp->resource_ft_type = NULL;
                }
            }
        }

        /* Cleanup ft type tracking for AGG ALU32 resources */
        res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAggAlu32);
        if (res_tmp != NULL) {
            for (mem_idx = 0;
                    mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, \
                        bcmiFtAluLoadTypeAggAlu32); mem_idx++) {
                if (res_tmp->resource_ft_type != NULL) {
                    sal_free(res_tmp->resource_ft_type);
                    res_tmp->resource_ft_type = NULL;
                }
                res_tmp++;
            }
        }
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

        /* Cleanup ft type tracking for session data banks */
        res_tmp = BCMI_FT_SESSION_DATA_BANK_INFO(unit);
        if ((res_tmp != NULL) && (res_tmp->resource_ft_type != NULL)) {
            sal_free(res_tmp->resource_ft_type);
            res_tmp->resource_ft_type = NULL;
        }

        /* Cleanup ft type tracking for timestamp engine resources */
        res_tmp = BCMI_FT_TIMESTAMP_ENGINE_INFO(unit);
        if ((res_tmp != NULL) && (res_tmp->resource_ft_type != NULL)) {
            sal_free(res_tmp->resource_ft_type);
            res_tmp->resource_ft_type = NULL;
        }
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    /* Cleanup ft type tracking for ALU32 resources */
    res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAlu32);
    if (res_tmp != NULL) {
        for (mem_idx = 0;
             mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32);
             mem_idx++) {
            if (res_tmp->resource_ft_type != NULL) {
                sal_free(res_tmp->resource_ft_type);
                res_tmp->resource_ft_type = NULL;
            }
            res_tmp++;
        }
    }

    /* Cleanup ft type tracking for LOAD16 resources */
    res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeLoad16);
    if (res_tmp != NULL) {
        for (mem_idx = 0;
             mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit,bcmiFtAluLoadTypeLoad16);
             mem_idx++) {
            if (res_tmp->resource_ft_type != NULL) {
                sal_free(res_tmp->resource_ft_type);
                res_tmp->resource_ft_type = NULL;
            }
            res_tmp++;
        }
    }

    /* Cleanup ft type tracking for ALU16 resources */
    res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAlu16);
    if (res_tmp != NULL) {
        for (mem_idx = 0;
             mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16);
             mem_idx++) {
            if (res_tmp->resource_ft_type != NULL) {
                sal_free(res_tmp->resource_ft_type);
                res_tmp->resource_ft_type = NULL;
            }
            res_tmp++;
        }
    }

    /* Cleanup ft type tracking for LOAD8 resources */
    res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeLoad8);
    if (res_tmp != NULL) {
        for (mem_idx = 0;
             mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeLoad8);
             mem_idx++) {
            if (res_tmp->resource_ft_type != NULL) {
                sal_free(res_tmp->resource_ft_type);
                res_tmp->resource_ft_type = NULL;
            }
            res_tmp++;
        }
    }
}

/*
 * Function:
 *   bcmi_ft_allocate_mem_resource_tracking
 * Purpose:
 *   Allocate resource for FT Type Tracking.
 * Parameters:
 *   unit - (IN) BCM device id.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 *   This api will allocate mem for ft_type in resources
 *   and initialise them by IFT stage by default
 */
STATIC int
bcmi_ft_allocate_mem_resource_tracking(int unit)
{
    int rv = BCM_E_NONE;
    int iter = 0;
    uint32 mem_idx = 0;
    alu_load_info_t *res_tmp = NULL;
    bcmi_ft_type_t *ft_type_tmp = NULL;

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

        /* Allocate ft type tracking for Session Data Banks */
        res_tmp = BCMI_FT_SESSION_DATA_BANK_INFO(unit);
        res_tmp->resource_ft_type =
                    sal_alloc((TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM *
                    sizeof(bcmi_ft_type_t)), "session data bank ft type");
        if (res_tmp->resource_ft_type == NULL) {
            goto cleanup;
        }
        ft_type_tmp = res_tmp->resource_ft_type;
        for (iter = 0;
             iter < TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM; iter++) {
            ft_type_tmp[iter] = bcmiFtTypeIFT;
        }

        /* Allocate ft type tracking for TimeStamp Engine Resources */
        res_tmp = BCMI_FT_TIMESTAMP_ENGINE_INFO(unit);
        res_tmp->resource_ft_type =
                    sal_alloc((TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM *
                    sizeof(bcmi_ft_type_t)), "timestamp engine ft type");
        if (res_tmp->resource_ft_type == NULL) {
            goto cleanup;
        }
        ft_type_tmp = res_tmp->resource_ft_type;
        for (iter = 0;
             iter < TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM; iter++) {
            ft_type_tmp[iter] = bcmiFtTypeIFT;
        }
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    /* Allocate ft type tracking for ALU32 Resources */
    res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAlu32);
    for (mem_idx = 0;
         mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32);
         mem_idx++) {
        res_tmp->resource_ft_type =
                    sal_alloc((TOTAL_GROUP_ALU32_DATA_NUM *
                    sizeof(bcmi_ft_type_t)), "ALU32 ft type");
        if (res_tmp->resource_ft_type == NULL) {
            goto cleanup;
        }
        ft_type_tmp = res_tmp->resource_ft_type;
        for (iter = 0;
             iter < TOTAL_GROUP_ALU32_DATA_NUM; iter++) {
            ft_type_tmp[iter] = bcmiFtTypeIFT;
        }
        res_tmp++;
    }

    /* Allocate ft type tracking for LOAD16 Resources */
    res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeLoad16);
    for (mem_idx = 0;
         mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeLoad16);
         mem_idx++) {
        res_tmp->resource_ft_type =
                    sal_alloc((TOTAL_GROUP_LOAD16_DATA_NUM *
                    sizeof(bcmi_ft_type_t)), "LOAD16 ft type");
        if (res_tmp->resource_ft_type == NULL) {
            goto cleanup;
        }
        ft_type_tmp = res_tmp->resource_ft_type;
        for (iter = 0;
             iter < TOTAL_GROUP_LOAD16_DATA_NUM; iter++) {
            ft_type_tmp[iter] = bcmiFtTypeIFT;
        }
        res_tmp++;
    }

    /* Allocate ft type tracking for ALU16 Resources */
    res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAlu16);
    for (mem_idx = 0;
         mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16);
         mem_idx++) {
        res_tmp->resource_ft_type =
                    sal_alloc((TOTAL_GROUP_ALU16_DATA_NUM *
                    sizeof(bcmi_ft_type_t)), "ALU16 ft type");
        if (res_tmp->resource_ft_type == NULL) {
            goto cleanup;
        }
        ft_type_tmp = res_tmp->resource_ft_type;
        for (iter = 0;
             iter < TOTAL_GROUP_ALU16_DATA_NUM; iter++) {
            ft_type_tmp[iter] = bcmiFtTypeIFT;
        }
        res_tmp++;
    }

    /* Allocate ft type tracking for LOAD8 Resources */
    res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeLoad8);
    for (mem_idx = 0;
         mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeLoad8);
         mem_idx++) {
        res_tmp->resource_ft_type =
                    sal_alloc((TOTAL_GROUP_LOAD8_DATA_NUM *
                    sizeof(bcmi_ft_type_t)), "LOAD8 ft type");
        if (res_tmp->resource_ft_type == NULL) {
            goto cleanup;
        }
        ft_type_tmp = res_tmp->resource_ft_type;
        for (iter = 0;
             iter < TOTAL_GROUP_LOAD8_DATA_NUM; iter++) {
            ft_type_tmp[iter] = bcmiFtTypeIFT;
        }
        res_tmp++;
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

        /* Allocate ft type tracking for Session Data Banks */
        for (mem_idx = 0;
                mem_idx < TOTAL_GROUP_AGG_SESSION_DATA_BANK_MEM;
                mem_idx++) {
            res_tmp = &BCMI_FT_AGG_SESSION_DATA_BANK_INFO(unit)[mem_idx];
            res_tmp->resource_ft_type = sal_alloc((sizeof(bcmi_ft_type_t)),
                        "agg session data bank ft type");
            if (res_tmp->resource_ft_type == NULL) {
                goto cleanup;
            }
            BCMI_FT_AGG_SESSION_DATA_BANK_FT_TYPE(unit, mem_idx, 0) = bcmiFtTypeAIFT;
        }

        /* Allocate ft type tracking for ALU32 Resources */
        res_tmp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAggAlu32);
        for (mem_idx = 0;
                mem_idx < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32);
                mem_idx++) {
            res_tmp->resource_ft_type =
                sal_alloc((TOTAL_GROUP_ALU32_DATA_NUM *
                            sizeof(bcmi_ft_type_t)), "ALU32 ft type");
            if (res_tmp->resource_ft_type == NULL) {
                goto cleanup;
            }
            ft_type_tmp = res_tmp->resource_ft_type;
            for (iter = 0;
                    iter < TOTAL_GROUP_ALU32_DATA_NUM; iter++) {
                ft_type_tmp[iter] = bcmiFtTypeAIFT;
            }
            res_tmp++;
        }
    }
#endif

    return rv;

cleanup:
    bcmi_ft_cleanup_mem_resource_tracking(unit);
    return BCM_E_MEMORY;
}

/*
 * Function:
 *   bcmi_ft_alu_load_init
 * Purpose:
 *   Initialize ALU and LOAD software state.
 * Parameters:
 *   unit - (IN) BCM device id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_load_init(int unit)
{
    int rv = BCM_E_NONE;
    int i = 0, j = 0;
    int total_indexes_per_mem = 0;

    /* The below should be done during group init. */
    

    /* Set ALU16 data. */
    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) =
                                            TOTAL_GROUP_ALU16_MEM;
    BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAlu16) =
                                            bcmi_alu16_mem_info;
    BCMI_FT_ALU_LOAD_PDD_INFO(unit, bcmiFtAluLoadTypeAlu16) =
                                            bcmi_alu16_pdd_info;
    BCMI_FT_ALU_LOAD_GROUP_FID_INFO(unit, bcmiFtAluLoadTypeAlu16) =
                                            bcmi_dg_group_alu16_fid;
    alu_load_methods[bcmiFtAluLoadTypeAlu16] = alu16_methods;

    /* Set ALU32 data. */
    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) =
                                            bcmi_ft_get_alu32_mem(unit);
    BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAlu32) =
                                            bcmi_alu32_mem_info;
    BCMI_FT_ALU_LOAD_PDD_INFO(unit, bcmiFtAluLoadTypeAlu32) =
                                            bcmi_alu32_pdd_info;
    BCMI_FT_ALU_LOAD_GROUP_FID_INFO(unit, bcmiFtAluLoadTypeAlu32) =
                                            bcmi_dg_group_alu32_fid;
    alu_load_methods[bcmiFtAluLoadTypeAlu32] = alu32_methods;

    /* Set LOAD8 data. */
    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeLoad8) =
                                            TOTAL_GROUP_LOAD8_MEM;
    BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeLoad8) =
                                            bcmi_load8_mem_info;
    BCMI_FT_ALU_LOAD_PDD_INFO(unit, bcmiFtAluLoadTypeLoad8) =
                                            bcmi_load8_pdd_info;
    BCMI_FT_ALU_LOAD_GROUP_FID_INFO(unit, bcmiFtAluLoadTypeLoad8) =
                                            bcmi_dg_group_load8_fid;
    alu_load_methods[bcmiFtAluLoadTypeLoad8] = load8_methods;

    /* Set LOAD16 data. */
    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeLoad16) =
                                            TOTAL_GROUP_LOAD16_MEM;
    BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeLoad16) =
                                            bcmi_load16_mem_info;
    BCMI_FT_ALU_LOAD_PDD_INFO(unit, bcmiFtAluLoadTypeLoad16) =
                                            bcmi_load16_pdd_info;
    BCMI_FT_ALU_LOAD_GROUP_FID_INFO(unit, bcmiFtAluLoadTypeLoad16) =
                                            bcmi_dg_group_load16_fid;
    alu_load_methods[bcmiFtAluLoadTypeLoad16] = load16_methods;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        /* Set Aggregate ALU32 data. */
        i = bcmiFtAluLoadTypeAggAlu32;
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, i) = TOTAL_GROUP_AGG_ALU32_MEM;
        BCMI_FT_ALU_LOAD_MEM_INFO(unit, i) = bcmi_agg_alu32_mem_info;
        BCMI_FT_ALU_LOAD_PDD_INFO(unit, i) = NULL;
        BCMI_FT_ALU_LOAD_GROUP_FID_INFO(unit, i) = bcmi_group_agg_alu32_fid;
        alu_load_methods[i] = alu32_methods;

        /* Set aggregate session data resource info */
        BCMI_FT_AGG_SESSION_DATA_BANK_INFO(unit) =
                                bcmi_agg_session_data_bank_info;
    }

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    /* Set session data and timestamp resource info */
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        BCMI_FT_SESSION_DATA_BANK_INFO(unit) =
                                bcmi_session_data_bank_info;
        BCMI_FT_TIMESTAMP_ENGINE_INFO(unit) =
                                bcmi_timestamp_engine_info;
    }
#endif

    /* Set the mem size for the ALUs. */
    for (i = bcmiFtAluLoadTypeLoad8; i < bcmiFtAluLoadTypeNone; i++) {

        total_indexes_per_mem = 0;
        for (j = 0; j < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, i); j++) {

            BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i, j) =
                soc_mem_index_count(unit,
                             BCMI_FT_ALU_LOAD_MEM(unit, i, j));

            total_indexes_per_mem +=
                BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i, j);

        }
        BCMI_FT_ALU_LOAD_MEM_TOTAL_INDEX(unit, i) = total_indexes_per_mem;

         /* lets calculate indexes or all memories of one type. */
         BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, i) =
                    sal_alloc(SHR_BITALLOCSIZE(total_indexes_per_mem),
                                                   "alu_load_bitmap");
         if (!BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, i)) {
             bcmi_ft_alu_load_cleanup(unit);
             return BCM_E_MEMORY;
         }
         sal_memset(BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, i), 0,
                            SHR_BITALLOCSIZE(total_indexes_per_mem));

         /* Now allocate the ref count memory. */
        BCMI_FT_ALU_LOAD_REFCOUNT_INFO(unit, i) =
                sal_alloc((total_indexes_per_mem * sizeof(uint32)),
                                                "Indexes Refcount");
         if (!BCMI_FT_ALU_LOAD_REFCOUNT_INFO(unit, i)) {
             bcmi_ft_alu_load_cleanup(unit);
             return BCM_E_MEMORY;
         }
         sal_memset(BCMI_FT_ALU_LOAD_REFCOUNT_INFO(unit, i), 0,
                            (total_indexes_per_mem * sizeof(uint32)));
    }

    /* Update ft_type - IFT/MFT/EFT information here based on config
     * variables set. In Helix5 - all resources are initialized to
     * IFT stage by default. Hence no need to specify config variables.
     */
    rv = bcmi_ft_allocate_mem_resource_tracking(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_ft_alu_load_cleanup(unit);
        return rv;
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

        /* allocate resources first as per config variables */
        rv = bcmi_ftv2_alu_load_mem_config_init(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_ft_alu_load_cleanup(unit);
            return rv;
        }

        /* store some session data bank related info */
        rv = bcmi_ftv2_resource_info_init(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_ft_alu_load_cleanup(unit);
            return rv;
        }
    }
#endif

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

        /* allocate resources first as per config variables */
        rv = bcmi_ftv3_alu_load_mem_config_init(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_ft_alu_load_cleanup(unit);
            return rv;
        }
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu_load_cleanup
 * Purpose:
 *   Clear ALU and LOAD software state.
 * Parameters:
 *   unit - (IN) BCM device id.
 * Returns:
 */
void
bcmi_ft_alu_load_cleanup(int unit)
{
    int i = 0, j = 0;

    /* cleanup resources allocate for FT Type Tracking */
    bcmi_ft_cleanup_mem_resource_tracking(unit);

    /* Set the mem size for the ALUs. */
    for (i = bcmiFtAluLoadTypeLoad8; i < bcmiFtAluLoadTypeNone; i++) {

        for (; j < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, i); j++) {

            BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i, j) = 0;
            BCMI_FT_ALU_LOAD_MEM_TOTAL_INDEX(unit, i) = 0;

        }

         /* lets calculate indexes or all memories of one type. */
        if (BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, i)) {
            sal_free(BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, i));
            BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, i) = NULL;
        }

         /* Now allocate the ref count memory. */
        if (BCMI_FT_ALU_LOAD_REFCOUNT_INFO(unit, i)) {
            sal_free(BCMI_FT_ALU_LOAD_REFCOUNT_INFO(unit, i));
            BCMI_FT_ALU_LOAD_REFCOUNT_INFO(unit, i) = NULL;
        }
    }

    return;
}

/*
 * Function:
 *   bcmi_ft_alu_load_pdd_mem_get
 * Purpose:
 *   Get relevant pdd memory for provided ft_type.
 * Parameters:
 *   unit    - (IN)  BCM device id
 *   ft_type - (IN)  Flowtracker type.
 *   pdd_mem - (OUT) PDD action memory.
 * Returns:
 *   -
 */
void
bcmi_ft_alu_load_pdd_mem_get(int unit,
                             bcmi_ft_type_t ft_type,
                             soc_mem_t *pdd_mem)
{
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        if (ft_type == bcmiFtTypeIFT) {
            *pdd_mem = BSD_POLICY_ACTION_PROFILE_0m;
        } else if (ft_type == bcmiFtTypeMFT) {
            *pdd_mem = BSD_POLICY_ACTION_PROFILE_1m;
        } else if (ft_type == bcmiFtTypeEFT) {
            *pdd_mem = BSD_POLICY_ACTION_PROFILE_2m;
        } else {
            *pdd_mem = BSD_POLICY_ACTION_PROFILE_0m;
        }
    } else
#endif
    {
        *pdd_mem = BSD_POLICY_ACTION_PROFILEm;
    }
}

/*
 * Function:
 *   bcmi_ft_alu_load_mem_index_get
 * Purpose:
 *   Get hw index for ALU and load memories from sw index.
 * Parameters:
 *   unit      - (IN) BCM device id
 *   index     - (IN) sw index.
 *   type      - (IN) ALU/Load memory type.
 *   new_index - (OUT) memory index.
 *   mem       - (OUT) memory.
 *   a_idx     - (OUT) array index of the memory in database.
 * Returns:
 *   NONE
 */
void
bcmi_ft_alu_load_mem_index_get(int unit,
                               int index, bcmi_ft_alu_load_type_t type,
                               int *new_index, soc_mem_t *mem, int *a_idx)
{
    alu_load_info_t *temp = NULL;
    int prev_index = 0, temp_index = 0;
    int i = 0, total_no_of_mem_entries = 0;

    /* Total number of memory for provided type */
    total_no_of_mem_entries = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, type);

    /* Retreive mem information for provided type */
    temp = BCMI_FT_ALU_LOAD_MEM_INFO(unit, type);

    /* Retreive actual idx, memory name & mem array index */
    for(i = 0; i < (total_no_of_mem_entries); i++) {

        temp_index += temp[i].mem_size;

        /* If index falls in this range then use this index. */
        if (temp_index > index) {
            if (new_index != NULL) {
                *new_index = index - prev_index;
            }
            if (mem != NULL) {
                *mem = temp[i].mem;
            }
            if (a_idx != NULL) {
                *a_idx = i;
            }
            return;
        }
        prev_index = temp_index;
    }
}

/*
 * Function:
 *   bcmi_ft_alu_hw_operation_get
 * Purpose:
 *   Get hw operations based on sw operations.
 * Parameters:
 *   unit              - (IN) BCM device id
 *   operation         - (IN) sw operation.
 *   check_0_operation - (IN) hw operation 1
 *   check_1_operation - (IN) hw operation 2
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_hw_operation_get(int unit,
                             bcm_flowtracker_check_operation_t operation,
                             int *check_0_operation,
                             int *check_1_operation)
{
    if (operation >= bcmFlowtrackerCheckOpCount) {
        return BCM_E_PARAM;
    }

    switch(operation) {

        case bcmFlowtrackerCheckOpEqual:
            *check_0_operation = 2;
            break;
        case bcmFlowtrackerCheckOpNotEqual:
            *check_0_operation = 3;
            break;
        case bcmFlowtrackerCheckOpGreater:
            *check_0_operation = 0;
            break;
        case bcmFlowtrackerCheckOpGreaterEqual:
            *check_0_operation = 1;
            break;
        case bcmFlowtrackerCheckOpSmaller:
            *check_1_operation = 4;
            break;
        case bcmFlowtrackerCheckOpSmallerEqual:
            *check_1_operation = 5;
            break;
        case bcmFlowtrackerCheckOpPass:
            *check_0_operation = 6;
            break;
        case bcmFlowtrackerCheckOpFail:
        case bcmFlowtrackerCheckOpNone:
            *check_0_operation = 7;
            *check_1_operation = 7;
            break;
        case bcmFlowtrackerCheckOpInRange:
            *check_0_operation = 1;
            *check_1_operation = 5;
            break;
        case bcmFlowtrackerCheckOpOutofRange:
            *check_0_operation = 5;
            *check_1_operation = 1;
            break;
        case bcmFlowtrackerCheckOpMask:
            *check_1_operation = 8;
            break;

        default :
            *check_0_operation = 6;
            *check_1_operation = 6;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flowchecker_opr_is_dual
 * Purpose:
 *   Check if hw operations is using dual actions.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   operation  - (IN) sw operation.
 * Returns:
 *   TRUE/FALSE
 */
int
bcmi_ft_flowchecker_opr_is_dual(int unit,
                                bcm_flowtracker_check_operation_t operation)
{
    switch(operation) {

        case bcmFlowtrackerCheckOpInRange:
        case bcmFlowtrackerCheckOpOutofRange:
            return 1;
        break;

        default:
            return 0;
    }

    return 0;
}

/*
 * Function:
 *   bcmi_ft_flowchecker_data_is_valid
 * Purpose:
 *   Check if flowchecker data is valid
 * Parameters:
 *   unit      - (IN) BCM device id
 *   fc_info   - (IN) Flowchecker information.
 * Returns:
 *   TRUE/FALSE
 */
int
bcmi_ft_flowchecker_data_is_valid(int unit,
                                  bcmi_flowtracker_flowchecker_info_t *fc_info)
{
    if (bcmi_ft_flowchecker_opr_is_dual(unit, fc_info->check1.operation)) {
        /*
         * If check1 is already using both hw operation
         * then check 2 should be empty.
         */
        if (fc_info->check2.param != bcmFlowtrackerTrackingParamTypeNone) {
            return 0;
        }
    }

    return 1;
}

/*
 * Function:
 *   bcmi_ft_flowchecker_values_get
 * Purpose:
 *   Check if flowchecker data is valid
 * Parameters:
 *   unit     - (IN) BCM device id
 *   fc_info  - (IN) Flowchecker information.
 *   val1     - (OUT) value for check 1.
 *   val2     - (OUT) value for check 2.
 *   opr1     - (OUT) operation for check 1.
 *   opr2     - (OUT) operation for check 2.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_flowchecker_values_get(int unit,
                               bcmi_flowtracker_flowchecker_info_t *fc_info,
                               int *val1, int *val2, int *opr1, int *opr2)
{
    int l_opr1 = -1, l_opr2 = -1;

    *val1 = *val2 = *opr1 = *opr2 = -1;

    /* sanity check first */
    if (!bcmi_ft_flowchecker_data_is_valid(unit, fc_info)) {
        return BCM_E_PARAM;
    }

    if (fc_info->check1.param) {

        /* get check1 operation and value first */
        BCM_IF_ERROR_RETURN
            (bcmi_ft_alu_hw_operation_get(unit,
                    fc_info->check1.operation, &l_opr1, &l_opr2));

        *opr1 = ((l_opr1 == -1) ? l_opr2 : l_opr1);
        *val1 = ((l_opr1 == -1) ? fc_info->check1.max_value :
                                    fc_info->check1.min_value);

        if (l_opr1 != -1) {
            /*
             * if both operations are valid on first check,
             * then it is range operation which has two opr and val.
             */
            *opr2 = ((l_opr2 != -1) ? l_opr2 : -1);
            *val2 = fc_info->check1.max_value;
        }

        if (soc_feature(unit, soc_feature_hx5_ft_32bit_param_update)) {

            /* Special Processing for Dos Attacks. */
            if (((fc_info->check1.param ==
                    bcmFlowtrackerTrackingParamTypeDosAttack) ||
                 (fc_info->check1.param ==
                    bcmFlowtrackerTrackingParamTypeInnerDosAttack)) &&
                 (fc_info->action_info.action ==
                    bcmFlowtrackerCheckActionUpdateValue)) {

                /* Mark both of the alu entries to make a combined test. */
                if (fc_info->check1.min_value) {
                   *val1 = fc_info->check1.min_value & 0xffff;
                   *val2 =  (fc_info->check1.min_value >> 16)  & 0xffff;
                   *opr2 = *opr1;
                } else {
                   *val1 = fc_info->check1.max_value & 0xffff;
                   *val2 =  (fc_info->check1.max_value >> 16)  & 0xffff;
                   *opr1 = *opr2;
                }
           }
        }

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        /* For ingress/egress drop vector value comes from
         * drop reason group id provided with checkers */
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            if ((fc_info->check1.param ==
                  bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector) ||
                (fc_info->check1.param ==
                  bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector)) {

                    BCM_IF_ERROR_RETURN
                        (bcmi_ftv2_drop_reason_group_vector_get(unit, fc_info,
                                                                  val1, val2));
            }
        }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
    } else {

        if (fc_info->check1.operation != bcmFlowtrackerCheckOpNone) {
            /* get check1 operation and value first */
            BCM_IF_ERROR_RETURN(bcmi_ft_alu_hw_operation_get(unit,
                        fc_info->check1.operation, &l_opr1, &l_opr2));

            *opr1 = ((l_opr1 == -1) ? l_opr2 : l_opr1);
            *val1 = 0;
        }
    }

    l_opr1 = l_opr2 = -1;

    if (fc_info->check2.param) {
        BCM_IF_ERROR_RETURN
            (bcmi_ft_alu_hw_operation_get(unit,
                            fc_info->check2.operation, &l_opr1, &l_opr2));

        *opr2 = ((l_opr1 == -1) ? l_opr2 : l_opr1);
        *val2 = ((l_opr1 == -1) ? fc_info->check2.max_value :
                                    fc_info->check2.min_value);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        /* For ingress/egress drop vector value comes from
         * drop reason group id provided with checkers */
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            if ((fc_info->check2.param ==
                  bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector) ||
                (fc_info->check2.param ==
                  bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector)) {

                    BCM_IF_ERROR_RETURN
                        (bcmi_ftv2_drop_reason_group_vector_get(unit, fc_info,
                                                                  NULL, val2));
            }
        }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
    } else {
        if (fc_info->check2.operation != bcmFlowtrackerCheckOpNone) {
            /* get check2 operation and value first */
            BCM_IF_ERROR_RETURN(bcmi_ft_alu_hw_operation_get(unit,
                        fc_info->check2.operation, &l_opr1, &l_opr2));

            *opr2 = ((l_opr1 == -1) ? l_opr2 : l_opr1);
            *val2 = 0;
        }
    }

    return BCM_E_NONE;
}


int
bcmi_ft_alu_check_kmap_data_get(int unit,
                bcmi_flowtracker_flowchecker_info_t *fc_info,
                int check_0_opr,
                int check_1_opr,
                uint32 *kmap_val,
                uint32 *bit_sel_array)
{
    int temp = 0, iter = 0;
    int dos_attack_update = FALSE;
    int ts_flowend_update = FALSE;
    int results = 0;
    int bit = 0, bit_select = FALSE;
    int final_output_arr[3][8];
    int opr_output_arr[8];
    int alu_data_kmap_bit_select[2];
    bcm_flowtracker_check_operation_t check_operation;

    sal_memset(opr_output_arr, 0, 8 * sizeof(int));
    sal_memset(bit_sel_array, 0, 4 * sizeof(uint32));

    opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = -1;
    opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_CHECK1_RES] = -1;
    opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS0] = -1;
    opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS1] = -1;
    opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS2] = -1;
    opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS3] = -1;
    opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS4] = -1;
    opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_STATE_TRIG] = -1;

    /* If either of the operations is FAIL, Output will be fail */
    if ((check_0_opr == 7) || (check_1_opr == 7)) {
        return BCM_E_NONE;
    }

    /* Get Check 0 Truth value */
    if (check_0_opr != -1) {
        opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 1;
    }

    /* Get Check 1 Truth value */
    if (check_1_opr != -1) {
        opr_output_arr[BCMI_FT_ALU_KMAP_BIT_SEL_CHECK1_RES] = 1;
    }

    /* Get rest of bits Truth value */
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        BCM_IF_ERROR_RETURN(
                bcmi_ftv2_alu_kmap_opr_result_get(unit, fc_info, opr_output_arr));
    }
#endif

    if (fc_info != NULL) {
        if (soc_feature(unit, soc_feature_hx5_ft_32bit_param_update)) {

            /* Check if Dos Attack Update operation */
            if (((fc_info->check1.param ==
                            bcmFlowtrackerTrackingParamTypeDosAttack) ||
                        (fc_info->check1.param ==
                         bcmFlowtrackerTrackingParamTypeInnerDosAttack)) &&
                    (fc_info->check1.operation == bcmFlowtrackerCheckOpNotEqual) &&
                    (fc_info->action_info.action ==
                     bcmFlowtrackerCheckActionUpdateValue)) {
                dos_attack_update =TRUE;
            }
        }

        if (fc_info->check1.param ==
                bcmFlowtrackerTrackingParamTypeTimestampFlowEnd) {
            /* Check if Timestamp Flow End */
            /* Need to add case for FIN || RST */
            for (bit = 0, bit_select = BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS0;
                    bit_select <= BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS4;
                    bit_select++) {

                if (opr_output_arr[bit_select] != -1) {
                    alu_data_kmap_bit_select[bit] = bit_select;
                    bit++;
                }
            }
            ts_flowend_update = TRUE;
        }
        check_operation = fc_info->check1.operation;

    } else {
        check_operation = bcmFlowtrackerCheckOpNone;
    }

    /* Select input bits for KMAP */
    bit_select = 0;
    for (bit = 0; bit < 8; bit++) {
        if (opr_output_arr[bit] != -1) {
            if (bit_select == 0) {
                bit_sel_array[0] = bit_sel_array[1] = bit;
                bit_sel_array[2] = bit_sel_array[3] = bit;
            } else {
                bit_sel_array[bit_select] = bit;
            }
            bit_select++;

            if (bit_select > 4) {
                return BCM_E_RESOURCE;
            }
        }
        /* Default result combination */
        final_output_arr[0][bit] = opr_output_arr[bit];
        final_output_arr[1][bit] = opr_output_arr[bit];
        final_output_arr[2][bit] = opr_output_arr[bit];
    }

    /* Get Result Output combintations */
    if (ts_flowend_update == TRUE) {
        results = 3;
        final_output_arr[0][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 1;
        final_output_arr[0][alu_data_kmap_bit_select[0]] = 1;
        final_output_arr[0][alu_data_kmap_bit_select[1]] = 1;

        final_output_arr[0][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 1;
        final_output_arr[1][alu_data_kmap_bit_select[0]] = 0;
        final_output_arr[2][alu_data_kmap_bit_select[1]] = 1;

        final_output_arr[0][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 1;
        final_output_arr[2][alu_data_kmap_bit_select[0]] = 1;
        final_output_arr[2][alu_data_kmap_bit_select[1]] = 0;

    } else if (dos_attack_update == TRUE) {
        results = 3;
        final_output_arr[0][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 1;
        final_output_arr[0][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK1_RES] = 1;

        final_output_arr[1][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 1;
        final_output_arr[1][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK1_RES] = 0;

        final_output_arr[2][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 0;
        final_output_arr[2][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK1_RES] = 1;

    } else if (check_operation == bcmFlowtrackerCheckOpOutofRange) {
        results = 2;
        final_output_arr[0][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 1;
        final_output_arr[0][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK1_RES] = 0;

        final_output_arr[1][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES] = 0;
        final_output_arr[1][BCMI_FT_ALU_KMAP_BIT_SEL_CHECK1_RES] = 1;
    } else {
        /* final_output_arr is populated */
        results = 1;
    }

    *kmap_val = 0;
    for (iter = 0; iter < results; iter++) {
        temp = 0;
        for (bit = 0; bit < 4; bit++) {
            bit_select = bit_sel_array[bit];
            if (final_output_arr[iter][bit_select] != -1) {
                temp |= final_output_arr[iter][bit_select] << bit;
            }
        }
        if (temp) {
            *kmap_val |= (1 << temp);
        }
    }

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "KMAP: bit0=%d"
                    " bit1=%d bit2=%d bit3=%d KMAP_val=0x%x\n"),
                bit_sel_array[0], bit_sel_array[1],
                bit_sel_array[2], bit_sel_array[3], *kmap_val));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu_hw_action_get
 * Purpose:
 *   Get ALU hardware actions.
 * Parameters:
 *   unit       - (IN) BCM device idd.
 *   alu_info   - (IN) ALU info
 *   action     - (OUT) software Action for element.
 *   hw_action  - (OUT) Action for element.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_hw_action_get(int unit,
                          bcmi_ft_group_alu_info_t *alu_info,
                          bcm_flowtracker_check_action_t action,
                          int *hw_action)
{
    bcm_flowtracker_tracking_param_type_t param;

    if (action >=  bcmFlowtrackerCheckActionCount) {
        return BCM_E_PARAM;
    }

    if (alu_info->action_element_type) {
        param = alu_info->action_element_type;
    } else {
        param = alu_info->element_type1;
    }

    switch(action) {

        case  bcmFlowtrackerCheckActionNone:
            *hw_action = 0;
            break;

        case  bcmFlowtrackerCheckActionCounterIncr:
            *hw_action = 1;
            break;

        case  bcmFlowtrackerCheckActionCounterIncrByPktBytes:
            *hw_action = 2;
            break;

        case  bcmFlowtrackerCheckActionCounterIncrByValue:
            *hw_action = 3;
            break;

        case  bcmFlowtrackerCheckActionUpdateLowerValue:
            *hw_action = 4;
            break;

        case  bcmFlowtrackerCheckActionUpdateHigherValue:
            *hw_action = 5;
            break;
        case  bcmFlowtrackerCheckActionUpdateValue:
            *hw_action = 6;

            /* Use bitwise_or if Dos Attack */
            if ((param == bcmFlowtrackerTrackingParamTypeDosAttack) ||
                (param == bcmFlowtrackerTrackingParamTypeInnerDosAttack)) {
                *hw_action = 8;
            } else if ((param == bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector) ||
                    (param == bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector)) {

                /* Use bitwise_or if flowcheck or stickyor_update flag set */
                if ((alu_info->flowchecker_id != 0) ||
                        (alu_info->flags & BCMI_FT_ALU_LOAD_STICKYOR_UPDATE)) {
                    *hw_action = 8;
                }
            } else {
                /* LOAD operation and Stickyor_update is set */
                if ((alu_info->flowchecker_id == 0) &&
                        (alu_info->flags & BCMI_FT_ALU_LOAD_STICKYOR_UPDATE)) {
                    *hw_action = 8;
                }
            }
            break;

        case  bcmFlowtrackerCheckActionUpdateAverageValue:
            *hw_action = 7;
            break;

        default:
            *hw_action = 0;
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu_load_values_normalize
 * Purpose:
 *   Get ALU/Load values to get ALU/LOAD types.
 * Parameters:
 *   unit - (IN) BCM device id.
 *   info  - (IN) Group ALU/LOAD data element.
 *   length - (OUT) length of extractor.
 *   alu - (OUT) ALU or LOAD.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_load_values_normalize(int unit,
                                  bcmi_ft_group_alu_info_t *info,
                                  int *length,
                                  int *alu)
{
    int rv = BCM_E_NONE;
    int load_length = 0;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    if (!info || !length || !alu) {
        return BCM_E_PARAM;
    }

    /* Cannot support ALU and LOAD mixed elements. */
    if (info->element_type2) {
        if (info->key1.is_alu != info->key2.is_alu) {
            return BCM_E_PARAM;
        }
    }

    *alu = info->key1.is_alu;

    /* Get the bigger length. */
    load_length = ((info->key1.length > info->key2.length) ?
            info->key1.length : info->key2.length);

    /* Override if user has given length */
    if (info->flowchecker_id != 0) {

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        if (soc_feature(unit,
                    soc_feature_flex_flowtracker_ver_2)) {
            load_length = ((info->action_key.length > load_length) ?
                    info->action_key.length : load_length);
        }
#endif

        rv = bcmi_ft_flowchecker_get(unit,
                info->flowchecker_id, &fc_info);
        BCM_IF_ERROR_RETURN(rv);

        if (fc_info.flags &
                BCM_FLOWTRACKER_CHECK_MODE_16BIT) {
            if (load_length > 16) {
                return BCM_E_CONFIG;
            }
        } else if (fc_info.flags &
                BCM_FLOWTRACKER_CHECK_MODE_32BIT) {
            load_length = 32;
        } else {
            /* No user flags is passed */
            if ((fc_info.action_info.action ==
                        bcmFlowtrackerCheckActionCounterIncr) ||
                    (fc_info.action_info.action ==
                     bcmFlowtrackerCheckActionCounterIncrByValue) ||
                    (fc_info.action_info.action ==
                     bcmFlowtrackerCheckActionCounterIncrByPktBytes)) {
                load_length = 32;
            }
        }
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            (info->flags & BCMI_FT_ALU_LOAD_USE_AGG_ALU32)) {
        *alu = 2;
        load_length = 32;
    }
#endif

   if ((info->flowchecker_id == 0) &&
       (!(info->flags & BCMI_FT_ALU_LOAD_USE_AGG_ALU32))) {

       /* Use alu if tracking data is more that 16 */
       if (load_length > 16) {
           *alu = 1;
           sal_memcpy(&info->action_key, &info->key1,
                   sizeof (bcmi_ft_alu_key_t));
           info->action_element_type = info->element_type1;
           info->action_custom_id = info->custom_id1;
       }
   }
    *length = load_length;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu_load_type_get
 * Purpose:
 *   Get ALU/Load type based on length/params.
 * Parameters:
 *   unit   - (IN) BCM device id.
 *   alu    - (IN) Is alu required.
 *   length - (IN) length of extractor.
 *   type   - (OUT) type of load/ALU.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_load_type_get(int unit,
                          int alu, int length,
                          bcmi_ft_alu_load_type_t *type)
{
    int is_alu = 0;
    int local_type = 0;
    int min_extractor_length = 0;

    /*
     * These checks should not fail as they are checked when
     * information is added into the group.
     * For now, keeping the length checks with fixed numbers.
     * 32 is for ALU32 check and 16 is for LOAD16 and ALU16.
     */
    if (alu && ((length > 32) || (length < 8))) {
        return BCM_E_PARAM;
    }


    if ((!alu) && (length > 16)) {
        return BCM_E_PARAM;
    }

    if (alu && ((length >= 8) && (length < 16))) {
        /*
         * if ALU is required but we have less than 16 bit of length
         * then this might be a case where extractor is not aligned properly.
         * Hence we will change length accordingly to pick correct alu type.
         */
        length = 16;
    }

    /* Get the min extractor length for each memory. */
    min_extractor_length = ((alu) ? 16 : 8);

    /* ALu memories are in upper indexes. */
    is_alu = alu;
    is_alu <<= 1;

    /* based on the alu type and length, get the type of memory to use. */
    local_type = is_alu + ((length > min_extractor_length) ? 1 : 0);

    switch (local_type) {
        case 0 :
            *type = bcmiFtAluLoadTypeLoad8;
            break;
        case 1 :
            *type = bcmiFtAluLoadTypeLoad16;
            break;
        case 2 :
            *type = bcmiFtAluLoadTypeAlu16;
            break;
        case 3 :
            *type = bcmiFtAluLoadTypeAlu32;
            break;
        case 5:
            *type = bcmiFtAluLoadTypeAggAlu32;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu_check_free_index
 * Purpose:
 *   Check ALU/Load type based on group.
 * Parameters:
 *   unit          - (IN) BCM device id.
 *   id            - (IN) FT group id.
 *   alu_load_type - (IN) type of load/ALU.
 *   index         - (IN) index of that memory.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_check_free_index(int unit,
                             bcm_flowtracker_group_t id,
                             bcmi_ft_alu_load_type_t alu_load_type,
                             int index)
{
    uint32 mem_idx = 0;
    uint32 *in_use_array = NULL;
    int temp_index = 0, size_per_mem = 0;

    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        in_use_array = BCMI_FT_GROUP_ALU16_MEM_USE(unit, id);
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
    } else if (alu_load_type == bcmiFtAluLoadTypeAggAlu32) {
        in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
    }

    temp_index = index;
    size_per_mem = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, mem_idx);

    /* check which memory idx, this entry index belongs to. */
    while (temp_index >= size_per_mem) {
        temp_index -= size_per_mem;
        mem_idx++;
        size_per_mem = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, mem_idx);
    }

    /* check if this alu memory is used */
    if (in_use_array != NULL) {
        if (in_use_array[mem_idx]) {
            return FALSE;
        }
    }

    return TRUE;
}

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
/*
 * Function:
 *   bcmi_ftv2_alu_mem_config_check
 * Purpose:
 *   Check if provided ALU/LOADs can be configured in system
 *   resource allocation.
 * Parameters:
 *   unit           - (IN) BCM device id.
 *   id             - (IN) FT group id.
 *   alu_load_type  - (IN) Type of load/ALU.
 *   mem_idx        - (IN) Memory index to check.
 *   group_alu_info - (IN) Group alu info.
 *   load_indexes   - (IN) Load indexes.
 * Returns:
 *   TRUE  - if mem config is valid and can be allocated.
 *   FALSE - if mem config is not valid and provided
 *           mem_idx need to be skipped
 */
STATIC int
bcmi_ftv2_alu_mem_config_check(int unit,
                               bcm_flowtracker_group_t id,
                               bcmi_ft_alu_load_type_t alu_load_type,
                               uint32 mem_idx,
                               bcmi_ft_group_alu_info_t *group_alu_info,
                               int *load_indexes)
{
    uint32 load_num = 0, max_entry = 0;
    uint32 ift_l_count = 0, mft_l_count = 0, eft_l_count = 0;
    uint32 ift_r_count = 0, mft_r_count = 0, eft_r_count = 0;
    bcmi_ft_group_alu_info_t *local = NULL;

    /* set max load entry to search */
    max_entry = ((alu_load_type == bcmiFtAluLoadTypeLoad16)
            ? TOTAL_GROUP_LOAD16_DATA_NUM : TOTAL_GROUP_LOAD8_DATA_NUM);

    /* Return True if ver2 feature is not set */
    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        return TRUE;
    }

    /* Do ft_type check before allocating free index */
    if (alu_load_type == bcmiFtAluLoadTypeAlu16 ||
        alu_load_type == bcmiFtAluLoadTypeAlu32 ||
        alu_load_type == bcmiFtAluLoadTypeAggAlu32) {

        /* Check ft_type with resource ft_type */
        if (group_alu_info->alu_ft_type ==
            BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, alu_load_type, mem_idx, 0)) {
            return TRUE;
        }

    } else if (alu_load_type == bcmiFtAluLoadTypeLoad8 ||
               alu_load_type == bcmiFtAluLoadTypeLoad16) {

        /* Count no. of IFT/MFT/EFT loads incoming */
        for (load_num = 0; load_num < max_entry; load_num++) {
            if (load_indexes[load_num] == -1) {
                break;
            }

            local = (&(group_alu_info[load_indexes[load_num]]));

            if (local->alu_ft_type == bcmiFtTypeIFT) {
                ift_l_count++;
            } else if (local->alu_ft_type == bcmiFtTypeMFT) {
                mft_l_count++;
            } else if (local->alu_ft_type == bcmiFtTypeEFT) {
                eft_l_count++;
            }
        }

        /* Count no. of IFT/MFT/EFT resources */
        if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
            ift_r_count =
                BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD8(unit, bcmiFtTypeIFT);
            mft_r_count =
                BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD8(unit, bcmiFtTypeMFT);
            eft_r_count =
                BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD8(unit, bcmiFtTypeEFT);
        } else {
            ift_r_count =
                BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD16(unit, bcmiFtTypeIFT);
            mft_r_count =
                BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD16(unit, bcmiFtTypeMFT);
            eft_r_count =
                BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD16(unit, bcmiFtTypeEFT);
        }

        /* Requested loads cannot be greater than allocated resources */
        if ((ift_l_count <= ift_r_count) &&
            (mft_l_count <= mft_r_count) &&
            (eft_l_count <= eft_r_count)) {
            return TRUE;
        }
    }

    if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Load8 resource exceeds allocated - Requested "
           "(ift<%d>, mft <%d>, eft <%d>) Allocated "
           "(ift<%d>, mft <%d>, eft <%d>).\n"),
             ift_l_count, mft_l_count, eft_l_count,
             ift_r_count, mft_r_count, eft_r_count));
    } else if (alu_load_type == bcmiFtAluLoadTypeLoad16) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Load16 resource exceeds allocated - Requested "
           "(ift<%d>, mft <%d>, eft <%d>) Allocated "
           "(ift<%d>, mft <%d>, eft <%d>).\n"),
             ift_l_count, mft_l_count, eft_l_count,
             ift_r_count, mft_r_count, eft_r_count));
    } else {
        /* Do nothing */
    }

    return FALSE;
}

/*
 * Function:
 *   bcmi_ftv2_get_load_mem_vector
 * Purpose:
 *   Get load mem vector for load indexes.
 * Parameters:
 *   unit            - (IN)  BCM Device id.
 *   id              - (IN)  FT group id.
 *   alu_load_type   - (IN)  Type of LOAD/ALU.
 *   load_indexes    - (IN)  Load indexes.
 *   load_mem_vector - (OUT) Vector of load mem banks allocated.
 * Notes:
 *   Purpose of this api is to give load mem vector
 *   which will be stored in hash entry and used in match
 *   calls to skip comparing memories with different
 *   bank allocation pattern.
 * Returns:
 */
void
bcmi_ftv2_get_load_mem_vector(int unit,
                              bcm_flowtracker_group_t id,
                              bcmi_ft_alu_load_type_t alu_load_type,
                              int *load_indexes,
                              uint32 *load_mem_vector)
{
    int max_entry = 0, load_bank_num = 0, load_index_num = 0;
    bcmi_ft_group_alu_info_t *group_data = NULL, *local = NULL;

    group_data = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    max_entry = ((alu_load_type == bcmiFtAluLoadTypeLoad16)
                    ? TOTAL_GROUP_LOAD16_DATA_NUM : TOTAL_GROUP_LOAD8_DATA_NUM);

    if ((load_indexes != NULL) &&
        (load_mem_vector != NULL) &&
       ((alu_load_type == bcmiFtAluLoadTypeLoad8) ||
        (alu_load_type == bcmiFtAluLoadTypeLoad16))) {

        /* initialize load mem vector */
        *load_mem_vector = 0;

        for (load_index_num = 0; load_index_num < max_entry; load_index_num++) {

            /* Now based on the incoming info, match the entries. */
            if (load_indexes[load_index_num] == -1) {
                break;
            }

            local = (&(group_data[load_indexes[load_index_num]]));

            /* Loop through all data memories */
            for (load_bank_num = 0;
                 load_bank_num < max_entry; load_bank_num++) {

                if (!(*load_mem_vector & (1 << load_bank_num))) {

                    if (local->alu_ft_type !=
                            BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                          alu_load_type, 0, load_bank_num)) {
                        continue;
                    }

                    *load_mem_vector |= (1 << load_bank_num);
                    break;
                }
            }
        }
    }
}
#endif

/*
 * Function:
 *   bcmi_ft_alu_get_free_index
 * Purpose:
 *   Get free index from alu/load memory
 * Parameters:
 *   unit           - (IN) BCM device id.
 *   id             - (IN) FT group id.
 *   alu_load_type  - (IN) type of load/ALU.
 *   group_alu_info - (IN) group alu info.
 *   load_indexes   - (IN) load indexes.
 *   index          - (OUT) Free index of that memory.
 *  Notes:
 *    1. Added call to bcmi_ftv2_alu_mem_config_check
 *       to check if particular mem idx is valid
 *       for incoming alu/load ft_type or not.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_get_free_index(int unit,
                           bcm_flowtracker_group_t id,
                           bcmi_ft_alu_load_type_t alu_load_type,
                           bcmi_ft_group_alu_info_t *group_alu_info,
                           int *load_indexes, int *index)
{
    uint32 *in_use_array = NULL;
    int i = 0, size_per_mem = 0;
    int start_index = 0, end_index = 0;
    uint32 mem_idx = 0, total_mems = 1;
    const char *load_type_str[] = {
        "Load8",
        "Load16",
        "Alu16",
        "Alu32",
        "Agg_Alu32"
    };
    const char *ft_type_str[] = BCMI_FT_TYPE_STRINGS;

    *index = -1;

    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        in_use_array = BCMI_FT_GROUP_ALU16_MEM_USE(unit, id);
        total_mems   = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16);
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
        total_mems   = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32);
    } else if (alu_load_type == bcmiFtAluLoadTypeAggAlu32) {
        in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
        total_mems   = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32);
    }

   LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
       (BSL_META_U(unit, "%s -  total_mems = %d\n"),
                        load_type_str[alu_load_type], total_mems));

    /* start index should be where the free index starts. */
    while (mem_idx < total_mems) {

        size_per_mem = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, mem_idx);

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit, "mem_idx = %d, size_per_mem = %d\n"),
                                            mem_idx, size_per_mem));

        if (in_use_array != NULL) {
            /* This ALU memory is used. go for next one. */
            if ((in_use_array[mem_idx]) ||
                (bcmi_ft_chip_debug_mem_check(unit, alu_load_type, mem_idx))) {
                mem_idx++;
                start_index += size_per_mem;
                continue;
            }
        }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        /* do ft_type check before allocating free index */
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            if (!bcmi_ftv2_alu_mem_config_check(unit, id,
                                              alu_load_type, mem_idx,
                                              group_alu_info, load_indexes)) {
                mem_idx++;
                start_index += size_per_mem;
                continue;
            }
        }
#endif

        /* set the end index according to the indexes in that memory. */
        end_index = start_index + size_per_mem;

        /*
         * Cannot not index 0 for ALU/LOAD.
         * If index 0 is used then Group may point to it and
         * malicious actions can be taken on that flow.
         */
        i = start_index;

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        if ((soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) &&
           ((alu_load_type == bcmiFtAluLoadTypeAlu16) ||
            (alu_load_type == bcmiFtAluLoadTypeAlu32) ||
            (alu_load_type == bcmiFtAluLoadTypeAggAlu32))) {
            /* First Index is not used in AGG ALU32 */
            i++;
        } else
#endif
        {
            i += ((start_index == 0) ? 1 : 0);
        }

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "start_index = %d, end_index = %d\n"),
                                                start_index, end_index));

        /* based on the above index select the bitmap */
        for (; i < end_index; i++) {
            if (!(SHR_BITGET(
                      BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, alu_load_type), i))) {

                *index = i;
                LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "mem_idx = %d, index = %d\n"),
                                                   mem_idx, *index));
                return BCM_E_NONE;
            }
        }
        start_index += size_per_mem;
        mem_idx++;
    }

    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
           (BSL_META_U(unit, "No %s resource free to allocate for stage %s.\n"),
            load_type_str[alu_load_type],ft_type_str[group_alu_info->alu_ft_type]));
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
           (BSL_META_U(unit, "No %s resource free to allocate for stage %s.\n"),
            load_type_str[alu_load_type],ft_type_str[group_alu_info->alu_ft_type]));
    } else if (alu_load_type == bcmiFtAluLoadTypeAggAlu32) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
           (BSL_META_U(unit, "No %s resource free to allocate for stage %s.\n"),
            load_type_str[alu_load_type],ft_type_str[group_alu_info->alu_ft_type]));
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "No %s resource free to allocate.\n"),
                 load_type_str[alu_load_type]));
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *   bcmi_ft_alu_entry_match
 * Purpose:
 *   Match ALU check entry
 * Parameters:
 *   unit            - (IN) BCM device id
 *   id              - (IN) FT group id.
 *   info            - (IN) ALU/Load information of group.
 *   load16_indexes  - (IN) Load 16 indexes in group
 *   index           - (OUT) sw index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_entry_match(int unit,
                        bcm_flowtracker_group_t id,
                        bcmi_ft_group_alu_info_t *group_alu_info,
                        bcmi_ft_alu_hash_info_t *alu_info,
                        int *load16_indexes, int *index)
{
    /*
     * Need to compare all the fields to check if
     * this is exact match.
     * location, length, type,
     * update location, update operation.
     * export threshold and operation.
     */
    if (group_alu_info->key1.location !=
                alu_info->key.location) {
        return BCM_E_NOT_FOUND;
    }

    if (group_alu_info->key1.length !=
                  alu_info->key.length) {
        return BCM_E_NOT_FOUND;
    }

    if (group_alu_info->key1.is_alu !=
                  alu_info->key.is_alu) {
        return BCM_E_NOT_FOUND;
    }

    if (group_alu_info->alu_ft_type !=
                alu_info->hash_ft_type) {
        return BCM_E_NOT_FOUND;
    }

    if (group_alu_info->element_type1 !=
                       alu_info->param) {
        return BCM_E_NOT_FOUND;
    }

    if (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction !=
                                (alu_info->direction)) {
        return BCM_E_NOT_FOUND;
    }

    if (group_alu_info->custom_id1 != alu_info->custom_id) {
        return BCM_E_NOT_FOUND;
    }

    /* For ALU based memories, we need to make sure that other than
     * basic data, update and export data should also match */
    if (bcmi_ft_check_list_compare(unit, &(alu_info->head),
                        group_alu_info->flowchecker_id)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_alu_actual_location_get
 * Purpose:
 *   Get the actual location of attributes, if required
 * Parameters:
 *   unit       - (IN) BCM device id
 *   info       - (IN) ALU/Load information of group.
 *   pri_attrib_actual_location - (OUT) Actual location of primary attribute.
 *   sec_attrib_actual_location - (OUT) Actual location of secondary attribute.
 *   update_actual_location     - (OUT) Actual location of action attribute.
 * Returns:
 *   None.
 *   If secondary check element is not present we return
 *   primary attribute actual location filled in secondary attribute output.
 *   If update element is not present we return
 *   primary attribute actual location filled in update output.
 */
void
bcmi_ft_group_alu_actual_location_get(int unit,
                                      bcmi_ft_group_alu_info_t *alu_info,
                                      int *pri_attrib_actual_location,
                                      int *sec_attrib_actual_location,
                                      int *update_actual_location)
{
    int pri_attrib_location_local = 0;
    int sec_attrib_location_local = 0;
    int update_location_local = 0;

    /* Set values asked to 0 by default. */
    if (pri_attrib_actual_location != NULL) {
        *pri_attrib_actual_location = 0;
    }
    if (sec_attrib_actual_location != NULL) {
        *sec_attrib_actual_location = 0;
    }
    if (update_actual_location != NULL) {
        *update_actual_location = 0;
    }

    if ((alu_info != NULL) &&
       ((alu_info->alu_load_type == bcmiFtAluLoadTypeAlu16) ||
        (alu_info->alu_load_type == bcmiFtAluLoadTypeAlu32) ||
        (alu_info->alu_load_type == bcmiFtAluLoadTypeAggAlu32))) {

        if (alu_info->alu_load_type == bcmiFtAluLoadTypeAlu16) {

            /* case for ALU16 - it can operate only in 16 bit gran. */
            pri_attrib_location_local = (alu_info->key1.location / 16);
            sec_attrib_location_local = (alu_info->key2.location / 16);
            update_location_local = (alu_info->action_key.location / 16);

        } else if ((alu_info->alu_load_type == bcmiFtAluLoadTypeAlu32) ||
                   (alu_info->alu_load_type == bcmiFtAluLoadTypeAggAlu32)) {

            /* case for ALU32 - it can operate in 16b or 32b gran
             * which depends upon key length. */
            if (alu_info->key1.length > 16) {
                pri_attrib_location_local = (alu_info->key1.location / 32);
            } else {
                pri_attrib_location_local = (alu_info->key1.location / 16);
            }
            if (alu_info->key2.length > 16) {
                sec_attrib_location_local = (alu_info->key2.location / 32);
            } else {
                sec_attrib_location_local = (alu_info->key2.location / 16);
            }
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                update_location_local = (alu_info->action_key.location / 32);
            } else
#endif
            {
                update_location_local = (alu_info->action_key.location / 16);
            }
        }

        /* If secondary check element is not present,
         * fill primary check info.*/
        if (!alu_info->element_type2) {
            sec_attrib_location_local = pri_attrib_location_local;
        }

        /* Return values retreived. */
        if (pri_attrib_actual_location != NULL) {
            *pri_attrib_actual_location = pri_attrib_location_local;
        }
        if (sec_attrib_actual_location != NULL) {
            *sec_attrib_actual_location = sec_attrib_location_local;
        }
        if (update_actual_location != NULL) {
            *update_actual_location = update_location_local;
        }
    }
}

/*
 * Function:
 *   bcmi_ft_group_alu_attrib_shift_bits_get
 * Purpose:
 *   Get the shift bits for attributes, if required
 *   for writing threshold value in hardware.
 * Parameters:
 *   unit                  - (IN)  BCM device id
 *   dual_operation        - (IN)  If check has binary operator.
 *   info                  - (IN)  ALU/Load information of group.
 *   pri_attrib_shift_bits - (OUT) Number of bits threshold needs to be shifted
 *                                 for primary check attribute before writing
 *                                 to hardware.
 *   sec_attrib_shift_bits - (OUT) Number of bits threshold needs to be shifted
 *                                 for secondary check attribute before writing
 *                                 to hardware.
 * Returns:
 *   None.
 * Notes:
 *   Whether ALU16 or 32, shift bits are computed by modulo operation with
 *   16 as ALU32 check operations can operate in gran16 & 32.
 *   If secondary check element is not present we return
 *   primary attribute shift bits filled in secondary attribute output.
 */
void
bcmi_ft_group_alu_attrib_shift_bits_get(int unit,
                                    int dual_operation,
                                    bcmi_ft_group_alu_info_t *info,
                                    int *pri_attrib_shift_bits,
                                    int *sec_attrib_shift_bits)
{
    int pri_shift_bits_local = 0, sec_shift_bits_local = 0;

    /* Set values asked to 0 by default. */
    if (pri_attrib_shift_bits != NULL) {
        *pri_attrib_shift_bits = 0;
    }
    if (sec_attrib_shift_bits != NULL) {
        *sec_attrib_shift_bits = 0;
    }

    if ((info != NULL) &&
       ((info->alu_load_type == bcmiFtAluLoadTypeAlu16) ||
        (info->alu_load_type == bcmiFtAluLoadTypeAlu32) ||
        (info->alu_load_type == bcmiFtAluLoadTypeAggAlu32))) {

        /* Return primary key shift bits based on its location. */
        if ((info->key1.location % 16) != 0) {
            pri_shift_bits_local = (info->key1.location % 16);
        }

        /* Return secondary key shift bits based on its location if
         * element is present. Else return primary key shifts bit. */
        if (info->element_type2) {
            if ((info->key2.location % 16) != 0) {
                sec_shift_bits_local = (info->key2.location % 16);
            }
        } else if (dual_operation == TRUE) {
            sec_shift_bits_local = pri_shift_bits_local;
        }

        /* Return values retreived. */
        if (pri_attrib_shift_bits != NULL) {
            *pri_attrib_shift_bits = pri_shift_bits_local;
        }
        if (sec_attrib_shift_bits != NULL) {
            *sec_attrib_shift_bits = sec_shift_bits_local;
        }
    }
}

/*
 * Function:
 *   bcmi_ft_group_alu_action_shift_bits_get
 * Purpose:
 *   Get the action shift bits, if required, for writing
 *   threshold value in hardware.
 * Parameters:
 *   unit              - (IN) BCM device id
 *   action            - (IN) FT Check Action.
 *   info              - (IN) ALU/Load information of group.
 *   action_shift_bits - (OUT) Number of bits threshold needs
 *                             to be shifted before writing to hardware.
 * Returns:
 *   None.
 */
void
bcmi_ft_group_alu_action_shift_bits_get(int unit,
                                        bcm_flowtracker_check_action_t action,
                                        bcmi_ft_group_alu_info_t *info,
                                        int *action_shift_bits)
{
    int shift_required = FALSE;

    *action_shift_bits = 0;

    if ((info->alu_load_type == bcmiFtAluLoadTypeAlu16) ||
        (info->alu_load_type == bcmiFtAluLoadTypeAlu32) ||
        (info->alu_load_type == bcmiFtAluLoadTypeAggAlu32)) {

        switch(action) {

            case  bcmFlowtrackerCheckActionNone:
            case  bcmFlowtrackerCheckActionCounterIncr:
            case  bcmFlowtrackerCheckActionCounterIncrByPktBytes:
                shift_required = FALSE;
                break;

            case  bcmFlowtrackerCheckActionCounterIncrByValue:
            case  bcmFlowtrackerCheckActionUpdateLowerValue:
            case  bcmFlowtrackerCheckActionUpdateHigherValue:
            case  bcmFlowtrackerCheckActionUpdateValue:
            case  bcmFlowtrackerCheckActionUpdateAverageValue:
                shift_required = TRUE;
                break;

            default:
                shift_required = FALSE;
        }

        if (shift_required == TRUE) {

            if (info->alu_load_type == bcmiFtAluLoadTypeAlu16) {
                /* ALU16 action picks up action
                 * in granularity of 16b always. */
                *action_shift_bits = info->action_key.location % 16;

            } else {
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
                if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                    /* BS2.0 ALU32 action picks up action
                     * in granularity of 32b. */
                    *action_shift_bits = info->action_key.location % 32;

                } else
#endif
                {
                    /* BS1.0 ALU32 action picks up action
                     * in granularity of 16b. */
                    *action_shift_bits = info->action_key.location % 16;
                }
            }
        }
    }
}

/*|************************************************************|
  |********         ALU Software Install Methods.      ********|
  |************************************************************|
  |*******                 End                         ********|
  |************************************************************|*/




/*|************************************************************|
  |********         ALU Hardware Install Methods.      ********|
  |************************************************************|
  |*******                 Start                       ********|
  |************************************************************|*/

/*
 * Function:
 *   bcmi_ft_flowchecker_alu_base_control_set
 * Purpose:
 *   Set base control memory for ALUs..
 * Parameters:
 *   unit     - (IN) BCM device id
 *   id       - (IN) FT group id.
 *   alu_info - (IN) ALU information of group.
 *   alu_fmt  - (IN) formatiinfo of memory.
 * Returns:
 *   NONE
 */
int
bcmi_ft_flowchecker_alu_base_control_set(int unit,
                                         bcm_flowtracker_group_t id,
                                         bcmi_ft_group_alu_info_t *alu_info,
                                         uint32 *alu_fmt)
{
    int rv = BCM_E_NONE;
    uint32 kmap_val = 0, val;
    int val1, val2, opr1, opr2;
    int update_location = 0, hw_action = 0;
    int check_0_opr = -1, check_1_opr = -1;
    uint32 bit_select_array[4];
    bcmi_flowtracker_flowchecker_info_t fc_info;
    bcm_flowtracker_check_action_t alu_action = 0;
    int actual_location1 = 0, actual_location2 = 0;
    int clear_on_export = 0, export_check_mode = 1;
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    int clear_on_periodic_export = 0;
#endif
    int check1_attr_sel = 0, check2_attr_sel = 0;
    int update_attr_sel = 0;
    bcm_flowtracker_direction_t direction = bcmFlowtrackerFlowDirectionNone;

    /* Input parameters check. */
    if ((alu_fmt == NULL) || (alu_info == NULL)) {
        return BCM_E_PARAM;
    }

    /* Initialise local variables. */
    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
    val1 = val2 = opr1 = opr2 = -1;

    /* Retreive relative locations from db absolute location values. */
    bcmi_ft_group_alu_actual_location_get(unit, alu_info, &actual_location1,
                                         &actual_location2, &update_location);

    /* Install ALU with or without checkers configured. */
    if (alu_info->flowchecker_id > 0) {

        /* Install ALU with checker added */
        BCM_IF_ERROR_RETURN
            (bcmi_ft_flowchecker_get(unit, alu_info->flowchecker_id, &fc_info));

        rv = bcmi_ft_flowchecker_values_get(unit, &fc_info, &val1, &val2,
                                                            &opr1, &opr2);
        BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        /* Retrieve check attribute selector */
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
            bcmi_ftv3_alu_check_attr_select_get(unit,
                    &fc_info, &check1_attr_sel,
                    &check2_attr_sel, &update_attr_sel);
        }
#endif
        /* Install primary check */
        if (opr1 != -1) {

            if (check1_attr_sel == BCMI_FT_ALU_ATTR_SELECT_EXTRACTION) {
                soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                        alu_fmt, CHECK_0_ATTR_SELECTf, actual_location1);
            }
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
            else if (check1_attr_sel == BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT) {
                soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                        alu_fmt, BYTE_COUNT_CHECK_0_SELECTf, 1);
            }
#endif
            soc_format_field32_set(unit,BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                             alu_fmt, CHECK_0_OPERATIONf, opr1);

            if (fc_info.flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1) {
                soc_format_field32_set(unit,
                                       BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                       alu_fmt, TIMESTAMP_0_TRIGf, 1);
            }
            if (fc_info.flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2) {
                soc_format_field32_set(unit,
                                       BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                       alu_fmt, TIMESTAMP_1_TRIGf, 1);
            }
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                if (fc_info.flags &
                            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3) {
                    soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                            alu_fmt, TIMESTAMP_2_TRIGf, 1);
                }
                if (fc_info.flags &
                            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4) {
                    soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                            alu_fmt, TIMESTAMP_3_TRIGf, 1);
                }
            }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
        }

        /* Install secondary check */
        if (opr2 != -1) {

            if (check2_attr_sel == BCMI_FT_ALU_ATTR_SELECT_EXTRACTION) {
                soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                        alu_fmt, CHECK_1_ATTR_SELECTf, actual_location2);
            }
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
            else if (check2_attr_sel == BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT) {
                soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                        alu_fmt, BYTE_COUNT_CHECK_1_SELECTf, 1);
            }
#endif
            soc_format_field32_set(unit,
                               BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                               alu_fmt, CHECK_1_OPERATIONf, opr2);

            soc_format_field32_set(unit,
                               BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                               alu_fmt, CHECK_LOGIC_BIT_SELECT_1f, 1);

            if (fc_info.flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1) {
                soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                        alu_fmt, TIMESTAMP_0_TRIGf, 1);
            }
            if (fc_info.flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2) {
                soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                        alu_fmt, TIMESTAMP_1_TRIGf, 1);
            }
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                if (fc_info.flags &
                        BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3) {
                    soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                            alu_fmt, TIMESTAMP_2_TRIGf, 1);
                }
                if (fc_info.flags &
                        BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4) {
                    soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                            alu_fmt, TIMESTAMP_3_TRIGf, 1);
                }
            }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
        }

        /* Set ALU actions. */
        alu_action = fc_info.action_info.action;

        if (fc_info.action_info.action == bcmFlowtrackerCheckActionUpdateValue){
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

            } else
#endif
            {
                if (alu_info->action_key.length > 16) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                     "Action Element len(%d) is more than supported len 16.\n"),
                      alu_info->action_key.length));
                    return BCM_E_PARAM;
                }
            }
        }

        direction = fc_info.action_info.direction;

        clear_on_export = ((fc_info.check1.flags &
                    BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_EXPORT) ? 1 : 0);

        export_check_mode = ((fc_info.check1.flags &
                    BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD) ? 0 : 1);

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

            clear_on_periodic_export = ((fc_info.check1.flags &
                  BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_PERIODIC_EXPORT) ? 1 : 0);
        }
#endif

        /* Set Kmap check logic bit select and get kmap */
        rv = bcmi_ft_alu_check_kmap_data_get(unit, &fc_info,
                    opr1, opr2, &kmap_val, &bit_select_array[0]);

    } else {
        if ((alu_info->element_type1 <
                    bcmFlowtrackerTrackingParamTypeSrcIPv4) ||
            (alu_info->element_type1 >=
                    bcmFlowtrackerTrackingParamTypeCount)) {
            return BCM_E_PARAM;
        }

        /* if it is not flowchecker then only one element in alu. */
        rv = bcmi_ft_element_alu_action_get(unit, id,
                    alu_info->element_type1, &alu_action);
        BCM_IF_ERROR_RETURN(rv);

        /* We can associate two flowcheckers together so that we can have . */
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                             alu_fmt, CHECK_0_ATTR_SELECTf, actual_location1);

        /* Make Check 0 operation as always pass. */
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                            alu_fmt, CHECK_0_OPERATIONf, 0x6);

        /* Make Check 1 operation as always pass. */
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                            alu_fmt, CHECK_1_OPERATIONf, 0x6);

        /* Set Kmap check logic bit select and get kmap */
        rv = bcmi_ft_alu_check_kmap_data_get(unit, NULL,
                    0x6, -1, &kmap_val, &bit_select_array[0]);

        direction = alu_info->direction;
    }

    BCM_IF_ERROR_RETURN(rv);

    /* Set all the bit selects to check 0 and set KMAP accordingly.*/
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_0f, bit_select_array[0]);

    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_1f, bit_select_array[1]);

    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_2f, bit_select_array[2]);

    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_3f, bit_select_array[3]);

    /* Set KMAP Value for Check. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, FLEX_CHECK_LOGIC_KMAP_CONTROLf, kmap_val);

    /* Now get the information about update operations. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_alu_hw_action_get(unit, alu_info, alu_action, &hw_action));

    /* Update operation with the action element. */
    if (update_attr_sel == BCMI_FT_ALU_ATTR_SELECT_EXTRACTION) {
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_ATTR_SELECTf, update_location);
    }
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    else if (update_attr_sel == BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT) {
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, BYTE_COUNT_UPDATE_ALU_SELECTf, 1);
    }
#endif

    /* Update operation with the action element. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                    alu_fmt, UPDATE_OPERATIONf, hw_action);

    /* Get weight hw encoding */
    rv = bcmi_ft_tracking_param_weight_hw_val_get(unit,
                                    fc_info.action_info.weight, &val);
    BCM_IF_ERROR_RETURN(rv);
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                          alu_fmt, UPDATE_OP_PARAMf, val);

    /* Set the load trigger for forward and reverse. */
    if (direction != bcmFlowtrackerFlowDirectionNone) {
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, LOAD_TRIGGERf, direction);
    } else {
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, LOAD_TRIGGERf,
                BCMI_FT_GROUP_FTFP_DATA(unit, id).direction);
    }

    /* Clear the data after export based on user input. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                       alu_fmt, CLEAR_ON_EXPORT_ENABLEf, clear_on_export);

    /* Export data if there is new change in existing session data. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                          alu_fmt, EXPORT_CHECK_MODEf, export_check_mode);

    /* Set export operation. */
    rv = bcmi_ft_alu_hw_operation_get(unit,
               fc_info.export_info.operation, &check_0_opr, &check_1_opr);
    BCM_IF_ERROR_RETURN(rv);

    if (check_0_opr != -1) {
        soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                    alu_fmt, EXPORT_OPERATIONf, check_0_opr);
    } else {
        soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                    alu_fmt, EXPORT_OPERATIONf, check_1_opr);
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

        /* clear the data after periodic export based on user input. */
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
         alu_fmt, CLEAR_ON_PERIODIC_EXPORT_ENABLEf, clear_on_periodic_export);

        /* allocate attribute shift & mask entry and set index in alu fmt */
        BCM_IF_ERROR_RETURN
            (bcmi_ftv2_group_alu_mask_set(unit, alu_info, &fc_info, alu_fmt));
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu32_entry_install
 * Purpose:
 *   Configure ALU32 entry in hardware
 * Parameters:
 *   unit         - (IN) BCM device id
 *   id           - (IN) FT group id.
 *   info         - (IN) ALU information of group.
 *   load_indexes - (IN) load indexes
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu32_entry_install(int unit,
                    bcm_flowtracker_group_t id,
                    bcmi_ft_group_alu_info_t *info,
                    int *load_indexes)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem = -1;
    int val1, val2, opr1, opr2;
    int shift_bits = 0, shift_bits2 = 0;
    int a_idx = -1, index = -1, new_index = -1;
    int min_value = 0, max_value = 0, threshold = 0;
    int dual_operation = FALSE;
    uint32 alu_base_ctrlfmt[5];
    uint32 alu32_specific_ctrlfmt[5];
    uint32 alu32_ctrlfmt[10];
    uint32 *entry_buf = NULL;
    bsc_dg_group_table_entry_t dg_entry;
    bcmi_ft_alu_load_type_t alu_load_type;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;

    sal_memset(&alu32_entry, 0, sizeof(bsc_dg_group_alu32_profile_0_entry_t));
    sal_memset(alu_base_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu32_specific_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu32_ctrlfmt, 0, 10 * sizeof(uint32));
    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    val1 = val2 = opr1 = opr2 = -1;

    alu_load_type = bcmiFtAluLoadTypeAlu32;
    if (info->flags & BCMI_FT_ALU_LOAD_USE_AGG_ALU32) {
        alu_load_type = bcmiFtAluLoadTypeAggAlu32;
    }

    /* First insert the hash entry. info carries single alu information */
    rv = bcmi_ft_alu_hash_insert
            (unit, id, alu_load_type, info, NULL, &index);

    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            bcmi_ft_alu_load_mem_index_get
                (unit, index, alu_load_type, &new_index, &mem, &a_idx);
            /* Now that BCM_E_EXISTS is handled. please change rv. */
            rv = BCM_E_NONE;
        } else {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "ALU32 allocate Failed: rv = %d"
                                " for check=%d element=%d\n"),
                     rv, info->flowchecker_id, info->element_type1));
            return rv;
        }
    } else {

        /* Index is obtained, convert the index into the memory type. */
        bcmi_ft_alu_load_mem_index_get
            (unit, index, alu_load_type, &new_index, &mem, &a_idx);

        /* Set the software entries. */
        rv = bcmi_ft_flowchecker_alu_base_control_set(unit, id,
                                                      info, alu_base_ctrlfmt);
        BCMI_CLEANUP_IF_ERROR(rv);

        if (info->flowchecker_id > 0) {
            /*
             * The update for an element can still be done without performing
             * any check. For example a template can just tell which element
             * need to be updated. Therefore other than check elements, all
             * other flow checker elements should be updated even without
             * a flowchecker id.
             */
            rv = bcmi_ft_flowchecker_get(unit, info->flowchecker_id, &fc_info);
            BCMI_CLEANUP_IF_ERROR(rv);

            dual_operation = bcmi_ft_flowchecker_opr_is_dual(unit,
                        fc_info.check1.operation);

            bcmi_ft_group_alu_attrib_shift_bits_get(unit,
                    dual_operation, info, &shift_bits, &shift_bits2);

            bcmi_ft_flowchecker_values_get(unit, &fc_info, &val1, &val2,
                                                           &opr1, &opr2);

            if (opr1 != -1) {

                min_value = (val1 << shift_bits);
                soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                            alu32_specific_ctrlfmt, CHECK_0_THRESHOLDf,
                            min_value);

                if (info->key1.length > 16) {
                    soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                            alu32_specific_ctrlfmt, CHECK_0_ATTR_SELECT_GRANf,
                            1);
                }
            }

            if (opr2 != -1) {

                max_value = (val2 << shift_bits2);
                soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                            alu32_specific_ctrlfmt, CHECK_1_THRESHOLDf,
                            max_value);

                if ((info->key2.length > 16) ||
                   (dual_operation && (info->key1.length > 16))) {
                    soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                            alu32_specific_ctrlfmt, CHECK_1_ATTR_SELECT_GRANf,
                            1);
                }
            }

            if (fc_info.export_info.export_check_threshold > 0) {

                shift_bits2 = 0;
                if (!(info->flags &  BCMI_FT_ALU_LOAD_DATA_SHIFT_APPLIED)) {
                    bcmi_ft_group_alu_action_shift_bits_get(unit,
                            fc_info.action_info.action, info, &shift_bits2);
                }

                threshold = fc_info.export_info.export_check_threshold << shift_bits2;

                soc_format_field32_set(unit,
                                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                                alu32_specific_ctrlfmt, EXPORT_THRESHOLDf,
                                threshold);
            }
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            /* set delay for checker provided */
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                rv = bcmi_ftv2_group_check_delay_set(unit, id, &fc_info,
                                                     alu32_specific_ctrlfmt);
                BCMI_CLEANUP_IF_ERROR(rv);
            }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

        } else {
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
             /* set delay for delay params without checker */
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                rv = bcmi_ftv2_group_param_delay_set(unit, id,
                                                     info, alu32_specific_ctrlfmt);
                BCMI_CLEANUP_IF_ERROR(rv);
            }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
        }

        /* SW war - avoid first packet iat/delay trigger
         * set update_attr_1_ts_select for update operations
         * not acting on packet attributes. */
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
            soc_feature(unit,
                      soc_feature_flowtracker_ver_2_first_pkt_delay_sw_war)) {
            val1 = soc_format_field32_get(unit,
                        BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                        alu_base_ctrlfmt, UPDATE_OPERATIONf);
            val2 = bcmi_alu_action_conversion[val1];
            if ((val2 == bcmFlowtrackerCheckActionCounterIncr) ||
                (val2 == bcmFlowtrackerCheckActionCounterIncrByPktBytes)) {
                soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                    alu32_specific_ctrlfmt, UPDATE_ATTR_1_TS_SELECTf, 1);
            }
        }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

        soc_format_field_set(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
                alu32_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

        soc_format_field_set(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
           alu32_ctrlfmt, ALU32_SPECIFIC_CONTROL_BUSf, alu32_specific_ctrlfmt);

        soc_mem_field_set(unit, mem, (uint32 *)&alu32_entry, DATAf,
                                                                alu32_ctrlfmt);

        /* The profile is set. Write into the index of group. */
        rv  = soc_mem_write(unit, mem, MEM_BLOCK_ALL, new_index, &alu32_entry);

        BCMI_CLEANUP_IF_ERROR(rv);
    }

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "ALU allocated : index = %d, Type = %s\n"),
                         index, alu_type_str[alu_load_type]));

    if (BCM_SUCCESS(rv) && (index != -1)) {

        /* Assign the bitmap to this index. */
        SHR_BITSET((BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, alu_load_type)), index);

        /* Also increment the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, alu_load_type, index);

        /* Set the groups IN use ALU memories. */
        (BCMI_FT_GROUP_ALU32_MEM_USE(unit, id))[a_idx] = 1;

        /* Update chip debug memory also if aplicable. */
        bcmi_ft_chip_debug_use_mem_update(unit, alu_load_type, a_idx, 1);

        if (BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type)) {
            /* retreive relevant pdd mem for device */
            bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

            /* Set pdd profile entry for this alu entry. */
            soc_mem_field32_set(unit, mem,
                    (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                    (BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type)
                        [a_idx]).param_id, 1);
        }

        /* Get Group memory */
        rv = bcmi_ft_group_type_mem_get(unit, 0,
                BCMI_FT_GROUP_TYPE_GET(id), &mem);
        BCMI_CLEANUP_IF_ERROR(rv);

        entry_buf = sal_alloc(SOC_MEM_WORDS(unit, mem) * 4,
                "dg group entry buf");
        if (entry_buf == NULL) {
            rv = BCM_E_MEMORY;
            BCMI_CLEANUP_IF_ERROR(rv);
        }

        rv  = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                BCMI_FT_GROUP_INDEX_GET(id), entry_buf);
        BCMI_CLEANUP1_IF_ERROR(rv);

        soc_mem_field32_set(unit, mem, entry_buf,
                BCMI_FT_ALU_LOAD_GROUP_FID_INFO(unit, alu_load_type)[a_idx],
                new_index);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                BCMI_FT_GROUP_INDEX_GET(id), entry_buf);
        BCMI_CLEANUP1_IF_ERROR(rv);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        /* Update triggers in state transtion table */
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
            rv = bcmi_ftv3_flow_transition_trigger_update(unit,
                    alu_load_type, index, info, 1);
            BCMI_CLEANUP1_IF_ERROR(rv);
        }
#endif
    }

    if (BCM_SUCCESS(rv)) {
        info->flags |= BCMI_FT_ALU_LOAD_HW_INSTALLED;
        return rv;
    }

cleanup1:

    if (index != -1) {
        /* also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, alu_load_type, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT(unit, alu_load_type, index))) {

            /* clear the bitmap to this index. */
            SHR_BITCLR(
                (BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, alu_load_type)), index);
        }
    }

cleanup:

    /* remove the previously added hash table entry. */
    bcmi_ft_alu_hash_remove(unit, id, alu_load_type,
                                          info, NULL, &index);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_alu16_entry_install
 * Purpose:
 *   Configure ALU16 entry in hardware
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) FT group id.
 *   info           - (IN) ALU information of group.
 *   load_indexes   - (IN) load indexes
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu16_entry_install(int unit,
                            bcm_flowtracker_group_t id,
                            bcmi_ft_group_alu_info_t *info,
                            int *load_indexes)
{
    int rv = BCM_E_NONE;
    int val1, val2, opr1, opr2;
    int fid_len = 0, fid_value = 0;
    int shift_bits = 0, shift_bits2 = 0;
    int a_idx = 0, index = -1, new_index = -1;
    int min_value = 0, max_value = 0, threshold = 0;
    int dual_operation = FALSE;
    soc_mem_t mem = -1;
    uint32 alu_base_ctrlfmt[5];
    uint32 alu16_specific_ctrlfmt[2];
    uint32 alu16_ctrlfmt[7];
    bsc_dg_group_table_entry_t dg_entry;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    int check1_attr_sel = 0, check2_attr_sel = 0;
    int update_attr_sel = 0;
#endif

    /* Initialize variables. */
    sal_memset(alu_base_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu16_specific_ctrlfmt, 0, 2 * sizeof(uint32));
    sal_memset(alu16_ctrlfmt, 0, 7 * sizeof(uint32));
    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    val1 = val2 = opr1 = opr2 = -1;

    /* First insert the hash entry. */
    rv = bcmi_ft_alu_hash_insert(unit,
                     id, bcmiFtAluLoadTypeAlu16, info, NULL, &index);

    if (BCM_FAILURE(rv)) {

        if (rv == BCM_E_EXISTS) {
            bcmi_ft_alu_load_mem_index_get(unit,
                index, bcmiFtAluLoadTypeAlu16, &new_index, &mem, &a_idx);
            /* now that BCM_E_EXISTS is handled. please change rv. */
            rv = BCM_E_NONE;
        } else {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "ALU16 allocate Failed: rv = %d"
                                " for check=%d element=%d\n"),
                     rv, info->flowchecker_id, info->element_type1));
            return rv;
        }

    } else {

        /* Index is obtained, convert the index into the memory type. */
        bcmi_ft_alu_load_mem_index_get(unit,
                    index, bcmiFtAluLoadTypeAlu16, &new_index, &mem, &a_idx);

        rv = bcmi_ft_flowchecker_alu_base_control_set(unit, id,
                                                      info, alu_base_ctrlfmt);
        BCMI_CLEANUP_IF_ERROR(rv);

        soc_format_field_set(unit, BSC_TL_DG_TO_DT_ALU16_CONTROL_BUSfmt,
                    alu16_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

        if (info->flowchecker_id > 0) {
            /*
             * The update for an element can still be done without performing
             * any check. For example a template can just tell which element
             * need to be updated. Therefore other than check elements, all
             * other flow checker elements should be updated even without
             * a flowchecker id.
             */
            BCM_IF_ERROR_RETURN
               (bcmi_ft_flowchecker_get(unit, info->flowchecker_id, &fc_info));

            dual_operation = bcmi_ft_flowchecker_opr_is_dual(unit,
                        fc_info.check1.operation);

            bcmi_ft_group_alu_attrib_shift_bits_get(unit,
                    dual_operation, info, &shift_bits, &shift_bits2);

            bcmi_ft_flowchecker_values_get(unit, &fc_info, &val1, &val2,
                                                            &opr1, &opr2);

            if (opr1 != -1) {

                min_value = (val1 << shift_bits);
                fid_len = soc_format_field_length(unit,
                            BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                            CHECK_0_THRESHOLDf);
                fid_value = ((1 << fid_len) -1) ;
                if ((min_value < 0) || (min_value > fid_value)) {
                    return BCM_E_PARAM;
                }

                soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                            alu16_specific_ctrlfmt, CHECK_0_THRESHOLDf,
                            min_value);
            }

            if (opr2 != -1) {

                max_value = (val2 << shift_bits2);
                fid_len = soc_format_field_length(unit,
                            BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                            CHECK_1_THRESHOLDf);
                fid_value = ((1 << fid_len) -1) ;
                if ((max_value < 0) || (max_value > fid_value)) {
                    rv = BCM_E_PARAM;
                    goto cleanup;
                }

                soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                        alu16_specific_ctrlfmt, CHECK_1_THRESHOLDf,
                        max_value);
            }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                bcmi_ftv3_alu_check_attr_select_get(unit,
                        &fc_info, &check1_attr_sel,
                        &check2_attr_sel, &update_attr_sel);
                if ((check1_attr_sel == BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT) ||
                    (check2_attr_sel == BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT) ||
                    (update_attr_sel == BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT)) {
                    soc_format_field32_set(unit,
                            BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                            alu16_specific_ctrlfmt, DISABLE_SOP_EVENT_TRIGGERf,
                            1);
                }
            }
#endif
            if (fc_info.export_info.export_check_threshold > 0) {

                shift_bits2 = 0;
                if (!(info->flags &  BCMI_FT_ALU_LOAD_DATA_SHIFT_APPLIED)) {
                    bcmi_ft_group_alu_action_shift_bits_get(unit,
                            fc_info.action_info.action, info, &shift_bits2);
                }

                threshold =
                      fc_info.export_info.export_check_threshold << shift_bits2;

                fid_len = soc_format_field_length(unit,
                            BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                            EXPORT_THRESHOLDf);
                fid_value = ((1 << fid_len) -1) ;
                if ((threshold < 0) || (threshold > fid_value)) {
                    rv = BCM_E_PARAM;
                    goto cleanup;
                }

                soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                        alu16_specific_ctrlfmt, EXPORT_THRESHOLDf,
                        threshold);
           }
        }

        soc_format_field_set(unit, BSC_TL_DG_TO_DT_ALU16_CONTROL_BUSfmt,
            alu16_ctrlfmt, ALU16_SPECIFIC_CONTROL_BUSf, alu16_specific_ctrlfmt);

        soc_mem_field_set(unit, mem, ((uint32 *) &alu16_entry), DATAf,
                                                            alu16_ctrlfmt);

        /* The profile is set and now we need to write into the idx of group. */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, new_index, &alu16_entry);

        BCMI_CLEANUP_IF_ERROR(rv);
    }

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "ALU allocated : index = %d, Type = %s\n"),
                        index, alu_type_str[bcmiFtAluLoadTypeAlu16]));

    if ((index != -1)) {
        /* Assign the bitmap to this index. */
        SHR_BITSET((BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, bcmiFtAluLoadTypeAlu16)),
                                                                    index);

        /* also increment the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeAlu16, index);

        /* Set the groups IN use ALU memories. */
        (BCMI_FT_GROUP_ALU16_MEM_USE(unit, id))[a_idx] = 1;

        /* Update chip debug memory also if aplicable. */
        bcmi_ft_chip_debug_use_mem_update(unit,
                    bcmiFtAluLoadTypeAlu16, a_idx, 1);

        /* Set pdd profile entry for this alu entry. */
        soc_mem_field32_set(unit, mem,
                (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                (BCMI_FT_ALU_LOAD_PDD_INFO(unit,
                 bcmiFtAluLoadTypeAlu16)[a_idx]).param_id, 1);

        rv = soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                          MEM_BLOCK_ANY, id, &dg_entry);
        BCMI_CLEANUP1_IF_ERROR(rv);

        soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_entry,
                        bcmi_dg_group_alu16_fid[a_idx], new_index);

        rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ALL,
                                                      id, &dg_entry);
    }

    if (BCM_SUCCESS(rv)) {
        info->flags |= BCMI_FT_ALU_LOAD_HW_INSTALLED;
        return rv;
    }

cleanup1:
    if (index != -1) {

        /* Also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeAlu16, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT(unit, bcmiFtAluLoadTypeAlu16, index))) {

            /* Clear the bitmap to this index. */
            SHR_BITCLR(
                (BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, bcmiFtAluLoadTypeAlu16)),
                                                                        index);
        }
    }

cleanup:
    /* remove the previously added hash table entry. */
    bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeAlu16,
        info, NULL, &index);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_alu32_entry_uninstall
 * Purpose:
 *   clear ALU32 entry in hardware
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   info - (IN) ALU information of group.
 *   type - (IN) ALU/LOAD type.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu32_entry_uninstall(int unit,
                              bcm_flowtracker_group_t id,
                              bcmi_ft_group_alu_info_t *info,
                              int *indexes)
{
    int rv = BCM_E_NONE;
    int bit = 0;
    int index = -1;
    int new_index = -1;
    soc_mem_t mem = -1;
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;
    bcmi_ft_alu_load_type_t alu_load_type;

    /* This entry is yet to be installed, so nothing to uninstall. */
    if (!info->alu_load_index && !info->alu_load_type) {
        return BCM_E_NONE;
    }

    index = info->alu_load_index;
    alu_load_type = info->alu_load_type;

    /* Decrement the ref count on this index. */
    BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, alu_load_type, index);

    /* First conver the index into the memory type. */
    bcmi_ft_alu_load_mem_index_get(unit, index, alu_load_type,
                                                &new_index, &mem, &bit);

    if (!(BCMI_FT_ALU_LOAD_REFCOUNT(unit, alu_load_type, index))) {

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        /* clear attribute shift & mask entry if set */
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            rv = bcmi_ftv2_group_alu32_mask_clear(unit, alu_load_type,
                                                        mem, new_index);
            if (BCM_FAILURE(rv)) {
                if (rv != BCM_E_NOT_FOUND) {
                    return rv;
                }
            }
        }
#endif

        sal_memset(&alu32_entry, 0,
                    sizeof(bsc_dg_group_alu32_profile_0_entry_t));

        /* The profile is set and now we need to write into the idx of group. */
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, new_index, &alu32_entry));

        SHR_BITCLR(
            (BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, alu_load_type)), index);

        /* We already found the entry above so this should always pass. */
        BCM_IF_ERROR_RETURN(
            bcmi_ft_alu_hash_remove(unit, id, alu_load_type,
                                                info, NULL, &index));
    }

    /* Remove this memory from group. */
    rv = bcmi_ft_group_type_mem_get(unit, 0,
                BCMI_FT_GROUP_TYPE_GET(id), &mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Remove this field from group. */
    soc_mem_field32_modify(unit, mem, BCMI_FT_GROUP_INDEX_GET(id),
            BCMI_FT_ALU_LOAD_GROUP_FID_INFO(unit, alu_load_type)[bit], 0);

    /* Set the groups IN use ALU memories. */
    (BCMI_FT_GROUP_ALU32_MEM_USE(unit, id))[bit] = 0;

    /* Update chip debug memory also if aplicable. */
    bcmi_ft_chip_debug_use_mem_update(unit, alu_load_type, bit, -1);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        rv = bcmi_ftv3_flow_transition_trigger_update(unit,
                alu_load_type, index, info, 0);
    }
#endif

    if (BCM_SUCCESS(rv)) {
        info->flags &= (~(BCMI_FT_ALU_LOAD_HW_INSTALLED));
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_alu16_entry_uninstall
 * Purpose:
 *   clear ALU16 entry in hardware
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   info - (IN) ALU information of group.
 *   type - (IN) ALU/LOAD type.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu16_entry_uninstall(int unit,
                              bcm_flowtracker_group_t id,
                              bcmi_ft_group_alu_info_t *info,
                              int *indexes)
{
    int bit = 0;
    int index = -1;
    int new_index = -1;
    soc_mem_t mem = -1;
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    int rv = BCM_E_NONE;
#endif

    /* This entry is yet to be installed, so nothing to uninstall. */
    if (!info->alu_load_index && !info->alu_load_type) {
        return BCM_E_NONE;
    }

    index = info->alu_load_index;

    /* Decrement the ref count on this index. */
    BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeAlu16, index);

    /* First conver the index into the memory type. */
    bcmi_ft_alu_load_mem_index_get(unit, index, bcmiFtAluLoadTypeAlu16,
                                                &new_index, &mem, &bit);

    if (!(BCMI_FT_ALU_LOAD_REFCOUNT(unit, bcmiFtAluLoadTypeAlu16, index))) {

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        /* clear attribute shift & mask entry if set */
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            rv = bcmi_ftv2_group_alu16_mask_clear(unit, mem, new_index);
            if (BCM_FAILURE(rv)) {
                if (rv != BCM_E_NOT_FOUND) {
                    return rv;
                }
            }
        }
#endif

        sal_memset(&alu16_entry, 0, sizeof(bsc_dg_group_alu16_profile_0_entry_t));

        /* The profile is set and now we need to write into the idx of group. */
        BCM_IF_ERROR_RETURN
           (soc_mem_write(unit, mem, MEM_BLOCK_ALL, new_index, &alu16_entry));

        SHR_BITCLR
          ((BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, bcmiFtAluLoadTypeAlu16)), index);

        /* We already found the entry above so this should always pass. */
        BCM_IF_ERROR_RETURN(
            bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeAlu16,
                                                  info, NULL, &index));
    }

    /* Remove this memory from group. */
    soc_mem_field32_modify(unit, BSC_DG_GROUP_TABLEm, id,
                                bcmi_dg_group_alu16_fid[bit], 0);

    /* Set the groups IN use ALU memories. */
    (BCMI_FT_GROUP_ALU16_MEM_USE(unit, id))[bit] = 0;

    /* Update chip debug memory also if aplicable. */
    bcmi_ft_chip_debug_use_mem_update(unit,
                    bcmiFtAluLoadTypeAlu16, bit, -1);

    info->flags &= ~BCMI_FT_ALU_LOAD_HW_INSTALLED;

    return BCM_E_NONE;
}

/*|************************************************************|
  |********         ALU Hardware Install Methods.      ********|
  |************************************************************|
  |*******                   End                       ********|
  |************************************************************|*/


/******************************************************
 |                  LOAD16 Space                       |
 */
/*
 * Function:
 *   bcmi_ft_load16_entry_match
 * Purpose:
 *   Match Load16 entry in hardware
 * Parameters:
 *   unit            - (IN) BCM device id
 *   id              - (IN) FT group id.
 *   info            - (IN) ALU/Load information of group.
 *   load16_indexes  - (IN) Load16 indexes in group
 *   index           - (IN) sw index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load16_entry_match(int unit,
                           bcm_flowtracker_group_t id,
                           bcmi_ft_group_alu_info_t *info,
                           bcmi_ft_alu_hash_info_t *alu_info,
                           int *load16_indexes, int *index)
{
    int i = 0, j = 0, max = 0;
    int match_found = 0, load_num = 0;
    uint32 load16_ctrlfmt[1];
    uint32 load_mem_vector = 0;
    uint32 location = 0, trigger = 0;
    bcmi_ft_group_alu_info_t *local = NULL;
    bsc_dg_group_load16_profile_entry_t load16_entry;
    int data[TOTAL_GROUP_LOAD16_DATA_NUM] =
                   {DATA_0f,
                    DATA_1f,
                    DATA_2f,
                    DATA_3f,
                    DATA_4f,
                    DATA_5f,
                    DATA_6f,
                    DATA_7f,
                    DATA_8f,
                    DATA_9f,
                    DATA_10f,
                    DATA_11f,
                    DATA_12f,
                    DATA_13f,
                    DATA_14f,
                    DATA_15f};

    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));
    sal_memset(load16_ctrlfmt, 0, sizeof(load16_ctrlfmt));

    /* Set the max for whole table if whole table is to be checked. */
    max = soc_mem_index_max(unit, BSC_DG_GROUP_LOAD16_PROFILEm);

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        bcmi_ftv2_get_load_mem_vector(unit, id, bcmiFtAluLoadTypeLoad16,
                                      load16_indexes, &load_mem_vector);
        if (alu_info->hash_load_mem_vector != load_mem_vector) {
            return BCM_E_NOT_FOUND;
        }
    }
#endif

    /* If an index is provided then match only that index. */
    if (*index != -1) {
        max = *index;
        i = *index;
    }

    for (; i <= max; i++) {

        match_found = 1;
        load_mem_vector = 0;
        sal_memset(&load16_entry, 0,
                   sizeof(bsc_dg_group_load16_profile_entry_t));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                                           MEM_BLOCK_ANY, i, &load16_entry));

        for (; j < TOTAL_GROUP_LOAD16_DATA_NUM; j++) {

            if (load16_indexes[j] != -1) {

                local = (&(info[load16_indexes[j]]));
                for (load_num = 0;
                     load_num < TOTAL_GROUP_LOAD16_DATA_NUM; load_num++) {
                    if (!(load_mem_vector & (1 << load_num))) {
                        if (local->alu_ft_type !=
                              BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                    bcmiFtAluLoadTypeLoad16, 0, load_num)) {
                            continue;
                        }
                        load_mem_vector |= (1 << load_num);
                        break;
                    }
                }
                /* should not happen preventive check */
                if (load_num == TOTAL_GROUP_LOAD16_DATA_NUM) {
                    return BCM_E_INTERNAL;
                }

                sal_memset(load16_ctrlfmt, 0, sizeof(uint32));
                soc_mem_field_get(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                                 (uint32 *) &load16_entry, data[load_num],
                                  load16_ctrlfmt);
                trigger = soc_format_field32_get(unit,
                                        BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                                        load16_ctrlfmt, LOAD_TRIGGERf);
                location = soc_format_field32_get(unit,
                                        BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                                        load16_ctrlfmt, ATTR_SELECTf);
                if ((trigger != BCMI_FT_GROUP_FTFP_DATA(unit, id).direction) ||
                    (location != (local->key1.location / 16))) {
                    match_found = 0;
                    break;
                }

            } else {

                for (load_num = 0;
                     load_num < TOTAL_GROUP_LOAD16_DATA_NUM; load_num++) {
                    if (!(load_mem_vector & (1 << load_num))) {
                        load_mem_vector |= (1 << load_num);
                        break;
                    }
                }
                /* should not happen preventive check */
                if (load_num == TOTAL_GROUP_LOAD16_DATA_NUM) {
                    return BCM_E_INTERNAL;
                }

                sal_memset(load16_ctrlfmt, 0, sizeof(uint32));
                soc_mem_field_get(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                                 (uint32 *) &load16_entry, data[load_num],
                                  load16_ctrlfmt);
                trigger = soc_format_field32_get(unit,
                                    BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                                    load16_ctrlfmt, LOAD_TRIGGERf);
                location = soc_format_field32_get(unit,
                                    BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                                    load16_ctrlfmt, ATTR_SELECTf);
                if ((trigger != 0) || (location != 0)) {
                    match_found = 0;
                    break;
                }
            }
        }

        if (!match_found) {
            continue;
        }

        break;
    }

    if (i <= max) {
        *index = i;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ft_load16_entry_write
 * Purpose:
 *   Write Load16 entry in hardware
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) FT group id.
 *   info           - (IN) ALU/Load information of group.
 *   load16_indexes - (IN) Load16 indexes in group
 *   index          - (IN) sw index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load16_entry_write(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *load16_indexes, int index)
{
    int j = 0,load_num = 0;
    uint32 location = 0;
    uint32 load16_ctrlfmt[1];
    uint16 load_mem_vector = 0;
    bcmi_ft_group_alu_info_t *local = NULL;
    bsc_dg_group_load16_profile_entry_t load16_entry;
    bcm_flowtracker_direction_t direction = bcmFlowtrackerFlowDirectionNone;
    int data[TOTAL_GROUP_LOAD16_DATA_NUM] =
                   {DATA_0f,
                    DATA_1f,
                    DATA_2f,
                    DATA_3f,
                    DATA_4f,
                    DATA_5f,
                    DATA_6f,
                    DATA_7f,
                    DATA_8f,
                    DATA_9f,
                    DATA_10f,
                    DATA_11f,
                    DATA_12f,
                    DATA_13f,
                    DATA_14f,
                    DATA_15f};

    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));
    sal_memset(load16_ctrlfmt, 0, sizeof(load16_ctrlfmt));

    for (; j < TOTAL_GROUP_LOAD16_DATA_NUM; j++) {

        /* Now based on the incoming info, match the entries. */
        if (load16_indexes[j] == -1) {
            break;
        }

        local = (&(info[load16_indexes[j]]));
        direction = local->direction;

        /* Update the load trigger based on direction configuration. */
        if (direction == bcmFlowtrackerFlowDirectionNone) {
            soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                    load16_ctrlfmt, LOAD_TRIGGERf,
                    BCMI_FT_GROUP_FTFP_DATA(unit, id).direction);
        } else {
            soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                    load16_ctrlfmt, LOAD_TRIGGERf, direction);
        }

        location = local->key1.location;

        /*
         * ftfp is sending location in continuous index of 256B.
         * Each extractor here is 16 bytes so long, so
         * need to divide it by 16 to get correct location.
         */
        location = (location / 16);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                                    load16_ctrlfmt, ATTR_SELECTf, location);

        if (local->flags & BCMI_FT_ALU_LOAD_STICKYOR_UPDATE) {
            soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                                                load16_ctrlfmt, LOAD_TYPEf, 1);
        } else {
            soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                                                load16_ctrlfmt, LOAD_TYPEf, 0);
        }

        for (load_num = 0; load_num < TOTAL_GROUP_LOAD16_DATA_NUM; load_num++) {
            if (!(load_mem_vector & (1 << load_num))) {
                if (local->alu_ft_type !=
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                            bcmiFtAluLoadTypeLoad16, 0, load_num)) {
                    continue;
                }
                load_mem_vector |= (1 << load_num);
                break;
            }
        }

        /* should not happen as number of incoming loads
         * is validated against maximum before. this is just
         * a preventive check */
        if (load_num == TOTAL_GROUP_LOAD16_DATA_NUM) {
            return BCM_E_RESOURCE;
        }

        /* Write into that entry. */
        soc_mem_field_set(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                         (uint32 *) &load16_entry, data[load_num],
                          load16_ctrlfmt);
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                             MEM_BLOCK_ANY, index, &load16_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_load16_entry_install
 * Purpose:
 *   Install Load16 entry in hardware
 * Parameters:
 *   unit     - (IN) BCM device id
 *   id       - (IN) FT group id.
 *   info     - (IN) ALU information of group
 *   indexes  - (IN) Load 16 indexes in group
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load16_entry_install(int unit,
                        bcm_flowtracker_group_t id,
                        bcmi_ft_group_alu_info_t *info,
                        int *indexes)
{
    int rv = BCM_E_NONE;
    int j = 0, index = -1;
    bsc_dg_group_table_entry_t dg_entry;
    bcmi_ft_group_alu_info_t *temp = NULL, *local = NULL;

    /* There is no load16 entry */
    if (indexes[0] == -1) {
        return BCM_E_NONE;
    }

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    rv = bcmi_ft_alu_hash_insert
                (unit, id, bcmiFtAluLoadTypeLoad16, temp, indexes, &index);
    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            /* Increment ref count and set up group table. */
            BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeLoad16,
                index);
            rv = BCM_E_NONE;
        } else {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "LOAD16 allocate Failed: rv = %d"
                                "\n"), rv));
            return rv;
        }
    } else {
        /* Now that we have got the index, now write into that index. */
        rv  = bcmi_ft_load16_entry_write(unit, id, temp, indexes, index);

        BCMI_CLEANUP_IF_ERROR(rv);

        if (index != -1) {
            /* Assign the bitmap to this index. */
            SHR_BITSET((BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, bcmiFtAluLoadTypeLoad16)),
                index);

            /* Also increment the ref count on this index. */
            BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeLoad16,
                index);

        }
    }

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "ALU allocated : index = %d, Type = %s\n"),
        index, alu_type_str[bcmiFtAluLoadTypeLoad16]));

    rv = bcmi_ft_load_pdd_entry_set
            (unit, id, bcmiFtAluLoadTypeLoad16, index, 0);

    BCMI_CLEANUP1_IF_ERROR(rv);

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm,
         ((uint32 *)&dg_entry), GROUP_LOAD16_PROFILE_IDXf, index);

    rv  = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id,
         &dg_entry);

    BCMI_CLEANUP1_IF_ERROR(rv);

    /* update type and index into all the data elements. */
    for (j = 0; j < TOTAL_GROUP_LOAD16_DATA_NUM; j++) {
        if (indexes[j] == -1) {
            break;
        }

        local = (&(temp[indexes[j]]));
        local->alu_load_index = index;
        local->alu_load_type = bcmiFtAluLoadTypeLoad16;
        local->flags |= BCMI_FT_ALU_LOAD_HW_INSTALLED;
    }

    return BCM_E_NONE;

cleanup1:

    if (index != -1) {
        /* also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeLoad16, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
                (unit, bcmiFtAluLoadTypeLoad16, index))) {

            /* clear the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, bcmiFtAluLoadTypeLoad16)),
                                                                         index);
        }

         bcmi_ft_load_pdd_entry_set(unit, id, bcmiFtAluLoadTypeLoad16, index, 1);
    }

cleanup:

    /* remove the previously added hash table entry. */
    bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeLoad16,
                                           info, NULL, &index);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_load16_entry_clear
 * Purpose:
 *   Clear Load 16 entry in hardware
 * Parameters:
 *   unit - (IN) BCM device id
 *   index  - (IN) Load 16 index
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load16_entry_clear(int unit, int index)
{
    bsc_dg_group_load16_profile_entry_t load16_entry;

    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                                    MEM_BLOCK_ANY, index, &load16_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_load16_entry_uninstall
 * Purpose:
 *   Uninstall Load16 entry in hardware
 * Parameters:
 *   unit          - (IN) BCM device id
 *   id            - (IN) FT group id.
 *   info          - (IN) FT group alu info.
 *   load_indexes  - (IN) Load16 indexes in group.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load16_entry_uninstall(int unit,
                               bcm_flowtracker_group_t id,
                               bcmi_ft_group_alu_info_t *info,
                               int *indexes)
{
    int rv = BCM_E_NONE;
    int index = 0, j = 0;
    bsc_dg_group_table_entry_t dg_entry;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_alu_info_t *local = NULL;

    /* There is no load16 entry */
    if (indexes[0] == -1) {
        return BCM_E_NONE;
    }

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* first get alu load index entry from the hash table. */
    rv =  bcmi_ft_alu_hash_mem_index_get(unit, id, bcmiFtAluLoadTypeLoad16,
                    BCMI_FT_GROUP_EXT_DATA_INFO(unit, id), indexes, &index);

    if (BCM_FAILURE(rv)) {
        /* first cleanup if something is configured. */
        if (rv == BCM_E_NOT_FOUND) {
            /* If this happens then it might be due to the error while adding
             * No need to do anything.just return back saying that it is clean*/
            return BCM_E_NONE;
        } else {
            return rv;
        }
    }

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                            MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_entry,
                                GROUP_LOAD16_PROFILE_IDXf, 0);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    if (BCM_SUCCESS(rv)) {

        /* also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeLoad16, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
                (unit, bcmiFtAluLoadTypeLoad16, index))) {

            /* Assign the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP_INFO
                       (unit, bcmiFtAluLoadTypeLoad16)) , index);

            /* We already found the entry above so this should always pass. */
            rv =  bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeLoad16,
                        BCMI_FT_GROUP_EXT_DATA_INFO(unit, id), indexes, &index);
            BCM_IF_ERROR_RETURN(rv);

            /* Clear the entry in the profile table. */
            rv = bcmi_ft_load16_entry_clear(unit, index);
        }
    }

    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Update type and index into all the data elements. */
    for (j = 0; j < TOTAL_GROUP_LOAD8_DATA_NUM; j++) {

        /* Now based on the incoming info, match the entries. */
        if (indexes[j] == -1) {
            break;
        }

        local = (&(temp[indexes[j]]));
        local->flags &= ~BCMI_FT_ALU_LOAD_HW_INSTALLED;
    }

    return rv;
}

/******************************************************
 |                  LOAD8 Space                       |
 */
/*
 * Function:
 *   bcmi_ft_load8_entry_match
 * Purpose:
 *   Match load 8 entry in hardware
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) FT group id.
 *   info           - (IN) ALU/Load information of group.
 *   load8_indexes  - (IN) Load8 indexes in group
 *   index          - (IN) sw index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load8_entry_match(int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                bcmi_ft_alu_hash_info_t *alu_info,
                int *load8_indexes, int *index)
{
    int i = 0, j = 0, max = 0, match_found = 0, load_num = 0;
    uint32 load8_ctrlfmt[1];
    uint32 load_mem_vector = 0;
    uint32 location = 0, trigger = 0;
    bcmi_ft_group_alu_info_t *local = NULL;
    bsc_dg_group_load8_profile_entry_t load8_entry;
    int data[TOTAL_GROUP_LOAD8_DATA_NUM] =
       {DATA_0f,
        DATA_1f,
        DATA_2f,
        DATA_3f,
        DATA_4f,
        DATA_5f,
        DATA_6f,
        DATA_7f};

    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));
    sal_memset(load8_ctrlfmt, 0, sizeof(load8_ctrlfmt));

    max = soc_mem_index_max(unit, BSC_DG_GROUP_LOAD8_PROFILEm);

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        bcmi_ftv2_get_load_mem_vector(unit, id, bcmiFtAluLoadTypeLoad8,
                                      load8_indexes, &load_mem_vector);
        if (alu_info->hash_load_mem_vector != load_mem_vector) {
            return BCM_E_NOT_FOUND;
        }
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    /* if the index is provided then match for only that index. */
    if (*index != -1) {
        max = *index;
        i = *index;
    }

    for (; i <= max; i++) {
        match_found = 1;
        load_mem_vector = 0;
        sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                                           MEM_BLOCK_ANY, i, &load8_entry));

        for (; j < TOTAL_GROUP_LOAD8_DATA_NUM; j++) {

            if (load8_indexes[j] != -1) {

                local = (&(info[load8_indexes[j]]));
                for (load_num = 0;
                     load_num < TOTAL_GROUP_LOAD8_DATA_NUM; load_num++) {
                    if (!(load_mem_vector & (1 << load_num))) {
                        if (local->alu_ft_type !=
                                BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                  bcmiFtAluLoadTypeLoad8, 0, load_num)) {
                            continue;
                        }
                        load_mem_vector |= (1 << load_num);
                        break;
                    }
                }
                /* should not happen preventive check */
                if (load_num == TOTAL_GROUP_LOAD8_DATA_NUM) {
                    return BCM_E_INTERNAL;
                }

                sal_memset(load8_ctrlfmt, 0, sizeof(uint32));
                soc_mem_field_get(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                                 (uint32 *) &load8_entry, data[load_num],
                                  load8_ctrlfmt);
                trigger = soc_format_field32_get(unit,
                                    BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                                    load8_ctrlfmt, LOAD_TRIGGERf);
                location = soc_format_field32_get(unit,
                                BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                                load8_ctrlfmt, ATTR_SELECTf);
                if ((trigger != BCMI_FT_GROUP_FTFP_DATA(unit, id).direction) ||
                    (location != (local->key1.location / 8))) {
                    match_found = 0;
                    break;
                }

            } else {

                for (load_num = 0;
                     load_num < TOTAL_GROUP_LOAD8_DATA_NUM; load_num++) {
                    if (!(load_mem_vector & (1 << load_num))) {
                        load_mem_vector |= (1 << load_num);
                        break;
                    }
                }
                /* should not happen preventive check */
                if (load_num == TOTAL_GROUP_LOAD8_DATA_NUM) {
                    return BCM_E_INTERNAL;
                }

                sal_memset(load8_ctrlfmt, 0, sizeof(uint32));
                soc_mem_field_get(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                                 (uint32 *) &load8_entry, data[load_num],
                                  load8_ctrlfmt);
                trigger = soc_format_field32_get(unit,
                                        BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                                        load8_ctrlfmt, LOAD_TRIGGERf);
                location = soc_format_field32_get(unit,
                                        BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                                        load8_ctrlfmt, ATTR_SELECTf);
                if ((trigger != 0) || location != 0) {
                    match_found = 0;
                    break;
                }
            }
        }

        if (!match_found) {
            continue;
        }

        break;
    }

    if (i <= max) {
        *index = i;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ft_load8_entry_write
 * Purpose:
 *   Write Load8 entry in hardware
 * Parameters:
 *   unit          - (IN) BCM device id
 *   id            - (IN) FT group id.
 *   info          - (IN) ALU/Load information of group.
 *   load8_indexes - (IN) Load8 indexes in group
 *   index         - (IN) sw index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load8_entry_write(int unit,
                          bcm_flowtracker_group_t id,
                          bcmi_ft_group_alu_info_t *info,
                          int *load8_indexes, int index)
{
    int j = 0, load_num = 0;
    uint32 location = 0;
    uint32 load8_ctrlfmt[1];
    uint16 load_mem_vector = 0;
    bcmi_ft_group_alu_info_t *local = NULL;
    bsc_dg_group_load8_profile_entry_t load8_entry;
    bcm_flowtracker_direction_t direction = bcmFlowtrackerFlowDirectionNone;
    int data[TOTAL_GROUP_LOAD8_DATA_NUM] =
                   {DATA_0f,
                    DATA_1f,
                    DATA_2f,
                    DATA_3f,
                    DATA_4f,
                    DATA_5f,
                    DATA_6f,
                    DATA_7f};

    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));
    sal_memset(load8_ctrlfmt, 0, sizeof(load8_ctrlfmt));

    for (; j < TOTAL_GROUP_LOAD8_DATA_NUM; j++) {

        /* Now based on the incoming info, match the entries. */
        if (load8_indexes[j] == -1) {
            break;
        }

        local = (&(info[load8_indexes[j]]));
        direction = local->direction;

        if (direction == bcmFlowtrackerFlowDirectionNone) {
            soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                    load8_ctrlfmt, LOAD_TRIGGERf,
                    BCMI_FT_GROUP_FTFP_DATA(unit, id).direction);
        } else {
            soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                    load8_ctrlfmt, LOAD_TRIGGERf, direction);
        }

        location = local->key1.location;

        /* if location is not at 8B boundary then return error. */
        if (location % 8) {
            return BCM_E_PARAM;
        }

        /*
         * ftfp is sending location in continuous index of 256B.
         * Each extractor here is 8 bytes so long, so
         * need to divide it by 8 to get correct location.
         */
        location = (location / 8);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                                    load8_ctrlfmt, ATTR_SELECTf, location);

        if (local->flags & BCMI_FT_ALU_LOAD_STICKYOR_UPDATE) {
            soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                                                 load8_ctrlfmt, LOAD_TYPEf, 1);
        } else {
            soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                                                 load8_ctrlfmt, LOAD_TYPEf, 0);
        }

        for (load_num = 0; load_num < TOTAL_GROUP_LOAD8_DATA_NUM; load_num++) {
            if (!(load_mem_vector & (1 << load_num))) {
                if (local->alu_ft_type !=
                        BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                bcmiFtAluLoadTypeLoad8, 0, load_num)) {
                    continue;
                }
                load_mem_vector |= (1 << load_num);
                break;
            }
        }

        /* should not happen as number of incoming loads
         * is validated against maximum before. this is just
         * a preventive check */
        if (load_num == TOTAL_GROUP_LOAD8_DATA_NUM) {
            return BCM_E_RESOURCE;
        }

        /* Write into that entry. */
        soc_mem_field_set(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                    (uint32 *) &load8_entry, data[load_num], load8_ctrlfmt);
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                             MEM_BLOCK_ANY, index, &load8_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_load8_entry_install
 * Purpose:
 *   Install Load8 entry in hardware
 * Parameters:
 *   unit     - (IN) BCM device id
 *   id       - (IN) FT group id.
 *   info     - (IN) ALU information of group
 *   indexes  - (IN) Load 8 indexes in group
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load8_entry_install(int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes)
{
    int rv = BCM_E_NONE;
    int j = 0, index = -1;
    bsc_dg_group_table_entry_t dg_entry;
    bcmi_ft_group_alu_info_t *temp = NULL, *local = NULL;

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* There is no load8 entry. */
    if (indexes[0] == -1) {
        return BCM_E_NONE;
    }

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    rv = bcmi_ft_alu_hash_insert
            (unit, id, bcmiFtAluLoadTypeLoad8, temp, indexes, &index);

    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {

            /* Increment ref count and set up group table. */
            BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeLoad8, index);
            rv = BCM_E_NONE;

        } else {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "LOAD8 allocate Failed: rv = %d"
                                "\n"), rv));
            return rv;
        }

    } else {

        /* Now that we have got the index, now write into that index. */
        rv  = bcmi_ft_load8_entry_write(unit, id, temp, indexes, index);
        BCMI_CLEANUP_IF_ERROR(rv);

        if (index != -1) {
            /* assign the bitmap to this index */
            SHR_BITSET(
                (BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, bcmiFtAluLoadTypeLoad8)),
                                                                        index);
            /* also increment the ref count on this index */
            BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeLoad8, index);
        }
    }

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "ALU allocated : index = %d, Type = %s\n"),
        index, alu_type_str[bcmiFtAluLoadTypeLoad8]));

    rv = bcmi_ft_load_pdd_entry_set(unit, id, bcmiFtAluLoadTypeLoad8, index, 0);
    BCMI_CLEANUP1_IF_ERROR(rv);

    /* First read the group entry. */
    rv = soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                            MEM_BLOCK_ANY, id, &dg_entry);
    BCMI_CLEANUP1_IF_ERROR(rv);

    /* now write this index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, ((uint32 *)&dg_entry),
                                         GROUP_LOAD8_PROFILE_IDXf, index);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);
    BCMI_CLEANUP1_IF_ERROR(rv);

    /* Update type and index into all the data elements. */
    for (j = 0; j < TOTAL_GROUP_LOAD8_DATA_NUM; j++) {

        /* Now based on the incoming info, match the entries. */
        if (indexes[j] == -1) {
            break;
        }

        local = (&(temp[indexes[j]]));
        local->alu_load_index = index;
        local->alu_load_type = bcmiFtAluLoadTypeLoad8;
        local->flags |= BCMI_FT_ALU_LOAD_HW_INSTALLED;
    }

    return BCM_E_NONE;

cleanup1:

    if (index != -1) {

        /* also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeLoad8, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT(unit, bcmiFtAluLoadTypeLoad8, index))) {

            /* clear the bitmap to this index. */
            SHR_BITCLR(
                (BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, bcmiFtAluLoadTypeLoad8)),
                                                                         index);
        }

         bcmi_ft_load_pdd_entry_set(unit, id, bcmiFtAluLoadTypeLoad8, index, 1);
    }

cleanup:

    /* remove the previously added hash table entry. */
    bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeLoad8,
                                            info, NULL, &index);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_load8_entry_clear
 * Purpose:
 *   Clear Load8 entry in hardware
 * Parameters:
 *   unit   - (IN) BCM device id
 *   index  - (IN) Load8 index
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load8_entry_clear(int unit, int index)
{
    bsc_dg_group_load8_profile_entry_t load8_entry;

    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                                    MEM_BLOCK_ANY, index, &load8_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_load8_entry_uninstall
 * Purpose:
 *   uninstall Load8 entry in hardware
 * Parameters:
 *   unit     - (IN) BCM device id
 *   id       - (IN) FT group id.
 *   indexes  - (IN) Load8 indexes in group
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_load8_entry_uninstall(int unit,
                              bcm_flowtracker_group_t id,
                              bcmi_ft_group_alu_info_t *info,
                              int *indexes)
{
    int rv = BCM_E_NONE;
    int index = 0, j = 0;
    bsc_dg_group_table_entry_t dg_entry;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_alu_info_t *local = NULL;

    /* there is no load8 entry */
    if (indexes[0] == -1) {
        return BCM_E_NONE;
    }

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* first get alu load index from the hash table. */
    rv = bcmi_ft_alu_hash_mem_index_get(unit, id, bcmiFtAluLoadTypeLoad8,
                  BCMI_FT_GROUP_EXT_DATA_INFO(unit, id), indexes, &index);

    if (BCM_FAILURE(rv)) {
        /* first cleanup if something is configured. */
        if (rv == BCM_E_NOT_FOUND) {
            /* If this happens then it might be due to the error while adding.
             * No need to do anything.just return back saying that it is clean*/
            return BCM_E_NONE;
        } else {
            return rv;
        }
    }

    /* Then clear the group table index. */
    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_entry,
                                    GROUP_LOAD8_PROFILE_IDXf, 0);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    if (BCM_SUCCESS(rv)) {

        /* also decrement the ref count on this index */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeLoad8, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
                (unit, bcmiFtAluLoadTypeLoad8, index))) {

            /* assign the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP_INFO
                       (unit, bcmiFtAluLoadTypeLoad8)) , index);

            /* we already found the entry above so this should always pass */
            rv =  bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeLoad8,
                        BCMI_FT_GROUP_EXT_DATA_INFO(unit, id), indexes, &index);
            BCM_IF_ERROR_RETURN(rv);

            /* clear the entry in the profile table. */
            rv = bcmi_ft_load8_entry_clear(unit, index);
        }
    }

    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Update type and index into all the data elements. */
    for (j = 0; j < TOTAL_GROUP_LOAD8_DATA_NUM; j++) {

        /* Now based on the incoming info, match the entries. */
        if (indexes[j] == -1) {
            break;
        }

        local = (&(temp[indexes[j]]));
        local->flags &= ~BCMI_FT_ALU_LOAD_HW_INSTALLED;
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_alu_load_index_match
 * Purpose:
 *   Match ALU index for this group.
 * Description:
 *   Even if we may get exact same ALU
 *   which is matching to some existing entry
 *   in hash table, we may not use it if
 *   THat particular ALU memory is used by this
 *   group. So in that case,
 *   we may have to get another index and then
 *   while removing, we may have to make sure this
 *   group has correct index.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   alu_load_type - (IN) ALU/LOAD type.
 *   alu_info - (IN) ALU hash information of group.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_alu_load_index_match(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_alu_load_type_t alu_load_type,
            bcmi_ft_alu_hash_info_t *alu_info)
{
    int index = -1;
    int new_index = -1;
    int a_idx = -1; /* Array index of field/memory array. */
    soc_mem_t mem;
    uint32 *in_use_array = NULL;

    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        in_use_array = BCMI_FT_GROUP_ALU16_MEM_USE(unit, id);
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
    } else if (alu_load_type == bcmiFtAluLoadTypeAggAlu32) {
        in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
    }

    /* Get the index from alu hash info. */
    index = alu_info->alu_load_index;

    if ((alu_load_type != bcmiFtAluLoadTypeAlu32) &&
        (alu_load_type != bcmiFtAluLoadTypeAlu16) &&
        (alu_load_type != bcmiFtAluLoadTypeAggAlu32)) {
       return BCM_E_NOT_FOUND;
    }

    /* Convert global ALU number into hw memory and index. */
    bcmi_ft_alu_load_mem_index_get
         (unit, index, alu_load_type, &new_index, &mem, &a_idx);

    if (new_index == -1) {
       return BCM_E_NOT_FOUND;
    }

    if (in_use_array[a_idx]) {
        /* This memory is already used in group. cant use it. */
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_ft_mode_get
 * Purpose:
 *   Set single/Double mode after checking session data.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_ft_mode_update(int unit,
                bcm_flowtracker_group_t id)
{
    int i = 0;
    soc_mem_t mem;
    int num_alus_loads = 0;
    int length, total_length;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_key_data_mode_t mode = bcmiFtGroupModeSingle;

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    /* Read the group state for the ALU inforamtion. */
    if (!(BCMI_FT_GROUP_EXT_DATA_OR_TS_TRIGGERS_SET(unit, id))) {
        return BCM_E_INTERNAL;
    }

    length = total_length = 0;

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Get total number of alus in this chunk. */
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* allocate the bitmap based on the type, length */
    for (i=0; i<num_alus_loads; i++) {

        length = temp->key1.length;
        if ((temp->key1.is_alu) && ((length >=8) && (length < 16))) {
            /*
             * if ALU is required but we have less than 16 bit of length
             * then this might be case where extractor is not aligned properly.
             * Hence we will change length accordingly to pick correct alu type.
             */
            length = 16;
        }
        total_length += length;
        temp++;
    }

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_FLOW_METERf)) {
        total_length += 48;
    }
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_FLOW_COUNT_TO_CPUf)) {
        total_length += 16;
    }
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_FLOW_STATEf)) {
        total_length += 8;
    }

    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_0f)) {
        total_length += 48;
    }
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_1f)) {
        total_length += 48;
    }
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_2f)) {
        total_length += 48;
    }
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_3f)) {
        total_length += 48;
    }

    if (total_length > BCMI_FT_SESSION_DATA_SINGLE_LEN) {
        mode = bcmiFtGroupModeDouble;
        bcmi_ft_group_extraction_mode_set(unit, 0, id, mode);
        bcmi_ft_group_extraction_mode_set(unit, 1, id, mode);
    }

    return BCM_E_NONE;
}

/* We need to change the load triggers,
 * whenever we get the indication from user. */
int
bcmi_ft_group_alu_load_trigger_set(int unit,
                 bcm_flowtracker_group_t id,
                 bcm_flowtracker_direction_t direction)
{
    int mem_idx = 0, type = 0;
    int index = -1, total_mems = 0;
    soc_mem_t mem;
    bsc_dg_group_table_entry_t dg_entry;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &dg_entry));

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    /* For all the types of memory types. */
    for (type = 0; type < bcmiFtAluLoadTypeNone; type++) {

        pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, type);
        if (!pdd_info) {
            continue;
        }

        total_mems = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, type);

        /* For all the alu/load types, check what memories are set in pdd. */
        for (mem_idx = 0; mem_idx < total_mems; mem_idx++) {

            /* Now get if this particular field is set in pdd. */
            if (soc_mem_field32_get(unit, mem,
                        (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                        pdd_info[mem_idx].param_id)) {

                index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
                         &dg_entry,
                        (BCMI_FT_ALU_LOAD_GROUP_FID_INFO(unit, type))[mem_idx]);

                if ((type == bcmiFtAluLoadTypeAlu32) ||
                    (type == bcmiFtAluLoadTypeAggAlu32)) {
                    BCM_IF_ERROR_RETURN(
                        bcmi_ft_group_alu32_trigger_set(unit, mem_idx, index,
                                                                  direction));
                } else if (type == bcmiFtAluLoadTypeAlu16) {
                    BCM_IF_ERROR_RETURN(
                        bcmi_ft_group_alu16_trigger_set(unit, mem_idx, index,
                                                                  direction));
                } else if (type == bcmiFtAluLoadTypeLoad16) {
                    BCM_IF_ERROR_RETURN(
                        bcmi_ft_group_load16_trigger_set(unit, mem_idx, index,
                                                                   direction));
                } else if (type == bcmiFtAluLoadTypeLoad8) {
                    BCM_IF_ERROR_RETURN(
                        bcmi_ft_group_load8_trigger_set(unit, mem_idx, index,
                                                                   direction));
                } else {
                    return BCM_E_PARAM;
                }

            }
        }
    }
    return BCM_E_NONE;
}

int
bcmi_ft_group_load16_trigger_set(int unit,
                int mem_idx, int index,
                bcm_flowtracker_direction_t direction)
{
    uint32 load16_ctrlfmt[1];
    bsc_dg_group_load16_profile_entry_t load16_entry;
    int data[TOTAL_GROUP_LOAD16_DATA_NUM] =
                   {DATA_0f,
                    DATA_1f,
                    DATA_2f,
                    DATA_3f,
                    DATA_4f,
                    DATA_5f,
                    DATA_6f,
                    DATA_7f,
                    DATA_8f,
                    DATA_9f,
                    DATA_10f,
                    DATA_11f,
                    DATA_12f,
                    DATA_13f,
                    DATA_14f,
                    DATA_15f};

    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));
    sal_memset(load16_ctrlfmt, 0, sizeof(load16_ctrlfmt));

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                      MEM_BLOCK_ANY, index, &load16_entry));

    soc_mem_field_get(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
        (uint32 *) &load16_entry, data[mem_idx], load16_ctrlfmt);

    /* Update the load trigger based on groups direction configuration. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                             load16_ctrlfmt, LOAD_TRIGGERf, direction);

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                                    MEM_BLOCK_ANY, index, &load16_entry));

    return BCM_E_NONE;
}

int
bcmi_ft_group_load8_trigger_set(int unit,
                   int mem_idx, int index,
        bcm_flowtracker_direction_t direction)
{
    uint32 load8_ctrlfmt[1];
    bsc_dg_group_load8_profile_entry_t load8_entry;
    int data[TOTAL_GROUP_LOAD8_DATA_NUM] =
                   {DATA_0f,
                    DATA_1f,
                    DATA_2f,
                    DATA_3f,
                    DATA_4f,
                    DATA_5f,
                    DATA_6f,
                    DATA_7f};

    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));
    sal_memset(load8_ctrlfmt, 0, sizeof(load8_ctrlfmt));

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                      MEM_BLOCK_ANY, index, &load8_entry));

    soc_mem_field_get(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                (uint32 *) &load8_entry, data[mem_idx], load8_ctrlfmt);

    /* Update the load trigger based on groups direction configuration. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                              load8_ctrlfmt, LOAD_TRIGGERf, direction);

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                                    MEM_BLOCK_ANY, index, &load8_entry));

    return BCM_E_NONE;
}

/* For load triggers on ALU memories. */
int
bcmi_ft_group_alu32_trigger_set(int unit,
                  int mem_idx, int index,
    bcm_flowtracker_direction_t direction)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    uint32 alu_base_ctrlfmt[5];
    uint32 alu32_specific_ctrlfmt[5];
    uint32 alu32_ctrlfmt[10];
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;

    sal_memset(alu_base_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu32_specific_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu32_ctrlfmt, 0, 10 * sizeof(uint32));

    mem = bcmi_alu32_mem_info[mem_idx].mem;

    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &alu32_entry);

    soc_mem_field_get(unit, mem, (uint32 *)&alu32_entry, DATAf,
                                                alu32_ctrlfmt);

    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
            alu32_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

    /* set the load trigger for forward and reverse. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
        alu_base_ctrlfmt, LOAD_TRIGGERf, direction);

    soc_format_field_set(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
        alu32_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

    soc_mem_field_set(unit, mem, (uint32 *)&alu32_entry, DATAf,
            alu32_ctrlfmt);

    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &alu32_entry);

    return rv;
}

/* For load triggers on ALU memories. */
int
bcmi_ft_group_alu16_trigger_set(int unit,
                  int mem_idx, int index,
    bcm_flowtracker_direction_t direction)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    uint32 alu_base_ctrlfmt[5];
    uint32 alu16_specific_ctrlfmt[2];
    uint32 alu16_ctrlfmt[7];
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;

    sal_memset(alu_base_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu16_specific_ctrlfmt, 0, 2 * sizeof(uint32));
    sal_memset(alu16_ctrlfmt, 0, 7 * sizeof(uint32));

    mem = bcmi_alu16_mem_info[mem_idx].mem;

    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &alu16_entry);

    soc_mem_field_get(unit, mem, (uint32 *)&alu16_entry, DATAf,
                                                alu16_ctrlfmt);

    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU16_CONTROL_BUSfmt,
            alu16_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

    /* Set the load trigger for forward and reverse. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                            alu_base_ctrlfmt, LOAD_TRIGGERf, direction);

    soc_format_field_set(unit, BSC_TL_DG_TO_DT_ALU16_CONTROL_BUSfmt,
        alu16_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

    soc_mem_field_set(unit, mem, (uint32 *)&alu16_entry, DATAf,
                                                  alu16_ctrlfmt);

    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &alu16_entry);

    return rv;
}

/* Chip Wide Debug methods. */
int
bcmi_ft_tracking_param_in_Debug_mode(int unit,
                    bcm_flowtracker_group_t id,
                    bcmi_ft_group_alu_info_t *group_alu_info,
                    bcmi_ft_alu_load_type_t alu_load_type)
{
    int i = 0;

    if (!BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        return 0;
    }

    if (group_alu_info == NULL) {
        return 0;
    }

    if ((alu_load_type != bcmiFtAluLoadTypeAlu32) &&
        (alu_load_type != bcmiFtAluLoadTypeAlu16)) {

        return 0;
    }

    for (i = 0; i < BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info; i++) {
        if (group_alu_info->element_type1 ==
               BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param) {
            return 1;
        }
    }

    return 0;
}

int
bcmi_ft_chip_debug_counter_init(int unit,
                                int mem_idx,
                                bcmi_ft_alu_load_type_t alu_load_type,
                                uint32 count)
{
    soc_mem_t mem;

    if (mem_idx == -1) {
        return BCM_E_NONE;
    }

    /* Get the free memory index of this type of memory. */
    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        mem  = bcmi_ft_alu16_debug_counter[mem_idx];
    } else {
        mem  = bcmi_ft_alu32_debug_counter[mem_idx];
    }

    BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit, mem,
                                        0, COUNTf, count));

    return BCM_E_NONE;
}

int
bcmi_ft_chip_debug_counter_get(int unit,
                               int mem_idx,
                               bcmi_ft_alu_load_type_t alu_load_type,
                               uint32 *count)
{
    soc_mem_t mem;
    uint32 buf[SOC_MAX_MEM_WORDS];

    if (mem_idx == -1) {
        return BCM_E_NONE;
    }

    if (count == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the free memory index of this type of memory. */
    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        mem  = bcmi_ft_alu16_debug_counter[mem_idx];
    } else {
        mem  = bcmi_ft_alu32_debug_counter[mem_idx];
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0, buf));

    *count = soc_mem_field32_get(unit, mem, buf, COUNTf);

    return BCM_E_NONE;
}

void
bcmi_ft_chip_debug_use_mem_update(int unit,
                                  bcmi_ft_alu_load_type_t alu_load_type,
                                  int mem_idx, int val)
{
    int i = 0;
    int *ref_count = NULL;
    int *in_use_array = NULL;

    if (!BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        return;
    }

    if ((alu_load_type != bcmiFtAluLoadTypeAlu32) &&
        (alu_load_type != bcmiFtAluLoadTypeAlu16)) {
        return;
    }

    /* Get the free memory index of this type of memory. */
    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        in_use_array = BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_mem_used;
        ref_count =    BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_ref_count;
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        in_use_array = BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_mem_used;
        ref_count =    BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_ref_count;
    }

    ref_count[mem_idx] += val;

    in_use_array[mem_idx] = ((ref_count[mem_idx]) ? 1 : 0);

    /*
     * Now if the ref_count on this memory index is 0.
     * We should remove the association with tracking param too.
     */
    if (ref_count[mem_idx] == 0) {
        /* Find which tracking param is using this memory.*/
        for (i=0; i<BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info; i++) {
            if (alu_load_type != BCMI_FT_CHIP_DEBUG_PARAM(unit, i).alu_type) {
                continue;
            }

            if (mem_idx == BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id) {
                BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id = -1;
                BCMI_FT_CHIP_DEBUG_PARAM(unit, i).alu_type = 0;
                break;
            }
        }
    }

    return;
}

int
bcmi_ft_chip_debug_mem_check(int unit,
                             bcmi_ft_alu_load_type_t alu_load_type,
                             int mem_idx)
{
    int i = 0;

    if (!BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        return 0;
    }

    /* Check in existing parameters. */
    for (i = 0; i < BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info; i++) {
        if (alu_load_type != BCMI_FT_CHIP_DEBUG_PARAM(unit, i).alu_type) {
            continue;
        }

        if (mem_idx == BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id) {
            return 1;
        }
    }

    return 0;
}

/*
 * Function:
 *   bcmi_ft_chip_debug_alu_index_get
 * Purpose:
 *   Get ALU/Load type based on length/params.
 * Parameters:
 *   unit - (IN) BCM device id.
 *   id  - (IN) FT group id.
 *   alu_load_type - (IN) type of load/ALU.
 *   index  - (OUT) Free index of that memory.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_chip_debug_alu_index_get(int unit,
                                 bcm_flowtracker_group_t id,
                                 bcmi_ft_group_alu_info_t *group_alu_info,
                                 bcmi_ft_alu_load_type_t alu_load_type,
                                 int *index)
{
    int i = 0;
    int *in_use_array = NULL;
    int mem_idx = -1, mem_id = 0;
    int p_idx = 0; /* param index. */
    int size_per_mem = 0;
    int start_index = 0, end_index = 0;
    uint32 count = 0;
    uint32 total_mems = 1;
    uint32 *group_in_use_array = NULL;

    if (!BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        return 0;
    }

    if (group_alu_info == NULL) {
        return BCM_E_PARAM;
    }

    if (index == NULL) {
        return BCM_E_PARAM;
    }

    if ((alu_load_type != bcmiFtAluLoadTypeAlu32) &&
        (alu_load_type != bcmiFtAluLoadTypeAlu16)) {
        return BCM_E_PARAM;
    }

    *index = -1;

    /* Get the total indexes for this type of memory. */
    size_per_mem = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, 0);

    /* Check in existing parameters. */
    for (i=0; i<BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info; i++) {
        if (group_alu_info->element_type1 ==
            BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param) {
            mem_idx = BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id;
            count = BCMI_FT_CHIP_DEBUG_PARAM(unit, i).count;
            p_idx = i;
            break;
        }
    }

    if (i == BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info) {
        return BCM_E_NOT_FOUND;
    }

    /* Get the free memory index of this type of memory. */
    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        in_use_array = BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_mem_used;
        group_in_use_array = BCMI_FT_GROUP_ALU16_MEM_USE(unit, id);
        total_mems   = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16);
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        in_use_array = BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_mem_used;
        group_in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
        total_mems   = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32);
    }

    if (mem_idx == -1) {

        /* start index should be where the free index starts. */
        while (mem_id < total_mems) {
            if (in_use_array != NULL) {
                if ((in_use_array[mem_id]) || (group_in_use_array[mem_id])) {
                    /* This ALU memory is used. go for next one. */
                    mem_id++;
                    continue;
                } else {
                    mem_idx = mem_id;
                    break;
                }
            } else {
                return BCM_E_PARAM;
            }
        }
        BCM_IF_ERROR_RETURN
            (bcmi_ft_chip_debug_counter_init(unit,
                                mem_idx, alu_load_type, count));
    } else {
        if (group_in_use_array[mem_idx]) {
            return BCM_E_PARAM;
        }
    }

    if (mem_idx == -1) {
        return BCM_E_RESOURCE;
    }

    for (mem_id=0; mem_id<mem_idx; mem_id++) {
        size_per_mem =
            BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, mem_id);

        start_index += size_per_mem;
    }

    size_per_mem = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, mem_idx);

    /* set the end index according to the indexes in that memory. */
    end_index = start_index + size_per_mem;

    /*
     * Cannot not index 0 for ALU/LOAD.
     * If index 0 is used then Group may point to it and
     * malicious actions can be taken on that flow.
     */
    i = start_index;
    i += ((start_index == 0) ? 1 : 0);

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "start_index = %d, end_index=%d\n"),
        start_index, end_index));

    /* based on the above index. select the bitmap. */
    for (; i<end_index; i++) {
        if (!(SHR_BITGET(BCMI_FT_ALU_LOAD_BITMAP_INFO
            (unit, alu_load_type), i))) {

            *index = i;
            in_use_array[mem_idx] = 1;
            BCMI_FT_CHIP_DEBUG_PARAM(unit, p_idx).mem_id = mem_idx;
            BCMI_FT_CHIP_DEBUG_PARAM(unit, p_idx).alu_type = alu_load_type;
            LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "mem_idx = %d, index = %d\n"),
                mem_idx, *index));

            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}


/*
 * Function:
 *   bcmi_ft_chip_debug_free_alu_index
 * Purpose:
 *   Get ALU/Load type based on length/params.
 * Parameters:
 *   unit - (IN) BCM device id.
 *   id  - (IN) FT group id.
 *   alu_load_type - (IN) type of load/ALU.
 *   index  - (OUT) Free index of that memory.
 * Returns:
 *   BCM_E_XXX
 */
void
bcmi_ft_chip_debug_free_alu_index(int unit,
                                  bcmi_ft_group_alu_info_t *group_alu_info,
                                  bcmi_ft_alu_load_type_t alu_load_type)
{
    int i = 0;
    int mem_idx = -1;
    int *ref_count = NULL;
    int *in_use_array = NULL;

    if (!BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        return;
    }

    if (group_alu_info == NULL) {
        return;
    }

    if ((alu_load_type != bcmiFtAluLoadTypeAlu32) &&
        (alu_load_type != bcmiFtAluLoadTypeAlu16)) {
        return;
    }

    /* Get the free memory index of this type of memory. */
    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        in_use_array = BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_mem_used;
        ref_count =    BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_ref_count;
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        in_use_array = BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_mem_used;
        ref_count =    BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_ref_count;
    }


    for (i = 0; i < BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info; i++) {
        if (group_alu_info->element_type1 ==
            BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param) {

            mem_idx = BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id;
            break;
        }
    }

    /* Nothing matching found. so just return. */
    if (i == BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info) {
        return;
    }

    ref_count[mem_idx]--;

    in_use_array[mem_idx] = ((ref_count[mem_idx]) ? 1 : 0);

    /*
     * Now that ref_count on this memory index is 0.
     * We should remove the association with tracking param too.
     */
    return;
}

int
bcmi_ft_chip_debug_data_sanity(int unit,
                               int num_info,
                               bcm_flowtracker_chip_debug_info_t *info)
{
    int i, j, k;
    soc_mem_t mem;
    int num_groups = 0;
    int num_tracking_elements = 0;
    bcm_flowtracker_tracking_param_type_t param;
    bcm_flowtracker_tracking_param_info_t *track_info = NULL;

    /* associate the memory. */
    mem = BSC_KG_GROUP_TABLEm;

    /* Total number of groups in device. */
    num_groups = soc_mem_index_count(unit, mem);

    for (j = 0; j < num_info; j++) {

        param = info[j].param;

        for (i = 0; i < num_groups; i++) {
            if (bcmi_ft_group_is_invalid(unit, i)) {
                continue;
            }

            num_tracking_elements = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, i);
            if (!num_tracking_elements) {
                continue;
            }

            track_info = BCMI_FT_GROUP_TRACK_PARAM(unit, i);
            if (!track_info) {
                continue;
            }

            for (k = 0; k < num_tracking_elements; k++) {
                if (param == track_info[k].param) {
                    return BCM_E_BUSY;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_alu_load_index_match_data_get
 * Purpose:
 *   Get matching data for this alu index.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   alu_type - (IN) ALU type.
 *   iter  - (IN) alu memory iterator.
 *   temp - (OUT) Pointer to point to data.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_alu_load_index_match_data_get(int unit,
                                            bcm_flowtracker_group_t id,
                                            bcmi_ft_alu_load_type_t alu_type,
                                            int iter,
                                            bcmi_ft_group_alu_info_t **temp)
{
    int num_alus_loads = 0;
    int mem_size = 0;
    int start_index = 0, j = 0, i = 0;

    for (; j < iter; j++) {
        mem_size = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_type, j);
        start_index += mem_size;
    }

    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    (*temp) = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    mem_size = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_type, iter);

    for (; i < num_alus_loads; i++) {

        if ((*temp)->alu_load_type == alu_type) {
            if (((*temp)->alu_load_index >= start_index) &&
                 ((*temp)->alu_load_index < (start_index + mem_size))) {
                return BCM_E_NONE;
            }
        }
        (*temp)++;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ft_group_alu_load_pdd_status_get
 * Purpose:
 *   Get pdd status for this alu memory.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   in_pdd_entry - (IN) Input pdd entry to look
 *   alu_type - (IN) ALU type.
 *   iter  - (IN) alu memory iterator.
 * Returns:
 *   1 = set
 *   0 = Not set.
 */
int
bcmi_ft_group_alu_load_pdd_status_get(int unit,
                                      bcm_flowtracker_group_t id,
                                      uint32 *in_pdd_entry,
                                      bcmi_ft_alu_load_type_t alu_type,
                                      int iter)
{
    soc_mem_t mem;

    /*
     * No need to check validity of group and other params.
     * If we are here so that all other checks are done.
     */

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    if (soc_mem_field32_get(unit, mem,
                            in_pdd_entry,
          (BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_type)[iter]).param_id)) {

        return 1;
    }

    return 0;
}

/*
 * Function:
 *   bcmi_ft_tracking_param_weight_hw_val_get
 * Purpose:
 *   Get hw encoding for tracking param weight.
 * Parameters:
 *   unit     - (IN) BCM device id
 *   weight   - (IN) Tracking param weight
 *   hw_val   - (OUT) Hw encoding
 * Returns:
 *   NONE
 */
int
bcmi_ft_tracking_param_weight_hw_val_get(
    int unit,
    bcm_flowtracker_tracking_param_weight_t weight,
    uint32 *hw_val)
{
    int i;
    uint32 weight_arr[] = {
        100, 96, 92, 88, 86, 84, 81, 78,
         75, 71, 67, 63, 59, 56, 53, 50,
         47, 44, 41, 37, 33, 29, 25, 22,
         19, 16, 12,  8,  4,  0};

    for (i = 0; i < COUNTOF(weight_arr); i++) {
        if (weight == weight_arr[i]) {
            if (hw_val != NULL) {
                *hw_val = i;
            }
            return BCM_E_NONE;
        }
    }

    return BCM_E_CONFIG;
}

/*
 * Function:
 *   bcmi_ft_tracking_param_weight_enum_get
 * Purpose:
 *   Get tracking param weight enum value from
 *   hw encoding.
 * Parameters:
 *   unit     - (IN) BCM device id
 *   hw_val   - (IN) Hw encoding
 *   weight   - (OUT) Tracking param weight
 * Returns:
 *   NONE
 */

int
bcmit_ft_tracking_param_weight_enum_get(
    int unit,
    uint32 hw_val,
    bcm_flowtracker_tracking_param_weight_t *weight)
{
    uint32 weight_arr[] = {
        100, 96, 92, 88, 86, 84, 81, 78,
         75, 71, 67, 63, 59, 56, 53, 50,
         47, 44, 41, 37, 33, 29, 25, 22,
         19, 16, 12,  8,  4,  0};

    if (hw_val >= COUNTOF(weight_arr)) {
        return BCM_E_INTERNAL;
    }

    *weight = weight_arr[hw_val];

    return BCM_E_NONE;
}



#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
/*
 * Function:
 *   bcmi_ftv2_alu_load_mem_config_init
 * Purpose:
 *   Allocate flowtracker resources as per user provided config.
 * Parameters:
 *   unit - (IN) BCM device id.
 * Notes:
 *   Intent of this api is to allocate flowtracker resources
 *   among IFT/MFT/EFT stages as per user provided config and
 *   update software tracking structures. It also program
 *   resource mapping hardware registers.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_alu_load_mem_config_init(int unit)
{
    int num_r = 0, res_tmp = 0, res_tmp_bank = 0;
    char *config_str = NULL, *sub_str = NULL, *sub_str_end = NULL;
    char ft_resource_config[BCMI_FTv2_RESOURCE_NUM][35] = {
        spn_FLOWTRACKER_SESSION_DATA_MEM_BANKS,
        spn_FLOWTRACKER_ALU16_MEM_BANKS,
        spn_FLOWTRACKER_ALU32_MEM_BANKS,
        spn_FLOWTRACKER_LOAD8_MEM_BANKS,
        spn_FLOWTRACKER_LOAD16_MEM_BANKS,
        spn_FLOWTRACKER_TIMESTAMP_MEM_ENGINES
    };
    const char *resource_str[] = {
        "Session Data Bank",
        "Alu16",
        "Alu32",
        "Load8",
        "Load16",
        "Timestamp Bank"
    };
    uint32 exp_bank_valid = 0;
    uint32 reg_val = 0;
    uint32 reg_ra_enable_val = 0;
    uint32 reg_exp_bank_valid_val = 0;
    uint8 mft_bank_alloc = 0, eft_bank_alloc = 0;
    uint8 ift_r = 0, mft_r = 0, eft_r = 0, unallocated_r = 0;
    uint32 max_value[BCMI_FTv2_RESOURCE_NUM] = {
        TOTAL_GROUP_SESSION_DATA_BANK_SW_DATA_NUM,
        TOTAL_GROUP_ALU16_MEM,
        TOTAL_GROUP_ALU32_MEM,
        TOTAL_GROUP_LOAD8_DATA_NUM,
        TOTAL_GROUP_LOAD16_DATA_NUM,
        TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM
    };
    soc_reg_t resource_reg[BCMI_FTv2_RESOURCE_NUM] = {
        BSC_DT_BANK_TO_INSTANCE_MAPPING_CONTROLr,
        BSC_DT_ALU16_TO_INSTANCE_MAPPING_CONTROLr,
        BSC_DT_ALU32_TO_INSTANCE_MAPPING_CONTROLr,
        BSC_DT_LOAD8_TO_INSTANCE_MAPPING_CONTROLr,
        BSC_DT_LOAD16_TO_INSTANCE_MAPPING_CONTROLr,
        BSC_DT_TIMESTAMP_ENGINE_TO_INSTANCE_MAPPING_CONTROLr
    };
    soc_field_t bs_num_mem_banks[TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM] = {
        BANK_0f, BANK_1f, BANK_2f, BANK_3f,
        BANK_4f, BANK_5f, BANK_6f, BANK_7f
    };
    soc_field_t bs_num_alu16[TOTAL_GROUP_ALU16_MEM] = {
        ALU16_0f, ALU16_1f, ALU16_2f, ALU16_3f,
        ALU16_4f, ALU16_5f, ALU16_6f, ALU16_7f
    };
    soc_field_t bs_num_alu32[TOTAL_GROUP_ALU32_MEM] = {
        ALU32_0f, ALU32_1f, ALU32_2f, ALU32_3f,
        ALU32_4f, ALU32_5f, ALU32_6f, ALU32_7f,
        ALU32_8f, ALU32_9f, ALU32_10f, ALU32_11f
    };
    soc_field_t bs_num_load8[TOTAL_GROUP_LOAD8_DATA_NUM] = {
        LOAD8_0f, LOAD8_1f, LOAD8_2f, LOAD8_3f,
        LOAD8_4f, LOAD8_5f, LOAD8_6f, LOAD8_7f
    };
    soc_field_t bs_num_load16[TOTAL_GROUP_LOAD16_DATA_NUM] = {
        LOAD16_0f, LOAD16_1f, LOAD16_2f, LOAD16_3f,
        LOAD16_4f, LOAD16_5f, LOAD16_6f, LOAD16_7f,
        LOAD16_8f, LOAD16_9f, LOAD16_10f, LOAD16_11f,
        LOAD16_12f, LOAD16_13f, LOAD16_14f, LOAD16_15f
    };
    soc_field_t bs_num_timestamp[TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM] = {
        TIMESTAMP_ENGINE_0f, TIMESTAMP_ENGINE_1f,
        TIMESTAMP_ENGINE_2f, TIMESTAMP_ENGINE_3f};

    max_value[BCMI_FTv2_RESOURCE_ALU32] = bcmi_ft_get_alu32_mem(unit);

    /* Loop through all broadscan resource to allocate */
    for (num_r = 0; num_r < BCMI_FTv2_RESOURCE_NUM; num_r++) {

        /* Reset variables */
        config_str = NULL;
        ift_r = mft_r = eft_r = 0, unallocated_r = 0, reg_val = 0, res_tmp = 0;
        config_str = soc_property_get_str(unit, ft_resource_config[num_r]);

        /* Validate and retreive resources from config specified by user */
        if (config_str != NULL) {

            /* Get IFT resources first */
            sub_str = config_str;
            ift_r = sal_ctoi(sub_str, &sub_str_end);

            /* Get MFT resources second */
            sub_str = sub_str_end;
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                   "Bad config string \"%s\"\n"), config_str));
                return BCM_E_CONFIG;
            }
            sub_str++;
            mft_r = sal_ctoi(sub_str, &sub_str_end);

            /* Get EFT resources in last */
            sub_str = sub_str_end;
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                   "Bad config string \"%s\"\n"), config_str));
                return BCM_E_CONFIG;
            }
            sub_str++;
            eft_r = sal_ctoi(sub_str, &sub_str_end);

            /* validate if allocation does not exceed maximum allowed */
            if (ift_r + mft_r + eft_r > max_value[num_r]) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                   "%s resource allocation exceeded max value %d allowed.\n"),
                    resource_str[num_r], max_value[num_r]));
                return BCM_E_RESOURCE;
            }
        }

        /* add any unallocated resource to IFT stage if any */
        unallocated_r = max_value[num_r] - ift_r - mft_r - eft_r;
        ift_r += unallocated_r;

        /* Program allocated values in hardware and update
         * software to be in sync */
        switch (num_r) {

            case BCMI_FTv2_RESOURCE_SESSION_BANK:
                /* Session Data Bank Info */
                exp_bank_valid = 0;
                /* As per arch recommendation, min 1 session
                 * data bank need to be allocated for IFT */
                if (ift_r == 0) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "Session data bank assigned to IFT cannot be 0.\n")));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < ift_r; res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r],
                      &reg_val, bs_num_mem_banks[res_tmp], BCMI_FTv2_STAGE_IFT);
                    soc_reg_field_set(unit,
                            resource_reg[num_r], &reg_val,
                            bs_num_mem_banks[res_tmp +
                            TOTAL_GROUP_SESSION_DATA_BANK_SW_DATA_NUM],
                            BCMI_FTv2_STAGE_IFT);
                    res_tmp_bank++;
                    exp_bank_valid |= (1 << (TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM - res_tmp_bank));
                    res_tmp_bank++;
                    exp_bank_valid |= (1 << (TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM - res_tmp_bank));
                    BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, res_tmp) =
                                                                  bcmiFtTypeIFT;
                    BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, res_tmp +
                     TOTAL_GROUP_SESSION_DATA_BANK_SW_DATA_NUM) = bcmiFtTypeIFT;
                }
                soc_reg_field_set(unit, BSC_DT_EXPORT_BANK_VALID_BITMAPr,
                        &reg_exp_bank_valid_val, FT_0_BANK_VALID_BITMAPf,
                                                         exp_bank_valid);

                exp_bank_valid = 0;
                mft_bank_alloc = mft_r;
                for (; res_tmp < (ift_r + mft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r],
                      &reg_val, bs_num_mem_banks[res_tmp], BCMI_FTv2_STAGE_MFT);
                    soc_reg_field_set(unit,
                            resource_reg[num_r], &reg_val,
                            bs_num_mem_banks[res_tmp +
                            TOTAL_GROUP_SESSION_DATA_BANK_SW_DATA_NUM],
                            BCMI_FTv2_STAGE_MFT);
                    res_tmp_bank++;
                    exp_bank_valid |= (1 << (TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM - res_tmp_bank));
                    res_tmp_bank++;
                    exp_bank_valid |= (1 << (TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM - res_tmp_bank));
                    BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, res_tmp) =
                                                                  bcmiFtTypeMFT;
                    BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, res_tmp +
                     TOTAL_GROUP_SESSION_DATA_BANK_SW_DATA_NUM) = bcmiFtTypeMFT;
                }
                soc_reg_field_set(unit, BSC_DT_EXPORT_BANK_VALID_BITMAPr,
                               &reg_exp_bank_valid_val, FT_1_BANK_VALID_BITMAPf,
                                                                exp_bank_valid);

                exp_bank_valid = 0;
                eft_bank_alloc = eft_r;
                for (; res_tmp < (ift_r + mft_r + eft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r],
                      &reg_val, bs_num_mem_banks[res_tmp], BCMI_FTv2_STAGE_EFT);
                    soc_reg_field_set(unit,
                            resource_reg[num_r], &reg_val,
                            bs_num_mem_banks[res_tmp +
                            TOTAL_GROUP_SESSION_DATA_BANK_SW_DATA_NUM],
                            BCMI_FTv2_STAGE_EFT);
                    res_tmp_bank++;
                    exp_bank_valid |= (1 << (TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM - res_tmp_bank));
                    res_tmp_bank++;
                    exp_bank_valid |= (1 << (TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM - res_tmp_bank));
                    BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, res_tmp) =
                                                                  bcmiFtTypeEFT;
                    BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, res_tmp +
                     TOTAL_GROUP_SESSION_DATA_BANK_SW_DATA_NUM) = bcmiFtTypeEFT;
                }
                soc_reg_field_set(unit, BSC_DT_EXPORT_BANK_VALID_BITMAPr,
                               &reg_exp_bank_valid_val, FT_2_BANK_VALID_BITMAPf,
                                                                exp_bank_valid);

                WRITE_BSC_DT_BANK_TO_INSTANCE_MAPPING_CONTROLr(unit, reg_val);
                WRITE_BSC_DT_EXPORT_BANK_VALID_BITMAPr(unit,
                                                       reg_exp_bank_valid_val);

                /* Enable FT stage in RA if session data bank
                 * is allocated for a stage */
                if (ift_r) {
                    soc_reg_field_set(unit, BSC_EX_BROADSCAN_RA_FT_ENABLEr,
                            &reg_ra_enable_val, FT_0_ENABLEf, 1);
                }
                if (mft_r) {
                    soc_reg_field_set(unit, BSC_EX_BROADSCAN_RA_FT_ENABLEr,
                            &reg_ra_enable_val, FT_1_ENABLEf, 1);
                }
                if (eft_r) {
                    soc_reg_field_set(unit, BSC_EX_BROADSCAN_RA_FT_ENABLEr,
                            &reg_ra_enable_val, FT_2_ENABLEf, 1);
                }
                WRITE_BSC_EX_BROADSCAN_RA_FT_ENABLEr(unit, reg_ra_enable_val);
                break;

            case BCMI_FTv2_RESOURCE_ALU16:
                /* ALU16 Resource Allocation */
                for (; res_tmp < ift_r; res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_alu16[res_tmp], BCMI_FTv2_STAGE_IFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAlu16,
                                res_tmp, 0) = bcmiFtTypeIFT;
                }

                if ((mft_r * 16) >
                    (mft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "No. of MFT ALU16 allocated <%d> exceeds session "
                       "data bank size <%d> allocated to accommodate.\n"),
                       mft_r, mft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_alu16[res_tmp], BCMI_FTv2_STAGE_MFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAlu16,
                                res_tmp, 0) = bcmiFtTypeMFT;
                }

                if ((eft_r * 16) >
                    (eft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "No. of EFT ALU16 allocated <%d> exceeds session "
                       "data bank size <%d> allocated to accommodate.\n"),
                       eft_r, eft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r + eft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_alu16[res_tmp], BCMI_FTv2_STAGE_EFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAlu16,
                                res_tmp, 0) = bcmiFtTypeEFT;
                }
                WRITE_BSC_DT_ALU16_TO_INSTANCE_MAPPING_CONTROLr(unit, reg_val);
                break;

            case BCMI_FTv2_RESOURCE_ALU32:
                /* ALU32 Resource Allocation */
                for (; res_tmp < ift_r; res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_alu32[res_tmp], BCMI_FTv2_STAGE_IFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAlu32,
                                res_tmp, 0) = bcmiFtTypeIFT;
                }

                if ((mft_r * 32) >
                    (mft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "No. of MFT ALU32 allocated <%d> exceeds session "
                       "data bank size <%d> allocated to accommodate.\n"),
                        mft_r, mft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_alu32[res_tmp], BCMI_FTv2_STAGE_MFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAlu32,
                                res_tmp, 0) = bcmiFtTypeMFT;
                }

                if ((eft_r * 32) >
                    (eft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "No. of EFT ALU32 allocated <%d> exceeds session "
                       "data bank size <%d> allocated to accommodate.\n"),
                        eft_r, eft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r + eft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_alu32[res_tmp], BCMI_FTv2_STAGE_EFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAlu32,
                                res_tmp, 0) = bcmiFtTypeEFT;
                }
                WRITE_BSC_DT_ALU32_TO_INSTANCE_MAPPING_CONTROLr(unit, reg_val);
                break;

            case BCMI_FTv2_RESOURCE_LOAD8:
                /* Load8 Resource Allocation */
                for (; res_tmp < ift_r; res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_load8[res_tmp], BCMI_FTv2_STAGE_IFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad8,
                                0, res_tmp) = bcmiFtTypeIFT;
                }

                if ((mft_r * 8) >
                    (mft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "No. of MFT LOAD8 allocated <%d> exceeds session "
                       "data bank size <%d> allocated to accommodate.\n"),
                        mft_r, mft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_load8[res_tmp], BCMI_FTv2_STAGE_MFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad8,
                                0, res_tmp) = bcmiFtTypeMFT;
                }

                if ((eft_r * 8) >
                    (eft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "No. of EFT LOAD8 allocated <%d> exceeds session "
                       "data bank size <%d> allocated to accommodate.\n"),
                        eft_r, eft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r + eft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_load8[res_tmp], BCMI_FTv2_STAGE_EFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad8,
                                0, res_tmp) = bcmiFtTypeEFT;
                }
                WRITE_BSC_DT_LOAD8_TO_INSTANCE_MAPPING_CONTROLr(unit, reg_val);
                break;

            case BCMI_FTv2_RESOURCE_LOAD16:
                /* Load16 Resource Allocation */
                for (; res_tmp < ift_r; res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_load16[res_tmp], BCMI_FTv2_STAGE_IFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad16,
                                0, res_tmp) = bcmiFtTypeIFT;
                }

                if ((mft_r * 16) >
                    (mft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "No. of MFT LOAD16 allocated <%d> exceeds session "
                       "data bank size <%d> allocated to accommodate.\n"),
                        mft_r, mft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_load16[res_tmp], BCMI_FTv2_STAGE_MFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad16,
                                0, res_tmp) = bcmiFtTypeMFT;
                }

                if ((eft_r * 16) >
                    (eft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "No. of EFT LOAD16 allocated <%d> exceeds session "
                       "data bank size <%d> allocated to accommodate.\n"),
                        eft_r, eft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r + eft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                                bs_num_load16[res_tmp], BCMI_FTv2_STAGE_EFT);
                    BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad16,
                                0, res_tmp) = bcmiFtTypeEFT;
                }
                WRITE_BSC_DT_LOAD16_TO_INSTANCE_MAPPING_CONTROLr(unit, reg_val);
                break;

            case BCMI_FTv2_RESOURCE_TIMESTAMP_ENGINE:
                /* TimeStamp Resource Allocation */
                for (; res_tmp < ift_r; res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                            bs_num_timestamp[res_tmp], BCMI_FTv2_STAGE_IFT);
                    BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, res_tmp) =
                                                              bcmiFtTypeIFT;
                }

                if ((mft_r * 48) >
                    (mft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                     "No. of MFT timestamp bank allocated <%d> exceeds session "
                     "data bank size <%d> allocated to accommodate.\n"),
                      mft_r, mft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                            bs_num_timestamp[res_tmp], BCMI_FTv2_STAGE_MFT);
                    BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, res_tmp) =
                                                              bcmiFtTypeMFT;
                }

                if ((eft_r * 48) >
                    (eft_bank_alloc * 2 *
                     BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                     "No. of EFT timestamp bank allocated <%d> exceeds session "
                     "data bank size <%d> allocated to accommodate.\n"),
                      eft_r, eft_bank_alloc));
                    return BCM_E_CONFIG;
                }
                for (; res_tmp < (ift_r + mft_r + eft_r); res_tmp++) {
                    soc_reg_field_set(unit, resource_reg[num_r], &reg_val,
                            bs_num_timestamp[res_tmp], BCMI_FTv2_STAGE_EFT);
                    BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, res_tmp) =
                                                              bcmiFtTypeEFT;
                }
                WRITE_BSC_DT_TIMESTAMP_ENGINE_TO_INSTANCE_MAPPING_CONTROLr(unit,
                                                                       reg_val);
                break;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_resource_info_init
 * Purpose:
 *   Allocate flowtracker resource info as per user provided config.
 * Parameters:
 *   unit - (IN) BCM device id.
 * Notes:
 *   Intent of this api is to store some data related to resource
 *   allocation which will be used in run time logic.
 *   Should be called after resources are allocated in
 *   api bcmi_ftv2_alu_load_mem_config_init.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_resource_info_init(int unit)
{
    int ts_bank_num = 0;
    uint8 max_delay_gran = 0;
    int export_fill_bytes = 0;
    int bank_id = 0, load_num = 0;
    bcmi_ft_type_t ft_type = bcmiFtTypeNone;
    uint32 total_size = 0, bank_size = 0, no_of_banks = 0;

    /* Initialise global structure */
    sal_memset((void *)&BCMI_FT_RESOURCE_SW_INFO(unit) , 0,
                        sizeof(bcmi_ft_resource_sw_info_t));

    /* Get session data system details in bytes */
    total_size = ((BCMI_FT_SESSION_DATA_SINGLE_LEN * 2) / 8);
    bank_size = BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE / 8;
    no_of_banks = total_size / bank_size;

    /*
     * Compute start pde index, fill size and offset jump
     * for bank allocation db.
     *0                         31||32                        63
     *|------|------|------|------||------|------|------|------|
     *   B4     B5     B6     B7  ||  B0     B1     B2     B3
     */
    for (bank_id = ((no_of_banks/2) - 1); bank_id >= 0; bank_id--) {
        if (BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, bank_id) ==
                                                      bcmiFtTypeIFT) {
            BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeIFT)++;
        } else if (BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, bank_id) ==
                                                      bcmiFtTypeMFT) {
            BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeMFT)++;;
        } else if (BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, bank_id) ==
                                                      bcmiFtTypeEFT) {
            BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeEFT)++;
        }
    }

    /* Compute Start PDE for each ft type */
    for (bank_id = 0; bank_id <=((no_of_banks/ 2) - 1); bank_id++) {
        ft_type = BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, bank_id);
        BCMI_FT_RESOURCE_SW_INFO_BANK_START_LOCAL_PDE_OFFSET(unit, ft_type) =
            (total_size - 1) - ((((no_of_banks / 2) - 1)- bank_id) * 8);
    }

    /* Compute offset jump and fill byte for each supported ft_type */
    export_fill_bytes = 0;
    for (ft_type = bcmiFtTypeIFT; ft_type <= bcmiFtTypeEFT; ft_type++) {

        BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, ft_type) =
            BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, ft_type) * bank_size;

        if (BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, ft_type) != 0) {

            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, ft_type) = 1;

            BCMI_FT_RESOURCE_SW_INFO_BANK_START_EXPORT_PDE_OFFSET
                                                            (unit, ft_type) =
                                      ((total_size - 1) - export_fill_bytes);

            export_fill_bytes +=
                (BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, ft_type) *
                (BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, ft_type) + 1));

            BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_EXPORT_JUMP(unit, ft_type) = 0;

            if (ft_type == bcmiFtTypeIFT) {

                BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_LOCAL_JUMP(unit, ft_type) =
                    (BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeMFT) +
                     BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeEFT)) *
                    bank_size;

            } else if (ft_type == bcmiFtTypeMFT) {

                BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_LOCAL_JUMP(unit, ft_type) =
                    (BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeIFT) +
                     BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeEFT)) *
                    bank_size;

            } else if (ft_type == bcmiFtTypeEFT) {

                BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_LOCAL_JUMP(unit, ft_type) =
                    (BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeIFT) +
                     BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeMFT)) *
                    bank_size;

            }

        } else {

            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, ft_type) = 0;
            BCMI_FT_RESOURCE_SW_INFO_BANK_START_EXPORT_PDE_OFFSET(unit, ft_type) = 0;
            BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_EXPORT_JUMP(unit, ft_type) = 0;
            BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_LOCAL_JUMP(unit, ft_type) = 0;

        }
    }

    /* Count no. of load8 engines assigned to each FT Stage */
    for (load_num = 0; load_num < TOTAL_GROUP_LOAD8_DATA_NUM; load_num++) {
        if (BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad8,
                                         0, load_num)  == bcmiFtTypeIFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD8(unit, bcmiFtTypeIFT)++;
        } else if (BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad8,
                                         0, load_num) == bcmiFtTypeMFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD8(unit, bcmiFtTypeMFT)++;
        } else if (BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad8,
                                         0, load_num) == bcmiFtTypeEFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD8(unit, bcmiFtTypeEFT)++;
        }
    }

    /* Count no. of load16 engines assigned to each FT Stage */
    for (load_num = 0; load_num < TOTAL_GROUP_LOAD16_DATA_NUM; load_num++) {
        if (BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad16,
                                         0, load_num)  == bcmiFtTypeIFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD16(unit, bcmiFtTypeIFT)++;
        } else if (BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad16,
                                         0, load_num) == bcmiFtTypeMFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD16(unit, bcmiFtTypeMFT)++;
        } else if (BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeLoad16,
                                         0, load_num) == bcmiFtTypeEFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD16(unit, bcmiFtTypeEFT)++;
        }
    }

    /* Count no. of timestamp engines assigned to each FT stage */
    for (ts_bank_num = 0;
         ts_bank_num < TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM; ts_bank_num++) {
        if (BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, ts_bank_num) ==
                                                            bcmiFtTypeIFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeIFT)++;
        } else if (BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, ts_bank_num) ==
                                                            bcmiFtTypeMFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeMFT)++;
        } else if (BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, ts_bank_num) ==
                                                            bcmiFtTypeEFT) {
            BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeEFT)++;
        }
    }

    /* Populate delay gran & offset */
    BCMI_FT_RESOURCE_SW_INFO_CHIP_GRAN(unit) =
         soc_property_get(unit, spn_FLOWTRACKER_CHIP_DELAY_GRANULARITY, 0);
    BCMI_FT_RESOURCE_SW_INFO_CHIP_OFFSET(unit) =
         soc_property_get(unit, spn_FLOWTRACKER_CHIP_DELAY_OFFSET, 0);
    BCMI_FT_RESOURCE_SW_INFO_E2E_GRAN(unit) =
         soc_property_get(unit, spn_FLOWTRACKER_E2E_DELAY_GRANULARITY, 0);
    BCMI_FT_RESOURCE_SW_INFO_E2E_OFFSET(unit) =
         soc_property_get(unit, spn_FLOWTRACKER_E2E_DELAY_OFFSET, 0);
    BCMI_FT_RESOURCE_SW_INFO_IPAT_GRAN(unit) =
         soc_property_get(unit, spn_FLOWTRACKER_IPAT_DELAY_GRANULARITY, 0);
    BCMI_FT_RESOURCE_SW_INFO_IPAT_OFFSET(unit) =
         soc_property_get(unit, spn_FLOWTRACKER_IPAT_DELAY_OFFSET, 0);
    BCMI_FT_RESOURCE_SW_INFO_IPDT_GRAN(unit) =
         soc_property_get(unit, spn_FLOWTRACKER_IPDT_DELAY_GRANULARITY, 0);
    BCMI_FT_RESOURCE_SW_INFO_IPDT_OFFSET(unit) =
         soc_property_get(unit, spn_FLOWTRACKER_IPDT_DELAY_OFFSET, 0);

    /* Validate delay gran provided in config */
    max_delay_gran = (1 << soc_format_field_length(unit,
                      BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                      DELAY_GRANULARITYf)) - 1;
    if ((BCMI_FT_RESOURCE_SW_INFO_CHIP_GRAN(unit) > max_delay_gran) ||
        (BCMI_FT_RESOURCE_SW_INFO_E2E_GRAN(unit) > max_delay_gran) ||
        (BCMI_FT_RESOURCE_SW_INFO_IPAT_GRAN(unit) > max_delay_gran) ||
        (BCMI_FT_RESOURCE_SW_INFO_IPDT_GRAN(unit) > max_delay_gran)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "Flowtracker chip delay gran config exceeded max"
                " delay gran 0x%x allowed.\n"), max_delay_gran));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_group_pdd_to_length_coversion
 * Purpose:
 *   Retreive bit length used for each stage from provided pdd
 * Parameters:
 *   unit          - (IN)  BCM device id.
 *   in_pdd_entry  - (IN)  Input pdd entry.
 *   length_in_use - (OUT) Length in use for each FT stage.
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ftv2_group_pdd_to_length_coversion(int unit,
                                        uint32 *in_pdd_entry,
                                        uint32 *length_in_use)
{
    int rv = BCM_E_NONE;
    soc_mem_t pdd_mem = INVALIDm;
    int total_mems = 0, mem_idx = 0;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeNone;

    /* Input parameters check */
    if ((in_pdd_entry == NULL) ||
        (length_in_use == NULL)) {
        return BCM_E_PARAM;
    }

    /* Retreive Relevant Memory */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &pdd_mem);

    /* Timestamp 0 */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_0f)) {
        length_in_use[(BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 0) - 1)] += 48;
    }

    /* Timestamp 1 */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_1f)) {
        length_in_use[(BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 1) - 1)] += 48;
    }

    /* Timestamp 2 */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_2f)) {
        length_in_use[(BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 2) - 1)] += 48;
    }

    /* Timestamp 3 */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_3f)) {
        length_in_use[(BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 3) - 1)] += 48;
    }

    /* Meter Entry - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_FLOW_METERf)) {
        length_in_use[(bcmiFtTypeIFT - 1)] += 48;
    }

    /* COUNT TO CPU - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_FLOW_COUNT_TO_CPUf)) {
        length_in_use[(bcmiFtTypeIFT - 1)] += 16;
    }

    /* For all the types of alu & load memory types. */
    for (alu_load_type = 0;
         alu_load_type < BCMI_FT_ALU_LOAD_MEMS; alu_load_type++) {

        /* Retreive db pdd info */
        pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type);
        if (!pdd_info) {
            continue;
        }

        total_mems = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, alu_load_type);

        if (alu_load_type == bcmiFtAluLoadTypeLoad16) {
            total_mems = TOTAL_GROUP_LOAD16_DATA_NUM;
        } else  if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
            total_mems = TOTAL_GROUP_LOAD8_DATA_NUM;
        }

        for (mem_idx = 0; mem_idx < total_mems; mem_idx++) {
            if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                        pdd_info[mem_idx].param_id)) {
                if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
                    length_in_use[(BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                alu_load_type, 0, mem_idx) - 1)] += 8;
                } else if (alu_load_type == bcmiFtAluLoadTypeLoad16) {
                    length_in_use[(BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                alu_load_type, 0, mem_idx) - 1)] += 16;
                } else if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
                    length_in_use[(BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                alu_load_type, mem_idx, 0) - 1)] += 16;
                } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
                    length_in_use[(BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                alu_load_type, mem_idx, 0) - 1)] += 32;
                } else {
                    /* Do nothing */
                }
            }
        }
    }

    /* FLOW STATE - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_FLOW_STATEf)) {
        length_in_use[(bcmiFtTypeIFT - 1)] += 8;
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_group_ft_mode_get
 * Purpose:
 *   Set single/Double mode after checking session data.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_ft_mode_update(int unit,
                               bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    uint32 *global_pdd_entry = NULL;
    uint32 length_in_use[3] = { 0 };
    bcmi_ft_group_key_data_mode_t mode = bcmiFtGroupModeSingle;

    /* Read the group state for the ALU inforamtion. */
    if (!(BCMI_FT_GROUP_EXT_DATA_OR_TS_TRIGGERS_SET(unit, id))) {
        return BCM_E_INTERNAL;
    }

    /* Retreive Global PDD Entry from Group Structure */
    global_pdd_entry = (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry));

    /* Retreive stage specific pdd entry from global pdd */
    rv = bcmi_ftv2_group_pdd_to_length_coversion(unit,
                                                 global_pdd_entry,
                                                 length_in_use);
    BCM_IF_ERROR_RETURN(rv);

    /* Now we have to check against session data bank allocated
     * to determine if mode has to be Double Wide */
    if ((length_in_use[0] >
            BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeIFT) *
            BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE) ||
        (length_in_use[1] >
            BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeMFT) *
            BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE) ||
        (length_in_use[2] >
            BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeEFT) *
            BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE)) {

        mode = bcmiFtGroupModeDouble;
        bcmi_ft_group_extraction_mode_set(unit, 0, id, mode);
        bcmi_ft_group_extraction_mode_set(unit, 1, id, mode);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_group_alu_mask_set
 * Purpose:
 *   Set alu mask profile entry
 * Parameters:
 *  unit     - (IN)  BCM device id.
 *  alu_info - (IN)  Group ALU Info.
 *  fc_info  - (IN)  Input flow checker info.
 *  alu_fmt  - (OUT) ALU format entry
 * Returns:
 *  BCM_E_XXX.
 */
int
bcmi_ftv2_group_alu_mask_set(int unit,
                             bcmi_ft_group_alu_info_t *alu_info,
                             bcmi_flowtracker_flowchecker_info_t *fc_info,
                             uint32 *alu_fmt)
{
    int rv = BCM_E_NONE;
    void *entries[1];
    int profile_idx = 0;
    int dual_operation = FALSE;
    uint32 pc_alu_mask_index = -1;
    uint32 sc_alu_mask_index = -1;
    uint32 act_alu_mask_index = -1;
    uint32 pc_check_mask_value = 0;
    uint32 sc_check_mask_value = 0;
    uint32 pc_check_shift_value = 0;
    uint32 sc_check_shift_value = 0;
    uint32 pc_check_action_mask_value = 0;
    uint32 pc_check_action_shift_value = 0;
    int shift_bits = 0, shift_bits2 = 0, shift_bits3 = 0;
    soc_mem_t alu_mask_profile_mem = INVALIDm;
    bsc_dt_alu_mask_profile_entry_t alu_mask_entry;

    /* input params check */
    if ((fc_info == NULL) || (alu_fmt == NULL) || (alu_info == NULL)) {
        return BCM_E_INTERNAL;
    }

    /* Get ALU Mask Profile */
    if (alu_info->alu_load_type == bcmiFtAluLoadTypeAggAlu32) {

        /* Mask is not needed for session data */
        if (alu_info->flowchecker_id == 0) {
            return BCM_E_NONE;
        }

        profile_idx  = 1;
        alu_mask_profile_mem = BSC_DT_AGG_ALU_MASK_PROFILEm;
    } else {
        profile_idx  = 0;
        alu_mask_profile_mem = BSC_DT_ALU_MASK_PROFILEm;
    }

    /* retreive primary & secondary mask values */
    if (bcmi_ft_flowchecker_opr_is_dual(unit, fc_info->check1.operation)) {
        dual_operation = TRUE;
        pc_check_mask_value = fc_info->check1.mask_value;
        sc_check_mask_value = fc_info->check1.mask_value;
        pc_check_shift_value = fc_info->check1.shift_value;
        sc_check_shift_value = fc_info->check1.shift_value;
    } else {
        pc_check_mask_value = fc_info->check1.mask_value;
        sc_check_mask_value = fc_info->check2.mask_value;
        pc_check_shift_value = fc_info->check1.shift_value;
        sc_check_shift_value = fc_info->check2.shift_value;
    }
    pc_check_action_mask_value = fc_info->action_info.mask_value;
    pc_check_action_shift_value =  fc_info->action_info.shift_value;

    /* If user provided mask is 0 for primary check, then apply
     * own mask if full ALU is not covered. */
    if (pc_check_mask_value == 0) {
        if (alu_info->alu_load_type == bcmiFtAluLoadTypeAlu16) {
            if ((alu_info->key1.length > 0) && (alu_info->key1.length < 16)) {
                pc_check_mask_value = ((1 << alu_info->key1.length) - 1);
            }
        } else {
            if ((alu_info->key1.length > 0) && (alu_info->key1.length < 32)) {
                pc_check_mask_value = ((1 << alu_info->key1.length) - 1);
            }
        }
    }

    /* If user provided mask is 0 for secondary check, then apply
     * own mask if full ALU is not covered. */
    if (dual_operation == TRUE) {
        sc_check_mask_value = pc_check_mask_value;
    } else if (sc_check_mask_value == 0) {
        if (alu_info->alu_load_type == bcmiFtAluLoadTypeAlu16) {
            if ((alu_info->key2.length > 0) && (alu_info->key2.length < 16)) {
                sc_check_mask_value = ((1 << alu_info->key2.length) - 1);
            }
        } else {
            if ((alu_info->key2.length > 0) && (alu_info->key2.length < 32)) {
                sc_check_mask_value = ((1 << alu_info->key2.length) - 1);
            }
        }
    }

    /* If user provided mask is 0 for secondary check, then apply
     * own mask if full ALU is not covered. */
    if (pc_check_action_mask_value == 0) {
        if (alu_info->alu_load_type == bcmiFtAluLoadTypeAlu16) {
            if ((alu_info->action_key.length > 0) &&
                (alu_info->action_key.length < 16)) {
                pc_check_action_mask_value =
                         ((1 << alu_info->action_key.length) - 1);
            }
        } else {
            if ((alu_info->action_key.length > 0) &&
                (alu_info->action_key.length < 32)) {
                pc_check_action_mask_value =
                         ((1 << alu_info->action_key.length) - 1);
            }
        }
    }

    /* shift values if needed */
    bcmi_ft_group_alu_attrib_shift_bits_get(unit,
            dual_operation, alu_info, &shift_bits, &shift_bits2);

    bcmi_ft_group_alu_action_shift_bits_get(unit,
                                    fc_info->action_info.action,
                                    alu_info, &shift_bits3);
    if (fc_info->action_info.action ==
                bcmFlowtrackerCheckActionCounterIncrByValue) {
        pc_check_action_shift_value += shift_bits3;
        alu_info->flags |= BCMI_FT_ALU_LOAD_DATA_SHIFT_APPLIED;
    }

    pc_check_mask_value = pc_check_mask_value << shift_bits;
    sc_check_mask_value = sc_check_mask_value << shift_bits2;
    pc_check_action_mask_value = pc_check_action_mask_value << shift_bits3;

    entries[0] = &alu_mask_entry;
    /* assign attribute mask & shift for primary check */
    if (pc_check_mask_value != 0) {
        sal_memset(&alu_mask_entry, 0, sizeof(bsc_dt_alu_mask_profile_entry_t));
        soc_mem_field32_set(unit, alu_mask_profile_mem,
                (uint32 *)&alu_mask_entry, ATTR_MASKf, pc_check_mask_value);
        soc_mem_field32_set(unit, alu_mask_profile_mem,
                (uint32 *)&alu_mask_entry, ATTR_RIGHT_SHIFTf, pc_check_shift_value);
        rv = bcmi_ft_group_alu_mask_profile_entry_add(unit, profile_idx,
                                        entries, 1, &pc_alu_mask_index);
        BCMI_CLEANUP_IF_ERROR(rv);
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                                    alu_fmt, MASK_EN_CHECK_0f, 1);
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                             alu_fmt, MASK_PROFILE_INDEX_CHECK_0f,
                                                               pc_alu_mask_index);
    }

    /* assign attribute mask & shift for secondary check */
    if (sc_check_mask_value != 0) {
        sal_memset(&alu_mask_entry, 0, sizeof(bsc_dt_alu_mask_profile_entry_t));
        soc_mem_field32_set(unit, alu_mask_profile_mem,
                                        (uint32 *)&alu_mask_entry, ATTR_MASKf,
                                                          sc_check_mask_value);
        soc_mem_field32_set(unit, alu_mask_profile_mem,
                                (uint32 *)&alu_mask_entry, ATTR_RIGHT_SHIFTf,
                                                        sc_check_shift_value);
        rv = bcmi_ft_group_alu_mask_profile_entry_add(unit, profile_idx,
                                            entries, 1, &sc_alu_mask_index);
        BCMI_CLEANUP_IF_ERROR(rv);
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                                alu_fmt, MASK_EN_CHECK_1f, 1);
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                        alu_fmt, MASK_PROFILE_INDEX_CHECK_1f,
                                                          sc_alu_mask_index);
    }

    /* assign attribute mask & shift for check action */
    if (pc_check_action_mask_value != 0) {
        sal_memset(&alu_mask_entry, 0, sizeof(bsc_dt_alu_mask_profile_entry_t));
        soc_mem_field32_set(unit, alu_mask_profile_mem,
                                (uint32 *)&alu_mask_entry, ATTR_MASKf,
                                                pc_check_action_mask_value);
        soc_mem_field32_set(unit, alu_mask_profile_mem,
                                (uint32 *)&alu_mask_entry, ATTR_RIGHT_SHIFTf,
                                                pc_check_action_shift_value);
        rv = bcmi_ft_group_alu_mask_profile_entry_add(unit, profile_idx,
                                            entries, 1, &act_alu_mask_index);
        BCMI_CLEANUP_IF_ERROR(rv);
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                            alu_fmt, MASK_EN_CHECK_UPDATEf, 1);
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                                            alu_fmt, MASK_PROFILE_INDEX_UPDATEf,
                                                            act_alu_mask_index);
    }

    return BCM_E_NONE;

cleanup:
    if (pc_alu_mask_index != -1) {
        BCM_IF_ERROR_RETURN(bcmi_ft_group_alu_mask_profile_entry_delete(unit,
                            profile_idx, pc_alu_mask_index));
    }
    if (sc_alu_mask_index != -1) {
        BCM_IF_ERROR_RETURN(bcmi_ft_group_alu_mask_profile_entry_delete(unit,
                            profile_idx, sc_alu_mask_index));
    }
    if (act_alu_mask_index != -1) {
        BCM_IF_ERROR_RETURN(bcmi_ft_group_alu_mask_profile_entry_delete(unit,
                            profile_idx, act_alu_mask_index));
    }
    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_group_alu16_mask_clear
 * Purpose:
 *   Clear alu16 mask profile entry
 * Parameters:
 *   unit    - (IN) BCM device id.
 *   mem     - (IN) alu16 profile memory
 *   index   - (IN) memory index
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_group_alu16_mask_clear(int unit,
                                 soc_mem_t mem,
                                 int index)
{
    uint32 alu_mask_index = 0;
    uint32 alu16_ctrlfmt[7];
    uint32 alu_base_ctrlfmt[5];
    uint32 alu16_specific_ctrlfmt[2];
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;

    /* initialise variables */
    sal_memset(&alu16_entry, 0, sizeof(bsc_dg_group_alu16_profile_0_entry_t));
    sal_memset(alu_base_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu16_specific_ctrlfmt, 0, 2 * sizeof(uint32));
    sal_memset(alu16_ctrlfmt, 0, 7 * sizeof(uint32));

    /* first read entry */
    BCM_IF_ERROR_RETURN
          (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &alu16_entry));

    /* retreive attribute mask profile index and delete if enabled */
    soc_mem_field_get(unit, mem, (uint32 *)&alu16_entry, DATAf, alu16_ctrlfmt);
    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU16_CONTROL_BUSfmt,
                       alu16_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

    if (soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_EN_CHECK_0f)) {
        alu_mask_index = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_PROFILE_INDEX_CHECK_0f);
        BCM_IF_ERROR_RETURN
              (bcmi_ft_group_alu_mask_profile_entry_delete(unit, 0,
                alu_mask_index));
    }

    if (soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_EN_CHECK_1f)) {
        alu_mask_index = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_PROFILE_INDEX_CHECK_1f);
        BCM_IF_ERROR_RETURN
              (bcmi_ft_group_alu_mask_profile_entry_delete(unit, 0,
                alu_mask_index));
    }

    if (soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_EN_CHECK_UPDATEf)) {
        alu_mask_index = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_PROFILE_INDEX_UPDATEf);
        BCM_IF_ERROR_RETURN
              (bcmi_ft_group_alu_mask_profile_entry_delete(unit, 0,
                alu_mask_index));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_group_alu32_mask_clear
 * Purpose:
 *   Clear alu32 mask profile entry
 * Parameters:
 *   unit    - (IN) BCM device id.
 *   alu_load_type - (IN) ALU/LOAD Type
 *   mem     - (IN) alu32 profile memory
 *   index   - (IN) memory index
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_group_alu32_mask_clear(int unit,
                                 bcmi_ft_alu_load_type_t alu_load_type,
                                 soc_mem_t mem,
                                 int index)
{
    int profile_idx = 0;
    uint32 alu_mask_index = 0;
    uint32 alu32_ctrlfmt[10];
    uint32 alu_base_ctrlfmt[5];
    uint32 alu32_specific_ctrlfmt[5];
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;

    /* initialise variables */
    sal_memset(&alu32_entry, 0, sizeof(bsc_dg_group_alu32_profile_0_entry_t));
    sal_memset(alu_base_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu32_specific_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu32_ctrlfmt, 0, 10 * sizeof(uint32));

    if (alu_load_type == bcmiFtAluLoadTypeAggAlu32) {
        profile_idx = 1;
    } else {
        profile_idx = 0;
    }

    /* first read entry */
    BCM_IF_ERROR_RETURN
          (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &alu32_entry));

    soc_mem_field_get(unit, mem, (uint32 *)&alu32_entry, DATAf, alu32_ctrlfmt);
    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
                       alu32_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

    /* retreive attribute mask profile index and delete if enabled */
    if (soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_EN_CHECK_0f)) {
        alu_mask_index = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_PROFILE_INDEX_CHECK_0f);
        BCM_IF_ERROR_RETURN
              (bcmi_ft_group_alu_mask_profile_entry_delete(unit,
                profile_idx, alu_mask_index));
    }

    if (soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_EN_CHECK_1f)) {
        alu_mask_index = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_PROFILE_INDEX_CHECK_1f);
        BCM_IF_ERROR_RETURN
              (bcmi_ft_group_alu_mask_profile_entry_delete(unit,
                profile_idx, alu_mask_index));
    }

    if (soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_EN_CHECK_UPDATEf)) {
        alu_mask_index = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_base_ctrlfmt, MASK_PROFILE_INDEX_UPDATEf);
        BCM_IF_ERROR_RETURN
              (bcmi_ft_group_alu_mask_profile_entry_delete(unit,
                profile_idx, alu_mask_index));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_group_param_delay_set
 * Purpose:
 *   Set delay for param if requested
 * Parameters:
 *   unit                    - (IN) BCM device id
 *   id                      - (IN) FT group id.
 *   alu_info                - (IN) ALU information of group.
 *   alu32_specific_ctrlfmt  - (IN) ALU format info of memory
 * Returns:
 *   NONE
 */
int
bcmi_ftv2_group_param_delay_set(int unit,
                                bcm_flowtracker_group_t id,
                                bcmi_ft_group_alu_info_t *alu_info,
                                uint32 *alu32_specific_ctrlfmt)
{
    uint8 delay_gran = 0;
    int delay_offset = 0;
    int hw_src_ts = 0, hw_dst_ts = 0, trig = 0;
    bcm_flowtracker_timestamp_source_t ts_src = 0;
    bcm_flowtracker_timestamp_source_t ts_dst = 0;

    /* input params check */
    if ((alu_info == NULL) || (alu32_specific_ctrlfmt == NULL)) {
        return BCM_E_INTERNAL;
    }

    /* set tracking param delay */
    if ((alu_info->element_type1 ==
                   bcmFlowtrackerTrackingParamTypeChipDelay) ||
        (alu_info->element_type1 ==
                   bcmFlowtrackerTrackingParamTypeE2EDelay) ||
        (alu_info->element_type1 ==
                   bcmFlowtrackerTrackingParamTypeIPATDelay) ||
        (alu_info->element_type1 ==
                   bcmFlowtrackerTrackingParamTypeIPDTDelay)) {

        if (alu_info->element_type1 ==
                      bcmFlowtrackerTrackingParamTypeChipDelay) {

            delay_gran = BCMI_FT_RESOURCE_SW_INFO_CHIP_GRAN(unit);
            delay_offset = BCMI_FT_RESOURCE_SW_INFO_CHIP_OFFSET(unit);
            /* Since ChipDelay is EFT, this is ingress MAC timestamp */
            ts_dst = bcmFlowtrackerTimestampSourcePacket;

        } else if (alu_info->element_type1 ==
                             bcmFlowtrackerTrackingParamTypeE2EDelay) {

            delay_gran = BCMI_FT_RESOURCE_SW_INFO_E2E_GRAN(unit);
            delay_offset = BCMI_FT_RESOURCE_SW_INFO_E2E_OFFSET(unit);
            /* Since E2E is IFT, this is In-situ OAM TS in the packet */
            ts_dst = bcmFlowtrackerTimestampSourcePacket;

        } else if (alu_info->element_type1 ==
                             bcmFlowtrackerTrackingParamTypeIPATDelay) {

            delay_gran = BCMI_FT_RESOURCE_SW_INFO_IPAT_GRAN(unit);
            delay_offset = BCMI_FT_RESOURCE_SW_INFO_IPAT_OFFSET(unit);
            for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {
                if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << trig)) {
                    if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) &
                                                BCMI_FT_TS_INTERNAL_IPAT) {
                        break;
                    }
                }
            }
            if (trig == BCMI_FT_GROUP_MAX_TS_TYPE) {
                /* trigger for ipat should have been allocated earlier,
                 * ideally this should never be hit */
                return BCM_E_INTERNAL;
            }

            /* retreive destination from trigger set */
            bcmi_ft_group_ts_type_src_get(unit, id, trig, &ts_dst);

        } else if (alu_info->element_type1 ==
                             bcmFlowtrackerTrackingParamTypeIPDTDelay) {

            delay_gran = BCMI_FT_RESOURCE_SW_INFO_IPDT_GRAN(unit);
            delay_offset = BCMI_FT_RESOURCE_SW_INFO_IPDT_OFFSET(unit);
            for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {
                if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << trig)) {
                    if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) &
                                                BCMI_FT_TS_INTERNAL_IPDT) {
                        break;
                    }
                }
            }
            if (trig == BCMI_FT_GROUP_MAX_TS_TYPE) {
                /* trigger for ipdt should have been allocated earlier,
                 * ideally this should never be hit */
                return BCM_E_INTERNAL;
            }

            /* retreive destination from trigger set */
            bcmi_ft_group_ts_type_src_get(unit, id, trig, &ts_dst);
        }

        /* retreive timestamp source from tracking param */
        BCM_IF_ERROR_RETURN
            (bcmi_ftv2_group_param_ts_src_get(unit, id,
                                              alu_info->element_type1,
                                              0, &ts_src));

        /* retreive hw ts value from source timestamp */
        BCM_IF_ERROR_RETURN
            (bcmi_ftv2_alu_hw_timestamp_get(unit, id, 0,
                                            ts_src, &hw_src_ts));

        /* retreive hw ts value from destination timestamp */
        BCM_IF_ERROR_RETURN
            (bcmi_ftv2_alu_hw_timestamp_get(unit, id, 0,
                                            ts_dst, &hw_dst_ts));

        /* update delay in action */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, UPDATE_ATTR_1_TS_SELECTf, 1);

        /* first delay operand */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, FIRST_DELAY_OPERANDf, hw_src_ts);

        /* second delay operand */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, SECOND_DELAY_OPERANDf, hw_dst_ts);

        /* delay mode */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_MODEf,
                BCMI_FT_GROUP_TS_SOURCE_DELAY_MODE(ts_src));

        /* delay granularity */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_GRANULARITYf,
                delay_gran);

        /* delay offset */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_OFFSETf,
                delay_offset);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_group_check_delay_set
 * Purpose:
 *   Set delay in checker if requested
 * Parameters:
 *  unit                   - (IN) BCM device id.
 *  id                     - (IN) flowtracker group id.
 *  fc_info                - (IN) Flow checker info.
 *  alu32_specific_ctrlfmt - (OUT) ALU32 format buffer.
 * Returns:
 *  BCM_E_XXX.
 */
int
bcmi_ftv2_group_check_delay_set(int unit,
                                bcm_flowtracker_group_t id,
                                bcmi_flowtracker_flowchecker_info_t *fc_info,
                                uint32 *alu32_specific_ctrlfmt)
{
    int hw_src_ts = 0, hw_dst_ts = 0;
    int dual_operation = FALSE;
    int is_check1_param_delay = FALSE;

    /* input params check */
    if ((fc_info == NULL) || (alu32_specific_ctrlfmt == NULL)) {
        return BCM_E_INTERNAL;
    }

    dual_operation = bcmi_ft_flowchecker_opr_is_dual(unit,
                        fc_info->check1.operation);

    /* retreive delay info and set */
    if ((fc_info->check1.param ==
                  bcmFlowtrackerTrackingParamTypeIngressDelay) ||
        (fc_info->check1.param ==
                  bcmFlowtrackerTrackingParamTypeEgressDelay) ||
        (fc_info->check2.param ==
                  bcmFlowtrackerTrackingParamTypeIngressDelay) ||
        (fc_info->check2.param ==
                  bcmFlowtrackerTrackingParamTypeEgressDelay) ||
        (fc_info->action_info.param ==
                  bcmFlowtrackerTrackingParamTypeIngressDelay) ||
        (fc_info->action_info.param ==
                  bcmFlowtrackerTrackingParamTypeEgressDelay)) {

        /* retreive hw ts value from source timestamp */
        BCM_IF_ERROR_RETURN
            (bcmi_ftv2_alu_hw_timestamp_get(unit, id, 1,
                                  fc_info->delay_info.src_ts, &hw_src_ts));

        /* retreive hw ts value from destination timestamp */
        BCM_IF_ERROR_RETURN
            (bcmi_ftv2_alu_hw_timestamp_get(unit, id, 1,
                                  fc_info->delay_info.dst_ts, &hw_dst_ts));

        /* select delay as primary check param if requested */
        if ((fc_info->check1.param ==
                      bcmFlowtrackerTrackingParamTypeIngressDelay) ||
            (fc_info->check1.param ==
                      bcmFlowtrackerTrackingParamTypeEgressDelay)) {
            soc_format_field32_set(unit,
                       BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                       alu32_specific_ctrlfmt, CHECK_ATTR_0_TS_SELECTf, 1);
            is_check1_param_delay = TRUE;
        }

        /* select delay as secondary check param if requested */
        if (((dual_operation == TRUE) && (is_check1_param_delay == TRUE)) ||
            ((fc_info->check2.param ==
                      bcmFlowtrackerTrackingParamTypeIngressDelay) ||
            (fc_info->check2.param ==
                      bcmFlowtrackerTrackingParamTypeEgressDelay))) {
            soc_format_field32_set(unit,
                       BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                       alu32_specific_ctrlfmt, CHECK_ATTR_1_TS_SELECTf, 1);
        }

        /* select delay as action param if requested */
        if ((fc_info->action_info.param ==
                      bcmFlowtrackerTrackingParamTypeIngressDelay) ||
            (fc_info->action_info.param ==
                      bcmFlowtrackerTrackingParamTypeEgressDelay)) {
            soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                    alu32_specific_ctrlfmt, UPDATE_ATTR_1_TS_SELECTf, 1);
        }

        /* first delay operand */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, FIRST_DELAY_OPERANDf, hw_src_ts);

        /* second delay operand */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, SECOND_DELAY_OPERANDf, hw_dst_ts);

        /* delay mode */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_MODEf,
                BCMI_FT_GROUP_TS_SOURCE_DELAY_MODE(fc_info->delay_info.src_ts));

        /* delay granularity */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_GRANULARITYf,
                fc_info->delay_info.gran);

        /* delay offset */
        soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_OFFSETf,
                fc_info->delay_info.offset);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_alu_hw_timestamp_get
 * Purpose:
 *   Get Hw timestamp value from Sw provided
 *   timestamp source value
 * Parameters:
 *   unit  - (IN)  BCM device id
 *   id    - (IN)  Group id
 *   user_req - (IN) User requested timestamp event
 *   sw_ts - (IN)  Sw timestamp value
 *   hw_ts - (OUT) Hw timestamp value
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_alu_hw_timestamp_get(int unit,
                               bcm_flowtracker_group_t id,
                               int user_req,
                               bcm_flowtracker_timestamp_source_t sw_ts,
                               int *hw_ts)
{
    uint16 trig = 0;
    const char *ts_srclist[] = BCM_FLOWTRACKER_TIMESTAMP_SOURCE_STRINGS;

    /* input params check */
    if ((hw_ts == NULL) || (sw_ts >= bcmFlowtrackerTimestampSourceCount)) {
        return BCM_E_PARAM;
    }

    switch(sw_ts) {
        case bcmFlowtrackerTimestampSourceLegacy:
        case bcmFlowtrackerTimestampSourceNTP:
        case bcmFlowtrackerTimestampSourcePTP:
        case bcmFlowtrackerTimestampSourceCMIC:
        case bcmFlowtrackerTimestampSourceIngressFlowtrackerStage:
        case bcmFlowtrackerTimestampSourcePacket:
        case bcmFlowtrackerTimestampSourceIngress:
        case bcmFlowtrackerTimestampSourceCMICTxStart:
        case bcmFlowtrackerTimestampSourceCMICTxEnd:
            /* Hw timestamp value for these sources are straight
             * forward encoded and fixed always */
            *hw_ts = BCMI_FT_GROUP_TS_CHECK_HW_SOURCE(sw_ts);
            break;
        case bcmFlowtrackerTimestampSourceNewLearn:
        case bcmFlowtrackerTimestampSourceFlowStart:
        case bcmFlowtrackerTimestampSourceFlowEnd:
        case bcmFlowtrackerTimestampSourceCheckEvent1:
        case bcmFlowtrackerTimestampSourceCheckEvent2:
        case bcmFlowtrackerTimestampSourceCheckEvent3:
        case bcmFlowtrackerTimestampSourceCheckEvent4:

            /* Retreive trigger type for above sources first */
            trig = BCMI_FT_GROUP_TS_TRIGGER(sw_ts);

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
                    BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
                *hw_ts = sw_ts - bcmFlowtrackerTimestampSourceCheckEvent1 + 2;
                break;
            }
#endif

            if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) &
                    BCMI_FT_TS_RESERVED_INGRESS) {
                trig = BCMI_FT_GROUP_TS_INGRESS;
            }
            if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) &
                    BCMI_FT_TS_RESERVED_EGRESS) {
                trig = BCMI_FT_GROUP_TS_EGRESS;
            }

            /* Check if any checker is added prior to
             * this with configured trigger source */
            if (!BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id , trig)) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                   "No checker configured to give timestamp source <%s>.\n"),
                                                        ts_srclist[sw_ts]));
                return BCM_E_CONFIG;
            }

            /* If user has requested the timestamp event, then internal
             * trigger flags should not be set
             */
            if ((user_req) && (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) &
                        (BCMI_FT_TS_INTERNAL_IPAT | BCMI_FT_TS_INTERNAL_IPDT))) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "No checker configured to give timestamp"
                                "source <%s>.\n"), ts_srclist[sw_ts]));
                return BCM_E_CONFIG;
            }

            /* Find TS Bank associated with trigger
             * and fill hardware value accordingly */
            if (BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id , trig) == 0) {
                *hw_ts = 2;
            } else if (BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id , trig) == 1) {
                *hw_ts = 3;
            } else if (BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id , trig) == 2) {
                *hw_ts = 4;
            } else if (BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id , trig) == 3) {
                *hw_ts = 5;
            }

            break;

        default :
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_alu_kmap_opr_result_get
 * Purpose:
 *   Get Kmap operator result for flow check.
 * Parameters:
 *   unit        - (IN) BCM device id.
 *   fc_info     - (IN) flowchecker info.
 *   opr_result  - (OUT) operator result
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_alu_kmap_opr_result_get(
    int unit,
    bcmi_flowtracker_flowchecker_info_t *fc_info,
    int *opr_result)
{
    int ctrl_flag1 = 0, ctrl_flag2 = 0;

    if (fc_info != NULL) {

        /* Unicast */
        if (fc_info->flags &
                BCM_FLOWTRACKER_CHECK_UNICAST_FLOW_ONLY) {
            opr_result[BCMI_FT_ALU_KMAP_BIT_SEL_STATE_TRIG] = 0;
            /* Multicast*/
        } else if (fc_info->flags &
                BCM_FLOWTRACKER_CHECK_MULTICAST_FLOW_ONLY) {
            opr_result[BCMI_FT_ALU_KMAP_BIT_SEL_STATE_TRIG] = 1;
            /* State Transition Event */
        } else if (fc_info->check1.state_transition_flags > 0) {
            opr_result[BCMI_FT_ALU_KMAP_BIT_SEL_STATE_TRIG] = 1;
        }

        /* Support for 64b NTP Timestamp */
        switch(fc_info->check1.param) {
            case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
                /* Get bit positions for SYN and ACK */
                BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit,
                            -1, _bcmFieldFtAluDataTcpFlagsSYN, &ctrl_flag1,
                            NULL));
                BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit,
                            -1, _bcmFieldFtAluDataTcpFlagsACK, &ctrl_flag2,
                            NULL));

                if ((ctrl_flag1 >= 5) || (ctrl_flag2 >= 5)) {
                    return BCM_E_INTERNAL;
                }
                opr_result[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS0 + ctrl_flag1] = 1;
                opr_result[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS0 + ctrl_flag2] = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
                /* Get bit positions for FIN and RST */
                BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit,
                            -1, _bcmFieldFtAluDataTcpFlagsFIN, &ctrl_flag1,
                            NULL));
                BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit,
                            -1, _bcmFieldFtAluDataTcpFlagsRST, &ctrl_flag2,
                            NULL));

                if ((ctrl_flag1 > 5) || (ctrl_flag2 > 5)) {
                    return BCM_E_INTERNAL;
                }
                opr_result[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS0 + ctrl_flag1] = 1;
                opr_result[BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS0 + ctrl_flag2] = 1;
                break;
            default:
                break;
        }
    }

    return BCM_E_NONE;
}

#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)

/*
 * Function:
 *  bcmi_ftv3_alu_load_mem_config_init
 * Purpose:
 *  Allocate flowtracker resources as per user provided config.
 * Parameters:
 *  unit - (IN) BCM device id.
 * Notes:
 *  Intent of this api is to allocate flowtracker resources
 *  among AIFT/AMFT/AEFT stages as per user provided config and
 *  update software tracking structures. It also program
 *  resource mapping hardware registers.
 * Returns:
 *  BCM_E_XXX
 */
int
bcmi_ftv3_alu_load_mem_config_init(
    int unit)
{
    int i = 0;
    int res = 0;
    int max_alu32 = 0;
    char *config_str = NULL, *sub_str = NULL, *sub_str_end = NULL;
    uint32 aift_r = 0, amft_r = 0, aeft_r = 0, unallocated_r = 0;
    uint32 reg_val = 0;
    soc_reg_t reg = INVALIDr;
    soc_field_t bank_array[] = {BANK_0f, BANK_1f, BANK_2f, BANK_3f};

    reg = BSC_DT_BANK_TO_AGG_INSTANCE_MAPPING_CONTROLr;

    max_alu32 = BCMI_FTv3_AGG_SESSION_DATA_BANKS * BCMI_FTv3_AGG_ALU32_PER_BANK;

    config_str = soc_property_get_str(unit, spn_FLOWTRACKER_AGGREGATE_ALU32_MEM_BANKS);

    /* Validate and retreive resources from config specified by user */
    if (config_str != NULL) {

        /* Get AIFT resources first */
        sub_str = config_str;
        aift_r = sal_ctoi(sub_str, &sub_str_end);

        /* Get AMFT resources second */
        sub_str = sub_str_end;
        if (*sub_str != ':') {
            LOG_CLI((BSL_META_U(unit,
                            "Bad config string \"%s\"\n"), config_str));
            return BCM_E_CONFIG;
        }
        sub_str++;
        amft_r = sal_ctoi(sub_str, &sub_str_end);

        /* Get AEFT resources in last */
        sub_str = sub_str_end;
        if (*sub_str != ':') {
            LOG_CLI((BSL_META_U(unit,
                            "Bad config string \"%s\"\n"), config_str));
            return BCM_E_CONFIG;
        }
        sub_str++;
        aeft_r = sal_ctoi(sub_str, &sub_str_end);

        /* validate if allocation does not exceed maximum allowed */
        if (aift_r + amft_r + aeft_r > max_alu32) {
            return BCM_E_RESOURCE;
        }
    }

    /* add any unallocated resource to AIFT stage if any */
    unallocated_r = max_alu32 - aift_r - amft_r - aeft_r;
    aift_r += unallocated_r;

    if (((aift_r % BCMI_FTv3_AGG_ALU32_PER_BANK) != 0) ||
            ((amft_r % BCMI_FTv3_AGG_ALU32_PER_BANK) != 0) ||
            ((aeft_r % BCMI_FTv3_AGG_ALU32_PER_BANK) != 0)) {
        return BCM_E_RESOURCE;
    }

    aift_r /= BCMI_FTv3_AGG_ALU32_PER_BANK;
    amft_r /= BCMI_FTv3_AGG_ALU32_PER_BANK;
    aeft_r /= BCMI_FTv3_AGG_ALU32_PER_BANK;

    /* Update Hardware registers */
    for (res = 0; res < aift_r; res++) {
        soc_reg_field_set(unit, reg, &reg_val, bank_array[res],
                BCMI_FTv3_STAGE_AIFT);
        for (i = 0; i < BCMI_FTv3_AGG_ALU32_PER_BANK; i++) {
            BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAggAlu32,
                            res * BCMI_FTv3_AGG_ALU32_PER_BANK + i,
                            0) = bcmiFtTypeAIFT;
        }
        BCMI_FT_AGG_SESSION_DATA_BANK_FT_TYPE(unit, res, 0) = bcmiFtTypeAIFT;
    }
    for (; res < (aift_r + amft_r); res++) {
        soc_reg_field_set(unit, reg, &reg_val, bank_array[res],
                BCMI_FTv3_STAGE_AMFT);
        for (i = 0; i < BCMI_FTv3_AGG_ALU32_PER_BANK; i++) {
            BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAggAlu32,
                            res * BCMI_FTv3_AGG_ALU32_PER_BANK + i,
                            0) = bcmiFtTypeAMFT;
        }
        BCMI_FT_AGG_SESSION_DATA_BANK_FT_TYPE(unit, res, 0) = bcmiFtTypeAMFT;
    }
    for (; res < (aift_r + amft_r + aeft_r); res++) {
        soc_reg_field_set(unit, reg, &reg_val, bank_array[res],
                BCMI_FTv3_STAGE_AEFT);
        for (i = 0; i < BCMI_FTv3_AGG_ALU32_PER_BANK; i++) {
            BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, bcmiFtAluLoadTypeAggAlu32,
                            res * BCMI_FTv3_AGG_ALU32_PER_BANK + i,
                            0) = bcmiFtTypeAEFT;
        }
        BCMI_FT_AGG_SESSION_DATA_BANK_FT_TYPE(unit, res, 0) = bcmiFtTypeAEFT;
    }

    WRITE_BSC_DT_BANK_TO_AGG_INSTANCE_MAPPING_CONTROLr(unit, reg_val);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_alu_check_attr_select_get
 * Purpose:
 *   Get the attribute selection for check0/1
 *    and update operation.
 * Parameters:
 *   unit              - (IN) BCM device id
 *   fc_info           - (IN) Flowtracker info
 *   check1_attr_sel   - (OUT) Selector for Check 1
 *   check2_attr_sel   - (OUT) Selector for Check 2
 *   update_attr_sel   - (OUT) Selector for update
 * Returns:
 *   NONE
 */
void
bcmi_ftv3_alu_check_attr_select_get(
    int unit,
    bcmi_flowtracker_flowchecker_info_t *fc_info,
    int *check1_attr_sel,
    int *check2_attr_sel,
    int *update_attr_sel)
{
    *check1_attr_sel = BCMI_FT_ALU_ATTR_SELECT_EXTRACTION;
    *check2_attr_sel = BCMI_FT_ALU_ATTR_SELECT_EXTRACTION;
    *update_attr_sel = BCMI_FT_ALU_ATTR_SELECT_EXTRACTION;

    /* Use Extraction for if not Flow Check */
    if (fc_info == NULL) {
        return;
    }

    if (fc_info->check2.param ==
            bcmFlowtrackerTrackingParamTypeByteCount) {
        *check2_attr_sel = BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT;
    }

    if (fc_info->check1.param ==
            bcmFlowtrackerTrackingParamTypeByteCount) {
        *check1_attr_sel = BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT;

        if (bcmi_ft_flowchecker_opr_is_dual(unit,
                    fc_info->check1.operation)) {
            *check2_attr_sel = BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT;
        }
    }
    if (fc_info->action_info.param ==
            bcmFlowtrackerTrackingParamTypeByteCount) {
        *update_attr_sel = BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT;
    }
}

/*
 * Function:
 *   bcmi_ftv3_flow_transition_trigger_update
 * Purpose:
 *   Update Flow transition state TCAM for ALU triggers.
 * Parameters:
 *   unit        - (IN) BCM device id.
 *   alu_load_type - (IN) ALU LOAD Type.
 *   index       - (IN) Index of ALU.
 *   info        - (IN) alu load info.
 *   fc_info     - (IN) flowchecker info.
 *   set         - (IN) Set or reset.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv3_flow_transition_trigger_update(
    int unit,
    bcmi_ft_alu_load_type_t alu_load_type,
    int index,
    bcmi_ft_group_alu_info_t *info,
    int set)
{
    int rv = BCM_E_NONE;
    int iter = 0;
    int alu_bank = 0;
    uint32 event_flag, transitions_flags = 0;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    sal_memset(&fc_info, 0,
            sizeof(bcmi_flowtracker_flowchecker_info_t));

    if (alu_load_type != bcmiFtAluLoadTypeAggAlu32) {
        return BCM_E_NONE;
    }

    if (info->flowchecker_id > 0) {

        sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
        rv = bcmi_ft_flowchecker_get(unit, info->flowchecker_id, &fc_info);
        BCM_IF_ERROR_RETURN(rv);

        /* Index is obtained, convert the index into the memory type. */
        bcmi_ft_alu_load_mem_index_get(unit, index,
                alu_load_type, NULL, NULL, &alu_bank);

        transitions_flags = fc_info.check1.state_transition_flags;

        /* Iterate State Transition flags */
        for (iter = 0; iter < 9; iter++) {
            event_flag = 1 << iter;
            if (transitions_flags & event_flag) {
                rv = bcmi_ftv3_flow_transition_alu_trigger_set(unit,
                        event_flag, alu_bank, set);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }

    return rv;
}

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#else /* BCM_FLOWTRACKER_SUPPORT*/
typedef int bcmi_ft_alu_load_hw_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT */
