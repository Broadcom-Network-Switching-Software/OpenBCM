/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_group.c
 * Purpose:     The purpose of this file is to set flow tracker group methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>

#ifdef BCM_FLOWTRACKER_SUPPORT

/*
 * 1. Allocate resource for LOAD/ALU based on element.
 * 2. Add the ALU information into the list group sw state.
 * 3. when the entry is installed then add the entry into hw.
 * 4. Set all the checks in hardware.
 * 5.
*/

/* Group Sw state. */
extern
bcmi_ft_group_sw_info_t **bcmi_ft_group_sw_state[BCM_MAX_NUM_UNITS];

/* ALU/LOAD vectore. */
bcmi_ft_alu_load_method_t *alu_load_methods[BCMI_FT_ALU_LOAD_MEMS];

/* Element Informations which can take action on their own. */
STATIC
 bcmi_ft_element_alu_info_t ft_element_info[] = {
    {bcmFlowtrackerTrackingParamTypePktCount,
         bcmFlowtrackerCheckOpNone,  bcmFlowtrackerCheckActionCounterIncr},
    {bcmFlowtrackerTrackingParamTypeByteCount,
         bcmFlowtrackerCheckOpNone,  bcmFlowtrackerCheckActionCounterIncrByPktBytes},
    {bcmFlowtrackerTrackingParamTypeVxlanNetworkId,
         bcmFlowtrackerCheckOpNone, bcmFlowtrackerCheckActionUpdateValue},
    {bcmFlowtrackerTrackingParamTypeTunnelClass,
         bcmFlowtrackerCheckOpNone, bcmFlowtrackerCheckActionUpdateValue},
    {bcmFlowtrackerTrackingParamTypeDosAttack,
         bcmFlowtrackerCheckOpNone, bcmFlowtrackerCheckActionCounterIncr},
    {bcmFlowtrackerTrackingParamTypeInnerDosAttack,
         bcmFlowtrackerCheckOpNone, bcmFlowtrackerCheckActionCounterIncr},
    {bcmFlowtrackerTrackingParamTypeCount, 0, 0}
    };

/*|************************************************************|
  |********         ALU Software Install Methods.      ********|
  |************************************************************|
  |*******                 Start                       ********|
  |************************************************************|*/


/* ALu/Load vectors. */
bcmi_ft_alu_load_method_t alu32_methods[] =
    {{bcmi_ft_alu_entry_match,
      bcmi_ft_alu32_entry_install,
      bcmi_ft_alu32_entry_uninstall}};

/* ALu/Load vectors. */
bcmi_ft_alu_load_method_t alu16_methods[] =
    {{bcmi_ft_alu_entry_match,
     bcmi_ft_alu16_entry_install,
     bcmi_ft_alu16_entry_uninstall}};

/* ALu/Load vectors. */
bcmi_ft_alu_load_method_t load16_methods[] =
    {{bcmi_ft_load16_entry_match,
     bcmi_ft_load16_entry_install,
     bcmi_ft_load16_entry_uninstall}};

/* ALu/Load vectors. */
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


alu_load_info_t alu_32_info[TOTAL_GROUP_ALU32_MEM] =
    {{BSC_DG_GROUP_ALU32_PROFILE_0m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_1m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_2m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_3m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_4m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_5m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_6m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_7m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_8m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_9m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_10m, -1},
     {BSC_DG_GROUP_ALU32_PROFILE_11m, -1}};


alu_load_info_t alu_16_info[TOTAL_GROUP_ALU16_MEM] =
    {{BSC_DG_GROUP_ALU16_PROFILE_0m, -1},
     {BSC_DG_GROUP_ALU16_PROFILE_1m, -1},
     {BSC_DG_GROUP_ALU16_PROFILE_2m, -1},
     {BSC_DG_GROUP_ALU16_PROFILE_3m, -1},
     {BSC_DG_GROUP_ALU16_PROFILE_4m, -1},
     {BSC_DG_GROUP_ALU16_PROFILE_5m, -1},
     {BSC_DG_GROUP_ALU16_PROFILE_6m, -1},
     {BSC_DG_GROUP_ALU16_PROFILE_7m, -1}};


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

/*
 * Debug Counters information.
 */
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


uint32 bcmi_dg_group_load8_fid[TOTAL_GROUP_LOAD8_MEM] =
    {GROUP_LOAD8_PROFILE_IDXf};

uint32 bcmi_dg_group_load16_fid[TOTAL_GROUP_LOAD16_MEM] =
    {GROUP_LOAD16_PROFILE_IDXf};

alu_load_info_t load_8_info[TOTAL_GROUP_LOAD8_MEM] =
    {{BSC_DG_GROUP_LOAD8_PROFILEm, -1},};
alu_load_info_t load_16_info[TOTAL_GROUP_LOAD16_MEM] =
    {{BSC_DG_GROUP_LOAD16_PROFILEm, -1},};

#define BCMI_FT_ALU_LOAD_MEMS             4

/* There are 4 different types of memories. */
alu_load_index_info_t
    alu_load_index_info[BCM_MAX_NUM_UNITS][BCMI_FT_ALU_LOAD_MEMS];

/*
 * Function:
 *     bcmi_ft_load_pdd_entry_set
 * Purpose:
 *     Set load PDD actions.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     load_type - (IN) Load memory type.
 *     index - (IN) memory index.
 *     clear - (IN) set/reset
 *
 * Returns:
 *     NONE
 */
int
bcmi_ft_load_pdd_entry_set(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_alu_load_type_t load_type,
            int index,
            int clear)
{

    uint32 trigger = 0;
    bsc_dg_group_load16_profile_entry_t load16_entry;
    bsc_dg_group_load8_profile_entry_t load8_entry;
    uint32 ctrlfmt[1];
    uint32 *entry;
    int start_index = 0, i;
    soc_mem_t mem;
    int fmt = 0;

    int data[TOTAL_GROUP_LOAD16_DATA_NUM] =
                   {DATA_15f,
                    DATA_14f,
                    DATA_13f,
                    DATA_12f,
                    DATA_11f,
                    DATA_10f,
                    DATA_9f,
                    DATA_8f,
                    DATA_7f,
                    DATA_6f,
                    DATA_5f,
                    DATA_4f,
                    DATA_3f,
                    DATA_2f,
                    DATA_1f,
                    DATA_0f};

    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));
    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));

    if (load_type == bcmiFtAluLoadTypeLoad16) {
        start_index = 0;
        mem = BSC_DG_GROUP_LOAD16_PROFILEm;
        entry = (uint32 *) (&load16_entry);
        fmt = BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt;
    } else {
        start_index = 8;
        mem = BSC_DG_GROUP_LOAD8_PROFILEm;
        entry = (uint32 *) (&load8_entry);
        fmt = BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry));

    for (i=start_index; i<16; i++) {

        soc_mem_field_get(unit, mem, entry, data[i], ctrlfmt);

        trigger = soc_format_field32_get(unit, fmt, ctrlfmt, LOAD_TRIGGERf);

        if (trigger && (!clear)) {
            /* Set pdd profile entry for this alu entry. */
            soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                (BCMI_FT_PDD_INFO(unit, load_type)[15-i]).param_id, 1);
        } else {
            /* Set pdd profile entry for this alu entry. */
            soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
                (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                (BCMI_FT_PDD_INFO(unit, load_type)[15-i]).param_id, 0);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_element_alu_action_get
 * Purpose:
 *     Get ALU actions.
 * Parameters:
 *     element - (IN) FT element type.
 *     action  - (OUT) Action for element.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_element_alu_action_get(
    bcm_flowtracker_tracking_param_type_t element,
    bcm_flowtracker_check_action_t *action)
{

    int i = 0;

    for (i = 0; i < COUNTOF(ft_element_info); i++) {
        if (element == ft_element_info[i].element) {
            *action = ft_element_info[i].action;
            return BCM_E_NONE;
        }
    }

    return BCM_E_PARAM;
}

/*
 * Function:
 *     bcmi_ft_get_alu32_mem
 * Purpose:
 *     Get alu32 enable memory banks.
 * Parameters:
 *     unit - (IN) BCM device id.
 *
 * Returns:
 *     enabled memory banks
 */
int
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
 *     bcmi_ft_alu_load_init
 * Purpose:
 *     Initialize ALU and LOAD software state.
 * Parameters:
 *     unit - (IN) BCM device id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu_load_init(int unit)
{

    int i=0, j=0;
    int total_indexes_per_mem = 0;
    /* The below should be done during group init. */

    /* Set ALU16 data. */
    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) =
        TOTAL_GROUP_ALU16_MEM;
    BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAlu16) = alu_16_info;
    BCMI_FT_PDD_INFO(unit, bcmiFtAluLoadTypeAlu16) = bcmi_alu16_pdd_info;
    BCMI_FT_GROUP_FID_INFO(unit, bcmiFtAluLoadTypeAlu16) =
        bcmi_dg_group_alu16_fid;
    alu_load_methods[bcmiFtAluLoadTypeAlu16] = alu16_methods;

    /* Set ALU32 data. */
    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) =
        bcmi_ft_get_alu32_mem(unit);
    BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeAlu32) = alu_32_info;
    BCMI_FT_PDD_INFO(unit, bcmiFtAluLoadTypeAlu32) = bcmi_alu32_pdd_info;
    BCMI_FT_GROUP_FID_INFO(unit, bcmiFtAluLoadTypeAlu32) =
        bcmi_dg_group_alu32_fid;
    alu_load_methods[bcmiFtAluLoadTypeAlu32] = alu32_methods;

    /* Set LOAD8 data. */
    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeLoad8) =
        TOTAL_GROUP_LOAD8_MEM;
    BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeLoad8) = load_8_info;
    BCMI_FT_PDD_INFO(unit, bcmiFtAluLoadTypeLoad8) = bcmi_load8_pdd_info;
    BCMI_FT_GROUP_FID_INFO(unit, bcmiFtAluLoadTypeLoad8) =
        bcmi_dg_group_load8_fid;
    alu_load_methods[bcmiFtAluLoadTypeLoad8] = load8_methods;


    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeLoad16) =
        TOTAL_GROUP_LOAD16_MEM;
    BCMI_FT_ALU_LOAD_MEM_INFO(unit, bcmiFtAluLoadTypeLoad16) = load_16_info;
    BCMI_FT_PDD_INFO(unit, bcmiFtAluLoadTypeLoad16) = bcmi_load16_pdd_info;
    BCMI_FT_GROUP_FID_INFO(unit, bcmiFtAluLoadTypeLoad16) =
        bcmi_dg_group_load16_fid;
    alu_load_methods[bcmiFtAluLoadTypeLoad16] = load16_methods;


    /* Set the mem size for the ALUs. */
    for (i=bcmiFtAluLoadTypeLoad8; i<bcmiFtAluLoadTypeNone; i++) {

        total_indexes_per_mem = 0;
        for (j=0; j<alu_load_index_info[unit][i].total_mem_count; j++) {


            alu_load_index_info[unit][i].alu_load_info[j].mem_size =
                soc_mem_index_count(
                    unit, alu_load_index_info[unit][i].alu_load_info[j].mem);

            total_indexes_per_mem +=
                alu_load_index_info[unit][i].alu_load_info[j].mem_size;

        }
        alu_load_index_info[unit][i].total_idx_per_memory =
            total_indexes_per_mem;

         /* lets calculate indexes or all memories of one type. */
         alu_load_index_info[unit][i].index_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(total_indexes_per_mem),
            "alu_load_bitmap");

         if (!alu_load_index_info[unit][i].index_bitmap) {
             bcmi_ft_alu_load_cleanup(unit);
             return BCM_E_MEMORY;
         }

         sal_memset(alu_load_index_info[unit][i].index_bitmap, 0,
            SHR_BITALLOCSIZE(total_indexes_per_mem));

         /* Now allocate the ref count memory. */
         alu_load_index_info[unit][i].idx_refcount =
            sal_alloc((total_indexes_per_mem * sizeof(uint32)),
            "Indexes Refcount");

         if (!alu_load_index_info[unit][i].idx_refcount) {
             bcmi_ft_alu_load_cleanup(unit);
             return BCM_E_MEMORY;
         }

         sal_memset(alu_load_index_info[unit][i].idx_refcount, 0,
                (total_indexes_per_mem * sizeof(uint32)));
    }
    return 0;
}

