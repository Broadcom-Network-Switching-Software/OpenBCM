
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_group_profiles.c
 * Purpose:     The purpose of this file is to set flow
 *              tracker group's profiles methods.
 * Requires:
 */
#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#if defined(BCM_FLOWTRACKER_SUPPORT)

/* Internal MACROs for AgeOut Profile usage */
#define BCMI_FT_GROUP_AGE_OUT_PROFILE_AGE_INTERVAL      (1 << 0)
#define BCMI_FT_GROUP_AGE_OUT_PROFILE_EXPORT_TRIGGER    (1 << 1)
#define BCMI_FT_GROUP_AGE_OUT_PROFILE_RESERVATION       (1 << 2)

/*
 * Function:
 *   bcmi_ft_group_profile_init
 * Purpose:
 *   Initialize Flowtracker group profile state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
int
bcmi_ft_group_profile_init(int unit)
{
    void *entries[3];
    uint32 index = 0;
    bsc_kg_age_out_profile_entry_t entry;
    bsd_policy_action_profile_entry_t pdd_entry;
    bsc_kg_flow_exceed_profile_entry_t flow_entry;
    bsc_dg_group_meter_profile_entry_t meter_entry;
    bsc_dg_group_timestamp_profile_entry_t ts_entry;
    bsc_dg_group_copy_to_collector_profile_entry_t collector_entry;
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    bsd_policy_action_profile_0_entry_t ift_pdd_entry;
    bsd_policy_action_profile_1_entry_t mft_pdd_entry;
    bsd_policy_action_profile_2_entry_t eft_pdd_entry;
#endif

    /* Default entry for BSC_KG_AGE_OUT_PROFILE. */
    /* Make the entry zeroed. */
    sal_memset(&entry, 0, sizeof(bsc_kg_age_out_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &entry;

    /* Create the default profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_age_profile_entry_add(unit, entries, 1,
                                                        (uint32 *)&index));

    /* Default entry for BSC_KG_FLOW_EXCEED_PROFILE. */
    /* Make the entry zeroed. */
    sal_memset(&flow_entry, 0, sizeof(bsc_kg_flow_exceed_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &flow_entry;

    /* Create the default profile entry for flow limit profile. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_flow_limit_profile_entry_add(unit,
                                            entries, 1, (uint32 *)&index));

    /* Default entry for BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILE. */
    /* Make the entry zeroed. */
    sal_memset(&collector_entry, 0,
                   sizeof(bsc_dg_group_copy_to_collector_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &collector_entry;

    /* Create the default profile entry for collector copy profile. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_collector_copy_profile_entry_add(unit,
                                            entries, 1, (uint32 *)&index));

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

        /* Default entry for BSD_POLICY_ACTION_PROFILE_x */
        sal_memset(&ift_pdd_entry, 0,
                   sizeof(bsd_policy_action_profile_0_entry_t));
        sal_memset(&mft_pdd_entry, 0,
                   sizeof(bsd_policy_action_profile_1_entry_t));
        sal_memset(&eft_pdd_entry, 0,
                   sizeof(bsd_policy_action_profile_2_entry_t));

        /* set the entry to profile entries. */
        entries[0] = &ift_pdd_entry;
        entries[1] = &mft_pdd_entry;
        entries[2] = &eft_pdd_entry;

        /* Create the default profile entry for new age out profile. */
        BCM_IF_ERROR_RETURN
            (bcmi_ft_group_pdd_profile_entry_add(unit,
                                            entries, 1, (uint32 *)&index));
    } else
#endif
    {
        /* Default entry for BSD_POLICY_ACTION_PROFILE. */
        /* Make the entry zeroed. */
        sal_memset(&pdd_entry, 0, sizeof(bsd_policy_action_profile_entry_t));

        /* set the entry to profile entries. */
        entries[0] = &pdd_entry;

        /* Create the default profile entry for new age out profile. */
        BCM_IF_ERROR_RETURN
            (bcmi_ft_group_pdd_profile_entry_add(unit,
                                            entries, 1, (uint32 *)&index));
    }

    /* Default entry for BSC_DG_GROUP_METER_PROFILE memory. */
    sal_memset(&meter_entry, 0, sizeof(bsc_dg_group_meter_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &meter_entry;

    /* Create the default profile entry for group meter profile. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_meter_profile_entry_add(unit, entries, 1,
                                                        (uint32 *)&index));

    /* Default entry for BSC_DG_GROUP_TIMESTAMP_PROFILE memory. */
    sal_memset(&ts_entry, 0, sizeof(bsc_dg_group_timestamp_profile_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &ts_entry;

    /* Create the default profile entry for group timestamp profile. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_timestamp_profile_entry_add(unit,
                                            entries, 1, (uint32 *)&index));

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_age_interval_is_invalid(uint32 interval, int *val)
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

STATIC int
bcmi_ft_age_interval_get(int val, uint32 *interval)
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
 *   bcmi_ft_group_age_profile_entry_set
 * Purpose:
 *   Insert entry in age_out profile
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   export_on_age - (IN) Enable export trigger for aged flows
 *   aging_interval - (IN) Interval for age out
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_age_profile_entry_set(int unit,
                                    uint32 flags,
                                    bcm_flowtracker_group_t id,
                                    uint32 export_on_age,
                                    int aging_interval)
{
    void *entries[1];
    uint32 age_fmt;
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;
    uint32 reservation = 0;
    bsc_kg_group_table_entry_t kg_entry;
    bsc_kg_age_out_profile_entry_t entry;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP(unit, id)->flags &
            BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {

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
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
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

        /* Set IPFIX reservation */
        reservation = 0;
        if ((export_on_age != 0) &&
                (flags & BCMI_FT_GROUP_AGE_OUT_PROFILE_RESERVATION)) {
            reservation = 1;
        }

        if (soc_format_field_valid(unit, BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                    DO_NOT_AGE_WHEN_FIFO_FULLf)) {

            soc_format_field32_set(unit, BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                    &age_fmt, DO_NOT_AGE_WHEN_FIFO_FULLf, reservation);

        }
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

        rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm,
                            MEM_BLOCK_ANY, id, &kg_entry);
    }

    if (old_index != 0) {
        (void)bcmi_ft_group_age_profile_entry_delete(unit, old_index);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_age_export_trigger_set
 * Purpose:
 *   Enable export trigger on age
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   flags  - (IN) Export trigger flags
 *   enable - (IN) Enable export trigger for aged flows
 * Returns:
 *   None.
 */
int
bcmi_ft_group_age_export_trigger_set(int unit,
                                     bcm_flowtracker_group_t id,
                                     uint32 flags,
                                     uint32 enable)
{
    uint32 profile_flags = 0;

    if (flags & BCM_FLOWTRACKER_EXPORT_TRIGGER_AGEOUT_GUARANTEE) {
        profile_flags |= BCMI_FT_GROUP_AGE_OUT_PROFILE_RESERVATION;
    }
    profile_flags |= BCMI_FT_GROUP_AGE_OUT_PROFILE_EXPORT_TRIGGER;

    return bcmi_ft_group_age_profile_entry_set(unit,
            profile_flags, id, enable, -1);
}

/*
 * Function:
 *   bcmi_ft_group_age_export_trigger_get
 * Purpose:
 *   Get export trigger on age-out
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   flags  - (OUT) export trigger flags
 *   enable - (OUT) Enable export trigger for aged flows
 * Returns:
 *   None.
 */
int
bcmi_ft_group_age_export_trigger_get(int unit,
                                     bcm_flowtracker_group_t id,
                                     uint32 *flags,
                                     uint32 *enable)
{
    uint32 index = 0;
    uint32 age_outfmt, val;
    bsc_kg_group_table_entry_t kg_entry;
    bsc_kg_age_out_profile_entry_t entry;

    *flags = *enable = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    /* initialize the format entry. */
    sal_memset(&age_outfmt, 0, sizeof(uint32));
    sal_memset(&entry, 0, sizeof(bsc_kg_age_out_profile_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                                   MEM_BLOCK_ANY, id, &kg_entry));

    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
                     ((uint32 *)&kg_entry), AGE_OUT_PROFILE_IDXf);

    /* Read the profile entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_AGE_OUT_PROFILEm,
                                   MEM_BLOCK_ANY, index, &entry));

    soc_mem_field_get(unit, BSC_KG_AGE_OUT_PROFILEm, (uint32 *)&entry,
                                              DATAf, &age_outfmt);

    *enable = soc_format_field32_get(unit,
                                     BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                                     &age_outfmt, EXPORT_AGE_OUTf);

    val = soc_format_field32_get(unit,
                                 BSC_TL_KG_TO_KT_AGE_CONTROL_BUSfmt,
                                 &age_outfmt, DO_NOT_AGE_WHEN_FIFO_FULLf);
    if (val) {
        *flags = BCM_FLOWTRACKER_EXPORT_TRIGGER_AGEOUT_GUARANTEE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_age_out_set
 * Purpose:
 *   Set the Age out profile on this group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   aging_interval_ms - (IN) Age out internam in ms.
 * Returns:
 *   None.
 */
int
bcmi_ft_group_age_out_set(int unit,
                          bcm_flowtracker_group_t id,
                          uint32 aging_interval_ms)
{
    int val = -1;

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Ageout is not supported on"
                            " Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
    }

    if (bcmi_ft_age_interval_is_invalid(aging_interval_ms, &val)) {
        return BCM_E_PARAM;
    }

    return bcmi_ft_group_age_profile_entry_set(unit,
            BCMI_FT_GROUP_AGE_OUT_PROFILE_AGE_INTERVAL, id, 0, val);
}

/*
 * Function:
 *   bcmi_ft_group_age_out_get
 * Purpose:
 *   get the Age out profile on this group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   aging_interval_ms - (OUT) Age out internam in ms.
 * Returns:
 *   None.
 */
int
bcmi_ft_group_age_out_get(int unit,
                          bcm_flowtracker_group_t id,
                          uint32 *aging_interval_ms)
{
    int val = -1;
    uint32 index = 0;
    uint32 age_outfmt;
    bsc_kg_group_table_entry_t kg_entry;
    bsc_kg_age_out_profile_entry_t entry;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Ageout is not supported on"
                            " Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
    }

    /* initialize the format entry. */
    sal_memset(&age_outfmt, 0, sizeof(uint32));
    sal_memset(&entry, 0, sizeof(bsc_kg_age_out_profile_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                        MEM_BLOCK_ANY, id, &kg_entry));

    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
                    ((uint32 *)&kg_entry), AGE_OUT_PROFILE_IDXf);

    /* Read age-out profile entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_AGE_OUT_PROFILEm,
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

        BCM_IF_ERROR_RETURN
            (bcmi_ft_age_interval_get(val, aging_interval_ms));

    } else {
        *aging_interval_ms = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_flow_exceed_profile_set
 * Purpose:
 *   set flow exceed profile on this group.
 * Parameters:
 *   unit      - (IN) BCM device id.
 *   id        - (IN) Flowtracker gorup id.
 *   kg_entry  - (IN) FT group KG entry.
 *   flags     - (IN) Configurations flags.
 *   value     - (IN) Value for configuration.
 * Returns:
 *   None.
 */
int bcmi_ft_group_flow_exceed_profile_set(int unit,
        bcm_flowtracker_group_t id,
        uint32 *kg_entry,
        uint32 flags,
        uint32 value)
{
    void *entries[1];
    uint32 flow_exceedfmt;
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;
    bsc_kg_group_table_entry_t kg_entry_local;
    bsc_kg_group_table_entry_t *kg_entry_p = NULL;
    bsc_kg_flow_exceed_profile_entry_t entry;

    /*
     * There are three cases here.
     * Add a new profile for a new group.
     * Change the current values for existing group.
     * Delete/set(0) for existing group.
     */

    /* initialize the format entry. */
    sal_memset(&flow_exceedfmt, 0, sizeof(uint32));

    /* Initialize the flow exceed profile entry. */
    sal_memset(&entry, 0, sizeof(bsc_kg_flow_exceed_profile_entry_t));

    /* Initialize Kg entry */
    sal_memset(&kg_entry_local, 0, sizeof(bsc_kg_group_table_entry_t));

    /* set the entry to profile entries. */
    entries[0] = &entry;

    if (kg_entry == NULL) {
        /* First read the group entry. */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                          MEM_BLOCK_ANY, id, &kg_entry_local));
        kg_entry_p = &kg_entry_local;
    } else {
        kg_entry_p = (bsc_kg_group_table_entry_t *) kg_entry;
    }

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
            kg_entry_p, FLOW_EXCEED_PROFILE_IDXf);

    /* Read Flow Exceed profile entry */
    rv = soc_mem_read(unit, BSC_KG_FLOW_EXCEED_PROFILEm,
            MEM_BLOCK_ANY, old_index, &entry);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field_get(unit, BSC_KG_FLOW_EXCEED_PROFILEm,
            (uint32 *) &entry, DATAf, &flow_exceedfmt);

    if (flags & BCMI_FT_GROUP_FLOW_EXCEED_PROFILE_FLOW_LIMIT) {
        soc_format_field32_set(unit, BSC_TL_KG_TO_KT_LIMITS_CONTROL_BUSfmt,
                &flow_exceedfmt, MAX_FLOW_LIMITf, value);
    }
    if (flags & BCMI_FT_GROUP_FLOW_EXCEED_PROFILE_RESERVATION) {
        soc_format_field32_set(unit, BSC_TL_KG_TO_KT_LIMITS_CONTROL_BUSfmt,
                &flow_exceedfmt, DO_NOT_LEARN_WHEN_FIFO_FULLf, value);
    }

    /* Set the entry */
    soc_mem_field_set(unit, BSC_KG_FLOW_EXCEED_PROFILEm, (uint32 *)&entry,
            DATAf, &flow_exceedfmt);

    /* Create teh profile entry for new flow limit profile. */
    rv = bcmi_ft_group_flow_limit_profile_entry_add(unit,
            entries, 1, (uint32 *)&index);
    BCM_IF_ERROR_RETURN(rv);

    if (old_index != index) {
        /* now write this new index into the group. */
        soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm,
                kg_entry_p, FLOW_EXCEED_PROFILE_IDXf, index);
    }

    if (old_index != 0) {
        (void) bcmi_ft_group_flow_limit_profile_entry_delete(unit, old_index);
    }

    if (kg_entry == NULL) {
        rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm,
                MEM_BLOCK_ANY, id, kg_entry_p);
    }

    return rv;
}

int
bcmi_ft_group_flow_limit_set(int unit,
                             bcm_flowtracker_group_t id,
                             uint32 flow_limit)
{
    int rv = BCM_E_NONE;
    int fid_len = 0;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Flow limit is not supported on"
                            " Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
    }

    if(BCMI_FT_GROUP(unit, id)->flags &
            BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
        LOG_VERBOSE(BSL_LS_BCM_FLOWTRACKER,
           (BSL_META_U(unit, "Configuration of flow limit on"
                             " software-managed group is not allowed\n")));
        return BCM_E_CONFIG;
    }

    fid_len = soc_format_field_length(unit,
                   BSC_TL_KG_TO_KT_LIMITS_CONTROL_BUSfmt, MAX_FLOW_LIMITf);

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

    rv = bcmi_ft_group_flow_exceed_profile_set(unit, id, NULL,
            BCMI_FT_GROUP_FLOW_EXCEED_PROFILE_FLOW_LIMIT, flow_limit);

   return rv;
}

