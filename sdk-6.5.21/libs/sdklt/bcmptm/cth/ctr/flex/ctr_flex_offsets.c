/*! \file ctr_flex_offsets.c
 *
 * Flex counter offset and key selectors computation routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_ctr_flex.h>

void
update_offset_map(active_entry_t *active_entry,
                      uint8_t offset,
                      uint8_t counter)
{
    active_entry->offset_table_map[offset].offset = counter;
    active_entry->offset_table_map[offset].count_enable = 1;
}

int
update_packet_selectors(int unit,
                        bool set_bit,
                        active_entry_t *active_entry,
                        flex_attribute_t *combined_flex_attribute)
{
    uint32_t pool_num;
    uint32_t key_id;
    uint32_t pkt_selector_reg_value[2] = {0, 0};
    uint8_t offset_mode;
    uint8_t current_bit_position;
    bool    ingress;
    flex_pkt_attribute_bits_t *flex_pkt_attribute_bits;
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
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pool_num = active_entry->entry_data->pool_id;
    offset_mode = active_entry->entry_data->offset_mode;
    ingress = active_entry->ingress;
    key_id = (pool_num/4)*4 + offset_mode;

    /* Get attribute field position and width */
    if (ingress) {
        flex_pkt_attribute_bits = device_info->ing_pkt_attr_bits;
    }
    else {
        flex_pkt_attribute_bits = device_info->egr_pkt_attr_bits;
    }

    /* Get current attr selector register value */
    /*
    SHR_IF_ERR_EXIT
        (read_pkt_selector_regvalue(unit,
                                    ingress,
                                    pool_num,
                                    pkt_selector_reg_value));
    */
    current_bit_position = 0;

    /* Scan through pkt attributes */
    if (combined_flex_attribute->ip) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->ip_bits,
                              flex_pkt_attribute_bits->ip_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->drop) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->drop_bits,
                              flex_pkt_attribute_bits->drop_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->pkt_type) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->pkt_type_bits,
                              flex_pkt_attribute_bits->pkt_type_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->tos_dscp) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->tos_dscp_bits,
                              flex_pkt_attribute_bits->tos_dscp_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->tos_ecn) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->tos_ecn_bits,
                              flex_pkt_attribute_bits->tos_ecn_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->port) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->port_bits,
                              flex_pkt_attribute_bits->port_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->int_cn) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->int_cn_bits,
                              flex_pkt_attribute_bits->int_cn_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }
    if (combined_flex_attribute->inner_dot1p) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->inner_dot1p_bits,
                              flex_pkt_attribute_bits->inner_dot1p_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->outer_dot1p) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->outer_dot1p_bits,
                              flex_pkt_attribute_bits->outer_dot1p_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->vlan) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->vlan_bits,
                              flex_pkt_attribute_bits->vlan_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->int_pri) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->int_pri_bits,
                              flex_pkt_attribute_bits->int_pri_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->pre_fp_color) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->pre_fp_color_bits,
                              flex_pkt_attribute_bits->pre_fp_color_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->fp_color) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->fp_color_bits,
                              flex_pkt_attribute_bits->fp_color_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->elephant_pkt) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->elephant_pkt_bits,
                              flex_pkt_attribute_bits->elephant_pkt_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->tcp_flags) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->tcp_flag_bits,
                              flex_pkt_attribute_bits->tcp_flag_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->svp_type) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->svp_bits,
                              flex_pkt_attribute_bits->svp_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->dvp_type) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->dvp_bits,
                              flex_pkt_attribute_bits->dvp_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->congestion_marked) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->congestion_bits,
                              flex_pkt_attribute_bits->congestion_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->mmu_queue_num) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->mmu_queue_bits,
                              flex_pkt_attribute_bits->mmu_queue_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }

    if (combined_flex_attribute->unicast_queueing) {
        SHR_IF_ERR_EXIT
            (update_pkt_attr_selector_bits(
                              unit,
                              ingress,
                              set_bit,
                              key_id,
                              flex_pkt_attribute_bits->unicast_queueing_bits,
                              flex_pkt_attribute_bits->unicast_queueing_pos,
                              &current_bit_position,
                              pkt_selector_reg_value));
    }


    /* Write pkt selector register */
    SHR_IF_ERR_EXIT
        (write_pkt_attr_selectors(
                        unit,
                        ingress,
                        key_id,
                        pkt_selector_reg_value));

    /* Update offset table */
    SHR_IF_ERR_EXIT
        (update_offset_table(unit,
                        ingress,
                        pool_num,
                        offset_mode,
                        active_entry->offset_table_map));

