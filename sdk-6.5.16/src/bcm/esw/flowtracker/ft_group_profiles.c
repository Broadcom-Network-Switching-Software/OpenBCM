
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_group_profiles.c
 * Purpose:     The purpose of this file is to set flow
 *              tracker group's profiles methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

/* Internal MACROs for AgeOut Profile usage */
#define BCMI_FT_GROUP_AGE_OUT_PROFILE_AGE_INTERVAL      (1 << 0)
#define BCMI_FT_GROUP_AGE_OUT_PROFILE_EXPORT_TRIGGER    (1 << 1)

/* Group Sw state. */
extern
bcmi_ft_group_sw_info_t **bcmi_ft_group_sw_state[BCM_MAX_NUM_UNITS];

/* ALU Load memories information. */
extern
alu_load_index_info_t
    alu_load_index_info[BCM_MAX_NUM_UNITS][BCMI_FT_ALU_LOAD_MEMS];

/*
 * Function:
 *     bcmi_ft_group_profile_init
 * Purpose:
 *     Initialize Flowtracker group profile state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_group_profile_init(int unit)
{
    bsc_kg_age_out_profile_entry_t entry;
    bsc_kg_flow_exceed_profile_entry_t flow_entry;
    bsc_dg_group_copy_to_collector_profile_entry_t collector_entry;
    bsd_policy_action_profile_entry_t pdd_entry;
    bsc_dg_group_meter_profile_entry_t meter_entry;
    bsc_dg_group_timestamp_profile_entry_t ts_entry;
    void *entries[1];
    uint32 index = 0;

    /* Default entry for BSC_KG_AGE_OUT_PROFILE. */
    /* Make the entry zeroed. */
    sal_memset(&entry, 0, sizeof(bsc_kg_age_out_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &entry;

    /* Create the default profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_age_profile_entry_add(unit, entries, 1,
        (uint32 *)&index));

    /* Default entry for BSC_KG_FLOW_EXCEED_PROFILE. */
    /* Make the entry zeroed. */
    sal_memset(&flow_entry, 0, sizeof(bsc_kg_flow_exceed_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &flow_entry;

    /* Create the default profile entry for flow limit profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_flow_limit_profile_entry_add(unit,
        entries, 1, (uint32 *)&index));

    /* Default entry for BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILE. */
    /* Make the entry zeroed. */
    sal_memset(&collector_entry, 0,
        sizeof(bsc_dg_group_copy_to_collector_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &collector_entry;

    /* Create the default profile entry for collector copy profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_collector_copy_profile_entry_add(unit,
        entries, 1, (uint32 *)&index));

    /* Default entry for BSD_POLICY_ACTION_PROFILE. */
    /* Make the entry zeroed. */
    sal_memset(&pdd_entry, 0, sizeof(bsd_policy_action_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &pdd_entry;

    /* Create the default profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_pdd_profile_entry_add(unit, entries, 1,
        (uint32 *)&index));


    /* Default entry for BSC_DG_GROUP_METER_PROFILE memory. */
    sal_memset(&meter_entry, 0, sizeof(bsc_dg_group_meter_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &meter_entry;

    /* Create the default profile entry for group meter profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_meter_profile_entry_add(unit, entries, 1,
        (uint32 *)&index));

    /* Default entry for BSC_DG_GROUP_TIMESTAMP_PROFILE memory. */
    sal_memset(&ts_entry, 0, sizeof(bsc_dg_group_timestamp_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &ts_entry;

    /* Create the default profile entry for group meter profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_timestamp_profile_entry_add(unit, entries, 1,
        (uint32 *)&index));

    return BCM_E_NONE;

}

int
bcmi_ft_age_intrval_is_invalid(uint32 interval, int *val)
{


    if (val == NULL) {
       return BCM_E_PARAM;
    }

    if (interval == 0) {
        return BCM_E_NONE;
    }

    switch(interval) {

        case 100:
            *val = 0;
            break;
        case 1000:
            *val = 1;
            break;
        case 10000:
            *val = 2;
            break;
        case 60000:
            *val = 3;
            break;
        case 600000:
            *val = 4;
            break;
        case 1800000:
            *val = 5;
            break;
        case 3600000:
            *val = 6;
            break;
        case 36000000:
            *val = 7;
            break;
        case 86400000:
            *val = 8;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


int
bcmi_ft_age_intrval_get(int val, uint32 *interval)
{

    if (interval == NULL) {
        return BCM_E_PARAM;
    }

    switch(val) {

        case 0:
            *interval = 100;
        break;
        case 1:
            *interval = 1000;
        break;
        case 2:
            *interval = 10000;
        break;
        case 3:
            *interval = 60000;
        break;
        case 4:
            *interval = 600000;
        break;
        case 5:
            *interval = 1800000;
        break;
        case 6:
            *interval = 3600000;
        break;
        case 7:
            *interval = 36000000;
        break;
        case 8:
            *interval = 86400000;
        break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_age_profile_entry_set
 * Purpose:
 *    Insert entry in age_out profile
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     export_on_age - (IN) Enable export trigger for aged flows
 *     aging_interval - (IN) Interval for age out
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_age_profile_entry_set(
            int unit,
            uint32 flags,
            bcm_flowtracker_group_t id,
            uint32 export_on_age,
            int aging_interval)
{

    bsc_kg_age_out_profile_entry_t entry;
    bsc_kg_group_table_entry_t kg_entry;
    void *entries[1];
    uint32 age_fmt;
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP(unit, id)->flags &
            BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {

        /* Validate export on age and aging interval */
        if ((export_on_age == 0) && (aging_interval == -1)) {
            return BCM_E_NONE;
        }
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Configuration of age timer/trigger on"
                            " software-managed group is not allowed\n")));
        return BCM_E_CONFIG;
    }

    /*
     * There are three cases here.
     * Add a new profile for a new group.
     * Change the interval/trigger for existing group.
     * Delete/set(0) for existing group.
     */

    /* initialize the format entry. */
    sal_memset(&age_fmt, 0, sizeof(uint32));

    /* Initialize the age out profile entry. */
    sal_memset(&entry, 0, sizeof(bsc_kg_age_out_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &entry;

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_entry), AGE_OUT_PROFILE_IDXf);

    /* Read the configured age out profile entry. */
    rv = soc_mem_read(unit, BSC_KG_AGE_OUT_PROFILEm,
                      MEM_BLOCK_ANY, old_index, &entry);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field_get(unit, BSC_KG_AGE_OUT_PROFILEm, (uint32 *)&entry,
                      DATAf, &age_fmt);

    if (flags & BCMI_FT_GROUP_AGE_OUT_PROFILE_AGE_INTERVAL) {
        if (aging_interval == -1) {
            soc_format_field32_set(unit, BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                    &age_fmt, AGE_OUT_INTERVALf, 0);

            soc_format_field32_set(unit, BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                    &age_fmt, AGE_OUT_ENABLEf, 0);
        } else {

            /* Each interval is at 4ms. */
            soc_format_field32_set(unit, BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                    &age_fmt, AGE_OUT_INTERVALf, (uint32) aging_interval);

            soc_format_field32_set(unit, BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                    &age_fmt, AGE_OUT_ENABLEf, 1);

            /* Export on Flush is always disabled.
             * Leaving EXPORT_FLUSH_ENf at default value (0x0) */
        }
    }

    if (flags & BCMI_FT_GROUP_AGE_OUT_PROFILE_EXPORT_TRIGGER) {
        soc_format_field32_set(unit, BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                           &age_fmt, EXPORT_AGE_OUTf, export_on_age);
    }

    /* Set the entry. */
    soc_mem_field_set(unit, BSC_KG_AGE_OUT_PROFILEm, (uint32 *)&entry,
                      DATAf, &age_fmt);

    /* Create the profile entry for new age out profile. */
    rv = bcmi_ft_group_age_profile_entry_add(unit, entries,
                                             1, (uint32 *)&index);
    BCM_IF_ERROR_RETURN(rv);

    if (old_index != index) {
        /* now write this new index into the group. */
        soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, ((uint32 *)&kg_entry),
                            AGE_OUT_PROFILE_IDXf, index);

        rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &kg_entry);
    }

    if (old_index != 0) {
        (void)bcmi_ft_group_age_profile_entry_delete(unit, old_index);
    }

    return rv;
}

/*
 * Function:
 *     bcmi_ft_group_age_export_trigger_set
 * Purpose:
 *    Enable export trigger on age
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     enable - (IN) Enable export trigger for aged flows
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_group_age_export_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 enable)
{
    return bcmi_ft_group_age_profile_entry_set(unit,
            BCMI_FT_GROUP_AGE_OUT_PROFILE_EXPORT_TRIGGER,
            id, enable, -1);
}

/*
 * Function:
 *     bcmi_ft_group_age_export_trigger_get
 * Purpose:
 *    Get export trigger on age-out
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     enable - (IN) Enable export trigger for aged flows
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_group_age_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *enable)
{
    bsc_kg_age_out_profile_entry_t entry;
    bsc_kg_group_table_entry_t kg_entry;
    uint32 age_outfmt;
    uint32 index = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    /* initialize the format entry. */
    sal_memset(&age_outfmt, 0, sizeof(uint32));
    sal_memset(&entry, 0, sizeof(bsc_kg_age_out_profile_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_entry));

    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_entry), AGE_OUT_PROFILE_IDXf);

    /* Read the profile entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_AGE_OUT_PROFILEm,
        MEM_BLOCK_ANY, index, &entry));

    soc_mem_field_get(unit, BSC_KG_AGE_OUT_PROFILEm, (uint32 *)&entry,
        DATAf, &age_outfmt);

    *enable = soc_format_field32_get(unit,
                                     BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                                     &age_outfmt, EXPORT_AGE_OUTf);

    return BCM_E_NONE;

}



/*
 * Function:
 *     bcmi_ft_group_age_out_set
 * Purpose:
 *     Set the Age out profile on this group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     aging_interval_ms - (IN) Age out internam in ms.
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_group_age_out_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 aging_interval_ms)
{
    int val = -1;

    if (bcmi_ft_age_intrval_is_invalid(aging_interval_ms, &val)) {
        return BCM_E_PARAM;
    }

    return bcmi_ft_group_age_profile_entry_set(unit,
            BCMI_FT_GROUP_AGE_OUT_PROFILE_AGE_INTERVAL, id, 0, val);
}

/*
 * Function:
 *     bcmi_ft_group_age_out_get
 * Purpose:
 *     get the Age out profile on this group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     aging_interval_ms - (OUT) Age out internam in ms.
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_group_age_out_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *aging_interval_ms)
{
    bsc_kg_age_out_profile_entry_t entry;
    bsc_kg_group_table_entry_t kg_entry;
    uint32 age_outfmt;
    uint32 index = 0;
    int val = -1;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    /* initialize the format entry. */
    sal_memset(&age_outfmt, 0, sizeof(uint32));
    sal_memset(&entry, 0, sizeof(bsc_kg_age_out_profile_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_entry));

    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_entry), AGE_OUT_PROFILE_IDXf);

    /* Read age-out profile entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_AGE_OUT_PROFILEm,
        MEM_BLOCK_ANY, index, &entry));

    soc_mem_field_get(unit, BSC_KG_AGE_OUT_PROFILEm, (uint32 *)&entry,
        DATAf, &age_outfmt);

    val = soc_format_field32_get(unit,
        BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
        &age_outfmt, AGE_OUT_ENABLEf);

    if (val) {
        /* Each interval is at 4ms. */
        val = soc_format_field32_get(unit,
            BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
            &age_outfmt, AGE_OUT_INTERVALf);

        BCM_IF_ERROR_RETURN(bcmi_ft_age_intrval_get(val, aging_interval_ms));
    } else {
        *aging_interval_ms = 0;
    }

    return BCM_E_NONE;
}