/*
 * Function:
 *   bcmi_ft_group_flow_limit_get
 * Purpose:
 *   Get the Age out profile on this group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   flow_limit - (OUT) Age out internam in ms.
 * Returns:
 *   None.
 */
int
bcmi_ft_group_flow_limit_get(int unit,
                             bcm_flowtracker_group_t id,
                             uint32 *flow_limit)
{
    uint32 index = 0;
    uint32 flow_exceedfmt;
    bsc_kg_group_table_entry_t kg_entry;
    bsc_kg_flow_exceed_profile_entry_t entry;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Flow limit is not supported on"
                            " Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
    }

    if(BCMI_FT_GROUP(unit, id)->flags &
            BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
        LOG_VERBOSE(BSL_LS_BCM_FLOWTRACKER,
           (BSL_META_U(unit, "Configuration of flow limit on"
                             " software-managed group is not allowed\n")));
        return BCM_E_CONFIG;
    }

    /* initialize the format entry. */
    sal_memset(&flow_exceedfmt, 0, sizeof(uint32));
    sal_memset(&entry, 0, sizeof(bsc_kg_flow_exceed_profile_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                                       MEM_BLOCK_ANY, id, &kg_entry));

    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
                     ((uint32 *)&kg_entry), FLOW_EXCEED_PROFILE_IDXf);

    if (index) {
        /* First read the group entry. */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, BSC_KG_FLOW_EXCEED_PROFILEm,
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
 *   bcmi_ft_group_profiles_destroy
 * Purpose:
 *   Destroy all the profiles in the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   info - (IN) Copy to collector related information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_profiles_destroy(int unit, bcm_flowtracker_group_t id)
{

    int old_index;
    bsc_kg_group_table_entry_t kg_entry;
    bsc_dg_group_table_entry_t dg_entry;

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                                    MEM_BLOCK_ANY, id, &kg_entry));

    /* Destroy FLOW_EXCEED_PROFILE from the group. */
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
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
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
 *   bcmi_ft_group_collector_copy_set
 * Purpose:
 *   Set copy to collector settings in group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   info - (IN) Copy to collector related information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_collector_copy_set(int unit,
                                 bcm_flowtracker_group_t id,
                                 bcm_flowtracker_collector_copy_info_t info)
{
    soc_mem_t mem;
    void *entries[1];
    uint32 fmt;
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;
    uint32 fid_len = 0;
    bsc_dg_group_table_entry_t dg_entry;
    bsc_dg_group_copy_to_collector_profile_entry_t entry;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Copy to Collector is not supported"
                            " on Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
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
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
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
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_collector_copy_profile_entry_add(unit,
                                            entries, 1, (uint32 *)&index));

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, ((uint32 *)&dg_entry),
                                    COPY_TO_COLLECTOR_PROFILE_IDXf, index);

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    soc_mem_field32_set(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
        BSD_FLEX_FLOW_COUNT_TO_CPUf, 1);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    if (rv || (index == 0)) {
        soc_mem_field32_set(unit, mem,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_FLOW_COUNT_TO_CPUf, 0);
    }

    return rv;

}

