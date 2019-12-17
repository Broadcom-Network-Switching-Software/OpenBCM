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

#if defined(BCM_FLOWTRACKER_SUPPORT)
/*
 * Global defines.
 */

extern int ft_initialized[BCM_MAX_NUM_UNITS];

bcmi_ft_group_flush_info_t
    bcmi_ft_group_flush_fields[BCMI_FT_GROUP_FLUSH_MAX_SCALE] =
    {{GROUP_ID0_VALIDf, GROUP_ID0f},
     {GROUP_ID1_VALIDf, GROUP_ID1f},
     {GROUP_ID2_VALIDf, GROUP_ID2f},
     {GROUP_ID3_VALIDf, GROUP_ID3f}};

/* Internal Database for timestamp management. */
bcmi_ft_group_ts_t bcmi_ft_group_ts_info[BCMI_FT_GROUP_MAX_TS_TYPE] =
    {{bcmFlowtrackerTrackingParamTypeTimestampNewLearn,
        BCMI_FT_GROUP_TS_NEW_LEARN, 4, 0},
     {bcmFlowtrackerTrackingParamTypeTimestampFlowStart,
        BCMI_FT_GROUP_TS_FLOW_START, 0, 0},
     {bcmFlowtrackerTrackingParamTypeTimestampFlowEnd,
        BCMI_FT_GROUP_TS_FLOW_END, 1, 0},
     {bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1,
        BCMI_FT_GROUP_TS_CHECK_EVENT1, 2, 0},
     {bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2,
        BCMI_FT_GROUP_TS_CHECK_EVENT2, 3, 0}};

/* Group Sw state. */
bcmi_ft_group_sw_info_t **bcmi_ft_group_sw_state[BCM_MAX_NUM_UNITS];

/* Group bitmap. */
bcmi_ft_group_bitmap_t bcmi_ft_group_bitmap[BCM_MAX_NUM_UNITS];


/* FT group Internal methods. */

/* clear the Flowtracker subsystem. */
/*
 * Function:
 *     bcmi_ft_group_state_clear
 * Purpose:
 *     Clear Flowtracker group state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     None.
 */

void
bcmi_ft_group_state_clear(int unit)
{

    if (bcmi_ft_group_sw_state[unit] != NULL) {
        sal_free(bcmi_ft_group_sw_state[unit]);
        bcmi_ft_group_sw_state[unit] = NULL;
    }

    if (bcmi_ft_group_bitmap[unit].ft_group_bitmap != NULL) {
        sal_free(bcmi_ft_group_bitmap[unit].ft_group_bitmap);
        bcmi_ft_group_bitmap[unit].ft_group_bitmap = NULL;
    }

    return;
}

/*
 * Function:
 *     bcmi_ft_group_state_init
 * Purpose:
 *     Initialize Flowtracker group state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
 */
int bcmi_ft_group_state_init(int unit)
{

    int num_groups = 0;
    soc_mem_t mem;

    mem = BSC_KG_GROUP_TABLEm;

    /* total number of groups. */
    num_groups = soc_mem_index_count(unit, mem);

    /* Allocate group software state. */
    bcmi_ft_group_sw_state[unit] = (bcmi_ft_group_sw_info_t **)
        sal_alloc((num_groups *
            sizeof(bcmi_ft_group_sw_info_t *)),
        "flowtracker group sw state");

    if (bcmi_ft_group_sw_state[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(bcmi_ft_group_sw_state[unit], 0,
        (num_groups * sizeof(bcmi_ft_group_sw_info_t *)));

    /* Allocate Group bitmap space for all the groups. */
    BCMI_FT_GROUP_BITMAP(unit) =
        sal_alloc(SHR_BITALLOCSIZE(num_groups),"ft_group_bitmap");

    if (bcmi_ft_group_bitmap[unit].ft_group_bitmap == NULL) {
        bcmi_ft_group_state_clear(unit);
    }

    sal_memset(bcmi_ft_group_bitmap[unit].ft_group_bitmap, 0,
               SHR_BITALLOCSIZE(num_groups));

    /*
     * Each group structure is not allocated here.
     * That will be allocated when each group is created.
     */

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_get_free_index
 * Purpose:
 *     get free index for the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (OUT) FT group id.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_get_free_index(int unit, int *index)
{

    int num_groups = 0;
    int i = 0;

    soc_mem_t mem;
    mem = BSC_KG_GROUP_TABLEm;

    /* Total number of groups in device. */
    num_groups = soc_mem_index_count(unit, mem);

    if (*index != -1) {
        /*
         * if valid index is sent, then use it.
         * Index free check is already done.
         */
        i = *index;
    }

    for (; i<num_groups; i++) {
        if (!(SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit), i))) {
            /* Got the free index. use it. */
            SHR_BITSET(BCMI_FT_GROUP_BITMAP(unit), i);
            *index = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *     bcmi_ft_group_is_invalid
 * Purpose:
 *     Check if the group is invalid.
 * Parameters:
 *     unit - (IN) BCM device id
 *     group_index   - (OUT) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_is_invalid(
            int unit,
            bcm_flowtracker_group_t group_index)
{
    int num_groups = 0;

    num_groups = soc_mem_index_max(unit, BSC_KG_GROUP_TABLEm);

    if (!ft_initialized[unit]) {
        return BCM_E_INIT;
    }

    if (group_index > num_groups) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_GROUP_BITMAP(unit) == NULL) {
        return BCM_E_INIT;
    }

    if (bcmi_ft_group_sw_state[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (!(SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit), group_index))) {
        return BCM_E_NOT_FOUND;
    }

    if (BCMI_FT_GROUP(unit, group_index) == NULL) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_collector_is_valid
 * Purpose:
 *     Check if collector is associated with ft group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_collector_is_valid(
            int unit,
            bcm_flowtracker_group_t id)
{
    if (!ft_initialized[unit]) {
        return BCM_E_INIT;
    }

    if (BCMI_FT_GROUP(unit, id)->collector_id == BCMI_FT_COLLECTOR_ID_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_create
 * Purpose:
 *     Create flowtracker group id.
 * Description :
 *     This function only creates a sw group index
 *     and allocates sw memory to store groups information.
 * Parameters:
 *     unit - (IN) BCM device id
 *     options - (IN) options for group creation
 *     id   - (OUT) FT group id.
 *     flow_group_info - (IN) Information of group.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_create(
    int unit,
    uint32 options,
    bcm_flowtracker_group_t *id,
    bcm_flowtracker_group_info_t *flow_group_info)
{
    int group_index = -1;
    int rv = BCM_E_NONE;
    int create = 1;
    bcm_flowtracker_group_stat_t group_stats;

    if (id == NULL) {
        return BCM_E_PARAM;
    }

    if (flow_group_info == NULL) {
        return BCM_E_PARAM;
    }

    if (!ft_initialized[unit]) {
        return BCM_E_INIT;
    }

    if ((options & BCM_FLOWTRACKER_GROUP_REPLACE) &&
       (!(options & BCM_FLOWTRACKER_GROUP_WITH_ID))) {
        return BCM_E_PARAM;
    }

    if (options & BCM_FLOWTRACKER_GROUP_REPLACE) {
        group_index = *id;

        if (group_index > soc_mem_index_max(unit, BSC_KG_GROUP_TABLEm)) {
            return BCM_E_PARAM;
        }

        /* Check if valid group and id range. */
        if (bcmi_ft_group_is_invalid(unit, group_index)) {
            return (BCM_E_NOT_FOUND);
        }

        if (BCMI_FT_GROUP(unit, group_index)) {
            if (BCMI_FT_GROUP_IS_BUSY(unit, group_index)) {
                return BCM_E_CONFIG;
            }
        }

    } else {
        if (options & BCM_FLOWTRACKER_GROUP_WITH_ID) {

            group_index = *id;

            /* Check if valid group and id range. */
            if (!bcmi_ft_group_is_invalid(unit, group_index)) {
                return (BCM_E_EXISTS);
            }

        }

        /*  Allocating a new group. Get a free group index. */
        BCM_IF_ERROR_RETURN(bcmi_ft_group_get_free_index(unit,
            &group_index));

        /* Allocate the sw state for this group first. */
        BCMI_FT_GROUP(unit, group_index) = (bcmi_ft_group_sw_info_t *)
            sal_alloc(sizeof(bcmi_ft_group_sw_info_t),
            "flowtracker group sw state");

        if (BCMI_FT_GROUP(unit, group_index) == NULL) {
            return BCM_E_MEMORY;
        }

        /* memset everything to zero. */
        sal_memset(BCMI_FT_GROUP(unit, group_index), 0,
            sizeof(bcmi_ft_group_sw_info_t));

    }

    /* Write group related information in hw tables.*/
    rv = bcmi_ft_group_entry_update(unit, group_index, flow_group_info, create);

    CLEANUP_ON_ERROR(rv)

    /* Set by default direction of group as forward. */
    bcmi_flowtracker_group_control_set(unit,group_index,
        bcmFlowtrackerGroupControlFlowDirection,
        bcmFlowtrackerFlowDirectionForward);

    /* Set by default, flowtrack and new learn control */
    BCMI_FT_GROUP_FTFP_DATA(unit, group_index).new_learn = TRUE;
    BCMI_FT_GROUP_FTFP_DATA(unit, group_index).flowtrack = TRUE;

    /* Copy flags. */
    (BCMI_FT_GROUP(unit, group_index))->flags = flow_group_info->group_flags;
    if (flow_group_info->group_flags & BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
        BCMI_FT_GROUP_FTFP_DATA(unit, group_index).new_learn = 0;
    }

    /* Assign invalid ids at group creation. */
    (BCMI_FT_GROUP(unit, group_index))->template_id = BCMI_FT_TEMPLATE_ID_INVALID;
    (BCMI_FT_GROUP(unit, group_index))->collector_id = BCMI_FT_COLLECTOR_ID_INVALID;

    /* Clear up old stats */
    bcm_flowtracker_group_stat_t_init(&group_stats);
    rv = bcm_esw_flowtracker_group_stat_set(unit, group_index, &group_stats);
    CLEANUP_ON_ERROR(rv);

    *id = group_index;

    return BCM_E_NONE;

cleanup:
    bcmi_ft_group_clear(unit, group_index);

    return rv;
}

/*
 * Function:
 *     bcmi_ft_group_create
 * Purpose:
 *     Create flowtracker group id.
 * Description :
 *     This function only creates a sw group index
 *     and allocates sw memory to store groups information.
 * Parameters:
 *     unit - (IN) BCM device id
 *     options - (IN) options for group creation
 *     id   - (OUT) FT group id.
 *     flow_group_info - (IN) Information of group.
 *
 * Returns:
 *     BCM_E_XXX
 */

int
bcmi_ft_group_entry_update(int unit, bcm_flowtracker_group_t group_id,
                           bcm_flowtracker_group_info_t *info,
                           int create)
{
    int rv = BCM_E_NONE;
    int fid_len = 0;
    bsc_kg_group_table_entry_t kg_group_entry;
    bsc_dg_group_table_entry_t dg_group_entry;

    sal_memset(&kg_group_entry, 0, sizeof(bsc_kg_group_table_entry_t));
    sal_memset(&dg_group_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    if (create) {

        if (!info) {
            return BCM_E_PARAM;
        }
        fid_len = soc_mem_field_length(unit, BSC_DG_GROUP_TABLEm, CLASS_IDf);

        if (info->group_class > ((1 << fid_len) -1)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_group_entry,
                        CLASS_IDf, info->group_class);

        soc_mem_field32_set(unit, BSC_DG_GROUP_TABLEm, &dg_group_entry,
                        GROUP_VALIDf, 1);

        soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, &kg_group_entry,
                        GROUP_VALIDf, 1);
    }

    /* Write into hardware memory. */
    rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ALL,
                       group_id, &kg_group_entry);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Write into the hardware. */
    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ALL,
                       group_id, &dg_group_entry);

    return rv;
}

/*
 * Function:
 *     bcmi_ft_group_get
 * Purpose:
 *     Get information of flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     flow_group_info - (IN) Information of group.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_info_t *flow_group_info)
{

    bsc_dg_group_table_entry_t dg_group_entry;


    if (flow_group_info == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    sal_memset(&dg_group_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &dg_group_entry));

    flow_group_info->group_class =
        soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm, &dg_group_entry,
                        CLASS_IDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_clear
 * Purpose:
 *     Clear flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (OUT) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_clear(
        int unit,
        bcm_flowtracker_group_t id)
{

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    /* Make sure that extraction info was cleared. */
    if (BCMI_FT_GROUP_EXT_DATA_INFO(unit, id) != NULL) {
        sal_free(BCMI_FT_GROUP_EXT_DATA_INFO(unit, id));
        BCMI_FT_GROUP_EXT_DATA_INFO(unit, id) = NULL;
    }

    if (BCMI_FT_GROUP_TRACK_PARAM(unit, id) != NULL) {
        sal_free(BCMI_FT_GROUP_TRACK_PARAM(unit, id));
        BCMI_FT_GROUP_TRACK_PARAM(unit, id) = NULL;
    }

    if (BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key != NULL) {
        sal_free(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key);
        BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key = NULL;
    }

    if (BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data != NULL) {
        sal_free(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data);
        BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data = NULL;
    }


    /* Clear the sw state. */
    sal_free(BCMI_FT_GROUP(unit, id));
    BCMI_FT_GROUP(unit, id) = NULL;

    /* Free the bitmap. */
    SHR_BITCLR(BCMI_FT_GROUP_BITMAP(unit), id);


    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_destroy
 * Purpose:
 *     Destroy flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (OUT) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_destroy(
        int unit,
        bcm_flowtracker_group_t id)
{

    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Trying to delete flow Group (%d) while it is installed.\n "), id));

        return BCM_E_BUSY;
    }

    if (BCMI_FT_GROUP_CHECK_LIST(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Deleting Flow Group (%d) while check is attached.\n "), id));
        return BCM_E_BUSY;
    }

    /* Check that collector is detached from the group. This check
     * also make sure that user entries, in case of software managed Group
     * are also removed
     */
    if (BCMI_FT_COLLECTOR_ID_INVALID !=
            BCMI_FT_GROUP(unit, id)->collector_id) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Deleting Flow Group (%d) while collector is"
                        " attached.\n "), id));
        return BCM_E_BUSY;
    }

    /* Uninstall the group */
    if (BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
        BCM_IF_ERROR_RETURN(bcmi_esw_ft_group_hw_uninstall(unit, id));
    }

    rv = bcmi_ft_group_profiles_destroy(unit, id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = bcmi_ft_group_entry_update(unit, id, NULL, 0);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    bcmi_ft_group_clear(unit, id);

    return rv;
}

/*
 * Function:
 *      bcmi_ft_group_get_all
 * Purpose:
 *      Returns the list of flowtracker group created in the system.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      max_size            - (IN) Size of array 'idz'
 *      idz                 - (OUT) Array of flowtracker group.
 *      list_size           - (OUT) Numbe of indices filled in 'idz'
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_get_all(
    int unit,
    int max_size,
    bcm_flowtracker_group_t *idz,
    int *list_size)
{
    int max_avail_groups = 0;
    int i = 0, j = 0;
    soc_mem_t mem;

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if ((max_size != 0) && (idz == NULL)) {
        return BCM_E_PARAM;
    }

    if (list_size == NULL) {
        return BCM_E_PARAM;
    }

    /* Total number of groups in device. */
    mem = BSC_KG_GROUP_TABLEm;
    max_avail_groups = soc_mem_index_count(unit, mem);

    SHR_BITCOUNT_RANGE(BCMI_FT_GROUP_BITMAP(unit),
            *list_size, 0, max_avail_groups);

    if (max_size == 0) {
        return BCM_E_NONE;
    }

    for (i = 0, j = 0; (i < max_avail_groups) && (j < max_size); i++) {
        if (SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit), i)) {
            idz[j] = i;
            j++;
        }
    }
    *list_size = j;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_tracking_param_set
 * Purpose:
 *     Set tracking parameters for the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (OUT) FT group id.
 *     num_tracking_elements - (IN) number of tracking params.
 *     list_of_tracking_elements - (IN) list of tracking params.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_tracking_param_set(
    int unit,
    bcm_flowtracker_group_t id,
    int num_tracking_elements,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements)
{

    int rv = BCM_E_NONE;

    /*
     * This function does nothing but it just sets the tracking
     * parameters in the sw state.
     */

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    if ((num_tracking_elements < 0) || (list_of_tracking_elements == NULL)) {
        return BCM_E_PARAM;
    }

    /* Clear the existing tracking params. */
    if (BCMI_FT_GROUP_TRACK_PARAM(unit, id)) {
        sal_free(BCMI_FT_GROUP_TRACK_PARAM(unit, id));
        BCMI_FT_GROUP_TRACK_PARAM(unit, id) = NULL;
    }

    if (num_tracking_elements == 0) {
        /* User wants to clear the tracking params. */
        return BCM_E_NONE;
    }

    /* Check if the list elements are valid. */
    rv = bcmi_ft_group_param_element_validate(unit,
                                              id,
                                              num_tracking_elements,
                                              list_of_tracking_elements);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCMI_FT_GROUP_TRACK_PARAM(unit, id) =
        (bcm_flowtracker_tracking_param_info_t *)
        sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                   num_tracking_elements),
        "flowtracker group tracking parameters");

    if (BCMI_FT_GROUP_TRACK_PARAM(unit, id) == NULL) {
        return BCM_E_MEMORY;
    }

    /* Update sw state with the tracking parameters. */
    (BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id)) = num_tracking_elements;

    sal_memcpy((BCMI_FT_GROUP_TRACK_PARAM(unit, id)),
                list_of_tracking_elements,
                (sizeof(bcm_flowtracker_tracking_param_info_t) *
                 num_tracking_elements));

    /* Set tracking param based triggers. last param is 1. */
    bcmi_ft_group_ts_triggers_set(unit, id, 0, BCMI_FT_TS_TYPE_TP,
        bcmFlowtrackerFlowCheckTsSet);

    return rv;
}

