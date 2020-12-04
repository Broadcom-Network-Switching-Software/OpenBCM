/*! \file ctr_flex_init.c
 *
 * Flex counter init and cleanup routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_ctr_flex.h>

/* Data driver init complete flag. */
static bool ctr_flex_supported[BCMDRD_CONFIG_MAX_UNITS];

/* Data driver for chip-specific routines. */
static const bcmptm_ctr_flex_data_driver_t *bcmptm_ctr_flex_data_cb[BCMDRD_CONFIG_MAX_UNITS];

/* Flex Counter Control Structure. */
static ctr_flex_control_t *ctr_flex_control[BCMDRD_CONFIG_MAX_UNITS];

static bcmlrd_sid_t attr_sid[2] = {CTR_ING_FLEX_ATTR_TEMPLATEt,
                                   CTR_EGR_FLEX_ATTR_TEMPLATEt};

static bcmlrd_sid_t entry_sid[2] = {CTR_ING_FLEX_ENTRYt,
                                   CTR_EGR_FLEX_ENTRYt};

static bcmlrd_fid_t pkt_attr_fid[2] =
              {CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_ING_FLEX_PKT_ATTR_TEMPLATE_IDf,
               CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_EGR_FLEX_PKT_ATTR_TEMPLATE_IDf};

static bcmlrd_fid_t attr_fid[2] =
              {CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_ING_FLEX_ATTR_TEMPLATE_IDf,
               CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_EGR_FLEX_ATTR_TEMPLATE_IDf};

static bcmlrd_fid_t instance_fid[2] =
              {CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_FLEX_INSTANCEf,
               CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_FLEX_INSTANCEf};

static bcmlrd_sid_t pkt_attr_sid[2] = {CTR_ING_FLEX_PKT_ATTR_TEMPLATEt,
                                 CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt};

static bcmlrd_fid_t pkt_attr_pkt_attr_fid[2] =
                 {CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_CTR_ING_FLEX_PKT_ATTR_TEMPLATE_IDf,
                  CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_CTR_EGR_FLEX_PKT_ATTR_TEMPLATE_IDf};

static bcmlrd_fid_t entry_fid[2] =
             {CTR_ING_FLEX_ENTRYt_CTR_ING_FLEX_ENTRY_IDf,
              CTR_EGR_FLEX_ENTRYt_CTR_EGR_FLEX_ENTRY_IDf};

static int
ctr_flex_ctrl_get(int unit,
                  ctr_flex_control_t **ctrl)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    SHR_NULL_CHECK(ctr_flex_control[unit], SHR_E_PARAM);
    *ctrl = ctr_flex_control[unit];

exit:
    SHR_FUNC_EXIT();
}

static int
ctr_flex_data_driver_get(int unit, const bcmptm_ctr_flex_data_driver_t **cb)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_NULL_CHECK(bcmptm_ctr_flex_data_cb[unit], SHR_E_PARAM);
    *cb = bcmptm_ctr_flex_data_cb[unit];

exit:
    SHR_FUNC_EXIT();
}

static int
recover_flex_pkt_attr_state(int unit, bool ingress)
{
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t key_fields = {0};
    bcmltd_field_t *data_field;
    uint32_t pkt_attr_id = 0;
    uint32_t i;
    size_t num_fields = 0;
    int rv, select;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    select  = ingress ? 0 : 1;

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &key_fields,
                                       1));

    key_fields.count = 1;
    key_fields.field[0]->id = pkt_attr_pkt_attr_fid[select];

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, pkt_attr_sid[select], &num_fields));

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /* get_first entry from table in IMM */
    rv = bcmimm_entry_get_first(unit,
                                pkt_attr_sid[select],
                                &imm_fields);

    while (rv == SHR_E_NONE) {
        /* extract pkt_attr_id */
        for (i = 0; i < imm_fields.count; i++) {
            data_field = imm_fields.field[i];
            if (data_field->id == pkt_attr_pkt_attr_fid[select]) {
                pkt_attr_id = data_field->data;
            }
        }

        /* Entry found, set the active flag */
        if (ingress) {
            ctrl->ing_pkt_attributes[pkt_attr_id].active = true;
        }
        else {
            ctrl->egr_pkt_attributes[pkt_attr_id].active = true;
        }

        /* setup the next key */
        key_fields.field[0]->data = pkt_attr_id;

        /* get_next FLEX_ENTRY entry from IMM */
        rv = bcmimm_entry_get_next(unit,
                                   pkt_attr_sid[select],
                                   &key_fields,
                                   &imm_fields);
    }

    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

exit:
    bcmptm_ctr_flex_flds_free(unit, &key_fields);
    imm_fields.count = num_fields;
    bcmptm_ctr_flex_flds_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

static int
recover_flex_attr_state(int unit, bool ingress)
{
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t key_fields = {0};
    bcmltd_field_t *data_field;
    uint32_t i;
    uint32_t attr_id=0;
    uint32_t instance_id=0;
    uint32_t pkt_attr_id=0;
    size_t num_fields = 0;
    llist_t **llist;
    int rv, select;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    select  = ingress ? 0 : 1;

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &key_fields,
                                       2));

    key_fields.count = 2;
    key_fields.field[0]->id = attr_fid[select];
    key_fields.field[1]->id = instance_fid[select];

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, attr_sid[select], &num_fields));

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /* get_first entry from table in IMM */
    rv = bcmimm_entry_get_first(unit,
                                attr_sid[select],
                                &imm_fields);

    while (rv == SHR_E_NONE) {
        /* extract attr_id, instance_id, pkt_attr_id */
        for (i = 0; i < imm_fields.count; i++) {
            data_field = imm_fields.field[i];
            if (data_field->id == pkt_attr_fid[select]) {
                pkt_attr_id = data_field->data;
            }
            else if (data_field->id == attr_fid[select]) {
                attr_id = data_field->data;
            }
            else if (data_field->id == instance_fid[select]) {
                instance_id = data_field->data;
            }
        }

        /* Entry found, subscribe to pkt attr template */
        llist = ingress ?
                &ctrl->ing_pkt_attributes[pkt_attr_id].ref_entry_list:
                &ctrl->egr_pkt_attributes[pkt_attr_id].ref_entry_list;

        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_SUBSCRIBE,
                                      llist,
                                      attr_id));
        if (ingress) {
            ctrl->ing_attributes[attr_id].num_instances++;
        }
        else {
            ctrl->egr_attributes[attr_id].num_instances++;
        }

        /* setup the next key */
        key_fields.field[0]->data = attr_id;
        key_fields.field[1]->data = instance_id;

        /* get_next FLEX_ENTRY entry from IMM */
        rv = bcmimm_entry_get_next(unit,
                                 attr_sid[select],
                                 &key_fields,
                                 &imm_fields);
    }

    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

exit:
    bcmptm_ctr_flex_flds_free(unit, &key_fields);
    imm_fields.count = num_fields;
    bcmptm_ctr_flex_flds_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

static int
recover_flex_entry_state(int unit, bool ingress)
{
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t key_fields = {0};
    uint32_t entry_id = 0;
    uint32_t attr_id = 0;
    size_t num_fields = 0;
    llist_t **llist;
    ctr_flex_user_data_t user_data = {0};
    ctr_flex_notify_entry_opcode_t entry_opcode = {0};
    ctr_flex_entry_data_t new_entry_data = {0};
    int rv, select;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    select  = ingress ? 0 : 1;

    user_data.ingress = ingress;
    user_data.data_type = CTR_FLEX_ENTRY_DATA;

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &key_fields,
                                       1));

    key_fields.count = 1;
    key_fields.field[0]->id = entry_fid[select];

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, entry_sid[select], &num_fields));

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /* get_first entry from table in IMM */
    rv = bcmimm_entry_get_first(unit,
                                entry_sid[select],
                                &imm_fields);

    while (rv == SHR_E_NONE) {
        /* Parse the flex entry fiels into entry_data */
        SHR_IF_ERR_EXIT
            (parse_flex_entry (unit,
                               ingress,
                               &imm_fields,
                               &user_data.entry_data));

        entry_id = user_data.entry_data.entry_id;
        attr_id  = user_data.entry_data.attr_id;

        /* Process the active entry */
        entry_opcode.insert = true;

        SHR_IF_ERR_EXIT
            (ctr_flex_notify_entry_template (unit,
                                             &entry_opcode,
                                             &user_data,
                                             &new_entry_data,
                                             NULL));

        /* subscribe new entry_id to attr_template_id */
        llist = ingress ? &ctrl->ing_attributes[attr_id].ref_entry_list :
                          &ctrl->egr_attributes[attr_id].ref_entry_list ;

        SHR_IF_ERR_EXIT
            (ctr_flex_subscribe_list (unit,
                                      CTR_FLEX_LIST_SUBSCRIBE,
                                      llist,
                                      entry_id));

        /* setup the next key */
        key_fields.field[0]->data = entry_id;

        /* get_next FLEX_ENTRY entry from IMM */
        rv = bcmimm_entry_get_next(unit,
                                 entry_sid[select],
                                 &key_fields,
                                 &imm_fields);
    }

    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

exit:
    bcmptm_ctr_flex_flds_free(unit, &key_fields);
    imm_fields.count = num_fields;
    bcmptm_ctr_flex_flds_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

static int
ctr_flex_warm_recover(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* 1a. Recover ing_pkt_attr state from ING FLEX_PKT_ATTR IMM table */
    SHR_IF_ERR_EXIT
        (recover_flex_pkt_attr_state(unit, true));

    /* 1b. Recover egr_pkt_attr state from EGR FLEX_PKT_ATTR IMM table */
    SHR_IF_ERR_EXIT
        (recover_flex_pkt_attr_state(unit, false));

    /* 2a. Recover ing_attr state from ING FLEX_ATTR IMM table */
    SHR_IF_ERR_EXIT
        (recover_flex_attr_state(unit, true));

    /* 2b. Recover egr_attr state from EGR FLEX_ATTR IMM table */
    SHR_IF_ERR_EXIT
        (recover_flex_attr_state(unit, false));

    /* 3a. Recover ing_entry state from ING FLEX_ENTRY IMM table */
    SHR_IF_ERR_EXIT
        (recover_flex_entry_state(unit, true));

    /* 3b. Recover egr_entry state from EGR FLEX_ENTRY IMM table */
    SHR_IF_ERR_EXIT
        (recover_flex_entry_state(unit, false));

exit:
    SHR_FUNC_EXIT();
}

static void
delete_list (llist_t **head)
{
    llist_t *curr = *head;
    while (curr) {
        llist_t *temp = curr;
        curr = curr->next;
        SHR_FREE(temp);
    }
    *head = NULL;
}

static int
ctr_flex_db_cleanup(int unit)
{
    size_t pool, pipe, i;
    ctr_flex_control_t *ctrl = NULL;
    ctr_flex_device_info_t *device_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ctrl->ctr_flex_device_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    device_info = ctrl->ctr_flex_device_info;

    /* Destroy pool lists */
    if (ctrl->ctr_flex_device_info) {
        for (pool = 0; pool < device_info->num_ingress_pools; pool++) {
            for (pipe = 0; pipe < device_info->num_pipes; pipe++) {
                bcmptm_ctr_flex_pool_mgr_destroy(&ctrl->ing_pool_list[pool][pipe]);
            }
        }

        for (pool = 0; pool < device_info->num_egress_pools; pool++) {
            for (pipe = 0; pipe < device_info->num_pipes; pipe++) {
                bcmptm_ctr_flex_pool_mgr_destroy(&ctrl->egr_pool_list[pool][pipe]);
            }
        }
    }

    /* Free allocated memory in ctr_flex_control_t */

    if (ctrl->ing_attributes) {
        /* Delete each ing_attribute entry's ref_entry_list */
        for (i = 0;
             i< ctrl->ctr_flex_table_sizes.max_ing_attr_entries;
             i++) {
            if (ctrl->ing_attributes[i].ref_entry_list) {
                delete_list(&ctrl->ing_attributes[i].ref_entry_list);
            }
        }
        SHR_FREE(ctrl->ing_attributes);
    }

    if (ctrl->egr_attributes) {
        /* Delete each egr_attribute entry's ref_entry_list */
        for (i = 0;
             i < ctrl->ctr_flex_table_sizes.max_egr_attr_entries;
             i++) {
            if (ctrl->egr_attributes[i].ref_entry_list) {
                delete_list(&ctrl->egr_attributes[i].ref_entry_list);
            }
        }
        SHR_FREE(ctrl->egr_attributes);
    }

    if (ctrl->ing_pkt_attributes) {
        /* Delete each ing_pkt_attribute entry's ref_entry_list */
        for (i = 0;
             i < ctrl->ctr_flex_table_sizes.max_ing_pkt_attr_entries;
             i++) {
            if (ctrl->ing_pkt_attributes[i].ref_entry_list) {
                delete_list(&ctrl->ing_pkt_attributes[i].ref_entry_list);
            }
        }
        SHR_FREE(ctrl->ing_pkt_attributes);
    }

    if (ctrl->egr_pkt_attributes) {
        /* Delete each egr_pkt_attribute entry's ref_entry_list */
        for (i = 0;
             i < ctrl->ctr_flex_table_sizes.max_egr_pkt_attr_entries;
             i++) {
            if (ctrl->egr_pkt_attributes[i].ref_entry_list) {
                delete_list(&ctrl->egr_pkt_attributes[i].ref_entry_list);
            }
        }
        SHR_FREE(ctrl->egr_pkt_attributes);
    }

    if (ctrl->ing_wait_list) {
        delete_list(&ctrl->ing_wait_list);
    }

    if (ctrl->egr_wait_list) {
        delete_list(&ctrl->egr_wait_list);
    }

exit:
    if (ctrl) {
        SHR_FREE(ctrl);
        ctr_flex_control[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
get_table_sizes (int unit, ctr_flex_table_sizes_t *table_sizes)
{
    bcmlrd_table_attrib_t t_attrib;

    SHR_FUNC_ENTER(unit);

    /* Get table sizes */
    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     "CTR_ING_FLEX_PKT_ATTR_TEMPLATE",
                                     &t_attrib));

    table_sizes->max_ing_pkt_attr_entries = t_attrib.max_entries;

    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     "CTR_EGR_FLEX_PKT_ATTR_TEMPLATE",
                                     &t_attrib));

    table_sizes->max_egr_pkt_attr_entries = t_attrib.max_entries;

    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     "CTR_ING_FLEX_ATTR_TEMPLATE",
                                     &t_attrib));

    table_sizes->max_ing_attr_entries = t_attrib.max_entries;

    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     "CTR_EGR_FLEX_ATTR_TEMPLATE",
                                     &t_attrib));

    table_sizes->max_egr_attr_entries = t_attrib.max_entries;

    /* sanity checks */
    if (table_sizes->max_egr_pkt_attr_entries == 0 ||
        table_sizes->max_ing_pkt_attr_entries == 0 ||
        table_sizes->max_ing_attr_entries == 0 ||
        table_sizes->max_egr_attr_entries == 0 ||
        table_sizes->max_egr_pkt_attr_entries > MAX_CTR_TABLE_DEPTH ||
        table_sizes->max_ing_pkt_attr_entries > MAX_CTR_TABLE_DEPTH ||
        table_sizes->max_ing_attr_entries > MAX_CTR_TABLE_DEPTH ||
        table_sizes->max_egr_attr_entries > MAX_CTR_TABLE_DEPTH) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
     }
exit:
    SHR_FUNC_EXIT();
}

static int
ctr_flex_get_lt_enum_info(int unit)
{
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      CTR_ING_FLEX_ENTRYt,
                                      CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                      "OPERATIONAL_STATE_OK",
                                      &ctrl->ctr_flex_enum.operational_state_ok));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      CTR_ING_FLEX_ENTRYt,
                                      CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                      "ATTR_TEMPLATE_NOT_PRESENT",
                                      &ctrl->ctr_flex_enum.attr_template_not_present));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      CTR_ING_FLEX_ENTRYt,
                                      CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                      "PKT_ATTR_TEMPLATE_NOT_PRESENT",
                                      &ctrl->ctr_flex_enum.pkt_attr_template_not_present));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      CTR_ING_FLEX_ENTRYt,
                                      CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                      "ATTR_TEMPLATE_INSTANCES_EXCEEDED",
                                      &ctrl->ctr_flex_enum.attr_template_exceeded));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      CTR_ING_FLEX_ENTRYt,
                                      CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                      "PKT_ATTR_TEMPLATE_WIDTH_EXCEEDED",
                                      &ctrl->ctr_flex_enum.pkt_attr_template_exceeded));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      CTR_ING_FLEX_ENTRYt,
                                      CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                      "AWAITING_COUNTER_RESOURCE",
                                      &ctrl->ctr_flex_enum.awaiting_counter_resource));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      CTR_ING_FLEX_ENTRYt,
                                      CTR_ING_FLEX_ENTRYt_OFFSET_MODEf,
                                      "DIRECT_MAP_MODE",
                                      &ctrl->ctr_flex_enum.direct_map_mode));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      CTR_ING_FLEX_ENTRYt,
                                      CTR_ING_FLEX_ENTRYt_OFFSET_MODEf,
                                      "UNCOMPRESSED_MODE",
                                      &ctrl->ctr_flex_enum.uncompressed_mode));
exit:
    SHR_FUNC_EXIT();
}

static int
ctr_flex_db_init(int unit, bool warm)
{
    size_t mem_size;
    ctr_flex_control_t *ctrl = NULL;
    ctr_flex_device_info_t *device_info = NULL;
    const bcmptm_ctr_flex_data_driver_t *cb = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get initialized flex counter control data structure */
    SHR_IF_ERR_EXIT
        (ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get device-specific driver callback routine */
    SHR_IF_ERR_EXIT
        (ctr_flex_data_driver_get(unit, &cb));

    if (cb == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (get_table_sizes (unit, &ctrl->ctr_flex_table_sizes));

    /* Allocate memory for table references */
    mem_size = sizeof(ctr_flex_pkt_attributes_t) *
               ctrl->ctr_flex_table_sizes.max_ing_pkt_attr_entries;

    SHR_ALLOC(ctrl->ing_pkt_attributes, mem_size,
              "bcmptmCtrFlexIngPktAttrib");

    SHR_NULL_CHECK(ctrl->ing_pkt_attributes, SHR_E_MEMORY);

    sal_memset(ctrl->ing_pkt_attributes, 0, mem_size);

    mem_size = sizeof(ctr_flex_pkt_attributes_t) *
               ctrl->ctr_flex_table_sizes.max_egr_pkt_attr_entries;

    SHR_ALLOC(ctrl->egr_pkt_attributes, mem_size,
              "bcmptmCtrFlexEgrPktAttrib");

    SHR_NULL_CHECK(ctrl->egr_pkt_attributes, SHR_E_MEMORY);

    sal_memset(ctrl->egr_pkt_attributes, 0, mem_size);

    mem_size = sizeof(ctr_flex_attributes_t) *
               ctrl->ctr_flex_table_sizes.max_ing_attr_entries;

    SHR_ALLOC(ctrl->ing_attributes, mem_size,
              "bcmptmCtrFlexIngAttrib");

    SHR_NULL_CHECK(ctrl->ing_attributes, SHR_E_MEMORY);

    sal_memset(ctrl->ing_attributes, 0, mem_size);

    mem_size = sizeof(ctr_flex_attributes_t) *
               ctrl->ctr_flex_table_sizes.max_egr_attr_entries;

    SHR_ALLOC(ctrl->egr_attributes, mem_size,
              "bcmptmCtrFlexEgrAttrib");

    SHR_NULL_CHECK(ctrl->egr_attributes, SHR_E_MEMORY);

    sal_memset(ctrl->egr_attributes, 0, mem_size);

    /* Populate ctr_flex_device_info structure. */
    SHR_IF_ERR_EXIT
        (cb->dev_info_get_fn(unit, &ctrl->ctr_flex_device_info));

    if (ctrl->ctr_flex_device_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    device_info = ctrl->ctr_flex_device_info;

    if (device_info->num_ingress_pools > MAX_INGRESS_POOLS ||
        device_info->num_egress_pools > MAX_EGRESS_POOLS ||
        device_info->num_pipes > MAX_NUM_PIPES) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Init the offset table entry width in device file */
    SHR_IF_ERR_EXIT
        (cb->offset_table_init_fn(unit));

    /* Read ingress packet types from device file */
    SHR_IF_ERR_EXIT
        (cb->ing_pkt_types_get_fn(unit, &device_info->ing_pkt_types));

    /* Read ingress packet attribute bits from device file */
    SHR_IF_ERR_EXIT
        (cb->ing_pkt_attr_bits_get_fn(unit,
                                      &device_info->ing_pkt_attr_bits));

    /* Read egress packet types from device file */
    SHR_IF_ERR_EXIT
        (cb->egr_pkt_types_get_fn(unit, &device_info->egr_pkt_types));

    /* Read egress packet attribute bits from device file */
    SHR_IF_ERR_EXIT
        (cb->egr_pkt_attr_bits_get_fn(unit,
                                      &device_info->egr_pkt_attr_bits));

    /* Read pool info from device file and fill POOL_INFO LT */
    SHR_IF_ERR_EXIT(ctr_flex_populate_ing_pool_info(unit, warm));

    SHR_IF_ERR_EXIT(ctr_flex_populate_egr_pool_info(unit, warm));

    /* Get the field info for LT ENTRY and LT STATS */
    SHR_IF_ERR_EXIT(ctr_flex_get_lt_field_info(unit));

    /* Get the enum used in the flex ctr LTs */
    SHR_IF_ERR_EXIT(ctr_flex_get_lt_enum_info(unit));

    /* Recover state from IMM in case of warm-boot */
    if (warm) {
        SHR_IF_ERR_EXIT(ctr_flex_warm_recover(unit));
    }

    SHR_FUNC_EXIT();

exit:
    ctr_flex_db_cleanup(unit);
    SHR_FUNC_EXIT();
}

static int
ctr_flex_init(int unit, bool warm)
{
    int rv;
    bcmlrd_table_attrib_t t_attrib;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_table_attributes_get(unit,
                                     "CTR_ING_FLEX_POOL_INFO",
                                     &t_attrib);

    /* If CTR_ING_FLEX_POOL_INFO LT doesn't exist, exit */
    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    /* If CTR_EGR_FLEX_POOL_INFO LT doesn't exist, exit */
    rv = bcmlrd_table_attributes_get(unit,
                                     "CTR_EGR_FLEX_POOL_INFO",
                                     &t_attrib);

    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    /* Allocate control structure */
    ctr_flex_control[unit] = NULL;

    SHR_ALLOC(ctr_flex_control[unit],
              sizeof(ctr_flex_control_t),
              "bcmptmCtrFlexControl");
    SHR_NULL_CHECK(ctr_flex_control[unit], SHR_E_MEMORY);
    sal_memset(ctr_flex_control[unit], 0, sizeof(ctr_flex_control_t));

    /* Create and initialize the flex counter database */
    SHR_IF_ERR_EXIT(ctr_flex_db_init(unit, warm));

    /* Register IMM callback routines */
    SHR_IF_ERR_EXIT(ctr_flex_imm_lt_register(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_driver_init(int unit, bool warm)
{

    SHR_FUNC_ENTER(unit);

    /* Register device-specific driver */
    bcmptm_ctr_flex_data_cb[unit] = bcmptm_ctr_flex_data_driver_register(unit);

    if (bcmptm_ctr_flex_data_cb[unit] == NULL) {
        /*
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "FLEX CTR driver missing.\n")));
        */
        ctr_flex_supported[unit] = false;
    }
    else {
        ctr_flex_supported[unit] = true;
    }

    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_init(int unit, bool warm)
{

    SHR_FUNC_ENTER(unit);

    if (!ctr_flex_supported[unit]) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(ctr_flex_init(unit, warm));

    SHR_FUNC_EXIT();

exit:
    /* error during init */
    if (ctr_flex_supported[unit]) {
        ctr_flex_db_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_cleanup(int unit)
{

    SHR_FUNC_ENTER(unit);

    if (!ctr_flex_supported[unit]) {
        SHR_EXIT();
    }

    ctr_flex_db_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/* Get flex counter control structure */
int
bcmptm_ctr_flex_ctrl_get(int unit,
                         ctr_flex_control_t **ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ctr_flex_ctrl_get(unit, ctrl));

exit:
    SHR_FUNC_EXIT();
}

/* Get flex counter device-specific driver callback */
int
bcmptm_ctr_flex_data_driver_get(int unit,
                                const bcmptm_ctr_flex_data_driver_t **cb)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ctr_flex_data_driver_get(unit, cb));

exit:
    SHR_FUNC_EXIT();
}
