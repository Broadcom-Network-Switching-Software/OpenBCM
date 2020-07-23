/*
 * $Id: ft_pack.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_pack.c
 * Purpose:     FT pack and unpack routines for:
 *              - FT Control messages
 *
 *
 * FT control messages
 *
 * FT messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The FT control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/ft.h>
#include <soc/shared/ft_msg.h>
#include <soc/shared/ft_pack.h>

/*
 * This is the SDK's messaging version when compiling for uKernel and the
 * uKernel's msg version when compiling for SDK
 */
uint32 ft_msg_version = 0;

/*
 * FT Initialization control message packing
 */
uint8*
shr_ft_msg_ctrl_init_pack(uint8 *buf, shr_ft_msg_ctrl_init_t *msg)
{
    int i, j;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->max_groups);

    _SHR_PACK_U8(buf, msg->num_pipes);

    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_PACK_U32(buf, msg->pipe[i].max_flows);

        _SHR_PACK_U8(buf, msg->pipe[i].num_ctr_pools);
        for (j = 0; j < msg->pipe[i].num_ctr_pools; j++) {
            _SHR_PACK_U8(buf, msg->pipe[i].ctr_pools[j]);
            _SHR_PACK_U16(buf, msg->pipe[i].ctr_pool_size[j]);
        }
    }

    _SHR_PACK_U32(buf, msg->rx_channel);
    _SHR_PACK_U16(buf, msg->max_export_pkt_length);
    _SHR_PACK_U32(buf, msg->cur_time_secs);

    _SHR_PACK_U32(buf, msg->export_interval_msecs);
    _SHR_PACK_U32(buf, msg->scan_interval_usecs);
    _SHR_PACK_U32(buf, msg->age_timer_tick_msecs);

    _SHR_PACK_U8(buf, msg->num_elph_profiles);

    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_PACK_U16(buf, msg->fsp_reinject_max_length);
    }

    return buf;
}

/*
 * FT Initialization control message
 */
uint8*
shr_ft_msg_ctrl_init_unpack(uint8 *buf, shr_ft_msg_ctrl_init_t *msg)
{
    int i, j;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->max_groups);

    _SHR_UNPACK_U8(buf, msg->num_pipes);

    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_UNPACK_U32(buf, msg->pipe[i].max_flows);

        _SHR_UNPACK_U8(buf, msg->pipe[i].num_ctr_pools);
        for (j = 0; j < msg->pipe[i].num_ctr_pools; j++) {
            _SHR_UNPACK_U8(buf, msg->pipe[i].ctr_pools[j]);
            _SHR_UNPACK_U16(buf, msg->pipe[i].ctr_pool_size[j]);
        }
    }

    _SHR_UNPACK_U32(buf, msg->rx_channel);
    _SHR_UNPACK_U16(buf, msg->max_export_pkt_length);
    _SHR_UNPACK_U32(buf, msg->cur_time_secs);

    _SHR_UNPACK_U32(buf, msg->export_interval_msecs);
    _SHR_UNPACK_U32(buf, msg->scan_interval_usecs);
    _SHR_UNPACK_U32(buf, msg->age_timer_tick_msecs);

    _SHR_UNPACK_U8(buf, msg->num_elph_profiles);

    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_UNPACK_U16(buf, msg->fsp_reinject_max_length);
    }

    return buf;
}

/*
 * EM key format export pack
 */
uint8*
shr_ft_msg_ctrl_em_key_format_pack(uint8 *buf,
                                   shr_ft_msg_ctrl_em_key_format_t *msg)
{
    int i, j;


    _SHR_PACK_U16(buf, msg->key_size);
    _SHR_PACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        _SHR_PACK_U8(buf, msg->qual[i]);
        _SHR_PACK_U8(buf, msg->qual_parts[i].num_parts);

        for (j = 0; j < msg->qual_parts[i].num_parts; j++) {
            _SHR_PACK_U16(buf, msg->qual_parts[i].offset[j]);
            _SHR_PACK_U8(buf, msg->qual_parts[i].width[j]);
        }
    }

    return buf;
}

/*
 * EM group create - pack
 */
uint8*
shr_ft_msg_ctrl_em_group_create_pack(uint8 *buf,
                                     shr_ft_msg_ctrl_em_group_create_t *msg)
{
    int i, j;

    _SHR_PACK_U8(buf, msg->group_idx);
    _SHR_PACK_U8(buf, msg->num_pipes);
    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_PACK_U8(buf, msg->lt_id[i]);
    }
    _SHR_PACK_U32(buf, msg->aset);
    _SHR_PACK_U16(buf, msg->key_size);
    _SHR_PACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        _SHR_PACK_U8(buf, msg->qual_info[i].qual);
        _SHR_PACK_U8(buf, msg->qual_info[i].num_parts);

        for (j = 0; j < msg->qual_info[i].num_parts; j++) {
            _SHR_PACK_U16(buf, msg->qual_info[i].part_offset[j]);
            _SHR_PACK_U8(buf, msg->qual_info[i].part_width[j]);
        }

        _SHR_PACK_U16(buf, msg->qual_info[i].width);
        _SHR_PACK_U8(buf, msg->qual_info[i].base_offset);
        _SHR_PACK_U8(buf, msg->qual_info[i].relative_offset);
    }

    _SHR_PACK_U8(buf, msg->num_tp);
    for (i = 0; i < msg->num_tp; i++) {
        _SHR_PACK_U8(buf, msg->tp_info[i].param);
        _SHR_PACK_U8(buf, msg->tp_info[i].num_qual);
        _SHR_PACK_U8(buf, msg->tp_info[i].udf);
        for (j = 0; j < msg->tp_info[i].num_qual; j++) {
            _SHR_PACK_U8(buf, msg->tp_info[i].qual[j]);
        }
    }

    return buf;
}

/*
 * EM group create - unpack
 */