/*
 * Function:
 *   bcmi_ft_group_collector_copy_get
 * Purpose:
 *   get copy to collector settings in group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   info - (OUT) Copy to collector related information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_collector_copy_get(int unit,
                                 bcm_flowtracker_group_t id,
                                 bcm_flowtracker_collector_copy_info_t *info)
{
    uint32 fmt;
    int old_index = 0;
    uint32 fid_value = 0;
    bsc_dg_group_table_entry_t dg_entry;
    bsc_dg_group_copy_to_collector_profile_entry_t entry;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Copy to Collector is not supported"
                            " on Flowtracker Group (%d)\n "), id));
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


    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
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
 *   bcmi_ft_group_meter_profile_set
 * Purpose:
 *   Set the Meter setting on this group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   info - (IN) Meter related information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_meter_profile_set(int unit,
                                bcm_flowtracker_group_t id,
                                bcm_flowtracker_meter_info_t info)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    void *entries[1];
    uint32 fmt[2];
    uint32 index = 0;
    int old_index = 0;
    uint32 flags = 0;
    uint32 bucketsize_commit = 0;   /* Bucket size.             */
    uint32 refresh_rate_commit = 0; /* Policer refresh rate.    */
    uint32 granularity_commit = 0;  /* Policer granularity.     */
    int refresh_bitsize;            /* Number of bits for the
                                       refresh rate field.      */
    int bucket_max_bitsize;         /* Number of bits for the
                                       bucket max field.        */
    bsc_dg_group_table_entry_t dg_entry;
    bsc_dg_group_meter_profile_entry_t entry;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Metering is not supported on"
                            " Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
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

    /* Set Flags based on meter refresh rate */
    BCMI_FT_METER_REFRESH_RATE_FLAG_SET(unit, flags);

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
    BCM_IF_ERROR_RETURN(rv);

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                                 MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
               ((uint32 *)&dg_entry), GROUP_METER_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_meter_profile_entry_delete(unit, old_index);
    }

    /* Initialize the group meter profile entry. */
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

    /* Create the profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_meter_profile_entry_add(unit, entries, 1,
                                                        (uint32 *)&index));

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, ((uint32 *)&dg_entry),
                                        GROUP_METER_PROFILE_IDXf, index);

    BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_burst = info.ckbits_burst;
    BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_sec = info.ckbits_sec;

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    soc_mem_field32_set(unit, mem,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                            BSD_FLEX_FLOW_METERf, 1);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    if (rv || (index == 0)) {
        soc_mem_field32_set(unit, mem,
                    (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                                   BSD_FLEX_FLOW_METERf, 0);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_meter_profile_get
 * Purpose:
 *   Get the Meter setting on this group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   info - (OUT) Meter related information.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_meter_profile_get(int unit,
                                bcm_flowtracker_group_t id,
                                bcm_flowtracker_meter_info_t *info)
{
    uint32 fmt[2];
    int old_index = 0;
    uint32 flags = 0;
    uint32 bucket_max = 0;    /* Bucket size.             */
    uint32 refresh_rate = 0;  /* Policer refresh rate.    */
    uint32 granularity = 0;   /* Policer granularity.     */
    bsc_dg_group_table_entry_t dg_entry;
    bsc_dg_group_meter_profile_entry_t entry;

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Metering is not supported on"
                            " Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
    }

    /* initialize the format entry. */
    sal_memset(&fmt, 0, 2 * sizeof(uint32));
    /* Initialize the age out profile entry. */
    sal_memset(&entry, 0, sizeof(bsc_dg_group_meter_profile_entry_t));
    /* Initialize group entry. */
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                                  MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
                 ((uint32 *)&dg_entry), GROUP_METER_PROFILE_IDXf);

    if (!old_index) {
        sal_memset(info, 0, sizeof(bcm_flowtracker_meter_info_t));
        return BCM_E_NONE;
    }

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_METER_PROFILEm,
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
 *   bcmi_ft_group_pdd_to_pde_coversion
 * Purpose:
 *   Set PDD to PDE conversion.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Flowtracker gorup id.
 *   pde_entry - (OUT) PDE entry
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_pdd_to_pde_coversion(int unit,
            bcm_flowtracker_group_t id,
            bsc_dt_pde_profile_entry_t *pde_entry)
{
    soc_mem_t mem;
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

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    /* Timestamp 0. 48 bit. 6 bytes. */
    if (soc_mem_field32_get(unit, mem,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_TIMESTAMP_0f)) {
        end = 1095;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }

    /* Timestamp 1.  48 bit. 6 bytes.*/
    if (soc_mem_field32_get(unit, mem,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_TIMESTAMP_1f)) {
        end = 1047;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }

    /* Timestamp 2.  48 bit. 6 bytes.*/
    if (soc_mem_field32_get(unit, mem,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_TIMESTAMP_2f)) {
        end = 999;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }
    /* Timestamp 3.  48 bit. 6 bytes.*/
    if (soc_mem_field32_get(unit, mem,
            (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_TIMESTAMP_3f)) {
        end = 951;
        for (i=0; i<6; i++) {
            pde[running_pde_index] = (((end) - (i * 8))/8);
            running_pde_index--;
        }
    }


    /* Meter entry.  48 bit. 6 bytes.*/
    if (soc_mem_field32_get(unit, mem,
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
        total_mems = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, alu_load_type);

        if (alu_load_type == bcmiFtAluLoadTypeLoad16) {
            total_mems = TOTAL_GROUP_LOAD16_DATA_NUM;
        } else  if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
            total_mems = TOTAL_GROUP_LOAD8_DATA_NUM;

           if (soc_mem_field32_get(unit, mem,
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
            pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type);
            if (!pdd_info) {
                return BCM_E_INTERNAL;
            }
            if (soc_mem_field32_get(unit, mem,
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

    if (soc_mem_field32_get(unit, mem,
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
 *   bcmi_ft_group_timestamp_trig_info_clear
 * Purpose:
 *   Clear group trigger info
 * Parameters:
 *   unit  - (IN) BCM device id
 *   id    - (IN) Flowtracker group id.
 * Returns:
 *   None.
 * Notes:
 *   ts_triggers are cleared for internally allocated
 *   and reserved by tracking params. These are again set with
 *   validation.
 *   ts_info is cleared for all triggers which are set with
 *   validation.
 */
void
bcmi_ft_group_timestamp_trig_info_clear(int unit,
                                        bcm_flowtracker_group_t id)
{
    int trig = 0;
    uint32 ts_flags = 0;

    ts_flags = (BCMI_FT_TS_INTERNAL_IPAT |
            BCMI_FT_TS_INTERNAL_IPDT |
            BCMI_FT_TS_RESERVED_NEW_LEARN |
            BCMI_FT_TS_RESERVED_FLOW_START |
            BCMI_FT_TS_RESERVED_FLOW_END |
            BCMI_FT_TS_INTERNAL_ASSIGNED_IFT_TYPE |
            BCMI_FT_TS_RESERVED_INGRESS |
            BCMI_FT_TS_RESERVED_EGRESS);

    /* Clear trigger info assigned */
    for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {

        if ((trig == BCMI_FT_GROUP_TS_NEW_LEARN) ||
                (trig == BCMI_FT_GROUP_TS_FLOW_START) ||
                (trig == BCMI_FT_GROUP_TS_FLOW_END) ||
                (trig == BCMI_FT_GROUP_TS_INGRESS) ||
                (trig == BCMI_FT_GROUP_TS_EGRESS)) {
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, trig) = 0;
        }

        if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) & ts_flags) {
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, trig) = 0;
            BCMI_FT_GROUP_TS_TRIGGERS(unit, id) &= ~(1 << trig);
        }

        BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) = 0;
        BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, trig) = 0;
        BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) = bcmiFtTypeNone;
        BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, trig) = 0;
    }
}

