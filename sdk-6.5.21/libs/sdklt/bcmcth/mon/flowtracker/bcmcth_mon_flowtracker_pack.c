/*! \file bcmcth_mon_flowtracker_pack.c
 *
 * Flowtracker pack and unpack routines for control messages.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_util_pack.h>
#include <bcmcth/bcmcth_mon_flowtracker_pack.h>

/******************************************************************************
 * Public functions
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_init_pack(uint32_t uc_msg_version,
                                 uint8_t *buf,
                                 mcs_ft_msg_ctrl_init_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->max_groups);

    SHR_UTIL_PACK_U8(buf, msg->num_pipes);

    for (i = 0; i < msg->num_pipes; i++) {
        SHR_UTIL_PACK_U32(buf, msg->pipe[i].max_flows);

        SHR_UTIL_PACK_U8(buf, msg->pipe[i].num_ctr_pools);
        for (j = 0; j < msg->pipe[i].num_ctr_pools; j++) {
            SHR_UTIL_PACK_U8(buf, msg->pipe[i].ctr_pools[j]);
            SHR_UTIL_PACK_U16(buf, msg->pipe[i].ctr_pool_size[j]);
        }
    }

    SHR_UTIL_PACK_U16(buf, msg->max_export_pkt_length);
    SHR_UTIL_PACK_U32(buf, msg->cur_time_secs);
    SHR_UTIL_PACK_U32(buf, msg->export_interval_msecs);
    SHR_UTIL_PACK_U32(buf, msg->scan_interval_usecs);
    SHR_UTIL_PACK_U32(buf, msg->age_timer_tick_msecs);
    SHR_UTIL_PACK_U8(buf, msg->num_elph_profiles);
    SHR_UTIL_PACK_U16(buf, msg->fsp_reinject_max_length);
    SHR_UTIL_PACK_U8(buf, msg->flex_ctr_action_idx);
    if (uc_msg_version >= FT_MSG_VERSION_4) {
        SHR_UTIL_PACK_U8(buf, msg->hw_learn);
    }
    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_init_unpack(uint8_t *buf, mcs_ft_msg_ctrl_init_t *msg)
{
    int i, j;

    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U32(buf, msg->max_groups);

    SHR_UTIL_UNPACK_U8(buf, msg->num_pipes);

    for (i = 0; i < msg->num_pipes; i++) {
        SHR_UTIL_UNPACK_U32(buf, msg->pipe[i].max_flows);

        SHR_UTIL_UNPACK_U8(buf, msg->pipe[i].num_ctr_pools);
        for (j = 0; j < msg->pipe[i].num_ctr_pools; j++) {
            SHR_UTIL_UNPACK_U8(buf, msg->pipe[i].ctr_pools[j]);
            SHR_UTIL_UNPACK_U16(buf, msg->pipe[i].ctr_pool_size[j]);
        }
    }

    SHR_UTIL_UNPACK_U16(buf, msg->max_export_pkt_length);
    SHR_UTIL_UNPACK_U32(buf, msg->cur_time_secs);
    SHR_UTIL_UNPACK_U32(buf, msg->export_interval_msecs);
    SHR_UTIL_UNPACK_U32(buf, msg->scan_interval_usecs);
    SHR_UTIL_UNPACK_U32(buf, msg->age_timer_tick_msecs);
    SHR_UTIL_UNPACK_U8(buf, msg->num_elph_profiles);
    SHR_UTIL_UNPACK_U16(buf, msg->fsp_reinject_max_length);
    SHR_UTIL_UNPACK_U8(buf, msg->flex_ctr_action_idx);
    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_em_key_format_pack(uint8_t *buf,
                                          mcs_ft_msg_ctrl_em_key_format_t *msg)
{
    int i, j;


    SHR_UTIL_PACK_U16(buf, msg->key_size);
    SHR_UTIL_PACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        SHR_UTIL_PACK_U8(buf, msg->qual[i]);
        SHR_UTIL_PACK_U8(buf, msg->qual_parts[i].num_parts);

        for (j = 0; j < msg->qual_parts[i].num_parts; j++) {
            SHR_UTIL_PACK_U16(buf, msg->qual_parts[i].offset[j]);
            SHR_UTIL_PACK_U8(buf, msg->qual_parts[i].width[j]);
        }
    }

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_em_group_create_pack
(   uint8_t *buf,
    mcs_ft_msg_ctrl_em_group_create_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U8(buf, msg->group_idx);
    SHR_UTIL_PACK_U8(buf, msg->num_pipes);
    for (i = 0; i < msg->num_pipes; i++) {
        SHR_UTIL_PACK_U8(buf, msg->lt_id[i]);
    }
    SHR_UTIL_PACK_U32(buf, msg->aset);
    SHR_UTIL_PACK_U16(buf, msg->key_size);
    SHR_UTIL_PACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        SHR_UTIL_PACK_U8(buf, msg->qual_info[i].qual);
        SHR_UTIL_PACK_U8(buf, msg->qual_info[i].num_parts);

        for (j = 0; j < msg->qual_info[i].num_parts; j++) {
            SHR_UTIL_PACK_U16(buf, msg->qual_info[i].part_offset[j]);
            SHR_UTIL_PACK_U8(buf, msg->qual_info[i].part_width[j]);
        }

        SHR_UTIL_PACK_U16(buf, msg->qual_info[i].width);
        SHR_UTIL_PACK_U8(buf, msg->qual_info[i].base_offset);
        SHR_UTIL_PACK_U8(buf, msg->qual_info[i].relative_offset);
    }

    SHR_UTIL_PACK_U8(buf, msg->num_tp);
    for (i = 0; i < msg->num_tp; i++) {
        SHR_UTIL_PACK_U8(buf, msg->tp_info[i].param);
        SHR_UTIL_PACK_U8(buf, msg->tp_info[i].num_qual);
        SHR_UTIL_PACK_U8(buf, msg->tp_info[i].udf);
        for (j = 0; j < msg->tp_info[i].num_qual; j++) {
            SHR_UTIL_PACK_U8(buf, msg->tp_info[i].qual[j]);
        }
    }

    return buf;
}


/*
 * EM group create - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_flex_em_group_create_pack(uint32_t uc_msg_version,
                                                 uint8_t *buf,
                                                 mcs_ft_msg_ctrl_flex_em_group_create_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U8(buf, msg->group_idx);
    SHR_UTIL_PACK_U8(buf, msg->key_type);
    SHR_UTIL_PACK_U8(buf, msg->num_key_parts);
    SHR_UTIL_PACK_U8(buf, msg->num_pipes);
    for (i = 0; i < msg->num_pipes; i++) {
        SHR_UTIL_PACK_U8(buf, msg->lt_id[i]);
    }
    SHR_UTIL_PACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        SHR_UTIL_PACK_U8(buf, msg->qual_info[i].qual);
        SHR_UTIL_PACK_U8(buf, msg->qual_info[i].num_parts);

        for (j = 0; j < msg->qual_info[i].num_parts; j++) {
            SHR_UTIL_PACK_U16(buf, msg->qual_info[i].part_offset[j]);
            SHR_UTIL_PACK_U8(buf, msg->qual_info[i].part_width[j]);
        }

        SHR_UTIL_PACK_U16(buf, msg->qual_info[i].width);
        SHR_UTIL_PACK_U8(buf, msg->qual_info[i].base_offset);
        SHR_UTIL_PACK_U8(buf, msg->qual_info[i].relative_offset);
    }

    SHR_UTIL_PACK_U8(buf, msg->num_tp);
    for (i = 0; i < msg->num_tp; i++) {
        SHR_UTIL_PACK_U8(buf, msg->tp_info[i].param);
        SHR_UTIL_PACK_U8(buf, msg->tp_info[i].num_qual);
        SHR_UTIL_PACK_U8(buf, msg->tp_info[i].udf);
        for (j = 0; j < msg->tp_info[i].num_qual; j++) {
            SHR_UTIL_PACK_U8(buf, msg->tp_info[i].qual[j]);
        }
    }

    for (i = 0; i < msg->num_key_parts; i++) {
        SHR_UTIL_PACK_U16(buf, msg->key_start_offset[i]);
    }
    for (i = 0; i < msg->num_key_parts; i++) {
        SHR_UTIL_PACK_U16(buf, msg->key_end_offset[i]);
    }
    SHR_UTIL_PACK_U8(buf, msg->num_actions);
    for (i = 0; i < msg->num_actions; i++) {
        SHR_UTIL_PACK_U8(buf, msg->aset[i]);
    }

    SHR_UTIL_PACK_U8(buf, msg->bucket_mode);
    SHR_UTIL_PACK_U16(buf, msg->action_start_offset);
    SHR_UTIL_PACK_U16(buf, msg->action_end_offset);

    for (i = 0; i < msg->num_actions; i++) {
        SHR_UTIL_PACK_U16(buf, msg->aset_offsets[i]);
    }
    for (i = 0; i < msg->num_actions; i++) {
        SHR_UTIL_PACK_U16(buf, msg->aset_widths[i]);
    }
    SHR_UTIL_PACK_U16(buf, msg->key_size);

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        SHR_UTIL_PACK_U8(buf, msg->num_base_entries);
    }

    return buf;
}
/*
 * EM group create - unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_em_group_create_unpack(uint8_t *buf,
                                              mcs_ft_msg_ctrl_em_group_create_t *msg)
{
    int i, j;

    SHR_UTIL_UNPACK_U8(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U8(buf, msg->num_pipes);
    for (i = 0; i < msg->num_pipes; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->lt_id[i]);
    }
    SHR_UTIL_UNPACK_U32(buf, msg->aset);
    SHR_UTIL_UNPACK_U16(buf, msg->key_size);
    SHR_UTIL_UNPACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->qual_info[i].qual);
        SHR_UTIL_UNPACK_U8(buf, msg->qual_info[i].num_parts);

        for (j = 0; j < msg->qual_info[i].num_parts; j++) {
            SHR_UTIL_UNPACK_U16(buf, msg->qual_info[i].part_offset[j]);
            SHR_UTIL_UNPACK_U8(buf, msg->qual_info[i].part_width[j]);
        }

        SHR_UTIL_UNPACK_U16(buf, msg->qual_info[i].width);
        SHR_UTIL_UNPACK_U8(buf, msg->qual_info[i].base_offset);
        SHR_UTIL_UNPACK_U8(buf, msg->qual_info[i].relative_offset);
    }

    SHR_UTIL_UNPACK_U8(buf, msg->num_tp);
    for (i = 0; i < msg->num_tp; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->tp_info[i].param);
        SHR_UTIL_UNPACK_U8(buf, msg->tp_info[i].num_qual);
        SHR_UTIL_UNPACK_U8(buf, msg->tp_info[i].udf);
        for (j = 0; j < msg->tp_info[i].num_qual; j++) {
            SHR_UTIL_UNPACK_U8(buf, msg->tp_info[i].qual[j]);
        }
    }

    return buf;
}

/*
 * EM key format export unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_em_key_format_unpack(uint8_t *buf,
                                            mcs_ft_msg_ctrl_em_key_format_t *msg)
{
    int i, j;


    SHR_UTIL_UNPACK_U16(buf, msg->key_size);
    SHR_UTIL_UNPACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->qual[i]);
        SHR_UTIL_UNPACK_U8(buf, msg->qual_parts[i].num_parts);

        for (j = 0; j < msg->qual_parts[i].num_parts; j++) {
            SHR_UTIL_UNPACK_U16(buf, msg->qual_parts[i].offset[j]);
            SHR_UTIL_UNPACK_U8(buf, msg->qual_parts[i].width[j]);
        }
    }

    return buf;
}

/*
 * Group create pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_create_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_group_create_t *msg)
{
    SHR_UTIL_PACK_U16(buf, msg->group_idx);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->flow_limit);
    SHR_UTIL_PACK_U32(buf, msg->aging_interval_msecs);
    SHR_UTIL_PACK_U32(buf, msg->domain_id);

    return buf;
}

/*
 * Group create unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_create_unpack(uint8_t *buf,
                                           mcs_ft_msg_ctrl_group_create_t *msg)
{
    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_limit);
    SHR_UTIL_UNPACK_U32(buf, msg->aging_interval_msecs);
    SHR_UTIL_UNPACK_U32(buf, msg->domain_id);

    return buf;
}

/*
 * Group get pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_get_pack(uint8_t *buf,
                                      mcs_ft_msg_ctrl_group_get_t *msg)
{
    SHR_UTIL_PACK_U16(buf, msg->group_idx);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->flow_limit);
    SHR_UTIL_PACK_U32(buf, msg->flow_count);
    SHR_UTIL_PACK_U32(buf, msg->domain_id);
    SHR_UTIL_PACK_U32(buf, msg->aging_interval);
    SHR_UTIL_PACK_U16(buf, msg->elph_profile_id);
    SHR_UTIL_PACK_U16(buf, msg->qos_profile_id);
    SHR_UTIL_PACK_U16(buf, msg->collector_id);
    SHR_UTIL_PACK_U16(buf, msg->template_id);

    return buf;
}

/*
 * Group get unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_get_unpack(uint8_t *buf,
                                        mcs_ft_msg_ctrl_group_get_t *msg)
{
    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_limit);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_count);
    SHR_UTIL_UNPACK_U32(buf, msg->domain_id);
    SHR_UTIL_UNPACK_U32(buf, msg->aging_interval);
    SHR_UTIL_UNPACK_U16(buf, msg->elph_profile_id);
    SHR_UTIL_UNPACK_U16(buf, msg->qos_profile_id);
    SHR_UTIL_UNPACK_U16(buf, msg->collector_id);
    SHR_UTIL_UNPACK_U16(buf, msg->template_id);

    return buf;
}


/*
 * Group update pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_update_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_group_update_t *msg)
{
    SHR_UTIL_PACK_U16(buf, msg->group_idx);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->update);
    SHR_UTIL_PACK_U32(buf, msg->param0);

    return buf;
}

/*
 * Group update unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_update_unpack(uint8_t *buf,
                                           mcs_ft_msg_ctrl_group_update_t *msg)
{
    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U32(buf, msg->update);
    SHR_UTIL_UNPACK_U32(buf, msg->param0);

    return buf;
}

/*
 * Create IPFIX template - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_template_create_pack(uint8_t *buf,
                                            mcs_ft_msg_ctrl_template_create_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->id);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U16(buf, msg->set_id);

    SHR_UTIL_PACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        SHR_UTIL_PACK_U8(buf, msg->info_elems[i]);
        SHR_UTIL_PACK_U16(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Create IPFIX template - unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_template_create_unpack(uint8_t *buf,
                                              mcs_ft_msg_ctrl_template_create_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->id);
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U16(buf, msg->set_id);

    SHR_UTIL_UNPACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->info_elems[i]);
        SHR_UTIL_UNPACK_U16(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Get IPFIX template - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_template_get_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_template_get_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->id);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U16(buf, msg->set_id);

    SHR_UTIL_PACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        SHR_UTIL_PACK_U8(buf, msg->info_elems[i]);
        SHR_UTIL_PACK_U16(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Get IPFIX template - unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_template_get_unpack(uint8_t *buf,
                                           mcs_ft_msg_ctrl_template_get_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->id);
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U16(buf, msg->set_id);

    SHR_UTIL_UNPACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->info_elems[i]);
        SHR_UTIL_UNPACK_U16(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Create collector - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_collector_create_pack(uint8_t *buf,
                                             mcs_ft_msg_ctrl_collector_create_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->id);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->encap[i]);
    }

    SHR_UTIL_PACK_U16(buf, msg->mtu);
    SHR_UTIL_PACK_U32(buf, msg->ip_base_checksum);
    SHR_UTIL_PACK_U32(buf, msg->udp_base_checksum);
    SHR_UTIL_PACK_U16(buf, msg->ip_offset);
    SHR_UTIL_PACK_U16(buf, msg->udp_offset);
    SHR_UTIL_PACK_U32(buf, msg->component_id);
    SHR_UTIL_PACK_U16(buf, msg->system_id_length);
    for (i = 0; i < msg->system_id_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->system_id[i]);
    }
    return buf;
}

/*
 * Create collector - unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_collector_create_unpack(uint8_t *buf,
                                               mcs_ft_msg_ctrl_collector_create_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->id);
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->encap[i]);
    }

    SHR_UTIL_UNPACK_U16(buf, msg->mtu);
    SHR_UTIL_UNPACK_U32(buf, msg->ip_base_checksum);
    SHR_UTIL_UNPACK_U32(buf, msg->udp_base_checksum);
    SHR_UTIL_UNPACK_U16(buf, msg->ip_offset);
    SHR_UTIL_UNPACK_U16(buf, msg->udp_offset);

    SHR_UTIL_UNPACK_U32(buf, msg->component_id);
    SHR_UTIL_UNPACK_U16(buf, msg->system_id_length);
    for (i = 0; i < msg->system_id_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->system_id[i]);
    }
    return buf;
}

/*
 * Get collector - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_collector_get_pack(uint8_t *buf,
                                          mcs_ft_msg_ctrl_collector_get_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->id);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->encap[i]);
    }

    SHR_UTIL_PACK_U16(buf, msg->mtu);
    SHR_UTIL_PACK_U16(buf, msg->ip_base_checksum);
    SHR_UTIL_PACK_U16(buf, msg->udp_base_checksum);
    SHR_UTIL_PACK_U16(buf, msg->ip_offset);
    SHR_UTIL_PACK_U16(buf, msg->udp_offset);

    SHR_UTIL_PACK_U32(buf, msg->component_id);
    SHR_UTIL_PACK_U16(buf, msg->system_id_length);
    for (i = 0; i < msg->system_id_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->system_id[i]);
    }

    return buf;
}

/*
 * Get collector - unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_collector_get_unpack(uint8_t *buf,
                                            mcs_ft_msg_ctrl_collector_get_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->id);
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->encap[i]);
    }

    SHR_UTIL_UNPACK_U16(buf, msg->mtu);
    SHR_UTIL_UNPACK_U16(buf, msg->ip_base_checksum);
    SHR_UTIL_UNPACK_U16(buf, msg->udp_base_checksum);
    SHR_UTIL_UNPACK_U16(buf, msg->ip_offset);
    SHR_UTIL_UNPACK_U16(buf, msg->udp_offset);

    SHR_UTIL_UNPACK_U32(buf, msg->component_id);
    SHR_UTIL_UNPACK_U16(buf, msg->system_id_length);
    for (i = 0; i < msg->system_id_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->system_id[i]);
    }

    return buf;
}

/*
 * Static flow set - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_static_flow_set_pack(uint32_t uc_msg_version,
                                            uint8_t *buf,
                                            mcs_ft_msg_ctrl_static_flow_set_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->group_idx);
    SHR_UTIL_PACK_U32(buf, msg->src_ipv4);
    SHR_UTIL_PACK_U32(buf, msg->dst_ipv4);
    SHR_UTIL_PACK_U16(buf, msg->src_l4_port);
    SHR_UTIL_PACK_U16(buf, msg->dst_l4_port);
    SHR_UTIL_PACK_U8(buf, msg->ip_proto);

    SHR_UTIL_PACK_U32(buf, msg->vnid);
    SHR_UTIL_PACK_U32(buf, msg->inner_src_ipv4);
    SHR_UTIL_PACK_U32(buf, msg->inner_dst_ipv4);
    SHR_UTIL_PACK_U16(buf, msg->inner_src_l4_port);
    SHR_UTIL_PACK_U16(buf, msg->inner_dst_l4_port);
    SHR_UTIL_PACK_U8(buf, msg->inner_ip_proto);
    SHR_UTIL_PACK_U8(buf, msg->custom_length);
    for (i = 0; i < msg->custom_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->custom[i]);
    }
    SHR_UTIL_PACK_U16(buf, msg->in_port);

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        for (i = 0; i < 16; i++) {
            SHR_UTIL_PACK_U8(buf, msg->inner_src_ipv6[i]);
        }
        for (i = 0; i < 16; i++) {
            SHR_UTIL_PACK_U8(buf, msg->inner_dst_ipv6[i]);
        }
    }

    return buf;
}

/*
 * Get data associated with a flow - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_flow_data_get_pack(uint32_t uc_msg_version,
                                                uint8_t *buf,
                                                mcs_ft_msg_ctrl_group_flow_data_get_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->group_idx);
    SHR_UTIL_PACK_U32(buf, msg->src_ip);
    SHR_UTIL_PACK_U32(buf, msg->dst_ip);
    SHR_UTIL_PACK_U16(buf, msg->l4_src_port);
    SHR_UTIL_PACK_U16(buf, msg->l4_dst_port);
    SHR_UTIL_PACK_U8(buf, msg->ip_protocol);

    SHR_UTIL_PACK_U32(buf, msg->vnid);
    SHR_UTIL_PACK_U32(buf, msg->inner_src_ip);
    SHR_UTIL_PACK_U32(buf, msg->inner_dst_ip);
    SHR_UTIL_PACK_U16(buf, msg->inner_l4_src_port);
    SHR_UTIL_PACK_U16(buf, msg->inner_l4_dst_port);
    SHR_UTIL_PACK_U8(buf, msg->inner_ip_protocol);
    SHR_UTIL_PACK_U8(buf, msg->custom_length);
    for (i = 0; i < msg->custom_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->custom[i]);
    }

    SHR_UTIL_PACK_U16(buf, msg->in_port);

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        for (i = 0; i < 16; i++) {
            SHR_UTIL_PACK_U8(buf, msg->inner_src_ipv6[i]);
        }
        for (i = 0; i < 16; i++) {
            SHR_UTIL_PACK_U8(buf, msg->inner_dst_ipv6[i]);
        }
    }

    return buf;
}

/*
 * Get data associated with a flow - unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_flow_data_get_unpack(uint32_t uc_msg_version,
                                                  uint8_t *buf,
                                                  mcs_ft_msg_ctrl_group_flow_data_get_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U32(buf, msg->src_ip);
    SHR_UTIL_UNPACK_U32(buf, msg->dst_ip);
    SHR_UTIL_UNPACK_U16(buf, msg->l4_src_port);
    SHR_UTIL_UNPACK_U16(buf, msg->l4_dst_port);
    SHR_UTIL_UNPACK_U8(buf, msg->ip_protocol);

    SHR_UTIL_UNPACK_U32(buf, msg->vnid);
    SHR_UTIL_UNPACK_U32(buf, msg->inner_src_ip);
    SHR_UTIL_UNPACK_U32(buf, msg->inner_dst_ip);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_l4_src_port);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_l4_dst_port);
    SHR_UTIL_UNPACK_U8(buf, msg->inner_ip_protocol);
    SHR_UTIL_UNPACK_U8(buf, msg->custom_length);
    for (i = 0; i < msg->custom_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->custom[i]);
    }
    SHR_UTIL_UNPACK_U16(buf, msg->in_port);

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_src_ipv6[i]);
        }
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_dst_ipv6[i]);
        }
    }

    return buf;
}

/*
 * Data associated with a flow - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_flow_data_get_reply_pack(uint8_t *buf,
                                                      mcs_ft_msg_ctrl_group_flow_data_get_reply_t *msg)
{
    int i;

    SHR_UTIL_PACK_U32(buf, msg->pkt_count_upper);
    SHR_UTIL_PACK_U32(buf, msg->pkt_count_lower);
    SHR_UTIL_PACK_U32(buf, msg->byte_count_upper);
    SHR_UTIL_PACK_U32(buf, msg->byte_count_lower);
    SHR_UTIL_PACK_U32(buf, msg->flow_start_ts_upper);
    SHR_UTIL_PACK_U32(buf, msg->flow_start_ts_lower);
    SHR_UTIL_PACK_U32(buf, msg->obs_ts_upper);
    SHR_UTIL_PACK_U32(buf, msg->obs_ts_lower);
    SHR_UTIL_PACK_U8(buf, msg->num_drop_reasons);
    for (i = 0; i < msg->num_drop_reasons; i++) {
        SHR_UTIL_PACK_U16(buf, msg->drop_reasons[i]);
    }
    return buf;
}

/*
 * Data associated with a flow - unpack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_group_flow_data_get_reply_unpack(uint8_t *buf,
                                                        mcs_ft_msg_ctrl_group_flow_data_get_reply_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_lower);
    SHR_UTIL_UNPACK_U8(buf, msg->num_drop_reasons);
    for (i = 0; i < msg->num_drop_reasons; i++) {
        SHR_UTIL_UNPACK_U16(buf, msg->drop_reasons[i]);
    }
    SHR_UTIL_UNPACK_U8(buf, msg->is_static);

    return buf;
}

/*
 * SER event - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_ser_event_pack(uint8_t *buf,
                                      mcs_ft_msg_ctrl_ser_event_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U8(buf, msg->pipe);
    SHR_UTIL_PACK_U32(buf, msg->mem);
    SHR_UTIL_PACK_U32(buf, msg->index);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_ser_event_unpack(uint8_t *buf,
                                        mcs_ft_msg_ctrl_ser_event_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U8(buf, msg->pipe);
    SHR_UTIL_UNPACK_U32(buf, msg->mem);
    SHR_UTIL_UNPACK_U32(buf, msg->index);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_template_xmit_pack(uint8_t *buf,
                                          mcs_ft_msg_ctrl_template_xmit_t *msg)
{
    int i;

    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U16(buf, msg->template_id);
    SHR_UTIL_PACK_U16(buf, msg->collector_id);
    SHR_UTIL_PACK_U16(buf, msg->initial_burst);
    SHR_UTIL_PACK_U16(buf, msg->interval_secs);
    SHR_UTIL_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->encap[i]);
    }

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_template_xmit_unpack(uint8_t *buf,
                                            mcs_ft_msg_ctrl_template_xmit_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U16(buf, msg->template_id);
    SHR_UTIL_UNPACK_U16(buf, msg->collector_id);
    SHR_UTIL_UNPACK_U16(buf, msg->initial_burst);
    SHR_UTIL_UNPACK_U16(buf, msg->interval_secs);
    SHR_UTIL_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->encap[i]);
    }

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_elph_profile_create_pack(uint8_t *buf,
                                                mcs_ft_msg_ctrl_elph_profile_create_t *msg)
{
    int i;

    SHR_UTIL_PACK_U8(buf, msg->profile_idx);
    SHR_UTIL_PACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        SHR_UTIL_PACK_U16(buf, msg->filters[i].scan_count);
        SHR_UTIL_PACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        SHR_UTIL_PACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        SHR_UTIL_PACK_U32(buf, msg->filters[i].rate_high_threshold);
        SHR_UTIL_PACK_U32(buf, msg->filters[i].rate_low_threshold);
        SHR_UTIL_PACK_U8(buf, msg->filters[i].incr_rate);
    }
    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_elph_profile_create_unpack(uint8_t *buf,
                                                  mcs_ft_msg_ctrl_elph_profile_create_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U8(buf, msg->profile_idx);
    SHR_UTIL_UNPACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        SHR_UTIL_UNPACK_U16(buf, msg->filters[i].scan_count);
        SHR_UTIL_UNPACK_U32(buf,
                            msg->filters[i].size_threshold_bytes_upper_32);
        SHR_UTIL_UNPACK_U32(buf,
                            msg->filters[i].size_threshold_bytes_lower_32);
        SHR_UTIL_UNPACK_U32(buf, msg->filters[i].rate_high_threshold);
        SHR_UTIL_UNPACK_U32(buf, msg->filters[i].rate_low_threshold);
        SHR_UTIL_UNPACK_U8(buf, msg->filters[i].incr_rate);
    }
    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_elph_profile_get_pack(uint8_t *buf,
                                             mcs_ft_msg_ctrl_elph_profile_get_t *msg)
{
    int i;

    SHR_UTIL_PACK_U8(buf, msg->profile_idx);
    SHR_UTIL_PACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        SHR_UTIL_PACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        SHR_UTIL_PACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        SHR_UTIL_PACK_U32(buf, msg->filters[i].rate_high_threshold);
        SHR_UTIL_PACK_U32(buf, msg->filters[i].rate_low_threshold);
    }
    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_elph_profile_get_unpack(uint8_t *buf,
                                               mcs_ft_msg_ctrl_elph_profile_get_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U8(buf, msg->profile_idx);
    SHR_UTIL_UNPACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        SHR_UTIL_UNPACK_U32(buf,
                            msg->filters[i].size_threshold_bytes_upper_32);
        SHR_UTIL_UNPACK_U32(buf,
                            msg->filters[i].size_threshold_bytes_lower_32);
        SHR_UTIL_UNPACK_U32(buf, msg->filters[i].rate_high_threshold);
        SHR_UTIL_UNPACK_U32(buf, msg->filters[i].rate_low_threshold);
    }
    return buf;
}

/*
 * Get learn stats - pack
 */
uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_learn_get_pack(uint8_t *buf,
                                            mcs_ft_msg_ctrl_stats_learn_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->num_status);
    for (i = 0; i < msg->num_status; i++) {
        SHR_UTIL_PACK_U32(buf, msg->count[i]);
        SHR_UTIL_PACK_U32(buf, msg->avg_usecs[i]);
    }
    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_learn_get_unpack(uint8_t *buf,
                                              mcs_ft_msg_ctrl_stats_learn_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->num_status);
    for (i = 0; i < msg->num_status; i++) {
        SHR_UTIL_UNPACK_U32(buf, msg->count[i]);
        SHR_UTIL_UNPACK_U32(buf, msg->avg_usecs[i]);
    }
    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_export_get_pack(uint8_t *buf,
                                             mcs_ft_msg_ctrl_stats_export_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->num_pkts);
    SHR_UTIL_PACK_U32(buf, msg->num_flows);
    SHR_UTIL_PACK_U32(buf, msg->ts);
    SHR_UTIL_PACK_U32(buf, msg->seq_num);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_export_get_unpack(uint8_t *buf,
                                               mcs_ft_msg_ctrl_stats_export_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->num_pkts);
    SHR_UTIL_UNPACK_U32(buf, msg->num_flows);
    SHR_UTIL_UNPACK_U32(buf, msg->ts);
    SHR_UTIL_UNPACK_U32(buf, msg->seq_num);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_age_get_pack(uint8_t *buf,
                                          mcs_ft_msg_ctrl_stats_age_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->num_flows);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_age_get_unpack(uint8_t *buf,
                                            mcs_ft_msg_ctrl_stats_age_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->num_flows);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_elph_get_pack(uint8_t *buf,
                                           mcs_ft_msg_ctrl_stats_elph_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U16(buf, msg->num_states);
    SHR_UTIL_PACK_U16(buf, msg->num_actions);

    for (i = 0; i < msg->num_states; i++) {
        for (j = 0; j < msg->num_actions; j++) {
            SHR_UTIL_PACK_U32(buf, msg->trans_count[i][j]);
        }
    }

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_elph_get_unpack(uint8_t *buf,
                                             mcs_ft_msg_ctrl_stats_elph_t *msg)
{
    int i, j;

    SHR_UTIL_UNPACK_U16(buf, msg->num_states);
    SHR_UTIL_UNPACK_U16(buf, msg->num_actions);

    for (i = 0; i < msg->num_states; i++) {
        for (j = 0; j < msg->num_actions; j++) {
            SHR_UTIL_UNPACK_U32(buf, msg->trans_count[i][j]);
        }
    }

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_sdk_features_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_sdk_features_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->features);
    SHR_UTIL_PACK_U8(buf, msg->cfg_mode);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_sdk_features_unpack(uint8_t *buf,
                                           mcs_ft_msg_ctrl_sdk_features_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->features);
    SHR_UTIL_UNPACK_U8(buf, msg->cfg_mode);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_uc_features_pack(uint8_t *buf,
                                        mcs_ft_msg_ctrl_uc_features_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->features);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_uc_features_unpack(uint8_t *buf,
                                          mcs_ft_msg_ctrl_uc_features_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->features);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_group_actions_set_pack(uint8_t *buf,
                                              mcs_ft_msg_ctrl_group_actions_set_t *msg)
{
    SHR_UTIL_PACK_U16(buf, msg->group_idx);
    SHR_UTIL_PACK_U32(buf, msg->actions);
    SHR_UTIL_PACK_U16(buf, msg->fsp_cosq);

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_group_actions_set_unpack(uint8_t *buf,
                                                mcs_ft_msg_ctrl_group_actions_set_t *msg)
{
    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U32(buf, msg->actions);
    SHR_UTIL_UNPACK_U16(buf, msg->fsp_cosq);

    return buf;
}

uint8_t *
bcmcth_mon_ft_flow_data_get_first_reply_unpack(uint32_t uc_msg_version,
                                               uint8_t *buf,
                                               mcs_ft_msg_ctrl_flow_data_get_first_reply_t *msg)
{
    int i;
    SHR_UTIL_UNPACK_U16(buf, msg->flow_idx);
    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U8 (buf, msg->is_static);
    SHR_UTIL_UNPACK_U32(buf, msg->src_ipv4);
    SHR_UTIL_UNPACK_U32(buf, msg->dst_ipv4);
    SHR_UTIL_UNPACK_U16(buf, msg->src_l4_port);
    SHR_UTIL_UNPACK_U16(buf, msg->dst_l4_port);
    SHR_UTIL_UNPACK_U8(buf, msg->ip_proto);

    SHR_UTIL_UNPACK_U32(buf, msg->inner_src_ipv4);
    SHR_UTIL_UNPACK_U32(buf, msg->inner_dst_ipv4);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_src_l4_port);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_dst_l4_port);
    SHR_UTIL_UNPACK_U8(buf, msg->inner_ip_proto);
    SHR_UTIL_UNPACK_U8(buf, msg->custom_length);
    for (i = 0; i < msg->custom_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->custom[i]);
    }

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_src_ipv6[i]);
        }
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_dst_ipv6[i]);
        }
    }

    SHR_UTIL_UNPACK_U32(buf, msg->vnid);
    SHR_UTIL_UNPACK_U16(buf, msg->in_port);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_lower);

    return buf;
}