int
bcmi_ft_group_flow_limit_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flow_limit)
{

    bsc_kg_flow_exceed_profile_entry_t entry;
    bsc_kg_group_table_entry_t kg_entry;
    void *entries[1];
    uint32 flow_exceedfmt;
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;
    int fid_len = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if(BCMI_FT_GROUP(unit, id)->flags &
            BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
        LOG_VERBOSE(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Configuration of flow limit on"
                            " software-managed group is not allowed\n")));
        return BCM_E_CONFIG;
    }

    fid_len = soc_format_field_length(
        unit, BSC_TL_KG_TO_KT_LIMITS_CONTROL_BUSfmt, MAX_FLOW_LIMITf);

    if (flow_limit > ((1 << fid_len) - 1)) {
        return BCM_E_PARAM;
    }

    /* If Group is busy, dont allow non-zero if collector is not configured. */
    if (BCMI_FT_GROUP_IS_BUSY(unit, id) &&
      (BCMI_FT_GROUP(unit, id)->collector_id == BCMI_FT_COLLECTOR_ID_INVALID) &&
      (flow_limit != 0)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Collector is not associated"
                            " with Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
    }

    /*
     * There are three cases here.
     * Add a new profile for a new group.
     * Change the interval for existing group.
     * Delete/set(0) for existing group.
     */

    /* initialize the format entry. */
    sal_memset(&flow_exceedfmt, 0, sizeof(uint32));

    /* Initialize the flow exceed profile entry. */
    sal_memset(&entry, 0, sizeof(bsc_kg_flow_exceed_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &entry;

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_entry), FLOW_EXCEED_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_flow_limit_profile_entry_delete(unit, old_index);
    }

    /* Initialize the flow limit profile entry. */
    sal_memset(&entry, 0, sizeof(bsc_kg_flow_exceed_profile_entry_t));

    if (flow_limit == 0) {
        /* The zero value means that delete the index. */
        soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, ((uint32 *)&kg_entry),
            FLOW_EXCEED_PROFILE_IDXf, 0);
    } else {

        /* Each interval is at 4ms. */
        soc_format_field32_set(unit, BSC_TL_KG_TO_KT_LIMITS_CONTROL_BUSfmt,
                           &flow_exceedfmt, MAX_FLOW_LIMITf, flow_limit);

        /* Set the entry. */
        soc_mem_field_set(unit, BSC_KG_FLOW_EXCEED_PROFILEm, (uint32 *)&entry,
            DATAf, &flow_exceedfmt);

        /* Create teh profile entry for new flow limit profile. */
        BCM_IF_ERROR_RETURN(bcmi_ft_group_flow_limit_profile_entry_add(unit,
            entries, 1, (uint32 *)&index));

        /* now write this new index into the group. */
        soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, ((uint32 *)&kg_entry),
            FLOW_EXCEED_PROFILE_IDXf, index);
    }

    rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &kg_entry);

    return rv;

}