/*
 * Function:
 *   bcmi_ft_group_timestamp_trig_info_validate
 * Purpose:
 *   validate group trigger info
 * Parameters:
 *   unit  - (IN) BCM device id
 *   id    - (IN) Flowtracker group id.
 * Returns:
 *   None.
 */
STATIC int
bcmi_ft_group_timestamp_trig_info_validate(int unit,
                                           bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    int trig = 0, index = 0;
    int trig_max_ts_type = 0;
    bcmi_ft_type_t ft_type = bcmiFtTypeNone;
    bcmi_ft_flowchecker_list_t *temp = NULL;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    const char *tlist[] = BCMI_FT_TYPE_STRINGS;
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    int rv_ts_src = BCM_E_NONE;
    uint8 trig_src_err = 0;
    bcm_flowtracker_timestamp_source_t ts_src = 0;
    int ift_l_count = 0, mft_l_count = 0, eft_l_count = 0;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    bcm_flowtracker_tracking_param_type_t param = 0;
#endif
    int res = FALSE;
    uint8 ts_engine_sel = 0;
    uint16 ft_type_bmp = 0;
    uint16 group_ft_type_bmp = 0;
    uint16 check_ft_type_bmp = 0;
    bcm_flowtracker_group_type_t group_type;
    bcm_flowtracker_tracking_param_info_t *param_info = NULL;

    group_type = BCMI_FT_GROUP_TYPE_GET(id);
    BCMI_FT_GROUP_FT_TYPE_BMP_GET(group_type, group_ft_type_bmp);

    /* Clear ts triggers / ts info before validation */
    bcmi_ft_group_timestamp_trig_info_clear(unit, id);

    /* First validate all check event flags
     * associated to group have consistent stage */
    temp = BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id);
    while (temp) {

        /* skip non primary checks */
        if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(temp->flowchecker_id)) {
            temp = temp->next;
            continue;
        }

        /* Retreive check id flags and set ft_type */
        sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
        BCM_IF_ERROR_RETURN
            (bcmi_ft_flowchecker_get(unit, temp->flowchecker_id, &fc_info));
        check_ft_type_bmp = fc_info.check_ft_type_bmp;

        /* Deduce ft_type for check */
        BCMI_FT_GROUP_CHECK_FT_TYPE_GET(group_ft_type_bmp, check_ft_type_bmp, \
                                ft_type_bmp, ft_type);

        /* Validate & Set check event1 flags ft_type */
        if (fc_info.flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1) {
            if (BCMI_FT_TYPE_F_IS_UNIQUE(ft_type_bmp)) {
                BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit,
                            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1, ts_engine_sel);
                BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                if (res == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                    "Timestamp engine do not belong to same flowtracker"\
                                    " type as flowtracker check\n\r")));
                    return BCM_E_CONFIG;
                }

                if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT1) != bcmiFtTypeNone) {
                    if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                                BCMI_FT_GROUP_TS_CHECK_EVENT1) != ft_type) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                           (BSL_META_U(unit, "CheckEvent1 associated to "
                           "flowchecker 0x%08x(stage %s) already associated"
                           " to stage %s "),temp->flowchecker_id,
                           tlist[ft_type],
                           tlist[BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE
                                (unit, id, BCMI_FT_GROUP_TS_CHECK_EVENT1)]));
                        return BCM_E_CONFIG;
                    }
                } else {
                    BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                                BCMI_FT_GROUP_TS_CHECK_EVENT1) = ft_type;
                    BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                                BCMI_FT_GROUP_TS_CHECK_EVENT1) |= ts_engine_sel;
                }
            } else {
                if ((BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                          BCMI_FT_GROUP_TS_CHECK_EVENT1) != bcmiFtTypeNone) &&
                    (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                          BCMI_FT_GROUP_TS_CHECK_EVENT1) != ft_type)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                       (BSL_META_U(unit, "CheckEvent1 associated to "
                           "flowchecker 0x%08x(stage %s) already associated"
                           " to stage %s "),temp->flowchecker_id,
                             tlist[ft_type],
                             tlist[BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE
                                (unit, id, BCMI_FT_GROUP_TS_CHECK_EVENT1)]));
                    return BCM_E_CONFIG;
                }
                BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT1) = ft_type;
            }
        }

         /* Validate & Set check event2 flags ft_type */
        if (fc_info.flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2) {
            if (BCMI_FT_TYPE_F_IS_UNIQUE(ft_type_bmp)) {
                BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit,
                            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2, ts_engine_sel);
                BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                if (res == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                    "Timestamp engine do not belong to same flowtracker"\
                                    " type as flowtracker check\n\r")));
                    return BCM_E_CONFIG;
                }

                if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                         BCMI_FT_GROUP_TS_CHECK_EVENT2) != bcmiFtTypeNone) {
                    if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                                BCMI_FT_GROUP_TS_CHECK_EVENT2) != ft_type) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                           (BSL_META_U(unit, "CheckEvent2 associated to "
                               "flowchecker 0x%08x(stage %s) already associated"
                               " to stage %s "),temp->flowchecker_id,
                                tlist[ft_type],
                                tlist[BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE
                                 (unit, id, BCMI_FT_GROUP_TS_CHECK_EVENT2)]));
                        return BCM_E_CONFIG;
                    }
                } else {
                    BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                                BCMI_FT_GROUP_TS_CHECK_EVENT2) = ft_type;
                    BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                                BCMI_FT_GROUP_TS_CHECK_EVENT2) |= ts_engine_sel;
                }
            } else {
                if ((BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                          BCMI_FT_GROUP_TS_CHECK_EVENT2) != bcmiFtTypeNone) &&
                    (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                          BCMI_FT_GROUP_TS_CHECK_EVENT2) != ft_type)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                       (BSL_META_U(unit, "CheckEvent2 associated to "
                           "flowchecker 0x%08x(stage %s) already associated"
                           " to stage %s "),temp->flowchecker_id,
                            tlist[ft_type],
                            tlist[BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE
                             (unit, id, BCMI_FT_GROUP_TS_CHECK_EVENT2)]));
                    return BCM_E_CONFIG;
                }
                BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT2) = ft_type;
            }
        }

         /* Validate & Set check event3 flags ft_type */
        if (fc_info.flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3) {
            if (BCMI_FT_TYPE_F_IS_UNIQUE(ft_type_bmp)) {
                BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit,
                            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3, ts_engine_sel);
                BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                if (res == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                    "Timestamp engine do not belong to same flowtracker"\
                                    " type as flowtracker check\n\r")));
                    return BCM_E_CONFIG;
                }

                if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                         BCMI_FT_GROUP_TS_CHECK_EVENT3) != bcmiFtTypeNone) {
                    if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                             BCMI_FT_GROUP_TS_CHECK_EVENT3) != ft_type) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                           (BSL_META_U(unit, "CheckEvent3 associated to "
                               "flowchecker 0x%08x(stage %s) already associated"
                               " to stage %s "),temp->flowchecker_id,
                                tlist[ft_type],
                                tlist[BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE
                                 (unit, id, BCMI_FT_GROUP_TS_CHECK_EVENT3)]));
                        return BCM_E_CONFIG;
                    }
                } else {
                    BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT3) = ft_type;
                    BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT3) |= ts_engine_sel;
                }
            } else {
                if ((BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                          BCMI_FT_GROUP_TS_CHECK_EVENT3) != bcmiFtTypeNone) &&
                    (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                          BCMI_FT_GROUP_TS_CHECK_EVENT3) != ft_type)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                       (BSL_META_U(unit, "CheckEvent3 associated to "
                           "flowchecker 0x%08x(stage %s) already associated"
                           " to stage %s "),temp->flowchecker_id,
                            tlist[ft_type],
                            tlist[BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE
                             (unit, id, BCMI_FT_GROUP_TS_CHECK_EVENT3)]));
                    return BCM_E_CONFIG;
                }
                BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                        BCMI_FT_GROUP_TS_CHECK_EVENT3) = ft_type;
            }
        }

         /* Validate & Set check event4 flags ft_type */
        if (fc_info.flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4) {
            if (BCMI_FT_TYPE_F_IS_UNIQUE(ft_type_bmp)) {
                BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit,
                            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4, ts_engine_sel);
                BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                if (res == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                    "Timestamp engine do not belong to same flowtracker"\
                                    " type as flowtracker check\n\r")));
                    return BCM_E_CONFIG;
                }

                if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT4) != bcmiFtTypeNone) {
                    if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                                   BCMI_FT_GROUP_TS_CHECK_EVENT4) != ft_type) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                           (BSL_META_U(unit, "CheckEvent4 associated to "
                               "flowchecker 0x%08x(stage %s) already associated"
                               " to stage %s "),temp->flowchecker_id,
                                tlist[ft_type],
                                tlist[BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE
                                 (unit, id, BCMI_FT_GROUP_TS_CHECK_EVENT4)]));
                        return BCM_E_CONFIG;
                    }
                } else {
                    BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT4) = ft_type;
                    BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT4) |= ts_engine_sel;
                }
            } else {
                if ((BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                          BCMI_FT_GROUP_TS_CHECK_EVENT4) != bcmiFtTypeNone) &&
                    (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                          BCMI_FT_GROUP_TS_CHECK_EVENT4) != ft_type)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                       (BSL_META_U(unit, "CheckEvent4 associated to "
                           "flowchecker 0x%08x(stage %s) already associated"
                           " to stage %s "),temp->flowchecker_id,
                            tlist[ft_type],
                            tlist[BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE
                            (unit, id, BCMI_FT_GROUP_TS_CHECK_EVENT4)]));
                    return BCM_E_CONFIG;
                }
                BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                        BCMI_FT_GROUP_TS_CHECK_EVENT4) = ft_type;
            }
        }

        temp = temp->next;
    }

    /* Set to IFT type if new learn or flow state based timestamp is requested */
    if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) &
            (1 << BCMI_FT_GROUP_TS_NEW_LEARN)) {
        ft_type = bcmiFtTypeIFT;
        if (!(bcmi_ft_group_param_present(unit, id,
                  bcmFlowtrackerTrackingParamTypeTimestampNewLearn, &index))) {
            param_info = &BCMI_FT_GROUP_TRACK_PARAM(unit, id)[index];
            BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, param_info->flags, ts_engine_sel);
            BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
            if (res == FALSE) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                "Timestamp engine do not belong to same flowtracker"\
                                " type as param/check\n\r")));                 \
                return BCM_E_CONFIG;
            }
            if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
                /* Need to reserve TS Engine */
                BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
                BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= (1 << trig);
                BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) |=
                                                BCMI_FT_TS_RESERVED_NEW_LEARN;
            }

            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                    BCMI_FT_GROUP_TS_NEW_LEARN) = ft_type;
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                    BCMI_FT_GROUP_TS_NEW_LEARN);
            BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                    BCMI_FT_GROUP_TS_NEW_LEARN) |= ts_engine_sel;
        }
    }
    if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) &
            (1 << BCMI_FT_GROUP_TS_FLOW_START)) {
        ft_type = bcmiFtTypeIFT;
        if (!(bcmi_ft_group_param_present(unit, id,
                        bcmFlowtrackerTrackingParamTypeTimestampFlowStart, &index))) {
            param_info = &BCMI_FT_GROUP_TRACK_PARAM(unit, id)[index];
            BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, param_info->flags, ts_engine_sel);
            BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
            if (res == FALSE) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                "Timestamp engine do not belong to same flowtracker"\
                                " type as param/check\n\r")));                 \
                return BCM_E_CONFIG;
            }
            if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
                /* Need to reserve TS Engine */
                BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
                BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= (1 << trig);
                BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) |=
                                                BCMI_FT_TS_RESERVED_FLOW_START;
            }
            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                    BCMI_FT_GROUP_TS_FLOW_START) = ft_type;
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                    BCMI_FT_GROUP_TS_FLOW_START);
            BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                    BCMI_FT_GROUP_TS_FLOW_START) |= ts_engine_sel;
        }
    }
    if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) &
            (1 << BCMI_FT_GROUP_TS_FLOW_END)) {
        ft_type = bcmiFtTypeIFT;
        if (!(bcmi_ft_group_param_present(unit, id,
                        bcmFlowtrackerTrackingParamTypeTimestampFlowEnd, &index))) {
            param_info = &BCMI_FT_GROUP_TRACK_PARAM(unit, id)[index];
            BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, param_info->flags, ts_engine_sel);
            BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
            if (res == FALSE) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                "Timestamp engine do not belong to same flowtracker"\
                                " type as param/check\n\r")));                 \
                return BCM_E_CONFIG;
            }

            if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
                /* Need to reserve TS Engine */
                BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
                BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= (1 << trig);
                BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) |=
                                                BCMI_FT_TS_RESERVED_FLOW_END;
            }
            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                    BCMI_FT_GROUP_TS_FLOW_END) = ft_type;
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                    BCMI_FT_GROUP_TS_FLOW_END);
            BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                    BCMI_FT_GROUP_TS_FLOW_END) |= ts_engine_sel;
        }
    }

    /* Set triggers info for Ingress Timestamp */
    if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) &
            (1 << BCMI_FT_GROUP_TS_INGRESS)) {
        ft_type = bcmiFtTypeIFT;
        if (!(bcmi_ft_group_param_present(unit, id,
                  bcmFlowtrackerTrackingParamTypeTimestampIngress, &index))) {
            param_info = &BCMI_FT_GROUP_TRACK_PARAM(unit, id)[index];
            BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, param_info->flags, ts_engine_sel);
            BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
            if (res == FALSE) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                "Timestamp engine do not belong to same flowtracker"\
                                " type as param/check\n\r")));                 \
                return BCM_E_CONFIG;
            }
            if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
                /* Need to reserve TS Engine */
                BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
                BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= (1 << trig);
                BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) |=
                                                BCMI_FT_TS_RESERVED_INGRESS;
            }

            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                    BCMI_FT_GROUP_TS_INGRESS) = ft_type;
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                    BCMI_FT_GROUP_TS_INGRESS);
            BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                    BCMI_FT_GROUP_TS_INGRESS) |= ts_engine_sel;
        }
    }

    /* Set triggers info for egress Timestamp */
    if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) &
            (1 << BCMI_FT_GROUP_TS_EGRESS)) {
        ft_type = bcmiFtTypeEFT;
        if (!(bcmi_ft_group_param_present(unit, id,
                  bcmFlowtrackerTrackingParamTypeTimestampEgress, &index))) {
            param_info = &BCMI_FT_GROUP_TRACK_PARAM(unit, id)[index];
            BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, param_info->flags, ts_engine_sel);
            BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
            if (res == FALSE) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                "Timestamp engine do not belong to same flowtracker"\
                                " type as param/check\n\r")));                 \
                return BCM_E_CONFIG;
            }
            if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
                /* Need to reserve TS Engine */
                BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
                BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= (1 << trig);
                BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) |=
                                                BCMI_FT_TS_RESERVED_EGRESS;
            }

            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                    BCMI_FT_GROUP_TS_EGRESS) = ft_type;
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                    BCMI_FT_GROUP_TS_EGRESS);
            BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id,
                    BCMI_FT_GROUP_TS_EGRESS) |= ts_engine_sel;
        }
    }

    /* Allocate one trigger for IPAT delay */
    if (!(bcmi_ft_group_param_present(unit, id,
                  bcmFlowtrackerTrackingParamTypeIPATDelay, &index))) {
        ft_type = bcmiFtTypeIFT;
        param_info = &BCMI_FT_GROUP_TRACK_PARAM(unit, id)[index];
        BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, param_info->flags, ts_engine_sel);
        BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
        if (res == FALSE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                            "Timestamp engine do not belong to same flowtracker"\
                            " type as param/check\n\r")));                 \
            return BCM_E_CONFIG;
        }

        trig = 0; trig_max_ts_type = BCMI_FT_GROUP_MAX_TS_TYPE;
        if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
            /* Need to reserve TS Engine */
            BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
            trig_max_ts_type = trig;
        }
        for (; trig < trig_max_ts_type; trig++) {
            if ((trig != BCMI_FT_GROUP_TS_CHECK_EVENT1) &&
                    (trig != BCMI_FT_GROUP_TS_CHECK_EVENT2) &&
                    (trig != BCMI_FT_GROUP_TS_CHECK_EVENT3) &&
                    (trig != BCMI_FT_GROUP_TS_CHECK_EVENT4)) {
                continue;
            }
            /* Check if Timestamp trigger is in use */
            if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << trig)) {
                continue;
            }
            /* Check corresponding TS engine belong to same ft type */
            BCMI_FT_TS_GROUP_TRIGGER_TO_TS_ENGINE_SEL(trig, ts_engine_sel);
            BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
            if (res == FALSE) {
                continue;
            }
            break;
        }

        /* Get free timestamp engine for IPAT */
        if (trig == BCMI_FT_GROUP_MAX_TS_TYPE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
               (BSL_META_U(unit, "Cannot allocate internal timestamp trigger "
                                 "for IPAT tracking param.")));
            return BCM_E_RESOURCE;
        }
        BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= (1 << trig);
        BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) = ft_type;
        BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) |=
                                                    BCMI_FT_TS_INTERNAL_IPAT;
        BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id, trig);
        BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, trig) |= ts_engine_sel;
    }

    /* Allocate one trigger for IPDT delay */
    if (!(bcmi_ft_group_param_present(unit,
                id, bcmFlowtrackerTrackingParamTypeIPDTDelay, &index))) {
        ft_type = bcmiFtTypeEFT;
        param_info = &BCMI_FT_GROUP_TRACK_PARAM(unit, id)[index];
        BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, param_info->flags, ts_engine_sel);
        BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
        if (res == FALSE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                            "Timestamp engine do not belong to same flowtracker"\
                            " type as param/check\n\r")));                 \
            return BCM_E_CONFIG;
        }

        trig = 0; trig_max_ts_type = BCMI_FT_GROUP_MAX_TS_TYPE;
        if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
            /* Need to reserve TS Engine */
            BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
            trig_max_ts_type = trig;
        }
        for (; trig < trig_max_ts_type; trig++) {
            if ((trig != BCMI_FT_GROUP_TS_CHECK_EVENT1) &&
                    (trig != BCMI_FT_GROUP_TS_CHECK_EVENT2) &&
                    (trig != BCMI_FT_GROUP_TS_CHECK_EVENT3) &&
                    (trig != BCMI_FT_GROUP_TS_CHECK_EVENT4)) {
                continue;
            }
            if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << trig)) {
                continue;
            }
            /* Check corresponding TS engine belong to same ft type */
            BCMI_FT_TS_GROUP_TRIGGER_TO_TS_ENGINE_SEL(trig, ts_engine_sel);
            BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
            if (res == FALSE) {
                continue;
            }
            break;
        }
        if (trig == BCMI_FT_GROUP_MAX_TS_TYPE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
               (BSL_META_U(unit, "Cannot allocate internal timestamp trigger "
                                 "for IPDT tracking param.")));
            return BCM_E_RESOURCE;
        }
        BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= (1 << trig);
        BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) = ft_type;
        BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) |=
                                                    BCMI_FT_TS_INTERNAL_IPDT;
        BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id, trig);
        BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, trig) |= ts_engine_sel;
    }

    /* Update timestampcheckevents triggers */
    bcmi_ft_group_param_ts_checkevent_trig_set(unit, id);

    /* Go through all triggers and check if any trig is of none type.
     * it means it has been added in group tracking param set
     * but not added through checkers. assign ift by default in such case */
    for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {
        if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << trig)) {
            if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) ==
                                                           bcmiFtTypeNone) {
                BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) =
                                                           bcmiFtTypeIFT;
                BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) |=
                                      BCMI_FT_TS_INTERNAL_ASSIGNED_IFT_TYPE;
                BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, trig) =
                                      BCMI_FT_TS_ENGINE_SELECT_ANY;
            }
        }
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    /* calculate number of resources per stage */
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {
            if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << trig)) {
                /* Skip if triggers is assigned to to below ts types as
                   they are counter individually
                 */
                if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) &
                        (BCMI_FT_TS_RESERVED_NEW_LEARN |
                         BCMI_FT_TS_RESERVED_FLOW_START |
                         BCMI_FT_TS_RESERVED_FLOW_END |
                         BCMI_FT_TS_RESERVED_INGRESS |
                         BCMI_FT_TS_RESERVED_EGRESS)) {
                    continue;
                }
                /* Get tracking param for ts_type */
                bcmi_ft_group_timestamp_trig_param_get(unit, id, trig, &param);

                /* Check if ts src is proper for provided trigger stage */
                rv_ts_src = bcmi_ftv2_group_param_ts_src_get(unit, id,
                                    param, 0, &ts_src);
                if (BCM_SUCCESS(rv_ts_src)) {
                    /* If found, validate src of timestamp */
                    if (ts_src == bcmFlowtrackerTimestampSourcePacket) {
                        if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) !=
                                                                bcmiFtTypeIFT) {
                            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                               (BSL_META_U(unit, "param <%s> has timestamp src as <Pkt>"
                               "and is not associated to IFT stage check which is invalid.\n"),
                                plist[param]));
                            trig_src_err = 1;
                        }
                    }
                    if (ts_src == bcmFlowtrackerTimestampSourceIngress) {
                        if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) !=
                                                                bcmiFtTypeEFT) {
                            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                               (BSL_META_U(unit, "param <%s> has timestamp src as <Ingress>"
                               "and is not associated to EFT stage check which is invalid.\n"),
                                plist[param]));
                            trig_src_err = 1;
                        }
                    }
                }

                /* Count stage count for all trigger */
                if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) ==
                                                               bcmiFtTypeIFT) {
                    ift_l_count++;
                } else if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) ==
                                                               bcmiFtTypeMFT) {
                    mft_l_count++;
                } else if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) ==
                                                               bcmiFtTypeEFT) {
                    eft_l_count++;
                }
            }
        }

        /* validate with max resources allocated in config */
        if ((ift_l_count >
                 BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeIFT)) ||
            (mft_l_count >
                 BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeMFT)) ||
            (eft_l_count >
                 BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeEFT))) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "Timestamp resource exceeds allocated - Requested "
               "(ift<%d>, mft <%d>, eft <%d>) Allocated "
               "(ift<%d>, mft <%d>, eft <%d>).\n"),
                 ift_l_count, mft_l_count, eft_l_count,
                 BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeIFT),
                 BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeMFT),
                 BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, bcmiFtTypeEFT)));
            return BCM_E_RESOURCE;
        }
    }
    if (trig_src_err) {
        rv = BCM_E_PARAM;
    }