uint8*
shr_ft_msg_ctrl_em_group_create_unpack(uint8 *buf,
                                       shr_ft_msg_ctrl_em_group_create_t *msg)
{
    int i, j;

    _SHR_UNPACK_U8(buf, msg->group_idx);
    _SHR_UNPACK_U8(buf, msg->num_pipes);
    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_UNPACK_U8(buf, msg->lt_id[i]);
    }
    _SHR_UNPACK_U32(buf, msg->aset);
    _SHR_UNPACK_U16(buf, msg->key_size);
    _SHR_UNPACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        _SHR_UNPACK_U8(buf, msg->qual_info[i].qual);
        _SHR_UNPACK_U8(buf, msg->qual_info[i].num_parts);

        for (j = 0; j < msg->qual_info[i].num_parts; j++) {
            _SHR_UNPACK_U16(buf, msg->qual_info[i].part_offset[j]);
            _SHR_UNPACK_U8(buf, msg->qual_info[i].part_width[j]);
        }

        _SHR_UNPACK_U16(buf, msg->qual_info[i].width);
        _SHR_UNPACK_U8(buf, msg->qual_info[i].base_offset);
        _SHR_UNPACK_U8(buf, msg->qual_info[i].relative_offset);
    }

    _SHR_UNPACK_U8(buf, msg->num_tp);
    for (i = 0; i < msg->num_tp; i++) {
        _SHR_UNPACK_U8(buf, msg->tp_info[i].param);
        _SHR_UNPACK_U8(buf, msg->tp_info[i].num_qual);
        _SHR_UNPACK_U8(buf, msg->tp_info[i].udf);
        for (j = 0; j < msg->tp_info[i].num_qual; j++) {
            _SHR_UNPACK_U8(buf, msg->tp_info[i].qual[j]);
        }
    }

    return buf;
}

/*
 * EM key format export unpack
 */
uint8*
shr_ft_msg_ctrl_em_key_format_unpack(uint8 *buf,
                                     shr_ft_msg_ctrl_em_key_format_t *msg)
{
    int i, j;


    _SHR_UNPACK_U16(buf, msg->key_size);
    _SHR_UNPACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        _SHR_UNPACK_U8(buf, msg->qual[i]);
        _SHR_UNPACK_U8(buf, msg->qual_parts[i].num_parts);

        for (j = 0; j < msg->qual_parts[i].num_parts; j++) {
            _SHR_UNPACK_U16(buf, msg->qual_parts[i].offset[j]);
            _SHR_UNPACK_U8(buf, msg->qual_parts[i].width[j]);
        }
    }

    return buf;
}

/*
 * Group create pack
 */
uint8*
shr_ft_msg_ctrl_group_create_pack(uint8 *buf,
                                  shr_ft_msg_ctrl_group_create_t *msg)
{
    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->flow_limit);
    _SHR_PACK_U32(buf, msg->aging_interval_msecs);
    _SHR_PACK_U32(buf, msg->domain_id);

    return buf;
}

/*
 * Group create unpack
 */
uint8*
shr_ft_msg_ctrl_group_create_unpack(uint8 *buf,
                                    shr_ft_msg_ctrl_group_create_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->flow_limit);
    _SHR_UNPACK_U32(buf, msg->aging_interval_msecs);
    _SHR_UNPACK_U32(buf, msg->domain_id);

    return buf;
}

/*
 * Group get pack
 */
uint8*
shr_ft_msg_ctrl_group_get_pack(uint8* buf, shr_ft_msg_ctrl_group_get_t *msg)
{
    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->flow_limit);
    _SHR_PACK_U32(buf, msg->flow_count);
    _SHR_PACK_U32(buf, msg->domain_id);
    _SHR_PACK_U32(buf, msg->aging_interval);
    _SHR_PACK_U16(buf, msg->elph_profile_id);
    _SHR_PACK_U16(buf, msg->qos_profile_id);
    _SHR_PACK_U16(buf, msg->collector_id);
    _SHR_PACK_U16(buf, msg->template_id);

    return buf;
}

/*
 * Group get unpack
 */
uint8*
shr_ft_msg_ctrl_group_get_unpack(uint8* buf, shr_ft_msg_ctrl_group_get_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->flow_limit);
    _SHR_UNPACK_U32(buf, msg->flow_count);
    _SHR_UNPACK_U32(buf, msg->domain_id);
    _SHR_UNPACK_U32(buf, msg->aging_interval);
    _SHR_UNPACK_U16(buf, msg->elph_profile_id);
    _SHR_UNPACK_U16(buf, msg->qos_profile_id);
    _SHR_UNPACK_U16(buf, msg->collector_id);
    _SHR_UNPACK_U16(buf, msg->template_id);

    return buf;
}


/*
 * Group update pack
 */
uint8*
shr_ft_msg_ctrl_group_update_pack(uint8* buf,
                                  shr_ft_msg_ctrl_group_update_t *msg)
{
    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->update);
    _SHR_PACK_U32(buf, msg->param0);

    return buf;
}

/*
 * Group update unpack
 */
uint8*
shr_ft_msg_ctrl_group_update_unpack(uint8* buf,
                                    shr_ft_msg_ctrl_group_update_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->update);
    _SHR_UNPACK_U32(buf, msg->param0);

    return buf;
}

/*
 * Create IPFIX template - pack
 */