uint8_t  *
bcmcth_mon_ft_flow_data_get_next_reply_unpack(uint32_t uc_msg_version,
                                              uint8_t*buf,
                                              mcs_ft_msg_ctrl_flow_data_get_next_reply_t *msg)
{
    int i;
    SHR_UTIL_UNPACK_U16(buf, msg->flow_idx);
    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U8 (buf, msg->is_static);
    SHR_UTIL_UNPACK_U32(buf, msg->src_ipv4);
    SHR_UTIL_UNPACK_U32(buf, msg->dst_ipv4);
    SHR_UTIL_UNPACK_U16(buf, msg->src_l4_port);
    SHR_UTIL_UNPACK_U16(buf, msg->dst_l4_port);
    SHR_UTIL_UNPACK_U8(buf, msg->ip_proto);

    SHR_UTIL_UNPACK_U32(buf, msg->inner_src_ipv4);
    SHR_UTIL_UNPACK_U32(buf, msg->inner_dst_ipv4);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_src_l4_port);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_dst_l4_port);
    SHR_UTIL_UNPACK_U8(buf, msg->inner_ip_proto);
    SHR_UTIL_UNPACK_U8(buf, msg->custom_length);
    for (i = 0; i < msg->custom_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->custom[i]);
    }

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_src_ipv6[i]);
        }
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_dst_ipv6[i]);
        }
    }

    SHR_UTIL_UNPACK_U32(buf, msg->vnid);
    SHR_UTIL_UNPACK_U16(buf, msg->in_port);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_lower);

    return buf;
}