#endif

    return rv;
}

int
bcmi_ft_group_ts_engine_select_allowed(int unit,
        bcm_flowtracker_group_t id,
        int ts_trig,
        int ts_data_engine,
        int *allowed)
{
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    uint8 grp_ts_engine_sel = 0;

    grp_ts_engine_sel = BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, ts_trig);

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        *allowed = FALSE;

        switch(ts_data_engine) {
            case 0:
                if ((grp_ts_engine_sel == BCMI_FT_TS_ENGINE_SELECT_ANY) ||
                        (grp_ts_engine_sel == BCMI_FT_TS_ENGINE_SELECT_0)) {
                    *allowed = TRUE;
                }
                break;
            case 1:
                if ((grp_ts_engine_sel == BCMI_FT_TS_ENGINE_SELECT_ANY) ||
                        (grp_ts_engine_sel == BCMI_FT_TS_ENGINE_SELECT_1)) {
                    *allowed = TRUE;
                }
                break;
            case 2:
                if ((grp_ts_engine_sel == BCMI_FT_TS_ENGINE_SELECT_ANY) ||
                        (grp_ts_engine_sel == BCMI_FT_TS_ENGINE_SELECT_2)) {
                    *allowed = TRUE;
                }
                break;
            case 3:
                if ((grp_ts_engine_sel == BCMI_FT_TS_ENGINE_SELECT_ANY) ||
                        (grp_ts_engine_sel == BCMI_FT_TS_ENGINE_SELECT_3)) {
                    *allowed =TRUE;
                }
                break;
            default:
                break;
        }
        if (*allowed == FALSE) {
            return BCM_E_NONE;
        }
        if (BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, ts_trig) !=
                BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, ts_data_engine)) {
            *allowed = FALSE;
            return BCM_E_NONE;
        }
    }