uint8*
shr_ft_msg_ctrl_template_create_pack(uint8* buf, shr_ft_msg_ctrl_template_create_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->set_id);

    _SHR_PACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        _SHR_PACK_U8(buf, msg->info_elems[i]);
        _SHR_PACK_U16(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Create IPFIX template - unpack
 */
uint8*
shr_ft_msg_ctrl_template_create_unpack(uint8* buf, shr_ft_msg_ctrl_template_create_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->set_id);

    _SHR_UNPACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        _SHR_UNPACK_U8(buf, msg->info_elems[i]);
        _SHR_UNPACK_U16(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Get IPFIX template - pack
 */
uint8*
shr_ft_msg_ctrl_template_get_pack(uint8* buf, shr_ft_msg_ctrl_template_get_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->set_id);

    _SHR_PACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        _SHR_PACK_U8(buf, msg->info_elems[i]);
        _SHR_PACK_U16(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Get IPFIX template - unpack
 */
uint8*
shr_ft_msg_ctrl_template_get_unpack(uint8* buf, shr_ft_msg_ctrl_template_get_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->set_id);

    _SHR_UNPACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        _SHR_UNPACK_U8(buf, msg->info_elems[i]);
        _SHR_UNPACK_U16(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Create collector - pack
 */
uint8*
shr_ft_msg_ctrl_collector_create_pack(uint8* buf, shr_ft_msg_ctrl_collector_create_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap[i]);
    }

    _SHR_PACK_U16(buf, msg->mtu);
    _SHR_PACK_U32(buf, msg->ip_base_checksum);
    _SHR_PACK_U32(buf, msg->udp_base_checksum);
    _SHR_PACK_U16(buf, msg->ip_offset);
    _SHR_PACK_U16(buf, msg->udp_offset);

    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_PACK_U32(buf, msg->component_id);
        _SHR_PACK_U16(buf, msg->system_id_length);
        for (i = 0; i < msg->system_id_length; i++) {
            _SHR_PACK_U8(buf, msg->system_id[i]);
        }
    }
    return buf;
}

/*
 * Create collector - unpack
 */
uint8*
shr_ft_msg_ctrl_collector_create_unpack(uint8* buf, shr_ft_msg_ctrl_collector_create_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap[i]);
    }

    _SHR_UNPACK_U16(buf, msg->mtu);
    _SHR_UNPACK_U32(buf, msg->ip_base_checksum);
    _SHR_UNPACK_U32(buf, msg->udp_base_checksum);
    _SHR_UNPACK_U16(buf, msg->ip_offset);
    _SHR_UNPACK_U16(buf, msg->udp_offset);

    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_UNPACK_U32(buf, msg->component_id);
        _SHR_UNPACK_U16(buf, msg->system_id_length);
        for (i = 0; i < msg->system_id_length; i++) {
            _SHR_UNPACK_U8(buf, msg->system_id[i]);
        }
    }

    return buf;
}

/*
 * Get collector - pack
 */
uint8*
shr_ft_msg_ctrl_collector_get_pack(uint8* buf, shr_ft_msg_ctrl_collector_get_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap[i]);
    }

    _SHR_PACK_U16(buf, msg->mtu);
    _SHR_PACK_U16(buf, msg->ip_base_checksum);
    _SHR_PACK_U16(buf, msg->udp_base_checksum);
    _SHR_PACK_U16(buf, msg->ip_offset);
    _SHR_PACK_U16(buf, msg->udp_offset);

    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_PACK_U32(buf, msg->component_id);
        _SHR_PACK_U16(buf, msg->system_id_length);
        for (i = 0; i < msg->system_id_length; i++) {
            _SHR_PACK_U8(buf, msg->system_id[i]);
        }
    }

    return buf;
}

/*
 * Get collector - unpack
 */
uint8*
shr_ft_msg_ctrl_collector_get_unpack(uint8* buf, shr_ft_msg_ctrl_collector_get_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap[i]);
    }

    _SHR_UNPACK_U16(buf, msg->mtu);
    _SHR_UNPACK_U16(buf, msg->ip_base_checksum);
    _SHR_UNPACK_U16(buf, msg->udp_base_checksum);
    _SHR_UNPACK_U16(buf, msg->ip_offset);
    _SHR_UNPACK_U16(buf, msg->udp_offset);

    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_UNPACK_U32(buf, msg->component_id);
        _SHR_UNPACK_U16(buf, msg->system_id_length);
        for (i = 0; i < msg->system_id_length; i++) {
            _SHR_UNPACK_U8(buf, msg->system_id[i]);
        }
    }

    return buf;
}

/*
 * Get data associated with a flow - pack
 */
uint8*
shr_ft_msg_ctrl_group_flow_data_get_pack(uint8* buf,
                                         shr_ft_msg_ctrl_group_flow_data_get_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->src_ip);
    _SHR_PACK_U32(buf, msg->dst_ip);
    _SHR_PACK_U16(buf, msg->l4_src_port);
    _SHR_PACK_U16(buf, msg->l4_dst_port);
    _SHR_PACK_U8(buf, msg->ip_protocol);

    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_PACK_U32(buf, msg->vnid);
        _SHR_PACK_U32(buf, msg->inner_src_ip);
        _SHR_PACK_U32(buf, msg->inner_dst_ip);
        _SHR_PACK_U16(buf, msg->inner_l4_src_port);
        _SHR_PACK_U16(buf, msg->inner_l4_dst_port);
        _SHR_PACK_U8(buf, msg->inner_ip_protocol);
        _SHR_PACK_U8(buf, msg->custom_length);
        for (i = 0; i < msg->custom_length; i++) {
            _SHR_PACK_U8(buf, msg->custom[i]);
        }
    }

    if (ft_msg_version >= FT_MSG_VERSION_2) {
        _SHR_PACK_U16(buf, msg->in_port);
    }

    if (ft_msg_version >= FT_MSG_VERSION_3) {
        for(i = 0; i < 16; i++) {
            _SHR_PACK_U8(buf, msg->inner_src_ipv6_addr[i]);
        }
        for(i = 0; i < 16; i++) {
            _SHR_PACK_U8(buf, msg->inner_dst_ipv6_addr[i]);
        }
    }
    return buf;
}

/*
 * Get data associated with a flow - unpack
 */
