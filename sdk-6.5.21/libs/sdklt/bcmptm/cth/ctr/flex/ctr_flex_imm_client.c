/*! \file ctr_flex_imm_client.c
 *
 * Flex counter IMM lookup, register, update routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmimm/bcmimm_int_comp.h>

static bcmlrd_sid_t attr_sid[2] = {CTR_ING_FLEX_ATTR_TEMPLATEt,
                                   CTR_EGR_FLEX_ATTR_TEMPLATEt};
static bcmlrd_sid_t entry_sid[2] = {CTR_ING_FLEX_ENTRYt, CTR_EGR_FLEX_ENTRYt};
static bcmlrd_fid_t entry_fid[2] = {CTR_ING_FLEX_ENTRYt_CTR_ING_FLEX_ENTRY_IDf,
                                    CTR_EGR_FLEX_ENTRYt_CTR_EGR_FLEX_ENTRY_IDf};
static bcmlrd_sid_t pkt_attr_sid[2] = {CTR_ING_FLEX_PKT_ATTR_TEMPLATEt,
                                       CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt};
static bcmlrd_fid_t op_fid[2] = {CTR_ING_FLEX_ENTRYt_OPERATIONAL_STATEf,
                                 CTR_EGR_FLEX_ENTRYt_OPERATIONAL_STATEf};
static bcmlrd_fid_t entry_attr_fid[2] = {CTR_ING_FLEX_ENTRYt_CTR_ING_FLEX_ATTR_TEMPLATE_IDf,
                                         CTR_EGR_FLEX_ENTRYt_CTR_EGR_FLEX_ATTR_TEMPLATE_IDf};
static bcmlrd_fid_t attr_attr_fid[2] =
              {CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_ING_FLEX_ATTR_TEMPLATE_IDf,
               CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_EGR_FLEX_ATTR_TEMPLATE_IDf};
static bcmlrd_fid_t pool_fid[2] = {CTR_ING_FLEX_ENTRYt_POOL_IDf,
                                   CTR_EGR_FLEX_ENTRYt_POOL_IDf};
static bcmlrd_fid_t base_index_auto_fid[2] = {CTR_ING_FLEX_ENTRYt_BASE_INDEX_AUTOf,
                                              CTR_EGR_FLEX_ENTRYt_BASE_INDEX_AUTOf};
static bcmlrd_fid_t base_index_oper_fid[2] = {CTR_ING_FLEX_ENTRYt_BASE_INDEX_OPERf,
                                              CTR_EGR_FLEX_ENTRYt_BASE_INDEX_OPERf};
static bcmlrd_fid_t base_index_fid[2] = {CTR_ING_FLEX_ENTRYt_BASE_INDEXf,
                                         CTR_EGR_FLEX_ENTRYt_BASE_INDEXf};
static bcmlrd_fid_t pipe_unique_fid[2] = {CTR_ING_FLEX_ENTRYt_PIPE_UNIQUEf,
                                          CTR_EGR_FLEX_ENTRYt_PIPE_UNIQUEf};
static bcmlrd_fid_t pipe_num_fid[2] = {CTR_ING_FLEX_ENTRYt_PIPEf,
                                       CTR_EGR_FLEX_ENTRYt_PIPEf};
static bcmlrd_fid_t offset_fid[2] = {CTR_ING_FLEX_ENTRYt_OFFSET_MODEf,
                                     CTR_EGR_FLEX_ENTRYt_OFFSET_MODEf};
static bcmlrd_fid_t max_instances_auto_fid[2] = {CTR_ING_FLEX_ENTRYt_MAX_INSTANCES_AUTOf,
                                                 CTR_EGR_FLEX_ENTRYt_MAX_INSTANCES_AUTOf};
static bcmlrd_fid_t max_instances_oper_fid[2] = {CTR_ING_FLEX_ENTRYt_MAX_INSTANCES_OPERf,
                                                 CTR_EGR_FLEX_ENTRYt_MAX_INSTANCES_OPERf};
static bcmlrd_fid_t max_instances_fid[2] = {CTR_ING_FLEX_ENTRYt_MAX_INSTANCESf,
                                            CTR_EGR_FLEX_ENTRYt_MAX_INSTANCESf};
static bcmlrd_fid_t instance_fid[2] =
              {CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_FLEX_INSTANCEf,
               CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_FLEX_INSTANCEf};
static bcmlrd_fid_t pkt_attr_fid[2] =
              {CTR_ING_FLEX_ATTR_TEMPLATEt_CTR_ING_FLEX_PKT_ATTR_TEMPLATE_IDf,
              CTR_EGR_FLEX_ATTR_TEMPLATEt_CTR_EGR_FLEX_PKT_ATTR_TEMPLATE_IDf};
static bcmlrd_fid_t pkt_attr_pkt_attr_fid[2] =
                 {CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_CTR_ING_FLEX_PKT_ATTR_TEMPLATE_IDf,
                  CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_CTR_EGR_FLEX_PKT_ATTR_TEMPLATE_IDf};

/*!
 * Lookup FLEX_ENTRY table given entry_id.
 * Return data fields. Return SHR_E_ if entry not found.
 */