exit:
    SHR_FUNC_EXIT();
}

int
process_per_counter_attributes(int unit,
                               active_entry_t *active_entry,
                               ctr_flex_pkt_attribute_template_t *pkt_attribute_template)
{
    sal_memset(&active_entry->flex_attribute, 0, sizeof(flex_attribute_t));

    /* Combine the attributes */
    active_entry->flex_attribute.ip = pkt_attribute_template->ip;

    active_entry->flex_attribute.drop = pkt_attribute_template->drop;

    if (pkt_attribute_template->pkt_type_all) {
        if (active_entry->ingress) {
            active_entry->flex_attribute.pkt_type = ING_ATTR_PKT_TYPE_ALL;
        } else {
            active_entry->flex_attribute.pkt_type = EGR_ATTR_PKT_TYPE_ALL;
        }
    } else if (active_entry->ingress) {
        active_entry->flex_attribute.pkt_type =
            (pkt_attribute_template->pkt_type_unknown << PKT_TYPE_UNKNOWN);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_control << PKT_TYPE_CONTROL);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_oam << PKT_TYPE_OAM);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_bfd << PKT_TYPE_BFD);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_bpdu << PKT_TYPE_BPDU);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_icnm << PKT_TYPE_ICNM);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_1588 << PKT_TYPE_1588);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_l2uc << PKT_TYPE_KNOWN_L2UC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unknown_l2uc << PKT_TYPE_UNKNOWN_L2UC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_l2bc << PKT_TYPE_L2BC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_l2mc << PKT_TYPE_KNOWN_L2MC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unknown_l2mc << PKT_TYPE_UNKNOWN_L2MC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_l3uc << PKT_TYPE_KNOWN_L3UC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unknown_l3uc << PKT_TYPE_UNKNOWN_L3UC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_ipmc << PKT_TYPE_KNOWN_IPMC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unknown_ipmc << PKT_TYPE_UNKNOWN_IPMC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_mpls_l2 << PKT_TYPE_KNOWN_MPLS_L2);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_mpls_l3 << PKT_TYPE_KNOWN_MPLS_L3);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_mpls << PKT_TYPE_KNOWN_MPLS);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unknown_mpls << PKT_TYPE_UNKNOWN_MPLS);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_mpls_mc << PKT_TYPE_KNOWN_MPLS_MC);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_mim << PKT_TYPE_KNOWN_MIM);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unknown_mim << PKT_TYPE_UNKNOWN_MIM);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_trill << PKT_TYPE_KNOWN_TRILL);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unknown_trill << PKT_TYPE_UNKNOWN_TRILL);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_known_niv << PKT_TYPE_KNOWN_NIV);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unknown_niv << PKT_TYPE_UNKNOWN_NIV);
    }
    else {      /* Egress */
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_unicast << PKT_TYPE_UNICAST);
        active_entry->flex_attribute.pkt_type |=
            (pkt_attribute_template->pkt_type_multicast << PKT_TYPE_MULTICAST);
    }

    active_entry->flex_attribute.tos_dscp = pkt_attribute_template->tos_dscp;

    active_entry->flex_attribute.tos_ecn = pkt_attribute_template->tos_ecn;

    active_entry->flex_attribute.port = pkt_attribute_template->port;

    active_entry->flex_attribute.int_cn = pkt_attribute_template->int_cn;

    active_entry->flex_attribute.outer_dot1p = pkt_attribute_template->outer_dot1p;

    active_entry->flex_attribute.inner_dot1p = pkt_attribute_template->inner_dot1p;

    active_entry->flex_attribute.vlan =
        (pkt_attribute_template->vlan_ut << VLAN_UT);
    active_entry->flex_attribute.vlan |=
        (pkt_attribute_template->vlan_sit << VLAN_SIT);
    active_entry->flex_attribute.vlan |=
        (pkt_attribute_template->vlan_sot << VLAN_SOT);
    active_entry->flex_attribute.vlan |=
        (pkt_attribute_template->vlan_dt << VLAN_DT);

    active_entry->flex_attribute.int_pri = pkt_attribute_template->int_pri;

    active_entry->flex_attribute.svp_type =
        pkt_attribute_template->access_dvp << ATTR_VP_ACCESS;
    active_entry->flex_attribute.svp_type |=
        pkt_attribute_template->network_dvp << ATTR_VP_NETWORK;

    active_entry->flex_attribute.dvp_type =
        pkt_attribute_template->access_dvp << ATTR_VP_ACCESS;
    active_entry->flex_attribute.dvp_type |=
        pkt_attribute_template->network_dvp << ATTR_VP_NETWORK;

    active_entry->flex_attribute.congestion_marked =
             pkt_attribute_template->congestion_marked;

    active_entry->flex_attribute.mmu_queue_num =
             pkt_attribute_template->mmu_queue_num;

    active_entry->flex_attribute.unicast_queueing =
             pkt_attribute_template->unicast_queueing;

    active_entry->flex_attribute.pre_fp_color =
        pkt_attribute_template->pre_fp_g_color << ATTR_G_COLOR;
    active_entry->flex_attribute.pre_fp_color |=
        pkt_attribute_template->pre_fp_y_color << ATTR_R_COLOR;
    active_entry->flex_attribute.pre_fp_color |=
        pkt_attribute_template->pre_fp_r_color << ATTR_Y_COLOR;

    active_entry->flex_attribute.fp_color =
        pkt_attribute_template->fp_g_color << ATTR_G_COLOR;
    active_entry->flex_attribute.fp_color |=
        pkt_attribute_template->fp_y_color << ATTR_R_COLOR;
    active_entry->flex_attribute.fp_color |=
        pkt_attribute_template->fp_r_color << ATTR_Y_COLOR;

    active_entry->flex_attribute.elephant_pkt = pkt_attribute_template->elephant_pkt;

    active_entry->flex_attribute.tcp_flags =
        pkt_attribute_template->tcp_flag_type_fin << TCP_FLAG_TYPE_FIN;
    active_entry->flex_attribute.tcp_flags |=
        pkt_attribute_template->tcp_flag_type_syn << TCP_FLAG_TYPE_SYN;
    active_entry->flex_attribute.tcp_flags |=
        pkt_attribute_template->tcp_flag_type_rst << TCP_FLAG_TYPE_RST;
    active_entry->flex_attribute.tcp_flags |=
        pkt_attribute_template->tcp_flag_type_psh << TCP_FLAG_TYPE_PSH;
    active_entry->flex_attribute.tcp_flags |=
        pkt_attribute_template->tcp_flag_type_ack << TCP_FLAG_TYPE_ACK;
    active_entry->flex_attribute.tcp_flags |=
        pkt_attribute_template->tcp_flag_type_urg << TCP_FLAG_TYPE_URG;
    active_entry->flex_attribute.tcp_flags |=
        pkt_attribute_template->tcp_flag_type_ece << TCP_FLAG_TYPE_ECE;
    active_entry->flex_attribute.tcp_flags |=
        pkt_attribute_template->tcp_flag_type_cwr << TCP_FLAG_TYPE_CWR;

    return SHR_E_NONE;
}

static int
get_offset_widths(int unit,
                  flex_attribute_t *combined_flex_attribute,
                  flex_pkt_attribute_bits_t *flex_pkt_attribute_bits,
                  active_entry_t *active_entry,
                  uint8_t *offset_width,
                  uint16_t *num_entries)
{
    uint8_t attribute_num = 0;
    uint8_t total_bits = 0;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Scan through the combined attributes */
    if (combined_flex_attribute->ip) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->ip_bits;
        total_bits += flex_pkt_attribute_bits->ip_bits;
    }

    if (combined_flex_attribute->drop) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->drop_bits;
        total_bits += flex_pkt_attribute_bits->drop_bits;
    }

    if (combined_flex_attribute->dvp_type) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->dvp_bits;
        total_bits += flex_pkt_attribute_bits->dvp_bits;
    }

    if (combined_flex_attribute->svp_type) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->svp_bits;
        total_bits += flex_pkt_attribute_bits->svp_bits;
    }

    if (combined_flex_attribute->pkt_type) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->pkt_type_bits;
        total_bits += flex_pkt_attribute_bits->pkt_type_bits;
    }

    if (combined_flex_attribute->tos_ecn) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->tos_ecn_bits;
        total_bits += flex_pkt_attribute_bits->tos_ecn_bits;
    }

    if (combined_flex_attribute->tos_dscp) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->tos_dscp_bits;
        total_bits += flex_pkt_attribute_bits->tos_dscp_bits;
    }

    if (combined_flex_attribute->port) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->port_bits;
        total_bits += flex_pkt_attribute_bits->port_bits;
    }

    if (combined_flex_attribute->int_cn) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->int_cn_bits;
        total_bits += flex_pkt_attribute_bits->int_cn_bits;
    }

    if (combined_flex_attribute->inner_dot1p) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->inner_dot1p_bits;
        total_bits += flex_pkt_attribute_bits->inner_dot1p_bits;
    }

    if (combined_flex_attribute->outer_dot1p) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->outer_dot1p_bits;
        total_bits += flex_pkt_attribute_bits->outer_dot1p_bits;
    }

    if (combined_flex_attribute->vlan) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->vlan_bits;
        total_bits += flex_pkt_attribute_bits->vlan_bits;
    }

    if (combined_flex_attribute->int_pri) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->int_pri_bits;
        total_bits += flex_pkt_attribute_bits->int_pri_bits;
    }

    if (combined_flex_attribute->fp_color) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->fp_color_bits;
        total_bits += flex_pkt_attribute_bits->fp_color_bits;
    }

    if (combined_flex_attribute->pre_fp_color) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->pre_fp_color_bits;
        total_bits += flex_pkt_attribute_bits->pre_fp_color_bits;
    }

    if (combined_flex_attribute->elephant_pkt) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->elephant_pkt_bits;
        total_bits += flex_pkt_attribute_bits->elephant_pkt_bits;
    }

    if (combined_flex_attribute->tcp_flags) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->tcp_flag_bits;
        total_bits += flex_pkt_attribute_bits->tcp_flag_bits;
    }

    if (combined_flex_attribute->congestion_marked) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->congestion_bits;
        total_bits += flex_pkt_attribute_bits->congestion_bits;
    }

    if (combined_flex_attribute->unicast_queueing) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->unicast_queueing_bits;
        total_bits += flex_pkt_attribute_bits->unicast_queueing_bits;
    }

    if (combined_flex_attribute->mmu_queue_num) {
        offset_width[attribute_num++] = flex_pkt_attribute_bits->mmu_queue_bits;
        total_bits += flex_pkt_attribute_bits->mmu_queue_bits;
    }

    /* Check total bits */
    if (total_bits > MAX_CTR_INSTANCES_BIT_WIDTH) {
        active_entry->entry_data->op_state = ctrl->ctr_flex_enum.pkt_attr_template_exceeded;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    *num_entries = (total_bits==0) ? 0 : (1<<total_bits);

exit:
    SHR_FUNC_EXIT();
}

int
process_offset_table(int unit,
                     active_entry_t *active_entry,
                     ctr_flex_user_data_t *user_data,
                     flex_attribute_t *combined_flex_attribute)
{
    uint32_t i, counter, instance_id, active_instances;
    uint8_t *offset_array[8] = {0};
    uint8_t offset_width[8] = {0};
    uint8_t attribute_num = 0;
    uint8_t max_count, loop_index;
    flex_pkt_attribute_bits_t *flex_pkt_attribute_bits;
    uint32_t    pkt_attribute_template_id;
    ctr_flex_pkt_attribute_template_t pkt_attribute_template = {0};
    int valid, j;
    uint16_t num_entries = 0;
    uint16_t index;
    uint8_t attribute_val;
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
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (active_entry->ingress) {
        flex_pkt_attribute_bits = device_info->ing_pkt_attr_bits;
    }
    else {
        flex_pkt_attribute_bits = device_info->egr_pkt_attr_bits;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (get_offset_widths(unit,
                           combined_flex_attribute,
                           flex_pkt_attribute_bits,
                           active_entry,
                           offset_width,
                           &num_entries));

    /* Allocate offset_array */
    for (i = 0; i < 8; i++) {
        SHR_ALLOC(offset_array[i], 256, "bcmptmCtrFlexOffsetArray");
        SHR_NULL_CHECK(offset_array[i], SHR_E_MEMORY);
        sal_memset(offset_array[i], 0, 256);
    }

    /* Clear offset_table_map */
    for (i = 0; i < 256; i++) {
        active_entry->offset_table_map[i].offset = 0;
        active_entry->offset_table_map[i].count_enable = 0;
    }

    active_instances = active_entry->active_instances;

    /* Loop through all the active instances in this group */
    for (counter = 0; counter < active_instances; counter++) {

        /* Lookup pkt attribute template IDs */
        pkt_attribute_template_id =
            active_entry->pkt_attribute_template_id[counter];

        instance_id =
            active_entry->ctr_instance_id[counter];

        for (attribute_num = 0; attribute_num < 8; attribute_num++) {
           sal_memset(offset_array[attribute_num], 0, 256);
        }

        sal_memset(&pkt_attribute_template, 0,
             sizeof(ctr_flex_pkt_attribute_template_t));

        SHR_IF_ERR_EXIT
            (lookup_flex_pkt_attr_table (unit,
                                         active_entry->ingress,
                                         true,
                                         pkt_attribute_template_id,
                                         user_data,
                                         &pkt_attribute_template));

        SHR_IF_ERR_EXIT
            (process_per_counter_attributes(unit,
                                       active_entry,
                                       &pkt_attribute_template));

        attribute_num = 0;

        /* Build the offset_arrays with valid offsets for each attribute */
        if ((combined_flex_attribute->ip) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if (active_entry->flex_attribute.ip) {
                offset_array[attribute_num][1] = 1;
            }
            else {
               /*
                * When an attribute exists in the group, but is not explicitly
                * specified for the instance, default to 0.
                */
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->drop) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if (active_entry->flex_attribute.drop) {
                   offset_array[attribute_num][1] = 1;
            }
            else {
                   offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->dvp_type) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->dvp_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.dvp_type) & (1 << loop_index)) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->svp_type) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->svp_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.svp_type) & (1 << loop_index)) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->pkt_type) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_CONTROL)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->control_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_OAM)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->oam_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_BFD)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->bfd_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_BPDU)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->bpdu_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_ICNM)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->icnm_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_1588)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->n1588_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_L2UC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_l2uc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_L2UC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_l2uc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_L2MC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_l2mc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_L2MC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_l2mc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_L2BC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->l2bc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_L3UC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_l3uc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_L3UC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_l3uc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_IPMC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_ipmc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_IPMC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_ipmc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_MPLS_L2)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_mpls_l2_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_MPLS)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_mpls_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_MPLS_L3)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_mpls_l3_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_MPLS)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_mpls_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_MPLS_MC)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_mpls_mc_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_MIM)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_mim_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_MIM)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_mim_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_TRILL)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_trill_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_TRILL)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_trill_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_NIV)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_niv_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_NIV)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_niv_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_NIV)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_niv_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_L2GRE)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_l2gre_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_VXLAN)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_vxlan_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_KNOWN_FCOE)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->known_fcoe_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                   (1 << PKT_TYPE_UNKNOWN_FCOE)) {
                   offset_array[attribute_num][device_info->ing_pkt_types->unknown_fcoe_pkt] = 1;
            }
            /* Egress packet types */
            if ((active_entry->flex_attribute.pkt_type) &
                    (1 << PKT_TYPE_UNICAST)) {
                   offset_array[attribute_num][device_info->egr_pkt_types->unicast_pkt] = 1;
            }
            if ((active_entry->flex_attribute.pkt_type) &
                    (1 << PKT_TYPE_MULTICAST)) {
                   offset_array[attribute_num][device_info->egr_pkt_types->multicast_pkt] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->tos_ecn) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->tos_ecn_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.tos_ecn) == loop_index) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->tos_dscp) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->tos_dscp_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.tos_dscp) == loop_index) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->port) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = (1 << flex_pkt_attribute_bits->port_bits) - 1;
            for (loop_index = 0; loop_index <= max_count; loop_index++) {
                if ((active_entry->flex_attribute.port) == loop_index) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            attribute_num++;
        }

        if ((active_entry->flex_attribute.int_cn) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->int_cn_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.int_cn) == loop_index) {
                    offset_array[attribute_num][loop_index] = 1;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->inner_dot1p) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->inner_dot1p_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.inner_dot1p) == loop_index) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->outer_dot1p) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->outer_dot1p_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.outer_dot1p) == loop_index) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->vlan) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if ((active_entry->flex_attribute.vlan) &
                   (1 << VLAN_UT)) {
                   offset_array[attribute_num][0] = 1;
            }
            if ((active_entry->flex_attribute.vlan) &
                   (1 << VLAN_SIT)) {
                   offset_array[attribute_num][1] = 1;
            }
            if ((active_entry->flex_attribute.vlan) &
                   (1 << VLAN_SOT)) {
                   offset_array[attribute_num][2] = 1;
            }
            if ((active_entry->flex_attribute.vlan) &
                   (1 << VLAN_DT)) {
                   offset_array[attribute_num][3] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->int_pri) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->int_pri_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.int_pri) == loop_index) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->fp_color) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if ((active_entry->flex_attribute.fp_color) &
                   (1 << ATTR_G_COLOR)) {
                   offset_array[attribute_num][0] = 1;
            }
            if ((active_entry->flex_attribute.fp_color) &
                   (1 << ATTR_R_COLOR)) {
                   offset_array[attribute_num][1] = 1;
            }
            if ((active_entry->flex_attribute.fp_color) &
                   (1 << ATTR_Y_COLOR)) {
                   offset_array[attribute_num][3] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->pre_fp_color) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if ((active_entry->flex_attribute.pre_fp_color) &
                   (1 << ATTR_G_COLOR)) {
                   offset_array[attribute_num][0] = 1;
            }
            if ((active_entry->flex_attribute.pre_fp_color) &
                   (1 << ATTR_R_COLOR)) {
                   offset_array[attribute_num][1] = 1;
            }
            if ((active_entry->flex_attribute.pre_fp_color) &
                   (1 << ATTR_Y_COLOR)) {
                   offset_array[attribute_num][3] = 1;
            }
            attribute_num++;
        }

        if ((active_entry->flex_attribute.elephant_pkt) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if (active_entry->flex_attribute.congestion_marked) {
                offset_array[attribute_num][1] = 1;
            }
            else {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((active_entry->flex_attribute.tcp_flags) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            offset_array[attribute_num]
            [active_entry->flex_attribute.tcp_flags] = 1;
            attribute_num++;
        }

        if ((combined_flex_attribute->congestion_marked) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if (active_entry->flex_attribute.congestion_marked) {
                offset_array[attribute_num][1] = 1;
            }
            else {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->unicast_queueing) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            if (active_entry->flex_attribute.unicast_queueing) {
                offset_array[attribute_num][1] = 1;
            }
            else {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        if ((combined_flex_attribute->mmu_queue_num) &&
            (attribute_num <= (MAX_CTR_INSTANCES_BIT_WIDTH - 2))) {
            max_count = 1 << flex_pkt_attribute_bits->mmu_queue_bits;
            for (loop_index = 0; loop_index < max_count; loop_index++) {
                if ((active_entry->flex_attribute.mmu_queue_num) == loop_index) {
                    offset_array[attribute_num][loop_index] = 1;
                    break;
                }
            }
            if (loop_index == max_count) {
                offset_array[attribute_num][0] = 1;
            }
            attribute_num++;
        }

        /* Write final offsets from the offset_arrays */
        for (index=0; index<num_entries; index++) {
            i = index;
            j = 0;
            valid = 1;
            while (j < attribute_num) {
                attribute_val = i & ((1<<offset_width[j])-1);
                valid &= offset_array[j][attribute_val];
                i >>= offset_width[j];
                j++;
            }
            if (valid) {
                update_offset_map(active_entry, index, instance_id);
            }
        }
    }

exit:
    /* Free allocated buffers */
    for (i = 0; i < 8; i++) {
        if (offset_array[i]) {
           SHR_FREE(offset_array[i]);
        }
    }

    SHR_FUNC_EXIT();
}

/*
 * scan all intances for this entry and combined the attributes.
 */
int
generate_combined_attributes(int unit,
                             active_entry_t *active_entry,
                             ctr_flex_user_data_t *user_data,
                             flex_attribute_t *combined_flex_attribute)
{
    uint32_t i, active_instances;
    uint32_t pkt_attribute_template_id;
    ctr_flex_pkt_attribute_template_t pkt_attribute_template = {0};

    SHR_FUNC_ENTER(unit);

    active_instances = active_entry->active_instances;

    /* Combine the attributes from all active instances */
    for (i = 0; i < active_instances; i++) {

        pkt_attribute_template_id =
            active_entry->pkt_attribute_template_id[i];

        SHR_IF_ERR_EXIT
            (lookup_flex_pkt_attr_table (unit,
                                         active_entry->ingress,
                                         true,
                                         pkt_attribute_template_id,
                                         user_data,
                                         &pkt_attribute_template));

        if (pkt_attribute_template.ip) {
           combined_flex_attribute->ip = 1;
        }
        if (pkt_attribute_template.drop) {
           combined_flex_attribute->drop = 1;
        }
        if (pkt_attribute_template.pkt_type_all ||
            pkt_attribute_template.pkt_type_unicast ||
            pkt_attribute_template.pkt_type_multicast ||
            pkt_attribute_template.pkt_type_unknown ||
            pkt_attribute_template.pkt_type_control ||
            pkt_attribute_template.pkt_type_oam ||
            pkt_attribute_template.pkt_type_bfd ||
            pkt_attribute_template.pkt_type_bpdu ||
            pkt_attribute_template.pkt_type_icnm ||
            pkt_attribute_template.pkt_type_1588 ||
            pkt_attribute_template.pkt_type_known_l2uc ||
            pkt_attribute_template.pkt_type_unknown_l2uc ||
            pkt_attribute_template.pkt_type_l2bc ||
            pkt_attribute_template.pkt_type_known_l2mc ||
            pkt_attribute_template.pkt_type_unknown_l2mc ||
            pkt_attribute_template.pkt_type_known_l3uc ||
            pkt_attribute_template.pkt_type_unknown_l3uc ||
            pkt_attribute_template.pkt_type_known_ipmc ||
            pkt_attribute_template.pkt_type_unknown_ipmc ||
            pkt_attribute_template.pkt_type_known_mpls_l2 ||
            pkt_attribute_template.pkt_type_known_mpls_l3 ||
            pkt_attribute_template.pkt_type_known_mpls ||
            pkt_attribute_template.pkt_type_unknown_mpls ||
            pkt_attribute_template.pkt_type_known_mpls_mc ||
            pkt_attribute_template.pkt_type_known_mim ||
            pkt_attribute_template.pkt_type_unknown_mim ||
            pkt_attribute_template.pkt_type_known_trill ||
            pkt_attribute_template.pkt_type_unknown_trill ||
            pkt_attribute_template.pkt_type_known_niv ||
            pkt_attribute_template.pkt_type_unknown_niv
           ) {
           combined_flex_attribute->pkt_type = 1;
        }
        if (pkt_attribute_template.tos_dscp) {
           combined_flex_attribute->tos_dscp = 1;
        }
        if (pkt_attribute_template.tos_ecn) {
           combined_flex_attribute->tos_ecn = 1;
        }
        if (pkt_attribute_template.port) {
           combined_flex_attribute->port = 1;
        }
        if (pkt_attribute_template.outer_dot1p) {
           combined_flex_attribute->outer_dot1p = 1;
        }
        if (pkt_attribute_template.inner_dot1p) {
           combined_flex_attribute->inner_dot1p = 1;
        }
        if (pkt_attribute_template.vlan_ut ||
            pkt_attribute_template.vlan_sit ||
            pkt_attribute_template.vlan_sot ||
            pkt_attribute_template.vlan_dt) {
           combined_flex_attribute->vlan = 1;
        }
        if (pkt_attribute_template.int_pri) {
           combined_flex_attribute->int_pri = 1;
        }
        if (pkt_attribute_template.pre_fp_g_color ||
            pkt_attribute_template.pre_fp_y_color ||
            pkt_attribute_template.pre_fp_r_color) {
           combined_flex_attribute->pre_fp_color = 1;
        }
        if (pkt_attribute_template.fp_g_color ||
            pkt_attribute_template.fp_y_color ||
            pkt_attribute_template.fp_r_color) {
           combined_flex_attribute->fp_color = 1;
        }
        if (pkt_attribute_template.elephant_pkt) {
           combined_flex_attribute->elephant_pkt = 1;
        }
        if (pkt_attribute_template.tcp_flag_type_fin ||
            pkt_attribute_template.tcp_flag_type_syn ||
            pkt_attribute_template.tcp_flag_type_rst ||
            pkt_attribute_template.tcp_flag_type_psh ||
            pkt_attribute_template.tcp_flag_type_ack ||
            pkt_attribute_template.tcp_flag_type_urg ||
            pkt_attribute_template.tcp_flag_type_ece ||
            pkt_attribute_template.tcp_flag_type_cwr) {
           combined_flex_attribute->tcp_flags = 1;
        }
        if (pkt_attribute_template.int_cn) {
           combined_flex_attribute->int_cn = 1;
        }
        if (pkt_attribute_template.access_svp ||
            pkt_attribute_template.network_svp) {
           combined_flex_attribute->svp_type = 1;
        }
        if (pkt_attribute_template.access_dvp ||
            pkt_attribute_template.network_dvp) {
           combined_flex_attribute->dvp_type = 1;
        }
        if (pkt_attribute_template.congestion_marked) {
           combined_flex_attribute->congestion_marked = 1;
        }
        if (pkt_attribute_template.unicast_queueing) {
           combined_flex_attribute->unicast_queueing = 1;
        }
        if (pkt_attribute_template.mmu_queue_num) {
           combined_flex_attribute->mmu_queue_num = 1;
        }
    }
    return SHR_E_NONE;

exit:
    SHR_FUNC_EXIT();

}

/*
 * Computes the offset map and programs the OFFET_TABLE with counter offsets
 * and counter enable. Also programs the packet key selectors bits.
 */
int
process_attributes(int unit, active_entry_t *active_entry,
                   ctr_flex_user_data_t *user_data)
{
    flex_attribute_t combined_flex_attribute = {0};
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

   /* Lookup all intances for this entry and combine the attributes */
    if (active_entry->entry_data->offset_mode == ctrl->ctr_flex_enum.direct_map_mode) {
        SHR_IF_ERR_EXIT
            (bcmptm_ctr_flex_enable_direct_map(unit,
                                               active_entry->ingress,
                                               active_entry->entry_data->pool_id));
        return SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT
        (generate_combined_attributes(unit,
                                      active_entry,
                                      user_data,
                                      &combined_flex_attribute));

    /* Compute the offset values for active instances */
    SHR_IF_ERR_VERBOSE_EXIT
        (process_offset_table(unit,
                              active_entry,
                              user_data,
                              &combined_flex_attribute));

    /* Update offset table and key selectors */
    SHR_IF_ERR_EXIT
        (update_packet_selectors(unit,
                                 CTR_FLEX_SET_BITS,
                                 active_entry,
                                 &combined_flex_attribute));
exit:
    if (SHR_FUNC_VAL_IS(SHR_E_CONFIG)) {
        return SHR_E_NONE;
    }
    SHR_FUNC_EXIT();
}