/*
 * Function:
 *     bcmi_ft_group_param_element_validate
 * Purpose:
 *     Set tracking parameters for the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (OUT) FT group id.
 *     num_tracking_elements - (IN) number of tracking params.
 *     list_of_tracking_elements - (IN) list of tracking params.
 *
 * Returns:
 *     BCM_E_XXX
 */

int
bcmi_ft_group_param_element_validate(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int num_tracking_elements,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements)
{
    int rv = BCM_E_NONE;
    int iter = 0;
    int ts_count = 0;
    bcm_flowtracker_tracking_param_info_t element_info;

    for (iter = 0; iter < num_tracking_elements; iter++) {
        sal_memcpy((void *)(&element_info),
                   (void *)(&(list_of_tracking_elements[iter])),
                   sizeof(bcm_flowtracker_tracking_param_info_t));

        if (element_info.param >= bcmFlowtrackerTrackingParamTypeCount) {
            return BCM_E_PARAM;
        }

        if (bcmi_ft_tracking_param_is_timestamp(unit, element_info.param)) {
            ts_count++;
        }

        if (ts_count > BCMI_FT_GROUP_MAX_TS) {

            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Maximum timestamps allowed on a group"
                                " are only = %d\n"), BCMI_FT_GROUP_MAX_TS));
            return BCM_E_CONFIG;
        }

        /* Tracking Param Flowtracker Check is validated. */
        if (element_info.param == bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
            rv = bcmi_ft_group_check_validate(unit,
                        BCMI_FT_GROUP_CHECK_MATCH_PRIMARY,
                        flow_group_id, element_info.check_id);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* VNID is not supported in tracking param data as we can not export it currently. */
        if ((element_info.flags != BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) &&
            ((element_info.param == bcmFlowtrackerTrackingParamTypeVxlanNetworkId) ||
            (element_info.param == bcmFlowtrackerTrackingParamTypeTunnelClass))) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_ft_group_tracking_param_orde_get
 * Purpose:
 *     Get tracking parameters for the group.
 *     The tracking params can be in order how user
 *     has sent the data or internal reordered data based on
 *     template.
 * Parameters:
 *     unit - (IN) BCM device id
 *     order - (IN) ordered elements based on template.
 *     id   - (OUT) FT group id.
 *     max_size - max size that user asked for.
 *     list_of_tracking_elements - (IN) list of tracking params.
 *     num_tracking_elements - (IN) number of tracking params.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_group_tracking_param_order_get(
        int unit,
        int order,
        bcm_flowtracker_group_t id,
        int max_size,
        bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements,
        int *num_tracking_elements)
{

    if (num_tracking_elements == NULL) {
        return BCM_E_PARAM;
    }


    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    /* If the max_alu_info is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if ((max_size != 0) && (list_of_tracking_elements == NULL)) {
        return BCM_E_PARAM;
    }

    if (order) {
        *num_tracking_elements = BCMI_FT_GROUP_ORDERED_TRACK_PARAM_NUM(unit, id);
    } else {
        *num_tracking_elements = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
    }

    /* If max_alu_info == 0,
     * return the number of export elements in list_size
     * variable.
     */
    if (max_size == 0 && list_of_tracking_elements == NULL) {

        return BCM_E_NONE;
    }

    /* Pick the number of elements to send back. */
    *num_tracking_elements = (*num_tracking_elements < max_size) ?
                              (*num_tracking_elements) : max_size;


    if (order) {
        sal_memcpy(list_of_tracking_elements,
               BCMI_FT_GROUP_TRACK_PARAM(unit, id),
               (sizeof(bcm_flowtracker_tracking_param_info_t) *
               (*num_tracking_elements)));
    } else {
        sal_memcpy(list_of_tracking_elements,
               BCMI_FT_GROUP_TRACK_PARAM(unit, id),
               (sizeof(bcm_flowtracker_tracking_param_info_t) *
               (*num_tracking_elements)));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_ft_group_tracking_param_get
 * Purpose:
 *     Get tracking parameters for the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (OUT) FT group id.
 *     max_size - max size that user asked for.
 *     list_of_tracking_elements - (IN) list of tracking params.
 *     num_tracking_elements - (IN) number of tracking params.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_tracking_param_get(
        int unit,
        bcm_flowtracker_group_t id,
        int max_size,
        bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements,
        int *num_tracking_elements)

{

    BCM_IF_ERROR_RETURN(bcmi_group_tracking_param_order_get(unit, 0,
        id, max_size, list_of_tracking_elements, num_tracking_elements));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_param_operate
 * Purpose:
 *     Operate group state parameters.
 * Parameters:
 *     unit - (IN) BCM device id
 *     value - (IN) data Value of the update
 *     opr   - (IN) operation to be performed on data.
 *     arg  - (IN) argument value.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_param_operate(int unit,
                           uint32 *value,
                           bcmi_ft_group_params_operation_t opr,
                           int arg)
{


    if (opr >= bcmiFtGroupParamOprCount) {
        return BCM_E_PARAM;
    }

    switch (opr) {

        case bcmiFtGroupParamsOprInc:
            *value += arg;
        break;

        case bcmiFtGroupParamsOprDec:
            *value -= arg;
        break;

        case bcmiFtGroupParamsOprUpdate:
            *value = arg;
        break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_param_update
 * Purpose:
 *     Update group state parameters.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     param - (IN) Group's parameters.
 *     opr   - (IN) operation to be performed on data.
 *     arg  - (IN) argument value.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_param_update(int unit,
                           bcm_flowtracker_group_t id,
                           bcmi_ft_group_params_t param,
                           bcmi_ft_group_params_operation_t opr,
                           int arg)
{


   /* First check if group is valid. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    switch (param) {

    case bcmiFtGroupParamFtfpEntryNum:
        bcmi_ft_group_param_operate(unit,
            (&(BCMI_FT_GROUP(unit, id)->num_ftfp_entries)), opr, arg);
    break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_param_retrieve
 * Purpose:
 *     Get group state parameters.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     param - (IN) Group's parameters.
 *     arg  - (OUT) argument value.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_param_retrieve(int unit,
                        bcm_flowtracker_group_t id,
                        bcmi_ft_group_params_t param,
                        int *arg)
{

   /* First check if group is valid. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    switch (param) {

    case bcmiFtGroupParamFtfpEntryNum:
        *arg = BCMI_FT_GROUP(unit, id)->num_ftfp_entries;
    break;

    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_ft_group_32bit_to_16_bit_param_conversion
 * Purpose:
 *     BCM56370 can not store more than 16 bits in PDD.
 *     therefore some 32 parameters may need split to perform
 *     various checks and export.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info  - (IN) list if ALU/load information.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_32bit_to_16_bit_param_conversion(
               int unit,
               bcm_flowtracker_group_t id,
               bcmi_ft_group_alu_info_t **info)
{

    bcmi_ft_group_alu_info_t *temp;
    int num_alus_loads = 0, new_num_alus_loads = 0;
    bcmi_ft_group_alu_info_t alu1, alu2;
    bcmi_ft_group_alu_info_t *new_alloc = NULL;
    int i = 0;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    /* Get total number of alus in this chunk. */
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    if (!num_alus_loads) {
        return BCM_E_NONE;
    }

    if (*info == NULL) {
        return BCM_E_NONE;
    }

    temp = *info;

    sal_memset(&alu1, 0, sizeof(bcmi_ft_group_alu_info_t));
    sal_memset(&alu2, 0, sizeof(bcmi_ft_group_alu_info_t));
    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

    /* allocate the bitmap based on the type, length */
    for (i=0; i<num_alus_loads; i++) {
        /*temp = (&((*info)[i]));*/
        temp = *info;
        if (BCMI_FT_IDX_IS_CHECK(temp[i].flowchecker_id)) {

            BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
                (unit, temp[i].flowchecker_id, &fc_info));
        }

        /* If check has update DoS attack || if exporting dos attack as data. */
        if ((((temp[i].action_element_type == bcmFlowtrackerTrackingParamTypeDosAttack) ||
             (temp[i].action_element_type == bcmFlowtrackerTrackingParamTypeInnerDosAttack)) &&
            (fc_info.action_info.action == bcmFlowtrackerCheckActionUpdateValue)) ||
            ((temp[i].key1.is_alu == 0) &&
            ((temp[i].element_type1 == bcmFlowtrackerTrackingParamTypeDosAttack) ||
            (temp[i].element_type1 == bcmFlowtrackerTrackingParamTypeInnerDosAttack)))) {

            /* The length sent is 32 bits but we need to allocate 16 bits for tracking.*/
            new_num_alus_loads = num_alus_loads + 1;

            /* allocate space needed to save this state in group. */
            new_alloc = (bcmi_ft_group_alu_info_t *) sal_alloc((new_num_alus_loads) *
                sizeof(bcmi_ft_group_alu_info_t), "Group alu info.");
            /* Copy state till last element. */
            sal_memcpy(new_alloc, temp, i * sizeof(bcmi_ft_group_alu_info_t));

            /* Now break this element into 2 elements. */
            sal_memcpy(&alu1, &temp[i], sizeof(bcmi_ft_group_alu_info_t));
            sal_memcpy(&alu2, &temp[i], sizeof(bcmi_ft_group_alu_info_t));

            /* Now change the ALUs from 32 bit to 16 bit. */
            alu1.key1.length = 16;
            alu2.key1.length = 16;
            alu1.key1.location = temp[i].key1.location;
            alu2.key1.location = temp[i].key1.location+16;

            if (temp[i].key1.is_alu) {
                sal_memcpy(&alu1.key2, &alu1.key1, sizeof(bcmi_ft_alu_key_t));
                sal_memcpy(&alu2.key2, &alu2.key1, sizeof(bcmi_ft_alu_key_t));
                /* Set the flags.*/
                alu1.flags |= BCMI_FT_ALU_LOAD_NEXT_ATTACH;

                /* As checks are identical, keep keys same. */
                alu1.key1.location = temp[i].key1.location;
                alu1.key2.location = temp[i].key1.location+16;
                alu2.key1.location = temp[i].key1.location;
                alu2.key2.location = temp[i].key1.location+16;

                alu1.element_type2 = alu1.element_type1;
                alu2.element_type2 = alu2.element_type1;

                /* Reverse the picking of truncated data at different ALU. */
                alu1.action_key.location = temp[i].action_key.location+16;
                alu2.action_key.location = temp[i].action_key.location;

                alu1.action_key.length = 16;
                alu2.action_key.length = 16;
            }


            sal_memcpy(&new_alloc[i], &alu1, sizeof(bcmi_ft_group_alu_info_t));
            sal_memcpy(&new_alloc[i+1], &alu2, sizeof(bcmi_ft_group_alu_info_t));

            /* Now copy the rest of information. */
            sal_memcpy(&new_alloc[i+2], &temp[i+1],
                (num_alus_loads - (i+1)) * sizeof(bcmi_ft_group_alu_info_t));

           (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info = new_num_alus_loads;
           sal_free(*info);
           *info = new_alloc;
           /* Update number of loads for next check and move iterator 1 further. */
           num_alus_loads = new_num_alus_loads;
           i++;
        }
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *     bcmi_ft_group_extraction_alu_info_set
 * Purpose:
 *     Set Alu extraction info in group sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     num_alu_info - (IN) Number of ALU/lod info structure.
 *     alu_info  - (IN) list if ALU/load information.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_extraction_alu_info_set(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int num_alu_info,
              bcmi_ft_group_alu_info_t *alu_info)
{

    bcmi_ft_group_alu_info_t **temp = NULL;
    int *num_info = 0;

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    if (num_alu_info == 0) {
        /* Make sure that extraction info was cleared. */
        if (key_or_data) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Key elements cleared"
                                " for group = %d\n"), id));
            if (BCMI_FT_GROUP_EXT_KEY_INFO(unit, id) != NULL) {
                sal_free(BCMI_FT_GROUP_EXT_KEY_INFO(unit, id));
                BCMI_FT_GROUP_EXT_KEY_INFO(unit, id) = NULL;
            }
            BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info = 0;
        } else {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Data/Alu elements cleared"
                                " for group = %d\n"), id));
            if (BCMI_FT_GROUP_EXT_DATA_INFO(unit, id) != NULL) {
                sal_free(BCMI_FT_GROUP_EXT_DATA_INFO(unit, id));
                BCMI_FT_GROUP_EXT_DATA_INFO(unit, id) = NULL;
            }
            BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info = 0;
        }

        /* Nothing to do */
        return BCM_E_NONE;
    }

    if (alu_info == NULL) {
        return BCM_E_PARAM;
    }


    if (key_or_data) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit, "Key ALU elements added for group = %d are : %d\n"),
            id, num_alu_info));

        temp = (&(BCMI_FT_GROUP_EXT_KEY_INFO(unit, id)));
        num_info = (&(BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info));
    } else {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit, "Data ALU elements added for group = %d are : %d\n"),
            id, num_alu_info));
        temp = (&(BCMI_FT_GROUP_EXT_DATA_INFO(unit, id)));
        num_info = (&(BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info));
    }

    /* allocate space needed to save this state in group. */
    *temp = (bcmi_ft_group_alu_info_t *) sal_alloc(num_alu_info *
                sizeof(bcmi_ft_group_alu_info_t), "Group alu info.");

    if (*temp == NULL) {
        return BCM_E_MEMORY;
    }

   *num_info = num_alu_info;

    /* Store all the information in local memory. */
    sal_memcpy(*temp, alu_info,
        (num_alu_info * sizeof(bcmi_ft_group_alu_info_t)));

    /* Check if Session Data requires 32-bit to 16-bit conversion */
    if (soc_feature(unit, soc_feature_hx5_ft_32bit_param_update)) {
        if (key_or_data == 0) {
            BCM_IF_ERROR_RETURN(bcmi_ft_group_32bit_to_16_bit_param_conversion
                    (unit, id, (&(BCMI_FT_GROUP_EXT_DATA_INFO(unit, id)))));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_extraction_alu_info_get
 * Purpose:
 *     Get Alu extraction info of group sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     max_alu_info - (IN) max information user asked for.
 *     alu_info  - (OUT) list if ALU/load information.
 *     num_alu_info - (OUT) Number of ALU/lod info structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_extraction_alu_info_get(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int max_alu_info,
              bcmi_ft_group_alu_info_t *alu_info,
              int *num_alu_info)
{

    bcmi_ft_group_alu_info_t **temp =  NULL;
    int *num_info = 0;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    /* If the max_alu_info is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if ((max_alu_info != 0) && (alu_info == NULL)) {
        return BCM_E_PARAM;
    }

    if (num_alu_info == NULL) {
        return BCM_E_PARAM;
    }

    *num_alu_info = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* If max_alu_info == 0 and list_of_export_elements == NULL,
     * return the number of export elements in list_size
     * variable.
     */
    if ((max_alu_info == 0) && (alu_info == NULL)) {

        return BCM_E_NONE;
    }

    if (key_or_data) {

        if (!BCMI_FT_GROUP_EXT_KEY_INFO(unit, id)) {
            return BCM_E_NOT_FOUND;
        }


        temp = (&(BCMI_FT_GROUP_EXT_KEY_INFO(unit, id)));
        num_info = (&(BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info));

    } else {

        if (!BCMI_FT_GROUP_EXT_DATA_INFO(unit, id)) {
            return BCM_E_NOT_FOUND;
        }

        temp = (&(BCMI_FT_GROUP_EXT_DATA_INFO(unit, id)));
        num_info = (&(BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info));
    }

    /* Pick the number of elements to send back. */
    *num_alu_info = (*num_info < max_alu_info) ?
                    (*num_info) : max_alu_info;

    /* Send the information back to FTFP module. */
    sal_memcpy(alu_info, *temp,
        ((*num_alu_info) * sizeof(bcmi_ft_group_alu_info_t)));


    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_extraction_hw_info_set
 * Purpose:
 *     Set hardware extraction info in group sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     key_or_data - (IN) key/data
 *     num_elements - (IN) Number of elements of hw extraction.
 *     elements - (IN) list of hw extrator info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_extraction_hw_info_set(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int num_elements,
              bcmi_ft_hw_extractor_info_t *elements)
{

    bcmi_ft_hw_extractor_info_t **temp = NULL;
    int *num_info = NULL;

    if (num_elements == 0) {
        /* Make sure that extraction info was cleared. */
        if (key_or_data) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Key hw extractors info"
                                "cleared for group = %d\n"), id));
            if (BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key != NULL) {
                sal_free(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key);
                BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key = NULL;
            }
            BCMI_FT_GROUP_EXT_HW_INFO(unit, id).num_ft_key = 0;
        } else {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Data/Alu hw extractors info"
                                "cleared for group = %d\n"), id));
            if (BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data != NULL) {
                sal_free(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data);
                BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data = NULL;
            }
            BCMI_FT_GROUP_EXT_HW_INFO(unit, id).num_ft_data = 0;
        }

        /* Nothing to do */
        return BCM_E_NONE;
    }

    if (elements == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (key_or_data) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit, "Key elements added for group = %d are : %d\n"),
            id, num_elements));

        temp = (&(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key));
        num_info = (&(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).num_ft_key));
    } else {

        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit, "Data elements added for group = %d are : %d\n"),
            id, num_elements));

        temp = (&(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data));
        num_info = (&(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).num_ft_data));
    }

    if (*temp != NULL) {
        sal_free(*temp);
        *temp = NULL;
    }

    /* allocate space needed to save this state in group. */
    *temp = (bcmi_ft_hw_extractor_info_t*)
            sal_alloc(num_elements *
                sizeof(bcmi_ft_hw_extractor_info_t), "hardware extractor info");

    if (*temp == NULL) {
        return BCM_E_MEMORY;
    }

    (*num_info) = num_elements;

    sal_memcpy((*temp), elements,
        (num_elements * sizeof(bcmi_ft_hw_extractor_info_t)));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_extraction_hw_info_get
 * Purpose:
 *     Get hardware extraction info in group sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     key_or_data - (IN) key/data
 *     max_elements - (IN) Max elements that user asked for.
 *     elements - (OUT) list of hw extrator info.
 *     num_elements - (OUT) Number of elements of hw extraction.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_extraction_hw_info_get(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              int max_elements,
              bcmi_ft_hw_extractor_info_t *elements,
              int *num_elements)
{
    bcmi_ft_hw_extractor_info_t **temp = NULL;
    int *num_info = NULL;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    /* If the max_alu_info is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if ((max_elements != 0) && (elements == NULL)) {
        return BCM_E_PARAM;
    }

    if (num_elements == NULL) {
        return BCM_E_PARAM;
    }

    if (key_or_data) {
        temp = (&(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key));
        num_info = (&(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).num_ft_key));
    } else {
        temp = (&(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data));
        num_info = (&(BCMI_FT_GROUP_EXT_HW_INFO(unit, id).num_ft_data));
    }

    *num_elements = *num_info;

    /* If max_elements == 0 and elements == NULL,
     * return the number of elements present.
     */
    if ((max_elements == 0) && (elements == NULL)) {

        return BCM_E_NONE;
    }

    /* Pick the number of elements to send back. */
    *num_elements = ((*num_info) < max_elements) ?
                    (*num_info) : max_elements;



    sal_memcpy(elements, *temp,
        ((*num_elements) * sizeof(bcmi_ft_hw_extractor_info_t)));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_extraction_mode_set
 * Purpose:
 *     Set extraction mode.
 * Parameters:
 *     unit - (IN) BCM device id
 *     key_or_data - (IN) key/data
 *     id   - (IN) FT group id.
 *     mode - (IN) Mode of the extration key/data.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_extraction_mode_set(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_key_data_mode_t mode)
{

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    if (key_or_data) {
        BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key_mode = mode;
        BCMI_FT_GROUP_FTFP_DATA(unit, id).session_key_mode = mode;
    } else {
        BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data_mode = mode;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_extraction_mode_get
 * Purpose:
 *     Get extraction mode.
 * Parameters:
 *     unit - (IN) BCM device id
 *     key_or_data - (IN) key/data
 *     id   - (IN) FT group id.
 *     mode - (OUT) Mode of the extration key/data.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_extraction_mode_get(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_key_data_mode_t *mode)
{
    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (key_or_data) {
        *mode = BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key_mode;
    } else {
        *mode = BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data_mode;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_session_key_type_set
 * Purpose:
 *     Set session key type.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     key_type - (IN) Session Key Type
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_session_key_type_set(
              int unit,
              bcm_flowtracker_group_t id,
              uint32 key_type)
{

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    BCMI_FT_GROUP(unit, id)->ftfp_data.session_key_type = key_type;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_session_key_type_get
 * Purpose:
 *     Get session key type.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     key_type - (OUT) Session Key Type.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_session_key_type_get(
              int unit,
              bcm_flowtracker_group_t id,
              uint32 *key_type)
{
    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    *key_type = BCMI_FT_GROUP(unit, id)->ftfp_data.session_key_type;

    return BCM_E_NONE;
}

int
bcmi_ft_group_ftfp_profiles_set(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_session_profiles_t *profile_info)
{
    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.session_key_profile_idx =
        profile_info->session_key_profile_idx;

    BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.session_data_profile_idx =
        profile_info->session_data_profile_idx;

    BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.alu_data_profile_idx =
        profile_info->alu_data_profile_idx;

    return BCM_E_NONE;
}

int
bcmi_ft_group_ftfp_profiles_get(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_session_profiles_t *profile_info)
{
    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (!profile_info) {
        return (BCM_E_PARAM);
    }

    profile_info->session_key_profile_idx =
        BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.session_key_profile_idx;

    profile_info->session_data_profile_idx =
        BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.session_data_profile_idx;

    profile_info->alu_data_profile_idx =
        BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.alu_data_profile_idx;

    return BCM_E_NONE;
}



int
bcmi_ft_group_ftfp_data_get(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_ftfp_data_t *data)
{

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (!data) {
        return (BCM_E_PARAM);
    }

    data->direction = BCMI_FT_GROUP_FTFP_DATA(unit, id).direction;
    data->flowtrack = BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack;
    data->new_learn = BCMI_FT_GROUP_FTFP_DATA(unit, id).new_learn;
    data->session_key_type = BCMI_FT_GROUP_FTFP_DATA(unit, id).session_key_type;

    data->session_key_mode = BCMI_FT_GROUP_FTFP_DATA(unit, id).session_key_mode;

    data->profiles.session_key_profile_idx =
        BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.session_key_profile_idx;
    data->profiles.session_data_profile_idx =
        BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.session_data_profile_idx;
    data->profiles.alu_data_profile_idx =
        BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.alu_data_profile_idx;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_hw_install
 * Purpose:
 *     Install hardware resource for the group.
 * Description:
 *     All the groups parameters are added into groups software
 *     state. The hardware resources will only get allocated when
 *     extraction algorithm finished setting up the hardware extrators.
 *     Later hardware extration information will be added into
 *     groups's software state. After that when the group is added
 *     into FTFP entry then this hardware install method will be
 *     called.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_hw_install(int unit, bcm_flowtracker_group_t id)
{
    int rv =  BCM_E_NONE;
    int num_alus_loads = 0, i=0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    int load16_idz[TOTAL_GROUP_LOAD16_DATA_NUM];
    int load8_idz[TOTAL_GROUP_LOAD8_DATA_NUM];
    int num_load_16=0, num_load_8=0;
    bcmi_ft_alu_load_type_t type = bcmiFtAluLoadTypeNone;
    int total_load8_mem = TOTAL_GROUP_LOAD8_DATA_NUM;
    int total_load16_mem = TOTAL_GROUP_LOAD16_DATA_NUM;
    bcmi_ft_group_key_data_mode_t mode = 0;
    int length=0, alu=0;

    /* Read the group state for the ALU inforamtion. */
    if (!(BCMI_FT_GROUP_EXT_DATA_OR_TS_TRIGGERS_SET(unit, id))) {
        return BCM_E_INTERNAL;
    }

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    for (; i<total_load16_mem; i++) {
        load16_idz[i] = -1;

        if (i<total_load8_mem) {
            load8_idz[i] = -1;
        }
    }

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Get total number of alus in this chunk. */
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* allocate the bitmap based on the type, length */
    for (i=0; i<num_alus_loads; i++) {

        if (temp->key1.location >= BCMI_FT_SESSION_DATA_SINGLE_LEN) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }

        /* Normalize length and alu status from multiple keys. */
        rv = bcmi_ft_alu_load_values_normalize(unit, temp, &length, &alu);
        CLEANUP_ON_ERROR(rv)

        /* Now get the type of the memory and take action. */
        rv = bcmi_ft_alu_load_type_get(unit, alu, length, &type);
        CLEANUP_ON_ERROR(rv)

        if (type == bcmiFtAluLoadTypeLoad16) {
            load16_idz[num_load_16] = i;
            num_load_16++;
            if (num_load_16 > total_load16_mem) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            temp++;
            /* We do not need to do anything here. */
            continue;
        } else if (type == bcmiFtAluLoadTypeLoad8 ) {
            load8_idz[num_load_8] = i;
            num_load_8++;
            if (num_load_8 > total_load8_mem) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            temp++;
            /* We do not need to do anything here. */
            continue;
        }
                /* This ALU memory is used. go for next one. */
                /* This ALU memory is used. go for next one. */

        /* Configure the ALU memory. */
        /*rv = bcmi_ft_group_alu_configure(unit, id, temp, type);*/
        BCMI_FT_METHOD_INSTALL_CHECK(type);
        CLEANUP_ON_ERROR(rv);

        rv = (BCMI_FT_METHOD_INSTALL_EXEC(type))(unit, id, temp,
                  NULL);
        CLEANUP_ON_ERROR(rv)

        /* Set that index into the group table. */
        temp++;
    }

    /*
     * The upper loop has created the ALU indexes and wrote into them.
     * Now we need to set the load profiles based on the indices
     * that we created above.
     * If there is no load information then no need to configure load memories.
     */
    if (load16_idz[0] != -1) {
        BCMI_FT_METHOD_INSTALL_CHECK(bcmiFtAluLoadTypeLoad16);
        CLEANUP_ON_ERROR(rv);

        rv = (BCMI_FT_METHOD_INSTALL_EXEC(bcmiFtAluLoadTypeLoad16))
                  (unit, id, NULL, load16_idz);

        CLEANUP_ON_ERROR(rv)
    }

    if (load8_idz[0] != -1) {
        BCMI_FT_METHOD_INSTALL_CHECK(bcmiFtAluLoadTypeLoad8);
        CLEANUP_ON_ERROR(rv);

        rv = (BCMI_FT_METHOD_INSTALL_EXEC(bcmiFtAluLoadTypeLoad8))
                  (unit, id, NULL, load8_idz);

        CLEANUP_ON_ERROR(rv);
    }

    /* Set the timestamp entry. */
    rv = bcmi_ft_group_timestamp_set(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Add the pdd/pde policy entry. */
    bcmi_ft_group_pdd_policy_profile_add(unit, id);

    /* Create template information list. */
    rv = bcmi_ft_group_template_data_create(unit, id, load16_idz, load8_idz);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Set the single/double session mode based on extraction length. */
    rv = bcmi_ft_group_ft_mode_update(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

    /*
     * we also need to set mode in group table.
     * FTFP and group data can both update data mode.
     * So check again if FTFP has also updated and
     * then install in group table.
     */
    bcmi_ft_group_extraction_mode_get(unit, 0, id, &mode);

    if (mode) {
        soc_mem_field32_modify(unit, BSC_KG_GROUP_TABLEm, id, DATA_MODEf, 1);
    }

    /* Install Session profiles and update ftfp data */
    rv = _bcm_field_ft_session_profiles_install(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Set the installed status. */
    BCMI_FT_GROUP_IS_VALIDATED(unit, id) = 1;

    return BCM_E_NONE;

cleanup :
    bcmi_ft_group_hw_uninstall(unit, id);
    return rv;
}


/*
 * Function:
 *     bcmi_ft_group_hw_uninstall
 * Purpose:
 *     Uninstall hardware resource for the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_hw_uninstall(int unit, bcm_flowtracker_group_t id)
{
    int rv =  BCM_E_NONE;
    int num_alus = 0, i = 0;
    bcmi_ft_group_alu_info_t *temp = NULL;
/*    int index  = 1;*/
    int load16_idz[TOTAL_GROUP_LOAD16_DATA_NUM];
    int load8_idz[TOTAL_GROUP_LOAD8_DATA_NUM];
    int num_load_16=0, num_load_8=0;
    bcmi_ft_alu_load_type_t type = bcmiFtAluLoadTypeNone;
    int total_load8_mem = TOTAL_GROUP_LOAD8_DATA_NUM;
    int total_load16_mem = TOTAL_GROUP_LOAD16_DATA_NUM;
    bcmi_ft_group_template_list_t **head;
    int length=0, alu=0;

    if (bcmi_ft_group_is_invalid(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Invalid flow Group (%d)\n "), id));

        return BCM_E_CONFIG;
    }

    /* Read the group state for the ALU inforamtion. */
    if (!(BCMI_FT_GROUP_EXT_DATA_OR_TS_TRIGGERS_SET(unit, id))) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Extraction information is not yet set for Group. (%d)\n "), id));

        return BCM_E_INTERNAL;
    }

    for (; i<total_load16_mem; i++) {
        load16_idz[i] = -1;

        if (i<total_load8_mem) {
            load8_idz[i] = -1;
        }
    }

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Get the head of the templs info list. */
    head = &((BCMI_FT_GROUP(unit, id))->template_head);

    /* Get total number of alus in this chunk. */
    num_alus = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* allocate the bitmap based on the type, length */
    for (i=0; i<num_alus; i++) {

        /* Normalize length and alu status from multiple keys. */
        rv = bcmi_ft_alu_load_values_normalize(unit, temp, &length, &alu);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Now get the type of the memory and take action. */
        rv = bcmi_ft_alu_load_type_get(unit, alu, length, &type);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (type == bcmiFtAluLoadTypeLoad16) {
            load16_idz[num_load_16] = i;
            num_load_16++;
            if (num_load_16 > total_load16_mem) {
                /* This actually should not happen. just a -ve check */
                return BCM_E_PARAM;
            }
            temp++;
            /* We do not need to do anything here. */
            continue;
        } else if (type == bcmiFtAluLoadTypeLoad8 ) {
            load8_idz[num_load_8] = i;
            num_load_8++;
            if (num_load_8 > total_load8_mem) {
                /* This actually should not happen. just a -ve check */
                return BCM_E_PARAM;
            }
            temp++;
            /* We do not need to do anything here. */
            continue;
        }

        /* Index is obtained, now start writing into that memory. */
        rv = (BCMI_FT_METHOD_UNINSTALL_EXEC(type))(unit, id, temp,
                  NULL);

        if (BCM_FAILURE(rv)) {
            /*
             * The group uninstall is called when install fails and not
             * all the memories are written. At that time, we need to clear
             * whatever is being written already for that group.
             * But because we are trying to clear the whole group,
             * we may find BCM_E_NOT_FOUND for some memories which were
             * not yet installed in that group.
             * In real uninstall case, logically no chance to get
             * BCM_E_NOT_FOUND as this installing is all handled internally.
             * In mem clear fails with above error then just continue as
             * most probably the memory was installed yet.
             */
            if (rv != BCM_E_NOT_FOUND) {
                return rv;
            }
        }

        /* Set that index into the group table. */
        temp++;
    }

    /*
     * Clear the load16 memories now.
     */
    if (load16_idz[0] != -1) {
        rv = (BCMI_FT_METHOD_UNINSTALL_EXEC(bcmiFtAluLoadTypeLoad16))
                 (unit, id, NULL, load16_idz);
        if (BCM_FAILURE(rv)) {
            if (rv != BCM_E_NOT_FOUND) {
                return rv;
            }
        }
    }

    /*
     * Clear the load8 memories now.
     */
    if (load8_idz[0] != -1) {
       rv = (BCMI_FT_METHOD_UNINSTALL_EXEC(bcmiFtAluLoadTypeLoad8))
                 (unit, id, NULL, load8_idz);

        if (BCM_FAILURE(rv)) {
            if (rv != BCM_E_NOT_FOUND) {
                return rv;
            }
        }
    }

    /* delete the pdd/pde policy entry. */
    rv = bcmi_ft_group_pdd_policy_profile_delete(unit, id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    bcmi_ft_group_template_list_delete_all(unit, head);

    /* Clear in use memories of group. */
    sal_memset(BCMI_FT_GROUP_ALU16_MEM_USE(unit, id), 0,
        (sizeof(uint32) *
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16)));
    sal_memset(BCMI_FT_GROUP_ALU32_MEM_USE(unit, id), 0,
        (sizeof(uint32) *
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32)));

    /* Un-install Session profiles and update ftfp data */
    rv = _bcm_field_ft_session_profiles_uninstall(unit, id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* reset the validated status. */
    BCMI_FT_GROUP_IS_VALIDATED(unit, id) = 0;

    return rv;

}

/*
 * Function:
 *     bcmi_ft_group_pdd_policy_profile_add
 * Purpose:
 *     Add policy profile entry for group action profile.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_pdd_policy_profile_add(
        int unit,
        bcm_flowtracker_group_t id)
{
    bsc_kg_group_table_entry_t kg_group_entry;
    bsc_dt_pde_profile_entry_t pde_entry;
    void *entries[1];
    uint32 index = 0;
    int rv = BCM_E_NONE;
    int old_index = 0;
    int ref_count = 0;

    /* Group has the pdd profile entry. Here we just need to set it. */

    /* set the entry to profile entries. */
    entries[0] = (&(BCMI_FT_GROUP(unit, id)->pdd_entry));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &kg_group_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);

    if (old_index) {
        bcmi_ft_group_pdd_profile_entry_delete(unit, old_index);
    }

    /* Create teh profile entry for new age out profile. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_pdd_profile_entry_add(unit, entries, 1,
        (uint32 *)&index));

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, ((uint32 *)&kg_group_entry),
        SESSION_DATA_TYPEf, index);

    /* Write the entry back into the group table. */
    rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &kg_group_entry);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    bcmi_ft_group_pdd_to_pde_coversion(unit, id, &pde_entry);

    rv = bcmi_ft_group_pdd_profile_refcount_get(unit, index, &ref_count);

    if (BCM_FAILURE(rv)) {
        bcmi_ft_group_pdd_profile_entry_delete(unit, index);
        return rv;
    }

    if (ref_count == 1) {
        /* This is a new entry. Write the PDE entry also at same index. */
        bcmi_ft_group_pdd_to_pde_coversion(unit, id, &pde_entry);
        rv = soc_mem_write(unit, BSC_DT_PDE_PROFILEm, MEM_BLOCK_ANY, index,
                 &pde_entry);

        if (BCM_FAILURE(rv)) {
            bcmi_ft_group_pdd_profile_entry_delete(unit, index);
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_pdd_policy_profile_delete
 * Purpose:
 *     Delete policy profile entry for group action profile.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_pdd_policy_profile_delete(
        int unit,
        bcm_flowtracker_group_t id)
{

    bsc_kg_group_table_entry_t entry;
    bsc_dt_pde_profile_entry_t pde_entry;
    int old_index = 0;
    int rv = BCM_E_NONE;
    int ref_count = 0;
    int alu_load_type = bcmiFtAluLoadTypeNone;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    int total_mems = 0, mem_idx = 0;

    /* Group has the pdd profile entry. Here we just need to set it. */

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
            ((uint32 *)&entry), SESSION_DATA_TYPEf);

    if (old_index) {
        bcmi_ft_group_pdd_profile_entry_delete(unit, old_index);
    }

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, ((uint32 *)&entry),
             SESSION_DATA_TYPEf, 0);

    /* Write the entry back into the group table. */
    rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &entry);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * First check the refcount of pdd entry.
     * Clear only if no other group is pointing to it.
     */
    rv = bcmi_ft_group_pdd_profile_refcount_get(unit, old_index, &ref_count);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (ref_count == 0) {
        /* Delete the PDE profile entry also.. */
        sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_entry_t));
        rv = soc_mem_write(unit, BSC_DT_PDE_PROFILEm, MEM_BLOCK_ANY, old_index,
                 &pde_entry);

        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /*
     * The group is uninstalled. clear the group's PDD entry.
     * Clear only ALU and Load memories. Rest should not be removed.
     */
    for (alu_load_type=0; alu_load_type<=bcmiFtAluLoadTypeAlu32;
        alu_load_type++) {

        total_mems = alu_load_index_info[unit][alu_load_type].total_mem_count;

        if (alu_load_type == bcmiFtAluLoadTypeLoad16) {
            total_mems = TOTAL_GROUP_LOAD16_DATA_NUM;
        } else  if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
            total_mems = TOTAL_GROUP_LOAD8_DATA_NUM;
        }

        /* For all the alu/load types, check what memories are set in pdd. */
        for (mem_idx=0; mem_idx<total_mems; mem_idx++) {
            /* Now get if this particular field is set in pdd. */
            pdd_info = alu_load_index_info[unit][alu_load_type].pdd_bus_info;
            if (!pdd_info) {
                return BCM_E_INTERNAL;
            }
            soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
                    (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                    pdd_info[mem_idx].param_id, 0);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_flush_set
 * Purpose:
 *     Remove hash entry from hash table.
 * Parameters:
 *     unit        - (IN) BCM device id
 *     flags       - (IN) Flags for group clear.
 *     num_groups  - (IN) Number of groups to be cleared.
 *     idz         - (IN) list of Ids of all the groups..
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_flush_set(
        int unit,
        uint32 flags,
        int num_groups,
        bcm_flowtracker_group_t *idz)
{

    uint64 val64;
    soc_reg_t reg = 0;
    int i = 0;
    int rv;
    int count = 5; /* count 5 is to make 20ms delay */
    uint32 flow_limit[4] = {0};
    int flow_limit_valid[4] = {FALSE};

    if (idz == NULL) {
        return BCM_E_PARAM;
    }

    if ((num_groups < 1) || (num_groups > 4)) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_FLOWTRACKER_GROUP_CLEAR_FLOW_DATA_ONLY) {
        return BCM_E_UNAVAIL;
    }

    reg = BSC_AG_GROUP_FLUSH_CONFIG_64r;
    /* Initializr to zero. */
    COMPILER_64_ZERO(val64);

    /* Validate each group index. */
    for (; i < num_groups; i++) {
        BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, idz[i]));

        if (BCMI_FT_GROUP(unit, idz[i])->flags
                & BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
            LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Group Flush is not supported for the"
                                " group = %d\n"), idz[i]));
            return BCM_E_PARAM;
        }
    }

    /* Initiate flush for groups. */
    for (i = 0; i < num_groups; i++) {
        soc_reg64_field32_set(unit, reg, &val64,
            bcmi_ft_group_flush_fields[i].group_id, idz[i]);

        soc_reg64_field32_set(unit, reg, &val64,
            bcmi_ft_group_flush_fields[i].group_valid, 1);

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit, "Group Flush initiated for the group = %d\n"),
            idz[i]));


    }

    /* Set flow limit to 0 so that no new flows is learnt. */
    for (i = 0; i < num_groups; i++) {
        rv = bcmi_ft_group_flow_limit_get(unit, idz[i], &flow_limit[i]);
        /* Flow Limit is not set on this Group */
        if ((rv == BCM_E_NOT_FOUND) || (rv == BCM_E_CONFIG)) {
            flow_limit_valid[i] = FALSE;
        } else {
            flow_limit_valid[i] = TRUE;
            BCM_IF_ERROR_RETURN(bcmi_ft_group_flow_limit_set(unit, idz[i], 0));
        }
    }

    /*
     * Give delay of 4 msec.
     * This is to ensure that all packets have reached groups queue
     * after entring into the flowtracker and got identified
     * to use this group in FTFP.
     */
    sal_usleep(4000);

    BCM_IF_ERROR_RETURN(WRITE_BSC_AG_GROUP_FLUSH_CONFIG_64r(unit, val64));

    /*
     * Now that we have written into the group register,
     * we need to check if done bit is set.
     * Count valus is 5 to make 20 ms delay but
     * hardware gurantees to come out in a max time of 16 msecs.
     */
    while (count > 1) {
        BCM_IF_ERROR_RETURN(READ_BSC_AG_GROUP_FLUSH_CONFIG_64r(unit, &val64));

        if (soc_reg64_field32_get(unit, reg, val64, GROUP_FLUSH_DONEf)) {
            LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "The Group Flush is done for all the requested groups.\n")));

            break;
        }
        count--;
        sal_usleep(4000);
    }

    /* initialize it again to clear the register value.*/
    COMPILER_64_ZERO(val64);

    /* Clear the flush register. */
    BCM_IF_ERROR_RETURN(WRITE_BSC_AG_GROUP_FLUSH_CONFIG_64r(unit, val64));

    /* Apply flow limit back to flowtracker group */
    for (i = 0; i < num_groups; i++) {
        if (flow_limit_valid[i] == TRUE) {
            BCM_IF_ERROR_RETURN(bcmi_ft_group_flow_limit_set(unit, idz[i], flow_limit[i]));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_group_template_data_create
 * Purpose:
 *    Add this flowtracker into the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) Flowtracker_group.
 *     load16_indexes - (IN) load16 element array.
 *     load8_indexes - (IN) load8 element array.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_template_data_create(
        int unit,
        bcm_flowtracker_group_t id,
        int *load16_indexes,
        int *load8_indexes)
{

    bcmi_ft_group_template_list_t **head;
    int running_length = 0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_alu_info_t *start = NULL;
    int i=0;
    int rv = BCM_E_NONE;
    bcmi_ft_group_alu_info_t *local = NULL;
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeNone;
    int copy_to_cpu = 0;
    int meter_enabled = 0;
    bcmi_ft_group_param_type_t ts_type = bcmiFtGroupParamTypeTsNewLearn;
    soc_field_t ts_data_fields[4] =
                    {DATA_0f, DATA_1f, DATA_2f, DATA_3f};

    /* First we should look into the in order of the PDD data. */

    /* Get the head of the templs info list. */
    head = &((BCMI_FT_GROUP(unit, id))->template_head);

    /*
     * Get the input from session data and then check
     * each type of memories to form the template data.
     * Start with LOAD8 as they are first in the list.
     * We need to do it in order of PDD as that will be the order
     * how the data will be exported. For now, load8 will start from 0.
     */

    /* assign the ALU memory chunk to local pointer. */
    start = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    temp = start;

    i = 0;
    /* Start setting the space for 4 timestamps. */
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_0f)) {
        ts_type = bcmi_ft_group_get_ts_type(unit, id, ts_data_fields[i]);

        rv = bcmi_ft_group_template_list_add(unit, head,
                running_length, 48, ts_type, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

        /* Timestamp 0 is enabled. length=48 bits. */
        running_length +=48;
        i++;
    }

    /* Start setting the space for 4 timestamps. */
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_1f)) {
        /* Timestamp 1 is enabled. length=48 bits. */
        ts_type = bcmi_ft_group_get_ts_type(unit, id, ts_data_fields[i]);

        rv = bcmi_ft_group_template_list_add(unit, head,
                running_length, 48, ts_type, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

        /* Timestamp 0 is enabled. length=48 bits. */
        running_length +=48;
        i++;
    }

    /* Start setting the space for 4 timestamps. */
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_2f)) {
        /* Timestamp 2 is enabled. length=48 bits. */
        ts_type = bcmi_ft_group_get_ts_type(unit, id, ts_data_fields[i]);

        rv = bcmi_ft_group_template_list_add(unit, head,
                running_length, 48, ts_type, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

        /* Timestamp 0 is enabled. length=48 bits. */
        running_length +=48;
        i++;
    }

    /* Start setting the space for 4 timestamps. */
    if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_3f)) {
        /* Timestamp 3 is enabled. length=48 bits. */
        ts_type = bcmi_ft_group_get_ts_type(unit, id, ts_data_fields[i]);

        rv = bcmi_ft_group_template_list_add(unit, head,
                running_length, 48, ts_type, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

        /* Timestamp 0 is enabled. length=48 bits. */
        running_length +=48;
        i++;
    }


    meter_enabled = soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
        BSD_FLEX_FLOW_METERf);

    if (meter_enabled) {
        /* 48 bits are used for metering. */
        rv = bcmi_ft_group_template_list_add(unit, head,
                running_length, 48, bcmiFtGroupParamTypeMeter, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

        running_length +=48;
    }

    /* Add ALU32s into the template list based on PDD. */
    for (i=0; i<TOTAL_GROUP_ALU32_MEM; i++) {
        alu_load_type = bcmiFtAluLoadTypeAlu32;

        if (bcmi_ft_group_alu_load_pdd_status_get(
            unit, id, alu_load_type, i)) {

            /* Check for this index and get corresponding data. */
            rv = bcmi_ft_group_alu_load_index_match_data_get
                     (unit, id, alu_load_type, i, &temp);

            if (BCM_FAILURE(rv)) {
                continue;
            }
            rv = bcmi_ft_group_template_list_add(unit, head,
                running_length, 32, bcmiFtGroupParamTypeTracking, temp);

            BCMI_CLEANUP_IF_ERROR(rv);
            running_length += 32;
        }
    }

    temp = start;
    /* Get all the elements for load16.*/
    for (i=TOTAL_GROUP_LOAD16_DATA_NUM-1; i>=0; i--) {
        int len=0;
        if (load16_indexes[i] == -1) {
            continue;
        }
        local = (&(start[load16_indexes[i]]));

        if ((local->element_type1 == bcmFlowtrackerTrackingParamTypeDosAttack) ||
            (local->element_type1 == bcmFlowtrackerTrackingParamTypeInnerDosAttack)) {
           /* Add 16 bit length to display proper results in export. */
           len = 16;
           i--;
        }

        /* If there is some data then add it into list. */
        rv = bcmi_ft_group_template_list_add(unit, head,
            running_length, 16+len, bcmiFtGroupParamTypeTracking, local);

        BCMI_CLEANUP_IF_ERROR(rv);
        running_length += (16+len);
    }

    temp = start;


    /* Add ALU16s into template list based on PDD. */
    for (i=0; i<TOTAL_GROUP_ALU16_MEM; i++) {
        alu_load_type = bcmiFtAluLoadTypeAlu16;

        if (bcmi_ft_group_alu_load_pdd_status_get(
            unit, id, alu_load_type, i)) {

            /* Check for this index and get corresponding data. */
            rv = bcmi_ft_group_alu_load_index_match_data_get
                     (unit, id, alu_load_type, i, &temp);

            if (BCM_FAILURE(rv)) {
                continue;
            }
            rv = bcmi_ft_group_template_list_add(unit, head,
                running_length, 16, bcmiFtGroupParamTypeTracking, temp);

            BCMI_CLEANUP_IF_ERROR(rv);
            running_length += 16;
        }
    }

    copy_to_cpu = soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
        BSD_FLEX_FLOW_COUNT_TO_CPUf);

    if (copy_to_cpu) {
        rv = bcmi_ft_group_template_list_add(unit, head,
            running_length, 16, bcmiFtGroupParamTypeCollectorCopy, NULL);

        BCMI_CLEANUP_IF_ERROR(rv);
        running_length += 16;
    }

    /* Get all the elements of load8 and then add them into order. */
    for (i=TOTAL_GROUP_LOAD8_DATA_NUM-1; i>=0; i--) {
        if (load8_indexes[i] == -1) {
            continue;
        }
        local = (&(start[load8_indexes[i]]));

        rv = bcmi_ft_group_template_list_add(unit, head,
            running_length, 8, bcmiFtGroupParamTypeTracking, local);

        BCMI_CLEANUP_IF_ERROR(rv);
        running_length += 8;
    }

    /* We can put FT_STATE here but anyways it is not exposed to user. */
    return BCM_E_NONE;