#endif

    /* All checks done */
    *allowed = TRUE;
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_timestamp_entry_set
 * Purpose:
 *   Set Timestamp entry settings in group.
 * Parameters:
 *   unit  - (IN) BCM device id
 *   id    - (IN) Flowtracker gorup id.
 *   entry - (OUT) timestamp profile entry.
 * Returns:
 *   None.
 */
STATIC int
bcmi_ft_group_timestamp_entry_set(int unit,
                    bcm_flowtracker_group_t id,
                    bsc_dg_group_timestamp_profile_entry_t *entry)
{
    int rv = BCM_E_NONE;
    uint32 fmt[1];
    int trig, i = 0, j = 0;
    int allowed;
    int bit_sel = 0;
    soc_mem_t mem = INVALIDm;
    soc_field_t ts_data_fields[TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM] =
        {DATA_0f, DATA_1f, DATA_2f, DATA_3f};
    soc_field_t ts_pdd_fields[TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM] =
        {BSD_FLEX_TIMESTAMP_0f,
         BSD_FLEX_TIMESTAMP_1f,
         BSD_FLEX_TIMESTAMP_2f,
         BSD_FLEX_TIMESTAMP_3f};
    uint16 ts_data_vector = 0;
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    uint32 ts_hw = 0;
    bcm_flowtracker_timestamp_source_t ts_src = 0;
    soc_field_t ts_src_select_fields[TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM] =
        {TS_SELECT_0f, TS_SELECT_1f, TS_SELECT_2f, TS_SELECT_3f};
#endif
    bcm_flowtracker_tracking_param_type_t param = 0;
    int trig_type_order[] = {
        BCMI_FT_GROUP_TS_CHECK_EVENT1, BCMI_FT_GROUP_TS_CHECK_EVENT2,
        BCMI_FT_GROUP_TS_CHECK_EVENT3, BCMI_FT_GROUP_TS_CHECK_EVENT4,
        BCMI_FT_GROUP_TS_NEW_LEARN, BCMI_FT_GROUP_TS_FLOW_START,
        BCMI_FT_GROUP_TS_FLOW_END, BCMI_FT_GROUP_TS_INGRESS,
        BCMI_FT_GROUP_TS_EGRESS
    };

    sal_memset(fmt, 0, sizeof(fmt));

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    /* All the triggers are set in the group structure. */
    for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {
        i = trig_type_order[trig];
        if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << i)) {

            /* Skip if Check Event trig is set due to below flags */
            if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , i) &
                    (BCMI_FT_TS_RESERVED_NEW_LEARN |
                     BCMI_FT_TS_RESERVED_FLOW_START |
                     BCMI_FT_TS_RESERVED_FLOW_END |
                     BCMI_FT_TS_RESERVED_INGRESS |
                     BCMI_FT_TS_RESERVED_EGRESS)) {
                continue;
            }

            /* Get tracking param and KMAP bit for ts_type */
            bcmi_ft_group_timestamp_trig_param_get(unit, id, i, &param);
            bcmi_ft_group_timestamp_trigger_bit_get(unit, id, i, &bit_sel);

            /* Choose trigger in input kmap select first */
            soc_format_field32_set(unit,
                    BSC_KMAP_CONTROLfmt, fmt, KMAP_INPUT_SELECT_0f, bit_sel);
            soc_format_field32_set(unit,
                    BSC_KMAP_CONTROLfmt, fmt, KMAP_INPUT_SELECT_1f, bit_sel);
            soc_format_field32_set(unit,
                    BSC_KMAP_CONTROLfmt, fmt, KMAP_INPUT_SELECT_2f, bit_sel);
            soc_format_field32_set(unit,
                    BSC_KMAP_CONTROLfmt, fmt, KMAP_INPUT_SELECT_3f, bit_sel);

           /* For internal IPAT/IPDT ALU cannot set timestamp
            * hence to trigger timestamp for every packet in flow
            * we are setting kmap control to always true */
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
               ((BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , i) &
                                                BCMI_FT_TS_INTERNAL_IPAT) ||
                (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , i) &
                                                BCMI_FT_TS_INTERNAL_IPDT) ||
                (i == BCMI_FT_GROUP_TS_INGRESS) ||
                (i == BCMI_FT_GROUP_TS_EGRESS))) {
                soc_format_field32_set(unit,
                            BSC_KMAP_CONTROLfmt, fmt, KMAP_CONTROLf, 0xFFFF);
            } else
#endif
            {
                soc_format_field32_set(unit,
                        BSC_KMAP_CONTROLfmt, fmt, KMAP_CONTROLf, 0x8000);
            }

            /* Allocate timestamp hw banks for triggers. */
            for (j = 0; j < TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM; j++) {
                if (!(ts_data_vector & (1 << j))) {
                    allowed = FALSE;
                    rv = bcmi_ft_group_ts_engine_select_allowed(unit,
                            id, i, j, &allowed);
                    BCMI_CLEANUP_IF_ERROR(rv);
                    if (allowed == TRUE) {
                        ts_data_vector |= (1 << j);
                        break;
                    }
                }
            }

            /* defensive check, all triggers should have been
             * validated while adding them through param set or
             * check add api & trigger info validate api */
            if (j == TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM) {
                BCMI_CLEANUP_IF_ERROR(BCM_E_INTERNAL);
            }

            /* update ts bank allocated for trigger */
            BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, i) = j;

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                ts_src = 0;
                (void) bcmi_ftv2_group_param_ts_src_get(unit, id, param,
                                        BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , i),
                                        &ts_src);
                ts_hw = BCMI_FT_GROUP_TS_PROFILE_HW_SOURCE(ts_src);
                soc_mem_field32_set(unit, BSC_DG_GROUP_TIMESTAMP_PROFILEm,
                            (uint32 *)entry, ts_src_select_fields[j], ts_hw);
            }