uint8_t *
bcmcth_mon_ft_static_flow_get_first_reply_unpack(uint32_t uc_msg_version,
                                                 uint8_t *buf,
                                                 mcs_ft_msg_ctrl_static_flow_get_first_reply_t *msg)
{
    int i;
    SHR_UTIL_UNPACK_U16(buf, msg->flow_idx);
    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U32(buf, msg->src_ipv4);
    SHR_UTIL_UNPACK_U32(buf, msg->dst_ipv4);
    SHR_UTIL_UNPACK_U16(buf, msg->src_l4_port);
    SHR_UTIL_UNPACK_U16(buf, msg->dst_l4_port);
    SHR_UTIL_UNPACK_U8(buf, msg->ip_proto);

    SHR_UTIL_UNPACK_U32(buf, msg->inner_src_ipv4);
    SHR_UTIL_UNPACK_U32(buf, msg->inner_dst_ipv4);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_src_l4_port);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_dst_l4_port);
    SHR_UTIL_UNPACK_U8(buf, msg->inner_ip_proto);
    SHR_UTIL_UNPACK_U8(buf, msg->custom_length);
    for (i = 0; i < msg->custom_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->custom[i]);
    }

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_src_ipv6[i]);
        }
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_dst_ipv6[i]);
        }
    }

    SHR_UTIL_UNPACK_U32(buf, msg->vnid);
    SHR_UTIL_UNPACK_U16(buf, msg->in_port);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_lower);

    return buf;
}

