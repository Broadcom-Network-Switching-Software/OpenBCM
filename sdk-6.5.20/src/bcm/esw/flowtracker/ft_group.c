/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_group.c
 * Purpose:     The purpose of this file is to set flow tracker group methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_field.h>
#include <bcm_int/esw/flowtracker/ft_common.h>
#include <bcm_int/esw/flowtracker/ft_interface.h>
#include <bcm_int/esw/flowtracker/ft_user.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)

#define BCMI_FTv2_STAGE_PDE  (1 << 0)
#define BCMI_FTv2_EXPORT_PDE (1 << 1)

const soc_field_t pde_field[64] = {
    SHIFT_AMOUNT_BYTE_0f, SHIFT_AMOUNT_BYTE_1f, SHIFT_AMOUNT_BYTE_2f,
    SHIFT_AMOUNT_BYTE_3f, SHIFT_AMOUNT_BYTE_4f, SHIFT_AMOUNT_BYTE_5f,
    SHIFT_AMOUNT_BYTE_6f, SHIFT_AMOUNT_BYTE_7f, SHIFT_AMOUNT_BYTE_8f,
    SHIFT_AMOUNT_BYTE_9f, SHIFT_AMOUNT_BYTE_10f, SHIFT_AMOUNT_BYTE_11f,
    SHIFT_AMOUNT_BYTE_12f, SHIFT_AMOUNT_BYTE_13f, SHIFT_AMOUNT_BYTE_14f,
    SHIFT_AMOUNT_BYTE_15f, SHIFT_AMOUNT_BYTE_16f, SHIFT_AMOUNT_BYTE_17f,
    SHIFT_AMOUNT_BYTE_18f, SHIFT_AMOUNT_BYTE_19f, SHIFT_AMOUNT_BYTE_20f,
    SHIFT_AMOUNT_BYTE_21f, SHIFT_AMOUNT_BYTE_22f, SHIFT_AMOUNT_BYTE_23f,
    SHIFT_AMOUNT_BYTE_24f, SHIFT_AMOUNT_BYTE_25f, SHIFT_AMOUNT_BYTE_26f,
    SHIFT_AMOUNT_BYTE_27f, SHIFT_AMOUNT_BYTE_28f, SHIFT_AMOUNT_BYTE_29f,
    SHIFT_AMOUNT_BYTE_30f, SHIFT_AMOUNT_BYTE_31f, SHIFT_AMOUNT_BYTE_32f,
    SHIFT_AMOUNT_BYTE_33f, SHIFT_AMOUNT_BYTE_34f, SHIFT_AMOUNT_BYTE_35f,
    SHIFT_AMOUNT_BYTE_36f, SHIFT_AMOUNT_BYTE_37f, SHIFT_AMOUNT_BYTE_38f,
    SHIFT_AMOUNT_BYTE_39f, SHIFT_AMOUNT_BYTE_40f, SHIFT_AMOUNT_BYTE_41f,
    SHIFT_AMOUNT_BYTE_42f, SHIFT_AMOUNT_BYTE_43f, SHIFT_AMOUNT_BYTE_44f,
    SHIFT_AMOUNT_BYTE_45f, SHIFT_AMOUNT_BYTE_46f, SHIFT_AMOUNT_BYTE_47f,
    SHIFT_AMOUNT_BYTE_48f, SHIFT_AMOUNT_BYTE_49f, SHIFT_AMOUNT_BYTE_50f,
    SHIFT_AMOUNT_BYTE_51f, SHIFT_AMOUNT_BYTE_52f, SHIFT_AMOUNT_BYTE_53f,
    SHIFT_AMOUNT_BYTE_54f, SHIFT_AMOUNT_BYTE_55f, SHIFT_AMOUNT_BYTE_56f,
    SHIFT_AMOUNT_BYTE_57f, SHIFT_AMOUNT_BYTE_58f, SHIFT_AMOUNT_BYTE_59f,
    SHIFT_AMOUNT_BYTE_60f, SHIFT_AMOUNT_BYTE_61f, SHIFT_AMOUNT_BYTE_62f,
    SHIFT_AMOUNT_BYTE_63f
};

#define _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(_u_, _idx_, _size_, \
                                          _pde_idx_, _pde_, _end_,            \
                                          _fill_size_, _fill_num_,            \
                                          _max_fill_size_, _max_fill_num_,    \
                                          _offset_jump_)                      \
do {                                                                          \
    for (_idx_ = 0; _idx_ < _size_; _idx_++) {                                \
        if (_fill_size_ == 0 && _fill_num_ < _max_fill_num_) {                \
            _pde_idx_ -= _offset_jump_;                                       \
            _fill_size_ = _max_fill_size_;                                    \
            _fill_num_++;                                                     \
        }                                                                     \
        _fill_size_--;                                                        \
        if (_fill_size_ < 0) {                                                \
            return BCM_E_RESOURCE;                                            \
        }                                                                     \
        soc_mem_field32_set(_u_, BSC_DT_PDE_PROFILE_0m, _pde_,                \
            pde_field[_pde_idx_], (((_end_) - (_idx_ * 8)) / 8));             \
        _pde_idx_--;                                                          \
    }                                                                         \
} while (0)                                                                   \

/*
 * Internal Database for timestamp management
 * Note - Indexed as per bcmFlowtrackerTimestampSourcexxx enum
 * Format - DelayMode, ToD Profile TimeStamp, Check Timestamp , Trigger
 * For NewLearn, FlowStart, FlowEnd, CheckEvent1-4 source,
 * Hw timestamp value is not picked from this mapping.
 * It will be computed run time based on bank allocated.
 */
bcmi_ft_group_ts_source_t
    bcmi_ft_group_ts_source_info[bcmFlowtrackerTimestampSourceCount] =
   {/* Timestamp Source Legacy */           {2, 0,  9, 0},
    /* Timestamp Source NTP */              {1, 1,  7, 0},
    /* Timestamp Source PTP */              {0, 2,  8, 0},
    /* Timestamp Source CMIC */             {2, 3,  6, 0},
    /* Timestamp Source Ingress FT Stage */ {3, 0, 10, 0},
    /* Timestamp Source Packet */           {2, 4,  0, 0},
    /* Timestamp Source Ingress */          {2, 5,  1, 0},
    /* Timestamp Source NewLearn */         {2, 0,  9, BCMI_FT_GROUP_TS_NEW_LEARN},
    /* Timestamp Source FlowStart */        {2, 0,  9, BCMI_FT_GROUP_TS_FLOW_START},
    /* Timestamp Source FlowEnd */          {2, 0,  9, BCMI_FT_GROUP_TS_FLOW_END},
    /* Timestamp Source CheckEvent1 */      {2, 0,  9, BCMI_FT_GROUP_TS_CHECK_EVENT1},
    /* Timestamp Source CheckEvent2 */      {2, 0,  9, BCMI_FT_GROUP_TS_CHECK_EVENT2},
    /* Timestamp Source CheckEvent3 */      {2, 0,  9, BCMI_FT_GROUP_TS_CHECK_EVENT3},
    /* Timestamp Source CheckEvent4 */      {2, 0,  9, BCMI_FT_GROUP_TS_CHECK_EVENT4},
    /* Timestamp Source CMIC Tx Start */    {2, 5,  1, 0},
    /* Timestamp Source CMIC Tx End */      {2, 3,  6, 0}};
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

/*
 * Global defines.
 */
bcmi_ft_group_flush_info_t
    bcmi_ft_group_flush_fields[BCMI_FT_GROUP_FLUSH_MAX_SCALE] =
    {{GROUP_ID0_VALIDf, GROUP_ID0f},
     {GROUP_ID1_VALIDf, GROUP_ID1f},
     {GROUP_ID2_VALIDf, GROUP_ID2f},
     {GROUP_ID3_VALIDf, GROUP_ID3f}};

/* Internal flowtracker group info db */
bcmi_ft_group_info_db_t bcmi_ft_group_info_db[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *   bcmi_ft_tracking_param_is_timestamp
 * Purpose:
 *   check if provided tracking param is timestamp related or not
 * Parameters:
 *   unit     - (IN) BCM device number
 *   t_param  - (IN) tracking parameter
 * Returns:
 *   TRUE/FALSE
 */
int
bcmi_ft_tracking_param_is_timestamp(int unit,
        bcm_flowtracker_tracking_param_type_t t_param)
{
    switch (t_param) {
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:
        case bcmFlowtrackerTrackingParamTypeTimestampIngress:
        case bcmFlowtrackerTrackingParamTypeTimestampEgress:
            return TRUE;
        default:
            return FALSE;
    }

    return FALSE;
}

/*
 * Function:
 *   bcmi_ft_tracking_param_is_delay
 * Purpose:
 *   check if provided tracking param is delay related or not
 * Parameters:
 *   unit     - (IN) BCM device number
 *   t_param  - (IN) tracking parameter
 * Returns:
 *   TRUE/FALSE
 */
int
bcmi_ft_tracking_param_is_delay(int unit,
        bcm_flowtracker_tracking_param_type_t t_param)
{
    switch (t_param) {
        case bcmFlowtrackerTrackingParamTypeChipDelay:
        case bcmFlowtrackerTrackingParamTypeE2EDelay:
        case bcmFlowtrackerTrackingParamTypeIPATDelay:
        case bcmFlowtrackerTrackingParamTypeIPDTDelay:
            return TRUE;
        default :
            return FALSE;
    }

    return FALSE;
}

/*
 * Function:
 *   bcmi_ft_group_state_clear
 * Purpose:
 *   Clear Flowtracker group state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
void
bcmi_ft_group_state_clear(int unit)
{
    bcm_flowtracker_group_type_t group_type = 0;

    for (group_type = bcmFlowtrackerGroupTypeNone;
            group_type < bcmFlowtrackerGroupTypeCount;
            group_type++) {

        if (BCMI_FT_GROUP_SW_STATE(unit, group_type) != NULL) {
            sal_free(BCMI_FT_GROUP_SW_STATE(unit, group_type));
            BCMI_FT_GROUP_SW_STATE(unit, group_type) = NULL;
        }

        if (BCMI_FT_GROUP_BITMAP(unit, group_type) != NULL) {
            sal_free(BCMI_FT_GROUP_BITMAP(unit, group_type));
            BCMI_FT_GROUP_BITMAP(unit, group_type) = NULL;
        }
    }

    return;
}

/*
 * Function:
 *   bcmi_ft_group_state_init
 * Purpose:
 *   Initialize Flowtracker group state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
int bcmi_ft_group_state_init(int unit)
{
    int rv = BCM_E_NONE;
    int num_groups = 0;
    soc_mem_t mem = INVALIDm;
    bcm_flowtracker_group_type_t group_type;

    /* Micro Flowtracker Groups */
    group_type = bcmFlowtrackerGroupTypeNone;
    rv = bcmi_ft_group_type_mem_get(unit, 1, group_type, &mem);
    BCM_IF_ERROR_RETURN(rv);

    num_groups = soc_mem_index_count(unit, mem);

    BCMI_FT_ALLOC_IF_ERR_CLEANUP(
            BCMI_FT_GROUP_SW_STATE(unit, group_type),
            num_groups * sizeof(bcmi_ft_group_sw_info_t *),
            "ft group sw state");

    BCMI_FT_ALLOC_IF_ERR_CLEANUP(
            BCMI_FT_GROUP_BITMAP(unit, group_type),
            SHR_BITALLOCSIZE(num_groups), "ft_group_bitmap");

    BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type) = num_groups;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

        /* Aggregate Flowtracker Groups */
        for (group_type = bcmFlowtrackerGroupTypeAggregateIngress;
                group_type < bcmFlowtrackerGroupTypeCount;
                group_type++) {

            rv = bcmi_ft_group_type_mem_get(unit, 1, group_type, &mem);
            BCMI_CLEANUP_IF_ERROR(rv);

            if (mem == INVALIDm) {
                BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type) = 0;
                continue;
            }

            num_groups = soc_mem_index_count(unit, mem);

            BCMI_FT_ALLOC_IF_ERR_CLEANUP(
                    BCMI_FT_GROUP_SW_STATE(unit, group_type),
                    num_groups * sizeof(bcmi_ft_group_sw_info_t *),
                    "ft group sw state");

            BCMI_FT_ALLOC_IF_ERR_CLEANUP(
                    BCMI_FT_GROUP_BITMAP(unit, group_type),
                    SHR_BITALLOCSIZE(num_groups), "ft_group_bitmap");

            BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type) = num_groups;
        }
    }
#endif

cleanup:

    if (rv != BCM_E_NONE) {
        bcmi_ft_group_state_clear(unit);
    }
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_get_free_index
 * Purpose:
 *   get free index for the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (OUT) FT group id.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_get_free_index(int unit,
    bcm_flowtracker_group_type_t group_type,
    int *index)
{
    int i = 0;
    int num_groups = 0;

    /* Check Group type range */
    if ((group_type < bcmFlowtrackerGroupTypeNone) ||
            (group_type >= bcmFlowtrackerGroupTypeCount)) {
        return BCM_E_PARAM;
    }

    /* Total number of groups for group_type in device. */
    num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);

    if (*index != -1) {
        /*
         * if valid index is sent, then use it.
         * Index free check is already done.
         */
        i = *index;
    }
    if ((i == 0) && (group_type == bcmFlowtrackerGroupTypeAggregateIngress)) {
        i++;
    }

    for (; i < num_groups; i++) {
        if (!(SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit, group_type), i))) {
             /* Got the free index. use it. */
            SHR_BITSET(BCMI_FT_GROUP_BITMAP(unit, group_type), i);
            *index = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *   bcmi_ft_group_is_invalid
 * Purpose:
 *   Check if the group is invalid.
 * Parameters:
 *   unit - (IN) BCM device id
 *   group_index   - (OUT) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_is_invalid(int unit,
            bcm_flowtracker_group_t id)
{
    int num_groups = 0;
    int group_index = 0;
    bcm_flowtracker_group_type_t group_type;

    if (!ft_initialized[unit]) {
        return BCM_E_INIT;
    }

    group_type = BCMI_FT_GROUP_TYPE_GET(id);

    /* Check Group type range */
    if ((group_type < bcmFlowtrackerGroupTypeNone) ||
            (group_type >= bcmFlowtrackerGroupTypeCount)) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_GROUP_SW_STATE(unit, group_type) == NULL) {
        return BCM_E_UNAVAIL;
    }

    group_index = BCMI_FT_GROUP_INDEX_GET(id);
    num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);

    if ((group_index < 0) || (group_index >= num_groups)) {
        return BCM_E_PARAM;
    }

    if (group_type == bcmFlowtrackerGroupTypeAggregateIngress) {
        if (group_index == 0) {
            return BCM_E_PARAM;
        }
    }

    if (!(SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit, group_type), group_index))) {
        return BCM_E_NOT_FOUND;
    }

    if (BCMI_FT_GROUP(unit, id) == NULL) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_collector_is_valid
 * Purpose:
 *   Check if collector is associated with ft group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_collector_is_valid(int unit,
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
 *   bcmi_ft_group_create
 * Purpose:
 *   Create flowtracker group id.
 * Description :
 *   This function only creates a sw group index
 *   and allocates sw memory to store groups information.
 * Parameters:
 *   unit            - (IN) BCM device id
 *   options         - (IN) options for group creation
 *   id              - (OUT) FT group id.
 *   flow_group_info - (IN) Information of group.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_create(int unit,
                uint32 options,
                bcm_flowtracker_group_t *id,
                bcm_flowtracker_group_info_t *flow_group_info)
{
    int rv = BCM_E_NONE;
    int num_alu32 = 0;
    int create = 1, group_index = -1;
    bcm_flowtracker_group_t flow_group;
    bcm_flowtracker_group_type_t group_type;
    bcm_flowtracker_group_stat_t group_stats;

    if ((id == NULL) || (flow_group_info == NULL)) {
        return BCM_E_PARAM;
    }

    if (!ft_initialized[unit]) {
        return BCM_E_INIT;
    }

    /* Aggregate Group Type not allowed without this feature */
    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (flow_group_info->group_type != bcmFlowtrackerGroupTypeNone) {
            return BCM_E_PARAM;
        }
    }

    if ((options & BCM_FLOWTRACKER_GROUP_REPLACE) &&
       (!(options & BCM_FLOWTRACKER_GROUP_WITH_ID))) {
        return BCM_E_PARAM;
    }

    if (options & BCM_FLOWTRACKER_GROUP_REPLACE) {
        /* Check if valid group and id range. */
        rv = bcmi_ft_group_is_invalid(unit, *id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (BCMI_FT_GROUP(unit, *id)) {
            if (BCMI_FT_GROUP_IS_BUSY(unit, *id)) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Flowtracker group is in use.\n ")));
                return BCM_E_CONFIG;
            }
        }

        group_type = BCM_FLOWTRACKER_GROUP_TYPE_GET(*id);
        if (group_type != flow_group_info->group_type) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "Flowtracker Group type mismatch.\n ")));
            return BCM_E_CONFIG;
        }
        flow_group = *id;

    } else {
        if (options & BCM_FLOWTRACKER_GROUP_WITH_ID) {

            /* Check if valid group and id range. */
            rv = bcmi_ft_group_is_invalid(unit, *id);
            if (BCM_SUCCESS(rv)) {
                return (BCM_E_EXISTS);
            }

            /* Anything other than NOT_FOUND is error */
            if ((rv != BCM_E_NOT_FOUND) && BCM_FAILURE(rv)) {
                return rv;
            }

            group_type = BCM_FLOWTRACKER_GROUP_TYPE_GET(*id);
            if (group_type != flow_group_info->group_type) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Flowtracker Group type mismatch.\n ")));
                return BCM_E_CONFIG;
            }

            group_index = BCMI_FT_GROUP_INDEX_GET(*id);
        }

        group_type = flow_group_info->group_type;

        /*  Allocating a new group. Get a free group index. */
        rv = bcmi_ft_group_get_free_index(unit,
                group_type, &group_index);
        BCM_IF_ERROR_RETURN(rv);

        BCM_FLOWTRACKER_GROUP_ID_SET(flow_group, group_type, group_index);

        /* Allocate the sw state for this group first. */
        BCMI_FT_GROUP(unit, flow_group) = (bcmi_ft_group_sw_info_t *)
            sal_alloc(sizeof(bcmi_ft_group_sw_info_t),
                    "flowtracker group sw state");

        if (BCMI_FT_GROUP(unit, flow_group) == NULL) {
            return BCM_E_MEMORY;
        }

        /* memset everything to zero. */
        sal_memset(BCMI_FT_GROUP(unit, flow_group),
                0, sizeof(bcmi_ft_group_sw_info_t));

        /* Allocate memory for ALU32_USE */
        if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(flow_group)) {
            num_alu32 = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32);
        } else {
            num_alu32 = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32);
        }

        BCMI_FT_ALLOC_IF_ERR_CLEANUP(
                BCMI_FT_GROUP_ALU32_MEM_USE(unit, flow_group),
                sizeof(uint32) * num_alu32, "ft group ali32 use");
    }

    rv = bcmi_ft_group_entry_update(unit, flow_group, flow_group_info, create);
    BCMI_CLEANUP_IF_ERROR(rv);

    if (!BCMI_FT_GROUP_TYPE_IS_AGGREGATE(flow_group)) {
        /* Set by default direction of group as forward. */
        bcmi_flowtracker_group_control_set(unit, flow_group,
                bcmFlowtrackerGroupControlFlowDirection,
                bcmFlowtrackerFlowDirectionForward);

        /* Set by default, flowtrack and new learn control */
        BCMI_FT_GROUP_FTFP_DATA(unit, flow_group).new_learn = TRUE;
        BCMI_FT_GROUP_FTFP_DATA(unit, flow_group).flowtrack = TRUE;

        /* Copy flags. */
        if (flow_group_info->group_flags & BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
            (BCMI_FT_GROUP(unit, flow_group))->flags |=
                BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY;
        }
        if (flow_group_info->group_flags &
                BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
            BCMI_FT_GROUP_FTFP_DATA(unit, flow_group).new_learn = 0;
        }

        /* Clear up old stats */
        bcm_flowtracker_group_stat_t_init(&group_stats);
        rv = bcm_esw_flowtracker_group_stat_set(unit, flow_group, &group_stats);
        BCMI_CLEANUP_IF_ERROR(rv);
    } else {

        /* Set by default, flowtrack and new learn control */
        BCMI_FT_GROUP_FTFP_DATA(unit, flow_group).flowtrack = TRUE;
        BCMI_FT_GROUP_FTFP_DATA(unit, flow_group).new_learn = FALSE;
        BCMI_FT_GROUP_FTFP_DATA(unit, flow_group).direction =
                                    bcmFlowtrackerFlowDirectionForward;

        /* Copy flags. */
        (BCMI_FT_GROUP(unit, flow_group))->flags |= BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY;
    }

    /* Assign invalid ids at group creation. */
    (BCMI_FT_GROUP(unit, flow_group))->template_id = BCMI_FT_TEMPLATE_ID_INVALID;
    (BCMI_FT_GROUP(unit, flow_group))->collector_id = BCMI_FT_COLLECTOR_ID_INVALID;

    *id = flow_group;

    return BCM_E_NONE;

cleanup:
    bcmi_ft_group_clear(unit, flow_group);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_entry_update
 * Purpose:
 *   update group entry
 * Description :
 *   This function updates group entry in hardware during
 *   create & destroy.
 * Parameters:
 *   unit            - (IN) BCM device id
 *   id              - (IN) FT group id.
 *   flow_group_info - (IN) Information of group.
 *   create          - (IN) create flag
 * Returns:
 *   BCM_E_XXX
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

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(group_id))) {
        return bcmi_ftv3_group_aggregate_entry_update(unit,
                group_id, info, create);
    }
#endif

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
                BCMI_FT_GROUP_INDEX_GET(group_id), &kg_group_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Write into the hardware. */
    rv = soc_mem_write(unit, BSC_DG_GROUP_TABLEm, MEM_BLOCK_ALL,
                BCMI_FT_GROUP_INDEX_GET(group_id), &dg_group_entry);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_get
 * Purpose:
 *   Get information of flowtracker group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   flow_group_info - (IN) Information of group.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_get(int unit,
                  bcm_flowtracker_group_t id,
                  bcm_flowtracker_group_info_t *flow_group_info)
{
    bsc_dg_group_table_entry_t dg_group_entry;

    if (flow_group_info == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    if (BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeNone) {
        sal_memset(&dg_group_entry, 0, sizeof(bsc_dg_group_table_entry_t));

        /* First read the group entry. */
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                    MEM_BLOCK_ANY, id, &dg_group_entry));

        flow_group_info->group_class =
            soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm, &dg_group_entry,
                    CLASS_IDf);
    }

    if (BCMI_FT_GROUP(unit, id)->flags &
            BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
        flow_group_info->group_flags |=
            BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY;
    }

    flow_group_info->group_type = BCMI_FT_GROUP_TYPE_GET(id);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_clear
 * Purpose:
 *   Clear flowtracker group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (OUT) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_clear(int unit,
                    bcm_flowtracker_group_t id)
{
    int group_index = -1;
    bcm_flowtracker_group_type_t group_type;

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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

    if (BCMI_FT_GROUP_ALU32_MEM_USE(unit, id) != NULL) {
        sal_free(BCMI_FT_GROUP_ALU32_MEM_USE(unit, id));
        BCMI_FT_GROUP_ALU32_MEM_USE(unit, id) = NULL;
    }

    group_index = BCM_FLOWTRACKER_GROUP_INDEX_GET(id);
    group_type = BCM_FLOWTRACKER_GROUP_TYPE_GET(id);

    /* Clear the sw state. */
    sal_free(BCMI_FT_GROUP(unit, id));
    BCMI_FT_GROUP(unit, id) = NULL;

    /* Free the bitmap. */
    SHR_BITCLR(BCMI_FT_GROUP_BITMAP(unit, group_type), group_index);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_learn_counter_nonzero
 * Purpose:
 *     Check if learn counter of the flow group is non-zero.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     id   - (IN) Flowtracker Group.
 *
 * Returns:
 *     TRUE - for Non-zero learn counter
 *     FALSE - for Zero learn counter
 */
STATIC int
bcmi_ft_group_learn_counter_nonzero(
    int unit,
    bcm_flowtracker_group_t id)
{
    uint64 val;

    COMPILER_64_ZERO(val);

    /* Aggregate Group does not support learning. */
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return FALSE;
    }

    /* Read Learn Counter */
    BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
         SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT,
         BCMI_FT_GROUP_INDEX_GET(id), &val));

    if (COMPILER_64_IS_ZERO(val)) {
        return FALSE;
    }

    return TRUE;
}

/*
 * Function:
 *   bcmi_ft_group_destroy
 * Purpose:
 *   Destroy flowtracker group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_destroy(int unit,
                      bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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

    /* Check User entries count for SwInsert Group */
    if (BCMI_FT_GROUP(unit, id)->flags
            & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
        if (bcmi_ft_group_learn_counter_nonzero(unit, id)) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Deleting Flow Group (%d) while user entries"
                        " is configured.\n "), id));
            return BCM_E_BUSY;
        }
    }

    /* Uninstall the group */
    if (BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
        BCM_IF_ERROR_RETURN
            (bcmi_esw_ft_group_hw_uninstall(unit, id));
    }

    rv = bcmi_ft_group_profiles_destroy(unit, id);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_ft_group_entry_update(unit, id, NULL, 0);
    BCM_IF_ERROR_RETURN(rv);

    bcmi_ft_group_clear(unit, id);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_get_all
 * Purpose:
 *   Returns the list of flowtracker group created in the system.
 * Parameters:
 *   unit       - (IN) BCM device number
 *   max_size   - (IN) Size of array 'idz'
 *   idz        - (OUT) Array of flowtracker group.
 *   list_size  - (OUT) Numbe of indices filled in 'idz'
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_get_all(int unit,
                    int max_size,
                    bcm_flowtracker_group_t *idz,
                    int *list_size)
{
    int i = 0, j = 0;
    int max_avail_groups = 0, valid_groups = 0;
    bcm_flowtracker_group_type_t group_type;

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if ((max_size != 0) && (idz == NULL)) {
        return BCM_E_PARAM;
    }

    if (list_size == NULL) {
        return BCM_E_PARAM;
    }

    max_avail_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit,
            bcmFlowtrackerGroupTypeNone);
    SHR_BITCOUNT_RANGE(
            BCMI_FT_GROUP_BITMAP(unit, bcmFlowtrackerGroupTypeNone),
            valid_groups, 0, max_avail_groups);
    *list_size = valid_groups;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        max_avail_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit,
                bcmFlowtrackerGroupTypeAggregateIngress);
        SHR_BITCOUNT_RANGE(BCMI_FT_GROUP_BITMAP(unit,
                    bcmFlowtrackerGroupTypeAggregateIngress),
                valid_groups, 0, max_avail_groups);
        *list_size += valid_groups;

        max_avail_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit,
                bcmFlowtrackerGroupTypeAggregateMmu);
        SHR_BITCOUNT_RANGE(BCMI_FT_GROUP_BITMAP(unit,
                    bcmFlowtrackerGroupTypeAggregateMmu),
                valid_groups, 0, max_avail_groups);
        *list_size += valid_groups;

        max_avail_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit,
                bcmFlowtrackerGroupTypeAggregateEgress);
        SHR_BITCOUNT_RANGE(BCMI_FT_GROUP_BITMAP(unit,
                    bcmFlowtrackerGroupTypeAggregateEgress),
                valid_groups, 0, max_avail_groups);
        *list_size += valid_groups;
    }
#endif

    if (max_size == 0) {
        return BCM_E_NONE;
    }

    i = 0;
    for (group_type = bcmFlowtrackerGroupTypeNone;
            (group_type < bcmFlowtrackerGroupTypeCount) && (i < max_size);
            group_type++) {

        max_avail_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);
        for (j = 0; (j < max_avail_groups) && (i < max_size); j++) {
            if (SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit, group_type), j)) {
                BCM_FLOWTRACKER_GROUP_ID_SET(idz[i], group_type, j);
                i++;
            }
        }
    }
    *list_size = i;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_tracking_param_set
 * Purpose:
 *   Set tracking parameters for the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (OUT) FT group id.
 *   num_tracking_elements - (IN) number of tracking params.
 *   list_of_tracking_elements - (IN) list of tracking params.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 *   This function does nothing but it just sets the tracking
 *   parameters in the sw state.
 */