/*
 * Function:
 *     bcmi_ft_alu_load_cleanup
 * Purpose:
 *     Clear ALU and LOAD software state.
 * Parameters:
 *     unit - (IN) BCM device id.
 *
 * Returns:
 *     BCM_E_XXX
 */
void
bcmi_ft_alu_load_cleanup(int unit)
{

    int i=0, j=0;

    /* Set the mem size for the ALUs. */
    for (i=bcmiFtAluLoadTypeLoad8; i<bcmiFtAluLoadTypeNone; i++) {

        for (; j<alu_load_index_info[unit][i].total_mem_count; j++) {

            alu_load_index_info[unit][i].alu_load_info[j].mem_size = 0;
            alu_load_index_info[unit][i].total_idx_per_memory = 0;
        }

         /* lets calculate indexes or all memories of one type. */
        if (alu_load_index_info[unit][i].index_bitmap) {
            sal_free(alu_load_index_info[unit][i].index_bitmap);
            alu_load_index_info[unit][i].index_bitmap = NULL;
        }

         /* Now allocate the ref count memory. */
        if (alu_load_index_info[unit][i].idx_refcount) {
            sal_free(alu_load_index_info[unit][i].idx_refcount);
            alu_load_index_info[unit][i].idx_refcount = NULL;
        }
    }

    return;
}

/*
 * Function:
 *     bcmi_ft_alu_load_mem_index_get
 * Purpose:
 *     Get hw index for ALU and load memories from sw index.
 * Parameters:
 *     unit - (IN) BCM device id
 *     index   - (IN) sw index.
 *     type - (IN) ALU/Load memory type.
 *     new_index - (OUT) memory index.
 *     mem - (OUT) memory.
 *     a_idx - (IN) array index of the memory in database.
 *
 * Returns:
 *     NONE
 */