int
lookup_flex_entry_table (int unit,
                         bool ingress,
                         uint32_t entry_id,
                         ctr_flex_entry_data_t *entry_data)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0;
    int select;

    SHR_FUNC_ENTER(unit);

    select  = ingress ? 0 : 1;

    if (entry_data == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &key_fields,
                                       1));

    key_fields.field[0]->id   = entry_fid[select];
    key_fields.field[0]->data = entry_id;

    entry_data->entry_id = entry_id;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, entry_sid[select], &num_fields));

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /* lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             entry_sid[select],
                             &key_fields,
                             &imm_fields));

    /* Parse the flex entry fiels into entry_data */
    SHR_IF_ERR_EXIT
        (parse_flex_entry (unit,
                           ingress,
                           &imm_fields,
                           entry_data));

exit:
    bcmptm_ctr_flex_flds_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmptm_ctr_flex_flds_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/*!
 * Convert field list to entry_data.
 */
int
parse_flex_entry (int unit,
                  bool ingress,
                  bcmltd_fields_t *imm_fields,
                  ctr_flex_entry_data_t *entry_data)
{
    uint32_t base_index_auto;
    uint32_t base_index_input;
    uint32_t base_index_oper;
    uint32_t max_instances_auto;
    uint32_t max_instances_input;
    uint32_t max_instances_oper;
    bcmltd_field_t *data_field;
    ctr_flex_entry_data_t default_value = {0};
    uint32_t i;
    int select;

    SHR_FUNC_ENTER(unit);

    select  = ingress ? 0 : 1;

    /* assign defaults */
    SHR_IF_ERR_EXIT
            (ctr_flex_entry_get_field_default(unit,
                                     ingress,
                                     &default_value));

    entry_data->pool_id = default_value.pool_id;
    entry_data->attr_id = default_value.attr_id;
    entry_data->offset_mode = default_value.offset_mode;
    entry_data->pipe_num = default_value.pipe_num;
    entry_data->pipe_unique = default_value.pipe_unique;

    base_index_input = default_value.base_index;
    base_index_oper = default_value.base_index;
    base_index_auto = default_value.base_index_mode_auto;

    max_instances_input = default_value.max_instances;
    max_instances_oper = default_value.max_instances;
    max_instances_auto = default_value.max_instances_mode_auto;

    for (i = 0; i < imm_fields->count; i++) {
        data_field = imm_fields->field[i];
        if (data_field->id == entry_fid[select]) {
            /* imm_fields contains the key field for get_first/next */
            entry_data->entry_id = data_field->data;
        }
        else if (data_field->id == entry_attr_fid[select]) {
            entry_data->attr_id = data_field->data;
        }
        else if (data_field->id == pool_fid[select]) {
            entry_data->pool_id = data_field->data;
        }
        else if (data_field->id == base_index_auto_fid[select]) {
            base_index_auto = data_field->data;
            entry_data->base_index_mode_auto_valid = true;
        }
        else if (data_field->id == base_index_oper_fid[select]) {
            base_index_oper = data_field->data;
        }
        else if (data_field->id == base_index_fid[select]) {
            base_index_input = data_field->data;
        }
        else if (data_field->id == offset_fid[select]) {
            entry_data->offset_mode = data_field->data;
        }
        else if (data_field->id == op_fid[select]) {
            entry_data->op_state = data_field->data;
        }
        else if (data_field->id == pipe_num_fid[select]) {
            entry_data->pipe_num = data_field->data;
        }
        else if (data_field->id == pipe_unique_fid[select]) {
            entry_data->pipe_unique = data_field->data;
        }
        else if (data_field->id == max_instances_auto_fid[select]) {
            max_instances_auto = data_field->data;
            entry_data->max_instances_mode_auto_valid = true;
        }
        else if (data_field->id == max_instances_oper_fid[select]) {
            max_instances_oper = data_field->data;
        }
        else if (data_field->id == max_instances_fid[select]) {
            max_instances_input = data_field->data;
        }
    }

   entry_data->base_index_mode_auto = base_index_auto;
   entry_data->max_instances_mode_auto = max_instances_auto;

   entry_data->base_index = base_index_auto ?
                            base_index_oper : base_index_input;

   entry_data->max_instances = max_instances_auto ?
                               max_instances_oper : max_instances_input;
exit:
    SHR_FUNC_EXIT();
}


/*!
 * Lookup ATTR_TEMPLATE table given attr_template_id and instance_id.
 * Return pkt_attr_template_id. Return SHR_E_ if entry not found.
 * user_data, if any, is entry still being processed in stage_cb.
 */
int
lookup_flex_attr_table (int unit,
                        bool ingress,
                        uint32_t attr_template_id,
                        uint32_t instance,
                        ctr_flex_user_data_t *user_data,
                        uint32_t *pkt_attr_template_id)
{
    int rv;
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields = 0;
    size_t i;
    int select;
    SHR_FUNC_ENTER(unit);

    select  = ingress ? 0 : 1;

    /* Check user data first */
    if (user_data != NULL &&
        user_data->data_type == CTR_FLEX_ATTR_DATA &&
        user_data->attr_data.attr_id == attr_template_id &&
        user_data->attr_data.instance_id == instance) {

        if (user_data->event_reason == BCMIMM_ENTRY_DELETE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (pkt_attr_template_id) {
            *pkt_attr_template_id = user_data->attr_data.pkt_attr_id;
        }

        SHR_EXIT();
    }

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &key_fields,
                                       2));

    key_fields.field[0]->id   = attr_attr_fid[select];
    key_fields.field[0]->data = attr_template_id;
    key_fields.field[1]->id   = instance_fid[select];
    key_fields.field[1]->data = instance;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, attr_sid[select], &num_fields));

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /*
     * Lookup IMM table;
     * exit with error if entry not found, but no message.
     */
    rv = bcmimm_entry_lookup(unit,
                             attr_sid[select],
                             &key_fields,
                             &imm_fields);

    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    flist = &imm_fields;

    if (pkt_attr_template_id) {
        *pkt_attr_template_id = 0;
        for (i=0; i < flist->count; i++) {
            data_field = flist->field[i];
            if (data_field->id == pkt_attr_fid[select]) {
                *pkt_attr_template_id = data_field->data;
            }
        }
    }

exit:
    bcmptm_ctr_flex_flds_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmptm_ctr_flex_flds_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/*!
 * Convert field list to ingress packet attributes struct.
 */
void
extract_flex_ing_pkt_attribute_fields(
    ctr_flex_pkt_attribute_template_t *ing_pkt_attribute_template,
    const bcmltd_field_t *data_field)
{
        switch (data_field->id) {
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_IPf:
            ing_pkt_attribute_template->ip = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_DROPf:
            ing_pkt_attribute_template->drop = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_NETWORK_SVPf:
            ing_pkt_attribute_template->network_svp = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_ACCESS_SVPf:
            ing_pkt_attribute_template->access_svp = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_ALLf:
            ing_pkt_attribute_template->pkt_type_all = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWNf:
            ing_pkt_attribute_template->pkt_type_unknown = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_CONTROLf:
            ing_pkt_attribute_template->pkt_type_control = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_OAMf:
            ing_pkt_attribute_template->pkt_type_oam = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_BFDf:
            ing_pkt_attribute_template->pkt_type_bfd = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_BPDUf:
            ing_pkt_attribute_template->pkt_type_bpdu = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_ICNMf:
            ing_pkt_attribute_template->pkt_type_icnm = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_1588f:
            ing_pkt_attribute_template->pkt_type_1588 = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_L2UCf:
            ing_pkt_attribute_template->pkt_type_known_l2uc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWN_L2UCf:
            ing_pkt_attribute_template->pkt_type_unknown_l2uc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_L2BCf:
            ing_pkt_attribute_template->pkt_type_l2bc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_L2MCf:
            ing_pkt_attribute_template->pkt_type_known_l2mc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWN_L2MCf:
            ing_pkt_attribute_template->pkt_type_unknown_l2mc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_L3UCf:
            ing_pkt_attribute_template->pkt_type_known_l3uc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWN_L3UCf:
            ing_pkt_attribute_template->pkt_type_unknown_l3uc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_IPMCf:
            ing_pkt_attribute_template->pkt_type_known_ipmc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWN_IPMCf:
            ing_pkt_attribute_template->pkt_type_unknown_ipmc = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_MPLS_L2f:
            ing_pkt_attribute_template->pkt_type_known_mpls_l2 = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_MPLS_L3f:
            ing_pkt_attribute_template->pkt_type_known_mpls_l3 = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_MPLSf:
            ing_pkt_attribute_template->pkt_type_known_mpls = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWN_MPLSf:
            ing_pkt_attribute_template->pkt_type_unknown_mpls = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_MIMf:
            ing_pkt_attribute_template->pkt_type_known_mim = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWN_MIMf:
            ing_pkt_attribute_template->pkt_type_unknown_mim = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_TRILLf:
            ing_pkt_attribute_template->pkt_type_known_trill = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWN_TRILLf:
            ing_pkt_attribute_template->pkt_type_unknown_trill = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_KNOWN_NIVf:
            ing_pkt_attribute_template->pkt_type_known_niv = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNKNOWN_NIVf:
            ing_pkt_attribute_template->pkt_type_unknown_niv = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TOS_ECNf:
            ing_pkt_attribute_template->tos_ecn = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TOS_DSCPf:
            ing_pkt_attribute_template->tos_dscp = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PORT_IDf:
            ing_pkt_attribute_template->port = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_INT_CNf:
            ing_pkt_attribute_template->int_cn = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_INNER_DOT1Pf:
            ing_pkt_attribute_template->inner_dot1p = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_OUTER_DOT1Pf:
            ing_pkt_attribute_template->outer_dot1p = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_VLAN_UTf:
            ing_pkt_attribute_template->vlan_ut = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_VLAN_SITf:
            ing_pkt_attribute_template->vlan_sit = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_VLAN_SOTf:
            ing_pkt_attribute_template->vlan_sot = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_VLAN_DTf:
            ing_pkt_attribute_template->vlan_dt = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_INT_PRIf:
            ing_pkt_attribute_template->int_pri = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_FP_G_COLORf:
            ing_pkt_attribute_template->fp_g_color = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_FP_Y_COLORf:
            ing_pkt_attribute_template->fp_y_color = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_FP_R_COLORf:
            ing_pkt_attribute_template->fp_r_color = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_G_COLORf:
            ing_pkt_attribute_template->pre_fp_g_color = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_Y_COLORf:
            ing_pkt_attribute_template->pre_fp_y_color = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_R_COLORf:
            ing_pkt_attribute_template->pre_fp_r_color = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_ELEPHANT_PKTf:
            ing_pkt_attribute_template->elephant_pkt = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TCP_FLAG_TYPE_FINf:
            ing_pkt_attribute_template->tcp_flag_type_fin = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TCP_FLAG_TYPE_SYNf:
            ing_pkt_attribute_template->tcp_flag_type_syn = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TCP_FLAG_TYPE_RSTf:
            ing_pkt_attribute_template->tcp_flag_type_rst = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TCP_FLAG_TYPE_PSHf:
            ing_pkt_attribute_template->tcp_flag_type_psh = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TCP_FLAG_TYPE_ACKf:
            ing_pkt_attribute_template->tcp_flag_type_ack = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TCP_FLAG_TYPE_URGf:
            ing_pkt_attribute_template->tcp_flag_type_urg = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TCP_FLAG_TYPE_ECEf:
            ing_pkt_attribute_template->tcp_flag_type_ece = data_field->data;
            break;
        case CTR_ING_FLEX_PKT_ATTR_TEMPLATEt_TCP_FLAG_TYPE_CWRf:
            ing_pkt_attribute_template->tcp_flag_type_cwr = data_field->data;
            break;
        default:
            break;
        }
}

static void
parse_flex_ing_pkt_attribute_fields (int unit,
                                     bcmltd_fields_t *imm_fields,
                   ctr_flex_pkt_attribute_template_t *ing_pkt_attribute_template)
{
    bcmltd_field_t *data_field;
    size_t i;

    for (i=0; i < imm_fields->count; i++) {
        data_field =imm_fields->field[i];
        extract_flex_ing_pkt_attribute_fields(ing_pkt_attribute_template,
                                         data_field);
    }
}

/*!
 * Convert field list to egress packet attributes struct.
 */
static void
parse_flex_egr_pkt_attribute_fields (int unit, bcmltd_fields_t *imm_fields,
                   ctr_flex_pkt_attribute_template_t *egr_pkt_attribute_template)
{
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t i;

    flist = imm_fields;

    for (i=0; i < flist->count; i++) {
        data_field = flist->field[i];
        switch (data_field->id) {
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_IPf:
            egr_pkt_attribute_template->ip = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_DROPf:
            egr_pkt_attribute_template->drop = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_NETWORK_SVPf:
            egr_pkt_attribute_template->network_svp = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_ACCESS_SVPf:
            egr_pkt_attribute_template->access_svp = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_NETWORK_DVPf:
            egr_pkt_attribute_template->network_dvp = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_ACCESS_DVPf:
            egr_pkt_attribute_template->access_dvp = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_MULTICASTf:
            egr_pkt_attribute_template->pkt_type_multicast = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_UNICASTf:
            egr_pkt_attribute_template->pkt_type_unicast = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PKT_TYPE_ALLf:
            egr_pkt_attribute_template->pkt_type_all = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_TOS_ECNf:
            egr_pkt_attribute_template->tos_ecn = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_TOS_DSCPf:
            egr_pkt_attribute_template->tos_dscp = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PORT_IDf:
            egr_pkt_attribute_template->port = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_INT_CNf:
            egr_pkt_attribute_template->int_cn = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_INNER_DOT1Pf:
            egr_pkt_attribute_template->inner_dot1p = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_OUTER_DOT1Pf:
            egr_pkt_attribute_template->outer_dot1p = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_VLAN_UTf:
            egr_pkt_attribute_template->vlan_ut = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_VLAN_SITf:
            egr_pkt_attribute_template->vlan_sit = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_VLAN_SOTf:
            egr_pkt_attribute_template->vlan_sot = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_VLAN_DTf:
            egr_pkt_attribute_template->vlan_dt = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_INT_PRIf:
            egr_pkt_attribute_template->int_pri = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_G_COLORf:
            egr_pkt_attribute_template->pre_fp_g_color = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_Y_COLORf:
            egr_pkt_attribute_template->pre_fp_y_color = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_PRE_FP_R_COLORf:
            egr_pkt_attribute_template->pre_fp_r_color = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_CONGESTION_MARKEDf:
            egr_pkt_attribute_template->congestion_marked = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_UNICAST_QUEUEINGf:
            egr_pkt_attribute_template->unicast_queueing = data_field->data;
            break;
        case CTR_EGR_FLEX_PKT_ATTR_TEMPLATEt_MMU_QUEUE_NUMf:
            egr_pkt_attribute_template->mmu_queue_num = data_field->data;
            break;
        default:
            break;
        }
    }
}

/*!
 * Lookup PKT_ATTR_TEMPLATE table given pkt_attr_template_id.
 * Return packet attributes in pkt_attribute_template.
 * Return SHR_E if entry not found.
 * user_data, if any, is entry still being processed in stage_cb.
 */
int
lookup_flex_pkt_attr_table (int unit,
                            bool ingress,
                            bool lookup_local,
                            uint32_t pkt_attr_template_id,
                            ctr_flex_user_data_t *user_data,
                ctr_flex_pkt_attribute_template_t *pkt_attribute_template)
{
    int rv;
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0;
    int select;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    select  = ingress ? 0 : 1;

    /* Check user data first, if it is supplied */
    if (user_data != NULL &&
        user_data->data_type == CTR_FLEX_PKT_ATTR_DATA &&
        user_data->pkt_attr_data.pkt_attr_id == pkt_attr_template_id) {

        if (user_data->event_reason == BCMIMM_ENTRY_DELETE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (pkt_attribute_template != NULL) {
            *pkt_attribute_template = *user_data->pkt_attr_data.template;
        }

        SHR_EXIT();
    }

    /* check the local data-base, if only existence is requested */
    if (pkt_attribute_template == NULL && lookup_local) {
        if (ingress) {
            if (ctrl->ing_pkt_attributes[pkt_attr_template_id].active) {
                SHR_EXIT();
            }
        }
        else {
            if (ctrl->egr_pkt_attributes[pkt_attr_template_id].active) {
                SHR_EXIT();
            }
        }
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &key_fields,
                                       1));

    key_fields.field[0]->id = pkt_attr_pkt_attr_fid[select];
    key_fields.field[0]->data = pkt_attr_template_id;

    /* Allocate imm_fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, pkt_attr_sid[select], &num_fields));

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /*
     * Lookup CTR_*_FLEX_PKT_ATTR_TEMPLATEt;
     * exit with error if entry not found, but no message.
     */
    rv = bcmimm_entry_lookup(unit,
                             pkt_attr_sid[select],
                             &key_fields,
                             &imm_fields);

    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    /* found an entry; attributes were not requested, so return */
    if (pkt_attribute_template == NULL) {
        SHR_EXIT();
    }

    if (ingress) {
        parse_flex_ing_pkt_attribute_fields (unit,
                                            &imm_fields,
                                            pkt_attribute_template);
    }
    else {
        parse_flex_egr_pkt_attribute_fields (unit,
                                            &imm_fields,
                                            pkt_attribute_template);
    }

exit:
    bcmptm_ctr_flex_flds_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmptm_ctr_flex_flds_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

/*!
 * Update the OPERATIONAL_STATUSf, BASE_INDEX_OPERf,
 * MAX_INSTANCES_OPERf in the IMM FLEX_ENTRY table.
 */
int
oper_state_update (int unit,
                   bool ingress,
                   uint32_t entry_id,
                   ctr_flex_entry_data_t *entry_data)
{
    int t;
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields = 0;
    int select;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    select  = ingress ? 0 : 1;

    for (t=0; t<LT_INFO_NUM_TABLES; t++) {
        if (!sal_strcmp("CTR_ING_FLEX_ENTRY", ctrl->ctr_flex_field_info[t].name)) break;
        if (!sal_strcmp("CTR_EGR_FLEX_ENTRY", ctrl->ctr_flex_field_info[t].name)) break;
    }

    if (t == LT_INFO_NUM_TABLES) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                         &key_fields,
                         (ctrl->ctr_flex_field_info[t].num_key_fields +
                         ctrl->ctr_flex_field_info[t].num_read_only_fields)));

    key_fields.count = ctrl->ctr_flex_field_info[t].num_key_fields;
    key_fields.field[0]->id   = entry_fid[select];
    key_fields.field[0]->data = entry_id;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, entry_sid[select], &num_fields));

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /* Lookup IMM table with key=entry_id; error if entry not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             entry_sid[select],
                             &key_fields,
                             &imm_fields));

    /*
     * Set the fields.count to 4 to include OPERATTIONAL_STATUSf,
     * BASE_INDEX_OPERf, MAX_INSTANCE_OPERf
     */
    key_fields.count = ctrl->ctr_flex_field_info[t].num_key_fields +
                       ctrl->ctr_flex_field_info[t].num_read_only_fields;
    key_fields.field[1]->id = op_fid[select];
    key_fields.field[2]->id = base_index_oper_fid[select];
    key_fields.field[3]->id = max_instances_oper_fid[select];
    key_fields.field[1]->data = entry_data->op_state;
    key_fields.field[2]->data = entry_data->base_index;
    key_fields.field[3]->data = entry_data->max_instances;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_update(unit, 0, entry_sid[select], &key_fields));

exit:
    bcmptm_ctr_flex_flds_free(unit, &key_fields);
    /* imm_fields.count may be modified by bcmimm_entry_lookup */
    imm_fields.count = num_fields;
    bcmptm_ctr_flex_flds_free(unit, &imm_fields);

    SHR_FUNC_EXIT();
}

/*!
 * Populate the Read-Only Fields of ING_POOL_INFOt
 */
int
ctr_flex_get_lt_field_info(int unit)
{
    int t;
    size_t i, num_fields;
    bcmlrd_table_attrib_t t_attrib;
    bcmlrd_client_field_info_t *fields_info = NULL;
    const char *lt_names[] = {"CTR_ING_FLEX_ENTRY", "CTR_EGR_FLEX_ENTRY",
                              "CTR_ING_FLEX_STATS", "CTR_EGR_FLEX_STATS"};
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (t = 0; t < LT_INFO_NUM_TABLES; t++) {
        SHR_IF_ERR_EXIT
            (bcmlrd_table_attributes_get(unit, lt_names[t], &t_attrib));

        num_fields = t_attrib.number_of_fields;

        SHR_ALLOC(fields_info,
                  sizeof(bcmlrd_client_field_info_t) * num_fields,
                  CTR_FLEX_FIELDS);

        SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

        sal_memset(fields_info, 0,
                   sizeof(bcmlrd_client_field_info_t) * num_fields);

        SHR_IF_ERR_EXIT
            (bcmlrd_table_fields_def_get(unit,
                                         lt_names[t],
                                         num_fields,
                                         fields_info,
                                         &num_fields));

        sal_strncpy(ctrl->ctr_flex_field_info[t].name, lt_names[t], LT_INFO_MAX_TABLE_NAME_LEN);

        for (i = 0; i < num_fields; i++) {
            if (fields_info[i].key) {
                /* Found one key entry */
                ctrl->ctr_flex_field_info[t].num_key_fields++;
            }
            else {
                /* Found one data entry */
                ctrl->ctr_flex_field_info[t].num_data_fields++;
                if (fields_info[i].access == BCMLT_FIELD_ACCESS_READ_ONLY) {
                    ctrl->ctr_flex_field_info[t].num_read_only_fields++;
                }
            }
        }
        SHR_FREE(fields_info);
    }

exit:
    if (fields_info) {
        SHR_FREE(fields_info);
    }
    SHR_FUNC_EXIT();
}
/*!
 * Populate the Read-Only Fields of ING_POOL_INFOt
 */
int
ctr_flex_populate_ing_pool_info(int unit, bool warm)
{
    size_t i;
    bcmltd_fields_t flex_pool_info_lt_flds = {0};
    bcmlrd_table_attrib_t t_attrib;
    size_t num_fields = 0;
    size_t f_cnt;
    bcmdrd_sid_t sid;
    bcmdrd_sym_info_t sinfo;
    bcmlrd_client_field_info_t *fields_info = NULL;
    ctr_flex_control_t *ctrl = NULL;
    ctr_flex_device_info_t *device_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    device_info = ctrl->ctr_flex_device_info;
    if (device_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get all table attributes for "CTR_ING_FLEX_POOL_INFO" LT table. */

    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     "CTR_ING_FLEX_POOL_INFO",
                                     &t_attrib));

    num_fields = t_attrib.number_of_fields;

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * num_fields,
        CTR_FLEX_FIELDS);

    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    sal_memset(fields_info, 0, sizeof(bcmlrd_client_field_info_t) * num_fields);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     "CTR_ING_FLEX_POOL_INFO",
                                     num_fields,
                                     fields_info,
                                     &num_fields));

    flex_pool_info_lt_flds.field = NULL;

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit, &flex_pool_info_lt_flds, num_fields));

    /* Populate ingress pool LT. */
    for (i = 0; i < device_info->num_ingress_pools; i++) {
        SHR_IF_ERR_EXIT
            (get_counter_sid(unit, 1, i, &sid));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_info_get(unit, sid, &sinfo));

        ctrl->ctr_ing_flex_pool[i].num_counters = sinfo.index_max+1;

        if (i < device_info->fp_pool_start) {
            ctrl->ctr_ing_flex_pool[i].pool_reservation = 0;
        }
        else {
            ctrl->ctr_ing_flex_pool[i].pool_reservation = 1;
        }

        f_cnt = 0;
        for (f_cnt = 0; f_cnt < num_fields; f_cnt++) {
            flex_pool_info_lt_flds.field[f_cnt]->id = fields_info[f_cnt].id;

            if (fields_info[f_cnt].id ==
                     CTR_ING_FLEX_POOL_INFOt_CTR_ING_FLEX_POOL_INFO_IDf) {
                flex_pool_info_lt_flds.field[f_cnt]->data = i;
            }
            else if (fields_info[f_cnt].id ==
                     CTR_ING_FLEX_POOL_INFOt_POOL_SIZEf) {
                flex_pool_info_lt_flds.field[f_cnt]->data =
                     ctrl->ctr_ing_flex_pool[i].num_counters;
            }
            else if (fields_info[f_cnt].id ==
                     CTR_ING_FLEX_POOL_INFOt_RESERVED_FOR_FPf) {
                flex_pool_info_lt_flds.field[f_cnt]->data =
                     ctrl->ctr_ing_flex_pool[i].pool_reservation;
            }
        }
        flex_pool_info_lt_flds.count = f_cnt;

        if (!warm) {
            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit,
                                     CTR_ING_FLEX_POOL_INFOt,
                                     &flex_pool_info_lt_flds));
        }
    }

exit:
    bcmptm_ctr_flex_flds_free(unit, &flex_pool_info_lt_flds);

    if (fields_info) {
        SHR_FREE(fields_info);
    }

    SHR_FUNC_EXIT();
}

/*!
 * Populate the Read-Only Fields of EGR_POOL_INFOt
 */
int
ctr_flex_populate_egr_pool_info(int unit, bool warm)
{
    size_t i;
    bcmltd_fields_t flex_pool_info_lt_flds = {0};
    bcmlrd_table_attrib_t t_attrib;
    size_t num_fields = 0;
    size_t f_cnt;
    bcmdrd_sid_t sid;
    bcmdrd_sym_info_t sinfo;
    bcmlrd_client_field_info_t *fields_info = NULL;
    ctr_flex_control_t *ctrl = NULL;
    ctr_flex_device_info_t *device_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    device_info = ctrl->ctr_flex_device_info;
    if (device_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get all table attributes for "CTR_EGR_FLEX_POOL_INFO" LT table. */
    flex_pool_info_lt_flds.field = NULL;

    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     "CTR_EGR_FLEX_POOL_INFO",
                                     &t_attrib));

    num_fields = t_attrib.number_of_fields;

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * num_fields,
            CTR_FLEX_FIELDS);

    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    sal_memset(fields_info, 0, sizeof(bcmlrd_client_field_info_t) * num_fields);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit,
                                     "CTR_EGR_FLEX_POOL_INFO",
                                     num_fields,
                                     fields_info,
                                     &num_fields));

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_flds_allocate(unit, &flex_pool_info_lt_flds, num_fields));

    /* Populate egress pool LT. */
    for (i = 0; i < device_info->num_egress_pools; i++) {
        SHR_IF_ERR_EXIT
            (get_counter_sid(unit, 0, i, &sid));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_info_get(unit, sid, &sinfo));

        ctrl->ctr_egr_flex_pool[i].num_counters = sinfo.index_max+1;
        ctrl->ctr_egr_flex_pool[i].pool_reservation = 0;

        f_cnt = 0;
        for (f_cnt = 0; f_cnt < num_fields; f_cnt++) {
            flex_pool_info_lt_flds.field[f_cnt]->id = fields_info[f_cnt].id;

            if (fields_info[f_cnt].id ==
                 CTR_EGR_FLEX_POOL_INFOt_CTR_EGR_FLEX_POOL_INFO_IDf) {
                flex_pool_info_lt_flds.field[f_cnt]->data = i;
            }
            else if (fields_info[f_cnt].id ==
                      CTR_EGR_FLEX_POOL_INFOt_POOL_SIZEf) {
                flex_pool_info_lt_flds.field[f_cnt]->data =
                 ctrl->ctr_egr_flex_pool[i].num_counters;
            }
        }
        flex_pool_info_lt_flds.count = f_cnt;

        if (!warm) {
            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit,
                                     CTR_EGR_FLEX_POOL_INFOt,
                                     &flex_pool_info_lt_flds));
        }
    }

exit:
    bcmptm_ctr_flex_flds_free(unit, &flex_pool_info_lt_flds);

    if (fields_info) {
        SHR_FREE(fields_info);
    }

    SHR_FUNC_EXIT();
}