int
bcmi_ft_group_tracking_param_set(int unit,
                                 bcm_flowtracker_group_t id,
                                 int num_tracking_elements,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements)
{
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,(BSL_META_U(unit, "setting tracking"
           " param to installed group (%d) is not allowed.\n"), id));
        return BCM_E_CONFIG;
    }

    if ((num_tracking_elements < 0) || (list_of_tracking_elements == NULL)) {
        return BCM_E_PARAM;
    }

    /* Check User entries count for SwInsert Group */
    if (BCMI_FT_GROUP(unit, id)->flags
            & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
        if (bcmi_ft_group_learn_counter_nonzero(unit, id)) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "Changing tracking param list of Flow Group (%d)"
                            " while user entries is configured.\n "), id));
            return BCM_E_BUSY;
        }
    }

    /* Check if the list elements are valid. */
    rv = bcmi_ft_group_param_element_validate(unit, id,
                                              num_tracking_elements,
                                              list_of_tracking_elements);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the existing tracking params. */
    if (BCMI_FT_GROUP_TRACK_PARAM(unit, id)) {
        sal_free(BCMI_FT_GROUP_TRACK_PARAM(unit, id));
        BCMI_FT_GROUP_TRACK_PARAM(unit, id) = NULL;
        BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id) = 0;
    }

    /* User wants to clear the tracking params. */
    if (num_tracking_elements == 0) {
        return BCM_E_NONE;
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

    /* set tracking param based triggers. last param is 1. */
    rv = bcmi_ft_group_ts_triggers_set(unit, id, bcmiFtTypeNone, 0,
                    BCMI_FT_TS_TYPE_TP, bcmFlowtrackerFlowCheckTsSet);

    if (BCM_FAILURE(rv)) {
        sal_free(BCMI_FT_GROUP_TRACK_PARAM(unit, id));
        BCMI_FT_GROUP_TRACK_PARAM(unit, id) = NULL;
        BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id) = 0;
    }

    return rv;
}

#define BCMI_FT_TRACKING_PARAM_TIMESTAMP_EVENTS                 \
            (BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT1 |  \
             BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT2 |  \
             BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT3 |  \
             BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT4)

STATIC int
bcmi_ft_tracking_param_ts_flags_check( int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_tracking_param_info_t *param_info)
{
    int res = FALSE;
    int bit_count = 0;
    uint8 ts_engine_sel = 0;
    uint32 ts_evt_flags = 0;
    uint32 param_flags = 0;
    bcmi_ft_type_t ft_type = bcmiFtTypeNone;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;

    switch(param_info->param) {
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
        case bcmFlowtrackerTrackingParamTypeTimestampIngress:
            ft_type = bcmiFtTypeIFT;
            param_flags = param_info->flags;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampEgress:
            ft_type = bcmiFtTypeEFT;
            param_flags = param_info->flags;
        default:
            break;
    }

    if (ft_type != bcmiFtTypeNone) {

        /* Check - Allow only 1 Event Flag */
        ts_evt_flags = param_flags & BCMI_FT_TRACKING_PARAM_TIMESTAMP_EVENTS;
        bit_count = _shr_popcount(ts_evt_flags);
        if (bit_count > 1) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "param <%s> has more than one timestamp"
                            " event set in flag."),
                        plist[param_info->param]));
            return BCM_E_CONFIG;
        }

        /* Check - Event belong to timestamp engine ft_type */
        BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, param_info->flags, ts_engine_sel);
        BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
        if (res == FALSE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                            "Timestamp engine do not belong to same flowtracker"\
                            " type as param/check\n\r")));
            return BCM_E_CONFIG;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_param_element_validate
 * Purpose:
 *   validate tracking parameters for the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (OUT) FT group id.
 *   num_tracking_elements - (IN) number of tracking params.
 *   list_of_tracking_elements - (IN) list of tracking params.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_param_element_validate(int unit,
                                     bcm_flowtracker_group_t flow_group_id,
                                     int num_tracking_elements,
          bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements)
{
    int rv = BCM_E_NONE;
    int iter = 0;
    int ts_count = 0;
    uint16 ft_type_bmp = 0;
    uint16 group_ft_type_bmp = 0;
    bcm_flowtracker_group_type_t group_type;
    bcm_flowtracker_tracking_param_info_t element_info;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    const char *tsrclist[] = BCM_FLOWTRACKER_TIMESTAMP_SOURCE_STRINGS;

    group_type = BCMI_FT_GROUP_TYPE_GET(flow_group_id);
    BCMI_FT_GROUP_FT_TYPE_BMP_GET(group_type, group_ft_type_bmp);

    for (iter = 0; iter < num_tracking_elements; iter++) {
        sal_memcpy((void *)(&element_info),
                   (void *)(&(list_of_tracking_elements[iter])),
                   sizeof(bcm_flowtracker_tracking_param_info_t));

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        if ((BCMI_FT_GROUP_TYPE_IS_AGGREGATE(flow_group_id)) &&
                (element_info.flags & BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "Flowtracker Group does not supports key parameters.")));
            return BCM_E_PARAM;
        }
#endif

        /* Check if tracking param is supported in system or not */
        rv = _bcm_field_ft_tracking_param_type_get(unit, element_info.param,
                    element_info.custom_id, element_info.flags, &ft_type_bmp);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                     "param <%s> is not supported in system.\n"),
                      plist[element_info.param]));
            return rv;
        }

        /* ft_type_bmp must have group type */
        if (!(group_ft_type_bmp & ft_type_bmp)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                     "param <%s> is not supported for this group.\n"),
                      plist[element_info.param]));
            return BCM_E_PARAM;
        }

        if (bcmi_ft_tracking_param_is_timestamp(unit, element_info.param)) {
            rv = bcmi_ft_tracking_param_ts_flags_check(unit,
                        flow_group_id, &element_info);
            BCM_IF_ERROR_RETURN(rv);

            ts_count++;
        }

        if (ts_count > BCMI_FT_GROUP_MAX_TS) {

            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Maximum timestamps allowed on a group"
                                " are only = %d\n"), BCMI_FT_GROUP_MAX_TS));
            return BCM_E_CONFIG;
        }

        /* Tracking Param Flowtracker check is validated. */
        if (element_info.param ==
                              bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
            rv = bcmi_ft_group_check_validate(unit,
                                         BCMI_FT_GROUP_CHECK_MATCH_PRIMARY,
                                         flow_group_id, element_info.check_id);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Validate timestamp src field when param is timestamp or delay type */
        if ((bcmi_ft_tracking_param_is_timestamp(unit, element_info.param)) ||
            (bcmi_ft_tracking_param_is_delay(unit, element_info.param))) {

            if (!((element_info.src_ts == bcmFlowtrackerTimestampSourceLegacy) ||
                  (element_info.src_ts == bcmFlowtrackerTimestampSourceNTP) ||
                  (element_info.src_ts == bcmFlowtrackerTimestampSourcePTP) ||
                  (element_info.src_ts == bcmFlowtrackerTimestampSourceCMIC) ||
                  (element_info.src_ts == bcmFlowtrackerTimestampSourcePacket) ||
                  (element_info.src_ts == bcmFlowtrackerTimestampSourceIngress) ||
                  (element_info.src_ts == bcmFlowtrackerTimestampSourceCMICTxStart) ||
                  (element_info.src_ts == bcmFlowtrackerTimestampSourceCMICTxEnd))) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                        "invalid timestamp src <%s>\n"),
                                        tsrclist[element_info.src_ts]));
                return BCM_E_PARAM;
            }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                if ((element_info.param == bcmFlowtrackerTrackingParamTypeChipDelay) ||
                    (element_info.param == bcmFlowtrackerTrackingParamTypeIPDTDelay)) {
                    if (!((element_info.src_ts == bcmFlowtrackerTimestampSourceLegacy) ||
                          (element_info.src_ts == bcmFlowtrackerTimestampSourceCMIC) ||
                          (element_info.src_ts == bcmFlowtrackerTimestampSourceCMICTxStart) ||
                          (element_info.src_ts == bcmFlowtrackerTimestampSourceCMICTxEnd))) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                        "invalid timestamp src <%s> for param <%s>\n"),
                                    tsrclist[element_info.src_ts],
                                    plist[element_info.param]));
                        return BCM_E_PARAM;
                    }
                } else if (element_info.param == bcmFlowtrackerTrackingParamTypeIPATDelay) {
                    if (!((element_info.src_ts == bcmFlowtrackerTimestampSourceLegacy) ||
                          (element_info.src_ts == bcmFlowtrackerTimestampSourceCMIC) ||
                          (element_info.src_ts == bcmFlowtrackerTimestampSourceIngress))) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                        "invalid timestamp src <%s> for param <%s>\n"),
                                    tsrclist[element_info.src_ts],
                                    plist[element_info.param]));
                        return BCM_E_PARAM;
                    }
                }
            } else
#endif
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                if ((element_info.param == bcmFlowtrackerTrackingParamTypeChipDelay) ||
                    (element_info.param == bcmFlowtrackerTrackingParamTypeIPDTDelay) ||
                    (element_info.param == bcmFlowtrackerTrackingParamTypeIPATDelay)) {
                    if (!(element_info.src_ts == bcmFlowtrackerTimestampSourceCMIC)) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                        "invalid timestamp src <%s> for param <%s>\n"),
                                    tsrclist[element_info.src_ts],
                                    plist[element_info.param]));
                        return BCM_E_PARAM;
                    }
                }
            }
            if (element_info.param == bcmFlowtrackerTrackingParamTypeE2EDelay) {
                if (!((element_info.src_ts == bcmFlowtrackerTimestampSourceNTP) ||
                      (element_info.src_ts == bcmFlowtrackerTimestampSourcePTP))) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                    "invalid timestamp src <%s> for param <%s>\n"),
                                tsrclist[element_info.src_ts],
                                plist[element_info.param]));
                    return BCM_E_PARAM;
                }
            }

        } else {

            if (element_info.src_ts != 0) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                   "timestamp src info is not required for param <%s>\n"),
                                             plist[element_info.param]));
                return BCM_E_PARAM;
            }
        }

        /* VNID is not supported in tracking param data as we can not export it currently. */
        if ((element_info.flags != BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) &&
            ((element_info.param == bcmFlowtrackerTrackingParamTypeVxlanNetworkId) ||
            (element_info.param == bcmFlowtrackerTrackingParamTypeTunnelClass))) {
            return BCM_E_PARAM;
        }

        /* special params that cannot be set as tracking */
        if ((element_info.param ==
                                bcmFlowtrackerTrackingParamTypeIngressDelay) ||
            (element_info.param ==
                                bcmFlowtrackerTrackingParamTypeEgressDelay)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "param <%s> cannot be used for tracking directly\n"),
                                                  plist[element_info.param]));
            return BCM_E_PARAM;
        }

        /* key params can only belong to ingress flowtracker ift */
        if (ft_type_bmp & (BCMI_FT_TYPE_F_MFT | BCMI_FT_TYPE_F_EFT)) {
            if (element_info.flags & BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                           "mft/eft params cannot be set as key\n")));
                return BCM_E_PARAM;
            }
        }

        if ((element_info.direction < bcmFlowtrackerFlowDirectionNone) ||
                (element_info.direction >= bcmFlowtrackerFlowDirectionCount)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "param <%s> flow direction invalid.\n"),
                        plist[element_info.param]));
            return BCM_E_PARAM;
        }

        /* direction is applicable to data tracking params */
        if ((element_info.flags == BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) &&
                ((element_info.direction != bcmFlowtrackerFlowDirectionNone) &&
                 (element_info.direction != bcmFlowtrackerFlowDirectionForward))) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "Key param <%s> does not support direction.\r\n"),
                        plist[element_info.param]));
            return BCM_E_PARAM;
        }

        if ((element_info.direction == bcmFlowtrackerFlowDirectionReverse) ||
            (element_info.direction == bcmFlowtrackerFlowDirectionBidirectional)) {
            if (!(soc_feature(unit,
                            soc_feature_flowtracker_ver_1_bidirectional))) {
                return BCM_E_UNAVAIL;
            }
            if (BCMI_FT_GROUP_FTFP_DATA(unit, flow_group_id).direction !=
                    bcmFlowtrackerFlowDirectionBidirectional) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Param (%s) direction not supported"
                                    " for flowtracker group(%d).\r\n"),
                         plist[element_info.param], flow_group_id));
                return BCM_E_CONFIG;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_tracking_param_order_get
 * Purpose:
 *   Get tracking parameters for the group.
 *   The tracking params can be in order how user
 *   has sent the data or internal reordered data based on
 *   template.
 * Parameters:
 *   unit - (IN) BCM device id
 *   order - (IN) ordered elements based on template.
 *   id   - (OUT) FT group id.
 *   max_size - max size that user asked for.
 *   list_of_tracking_elements - (IN) list of tracking params.
 *   num_tracking_elements - (IN) number of tracking params.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_group_tracking_param_order_get(int unit,
                                   int order,
                  bcm_flowtracker_group_t id,
                                int max_size,
        bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements,
                    int *num_tracking_elements)
{
    if (num_tracking_elements == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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

    /* Return if tracking param are not present */
    if (*num_tracking_elements == 0) {
        return BCM_E_NOT_FOUND;
    }

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
 *   bcmi_ft_group_tracking_param_get
 * Purpose:
 *   Get tracking parameters for the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (OUT) FT group id.
 *   max_size - max size that user asked for.
 *   list_of_tracking_elements - (IN) list of tracking params.
 *   num_tracking_elements - (IN) number of tracking params.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_tracking_param_get(int unit,
            bcm_flowtracker_group_t id,
            int max_size,
            bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements,
            int *num_tracking_elements)

{
    BCM_IF_ERROR_RETURN
        (bcmi_group_tracking_param_order_get(unit, 0,
            id, max_size, list_of_tracking_elements, num_tracking_elements));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_param_operate
 * Purpose:
 *   Operate group state parameters.
 * Parameters:
 *   unit - (IN) BCM device id
 *   value - (IN) data Value of the update
 *   opr   - (IN) operation to be performed on data.
 *   arg  - (IN) argument value.
 * Returns:
 *   BCM_E_XXX
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
 *   bcmi_ft_group_param_update
 * Purpose:
 *   Update group state parameters.
 * Parameters:
 *   unit  - (IN) BCM device id
 *   id    - (IN) FT group id.
 *   param - (IN) Group's parameters.
 *   opr   - (IN) operation to be performed on data.
 *   arg   - (IN) argument value.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_param_update(int unit,
                           bcm_flowtracker_group_t id,
                           bcmi_ft_group_params_t param,
                           bcmi_ft_group_params_operation_t opr,
                           int arg)
{
   /* First check if group is valid. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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
 *   bcmi_ft_group_param_retrieve
 * Purpose:
 *   Get group state parameters.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   param - (IN) Group's parameters.
 *   arg  - (OUT) argument value.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_param_retrieve(int unit,
                        bcm_flowtracker_group_t id,
                        bcmi_ft_group_params_t param,
                        int *arg)
{
   /* First check if group is valid. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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
 *   bcmi_ft_group_32bit_to_16_bit_param_conversion
 * Purpose:
 *   BCM56370 can not store more than 16 bits in PDD.
 *   therefore some 32 parameters may need split to perform
 *   various checks and export.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   info  - (IN) list if ALU/load information.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_32bit_to_16_bit_param_conversion(int unit,
                             bcm_flowtracker_group_t id,
                             bcmi_ft_group_alu_info_t **info)
{
    int i = 0;
    bcmi_ft_group_alu_info_t *temp;
    int num_alus_loads = 0, new_num_alus_loads = 0;
    bcmi_ft_group_alu_info_t alu1, alu2;
    bcmi_ft_group_alu_info_t *new_alloc = NULL;
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

            BCM_IF_ERROR_RETURN
                (bcmi_ft_flowchecker_get(unit,
                        temp[i].flowchecker_id, &fc_info));
        }

        /* If check has update DoS attack || if exporting dos attack as data. */
        if ((((temp[i].action_element_type ==
                                  bcmFlowtrackerTrackingParamTypeDosAttack) ||
              (temp[i].action_element_type ==
                            bcmFlowtrackerTrackingParamTypeInnerDosAttack)) &&
              (fc_info.action_info.action ==
                            bcmFlowtrackerCheckActionUpdateValue)) ||
             ((temp[i].key1.is_alu == 0) &&
             ((temp[i].element_type1 ==
                            bcmFlowtrackerTrackingParamTypeDosAttack) ||
              (temp[i].element_type1 ==
                            bcmFlowtrackerTrackingParamTypeInnerDosAttack)))) {

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
                alu2.flags |= BCMI_FT_ALU_LOAD_NEXT_ATTACH;

                /* As checks are identical, keep keys same. */
                alu1.key1.location = temp[i].key1.location;
                alu1.key2.location = temp[i].key1.location+16;
                alu2.key1.location = temp[i].key1.location;
                alu2.key2.location = temp[i].key1.location+16;

                alu1.element_type2 = alu1.element_type1;
                alu2.element_type2 = alu2.element_type1;

                alu1.action_key.location = temp[i].action_key.location;
                alu2.action_key.location = temp[i].action_key.location+16;

                alu1.action_key.length = 16;
                alu2.action_key.length = 16;
            }

            sal_memcpy(&new_alloc[i], &alu2, sizeof(bcmi_ft_group_alu_info_t));
            sal_memcpy(&new_alloc[i+1], &alu1, sizeof(bcmi_ft_group_alu_info_t));

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
 *   bcmi_ft_group_extraction_alu_info_set
 * Purpose:
 *   Set Alu extraction info in group sw state.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   num_alu_info - (IN) Number of ALU/lod info structure.
 *   alu_info  - (IN) list if ALU/load information.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_extraction_alu_info_set(int unit,
                    bcm_flowtracker_group_t id,
                    int key_or_data,
                    int num_alu_info,
                    bcmi_ft_group_alu_info_t *alu_info)
{
    int *num_info = 0;
    bcmi_ft_group_alu_info_t **temp = NULL;

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
            BCM_IF_ERROR_RETURN
                (bcmi_ft_group_32bit_to_16_bit_param_conversion(unit,
                     id, (&(BCMI_FT_GROUP_EXT_DATA_INFO(unit, id)))));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_extraction_alu_info_get
 * Purpose:
 *   Get Alu extraction info of group sw state.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   max_alu_info - (IN) max information user asked for.
 *   alu_info  - (OUT) list if ALU/load information.
 *   num_alu_info - (OUT) Number of ALU/lod info structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_extraction_alu_info_get(int unit,
                    bcm_flowtracker_group_t id,
                    int key_or_data,
                    int max_alu_info,
                    bcmi_ft_group_alu_info_t *alu_info,
                    int *num_alu_info)
{
    int *num_info = 0;
    bcmi_ft_group_alu_info_t **temp =  NULL;

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    /* If the max_alu_info is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if ((max_alu_info != 0) && (alu_info == NULL)) {
        return BCM_E_PARAM;
    }

    if (num_alu_info == NULL) {
        return BCM_E_PARAM;
    }

    if (key_or_data) {
        *num_alu_info = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_key_info;
    } else {
        *num_alu_info = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;
    }

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
 *   bcmi_ft_group_extraction_hw_info_set
 * Purpose:
 *   Set hardware extraction info in group sw state.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   key_or_data - (IN) key/data
 *   num_elements - (IN) Number of elements of hw extraction.
 *   elements - (IN) list of hw extrator info.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_extraction_hw_info_set(int unit,
                    bcm_flowtracker_group_t id,
                    int key_or_data,
                    int num_elements,
                    bcmi_ft_hw_extractor_info_t *elements)
{
    int *num_info = NULL;
    bcmi_ft_hw_extractor_info_t **temp = NULL;

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

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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
 *   bcmi_ft_group_extraction_hw_info_get
 * Purpose:
 *   Get hardware extraction info in group sw state.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   key_or_data - (IN) key/data
 *   max_elements - (IN) Max elements that user asked for.
 *   elements - (OUT) list of hw extrator info.
 *   num_elements - (OUT) Number of elements of hw extraction.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_extraction_hw_info_get(int unit,
                            int key_or_data,
                            bcm_flowtracker_group_t id,
                            int max_elements,
                            bcmi_ft_hw_extractor_info_t *elements,
                            int *num_elements)
{
    int *num_info = NULL;
    bcmi_ft_hw_extractor_info_t **temp = NULL;

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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
 *   bcmi_ft_group_extraction_mode_set
 * Purpose:
 *   Set extraction mode.
 * Parameters:
 *   unit - (IN) BCM device id
 *   key_or_data - (IN) key/data
 *   id   - (IN) FT group id.
 *   mode - (IN) Mode of the extration key/data.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_extraction_mode_set(int unit,
                        int key_or_data,
                        bcm_flowtracker_group_t id,
                        bcmi_ft_group_key_data_mode_t mode)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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
 *   bcmi_ft_group_extraction_mode_get
 * Purpose:
 *   Get extraction mode.
 * Parameters:
 *   unit - (IN) BCM device id
 *   key_or_data - (IN) key/data
 *   id   - (IN) FT group id.
 *   mode - (OUT) Mode of the extration key/data.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_extraction_mode_get(int unit,
                            int key_or_data,
                            bcm_flowtracker_group_t id,
                            bcmi_ft_group_key_data_mode_t *mode)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    if (key_or_data) {
        *mode = BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key_mode;
    } else {
        *mode = BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data_mode;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_session_key_type_set
 * Purpose:
 *   Set session key type.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   key_type - (IN) Session Key Type
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_session_key_type_set(int unit,
                 bcm_flowtracker_group_t id,
                 uint32 key_type)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    BCMI_FT_GROUP(unit, id)->ftfp_data.session_key_type = key_type;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_session_key_type_get
 * Purpose:
 *   Get session key type.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   key_type - (OUT) Session Key Type.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_session_key_type_get(int unit,
                 bcm_flowtracker_group_t id,
                 uint32 *key_type)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    *key_type = BCMI_FT_GROUP(unit, id)->ftfp_data.session_key_type;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_state_transition_type_get
 * Purpose:
 *   Get group state transition  type.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   st_type - (OUT) State transition Type.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_state_transition_type_get(int unit,
                 bcm_flowtracker_group_t id,
                 bcm_flowtracker_state_transition_type_t *st_type)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    *st_type = BCMI_FT_GROUP_CONFIG(unit, id).st_type;

    return BCM_E_NONE;
}

int
bcmi_ft_group_ftfp_profiles_set(int unit,
                   bcm_flowtracker_group_t id,
                   bcmi_ft_session_profiles_t *profile_info)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.session_key_profile_idx =
        profile_info->session_key_profile_idx;

    BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.session_data_profile_idx =
        profile_info->session_data_profile_idx;

    BCMI_FT_GROUP(unit, id)->ftfp_data.profiles.alu_data_profile_idx =
        profile_info->alu_data_profile_idx;

    return BCM_E_NONE;
}

int
bcmi_ft_group_ftfp_profiles_get(int unit,
                   bcm_flowtracker_group_t id,
                   bcmi_ft_session_profiles_t *profile_info)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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
bcmi_ft_group_ftfp_data_get(int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_ftfp_data_t *data)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    if (!data) {
        return (BCM_E_PARAM);
    }

    data->dir_ctrl_type = BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type;
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
 *   bcmi_ft_group_hw_install
 * Purpose:
 *   Install hardware resource for the group.
 * Description:
 *   All the groups parameters are added into groups software
 *   state. The hardware resources will only get allocated when
 *   extraction algorithm finished setting up the hardware extrators.
 *   Later hardware extration information will be added into
 *   groups's software state. After that when the group is added
 *   into FTFP entry then this hardware install method will be
 *   called.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_hw_install(int unit, bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    int num_alus_loads = 0, i = 0;
    int num_load_16 = 0, num_load_8 = 0;
    int load16_idz[TOTAL_GROUP_LOAD16_DATA_NUM];
    int load8_idz[TOTAL_GROUP_LOAD8_DATA_NUM];
    int total_load8_mem = TOTAL_GROUP_LOAD8_DATA_NUM;
    int total_load16_mem = TOTAL_GROUP_LOAD16_DATA_NUM;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_alu_load_type_t type = bcmiFtAluLoadTypeNone;

    /* Read the group state for the ALU inforamtion. */
    if (!(BCMI_FT_GROUP_EXT_DATA_OR_TS_TRIGGERS_SET(unit, id))) {
        return BCM_E_INTERNAL;
    }

    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        return BCM_E_CONFIG;
    }

    for (; i < total_load16_mem; i++) {
        load16_idz[i] = -1;
        if (i < total_load8_mem) {
            load8_idz[i] = -1;
        }
    }

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Get total number of alus in this chunk. */
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* preventive check to make sure all data belongs to valid stage */
    for (i = 0; i < num_alus_loads; i++) {
        if (!((temp->alu_ft_type >= bcmiFtTypeIFT) &&
              (temp->alu_ft_type <= bcmiFtTypeAEFT))) {
            return BCM_E_INTERNAL;
        }
        temp++;
    }

    /* Set the timestamp entry. */
    rv = bcmi_ft_group_timestamp_set(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flowtracker_coupled_data_engines)) {
        rv = bcmi_ftv2_group_internal_alu_info_add(unit, id);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif

    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* allocate the bitmap based on the type, length */
    for (i = 0; i < num_alus_loads; i++) {

        if (temp->key1.location >= BCMI_FT_SESSION_DATA_SINGLE_LEN) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }

        /* Get ALU/LOAD type for this alu_info */
        rv = bcmi_ft_alu_load_type_resolve(unit, id, temp, &type);
        BCMI_CLEANUP_IF_ERROR(rv);

        if (type == bcmiFtAluLoadTypeLoad16) {
            load16_idz[num_load_16] = i;
            num_load_16++;
            if (num_load_16 > total_load16_mem) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            temp++;
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

        /* Configure the ALU memory. */
        BCMI_FT_METHOD_INSTALL_CHECK(type);
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = (BCMI_FT_METHOD_INSTALL_EXEC(type))(unit, id, temp, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

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
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = (BCMI_FT_METHOD_INSTALL_EXEC(bcmiFtAluLoadTypeLoad16))
                  (unit, id, NULL, load16_idz);

        BCMI_CLEANUP_IF_ERROR(rv);
    }

    if (load8_idz[0] != -1) {
        BCMI_FT_METHOD_INSTALL_CHECK(bcmiFtAluLoadTypeLoad8);
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = (BCMI_FT_METHOD_INSTALL_EXEC(bcmiFtAluLoadTypeLoad8))
                  (unit, id, NULL, load8_idz);

        BCMI_CLEANUP_IF_ERROR(rv);
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        /* Enable FlowState tracking for micro group
         * where bi-direction is enabled
         */
        if (!BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id) &&
                (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction ==
                 bcmFlowtrackerFlowDirectionBidirectional)) {
            rv = bcmi_ftv3_group_flow_transition_state_set(unit, id, 1);
            BCMI_CLEANUP_IF_ERROR(rv);
        }
    }
#endif

#if defined (BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flowtracker_coupled_data_engines)) {
        rv = bcmi_ftv2_data_engine_db_create(unit, id, load16_idz, load8_idz);
        BCMI_CLEANUP_IF_ERROR(rv);
    }
#endif

    /* Add PDD policy profile for FT Group */
    rv = bcmi_ft_group_pdd_pde_policy_add(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Add Template data for FT Group */
    rv = bcmi_ft_group_template_data_add(unit, id, load16_idz, load8_idz);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Set the single/double session mode based on extraction length. */
    rv = bcmi_ft_group_ft_mode_recompute(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

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
 *   bcmi_ft_group_hw_uninstall
 * Purpose:
 *   Uninstall hardware resource for the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_hw_uninstall(int unit, bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    int num_alus = 0, i = 0;
    bcmi_ft_group_template_list_t **head;
    bcmi_ft_group_alu_info_t *temp = NULL;
    int num_load_16 = 0, num_load_8 = 0;
    int load8_idz[TOTAL_GROUP_LOAD8_DATA_NUM];
    int load16_idz[TOTAL_GROUP_LOAD16_DATA_NUM];
    int total_load8_mem = TOTAL_GROUP_LOAD8_DATA_NUM;
    int total_load16_mem = TOTAL_GROUP_LOAD16_DATA_NUM;
    bcmi_ft_alu_load_type_t type = bcmiFtAluLoadTypeNone;

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

    for (; i < total_load16_mem; i++) {
        load16_idz[i] = -1;
        if (i < total_load8_mem) {
            load8_idz[i] = -1;
        }
    }

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Get the head of the template info list. */
    head = &((BCMI_FT_GROUP(unit, id))->template_head);

    /* Get total number of alus in this chunk. */
    num_alus = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* allocate the bitmap based on the type, length */
    for (i = 0; i < num_alus; i++) {

        if (!(temp->flags & BCMI_FT_ALU_LOAD_HW_INSTALLED)) {
            continue;
        }

        type = temp->alu_load_type;
        if (type == bcmiFtAluLoadTypeLoad16) {
            load16_idz[num_load_16] = i;
            num_load_16++;
            if (num_load_16 > total_load16_mem) {
                /* This actually should not happen. just a -ve check */
                return BCM_E_PARAM;
            }
            temp++;
            continue;

        } else if (type == bcmiFtAluLoadTypeLoad8 ) {
            load8_idz[num_load_8] = i;
            num_load_8++;
            if (num_load_8 > total_load8_mem) {
                /* This actually should not happen. just a -ve check */
                return BCM_E_PARAM;
            }
            temp++;
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
    BCM_IF_ERROR_RETURN(rv);

    /* delete template list */
    bcmi_ft_group_template_list_delete_all(unit, head);

    /* Clear in use memories of group. */
    sal_memset(BCMI_FT_GROUP_ALU16_MEM_USE(unit, id), 0,
        (sizeof(uint32) * TOTAL_GROUP_ALU16_MEM));

#if defined(BCM_FLOWTRACKE_V3_SUPPORT)
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        sal_memset(BCMI_FT_GROUP_ALU32_MEM_USE(unit, id),
                0, (sizeof(uint32) * \
                    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, \
                        bcmiFtAluLoadTypeAggAlu32)));
    } else
#endif
    {
        sal_memset(BCMI_FT_GROUP_ALU32_MEM_USE(unit, id),
                0, (sizeof(uint32) * \
                    BCMI_FT_ALU_LOAD_MEM_COUNT(unit, \
                        bcmiFtAluLoadTypeAlu32)));
    }

    /* Clear internally allocated triggers for a group. */
    bcmi_ft_group_param_int_ts_trig_info_clear(unit, id);

    /* Un-install Session profiles and update ftfp data */
    rv = _bcm_field_ft_session_profiles_uninstall(unit, id);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (BCMI_FT_GROUP(unit, id)->engine_db != NULL) {
        BCMI_FT_FREE(BCMI_FT_GROUP(unit, id)->engine_db);
    }

    for (i = 0; i < BCMI_FT_INT_CHECK_MAX; i++) {
        BCMI_FT_FREE(BCMI_FT_GROUP(unit, id)->int_fc_info_arr[i]);
    }
#endif

    /* reset the validated status. */
    BCMI_FT_GROUP_IS_VALIDATED(unit, id) = 0;

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_pdd_policy_profile_add
 * Purpose:
 *   Add policy profile entry for group action profile.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_pdd_policy_profile_add(int unit,
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
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &kg_group_entry));

    /* now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);

    if (old_index) {
        bcmi_ft_group_pdd_profile_entry_delete(unit, old_index);
    }

    /* Create the profile entry for group pdd profile. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_pdd_profile_entry_add(unit, entries, 1,
                                            (uint32 *)&index));

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm, ((uint32 *)&kg_group_entry),
                                                    SESSION_DATA_TYPEf, index);

    /* Write the entry back into the group table. */
    rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm, MEM_BLOCK_ANY, id, &kg_group_entry);
    BCM_IF_ERROR_RETURN(rv);

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
 *   bcmi_ft_group_pdd_policy_profile_delete
 * Purpose:
 *   Delete policy profile entry for group action profile.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_pdd_policy_profile_delete(int unit,
                      bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    int ref_count = 0, old_index = 0;
    int total_mems = 0, mem_idx = 0;
    int alu_load_type = bcmiFtAluLoadTypeNone;
    bsc_kg_group_table_entry_t entry;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    /* Skip for Aggregate Flow groups */
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }
#endif

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
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
    BCM_IF_ERROR_RETURN(rv);

    /*
     * First check the refcount of pdd entry.
     * Clear only if no other group is pointing to it.
     */
    rv = bcmi_ft_group_pdd_profile_refcount_get(unit, old_index, &ref_count);
    BCM_IF_ERROR_RETURN(rv);

    if (ref_count == 0) {
        /* Delete the PDE profile entry also. */
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DT_PDE_PROFILE_0m,
                    MEM_BLOCK_ANY, old_index,
                    soc_mem_entry_null(unit, BSC_DT_PDE_PROFILE_0m)));
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DT_PDE_PROFILE_1m,
                    MEM_BLOCK_ANY, old_index,
                    soc_mem_entry_null(unit, BSC_DT_PDE_PROFILE_1m)));
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DT_PDE_PROFILE_2m,
                    MEM_BLOCK_ANY, old_index,
                    soc_mem_entry_null(unit, BSC_DT_PDE_PROFILE_2m)));
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_0m,
                    MEM_BLOCK_ANY, old_index,
                    soc_mem_entry_null(unit, BSC_DT_EXPORT_PDE_PROFILE_0m)));
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_1m,
                    MEM_BLOCK_ANY, old_index,
                    soc_mem_entry_null(unit, BSC_DT_EXPORT_PDE_PROFILE_1m)));
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_2m,
                    MEM_BLOCK_ANY, old_index,
                    soc_mem_entry_null(unit, BSC_DT_EXPORT_PDE_PROFILE_2m)));
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, BSC_DT_PDE_PROFILEm,
                    MEM_BLOCK_ANY, old_index,
                    soc_mem_entry_null(unit, BSC_DT_PDE_PROFILEm)));
        }
    }

    /*
     * The group is uninstalled. clear the group's PDD entry.
     * Clear only ALU and Load memories. Rest should not be removed.
     */
    for (alu_load_type = 0;
         alu_load_type <= bcmiFtAluLoadTypeAlu32; alu_load_type++) {

        total_mems = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, alu_load_type);

        if (alu_load_type == bcmiFtAluLoadTypeLoad16) {
            total_mems = TOTAL_GROUP_LOAD16_DATA_NUM;
        } else  if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
            total_mems = TOTAL_GROUP_LOAD8_DATA_NUM;
        }

        /* For all the alu/load types, check what memories are set in pdd. */
        for (mem_idx = 0; mem_idx < total_mems; mem_idx++) {
            /* Now get if this particular field is set in pdd. */
            pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type);
            if (!pdd_info) {
                return BCM_E_INTERNAL;
            }
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILE_0m,
                        (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                        pdd_info[mem_idx].param_id, 0);

            } else