void
bcmi_ft_alu_load_mem_index_get(
                int unit,
                int index, bcmi_ft_alu_load_type_t type,
                int *new_index, soc_mem_t *mem, int *a_idx)
{

    alu_load_info_t *temp;
    int total_no_of_mem_entries = 0;
    int i = 0;
    int prev_index = 0, temp_index = 0;

    total_no_of_mem_entries = BCMI_FT_ALU_INFO(unit, type).total_mem_count;

    temp = BCMI_FT_ALU_INFO(unit, type).alu_load_info;

    for(i=0; i<(total_no_of_mem_entries); i++) {

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
 *     bcmi_ft_alu_hw_operation_get
 * Purpose:
 *     Get hw operations based on sw operations.
 * Parameters:
 *     unit - (IN) BCM device id
 *     operation   - (IN) sw operation.
 *     check_0_operation - (IN) hw operation 1
 *     check_1_operation - (IN) hw operation 2
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu_hw_operation_get(
                int unit,
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

        default :
            *check_0_operation = 6;
            *check_1_operation = 6;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_flowchecker_opr_is_dual
 * Purpose:
 *     Check if hw operations is using dual actions.
 * Parameters:
 *     unit - (IN) BCM device id
 *     operation   - (IN) sw operation.
 *
 * Returns:
 *     TRUE/FALSE
 */
int
bcmi_ft_flowchecker_opr_is_dual(
                int unit,
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
 *     bcmi_ft_flowchecker_data_is_valid
 * Purpose:
 *     Check if flowchecker data is valid
 * Parameters:
 *     unit - (IN) BCM device id
 *     fc_info   - (IN) Flowchecker information.
 *
 * Returns:
 *     TRUE/FALSE
 */
int
bcmi_ft_flowchecker_data_is_valid(
                int unit,
                bcmi_flowtracker_flowchecker_info_t *fc_info)
{

    if (bcmi_ft_flowchecker_opr_is_dual(unit, fc_info->check1.operation)) {
        /*
         * If check1 is already using both hw operation
         * then check 1 should be empty.
         */
        if (fc_info->check2.param != bcmFlowtrackerTrackingParamTypeNone) {
            return 0;
        }
    }

    return 1;
}

/*
 * Function:
 *     bcmi_ft_flowchecker_values_get
 * Purpose:
 *     Check if flowchecker data is valid
 * Parameters:
 *     unit - (IN) BCM device id
 *     fc_info   - (IN) Flowchecker information.
 *     val1     - (OUT) value for check 1.
 *     val2     - (OUT) value for check 2.
 *     opr1     - (OUT) operation for check 1.
 *     opr2     - (OUT) operation for check 2.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_flowchecker_values_get(int unit,
                bcmi_flowtracker_flowchecker_info_t *fc_info,
                int *val1, int *val2, int *opr1, int *opr2)
{
    int l_opr1 = -1, l_opr2 = -1;

    *val1 = *val2 = *opr1 = *opr2 = -1;

    /* Sanity check first. */
    if (!bcmi_ft_flowchecker_data_is_valid(unit, fc_info)) {
        return BCM_E_PARAM;
    }

    if (fc_info->check1.param) {
        /* Get check1 operation and value first. */
        BCM_IF_ERROR_RETURN(bcmi_ft_alu_hw_operation_get(unit,
            fc_info->check1.operation, &l_opr1, &l_opr2));

        *opr1 =  ((l_opr1 == -1) ? l_opr2 : l_opr1);
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
    }

    l_opr1 = l_opr2 = -1;

    if (fc_info->check2.param) {
        /* Get check1 operation and value first. */
        BCM_IF_ERROR_RETURN(bcmi_ft_alu_hw_operation_get(unit,
            fc_info->check2.operation, &l_opr1, &l_opr2));

        *opr2 =  ((l_opr1 == -1) ? l_opr2 : l_opr1);
        *val2 = ((l_opr1 == -1) ? fc_info->check2.max_value :
                    fc_info->check2.min_value);

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_kmap_val_get
 * Purpose:
 *     Get ALU KMAP value.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     param  - (IN) tracking paramete.
 *     kmap_val  - (OUT) Kmap value.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_kmap_val_get(
                int unit,
                bcmi_flowtracker_flowchecker_info_t *fc_info,
                bcm_flowtracker_check_operation_t operation,
                uint32 *kmap_val)
{


    if (operation >= bcmFlowtrackerCheckOpCount) {
        return BCM_E_PARAM;
    }


    if (soc_feature(unit, soc_feature_hx5_ft_32bit_param_update)) {

        if (((fc_info->check1.param ==
             bcmFlowtrackerTrackingParamTypeDosAttack) ||
            (fc_info->check1.param ==
             bcmFlowtrackerTrackingParamTypeInnerDosAttack)) &&
                (fc_info->check1.operation == bcmFlowtrackerCheckOpNotEqual) &&
                    (fc_info->action_info.action ==
                     bcmFlowtrackerCheckActionUpdateValue)) {

            *kmap_val = 0xa004;
            return BCM_E_NONE;
        }
    }


    switch(operation) {

        case  bcmFlowtrackerCheckOpOutofRange:
            *kmap_val = 0x2004;
        break;
        default:
            *kmap_val = 0x8000;
        break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_alu_hw_action_get
 * Purpose:
 *     Get ALU hardware actions.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     action  - (OUT) software Action for element.
 *     hw_action  - (OUT) Action for element.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu_hw_action_get(
                int unit,
                bcm_flowtracker_tracking_param_type_t param,
                bcm_flowtracker_check_action_t action,
                int *hw_action)
{

    if (action >=  bcmFlowtrackerCheckActionCount) {
        return BCM_E_PARAM;
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

            if ((param == bcmFlowtrackerTrackingParamTypeDosAttack) ||
               (param == bcmFlowtrackerTrackingParamTypeInnerDosAttack)) {
                *hw_action = 8;
            } else {
                *hw_action = 6;
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
 *     bcmi_ft_alu_load_values_normalize
 * Purpose:
 *     Get ALU/Load values to get ALU/LOAD types.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     info  - (IN) Group ALU/LOAD data element.
 *     length - (OUT) length of extractor.
 *     alu - (OUT) ALU or LOAD.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu_load_values_normalize(
                 int unit,
                 bcmi_ft_group_alu_info_t *info,
                 int *length,
                 int *alu)
{

    if (!info || !length || !alu) {
        return BCM_E_PARAM;
    }

    if (info->element_type2) {
        /* Cannot support ALU and LOAD mixed elements. */
        if (info->key1.is_alu != info->key2.is_alu) {
            return BCM_E_PARAM;
        }
    }

    *alu = info->key1.is_alu;

    /* Get the bigger length. */
    *length = ((info->key1.length > info->key2.length) ?
                info->key1.length : info->key2.length);

    return BCM_E_NONE;
}
/*
 * Function:
 *     bcmi_ft_alu_load_type_get
 * Purpose:
 *     Get ALU/Load type based on length/params.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     alu  - (IN) Is alu required.
 *     length - (IN) length of extractor.
 *     type - (OUT) type of load/ALU.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu_load_type_get(
                 int unit,
                 int alu,
                 int length,
                 bcmi_ft_alu_load_type_t *type)
{


    int min_extractor_length = 0;
    int local_type = 0;
    int is_alu = 0;

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

    if (alu && ((length >=8) && (length < 16))) {
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
    local_type = is_alu +  ((length > min_extractor_length) ? 1 : 0);


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
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_alu_check_free_index
 * Purpose:
 *     Check ALU/Load type based on group.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     id  - (IN) FT group id.
 *     alu_load_type - (IN) type of load/ALU.
 *     index  - (IN) Free index of that memory.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu_check_free_index(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        int index)
{

    uint32 *in_use_array = NULL;
    int temp_index;
    uint32 mem_idx = 0;
    int size_per_mem = 0;

    /* Get the total indexes for this type of memory. */
    size_per_mem =
        alu_load_index_info[unit][alu_load_type].alu_load_info->mem_size;

    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        in_use_array = BCMI_FT_GROUP_ALU16_MEM_USE(unit, id);
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
    }

    temp_index = index;
    size_per_mem = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, mem_idx);

    /* Check which memory idx, this entry index belongs to. */
    while (temp_index >= size_per_mem) {

        temp_index -= size_per_mem;
        mem_idx++;
        size_per_mem = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, mem_idx);
    }

    if (in_use_array != NULL) {
        /* This ALU memory is used. go for next one. */
        if (in_use_array[mem_idx]) {
            return FALSE;
        }
    }

    return TRUE;
}


/*
 * Function:
 *     bcmi_ft_alu_get_free_index
 * Purpose:
 *     Get ALU/Load type based on length/params.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     id  - (IN) FT group id.
 *     alu_load_type - (IN) type of load/ALU.
 *     index  - (OUT) Free index of that memory.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu_get_free_index(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        int *index)
{

    int i = 0;
    uint32 *in_use_array = NULL;
    uint32 total_mems = 1;
    uint32 mem_idx = 0;
    int size_per_mem = 0;
    int start_index = 0, end_index = 0;

    *index = -1;

    /* Get the total indexes for this type of memory. */
    size_per_mem =
        alu_load_index_info[unit][alu_load_type].alu_load_info->mem_size;

    if (alu_load_type == bcmiFtAluLoadTypeAlu16) {
        in_use_array = BCMI_FT_GROUP_ALU16_MEM_USE(unit, id);
        total_mems   = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16);
    } else if (alu_load_type == bcmiFtAluLoadTypeAlu32) {
        in_use_array = BCMI_FT_GROUP_ALU32_MEM_USE(unit, id);
        total_mems   = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32);
    }

   LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
       (BSL_META_U(unit, "total_mems=%d\n"), total_mems));


    /* start index should be where the free index starts. */
    while (mem_idx < total_mems) {

        size_per_mem = BCMI_FT_ALU_LOAD_MEM_SIZE(unit, alu_load_type, mem_idx);

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit, "mem_idx = %d, size_per_mem=%d\n"),
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
            if (!(SHR_BITGET(BCMI_FT_ALU_LOAD_BITMAP
                (unit, alu_load_type), i))) {

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

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *     bcmi_ft_alu_entry_match
 * Purpose:
 *     Match Load 16 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU/Load information of group.
 *     load16_indexes  - (IN) Load 16 indexes in group
 *     index - (OUT) sw index.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu_entry_match(
            int unit,
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

    if (group_alu_info->element_type1 !=
        alu_info->param) {
        return BCM_E_NOT_FOUND;
    }

    if (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction !=
        (alu_info->direction)) {
        return BCM_E_NOT_FOUND;
    }

    /*
     * For ALU based memories, we need to make sure that
     * other than basic data, update and export data should
     * also match.
     */
    if (bcmi_ft_check_list_compare(unit, &(alu_info->head),
        group_alu_info->flowchecker_id)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_check_action_shift_bits_get
 * Purpose:
 *     Get the shift bits, if required, for writing
 *     threshold value in hardware.
 * Parameters:
 *     unit - (IN) BCM device id
 *     action   - (IN) FT Check Action.
 *     info - (IN) ALU/Load information of group.
 *     shift_bits - (OUT) Number of bits threshold needs
 *            to be shifted before writing to hardware.
 *
 * Returns:
 *     None.
 */
void
bcmi_ft_check_action_shift_bits_get(
                int unit,
                bcm_flowtracker_check_action_t action,
                bcmi_ft_group_alu_info_t *info,
                int *shift_bits)
{
    int shift_required = FALSE;

    *shift_bits = 0;

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
        *shift_bits = info->action_key.location % 16;
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
 *     bcmi_ft_flowchecker_alu_base_control_set
 * Purpose:
 *     Set base control memory for ALUs..
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     alu_info - (IN) ALU information of group.
 *     alu_fmt - (IN) formatiinfo of memory.
 *
 * Returns:
 *     NONE
 */

int
bcmi_ft_flowchecker_alu_base_control_set(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *alu_info,
                uint32 *alu_fmt)
{
    int check_0_opr = -1, check_1_opr = -1;
    int rv = BCM_E_NONE;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    int hw_action = 0;
    bcm_flowtracker_check_action_t alu_action = 0;
    int actual_location = 0, actual_location2 = 0;
    int update_location = 0;
    bcm_flowtracker_tracking_param_type_t element = 0;
    uint32 kmap_val = 0;
    int length = 16, length2 = 16;
    bcm_flowtracker_check_operation_t opr = bcmFlowtrackerCheckOpNone;
    int val1, val2, opr1, opr2;
    int clear_on_export = 0;
    int export_check_mode = 1;

    if (alu_fmt == NULL) {
        return BCM_E_PARAM;
    }

    if (alu_info == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
    val1 = val2 = opr1 = opr2 = -1;

    actual_location = alu_info->key1.location;
    actual_location2 = alu_info->key2.location;

    if ((alu_info->key1.location % 16) != 0) {
        actual_location = ((alu_info->key1.location / 16) * 16);
    }

    if (alu_info->key1.length > 16) {
        /* ALU is at 32 bit boundary. */
        length = 32;
    }

    if (alu_info->element_type2) {
        if ((alu_info->key2.location % 16) != 0) {
            actual_location2 = ((alu_info->key2.location / 16) * 16);
        }

        if (alu_info->key2.length > 16) {
            /* ALU is at 32 bit boundary. */
            length2 = 32;
        }

    } else {
        actual_location2 = actual_location;
        length2 = length;
    }

    /*
     * ftfp is sending location in continuous index of 256B.
     * Each extractor here is 16 bytes so long, so
     * need to divide it by 16 to get correct location.
     */
    actual_location = (actual_location / length);
    actual_location2 = (actual_location2 / length2);

    /* Update the action location. */
    if (alu_info->action_element_type) {
        update_location = alu_info->action_key.location / 16;
        element = alu_info->action_element_type;
    } else {
        update_location = actual_location;
        element = alu_info->element_type1;
    }

    if (alu_info->flowchecker_id > 0) {
        BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
            (unit, alu_info->flowchecker_id, &fc_info));

        rv = bcmi_ft_flowchecker_values_get(unit, &fc_info, &val1, &val2,
            &opr1, &opr2);
        opr = fc_info.check1.operation;

        if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (opr1 != -1) {
            /* We can associate two flowcheckers together so that we can have . */
            soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_0_ATTR_SELECTf, actual_location);

            /* Convert bcm layer opertaion in hw operations. */
            soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
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

        }

        if (opr2 != -1) {

            soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_1_ATTR_SELECTf, actual_location2);

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

        }

        /* Set all the bit selects to check 0 and set KMAP accordingly.*/
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_0f, (opr1 == -1) ? 1 : 0);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_1f, (opr2 == -1) ? 0 : 1);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_2f, (opr1 == -1) ? 1 : 0);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_3f, (opr1 == -1) ? 1 : 0);

        /* Set ALU actions. */
        alu_action = fc_info.action_info.action;

        if (fc_info.action_info.action == bcmFlowtrackerCheckActionUpdateValue) {
            if (alu_info->action_key.length > 16) {
                /* Update cant support more than 16 length.*/
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                    "Action Element len (%d) is more than supported length 16 \n"),
                    alu_info->action_key.length));

                return BCM_E_PARAM;
            }
        }

        clear_on_export = ((fc_info.check1.flags &
           BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_EXPORT) ? 1 : 0);

        export_check_mode = ((fc_info.check1.flags &
            BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD) ? 0 : 1);

    } else {
        if ((alu_info->element_type1 <= bcmFlowtrackerTrackingParamTypeSrcIPv4)
            || (alu_info->element_type1 >=
                    bcmFlowtrackerTrackingParamTypeCount)) {
            return BCM_E_PARAM;
        }
        /* if it is not flowchecker then only one element in alu. */
        rv = bcmi_ft_element_alu_action_get
            (alu_info->element_type1, &alu_action);

        if (BCM_FAILURE(rv)) {
            return rv;
        }
        /* We can associate two flowcheckers together so that we can have . */
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_0_ATTR_SELECTf, actual_location);

        if (alu_info->key1.length <= 16) {
            soc_format_field32_set(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_ATTR_SELECTf, actual_location);
        }

        /* Make Check 0 operation as always pass. */
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_0_OPERATIONf, 0x6);

        /* Make Check 0 operation as always pass. */
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_1_OPERATIONf, 0x6);

        /* Set all the bit selects to check 0 and set KMAP accordingly.*/
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_0f, 0);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_1f, 0);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_2f, 0);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_3f, 0);

    }

    BCM_IF_ERROR_RETURN(bcmi_ft_kmap_val_get(unit, &fc_info, opr, &kmap_val));

    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
        alu_fmt, FLEX_CHECK_LOGIC_KMAP_CONTROLf, kmap_val);

    /* Now get the information about update operations. */
    BCM_IF_ERROR_RETURN(bcmi_ft_alu_hw_action_get(unit, element,
             alu_action, &hw_action));

    /* Update operation with the action element. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
        alu_fmt, UPDATE_ATTR_SELECTf, update_location);

    /* Update operation with the action element. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
        alu_fmt, UPDATE_OPERATIONf, hw_action);


    /* Do equal average for incoming and runnung values for now.
     * Encodings are:
     * 0  => 100%  1  => 96%  2  => 92%  3  => 88%  4  => 86%  5  => 84%
     * 6  => 81%   7  => 78%  8  => 75% 9  => 71%10  => 67% 11  => 63%
     * 12  => 59%  13  => 56%  14  => 53% 15  => 50%  16  => 47%
     * 17  => 44%  18  => 41% 19  => 37% 20  => 33%
     * 21  => 29%  22  => 25% 23  => 22% 24  => 19% 25  => 16% 26  => 14%
     * 27  => 12%  28  => 8% 29  => 4% 30  => 0% 31  => Reserved
     */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
        alu_fmt, UPDATE_OP_PARAMf, 0xf);

    /* Set the load trigger for forward and reverse. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
        alu_fmt, LOAD_TRIGGERf, BCMI_FT_GROUP_FTFP_DATA(unit, id).direction);


    /* Clear the data after export based on user input. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
        alu_fmt, CLEAR_ON_EXPORT_ENABLEf, clear_on_export);

    /* Export data if there is new change in existing session data. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
        alu_fmt, EXPORT_CHECK_MODEf, export_check_mode);

    /* Set export operation. */
    rv = bcmi_ft_alu_hw_operation_get
        (unit, fc_info.export_info.operation, &check_0_opr, &check_1_opr);


    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (check_0_opr != -1) {
        soc_format_field32_set(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, EXPORT_OPERATIONf, check_0_opr);
    } else {
        soc_format_field32_set(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, EXPORT_OPERATIONf, check_1_opr);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_alu32_entry_install
 * Purpose:
 *     Configure ALU32 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU information of group.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu32_entry_install(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *load_indexes)
{


    bcmi_flowtracker_flowchecker_info_t fc_info;
    int rv = BCM_E_NONE;
    int new_index = -1;
    soc_mem_t mem = -1;
    int index = -1;
    int a_idx = -1; /* Array index of field/memory array. */
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeAlu32;
    bsc_dg_group_table_entry_t dg_entry;
    uint32 alu_base_ctrlfmt[3];
    uint32 alu32_specific_ctrlfmt[4];
    uint32 alu32_ctrlfmt[7];
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;
    int shift_bits = 0, shift_bits2 = 0;
    int min_value = 0, max_value = 0, threshold = 0;
    int val1, val2, opr1, opr2;

    sal_memset(alu_base_ctrlfmt, 0, 3 * sizeof(uint32));
    sal_memset(alu32_specific_ctrlfmt, 0, 4*sizeof(uint32));
    sal_memset(alu32_ctrlfmt, 0, 7*sizeof(uint32));
    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    val1 = val2 = opr1 = opr2 = -1;

    /* First insert the hash entry. info carries single alu information */
    rv = bcmi_ft_alu_hash_insert
        (unit, id, bcmiFtAluLoadTypeAlu32, info, NULL, &index);

    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            bcmi_ft_alu_load_mem_index_get
                (unit, index, alu_load_type, &new_index, &mem, &a_idx);

            /* Now that BCM_E_EXISTS is handled. please change rv. */
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    } else {

        /* Index is obtained, convert the index into the memory type. */
        bcmi_ft_alu_load_mem_index_get
            (unit, index, alu_load_type, &new_index, &mem, &a_idx);

        /* Set the software entries. */
        rv = bcmi_ft_flowchecker_alu_base_control_set(unit, id, info,
            alu_base_ctrlfmt);

         BCMI_CLEANUP_IF_ERROR(rv);

        soc_format_field_set(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
            alu32_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

        if (info->flowchecker_id > 0) {
            /*
             * The update for an element can still be done
             * without performing any check.
             * For example a template can just tell which
             * element need to be updated.
             * Therefore other than check elements, all other
             * flow checker elements should be updated even
             * without a flowchecker id.
             */
            rv = bcmi_ft_flowchecker_get
                (unit, info->flowchecker_id, &fc_info);
            BCMI_CLEANUP_IF_ERROR(rv);


            if ((info->key1.location % 16) != 0) {
                shift_bits = (info->key1.location % 16);
            }

            if (info->element_type2) {
                if ((info->key2.location % 16) != 0) {
                    shift_bits2 = (info->key1.location % 16);
                }
            } else {
                shift_bits2 = shift_bits;
            }

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
                        alu32_specific_ctrlfmt, CHECK_0_ATTR_SELECT_GRANf, 1);
                }

            }

            if (opr2 != -1) {
                max_value = (val2 << shift_bits2);

                soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                    alu32_specific_ctrlfmt, CHECK_1_THRESHOLDf,
                    max_value);

                if (info->key1.length > 16) {
                    soc_format_field32_set(unit,
                        BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                        alu32_specific_ctrlfmt, CHECK_1_ATTR_SELECT_GRANf, 1);
                }

            }

            if (fc_info.export_info.export_check_threshold > 0) {

                shift_bits2 = 0;
                bcmi_ft_check_action_shift_bits_get(unit,
                        fc_info.action_info.action, info, &shift_bits2);

                threshold = fc_info.export_info.export_check_threshold << shift_bits2;

                soc_format_field32_set(unit,
                    BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                    alu32_specific_ctrlfmt, EXPORT_THRESHOLDf,
                    threshold);
            }
        }
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
        index, alu_type_str[bcmiFtAluLoadTypeAlu32]));

    if (BCM_SUCCESS(rv) && (index != -1)) {
        /* Assign the bitmap to this index. */
        SHR_BITSET((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeAlu32)),
            index);

        /* Also increment the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeAlu32, index);

        /* Set the groups IN use ALU memories. */
        (BCMI_FT_GROUP_ALU32_MEM_USE(unit, id))[a_idx] = 1;

        /* Update chip debug memory also if aplicable. */
        bcmi_ft_chip_debug_use_mem_update(unit,
            bcmiFtAluLoadTypeAlu32, a_idx, 1);

        /* Set pdd profile entry for this alu entry. */
        soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            (BCMI_FT_PDD_INFO(unit, bcmiFtAluLoadTypeAlu32)[a_idx]).param_id, 1);


        rv  = soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry);
        BCMI_CLEANUP1_IF_ERROR(rv);


        soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_entry,
            bcmi_dg_group_alu32_fid[a_idx], new_index);

        rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ALL,
            id, &dg_entry);
    }

    if (BCM_SUCCESS(rv)) {
        return rv;
    }