cleanup :
    bcmi_ft_group_template_list_delete_all(unit, head);
    return rv;
}


/*
 * Function:
 *    bcmi_ft_group_template_list_add
 * Purpose:
 *    Add this flowtracker into the group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     head - (INOUT) Head pointer of the list.
 *     start - (IN) start of location.
 *     length - (IN) length of element.
 *     type  - (IN) type of element.
 *
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_ft_group_template_list_add(
        int unit,
        bcmi_ft_group_template_list_t **head,
        int start,
        int length,
        bcmi_ft_group_param_type_t type,
        bcmi_ft_group_alu_info_t *info)
{
    int rv = BCM_E_NONE;
    int shift_bits = 0;
    bcmi_flowtracker_flowchecker_info_t check_info;
    bcmi_ft_group_template_list_t *temp_node = NULL;
    bcmi_ft_group_template_list_t *node_iter = NULL;
    bcmi_ft_group_template_list_t *node_prev = NULL;

    temp_node = (bcmi_ft_group_template_list_t *)
        sal_alloc(sizeof(bcmi_ft_group_template_list_t),
                "flowchecker list Node");
    if (temp_node == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(temp_node, 0, sizeof (bcmi_ft_group_template_list_t));

    temp_node->info.cont_offset = start;
    temp_node->info.data_shift = 0;
    temp_node->info.cont_width = length;
    temp_node->info.param_type = type;
    temp_node->next = NULL;

    if (info != NULL) {
        temp_node->info.param = info->element_type1;
        temp_node->info.check_id = info->flowchecker_id;
        temp_node->info.index = info->alu_load_index;
        temp_node->info.type = info->alu_load_type;
        temp_node->info.flags = info->flags;

        if (temp_node->info.check_id) {
            temp_node->info.param_type = bcmiFtGroupParamTypeFlowchecker;

            /* Shift data_offset, if required */
            sal_memset(&check_info, 0,
                    sizeof (bcmi_flowtracker_flowchecker_info_t));
            rv = bcmi_ft_flowchecker_get
                (unit, temp_node->info.check_id, &check_info);
            if (BCM_FAILURE(rv)) {
                sal_free(temp_node);
                return rv;
            }
            bcmi_ft_check_action_shift_bits_get(unit,
                    check_info.action_info.action, info, &shift_bits);
            temp_node->info.data_shift = shift_bits;
        }
    }

    /* Insert temp_node at appropriate location. */
    node_prev = NULL;
    node_iter = (*head);
    while (node_iter != NULL) {
        if (info && (node_iter->info.type == info->alu_load_type)) {
            if (node_iter->info.index > info->alu_load_index) {
                break;
            }
        }
        node_prev = node_iter;
        node_iter = node_iter->next;
    }

    if (node_prev != NULL) {
        /* Insert after node_prev */
        temp_node->next = node_prev->next;
        node_prev->next = temp_node;
    } else {
        /* Insert at HEAD */
        temp_node->next = (*head);
        (*head) = temp_node;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_ft_group_template_list_delete_all
 * Purpose:
 *    Delete this flowtracker check from group list.
 * Parameters:
 *     unit - (IN) BCM device id
 *     head - (INOUT) Head pointer of the list.
 *
 * Returns:
 *     BCM_E_XXX.
 */
void
bcmi_ft_group_template_list_delete_all(
        int unit,
        bcmi_ft_group_template_list_t **head)
{

    bcmi_ft_group_template_list_t *temp, *prev;

    if (!head) {
        return;
    }

    prev = temp = (*head);

    while(temp) {
        prev = temp;
        temp = temp->next;
        sal_free(prev);
        prev = NULL;

    }

    *head = NULL;
    return;
}

/*
 * Function:
 *      bcmi_flowtracker_group_control_set
 * Purpose:
 *      Set value of group control.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      type          - (IN)  type of group control.
 *      arg           - (IN) value of control
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_flowtracker_group_control_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_group_control_type_t type,
    int arg)
{

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    switch(type) {
        case bcmFlowtrackerGroupControlNewLearnEnable:
            if (arg > 1) {
                return BCM_E_PARAM;
            }

            if ((BCMI_FT_GROUP(unit, id))->flags & BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
                return BCM_E_CONFIG;
            }

            BCM_IF_ERROR_RETURN(
                _bcm_field_ft_group_control_set(unit, id, type, arg));

            /* Add that value in local data and send notification to ftfp. */
            BCMI_FT_GROUP_FTFP_DATA(unit, id).new_learn = arg;
        break;

        case bcmFlowtrackerGroupControlFlowtrackerEnable:
            if (arg > 1 ) {
                return BCM_E_PARAM;
            }

            /* Check if FT Collector is present if when FT Group is busy */
            if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
                if (BCMI_FT_GROUP(unit, id)->collector_id == BCMI_FT_COLLECTOR_ID_INVALID) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                            (BSL_META_U(unit, "Collector is not associated"
                                        " with Flowtracker Group (%d)\n "), id));
                    return BCM_E_CONFIG;
                }
            }

            BCM_IF_ERROR_RETURN(
                _bcm_field_ft_group_control_set(unit, id, type, arg));

            /* Add that value in local data and send notification to ftfp. */
            BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack = arg;
        break;

        case bcmFlowtrackerGroupControlFlowDirection:

            if (arg == bcmFlowtrackerFlowDirectionBidirectional) {
                if (!(soc_feature(unit,
                    soc_feature_flowtracker_ver_1_bidirectional))) {
                    return BCM_E_UNAVAIL;
                }
            } else if (arg == bcmFlowtrackerFlowDirectionReverse) {
                return BCM_E_UNAVAIL;
            }

            if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                    "Not allowed to alter installed flow Group (%d)\n "), id));

                return BCM_E_CONFIG;
            }

            if (arg >= bcmFlowtrackerFlowDirectionCount) {
                return BCM_E_PARAM;
            }

            if (arg == BCMI_FT_GROUP_FTFP_DATA(unit, id).direction) {
                return BCM_E_NONE;
            }

            BCM_IF_ERROR_RETURN(
                _bcm_field_ft_group_control_set(unit, id, type, arg));

            /* Add that value in local data and send notification to ftfp. */
            BCMI_FT_GROUP_FTFP_DATA(unit, id).direction = arg;
            /*
             * To change the direction run time, we need to change
             * ALU/LOAD memories associated with this group.
             */
            BCM_IF_ERROR_RETURN(
                bcmi_ft_group_alu_load_trigger_set(unit, id, arg));
        break;

        default:
            /* Nothing else is supported.. */
        return BCM_E_PARAM;
   }


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_flowtracker_group_control_get
 * Purpose:
 *      Set value of group control.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      type          - (IN)  type of group control.
 *      arg           - (OUT) value of control
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_flowtracker_group_control_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_group_control_type_t type,
    int *arg)
{

    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    switch(type) {

        case bcmFlowtrackerGroupControlNewLearnEnable:
            /* Add that value in local data and send notification to ftfp. */
            *arg = BCMI_FT_GROUP_FTFP_DATA(unit, id).new_learn;
        break;

        case bcmFlowtrackerGroupControlFlowtrackerEnable:
            /* Add that value in local data and send notification to ftfp. */
            *arg = BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack;
        break;

        case bcmFlowtrackerGroupControlFlowDirection:
            /* Add that value in local data and send notification to ftfp. */
            *arg = BCMI_FT_GROUP_FTFP_DATA(unit, id).direction;
        break;

        default:
            /* Nothing else is supported.. */
        return BCM_E_PARAM;
   }

    return rv;
}