uint8_t *
bcmcth_mon_ft_static_flow_get_next_reply_unpack(uint32_t uc_msg_version,
                                                uint8_t *buf,
                                                mcs_ft_msg_ctrl_static_flow_get_next_reply_t *msg)
{
    int i;
    SHR_UTIL_UNPACK_U16(buf, msg->flow_idx);
    SHR_UTIL_UNPACK_U16(buf, msg->group_idx);
    SHR_UTIL_UNPACK_U32(buf, msg->src_ipv4);
    SHR_UTIL_UNPACK_U32(buf, msg->dst_ipv4);
    SHR_UTIL_UNPACK_U16(buf, msg->src_l4_port);
    SHR_UTIL_UNPACK_U16(buf, msg->dst_l4_port);
    SHR_UTIL_UNPACK_U8(buf, msg->ip_proto);

    SHR_UTIL_UNPACK_U32(buf, msg->inner_src_ipv4);
    SHR_UTIL_UNPACK_U32(buf, msg->inner_dst_ipv4);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_src_l4_port);
    SHR_UTIL_UNPACK_U16(buf, msg->inner_dst_l4_port);
    SHR_UTIL_UNPACK_U8(buf, msg->inner_ip_proto);
    SHR_UTIL_UNPACK_U8(buf, msg->custom_length);
    for (i = 0; i < msg->custom_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->custom[i]);
    }

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_src_ipv6[i]);
        }
        for (i = 0; i < 16; i++) {
            SHR_UTIL_UNPACK_U8(buf, msg->inner_dst_ipv6[i]);
        }
    }

    SHR_UTIL_UNPACK_U32(buf, msg->vnid);
    SHR_UTIL_UNPACK_U16(buf, msg->in_port);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->pkt_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->byte_count_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->flow_start_ts_lower);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_upper);
    SHR_UTIL_UNPACK_U32(buf, msg->obs_ts_lower);

    return buf;
}

uint8_t *
bcmcth_mon_ft_group_flow_data_set_pack(uint32_t uc_msg_version,
                                       uint8_t *buf,
                                       mcs_ft_msg_ctrl_group_flow_data_set_t *msg)
{
    int i;
    SHR_UTIL_PACK_U16(buf, msg->group_idx);
    SHR_UTIL_PACK_U32(buf, msg->src_ipv4);
    SHR_UTIL_PACK_U32(buf, msg->dst_ipv4);
    SHR_UTIL_PACK_U16(buf, msg->src_l4_port);
    SHR_UTIL_PACK_U16(buf, msg->dst_l4_port);
    SHR_UTIL_PACK_U8(buf, msg->ip_proto);

    SHR_UTIL_PACK_U32(buf, msg->inner_src_ipv4);
    SHR_UTIL_PACK_U32(buf, msg->inner_dst_ipv4);
    SHR_UTIL_PACK_U16(buf, msg->inner_src_l4_port);
    SHR_UTIL_PACK_U16(buf, msg->inner_dst_l4_port);
    SHR_UTIL_PACK_U8(buf, msg->inner_ip_proto);
    SHR_UTIL_PACK_U8(buf, msg->custom_length);
    for (i = 0; i < msg->custom_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->custom[i]);
    }

    if (uc_msg_version >= FT_MSG_VERSION_5) {
        for (i = 0; i < 16; i++) {
            SHR_UTIL_PACK_U8(buf, msg->inner_src_ipv6[i]);
        }
        for (i = 0; i < 16; i++) {
            SHR_UTIL_PACK_U8(buf, msg->inner_dst_ipv6[i]);
        }
    }

    SHR_UTIL_PACK_U32(buf, msg->vnid);
    SHR_UTIL_PACK_U16(buf, msg->in_port);

    return buf;
}

uint8_t *
bcmcth_mon_ft_em_bank_uft_info_send_msg_pack(uint8_t *buf,
                                             mcs_ft_msg_ctrl_em_bank_uft_info_t *msg)
{
    int i;
    SHR_UTIL_PACK_U8(buf, msg->num_banks);
    for (i=0;i < msg->num_banks; i++) {
        SHR_UTIL_PACK_U32(buf, msg->bank_id[i]);
        SHR_UTIL_PACK_U32(buf, msg->bank_offset[i]);
        SHR_UTIL_PACK_U32(buf, msg->bank_num_buckets[i]);
    }

    return buf;
}

uint8_t *
bcmcth_mon_ft_hw_learn_opt_msg_pack(uint8_t *buf,
                                    mcs_ft_msg_ctrl_hw_learn_opt_msg_t *msg)
{
    int i,j;

    SHR_UTIL_PACK_U8(buf, msg->num_hitbit_ctr_banks);
    for (i = 0; i < msg->num_hitbit_ctr_banks; i++) {
        SHR_UTIL_PACK_U8(buf, msg->ctr_bank_info[i].hitbit_type);
        SHR_UTIL_PACK_U8(buf, msg->ctr_bank_info[i].num_ctr_pools);
        SHR_UTIL_PACK_U16(buf, msg->ctr_bank_info[i].ctr_bank_base_idx);
        for (j = 0; j < msg->ctr_bank_info[i].num_ctr_pools; j++) {
            SHR_UTIL_PACK_U8(buf, msg->ctr_bank_info[i].ctr_pool_id[j]);
            SHR_UTIL_PACK_U16(buf, msg->ctr_bank_info[i].ctr_pool_start_idx[j]);
            SHR_UTIL_PACK_U16(buf, msg->ctr_bank_info[i].ctr_pool_size[j]);
        }
    }

    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_rxpmd_flex_format_set_pack(
                uint8_t *buf,
                mcs_ft_msg_ctrl_rxpmd_flex_format_t *msg)
{
    int i,j;

    SHR_UTIL_PACK_U8(buf, msg->num_fields);
    SHR_UTIL_PACK_U16(buf, msg->num_profiles);
    for (j = 0; j < msg->num_profiles; j++) {
        SHR_UTIL_PACK_U8(buf, msg->profiles[j]);
    }
    for (i = 0; i < msg->num_fields; i++) {
        SHR_UTIL_PACK_U8(buf, msg->fids[i]);
        for (j = 0; j < msg->num_profiles; j++) {
            SHR_UTIL_PACK_U16(buf, msg->offsets[i][j]);
            SHR_UTIL_PACK_U8(buf, msg->widths[i][j]);
        }
    }
    return buf;
}

uint8_t *
bcmcth_mon_ft_msg_ctrl_match_id_set_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_match_id_set_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->num_fields);

    for (i = 0; i < msg->num_fields; i++) {
        SHR_UTIL_PACK_U16(buf, msg->fields[i]);
        SHR_UTIL_PACK_U16(buf, msg->offsets[i]);
        SHR_UTIL_PACK_U16(buf, msg->widths[i]);
        SHR_UTIL_PACK_U16(buf, msg->values[i]);
    }

    return buf;
}