cleanup1:
    if (index != -1) {
        /* Also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeAlu32, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
            (unit, bcmiFtAluLoadTypeAlu32, index))) {

            /* Clear the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeAlu32)),
                index);
        }
    }

cleanup:
    /* remove the previously added hash table entry. */
    bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeAlu32,
        info, NULL, &index);

    return rv;
}

/*
 * Function:
 *     bcmi_ft_alu16_entry_install
 * Purpose:
 *     Configure ALU16 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU information of group.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu16_entry_install(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *load16_indexes)
{

    bcmi_flowtracker_flowchecker_info_t fc_info;
    int new_index = -1;
    soc_mem_t mem = -1;
    int index = -1;
    int rv = BCM_E_NONE;
    int a_idx = 0;
    int fid_len = 0, fid_value = 0;
    uint32 alu_base_ctrlfmt[3];
    uint32 alu16_specific_ctrlfmt[3];
    uint32 alu16_ctrlfmt[6];
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;
    bsc_dg_group_table_entry_t dg_entry;
    int min_value = 0, max_value = 0, threshold = 0;
    int shift_bits = 0, shift_bits2 = 0;
    int val1, val2, opr1, opr2;

    sal_memset(alu_base_ctrlfmt, 0, 3 * sizeof(uint32));
    sal_memset(alu16_specific_ctrlfmt, 0, 3*sizeof(uint32));
    sal_memset(alu16_ctrlfmt, 0, 6*sizeof(uint32));
    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));


    val1 = val2 = opr1 = opr2 = -1;
    /* First insert the hash entry. */
    rv = bcmi_ft_alu_hash_insert(unit, id, bcmiFtAluLoadTypeAlu16, info, NULL,
        &index);

    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            bcmi_ft_alu_load_mem_index_get
                (unit, index, bcmiFtAluLoadTypeAlu16, &new_index, &mem, &a_idx);

            /* Now that BCM_E_EXISTS is handled. please change rv. */
            rv = BCM_E_NONE;

        } else {
            return rv;
        }
    } else {

        /* Index is obtained, convert the index into the memory type. */
        bcmi_ft_alu_load_mem_index_get
            (unit, index, bcmiFtAluLoadTypeAlu16, &new_index, &mem, &a_idx);

        rv = bcmi_ft_flowchecker_alu_base_control_set(unit, id, info,
                alu_base_ctrlfmt);

        BCMI_CLEANUP_IF_ERROR(rv);

        soc_format_field_set(unit, BSC_TL_DG_TO_DT_ALU16_CONTROL_BUSfmt,
            alu16_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);

        if (info->flowchecker_id > 0) {
            /*
             * The update for an element can still be done
             * without performing any check.
             * For example a template can just tell which
             * element need to be updated.
             * Therefore other than check elements, all other
             * flow checker elements should be updated even
             * without a flowchecker id.
             */
            BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
                (unit, info->flowchecker_id, &fc_info));

            if ((info->key1.location % 16) != 0) {
                shift_bits = (info->key1.location % 16);
            }

            if (info->element_type2) {
                if ((info->key2.location % 16) != 0) {
                    shift_bits2 = (info->key1.location % 16);
                }
            } else {
                shift_bits2 = shift_bits;
            }

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
                    CHECK_0_THRESHOLDf);

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


            if (fc_info.export_info.export_check_threshold > 0) {

                shift_bits2 = 0;
                bcmi_ft_check_action_shift_bits_get(unit,
                        fc_info.action_info.action, info, &shift_bits2);

                threshold = fc_info.export_info.export_check_threshold << shift_bits2;

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

        /* The profile is set and now we need to write into the index of group. */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, new_index, &alu16_entry);
        BCMI_CLEANUP_IF_ERROR(rv);

    }

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "ALU allocated : index = %d, Type = %s\n"),
        index, alu_type_str[bcmiFtAluLoadTypeAlu16]));

    if ((index != -1)) {
        /* Assign the bitmap to this index. */
        SHR_BITSET((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeAlu16)),
            index);

        /* Also increment the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeAlu16, index);

        /* Set the groups IN use ALU memories. */
        (BCMI_FT_GROUP_ALU16_MEM_USE(unit, id))[a_idx] = 1;

        /* Update chip debug memory also if aplicable. */
        bcmi_ft_chip_debug_use_mem_update(unit,
            bcmiFtAluLoadTypeAlu16, a_idx, 1);

        /* Set pdd profile entry for this alu entry. */
        soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            (BCMI_FT_PDD_INFO(unit, bcmiFtAluLoadTypeAlu16)[a_idx]).param_id, 1);

        rv = soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry);

         BCMI_CLEANUP1_IF_ERROR(rv);

		soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_entry,
            bcmi_dg_group_alu16_fid[a_idx], new_index);

        rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ALL,
            id, &dg_entry);
    }

    if (BCM_SUCCESS(rv)) {
        return rv;
    }

cleanup1:
    if (index != -1) {
        /* Also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeAlu16, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
            (unit, bcmiFtAluLoadTypeAlu16, index))) {

            /* Clear the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeAlu16)),
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
 *     bcmi_ft_alu32_entry_uninstall
 * Purpose:
 *     clear ALU32 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU information of group.
 *     type - (IN) ALU/LOAD type.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu32_entry_uninstall(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes)
{

    int rv = BCM_E_NONE;
    int new_index = -1;
    int index = -1;
    soc_mem_t mem = -1;
    int bit = 0;
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;

    if (!info->alu_load_index && !info->alu_load_type) {
        /* This entry is yet to be installed, so nothing to uninstall. */
        return BCM_E_NONE;
    }

    index = info->alu_load_index;

    /* Decrement the ref count on this index. */
    BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeAlu32, index);

    /* First conver the index into the memory type. */
    bcmi_ft_alu_load_mem_index_get(unit, index, bcmiFtAluLoadTypeAlu32,
        &new_index, &mem, &bit);

    if (!(BCMI_FT_ALU_LOAD_REFCOUNT(unit, bcmiFtAluLoadTypeAlu32, index))) {

        sal_memset(&alu32_entry, 0, sizeof(bsc_dg_group_alu32_profile_0_entry_t));

        /* The profile is set and now we need to write into the index of group. */
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, new_index, &alu32_entry));


        SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeAlu32)),
            index);

        /* We already found the entry above so this should always pass. */
        BCM_IF_ERROR_RETURN(
            bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeAlu32,
            info, NULL, &index));
    }

    /* Remove this memory from group. */
    soc_mem_field32_modify(unit, BSC_DG_GROUP_TABLEm, id,
        bcmi_dg_group_alu32_fid[bit], 0);

    /* Set the groups IN use ALU memories. */
    (BCMI_FT_GROUP_ALU32_MEM_USE(unit, id))[bit] = 0;

    /* Update chip debug memory also if aplicable. */
    bcmi_ft_chip_debug_use_mem_update(unit,
        bcmiFtAluLoadTypeAlu32, bit, -1);

    return rv;
}

/*
 * Function:
 *     bcmi_ft_alu16_entry_uninstall
 * Purpose:
 *     clear ALU16 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU information of group.
 *     type - (IN) ALU/LOAD type.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu16_entry_uninstall(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes)
{
    int rv = BCM_E_NONE;
    int new_index = -1;
    int index = -1;
    soc_mem_t mem = -1;
    int bit = 0;
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;

    /* Here we just need to add into the sw list. */
    /* start writing the information into hardware now. */
    if (!info->alu_load_index && !info->alu_load_type) {
        /* This entry is yet to be installed, so nothing to uninstall. */
        return BCM_E_NONE;
    }

    index = info->alu_load_index;

    /* Decrement the ref count on this index. */
    BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeAlu16, index);

    /* First conver the index into the memory type. */
    bcmi_ft_alu_load_mem_index_get(unit, index, bcmiFtAluLoadTypeAlu16,
        &new_index, &mem, &bit);

    if (!(BCMI_FT_ALU_LOAD_REFCOUNT(unit, bcmiFtAluLoadTypeAlu16, index))) {

        sal_memset(&alu16_entry, 0, sizeof(bsc_dg_group_alu16_profile_0_entry_t));

        /* The profile is set and now we need to write into the index of group. */
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, new_index, &alu16_entry));


        SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeAlu16)),
            index);

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

    return rv;
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
 *     bcmi_ft_load16_entry_match
 * Purpose:
 *     Match Load 16 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU/Load information of group.
 *     load16_indexes  - (IN) Load 16 indexes in group
 *     index - (OUT) sw index.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_load16_entry_match(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            bcmi_ft_alu_hash_info_t *alu_info,
            int *load16_indexes, int *index)
{

    bcmi_ft_group_alu_info_t *local = NULL;
    uint32 load16_ctrlfmt[1];
    bsc_dg_group_load16_profile_entry_t load16_entry;
    uint32 location = 0, trigger = 0;
    int i=0, j=0;
    int max = 0;
    int match_found = 0;

    int data[TOTAL_GROUP_LOAD16_DATA_NUM] =
                   {DATA_15f,
                    DATA_14f,
                    DATA_13f,
                    DATA_12f,
                    DATA_11f,
                    DATA_10f,
                    DATA_9f,
                    DATA_8f,
                    DATA_7f,
                    DATA_6f,
                    DATA_5f,
                    DATA_4f,
                    DATA_3f,
                    DATA_2f,
                    DATA_1f,
                    DATA_0f};

    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));
    sal_memset(load16_ctrlfmt, 0, sizeof(uint32));

    /* Set the max for whole table if whole table is to be checked. */
    max = soc_mem_index_max(unit, BSC_DG_GROUP_LOAD16_PROFILEm);

    /* If an index is provided then match only that index. */
    if (*index != -1) {
        max = *index;
        i = *index;
    }

    for (; i<=max; i++) {
        /* Check for the bitmap first if the entry is in use.*/

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
            MEM_BLOCK_ANY, i, &load16_entry));

        for (; j<TOTAL_GROUP_LOAD16_DATA_NUM; j++) {

            soc_mem_field_get(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
                (uint32 *) &load16_entry, data[j], load16_ctrlfmt);

            trigger = soc_format_field32_get(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                load16_ctrlfmt, LOAD_TRIGGERf);

            if (load16_indexes[j] != -1) {
                if (!trigger) {
                    /*
                     * If trigger is not set but still
                     * there is some info at that index
                     * then this will not match this load entry.
                     */
                    match_found = 0;
                    break;
                } else if (trigger !=
                     BCMI_FT_GROUP_FTFP_DATA(unit, id).direction) {
                    /*
                     * If trigger is set but not matching
                     * to the group direction then also no match.
                     */
                    match_found = 0;
                    break;
                }
            } else {
                if (trigger) {
                    /*
                     * If trigger is set but still
                     * there is no info at that index
                     * then this will not match this load entry.
                     */
                    match_found = 0;
                }
                /* if trigger is also not set, then this could be match
                 * based on the previous match result. just breack from here.
                 */
                break;
            }

            local = (&(info[load16_indexes[j]]));
            location = soc_format_field32_get(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
                load16_ctrlfmt, ATTR_SELECTf);

            if (location != (local->key1.location/16)) {
                /*
                 * If entry does not match then return.
                 * by setting that this is not a match.
                 */
                match_found = 0;
                break;
            }
            /* If we have reached here then that is a match for now. */
            match_found = 1;
        }
        if (match_found) {
            break;
        }
    }

    if (match_found) {
        *index = i;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcmi_ft_load16_entry_write
 * Purpose:
 *     Write Load 16 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU/Load information of group.
 *     load16_indexes  - (IN) Load 16 indexes in group
 *     index - (IN) sw index.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_load16_entry_write(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *load16_indexes, int index)
{

    bcmi_ft_group_alu_info_t *local = NULL;
    uint32 load16_ctrlfmt[1];
    bsc_dg_group_load16_profile_entry_t load16_entry;
    uint32 location = 0;
    int j=0;
    int rv = BCM_E_NONE;

    int data[TOTAL_GROUP_LOAD16_DATA_NUM] =
                   {DATA_15f,
                    DATA_14f,
                    DATA_13f,
                    DATA_12f,
                    DATA_11f,
                    DATA_10f,
                    DATA_9f,
                    DATA_8f,
                    DATA_7f,
                    DATA_6f,
                    DATA_5f,
                    DATA_4f,
                    DATA_3f,
                    DATA_2f,
                    DATA_1f,
                    DATA_0f};

    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));
    sal_memset(load16_ctrlfmt, 0, sizeof(uint32));

    for (; j<TOTAL_GROUP_LOAD16_DATA_NUM; j++) {
        /* Now based on the incoming info, match the entries. */
        if (load16_indexes[j] == -1) {
            break;
        }

        local = (&(info[load16_indexes[j]]));

        /* Update the load trigger based on groups direction configuration. */
        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
            load16_ctrlfmt, LOAD_TRIGGERf,
             BCMI_FT_GROUP_FTFP_DATA(unit, id).direction);

        location = local->key1.location;

        /*
         * ftfp is sending location in continuous index of 256B.
         * Each extractor here is 16 bytes so long, so
         * need to divide it by 16 to get correct location.
         */
        location = (location / 16);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
            load16_ctrlfmt, ATTR_SELECTf, location);

        /* Write into that entry. */
        soc_mem_field_set(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
            (uint32 *) &load16_entry,
            data[j], load16_ctrlfmt);
    }

    rv = soc_mem_write(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
        MEM_BLOCK_ANY, index, &load16_entry);

    return rv;
}

/*
 * Function:
 *     bcmi_ft_load16_entry_install
 * Purpose:
 *     Install Load 16 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     indexes  - (IN) Load 16 indexes in group
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_load16_entry_install(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *indexes)
{
    bcmi_ft_group_alu_info_t *temp = NULL, *local = NULL;
    bsc_dg_group_table_entry_t dg_entry;
    int rv = BCM_E_NONE;
    int j, index = -1;

    if (indexes[0] == -1) {
        /* There is no load 16 entry. */
        return BCM_E_NONE;
    }

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    rv = bcmi_ft_alu_hash_insert(unit, id, bcmiFtAluLoadTypeLoad16,
        temp, indexes, &index);

    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            /* Increment ref count and set up group table. */
            BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeLoad16,
                index);
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    } else {
        /* Now that we have got the index, now write into that index. */
        rv  = bcmi_ft_load16_entry_write(unit, id, temp, indexes, index);

        BCMI_CLEANUP_IF_ERROR(rv);

        if (index != -1) {
            /* Assign the bitmap to this index. */
            SHR_BITSET((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeLoad16)),
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
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm,
         ((uint32 *)&dg_entry), GROUP_LOAD16_PROFILE_IDXf, index);

    rv  = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id,
         &dg_entry);
    BCMI_CLEANUP1_IF_ERROR(rv);

    /* Update type and index into all the data elements. */
    for (j=0; j<TOTAL_GROUP_LOAD16_DATA_NUM; j++) {
        /* Now based on the incoming info, match the entries. */
        if (indexes[j] == -1) {
            break;
        }

        local = (&(temp[indexes[j]]));
        local->alu_load_index = index;
        local->alu_load_type = bcmiFtAluLoadTypeLoad16;
    }

    return BCM_E_NONE;

cleanup1:
    if (index != -1) {
        /* Also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeLoad16, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
            (unit, bcmiFtAluLoadTypeLoad16, index))) {

            /* Clear the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeLoad16)),
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
 *     bcmi_ft_load16_entry_clear
 * Purpose:
 *     Clear Load 16 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     index  - (IN) Load 16 index
 *
 * Returns:
 *     BCM_E_XXX
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
 *     bcmi_ft_load16_entry_uninstall
 * Purpose:
 *     Clear Load 16 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     load16_indexes  - (IN) Load 16 indexes in group
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_load16_entry_uninstall(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *indexes)
{
    int index = 0;
    int rv = BCM_E_NONE;
    bsc_dg_group_table_entry_t dg_entry;

    if (indexes[0] == -1) {
        /* There is no load 16 entry. */
        return BCM_E_NONE;
    }

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* First remove the entry from the hash table. */
    rv =  bcmi_ft_alu_hash_mem_index_get(unit, id, bcmiFtAluLoadTypeLoad16,
         BCMI_FT_GROUP_EXT_DATA_INFO(unit, id), indexes, &index);

    if (BCM_FAILURE(rv)) {
        /* first cleanup if something is configured. */
        if (rv == BCM_E_NOT_FOUND) {
            /*
             * If this happens then it might be due to the error while adding.
             * No need to do anything. just return back saying that it is clean.
             */
            return BCM_E_NONE;
        } else {
            return rv;
        }
    }

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_entry,
         GROUP_LOAD16_PROFILE_IDXf, 0);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    if (BCM_SUCCESS(rv)) {

        /* Also increment the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeLoad16, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
                (unit, bcmiFtAluLoadTypeLoad16, index))) {

            /* Assign the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP
                (unit, bcmiFtAluLoadTypeLoad16)) , index);

            /* We already found the entry above so this should always pass. */
            rv =  bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeLoad16,
                BCMI_FT_GROUP_EXT_DATA_INFO(unit, id), indexes, &index);

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Clear the entry in the profile table. */
            rv = bcmi_ft_load16_entry_clear(unit, index);
        }
    }

    return rv;
}





/******************************************************
 |                  LOAD8 Space                       |
 */

/*
 * Function:
 *     bcmi_ft_load8_entry_match
 * Purpose:
 *     Match load 8 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU/Load information of group.
 *     load8_indexes  - (IN) Load 8 indexes in group
 *     index - (OUT) sw index.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_load8_entry_match(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            bcmi_ft_alu_hash_info_t *alu_info,
            int *load8_indexes,
            int *index)
{

    bcmi_ft_group_alu_info_t *local = NULL;
    uint32 load8_ctrlfmt[1];
    bsc_dg_group_load8_profile_entry_t load8_entry;
    uint32 location = 0, trigger = 0;
    int i=0, j=0;
    int max = 0;
    int match_found = 0;

    int data[TOTAL_GROUP_LOAD8_DATA_NUM] =
                   {DATA_7f,
                    DATA_6f,
                    DATA_5f,
                    DATA_4f,
                    DATA_3f,
                    DATA_2f,
                    DATA_1f,
                    DATA_0f};

    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));
    sal_memset(load8_ctrlfmt, 0, sizeof(uint32));

    max = soc_mem_index_max(unit, BSC_DG_GROUP_LOAD8_PROFILEm);

    /* if the index is provided then match for only that index. */
    if (*index != -1) {
        max = *index;
        i = *index;
    }

    for (; i<=max; i++) {
        /* Check for the bitmap first if the entry is in use.*/

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
            MEM_BLOCK_ANY, i, &load8_entry));

        for (; j<TOTAL_GROUP_LOAD8_DATA_NUM; j++) {

            soc_mem_field_get(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
                (uint32 *) &load8_entry, data[j], load8_ctrlfmt);

            trigger = soc_format_field32_get(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                load8_ctrlfmt, LOAD_TRIGGERf);

            if (load8_indexes[j] != -1) {
                if (!trigger) {
                    /*
                     * If trigger is not set but still
                     * there is some info at that index
                     * then this will not match this load entry.
                     */
                    match_found = 0;
                    break;
                } else if (trigger !=
                     BCMI_FT_GROUP_FTFP_DATA(unit, id).direction) {
                    /*
                     * If trigger is set but not matching
                     * to the group direction then also no match.
                     */
                    match_found = 0;
                    break;
                }
            } else {
                if (trigger) {
                    /*
                     * If trigger is set but still
                     * there is no info at that index
                     * then this will not match this load entry.
                     */
                    match_found = 0;
                }
                /* if trigger is also not set, then this could be match
                 * based on the previous match result. just breack from here.
                 */
                break;
            }

            local = (&(info[load8_indexes[j]]));
            location = soc_format_field32_get(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
                load8_ctrlfmt, ATTR_SELECTf);

            if (location != (local->key1.location / 8)) {
                /*
                 * If entry does not match then return.
                 * by setting that this is not a match.
                 */
                match_found = 0;
                break;
            }
            match_found = 1;
        }
        if (match_found) {
            break;
        }
    }

    if (match_found) {
        *index = i;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcmi_ft_load8_entry_write
 * Purpose:
 *     Write Load 8 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info - (IN) ALU/Load information of group.
 *     load8_indexes  - (IN) Load 8 indexes in group
 *     index - (IN) sw index.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_load8_entry_write(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *load8_indexes,
            int index)
{

    bcmi_ft_group_alu_info_t *local = NULL;
    uint32 load8_ctrlfmt[1];
    bsc_dg_group_load8_profile_entry_t load8_entry;
    uint32 location = 0;
    int j=0;

    int data[TOTAL_GROUP_LOAD8_DATA_NUM] =
                   {DATA_7f,
                    DATA_6f,
                    DATA_5f,
                    DATA_4f,
                    DATA_3f,
                    DATA_2f,
                    DATA_1f,
                    DATA_0f};

    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));
    sal_memset(load8_ctrlfmt, 0, sizeof(uint32));

    for (; j<TOTAL_GROUP_LOAD8_DATA_NUM; j++) {
        /* Now based on the incoming info, match the entries. */
        if (load8_indexes[j] == -1) {
            break;
        }

        local = (&(info[load8_indexes[j]]));

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
            load8_ctrlfmt, LOAD_TRIGGERf,
            BCMI_FT_GROUP_FTFP_DATA(unit, id).direction);

        location = local->key1.location;

        if (location % 8) {
            /* if location is not at 8B boundary then return error. */
            return BCM_E_PARAM;
        }

        /*
         * ftfp is sending location in continuous index of 256B.
         * Each extractor here is 8 bytes so long, so
         * need to divide it by 32 to get correct location.
         */
        location = (location / 8);

        soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
            load8_ctrlfmt, ATTR_SELECTf, location);

        /* Write into that entry. */
        soc_mem_field_set(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
            (uint32 *) &load8_entry, data[j], load8_ctrlfmt);
    }

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
        MEM_BLOCK_ANY, index, &load8_entry));

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_ft_load8_entry_install
 * Purpose:
 *     Install Load 8 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     indexes  - (IN) Load 8 indexes in group
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_load8_entry_install(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *indexes)
{
    bcmi_ft_group_alu_info_t *temp = NULL, *local = NULL;
    bsc_dg_group_table_entry_t dg_entry;
    int rv = BCM_E_NONE;
    int j, index = -1;

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    if (indexes[0] == -1) {
        /* There is no load8 entry. */
        return BCM_E_NONE;
    }

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    rv = bcmi_ft_alu_hash_insert(unit, id, bcmiFtAluLoadTypeLoad8,
        temp, indexes, &index);

    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            /* Increment ref count and set up group table. */
            BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeLoad8, index);
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    } else {
        /* Now that we have got the index, now write into that index. */
        rv  = bcmi_ft_load8_entry_write(unit, id, temp, indexes, index);
        BCMI_CLEANUP_IF_ERROR(rv);

        if (index != -1) {
            /* Assign the bitmap to this index. */
            SHR_BITSET((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeLoad8)),
                index);

            /* Also increment the ref count on this index. */
            BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, bcmiFtAluLoadTypeLoad8, index);
        }
    }

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "ALU allocated : index = %d, Type = %s\n"),
        index, alu_type_str[bcmiFtAluLoadTypeLoad8]));

    rv = bcmi_ft_load_pdd_entry_set(unit, id, bcmiFtAluLoadTypeLoad8, index, 0);
    BCMI_CLEANUP_IF_ERROR(rv);

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
    for (j = 0; j<TOTAL_GROUP_LOAD8_DATA_NUM; j++) {
        /* Now based on the incoming info, match the entries. */
        if (indexes[j] == -1) {
            break;
        }

        local = (&(temp[indexes[j]]));
        local->alu_load_index = index;
        local->alu_load_type = bcmiFtAluLoadTypeLoad8;
    }

    return BCM_E_NONE;

cleanup1:
    if (index != -1) {
        /* Also decrement the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeLoad8, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
            (unit, bcmiFtAluLoadTypeLoad8, index))) {

            /* Clear the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP(unit, bcmiFtAluLoadTypeLoad8)),
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
 *     bcmi_ft_load8_entry_clear
 * Purpose:
 *     Clear Load 8 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     index  - (IN) Load 8 index
 *
 * Returns:
 *     BCM_E_XXX
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
 *     bcmi_ft_load8_entry_uninstall
 * Purpose:
 *     uninstall Load 8 entry in hardware
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     indexes  - (IN) Load 8 indexes in group
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_load8_entry_uninstall(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *indexes)
{
    int index = 0;
    int rv = BCM_E_NONE;
    bsc_dg_group_table_entry_t dg_entry;

    if (indexes[0] == -1) {
        /* There is no load 16 entry. */
        return BCM_E_NONE;
    }

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* First remove the entry from the hash table. */
    rv = bcmi_ft_alu_hash_mem_index_get(unit, id, bcmiFtAluLoadTypeLoad8,
         BCMI_FT_GROUP_EXT_DATA_INFO(unit, id), indexes, &index);

    if (BCM_FAILURE(rv)) {
        /* first cleanup if something is configured. */
        if (rv == BCM_E_NOT_FOUND) {
            /*
             * If this happens then it might be due to the error while adding.
             * No need to do anything. just return back saying that it is clean.
             */
            return BCM_E_NONE;
        } else {
            return rv;
        }
    }

    /* Then clear the group table index. */
    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_entry,
            GROUP_LOAD8_PROFILE_IDXf, 0);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    if (BCM_SUCCESS(rv)) {

        /* Also increment the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_DEC(unit, bcmiFtAluLoadTypeLoad8, index);

        if (!(BCMI_FT_ALU_LOAD_REFCOUNT
            (unit, bcmiFtAluLoadTypeLoad8, index))) {

            /* Assign the bitmap to this index. */
            SHR_BITCLR((BCMI_FT_ALU_LOAD_BITMAP
                (unit, bcmiFtAluLoadTypeLoad8)) , index);
           /* We already found the entry above so this should always pass. */
            rv =  bcmi_ft_alu_hash_remove(unit, id, bcmiFtAluLoadTypeLoad8,
                 BCMI_FT_GROUP_EXT_DATA_INFO(unit, id), indexes, &index);

            if (BCM_FAILURE(rv)) {
               return rv;
            }

            /* Clear the entry in the profile table. */
            rv = bcmi_ft_load8_entry_clear(unit, index);
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmi_ft_group_alu_load_index_match
 * Purpose:
 *     Match ALU index for this group.
 * Description:
 *     Even if we may get exact same ALU
 *     which is matching to some existing entry
 *     in hash table, we may not use it if
 *     THat particular ALU memory is used by this
 *     group. So in that case,
 *     we may have to get another index and then
 *     while removing, we may have to make sure this
 *     group has correct index.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     alu_load_type - (IN) ALU/LOAD type.
 *     alu_info - (IN) ALU hash information of group.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_alu_load_index_match(
            int unit,
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
    }


    /* Get the index from alu hash info. */
    index = alu_info->alu_load_index;

    if ((alu_load_type != bcmiFtAluLoadTypeAlu32) &&
        (alu_load_type != bcmiFtAluLoadTypeAlu16)) {
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
 *     bcmi_ft_group_ft_mode_get
 * Purpose:
 *     Set single/Double mode after checking session data.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_ft_mode_update(
    int unit,
    bcm_flowtracker_group_t id)
{

    int i=0;
    int num_alus_loads = 0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    int length, total_length;
    bcmi_ft_group_key_data_mode_t mode = bcmiFtGroupModeSingle;

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
             * then this might be a case where extractor is not aligned properly.
             * Hence we will change length accordingly to pick correct alu type.
             */
            length = 16;
        }
        total_length += length;
        temp++;
    }

    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_FLOW_METERf)) {
        total_length += 48;
    }
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_FLOW_COUNT_TO_CPUf)) {
        total_length += 16;
    }
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_FLOW_STATEf)) {
        total_length += 8;
    }

    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_0f)) {
        total_length += 48;
    }
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_1f)) {
        total_length += 48;
    }
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_2f)) {
        total_length += 48;
    }
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
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


/* We need to change the load triggers whenever we get the indication from user. */
int
bcmi_ft_group_alu_load_trigger_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_direction_t direction)
{
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    int total_mems = 0;
    int mem_idx = 0, type=0;
    bsc_dg_group_table_entry_t dg_entry;
    int index = -1;

    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &dg_entry));

    /* For all the types of memory types. */
    for (type=0; type<bcmiFtAluLoadTypeNone; type++) {

        total_mems = alu_load_index_info[unit][type].total_mem_count;

        /* For all the alu/load types, check what memories are set in pdd. */
        for (mem_idx=0; mem_idx<total_mems; mem_idx++) {
            /* Now get if this particular field is set in pdd. */
            pdd_info = alu_load_index_info[unit][type].pdd_bus_info;

            if (!pdd_info) {
                return BCM_E_INTERNAL;
            }
            if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
                    (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                    pdd_info[mem_idx].param_id)) {

                index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
                        &dg_entry,
                        (BCMI_FT_GROUP_FID_INFO(unit, type))[mem_idx]);

                if (type == bcmiFtAluLoadTypeAlu32) {
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
bcmi_ft_group_load16_trigger_set(
    int unit,
    int mem_idx,
    int index,
    bcm_flowtracker_direction_t direction)
{
    uint32 load16_ctrlfmt[1];
    bsc_dg_group_load16_profile_entry_t load16_entry;
    int rv = BCM_E_NONE;

    int data[TOTAL_GROUP_LOAD16_DATA_NUM] =
                   {DATA_15f,
                    DATA_14f,
                    DATA_13f,
                    DATA_12f,
                    DATA_11f,
                    DATA_10f,
                    DATA_9f,
                    DATA_8f,
                    DATA_7f,
                    DATA_6f,
                    DATA_5f,
                    DATA_4f,
                    DATA_3f,
                    DATA_2f,
                    DATA_1f,
                    DATA_0f};
    sal_memset(&load16_entry, 0, sizeof(bsc_dg_group_load16_profile_entry_t));
    sal_memset(load16_ctrlfmt, 0, sizeof(uint32));

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
        MEM_BLOCK_ANY, index, &load16_entry));

    soc_mem_field_get(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
        (uint32 *) &load16_entry, data[mem_idx], load16_ctrlfmt);

    /* Update the load trigger based on groups direction configuration. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD16_CONTROL_BUSfmt,
        load16_ctrlfmt, LOAD_TRIGGERf, direction);

    rv = soc_mem_write(unit, BSC_DG_GROUP_LOAD16_PROFILEm,
        MEM_BLOCK_ANY, index, &load16_entry);

    return rv;
}



int
bcmi_ft_group_load8_trigger_set(
    int unit,
    int mem_idx,
    int index,
    bcm_flowtracker_direction_t direction)
{
    uint32 load8_ctrlfmt[1];
    bsc_dg_group_load8_profile_entry_t load8_entry;
    int rv = BCM_E_NONE;
    int data[TOTAL_GROUP_LOAD8_DATA_NUM] =
                   {DATA_7f,
                    DATA_6f,
                    DATA_5f,
                    DATA_4f,
                    DATA_3f,
                    DATA_2f,
                    DATA_1f,
                    DATA_0f};

    sal_memset(&load8_entry, 0, sizeof(bsc_dg_group_load8_profile_entry_t));
    sal_memset(load8_ctrlfmt, 0, sizeof(uint32));

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
        MEM_BLOCK_ANY, index, &load8_entry));

    soc_mem_field_get(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
        (uint32 *) &load8_entry, data[mem_idx], load8_ctrlfmt);

    /* Update the load trigger based on groups direction configuration. */
    soc_format_field32_set(unit, BSC_TL_DG_TO_DT_LOAD8_CONTROL_BUSfmt,
        load8_ctrlfmt, LOAD_TRIGGERf, direction);

    rv = soc_mem_write(unit, BSC_DG_GROUP_LOAD8_PROFILEm,
        MEM_BLOCK_ANY, index, &load8_entry);

    return rv;
}


/* For load triggers on ALU memories. */
int
bcmi_ft_group_alu32_trigger_set(
    int unit,
    int mem_idx,
    int index,
    bcm_flowtracker_direction_t direction)
{

    int rv = BCM_E_NONE;
    uint32 alu_base_ctrlfmt[3];
    uint32 alu32_specific_ctrlfmt[4];
    uint32 alu32_ctrlfmt[7];
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;
    soc_mem_t mem;

    sal_memset(alu_base_ctrlfmt, 0, 3 * sizeof(uint32));
    sal_memset(alu32_specific_ctrlfmt, 0, 4*sizeof(uint32));
    sal_memset(alu32_ctrlfmt, 0, 7*sizeof(uint32));


    mem = alu_32_info[mem_idx].mem;

    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &alu32_entry);

    soc_mem_field_get(unit, mem, (uint32 *)&alu32_entry, DATAf,
            alu32_ctrlfmt);

    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
        alu32_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_base_ctrlfmt);
    /* Set the load trigger for forward and reverse. */
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
bcmi_ft_group_alu16_trigger_set(
    int unit,
    int mem_idx,
    int index,
    bcm_flowtracker_direction_t direction)
{

    int rv = BCM_E_NONE;
    uint32 alu_base_ctrlfmt[3];
    uint32 alu16_specific_ctrlfmt[4];
    uint32 alu16_ctrlfmt[6];
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;
    soc_mem_t mem;
    sal_memset(alu_base_ctrlfmt, 0, 3 * sizeof(uint32));
    sal_memset(alu16_specific_ctrlfmt, 0, 4*sizeof(uint32));
    sal_memset(alu16_ctrlfmt, 0, 6*sizeof(uint32));


    mem = alu_16_info[mem_idx].mem;

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
bcmi_ft_tracking_param_in_Debug_mode(
                int unit,
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

    for (i=0; i<BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info; i++) {
        if (group_alu_info->element_type1 ==
            BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param) {
            return 1;
        }
    }


    return 0;
}


int
bcmi_ft_chip_debug_counter_init(
        int unit,
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
bcmi_ft_chip_debug_counter_get(
        int unit,
        int mem_idx,
        bcmi_ft_alu_load_type_t alu_load_type,
        uint32 *count)
{

    uint32 buf[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;

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


    /* Lock the memory. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
        0, buf));

    *count = soc_mem_field32_get(unit, mem, buf, COUNTf);


    return BCM_E_NONE;
}


void
bcmi_ft_chip_debug_use_mem_update(
        int unit,
        bcmi_ft_alu_load_type_t alu_load_type,
        int mem_idx,
        int val)
{

    int *in_use_array = NULL;
    int *ref_count = NULL;
    int i = 0;

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
bcmi_ft_chip_debug_mem_check(
        int unit,
        bcmi_ft_alu_load_type_t alu_load_type,
        int mem_idx)
{

    int i = 0;

    if (!BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        return 0;
    }


    /* Check in existing parameters. */
    for (i=0; i<BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info; i++) {
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
 *     bcmi_ft_chip_debug_alu_index_get
 * Purpose:
 *     Get ALU/Load type based on length/params.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     id  - (IN) FT group id.
 *     alu_load_type - (IN) type of load/ALU.
 *     index  - (OUT) Free index of that memory.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_chip_debug_alu_index_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *group_alu_info,
        bcmi_ft_alu_load_type_t alu_load_type,
        int *index)
{

    int i = 0;
    int *in_use_array = NULL;
    uint32 *group_in_use_array = NULL;
    uint32 total_mems = 1;
    int mem_idx = -1, mem_id = 0;
    int p_idx = 0; /* param index. */
    int size_per_mem = 0;
    int start_index = 0, end_index = 0;
    uint32 count = 0;

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
    size_per_mem =
        alu_load_index_info[unit][alu_load_type].alu_load_info->mem_size;

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
        BCM_IF_ERROR_RETURN(bcmi_ft_chip_debug_counter_init
            (unit, mem_idx, alu_load_type, count));
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
        if (!(SHR_BITGET(BCMI_FT_ALU_LOAD_BITMAP
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
 *     bcmi_ft_chip_debug_free_alu_index
 * Purpose:
 *     Get ALU/Load type based on length/params.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     id  - (IN) FT group id.
 *     alu_load_type - (IN) type of load/ALU.
 *     index  - (OUT) Free index of that memory.
 * Returns:
 *     BCM_E_XXX
 */
void
bcmi_ft_chip_debug_free_alu_index (
        int unit,
        bcmi_ft_group_alu_info_t *group_alu_info,
        bcmi_ft_alu_load_type_t alu_load_type)
{

    int i = 0;
    int *in_use_array = NULL;
    int *ref_count = NULL;
    int mem_idx = -1;

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


    for (i=0; i<BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info; i++) {
        if (group_alu_info->element_type1 ==
            BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param) {
            mem_idx = BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id;

            break;
        }
    }

    if (i == BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info) {
        /* Nothing matching found. so just return. */
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
bcmi_ft_chip_debug_data_sanity(
        int unit,
        int num_info,
        bcm_flowtracker_chip_debug_info_t *info)
{

    int num_groups = 0;
    int i, j, k;
    int num_tracking_elements = 0;
    soc_mem_t mem;
    bcm_flowtracker_tracking_param_info_t *track_info = NULL;
    bcm_flowtracker_tracking_param_type_t param;

    /* associate the memory. */
    mem = BSC_KG_GROUP_TABLEm;

    /* Total number of groups in device. */
    num_groups = soc_mem_index_count(unit, mem);

    for (j=0; j<num_info; j++) {
        param = info[j].param;

        for (i=0; i<num_groups; i++) {
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

            for (k=0; k<num_tracking_elements; k++) {
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
 *     bcmi_ft_group_alu_load_index_match_data_get
 * Purpose:
 *     Get matching data for this alu index.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     alu_type - (IN) ALU type.
 *     iter  - (IN) alu memory iterator.
 *     temp - (OUT) Pointer to point to data.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_alu_load_index_match_data_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_type,
        int iter,
        bcmi_ft_group_alu_info_t **temp)
{

    int start_index=0, j=0, i=0;
    int num_alus_loads = 0;
    int mem_size = 0;

    for (; j<iter; j++) {
        mem_size =
            alu_load_index_info[unit][alu_type].
                     alu_load_info[j].mem_size;
        start_index += mem_size;
    }

    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    (*temp) = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    mem_size = alu_load_index_info[unit][alu_type].
                   alu_load_info[iter].mem_size;

    for (; i<num_alus_loads; i++) {

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
 *     bcmi_ft_group_alu_load_pdd_status_get
 * Purpose:
 *     Get pdd status for this alu memory.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     alu_type - (IN) ALU type.
 *     iter  - (IN) alu memory iterator.
 *
 * Returns:
 *     1 = set
 *     0 = Not set.
 */
int
bcmi_ft_group_alu_load_pdd_status_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_type,
        int iter)
{
    /*
     * No need to check validity of group and other params.
     * If we are here so that all other checks are done.
     */

    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
          (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
          (BCMI_FT_PDD_INFO(unit, alu_type)[iter]).param_id)) {

        return 1;
    }

    return 0;
}

#else /* BCM_FLOWTRCAKER_SUPPORT*/
int bcmi_ft_alu_load_hw_not_empty;
#endif /* BCM_FLOWTRCAKER_SUPPORT */
