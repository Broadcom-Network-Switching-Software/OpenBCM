/*! \file ctr_flex_notify.c
 *
 * Flex counter notify and subscribe routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"
#include <bcmltd/chip/bcmltd_id.h>

/*!
 * Release an active entry created by init_active_entry
 */
static void
free_active_entry (active_entry_t *active_entry)
{
    if (active_entry == NULL) {
        return;
    }
    if (active_entry->ctr_instance_id) {
        sal_free(active_entry->ctr_instance_id);
    }
    if (active_entry->pkt_attribute_template_id) {
        sal_free(active_entry->pkt_attribute_template_id);
    }
    if (active_entry->offset_table_map) {
        sal_free(active_entry->offset_table_map);
    }
    if (active_entry->entry_data) {
        sal_free(active_entry->entry_data);
    }
    sal_free(active_entry);
}

/*!
 * Create and initialize an active entry
 */
static int
create_active_entry (int unit, bool ingress, active_entry_t **active_entry_ref)
{
    active_entry_t *active_entry = NULL;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (*active_entry_ref != NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_ALLOC(active_entry,
              sizeof(active_entry_t),
              "bcmptmCtrFlexActiveEntry");
    SHR_NULL_CHECK(active_entry, SHR_E_MEMORY);
    sal_memset(active_entry, 0, sizeof(active_entry_t));

    SHR_ALLOC(active_entry->ctr_instance_id,
              sizeof(uint32_t)*(MAX_CTR_INSTANCES+1),
              "bcmptmCtrFlexActiveEntryInstanceArray");
    SHR_NULL_CHECK(active_entry->ctr_instance_id, SHR_E_MEMORY);
    sal_memset(active_entry->ctr_instance_id, 0,
              sizeof(uint32_t)*(MAX_CTR_INSTANCES+1));

    SHR_ALLOC(active_entry->pkt_attribute_template_id,
              sizeof(uint32_t)*(MAX_CTR_INSTANCES+1),
              "bcmptmCtrFlexActiveEntryPktAttrArray");
    SHR_NULL_CHECK(active_entry->pkt_attribute_template_id, SHR_E_MEMORY);
    sal_memset(active_entry->pkt_attribute_template_id, 0,
              sizeof(uint32_t)*(MAX_CTR_INSTANCES+1));

    SHR_ALLOC(active_entry->offset_table_map,
              sizeof(offset_table_entry_t)*MAX_CTR_INSTANCES,
              "bcmptmCtrFlexActiveEntryOffsetTableMap");
    SHR_NULL_CHECK(active_entry->offset_table_map, SHR_E_MEMORY);
    sal_memset(active_entry->offset_table_map, 0,
              sizeof(offset_table_entry_t)*MAX_CTR_INSTANCES);

    SHR_ALLOC(active_entry->entry_data,
              sizeof(ctr_flex_entry_data_t),
              "bcmptmCtrFlexActiveEntryData");
    SHR_NULL_CHECK(active_entry->entry_data, SHR_E_MEMORY);
    sal_memset(active_entry->entry_data, 0,
              sizeof(ctr_flex_entry_data_t));

    active_entry->ingress = ingress;
    active_entry->entry_data->pool_id = INVALID_CTR_FLEX_ID;
    active_entry->entry_data->base_index = INVALID_CTR_FLEX_ID;
    active_entry->entry_data->max_instances = INVALID_CTR_FLEX_ID;
    active_entry->entry_data->op_state = ctrl->ctr_flex_enum.attr_template_not_present;
    *active_entry_ref = active_entry;

    SHR_FUNC_EXIT();

exit:
   free_active_entry(active_entry);
   SHR_FUNC_EXIT();
}

/*!
 * Get all active instances associated with this active_entry.
 * This routine will lookup the IMM tables.
 * user_data is the new entry being processed, not yet written to IMM.
 * Set user to NULL if only current instances that exist in IMM are needed.
 */
static int
ctr_flex_get_instances (int unit, active_entry_t *active_entry,
                        ctr_flex_user_data_t *user_data)
{
    uint32_t i, num_instances, max_instances;
    uint32_t active_instance_cnt = 0;
    uint32_t pkt_attribute_template_id;
    uint32_t op_state;
    ctr_flex_control_t *ctrl = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    op_state = ctrl->ctr_flex_enum.attr_template_not_present;

    if (active_entry->entry_data->offset_mode == ctrl->ctr_flex_enum.direct_map_mode) {
        active_entry->entry_data->op_state = ctrl->ctr_flex_enum.operational_state_ok;
        SHR_EXIT();
    }

    if (active_entry->ingress) {
        num_instances =
            ctrl->ing_attributes[active_entry->entry_data->attr_id].num_instances ;
    }
    else {
        num_instances =
            ctrl->egr_attributes[active_entry->entry_data->attr_id].num_instances ;
    }

    max_instances = active_entry->entry_data->max_instances;

    /* account for the one being processed */
    if (user_data && user_data->data_type == CTR_FLEX_ATTR_DATA) {
        if (user_data->event_reason == BCMIMM_ENTRY_INSERT) {
            num_instances++;
        }
        else if (user_data->event_reason == BCMIMM_ENTRY_DELETE) {
            num_instances--;
        }
    }

    if (max_instances > 0 && num_instances > max_instances) {
        /*
         * Reached max instances allocated for this group;
         * do not process
         */
        num_instances = 0;
        active_instance_cnt = 0;
        op_state = ctrl->ctr_flex_enum.attr_template_exceeded;
    }

    /* find all active instances; they are at least num_instances */
    for (i = 0; i < MAX_CTR_INSTANCES; i++) {
        if (active_instance_cnt == num_instances) {
            break;
        }
        rv = lookup_flex_attr_table(unit,
                                    active_entry->ingress,
                                    active_entry->entry_data->attr_id,
                                    i,
                                    user_data,
                                    &pkt_attribute_template_id);

        if (rv == SHR_E_NONE) {
            if (op_state != ctrl->ctr_flex_enum.operational_state_ok) {
                op_state = ctrl->ctr_flex_enum.pkt_attr_template_not_present;
            }

            /* Check if corresponding pkt_attribute_template LT exists */
            rv = lookup_flex_pkt_attr_table(unit,
                                            active_entry->ingress,
                                            true,
                                            pkt_attribute_template_id,
                                            user_data,
                                            NULL);

            /* Update op_state and active_entry with this instance */
            if (rv == SHR_E_NONE) {
                op_state = ctrl->ctr_flex_enum.operational_state_ok;
                active_entry->ctr_instance_id[active_instance_cnt] = i;
                active_entry->pkt_attribute_template_id[active_instance_cnt] =
                    pkt_attribute_template_id;
                active_instance_cnt++;
            }
        }
    }

    active_entry->active_instances = active_instance_cnt;
    if (user_data != NULL) {
        /*
         * set the expected op_state for a new active entry;
         * keep the IMM op_state for a current active entry
         */
        active_entry->entry_data->op_state = op_state;
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * Subscribe an ID to a notification list.
 * Notification lists may be in the FLEX_ATTR or FLEX_PKT_ATTR tables.
 * Node added to tail of list. Refcount used to avoid duplicate nodes.
 */
int
ctr_flex_subscribe_list (int unit, bool subscribe, llist_t **llist, uint32_t id)
{
    llist_t *curr = *llist;

    SHR_FUNC_ENTER(unit);

    if (subscribe) {
        llist_t *new_node = NULL;

        if (curr == NULL) {
            /* add the first node */
            SHR_ALLOC(new_node, sizeof(llist_t), "bcmptmCtrFlexListNode");
            SHR_NULL_CHECK(new_node, SHR_E_MEMORY);
            new_node->val = id;
            new_node->next = NULL;
            new_node->ref_count = 1;
            *llist = new_node;
            SHR_EXIT();
        }

        /* search for id */
        while (curr->val != id && curr->next) {
            curr = curr->next;
        }

        if (curr->val == id) {
            /* found matching id */
            curr->ref_count++;
        }
        else {
            /* add node to tail */
            SHR_ALLOC(new_node, sizeof(llist_t), "bcmptmCtrFlexListNode");
            SHR_NULL_CHECK(new_node, SHR_E_MEMORY);
            new_node->val = id;
            new_node->next = NULL;
            new_node->ref_count = 1;
            curr->next = new_node;
        }
    }
    else {
        if (curr == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                       "entry %d not found in subscription list.\n"), id));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (curr->val == id) {
            /* head matches */
            curr->ref_count--;
            if (curr->ref_count == 0) {
                *llist = curr->next;
                sal_free(curr);
            }
            SHR_EXIT();
        }

        /* search for id */
        while (curr->next && curr->next->val != id) {
            curr = curr->next;
        }

        if (curr->next == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                       "entry %d not found in subscription list.\n"), id));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        /* found matching id */
        curr->next->ref_count--;

        if (curr->next->ref_count == 0) {
            /* remove node */
            llist_t *temp = curr->next;
            curr->next = curr->next->next;
            sal_free(temp);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Provision the counter resources for the requested operation.
 * Also manage the list of entries waiting for resources.
 */
static int
ctr_flex_manage_resource (int unit,
                          bool ingress,
                          ctr_flex_notify_entry_opcode_t *entry_opcode,
                          active_entry_t *new_active_entry,
                          active_entry_t *current_active_entry)
{
    int rv = SHR_E_NONE;
    bool move_entry = false;
    bool resize_entry = false;
    bool retry_entry = false;
    bool delete_entry = false;
    uint32_t current_op_state;
    ctr_flex_control_t *ctrl = NULL;
    llist_t **llist;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    llist = ingress ? &ctrl->ing_wait_list :
                      &ctrl->egr_wait_list ;

    current_op_state = current_active_entry->entry_data->op_state;

    retry_entry = entry_opcode->retry ||
                  (entry_opcode->update &&
                   (current_op_state == ctrl->ctr_flex_enum.awaiting_counter_resource));

    delete_entry = entry_opcode->delete &&
                   current_op_state != ctrl->ctr_flex_enum.awaiting_counter_resource;

    /* detect updates to BASE_INDEXf, POOL_IDf, PIPE_NUMf */
    move_entry = entry_opcode->update &&
                 current_op_state != ctrl->ctr_flex_enum.awaiting_counter_resource &&
        ((new_active_entry->entry_data->pool_id != current_active_entry->entry_data->pool_id) ||
         (new_active_entry->entry_data->base_index != current_active_entry->entry_data->base_index) ||
         (new_active_entry->entry_data->pipe_num != current_active_entry->entry_data->pipe_num)) ;


    if (!move_entry && !retry_entry && entry_opcode->update) {
        /* detect update to MAX_INSTANCES_AUTOf */
        if (new_active_entry->entry_data->max_instances_mode_auto !=
            current_active_entry->entry_data->max_instances_mode_auto) {
            resize_entry = true;
        }
        /* detect update to MAX_INSTANCESf in manual mode */
        if (!new_active_entry->entry_data->max_instances_mode_auto &&
            (new_active_entry->entry_data->max_instances !=
             current_active_entry->entry_data->max_instances)) {
            resize_entry = true;
        }
        /* detect update to ATTRIBUTE_TEMPLATE_IDf */
        if ((new_active_entry->entry_data->attr_id !=
          current_active_entry->entry_data->attr_id)) {
            resize_entry = true;
        }
    }

    if (resize_entry) {
        rv = resize_counters(unit, new_active_entry,
                             current_active_entry->entry_data->max_instances);
    }
    else {
        if (entry_opcode->insert || move_entry || retry_entry) {
            rv = allocate_counters(unit, new_active_entry);
        }

        if (delete_entry || move_entry) {
            SHR_IF_ERR_EXIT
                (free_counters(unit, current_active_entry));
        }
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);

    if (rv == SHR_E_RESOURCE) {
        if (current_op_state != ctrl->ctr_flex_enum.awaiting_counter_resource) {
        /* This entry does not have resources and is not yet waitlisted. */
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_SUBSCRIBE,
                                      llist,
                                      new_active_entry->entry_data->entry_id));
        }
        SHR_ERR_EXIT(rv);
    }
    else {
        if (current_op_state == ctrl->ctr_flex_enum.awaiting_counter_resource) {
        /* This entry was waitlisted, but has acquired resources now. */
        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_UNSUBSCRIBE,
                                      llist,
                                      new_active_entry->entry_data->entry_id));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Init a new active entry with non-updated fields */
static void
update_active_entry(active_entry_t *new_active_entry,
                    active_entry_t *current_active_entry)
{
    if (new_active_entry->entry_data->attr_id == INVALID_CTR_FLEX_ID) {
        new_active_entry->entry_data->attr_id =
        current_active_entry->entry_data->attr_id;
    }
    if (new_active_entry->entry_data->pool_id == INVALID_CTR_FLEX_ID) {
        new_active_entry->entry_data->pool_id = current_active_entry->entry_data->pool_id;
    }
    if (new_active_entry->entry_data->pipe_num == INVALID_CTR_FLEX_ID) {
        new_active_entry->entry_data->pipe_num = current_active_entry->entry_data->pipe_num;
    }
    if (new_active_entry->entry_data->base_index_mode_auto_valid == 0) {
        new_active_entry->entry_data->base_index_mode_auto =
        current_active_entry->entry_data->base_index_mode_auto;
    }
    if (new_active_entry->entry_data->max_instances_mode_auto_valid == 0) {
        new_active_entry->entry_data->max_instances_mode_auto =
        current_active_entry->entry_data->max_instances_mode_auto;
    }
    if (new_active_entry->entry_data->base_index == INVALID_CTR_FLEX_ID ||
        new_active_entry->entry_data->base_index_mode_auto) {
        new_active_entry->entry_data->base_index = current_active_entry->entry_data->base_index;
    }
    if (new_active_entry->entry_data->max_instances == INVALID_CTR_FLEX_ID ||
        new_active_entry->entry_data->max_instances_mode_auto) {
        new_active_entry->entry_data->max_instances =
        current_active_entry->entry_data->max_instances;
    }
}

/*!
 * Process the requested operation on an FLEX_ENTRY table entry.
 * This operation may be an insert, update or delete.
 * Insert and Deletes were triggered by LT insert or delete
 * on the FLEX_ENTRY table.
 * Update may be triggered by LT update on the FLEX_ENTRY table.
 * Update may also be triggered by changes in the FLEX_ATTR or
 * FLEX_PKT_ATTR tables, resulting in notifications.
 */
int
ctr_flex_notify_entry_template (int unit,
                                ctr_flex_notify_entry_opcode_t *entry_opcode,
                                ctr_flex_user_data_t *user_data,
                                ctr_flex_entry_data_t *new_entry_data,
                                ctr_flex_entry_data_t *current_entry_data)
{
    bool ingress;
    active_entry_t *current_active_entry = NULL;
    active_entry_t *new_active_entry = NULL;
    ctr_flex_control_t *ctrl = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (user_data == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ingress = user_data->ingress;

    SHR_IF_ERR_EXIT
        (create_active_entry(unit,
                             ingress,
                             &new_active_entry));

    SHR_IF_ERR_EXIT
        (create_active_entry(unit,
                             ingress,
                             &current_active_entry));

    if (user_data->data_type == CTR_FLEX_ENTRY_DATA) {
        *new_active_entry->entry_data = user_data->entry_data;
    }
    else {
        /* This is attr_id from LT ATTR_TEMPLATE */
        new_active_entry->entry_data->attr_id = user_data->attr_data.attr_id;
    }

    if (entry_opcode->delete || entry_opcode->update) {
        /* recover active entry being updated/deleted */
        SHR_IF_ERR_EXIT
            (lookup_flex_entry_table (unit,
                                      ingress,
                                      user_data->entry_data.entry_id,
                                      current_active_entry->entry_data));

        /*
         * Scan the ATTR_TEMPLATE for all instances and recover
         * current_active_entry
         */
        SHR_IF_ERR_EXIT
            (ctr_flex_get_instances (unit,
                                     current_active_entry,
                                     NULL));
    }

    if (entry_opcode->retry) {
        current_active_entry->entry_data->op_state =
        ctrl->ctr_flex_enum.awaiting_counter_resource;
    }

    /* init new active entry with non-updated fields */
    update_active_entry(new_active_entry, current_active_entry);

    /* Reject update change to PIPE_UNIQUEf */
    if (entry_opcode->update &&
        new_active_entry->entry_data->pipe_unique_valid &&
        (new_active_entry->entry_data->pipe_unique !=
         current_active_entry->entry_data->pipe_unique)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = ctr_flex_manage_resource (unit,
                                   ingress,
                                   entry_opcode,
                                   new_active_entry,
                                   current_active_entry);

    /* Exit when SHR_E_RESOURCE */
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_RESOURCE) {
            rv = SHR_E_NONE;
        }
        SHR_ERR_EXIT(rv);
    }

    if (entry_opcode->insert || entry_opcode->update || entry_opcode->retry) {
        /*
         * Scan the ATTR_TEMPLATE for all instances and
         * populate new_active_entry
         */
        SHR_IF_ERR_EXIT
            (ctr_flex_get_instances (unit,
                                     new_active_entry,
                                     user_data));
    }

    /* process new active entry instances and write to hw */
    if (current_active_entry->entry_data->op_state == ctrl->ctr_flex_enum.operational_state_ok ||
        new_active_entry->entry_data->op_state == ctrl->ctr_flex_enum.operational_state_ok) {
        SHR_IF_ERR_EXIT
            (process_attributes(unit, new_active_entry, user_data));
    }

    /* remove current active entry instances from hw */
    if (entry_opcode->update &&
         (new_active_entry->entry_data->attr_id !=
          current_active_entry->entry_data->attr_id)) {
            SHR_IF_ERR_EXIT
                (process_attributes(unit, current_active_entry, NULL));
    }

exit:
    if (current_entry_data && current_active_entry) {
        *current_entry_data = *current_active_entry->entry_data;
    }
    if (new_entry_data && new_active_entry) {
        *new_entry_data = *new_active_entry->entry_data;
    }
    free_active_entry(new_active_entry);
    free_active_entry(current_active_entry);
    SHR_FUNC_EXIT();
}

/*!
 * Notify all subscibers of the FLEX_ATTR table entry.
 * This notifies each member of the list of FLEX_ENTRY entries that
 * want to be notified of any changes to this FLEX_ATTR table entry.
 */
int
ctr_flex_notify_attr_template (int unit, ctr_flex_user_data_t *user_data)
{
    llist_t *llist;
    bool ingress;
    uint32_t attr_id;
    ctr_flex_notify_entry_opcode_t entry_opcode = {0};
    ctr_flex_entry_data_t new_entry_data = {0};
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (user_data == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ingress = user_data->ingress;
    attr_id = user_data->attr_data.attr_id;

    llist = ingress ?
            ctrl->ing_attributes[attr_id].ref_entry_list :
            ctrl->egr_attributes[attr_id].ref_entry_list ;

    entry_opcode.update = true;

    while (llist) {
        user_data->entry_data.entry_id = llist->val;

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_entry_template (unit,
                                             &entry_opcode,
                                             user_data,
                                             &new_entry_data,
                                             NULL));
        SHR_IF_ERR_EXIT
            (oper_state_update(unit,
                               ingress,
                               user_data->entry_data.entry_id,
                               &new_entry_data));

        llist = llist->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Notify all subscibers of the FLEX_PKT_ATTR table entry.
 * This notifies each member of the list of FLEX_ATTR entries that
 * want to be notified of any changes to this FLEX_PKT_ATTR table entry.
 */
int
ctr_flex_notify_pkt_attr_template (int unit, ctr_flex_user_data_t *user_data)
{

    llist_t *llist;
    bool ingress;
    uint32_t pkt_attr_id;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (user_data == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ingress = user_data->ingress;
    pkt_attr_id = user_data->pkt_attr_data.pkt_attr_id;

    llist = ingress ?
            ctrl->ing_pkt_attributes[pkt_attr_id].ref_entry_list :
            ctrl->egr_pkt_attributes[pkt_attr_id].ref_entry_list ;

    while (llist) {
        user_data->attr_data.attr_id = llist->val;

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_attr_template(unit, user_data));

        llist = llist->next;
    }

exit:
    SHR_FUNC_EXIT();
}