uint8*
shr_ft_msg_ctrl_group_flow_data_get_unpack(uint8* buf,
                                          shr_ft_msg_ctrl_group_flow_data_get_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->src_ip);
    _SHR_UNPACK_U32(buf, msg->dst_ip);
    _SHR_UNPACK_U16(buf, msg->l4_src_port);
    _SHR_UNPACK_U16(buf, msg->l4_dst_port);
    _SHR_UNPACK_U8(buf, msg->ip_protocol);

    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_UNPACK_U32(buf, msg->vnid);
        _SHR_UNPACK_U32(buf, msg->inner_src_ip);
        _SHR_UNPACK_U32(buf, msg->inner_dst_ip);
        _SHR_UNPACK_U16(buf, msg->inner_l4_src_port);
        _SHR_UNPACK_U16(buf, msg->inner_l4_dst_port);
        _SHR_UNPACK_U8(buf, msg->inner_ip_protocol);
        _SHR_UNPACK_U8(buf, msg->custom_length);
        for (i = 0; i < msg->custom_length; i++) {
            _SHR_UNPACK_U8(buf, msg->custom[i]);
        }
    }
    if (ft_msg_version >= FT_MSG_VERSION_2) {
        _SHR_UNPACK_U16(buf, msg->in_port);
    }

    if (ft_msg_version >= FT_MSG_VERSION_3) {
        for(i = 0; i < 16; i++) {
            _SHR_UNPACK_U8(buf, msg->inner_src_ipv6_addr[i]);
        }
        for(i = 0; i < 16; i++) {
            _SHR_UNPACK_U8(buf, msg->inner_dst_ipv6_addr[i]);
        }
    }
    return buf;
}

/*
 * Data associated with a flow - pack
 */
uint8*
shr_ft_msg_ctrl_group_flow_data_get_reply_pack(
                                    uint8* buf,
                                    shr_ft_msg_ctrl_group_flow_data_get_reply_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->pkt_count_upper);
    _SHR_PACK_U32(buf, msg->pkt_count_lower);
    _SHR_PACK_U32(buf, msg->byte_count_upper);
    _SHR_PACK_U32(buf, msg->byte_count_lower);
    _SHR_PACK_U32(buf, msg->flow_start_ts_upper);
    _SHR_PACK_U32(buf, msg->flow_start_ts_lower);
    _SHR_PACK_U32(buf, msg->obs_ts_upper);
    _SHR_PACK_U32(buf, msg->obs_ts_lower);
    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_PACK_U8(buf, msg->num_drop_reasons);
        for (i = 0; i < msg->num_drop_reasons; i++) {
            _SHR_PACK_U16(buf, msg->drop_reasons[i]);
        }
    }
    return buf;
}

/*
 * Data associated with a flow - unpack
 */
uint8*
shr_ft_msg_ctrl_group_flow_data_get_reply_unpack(
                                    uint8* buf,
                                    shr_ft_msg_ctrl_group_flow_data_get_reply_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->pkt_count_upper);
    _SHR_UNPACK_U32(buf, msg->pkt_count_lower);
    _SHR_UNPACK_U32(buf, msg->byte_count_upper);
    _SHR_UNPACK_U32(buf, msg->byte_count_lower);
    _SHR_UNPACK_U32(buf, msg->flow_start_ts_upper);
    _SHR_UNPACK_U32(buf, msg->flow_start_ts_lower);
    _SHR_UNPACK_U32(buf, msg->obs_ts_upper);
    _SHR_UNPACK_U32(buf, msg->obs_ts_lower);
    if (ft_msg_version >= FT_MSG_VERSION_1) {
        _SHR_UNPACK_U8(buf, msg->num_drop_reasons);
        for (i = 0; i < msg->num_drop_reasons; i++) {
            _SHR_UNPACK_U16(buf, msg->drop_reasons[i]);
        }
    }

    return buf;
}

/*
 * SER event - pack
 */
uint8*
shr_ft_msg_ctrl_ser_event_pack(uint8* buf, shr_ft_msg_ctrl_ser_event_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U8(buf, msg->pipe);
    _SHR_PACK_U32(buf, msg->mem);
    _SHR_PACK_U32(buf, msg->index);

    return buf;
}

/*
 * SER event - unpack
 */
uint8*
shr_ft_msg_ctrl_ser_event_unpack(uint8* buf, shr_ft_msg_ctrl_ser_event_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U8(buf, msg->pipe);
    _SHR_UNPACK_U32(buf, msg->mem);
    _SHR_UNPACK_U32(buf, msg->index);

    return buf;
}


/*
 * Template set transmit - pack
 */
uint8*
shr_ft_msg_ctrl_template_xmit_pack(uint8* buf,
                                   shr_ft_msg_ctrl_template_xmit_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->template_id);
    _SHR_PACK_U16(buf, msg->collector_id);
    _SHR_PACK_U16(buf, msg->initial_burst);
    _SHR_PACK_U16(buf, msg->interval_secs);
    _SHR_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap[i]);
    }

    return buf;
}

/*
 * Template set transmit - unpack
 */
uint8*
shr_ft_msg_ctrl_template_xmit_unpack(uint8* buf,
                                     shr_ft_msg_ctrl_template_xmit_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->template_id);
    _SHR_UNPACK_U16(buf, msg->collector_id);
    _SHR_UNPACK_U16(buf, msg->initial_burst);
    _SHR_UNPACK_U16(buf, msg->interval_secs);
    _SHR_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap[i]);
    }

    return buf;
}

/*
 * Add elephant profile - pack
 */
uint8*
shr_ft_msg_ctrl_elph_profile_create_pack(uint8* buf,
                                         shr_ft_msg_ctrl_elph_profile_create_t *msg)
{
    int i;

    _SHR_PACK_U8(buf, msg->profile_idx);
    _SHR_PACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        _SHR_PACK_U16(buf, msg->filters[i].scan_count);
        _SHR_PACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        _SHR_PACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        _SHR_PACK_U32(buf, msg->filters[i].rate_high_threshold);
        _SHR_PACK_U32(buf, msg->filters[i].rate_low_threshold);
        _SHR_PACK_U8(buf, msg->filters[i].incr_rate);
    }
    return buf;
}

/*
 * Add elephant profile - unpack
 */
uint8*
shr_ft_msg_ctrl_elph_profile_create_unpack(uint8* buf,
                                           shr_ft_msg_ctrl_elph_profile_create_t *msg)
{
    int i;

    _SHR_UNPACK_U8(buf, msg->profile_idx);
    _SHR_UNPACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        _SHR_UNPACK_U16(buf, msg->filters[i].scan_count);
        _SHR_UNPACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        _SHR_UNPACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        _SHR_UNPACK_U32(buf, msg->filters[i].rate_high_threshold);
        _SHR_UNPACK_U32(buf, msg->filters[i].rate_low_threshold);
        _SHR_UNPACK_U8(buf, msg->filters[i].incr_rate);
    }
    return buf;
}

/*
 * Get elephant profile - pack
 */
uint8*
shr_ft_msg_ctrl_elph_profile_get_pack(uint8* buf,
                                      shr_ft_msg_ctrl_elph_profile_get_t *msg)
{
    int i;

    _SHR_PACK_U8(buf, msg->profile_idx);
    _SHR_PACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        _SHR_PACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        _SHR_PACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        _SHR_PACK_U32(buf, msg->filters[i].rate_high_threshold);
        _SHR_PACK_U32(buf, msg->filters[i].rate_low_threshold);
    }
    return buf;
}

/*
 * Get elephant profile - unpack
 */
uint8*
shr_ft_msg_ctrl_elph_profile_get_unpack(uint8* buf,
                                        shr_ft_msg_ctrl_elph_profile_get_t *msg)
{
    int i;

    _SHR_UNPACK_U8(buf, msg->profile_idx);
    _SHR_UNPACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        _SHR_UNPACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        _SHR_UNPACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        _SHR_UNPACK_U32(buf, msg->filters[i].rate_high_threshold);
        _SHR_UNPACK_U32(buf, msg->filters[i].rate_low_threshold);
    }
    return buf;
}

/*
 * Get learn stats - pack
 */
uint8*
shr_ft_msg_ctrl_stats_learn_get_pack(uint8* buf,
                                    shr_ft_msg_ctrl_stats_learn_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->num_status);
    for (i = 0; i < msg->num_status; i++) {
        _SHR_PACK_U32(buf, msg->count[i]);
        _SHR_PACK_U32(buf, msg->avg_usecs[i]);
    }
    return buf;
}

/*
 * Get learn stats - unpack
 */
uint8*
shr_ft_msg_ctrl_stats_learn_get_unpack(uint8* buf,
                                       shr_ft_msg_ctrl_stats_learn_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->num_status);
    for (i = 0; i < msg->num_status; i++) {
        _SHR_UNPACK_U32(buf, msg->count[i]);
        _SHR_UNPACK_U32(buf, msg->avg_usecs[i]);
    }
    return buf;
}

/*
 * Get export stats - pack
 */
uint8*
shr_ft_msg_ctrl_stats_export_get_pack(uint8* buf,
                                      shr_ft_msg_ctrl_stats_export_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_pkts);
    _SHR_PACK_U32(buf, msg->num_flows);
    _SHR_PACK_U32(buf, msg->ts);
    _SHR_PACK_U32(buf, msg->seq_num);

    return buf;
}

/*
 * Get export stats - unpack
 */
uint8*
shr_ft_msg_ctrl_stats_export_get_unpack(uint8* buf,
                                        shr_ft_msg_ctrl_stats_export_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->num_pkts);
    _SHR_UNPACK_U32(buf, msg->num_flows);
    _SHR_UNPACK_U32(buf, msg->ts);
    _SHR_UNPACK_U32(buf, msg->seq_num);

    return buf;
}

/*
 * Get age stats - pack
 */
uint8*
shr_ft_msg_ctrl_stats_age_get_pack(uint8* buf,
                                   shr_ft_msg_ctrl_stats_age_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_flows);

    return buf;
}

/*
 * Get age stats - unpack
 */
uint8*
shr_ft_msg_ctrl_stats_age_get_unpack(uint8* buf,
                                     shr_ft_msg_ctrl_stats_age_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->num_flows);

    return buf;
}

/*
 * Get elph stats - pack
 */
uint8*
shr_ft_msg_ctrl_stats_elph_get_pack(uint8* buf,
                                    shr_ft_msg_ctrl_stats_elph_t *msg)
{
    int i, j;

    _SHR_PACK_U16(buf, msg->num_states);
    _SHR_PACK_U16(buf, msg->num_actions);

    for (i = 0; i < msg->num_states; i++) {
        for (j = 0; j < msg->num_actions; j++) {
            _SHR_PACK_U32(buf, msg->trans_count[i][j]);
        }
    }

    return buf;
}

/*
 * Get elph stats - unpack
 */
uint8*
shr_ft_msg_ctrl_stats_elph_get_unpack(uint8* buf,
                                      shr_ft_msg_ctrl_stats_elph_t *msg)
{
    int i, j;

    _SHR_UNPACK_U16(buf, msg->num_states);
    _SHR_UNPACK_U16(buf, msg->num_actions);

    for (i = 0; i < msg->num_states; i++) {
        for (j = 0; j < msg->num_actions; j++) {
            _SHR_UNPACK_U32(buf, msg->trans_count[i][j]);
        }
    }

    return buf;
}

/*
 * SDK features - pack
 */
uint8*
shr_ft_msg_ctrl_sdk_features_pack(uint8* buf,
                                  shr_ft_msg_ctrl_sdk_features_t *msg)
{
    _SHR_PACK_U32(buf, msg->features);
    _SHR_PACK_U8(buf, msg->cfg_mode);

    return buf;
}

/*
 * SDK features - unpack
 */
uint8*
shr_ft_msg_ctrl_sdk_features_unpack(uint8* buf,
                                    shr_ft_msg_ctrl_sdk_features_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->features);
    _SHR_UNPACK_U8(buf, msg->cfg_mode);

    return buf;
}

/*
 * uC features - pack
 */
uint8*
shr_ft_msg_ctrl_uc_features_pack(uint8* buf,
                                 shr_ft_msg_ctrl_uc_features_t *msg)
{
    _SHR_PACK_U32(buf, msg->features);

    return buf;
}

/*
 * uC features - unpack
 */
uint8*
shr_ft_msg_ctrl_uc_features_unpack(uint8* buf,
                                   shr_ft_msg_ctrl_uc_features_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->features);

    return buf;
}

/*
 * Group actions set - pack
 */
uint8*
shr_ft_msg_ctrl_group_actions_set_pack(uint8* buf,
                                       shr_ft_msg_ctrl_group_actions_set_t *msg)
{
    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->actions);
    _SHR_PACK_U16(buf, msg->fsp_cosq);

    return buf;
}

/*
 * Group actions set - unpack
 */
uint8*
shr_ft_msg_ctrl_group_actions_set_unpack(uint8* buf,
                                         shr_ft_msg_ctrl_group_actions_set_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->actions);
    _SHR_UNPACK_U16(buf, msg->fsp_cosq);

    return buf;
}

/*
 * QCM Init config
 */
uint8*
shr_ft_qcm_msg_ctrl_qcm_init_cfg_pack(uint8* buf,
                                      shr_ft_qcm_msg_ctrl_qcm_init_cfg_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->max_port_num);
    _SHR_PACK_U16(buf, msg->queues_per_pipe);
    _SHR_PACK_U16(buf, msg->ports_per_pipe);
    _SHR_PACK_U8(buf, msg->queues_per_port);
    _SHR_PACK_U8(buf, msg->num_pipes);
    _SHR_PACK_U8(buf, msg->num_counters_per_pipe);

    for (i = 0; i < (msg->num_counters_per_pipe * msg->num_pipes); i++) {
        _SHR_PACK_U32(buf, msg->mmu_drop_base[i]);
    }

    for (i = 0; i < (msg->num_counters_per_pipe * msg->num_pipes); i++) {
        _SHR_PACK_U8(buf, msg->mmu_acc_type[i]);
    }

    for (i = 0; i < (msg->num_counters_per_pipe * msg->num_pipes); i++) {
        _SHR_PACK_U32(buf, msg->mmu_util_base[i]);
    }

    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_PACK_U32(buf, msg->mmu_egr_base[i]);
    }

    return buf;
}

/*
 * QCM Init config
 */
uint8*
shr_ft_qcm_msg_ctrl_qcm_init_cfg_unpack(uint8* buf,
                                        shr_ft_qcm_msg_ctrl_qcm_init_cfg_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->max_port_num);
    _SHR_UNPACK_U16(buf, msg->queues_per_pipe);
    _SHR_UNPACK_U16(buf, msg->ports_per_pipe);
    _SHR_UNPACK_U8(buf, msg->queues_per_port);
    _SHR_UNPACK_U8(buf, msg->num_pipes);
    _SHR_UNPACK_U8(buf, msg->num_counters_per_pipe);

    for (i = 0; i < (msg->num_counters_per_pipe * msg->num_pipes); i++) {
        _SHR_UNPACK_U32(buf, msg->mmu_drop_base[i]);
    }

    for (i = 0; i < (msg->num_counters_per_pipe * msg->num_pipes); i++) {
        _SHR_UNPACK_U8(buf, msg->mmu_acc_type[i]);
    }

    for (i = 0; i < (msg->num_counters_per_pipe * msg->num_pipes); i++) {
        _SHR_UNPACK_U32(buf, msg->mmu_util_base[i]);
    }

    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_UNPACK_U32(buf, msg->mmu_egr_base[i]);
    }

    return buf;
}

/*
 * QCM host memory buffer information
 */
uint8*
shr_ft_qcm_msg_ctrl_host_buf_pack(uint8* buf,
                                  shr_ft_qcm_msg_ctrl_host_buf_t *msg)
{
    _SHR_PACK_U32(buf, msg->host_buf_addr);
    _SHR_PACK_U32(buf, msg->host_buf_size);
    _SHR_PACK_U32(buf, msg->flow_view_size);
    _SHR_PACK_U32(buf, msg->num_samples);
    _SHR_PACK_U8(buf, msg->num_flow_views);

    return buf;
}

/*
 * QCM host memory buffer information
 */
uint8*
shr_ft_qcm_msg_ctrl_host_buf_unpack(uint8* buf,
                                    shr_ft_qcm_msg_ctrl_host_buf_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->host_buf_addr);
    _SHR_UNPACK_U32(buf, msg->host_buf_size);
    _SHR_UNPACK_U32(buf, msg->flow_view_size);
    _SHR_UNPACK_U32(buf, msg->num_samples);
    _SHR_UNPACK_U8(buf, msg->num_flow_views);

    return buf;
}

/*
 * Queue congestion monitor port queue Configuration
 */
uint8*
shr_ft_qcm_msg_ctrl_config_pack(uint8* buf,
                                shr_ft_qcm_msg_ctrl_config_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->num_gports);
    for (i = 0; i < msg->num_gports; i++) {
        _SHR_PACK_U32(buf, msg->gport[i]);
    }

    for (i = 0; i < msg->num_gports; i++) {
        _SHR_PACK_U32(buf, msg->port_pipe_queue_num[i]);
    }

    for (i = 0; i < msg->num_gports; i++) {
        _SHR_PACK_U16(buf, msg->pport[i]);
    }

    return buf;
}

/*
 * Queue congestion monitor port queue Configuration
 */
uint8*
shr_ft_qcm_msg_ctrl_config_unpack(uint8* buf,
                                shr_ft_qcm_msg_ctrl_config_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->num_gports);
    for (i = 0; i < msg->num_gports; i++) {
        _SHR_UNPACK_U32(buf, msg->gport[i]);
    }

    for (i = 0; i < msg->num_gports; i++) {
        _SHR_UNPACK_U32(buf, msg->port_pipe_queue_num[i]);
    }

    for (i = 0; i < msg->num_gports; i++) {
        _SHR_UNPACK_U16(buf, msg->pport[i]);
    }

    return buf;
}

/*
 * Queue congestion monitor flow view Configuration
 */
uint8*
shr_ft_qcm_msg_ctrl_flow_view_cfg_pack(uint8* buf,
                                       shr_ft_qcm_msg_ctrl_flow_view_t *msg)
{
    int    i;

    _SHR_PACK_U32(buf, msg->export_threshold_hi);
    _SHR_PACK_U32(buf, msg->export_threshold_lo);
    _SHR_PACK_U32(buf, msg->age_threshold);
    _SHR_PACK_U32(buf, msg->scan_interval);
    _SHR_PACK_U32(buf, msg->start_timer_hi);
    _SHR_PACK_U32(buf, msg->start_timer_lo);
    _SHR_PACK_U32(buf, msg->age_timer_hi);
    _SHR_PACK_U32(buf, msg->age_timer_lo);
    _SHR_PACK_U32(buf, msg->num_samples);
    _SHR_PACK_U32(buf, msg->num_flows);
    _SHR_PACK_U32(buf, msg->num_flows_clear);
    _SHR_PACK_U32(buf, msg->flags);
    for (i = 0; i < SHR_FT_QCM_INTEREST_FACTOR_WEIGHT_MAX_COUNT; i++) {
        _SHR_PACK_U8(buf, msg->weights[i]);
    }

    return buf;
}

/*
 * Queue congestion monitor flow view Configuration
 */
uint8*
shr_ft_qcm_msg_ctrl_flow_view_cfg_unpack(uint8* buf,
                                         shr_ft_qcm_msg_ctrl_flow_view_t *msg)
{
    int    i;

    _SHR_UNPACK_U32(buf, msg->export_threshold_hi);
    _SHR_UNPACK_U32(buf, msg->export_threshold_lo);
    _SHR_UNPACK_U32(buf, msg->age_threshold);
    _SHR_UNPACK_U32(buf, msg->scan_interval);
    _SHR_UNPACK_U32(buf, msg->start_timer_hi);
    _SHR_UNPACK_U32(buf, msg->start_timer_lo);
    _SHR_UNPACK_U32(buf, msg->age_timer_hi);
    _SHR_UNPACK_U32(buf, msg->age_timer_lo);
    _SHR_UNPACK_U32(buf, msg->num_samples);
    _SHR_UNPACK_U32(buf, msg->num_flows);
    _SHR_UNPACK_U32(buf, msg->num_flows_clear);
    _SHR_UNPACK_U32(buf, msg->flags);
    for (i = 0; i < SHR_FT_QCM_INTEREST_FACTOR_WEIGHT_MAX_COUNT; i++) {
        _SHR_UNPACK_U8(buf, msg->weights[i]);
    }

    return buf;
}

/*
 * Queue congestion monitor current FW time set function
 */
uint8*
shr_ft_qcm_msg_cur_time_pack(uint8* buf,
                             shr_ft_qcm_msg_cur_time_t *msg)
{
    _SHR_PACK_U32(buf, msg->time_usecs_hi);
    _SHR_PACK_U32(buf, msg->time_usecs_lo);
    return buf;
}

/*
 * Queue congestion monitor current FW time get functionality
 */
uint8*
shr_ft_qcm_msg_cur_time_unpack(uint8* buf,
                               shr_ft_qcm_msg_cur_time_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->time_usecs_hi);
    _SHR_UNPACK_U32(buf, msg->time_usecs_lo);
    return buf;
}

/*
 * QCM EM key format export unpack
 */
uint8*
shr_ft_qcm_msg_ctrl_em_key_format_unpack(uint8 *buf,
                                         shr_ft_qcm_msg_ctrl_em_key_format_t *msg)
{
    int i, j;

    _SHR_UNPACK_U8(buf, msg->group_idx);
    _SHR_UNPACK_U16(buf, msg->key_size);
    _SHR_UNPACK_U8(buf, msg->num_pipes);
    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_UNPACK_U8(buf, msg->lt_id[i]);
    }
    _SHR_UNPACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        _SHR_UNPACK_U8(buf, msg->qual_info[i].qual);
        _SHR_UNPACK_U8(buf, msg->qual_info[i].num_parts);

        for (j = 0; j < msg->qual_info[i].num_parts; j++) {
            _SHR_UNPACK_U16(buf, msg->qual_info[i].offset[j]);
            _SHR_UNPACK_U8(buf, msg->qual_info[i].width[j]);
        }
    }

    return buf;
}

/*
 * QCM EM key format export pack
 */
uint8*
shr_ft_qcm_msg_ctrl_em_key_format_pack(uint8 *buf,
                                       shr_ft_qcm_msg_ctrl_em_key_format_t *msg)
{
    int i, j;

    _SHR_PACK_U8(buf, msg->group_idx);
    _SHR_PACK_U16(buf, msg->key_size);
    _SHR_PACK_U8(buf, msg->num_pipes);
    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_PACK_U8(buf, msg->lt_id[i]);
    }
    _SHR_PACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        _SHR_PACK_U8(buf, msg->qual_info[i].qual);
        _SHR_PACK_U8(buf, msg->qual_info[i].num_parts);

        for (j = 0; j < msg->qual_info[i].num_parts; j++) {
            _SHR_PACK_U16(buf, msg->qual_info[i].offset[j]);
            _SHR_PACK_U8(buf, msg->qual_info[i].width[j]);
        }
    }

    return buf;
}

uint8*
shr_ft_qcm_msg_ctrl_group_flow_data_get_pack(uint8* buf,
                                         shr_ft_qcm_msg_ctrl_group_flow_data_get_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->src_ip);
    _SHR_PACK_U32(buf, msg->dst_ip);
    for(i=0; i < SHR_QCM_IPV6_ADDR_LEN; i++) {
        _SHR_PACK_U8(buf, msg->src_ipv6[i]);
    }
    for(i=0; i < SHR_QCM_IPV6_ADDR_LEN; i++) {
        _SHR_PACK_U8(buf, msg->dst_ipv6[i]);
    }
    _SHR_PACK_U16(buf, msg->l4_src_port);
    _SHR_PACK_U16(buf, msg->l4_dst_port);
    _SHR_PACK_U8(buf, msg->ip_protocol);
    _SHR_PACK_U16(buf, msg->in_port);

    return buf;
}

uint8*
shr_ft_qcm_msg_ctrl_group_flow_data_get_unpack(uint8* buf,
                                         shr_ft_qcm_msg_ctrl_group_flow_data_get_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->src_ip);
    _SHR_UNPACK_U32(buf, msg->dst_ip);
    for(i=0; i < SHR_QCM_IPV6_ADDR_LEN; i++) {
        _SHR_UNPACK_U8(buf, msg->src_ipv6[i]);
    }
    for(i=0; i < SHR_QCM_IPV6_ADDR_LEN; i++) {
        _SHR_UNPACK_U8(buf, msg->dst_ipv6[i]);
    }
    _SHR_UNPACK_U16(buf, msg->l4_src_port);
    _SHR_UNPACK_U16(buf, msg->l4_dst_port);
    _SHR_UNPACK_U8(buf, msg->ip_protocol);
    _SHR_UNPACK_U16(buf, msg->in_port);

    return buf;
}

/*
 * Get QCM learn stats - pack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_learn_get_pack(uint8* buf,
                                    shr_ft_qcm_msg_ctrl_stats_learn_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->num_status);

    for (i = 0; i < msg->num_status; i++) {
        _SHR_PACK_U32(buf, msg->count[i]);
    }

    for (i = 0; i < SHR_FT_MAX_PIPES;i++) {
       _SHR_PACK_U16(buf, msg->pipe_flowcount[i]);
    }

    for (i = 0; i < SHR_QCM_MAX_FLOW_GROUPS; i++) {
        _SHR_PACK_U16(buf, msg->group_flowcount[i]);
    }

    return buf;
}

/*
 * Get learn stats - unpack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_learn_get_unpack(uint8* buf,
                                       shr_ft_qcm_msg_ctrl_stats_learn_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->num_status);

    for (i = 0; i < msg->num_status; i++) {
        _SHR_UNPACK_U32(buf, msg->count[i]);
    }

    for (i = 0; i < SHR_FT_MAX_PIPES;i++) {
        _SHR_UNPACK_U16(buf, msg->pipe_flowcount[i]);
    }

    for (i = 0; i < SHR_QCM_MAX_FLOW_GROUPS; i++) {
        _SHR_UNPACK_U16(buf, msg->group_flowcount[i]);
    }

    return buf;
}

/*
 * Get QCM export stats - pack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_export_get_pack(uint8* buf,
                                      shr_ft_qcm_msg_ctrl_stats_export_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_pkts);
    _SHR_PACK_U8(buf, msg->num_flow_views);
    _SHR_PACK_U32(buf, msg->seq_num);
    _SHR_PACK_U32(buf, msg->min_interest_factor_lower);
    _SHR_PACK_U32(buf, msg->min_interest_factor_upper);
    _SHR_PACK_U32(buf, msg->max_interest_factor_lower);
    _SHR_PACK_U32(buf, msg->max_interest_factor_upper);

    _SHR_PACK_U16(buf, msg->view_id);
    _SHR_PACK_U32(buf, msg->pkts_per_view_lower);
    _SHR_PACK_U32(buf, msg->pkts_per_view_upper);
    _SHR_PACK_U16(buf, msg->flows_per_view);
    _SHR_PACK_U32(buf, msg->interest_factor_lower);
    _SHR_PACK_U32(buf, msg->interest_factor_upper);

    return buf;
}

/*
 * Get QCM export stats - unpack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_export_get_unpack(uint8* buf,
                                        shr_ft_qcm_msg_ctrl_stats_export_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->num_pkts);
    _SHR_UNPACK_U8(buf, msg->num_flow_views);
    _SHR_UNPACK_U32(buf, msg->seq_num);
    _SHR_UNPACK_U32(buf, msg->min_interest_factor_lower);
    _SHR_UNPACK_U32(buf, msg->min_interest_factor_upper);
    _SHR_UNPACK_U32(buf, msg->max_interest_factor_lower);
    _SHR_UNPACK_U32(buf, msg->max_interest_factor_upper);

    _SHR_UNPACK_U16(buf, msg->view_id);
    _SHR_UNPACK_U32(buf, msg->pkts_per_view_lower);
    _SHR_UNPACK_U32(buf, msg->pkts_per_view_upper);
    _SHR_UNPACK_U16(buf, msg->flows_per_view);
    _SHR_UNPACK_U32(buf, msg->interest_factor_lower);
    _SHR_UNPACK_U32(buf, msg->interest_factor_upper);

   return buf;
}

/*
 * Get QCM age stats - pack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_age_get_pack(uint8* buf,
                                   shr_ft_qcm_msg_ctrl_stats_age_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_flows);
    _SHR_PACK_U32(buf, msg->num_flows_forced);

    return buf;
}

/*
 * Get age stats - unpack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_age_get_unpack(uint8* buf,
                                   shr_ft_qcm_msg_ctrl_stats_age_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->num_flows);
    _SHR_UNPACK_U32(buf, msg->num_flows_forced);

    return buf;
}

/*
 * Get QCM scan stats - pack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_scan_get_pack(uint8* buf,
                                   shr_ft_qcm_msg_ctrl_stats_scan_t *msg)
{
    _SHR_PACK_U32(buf, msg->min_sample_interval);
    _SHR_PACK_U32(buf, msg->max_sample_interval);
    _SHR_PACK_U32(buf, msg->total_scan_interval_view);
    _SHR_PACK_U16(buf, msg->num_flows);

    return buf;
}

/*
 * Get scan stats - unpack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_scan_get_unpack(uint8* buf,
                                   shr_ft_qcm_msg_ctrl_stats_scan_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->min_sample_interval);
    _SHR_UNPACK_U32(buf, msg->max_sample_interval);
    _SHR_UNPACK_U32(buf, msg->total_scan_interval_view);
    _SHR_UNPACK_U16(buf, msg->num_flows);

    return buf;
}

/*
 * Enable/disable stats update - pack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_enable_pack(uint8* buf,
                                    shr_ft_qcm_msg_ctrl_stats_enable_t *msg)
{
    _SHR_PACK_U8(buf, msg->enable);
    return buf;
}

/*
 * Enable/disable stats update - unpack
 */
uint8*
shr_ft_qcm_msg_ctrl_stats_enable_unpack(uint8* buf,
                                       shr_ft_qcm_msg_ctrl_stats_enable_t *msg)
{
    _SHR_UNPACK_U8(buf, msg->enable);
    return buf;
}