#endif
            {
                soc_mem_field32_set(unit, BSD_POLICY_ACTION_PROFILEm,
                        (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                        pdd_info[mem_idx].param_id, 0);
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_flush_set
 * Purpose:
 *   Remove hash entry from hash table.
 * Parameters:
 *   unit        - (IN) BCM device id
 *   flags       - (IN) Flags for group clear.
 *   num_groups  - (IN) Number of groups to be cleared.
 *   idz         - (IN) list of Ids of all the groups..
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_flush_set(int unit,
                        uint32 flags,
                        int num_groups,
                        bcm_flowtracker_group_t *idz)
{
    int rv = BCM_E_NONE;
    int i = 0;
    uint64 val64;
    int count = 5; /* count 5 is to make 20ms delay */
    soc_reg_t reg = 0;
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
        BCM_IF_ERROR_RETURN
            (bcmi_ft_group_is_invalid(unit, idz[i]));

        if (BCMI_FT_GROUP(unit, idz[i])->flags
                & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
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
            BCM_IF_ERROR_RETURN
                (bcmi_ft_group_flow_limit_set(unit, idz[i], 0));
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
    BCM_IF_ERROR_RETURN
        (WRITE_BSC_AG_GROUP_FLUSH_CONFIG_64r(unit, val64));

    /* Apply flow limit back to flowtracker group */
    for (i = 0; i < num_groups; i++) {
        if (flow_limit_valid[i] == TRUE) {
            BCM_IF_ERROR_RETURN
                (bcmi_ft_group_flow_limit_set(unit, idz[i], flow_limit[i]));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_template_data_create
 * Purpose:
 *   Create template list for group.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) Flowtracker_group.
 *   load16_indexes - (IN) load16 element array.
 *   load8_indexes  - (IN) load8 element array.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_template_data_create(int unit,
                                   bcm_flowtracker_group_t id,
                                   int *load16_indexes,
                                   int *load8_indexes)
{
    int rv = BCM_E_NONE;
    int i = 0;
    soc_mem_t mem;
    int copy_to_cpu = 0;
    int meter_enabled = 0;
    int running_length = 0;
    bcmi_ft_group_template_list_t **head;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_alu_info_t *start = NULL;
    bcmi_ft_group_alu_info_t *local = NULL;
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeNone;
    bcmi_ft_group_param_type_t ts_type = bcmiFtGroupParamTypeTsNewLearn;

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

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    /* Start setting the space for 4 timestamps. */
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_0f)) {
        ts_type = bcmi_ft_group_get_ts_type(unit, id, 0);

        rv = bcmi_ft_group_template_list_add(unit, head,
                                      running_length, 48, ts_type, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

        /* Timestamp 0 is enabled. length = 48 bits. */
        running_length += 48;
    }

    /* Start setting the space for 4 timestamps. */
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_1f)) {
        /* Timestamp 1 is enabled. length = 48 bits. */
        ts_type = bcmi_ft_group_get_ts_type(unit, id, 1);

        rv = bcmi_ft_group_template_list_add(unit, head,
                                      running_length, 48, ts_type, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

        /* Timestamp 1 is enabled. length = 48 bits. */
        running_length += 48;
    }

    /* Start setting the space for 4 timestamps. */
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_2f)) {
        /* Timestamp 2 is enabled. length = 48 bits. */
        ts_type = bcmi_ft_group_get_ts_type(unit, id, 2);

        rv = bcmi_ft_group_template_list_add(unit, head,
                                      running_length, 48, ts_type, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);

        /* Timestamp 2 is enabled. length = 48 bits. */
        running_length += 48;
    }

    /* Start setting the space for 4 timestamps. */
    if (soc_mem_field32_get(unit, mem,
        (&(BCMI_FT_GROUP(unit, id)->pdd_entry)), BSD_FLEX_TIMESTAMP_3f)) {
        /* Timestamp 3 is enabled. length = 48 bits. */
        ts_type = bcmi_ft_group_get_ts_type(unit, id, 3);

        rv = bcmi_ft_group_template_list_add(unit, head,
                                      running_length, 48, ts_type, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);
        /* Timestamp 3 is enabled. length = 48 bits. */
        running_length += 48;
    }

    meter_enabled = soc_mem_field32_get(unit, mem,
                           (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                              BSD_FLEX_FLOW_METERf);
    if (meter_enabled) {
        /* 48 bits are used for metering. */
        rv = bcmi_ft_group_template_list_add(unit, head,
                    running_length, 48, bcmiFtGroupParamTypeMeter, NULL);
        BCMI_CLEANUP_IF_ERROR(rv);
        running_length += 48;
    }

    /* Add ALU32s into the template list based on PDD. */
    for (i = 0; i < TOTAL_GROUP_ALU32_MEM; i++) {

        alu_load_type = bcmiFtAluLoadTypeAlu32;

        if (bcmi_ft_group_alu_load_pdd_status_get(unit, id,
                   (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                                                 alu_load_type, i)) {

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
    for (i = 0; i < TOTAL_GROUP_LOAD16_DATA_NUM; i++) {
        int len=0;
        if (load16_indexes[i] == -1) {
            continue;
        }
        local = (&(start[load16_indexes[i]]));

        if ((local->element_type1 ==
                    bcmFlowtrackerTrackingParamTypeDosAttack) ||
            (local->element_type1 ==
                    bcmFlowtrackerTrackingParamTypeInnerDosAttack)) {
            /* Add 16 bit length to display proper results in export. */
            len = 16;
            i++;
        }

        /* If there is some data then add it into list. */
        rv = bcmi_ft_group_template_list_add(unit, head,
                  running_length, 16+len, bcmiFtGroupParamTypeTracking, local);

        BCMI_CLEANUP_IF_ERROR(rv);
        running_length += (16+len);
    }

    temp = start;
    /* Add ALU16s into template list based on PDD. */
    for (i = 0; i < TOTAL_GROUP_ALU16_MEM; i++) {
        alu_load_type = bcmiFtAluLoadTypeAlu16;

        if (bcmi_ft_group_alu_load_pdd_status_get(unit, id,
                    (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                                                  alu_load_type, i)) {

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

    copy_to_cpu = soc_mem_field32_get(unit, mem,
                            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                               BSD_FLEX_FLOW_COUNT_TO_CPUf);
    if (copy_to_cpu) {
        rv = bcmi_ft_group_template_list_add(unit, head,
                  running_length, 16, bcmiFtGroupParamTypeCollectorCopy, NULL);

        BCMI_CLEANUP_IF_ERROR(rv);
        running_length += 16;
    }

    /* Get all the elements of load8 and then add them into order. */
    for (i = 0; i < TOTAL_GROUP_LOAD8_DATA_NUM;  i++) {
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
 *   bcmi_ft_group_template_list_add
 * Purpose:
 *   Add template info into the group.
 * Parameters:
 *   unit   - (IN) BCM device id
 *   head   - (IN/OUT) Head pointer of the list.
 *   start  - (IN) start of location.
 *   length - (IN) length of element.
 *   type   - (IN) type of element.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_template_list_add(int unit,
                                bcmi_ft_group_template_list_t **head,
                                int start, int length,
                                bcmi_ft_group_param_type_t type,
                                bcmi_ft_group_alu_info_t *info)
{
    int rv = BCM_E_NONE;
    int shift_bits = 0, action = 0;
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
    temp_node->info.data_width = length;
    temp_node->info.param_type = type;
    temp_node->next = NULL;

    if (info != NULL) {
        temp_node->info.param = info->element_type1;
        temp_node->info.index = info->alu_load_index;
        temp_node->info.type = info->alu_load_type;
        temp_node->info.flags = info->flags;
        temp_node->info.check_id = info->flowchecker_id;
        temp_node->info.direction = info->direction;
        temp_node->info.custom_id = info->custom_id1;

        if (temp_node->info.check_id) {

            /* Shift data_offset, if required */
            sal_memset(&check_info, 0,
                    sizeof (bcmi_flowtracker_flowchecker_info_t));
            rv = bcmi_ft_flowchecker_get(unit,
                           temp_node->info.check_id, &check_info);
            if (BCM_FAILURE(rv)) {
                sal_free(temp_node);
                return rv;
            }

            if (!(info->flags & BCMI_FT_ALU_LOAD_DATA_SHIFT_APPLIED)) {
                action = check_info.action_info.action;

                bcmi_ft_group_alu_action_shift_bits_get(unit,
                                            action, info, &shift_bits);
                if ((info->flags & BCMI_FT_ALU_LOAD_INTERNAL_DATA_SHIFT_ADJUSTED) &&
                    (info->flags & BCMI_FT_ALU_LOAD_INTERNAL) &&
                    (info->flags & BCMI_FT_ALU_LOAD_EXTENDED)) {
                    shift_bits = 0;
                }
                temp_node->info.data_shift = shift_bits;

                if ((action == bcmFlowtrackerCheckActionUpdateLowerValue) ||
                        (action == bcmFlowtrackerCheckActionUpdateHigherValue) ||
                        (action == bcmFlowtrackerCheckActionUpdateValue) ||
                        (action == bcmFlowtrackerCheckActionUpdateAverageValue)) {
                    temp_node->info.data_width = info->action_key.length;
                }
            }

            if (BCMI_FT_IDX_IS_INT_CHECK(temp_node->info.check_id)) {
                temp_node->info.check_id = 0;
            } else {
                temp_node->info.param_type = bcmiFtGroupParamTypeFlowchecker;
            }
        }
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

            /* Session data for Aggregate ALU32 (instead of LOAD) */
            if ((info->flags & BCMI_FT_ALU_LOAD_USE_AGG_ALU32) &&
                    (temp_node->info.check_id == 0) && (info->key1.is_alu == 0)) {

                bcmi_ft_group_alu_action_shift_bits_get(unit,
                        bcmFlowtrackerCheckActionUpdateValue,
                        info, &shift_bits);
                temp_node->info.data_shift = shift_bits;
                temp_node->info.data_width = info->action_key.length;
            }
        }
#endif
    }

    /* Insert temp_node at appropriate location. */
    node_prev = NULL;
    node_iter = (*head);
    while (node_iter != NULL) {
        if (!soc_feature(unit, soc_feature_flowtracker_coupled_data_engines)) {

            /* Find location to insert based on ascending order of
               index per data engine */
            if ((info != NULL) &&
                (node_iter->info.type == info->alu_load_type) &&
                (node_iter->info.index > info->alu_load_index)) {
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
 *   bcmi_ft_group_template_list_delete_all
 * Purpose:
 *   Delete template list from group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   head - (IN/OUT) Head pointer of the list.
 * Returns:
 *   BCM_E_XXX.
 */
void
bcmi_ft_group_template_list_delete_all(int unit,
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
 *   bcmi_flowtracker_group_control_set
 * Purpose:
 *   Set value of group control.
 * Parameters:
 *   unit   - (IN)  BCM device number
 *   id     - (IN)  Flow group Id
 *   type   - (IN)  type of group control.
 *   arg    - (IN) value of control
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_flowtracker_group_control_set(int unit,
                 bcm_flowtracker_group_t id,
                 bcm_flowtracker_group_control_type_t type,
                 int arg)
{
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    switch(type) {
        case bcmFlowtrackerGroupControlNewLearnEnable:
            if (arg > 1) {
                return BCM_E_PARAM;
            }
            if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Group Control NewLearn is not"
                                    " supported on Flowtracker Group (%d)\n "),
                         id));
                return BCM_E_CONFIG;
            }

            if ((BCMI_FT_GROUP(unit, id))->flags &
                    BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {

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

            if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Group Control FlowtrackerEnable is not"
                                    " supported on Flowtracker Group (%d)\n "),
                         id));
                return BCM_E_CONFIG;
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
        case bcmFlowtrackerGroupControlUdpFlowDirection:

            if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Group Control FlowDirection is not"
                                    " supported on Flowtracker Group (%d)\n "),
                         id));
                return BCM_E_CONFIG;
            }

            if ((type == bcmFlowtrackerGroupControlUdpFlowDirection) &&
                    (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3))) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Group Control UdpFlowDirection is not"
                                    " supported on Flowtracker Group (%d)\n "),
                         id));
                return BCM_E_PARAM;
            }

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

            if ((arg == BCMI_FT_GROUP_FTFP_DATA(unit, id).direction) &&
                (type == BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type)) {
                return BCM_E_NONE;
            }

            BCM_IF_ERROR_RETURN(
                _bcm_field_ft_group_control_set(unit, id, type, arg));

            /* Add that value in local data and send notification to ftfp. */
            BCMI_FT_GROUP_FTFP_DATA(unit, id).direction = arg;
            BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type = type;
            /*
             * To change the direction run time, we need to change
             * ALU/LOAD memories associated with this group.
             */
            BCM_IF_ERROR_RETURN(
                bcmi_ft_group_alu_load_trigger_set(unit, id, arg));
        break;

        case bcmFlowtrackerGroupControlStateTransitionType:
            if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Group Control FlowDirection is not"
                                    " supported on Flowtracker Group (%d)\n "),
                         id));
                return BCM_E_UNAVAIL;
            }

            if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                    "Not allowed to alter installed flow Group (%d)\n "), id));

                return BCM_E_CONFIG;
            }

            if ((arg < 0) || (arg >= bcmFlowtrackerStateTransitionTypeCount)) {
                return BCM_E_PARAM;
            }

            BCMI_FT_GROUP_CONFIG(unit, id).st_type = arg;
        break;

        default:
            /* Nothing else is supported.. */
        return BCM_E_PARAM;
   }


    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_flowtracker_group_control_get
 * Purpose:
 *   Get value of group control.
 * Parameters:
 *   unit   - (IN)  BCM device number
 *   id     - (IN)  Flow group Id
 *   type   - (IN)  type of group control.
 *   arg    - (OUT) value of control
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_flowtracker_group_control_get(int unit,
                 bcm_flowtracker_group_t id,
                 bcm_flowtracker_group_control_type_t type,
                 int *arg)
{
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

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
            if ((BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type !=
                        bcmFlowtrackerGroupControlNone) &&
                    (BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type != type)) {
                return BCM_E_PARAM;
            }
            *arg = BCMI_FT_GROUP_FTFP_DATA(unit, id).direction;
        break;

        case bcmFlowtrackerGroupControlUdpFlowDirection:
            if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Group Control UdpFlowDirection is not"
                                    " supported on Flowtracker Group (%d)\n "),
                         id));
                return BCM_E_PARAM;
            }
            /* Add that value in local data and send notification to ftfp. */
            if ((BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type !=
                        bcmFlowtrackerGroupControlNone) &&
                    (BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type != type)) {
                return BCM_E_PARAM;
            }
            *arg = BCMI_FT_GROUP_FTFP_DATA(unit, id).direction;
        break;

        case bcmFlowtrackerGroupControlStateTransitionType:
            *arg = BCMI_FT_GROUP_CONFIG(unit, id).st_type;
        break;

        default:
            /* Nothing else is supported.. */
        return BCM_E_PARAM;
   }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_stat_get
 * Purpose:
 *   Get statistics of group.
 * Parameters:
 *   unit         - (IN)  BCM device number
 *   id           - (IN)  Flow group Id
 *   group_stats  - (OUT) stats of the group.
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_stat_get(int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_stat_t *group_stats)
{
    uint64 val;
    uint32 val32 = 0;

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    if (!group_stats) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Stats is not supported on"
                            " Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
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
            BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
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

    if (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction ==
            bcmFlowtrackerFlowDirectionBidirectional) {

        BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
                    SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_START_CNT, id, &val));

        COMPILER_64_COPY(group_stats->flow_start_count, val);

        COMPILER_64_TO_32_LO(val32, val);

        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "flow_start_count = %d for group: %d\n"), val32, id));

        BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
                    SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_END_CNT, id, &val));

        COMPILER_64_COPY(group_stats->flow_end_count, val);

        COMPILER_64_TO_32_LO(val32, val);

        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "flow_end_count = %d for group: %d\n"), val32, id));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_stat_set
 * Purpose:
 *   Set statistics of group.
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   id            - (IN)  Flow group Id
 *   group_stats   - (IN) stats of the group.
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_stat_set(int unit,
            bcm_flowtracker_group_t id,
            bcm_flowtracker_group_stat_t *group_stats)
{
    uint64 val;

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    if (!group_stats) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Stats is not supported on"
                            " Flowtracker Group (%d)\n "), id));
        return BCM_E_CONFIG;
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

    /* Flow start count. */
    COMPILER_64_COPY(val, group_stats->flow_start_count);

    BCM_IF_ERROR_RETURN(soc_counter_set(unit, REG_PORT_ANY,
                SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_START_CNT, id, val));

    /* Flow end count. */
    COMPILER_64_COPY(val, group_stats->flow_end_count);

    BCM_IF_ERROR_RETURN(soc_counter_set(unit, REG_PORT_ANY,
                SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_END_CNT, id, val));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_export_trigger_verify
 * Purpose:
 *   verify export triggers on group.
 * Parameters:
 *   unit                - (IN) BCM device number
 *   id                  - (IN) Flow group Id
 *   export_trigger_info - (IN) Export trigger info.
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_export_trigger_verify(int unit,
                    bcm_flowtracker_group_t id,
                    bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    uint32 val;

    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                    bcmFlowtrackerExportTriggerNewLearn)) ? 1 : 0);
    if (val != 0) {
        if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Export trigger NewLearn is not"
                                " supported on Flowtracker Group (%d)\n "),
                     id));
            return BCM_E_CONFIG;
        }
        if (BCMI_FT_GROUP(unit, id)->flags
                & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
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
        if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Export trigger Ageout is not"
                                " supported on Flowtracker Group (%d)\n "),
                     id));
            return BCM_E_CONFIG;
        }
        if (BCMI_FT_GROUP(unit, id)->flags
                & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
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

    /* export trigger on Congestion . */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                    bcmFlowtrackerExportTriggerCongestion)) ? 1 : 0);
    if (val != 0) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Export"
                        " on Congestion Monitor is not supported\n\r")));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_export_trigger_set
 * Purpose:
 *   Set export triggers on group.
 * Parameters:
 *   unit                - (IN) BCM device number
 *   id                  - (IN) Flow group Id
 *   export_trigger_info - (IN) Export trigger info.
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_export_trigger_set(int unit,
                    bcm_flowtracker_group_t id,
                    bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    int rv = BCM_E_NONE;
    uint32 val = 0;

    if (!export_trigger_info) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    rv = bcmi_ft_group_export_trigger_verify(unit, id, export_trigger_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Set export trigger for new learn. */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
            bcmFlowtrackerExportTriggerNewLearn)) ? 1 : 0);

    rv = bcmi_ft_group_new_learn_export_trigger_set(unit,
                id, export_trigger_info->flags, val);
    BCM_IF_ERROR_RETURN(rv);

    /* Set export trigger for periodic timer. */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
            bcmFlowtrackerExportTriggerTimer)) ? 1 : 0);

    rv = bcmi_ft_group_timer_export_trigger_set(unit, id, val);
    BCM_IF_ERROR_RETURN(rv);

    /* Set export trigger for age out. */
    val = ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
            bcmFlowtrackerExportTriggerAgeOut)) ? 1 : 0);

    rv = bcmi_ft_group_age_export_trigger_set(unit,
                id, export_trigger_info->flags, val);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_export_trigger_get
 * Purpose:
 *   Get export triggers of group.
 * Parameters:
 *   unit                - (IN) BCM device number
 *   id                  - (IN) Flow group Id
 *   export_trigger_info - (OUT) Export trigger info.
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_group_export_trigger_get(int unit,
            bcm_flowtracker_group_t id,
            bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    int rv = BCM_E_NONE;
    uint32 val = 0;
    uint32 flags = 0;

    if (!export_trigger_info) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, id));

    sal_memset(export_trigger_info, 0,
        sizeof(bcm_flowtracker_export_trigger_info_t));

    /* Get export trigger for new learn. */
    rv = bcmi_ft_group_new_learn_export_trigger_get(unit,
                            id, &flags, &val);
    BCM_IF_ERROR_RETURN(rv);
    if (val) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
            bcmFlowtrackerExportTriggerNewLearn);
    }
    export_trigger_info->flags |= flags;

    /* Get export trigger for periodic timer. */
    rv = bcmi_ft_group_timer_export_trigger_get(unit, id, &val);
    BCM_IF_ERROR_RETURN(rv);
    if (val) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
            bcmFlowtrackerExportTriggerTimer);
    }

    /* Get export trigger for age out. */
    rv = bcmi_ft_group_age_export_trigger_get(unit,
            id, &flags, &val);
    BCM_IF_ERROR_RETURN(rv);
    if (val) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
            bcmFlowtrackerExportTriggerAgeOut);
    }
    export_trigger_info->flags |= flags;

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_check_ts_set
 * Purpose:
 *   set or clear timestamp trigger for provided checker
 * Parameters:
 *   unit           - (IN) BCM device number
 *   id             - (IN) FLOW group id
 *   flowchecker_id - (IN) check id
 *   reset          - (IN) set/reset trigger for checker
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_check_ts_set(int unit,
            bcm_flowtracker_group_t id,
            bcm_flowtracker_check_t flowchecker_id,
            bcmi_flowtracker_check_set_t reset)
{
    bcmi_ft_type_t ft_type = bcmiFtTypeNone;
    uint32 ft_type_bmp, group_ft_type_bmp;
    bcm_flowtracker_group_type_t group_type;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));

    BCM_IF_ERROR_RETURN
        (bcmi_ft_flowchecker_get(unit, flowchecker_id, &fc_info));

    group_type = BCMI_FT_GROUP_TYPE_GET(id);
    BCMI_FT_GROUP_FT_TYPE_BMP_GET(group_type, group_ft_type_bmp);
    BCMI_FT_GROUP_CHECK_FT_TYPE_GET(group_ft_type_bmp,
                fc_info.check_ft_type_bmp, ft_type_bmp, ft_type);

    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_ts_triggers_set(unit, id, ft_type,
                fc_info.flags, BCMI_FT_TS_TYPE_FC, reset));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_ts_triggers_set
 * Purpose:
 *   set timestamp triggers in group
 * Parameters:
 *   unit         - (IN) BCM device number
 *   id           - (IN) FLOW group id
 *   flags        - (IN) flags from flow check / tracking param
 *   trigger_type - (IN) tracking param/flow check trigger type.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_ts_triggers_set(int unit,
                              bcm_flowtracker_group_t id,
                              bcmi_ft_type_t ft_type,
                              uint32 flags, int trigger_type,
                              bcmi_flowtracker_check_set_t reset)
{
    int res = FALSE;
    int i = 0;
    uint8 ts_engine_sel = 0;
    uint32 ts_triggers = 0;
    bcm_flowtracker_tracking_param_info_t *t_info = NULL;
    int num_elements = 0, ts_count = 0, ts_internal_count = 0;
    int is_timestamp_param = 0;
    uint32 param_ts_triggers = 0;
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    int trig = 0;
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    num_elements = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
    t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    ts_triggers = BCMI_FT_GROUP_TS_TRIGGERS(unit, id);
    param_ts_triggers = BCMI_FT_GROUP_PARAM_TS_TRIGGERS(unit, id);

    if (trigger_type == BCMI_FT_TS_TYPE_TP) {

        /* Clear previous set trigger info for tracking params for a group. */
        bcmi_ft_group_param_ts_trig_info_clear(unit, id, &ts_triggers);
        param_ts_triggers = 0;

        /* set triggers based on the tracking params */
        for (i = 0; i < num_elements; i++, t_info++) {
            ft_type = bcmiFtTypeNone;
            is_timestamp_param = TRUE;
            switch(t_info->param) {
                case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
                    ft_type = bcmiFtTypeIFT;
                    param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_NEW_LEARN);
                    break;
                case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
                    ft_type = bcmiFtTypeIFT;
                    param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_FLOW_START);
                    break;
                case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
                    ft_type = bcmiFtTypeIFT;
                    param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_FLOW_END);
                    break;
                case bcmFlowtrackerTrackingParamTypeTimestampIngress:
                    ft_type = bcmiFtTypeIFT;
                    param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_INGRESS);
                    break;
                case bcmFlowtrackerTrackingParamTypeTimestampEgress:
                    ft_type = bcmiFtTypeEFT;
                    param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_EGRESS);
                    break;
                case bcmFlowtrackerTrackingParamTypeIPATDelay:
                    ft_type = bcmiFtTypeIFT;
                    ts_internal_count++;
                    break;
                case bcmFlowtrackerTrackingParamTypeIPDTDelay:
                    ft_type = bcmiFtTypeEFT;
                    ts_internal_count++;
                    break;
                default:
                    is_timestamp_param = FALSE;
                    break;
            }
            if (is_timestamp_param == FALSE) {
                continue;
            }
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, t_info->flags, ts_engine_sel);
                if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
                    BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
                    BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                    if (res == FALSE) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                        "Timestamp engine do not belong to same"
                                        " flowtracker type as param\n\r")));
                        return BCM_E_CONFIG;
                    }

                    if ((1 << trig) & ts_triggers) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                        "Timestamp engine is reserved for flowtracker"\
                                        " check\n\r")));
                        return BCM_E_CONFIG;
                    }
                    if ((1 << trig) & param_ts_triggers) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                        "Timestamp engine is reserved for other"\
                                        " tracking param\n\r")));
                        return BCM_E_CONFIG;
                    }
                    param_ts_triggers |= (1 << trig);
                }
            }
#endif
        }
    } else {

        /* Clear Internally allocated ts triggers */
        bcmi_ft_group_param_int_ts_trig_info_clear(unit, id);

        /* ALU triggers can also be set from ALU methods */
        if (flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1) {
            if (reset == bcmFlowtrackerFlowCheckTsSet) {
                BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit,
                    BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1, ts_engine_sel);
                BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                if (res == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                    "Timestamp engine do not belong to same flowtracker"\
                                    " type as param/check\n\r")));                 \
                    return BCM_E_CONFIG;
                }
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
                if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                    trig = BCMI_FT_GROUP_TS_CHECK_EVENT1;
                    if ((1 << trig) & param_ts_triggers) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                        "Timestamp engine is reserved for other"\
                                        " tracking param\n\r")));
                        return BCM_E_CONFIG;
                    }
                }
#endif
                BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT1);
                ts_triggers |= (1 << BCMI_FT_GROUP_TS_CHECK_EVENT1);
            } else {
                BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_DECR(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT1);
                if (!BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT1)) {
                    ts_triggers &= ~(1 << BCMI_FT_GROUP_TS_CHECK_EVENT1);
                    BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT1) = bcmiFtTypeNone;
                    BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT1) = 0;
                }
            }
        }

        if (flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2) {
            if (reset == bcmFlowtrackerFlowCheckTsSet) {
                BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit,
                    BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2, ts_engine_sel);
                BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                if (res == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                    "Timestamp engine do not belong to same flowtracker"\
                                    " type as param/check\n\r")));                 \
                    return BCM_E_CONFIG;
                }
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
                if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                    trig = BCMI_FT_GROUP_TS_CHECK_EVENT2;
                    if ((1 << trig) & param_ts_triggers) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                        "Timestamp engine is reserved for other"\
                                        " tracking param\n\r")));
                        return BCM_E_CONFIG;
                    }
                }
#endif
                BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT2);
                ts_triggers |= (1 << BCMI_FT_GROUP_TS_CHECK_EVENT2);
            } else {
                BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_DECR(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT2);
                if (!BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT2)) {
                    ts_triggers &= ~(1 << BCMI_FT_GROUP_TS_CHECK_EVENT2);
                    BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT2) = bcmiFtTypeNone;
                    BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT2) = 0;
                }
            }
        }

        if (flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3) {
            if (reset == bcmFlowtrackerFlowCheckTsSet) {
                BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit,
                    BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3, ts_engine_sel);
                BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                if (res == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                    "Timestamp engine do not belong to same flowtracker"\
                                    " type as param/check\n\r")));                 \
                    return BCM_E_CONFIG;
                }
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
                if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                    trig = BCMI_FT_GROUP_TS_CHECK_EVENT3;
                    if ((1 << trig) & param_ts_triggers) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                        "Timestamp engine is reserved for other"\
                                        " tracking param\n\r")));
                        return BCM_E_CONFIG;
                    }
                }
#endif
                BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT3);
                ts_triggers |= (1 << BCMI_FT_GROUP_TS_CHECK_EVENT3);
            } else {
                BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_DECR(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT3);
                if (!BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT3)) {
                    ts_triggers &= ~(1 << BCMI_FT_GROUP_TS_CHECK_EVENT3);
                    BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT3) = bcmiFtTypeNone;
                    BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT3) = 0;
                }
            }
        }

        if (flags & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4) {
            if (reset == bcmFlowtrackerFlowCheckTsSet) {
                BCMI_FT_CHECK_TS_ENGINE_SEL_GET(unit,
                    BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4, ts_engine_sel);
                BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(unit, ts_engine_sel, ft_type, res);
                if (res == FALSE) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                    "Timestamp engine do not belong to same flowtracker"\
                                    " type as param/check\n\r")));                 \
                    return BCM_E_CONFIG;
                }
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
                if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                    trig = BCMI_FT_GROUP_TS_CHECK_EVENT4;
                    if (1 << (trig) & param_ts_triggers) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, \
                                        "Timestamp engine is reserved for other"\
                                        " tracking param\n\r")));
                        return BCM_E_CONFIG;
                    }
                }
#endif
                BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT4);
                ts_triggers |= (1 << BCMI_FT_GROUP_TS_CHECK_EVENT4);
            } else {
                BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_DECR(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT4);
                if (!BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id,
                                    BCMI_FT_GROUP_TS_CHECK_EVENT4)) {
                    ts_triggers &= ~(1 << BCMI_FT_GROUP_TS_CHECK_EVENT4);
                    BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT4) = bcmiFtTypeNone;
                     BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id,
                            BCMI_FT_GROUP_TS_CHECK_EVENT4) = 0;
                }
            }
        }
    }

    /* Count number of trigger requested */
    ts_count = 0;
    for (i = 0; i < BCMI_FT_GROUP_MAX_TS_TYPE; i++) {
        if ((ts_triggers) & (1 << i)) {
            ts_count++;
        } else if ((param_ts_triggers) & (1 << i)) {
            /* Skip for Reserved triggers */
            if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , i) &
                    (BCMI_FT_TS_RESERVED_NEW_LEARN |
                     BCMI_FT_TS_RESERVED_FLOW_START |
                     BCMI_FT_TS_RESERVED_FLOW_END |
                     BCMI_FT_TS_RESERVED_INGRESS |
                     BCMI_FT_TS_RESERVED_EGRESS)) {
                continue;
            }
            ts_count++;
        }
    }

    /* Total timestamp requested should be less than overall max.
     * Individual flowtracker stage level check will be done later */
    if ((ts_count + ts_internal_count) > BCMI_FT_GROUP_MAX_TS) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Timestamp requested <%d> exceeds max <%d> allowed in system.\n"),
                       (ts_count + ts_internal_count),BCMI_FT_GROUP_MAX_TS));
        return BCM_E_CONFIG;
    }
    COMPILER_REFERENCE(ft_type);

    /* Set triggers requested in group */
    BCMI_FT_GROUP_TS_TRIGGERS(unit, id) = ts_triggers;
    BCMI_FT_GROUP_TS_TRIGGERS(unit, id) |= param_ts_triggers;

    /* Set param triggers requested in the group */
    BCMI_FT_GROUP_PARAM_TS_TRIGGERS(unit, id) = param_ts_triggers;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_get_param_type
 * Purpose:
 *   Get tracking parameter type from tracking parameters.
 * Parameters:
 *   unit                - (IN) BCM device number
 *   param               - (IN) tracking parameter
 * Returns:
 *   bcmi_ft_group_param_type_t.
 */
bcmi_ft_group_param_type_t
bcmi_ft_group_get_param_type(int unit,
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
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
            param_type = bcmiFtGroupParamTypeTsCheckEvent3;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:
            param_type = bcmiFtGroupParamTypeTsCheckEvent4;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampIngress:
            param_type = bcmiFtGroupParamTypeTsIngress;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampEgress:
            param_type = bcmiFtGroupParamTypeTsEgress;
            break;
        default :
            param_type = bcmiFtGroupParamTypeTracking;
    }

    return param_type;
}

/*
 * Function:
 *   bcmi_ft_group_get_tracking_param
 * Purpose:
 *   Get tracking parameter from tracking parameters type
 * Parameters:
 *   unit       - (IN) BCM device number
 *   param_type - (IN) tracking parameter type
 * Returns:
 *   bcm_flowtracker_tracking_param_type_t.
 */
bcm_flowtracker_tracking_param_type_t
bcmi_ft_group_get_tracking_param(int unit,
                                 bcmi_ft_group_param_type_t param_type)
{
    bcm_flowtracker_tracking_param_type_t param;

    param = bcmFlowtrackerTrackingParamTypeNone;

    switch (param_type) {
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
        case bcmiFtGroupParamTypeTsCheckEvent3:
            param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3;
            break;
        case bcmiFtGroupParamTypeTsCheckEvent4:
            param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4;
            break;
        case bcmiFtGroupParamTypeTsIngress:
            param = bcmFlowtrackerTrackingParamTypeTimestampIngress;
            break;
        case bcmiFtGroupParamTypeTsEgress:
            param = bcmFlowtrackerTrackingParamTypeTimestampEgress;
            break;
        default :
            param = bcmFlowtrackerTrackingParamTypeNone;
    }

    return param;
}

/*
 * Function:
 *   bcmi_ft_group_get_ts_type
 * Purpose:
 *   Get tracking parameter type matching a field.
 * Parameters:
 *   unit  - (IN) BCM device number
 *   id    - (IN) flow Group id.
 *   ts_bank - (IN) ts bank
 * Returns:
 *   bcmi_ft_group_param_type_t
 */
bcmi_ft_group_param_type_t
bcmi_ft_group_get_ts_type(int unit,
                          bcm_flowtracker_group_t id,
                          int ts_bank)
{
    int trig = 0;
    uint32 ts_flags = 0;
    bcmi_ft_group_param_type_t param_type;
    bcmi_ft_group_trigger_info_t *ts_info = NULL;

    ts_flags = (BCMI_FT_TS_RESERVED_NEW_LEARN |
            BCMI_FT_TS_RESERVED_FLOW_START |
            BCMI_FT_TS_RESERVED_FLOW_END |
            BCMI_FT_TS_RESERVED_INGRESS |
            BCMI_FT_TS_RESERVED_EGRESS);
    ts_info = BCMI_FT_GROUP_TS_TRIGGER_INFO(unit, id);

    for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {
        if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << trig)) {

            /* Skip if trigger is reserved for other ts_type */
            if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) & ts_flags) {
                continue;
            }
            if (ts_info[trig].ts_bank == ts_bank) {
                break;
            }
        }
    }

    switch(trig) {
        case BCMI_FT_GROUP_TS_NEW_LEARN:
            param_type = bcmiFtGroupParamTypeTsNewLearn;
            break;
        case BCMI_FT_GROUP_TS_FLOW_START:
            param_type = bcmiFtGroupParamTypeTsFlowStart;
            break;
        case BCMI_FT_GROUP_TS_FLOW_END:
            param_type = bcmiFtGroupParamTypeTsFlowEnd;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT1:
            param_type = bcmiFtGroupParamTypeTsCheckEvent1;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT2:
            param_type = bcmiFtGroupParamTypeTsCheckEvent2;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT3:
            param_type = bcmiFtGroupParamTypeTsCheckEvent3;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT4:
            param_type = bcmiFtGroupParamTypeTsCheckEvent4;
            break;
        case BCMI_FT_GROUP_TS_INGRESS:
            param_type = bcmiFtGroupParamTypeTsIngress;
            break;
        case BCMI_FT_GROUP_TS_EGRESS:
            param_type = bcmiFtGroupParamTypeTsEgress;
            break;
        default:
            param_type = bcmiFtGroupParamTypeTracking;
            break;
    }

    return param_type;
}

/*
 * Function:
 *   bcmi_ft_group_entry_add_check
 * Purpose:
 *   Check if Flowtracker Group is ready to be added to ftfp entry.
 * Parameters:
 *   unit - (IN) Unit number.
 *   group_type - (IN) Flowtracker Group Type
 *   id - (IN) Flowtracker flow group ID.
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
int
bcmi_ft_group_entry_add_check(int unit,
                              bcm_flowtracker_group_type_t group_type,
                              bcm_flowtracker_group_t id)
{
    int rv;
    int ft_enable;
    int user_entry = 0;
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

    if (group_type != BCMI_FT_GROUP_TYPE_GET(id)) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "FP(unit %d)"
                        " Error: Flowtracker Group %d is not"
                        " supported for this action.\n"), unit, id));
        return BCM_E_CONFIG;
    }

    /* Check if FT Group template params extracted */
    rv = bcmi_esw_ft_group_extraction_alu_info_get(unit,
                                id, 0, 0, NULL, &num_key_info);
    if ((rv != BCM_E_NONE) || (num_key_info == 0)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit, "FP(unit %d)"
                        "Error: Flowtracker Group %d is not"
                        " validated.\n"), unit, id));
        return BCM_E_CONFIG;
    }

    ft_enable = BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack;
    rv = bcmi_ft_group_flow_limit_get(unit, id, &flow_limit);
    if (rv != BCM_E_NONE) {
        flow_limit = 0;
    }
    user_entry = (BCMI_FT_GROUP(unit, id)->flags
            & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY);

    /* h/w learnt flow group where new learn is disabled
     * or flow limit is set to 0 allow even if collector is
     * not attached to the flowtracker group since packet
     * will not be learnt.
     */
    if ((user_entry == FALSE) &&
                ((ft_enable == FALSE) || (flow_limit == 0))) {
        return BCM_E_NONE;
    }

    /* Check if collector is attached */
    rv = bcmi_ft_group_collector_is_valid(unit, id);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Error: Collector is not "
                        "associated with FlowtrackerGroup %d\r\n"),
                    unit, id));
        rv = BCM_E_CONFIG;
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_is_check_valid
 * Purpose:
 *   Check if FlowCheck is associated with ft group.
 * Parameters:
 *   unit - (IN) Unit number.
 *   id - (IN) Flowtracker flow group ID.
 *   check_id - (IN) Check Id to verify
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
int bcmi_ft_group_is_check_valid(int unit,
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
 *   bcmi_ft_group_export_checks_validate
 * Purpose:
 *   Verify flowchecks in in export list.
 * Parameters:
 *   unit - (IN) Unit number.
 *   id - (IN) Flowtracker flow group ID.
 *   num_in_export_elements - Number of export elements.
 *   in_export_elements - (IN) List of export elements.
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
int
bcmi_ft_group_export_checks_validate(int unit,
                    bcm_flowtracker_group_t id,
                    int num_in_export_elements,
                    bcm_flowtracker_export_element_info_t *in_export_elements)
{
    int rv = BCM_E_NONE;
    int options = 0;
    int iter = 0, iter1 = 0;
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
 *   bcmi_ft_group_norm_tracking_params_list_get
 * Purpose:
 *   Get the list of tracking_param info eligible for
 *   normalization in the given flowtracker group.
 * Parameters:
 *   unit - (IN) Unit number.
 *   id - (IN) Flowtracker flow group ID.
 *   norm_tracking_params_info - (OUT) list of norm tracking params.
 * Returns:
 *   BCM_E_xxx
 * Notes:
 *   The returned 'norm_tracking_params_info' is ordered based on
 *   priority of normalization and if some params are not present
 *   in the given FT Group, it is set to NULL.
 */
int
bcmi_ft_group_norm_tracking_params_info_get(int unit,
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

/*
 * Function:
 *   bcmi_ft_group_param_present
 * Purpose:
 *   check if param is present or not in a group
 * Parameters:
 *   unit   - (IN) BCM device id
 *   id     - (IN) FT group id.
 *   param  - (IN) tracking param
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_param_present(int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_tracking_param_type_t tparam,
        int *index)
{
    int i = 0, num_param = 0;
    bcm_flowtracker_tracking_param_info_t *grp_param = NULL;

    /* first check if group is valid. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    num_param = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
    grp_param = BCMI_FT_GROUP_TRACK_PARAM(unit, id);

    for (i = 0; i < num_param; i++) {
        if (grp_param->param == tparam) {
            if (index != NULL) {
                *index = i;
            }
            return BCM_E_NONE;
        }
        grp_param++;
    }

    return BCM_E_NOT_FOUND;
}

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
/*
 * Function:
 *   bcmi_ftv2_group_param_ts_src_update
 * Purpose:
 *   Update timestamp source to default source for provided param
 *   if user have provided legacy
 * Parameters:
 *   unit     - (IN) BCM device id
 *   tparam   - (IN) tracking param
 *   ts_src   - (IN/OUT) timestamp source
 * Returns:
 *   BCM_E_XXX
 */
void
bcmi_ftv2_group_param_ts_src_update(int unit,
                        bcm_flowtracker_tracking_param_type_t tparam,
                        bcm_flowtracker_timestamp_source_t *ts_src)
{
    if (*ts_src != bcmFlowtrackerTimestampSourceLegacy) {
        return;
    }

    switch(tparam) {
        case bcmFlowtrackerTrackingParamTypeChipDelay:
        case bcmFlowtrackerTrackingParamTypeIPDTDelay:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                *ts_src = bcmFlowtrackerTimestampSourceCMICTxStart;
            } else {
                *ts_src = bcmFlowtrackerTimestampSourceCMIC;
            }
            break;
        case bcmFlowtrackerTrackingParamTypeIPATDelay:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                *ts_src = bcmFlowtrackerTimestampSourceIngress;
            } else {
                *ts_src = bcmFlowtrackerTimestampSourceCMIC;
            }
            break;
        default:
            break;
    }
}

/*
 * Function:
 *   bcmi_ftv2_group_param_ts_src_get
 * Purpose:
 *   Get timestamp source for provided param in ft group
 * Parameters:
 *   unit     - (IN) BCM device id
 *   id       - (IN) FT group id.
 *   param    - (IN) tracking param
 *   ts_flags - (IN) timestamp internal flags
 *   ts_src   - (OUT) timestamp source
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_param_ts_src_get(int unit,
                        bcm_flowtracker_group_t id,
                        bcm_flowtracker_tracking_param_type_t tparam,
                        uint8 ts_flags,
                        bcm_flowtracker_timestamp_source_t *ts_src)
{
    int i = 0, num_param = 0;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    bcm_flowtracker_tracking_param_info_t *grp_param = NULL;

    /* first check if group is valid. */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_group_is_invalid(unit, id));

    num_param = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
    grp_param = BCMI_FT_GROUP_TRACK_PARAM(unit, id);

    if (ts_flags & BCMI_FT_TS_INTERNAL_IPAT) {
        tparam = bcmFlowtrackerTrackingParamTypeIPATDelay;
    }
    if (ts_flags & BCMI_FT_TS_INTERNAL_IPDT) {
        tparam = bcmFlowtrackerTrackingParamTypeIPDTDelay;
    }

    for (i = 0; i < num_param; i++) {
        if (grp_param->param == tparam) {
            *ts_src = grp_param->src_ts;

            bcmi_ftv2_group_param_ts_src_update(unit,
                    tparam, ts_src);
            return BCM_E_NONE;
        }
        grp_param++;
    }

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                "Tracking Param <%s> should be set for "
                "flowtracker group %d, ts_flags 0x%02x.\n"),
                                 plist[tparam], id, ts_flags));
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ftv2_group_pdd_to_stage_pdd_coversion
 * Purpose:
 *   Convert input pdd to stage specific pdd entries.
 * Parameters:
 *   unit          - (IN)  BCM device id.
 *   in_pdd_entry  - (IN)  Input pdd entry.
 *   out_pdd_entry - (OUT) Output pdd entries.
 * Notes:
 *   in_pdd_entry is global pdd entry where resources
 *   allocated for all stages is set. Out pdd returns
 *   ift/mft/eft pdd entries.
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ftv2_group_pdd_to_stage_pdd_coversion(int unit,
                                           uint32 *in_pdd_entry,
                                           uint32 **out_pdd_entry)
{
    soc_mem_t pdd_mem = INVALIDm;
    uint32 *stage_pdd_entry = NULL;
    int total_mems = 0, mem_idx = 0;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeNone;

    /* Input parameters check */
    if ((in_pdd_entry == NULL) ||
        (out_pdd_entry == NULL)) {
        return BCM_E_PARAM;
    }

    /* Retreive Relevant Memory */
    bcmi_ft_alu_load_pdd_mem_get(unit, bcmiFtTypeIFT, &pdd_mem);

    /* Timestamp 0 */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                       BSD_FLEX_TIMESTAMP_0f)) {
        stage_pdd_entry =
            out_pdd_entry[(BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 0) - 1)];
        soc_mem_field32_set(unit, pdd_mem, stage_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_0f, 1);
    }

    /* Timestamp 1 */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_1f)) {
        stage_pdd_entry =
            out_pdd_entry[(BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 1) - 1)];
        soc_mem_field32_set(unit, pdd_mem, stage_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_1f, 1);
    }

    /* Timestamp 2 */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_2f)) {
        stage_pdd_entry =
            out_pdd_entry[(BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 2) - 1)];
        soc_mem_field32_set(unit, pdd_mem, stage_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_2f, 1);
    }

    /* Timestamp 3 */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_3f)) {
        stage_pdd_entry =
            out_pdd_entry[(BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 3) - 1)];
        soc_mem_field32_set(unit, pdd_mem, stage_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_3f, 1);
    }

    /* Meter Entry - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_FLOW_METERf)) {
        stage_pdd_entry = out_pdd_entry[(bcmiFtTypeIFT - 1)];
        soc_mem_field32_set(unit, pdd_mem, stage_pdd_entry,
                                      BSD_FLEX_FLOW_METERf, 1);
    }

    /* COUNT TO CPU - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_FLOW_COUNT_TO_CPUf)) {
        stage_pdd_entry = out_pdd_entry[(bcmiFtTypeIFT - 1)];
        soc_mem_field32_set(unit, pdd_mem, stage_pdd_entry,
                                      BSD_FLEX_FLOW_COUNT_TO_CPUf, 1);
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
                if ((alu_load_type == bcmiFtAluLoadTypeLoad16) ||
                    (alu_load_type == bcmiFtAluLoadTypeLoad8)) {
                    stage_pdd_entry =
                        out_pdd_entry[(BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                        alu_load_type, 0, mem_idx) - 1)];
                } else {
                    stage_pdd_entry =
                        out_pdd_entry[(BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                                        alu_load_type, mem_idx, 0) - 1)];
                }
                soc_mem_field32_set(unit, pdd_mem, stage_pdd_entry,
                                          pdd_info[mem_idx].param_id, 1);
            }
        }
    }

    /* FLOW STATE - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_FLOW_STATEf)) {
        stage_pdd_entry = out_pdd_entry[(bcmiFtTypeIFT - 1)];
        soc_mem_field32_set(unit, pdd_mem, stage_pdd_entry,
                                      BSD_FLEX_FLOW_STATEf, 1);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_legacy_group_pdd_to_stage_pde_coversion
 * Purpose:
 *   Convert input pdd into stage specific pde.
 * Parameters:
 *   unit          - (IN)  BCM device id
 *   ft_type       - (IN)  Flowtracker Type.
 *   flag          - (IN)  Flags.
 *   in_pdd_entry  - (IN)  Input PDD entry.
 *   out_pde_entry - (OUT) Output PDE entry.
 * Notes:
 *   in_pdd should be stage specific.
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ftv2_legacy_group_pdd_to_stage_pde_coversion(int unit,
                                           bcmi_ft_type_t ft_type,
                                           uint8 flag,
                                           uint32 *in_pdd_entry,
                                           uint32 *out_pde_entry)
{
    int rv = BCM_E_NONE;
    uint8 *pde = NULL;
    soc_mem_t pdd_mem = INVALIDm;
    int idx = 0, mem_idx = 0, total_mems = 0;
    int alu_load_type = bcmiFtAluLoadTypeNone;
    int running_pde_idx = 0, type = 0, end = 0;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    int running_fill_size = 0, running_fill_num = 0;
    uint32 max_fill_size = 0, max_fill_num = 0, offset_jump = 0;
    bcmi_ft_alu_load_type_t pde_order[BCMI_FT_ALU_LOAD_MEMS] = {
        bcmiFtAluLoadTypeAlu32,
        bcmiFtAluLoadTypeLoad16,
        bcmiFtAluLoadTypeAlu16,
        bcmiFtAluLoadTypeLoad8,
        bcmiFtAluLoadTypeAggAlu32
    };

    /* Input parameters check */
    if ((in_pdd_entry == NULL) ||
        (out_pde_entry == NULL)) {
        return BCM_E_INTERNAL;
    }

    /* Retreive Relevant Memory */
    bcmi_ft_alu_load_pdd_mem_get(unit, ft_type, &pdd_mem);

    /* Point to Output PDE Entry */
    pde = (uint8 *)out_pde_entry;

    /* Retreive resource sw state from global structure */
    if (flag & BCMI_FTv2_STAGE_PDE) {
        running_pde_idx =
          BCMI_FT_RESOURCE_SW_INFO_BANK_START_LOCAL_PDE_OFFSET(unit, ft_type);
        offset_jump =
          BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_LOCAL_JUMP(unit, ft_type);
    } else if (flag & BCMI_FTv2_EXPORT_PDE) {
        running_pde_idx =
          BCMI_FT_RESOURCE_SW_INFO_BANK_START_EXPORT_PDE_OFFSET(unit, ft_type);
        offset_jump =
          BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_EXPORT_JUMP(unit, ft_type);
    } else {
        return BCM_E_INTERNAL;
    }
    max_fill_size = running_fill_size =
         BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, ft_type);
    max_fill_num =
         BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, ft_type);

    /* Timestamp0 48bit(6 bytes) */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_0f)) {
        end = 1095;
        _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                                           idx, 6, running_pde_idx,
                                           pde, end, running_fill_size,
                                           running_fill_num,
                                           max_fill_size, max_fill_num,
                                           offset_jump);
    }

    /* Timestamp1 48bit(6 bytes) */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_1f)) {
        end = 1047;
        _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                                           idx, 6, running_pde_idx,
                                           pde, end, running_fill_size,
                                           running_fill_num,
                                           max_fill_size, max_fill_num,
                                           offset_jump);
    }

    /* Timestamp2 48bit(6 bytes) */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_2f)) {
        end = 999;
        _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                                           idx, 6, running_pde_idx,
                                           pde, end, running_fill_size,
                                           running_fill_num,
                                           max_fill_size, max_fill_num,
                                           offset_jump);
    }

    /* Timestamp3 48bit(6 bytes) */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_3f)) {
        end = 951;
        _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                                           idx, 6, running_pde_idx,
                                           pde, end, running_fill_size,
                                           running_fill_num,
                                           max_fill_size, max_fill_num,
                                           offset_jump);
    }

    /* Meter entry 48bit(6 bytes) */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_FLOW_METERf)) {
        end = 903;
        _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                                           idx, 6, running_pde_idx,
                                           pde, end, running_fill_size,
                                           running_fill_num,
                                           max_fill_size, max_fill_num,
                                           offset_jump);
    }

    /* For all the types of memory types. */
    for (type = 0; type < BCMI_FT_ALU_LOAD_MEMS; type++) {

        alu_load_type = pde_order[type];
        pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type);
        if (!pdd_info) {
            continue;
        }

        total_mems = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, alu_load_type);

        if (alu_load_type == bcmiFtAluLoadTypeLoad16) {
            total_mems = TOTAL_GROUP_LOAD16_DATA_NUM;
        } else  if (alu_load_type == bcmiFtAluLoadTypeLoad8) {
            total_mems = TOTAL_GROUP_LOAD8_DATA_NUM;
            if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                              BSD_FLEX_FLOW_COUNT_TO_CPUf)) {
                end = 87;
                _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                                                   idx, 2,
                                                   running_pde_idx,
                                                   pde, end, running_fill_size,
                                                   running_fill_num,
                                                   max_fill_size, max_fill_num,
                                                   offset_jump);
            }
        }

        /* For all the alu/load types, check what memories are set in pdd. */
        for (mem_idx = 0; mem_idx < total_mems; mem_idx++) {
            /* Now get if this particular field is set in pdd. */
            if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                              pdd_info[mem_idx].param_id)) {
                _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                                                   idx,
                                                   pdd_info[mem_idx].length,
                                                   running_pde_idx,
                                                   pde, (pdd_info[mem_idx].end),
                                                   running_fill_size,
                                                   running_fill_num,
                                                   max_fill_size, max_fill_num,
                                                   offset_jump);
            }
        }
    }

    /* Flow State */
    if (soc_mem_field32_get(unit, pdd_mem, in_pdd_entry,
                                      BSD_FLEX_FLOW_STATEf)) {
        end = 7;
        _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                                           idx, 1,
                                           running_pde_idx,
                                           pde, end, running_fill_size,
                                           running_fill_num,
                                           max_fill_size, max_fill_num,
                                           offset_jump);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_legacy_group_pdd_policy_profile_add
 * Purpose:
 *   Add policy profile entry for group action profile.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_legacy_group_pdd_policy_profile_add(int unit,
                                              bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    uint32 index = 0;
    uint32 *pdd_entry_tmp[3];
    uint32 *global_pdd_entry = NULL;
    int old_index = 0, ref_count = 0;
    bsc_dt_pde_profile_0_entry_t pde_entry;
    bsc_kg_group_table_entry_t kg_group_entry;
    bsd_policy_action_profile_0_entry_t pdd_entry[3];

    /* Default entry for PDD & PDE memory */
    sal_memset(&pdd_entry, 0, sizeof(pdd_entry));
    sal_memset(&pde_entry, 0, sizeof(pde_entry));

    /* Retreive Global PDD Entry from Group Structure */
    global_pdd_entry = (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry));

    /* Point temp entry pointer to individual stage pdd entries */
    pdd_entry_tmp[0] = (uint32 *)&pdd_entry[0];
    pdd_entry_tmp[1] = (uint32 *)&pdd_entry[1];
    pdd_entry_tmp[2] = (uint32 *)&pdd_entry[2];

    /* Retreive stage specific pdd entry from global pdd */
    rv = bcmi_ftv2_group_pdd_to_stage_pdd_coversion(unit,
                                                   global_pdd_entry,
                                                  (uint32 **)(&pdd_entry_tmp));
    BCM_IF_ERROR_RETURN(rv);

    /* First read the group entry. */
    rv = soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &kg_group_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
                        ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);
    if (old_index) {
        bcmi_ft_group_pdd_profile_entry_delete(unit, old_index);
    }

    /* Create the profile entry for new pdd entries profile. */
    rv = bcmi_ft_group_pdd_profile_entry_add(unit, (void **)pdd_entry_tmp, 1,
                                                            (uint32 *)&index);
    BCM_IF_ERROR_RETURN(rv);

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm,
                      ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf, index);

    /* Write the entry back into the group table. */
    rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm,
                       MEM_BLOCK_ANY, id, &kg_group_entry);
    BCMI_CLEANUP_IF_ERROR(rv);

    rv = bcmi_ft_group_pdd_profile_refcount_get(unit, index, &ref_count);
    BCMI_CLEANUP_IF_ERROR(rv);

    if (ref_count == 1) {

        /* This is a new entry. Write the PDE entry also at same index. */
        rv = bcmi_ftv2_legacy_group_pdd_to_stage_pde_coversion(unit,
                                                        bcmiFtTypeIFT,
                                                        BCMI_FTv2_STAGE_PDE,
                                                        pdd_entry_tmp[0],
                                                       (uint32 *)(&pde_entry));
        if (rv == BCM_E_RESOURCE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "IFT session data memory is not enough to fill.")));
        }
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = soc_mem_write(unit, BSC_DT_PDE_PROFILE_0m,
                           MEM_BLOCK_ANY, index, &pde_entry);
        BCMI_CLEANUP_IF_ERROR(rv);

        sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
        rv = bcmi_ftv2_legacy_group_pdd_to_stage_pde_coversion(unit,
                                                        bcmiFtTypeMFT,
                                                        BCMI_FTv2_STAGE_PDE,
                                                        pdd_entry_tmp[1],
                                                       (uint32 *)(&pde_entry));
        if (rv == BCM_E_RESOURCE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "MFT session data memory is not enough to fill.")));
        }
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = soc_mem_write(unit, BSC_DT_PDE_PROFILE_1m,
                           MEM_BLOCK_ANY, index, &pde_entry);
        BCMI_CLEANUP_IF_ERROR(rv);

        sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
        rv = bcmi_ftv2_legacy_group_pdd_to_stage_pde_coversion(unit,
                                                        bcmiFtTypeEFT,
                                                        BCMI_FTv2_STAGE_PDE,
                                                        pdd_entry_tmp[2],
                                                       (uint32 *)(&pde_entry));
        if (rv == BCM_E_RESOURCE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "EFT session data memory is not enough to fill.")));
        }
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = soc_mem_write(unit, BSC_DT_PDE_PROFILE_2m,
                           MEM_BLOCK_ANY, index, &pde_entry);
        BCMI_CLEANUP_IF_ERROR(rv);

        sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
        rv = bcmi_ftv2_legacy_group_pdd_to_stage_pde_coversion(unit,
                                                        bcmiFtTypeIFT,
                                                        BCMI_FTv2_EXPORT_PDE,
                                                        pdd_entry_tmp[0],
                                                       (uint32 *)(&pde_entry));
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_0m,
                           MEM_BLOCK_ANY, index, &pde_entry);
        BCMI_CLEANUP_IF_ERROR(rv);

        sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
        rv = bcmi_ftv2_legacy_group_pdd_to_stage_pde_coversion(unit,
                                                        bcmiFtTypeMFT,
                                                        BCMI_FTv2_EXPORT_PDE,
                                                        pdd_entry_tmp[1],
                                                       (uint32 *)(&pde_entry));
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_1m,
                           MEM_BLOCK_ANY, index, &pde_entry);
        BCMI_CLEANUP_IF_ERROR(rv);

        sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
        rv = bcmi_ftv2_legacy_group_pdd_to_stage_pde_coversion(unit,
                                                        bcmiFtTypeEFT,
                                                        BCMI_FTv2_EXPORT_PDE,
                                                        pdd_entry_tmp[2],
                                                       (uint32 *)(&pde_entry));
        BCMI_CLEANUP_IF_ERROR(rv);

        rv = soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_2m,
                           MEM_BLOCK_ANY, index, &pde_entry);
        BCMI_CLEANUP_IF_ERROR(rv);
    }

    return rv;

cleanup:

    sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
    bcmi_ft_group_pdd_profile_entry_delete(unit, index);
    soc_mem_write(unit,
                  BSC_DT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
                  BSC_DT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
                  BSC_DT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
                  BSC_DT_EXPORT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
                  BSC_DT_EXPORT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
                  BSC_DT_EXPORT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_legacy_group_template_data_create
 * Purpose:
 *   Create template list for group.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) Flowtracker_group.
 *   load16_indexes - (IN) load16 element array.
 *   load8_indexes  - (IN) load8 element array.
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_legacy_group_template_data_create(int unit,
                                     bcm_flowtracker_group_t id,
                                     int *load16_indexes,
                                     int *load8_indexes)
{
    int rv = BCM_E_NONE;
    int i = 0;
    soc_mem_t mem;
    int copy_to_cpu = 0;
    int meter_enabled = 0;
    int running_length = 0;
    uint32 *pdd_entry_tmp[3];
    bcmi_ft_type_t ft_type_tmp = 0;
    uint32 *global_pdd_entry = NULL;
    bcmi_ft_group_template_list_t **head;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_alu_info_t *start = NULL;
    bcmi_ft_group_alu_info_t *local = NULL;
    bsd_policy_action_profile_0_entry_t pdd_entry[3];
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeNone;
    bcmi_ft_group_param_type_t ts_type = bcmiFtGroupParamTypeTsNewLearn;

    /* Aggregate Does not support PDD Profile */
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id))) {
        return bcmi_ftv3_group_aggregate_template_data_create(unit, id);
    }
#endif

    /* Default entry for PDD & PDE memory */
    sal_memset(&pdd_entry, 0, sizeof(pdd_entry));

    /* First we should look into the in order of the PDD data. */
    /* Get the head of the templs info list. */
    head = &((BCMI_FT_GROUP(unit, id))->template_head);

    /* Retreive Global PDD Entry from Group Structure */
    global_pdd_entry = (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry));

    /* Point temp entry pointer to individual stage pdd entries */
    pdd_entry_tmp[0] = (uint32 *)&pdd_entry[0];
    pdd_entry_tmp[1] = (uint32 *)&pdd_entry[1];
    pdd_entry_tmp[2] = (uint32 *)&pdd_entry[2];

    /* Retreive stage specific pdd entry from global pdd */
    rv = bcmi_ftv2_group_pdd_to_stage_pdd_coversion(unit,
                                                    global_pdd_entry,
                                        (uint32 **)(&pdd_entry_tmp));
    BCM_IF_ERROR_RETURN(rv);

    /* assign the ALU memory chunk to local pointer. */
    start = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    temp = start;

    for (ft_type_tmp = bcmiFtTypeIFT; ft_type_tmp <= bcmiFtTypeEFT; ft_type_tmp++) {

        /* retreive start of container for each stage */
        if (ft_type_tmp == bcmiFtTypeIFT) {
            running_length = 0;
        }  else if (ft_type_tmp == bcmiFtTypeMFT) {
            running_length =
               ((BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, bcmiFtTypeIFT) *
               (BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, bcmiFtTypeIFT) + 1)) * 8);
        } else {
            running_length =
                (((BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, bcmiFtTypeIFT) *
                   (BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, bcmiFtTypeIFT) + 1)) * 8) +
                 ((BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, bcmiFtTypeMFT) *
                  (BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, bcmiFtTypeMFT) + 1)) * 8));
        }

        /* retreive relevant pdd mem for device */
        bcmi_ft_alu_load_pdd_mem_get(unit, ft_type_tmp, &mem);

        /* Start setting the space for 4 timestamps. */
        if (soc_mem_field32_get(unit, mem,
                    pdd_entry_tmp[ft_type_tmp - 1], BSD_FLEX_TIMESTAMP_0f)) {
            ts_type = bcmi_ft_group_get_ts_type(unit, id, 0);
            rv = bcmi_ft_group_template_list_add(unit, head,
                                          running_length, 48, ts_type, NULL);
            BCMI_CLEANUP_IF_ERROR(rv);

            /* Timestamp 0 is enabled. length = 48 bits. */
            running_length += 48;
        }

        /* Start setting the space for 4 timestamps. */
        if (soc_mem_field32_get(unit, mem,
                    pdd_entry_tmp[ft_type_tmp - 1], BSD_FLEX_TIMESTAMP_1f)) {
            /* Timestamp 1 is enabled. length = 48 bits. */
            ts_type = bcmi_ft_group_get_ts_type(unit, id, 1);

            rv = bcmi_ft_group_template_list_add(unit, head,
                                          running_length, 48, ts_type, NULL);
            BCMI_CLEANUP_IF_ERROR(rv);

            /* Timestamp 1 is enabled. length = 48 bits. */
            running_length += 48;
        }

        /* Start setting the space for 4 timestamps. */
        if (soc_mem_field32_get(unit, mem,
                    pdd_entry_tmp[ft_type_tmp - 1], BSD_FLEX_TIMESTAMP_2f)) {
            /* Timestamp 2 is enabled. length = 48 bits. */
            ts_type = bcmi_ft_group_get_ts_type(unit, id, 2);

            rv = bcmi_ft_group_template_list_add(unit, head,
                                          running_length, 48, ts_type, NULL);
            BCMI_CLEANUP_IF_ERROR(rv);

            /* Timestamp 2 is enabled. length = 48 bits. */
            running_length += 48;
        }

        /* Start setting the space for 4 timestamps. */
        if (soc_mem_field32_get(unit, mem,
                    pdd_entry_tmp[ft_type_tmp - 1], BSD_FLEX_TIMESTAMP_3f)) {
            /* Timestamp 3 is enabled. length = 48 bits. */
            ts_type = bcmi_ft_group_get_ts_type(unit, id, 3);

            rv = bcmi_ft_group_template_list_add(unit, head,
                                          running_length, 48, ts_type, NULL);
            BCMI_CLEANUP_IF_ERROR(rv);
            /* Timestamp 3 is enabled. length = 48 bits. */
            running_length += 48;
        }

        if (ft_type_tmp == bcmiFtTypeIFT) {
            meter_enabled = soc_mem_field32_get(unit, mem,
                               pdd_entry_tmp[bcmiFtTypeIFT - 1],
                                          BSD_FLEX_FLOW_METERf);
            if (meter_enabled) {
                /* 48 bits are used for metering. */
                rv = bcmi_ft_group_template_list_add(unit, head,
                        running_length, 48, bcmiFtGroupParamTypeMeter, NULL);
                BCMI_CLEANUP_IF_ERROR(rv);
                running_length += 48;
            }
        }

        /* Add ALU32s into the template list based on PDD. */
        for (i = 0; i < TOTAL_GROUP_ALU32_MEM; i++) {

            alu_load_type = bcmiFtAluLoadTypeAlu32;

            if (bcmi_ft_group_alu_load_pdd_status_get(unit, id,
                     pdd_entry_tmp[ft_type_tmp - 1], alu_load_type, i)) {

                /* Check for this index and get corresponding data. */
                rv = bcmi_ft_group_alu_load_index_match_data_get(unit, id,
                                                  alu_load_type, i, &temp);
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
        for (i = 0; i < TOTAL_GROUP_LOAD16_DATA_NUM; i++) {
            int len=0;
            if (load16_indexes[i] == -1) {
                continue;
            }
            local = (&(start[load16_indexes[i]]));

            if (local->alu_ft_type != ft_type_tmp) {
                continue;
            }

            if ((local->element_type1 ==
                        bcmFlowtrackerTrackingParamTypeDosAttack) ||
                (local->element_type1 ==
                        bcmFlowtrackerTrackingParamTypeInnerDosAttack)) {
                /* Add 16 bit length to display proper results in export. */
                len = 16;
                i++;
            }

            /* If there is some data then add it into list. */
            rv = bcmi_ft_group_template_list_add(unit, head,
                    running_length, 16+len, bcmiFtGroupParamTypeTracking, local);
            BCMI_CLEANUP_IF_ERROR(rv);

            running_length += (16+len);
        }

        temp = start;
        /* Add ALU16s into template list based on PDD. */
        for (i = 0; i < TOTAL_GROUP_ALU16_MEM; i++) {
            alu_load_type = bcmiFtAluLoadTypeAlu16;

            if (bcmi_ft_group_alu_load_pdd_status_get(unit, id,
                    pdd_entry_tmp[ft_type_tmp - 1], alu_load_type, i)) {

                /* Check for this index and get corresponding data. */
                rv = bcmi_ft_group_alu_load_index_match_data_get(unit, id,
                                                  alu_load_type, i, &temp);
                if (BCM_FAILURE(rv)) {
                    continue;
                }
                rv = bcmi_ft_group_template_list_add(unit, head,
                        running_length, 16, bcmiFtGroupParamTypeTracking, temp);
                BCMI_CLEANUP_IF_ERROR(rv);

                running_length += 16;
            }
        }

        if (ft_type_tmp == bcmiFtTypeIFT) {
            copy_to_cpu = soc_mem_field32_get(unit, mem,
                            pdd_entry_tmp[bcmiFtTypeIFT - 1],
                                BSD_FLEX_FLOW_COUNT_TO_CPUf);
            if (copy_to_cpu) {
                rv = bcmi_ft_group_template_list_add(unit, head,
                            running_length, 16, bcmiFtGroupParamTypeCollectorCopy, NULL);
                BCMI_CLEANUP_IF_ERROR(rv);

                running_length += 16;
            }
        }

        /* Get all the elements of load8 and then add them into order. */
        for (i = 0; i < TOTAL_GROUP_LOAD8_DATA_NUM;  i++) {
            if (load8_indexes[i] == -1) {
                continue;
            }
            local = (&(start[load8_indexes[i]]));

            if (local->alu_ft_type != ft_type_tmp) {
                continue;
            }

            rv = bcmi_ft_group_template_list_add(unit, head,
                    running_length, 8, bcmiFtGroupParamTypeTracking, local);
            BCMI_CLEANUP_IF_ERROR(rv);

            running_length += 8;
        }
    }

    /* We can put FT_STATE here but anyways it is not exposed to user. */
    return BCM_E_NONE;

cleanup :
    bcmi_ft_group_template_list_delete_all(unit, head);
    return rv;
}
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

/*
 * Function:
 *   bcmi_ft_alu_load_type_resolve
 * Purpose:
 *   Get alu_load_type for given alu_info of the group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   alu_info   - (IN) Alu load info
 *   type       - (OUT) alu_load_type
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_load_type_resolve(
    int unit,
    bcm_flowtracker_group_t id,
    bcmi_ft_group_alu_info_t *alu_info,
    bcmi_ft_alu_load_type_t *type)
{
    int rv = BCM_E_NONE;
    int length = 0, alu = 0;

    /* Update alu_info for supported alu_load type of group */
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        rv = bcmi_ftv3_alu_load_info_flags_set(unit, id, alu_info);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif

    /* Normalize length and alu status from multiple keys. */
    rv = bcmi_ft_alu_load_values_normalize(unit, alu_info, &length, &alu);
    BCM_IF_ERROR_RETURN(rv);

    /* Now get the type of the memory and take action. */
    rv = bcmi_ft_alu_load_type_get(unit, alu, length, type);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_ft_mode_recompute
 * Purpose:
 *   Derive new session key/data mode based on latest
 *   tracking params key/data extraction information.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_ft_mode_recompute(
    int unit,
    bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    bcmi_ft_group_key_data_mode_t mode;

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    /* Set the single/double session mode based on extraction length. */
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        rv = bcmi_ftv2_group_ft_mode_update(unit, id);
    } else
#endif
    {
        rv = bcmi_ft_group_ft_mode_update(unit, id);
    }
    BCM_IF_ERROR_RETURN(rv);

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

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_new_learn_export_trigger_set
 * Purpose:
 *   Set export trigger for new learn to a group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   flags     -  (IN) Export trigger flags
 *   val        - (IN) New learn exrport trigger
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_new_learn_export_trigger_set(int unit,
                bcm_flowtracker_group_t id,
                uint32 flags,
                uint32 val)
{
    int rv = BCM_E_NONE;
    int index = -1;
    uint32 curr_val = 0;
    uint32 reservation = 0;
    bsc_kg_group_table_entry_t kg_entry;

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    index = BCMI_FT_GROUP_INDEX_GET(id);

    rv = soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, index, &kg_entry);
    BCM_IF_ERROR_RETURN(rv);

    curr_val = soc_mem_field32_get(unit,
            BSC_KG_GROUP_TABLEm, &kg_entry, EXPORT_LEARNf);

    if (curr_val != val) {
        soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm,
                &kg_entry, EXPORT_LEARNf, val);
    }

    /* Set IPFIX Reservation */
    reservation = 0;
    if ((val != 0) &&
            (flags & BCM_FLOWTRACKER_EXPORT_TRIGGER_LEARN_GUARANTEE)) {
        reservation = 1;
    }

    rv = bcmi_ft_group_flow_exceed_profile_set(unit,
            id, (uint32 * ) &kg_entry,
            BCMI_FT_GROUP_FLOW_EXCEED_PROFILE_RESERVATION, reservation);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ALL, index, &kg_entry);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_new_learn_export_trigger_get
 * Purpose:
 *   Get export trigger for new learn to a group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   flgs       - (OUT) Export trigger flags
 *   val        - (OUT) New learn exrport trigger
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_new_learn_export_trigger_get(int unit,
                bcm_flowtracker_group_t id,
                uint32 *flags,
                uint32 *val)
{
    int rv = BCM_E_NONE;
    int group_index = -1;
    uint32 index = 0;
    uint32 flow_exceedfmt, data = 0;
    bsc_kg_group_table_entry_t kg_entry;
    bsc_kg_flow_exceed_profile_entry_t entry;

    *val = *flags = 0;
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    group_index = BCMI_FT_GROUP_INDEX_GET(id);

    rv = soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, group_index, &kg_entry);
    BCM_IF_ERROR_RETURN(rv);

    *val = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
            &kg_entry, EXPORT_LEARNf);

    /* initialize the format entry. */
    sal_memset(&flow_exceedfmt, 0, sizeof(uint32));
    sal_memset(&entry, 0, sizeof(bsc_kg_age_out_profile_entry_t));

    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
            ((uint32 *)&kg_entry), FLOW_EXCEED_PROFILE_IDXf);

    /* Read the profile entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_FLOW_EXCEED_PROFILEm,
                      MEM_BLOCK_ANY, index, &entry));

    soc_mem_field_get(unit, BSC_KG_FLOW_EXCEED_PROFILEm,
            (uint32 *)&entry, DATAf, &flow_exceedfmt);

    data = soc_format_field32_get(unit,
            BSC_TL_KG_TO_KT_LIMITS_CONTROL_BUSfmt,
            &flow_exceedfmt, DO_NOT_LEARN_WHEN_FIFO_FULLf);

    if (data) {
        *flags = BCM_FLOWTRACKER_EXPORT_TRIGGER_LEARN_GUARANTEE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_timer_export_trigger_set
 * Purpose:
 *   Set export trigger for periodic timer to a group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   val        - (IN) Periodic timer exrport trigger
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_timer_export_trigger_set(int unit,
                bcm_flowtracker_group_t id,
                uint32 val)
{
    int rv = BCM_E_NONE;
    int index = -1;
    uint32 curr_val = 0;
    uint32 *entry = NULL;
    soc_mem_t mem;
    bcm_flowtracker_group_type_t type;

    index = BCM_FLOWTRACKER_GROUP_INDEX_GET(id);
    type = BCM_FLOWTRACKER_GROUP_TYPE_GET(id);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
        soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {
        if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
            if (val == 1) {
                BCMI_FT_GROUP(unit, id)->flags |=
                        BCMI_FT_GROUP_INFO_F_PERIODIC_EXPORT;
            } else {
                BCMI_FT_GROUP(unit, id)->flags &=
                        (~BCMI_FT_GROUP_INFO_F_PERIODIC_EXPORT);
            }
            rv = bcmi_ftv3_aft_export_sw_grp_timer_set(unit, id, val);
            return rv;
        }
    }
#endif

    rv = bcmi_ft_group_type_mem_get(unit, 1, type, &mem);
    BCM_IF_ERROR_RETURN(rv);

    BCMI_FT_ALLOC(entry, sizeof(uint32) * soc_mem_entry_words(unit, mem),
            "ft group entry");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry);
    if (BCM_FAILURE(rv)) {
        BCMI_FT_FREE(entry);
        return rv;
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, PERIODIC_EXPORT_ENf)) {
        curr_val = soc_mem_field32_get(unit, mem, entry, PERIODIC_EXPORT_ENf);

        if (curr_val != val) {
            soc_mem_field32_set(unit, mem, entry, PERIODIC_EXPORT_ENf, val);

            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
        }
    }

    BCMI_FT_FREE(entry);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_timer_export_trigger_get
 * Purpose:
 *   Get export trigger for periodic timer to a group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   val        - (OUT) Periodic timer exrport trigger
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_timer_export_trigger_get(int unit,
                bcm_flowtracker_group_t id,
                uint32 *val)
{
    int rv = BCM_E_NONE;
    int index = -1;
    uint32 *entry = NULL;
    soc_mem_t mem;
    bcm_flowtracker_group_type_t type;

    *val = 0;
    type = BCM_FLOWTRACKER_GROUP_TYPE_GET(id);
    index = BCM_FLOWTRACKER_GROUP_INDEX_GET(id);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
        soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {
        if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
            if (BCMI_FT_GROUP(unit, id)->flags &
                    BCMI_FT_GROUP_INFO_F_PERIODIC_EXPORT) {
                *val = 1;
            }
        }
    }
#endif

    rv = bcmi_ft_group_type_mem_get(unit, 1, type, &mem);
    BCM_IF_ERROR_RETURN(rv);

    BCMI_FT_ALLOC(entry, sizeof(uint32) * soc_mem_entry_words(unit, mem),
            "ft group entry");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry);
    if (BCM_FAILURE(rv)) {
        BCMI_FT_FREE(entry);
        return rv;
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, PERIODIC_EXPORT_ENf)) {
        *val = soc_mem_field32_get(unit, mem, entry, PERIODIC_EXPORT_ENf);
    }

    BCMI_FT_FREE(entry);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_pdd_pde_policy_add
 * Purpose:
 *   Add PDD and PDE profiles to the group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_pdd_pde_policy_add(int unit,
                                 bcm_flowtracker_group_t id)
{
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
        soc_feature(unit, soc_feature_flowtracker_coupled_data_engines)) {
        return bcmi_ftv2_group_pdd_policy_profile_add(unit, id);
    } else if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        return bcmi_ftv2_legacy_group_pdd_policy_profile_add(unit, id);
    }
#endif

    /* version 1 */
    return bcmi_ft_group_pdd_policy_profile_add(unit, id);
}

/*
 * Function:
 *   bcmi_ft_group_template_data_add
 * Purpose:
 *   Add template information to a group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_template_data_add(int unit,
                    bcm_flowtracker_group_t id,
                    int *load16_indexes,
                    int *load8_indexes)
{
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
        soc_feature(unit, soc_feature_flowtracker_coupled_data_engines)) {
        return bcmi_ftv2_group_template_data_create(unit,
                                id, load16_indexes, load8_indexes);
    } else if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        return bcmi_ftv2_legacy_group_template_data_create(unit,
                                id, load16_indexes, load8_indexes);
    }
#endif

    /* version 1 */
    return bcmi_ft_group_template_data_create(unit,
            id, load16_indexes, load8_indexes);
}

/*
 * Function:
 *   bcmi_ft_group_data_template_print
 * Purpose:
 *   Print Template data for the Group
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker Group.
 * Returns:
 *   None.
 */
int
bcmi_ft_group_data_template_print(int unit,
                                  bcmi_ft_group_template_list_t *head)
{
    int idx = 0;
    bcmi_ft_group_template_info_t *info = NULL;
    bcmi_ft_group_template_list_t *node = NULL;
    const char *param_strings[] = BCM_FT_TRACKING_PARAM_STRINGS;
    const char *param_type_strings[] = BCMI_FT_GROUP_PARAM_TYPE_STRINGS;

    node = head;
    idx = 0;
    while(node != NULL) {
        info = &node->info;

        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        " [%2d] -> type=%s param=%s(%d) check=0x%x flags=0x%x\n"),
                    idx, param_type_strings[info->param_type],
                    param_strings[info->param], info->custom_id,
                    info->check_id, info->flags));

        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "       -> (%s)[%d] - c_o=%d c_w=%d"
                        " d_s=%d d_w=%d\n"), alu_type_str[info->type],
                    info->index, info->cont_offset, info->data_shift,
                    info->data_width, info->cont_width));
        node = node->next;
        idx++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_group_tracking_param_mask_status_get
 * Purpose:
 *   Get mask status of tracking param key mask.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   tp_info           - (IN) Tracking param info.
 *   qual_size         - (IN) Size of corresponding qualifier.
 *   mask_present      - (OUT) Is mask present
 *   partial_mask      - (OUT) Mask position where mask is not 0xFFFF.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_tracking_param_mask_status_get(
    int unit,
    bcm_flowtracker_tracking_param_info_t *tp_info,
    uint16 qual_size,
    int *mask_present,
    int *partial_mask)
{
    int rem_size = 0;
    int bytes_seen = 0;
    int mask_pos = 0;
    uint8 *ptr = NULL;
    uint16 u16mask1, u16mask2;
    uint32 q_mask_bitmap;

    ptr = tp_info->mask;
    rem_size = qual_size;
    while(rem_size > 0) {
        q_mask_bitmap = 0;
        if (rem_size >= 4) {
            _SHR_UNPACK_U32(ptr, q_mask_bitmap);
            bytes_seen = 4;
        } else if (rem_size >= 3) {
            _SHR_UNPACK_U24(ptr, q_mask_bitmap);
            bytes_seen = 3;
        } else if (rem_size >= 2) {
            _SHR_UNPACK_U16(ptr, q_mask_bitmap);
            bytes_seen = 2;
        } else {
            _SHR_UNPACK_U8(ptr, q_mask_bitmap);
            bytes_seen = 1;
        }
        if (q_mask_bitmap != 0) {
            if (mask_present != NULL) {
                *mask_present = 1;
            }
            if ((partial_mask != NULL) && (*partial_mask == -1)) {
                u16mask1 = q_mask_bitmap & 0xffff;
                u16mask2 = (q_mask_bitmap >> 16) & 0xffff;
                if ((u16mask2 != 0) && (u16mask2 != 0xffff)) {
                    *partial_mask = (rem_size);
                } else if ((u16mask1 != 0) && (u16mask1 != 0xffff)) {
                    mask_pos = (rem_size >= 2) ? 2 : rem_size;
                    *partial_mask = (rem_size - mask_pos);
                }
            }
        }
        rem_size -= bytes_seen;
    }

    return BCM_E_NONE;
}

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
/*
 * Function:
 *   bcmi_ftv3_group_aggregate_entry_update
 * Purpose:
 *   Update hw entry for aggregate flowtracker group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   info       - (IN) Group Info.
 *   create     - (IN) create or destroy
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv3_group_aggregate_entry_update(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_group_info_t *info,
    int create)
{
    int rv = BCM_E_NONE;
    int index = -1;
    soc_mem_t mem = INVALIDm;
    bcm_flowtracker_group_type_t type;

    type = BCM_FLOWTRACKER_GROUP_TYPE_GET(id);
    index = BCM_FLOWTRACKER_GROUP_INDEX_GET(id);

    rv = bcmi_ft_group_type_mem_get(unit, 1, type, &mem);
    BCM_IF_ERROR_RETURN(rv);

    if (SOC_MEM_FIELD_VALID(unit, mem, AGG_GROUP_VALIDf)) {
        soc_mem_field32_modify(unit, mem,
                index, AGG_GROUP_VALIDf, (create ? 1 : 0));
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ftv3_alu_load_info_flags_set
 * Purpose:
 *   Update alu load info flags based on group type
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   alu_info   - (IN/OUT) alu load info.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv3_alu_load_info_flags_set(int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *alu_info)
{
    if (!BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    alu_info->flags |= BCMI_FT_ALU_LOAD_USE_AGG_ALU32;

    /* Session Data, update action key */
    if (alu_info->flowchecker_id == 0) {
        sal_memcpy(&alu_info->action_key, &alu_info->key1,
                    sizeof (bcmi_ft_alu_key_t));
        alu_info->action_element_type = alu_info->element_type1;
        alu_info->action_custom_id = alu_info->custom_id1;
    }

    return BCM_E_NONE;
}
#endif

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
/*
 * Function:
 *   bcmi_ftv2_group_internal_alu_info_create
 * Purpose:
 *   Create internal alu_info for the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker Group.
 *   param - (IN) Flowtracker tracking param
 *   int_check_id - (IN) Internal check Id for new alu_info
 *   check_alu_info - (IN) alu_infro to derive new alu_info
 *   alu_info - (OUT) New alu_info
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_internal_alu_info_create(int unit,
                bcm_flowtracker_group_t id,
                bcm_flowtracker_tracking_param_type_t param,
                bcm_flowtracker_check_t int_check_id,
                bcmi_ft_group_alu_info_t *check_alu_info,
                bcmi_ft_group_alu_info_t *alu_info)
{
    int upper16b_pos = 0;

    sal_memset(alu_info, 0, sizeof(bcmi_ft_group_alu_info_t));

    /* For NTP64b in FB6 upper 16b are in fixed position 240
     * whereas in HR4, it comes from vfp opaqueid2 action which
     * is extracted dynamically and stored in group struct */
    if (soc_feature(unit, soc_feature_flowtracker_ts_64_sw_war_support)) {
        upper16b_pos =
            BCMI_FT_GROUP_EXT_HW_INFO_VFP_OPAQUEID2_START_BIT(unit, id);
    } else {
        upper16b_pos = BCMI_FT_TS_NTP64_UPPER16_LOC;
    }

    switch(param) {

        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
        case bcmFlowtrackerTrackingParamTypeTimestampIngress:
            alu_info->flags = BCMI_FT_ALU_LOAD_INTERNAL;
            alu_info->flags |= BCMI_FT_ALU_LOAD_EXTENDED;
            alu_info->key1.location = upper16b_pos;
            alu_info->key1.is_alu = 0;
            alu_info->key1.length = 16;
            alu_info->element_type1 = param;
            alu_info->alu_ft_type = bcmiFtTypeIFT;
            break;

        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
            alu_info->flags = BCMI_FT_ALU_LOAD_INTERNAL;
            alu_info->flags |= BCMI_FT_ALU_LOAD_EXTENDED;
            alu_info->key1.location = 0;
            alu_info->key1.is_alu = 1;
            alu_info->key1.length = 16;
            alu_info->action_key.location = upper16b_pos;
            alu_info->action_key.is_alu = 1;
            alu_info->action_key.length = 16;
            alu_info->element_type1 = param;
            alu_info->action_element_type = param;
            alu_info->flowchecker_id = int_check_id;
            alu_info->alu_ft_type = bcmiFtTypeIFT;
            break;

        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:
            /* Copy Check alu info */
            sal_memcpy(alu_info, check_alu_info,
                        sizeof(bcmi_ft_group_alu_info_t));

            alu_info->flags |= BCMI_FT_ALU_LOAD_INTERNAL;
            alu_info->flags |= BCMI_FT_ALU_LOAD_EXTENDED;

            /* Update Action */
            alu_info->action_key.is_alu = 1;
            alu_info->action_key.length = 16;
            alu_info->action_element_type = param;
            alu_info->flowchecker_id = int_check_id;
            alu_info->alu_ft_type = check_alu_info->alu_ft_type;
            alu_info->action_key.location = upper16b_pos;

            /* Mark internal shift adjusted */
            if (upper16b_pos % 32 != 0) {
                alu_info->flags |= BCMI_FT_ALU_LOAD_INTERNAL_DATA_SHIFT_ADJUSTED;
            }
            break;

        default:
            break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_group_fc_alu_info_find
 * Purpose:
 *   Search group alu_info for check with timestamp
 *   event set.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker Group.
 *   param- (IN) Tracking param
 *   check_alu_info - (OUT) pointer to found alu_info
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_fc_alu_info_find(int unit,
                    bcm_flowtracker_group_t id,
                    bcm_flowtracker_tracking_param_type_t param,
                    bcmi_ft_group_alu_info_t **check_alu_info)
{
    int rv = BCM_E_NONE;
    int i = 0;
    int num_alu_loads = 0;
    uint32 check_event_flag = 0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_flowtracker_flowchecker_info_t fc_info;

    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    num_alu_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    if (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1) {
        check_event_flag =
            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
    } else if (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2) {
        check_event_flag =
            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2;
    } else if (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3) {
        check_event_flag =
            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3;
    } else if (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4) {
        check_event_flag =
            BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4;
    } else {
        return BCM_E_PARAM;
    }

    for (i = 0; i < num_alu_loads; i++) {
        if (temp[i].flowchecker_id != 0) {

            sal_memset(&fc_info, 0,
                    sizeof(bcmi_flowtracker_flowchecker_info_t));
            rv = bcmi_ft_flowchecker_get(unit,
                    temp[i].flowchecker_id, &fc_info);
            BCM_IF_ERROR_RETURN(rv);

            if (fc_info.flags & check_event_flag) {
                *check_alu_info = &temp[i];
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ftv2_group_internal_alu_info_add
 * Purpose:
 *   Append Internal alu_info to extracted alu_info
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker group Id.
 * Returns:
 *   None.
 */
int
bcmi_ftv2_group_internal_alu_info_add(int unit,
                                      bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    int i = 0, idx = 0;
    int num_alu_loads = 0;
    int num_tracking_params = 0;
    bcm_flowtracker_check_t int_check_id;
    bcm_flowtracker_tracking_param_info_t *t_info;
    bcmi_ft_group_alu_info_t *new_alu_info = NULL;
    bcmi_ft_group_alu_info_t *temp_alu_info = NULL;
    bcmi_ft_group_alu_info_t *group_alu_info = NULL;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    bcmi_ft_group_alu_info_t int_alu_info_arr[BCMI_FT_INT_ALU_INFO_MAX];

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }
#endif

    t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    num_tracking_params = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);

    /* For alu_info which needs to be extended */
    for (i = 0, idx = 0; i < num_tracking_params; i++) {

        if ((t_info[i].flags & BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) ||
            (!((soc_feature(unit, soc_feature_flowtracker_ts_ntp64_support) &&
                    (t_info[i].src_ts == bcmFlowtrackerTimestampSourceNTP)) ||
               (soc_feature(unit, soc_feature_flowtracker_ts_64_sw_war_support) &&
                   ((t_info[i].src_ts == bcmFlowtrackerTimestampSourceNTP) ||
                    (t_info[i].src_ts == bcmFlowtrackerTimestampSourcePTP)))))) {
            continue;
        }

        switch(t_info[i].param) {

            case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
            case bcmFlowtrackerTrackingParamTypeTimestampIngress:

                rv = bcmi_ftv2_group_internal_alu_info_create(unit, id,
                        t_info[i].param, 0, NULL, &int_alu_info_arr[idx]);
                BCM_IF_ERROR_RETURN(rv);

                idx++;
                break;

            case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
            case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:

                rv = bcmi_ftv2_group_internal_flowcheck_create(unit,
                        id, t_info[i].param, NULL, &int_check_id);
                BCM_IF_ERROR_RETURN(rv);

                rv = bcmi_ftv2_group_internal_alu_info_create(unit,
                        id, t_info[i].param, int_check_id, NULL,
                        &int_alu_info_arr[idx]);
                BCM_IF_ERROR_RETURN(rv);

                idx++;
                break;

            case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
            case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
            case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
            case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:

                rv = bcmi_ftv2_group_fc_alu_info_find(unit,
                        id, t_info[i].param, &temp_alu_info);
                if (BCM_FAILURE(rv)) {
                    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                    "Flow check with timestamp flags for"
                                    " tracking param <%s> not present\n"),
                                plist[t_info[i].param]));
                    return rv;
                }

                /* Ignore alu for other stage except IFT */
                if (temp_alu_info->alu_ft_type != bcmiFtTypeIFT) {
                    continue;
                }
                rv = bcmi_ftv2_group_internal_flowcheck_create(unit, id,
                        t_info[i].param, temp_alu_info, &int_check_id);
                BCM_IF_ERROR_RETURN(rv);

                rv = bcmi_ftv2_group_internal_alu_info_create(unit, id,
                        t_info[i].param, int_check_id, temp_alu_info,
                        &int_alu_info_arr[idx]);
                BCM_IF_ERROR_RETURN(rv);

                idx++;
                break;

            default:
                break;
        }
    }

    group_alu_info = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    num_alu_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* For alu_info which needs to be divided */
    for (i = 0; i < num_alu_loads; i++) {
        temp_alu_info = &group_alu_info[i];

        /* Only loads */
        if (BCMI_FT_IDX_IS_CHECK(temp_alu_info->flowchecker_id)) {
            continue;
        }

        switch(temp_alu_info->element_type1) {
            case bcmFlowtrackerTrackingParamTypeNextHopA:
            case bcmFlowtrackerTrackingParamTypeNextHopB:
                /* BS2.0 next hop is 16b, hence need not be split */
                if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                    break;
                }
            case bcmFlowtrackerTrackingParamTypeDosAttack:
            case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
                /* Create 1 internal alu_info and update keys */
                sal_memcpy(&int_alu_info_arr[idx], temp_alu_info,
                        sizeof (bcmi_ft_group_alu_info_t));

                int_alu_info_arr[idx].flags |= BCMI_FT_ALU_LOAD_INTERNAL;
                int_alu_info_arr[idx].flags |= BCMI_FT_ALU_LOAD_EXTENDED;

                /* Update length for original alu_info */
                temp_alu_info->key1.length    = 16;

                /* Update location/length for internal alu_info */
                int_alu_info_arr[idx].key1.location =
                    temp_alu_info->key1.location + 16;
                int_alu_info_arr[idx].key1.length = 16;

                idx++;
                break;
            default:
                break;
        }

    }

    if (idx == 0) {
        return rv;
    }

    /* Final number of alu/load */
    num_alu_loads = idx + (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* Allocate memory for alu_info */
    BCMI_FT_ALLOC_IF_ERR_CLEANUP(new_alu_info,
            num_alu_loads * sizeof(bcmi_ft_group_alu_info_t),
            "group data alu_info");

    num_alu_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    sal_memcpy(&new_alu_info[0], BCMI_FT_GROUP_EXT_DATA_INFO(unit, id),
            sizeof(bcmi_ft_group_alu_info_t) * num_alu_loads);

    sal_memcpy(&new_alu_info[num_alu_loads], &int_alu_info_arr[0],
            sizeof(bcmi_ft_group_alu_info_t) * idx);

    sal_free(BCMI_FT_GROUP_EXT_DATA_INFO(unit, id));

    BCMI_FT_GROUP_EXT_DATA_INFO(unit, id) = new_alu_info;
    (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info = num_alu_loads + idx;

cleanup:

    return rv;
}

/*
 * Function:
 *   bcmi_ft_data_engine_db_show
 * Purpose:
 *   Show Data engine usage for the group
 * Parameters:
 *   unit - (IN) BCM device id
 *   engine_db - (IN) Data Engine DB
 *   enabled - (IN) If set, show only enabled
 * Returns:
 *   None.
 */
int
bcmi_ft_data_engine_db_show(int unit,
                bcmi_ft_data_engine_db_t *engine_db,
                                        int enabled)
{
    int idx;
    bcmi_ft_type_t ft_type;
    bcmi_ft_data_engine_info_t *engine_info;
    bcmi_ft_data_engine_per_ft_type_map_t *map;
    const char *ft_type_strings[] = BCMI_FT_TYPE_STRINGS;
    const char *engine_strings[] = BCM_FT_DATA_ENGINE_TYPE_STRINGS;

    for (ft_type = bcmiFtTypeIFT; ft_type <= bcmiFtTypeEFT; ft_type++) {

        map = &engine_db->ft_type_map[ft_type];

        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "FT Type = %s\n"), ft_type_strings[ft_type]));

        for (idx = 0; idx < map->count; idx++) {

            engine_info = map->list[idx];

            if ((enabled != -1) && (engine_info->enabled != enabled)) {
                continue;
            }

            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            " [%2d] -> %s(%d) Enabled=%x alu_info=%s\n"),
                        idx, engine_strings[engine_info->engine_type],
                        engine_info->index, engine_info->enabled,
                        ((engine_info->alu_info) ? "TRUE" : "FALSE")));

            LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            " \t\tOffset - PDE=0x%x PDD=0x%x)\n"),
                        engine_info->rev_pde_data_offset/8,
                        engine_info->pdd_offset));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_data_engine_db_info_add
 * Purpose:
 *   Add engine info per ft type to db
 * Parameters:
 *   unit - (IN) BCM device id
 *   ft_type - (IN) Flowtracker type
 *   engine_type - (IN) Data Engine type
 *   index - (IN) Index to data engine instance
 *   field_info - (IN) Field Info
 *   engine_db - (IN) Data Engine DB
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_data_engine_db_info_add(int unit,
                    bcmi_ft_type_t ft_type,
                    bcmi_ft_data_engine_type_t engine_type,
                    int index,
                    soc_field_info_t *field_info,
                    bcmi_ft_data_engine_db_t *engine_db)
{
    int rv = BCM_E_NONE;
    bcmi_ft_data_engine_per_ft_type_map_t *map = NULL;
    bcmi_ft_data_engine_info_t *engine_info = NULL;

    switch(engine_type) {
        case bcmiFtDataEngineTypeTimestamp:
            if (engine_db->ts_count == TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM) {
                return BCM_E_INTERNAL;
            }
            engine_db->ts[index].engine_type = engine_type;
            engine_db->ts[index].index = index;
            engine_db->ts_count++;
            engine_info = &engine_db->ts[index];
            break;
        case bcmiFtDataEngineTypeMeter:
            engine_db->meter.engine_type = engine_type;
            engine_db->meter.index = index;
            engine_info = &engine_db->meter;
            break;
        case bcmiFtDataEngineTypeAlu32:
            if (engine_db->alu32_count == TOTAL_GROUP_ALU32_MEM) {
                return BCM_E_INTERNAL;
            }
            engine_db->alu32[index].engine_type = engine_type;
            engine_db->alu32[index].index = index;
            engine_db->alu32_count++;
            engine_info = &engine_db->alu32[index];
            break;
        case bcmiFtDataEngineTypeLoad16:
            if (engine_db->load16_count == TOTAL_GROUP_LOAD16_DATA_NUM) {
                return BCM_E_INTERNAL;
            }
            engine_db->load16[index].engine_type = engine_type;
            engine_db->load16[index].index = index;
            engine_db->load16_count++;
            engine_info = &engine_db->load16[index];
            break;
        case bcmiFtDataEngineTypeAlu16:
            if (engine_db->alu16_count == TOTAL_GROUP_ALU16_MEM) {
                return BCM_E_INTERNAL;
            }
            engine_db->alu16[index].engine_type = engine_type;
            engine_db->alu16[index].index = index;
            engine_db->alu16_count++;
            engine_info = &engine_db->alu16[index];
            break;
        case bcmiFtDataEngineTypeCollectorCopy:
            engine_db->collector_copy.engine_type = engine_type;
            engine_db->collector_copy.index = index;
            engine_info = &engine_db->collector_copy;
            break;
        case bcmiFtDataEngineTypeLoad8:
            if (engine_db->load8_count == TOTAL_GROUP_LOAD8_DATA_NUM) {
                return BCM_E_INTERNAL;
            }
            engine_db->load8[index].engine_type = engine_type;
            engine_db->load8[index].index = index;
            engine_db->load8_count++;
            engine_info = &engine_db->load8[index];
            break;
        case bcmiFtDataEngineTypeFlowState:
            engine_db->flow_state.engine_type = engine_type;
            engine_db->flow_state.index = index;
            engine_info = &engine_db->flow_state;
            break;
        case bcmiFtDataEngineTypeCount:
        default:
            return BCM_E_INTERNAL;
    }

    engine_info->field = field_info->field;
    engine_info->size = field_info->len;

    engine_info->rev_pde_data_offset = (field_info->bp + engine_info->size - 1);

    map = &engine_db->ft_type_map[ft_type];
    map->list[map->count] = engine_info;
    map->count++;

    return rv;
}

/*
 * Function:
 *   bcmi_ft_data_engine_db_pdd_offset_set
 * Purpose:
 *   Set PDD offset of each valid data engine per FT Type
 * Parameters:
 *   unit - (IN) BCM device id
 *   engine_db - (IN) Data Engine DB
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_data_engine_db_pdd_offset_set(int unit,
                  bcmi_ft_data_engine_db_t *engine_db)
{
    int idx, offset = 0;
    bcmi_ft_type_t ft_type;
    bcmi_ft_data_engine_info_t *engine_info;
    bcmi_ft_data_engine_per_ft_type_map_t *map;

    for (ft_type = bcmiFtTypeIFT; ft_type <= bcmiFtTypeEFT; ft_type++) {

        map = &engine_db->ft_type_map[ft_type];

        for (idx = 0, offset = 0; idx < map->count; idx++) {

            engine_info = map->list[idx];
            if (engine_info->enabled == 0) {
                continue;
            }
            engine_info->pdd_offset = offset;
            offset += engine_info->size / 8;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_data_engine_db_create
 * Purpose:
 *   Create data engine db for the group. Also, enables
 *   relevent data engines.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker group
 *   load16_indexes - (IN) load16 element array.
 *   load8_indexes  - (IN) load8 element array.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_data_engine_db_create(int unit,
                bcm_flowtracker_group_t id,
                int *load16_indexes,
                int *load8_indexes)
{
    int rv = BCM_E_NONE;
    int idx, iter, total_mems;
    int ft_type_iter[bcmiFtTypeCount];
    uint32 *global_pdd_entry = NULL;
    bcmi_ft_type_t ft_type;
    soc_mem_t pdd_mem = INVALIDm;
    soc_format_t fmt = INVALIDfmt;
    soc_field_info_t *field_info = NULL;
    bcmi_ft_alu_load_type_t alu_load_type;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_alu_info_t *start = NULL;

    bcmi_ft_data_engine_db_t *engine_db = NULL;

    soc_field_t ts_fields[] = {
        BSD_FLEX_TIMESTAMP_0f, BSD_FLEX_TIMESTAMP_1f,
        BSD_FLEX_TIMESTAMP_2f, BSD_FLEX_TIMESTAMP_3f};

    /* Skip for Aggregate Flow groups */
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    BCMI_FT_ALLOC(BCMI_FT_GROUP(unit, id)->engine_db,
            sizeof (bcmi_ft_data_engine_db_t), "FT Data Engine");

    engine_db = BCMI_FT_GROUP(unit, id)->engine_db;
    if (engine_db == NULL) {
        return BCM_E_MEMORY;
    }

    fmt = BSD_POLICY_PDD_OUT_BUSfmt;

    start = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Retreive Global PDD Entry from Group Structure */
    global_pdd_entry = (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry));

    /* Retreive Relevant Memory */
    bcmi_ft_alu_load_pdd_mem_get(unit, bcmiFtTypeIFT, &pdd_mem);

    engine_db->total_bits = 1096;

    /* Timestamp */
    for (idx = 0; idx < TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM; idx++) {
        field_info = soc_format_fieldinfo_get(unit, fmt, ts_fields[idx]);
        if (field_info != NULL) {
            ft_type = BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, idx);
            rv = bcmi_ft_data_engine_db_info_add(unit, ft_type,
                    bcmiFtDataEngineTypeTimestamp, idx, field_info,
                    engine_db);

            if (soc_mem_field32_get(unit, pdd_mem,
                        global_pdd_entry, ts_fields[idx])) {
                engine_db->ts[idx].enabled = 1;
            }
        }
    }

    /* Meter */
    field_info = soc_format_fieldinfo_get(unit, fmt, BSD_FLEX_FLOW_METERf);
    if (field_info != NULL) {
        ft_type = bcmiFtTypeIFT;
        rv = bcmi_ft_data_engine_db_info_add(unit, ft_type,
                bcmiFtDataEngineTypeMeter, 0, field_info, engine_db);
        if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                    BSD_FLEX_FLOW_METERf)) {
            engine_db->meter.enabled = 1;
        }
    }

    /* ALU32 */
    alu_load_type = bcmiFtAluLoadTypeAlu32;
    pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type);
    total_mems = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, alu_load_type);
    for (idx = 0; idx < total_mems; idx++) {

        field_info  = soc_format_fieldinfo_get(unit, fmt, pdd_info[idx].param_id);
        if (field_info != NULL) {
            ft_type = BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                    alu_load_type, idx, 0);
            rv = bcmi_ft_data_engine_db_info_add(unit, ft_type,
                    bcmiFtDataEngineTypeAlu32, idx, field_info, engine_db);

            if (bcmi_ft_group_alu_load_pdd_status_get(unit, id,
                        global_pdd_entry, alu_load_type, idx)) {

                /* Check for this index and get corresponding data. */
                rv = bcmi_ft_group_alu_load_index_match_data_get(unit,
                        id, alu_load_type, idx, &temp);
                if (BCM_FAILURE(rv)) {
                    continue;
                }
                engine_db->alu32[idx].enabled = 1;
                engine_db->alu32[idx].alu_info = temp;
            }
        }
    }

    /* LOAD16 */
    alu_load_type = bcmiFtAluLoadTypeLoad16;
    pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type);
    ft_type_iter[0] = ft_type_iter[1] = -1;
    ft_type_iter[2] = ft_type_iter[3] = -1;
    for (idx = 0, iter = 0; idx < TOTAL_GROUP_LOAD16_DATA_NUM; idx++) {

        field_info  = soc_format_fieldinfo_get(unit, fmt, pdd_info[idx].param_id);
        if (field_info != NULL) {
            ft_type = BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                    alu_load_type, 0, idx);
            rv = bcmi_ft_data_engine_db_info_add(unit, ft_type,
                    bcmiFtDataEngineTypeLoad16, idx, field_info, engine_db);

            if (bcmi_ft_group_alu_load_pdd_status_get(unit, id,
                        global_pdd_entry, alu_load_type, idx)) {
                iter = ft_type_iter[ft_type] + 1;
                while (load16_indexes[iter] != -1) {
                    temp = (&(start[load16_indexes[iter]]));
                    if (temp->alu_ft_type == ft_type) {
                        break;
                    }
                    iter++;
                }
                engine_db->load16[idx].enabled = 1;
                engine_db->load16[idx].alu_info = temp;
                ft_type_iter[ft_type] = iter;
            }
        }
    }

    /* ALU16 */
    alu_load_type = bcmiFtAluLoadTypeAlu16;
    pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type);
    total_mems = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, alu_load_type);
    for (idx = 0; idx < total_mems; idx++) {

        field_info  = soc_format_fieldinfo_get(unit, fmt, pdd_info[idx].param_id);
        if (field_info != NULL) {
            ft_type = BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                    alu_load_type, idx, 0);
            rv = bcmi_ft_data_engine_db_info_add(unit, ft_type,
                    bcmiFtDataEngineTypeAlu16, idx, field_info, engine_db);

            if (bcmi_ft_group_alu_load_pdd_status_get(unit, id,
                        global_pdd_entry, alu_load_type, idx)) {

                /* Check for this index and get corresponding data. */
                rv = bcmi_ft_group_alu_load_index_match_data_get(unit, id,
                        alu_load_type, idx, &temp);
                if (BCM_FAILURE(rv)) {
                    continue;
                }
                engine_db->alu16[idx].enabled = 1;
                engine_db->alu16[idx].alu_info = temp;
            }
        }
    }

    /* Collector Copy */
    field_info = soc_format_fieldinfo_get(unit, fmt,
            BSD_FLEX_FLOW_COUNT_TO_CPUf);
    if (field_info != NULL) {
        ft_type = bcmiFtTypeIFT;
        rv = bcmi_ft_data_engine_db_info_add(unit, ft_type,
                bcmiFtDataEngineTypeCollectorCopy, 0, field_info, engine_db);

        if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                    BSD_FLEX_FLOW_COUNT_TO_CPUf)) {
            engine_db->collector_copy.enabled = 1;
        }
    }

    /* LOAD8 */
    alu_load_type = bcmiFtAluLoadTypeLoad8;
    pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, alu_load_type);
    ft_type_iter[0] = ft_type_iter[1] = -1;
    ft_type_iter[2] = ft_type_iter[3] = -1;
    for (idx = 0, iter = 0; idx < TOTAL_GROUP_LOAD8_DATA_NUM; idx++) {

        field_info  = soc_format_fieldinfo_get(unit, fmt, pdd_info[idx].param_id);
        if (field_info != NULL) {
            ft_type = BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit,
                    alu_load_type, 0, idx);
            rv = bcmi_ft_data_engine_db_info_add(unit, ft_type,
                    bcmiFtDataEngineTypeLoad8, idx, field_info, engine_db);

            if (bcmi_ft_group_alu_load_pdd_status_get(unit, id,
                        global_pdd_entry, alu_load_type, idx)) {
                iter = ft_type_iter[ft_type] + 1;
                while (load8_indexes[iter] != -1) {
                    temp = (&(start[load8_indexes[iter]]));
                    if (temp->alu_ft_type == ft_type) {
                        break;
                    }
                    iter++;
                }
                engine_db->load8[idx].enabled = 1;
                engine_db->load8[idx].alu_info = temp;
                ft_type_iter[ft_type] = iter;
            }
        }
    }

    /* Flow State */
    field_info = soc_format_fieldinfo_get(unit, fmt, BSD_FLEX_FLOW_STATEf);
    if (field_info != NULL) {
        ft_type = bcmiFtTypeIFT;
        rv = bcmi_ft_data_engine_db_info_add(unit, ft_type,
                bcmiFtDataEngineTypeFlowState, 0, field_info, engine_db);

        if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                    BSD_FLEX_FLOW_STATEf)) {
            engine_db->flow_state.enabled = 1;
        }
    }

    /* Update PDD offset */
    bcmi_ft_data_engine_db_pdd_offset_set(unit, engine_db);

    /* Show Data Engine Status */
    bcmi_ft_data_engine_db_show(unit, engine_db, 1);

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_data_engine_search
 * Purpose:
 *   Search for data engine with given tracking param.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker group
 *   map  - (IN) engine info map for ft type
 *   engine_type - (IN) Data engine type
 *   param - (IN) Tracking param to search
 *   skip_index - (IN) Index not to use
 *   target_index - (OUT) Index at which param found
 * Notes:
 *   Note that scope of index is within map per ft type.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_data_engine_search(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_data_engine_per_ft_type_map_t *map,
            bcmi_ft_data_engine_type_t engine_type,
            bcm_flowtracker_tracking_param_type_t param,
            int skip_index,
            int *target_index)
{
    int idx = 0;
    bcmi_ft_group_param_type_t ts_type1;
    bcmi_ft_group_param_type_t ts_type2;

    *target_index = -1;
    for (idx = 0; idx < map->count; idx++) {
        if (idx == skip_index) {
            continue;
        }

        if (map->list[idx]->engine_type != engine_type) {
            continue;
        }

        if (bcmi_ft_tracking_param_is_timestamp(unit, param)) {
            ts_type1 = bcmi_ft_group_get_param_type(unit, param);
            ts_type2 = bcmi_ft_group_get_ts_type(unit,
                    id, map->list[idx]->index);
            if (ts_type1 == ts_type2) {
                *target_index = idx;
                return BCM_E_NONE;
            }
        } else {
            if (map->list[idx]->alu_info != NULL) {
                if (map->list[idx]->alu_info->element_type1 == param) {
                    *target_index = idx;
                    return BCM_E_NONE;
                }
            }
        }
    }

    return BCM_E_INTERNAL;
}

/*
 * Function:
 *   bcmi_ftv2_data_engine_info_move
 * Purpose:
 *   Create data engine db for the group. Also, enables
 *   relevent data engines.
 * Parameters:
 *   unit - (IN) BCM device id
 *   map  - (IN) engine info map for ft type
 *   first - (IN) Move First index
 *   second - (IN) Move second index
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_data_engine_info_move(int unit,
                bcmi_ft_data_engine_per_ft_type_map_t *map,
                int first, int second)
{
    int idx, start, end;
    bcmi_ft_data_engine_info_t *engine_info_copy;

    if (first < second) {
        start = second;
        end = first + 1;
    } else {
        start = first;
        end = second;
    }
    engine_info_copy = map->list[start];

    /* Move */
    for (idx = start; idx > end; idx--) {
        map->list[idx] = map->list[idx - 1];
    }

    map->list[end] = engine_info_copy;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_group_data_engine_rearrange
 * Purpose:
 *   Rearrange data engine based on output sequence
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker Group.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_data_engine_rearrange(int unit,
                                      bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    int idx, adj_idx, check_idx;
    bcmi_ft_type_t ft_type;
    bcmi_ft_data_engine_db_t *engine_db;
    bcmi_ft_data_engine_info_t *engine_info;
    bcmi_ft_group_alu_info_t *alu_info;
    bcm_flowtracker_tracking_param_type_t param;
    bcmi_ft_data_engine_per_ft_type_map_t *map;

    engine_db = BCMI_FT_GROUP(unit, id)->engine_db;

    for (ft_type = bcmiFtTypeIFT; ft_type <= bcmiFtTypeEFT; ft_type++) {
        map = &engine_db->ft_type_map[ft_type];

        for (idx = 0; idx < map->count; idx++) {

            engine_info = map->list[idx];

            if ((!engine_info->enabled) ||
                    (engine_info->alu_info == NULL)) {
                continue;
            }

            alu_info = engine_info->alu_info;
            if (!(alu_info->flags & BCMI_FT_ALU_LOAD_EXTENDED)) {
                continue;
            }
            switch(alu_info->element_type1) {
                case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
                case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
                case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
                case bcmFlowtrackerTrackingParamTypeTimestampIngress:
                    rv = bcmi_ftv2_data_engine_search(unit, id,
                            map, bcmiFtDataEngineTypeTimestamp,
                            alu_info->element_type1, idx, &adj_idx);
                    BCM_IF_ERROR_RETURN(rv);

                    rv = bcmi_ftv2_data_engine_info_move(unit, map, idx, adj_idx);
                    BCM_IF_ERROR_RETURN(rv);

                    engine_info->flags |= BCMI_FT_DATA_ENGINE_NEXT_ATTACH;
                    break;
                case bcmFlowtrackerTrackingParamTypeNextHopA:
                case bcmFlowtrackerTrackingParamTypeNextHopB:
                case bcmFlowtrackerTrackingParamTypeDosAttack:
                case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
                    rv = bcmi_ftv2_data_engine_search(unit, id,
                            map,  engine_info->engine_type,
                            alu_info->element_type1, idx, &adj_idx);
                    BCM_IF_ERROR_RETURN(rv);

                    rv = bcmi_ftv2_data_engine_info_move(unit, map, idx, adj_idx);
                    BCM_IF_ERROR_RETURN(rv);

                    engine_info->flags |= BCMI_FT_DATA_ENGINE_NEXT_ATTACH;
                    break;
                default:
                    /* Check Events */
                    check_idx = BCMI_FT_INT_CHECK_IDX_GET(alu_info->flowchecker_id);
                    if (check_idx == BCMI_FT_INT_CHECK_CHECKEVENT1_IDX) {
                        param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
                    } else if (check_idx == BCMI_FT_INT_CHECK_CHECKEVENT2_IDX) {
                        param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2;
                    } else if (check_idx == BCMI_FT_INT_CHECK_CHECKEVENT3_IDX) {
                        param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3;
                    } else if (check_idx == BCMI_FT_INT_CHECK_CHECKEVENT4_IDX) {
                        param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4;
                    } else {
                        break;
                    }
                    rv = bcmi_ftv2_data_engine_search(unit, id, map,
                            bcmiFtDataEngineTypeTimestamp, param, idx, &adj_idx);
                    BCM_IF_ERROR_RETURN(rv);

                    rv = bcmi_ftv2_data_engine_info_move(unit, map, idx, adj_idx);
                    BCM_IF_ERROR_RETURN(rv);

                    engine_info->flags |= BCMI_FT_DATA_ENGINE_NEXT_ATTACH;
                    break;
            }
        }
    }

    bcmi_ft_data_engine_db_show(unit, engine_db, 1);

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_group_pdd_to_pde_conversion
 * Purpose:
 *   PDD to PDE conversion per FT type
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   ft_type    - (IN) Flowtracker stage
 *   flags      - (IN) PDE type
 *   pdd_entry  - (IN) incoming pdd entry
 *   pde_entry  - (OUT) Geneted pde entry
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_pdd_to_pde_conversion(int unit,
                    bcm_flowtracker_group_t id,
                    bcmi_ft_type_t ft_type,
                    uint32 flags,
                    uint32 *pdd_entry,
                    uint32 *pde_entry)
{
    int rv = BCM_E_NONE;
    int idx, iter, running_fill_size;
    int running_fill_num = 0;
    int running_pde_idx, offset_jump, max_fill_size, max_fill_num;
    bcmi_ft_data_engine_db_t *engine_db;
    bcmi_ft_data_engine_info_t *engine_info;
    bcmi_ft_data_engine_per_ft_type_map_t *map;
    const char *engine_strings[] = BCM_FT_DATA_ENGINE_TYPE_STRINGS;

    engine_db = BCMI_FT_GROUP(unit, id)->engine_db;

    map = &engine_db->ft_type_map[ft_type];

    /* Retreive resource sw state from global structure */
    if (flags & BCMI_FTv2_STAGE_PDE) {
        running_pde_idx =
            BCMI_FT_RESOURCE_SW_INFO_BANK_START_LOCAL_PDE_OFFSET(unit, ft_type);
        offset_jump =
            BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_LOCAL_JUMP(unit, ft_type);
    } else if (flags & BCMI_FTv2_EXPORT_PDE) {
        running_pde_idx =
            BCMI_FT_RESOURCE_SW_INFO_BANK_START_EXPORT_PDE_OFFSET(unit, ft_type);
        offset_jump =
            BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_EXPORT_JUMP(unit, ft_type);
    } else {
        return BCM_E_INTERNAL;
    }
    max_fill_size = running_fill_size =
        BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, ft_type);
    max_fill_num =
        BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, ft_type);

    for (idx = 0; idx < map->count; idx++) {

        engine_info = map->list[idx];

        if (engine_info->enabled == FALSE) {
            continue;
        }

        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        " [%2d] -> %s(%d) Offset=%x(B=%x) Use=%d byte(s)\n"),
                    running_pde_idx, engine_strings[engine_info->engine_type],
                    engine_info->index, engine_info->rev_pde_data_offset,
                    engine_info->rev_pde_data_offset/8, engine_info->size/8));

        _FTv2_GROUP_FILL_AND_MOVE_PDE_WITH_RESOURCE_CHECK(unit,
                iter, engine_info->size/8, running_pde_idx,
                pde_entry, engine_info->rev_pde_data_offset,
                running_fill_size, running_fill_num, max_fill_size,
                max_fill_num, offset_jump);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_group_stage_pde_policy_add
 * Purpose:
 *   Add PDE Data profile oe group (for ver 3)
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   index      - (IN) Index in pde policy table.
 *   pdd_entry_arr - (IN) PDD entry per stage
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_stage_pde_policy_add(int unit,
                        bcm_flowtracker_group_t id,
                        int index,
                        uint32 **pdd_entry_arr)
{
    int rv;
    bsc_dt_pde_profile_0_entry_t pde_entry;

    sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
    rv = bcmi_ftv2_group_pdd_to_pde_conversion(unit, id,
                                               bcmiFtTypeIFT,
                                               BCMI_FTv2_STAGE_PDE,
                                               pdd_entry_arr[0],
                                               (uint32 *)(&pde_entry));
    if (rv == BCM_E_RESOURCE) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "IFT session data memory is not enough to fill.")));
    }
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_write(unit, BSC_DT_PDE_PROFILE_0m,
            MEM_BLOCK_ANY, index, &pde_entry);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
    rv = bcmi_ftv2_group_pdd_to_pde_conversion(unit, id,
                                               bcmiFtTypeMFT,
                                               BCMI_FTv2_STAGE_PDE,
                                               pdd_entry_arr[1],
                                               (uint32 *)(&pde_entry));
    if (rv == BCM_E_RESOURCE) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "MFT session data memory is not enough to fill.")));
    }
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_write(unit, BSC_DT_PDE_PROFILE_1m,
            MEM_BLOCK_ANY, index, &pde_entry);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
    rv = bcmi_ftv2_group_pdd_to_pde_conversion(unit, id,
                                               bcmiFtTypeEFT,
                                               BCMI_FTv2_STAGE_PDE,
                                               pdd_entry_arr[2],
                                               (uint32 *)(&pde_entry));
    if (rv == BCM_E_RESOURCE) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "EFT session data memory is not enough to fill.")));
    }
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_write(unit, BSC_DT_PDE_PROFILE_2m,
            MEM_BLOCK_ANY, index, &pde_entry);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_group_export_pde_policy_add
 * Purpose:
 *   Add PDE Export profile oe group (for ver 3)
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 *   index      - (IN) Index in pde policy table.
 *   pdd_entry_arr - (IN) PDD entry per stage
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_export_pde_policy_add(int unit,
                    bcm_flowtracker_group_t id,
                    int index,
                    uint32 **pdd_entry_arr)
{
    int rv;
    bsc_dt_pde_profile_0_entry_t pde_entry;

    sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
    rv = bcmi_ftv2_group_pdd_to_pde_conversion(unit, id,
                                               bcmiFtTypeIFT,
                                               BCMI_FTv2_EXPORT_PDE,
                                               pdd_entry_arr[0],
                                               (uint32 *)(&pde_entry));
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_0m,
            MEM_BLOCK_ANY, index, &pde_entry);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
    rv = bcmi_ftv2_group_pdd_to_pde_conversion(unit, id,
                                               bcmiFtTypeMFT,
                                               BCMI_FTv2_EXPORT_PDE,
                                               pdd_entry_arr[1],
                                               (uint32 *)(&pde_entry));
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_1m,
            MEM_BLOCK_ANY, index, &pde_entry);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
    rv = bcmi_ftv2_group_pdd_to_pde_conversion(unit, id,
                                               bcmiFtTypeEFT,
                                               BCMI_FTv2_EXPORT_PDE,
                                               pdd_entry_arr[2],
                                               (uint32 *)(&pde_entry));
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_write(unit, BSC_DT_EXPORT_PDE_PROFILE_2m,
            MEM_BLOCK_ANY, index, &pde_entry);

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_group_pdd_policy_profile_add
 * Purpose:
 *   Add policy profile entry for group action profile.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_group_pdd_policy_profile_add(int unit,
                                       bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    uint32 index = 0;
    uint32 *pdd_entry_tmp[3];
    uint32 *global_pdd_entry = NULL;
    int old_index = 0, ref_count = 0;
    bsc_dt_pde_profile_0_entry_t pde_entry;
    bsc_kg_group_table_entry_t kg_group_entry;
    bsd_policy_action_profile_0_entry_t pdd_entry[3];

    /* Default entry for PDD & PDE memory */
    sal_memset(&pdd_entry, 0, sizeof(pdd_entry));
    sal_memset(&pde_entry, 0, sizeof(pde_entry));

    /* Skip for Aggregate Flow groups */
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    /* Retreive Global PDD Entry from Group Structure */
    global_pdd_entry = (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry));

    /* Point temp entry pointer to individual stage pdd entries */
    pdd_entry_tmp[0] = (uint32 *)&pdd_entry[0];
    pdd_entry_tmp[1] = (uint32 *)&pdd_entry[1];
    pdd_entry_tmp[2] = (uint32 *)&pdd_entry[2];

    /* Retreive stage specific pdd entry from global pdd */
    rv = bcmi_ftv2_group_pdd_to_stage_pdd_coversion(unit,
            global_pdd_entry, (uint32 **)(&pdd_entry_tmp));
    BCM_IF_ERROR_RETURN(rv);

    /* First read the group entry. */
    rv = soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_group_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Now write this index into the group. */
    old_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
            ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);
    if (old_index) {
        bcmi_ft_group_pdd_profile_entry_delete(unit, old_index);
    }

    /* Create the profile entry for new pdd entries profile. */
    rv = bcmi_ft_group_pdd_profile_entry_add(unit,
            (void **)pdd_entry_tmp, 1, (uint32 *)&index);
    BCM_IF_ERROR_RETURN(rv);

    /* now write this new index into the group. */
    soc_mem_field32_set(unit, BSC_KG_GROUP_TABLEm,
            ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf, index);

    /* Write the entry back into the group table. */
    rv = soc_mem_write(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_group_entry);
    BCMI_CLEANUP_IF_ERROR(rv);

    rv = bcmi_ft_group_pdd_profile_refcount_get(unit, index, &ref_count);
    BCMI_CLEANUP_IF_ERROR(rv);

    if (ref_count == 1) {

        /* Add Data PDE policy */
        rv = bcmi_ftv2_group_stage_pde_policy_add(unit,
                id, index, (uint32 **) pdd_entry_tmp);
        BCMI_CLEANUP_IF_ERROR(rv);
    }

    /* Rearrange data engines based on extended alu_info */
    rv = bcmi_ftv2_group_data_engine_rearrange(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

    if (ref_count == 1) {

        /* Add export PDE policy */
        rv = bcmi_ftv2_group_export_pde_policy_add(unit,
                id, index, (uint32 **) pdd_entry_tmp);
    }

    return rv;

cleanup:

    sal_memset(&pde_entry, 0, sizeof(bsc_dt_pde_profile_0_entry_t));
    bcmi_ft_group_pdd_profile_entry_delete(unit, index);
    soc_mem_write(unit,
            BSC_DT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
            BSC_DT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
            BSC_DT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
            BSC_DT_EXPORT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
            BSC_DT_EXPORT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);
    soc_mem_write(unit,
            BSC_DT_EXPORT_PDE_PROFILE_0m, MEM_BLOCK_ANY, index, &pde_entry);

    return rv;
}
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
/*
 * Function:
 *   bcmi_ftv3_group_aggregate_template_data_create
 * Purpose:
 *   Create template data for aggregate flowtracker group.
 * Parameters:
 *   unit       - (IN) BCM device id
 *   id         - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv3_group_aggregate_template_data_create(
    int unit,
    bcm_flowtracker_group_t id)
{
    int i = 0;
    int rv = BCM_E_NONE;
    int running_length = 0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_template_list_t **head = NULL;
    bcmi_ft_alu_load_type_t alu_load_type;

    alu_load_type = bcmiFtAluLoadTypeAggAlu32;

    /* Get the head of the templs info list. */
    head = &((BCMI_FT_GROUP(unit, id))->template_head);

    for (i = TOTAL_GROUP_AGG_ALU32_MEM - 1; i >= 0; i--) {

        rv = bcmi_ft_group_alu_load_index_match_data_get(unit,
                id, alu_load_type, i, &temp);

        if (BCM_FAILURE(rv)) {
            running_length += 32;
            continue;
        }
        rv = bcmi_ft_group_template_list_add(unit,
                head, running_length, 32,
                bcmiFtGroupParamTypeTracking, temp);
        BCMI_CLEANUP_IF_ERROR(rv);
        running_length += 32;
    }

    return BCM_E_NONE;

cleanup:
    bcmi_ft_group_template_list_delete_all(unit, head);
    return rv;
}

/*
 * Function:
 *   bcmi_ftv3_group_flow_transition_state_set
 * Purpose:
 *   Set/Clear Flow State transition bit in PDD
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker Group.
 *   enable - (IN) Enable/Disable
 * Returns:
 *   None.
 */
int
bcmi_ftv3_group_flow_transition_state_set(int unit,
                    bcm_flowtracker_group_t id,
                    uint32 enable)
{
    uint32 curr_state;
    soc_mem_t pdd_mem = INVALIDm;

    bcmi_ft_alu_load_pdd_mem_get(unit, bcmiFtTypeIFT, &pdd_mem);

    curr_state = soc_mem_field32_get(unit, pdd_mem,
            (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
            BSD_FLEX_FLOW_STATEf);

    if (curr_state != enable) {
        soc_mem_field32_set(unit, pdd_mem,
                (&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                BSD_FLEX_FLOW_STATEf, enable);
    }

    return BCM_E_NONE;
}

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
/*
 * Function:
 *   bcmi_ftv2_group_template_data_create
 * Purpose:
 *   Create Template data for the Group (version 3).
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker Group.
 *   load16_indexes - (IN) load16 element array.
 *   load8_indexes  - (IN) load8 element array.
 * Returns:
 *   None.
 */
int
bcmi_ftv2_group_template_data_create(int unit,
                    bcm_flowtracker_group_t id,
                    int *load16_indexes,
                    int *load8_indexes)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int running_length = 0;
    bcmi_ft_type_t ft_type = 0;
    bcmi_ft_group_alu_info_t *alu_info;
    bcmi_ft_data_engine_db_t *engine_db;
    bcmi_ft_data_engine_info_t *engine_info;
    bcmi_ft_data_engine_info_t *engine_info2;
    bcmi_ft_data_engine_per_ft_type_map_t *map;
    bcmi_ft_group_template_list_t **head;
    bcmi_ft_group_param_type_t type = bcmiFtGroupParamTypeTsNewLearn;

    /* Aggregate Does not support PDD Profile */
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return bcmi_ftv3_group_aggregate_template_data_create(unit, id);
    }
#endif

    engine_db = BCMI_FT_GROUP(unit, id)->engine_db;
    if (engine_db == NULL) {
        return BCM_E_INTERNAL;
    }

    /* Get the head of the templs info list. */
    head = &((BCMI_FT_GROUP(unit, id))->template_head);

    for (ft_type = bcmiFtTypeIFT; ft_type <= bcmiFtTypeEFT; ft_type++) {
        map = &engine_db->ft_type_map[ft_type];

        if (ft_type == bcmiFtTypeIFT) {
            running_length = 0;
        }  else if (ft_type == bcmiFtTypeMFT) {
            running_length =
                ((BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, bcmiFtTypeIFT) *
                  (BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, bcmiFtTypeIFT) + 1)) * 8);
        } else {
            running_length =
                (((BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, bcmiFtTypeIFT) *
                   (BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, bcmiFtTypeIFT) + 1)) * 8) +
                 ((BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, bcmiFtTypeMFT) *
                   (BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, bcmiFtTypeMFT) + 1)) * 8));
        }

        for (idx = 0; idx < map->count; idx++) {
            engine_info = map->list[idx];
            if (engine_info->enabled == FALSE) {
                continue;
            }

            switch(engine_info->engine_type) {
                case bcmiFtDataEngineTypeTimestamp:
                    type = bcmi_ft_group_get_ts_type(unit,
                            id, engine_info->index);
                    break;
                case bcmiFtDataEngineTypeMeter:
                    type = bcmiFtGroupParamTypeMeter;
                    break;
                case bcmiFtDataEngineTypeAlu32:
                case bcmiFtDataEngineTypeLoad16:
                case bcmiFtDataEngineTypeAlu16:
                case bcmiFtDataEngineTypeLoad8:
                    type = bcmiFtGroupParamTypeTracking;
                    break;
                case bcmiFtDataEngineTypeCollectorCopy:
                    type = bcmiFtGroupParamTypeCollectorCopy;
                    break;
                /* Flow State is not exported */
                case bcmiFtDataEngineTypeFlowState:
                default:
                    continue;
            }

            alu_info = NULL;
            if (engine_info->alu_info) {
                alu_info = engine_info->alu_info;

                if (engine_info->flags & BCMI_FT_DATA_ENGINE_NEXT_ATTACH) {
                    engine_info2 = map->list[idx + 1];
                    /* If attached with Timestamp Engine, update 'type' */
                    if (engine_info2->engine_type ==
                            bcmiFtDataEngineTypeTimestamp) {
                        type = bcmi_ft_group_get_ts_type(unit, id,
                                                    engine_info2->index);
                    }

                    alu_info->flags |= BCMI_FT_ALU_LOAD_NEXT_ATTACH;
                }
            }
            rv = bcmi_ft_group_template_list_add(unit, head,
                    running_length, engine_info->size, type, alu_info);
            BCMI_CLEANUP_IF_ERROR(rv);

            running_length += engine_info->size;
        }
    }

    bcmi_ft_group_data_template_print(unit, *head);

    return rv;

cleanup:

    bcmi_ft_group_template_list_delete_all(unit, head);

    return rv;
}
#endif

/*
 * Function:
 *   bcmi_ft_group_param_ts_trig_info_clear
 * Purpose:
 *   Clear timestamp triggers and corresponding ts_info
 *   in the group that are set due to tracking params configurations.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   id             - (IN) FLOW group id
 *   ts_triggers    - (OUT) Ts Triggers of the group
 * Returns:
 *   BCM_E_XXX.
 * Notes:
 *   Clears ts_triggers which are set from
 *   1. Tracking params NewLearn/FlowStart/FlowEnd
 *   2. Internal allocation in IPAT/IPDT
 *   3. TimestampCheckEvents which is not mapped to flowcheck.
 *   4. Triggers reserved for tracking params.
 */
void
bcmi_ft_group_param_ts_trig_info_clear(int unit,
                                       bcm_flowtracker_group_t id,
                                       uint32 *ts_triggers)
{
    int trig = 0;
    uint32 temp_ts_triggers = 0;
    uint32 param_ts_triggers = 0;

    param_ts_triggers = BCMI_FT_GROUP_PARAM_TS_TRIGGERS(unit, id);
    temp_ts_triggers = param_ts_triggers;

    /* Clear trigger info assigned */
    for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {

        /* TS triggers allocated internaly */
        if ((BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) &
                                        BCMI_FT_TS_INTERNAL_IPAT) ||
            (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) &
                                        BCMI_FT_TS_INTERNAL_IPDT)) {
            temp_ts_triggers |= (1 << trig);
        }

        /* TS triggers with ref_count=0 for CheckEvents */
        if (BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, trig) == 0) {
            temp_ts_triggers |= (1 << trig);
        }

        if (temp_ts_triggers & (1 << trig)) {
            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) =
                bcmiFtTypeNone;
            BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, trig) = 0;
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, trig) = 0;
            BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) = 0;
            BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, trig) = 0;
        }
    }

    (*ts_triggers) &= (~temp_ts_triggers);
}

/*
 * Function:
 *   bcmi_ft_group_param_int_ts_trig_info_clear
 * Purpose:
 *   Clear timestamp triggers and corresponding ts_info
 *   in the group that are set due to allocation while doing
 *   validation.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   id             - (IN) FLOW group id
 * Returns:
 *   BCM_E_XXX.
 * Notes:
 *   Clears ts_triggers which are set from
 *   1. Internal allocation in IPAT/IPDT
 *   2. TimestampCheckEvents which is not mapped to flowcheck.
 */
void
bcmi_ft_group_param_int_ts_trig_info_clear(int unit,
                                           bcm_flowtracker_group_t id)
{
    int trig = 0;
    uint32 ts_flags = 0;
    uint32 temp_ts_triggers = 0;
    uint32 param_ts_triggers = 0;

    ts_flags = (BCMI_FT_TS_INTERNAL_IPAT |
            BCMI_FT_TS_INTERNAL_IPDT |
            BCMI_FT_TS_RESERVED_NEW_LEARN |
            BCMI_FT_TS_RESERVED_FLOW_START |
            BCMI_FT_TS_RESERVED_FLOW_END |
            BCMI_FT_TS_RESERVED_INGRESS |
            BCMI_FT_TS_RESERVED_EGRESS);

    param_ts_triggers = BCMI_FT_GROUP_PARAM_TS_TRIGGERS(unit, id);

    /* Clear trigger info assigned */
    for (trig = 0; trig < BCMI_FT_GROUP_MAX_TS_TYPE; trig++) {

        if ((trig == BCMI_FT_GROUP_TS_NEW_LEARN) ||
            (trig == BCMI_FT_GROUP_TS_FLOW_START) ||
            (trig == BCMI_FT_GROUP_TS_FLOW_END) ||
            (trig == BCMI_FT_GROUP_TS_INGRESS) ||
            (trig == BCMI_FT_GROUP_TS_EGRESS)) {
            continue;
        }

        /* TS triggers allocated internally, but not reserved */
        if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) & ts_flags) {
            if (!(param_ts_triggers & (1 << trig))) {
                temp_ts_triggers |= (1 << trig);
            }
        }

        /* TS triggers with ref_count=0 for CheckEvents */
        if (BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, trig) == 0) {
            temp_ts_triggers |= (1 << trig);
        }

        if (temp_ts_triggers & (1 << trig)) {
            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) =
                bcmiFtTypeNone;
            BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, trig) = 0;
            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, trig) = 0;
            BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id , trig) = 0;
            BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, trig) = 0;
        }
    }

    BCMI_FT_GROUP_TS_TRIGGERS(unit, id) &= (~temp_ts_triggers);
}

/*
 * Function:
 *   bcmi_ft_group_ts_type_src_get
 * Purpose:
 *   Get timestamp sources corresponding to given timestamp
 *   trigger type for the group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   id             - (IN) FLOW group id
 *   ts_type        - (IN) Timestamp trigger type
 *   ts_src         - (OUT) Timestamp source
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_ts_type_src_get(int unit,
                              bcm_flowtracker_group_t id,
                              int ts_type,
                              bcm_flowtracker_timestamp_source_t *ts_src)
{
    switch(ts_type) {
        case BCMI_FT_GROUP_TS_NEW_LEARN:
            *ts_src = bcmFlowtrackerTimestampSourceNewLearn;
            break;
        case BCMI_FT_GROUP_TS_FLOW_START:
            *ts_src = bcmFlowtrackerTimestampSourceFlowStart;
            break;
        case BCMI_FT_GROUP_TS_FLOW_END:
            *ts_src = bcmFlowtrackerTimestampSourceFlowEnd;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT1:
            *ts_src = bcmFlowtrackerTimestampSourceCheckEvent1;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT2:
            *ts_src = bcmFlowtrackerTimestampSourceCheckEvent2;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT3:
            *ts_src = bcmFlowtrackerTimestampSourceCheckEvent3;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT4:
            *ts_src = bcmFlowtrackerTimestampSourceCheckEvent4;
            break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_timestamp_trig_param_get
 * Purpose:
 *   Get tracking param corresponding to given timestamp
 *   trigger type for the group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   id             - (IN) FLOW group id
 *   ts_type        - (IN) Timestamp trigger type
 *   param          - (OUT) Tracking param
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_timestamp_trig_param_get(int unit,
                                       bcm_flowtracker_group_t id,
                                       int ts_type,
                                       bcm_flowtracker_tracking_param_type_t *param)
{
    switch(ts_type) {
        case BCMI_FT_GROUP_TS_NEW_LEARN:
            *param = bcmFlowtrackerTrackingParamTypeTimestampNewLearn;
            break;
        case BCMI_FT_GROUP_TS_FLOW_START:
            *param = bcmFlowtrackerTrackingParamTypeTimestampFlowStart;
            break;
        case BCMI_FT_GROUP_TS_FLOW_END:
            *param = bcmFlowtrackerTrackingParamTypeTimestampFlowEnd;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT1:
            *param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT2:
            *param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT3:
            *param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3;
            break;
        case BCMI_FT_GROUP_TS_CHECK_EVENT4:
            *param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4;
            break;
        case BCMI_FT_GROUP_TS_INGRESS:
            *param = bcmFlowtrackerTrackingParamTypeTimestampIngress;
            break;
        case BCMI_FT_GROUP_TS_EGRESS:
            *param = bcmFlowtrackerTrackingParamTypeTimestampEgress;
            break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_hw_entry_buffer_allocate
 * Purpose:
 *   Allocate buffer to read hardware tables during traversing records
 *   in the flowtracker group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   trav_data      - (IN) Traversal Data
 * Returns:
 *   BCM_E_XXX.
 */
STATIC void
bcmi_ft_group_hw_entry_buffer_allocate(
        int unit,
        bcmi_ft_group_record_trav_data_t *trav_data)
{
    int size;

    size = 64;
    BCMI_FT_ALLOC(trav_data->u32buffer1, sizeof (uint32) * size,
                "trav data u32buffer1");
    BCMI_FT_ALLOC(trav_data->u32buffer2, sizeof (uint32) * size,
                "trav data u32buffer2");
    BCMI_FT_ALLOC(trav_data->u32buffer3, sizeof (uint32) * size,
                "trav data u32buffer3");

    size = 128;
    BCMI_FT_ALLOC(trav_data->u8buffer1, sizeof (uint8) * size,
                "trav data u8buffer1");
    BCMI_FT_ALLOC(trav_data->u8buffer2, sizeof (uint8) * size,
                "trav data u8buffer2");

    BCMI_FT_ALLOC(trav_data->record_info,
                sizeof (bcm_flowtracker_group_record_info_t),
                "trav data record info");

}

/*
 * Function:
 *   bcmi_ft_group_hw_entry_buffer_free
 * Purpose:
 *   Free buffer used in traversing records in the flowtracker group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   trav_data      - (IN) Traversal Data
 * Returns:
 *   BCM_E_XXX.
 */
STATIC void
bcmi_ft_group_hw_entry_buffer_free(
        int unit,
        bcmi_ft_group_record_trav_data_t *trav_data)
{
    BCMI_FT_FREE(trav_data->u32buffer1);
    BCMI_FT_FREE(trav_data->u32buffer2);
    BCMI_FT_FREE(trav_data->u32buffer3);

    BCMI_FT_FREE(trav_data->u8buffer1);
    BCMI_FT_FREE(trav_data->u8buffer2);

    BCMI_FT_FREE(trav_data->record_info);
}

/*
 * Function:
 *   bcmi_ft_group_record_offsets_set
 * Purpose:
 *   Set offset of key/data fields in the export record buffer to be used in
 *   traversing records in the flowtracker group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   trav_data      - (IN) Traversal Data
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ft_group_record_offsets_set(int unit,
    bcmi_ft_group_record_trav_data_t *trav_data)
{
    int total_bits = 0;
    soc_field_info_t *field_info = NULL;
    soc_format_t format;

    format = BSC_EX_EXPORT_RECORDfmt;

    /* Get record total bits, base offset of SET HEADER */
    field_info = soc_format_fieldinfo_get(unit, format, SET_HEADERf);
    if (field_info != NULL) {
        total_bits = field_info->bp + field_info->len;
        trav_data->md_offset = field_info->len / 8;
    }

    /* FT_Group */
    field_info = soc_format_fieldinfo_get(unit, format, GROUP_IDf);
    if (field_info != NULL) {
        trav_data->group_idx_bits = field_info->len;
    }

    /* FLOW_KEY */
    field_info = soc_format_fieldinfo_get(unit, format, FLOW_KEYf);
    if (field_info != NULL) {
        trav_data->key_offset = (total_bits -
                (field_info->bp + field_info->len)) / 8 - 1;
    }

    /* FLOW_DATA */
    field_info = soc_format_fieldinfo_get(unit, format, FLOW_DATAf);
    if (field_info != NULL) {
        trav_data->data_offset = (total_bits -
                (field_info->bp + field_info->len)) / 8;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_records_mem_traverse
 * Purpose:
 *   Internal traversal function to read hardware memory to get valid record
 *   of the flowtracker group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   mem            - (IN) Memory
 *   trav_data      - (IN) Traversal Data
 *   cb             - (IN) Internal callback function to read key/data of record.
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ft_group_records_mem_traverse(
        int unit,
        soc_mem_t mem,
        bcmi_ft_group_record_trav_data_t *trav_data,
        bcmi_ft_group_hw_entry_get_cb_f cb)
{
#define DMA_CHUNK_SIZE 1024

    int rv = BCM_E_NONE;
    int jmp = 0;
    int8 *buffer = NULL;
    void *entry = NULL;
    uint32 c_idx = 0, e_idx = 0;
    int mem_min = 0, mem_max = 0;
    uint32 chunk_size = 0, mem_entry_size = 0;

    mem_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);

    chunk_size = ((mem_max - mem_min + 1) > DMA_CHUNK_SIZE) ?
                DMA_CHUNK_SIZE : (mem_max - mem_min + 1);
    mem_entry_size = SOC_MEM_WORDS(unit, mem) * sizeof(uint32);

    buffer = soc_cm_salloc(unit, mem_entry_size * chunk_size, "mem buffer");
    if (buffer == NULL)  {
        return BCM_E_MEMORY;
    }
    for (c_idx = mem_min; c_idx <= mem_max; c_idx += chunk_size) {
        chunk_size = ((mem_max - c_idx + 1) > DMA_CHUNK_SIZE) ?
                    DMA_CHUNK_SIZE : (mem_max - c_idx + 1);

        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                    c_idx, c_idx + chunk_size - 1, buffer)) != SOC_E_NONE) {
            break;
        }
        for (e_idx = 0; e_idx < chunk_size;  e_idx += jmp) {

            entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                    buffer, e_idx);
            rv = (*cb)(unit, mem, c_idx + e_idx,
                    entry, (void *) trav_data, &jmp);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    if (buffer != NULL) {
        soc_cm_sfree(unit, buffer);
    }
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_record_key_get
 * Purpose:
 *   To get key fields of the record of the flowtracker group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   id             - (IN) Flowtracker Group Id
 *   trav_data      - (IN) Traversal Data
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ft_group_record_key_get(int unit,
    bcm_flowtracker_group_t id,
    bcmi_ft_group_record_trav_data_t *trav_data)
{
    int rv = BCM_E_NONE;
    int w = 0, num_bits = 0, num_bytes = 0;
    uint8 *ptr = NULL;
    uint32 *entry_buf = NULL;
    uint32 *skey_buf = NULL, *dkey_buf = NULL;
    soc_mem_t key_mem = INVALIDm;
    bcmi_ft_group_key_data_mode_t mode;
    bcm_flowtracker_export_record_t *ex_record = NULL;

    /* Buffer pointers */
    entry_buf = trav_data->u32buffer1;
    skey_buf = trav_data->u32buffer2;
    dkey_buf = trav_data->u32buffer3;

    num_bits = soc_mem_field_length(unit, DO_NOT_FT_ENTRYm, KEY_0f);
    num_bytes = BITS2BYTES(num_bits) * 2;

    /* Export Record */
    ex_record = &trav_data->record_info->record;

    mode = BCMI_FT_GROUP_FTFP_DATA(unit, id).session_key_mode;
    if (mode == bcmiFtGroupModeSingle) {
        key_mem = FT_KEY_SINGLEm;

        rv = soc_mem_read(unit, key_mem,
                MEM_BLOCK_ANY, trav_data->index, entry_buf);
        BCM_IF_ERROR_RETURN(rv);

        soc_mem_field_get(unit, key_mem, entry_buf, KEYf, skey_buf);
        SHR_BITCOPY_RANGE(dkey_buf, 0, skey_buf, 0, num_bits);

    } else {

        key_mem = FT_KEY_DOUBLEm;

        rv = soc_mem_read(unit, key_mem,
                MEM_BLOCK_ANY, trav_data->index, entry_buf);
        BCM_IF_ERROR_RETURN(rv);

        /* High part of the Key */
        soc_mem_field_get(unit, key_mem, entry_buf, KEY_1f, skey_buf);
        SHR_BITCOPY_RANGE(dkey_buf, num_bits, skey_buf, 0, num_bits);

        /* Low part of the Key */
        soc_mem_field_get(unit, key_mem, entry_buf, KEY_0f, skey_buf);
        SHR_BITCOPY_RANGE(dkey_buf, 0, skey_buf, 0, num_bits);
    }

    ptr = &ex_record->data_record[trav_data->key_offset];
    for (w = 0; w < BYTES2WORDS(num_bytes); w++) {
        _SHR_PACK_U32(ptr, dkey_buf[(BYTES2WORDS(num_bytes) - w - 1)]);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_record_data_get
 * Purpose:
 *   To get data fields of the record of the flowtracker group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   id             - (IN) Flowtracker Group Id
 *   trav_data      - (IN) Traversal Data
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ft_group_record_data_get(int unit,
    bcm_flowtracker_group_t id,
    bcmi_ft_group_record_trav_data_t *trav_data)
{
    int rv = BCM_E_NONE;
    int data_words = 0;
    int w = 0;
    uint8 *ptr = NULL;
    uint32 *entry_buf = NULL;
    uint32 *u32data_buf = NULL;
    soc_mem_t data_mem = INVALIDm;
    bcmi_ft_group_key_data_mode_t mode;
    bcm_flowtracker_export_record_t *ex_record = NULL;

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    int ft_pdd_pos[3];
    int iter = 0, num_banks = 0;
    int byte = 0, num_bytes = 0;
    int offset = 0, data_offset = 0;
    uint8 *ptr2 = NULL;
    bcmi_ft_type_t ft_type;
    bcmi_ft_data_engine_db_t *engine_db = NULL;
    bcmi_ft_data_engine_info_t *engine_info = NULL;
    bcmi_ft_data_engine_per_ft_type_map_t *map = NULL;
#endif

    /* Buffer pointers */
    entry_buf   = trav_data->u32buffer1;
    u32data_buf = trav_data->u32buffer2;

    /* Export Record */
    ex_record = &trav_data->record_info->record;

    mode = BCMI_FT_GROUP_FTFP_DATA(unit, id).session_key_mode;
    if (mode == bcmiFtGroupModeSingle) {
        data_mem = BSC_DT_FLEX_SESSION_DATA_SINGLEm;
    } else {
        data_mem = BSC_DT_FLEX_SESSION_DATA_DOUBLEm;
    }

    rv = soc_mem_read(unit, data_mem,
            MEM_BLOCK_ANY, trav_data->index, entry_buf);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field_get(unit, data_mem, entry_buf, OPAQUE_DATAf, u32data_buf);
    data_words = BITS2WORDS(soc_mem_field_length(unit, data_mem, OPAQUE_DATAf));

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

        ptr = trav_data->u8buffer1;
        for (iter = 0; iter < data_words; iter++) {
            _SHR_PACK_U32(ptr, u32data_buf[data_words - iter - 1]);
        }

        /* FT Type PDD base offset */
        ft_pdd_pos[bcmiFtTypeIFT - 1] = 0;
        num_banks = BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeIFT);
        ft_pdd_pos[bcmiFtTypeMFT - 1] =
            ft_pdd_pos[bcmiFtTypeIFT - 1] + num_banks * 2 * 8;
        num_banks = BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeMFT);
        ft_pdd_pos[bcmiFtTypeEFT - 1] =
            ft_pdd_pos[bcmiFtTypeMFT - 1] + num_banks * 2 * 8;

        num_bytes = 8;
        ptr  = trav_data->u8buffer1;
        ptr2 = trav_data->u8buffer2;
        iter = (mode == bcmiFtGroupModeSingle) ? 4: 8;
        for (; iter > 0; iter--, ptr += num_bytes) {

            ft_type = BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, iter - 1);
            sal_memcpy(ptr2 + ft_pdd_pos[ft_type - 1], ptr, num_bytes);
            ft_pdd_pos[ft_type - 1] += num_bytes;
        }

        /* Reset FT Type PDD base offset */
        ft_pdd_pos[bcmiFtTypeIFT - 1] = 0;
        num_banks = BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeIFT);
        ft_pdd_pos[bcmiFtTypeMFT - 1] =
            ft_pdd_pos[bcmiFtTypeIFT - 1] + num_banks * 2 * 8;
        num_banks = BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, bcmiFtTypeMFT);
        ft_pdd_pos[bcmiFtTypeEFT - 1] =
            ft_pdd_pos[bcmiFtTypeMFT - 1] + num_banks * 2 * 8;

        engine_db = BCMI_FT_GROUP(unit, id)->engine_db;

        for (ft_type = bcmiFtTypeIFT; ft_type <= bcmiFtTypeEFT; ft_type++) {

            map = &engine_db->ft_type_map[ft_type];
            offset = trav_data->data_offset + ft_pdd_pos[ft_type-1];

            for (iter = 0; iter < map->count; iter++) {

                engine_info = map->list[iter];
                if (engine_info->enabled == 0) {
                    continue;
                }

                num_bytes = engine_info->size / 8;
                data_offset = engine_info->pdd_offset + ft_pdd_pos[ft_type-1];
                for (byte = 0; byte < num_bytes; byte++) {
                    ex_record->data_record[offset + byte] =
                        trav_data->u8buffer2[data_offset + byte];
                }
                offset += num_bytes;
            }
        }
    } else
#endif
    {
        ptr = &ex_record->data_record[trav_data->data_offset];
        for (w = 0; w < data_words; w++) {
            _SHR_PACK_U32(ptr, u32data_buf[data_words - w - 1]);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_hw_entry_record_get
 * Purpose:
 *   To get key/data fields of the record of the flowtracker group and
 *   call user callback.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   mem            - (IN) Memory
 *   index          - (IN) Index of the entry
 *   entry          - (IN) Hardware entry
 *   user_data      - (IN) Traversal data
 *   move           - (IN) Position to next entry to read
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_hw_entry_record_get(int unit,
    soc_mem_t mem, uint32 index,
    void *entry, void *user_data, int *move)
{
    int rv = BCM_E_NONE;
    uint8 *ptr = NULL;
    uint32 flow_identifier = 0;
    bcmi_ft_group_key_data_mode_t mode;
    bcm_flowtracker_group_t ent_gid;
    bcm_flowtracker_group_type_t group_type;
    bcmi_ft_group_record_trav_data_t *trav_data = NULL;
    bcm_flowtracker_export_record_t *ex_record = NULL;

    *move = 1;
    trav_data = (bcmi_ft_group_record_trav_data_t *) user_data;

    if (!soc_mem_field32_get(unit, mem, entry, ENTRY_VALIDf)) {
        return BCM_E_NONE;
    }

    group_type = bcmFlowtrackerGroupTypeNone;
    ent_gid = soc_mem_field32_get(unit, mem, entry, GROUP_IDf);

    /* Skip if group is not configured */
    if (!SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit, group_type), ent_gid)) {
        return BCM_E_NONE;
    }

    mode = BCMI_FT_GROUP_FTFP_DATA(unit, ent_gid).session_key_mode;
    if (mode == bcmiFtGroupModeSingle) {
        *move = 1;
        trav_data->index = index;
    } else {
        *move = 2;
        trav_data->index = index / 2;
    }

    if (trav_data->flags & BCMI_FT_GROUP_RECORD_TRV_F_GROUP) {
        if (trav_data->group_id != ent_gid) {
            return BCM_E_NONE;
        }
    }

    /* Group Record */
    trav_data->record_info->group_id = ent_gid;
    trav_data->record_info->index = trav_data->index;
    trav_data->record_info->type = trav_data->record_type;

    /* Flow Identifier */
    ex_record = &trav_data->record_info->record;
    flow_identifier = (trav_data->index << trav_data->group_idx_bits) +
        BCMI_FT_GROUP_INDEX_GET(trav_data->group_id);
    ptr = &ex_record->data_record[trav_data->md_offset];
    _SHR_PACK_U32(ptr, flow_identifier);

    if (trav_data->flags & BCMI_FT_GROUP_RECORD_TRV_F_KEY) {
        rv = bcmi_ft_group_record_key_get(unit, ent_gid, trav_data);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (trav_data->flags & BCMI_FT_GROUP_RECORD_TRV_F_DATA) {
        rv = bcmi_ft_group_record_data_get(unit, ent_gid, trav_data);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Call Callback routine */
    if (trav_data->cb != NULL) {
        rv = trav_data->cb(unit, trav_data->record_info,
                (void *) trav_data->user_data);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_record_traverse
 * Purpose:
 *   Traversal function to get all records of the flowtracker group.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   group_id       - (IN) Flowtracker Group Id
 *   type           - (IN) Record content to read
 *   cb             - (IN) User callback function
 *   user_data      - (IN) User data
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ft_group_record_traverse(int unit,
        bcm_flowtracker_group_t group_id,
        bcm_flowtracker_group_record_type_t type,
        bcm_flowtracker_group_record_traverse_cb cb,
        void *user_data)
{
    int rv = BCM_E_NONE;
    bcmi_ft_group_record_trav_data_t trav_data;

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    /* Check if group exists */
    if (bcmi_ft_group_is_invalid(unit, group_id)) {
        return BCM_E_NOT_FOUND;
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(group_id)) {
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
            return bcmi_ftv3_agg_group_record_traverse(unit,
                    group_id, type, cb, user_data);
        }
#endif
        return BCM_E_PARAM;
    }

    sal_memset(&trav_data, 0, sizeof(trav_data));
    if (type == bcmFlowtrackerGroupRecordTypeKeyOnly) {
        trav_data.flags |= BCMI_FT_GROUP_RECORD_TRV_F_KEY;
    } else if (type == bcmFlowtrackerGroupRecordTypeKeyData) {
        trav_data.flags |= BCMI_FT_GROUP_RECORD_TRV_F_KEY;
        trav_data.flags |= BCMI_FT_GROUP_RECORD_TRV_F_DATA;
    } else {
        return BCM_E_PARAM;
    }

    trav_data.group_id = group_id;
    trav_data.flags |= BCMI_FT_GROUP_RECORD_TRV_F_GROUP;

    trav_data.record_type = type;
    trav_data.cb = cb;
    trav_data.user_data = user_data;

    bcmi_ft_group_record_offsets_set(unit, &trav_data);

    /* allocate buffers for key/data fields */
    bcmi_ft_group_hw_entry_buffer_allocate(unit, &trav_data);

    rv = bcmi_ft_group_records_mem_traverse(unit, BSC_AG_AGE_TABLEm,
            &trav_data, bcmi_ft_group_hw_entry_record_get);
    BCMI_CLEANUP_IF_ERROR(rv);

cleanup:

    /* Free all allocated buffers */
    bcmi_ft_group_hw_entry_buffer_free(unit, &trav_data);

    return rv;
}

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
/*
 * Function:
 *   bcmi_ftv3_agg_hw_entry_record_get
 * Purpose:
 *   To get hardware record for aggregate flowtracker group and call
 *   usre callback function.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   trav_data      - (IN) Traversal data
 *   group_id       - (IN) Flowtracker Group Id
 *   type           - (IN) Record content to read
 *   cb             - (IN) User callback function
 *   user_data      - (IN) User data
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ftv3_agg_hw_entry_record_get(int unit,
        bcmi_ft_group_record_trav_data_t *trav_data)
{
    int rv = BCM_E_NONE;
    int w = 0, num_bytes = 0;
    int offset = 0, bank = 0;
    uint8 *ptr = NULL;
    uint32 flow_identifier = 0;
    uint32 *data_buf = NULL, *entry_buf = NULL;
    bcmi_ft_type_t ft_type;
    bcm_flowtracker_export_record_t *ex_record = NULL;
    soc_mem_t data_bank_mem[] = {
        BSC_DT_FLEX_AFT_SESSION_DATA_BANK_0m,
        BSC_DT_FLEX_AFT_SESSION_DATA_BANK_1m,
        BSC_DT_FLEX_AFT_SESSION_DATA_BANK_2m,
        BSC_DT_FLEX_AFT_SESSION_DATA_BANK_3m
    };

    data_buf  = trav_data->u32buffer1;
    entry_buf = trav_data->u32buffer2;

    for (bank = 0;
            bank < TOTAL_GROUP_AGG_SESSION_DATA_BANK_MEM; bank++) {

        ft_type = BCMI_FT_AGG_SESSION_DATA_BANK_FT_TYPE(unit, bank, 0);

        if ((1 << ft_type) != trav_data->ft_type_bmp) {
            continue;
        }

        rv = soc_mem_read(unit, data_bank_mem[bank],
                MEM_BLOCK_ANY, trav_data->index, entry_buf);
        BCM_IF_ERROR_RETURN(rv);

        /* Check if Entry is valid */
        if(!soc_mem_field32_get(unit, data_bank_mem[bank],
                    entry_buf, VALIDf)) {
            return BCM_E_NONE;
        }

        soc_mem_field_get(unit, data_bank_mem[bank],
                entry_buf, OPAQUE_DATAf, data_buf);

        num_bytes = BITS2BYTES(soc_mem_field_length(unit,
                    data_bank_mem[bank], OPAQUE_DATAf));

        offset = trav_data->data_offset +
            (TOTAL_GROUP_AGG_SESSION_DATA_BANK_MEM - bank - 1) * num_bytes;

        ex_record = &trav_data->record_info->record;
        ptr = &ex_record->data_record[offset];
        for (w = 0; w < BYTES2WORDS(num_bytes); w++) {
            _SHR_PACK_U32(ptr, data_buf[(BYTES2WORDS(num_bytes) - w - 1)]);
        }
    }

    /* Flow Identifier */
    ex_record = &trav_data->record_info->record;
    flow_identifier = (trav_data->index << trav_data->group_idx_bits) +
        BCMI_FT_GROUP_INDEX_GET(trav_data->group_id);
    ptr = &ex_record->data_record[trav_data->md_offset];
    _SHR_PACK_U32(ptr, flow_identifier);

    /* Group Record */
    trav_data->record_info->index = trav_data->index;
    trav_data->record_info->group_id = trav_data->group_id;
    trav_data->record_info->type = trav_data->record_type;

    /* Call Callback routine */
    if (trav_data->cb != NULL) {
        rv = trav_data->cb(unit, trav_data->record_info,
                (void *) trav_data->user_data);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ftv3_agg_group_record_traverse
 * Purpose:
 *   Traversal function for aggregate flowtracker groups.
 * Parameters:
 *   unit           - (IN) BCM device number
 *   group_id       - (IN) Flowtracker Group Id
 *   type           - (IN) Record content to read
 *   cb             - (IN) User callback function
 *   user_data      - (IN) User data
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv3_agg_group_record_traverse(int unit,
        bcm_flowtracker_group_t group_id,
        bcm_flowtracker_group_record_type_t type,
        bcm_flowtracker_group_record_traverse_cb cb,
        void *user_data)
{
    int rv = BCM_E_NONE;
    int e_idx = 0, num_entries = 0;
    bcm_flowtracker_group_type_t group_type;
    bcmi_ft_group_record_trav_data_t trav_data;

    if (type != bcmFlowtrackerGroupRecordTypeKeyData) {
        return BCM_E_PARAM;
    }

    group_type = BCMI_FT_GROUP_TYPE_GET(group_id);
    num_entries = BCMI_FTK_USER_ENTRY_MAX_COUNT(unit, group_type);

    sal_memset(&trav_data, 0, sizeof(trav_data));
    trav_data.group_id = group_id;
    BCMI_FT_GROUP_FT_TYPE_BMP_GET(group_type, trav_data.ft_type_bmp);

    trav_data.record_type = type;
    trav_data.cb = cb;
    trav_data.user_data = user_data;

    bcmi_ft_group_record_offsets_set(unit, &trav_data);

    /* Allocate memory */
    bcmi_ft_group_hw_entry_buffer_allocate(unit, &trav_data);

    for (e_idx = 0; e_idx < num_entries; e_idx++) {

        if (!BCMI_FTK_USER_ENTRY_INFO(unit, group_type, e_idx)) {
            continue;
        }

        if (trav_data.group_id !=
                BCMI_FTK_USER_ENTRY_INFO(unit, group_type, e_idx)->group_id) {
            continue;
        }

        trav_data.index = e_idx;
        rv = bcmi_ftv3_agg_hw_entry_record_get(unit, &trav_data);
        BCMI_CLEANUP_IF_ERROR(rv);
    }

cleanup:

    /* Free up memory */
    bcmi_ft_group_hw_entry_buffer_free(unit, &trav_data);

    return rv;
}

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

/*
 * Function:
 *   bcmi_ft_group_checks_bitmap_get
 * Purpose:
 *   Get flowtracker check in ALU16 bitmap.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *  id         - (IN) Flowtracker Group Id.
 *  num_checks - (IN) Number of flowchecks.
 *  check_list - (IN) List of Flowchecks
 *  check_bmp_data - (OUT) Data bitmap
 *  check_bmp_mask - (OUT) Mask bitmap.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_ft_group_checks_bitmap_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *check_bmp_data,
        uint32 *check_bmp_mask)
{
    int rv = BCM_E_NONE;
    int bit = 0;
    int iter = 0;
    int flags = 0;
    uint32 found = 0;
    uint32 value = 0;
    bcm_flowtracker_check_t check_id;
    bcm_flowtracker_check_t check_map[8] = {0};

    rv = bcmi_ft_group_is_invalid(unit, id);
    BCM_IF_ERROR_RETURN(rv);

    /* Get List of Flowcheck for flowtracker group */
    rv = bcmi_ft_alu_check16_mapping_get(unit, id, check_map);
    BCM_IF_ERROR_RETURN(rv);

    flags = BCMI_FT_GROUP_CHECK_MATCH_PRIMARY;

    for (iter = 0; iter < num_checks; iter++) {
        check_id = check_list[iter].flow_check_id;
        value = check_list[iter].value;

        rv = bcmi_ft_group_check_validate(unit,
                flags, id, check_id);
        BCM_IF_ERROR_RETURN(rv);

        for (bit = 0; bit < 8; bit++) {
            if (check_map[bit] == check_id) {
                *check_bmp_mask |= (1 << bit);
                if (value == TRUE) {
                    *check_bmp_data |= (1 << bit);
                }
                found++;
                break;
            }
        }
    }

    if (found != num_checks) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_checks_list_get
 * Purpose:
 *   Get list of flowtracker check from ALU16 bitmap.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracker Group Id.
 *   check_bmp_data - (IN) Data bitmap.
 *   check_bmp_mask - (IN) Mask bitmap.
 *   num_checks - (IN) Number of flowchecks.
 *   check_list - (OUT) List of Flowchecks.
 *   actual_num_checks - (OUT) actual number of flowchecks.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_checks_list_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 check_bmp_data,
        uint32 check_bmp_mask,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *actual_num_checks)
{
    int rv = BCM_E_NONE;
    int i = 0, check_idx = 0;
    bcm_flowtracker_check_t check_map[8] = {0};

    if (actual_num_checks == NULL) {
        return BCM_E_PARAM;
    }

    rv = bcmi_ft_group_is_invalid(unit, id);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_ft_alu_check16_mapping_get(unit, id, check_map);
    BCM_IF_ERROR_RETURN(rv);

    if (check_list == NULL) {
        *actual_num_checks = _shr_popcount(check_bmp_mask);
        return BCM_E_NONE;
    }

    for (i = 0; (i < 8) && (check_idx < num_checks); i++) {

        if ((check_bmp_data) & (1 << i)) {
            check_list[check_idx].value = 1;
        }
        if ((check_bmp_mask) & (1 << i)) {
            check_list[check_idx].flow_check_id = check_map[i];
            check_idx++;

            if (check_idx >= num_checks) {
                break;
            }
        }
    }

    *actual_num_checks = check_idx;

    return BCM_E_NONE;
}

#else /* BCM_FLOWTRACKER_SUPPORT*/
typedef int bcmi_ft_group_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT */
