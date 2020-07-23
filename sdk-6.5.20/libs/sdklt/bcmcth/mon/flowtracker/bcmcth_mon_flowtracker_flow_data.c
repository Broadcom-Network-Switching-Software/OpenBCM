/*! \file bcmcth_mon_flowtracker_flow_data.c
 *
 * This file contains Flowtracker custom handler fucntions
 * for handling insert, delete, update ,lookup and
 * validate of MON_FLOWTRACKER_FLOW_DATA table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/mcs_shared/mcs_mon_flowtracker_msg.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_mon_flowtracker_flow.h>
#include <bcmcth/bcmcth_mon_flowtracker_static.h>
#include <bcmcth/bcmcth_mon_flowtracker_int.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_MON


static int
bcmcth_mon_flowtracker_flow_data_parse(int unit, const bcmltd_fields_t *in,
                                           bcmcth_mon_flowtracker_flow_data_t *param)
{

    uint32_t idx, fid;
    uint64_t fval;
    bcmcth_mon_flowtracker_flow_data_t *entry = param;
    SHR_FUNC_ENTER(unit);

    if ((in == NULL) || (entry == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < in->count; idx++) {
        fid      = in->field[idx]->id;
        fval    = in->field[idx]->data;

        switch (fid) {
            case MON_FLOWTRACKER_FLOW_DATAt_MON_FLOWTRACKER_GROUP_IDf:
                entry->group_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_STATICf:
                entry->is_static = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_SRC_IPV4f:
                entry->src_ipv4 = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_DST_IPV4f:
                entry->dst_ipv4 = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_SRC_L4_PORTf:
                entry->src_l4_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_DST_L4_PORTf:
                entry->dst_l4_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_IP_PROTOf:
                entry->ip_proto = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV4f:
                entry->inner_src_ipv4 = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV4f:
                entry->inner_dst_ipv4 = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_L4_PORTf:
                entry->inner_src_l4_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_L4_PORTf:
                entry->inner_dst_l4_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_IP_PROTOf:
                entry->inner_ip_proto = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_VNIDf:
                entry->vnid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_CUSTOM_KEY_UPPERf:
                entry->custom_key_upper = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_CUSTOM_KEY_LOWERf:
                entry->custom_key_lower = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_ING_PORT_IDf:
                entry->ing_port_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_UPPERf:
                BCMCTH_IPV6_ADDR_UPPER_SET(entry->inner_src_ipv6, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_LOWERf:
                BCMCTH_IPV6_ADDR_LOWER_SET(entry->inner_src_ipv6, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_UPPERf:
                BCMCTH_IPV6_ADDR_UPPER_SET(entry->inner_dst_ipv6, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_LOWERf:
                BCMCTH_IPV6_ADDR_LOWER_SET(entry->inner_dst_ipv6, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_DATAu_PKT_COUNTf:
                entry->pkt_count = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_DATAu_BYTE_COUNTf:
                entry->byte_count = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_DATAu_FLOW_START_TIMESTAMP_MSECSf:
                entry->flow_start_time_msecs = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_DATAu_OBSERVATION_TIMESTAMP_MSECSf:
                entry->observation_time_msecs = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
                break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_flow_data_fill(int unit,
                             bcmcth_mon_flowtracker_flow_data_t *entry,
                             bcmltd_fields_t *out
                            )
{
    size_t      num_fid = 0, count = 0;
    uint32_t    idx = 0, fid = 0;
    uint32_t    table_sz = 0;
    bcmlrd_fid_t   *fid_list = NULL;
    bcmltd_field_t *field = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, MON_FLOWTRACKER_FLOW_DATAt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthMonFtStatic");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               MON_FLOWTRACKER_FLOW_DATAt,
                               num_fid,
                               fid_list,
                               &count));
    out->count = count;
    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];
        field = out->field[idx];
        field->id = fid;

        switch (fid) {
            case MON_FLOWTRACKER_FLOW_DATAt_MON_FLOWTRACKER_GROUP_IDf:
                field->data = entry->group_id;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_STATICf:
                field->data = entry->is_static;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_SRC_IPV4f:
                field->data = entry->src_ipv4;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_DST_IPV4f:
                field->data = entry->dst_ipv4;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_SRC_L4_PORTf:
                field->data = entry->src_l4_port;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_DST_L4_PORTf:
                field->data = entry->dst_l4_port;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_IP_PROTOf:
                field->data = entry->ip_proto;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV4f:
                field->data = entry->inner_src_ipv4;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV4f:
                field->data = entry->inner_dst_ipv4;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_L4_PORTf:
                field->data = entry->inner_src_l4_port;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_L4_PORTf:
                field->data = entry->inner_dst_l4_port;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_IP_PROTOf:
                field->data = entry->inner_ip_proto;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_VNIDf:
                field->data = entry->vnid;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_CUSTOM_KEY_UPPERf:
                field->data = entry->custom_key_upper;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_CUSTOM_KEY_LOWERf:
                field->data = entry->custom_key_lower;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_ING_PORT_IDf:
                field->data = entry->ing_port_id;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_UPPERf:
                field->data = BCMCTH_IPV6_ADDR_UPPER_GET(entry->inner_src_ipv6);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_LOWERf:
                field->data = BCMCTH_IPV6_ADDR_LOWER_GET(entry->inner_src_ipv6);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_UPPERf:
                field->data = BCMCTH_IPV6_ADDR_UPPER_GET(entry->inner_dst_ipv6);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_LOWERf:
                field->data = BCMCTH_IPV6_ADDR_LOWER_GET(entry->inner_dst_ipv6);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_DATAu_PKT_COUNTf:
                field->data = entry->pkt_count;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_DATAu_BYTE_COUNTf:
                field->data = entry->byte_count;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_DATAu_FLOW_START_TIMESTAMP_MSECSf:
                field->data = entry->flow_start_time_msecs;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_DATAu_OBSERVATION_TIMESTAMP_MSECSf:
                field->data = entry->observation_time_msecs;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
                break;
        }
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_flowtracker_flow_data_get_msg_fill(
                 bcmcth_mon_flowtracker_flow_data_t *entry,
                 mcs_ft_msg_ctrl_group_flow_data_get_t *msg)
{
    int lower_size, upper_size;

    msg->src_ip             = entry->src_ipv4;
    msg->dst_ip             = entry->dst_ipv4;
    msg->inner_src_ip       = entry->inner_src_ipv4;
    msg->inner_dst_ip       = entry->inner_dst_ipv4;
    msg->vnid               = entry->vnid;
    msg->group_idx          = entry->group_id;
    msg->l4_src_port        = entry->src_l4_port;
    msg->l4_dst_port        = entry->dst_l4_port;
    msg->inner_l4_src_port  = entry->inner_src_l4_port;
    msg->inner_l4_dst_port  = entry->inner_dst_l4_port;
    msg->ip_protocol        = entry->ip_proto;
    msg->inner_ip_protocol  = entry->inner_ip_proto;
    msg->custom_length      = MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH;
    lower_size = upper_size = MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH/2;
    sal_memcpy(&(msg->custom[0]), &entry->custom_key_lower, lower_size);
    sal_memcpy(&(msg->custom[lower_size]), &entry->custom_key_lower,
               upper_size);
    msg->in_port            =  entry->ing_port_id;
    sal_memcpy(msg->inner_src_ipv6, entry->inner_src_ipv6, sizeof(shr_ip6_t));
    sal_memcpy(msg->inner_dst_ipv6, entry->inner_dst_ipv6, sizeof(shr_ip6_t));
}

static void
bcmcth_mon_flowtracker_flow_data_set_msg_fill(
                 bcmcth_mon_flowtracker_flow_data_t *entry,
                 mcs_ft_msg_ctrl_group_flow_data_set_t *msg)
{
    int lower_size, upper_size;

    msg->src_ipv4             = entry->src_ipv4;
    msg->dst_ipv4             = entry->dst_ipv4;
    msg->inner_src_ipv4       = entry->inner_src_ipv4;
    msg->inner_dst_ipv4       = entry->inner_dst_ipv4;
    msg->vnid                 = entry->vnid;
    msg->group_idx            = entry->group_id;
    msg->src_l4_port          = entry->src_l4_port;
    msg->dst_l4_port          = entry->dst_l4_port;
    msg->inner_src_l4_port    = entry->inner_src_l4_port;
    msg->inner_dst_l4_port    = entry->inner_dst_l4_port;
    msg->ip_proto             = entry->ip_proto;
    msg->inner_ip_proto       = entry->inner_ip_proto;
    msg->custom_length        = MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH;
    lower_size = upper_size   = MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH/2;
    sal_memcpy(&(msg->custom[0]), &entry->custom_key_lower, lower_size);
    sal_memcpy(&(msg->custom[lower_size]), &entry->custom_key_lower,
               upper_size);
    msg->in_port              =  entry->ing_port_id;
    sal_memcpy(msg->inner_src_ipv6, entry->inner_src_ipv6, sizeof(shr_ip6_t));
    sal_memcpy(msg->inner_dst_ipv6, entry->inner_dst_ipv6, sizeof(shr_ip6_t));
}

static void
bcmcth_mon_flowtracker_flow_data_reply_msg_extract(
                 mcs_ft_msg_ctrl_group_flow_data_get_reply_t *msg,
                 bcmcth_mon_flowtracker_flow_data_t *entry)
{
    entry->pkt_count  = (uint64_t)(msg->pkt_count_lower)  |
                        ((uint64_t)(msg->pkt_count_upper)) << 32;
    entry->byte_count = (uint64_t)(msg->byte_count_lower) |
                        ((uint64_t)(msg->byte_count_upper)) << 32;
    entry->flow_start_time_msecs = ((uint64_t)(msg->flow_start_ts_lower)) |
                                   ((uint64_t)(msg->flow_start_ts_upper)) << 32;
    entry->observation_time_msecs = ((uint64_t)(msg->obs_ts_lower)) |
                                    ((uint64_t)(msg->obs_ts_upper)) << 32;
    entry->is_static              = msg->is_static;
}

static void
bcmcth_mon_flowtracker_get_first_next_flow_data_reply_msg_extract(
                 mcs_ft_msg_ctrl_flow_data_get_first_reply_t *msg,
                 uint32_t *curr_flow_index,
                 bcmcth_mon_flowtracker_flow_data_t *entry)
{
    int lower_size, upper_size;
    *curr_flow_index  = msg->flow_idx;

    entry->group_id               = msg->group_idx;
    entry->src_ipv4               = msg->src_ipv4;
    entry->dst_ipv4               = msg->dst_ipv4;
    entry->inner_src_ipv4         = msg->inner_src_ipv4;
    entry->inner_dst_ipv4         = msg->inner_dst_ipv4;
    entry->src_l4_port            = msg->src_l4_port;
    entry->dst_l4_port            = msg->dst_l4_port;
    entry->inner_src_l4_port      = msg->inner_src_l4_port;
    entry->inner_dst_l4_port      = msg->inner_dst_l4_port;
    entry->ip_proto               = msg->ip_proto;
    entry->inner_ip_proto         = msg->inner_ip_proto;
    entry->ing_port_id            = msg->in_port;
    lower_size = upper_size       = MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH/2;
    sal_memcpy(&entry->custom_key_lower, &(msg->custom[0]), lower_size);
    sal_memcpy(&entry->custom_key_lower, &(msg->custom[lower_size]),
               upper_size);
    entry->inner_ip_proto         = msg->inner_ip_proto;
    entry->vnid                   = msg->vnid;
    sal_memcpy(entry->inner_src_ipv6, msg->inner_src_ipv6, sizeof(shr_ip6_t));
    sal_memcpy(entry->inner_dst_ipv6, msg->inner_dst_ipv6, sizeof(shr_ip6_t));
    entry->pkt_count              = ((uint64_t)(msg->pkt_count_lower)) |
                                    (((uint64_t)(msg->pkt_count_upper)) << 32);
    entry->byte_count             = ((uint64_t)(msg->byte_count_lower)) |
                                    (((uint64_t)(msg->byte_count_upper)) << 32);
    entry->flow_start_time_msecs  = ((uint64_t)(msg->flow_start_ts_lower)) |
                                    (((uint64_t)msg->flow_start_ts_upper) << 32);
    entry->observation_time_msecs = ((uint64_t)(msg->obs_ts_lower)) |
                                    (((uint64_t)(msg->obs_ts_upper)) << 32);
    entry->is_static              = msg->is_static;
}

static int
bcmcth_mon_flowtracker_flow_data_get(int unit,
                                     bcmcth_mon_flowtracker_flow_data_t *entry)
{
    mcs_ft_msg_ctrl_group_flow_data_get_t send_msg = {0};
    mcs_ft_msg_ctrl_group_flow_data_get_reply_t reply_msg = {0};
    SHR_FUNC_ENTER(unit);

    /* Fill the flow data get message */
    bcmcth_mon_flowtracker_flow_data_get_msg_fill(entry, &send_msg);

    /* Send the flow data get message to UKERNEL and get the flow data */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_get_flow_data(unit, &send_msg,
                &reply_msg));

    /* Fill the flow data into the entry structure back */
    bcmcth_mon_flowtracker_flow_data_reply_msg_extract(&reply_msg, entry);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_flow_data_set(int unit,
                                     bcmcth_mon_flowtracker_flow_data_t *entry)
{
    mcs_ft_msg_ctrl_group_flow_data_set_t send_msg = {0};
    SHR_FUNC_ENTER(unit);

    /* Fill the flow data set message */
    bcmcth_mon_flowtracker_flow_data_set_msg_fill(entry, &send_msg);

    /* Send the flow data set message to UKERNEL */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_group_flow_data_set(unit, &send_msg));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_get_first_flow_data
                                (
                                 int unit,
                                 bcmcth_mon_flowtracker_flow_data_t *entry
                                )
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    uint32_t curr_flow_index;
    mcs_ft_msg_ctrl_flow_data_get_first_reply_t reply_msg = {0};
    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    if (ft_info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Send the first flow data get message to UKERNEL */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_get_first_flow_data(unit, &reply_msg));

    /* Fill the flow data into the entry structure back */
    bcmcth_mon_flowtracker_get_first_next_flow_data_reply_msg_extract
                                                    (&reply_msg,
                                                     &curr_flow_index,
                                                     entry);

    /* Update the curr flow index to use in get next */
    ft_info->curr_flow_index = curr_flow_index;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_get_next_flow_data
                                (
                                 int unit,
                                 bcmcth_mon_flowtracker_flow_data_t *entry
                                )
{
    bcmcth_mon_ft_info_t *ft_info = NULL;
    mcs_ft_msg_ctrl_flow_data_get_next_reply_t reply_msg = {0};
    uint32_t curr_flow_index;
    SHR_FUNC_ENTER(unit);

    ft_info = bcmcth_mon_ft_info_get(unit);
    if (ft_info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Send the next flow data get message to UKERNEL */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_get_next_flow_data
                                                    (unit,
                                                     ft_info->curr_flow_index,
                                                     &reply_msg));

    /* Fill the flow data into the entry structure back */
    bcmcth_mon_flowtracker_get_first_next_flow_data_reply_msg_extract
                                                    (&reply_msg,
                                                     &curr_flow_index,
                                                     entry);

    /* Update the curr flow index to use in get next */
    ft_info->curr_flow_index = curr_flow_index;

exit:
    SHR_FUNC_EXIT();
}



/*******************************************************************************
 * Public functions
 */

int
bcmcth_mon_flowtracker_flow_validate(int unit,
                                     bcmlt_opcode_t opcode,
                                     const bcmltd_fields_t *in,
                                     const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_flow_data_t param;
    bcmcth_mon_ft_info_t *info = NULL;
    SHR_FUNC_ENTER(unit);

    /*
     * Return error for un-supported opcodes.
     */
    if ((opcode != BCMLT_OPCODE_LOOKUP) &&
            (opcode != BCMLT_OPCODE_TRAVERSE) &&
            (opcode != BCMLT_OPCODE_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }


    if (FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    /*
     * Validate if flow group exists.
     */
    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_flow_data_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_flow_data_parse(unit, in, &param));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_flow_insert(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    /*
     * Update only table.
     * Unexpected opcode.
     */
     SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_flow_lookup(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_flow_data_t param;
    bcmcth_mon_ft_info_t *info = NULL;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_flow_data_t));

    /* Get the input fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_flow_data_parse(unit, in, &param));

    if (info->ha->run == true) {
        /* Get the output fields based on input fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_flow_data_get(unit, &param));

        /* Fill the output fields into the appropriate out structure */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_flow_data_fill(unit, &param, out));
    } else {
         SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_flow_delete(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    /*
     * Update only table.
     * Unexpected opcode.
     */
     SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_flow_update(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_flow_data_t param;
    bcmcth_mon_ft_info_t *info = NULL;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_flow_data_t));

    if (info->ha->run == true) {
        /* Get the input fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_flow_data_parse(unit, in, &param));

        /* Send msg to UKERNEL to update. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_flow_data_set(unit, &param));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_flow_first(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_flow_data_t param;
    bcmcth_mon_ft_info_t *info = NULL;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_flow_data_t));

    if (info->ha->run == true) {
        /* Get the first flow from UKERNEL */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_get_first_flow_data(unit, &param));

        /* Fill the output fields into the appropriate out structure */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_flow_data_fill(unit, &param, out));
    } else {
         SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_flow_next(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_flow_data_t param;
    bcmcth_mon_ft_info_t *info = NULL;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_flow_data_t));

    if (info->ha->run == true) {
        /* Get the next flow from UKERNEL */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_get_next_flow_data(unit, &param));

        /* Fill the output fields into the appropriate out structure */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_flow_data_fill(unit, &param, out));
    } else {
         SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}
