/*! \file flowtracker_common.c
 *
 * Common functions for both XGS and XFS platforms for Flowtracker.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* API related header files */
#include <bcm/flowtracker.h>
#include <bcm/field.h>
#include <bcm/flexctr.h>

/* Internal common header files */
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/flexctr.h>

/* Internal flowtracker header files */
#include <bcm_int/ltsw/flowtracker_int.h>
#include <bcm_int/ltsw/mbcm/flowtracker.h>

/* SDKLT specific header files */
#include <bcmltd/chip/bcmltd_str.h>

/* Shared library header files */
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

#define BSL_LOG_MODULE BSL_LS_BCM_FLOWTRACKER

/* Flow tracker global information */
bcm_int_ft_info_t *ft_glb_info[BCM_MAX_NUM_UNITS] = {NULL};

/* Template ID pool */
SHR_BITDCL *export_template_pool = NULL;

/* Flow group ID pool */
SHR_BITDCL *flow_group_pool = NULL;

/* Elephant profile ID pool */
SHR_BITDCL *elph_profile_pool = NULL;

/* Template elements API <--> app mapping */
static bcm_ltsw_ft_template_support_mapping_t ft_template_element_mapping[] = {
    {bcmFlowtrackerExportElementTypeSrcIPv4,    FT_TEMPLATE_INFO_ELEM_SRC_IPV4, 4},
    {bcmFlowtrackerExportElementTypeDstIPv4,    FT_TEMPLATE_INFO_ELEM_DST_IPV4, 4},
    {bcmFlowtrackerExportElementTypeL4SrcPort,  FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT, 2},
    {bcmFlowtrackerExportElementTypeL4DstPort,  FT_TEMPLATE_INFO_ELEM_L4_DST_PORT, 2},
    {bcmFlowtrackerExportElementTypeIPProtocol, FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL, 1},
    {bcmFlowtrackerExportElementTypePktCount,   FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT, 4},
    {bcmFlowtrackerExportElementTypeByteCount,  FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT, 6},
    {bcmFlowtrackerExportElementTypeFlowStartTimeMsecs, FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC, 8},
    {bcmFlowtrackerExportElementTypeObservationTimeMsecs, FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC, 8},
    {bcmFlowtrackerExportElementTypeFlowtrackerGroup, FT_TEMPLATE_INFO_ELEM_GROUP_ID, 1},
    {bcmFlowtrackerExportElementTypeVxlanNetworkId, FT_TEMPLATE_INFO_ELEM_VNID, 3},
    {bcmFlowtrackerExportElementTypeCustom, FT_TEMPLATE_INFO_ELEM_CUSTOM, 16},
    {bcmFlowtrackerExportElementTypeInnerSrcIPv4, FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV4, 4},
    {bcmFlowtrackerExportElementTypeInnerDstIPv4, FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV4, 4},
    {bcmFlowtrackerExportElementTypeInnerL4SrcPort, FT_TEMPLATE_INFO_ELEM_INNER_L4_SRC_PORT, 2},
    {bcmFlowtrackerExportElementTypeInnerL4DstPort, FT_TEMPLATE_INFO_ELEM_INNER_L4_DST_PORT, 2},
    {bcmFlowtrackerExportElementTypeInnerIPProtocol, FT_TEMPLATE_INFO_ELEM_INNER_IP_PROTOCOL, 1},
    {bcmFlowtrackerExportElementTypeFlowViewID, FT_TEMPLATE_INFO_ELEM_FLOW_VIEW_ID, 1},
    {bcmFlowtrackerExportElementTypeFlowViewSampleCount, FT_TEMPLATE_INFO_ELEM_SAMPLE_COUNT, 2},
    {bcmFlowtrackerExportElementTypeFlowCount, FT_TEMPLATE_INFO_ELEM_FLOW_COUNT, 2},
    {bcmFlowtrackerExportElementTypeDropPktCount, FT_TEMPLATE_INFO_ELEM_DROP_PKT_COUNT, 4},
    {bcmFlowtrackerExportElementTypeDropByteCount, FT_TEMPLATE_INFO_ELEM_DROP_BYTE_COUNT, 6},
    {bcmFlowtrackerExportElementTypeEgrPort, FT_TEMPLATE_INFO_ELEM_EGR_PORT, 2},
    {bcmFlowtrackerExportElementTypeQueueID, FT_TEMPLATE_INFO_ELEM_QUEUE_ID, 1},
    {bcmFlowtrackerExportElementTypeQueueBufferUsage, FT_TEMPLATE_INFO_ELEM_QUEUE_BUF_USAGE, 2},
    {bcmFlowtrackerExportElementTypeInnerSrcIPv6, FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV6, 16},
    {bcmFlowtrackerExportElementTypeInnerDstIPv6, FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV6, 16},
};
/******************************************************************************
 * Private Functions
 */


/* By default, this creates with HA memory alloc only. */
static SHR_BITDCL *
bcm_ltsw_flowtracker_list_create(int unit,
                                 bool warm,
                                 int sub_comp_id,
                                 int max_ids)
{
    const char *blk_ids[] = FT_HA_SUB_COMP_ID_STRINGS;
    SHR_BITDCL *ptr = NULL;
    uint32_t alloc_sz = SHR_BITALLOCSIZE(max_ids);

    ptr = bcmi_ltsw_ha_mem_alloc(
                        unit,
                        BCMI_HA_COMP_ID_FLOWTRACKER,
                        sub_comp_id,
                        blk_ids[sub_comp_id],
                        &alloc_sz);
    if (!warm) {
        sal_memset(ptr, 0, alloc_sz);
    }
    return ptr;
}

static void
bcm_ltsw_flowtracker_list_destroy(int unit, SHR_BITDCL *bmp)
{
    (void)(bcmi_ltsw_ha_mem_free)(unit, bmp);
}

static bool
bcm_ltsw_flowtracker_list_id_present(int unit, SHR_BITDCL *bmp, int id)
{
    return SHR_BITGET(bmp, id);
}

static int
bcm_ltsw_flowtracker_list_id_alloc(int unit,
                                   SHR_BITDCL *bmp,
                                   int max_ids,
                                   int *id)
{
    int i = 0;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < max_ids; i++) {
        if (!SHR_BITGET(bmp, i)) {
            *id = i;
            SHR_BITSET(bmp, i);
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_RESOURCE);
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_list_id_clr(int unit, SHR_BITDCL *bmp, int id)
{
    SHR_FUNC_ENTER(unit);
    if (SHR_BITGET(bmp, id)) {
        SHR_BITCLR(bmp,id);
    } else {
        /* Clearing an unset resource */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_list_id_reserve(int unit, SHR_BITDCL *bmp, int id)
{
    SHR_FUNC_ENTER(unit);
    if (!SHR_BITGET(bmp, id)) {
        SHR_BITSET(bmp, id);
    } else {
        /* Reserving a set resource */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    SHR_FUNC_EXIT();
}

static void
bcm_ltsw_flowtracker_free_index_lists(int unit)
{
    if (flow_group_pool != NULL) {
        bcm_ltsw_flowtracker_list_destroy(unit, flow_group_pool);
        flow_group_pool = NULL;
    }
    if (export_template_pool != NULL) {
        bcm_ltsw_flowtracker_list_destroy(unit, export_template_pool);
        export_template_pool = NULL;
    }
    if (elph_profile_pool != NULL) {
        bcm_ltsw_flowtracker_list_destroy(unit, elph_profile_pool);
        elph_profile_pool = NULL;
    }
}

static int
bcm_ltsw_flowtracker_init_index_lists(int unit, bool warm)
{
    int reserved_id = 0;
    SHR_FUNC_ENTER(unit);

    /* Initialize the flow group index list */
    flow_group_pool = bcm_ltsw_flowtracker_list_create
                                            (unit,
                                            warm,
                                            FT_SUB_COMP_ID_FLOW_GROUP_POOL,
                                            FT_MAX_FLOW_GROUPS);
    if (!flow_group_pool) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        /* Reserve index 0 since that is invalid flow group id */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flowtracker_list_id_reserve
                            (unit, flow_group_pool, reserved_id));
    }


    /* Initialize the export template index list */
    export_template_pool = bcm_ltsw_flowtracker_list_create
                                            (unit,
                                            warm,
                                            FT_SUB_COMP_ID_EXPORT_TEMPLATE_POOL,
                                            FT_MAX_EXPORT_TEMPLATES+1);
    if (!export_template_pool) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        /* Reserve index 0 since that is invalid export template id */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flowtracker_list_id_reserve(
                    unit, export_template_pool, reserved_id));
    }

    /* Initialize the export template index list */
    elph_profile_pool = bcm_ltsw_flowtracker_list_create
                                          (unit,
                                          warm,
                                          FT_SUB_COMP_ID_ELEPHANT_PROFILE_POOL,
                                          FT_MAX_ELEPHANT_PROFILES + 1);

    if (!elph_profile_pool) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        /* Reserving zeroth index to use it as invaild id */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_flowtracker_list_id_reserve(
                 unit, elph_profile_pool, reserved_id));
    }
exit:
    if(SHR_FUNC_ERR()) {
        bcm_ltsw_flowtracker_free_index_lists(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_create_validate_and_alloc_id
 * Purpose:
 *      Validate flowtracker group params and allocate/reserve group ID.
 * Parameters:
 *      unit              - (IN)     BCM device number
 *      options           - (IN)     Group creation options
 *      id                - (IN/OUT) Flowtracker group ID
 *      flow_group_info   - (IN)     Flowtracker group information
 *      id_alloced        - (OUT)    Was the ID alloced/reserved ?
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
static int
bcm_ltsw_flowtracker_group_create_validate_and_alloc_id(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t *id,
        bcm_flowtracker_group_info_t *flow_group_info,
        bool *id_alloced)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bool def_grp_supp = false;
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    int rv = SHR_E_NONE, i;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(flow_group_info, SHR_E_PARAM);

    /*
     * IF replace, WITH_ID is mandatory. If not provided, return
     * SHR_E_PARAM.
     */
    if ((options & BCM_FLOWTRACKER_GROUP_REPLACE) &&
       !(options & BCM_FLOWTRACKER_GROUP_WITH_ID)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if default group config is supported */
    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_flowtracker_default_grp_supported(unit, &def_grp_supp));

    /*
     * If default group is not supported, check it and return
     * SHR_E_UNAVAIL.
     */
    if ((flow_group_info->group_flags & BCM_FLOWTRACKER_GROUP_DEFAULT) &&
        (def_grp_supp == false)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /*
     * Verify that a flow group with such an ID
     * exists, else return SHR_E_NOT_FOUND
     */
    if ((options & BCM_FLOWTRACKER_GROUP_REPLACE) &&
        (options & BCM_FLOWTRACKER_GROUP_WITH_ID)) {
        if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool, *id)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                       unit, *id));
            SHR_ERR_EXIT (SHR_E_NOT_FOUND);
        }
        sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
        flow_group_int_info.flow_group_id = *id;
        /* Get the flow group info for this group id */
        rv = bcm_ltsw_flowtracker_group_entry_get(unit,&flow_group_int_info);

        /* Return SHR_E_NOT_FOUND if not found */
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }

        
        for (i = 1; i < ft_info->num_pipes; ++i) {
            if (flow_group_info->field_group[i] !=
                        flow_group_info->field_group[0]) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                "FT(unit %d) Error: Group (ID:%d) Per pipe EM mode\
                not supported.\n"),
                unit, *id));
                SHR_ERR_EXIT (SHR_E_CONFIG);
            }
        }

        for (i = 0; i < ft_info->num_pipes; ++i) {
            if (flow_group_info->field_group[i] !=
                        flow_group_int_info.field_group[i]) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                "FT(unit %d) Error: Group (ID:%d) field group\
                can not modify.\n"),
                unit, *id));
                SHR_ERR_EXIT (SHR_E_CONFIG);
            }
        }
    }

    /* WITH_ID */
    if (options & BCM_FLOWTRACKER_GROUP_WITH_ID) {
        if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
            /* If WITH_ID and not replace, reserve the ID */
            rv = bcm_ltsw_flowtracker_list_id_reserve(unit, flow_group_pool,
                                         *id);
            if (SHR_FAILURE(rv)) {
                SHR_ERR_EXIT ((rv == SHR_E_RESOURCE) ? SHR_E_EXISTS : rv);
            }
            *id_alloced = true;
        }
    } else {
        /* Else allocate the ID */
        rv = bcm_ltsw_flowtracker_list_id_alloc(unit, flow_group_pool,
                                   FT_MAX_FLOW_GROUPS, id);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }
        *id_alloced = true;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_group_delete_validate(
        int unit,
        bcm_flowtracker_group_t id
        )
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    bcm_int_ft_group_col_map_t col_map_entry;
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = id;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    if ((ft_info->elph_mode == TRUE) &&
        (flow_group_int_info.elph_profile_id != 0)) {
        /* Elephant profile must be detached prior to group delete */
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (flow_group_int_info.num_actions != 0) {
        /* Actions must be unset prior to group delete */
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /*
     * Do coll map get and find out if entry exists.
     * Return error if it exists.
     */
    sal_memset(&col_map_entry, 0, sizeof(col_map_entry));
    col_map_entry.group_col_map_id = id;
    rv = bcm_ltsw_flowtracker_group_collector_map_entry_get(unit,
                                                            &col_map_entry);
    if (rv == SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_group_tracking_params_validate(
                                int unit,
                                bcm_flowtracker_group_t id,
                                int num_tracking_params,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params,
    bcm_int_ft_flow_group_info_t *flow_group_int_info)
{
    SHR_FUNC_ENTER(unit);

    if (num_tracking_params == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else {
        SHR_NULL_CHECK(list_of_tracking_params, SHR_E_PARAM);
    }

    /* Chip specific validations */
    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_flowtracker_group_tracking_params_validate(
                                                    unit,
                                                    id,
                                                    num_tracking_params,
                                                    list_of_tracking_params));

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get the flow group info */
    flow_group_int_info->flow_group_id = id;
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_entry_get(unit,
                                                flow_group_int_info));

    if (flow_group_int_info->num_tracking_params != 0) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_group_export_triggers_validate(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info,
        bcm_int_ft_flow_group_info_t *flow_group_int_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(export_trigger_info, SHR_E_PARAM);

    /* Chip specific validations */
    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_flowtracker_group_export_triggers_validate(
                                                    unit,
                                                    id,
                                                    export_trigger_info));
    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get the flow group info */
    flow_group_int_info->flow_group_id = id;
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_entry_get(unit,
                                                flow_group_int_info));
exit:
    SHR_FUNC_EXIT();
}

static void
bcm_ltsw_flowtracker_group_export_triggers_info_update(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info,
        bcm_int_ft_flow_group_info_t *flow_group_int_info)
{
    int num_export_triggers = 0,i;

    /* Make them zero before updating with currently configured values */
    flow_group_int_info->num_export_triggers = 0;
    sal_memset(&(flow_group_int_info->export_triggers), 0,
                sizeof(flow_group_int_info->export_triggers));

    /* Update the configured values */
    for (i = bcmFlowtrackerExportTriggerNone + 1;
         i < bcmFlowtrackerExportTriggerCount;
         i++) {
        if (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info, i)) {
            flow_group_int_info->export_triggers[num_export_triggers] = i;
            num_export_triggers++;
        }
    }
    flow_group_int_info->num_export_triggers = num_export_triggers;
}

static int
bcm_ltsw_flowtracker_group_flow_limit_validate(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flow_limit,
        bcm_int_ft_flow_group_info_t *flow_group_int_info)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Validate flow limit against max flow limit  */
    if (flow_limit > ft_info->max_flows_total) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the flow group info */
    flow_group_int_info->flow_group_id = id;
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_entry_get(unit,
                                                flow_group_int_info));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_group_age_timer_validate(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 aging_interval_ms,
        bcm_int_ft_flow_group_info_t *flow_group_int_info)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint32 mult = FT_AGING_INTERVAL_SCAN_MULTIPLE;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (aging_interval_ms == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((aging_interval_ms % FT_AGING_INTERVAL_STEP_MSECS) != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (aging_interval_ms >
            mult * (ft_info->scan_interval_usecs / 1000)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get the flow group info */
    flow_group_int_info->flow_group_id = id;
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_entry_get(unit,
                                                flow_group_int_info));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Compare actions given in 2 different actions list and returns an error
 * if there is a mismatch.
 */
static int
bcm_ltsw_flowtracker_validate_actions_mismatch(
                                int unit,
                                int num_actions,
                                bcm_int_ft_group_action_info_t *action_list1,
                                bcm_int_ft_group_action_info_t *action_list2)
{
    int i, j;
    bool action_found = false;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_actions; i++) {
        action_found = false;
        for (j = 0; j < num_actions; j++) {
            if (action_list2[j].action == action_list1[i].action) {
                action_found = true;
                break;
            }
        }
        if (action_found == false) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_validate_conflicting_actions(
                                int unit,
                                int num_actions,
                                bcm_flowtracker_group_action_info_t *action_list)
{
    int i;
    int redirect_num_actions = 0;
    int dest_type_num_actions = 0;
    bool drop_action = false, drop_cancel_action = false; 
    bcm_flowtracker_group_action_t redirect_action =
                                        bcmFlowtrackerGroupActionCount;
    bcm_flowtracker_group_action_dest_type_t dest_type =
                                        bcmFlowtrackerGroupActionDestTypeCount;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_actions; i++) {
        /* Check if more than one redirect action present */
        if (action_list[i].action == bcmFlowtrackerGroupActionRedirectPort ||
            action_list[i].action == bcmFlowtrackerGroupActionRedirectIpmc ||
            action_list[i].action == bcmFlowtrackerGroupActionRedirectMcast ||
            action_list[i].action == bcmFlowtrackerGroupActionL3Switch ||
            action_list[i].action == bcmFlowtrackerGroupActionRedirectCancel) {
            redirect_num_actions++;
            if (redirect_num_actions >= 2) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            redirect_action = action_list[i].action;
        }

        /* Check if drop and drop cancel are present together */
        if (action_list[i].action == bcmFlowtrackerGroupActionDrop) {
            drop_action = true;
        } else if (action_list[i].action == bcmFlowtrackerGroupActionDropCancel) {
            drop_cancel_action = true;
        }

        if (drop_action == true && drop_cancel_action == true) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Check if destination type and redirect action is matching */
        if (action_list[i].action == bcmFlowtrackerGroupActionDestinationType) {
            dest_type_num_actions++;
            if (dest_type_num_actions >= 2) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            dest_type = action_list[i].params.param0;
        }
        if ((redirect_num_actions > 0) && (dest_type_num_actions > 0)) {
            if (redirect_action == bcmFlowtrackerGroupActionRedirectPort) {
                if(dest_type != bcmFlowtrackerGroupActionDestTypeL2EgrIntf &&
                   dest_type != bcmFlowtrackerGroupActionDestTypeVp) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else if (redirect_action == bcmFlowtrackerGroupActionRedirectIpmc) {
                if(dest_type != bcmFlowtrackerGroupActionDestTypeL3Mcast) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else if (redirect_action == bcmFlowtrackerGroupActionRedirectMcast) {
                if(dest_type != bcmFlowtrackerGroupActionDestTypeL2Mcast) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else if (redirect_action == bcmFlowtrackerGroupActionL3Switch) {
                if(dest_type != bcmFlowtrackerGroupActionDestTypeL3Egr &&
                   dest_type != bcmFlowtrackerGroupActionDestTypeEcmp) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else if (redirect_action == bcmFlowtrackerGroupActionRedirectCancel) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_validate_supported_actions(
                                int unit,
                                int num_actions,
                                bcm_flowtracker_group_action_info_t *action_list)
{
    int i;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_actions; i++) {
        /*
         * Only actions supported when eapp is involved are
         * implicit flex ctr action (not configured) and
         * em_ft_opaque_object.
         */
        if (action_list[i].action != bcmFlowtrackerGroupActionOpaqueObject) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_group_actions_validate(
                           int unit,
                           bcm_flowtracker_group_t id,
                           uint32 flags,
                           int num_actions,
                           bcm_flowtracker_group_action_info_t *action_list,
                           bcm_int_ft_flow_group_info_t *flow_group_int_info)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Chip specific validations */
    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_flowtracker_group_actions_validate(
                                                    unit,
                                                    id,
                                                    flags,
                                                    ft_info->hw_learn_en,
                                                    num_actions,
                                                    action_list));
    /*
     * If number of actions is not 0, then the
     * action list should be non NULL
     */
    if ((num_actions != 0) && (action_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flowtracker_validate_conflicting_actions(
                    unit,
                    num_actions,
                    action_list));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flowtracker_validate_supported_actions(
                    unit,
                    num_actions,
                    action_list));
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get the flow group info */
    flow_group_int_info->flow_group_id = id;
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_entry_get(unit,
                                                flow_group_int_info));

    if ((flow_group_int_info->num_actions == 0) && (num_actions == 0)) {
        /* Nothing installed, and a delete is requested num_actions == 0,
         * implies delete
         */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_group_flush(
        int unit,
        bcm_flowtracker_group_t id)
{
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    /* Get the flow group info */
    flow_group_int_info.flow_group_id = id;
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_entry_get(unit,
                                                &flow_group_int_info));

    /* Temporarily disable the group's learning */
    flow_group_int_info.enable = false;
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_entry_set(unit, true,
                                                &flow_group_int_info));

    /* Enable back the group's learning */
    flow_group_int_info.enable = true;
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_entry_set(unit, true,
                                                &flow_group_int_info));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_clear_tables(int unit)
{
    int i, num;
    const char *ltname[] = {
        MON_FLOWTRACKER_FLOW_STATICs,
        MON_FLOWTRACKER_ELEPHANT_PROFILEs,
        MON_FLOWTRACKER_EXPORT_TEMPLATEs,
        MON_FLOWTRACKER_GROUP_COLLECTOR_MAPs,
        MON_FLOWTRACKER_GROUPs,
        MON_FLOWTRACKER_CONTROLs
    };

    SHR_FUNC_ENTER(unit);

    num = COUNTOF(ltname);

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, ltname[i]));
    }
    SHR_VERBOSE_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_ft_export_interval_validate(int unit, uint32_t export_interval)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->elph_mode) {
        if (export_interval < FT_ELPH_DEF_EXPORT_INTERVAL_MSECS) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (export_interval < FT_DEF_EXPORT_INTERVAL_MSECS) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_group_collector_param_validate(int unit,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id,
        bcm_int_ft_group_col_map_t *entry,
        bcm_collector_info_t *collector_info)
{
    bcm_collector_export_profile_t export_profile_info;

    SHR_FUNC_ENTER(unit);

    /* Get collector info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_get(unit, collector_id, collector_info));

    /* Get export profile info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_export_profile_get(unit, export_profile_id,
                                               &export_profile_info));

    /* Verify if template exists */
    if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
        if (!bcm_ltsw_flowtracker_list_id_present(unit, export_template_pool,
                                                  template_id)) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Template (ID:%d) does not exist.\n"),
                     unit, template_id));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
        if (bcmi_ltsw_collector_check_user_is_other(unit, collector_id,
                LTSW_COLLECTOR_EXPORT_APP_FT_IPFIX)) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Collector (ID:%d) is associated to another wire format.\n"),
                     unit, collector_id));
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    } else {
        if (bcmi_ltsw_collector_check_user_is_other(unit, collector_id,
                LTSW_COLLECTOR_EXPORT_APP_FT_PROTOBUF)) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Collector (ID:%d) is associated to another wire format.\n"),
                     unit, collector_id));
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }
    /* 0 is a special case in API. Do not validate. */
    if (export_profile_info.export_interval != 0) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_ft_export_interval_validate(unit,
                export_profile_info.export_interval));
    }

    if (collector_info->transport_type != bcmCollectorTransportTypeIpv4Udp &&
        collector_info->transport_type != bcmCollectorTransportTypeIpv6Udp) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_ft_elephant_filter_validate(int unit,
        uint8_t demotion,
        bcm_flowtracker_elephant_filter_t *filter)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if (filter->monitor_interval_usecs == 0) {
        /* No filter configured, add default filter if it is demotion,
         * promotion filters are optional
         */
        if (demotion) {
            filter->monitor_interval_usecs = ft_info->scan_interval_usecs;
            COMPILER_64_SET(filter->size_threshold_bytes, 0, 1);
        }
        SHR_EXIT();
    }

    if ((filter->monitor_interval_usecs % (ft_info->scan_interval_usecs)) != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((filter->rate_low_threshold_kbits_sec %
                FT_ELPH_RATE_THRESHOLD_STEP) != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((filter->rate_high_threshold_kbits_sec %
                FT_ELPH_RATE_THRESHOLD_STEP) != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_template_validate_convert(int unit,
        uint16 set_id,
        int num_elements,
        bcm_flowtracker_export_element_info_t *elements,
        bcm_int_ft_export_template_info_t *template)
{
    int i, j;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(template, 0, sizeof(bcm_int_ft_export_template_info_t));

    /* Validate set id. Should be between 256 and 65535 */
    if (!FT_TEMPLATE_SET_ID_VALID(set_id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    template->set_id = set_id;

    if (num_elements > FT_TEMPLATE_MAX_INFO_ELEMENTS) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    template->num_export_elements = num_elements;

    for (i = 0; i < num_elements; i++) {
        for (j = 0; j < FT_TEMPLATE_MAX_INFO_ELEMENTS; j++) {
            if (elements[i].element == ft_template_element_mapping[j].element) {
                if ((elements[i].data_size != 0) &&
                        (elements[i].data_size !=
                         ft_template_element_mapping[j].data_size)) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }

                if (elements[i].element ==
                        bcmFlowtrackerExportElementTypeFlowStartTimeMsecs) {
                    if (!ft_info->flow_start_ts) {
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                }

                template->elements[i].element =
                    ft_template_element_mapping[j].int_element;
                template->elements[i].data_size =
                    ft_template_element_mapping[j].data_size;
                template->elements[i].id =
                    elements[i].info_elem_id;
                if (elements[i].flags &
                        BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_ENTERPRISE) {
                    template->elements[i].enterprise = 1;
                }
                break;
            }
        }
        if (j >= FT_TEMPLATE_MAX_INFO_ELEMENTS) {
            /* No match found */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_ft_flexctr_pkt_byte_reserve(uint32_t unit, bcm_int_ft_info_t *ft_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_flowtracker_flex_ctr_pkt_byte_reserve(
                unit,
                ft_info->max_flows[0],
                ft_info->hw_learn_en,
                &(ft_info->flex_ctr_stat_id),
                &(ft_info->flex_counter_id)));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_ft_flexctr_hitbit_reserve(uint32_t unit, bcm_int_ft_info_t *ft_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_flowtracker_flex_ctr_hit_bit_reserve(
                unit,
                FT_HITBIT_TYPE_2x,
                ft_info->max_flows[0],
                ft_info->hw_learn_en,
                &(ft_info->db_wide_hitbit_flex_ctr_stat_id),
                &(ft_info->db_wide_flex_counter_id)));

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_flowtracker_flex_ctr_hit_bit_reserve(
                unit,
                FT_HITBIT_TYPE_4x,
                ft_info->max_flows[0],
                ft_info->hw_learn_en,
                &(ft_info->qd_wide_hitbit_flex_ctr_stat_id),
                &(ft_info->qd_wide_flex_counter_id)));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_ft_flexctr_group_actions(uint32_t unit, bcm_int_ft_info_t *ft_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_flowtracker_flex_ctr_action_group(
                unit,
                ft_info->flex_ctr_stat_id,
                ft_info->db_wide_hitbit_flex_ctr_stat_id,
                &(ft_info->db_wide_hitbit_flex_ctr_grp_act_stat_id),
                &(ft_info->db_wide_flex_counter_grp_act_hw_idx)));

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_flowtracker_flex_ctr_action_group(
                unit,
                ft_info->flex_ctr_stat_id,
                ft_info->qd_wide_hitbit_flex_ctr_stat_id,
                &(ft_info->qd_wide_hitbit_flex_ctr_grp_act_stat_id),
                &(ft_info->qd_wide_flex_counter_grp_act_hw_idx)));
exit:
    SHR_FUNC_EXIT();
}

static bool bcm_ltsw_flowtracker_enable_prop_get(int unit) {

    uint32_t enable = 0;
    uint32_t hw_learn_en = 0;

    enable = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_ENABLE,
            0);
    hw_learn_en   = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_HARDWARE_LEARN_ENABLE,
            0);

    switch (enable) {
        case 0:
            return false;

        case 1:
            if (hw_learn_en == 2) {
                /*
                 * Dont care about flowtracker enable = 2
                 * if hw learn is without app
                 */
                return true;
            } else {
                LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                                "FT(unit %d) Error: flowtracker_enable value = 1 is not supported\n"),
                            unit));
                return false;
            }

        case 2:
            return true;

        default:
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                            "FT(unit %d) Error: flowtracker_enable value = %d is not supported\n"),
                        unit, enable));
            return false;
    }
}
static int
bcm_ltsw_flowtracker_info_init(int unit, bcm_int_ft_info_t *ft_info)
{
    int pipe = 0;
    uint32_t max_flows = 0;
    bool supported = false;

    SHR_FUNC_ENTER(unit);

    ft_info->num_pipes = bcmi_ltsw_dev_max_pp_pipe_num(unit);

    ft_info->enable = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_ENABLE,
            0);

    ft_info->elph_mode = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_ELEPHANT_ENABLE,
            0);

    ft_info->scan_interval_usecs = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_SCAN_INTERVAL_USECS,
            FT_MAX_SCAN_INTERVAL_USECS);

    ft_info->max_flow_groups = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_MAX_FLOW_GROUPS,
            FT_MAX_FLOW_GROUPS - 1);

    ft_info->max_export_pkt_length = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_MAX_EXPORT_PKT_LENGTH,
            1500);

    ft_info->flow_start_ts = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_FLOW_START_TIMESTAMP_IE_ENABLE,
            0);

    ft_info->hw_learn_en   = bcmi_ltsw_property_get(unit,
            BCMI_CPN_FLOWTRACKER_HARDWARE_LEARN_ENABLE,
            0);
    switch(ft_info->hw_learn_en) {
        case 0:
        case 1:
            /* Enable host memory when embedded app is used */
            ft_info->host_mem = 1;
            break;
        case 2:
            /* Host memory is not required when embedded app is not used */
            ft_info->host_mem = 0;
            break;
    }

    ft_info->uft_mode      = bcmi_ltsw_property_get(unit, BCMI_CPN_UFT_MODE, 0);

    if (ft_info->hw_learn_en) {

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flowtracker_hw_learn_is_supported_uft_mode(
                                                        unit,
                                                        ft_info->uft_mode,
                                                        &supported));
        if (supported == false) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flowtracker_hw_learn_max_flows_per_pipe_get(unit,
                                                            ft_info->uft_mode,
                                                            &max_flows));
        for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
            ft_info->max_flows[pipe] = max_flows;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flowtracker_sw_learn_is_supported_uft_mode(
                                                        unit,
                                                        ft_info->uft_mode,
                                                        &supported));
        if (supported == false) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flowtracker_sw_learn_max_flows_per_pipe_get(unit,
                                                                &max_flows));
        for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
            ft_info->max_flows[pipe] =  max_flows;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
/*
 * Function:
 *      bcm_ltsw_flowtracker_init_common
 * Purpose:
 *      Flowtracker initialization common to all devices.
 * Parameters:
 *      unit              - (IN)     BCM device number
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_init_common(int unit)
{
    int warmboot = false;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint32_t alloc_size = 0, rv = SHR_E_NONE;
    bool ft_enable = false;

    SHR_FUNC_ENTER(unit);

    ft_enable = bcm_ltsw_flowtracker_enable_prop_get(unit);

    if (ft_enable == false) {
        SHR_EXIT();
    }

    warmboot = bcmi_warmboot_get(unit);
    if (ft_info != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_detach_common(unit));
    }

    /* Alloc global info */
    if (ft_info == NULL) {
        const char *blk_ids[] = FT_HA_SUB_COMP_ID_STRINGS;
        alloc_size = sizeof(bcm_int_ft_info_t);
        ft_glb_info[unit] = bcmi_ltsw_ha_mem_alloc(unit,
                BCMI_HA_COMP_ID_FLOWTRACKER,
                FT_SUB_COMP_ID_GLOBAL,
                blk_ids[FT_SUB_COMP_ID_GLOBAL],
                &alloc_size);
        ft_info = ft_glb_info[unit];
        SHR_NULL_CHECK(ft_info, SHR_E_MEMORY);
        if (!warmboot) {
            sal_memset(ft_info, 0, alloc_size);
        }
    }


    if (!warmboot) {
        /* If it is coldboot, initialize via LT */
        rv = bcm_ltsw_flowtracker_info_init(unit, ft_info);

        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_detach_common(unit));
            SHR_EXIT();
        }

        if (ft_info->hw_learn_en != FT_HW_LEARN_ENABLE_WO_EAPP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_ft_flexctr_pkt_byte_reserve(unit, ft_info));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_ft_flexctr_hitbit_reserve(unit, ft_info));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_ft_flexctr_group_actions(unit, ft_info));
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_control_entry_set(unit, false, ft_info));
        if (ft_info->hw_learn_en) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flowtracker_hw_learn_global_enable_set(unit, true));
        }
    }

    /* Get and check if app got initialized */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_control_entry_get(unit, ft_info));

    if ((warmboot) && (ft_info->host_mem)) {
        /* WB is not supported if host mem is enabled */
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
          "FT(unit %d) Warning: Flowtracker app using host memory.\
            Warmboot not supported.\n"),
                 unit));
        SHR_EXIT();
    }

    if (!ft_info->init) {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
          "FT(unit %d) Warning: Flowtracker app not initialized.\n"),
                 unit));
        SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_detach_common(unit));
        SHR_EXIT();
    }

    /* Initialize the index lists */
    SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_init_index_lists(unit, warmboot));

exit:
    if (SHR_FUNC_ERR() && (ft_info)) {
        if (!warmboot) {
            (void)bcmi_ltsw_ha_mem_free(unit, ft_info);
            ft_info = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_detach_common
 * Purpose:
 *      Flowtracker de-initialization common to all devices.
 * Parameters:
 *      unit              - (IN)     BCM device number
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_detach_common(int unit)
{
    int warmboot = false;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint8_t hw_learn_en = 0;
    SHR_FUNC_ENTER(unit);

    warmboot = bcmi_warmboot_get(unit);

    if (ft_info == NULL) {
        /*
         * Module not initialized.
         * Return E_NONE since this can happen during boot.
         */
        SHR_EXIT();
    }

    if ((warmboot) && (ft_info->host_mem)) {
        /* Warmboot is not supported if app uses host memory */
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    hw_learn_en = ft_info->hw_learn_en;

    if (!warmboot) {

        if (ft_info->init == true) {
            ft_info->enable = false;
            /* If it is not warmboot, de-init via LT */
            SHR_IF_ERR_VERBOSE_EXIT(
                    bcm_ltsw_flowtracker_control_entry_set(unit, true, ft_info));
            ft_info->init = false;
        }

        /* Free up the lists */
        bcm_ltsw_flowtracker_free_index_lists(unit);

        /*
         * Free up flex counters.
         * Destroy the group actions before individual flex counter actions
         */
        if (ft_info->db_wide_hitbit_flex_ctr_grp_act_stat_id) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flexctr_group_action_destroy(
                unit,
                ft_info->db_wide_hitbit_flex_ctr_grp_act_stat_id));
        }
        if (ft_info->qd_wide_hitbit_flex_ctr_grp_act_stat_id) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flexctr_group_action_destroy(
                unit,
                ft_info->qd_wide_hitbit_flex_ctr_grp_act_stat_id));
        }
        if (ft_info->flex_ctr_stat_id) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flexctr_action_destroy(unit, ft_info->flex_ctr_stat_id));
        }
        if (ft_info->db_wide_hitbit_flex_ctr_stat_id) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flexctr_action_destroy(
                unit,
                ft_info->db_wide_hitbit_flex_ctr_stat_id));
        }
        if (ft_info->qd_wide_hitbit_flex_ctr_stat_id) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flexctr_action_destroy(
                unit,
                ft_info->qd_wide_hitbit_flex_ctr_stat_id));
        }

        /* Free global info */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmi_ltsw_ha_mem_free(unit, ft_glb_info[unit]));
        ft_glb_info[unit] = NULL;

        /* Clear all LTs */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flowtracker_clear_tables(unit));
        if (hw_learn_en) {
            SHR_IF_ERR_VERBOSE_EXIT(
                    bcm_ltsw_flowtracker_hw_learn_global_enable_set(unit, false));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_group_create_common
 * Purpose:
 *      Create flowtracker group
 * Parameters:
 *      unit              - (IN)     BCM device number
 *      options           - (IN)     Group creation options
 *      flow_group_id     - (IN/OUT) Flowtracker group ID
 *      flow_group_info   - (IN)     Flowtracker group information
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_create_common(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t *flow_group_id,
        bcm_flowtracker_group_info_t *flow_group_info)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i, rv = SHR_E_NONE;
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    bool id_alloced = false, flow_group_added = false;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Validate the parameters and reserve/alloc the GROUP ID */
    SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_group_create_validate_and_alloc_id(unit,
                options,
                flow_group_id,
                flow_group_info,
                &id_alloced));

    /* Observation domain Id is a global parameter. Setting it for one group
     * changes it for all groups
     */
    ft_info->observation_domain_id = flow_group_info->observation_domain_id;

    /* Insert into MON_FLOWTRACKER_GROUP LT.
     * Only thing that can be replaced using flow group replace API is
     * observation domain which is updated in MON_FLOWTRACKER_CONTROL LT.
     * So we need not update anything in MON_FLOWTRACKER_GROUP LT.
     */
    if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
        flow_group_int_info.flow_group_id = *flow_group_id;
        for (i = 0; i < ft_info->num_pipes; ++i) {
            flow_group_int_info.field_group[i] =
                                flow_group_info->field_group[i];
        }
        flow_group_int_info.enable = true;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_group_entry_set(unit, false,
                                                 &flow_group_int_info));
        flow_group_added = true;
        /* Do a get to see if it has failed */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));
    }

    if (ft_info->hw_learn_en != FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_flowtracker_control_entry_set(unit, true, ft_info));
    }

exit:
    if (SHR_FAILURE(rv)) {
        /* Free up the flow_group id allocated */
        if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
            if (id_alloced == true) {
                (void)bcm_ltsw_flowtracker_list_id_clr(unit, flow_group_pool,
                            *flow_group_id);
            }
            if (flow_group_added == true) {
               (void)bcm_ltsw_flowtracker_group_entry_delete(unit,
                                                    *flow_group_id);
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_destroy_common
 * Purpose:
 *      Destroy a flowtracker flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_destroy_common(
        int unit,
        bcm_flowtracker_group_t id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_delete_validate(unit, id));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_delete(unit, id));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_list_id_clr(unit,
                                         flow_group_pool,
                                         id));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_clear_common
 * Purpose:
 *      Clear a flow group's flow entries.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      flags         - (IN)  Clear params
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_clear_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flags)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (flags == BCM_FLOWTRACKER_GROUP_CLEAR_ALL) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_group_flush(unit, id));
    } else if (flags == BCM_FLOWTRACKER_GROUP_CLEAR_FLOW_DATA_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_group_flow_data_clear_all(unit, id));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_template_transmit_config_set_common(
        int unit,
        bcm_flowtracker_export_template_t template_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_template_transmit_config_t *config)
{
    bcm_int_ft_template_transmit_config_t export_template;
    bcm_collector_info_t collector_info;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_ltsw_collector_transport_type_t coll_type;
    uint32_t collector_int_id;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    export_template.export_template_id = template_id;

    /* Check if export template is configured */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_template_transmit_config_entry_get(unit, &export_template));

    /* Check if collector is configured */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_get(unit, collector_id, &collector_info));

    /* Update export template LT to start transmit*/
    export_template.transmit_interval = config->retransmit_interval_secs;
    export_template.initial_burst = config->initial_burst;
    export_template.collector_type = collector_info.transport_type;

    /* Get the collector internal ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_internal_id_get(unit, collector_id,
                                             &collector_int_id, &coll_type));
    if (collector_info.transport_type ==
        bcmCollectorTransportTypeIpv4Udp) {
        export_template.collector_ipv4_id = collector_int_id;
    } else if (collector_info.transport_type ==
        bcmCollectorTransportTypeIpv6Udp) {
        export_template.collector_ipv6_id =  collector_int_id;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_export_template_entry_update(unit,
            &export_template));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_get_common
 * Purpose:
 *      Get flowtracker flow group information
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      id                - (IN)  Group Id
 *      flow_group_info   - (OUT) Group Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_info_t *flow_group_info)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    int i;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(flow_group_info, SHR_E_PARAM);

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = id;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    flow_group_info->observation_domain_id = ft_info->observation_domain_id;
    for (i = 0; i < ft_info->num_pipes; i++) {
        flow_group_info->field_group[i] = flow_group_int_info.field_group[i];
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_template_transmit_config_get_common(
    int unit,
    bcm_flowtracker_export_template_t template_id,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_template_transmit_config_t *config)
{

    bcm_int_ft_template_transmit_config_t transmit_config;
    bcm_collector_info_t collector_info;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    transmit_config.export_template_id = template_id;

    /* Check if export template is configured */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_template_transmit_config_entry_get(unit,
            &transmit_config));

    /* Check if collector is configured */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_get(unit, collector_id, &collector_info));

    config->retransmit_interval_secs = transmit_config.transmit_interval;
    config->initial_burst = transmit_config.initial_burst;
exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_group_elephant_profile_attach_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_elephant_profile_t profile_id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if elephant profile ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, elph_profile_pool, profile_id)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Profile (ID:%d) does not exist.\n"),
                 unit, profile_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check if group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool, flow_group_id)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                 unit, flow_group_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Update group with elephant profile id */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    flow_group_int_info.elph_profile_id = profile_id;
    flow_group_int_info.flow_group_id = flow_group_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_entry_set(unit, true, &flow_group_int_info));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_get_all_common
 * Purpose:
 *      Get list of all flow groups created
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      max_size          - (IN)  Size of the flow group list array
 *      flow_group_list   - (OUT) List of flow groups created
 *      list_size         - (OUT) Number of flow grups in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_get_all_common(
        int unit,
        int max_size,
        bcm_flowtracker_group_t *flow_group_list,
        int *list_size)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int group;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (max_size != 0 && flow_group_list == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *list_size = 0;
    for (group = 1; group < FT_MAX_FLOW_GROUPS; group++) {
        if (bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool, group)) {
            if (max_size != 0) {
                if (max_size > (*list_size)) {
                    flow_group_list[*list_size] = group;
                }
            }
            (*list_size)++;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_tracking_params_get_common
 * Purpose:
 *      Get tracking parameter for this flowtracker group
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Flow group Id
 *      max_size                - (IN)  Maximum number of tracking params
 *                                      that the array can hold
 *      list_of_tracking_params - (OUT) Array of tracking param info
 *      list_size               - (OUT) Actual number of tracking params filled
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_tracking_params_get_common(
    int unit,
    bcm_flowtracker_group_t id,
    int max_size,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params,
    int *list_size)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i;
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    int temp_num_tp = 0;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* If the max_actions is non zero and list == NULL, return
     * SHR_E_PARAM.
     */
    if (max_size != 0 && list_of_tracking_params == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if group exists */
    if (!(bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                               id))) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                unit, id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = id;
    /* Get the MON_FLOWTRACKER_GROUP entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    if (max_size == 0) {
        *list_size = flow_group_int_info.num_tracking_params;
    } else {
        for (i = 0; i < flow_group_int_info.num_tracking_params; i++) {
            if (i >= max_size) {
                break;
            }
            list_of_tracking_params[i].param =
                flow_group_int_info.tracking_params[i].param;
             list_of_tracking_params[i].udf_id =
                flow_group_int_info.tracking_params[i].udf_id;
            temp_num_tp++;
        }
        *list_size = temp_num_tp;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_group_tracking_params_set_common
 * Purpose:
 *      Set tracking parameters on the flowtracker group
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flowtracker group ID
 *      num_tracking_params     - (IN)  Number of tracking params
 *      list_of_tracking_params - (IN)  Array of tracking param info
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int
bcm_ltsw_flowtracker_group_tracking_params_set_common(
                                int unit,
                                bcm_flowtracker_group_t id,
                                int num_tracking_params,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i;
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Validate the params and get the current LT info */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_tracking_params_validate(
                                                    unit,
                                                    id,
                                                    num_tracking_params,
                                                    list_of_tracking_params,
                                                    &flow_group_int_info));

    /* Update the tracking params info */
    flow_group_int_info.num_tracking_params = num_tracking_params;
    for (i = 0; i < num_tracking_params; i++) {
        flow_group_int_info.tracking_params[i].param =
                                        list_of_tracking_params[i].param;
        flow_group_int_info.tracking_params[i].udf_id =
                                        list_of_tracking_params[i].udf_id;
    }

    /* Update the MON_FLOWTRACKER_GROUP entry with tracking params */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_set(unit, true, &flow_group_int_info));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_group_elephant_profile_attach_get_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_elephant_profile_t *profile_id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if group exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool, flow_group_id)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                 unit, flow_group_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* get elephant profile id from group info */
    flow_group_int_info.flow_group_id = flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    if (flow_group_int_info.elph_profile_id == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *profile_id =  flow_group_int_info.elph_profile_id;
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_export_trigger_get_common
 * Purpose:
 *  Get export trigger information of the flow group with ID
 *
 * Parameters:
 *      unit                - (IN)   BCM device number
 *      id                  - (IN)   Flow group Id
 *      export_trigger_info - (OUT)  Export trigger information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_export_trigger_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    bcm_flowtracker_export_trigger_t trigger;
    int i;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (export_trigger_info == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if group exists */
    if (!(bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                               id))) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                unit, id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = id;
    /* Get the MON_FLOWTRACKER_GROUP entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    BCM_FLOWTRACKER_TRIGGER_CLEAR_ALL(*export_trigger_info);

    for (i = 0; i < flow_group_int_info.num_export_triggers; i++) {
        trigger = flow_group_int_info.export_triggers[i];
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info, trigger);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_export_trigger_set_common
 * Purpose:
 *  Set export trigger information of the flow group with ID
 *
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      id                  - (IN)  Flow group Id
 *      export_trigger_info - (IN)  Export trigger information
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_export_trigger_set_common(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Validate the triggers and get the current LT info */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_export_triggers_validate(
                                                    unit,
                                                    id,
                                                    export_trigger_info,
                                                    &flow_group_int_info));

    /* Update the export triggers info */
    bcm_ltsw_flowtracker_group_export_triggers_info_update(
                                                    unit,
                                                    id,
                                                    export_trigger_info,
                                                    &flow_group_int_info);

    /* Update the MON_FLOWTRACKER_GROUP entry with export triggers */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_set(unit, true, &flow_group_int_info));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_group_elephant_profile_detach_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if group exists */
    if (!(bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,flow_group_id))) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                 unit, flow_group_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* get elephant profile id from group info */
    flow_group_int_info.flow_group_id = flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    /* Profile not attached */
    if (flow_group_int_info.elph_profile_id == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Flow limit has to be set to 0 before detaching elephant profile */
    if (flow_group_int_info.flow_limit != 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    flow_group_int_info.elph_profile_id = 0;

    /* Update elephant profile id to zero */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_entry_set(unit, true, &flow_group_int_info));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_ft_flow_group_flow_limit_get_common
 * Purpose:
 *      Get flow limit of the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (OUT) Flow limit
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_limit_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_limit)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check if group exists */
    if (!(bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                               id))) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                unit, id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = id;
    /* Get the MON_FLOWTRACKER_GROUP entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    /* Update the flow limit value */
    *flow_limit = flow_group_int_info.flow_limit;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_flow_limit_set_common
 * Purpose:
 *      Set flow limit on the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (IN) Max number of flows that can be learnt on the group
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_limit_set_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flow_limit)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Validate the flow limit and get the current LT info */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_flow_limit_validate(
                                                    unit,
                                                    id,
                                                    flow_limit,
                                                    &flow_group_int_info));

    /* Update the flow limit value */
    flow_group_int_info.flow_limit = flow_limit;


    /* Update the MON_FLOWTRACKER_GROUP entry with flow limit */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_set(unit, true, &flow_group_int_info));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcm_ltsw_flowtracker_group_age_timer_get_common
 * Purpose:
 *      Get aging timer interval in ms set on the flow group.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      id                  - (IN)  Flow group Id
 *      aging_interval_ms   - (OUT) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_age_timer_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *aging_interval_ms)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if group exists */
    if (!(bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                               id))) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                unit, id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = id;
    /* Get the MON_FLOWTRACKER_GROUP entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    /* Update the age timer value */
    *aging_interval_ms = flow_group_int_info.aging_interval_msecs;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_age_timer_set_common
 * Purpose:
 *      Set aging timer interval in ms on the flow group. Aging interval has to
 *      be configured in steps of 1sec with a minimum of 1sec. Default value of
 *      1 minute.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      aging_interval_ms   - (IN) Aging interval in msec
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_age_timer_set_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 aging_interval_ms)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Validate the age timer and get the current LT info */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_age_timer_validate(
                                                    unit,
                                                    id,
                                                    aging_interval_ms,
                                                    &flow_group_int_info));

    /* Update the age timer value */
    flow_group_int_info.aging_interval_msecs = aging_interval_ms;


    /* Update the MON_FLOWTRACKER_GROUP entry with age timer */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_set(unit, true, &flow_group_int_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_collector_attach_common(int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_group_col_map_t entry;
    bcm_collector_export_profile_t export_profile_info;
    bcm_collector_info_t collector_info;
    bcm_ltsw_collector_transport_type_t coll_type;
    uint32_t collector_int_id;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if group exists */
    if (!(bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool, flow_group_id))) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                 unit, flow_group_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Traverse through MON_FLOWTRACKER_GROUP_COLLECTOR_MAP
       and check if collector is already attached*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_is_collector_attached(unit, flow_group_id));

    /* Validate and populate relevant fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_collector_param_validate(unit,
                collector_id, export_profile_id, template_id, &entry,
                &collector_info));

    entry.group_col_map_id = flow_group_id;
    entry.ft_group_id = flow_group_id;
    entry.collector_type = collector_info.transport_type;

    /* Get the collector internal ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_internal_id_get(unit, collector_id,
                                             &collector_int_id, &coll_type));

    if (collector_info.transport_type ==
            bcmCollectorTransportTypeIpv4Udp) {
        entry.collector_ipv4_id = collector_int_id;
    } else if (collector_info.transport_type ==
            bcmCollectorTransportTypeIpv6Udp) {
        entry.collector_ipv6_id =  collector_int_id;
    }

    entry.export_template_id = template_id;
    entry.export_profile_id = export_profile_id;

    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_group_collector_map_entry_set(unit, false, &entry));

    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_group_collector_map_entry_get(unit, &entry));

    SHR_IF_ERR_EXIT(bcmi_ltsw_collector_ref_count_update(unit, collector_id, 1));

    SHR_IF_ERR_EXIT(bcmi_ltsw_collector_export_profile_ref_count_update(unit, export_profile_id, 1));

    /* Get export profile info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_export_profile_get(unit, export_profile_id, &export_profile_info));


    if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
        SHR_IF_ERR_EXIT(bcmi_ltsw_collector_user_update(unit,
                    collector_id, LTSW_COLLECTOR_EXPORT_APP_FT_IPFIX));
    } else {
        SHR_IF_ERR_EXIT(bcmi_ltsw_collector_user_update(unit,
                    collector_id, LTSW_COLLECTOR_EXPORT_APP_FT_PROTOBUF));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_actions_get_common
 * Purpose:
 *      Get list of actions applied on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      max_actions   - (IN) Maximum number of actions that can be
 *                           accomodated in the list.
 *      action_list   - (OUT) Action list.
 *      num_actions   - (OUT) Actual number of actions in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_actions_get_common(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int max_actions,
                               bcm_flowtracker_group_action_info_t *action_list,
                               int *num_actions)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i;
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    int temp_num_actions = 0;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* If the max_actions is non zero and list == NULL, return
     * SHR_E_PARAM.
     */
    if (max_actions != 0 && action_list == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if group exists */
    if (!(bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                               flow_group_id))) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                unit, flow_group_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = flow_group_id;
    /* Get the MON_FLOWTRACKER_GROUP entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));


    /* Get the actions value */
    if (max_actions == 0) {
        *num_actions = flow_group_int_info.num_actions;
    } else {
        for (i = 0; i < flow_group_int_info.num_actions; i++) {
            if (i >= max_actions) {
                break;
            }
            action_list[i].action =
                flow_group_int_info.action_list[i].action;
            action_list[i].params.param0 =
                flow_group_int_info.action_list[i].params[0];
            action_list[i].params.param1 =
                flow_group_int_info.action_list[i].params[1];
            temp_num_actions++;
        }
        *num_actions = temp_num_actions;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_actions_set_common
 * Purpose:
 *      Set list of actions on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      id            - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      num_actions   - (IN) Number of actions in the list.
 *      action_list   - (IN) Action list.
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_actions_set_common(
                               int unit,
                               bcm_flowtracker_group_t id,
                               uint32 flags,
                               int num_actions,
                               bcm_flowtracker_group_action_info_t *action_list)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i;
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Validate the actions and get the current LT info */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_actions_validate(
                                                    unit,
                                                    id,
                                                    flags,
                                                    num_actions,
                                                    action_list,
                                                    &flow_group_int_info));

    /* Update the actions value */
    flow_group_int_info.num_actions = num_actions;
    for (i = 0; i < num_actions; i++) {
        flow_group_int_info.action_list[i].action    =
                                    action_list[i].action;
        flow_group_int_info.action_list[i].params[0] =
                                    action_list[i].params.param0;
        flow_group_int_info.action_list[i].params[1] =
                                    action_list[i].params.param1;
    }


    /* Update the MON_FLOWTRACKER_GROUP entry with actions */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_set(unit, true, &flow_group_int_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_collector_detach_common(int unit,
                                  bcm_flowtracker_group_t flow_group_id,
                                  bcm_collector_t collector_id,
                                  int export_profile_id,
                                  bcm_flowtracker_export_template_t template_id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_collector_export_profile_t export_profile_info;
    bcm_collector_info_t collector_info;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if group exists */
    if (!(bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool, flow_group_id))) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                 unit, flow_group_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get collector info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_get(unit, collector_id, &collector_info));

    /* Get export profile info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_export_profile_get(unit, export_profile_id, &export_profile_info));

    /* Verify if template exists */
    if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
        if (!bcm_ltsw_flowtracker_list_id_present(unit, export_template_pool, template_id)) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Template (ID:%d) does not exist.\n"),
                     unit, template_id));
            return (SHR_E_NOT_FOUND);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_collector_map_delete(unit, flow_group_id));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_flow_count_get_common
 * Purpose:
 *      Get the number of flows learnt in the flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 *      flow_count       - (OUT) Number of flows learnt
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_count_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_count)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = id;
    /* Get the flow group status */
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_flowtracker_group_status_get(unit,
                                                &flow_group_int_info));
    *flow_count = flow_group_int_info.flow_count;
exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_elephant_profile_create_common(
        int unit,
        uint32_t options,
        bcm_flowtracker_elephant_profile_info_t *profile,
        bcm_flowtracker_elephant_profile_t *profile_id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int id = 0, i;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false ||
        ft_info->elph_mode != TRUE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Validate the profile */
    if (profile->num_promotion_filters > FT_ELEPHANT_MAX_PROMOTION_FILTERS) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    for (i = 0; i < profile->num_promotion_filters; i++) {
        SHR_IF_ERR_EXIT(
                bcm_ltsw_ft_elephant_filter_validate(unit, 0,
                    &(profile->promotion_filters[i])));
    }
    SHR_IF_ERR_EXIT(
            bcm_ltsw_ft_elephant_filter_validate(unit, 1,
                &(profile->demotion_filter)));

    if (options & BCM_FLOWTRACKER_ELEPHANT_PROFILE_WITH_ID) {
        if (*profile_id > FT_MAX_ELEPHANT_PROFILES+1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* Reserve the requested Id */
        rv = bcm_ltsw_flowtracker_list_id_reserve(unit, elph_profile_pool,
                                                  *profile_id);
        id = *profile_id;
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT ((rv == SHR_E_RESOURCE) ? SHR_E_EXISTS : rv);
        }
    } else {
        /* Allocate a free ID */

        SHR_IF_ERR_EXIT
            (bcm_ltsw_flowtracker_list_id_alloc(unit,
                                                elph_profile_pool,FT_MAX_ELEPHANT_PROFILES+1, &id));
        *profile_id = id;
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_elephant_profile_entry_set(unit, false, &id, options, profile));

exit:
    if (SHR_FAILURE(_func_rv)) {
        (void)bcm_ltsw_flowtracker_list_id_clr(unit, elph_profile_pool, id);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_group_data_get_common
 * Purpose:
 *      Get flow data for a given flow key within the given flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      flow_key      - (IN)  Five tuple that constitutes a flow
 *      flow_data     - (OUT) Data associated with the flow key
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_data_get_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_flow_key_t *flow_key,
        bcm_flowtracker_flow_data_t *flow_data)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              flow_group_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_flow_data_get(unit,
                                                 flow_group_id,
                                                 flow_key,
                                                 flow_data));
exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_elephant_profile_destroy_common(int unit,
        bcm_flowtracker_elephant_profile_t profile_id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bool is_used = false;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_is_elephant_profile_used(unit, profile_id,
         &is_used));

    if (is_used == true) {
        /* One cannot delete a profile without deleting the
         * association of the profile to the flow group.
         */
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    (void)bcm_ltsw_flowtracker_list_id_clr(unit, elph_profile_pool,
            profile_id);

    (void)bcm_ltsw_flowtracker_elephant_profile_entry_delete(unit,
            profile_id);

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_elephant_profile_get_common(
        int unit,
        bcm_flowtracker_elephant_profile_t profile_id,
        bcm_flowtracker_elephant_profile_info_t *profile)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if(!bcm_ltsw_flowtracker_list_id_present(unit, elph_profile_pool, profile_id)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Elephant profile (ID:%d) does not exist.\n"),
                 unit, profile_id));
        SHR_ERR_EXIT (SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_elephant_profile_entry_get(unit, profile_id,
                                                         profile));
exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_elephant_profile_get_all_common(
        int unit,
        int max,
        bcm_flowtracker_elephant_profile_t *profile_list,
        int *count)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int num_profiles, i;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max != 0 && profile_list == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    num_profiles = 0;
    *count       = 0;

    for (i = 1; i <= FT_MAX_ELEPHANT_PROFILES; i++) {
        if (bcm_ltsw_flowtracker_list_id_present(unit, elph_profile_pool, i)) {
            /* Profile exists */
            num_profiles++;
            if (*count < max) {
                profile_list[*count] = i;
                (*count)++;
            }
        }
    }

    if (max == 0) {
        /* Special case, just return the number of profiles */
        *count = num_profiles;
        SHR_EXIT();
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_export_template_create_common(
        int unit,
        uint32 options,
        bcm_flowtracker_export_template_t *id,
        uint16 set_id,
        int num_export_elements,
        bcm_flowtracker_export_element_info_t *list_of_export_elements)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_export_template_info_t template_int_info;
    int rv = SHR_E_NONE;
    int profile_id = 0;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if template is supported templates and convert to internal
     * representation.
     */
    rv = bcm_ltsw_template_validate_convert(unit,
            set_id,
            num_export_elements,
            list_of_export_elements,
            &template_int_info);
    SHR_IF_ERR_EXIT(rv);

    if (options & BCM_FLOWTRACKER_EXPORT_TEMPLATE_WITH_ID) {
        /* Reserve the requested Id */
        rv = bcm_ltsw_flowtracker_list_id_reserve(unit, export_template_pool,
                *id);
        profile_id = *id;
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT ((rv == SHR_E_RESOURCE) ? SHR_E_EXISTS : rv);
        }
    } else {
        /* Allocate a free ID */
        SHR_IF_ERR_EXIT
            (bcm_ltsw_flowtracker_list_id_alloc(unit,
             export_template_pool, FT_MAX_EXPORT_TEMPLATES+1, &profile_id));
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }
    }
    template_int_info.template_id = profile_id;
    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_export_template_entry_insert(unit, &template_int_info));
    *id = profile_id;

exit:
    if (SHR_FAILURE(_func_rv) && (_func_rv != SHR_E_EXISTS)) {
        (void)bcm_ltsw_flowtracker_list_id_clr(unit, export_template_pool, profile_id);
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_export_template_destroy_common(int unit,
        bcm_flowtracker_export_template_t id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bool is_used = false;
    bcm_int_ft_export_template_info_t entry;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if export template exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, export_template_pool, id)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Export template (ID:%d) does not exist.\n"),
                 unit, id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_is_export_template_used(unit, id, &is_used));

    if (is_used == true) {
        /* One cannot delete a profile without deleting the
         * association of the template to the flow group.
         */
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_export_template_entry_get(unit, id , &entry));

    if (entry.interval_secs != 0) {
        /* We are still periodically exporting this template, reject
         * the delete
         */
        SHR_ERR_EXIT (SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_export_template_entry_delete(unit , id));

    (void)bcm_ltsw_flowtracker_list_id_clr(unit, export_template_pool, id);

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_export_template_get_common(
                  int unit,
                  bcm_flowtracker_export_template_t id,
                  uint16 *set_id,
                  int max_size,
                  bcm_flowtracker_export_element_info_t *list_of_export_elements,
                  int *list_size)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_export_template_info_t template_int_info;
    int num_elements_to_copy = 0;
    int i, j;

    SHR_FUNC_ENTER(unit);
    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    sal_memset(&template_int_info, 0, sizeof(template_int_info));

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_size != 0 && list_of_export_elements == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT
        (bcm_ltsw_flowtracker_export_template_entry_get(unit, id, &template_int_info));

    *set_id = template_int_info.set_id;

    /* Else, if number of export elements less than max_size,
     * fill the list with the elements and fill list_size with
     * num_export_elements.
     */
    if (template_int_info.num_export_elements < max_size) {
        num_elements_to_copy = template_int_info.num_export_elements;
    } else {
        /* Else, if number of export elements greater than or equal to max_size,
         * fill the list with the elements upto max_size and fill list_size
         * with max_size.
         */
        num_elements_to_copy = max_size;
    }
    for (i = 0; i < num_elements_to_copy; i++) {
        /* Convert internal representation to API enum */
        for (j = 0; j < FT_TEMPLATE_MAX_INFO_ELEMENTS; j++) {
            if (template_int_info.elements[i].element ==
                    ft_template_element_mapping[j].int_element) {
                list_of_export_elements[i].element =
                    ft_template_element_mapping[j].element;
                list_of_export_elements[i].data_size =
                    ft_template_element_mapping[j].data_size;
                break;
            }
        }
        list_of_export_elements[i].info_elem_id =
            template_int_info.elements[i].id;
        if (template_int_info.elements[i].enterprise) {
            list_of_export_elements[i].flags |=
                BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_ENTERPRISE;
        }
    }
    *list_size = num_elements_to_copy;

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_group_collector_attach_get_all_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        int max_list_size,
        bcm_flowtracker_collector_t *list_of_collectors,
        bcm_flowtracker_export_template_t *list_of_templates,
        int *list_of_export_profile_ids,
        int *list_size)
{
    bcm_int_ft_group_col_map_t entry;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint32_t collector_id = FT_COLLECTOR_INVALID_COLLECTOR_ID;
    bcm_collector_t ext_id = 0;

    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool, flow_group_id)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                 unit, flow_group_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    entry.group_col_map_id = flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_group_collector_map_entry_get(unit, &entry));

    if (entry.collector_type == LTSW_COLLECTOR_TYPE_IPV4_UDP) {
        collector_id = entry.collector_ipv4_id;
    } else if (entry.collector_type == LTSW_COLLECTOR_TYPE_IPV6_UDP) {
        collector_id = entry.collector_ipv6_id;
    }

    /* Convert to external ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_external_id_get(unit, collector_id,
                                             entry.collector_type, &ext_id));

    /* max_list_size == 0, just populate list_size and return */
    if (max_list_size == 0) {
        if (collector_id == FT_COLLECTOR_INVALID_COLLECTOR_ID) {
            *list_size = 0;
        } else {
            /* Only one collector and template can be attached to a group */
            *list_size = 1;
        }
        SHR_EXIT();
    }

    /* Get the collector info */
    *list_size                    = 1;
    list_of_templates[0]          = entry.export_template_id;
    list_of_collectors[0]         = ext_id;
    list_of_export_profile_ids[0] = entry.export_profile_id;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_static_flow_add_common
 * Purpose:
 *      Add a new static flow to the flow group.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      flow_key                - (IN)  Packet attributes identifying a flow.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_static_flow_add_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_flow_key_t *flow_key)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              flow_group_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_NULL_CHECK(flow_key, SHR_E_PARAM);

    /* Insert it into LT. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_static_flow_entry_set(unit, false,
                                                         flow_group_id,
                                                         flow_key));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_static_flow_delete_common
 * Purpose:
 *      Delete an existing static flow from the flow group.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      flow_key                - (IN)  Packet attributes identifying a flow.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_static_flow_delete_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_flow_key_t *flow_key)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);
    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              flow_group_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_NULL_CHECK(flow_key, SHR_E_PARAM);

    /* Delete the entry from LT. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_static_flow_entry_set(unit, true,
                                                         flow_group_id,
                                                         flow_key));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_flowtracker_static_flow_delete_all_common
 * Purpose:
 *      Delete all existing static flows from the flow group.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_static_flow_delete_all_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);
    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              flow_group_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Delete all the entries from LT belonging to the group. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_static_flow_entry_delete_all(unit,
                                                                flow_group_id));
exit:
    SHR_FUNC_EXIT();
}
/*
 * Function:
 *      bcm_ltsw_flowtracker_static_flow_get_all_common
 * Purpose:
 *      Get all existing static flows from the flow group.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      max_size                - (IN)  Maximum number of flow keys that can
 *                                      be filled in the flow_key_arr array.
 *      flow_key_arr            - (OUT) Array of flow keys of static flows
 *                                      in the flow group
 *      list_size               - (OUT) Actual number of flow keys in the array.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_static_flow_get_all_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_size,
    bcm_flowtracker_flow_key_t *flow_key_arr,
    int *list_size)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);
    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              flow_group_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (max_size != 0 && flow_key_arr == NULL) {
        /* When max size is non zero, array should not be NULL */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get all the entries from LT belonging to the group. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_static_flow_entry_get_all(unit,
                                                             flow_group_id,
                                                             max_size,
                                                             flow_key_arr,
                                                             list_size));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_group_flow_delete_common
 * Purpose:
 *      Delete a HW learnt flow.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      action_info             - (IN)  Information for delete action.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_delete_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_group_flow_action_info_t *action_info)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);
    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en != FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              flow_group_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }


    /* Set the delete action for the given HW info. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_hw_learn_flow_action_control_set(
                unit,
                FT_HW_LEARN_FLOW_CMD_DELETE,
                flow_group_id,
                action_info,
                0 /* NA */,
                NULL /* NA */));

    /* Check the status. The API directly returns error. */
    SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_hw_learn_flow_action_state_get(
                unit,
                action_info->pipe_idx));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_flowtracker_group_flow_action_update_common
 * Purpose:
 *      Update actions of a HW learnt flow.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      action_info             - (IN)  Information for update action.
 *      num_actions             - (IN)  Number of actions to update
 *      action_list             - (IN)  List of actions to update.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_flowtracker_group_flow_action_update_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_group_flow_action_info_t *action_info,
    int num_actions,
    bcm_flowtracker_group_action_info_t *action_list)
{
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_int_ft_flow_group_info_t flow_group_int_info;
    bcm_int_ft_group_action_info_t action_list_int[FLOWTRACKER_ACTIONS_MAX];
    int i;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL || ft_info->init == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ft_info->hw_learn_en != FT_HW_LEARN_ENABLE_WO_EAPP) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if flow group ID exists */
    if (!bcm_ltsw_flowtracker_list_id_present(unit, flow_group_pool,
                                              flow_group_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    flow_group_int_info.flow_group_id = flow_group_id;
    /* Get the MON_FLOWTRACKER_GROUP entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_group_entry_get(unit, &flow_group_int_info));

    if (flow_group_int_info.num_actions != num_actions) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < num_actions; i++) {
        action_list_int[i].action    =
                                    action_list[i].action;
        action_list_int[i].params[0] =
                                    action_list[i].params.param0;
        action_list_int[i].params[1] =
                                    action_list[i].params.param1;
    }

    /* Validate if group actions and provided actions are mismatching. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_validate_actions_mismatch(
                unit,
                num_actions,
                flow_group_int_info.action_list,
                action_list_int));

    /* Set the modify action for the given HW info. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_hw_learn_flow_action_control_set(
                unit,
                FT_HW_LEARN_FLOW_CMD_MODIFY,
                flow_group_id,
                action_info,
                num_actions,
                action_list_int));

    /* Check the status. The API directly returns error. */
    SHR_IF_ERR_VERBOSE_EXIT(
            bcm_ltsw_flowtracker_hw_learn_flow_action_state_get(
                unit,
                action_info->pipe_idx));

exit:
    SHR_FUNC_EXIT();
}