/*
 * Function:
 *      bcmi_ft_group_stat_get
 * Purpose:
 *      Get statistics of group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      group_stats   - (OUT) stats of the group.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_stat_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_stat_t *group_stats)
{

    uint64 val;
    uint32 val32 = 0;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (!group_stats) {
        return BCM_E_PARAM;
    }

    sal_memset(group_stats, 0, sizeof(bcm_flowtracker_group_stat_t));

    COMPILER_64_ZERO(val);
    BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_NUM_EXCEEDED_CNT, id, &val));

    COMPILER_64_COPY(group_stats->flow_exceeded_count, val);

    COMPILER_64_TO_32_LO(val32, val);

    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "flow_exceeded_count = %d for group: %d\n"), val32, id));


    BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_MISSED_CNT, id, &val));

    COMPILER_64_COPY(group_stats->flow_missed_count, val);

    COMPILER_64_TO_32_LO(val32, val);

    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "flow_missed_count = %d for group: %d\n"), val32, id));


    BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_AGE_OUT_CNT, id, &val));

    COMPILER_64_COPY(group_stats->flow_aged_out_count, val);

    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "flow_aged_out_count = %d for group: %d\n"), val32, id));

    BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT, id, &val));

    if (BCMI_FT_GROUP(unit, id)->flags &
            BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
        COMPILER_64_ZERO(val);
    }

    COMPILER_64_COPY(group_stats->flow_learnt_count, val);

    COMPILER_64_TO_32_LO(val32, val);

    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "flow_learnt_count = %d for group: %d\n"), val32, id));


    BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_METER_EXCEEDED_CNT, id, &val));

    COMPILER_64_COPY(group_stats->flow_meter_exceeded_count, val);

    COMPILER_64_TO_32_LO(val32, val);


    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "flow_meter_exceeded_count = %d for group: %d\n"), val32, id));

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcmi_ft_group_stat_set
 * Purpose:
 *      Set statistics of group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      group_stats   - (IN) stats of the group.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_stat_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_stat_t *group_stats)
{

    uint64 val;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (!group_stats) {
        return BCM_E_PARAM;
    }

    /* Flows missed because of group's flows limit is reached. */
    COMPILER_64_COPY(val, group_stats->flow_exceeded_count);

    BCM_IF_ERROR_RETURN(soc_counter_set(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_NUM_EXCEEDED_CNT, id, val));


    /* Flows missed because of table full on this group. */
    COMPILER_64_COPY(val, group_stats->flow_missed_count);

    BCM_IF_ERROR_RETURN(soc_counter_set(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_MISSED_CNT, id, val));


    /* Flows aged out on this group. */
    COMPILER_64_COPY(val, group_stats->flow_aged_out_count);

    BCM_IF_ERROR_RETURN(soc_counter_set(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_AGE_OUT_CNT, id, val));

    /* FLows learnt on a group. */
    COMPILER_64_COPY(val, group_stats->flow_learnt_count);

    BCM_IF_ERROR_RETURN(soc_counter_set(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT, id, val));


    /* Flow meter exceed count. */
    COMPILER_64_COPY(val, group_stats->flow_meter_exceeded_count);

    BCM_IF_ERROR_RETURN(soc_counter_set(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_METER_EXCEEDED_CNT, id, val));

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcmi_ft_group_export_trigger_verify
 * Purpose:
 *      verify export triggers on group.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      export_trigger_info - (IN) Export trigger info.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_export_trigger_verify(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    uint32 val;
    uint32 interval = 0;
    uint64 reg64;

    /* export trigger for periodic timer. */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                    bcmFlowtrackerExportTriggerTimer)) ? 1 : 0);
    if (val != 0) {
        COMPILER_64_SET(reg64, 0, 0);
        BCM_IF_ERROR_RETURN(READ_BSC_AG_PERIODIC_EXPORTr(unit, &reg64));
        interval = soc_reg64_field32_get(unit,
                BSC_AG_PERIODIC_EXPORTr, reg64, EXPORT_PERIODf);
        if (interval == 0) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Periodic"
                            " export interval is not configured\n\r")));
            return BCM_E_CONFIG;
        }
    }

    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                    bcmFlowtrackerExportTriggerNewLearn)) ? 1 : 0);
    if (val != 0) {
        if (BCMI_FT_GROUP(unit, id)->flags
                & BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Export"
                            " on New Learn is not supported on Software"
                            " mananged Flowtracker group\n\r")));
            return BCM_E_CONFIG;
        }
    }

    /* export trigger for ageout. */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                    bcmFlowtrackerExportTriggerAgeOut)) ? 1 : 0);
    if (val != 0) {
        if (BCMI_FT_GROUP(unit, id)->flags
                & BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Export"
                            " on ageout is not supported on Software"
                            " mananged Flowtracker group\n\r")));
            return BCM_E_CONFIG;
        }
    }

    /* export trigger on drop . */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                    bcmFlowtrackerExportTriggerDrop)) ? 1 : 0);
    if (val != 0) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Export"
                        " on drop is not supported\n\r")));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_ft_group_export_trigger_set
 * Purpose:
 *      Set export triggers on group.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      export_trigger_info - (IN) Export trigger info.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_export_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    uint32 val = 0;
    bsc_kg_group_table_entry_t kg_entry;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (!export_trigger_info) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_group_export_trigger_verify(unit,
                id, export_trigger_info));

    /* Set export trigger for new learn. */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
            bcmFlowtrackerExportTriggerNewLearn)) ? 1 : 0);

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
         MEM_BLOCK_ANY, id, &kg_entry));

    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
        EXPORT_LEARNf, val);

    /* Set export trigger for periodic timer. */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
            bcmFlowtrackerExportTriggerTimer)) ? 1 : 0);

    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
        PERIODIC_EXPORT_ENf, val);

    /* Write the entry back into group table. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ALL,
        id, &kg_entry));

    /* Set export trigger for age out. */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                    bcmFlowtrackerExportTriggerAgeOut)) ? 1 : 0);

    BCM_IF_ERROR_RETURN(bcmi_ft_group_age_export_trigger_set(unit, id, val));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_ft_group_export_trigger_get
 * Purpose:
 *      Get export triggers of group.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      export_trigger_info - (OUT) Export trigger info.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{

    bsc_kg_group_table_entry_t kg_entry;
    uint32 val = 0;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (!export_trigger_info) {
        return BCM_E_PARAM;
    }

    sal_memset(export_trigger_info, 0,
        sizeof(bcm_flowtracker_export_trigger_info_t));

    /* Read the group table. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
         MEM_BLOCK_ANY, id, &kg_entry));

    val = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
              EXPORT_LEARNf);

    /* Set export trigger for new learn. */
    if (val) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
            bcmFlowtrackerExportTriggerNewLearn);
    }

    val = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
              PERIODIC_EXPORT_ENf);

    /* Set export trigger for periodic timer. */
    if (val) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
            bcmFlowtrackerExportTriggerTimer);
    }

    /* Check export trigger for age out. */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_age_export_trigger_get(unit, id, &val));
    if (val) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
            bcmFlowtrackerExportTriggerAgeOut);
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_ft_group_cpu_notification_trigger_set
 * Purpose:
 *      Set cpu notificaiton on of group.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      notifications       - (IN) Export trigger info.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_cpu_notification_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_cpu_notification_info_t *notifications)
{

    bsc_kg_group_table_entry_t kg_entry;
    uint32 val = 0;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (!notifications) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &kg_entry));

    /* Notify CPU if flow exceeded than configured on group. */
    val = ((BCM_FLOWTRACKER_CPU_NOTIFICATION_GET(
        *notifications,
        bcmFlowtrackerCpuNotificationFlowExceeded)) ? 1 : 0);

    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
        FLOW_EXCEED_INTR_ENf, val);


    /* Notify CPU if Table is full and no new flow can be learnt. */
    val = ((BCM_FLOWTRACKER_CPU_NOTIFICATION_GET(
        *notifications,
        bcmbcmFlowtrackerCpuNotificationTableFull)) ? 1 : 0);

    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
        FT_TABLE_FULL_INTR_ENf, val);

    /* Write the entry back into group table. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ALL,
        id, &kg_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_ft_group_cpu_notification_trigger_get
 * Purpose:
 *      Get cpu notifications of group.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      notifications       - (OUT) Export trigger info.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_cpu_notification_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_cpu_notification_info_t *notifications)
{

    bsc_kg_group_table_entry_t kg_entry;
    uint32 val = 0;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (!notifications) {
        return BCM_E_PARAM;
    }

    /* Initialize all notifications to 0. */
    sal_memset(notifications, 0,
        sizeof(bcm_flowtracker_cpu_notification_info_t));

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &kg_entry));

    val = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
        FLOW_EXCEED_INTR_ENf);

    if (val) {
        BCM_FLOWTRACKER_CPU_NOTIFICATION_SET(
            *notifications,
            bcmFlowtrackerCpuNotificationFlowExceeded);
    }

    val = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
        FT_TABLE_FULL_INTR_ENf);

    if (val) {
        BCM_FLOWTRACKER_CPU_NOTIFICATION_SET(
            *notifications,
            bcmbcmFlowtrackerCpuNotificationTableFull);
    }

    /* Notify CPU if Table is full and no new flow can be learnt. */
    val = ((BCM_FLOWTRACKER_CPU_NOTIFICATION_GET(
        *notifications,
        bcmbcmFlowtrackerCpuNotificationTableFull)) ? 1 : 0);

    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, &kg_entry,
        FT_TABLE_FULL_INTR_ENf, val);

    /* Write the entry back into group table. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ALL,
        id, &kg_entry));

    return BCM_E_NONE;
}


int
bcmi_ft_group_element_print(
    int unit,
    bcm_flowtracker_tracking_param_type_t element)
{

    switch(element) {
    case bcmFlowtrackerTrackingParamTypeNone:
        LOG_CLI((BSL_META_U(unit, "None")));
        break;
    case bcmFlowtrackerTrackingParamTypeSrcIPv4:
        LOG_CLI((BSL_META_U(unit, "SrcIPv4")));
        break;
    case bcmFlowtrackerTrackingParamTypeDstIPv4 :
        LOG_CLI((BSL_META_U(unit, "DstIPv4")));
        break;
    case bcmFlowtrackerTrackingParamTypeSrcIPv6 :
        LOG_CLI((BSL_META_U(unit, "SrcIPv6")));
        break;
    case bcmFlowtrackerTrackingParamTypeDstIPv6 :
        LOG_CLI((BSL_META_U(unit, "DstIPv6")));
        break;
    case bcmFlowtrackerTrackingParamTypeL4SrcPort :
        LOG_CLI((BSL_META_U(unit, "L4SrcPort")));
        break;
    case bcmFlowtrackerTrackingParamTypeL4DstPort :
        LOG_CLI((BSL_META_U(unit, "L4DstPort")));
        break;
    case bcmFlowtrackerTrackingParamTypeIPProtocol :
        LOG_CLI((BSL_META_U(unit, "IPProto")));
        break;
    case bcmFlowtrackerTrackingParamTypePktCount :
        LOG_CLI((BSL_META_U(unit, "PktCount")));
        break;
    case bcmFlowtrackerTrackingParamTypeByteCount :
        LOG_CLI((BSL_META_U(unit, "ByteCount")));
        break;
    case bcmFlowtrackerTrackingParamTypeVRF :
        LOG_CLI((BSL_META_U(unit, "VRF")));
        break;
    case bcmFlowtrackerTrackingParamTypeTTL :
        LOG_CLI((BSL_META_U(unit, "TTL")));
        break;
    case bcmFlowtrackerTrackingParamTypeIPLength :
        LOG_CLI((BSL_META_U(unit, "IPLength")));
        break;
    case bcmFlowtrackerTrackingParamTypeTcpWindowSize :
        LOG_CLI((BSL_META_U
            (unit, "TcpWindowSize")));
        break;
    case bcmFlowtrackerTrackingParamTypeDosAttack:
        LOG_CLI((BSL_META_U
            (unit, "DosAttack")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
        LOG_CLI((BSL_META_U
            (unit, "InnerDosAttack")));
        break;
    case bcmFlowtrackerTrackingParamTypeVxlanNetworkId:
        LOG_CLI((BSL_META_U
            (unit, "VxlanNetworkId")));
        break;
    case bcmFlowtrackerTrackingParamTypeNextHeader:
        LOG_CLI((BSL_META_U(unit, "NextHeader")));
        break;
    case bcmFlowtrackerTrackingParamTypeHopLimit:
        LOG_CLI((BSL_META_U(unit, "HopLimit")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerSrcIPv4:
        LOG_CLI((BSL_META_U
            (unit, "InnerSrcIPv4")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerDstIPv4:
        LOG_CLI((BSL_META_U
            (unit, " InnerDstIPv4")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerSrcIPv6:
        LOG_CLI((BSL_META_U
            (unit, " InnerSrcIPv6")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerDstIPv6:
        LOG_CLI((BSL_META_U
            (unit, " InnerDstIPv6")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerIPProtocol:
        LOG_CLI((BSL_META_U
            (unit, " InnerIPProtocol")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerTTL:
        LOG_CLI((BSL_META_U(unit, " InnerTTL")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerNextHeader:
        LOG_CLI((BSL_META_U
            (unit, " InnerNextHeader")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerHopLimit:
        LOG_CLI((BSL_META_U
            (unit, " InnerHopLimit")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerL4SrcPort:
        LOG_CLI((BSL_META_U
            (unit, " InnerL4SrcPort")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerL4DstPort:
        LOG_CLI((BSL_META_U
            (unit, " InnerL4DstPort")));
        break;
    case bcmFlowtrackerTrackingParamTypeTcpFlags:
        LOG_CLI((BSL_META_U(unit, " TcpFlags")));
        break;
    case bcmFlowtrackerTrackingParamTypeOuterVlanTag:
        LOG_CLI((BSL_META_U
            (unit, " OuterVlanTag")));
        break;
    case bcmFlowtrackerTrackingParamTypeIP6Length:
        LOG_CLI((BSL_META_U(unit, " IP6Length")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerIPLength:
        LOG_CLI((BSL_META_U
            (unit, " InnerIPLength")));
        break;
    case bcmFlowtrackerTrackingParamTypeInnerIP6Length:
        LOG_CLI((BSL_META_U
            (unit, " InnerIP6Length")));
        break;
    case bcmFlowtrackerTrackingParamTypeTunnelClass:
        LOG_CLI((BSL_META_U
            (unit, "TunnelClass")));
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_ft_tracking_param_is_timestamp
 * Purpose:
 *      Get cpu notifications of group.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      t_param             - (IN) tracking parameter
 * Returns:
 *      TRUE/FALSE.
 */
int
bcmi_ft_tracking_param_is_timestamp(
        int unit,
        bcm_flowtracker_tracking_param_type_t t_param)
{

    switch (t_param) {
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
            return TRUE;
        default :
            return FALSE;
    }

    return FALSE;
}

/*
 * Function:
 *      bcmi_ft_group_check_ts_set
 * Purpose:
 *      Get cpu notifications of group.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) FLOW group id
 *      flowchecker_id      - (IN) check id.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcmi_ft_group_check_ts_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t flowchecker_id,
        bcmi_flowtracker_check_set_t reset)
{

    bcmi_flowtracker_flowchecker_info_t fc_info;
    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

    BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
        (unit, flowchecker_id, &fc_info));

    BCM_IF_ERROR_RETURN(bcmi_ft_group_ts_triggers_set(unit, id, fc_info.flags,
        BCMI_FT_TS_TYPE_FC, reset));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_ft_group_ts_triggers_set
 * Purpose:
 *      Get cpu notifications of group.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) FLOW group id
 *      check_flags         - (IN) flags from flow check
 *      trigger_type        - (IN) tracking param/flow check trigger type.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcmi_ft_group_ts_triggers_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 check_flags,
        int trigger_type,
        bcmi_flowtracker_check_set_t reset)
{
    int num_elements = 0;
    bcm_flowtracker_tracking_param_info_t *t_info = NULL;
    int ts_count = 0;
    int i = 0, j = 0;
    uint32 ts_triggers = 0;

    num_elements = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
    t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    ts_triggers = BCMI_FT_GROUP_TS_TRIGGERS(unit, id);

    if (trigger_type == BCMI_FT_TS_TYPE_TP) {

        /* Cleanup any old triggers for this type. */
        ts_triggers &=
            ~(BCMI_FT_GROUP_TS_NEW_LEARN | BCMI_FT_GROUP_TS_FLOW_START |
              BCMI_FT_GROUP_TS_FLOW_END);

        /* Set triggers based on the tracking params.*/
        for (i = 0; i < num_elements; i++) {
            if (bcmi_ft_tracking_param_is_timestamp(unit, t_info->param)) {
                for (j = 0; j < BCMI_FT_GROUP_MAX_TS_TYPE; j++) {
                    if (t_info->param != bcmi_ft_group_ts_info[j].ts_param) {
                        continue;
                    }

                    ts_triggers |= bcmi_ft_group_ts_info[j].flag_id;
                }
            }
            t_info++;
        }
    } else {
        /* ALU triggers can also be set from ALU methods. */
        if (check_flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1) {
            if (reset == bcmFlowtrackerFlowCheckTsSet) {
                ts_triggers |= BCMI_FT_GROUP_TS_CHECK_EVENT1;
            } else {
                ts_triggers &= ~(BCMI_FT_GROUP_TS_CHECK_EVENT1);
            }
        }

        if (check_flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2) {
            if (reset == bcmFlowtrackerFlowCheckTsSet) {
                ts_triggers |= BCMI_FT_GROUP_TS_CHECK_EVENT2;
            } else {
                ts_triggers &= ~(BCMI_FT_GROUP_TS_CHECK_EVENT2);
            }
        }
    }

    ts_count = 0;
    for (i = 0; i < BCMI_FT_GROUP_MAX_TS_TYPE; i++) {
        if ((ts_triggers) & (1 << i)) {
            ts_count++;
        }
    }

    if (ts_count > BCMI_FT_GROUP_MAX_TS) {
        return BCM_E_CONFIG;
    }

    BCMI_FT_GROUP_TS_TRIGGERS(unit, id) = ts_triggers;

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcmi_ft_group_get_param_type
 * Purpose:
 *      Get tracking parameter type from tracking parameters.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      param               - (IN) tracking parameter
 * Returns:
 *      bcmi_ft_group_param_type_t.
 */
bcmi_ft_group_param_type_t
bcmi_ft_group_get_param_type(
        int unit,
        bcm_flowtracker_tracking_param_type_t t_param)
{

    bcmi_ft_group_param_type_t param_type;

    param_type = bcmiFtGroupParamTypeTracking;

    switch (t_param) {
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
            param_type = bcmiFtGroupParamTypeTsNewLearn;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
            param_type = bcmiFtGroupParamTypeTsFlowStart;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
            param_type = bcmiFtGroupParamTypeTsFlowEnd;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
            param_type = bcmiFtGroupParamTypeTsCheckEvent1;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
            param_type = bcmiFtGroupParamTypeTsCheckEvent2;
            break;
        default :
            param_type = bcmiFtGroupParamTypeTracking;
    }

    return param_type;
}


/*
 * Function:
 *      bcmi_ft_group_get_tracking_param
 * Purpose:
 *      Get tracking parameter from tracking parameters type
 * Parameters:
 *      unit                - (IN) BCM device number
 *      param               - (IN) tracking parameter
 * Returns:
 *      bcm_flowtracker_tracking_param_type_t.
 */
bcm_flowtracker_tracking_param_type_t
bcmi_ft_group_get_tracking_param(
        int unit,
        bcmi_ft_group_param_type_t pram_type)
{

    bcm_flowtracker_tracking_param_type_t param;

    param = bcmFlowtrackerTrackingParamTypeNone;

    switch (pram_type) {
        case bcmiFtGroupParamTypeTsNewLearn:
            param = bcmFlowtrackerTrackingParamTypeTimestampNewLearn;
            break;
        case bcmiFtGroupParamTypeTsFlowStart:
            param = bcmFlowtrackerTrackingParamTypeTimestampFlowStart;
            break;
        case bcmiFtGroupParamTypeTsFlowEnd:
            param = bcmFlowtrackerTrackingParamTypeTimestampFlowEnd;
            break;
        case bcmiFtGroupParamTypeTsCheckEvent1:
            param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            break;
        case bcmiFtGroupParamTypeTsCheckEvent2:
            param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2;
            break;
        default :
            param = bcmFlowtrackerTrackingParamTypeNone;
    }

    return param;
}

/*
 * Function:
 *      bcmi_ft_group_get_ts_type
 * Purpose:
 *      Get tracking parameter type matching a field.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) flow Group id.
 *      fid                 - (IN) ts data field.
 * Returns:
 *      bcmi_ft_group_param_type_t
 */
bcmi_ft_group_param_type_t
bcmi_ft_group_get_ts_type(
        int unit,
        bcm_flowtracker_group_t id,
        soc_field_t fid)
{
    bsc_dg_group_timestamp_profile_entry_t entry;
    bsc_dg_group_table_entry_t dg_entry;
    uint32 index=0;
    uint32 fmt[1];
    int i=0;
    uint32 bit_sel = 0;
/*
    soc_field_t ts_data_fields[4] =
                    {DATA_0f, DATA_1f, DATA_2f, DATA_3f};
*/

    /* Initialize the timestamp profile entry. */
    sal_memset(&entry, 0,
        sizeof(bsc_dg_group_timestamp_profile_entry_t));
    /* Initialize group entry. */
    sal_memset(&dg_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_entry));

    /* now write this new index into the group. */
    index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
        ((uint32 *)&dg_entry), TIMESTAMP_PROFILE_IDXf);

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TIMESTAMP_PROFILEm,
            MEM_BLOCK_ANY, index, &entry));


    /* Set the entry. */
    soc_mem_field_get(unit, BSC_DG_GROUP_TIMESTAMP_PROFILEm,
        (uint32 *)(&entry), fid, fmt);

    bit_sel = soc_format_field32_get(unit, BSC_KMAP_CONTROLfmt, fmt,
                  KMAP_INPUT_SELECT_0f);

    for (; i<BCMI_FT_GROUP_MAX_TS_TYPE; i++) {
        if (bit_sel == bcmi_ft_group_ts_info[i].ts_trigger_bit_sel) {
            return bcmi_ft_group_get_param_type(unit,
                 bcmi_ft_group_ts_info[i].ts_param);
         }
    }

    return bcmiFtGroupParamTypeTracking;

}

/*
 * Function:
 *      bcmi_ft_group_entry_add_check
 * Purpose:
 *      Check if Flowtracker Group is ready to be added to ftfp entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      id - (IN) Flowtracker flow group ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_ft_group_entry_add_check(
    int unit,
    bcm_flowtracker_group_t id)
{
    int rv;
    int ft_enable;
    int num_key_info = 0;
    uint32 flow_limit;

    /* Check if FT Group created */
    rv = bcmi_ft_group_is_invalid(unit, id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit, "FP(unit %d)"
                        " Error: Flowtracker Group %d is not"
                        " created.\n"), unit, id));
        return rv;
    }

    /* Check if FT Group template params extracted */
    rv = bcmi_esw_ft_group_extraction_alu_info_get(unit,
            id, 1, 0, NULL, &num_key_info);
    if ((rv != BCM_E_NONE) || (num_key_info == 0)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit, "FP(unit %d)"
                        "Error: Flowtracker Group %d is not"
                        " validated.\n"), unit, id));
        return BCM_E_CONFIG;
    }

    /* If Flowtracking is disabled or flow_limit is 0, pass */
    ft_enable = BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack;
    rv = bcmi_ft_group_flow_limit_get(unit, id, &flow_limit);
    if (rv != BCM_E_NONE) {
        flow_limit = 0;
    }
    if ((ft_enable == FALSE) || (flow_limit == 0)) {
        return BCM_E_NONE;
    }

    /* Check if collector is attached */
    rv = bcmi_ft_group_collector_is_valid(unit, id);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Error: Collector is not "
                        "associated with FlowtrackerGroup %d\r\n"),
                    unit, id));
    }

    return rv;
}

/*
 * Function:
 *      bcmi_ft_group_is_check_valid
 * Purpose:
 *      Check if FlowCheck is associated with ft group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      id - (IN) Flowtracker flow group ID.
 *      check_id - (IN) Check Id to verify
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcmi_ft_group_is_check_valid(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_check_t check_id)
{
    bcmi_ft_flowchecker_list_t *check_list = NULL;

    check_list = BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id);
    if (check_list == NULL) {
        return BCM_E_PARAM;
    }

    while (check_list != NULL) {
        if (check_list->flowchecker_id == check_id) {
            return BCM_E_NONE;
        }
        check_list = check_list->next;
    }

    return BCM_E_PARAM;
}

/*
 * Function:
 *      bcmi_ft_group_export_checks_validate
 * Purpose:
 *      Verify flowchecks in in export list.
 * Parameters:
 *      unit - (IN) Unit number.
 *      id - (IN) Flowtracker flow group ID.
 *      num_in_export_elements - Number of export elements.
 *      in_export_elements - (IN) List of export elements.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_ft_group_export_checks_validate(
    int unit,
    bcm_flowtracker_group_t id,
    int num_in_export_elements,
    bcm_flowtracker_export_element_info_t *in_export_elements)
{
    int rv = BCM_E_NONE;
    int iter = 0, iter1 = 0;
    int options = 0;
    int num_trackin_params = 0;
    bcm_flowtracker_check_t check_id;
    bcm_flowtracker_export_element_info_t *export_elem_info = NULL;
    bcm_flowtracker_tracking_param_info_t *tracking_param_list = NULL;

    tracking_param_list = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    num_trackin_params = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
    options = BCMI_FT_GROUP_CHECK_MATCH_PRIMARY |
              BCMI_FT_GROUP_CHECK_MATCH_GROUP;

    for (iter = 0; iter < num_in_export_elements; iter++) {
        export_elem_info = &in_export_elements[iter];

        /* Skip if export element is not flowcheck. */
        if (export_elem_info->element !=
                bcmFlowtrackerExportElementTypeFlowtrackerCheck) {
            continue;
        }

        check_id = export_elem_info->check_id;

        /* Validate if Check_id is allowed for the given list */
        rv = bcmi_ft_group_check_validate(unit, options, id, check_id);
        BCM_IF_ERROR_RETURN(rv);

        /* Make sure that tracking param list also has check_id */
        if (BCMI_FT_IDX_IS_PRIMARY_CHECK(check_id)) {
            for (iter1 = 0; iter1 < num_trackin_params; iter1++) {
                if ((tracking_param_list[iter1].param ==
                            bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) &&
                        (check_id == tracking_param_list[iter1].check_id)) {
                    break;
                }
            }
            if (iter1 == num_trackin_params) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Given Flow Check(0x%x) is missing from"
                                " tracking params input list of Group(%d)\n"),
                            check_id, id));
                return BCM_E_PARAM;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_ft_group_norm_tracking_params_list_get
 * Purpose:
 *      Get the list of tracking_param info eligible for
 *      normalization in the given flowtracker group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      id - (IN) Flowtracker flow group ID.
 *      norm_tracking_params_info - (OUT) list of norm tracking params.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      The returned 'norm_tracking_params_info' is ordered based on
 *      priority of normalization and if some params are not present
 *      in the given FT Group, it is set to NULL.
 */
int
bcmi_ft_group_norm_tracking_params_info_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_tracking_param_info_t **norm_tracking_params_info)
{
    int i = 0, j = 0;
    int num_tracking_params = 0;
    bcm_flowtracker_tracking_param_info_t *tracking_params_info = NULL;

    /* Tracking params pairs for normalization ordered in the priority
     * param used as base of normalization. */
    bcm_flowtracker_tracking_param_type_t
        norm_tracking_pairs[BCMI_FT_GROUP_NORM_TRACKING_PARAMS] = {
            bcmFlowtrackerTrackingParamTypeSrcIPv4,
            bcmFlowtrackerTrackingParamTypeDstIPv4,
            bcmFlowtrackerTrackingParamTypeSrcIPv6,
            bcmFlowtrackerTrackingParamTypeDstIPv6,
            bcmFlowtrackerTrackingParamTypeInnerSrcIPv4,
            bcmFlowtrackerTrackingParamTypeInnerDstIPv4,
            bcmFlowtrackerTrackingParamTypeInnerSrcIPv6,
            bcmFlowtrackerTrackingParamTypeInnerDstIPv6,
            bcmFlowtrackerTrackingParamTypeL4SrcPort,
            bcmFlowtrackerTrackingParamTypeL4DstPort,
            bcmFlowtrackerTrackingParamTypeInnerL4SrcPort,
            bcmFlowtrackerTrackingParamTypeInnerL4DstPort
        };

    tracking_params_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    num_tracking_params = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);

    for (i = 0; i < COUNTOF(norm_tracking_pairs); i++) {
        for (j = 0; j < num_tracking_params; j++) {
            if (tracking_params_info[j].flags &
                    BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
                if (tracking_params_info[j].param == norm_tracking_pairs[i]) {
                    norm_tracking_params_info[i] = &tracking_params_info[j];
                    break;
                }
            }
        }
    }

    return BCM_E_NONE;
}

#else /* BCM_FLOWTRCAKER_SUPPORT*/
int bcmi_ft_group_not_empty;
#endif /* BCM_FLOWTRCAKER_SUPPORT */