#endif
            /* Set the entry. */
            soc_mem_field_set(unit, BSC_DG_GROUP_TIMESTAMP_PROFILEm,
                             (uint32 *)entry, ts_data_fields[j], fmt);

            /* set the pdd entry */
            soc_mem_field32_set(unit, mem,
                               (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                                ts_pdd_fields[j], 1);

             /* If the trigger is flow state based then
              * flow state needs to be taken in pdd entry */
            if ((param ==
                    bcmFlowtrackerTrackingParamTypeTimestampFlowStart) ||
                (param ==
                    bcmFlowtrackerTrackingParamTypeTimestampFlowEnd)) {

                /* set the pdd entry */
                soc_mem_field32_set(unit, mem,
                                   (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                                    BSD_FLEX_FLOW_STATEf, 1);
            }
        }
    }

    return BCM_E_NONE;

cleanup:
    fmt[0] = 0;
    for (; i < BCMI_FT_GROUP_MAX_TS_TYPE; i++) {
        if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << i)) {
            BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, i) = 0;
            if ((param ==
                        bcmFlowtrackerTrackingParamTypeTimestampFlowStart) ||
                (param ==
                     bcmFlowtrackerTrackingParamTypeTimestampFlowEnd)) {
                soc_mem_field32_set(unit, mem,
                        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                        BSD_FLEX_FLOW_STATEf, 0);
            }
        }
    }
    for (j = 0; j < TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM; j++) {
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            soc_mem_field32_set(unit, BSC_DG_GROUP_TIMESTAMP_PROFILEm,
                        (uint32 *)entry, ts_src_select_fields[j], 0);
        }
#endif
        soc_mem_field_set(unit, BSC_DG_GROUP_TIMESTAMP_PROFILEm,
                (uint32 *)entry, ts_data_fields[j], fmt);
        soc_mem_field32_set(unit, mem,
                (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                ts_pdd_fields[j], 0);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_timestamp_set
 * Purpose:
 *   Set Timestamp settings in group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker gorup id.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_timestamp_set(int unit,
                            bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    void *entries[1];
    uint32 index = 0;
    int old_index = 0;
    bsc_dg_group_table_entry_t dg_entry;
    bsc_dg_group_timestamp_profile_entry_t entry;

    /* Check if valid group and id range. */
    if (bcmi_ft_group_is_invalid(unit, id)) {
        return (BCM_E_PARAM);
    }

    /* Aggregate Groups do not support timestamp profile*/
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return rv;
    }

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    /* Validate & Fill trigger info */
    rv = bcmi_ft_group_timestamp_trig_info_validate(unit, id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
           (BSL_META_U(unit, "timestamp trigger info validate failure"
                             " for flowtracker group (%d)\n"), id));
        return rv;
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
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
                    ((uint32 *)&dg_entry), TIMESTAMP_PROFILE_IDXf);

    if (old_index) {
        bcmi_ft_group_timestamp_profile_entry_delete(unit, old_index);
    }

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_timestamp_entry_set(unit, id, &entry));

    /* Create the profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_timestamp_profile_entry_add(unit,
                                       entries, 1, (uint32 *)&index));

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, ((uint32 *)&dg_entry),
                                          TIMESTAMP_PROFILE_IDXf, index);

    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &dg_entry);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_param_ts_checkevent_trig_set
 * Purpose:
 *   Update ts_tiggers and ts_info for timestampcheckevent
 *   tracking params which is not mapped to any flowchecks.
 * Parameters:
 *   unit  - (IN) BCM device id
 *   id    - (IN) Flowtracker group id.
 * Returns:
 *   None.
 */
void
bcmi_ft_group_param_ts_checkevent_trig_set(int unit,
                                           bcm_flowtracker_group_t id)
{
    int ts_engine_sel = 0;
    int i = 0, num_param = 0;
    uint32 event_flags = 0, trig = 0;
    bcmi_ft_type_t ft_type = bcmiFtTypeIFT;
    bcm_flowtracker_tracking_param_info_t *t_info = NULL;
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    int index = 0;
#endif

    t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    num_param = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);

    for (i = 0; i < num_param; i++, t_info++) {
        switch(t_info->param) {
            case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
                trig = BCMI_FT_GROUP_TS_CHECK_EVENT1;
                event_flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
                ft_type = bcmiFtTypeIFT;
#if defined(BCM_FLOWTACKER_V2_SUPPORT)
                index = 0;
#endif
                break;
            case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
                trig = BCMI_FT_GROUP_TS_CHECK_EVENT2;
                event_flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2;
                ft_type = bcmiFtTypeIFT;
#if defined(BCM_FLOWTACKER_V2_SUPPORT)
                index = 1;
#endif
                break;
            case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
                trig = BCMI_FT_GROUP_TS_CHECK_EVENT3;
                event_flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3;
                ft_type = bcmiFtTypeIFT;
#if defined(BCM_FLOWTACKER_V2_SUPPORT)
                index = 2;
#endif
                break;
            case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:
                trig = BCMI_FT_GROUP_TS_CHECK_EVENT4;
                event_flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4;
                ft_type = bcmiFtTypeIFT;
#if defined(BCM_FLOWTACKER_V2_SUPPORT)
                index = 3;
#endif
                break;
            default:
                continue;
        }
        if (!(BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << trig))) {
            BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit, event_flags, ts_engine_sel);
            BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, trig) = ts_engine_sel;
            BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= (1 << trig);
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
            ft_type = BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, index);
#endif
            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) = ft_type;
        }
    }
}

/*
 * Function:
 *   bcmi_ft_group_timestamp_trigger_bit_get
 * Purpose:
 *   Get trigger bit used by the KMAP for given timestamp
 *   trigger type of a group.
 * Parameters:
 *   unit  - (IN) BCM device id
 *   id    - (IN) Flowtracker group id.
 *   ts_type - (IN) Timestamp trigger type
 *   bit_sel - (OUT) bit for trigger type
 * Returns:
 *   None.
 */
int
bcmi_ft_group_timestamp_trigger_bit_get(int unit,
    bcm_flowtracker_group_t id,
    int ts_type,
    int *bit_sel)
{
    int idx = 0, ts_bank = 0;
    bcmi_ft_group_trigger_info_t *ts_info = NULL;
    int ts_trigger_bit_sel_array[][2] = {
        {BCMI_FT_GROUP_TS_NEW_LEARN, 4},
        {BCMI_FT_GROUP_TS_FLOW_START, 0},
        {BCMI_FT_GROUP_TS_FLOW_END, 1},
        {BCMI_FT_GROUP_TS_CHECK_EVENT1, 2},
        {BCMI_FT_GROUP_TS_CHECK_EVENT2, 3},
        {BCMI_FT_GROUP_TS_CHECK_EVENT3, 5},
        {BCMI_FT_GROUP_TS_CHECK_EVENT4, 6}
    };

    /* Map Ingress/Egress to CheckEvents */
    ts_info = BCMI_FT_GROUP_TS_TRIGGER_INFO(unit, id);
    if ((ts_type == BCMI_FT_GROUP_TS_INGRESS) ||
        (ts_type == BCMI_FT_GROUP_TS_EGRESS)) {
        ts_bank = ts_info[ts_type].ts_bank;
        ts_type = ts_bank + BCMI_FT_GROUP_TS_CHECK_EVENT1;
    }

    for (idx = 0; idx < COUNTOF(ts_trigger_bit_sel_array); idx++) {
        if (ts_trigger_bit_sel_array[idx][0] == ts_type) {
            *bit_sel = ts_trigger_bit_sel_array[idx][1];
            break;
        }
    }

    return BCM_E_NONE;
}

#else /* BCM_FLOWTRACKER_SUPPORT*/
typedef int bcmi_ft_group_profiles_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT */
