/*! \file ctr_flex_imm_cb.c
 *
 * Flex counter IMM callback functions and registration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmevm/bcmevm_api.h>

static bcmlrd_fid_t entry_fid[2] = {CTR_ING_FLEX_ENTRYt_CTR_ING_FLEX_ENTRY_IDf,
                                   CTR_EGR_FLEX_ENTRYt_CTR_EGR_FLEX_ENTRY_IDf};
static bcmlrd_fid_t attr_fid[2] = {CTR_ING_FLEX_ENTRYt_CTR_ING_FLEX_ATTR_TEMPLATE_IDf,
                                   CTR_EGR_FLEX_ENTRYt_CTR_EGR_FLEX_ATTR_TEMPLATE_IDf};
static bcmlrd_fid_t pool_fid[2] = {CTR_ING_FLEX_ENTRYt_POOL_IDf,
                                   CTR_EGR_FLEX_ENTRYt_POOL_IDf};
static bcmlrd_fid_t base_index_fid[2] = {CTR_ING_FLEX_ENTRYt_BASE_INDEXf,
                                         CTR_EGR_FLEX_ENTRYt_BASE_INDEXf};
static bcmlrd_fid_t base_index_auto_fid[2] = {CTR_ING_FLEX_ENTRYt_BASE_INDEX_AUTOf,
                                              CTR_EGR_FLEX_ENTRYt_BASE_INDEX_AUTOf};
static bcmlrd_fid_t offset_fid[2] = {CTR_ING_FLEX_ENTRYt_OFFSET_MODEf,
                                     CTR_EGR_FLEX_ENTRYt_OFFSET_MODEf};
static bcmlrd_fid_t base_index_oper_fid[2] = {CTR_ING_FLEX_ENTRYt_BASE_INDEX_OPERf,
                                              CTR_EGR_FLEX_ENTRYt_BASE_INDEX_OPERf};
static bcmlrd_fid_t pipe_unique_fid[2] = {CTR_ING_FLEX_ENTRYt_PIPE_UNIQUEf,
                                          CTR_EGR_FLEX_ENTRYt_PIPE_UNIQUEf};
static bcmlrd_fid_t pipe_num_fid[2] = {CTR_ING_FLEX_ENTRYt_PIPEf,
                                       CTR_EGR_FLEX_ENTRYt_PIPEf};
static bcmlrd_fid_t op_fid[2] = {CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                 CTR_EGR_FLEX_ENTRYt_OPERATIONAL_STATEf};
static bcmlrd_fid_t max_instances_oper_fid[2] = {CTR_ING_FLEX_ENTRYt_MAX_INSTANCES_OPERf,
                                                 CTR_EGR_FLEX_ENTRYt_MAX_INSTANCES_OPERf};

static bcmlrd_fid_t max_instances_fid[2] = {CTR_ING_FLEX_ENTRYt_MAX_INSTANCESf,
                                            CTR_EGR_FLEX_ENTRYt_MAX_INSTANCESf};
static bcmlrd_fid_t max_instances_auto_fid[2] = {CTR_ING_FLEX_ENTRYt_MAX_INSTANCES_AUTOf,
                                            CTR_EGR_FLEX_ENTRYt_MAX_INSTANCES_AUTOf};
static bcmlrd_fid_t op_state_fid[2] = {CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                       CTR_EGR_FLEX_ENTRYt_OPERATIONAL_STATEf};
/*!
 * In-memory logical table validate callback function.
 */
static int
ctr_ing_flex_pool_info_lt_validate_cb(int unit,
                bcmltd_sid_t sid,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Insert, Update and Delete not supported for read-only logical tables */
    if ((event_reason == BCMIMM_ENTRY_INSERT) ||
         (event_reason == BCMIMM_ENTRY_DELETE) ||
         (event_reason == BCMIMM_ENTRY_INSERT)) {
        rv = SHR_E_DISABLED;
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory logical table validate callback function.
 */
static int
ctr_egr_flex_pool_info_lt_validate_cb(int unit,
                bcmltd_sid_t sid,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Insert, Update and Delete not supported for read-only logical tables */
    if ((event_reason == BCMIMM_ENTRY_INSERT) ||
         (event_reason == BCMIMM_ENTRY_DELETE) ||
         (event_reason == BCMIMM_ENTRY_INSERT)) {
        rv = SHR_E_DISABLED;
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory ING PKT_ATTR_TEMPLATE stage callback function.
 */
static int
ctr_ing_flex_pkt_attr_template_lt_stage_cb(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    uint32_t ing_pkt_attr_template_id = 0;
    ctr_flex_user_data_t user_data = {0};
    ctr_flex_pkt_attribute_template_t ing_pkt_attribute_template = {0};
    bool ingress = true;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    user_data.ingress = true;
    user_data.data_type = CTR_FLEX_PKT_ATTR_DATA;
    user_data.event_reason = event_reason;

    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "inside ctr_ing_flex_pkt_attr_template_lt_stage_cb")));

   /* Get all the fields */
    while (key_fields) {
        if (key_fields->id ==
            CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_CTR_ING_FLEX_PKT_ATTR_TEMPLATE_IDf) {
            ing_pkt_attr_template_id = key_fields->data;
            break;
        }
        key_fields = key_fields->next;
    }

    user_data.pkt_attr_data.pkt_attr_id = ing_pkt_attr_template_id;

    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
             (lookup_flex_pkt_attr_table (unit,
                                     ingress,
                                     true,
                                     ing_pkt_attr_template_id,
                                     NULL,
                                     &ing_pkt_attribute_template));
    }

    while (data_fields) {
        extract_flex_ing_pkt_attribute_fields(&ing_pkt_attribute_template,
                                              data_fields);
        data_fields = data_fields->next;
    }

    if (output_fields) {
        output_fields->count = 0;
    }

    if (event_reason != BCMIMM_ENTRY_LOOKUP) {
        user_data.pkt_attr_data.template = &ing_pkt_attribute_template;

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_pkt_attr_template (unit, &user_data));
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        ctrl->ing_pkt_attributes[ing_pkt_attr_template_id].active = true;
    }
    else if (event_reason == BCMIMM_ENTRY_DELETE) {
        ctrl->ing_pkt_attributes[ing_pkt_attr_template_id].active = false;
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * In-memory EGR PKT_ATTR_TEMPLATE stage callback function.
 */
static int
ctr_egr_flex_pkt_attr_template_lt_stage_cb(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    uint32_t egr_pkt_attr_template_id = 0;
    ctr_flex_user_data_t user_data = {0};
    ctr_flex_pkt_attribute_template_t egr_pkt_attribute_template = {0};
    bool ingress = false;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    user_data.ingress = false;
    user_data.data_type = CTR_FLEX_PKT_ATTR_DATA;
    user_data.event_reason = event_reason;

    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "inside ctr_egr_flex_pkt_attr_template_lt_stage_cb")));

    /* Get all the fields */
    while (key_fields) {
        if (key_fields->id ==
                CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_CTR_EGR_FLEX_PKT_ATTR_TEMPLATE_IDf) {
            egr_pkt_attr_template_id = key_fields->data;
            break;
        }
        key_fields = key_fields->next;
    }

    user_data.pkt_attr_data.pkt_attr_id = egr_pkt_attr_template_id;

    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
             (lookup_flex_pkt_attr_table (unit,
                                     ingress,
                                     true,
                                     egr_pkt_attr_template_id,
                                     NULL,
                                     &egr_pkt_attribute_template));
    }

    while (data_fields) {
        switch (data_fields->id) {
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_IPf:
            egr_pkt_attribute_template.ip = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_DROPf:
            egr_pkt_attribute_template.drop = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_NETWORK_SVPf:
            egr_pkt_attribute_template.network_svp = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_ACCESS_SVPf:
            egr_pkt_attribute_template.access_svp = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_NETWORK_DVPf:
            egr_pkt_attribute_template.network_dvp = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_ACCESS_DVPf:
            egr_pkt_attribute_template.access_dvp = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_MULTICASTf:
            egr_pkt_attribute_template.pkt_type_multicast = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNICASTf:
            egr_pkt_attribute_template.pkt_type_unicast = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_ALLf:
            egr_pkt_attribute_template.pkt_type_all = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_TOS_ECNf:
            egr_pkt_attribute_template.tos_ecn = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_TOS_DSCPf:
            egr_pkt_attribute_template.tos_dscp = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PORT_IDf:
            egr_pkt_attribute_template.port = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_INT_CNf:
            egr_pkt_attribute_template.int_cn = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_INNER_DOT1Pf:
            egr_pkt_attribute_template.inner_dot1p = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_OUTER_DOT1Pf:
            egr_pkt_attribute_template.outer_dot1p = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_VLAN_UTf:
            egr_pkt_attribute_template.vlan_ut = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_VLAN_SITf:
            egr_pkt_attribute_template.vlan_sit = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_VLAN_SOTf:
            egr_pkt_attribute_template.vlan_sot = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_VLAN_DTf:
            egr_pkt_attribute_template.vlan_dt = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_INT_PRIf:
            egr_pkt_attribute_template.int_pri = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_G_COLORf:
            egr_pkt_attribute_template.pre_fp_g_color = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_Y_COLORf:
            egr_pkt_attribute_template.pre_fp_y_color = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_R_COLORf:
            egr_pkt_attribute_template.pre_fp_r_color = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_CONGESTION_MARKEDf:
            egr_pkt_attribute_template.congestion_marked = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_UNICAST_QUEUEINGf:
            egr_pkt_attribute_template.unicast_queueing = data_fields->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_MMU_QUEUE_NUMf:
            egr_pkt_attribute_template.mmu_queue_num = data_fields->data;
            break;
        default:
            break;
        }
        data_fields = data_fields->next;
    }

    if (output_fields) {
        output_fields->count = 0;
    }

    if (event_reason != BCMIMM_ENTRY_LOOKUP) {
        user_data.pkt_attr_data.template = &egr_pkt_attribute_template;

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_pkt_attr_template (unit, &user_data));
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        ctrl->egr_pkt_attributes[egr_pkt_attr_template_id].active = true;
    }
    else if (event_reason == BCMIMM_ENTRY_DELETE) {
        ctrl->egr_pkt_attributes[egr_pkt_attr_template_id].active = false;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory ING ATTR_TEMPLATE stage callback function.
 */
static int
ctr_ing_flex_attr_template_lt_stage_cb(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    uint32_t attr_id = 0;
    uint32_t instance_id = 0;
    uint32_t current_pkt_attr_id = 0;
    uint32_t pkt_attr_id = INVALID_CTR_FLEX_ID;
    llist_t **llist;
    bool ingress = true;
    ctr_flex_user_data_t user_data = {0};
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    user_data.ingress = true;
    user_data.data_type = CTR_FLEX_ATTR_DATA;
    user_data.event_reason = event_reason;

   /* Get all the fields */
    while (key_fields) {
        if (key_fields->id ==
             CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_ING_FLEX_ATTR_TEMPLATE_IDf) {
            attr_id = key_fields->data;
        }
        else if (key_fields->id == CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_FLEX_INSTANCEf) {
            instance_id = key_fields->data;
        }
        key_fields = key_fields->next;
    }

    while (data_fields) {
        if (data_fields->id ==
             CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_ING_FLEX_PKT_ATTR_TEMPLATE_IDf) {
            pkt_attr_id = data_fields->data;
            break;
        }
        data_fields = data_fields->next;
    }

    if (output_fields) {
        output_fields->count = 0;
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        /* default pkt_attr_id for insert if not provided */
        if (pkt_attr_id == INVALID_CTR_FLEX_ID) {
            pkt_attr_id = 0;
        }
    }
    else {
        /* recover pkt_attr_id for delete and update */
        SHR_IF_ERR_EXIT
            (lookup_flex_attr_table(unit,
                                    ingress,
                                    attr_id,
                                    instance_id,
                                    NULL,
                                    &current_pkt_attr_id));

        if (pkt_attr_id == INVALID_CTR_FLEX_ID) {
            pkt_attr_id = current_pkt_attr_id;
        }
    }

    user_data.attr_data.attr_id = attr_id;
    user_data.attr_data.instance_id = instance_id;
    user_data.attr_data.pkt_attr_id = pkt_attr_id;

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
        /* subscribe to pkt attr template */
        llist = &ctrl->ing_pkt_attributes[pkt_attr_id].ref_entry_list;
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_SUBSCRIBE,
                                      llist,
                                      attr_id));

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_attr_template (unit, &user_data));

        /* If LT inserted add entry to internal database entry. */
        ctrl->ing_attributes[attr_id].num_instances++;
        break;

    case BCMIMM_ENTRY_DELETE:
        /* unsubscribe from pkt attr template */
        llist = &ctrl->ing_pkt_attributes[current_pkt_attr_id].ref_entry_list;
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_UNSUBSCRIBE,
                                      llist,
                                      attr_id));

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_attr_template (unit, &user_data));

        /* If LT deleted, delete internal database entry. */
        if (ctrl->ing_attributes[attr_id].num_instances <= 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        ctrl->ing_attributes[attr_id].num_instances--;
        break;

    case BCMIMM_ENTRY_UPDATE:
        /* unsubscribe current_pkt_attr_id */
        llist = &ctrl->ing_pkt_attributes[current_pkt_attr_id].ref_entry_list;
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_UNSUBSCRIBE,
                                      llist,
                                      attr_id));

        /* subscribe new pkt_attr_id */
        llist = &ctrl->ing_pkt_attributes[pkt_attr_id].ref_entry_list;
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_SUBSCRIBE,
                                      llist,
                                      attr_id));

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_attr_template (unit, &user_data));
        break;

    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory EGR ATTR_TEMPLATE stage callback function.
 */
static int
ctr_egr_flex_attr_template_lt_stage_cb(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    uint32_t attr_id = 0;
    uint32_t instance_id = 0;
    uint32_t current_pkt_attr_id = 0;
    uint32_t pkt_attr_id = INVALID_CTR_FLEX_ID;
    llist_t  **llist;
    bool ingress = false;
    ctr_flex_user_data_t user_data = {0};
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    user_data.ingress = false;
    user_data.data_type = CTR_FLEX_ATTR_DATA;
    user_data.event_reason = event_reason;

   /* Get all the fields */
    while (key_fields) {
        if (key_fields->id ==
             CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_EGR_FLEX_ATTR_TEMPLATE_IDf) {
            attr_id = key_fields->data;
        }
        else if (key_fields->id == CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_FLEX_INSTANCEf) {
            instance_id = key_fields->data;
        }
        key_fields = key_fields->next;
    }

    while (data_fields) {
        if (data_fields->id ==
             CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_EGR_FLEX_PKT_ATTR_TEMPLATE_IDf) {
            pkt_attr_id = data_fields->data;
            break;
        }
        data_fields = data_fields->next;
    }

    if (output_fields) {
        output_fields->count = 0;
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        /* default pkt_attr_id for insert if not provided */
        if (pkt_attr_id == INVALID_CTR_FLEX_ID) {
            pkt_attr_id = 0;
        }
    }
    else {
        /* recover pkt_attr_id for delete and update */
        SHR_IF_ERR_EXIT
            (lookup_flex_attr_table(unit,
                                    ingress,
                                    attr_id,
                                    instance_id,
                                    NULL,
                                    &current_pkt_attr_id));

        if (pkt_attr_id == INVALID_CTR_FLEX_ID) {
            pkt_attr_id = current_pkt_attr_id;
        }
    }

    user_data.attr_data.attr_id = attr_id;
    user_data.attr_data.instance_id = instance_id;
    user_data.attr_data.pkt_attr_id = pkt_attr_id;

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
        /* subscribe to pkt attr template */
        llist = &ctrl->egr_pkt_attributes[pkt_attr_id].ref_entry_list;
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_SUBSCRIBE,
                                      llist,
                                      attr_id));

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_attr_template (unit, &user_data));

        /* If LT inserted add entry to internal database entry. */
        ctrl->egr_attributes[attr_id].num_instances++;
        break;

    case BCMIMM_ENTRY_DELETE:
        /* unsubscribe from pkt attr template */
        llist = &ctrl->egr_pkt_attributes[current_pkt_attr_id].ref_entry_list;
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_UNSUBSCRIBE,
                                      llist,
                                      attr_id));

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_attr_template (unit, &user_data));

        /* If LT deleted, delete internal database entry. */
        if (ctrl->egr_attributes[attr_id].num_instances <= 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        ctrl->egr_attributes[attr_id].num_instances--;
        break;

    case BCMIMM_ENTRY_UPDATE:
        /* unsubscribe current_pkt_attr_id */
        llist = &ctrl->egr_pkt_attributes[current_pkt_attr_id].ref_entry_list;
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_UNSUBSCRIBE,
                                      llist,
                                      attr_id));

        /* subscribe new pkt_attr_id */
        llist = &ctrl->egr_pkt_attributes[pkt_attr_id].ref_entry_list;
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_SUBSCRIBE,
                                      llist,
                                      attr_id));

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_attr_template (unit, &user_data));
        break;

    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! get LT fields default values */
int
ctr_flex_entry_get_field_default(int unit,
                                 bool ingress,
                                 ctr_flex_entry_data_t *default_value)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    size_t num_fid, i;
    int select;

    bcmlrd_sid_t table_sid[2] = {CTR_ING_FLEX_ENTRYt, CTR_EGR_FLEX_ENTRYt};
    SHR_FUNC_ENTER(unit);

    select  = ingress ? 0 : 1;

    SHR_IF_ERR_EXIT(
        bcmlrd_field_count_get(unit, table_sid[select], &num_fid));

    lt_info = bcmlrd_table_get(table_sid[select]);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmptmCtrFlexEntryFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     lt_info->name,
                                     num_fid,
                                     f_info,
                                     &num_fid));

    for (i=0; i<num_fid; i++) {
        if (f_info[i].id == entry_fid[select]) {
            default_value->entry_id = f_info[i].def.u64;
        }
        else if (f_info[i].id == base_index_auto_fid[select]) {
            default_value->base_index_mode_auto = f_info[i].def.u64;
        }
        else if (f_info[i].id == base_index_fid[select]) {
            default_value->base_index = f_info[i].def.u64;
        }
        else if (f_info[i].id == attr_fid[select]) {
            default_value->attr_id = f_info[i].def.u64;
        }
        else if (f_info[i].id == pool_fid[select]) {
            default_value->pool_id = f_info[i].def.u64;
        }
        else if (f_info[i].id == offset_fid[select]) {
            default_value->offset_mode = f_info[i].def.u64;
        }
        else if (f_info[i].id == pipe_num_fid[select]) {
            default_value->pipe_num = f_info[i].def.u64;
        }
        else if (f_info[i].id == pipe_unique_fid[select]) {
            default_value->pipe_unique = f_info[i].def.u64;
        }
        else if (f_info[i].id == max_instances_auto_fid[select]) {
            default_value->max_instances_mode_auto = f_info[i].def.u64;
        }
        else if (f_info[i].id == max_instances_fid[select]) {
            default_value->max_instances = f_info[i].def.u64;
        }
    }

exit:
    if (f_info) {
        sal_free(f_info);
    }
    SHR_FUNC_EXIT();
}

static int
ctr_flex_process_waitlist(int unit, bool ingress, uint32_t current_entry_id,
                          uint32_t pipe_num, uint32_t pool_id,
                          uint32_t base_index)
{
    uint32_t entry_id;
    ctr_flex_notify_entry_opcode_t entry_opcode = {0};
    ctr_flex_user_data_t waitlist_data = {0};
    ctr_flex_entry_data_t new_entry_data = {0};
    ctr_flex_control_t *ctrl = NULL;
    llist_t *llist = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    llist = ingress ? ctrl->ing_wait_list :
                      ctrl->egr_wait_list ;

    if (llist == NULL ||
        bcmptm_ctr_flex_pool_mgr_is_pool_full(unit, ingress,
                                              pipe_num, pool_id)) {
        SHR_EXIT();
    }

    waitlist_data.ingress = ingress;
    waitlist_data.data_type = CTR_FLEX_ENTRY_DATA;

    while (llist) {
        entry_id = llist->val;

        llist = llist->next;

        /* don't retry an entry that was just updated; IMM not updated yet */
        if (current_entry_id == entry_id) continue;

        /* recover waitlisted entry's pool_id & base_index from imm */
        SHR_IF_ERR_EXIT
            (lookup_flex_entry_table (unit,
                                      ingress,
                                      entry_id,
                                      &waitlist_data.entry_data));

       if (waitlist_data.entry_data.pool_id != pool_id) continue;

       if (!waitlist_data.entry_data.base_index_mode_auto &&
           waitlist_data.entry_data.base_index != base_index) continue;

       /* retry waitlisted entry */
       entry_opcode.retry = true;

       SHR_IF_ERR_EXIT
           (ctr_flex_notify_entry_template (unit,
                                            &entry_opcode,
                                            &waitlist_data,
                                            &new_entry_data,
                                            NULL));

       if (new_entry_data.op_state != ctrl->ctr_flex_enum.awaiting_counter_resource) {
            /* update operational status in imm */
            SHR_IF_ERR_EXIT
                (oper_state_update(unit,
                                   ingress,
                                   entry_id,
                                   &new_entry_data));

            bcmevm_publish_event_notify(unit,
                                        "bcmptmCtrFlexOpStateChg",
                                        entry_id);

            if (bcmptm_ctr_flex_pool_mgr_is_pool_full(unit, ingress,
                                              pipe_num, pool_id)) {
                SHR_EXIT();
            }
       }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory entry_stage callback helper function.
 */
static int
ctr_flex_entry_lt_stage_cb(int unit,
                bool ingress,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                bcmltd_fields_t *output_fields)
{
    llist_t **llist;
    ctr_flex_user_data_t user_data = {0};
    ctr_flex_notify_entry_opcode_t entry_opcode = {0};
    uint32_t attr_id = INVALID_CTR_FLEX_ID;  /* used to detect changes */
    uint32_t pool_id = INVALID_CTR_FLEX_ID;
    uint32_t base_index = INVALID_CTR_FLEX_ID;
    uint32_t pipe_num = INVALID_CTR_FLEX_ID;
    uint32_t max_instances = INVALID_CTR_FLEX_ID;
    bool max_instances_auto = false;
    bool max_instances_auto_valid = false;
    bool base_index_auto = false;
    bool base_index_auto_valid = false;
    bool pipe_unique = false;
    bool pipe_unique_valid = false;
    uint32_t offset_mode = 0;
    uint32_t op_state = 0;
    uint32_t entry_id = 0;
    ctr_flex_entry_data_t default_value = {0};
    ctr_flex_entry_data_t new_entry_data = {0};
    ctr_flex_entry_data_t current_entry_data = {0};
    int select;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    op_state = ctrl->ctr_flex_enum.attr_template_not_present;
    select  = ingress ? 0 : 1;

    user_data.ingress = ingress;
    user_data.data_type = CTR_FLEX_ENTRY_DATA;
    user_data.event_reason = event_reason;

    SHR_IF_ERR_EXIT
        (ctr_flex_entry_get_field_default(unit,
                                 ingress,
                                 &default_value));

    offset_mode = default_value.offset_mode;

    /*
     * Assign default values for insert only.
     * Un-modified values for delete/update will be recovered from IMM.
     * Modified values will trigger counters realloc.
     */
    if (event_reason == BCMIMM_ENTRY_INSERT) {
        attr_id = default_value.attr_id;
        pool_id = default_value.pool_id;
        base_index = default_value.base_index;
        pipe_num = default_value.pipe_num;
        pipe_unique = default_value.pipe_unique;
        max_instances = default_value.max_instances;
        base_index_auto = default_value.base_index_mode_auto;
        max_instances_auto = default_value.max_instances_mode_auto;
        pipe_unique_valid = default_value.pipe_unique_valid;
        base_index_auto_valid = default_value.base_index_mode_auto;
        max_instances_auto_valid = default_value.max_instances_mode_auto;
    }

    /* Parse the user data and key fields */
    while (key_fields) {
        if (key_fields->id == entry_fid[select]) {
            entry_id = key_fields->data;
        }
        key_fields = key_fields->next;
    }

    while (data_fields) {
        if (data_fields->id == attr_fid[select]) {
            attr_id = data_fields->data;
        }
        else if (data_fields->id == pool_fid[select]) {
            pool_id = data_fields->data;
        }
        else if (data_fields->id == base_index_auto_fid[select]) {
            base_index_auto = data_fields->data;
            base_index_auto_valid = true;
        }
        else if (data_fields->id == base_index_fid[select]) {
            base_index = data_fields->data;
        }
        else if (data_fields->id == offset_fid[select]) {
            offset_mode = data_fields->data;
        }
        else if (data_fields->id == op_state_fid[select]) {
            op_state = data_fields->data;
        }
        else if (data_fields->id == pipe_num_fid[select]) {
            pipe_num = data_fields->data;
        }
        else if (data_fields->id == pipe_unique_fid[select]) {
            pipe_unique = data_fields->data;
            pipe_unique_valid = true;
        }
        else if (data_fields->id == max_instances_fid[select]) {
            max_instances = data_fields->data;
        }
        else if (data_fields->id == max_instances_auto_fid[select]) {
            max_instances_auto = data_fields->data;
            max_instances_auto_valid = true;
        }
        data_fields = data_fields->next;
    }

    user_data.entry_data.entry_id = entry_id;
    user_data.entry_data.attr_id = attr_id;
    user_data.entry_data.pool_id = pool_id;
    user_data.entry_data.offset_mode = offset_mode;
    user_data.entry_data.op_state = op_state;
    user_data.entry_data.base_index_mode_auto = base_index_auto;
    user_data.entry_data.base_index_mode_auto_valid = base_index_auto_valid;
    user_data.entry_data.max_instances_mode_auto = max_instances_auto;
    user_data.entry_data.max_instances_mode_auto_valid = max_instances_auto_valid;
    user_data.entry_data.max_instances = max_instances;
    user_data.entry_data.base_index = base_index;
    user_data.entry_data.pipe_num = pipe_num;
    user_data.entry_data.pipe_unique = pipe_unique;
    user_data.entry_data.pipe_unique_valid = pipe_unique_valid;

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
        entry_opcode.insert = true;

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_entry_template (unit,
                                             &entry_opcode,
                                             &user_data,
                                             &new_entry_data,
                                             NULL));

        llist = ingress ?
                &ctrl->ing_attributes[attr_id].ref_entry_list:
                &ctrl->egr_attributes[attr_id].ref_entry_list;

        /* subscribe new entry_id to attr_template_id */
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_SUBSCRIBE,
                                      llist,
                                      entry_id));
        break;

    case BCMIMM_ENTRY_DELETE:
        entry_opcode.delete = true;

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_entry_template (unit,
                                             &entry_opcode,
                                             &user_data,
                                             &new_entry_data,
                                             &current_entry_data));

        /* unsubscribe current entry_id from attr_template_id */
        llist = ingress ?
                &ctrl->ing_attributes[current_entry_data.attr_id].ref_entry_list:
                &ctrl->egr_attributes[current_entry_data.attr_id].ref_entry_list;

        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_UNSUBSCRIBE,
                                      llist,
                                      entry_id));
        /*
         * no more entries using current_entry_data.attr_id;
         * reset max_instances
         */
        if (*llist == NULL) {
            max_instances = 0;
        }
        break;

    case BCMIMM_ENTRY_UPDATE:
        entry_opcode.update = true;

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_entry_template (unit,
                                             &entry_opcode,
                                             &user_data,
                                             &new_entry_data,
                                             &current_entry_data));

        if (attr_id != INVALID_CTR_FLEX_ID &&
            attr_id != current_entry_data.attr_id) {
            /* unsubscribe entry_id from current attr_template_id */
            llist = ingress ?
                    &ctrl->ing_attributes[current_entry_data.attr_id].ref_entry_list:
                    &ctrl->egr_attributes[current_entry_data.attr_id].ref_entry_list;

            SHR_IF_ERR_EXIT
                (ctr_flex_subscribe_list (unit,
                                          CTR_FLEX_LIST_UNSUBSCRIBE,
                                          llist,
                                          entry_id));

            /* subscribe entry_id to new attr_template_id */
            llist = ingress ?
                    &ctrl->ing_attributes[attr_id].ref_entry_list:
                    &ctrl->egr_attributes[attr_id].ref_entry_list;

            SHR_IF_ERR_EXIT
                (ctr_flex_subscribe_list (unit,
                                          CTR_FLEX_LIST_SUBSCRIBE,
                                          llist,
                                          entry_id));
        }

        break;

    default:
        break;
    }

    if (event_reason == BCMIMM_ENTRY_DELETE ||
        event_reason == BCMIMM_ENTRY_UPDATE) {
        /*
         * There may be room for waitlisted entries following
         * an update or delete.
         */
        SHR_IF_ERR_EXIT
            (ctr_flex_process_waitlist(unit,
                                       ingress,
                                       current_entry_data.entry_id,
                                       current_entry_data.pipe_num,
                                       current_entry_data.pool_id,
                                       current_entry_data.base_index));
    }

    if (output_fields) {
        output_fields->count = 3;
        output_fields->field[0]->id = op_fid[select];
        output_fields->field[0]->data = new_entry_data.op_state;
        output_fields->field[1]->id = max_instances_oper_fid[select];
        output_fields->field[1]->data = new_entry_data.max_instances;
        output_fields->field[2]->id = base_index_oper_fid[select];
        output_fields->field[2]->data = new_entry_data.base_index;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory ING_FLEX_ENTRY stage callback function.
 */
static int
ctr_ing_flex_entry_lt_stage_cb(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    int t;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (t=0; t<LT_INFO_NUM_TABLES; t++) {
        if (!sal_strcmp("CTR_ING_FLEX_ENTRY",
                        ctrl->ctr_flex_field_info[t].name))
            break;
    }

    if (output_fields) {
        if (t == LT_INFO_NUM_TABLES) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        else if (output_fields->count < ctrl->ctr_flex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "output_fields allocated by IMM callback is too "
                                  "small to hold the desired fields.\n")));
            return SHR_E_INTERNAL;
        }
    }

    SHR_IF_ERR_EXIT
        (ctr_flex_entry_lt_stage_cb(unit,
                                    true,
                                    event_reason,
                                    key_fields,
                                    data_fields,
                                    output_fields));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory logical table stage callback function.
 */
static int
ctr_egr_flex_entry_lt_stage_cb(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    int t;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (t=0; t<LT_INFO_NUM_TABLES; t++) {
        if (!sal_strcmp("CTR_EGR_FLEX_ENTRY",
                        ctrl->ctr_flex_field_info[t].name))
            break;
    }

    if (output_fields) {
        if (t == LT_INFO_NUM_TABLES) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        else if (output_fields->count < ctrl->ctr_flex_field_info[t].num_data_fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "output_fields allocated by IMM callback is too "
                                  "small to hold the desired fields.\n")));
            return SHR_E_INTERNAL;
        }
    }

    SHR_IF_ERR_EXIT
        (ctr_flex_entry_lt_stage_cb(unit,
                                    false,
                                    event_reason,
                                    key_fields,
                                    data_fields,
                                    output_fields));
exit:
    SHR_FUNC_EXIT();
}

int
ctr_flex_imm_lt_register(int unit, bool warm)
{
    bcmimm_lt_cb_t ctr_flex_lt_cb;

    SHR_FUNC_ENTER(unit);

    /* Register ctr_ing_flex_pool_info_lt */
    sal_memset(&ctr_flex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_flex_lt_cb.validate = ctr_ing_flex_pool_info_lt_validate_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_ING_FLEX_POOL_INFOt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_egr_flex_pool_info_lt */
    ctr_flex_lt_cb.validate = ctr_egr_flex_pool_info_lt_validate_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_EGR_FLEX_POOL_INFOt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_egr_flex_pkt_attr_template_lt */
    sal_memset(&ctr_flex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
    ctr_flex_lt_cb.stage = ctr_egr_flex_pkt_attr_template_lt_stage_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_ing_flex_pkt_attr_template_lt */
    ctr_flex_lt_cb.stage = ctr_ing_flex_pkt_attr_template_lt_stage_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_ING_FLEX_PKT_ATTR_TEMPLATEt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_ing_flex_attr_template_lt */
    ctr_flex_lt_cb.stage = ctr_ing_flex_attr_template_lt_stage_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_ING_FLEX_ATTR_TEMPLATEt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_egr_flex_attr_template_lt */
    ctr_flex_lt_cb.stage = ctr_egr_flex_attr_template_lt_stage_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_EGR_FLEX_ATTR_TEMPLATEt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_ing_flex_entry_lt */
    ctr_flex_lt_cb.stage = ctr_ing_flex_entry_lt_stage_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_ING_FLEX_ENTRYt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_egr_flex_entry_lt */
    ctr_flex_lt_cb.stage = ctr_egr_flex_entry_lt_stage_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_EGR_FLEX_ENTRYt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_ing_flex_stats_lt */
    ctr_flex_lt_cb.stage = bcmptm_flex_ing_ctr_stats_lt_stage_cb;
    ctr_flex_lt_cb.lookup = bcmptm_flex_ing_ctr_stats_lt_lookup_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_ING_FLEX_STATSt,
                            &ctr_flex_lt_cb,
                            NULL));

    /* Register ctr_egr_flex_stats_lt */
    ctr_flex_lt_cb.stage = bcmptm_flex_egr_ctr_stats_lt_stage_cb;
    ctr_flex_lt_cb.lookup = bcmptm_flex_egr_ctr_stats_lt_lookup_cb;

    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            CTR_EGR_FLEX_STATSt,
                            &ctr_flex_lt_cb,
                            NULL));
exit:
    SHR_FUNC_EXIT();
}