/*
 * Function:
 *     bcmi_ft_group_flow_limit_get
 * Purpose:
 *     Get the Age out profile on this group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     flow_limit - (OUT) Age out internam in ms.
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_group_flow_limit_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_limit)
{
    bsc_kg_flow_exceed_profile_entry_t entry;
    bsc_kg_group_table_entry_t kg_entry;
    uint32 flow_exceedfmt;
    uint32 index = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if(BCMI_FT_GROUP(unit, id)->flags &
            BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
        LOG_VERBOSE(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Configuration of flow limit on"
                            " software-managed group is not allowed\n")));
        return BCM_E_CONFIG;
    }

    /* initialize the format entry. */
    sal_memset(&flow_exceedfmt, 0, sizeof(uint32));
    sal_memset(&entry, 0, sizeof(bsc_kg_flow_exceed_profile_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_entry));

    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_entry), FLOW_EXCEED_PROFILE_IDXf);

    if (index) {
        /* First read the group entry. */
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_FLOW_EXCEED_PROFILEm,
            MEM_BLOCK_ANY, index, &entry));

        soc_mem_field_get(unit, BSC_KG_FLOW_EXCEED_PROFILEm, (uint32 *)&entry,
            DATAf, &flow_exceedfmt);

        /* Retrieve the interval. */
        *flow_limit = soc_format_field32_get(unit,
            BSC_TL_KG_TO_KT_LIMITS_CONTROL_BUSfmt,
            &flow_exceedfmt, MAX_FLOW_LIMITf);

        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *    bcmi_ft_group_profiles_destroy
 * Purpose:
 *     Destroy all the profiles in the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     info - (IN) Copy to collector related information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_profiles_destroy(int unit, bcm_flowtracker_group_t id)
{

    int old_index;
    bsc_kg_group_table_entry_t kg_entry;
    bsc_dg_group_table_entry_t dg_entry;

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_entry));

    /* Destroy FLOW_EXCEED_PROFILE  from the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_entry), FLOW_EXCEED_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_flow_limit_profile_entry_delete(unit, old_index);
    }

    /* Destroy AGE_OUT_PROFILE from the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_entry), AGE_OUT_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_age_profile_entry_delete(unit, old_index);
    }

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry));

    /* Destroy COPY_TO_COLLECTOR_PROFILE from the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
        ((uint32 *)&dg_entry), COPY_TO_COLLECTOR_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_collector_copy_profile_entry_delete(unit, old_index);
    }

    /* Destroy GROUP_METER_PROFILE_IDX from the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
        ((uint32 *)&dg_entry), GROUP_METER_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_meter_profile_entry_delete(unit, old_index);
    }

    /* Destroy Timestamp profile index from the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
        ((uint32 *)&dg_entry), TIMESTAMP_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_timestamp_profile_entry_delete(unit, old_index);
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_group_collector_copy_set
 * Purpose:
 *     Set copy to collector settings in group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     info - (IN) Copy to collector related information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_collector_copy_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_collector_copy_info_t info)
{

    bsc_dg_group_copy_to_collector_profile_entry_t entry;
    bsc_dg_group_table_entry_t dg_entry;
    void *entries[1];
    uint32 fmt;
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;
    uint32 fid_len = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    /*
     * There are three cases here.
     * Add a new profile for a new group.
     * Change the interval for existing group.
     * Delete/set(0) for existing group.
     */

    /* initialize the format entry. */
    sal_memset(&fmt, 0, sizeof(uint32));
    /* Initialize the age out profile entry. */
    sal_memset(&entry, 0,
        sizeof(bsc_dg_group_copy_to_collector_profile_entry_t));
    /* Initialize group entry. */
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &entry;

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
        ((uint32 *)&dg_entry), COPY_TO_COLLECTOR_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_collector_copy_profile_entry_delete(unit, old_index);
    }

    /* Initialize the age out profile entry. */
    sal_memset(&entry, 0,
        sizeof(bsc_dg_group_copy_to_collector_profile_entry_t));

    fid_len = soc_format_field_length(
        unit, BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt, MAX_COUNTf);

    if (info.num_pkts_initial_samples > ((1 << fid_len) - 1)) {
        return BCM_E_PARAM;
    }

    fid_len = soc_format_field_length(
        unit, BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt,
        SAMPLE_UPPER_LIMITf);

    if (info.num_pkts_skip_for_next_sample > ((1 << fid_len) - 1)) {
        return BCM_E_PARAM;
    }

    if (info.flags & BCM_FLOWTRACKER_COLLECTOR_COPY_SAMPLE_RANDOM) {
        soc_format_field32_set(unit,
            BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt,
            &fmt, PACKET_SAMPLING_METHODf, 1);
    }

    if (info.num_pkts_initial_samples) {
        soc_format_field32_set(unit,
            BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt,
            &fmt, INITIAL_COPY_COUNT_ENABLEf, 1);

        soc_format_field32_set(unit,
            BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt,
            &fmt, MAX_COUNTf, info.num_pkts_initial_samples);
    }

    if (info.num_pkts_skip_for_next_sample) {
        soc_format_field32_set(unit,
            BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt,
            &fmt, SAMPLE_COPY_COUNT_ENABLEf, 1);

        soc_format_field32_set(unit,
            BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt,
            &fmt, SAMPLE_UPPER_LIMITf, info.num_pkts_skip_for_next_sample);
    }

    /* Set the entry. */
    soc_mem_field_set(unit, BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILEm,
        (uint32 *)&entry, DATAf, &fmt);

    /* Create teh profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_collector_copy_profile_entry_add(unit,
        entries, 1, (uint32 *)&index));

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, ((uint32 *)&dg_entry),
        COPY_TO_COLLECTOR_PROFILE_IDXf, index);

    soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
        BSD_FLEX_FLOW_COUNT_TO_CPUf, 1);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    if (rv || (index == 0)) {
        soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_FLOW_COUNT_TO_CPUf, 0);
    }

    return rv;

}

/*
 * Function:
 *    bcmi_ft_group_collector_copy_get
 * Purpose:
 *     get copy to collector settings in group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     info - (OUT) Copy to collector related information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_collector_copy_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_collector_copy_info_t *info)
{

    bsc_dg_group_copy_to_collector_profile_entry_t entry;
    bsc_dg_group_table_entry_t dg_entry;
    uint32 fmt;
    int old_index = 0;
    uint32 fid_value = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    /*
     * There are three cases here.
     * Add a new profile for a new group.
     * Change the interval for existing group.
     * Delete/set(0) for existing group.
     */

    /* initialize the format entry. */
    sal_memset(&fmt, 0, sizeof(uint32));
    /* Initialize the age out profile entry. */
    sal_memset(&entry, 0,
        sizeof(bsc_dg_group_copy_to_collector_profile_entry_t));
    /* Initialize group entry. */
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));


    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
       ((uint32 *)&dg_entry), COPY_TO_COLLECTOR_PROFILE_IDXf);

    if (!old_index) {
        sal_memset(info, 0, sizeof(bcm_flowtracker_collector_copy_info_t));
        return BCM_E_NONE;
    }


    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit,
        BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILEm,
        MEM_BLOCK_ANY, old_index, &entry));

    soc_mem_field_get(unit, BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILEm,
        (uint32 *)&entry, DATAf, &fmt);

    /* Retrieve the interval. */
    fid_value = soc_format_field32_get(unit,
        BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt,
        &fmt, INITIAL_COPY_COUNT_ENABLEf);

    if (fid_value) {
       info->num_pkts_initial_samples = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt, &fmt,
            MAX_COUNTf);
    }

    /* Retrieve the interval. */
    fid_value = soc_format_field32_get(unit,
         BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt,
         &fmt, SAMPLE_COPY_COUNT_ENABLEf);

    if (fid_value) {
       info->num_pkts_skip_for_next_sample = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt, &fmt,
            SAMPLE_UPPER_LIMITf);
    }

    if (soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt, &fmt,
            PACKET_SAMPLING_METHODf)) {
        info->flags |= BCM_FLOWTRACKER_COLLECTOR_COPY_SAMPLE_RANDOM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_group_meter_profile_set
 * Purpose:
 *     Set the Meter setting on this group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     info - (IN) Meter related information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_meter_profile_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_meter_info_t info)
{

    bsc_dg_group_meter_profile_entry_t entry;
    bsc_dg_group_table_entry_t dg_entry;
    void *entries[1];
    uint32 fmt[2];
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;
    uint32 flags = 0;
    uint32    bucketsize_commit = 0;    /* Bucket size.             */
    uint32    refresh_rate_commit = 0;  /* Policer refresh rate.    */
    uint32    granularity_commit = 0;   /* Policer granularity.     */
    int       refresh_bitsize;          /* Number of bits for the
                                           refresh rate field.      */
    int       bucket_max_bitsize;       /* Number of bits for the
                                           bucket max field.        */

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    /* No update to meter after group is installed.*/
    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    /*
     * There are three cases here.
     * Add a new profile for a new group.
     * Change the interval for existing group.
     * Delete/set(0) for existing group.
     */

    /* initialize the format entry. */
    sal_memset(&fmt, 0, 2 * sizeof(uint32));
    /* Initialize the age out profile entry. */
    sal_memset(&entry, 0, sizeof(bsc_dg_group_meter_profile_entry_t));
    /* Initialize group entry. */
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &entry;

    /* lookup bucket size from tables */
    flags = _BCM_XGS_METER_FLAG_GRANULARITY;
    flags |= _BCM_XGS_METER_FLAG_REFRESH_RATE_15p625;

    /* Set packet mode flags setting */
    if (info.flags & BCM_FLOWTRACKER_METER_MODE_PACKET) {
        flags |= _BCM_XGS_METER_FLAG_PACKET_MODE;
    } else {
        flags &= ~_BCM_XGS_METER_FLAG_PACKET_MODE;
    }


    refresh_bitsize = soc_format_field_length(
        unit, BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt, REFRESHCOUNTf);

    bucket_max_bitsize = soc_format_field_length(
        unit, BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt, BUCKETSIZEf);


    /* Calculate policer bucket size/refresh_rate/granularity. */
    rv = _bcm_xgs_kbits_to_bucket_encoding(info.ckbits_sec,
                                           info.ckbits_burst,
                                           flags, refresh_bitsize,
                                           bucket_max_bitsize,
                                           &refresh_rate_commit,
                                           &bucketsize_commit,
                                           &granularity_commit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
        ((uint32 *)&dg_entry), GROUP_METER_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_meter_profile_entry_delete(unit, old_index);
    }

    /* Initialize the age out profile entry. */
    sal_memset(&entry, 0, sizeof(bsc_dg_group_meter_profile_entry_t));


    soc_format_field32_set(unit,
        BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt,
        fmt, BUCKETSIZEf, bucketsize_commit);

    soc_format_field32_set(unit,
        BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt,
        fmt, REFRESHCOUNTf, refresh_rate_commit);

    soc_format_field32_set(unit,
        BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt,
        fmt, METER_GRANf, granularity_commit);

    if (info.flags & BCM_FLOWTRACKER_METER_MODE_PACKET) {
        soc_format_field32_set(unit,
            BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt,
            fmt, PKTS_BYTESf, 1);
    }

    /* Set the entry. */
    soc_mem_field_set(unit, BSC_DG_GROUP_METER_PROFILEm, (uint32 *)&entry,
        DATAf, fmt);

    /* Create teh profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_meter_profile_entry_add(unit, entries, 1,
        (uint32 *)&index));

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, ((uint32 *)&dg_entry),
        GROUP_METER_PROFILE_IDXf, index);

    BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_burst = info.ckbits_burst;
    BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_sec = info.ckbits_sec;

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);
    soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
        BSD_FLEX_FLOW_METERf, 1);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    if (rv || (index == 0)) {
        soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_FLOW_METERf, 0);
    }

    return rv;
}

/*
 * Function:
 *     bcmi_ft_group_meter_profile_get
 * Purpose:
 *     Get the Meter setting on this group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     info - (OUT) Meter related information.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_meter_profile_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_meter_info_t *info)
{

    bsc_dg_group_meter_profile_entry_t entry;
    bsc_dg_group_table_entry_t dg_entry;
    uint32 fmt[2];
    int old_index = 0;
    uint32 flags = 0;
    uint32 bucket_max = 0;    /* Bucket size.             */
    uint32 refresh_rate = 0;  /* Policer refresh rate.    */
    uint32 granularity = 0;   /* Policer granularity.     */


    /* initialize the format entry. */
    sal_memset(&fmt, 0, 2 * sizeof(uint32));
    /* Initialize the age out profile entry. */
    sal_memset(&entry, 0, sizeof(bsc_dg_group_meter_profile_entry_t));
    /* Initialize group entry. */
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));


    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
       ((uint32 *)&dg_entry), GROUP_METER_PROFILE_IDXf);

    if (!old_index) {
        sal_memset(info, 0, sizeof(bcm_flowtracker_meter_info_t));
        return BCM_E_NONE;
    }

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_METER_PROFILEm,
        MEM_BLOCK_ANY, old_index, &entry));

    soc_mem_field_get(unit, BSC_DG_GROUP_METER_PROFILEm, (uint32 *)&entry,
        DATAf, fmt);

    refresh_rate = soc_format_field32_get(unit,
        BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt,
        fmt, REFRESHCOUNTf);

    granularity = soc_format_field32_get(unit,
        BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt,
        fmt, METER_GRANf);

    bucket_max = soc_format_field32_get(unit,
        BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt,
        fmt, BUCKETSIZEf);


    if (soc_format_field32_get(unit, BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt,
        fmt, PKTS_BYTESf)) {
        info->flags |= BCM_FLOWTRACKER_METER_MODE_PACKET;
        flags |= _BCM_XGS_METER_FLAG_PACKET_MODE;
    }

    flags |= _BCM_XGS_METER_FLAG_GRANULARITY;
    flags |= _BCM_XGS_METER_FLAG_REFRESH_RATE_15p625;

    BCM_IF_ERROR_RETURN(_bcm_xgs_bucket_encoding_to_kbits
        (refresh_rate, bucket_max, granularity, flags,
         (&(info->actual_ckbits_sec)), (&(info->actual_ckbits_burst))));

    info->ckbits_sec = BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_sec;
    info->ckbits_burst = BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_burst;


    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_pdd_to_pde_coversion
 * Purpose:
 *     Set PDD to PDE conversion.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     pde_entry - (OUT) PDE entry
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_pdd_to_pde_coversion(
        int unit,
        bcm_flowtracker_group_t id,
        bsc_dt_pde_profile_entry_t *pde_entry)
{
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    int alu_load_type = bcmiFtAluLoadTypeNone;
    int total_mems = 0;
    uint8 *pde = NULL;
    int running_pde_index = 63;
    int mem_idx = 0, i = 0, type=0;
    int end = 0;
    bcmi_ft_alu_load_type_t pde_order[4] =
        {bcmiFtAluLoadTypeAlu32,
         bcmiFtAluLoadTypeLoad16,
         bcmiFtAluLoadTypeAlu16,
         bcmiFtAluLoadTypeLoad8};

    if (pde_entry == NULL) {
        return BCM_E_PARAM;
    }


    sal_memset(pde_entry, 0, sizeof(bsc_dt_pde_profile_entry_t));
    pde = (uint8*)pde_entry;


    /* Timestamp 0. 48 bit. 6 bytes. */
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_TIMESTAMP_0f)) {
        end = 1095;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }

    /* Timestamp 1.  48 bit. 6 bytes.*/
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_TIMESTAMP_1f)) {
        end = 1047;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }

    /* Timestamp 2.  48 bit. 6 bytes.*/
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_TIMESTAMP_2f)) {
        end = 999;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }
    /* Timestamp 3.  48 bit. 6 bytes.*/
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_TIMESTAMP_3f)) {
        end = 951;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }


    /* Meter entry.  48 bit. 6 bytes.*/
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_FLOW_METERf)) {
        end = 903;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }


    /* For all the types of memory types. */
    for (type=0; type<4; type++) {

        alu_load_type = pde_order[type];
        total_mems = alu_load_index_info[unit][alu_load_type].total_mem_count;

        if (alu_load_type == bcmiFtAluLoadTypeLoad16) {
            total_mems = TOTAL_GROUP_LOAD16_DATA_NUM;
        } else  if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
            total_mems = TOTAL_GROUP_LOAD8_DATA_NUM;

           if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
                (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                BSD_FLEX_FLOW_COUNT_TO_CPUf)) {

                end = 87;
                for (i=0; i<2; i++) {
                    pde[running_pde_index] = (((end) - (i * 8))/8);
                    running_pde_index--;
                }
            }
        }

        /* For all the alu/load types, check what memories are set in pdd. */
        for (mem_idx=0; mem_idx<total_mems; mem_idx++) {
            /* Now get if this particular field is set in pdd. */
            pdd_info = alu_load_index_info[unit][alu_load_type].pdd_bus_info;
            if (!pdd_info) {
                return BCM_E_INTERNAL;
            }
            if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
                    (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                    pdd_info[mem_idx].param_id)) {
                for (i=0; i<pdd_info[mem_idx].length; i++) {
                    pde[running_pde_index] =
                        (((pdd_info[mem_idx].end) - (i * 8))/8);

                    running_pde_index--;
                }
            }
        }
    }

    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_FLOW_STATEf)) {
        end = 7;
        for (i=0; i<1; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_group_timestamp_entry_set
 * Purpose:
 *     Set Timestamp entry settings in group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *     entry - (OUT) timestamp profile entry.
 *
 * Returns:
 *     None.
 */
void
bcmi_ft_group_timestamp_entry_set(
        int unit,
        bcm_flowtracker_group_t id,
        bsc_dg_group_timestamp_profile_entry_t *entry)
{
    uint32 fmt[1];
    int i=0, j=0;

    soc_field_t ts_data_fields[4] =
                    {DATA_0f, DATA_1f, DATA_2f, DATA_3f};

    soc_field_t ts_pdd_fields[4] =
                    {BSD_FLEX_TIMESTAMP_0f,
                     BSD_FLEX_TIMESTAMP_1f,
                     BSD_FLEX_TIMESTAMP_2f,
                     BSD_FLEX_TIMESTAMP_3f};

    sal_memset(fmt, 0, sizeof(uint32));

    /* All the triggers are set in the group structure. */
    for (; i<BCMI_FT_GROUP_MAX_TS_TYPE; i++) {
        if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << i)) {
            soc_format_field32_set(unit,
                BSC_KMAP_CONTROLfmt, fmt, KMAP_INPUT_SELECT_0f,
                bcmi_ft_group_ts_info[i].ts_trigger_bit_sel);
            soc_format_field32_set(unit,
                BSC_KMAP_CONTROLfmt, fmt, KMAP_INPUT_SELECT_1f,
                bcmi_ft_group_ts_info[i].ts_trigger_bit_sel);
            soc_format_field32_set(unit,
                BSC_KMAP_CONTROLfmt, fmt, KMAP_INPUT_SELECT_2f,
                bcmi_ft_group_ts_info[i].ts_trigger_bit_sel);
            soc_format_field32_set(unit,
                BSC_KMAP_CONTROLfmt, fmt, KMAP_INPUT_SELECT_3f,
                bcmi_ft_group_ts_info[i].ts_trigger_bit_sel);

            soc_format_field32_set(unit,
                BSC_KMAP_CONTROLfmt, fmt, KMAP_CONTROLf, 0x8000);

            /* Set the entry. */
            soc_mem_field_set(unit, BSC_DG_GROUP_TIMESTAMP_PROFILEm,
                (uint32 *)entry, ts_data_fields[j], fmt);

            /* Setting this field id into structure for bookkeeping.*/
            bcmi_ft_group_ts_info[i].ts_field = ts_data_fields[j];

            /* Set the PDD entry. */
            soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                ts_pdd_fields[j], 1);

            /*
             * If the trigger is flow state based then
             * flow state needs to be taken in pdd entry.
             */
            if ((bcmi_ft_group_ts_info[i].ts_param ==
                    bcmFlowtrackerTrackingParamTypeTimestampFlowStart) ||
                    (bcmi_ft_group_ts_info[i].ts_param ==
                    bcmFlowtrackerTrackingParamTypeTimestampFlowEnd)) {

                /* Set the PDD entry. */
                soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
                    (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                    BSD_FLEX_FLOW_STATEf, 1);
            }

            j++;

        }
    }

}

/*
 * Function:
 *    bcmi_ft_group_timestamp_set
 * Purpose:
 *     Set Timestamp settings in group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id - (IN) Flowtracker gorup id.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_timestamp_set(
        int unit,
        bcm_flowtracker_group_t id)
{

    bsc_dg_group_timestamp_profile_entry_t entry;
    bsc_dg_group_table_entry_t dg_entry;
    void *entries[1];
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    /*
     * There are three cases here.
     * Add a new profile for a new group.
     * Change the interval for existing group.
     * Delete/set(0) for existing group.
     */

    /* Initialize the timestamp profile entry. */
    sal_memset(&entry, 0,
        sizeof(bsc_dg_group_timestamp_profile_entry_t));
    /* Initialize group entry. */
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &entry;

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
        ((uint32 *)&dg_entry), TIMESTAMP_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_timestamp_profile_entry_delete(unit, old_index);
    }

    bcmi_ft_group_timestamp_entry_set(unit, id, &entry);

    /* Create teh profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_timestamp_profile_entry_add(unit,
        entries, 1, (uint32 *)&index));

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, ((uint32 *)&dg_entry),
        TIMESTAMP_PROFILE_IDXf, index);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    return rv;

}
#else /* BCM_FLOWTRCAKER_SUPPORT*/
int bcmi_ft_group_profiles_not_empty;
#endif /* BCM_FLOWTRCAKER_SUPPORT */
