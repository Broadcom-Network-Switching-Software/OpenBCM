/*! \file bcmcth_mon_int_pack.c
 *
 * Pack and unpack routines for the Inband Telemetry
 * embedded application messages.
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
#include <bcmbd/mcs_shared/mcs_ifa_common.h>
#include <bcmcth/bcmcth_mon_int_pack.h>

/*******************************************************************************
* Public functions
 */
uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_init_pack(uint8_t *buf,
                                      mcs_ifa_msg_ctrl_init_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->npl_version);
    SHR_UTIL_PACK_U32(buf, msg->device_id);
    SHR_UTIL_PACK_U32(buf, msg->init_time_hi);
    SHR_UTIL_PACK_U32(buf, msg->init_time_lo);

    SHR_UTIL_PACK_U16(buf, msg->max_export_pkt_length);
    SHR_UTIL_PACK_U16(buf, msg->max_rx_pkt_length);
    SHR_UTIL_PACK_U16(buf, msg->fifo_width);
    SHR_UTIL_PACK_U16(buf, msg->fifo_depth);

    return buf;
}

uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_rxpmd_flex_format_set_pack(uint8_t *buf,
                                                       mcs_ifa_msg_ctrl_rxpmd_flex_format_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U16(buf, msg->num_profiles);
    for (i = 0; i < msg->num_profiles; i++) {
        SHR_UTIL_PACK_U16(buf, msg->profiles[i]);
    }

    SHR_UTIL_PACK_U8(buf, msg->num_fields);
    for (i = 0; i < msg->num_fields; i++) {
        SHR_UTIL_PACK_U8(buf, msg->fields[i]);
    }

    for (i = 0; i < msg->num_profiles; i++) {
        for (j = 0; j < msg->num_fields; j++) {
            SHR_UTIL_PACK_U16(buf, msg->offsets[i][j]);
            SHR_UTIL_PACK_U8(buf, msg->widths[i][j]);
        }
    }

    return buf;
}

uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_fifo_format_set_pack(uint8_t *buf,
                                                 mcs_ifa_msg_ctrl_fifo_format_set_t *msg)
{
    int i;

    SHR_UTIL_PACK_U32(buf, msg->flags);

    SHR_UTIL_PACK_U16(buf, msg->id);
    SHR_UTIL_PACK_U16(buf, msg->num_elements);

    for (i = 0; i < msg->num_elements; i++) {
        SHR_UTIL_PACK_U16(buf, msg->elements[i]);
    }

    return buf;
}

uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_md_format_set_pack(uint8_t *buf,
                                               mcs_ifa_msg_ctrl_md_format_set_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U32(buf, msg->flags);

    SHR_UTIL_PACK_U16(buf, msg->id);
    SHR_UTIL_PACK_U16(buf, msg->fifo_id);
    SHR_UTIL_PACK_U16(buf, msg->num_elements);

    for (i = 0; i < msg->num_elements; i++) {
        SHR_UTIL_PACK_U16(buf, msg->elements[i]);
        SHR_UTIL_PACK_U16(buf, msg->element_widths[i]);

        SHR_UTIL_PACK_U8(buf, msg->num_fifo_parts[i]);
        for (j = 0; j < msg->num_fifo_parts[i]; j++) {
            SHR_UTIL_PACK_U16(buf, msg->fifo_parts[i][j]);
            SHR_UTIL_PACK_U16(buf, msg->fifo_part_offsets[i][j]);
            SHR_UTIL_PACK_U16(buf, msg->fifo_part_widths[i][j]);
        }
    }

    SHR_UTIL_PACK_U8(buf, msg->namespace);

    return buf;
}

uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_config_update_pack(uint8_t *buf,
                                               mcs_ifa_msg_ctrl_config_update_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U32(buf, msg->flags);

    SHR_UTIL_PACK_U16(buf, msg->num_params);

    for (i = 0; i < msg->num_params; i++) {
        SHR_UTIL_PACK_U32(buf, msg->params[i]);
        SHR_UTIL_PACK_U16(buf, msg->num_values[i]);
        for (j = 0; j < msg->num_values[i]; j++) {
            SHR_UTIL_PACK_U32(buf, msg->values[i][j]);
        }
    }

    return buf;
}

uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_collector_create_pack(uint8_t *buf,
                                                  mcs_ifa_msg_ctrl_collector_create_t *msg)
{
    int idx;

    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->ip_base_checksum);
    SHR_UTIL_PACK_U32(buf, msg->udp_base_checksum);
    SHR_UTIL_PACK_U32(buf, msg->observation_domain);

    SHR_UTIL_PACK_U16(buf, msg->mtu);
    SHR_UTIL_PACK_U16(buf, msg->num_records);
    SHR_UTIL_PACK_U16(buf, msg->ip_offset);
    SHR_UTIL_PACK_U16(buf, msg->udp_offset);

    SHR_UTIL_PACK_U16(buf, msg->encap_length);
    for (idx = 0; idx < msg->encap_length; idx++) {
        SHR_UTIL_PACK_U8(buf, msg->encap[idx]);
    }

    return buf;
}

uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_ipfix_template_set_pack(uint8_t *buf,
                                                    mcs_ifa_msg_ctrl_ipfix_template_set_t *msg)
{
    int i;

    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->tx_interval);
    SHR_UTIL_PACK_U32(buf, msg->initial_burst);
    SHR_UTIL_PACK_U16(buf, msg->set_id);
    SHR_UTIL_PACK_U8(buf, msg->num_elements);
    for (i = 0; i < msg->num_elements; i++) {
        SHR_UTIL_PACK_U8(buf, msg->elements[i]);
    }
    SHR_UTIL_PACK_U16(buf, msg->template_set_length);
    for (i = 0; i < msg->template_set_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->template_set[i]);
    }

    return buf;
}

uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_stats_set_pack(uint8_t *buf,
                                           mcs_ifa_msg_ctrl_stats_set_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->valid_bmp);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_cnt_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_cnt_lo);
    SHR_UTIL_PACK_U32(buf, msg->tx_pkt_cnt_hi);
    SHR_UTIL_PACK_U32(buf, msg->tx_pkt_cnt_lo);
    SHR_UTIL_PACK_U32(buf, msg->tx_record_cnt_hi);
    SHR_UTIL_PACK_U32(buf, msg->tx_record_cnt_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_export_disabled_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_export_disabled_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_no_export_config_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_no_export_config_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_no_ipfix_config_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_no_ipfix_config_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_current_length_exceed_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_current_length_exceed_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_length_exceed_max_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_length_exceed_max_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_parse_error_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_parse_error_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_unknown_namespace_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_unknown_namespace_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_excess_rate_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_excess_rate_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_incomplete_metadata_discard_hi);
    SHR_UTIL_PACK_U32(buf, msg->rx_pkt_incomplete_metadata_discard_lo);
    SHR_UTIL_PACK_U32(buf, msg->tx_pkt_failure_cnt_hi);
    SHR_UTIL_PACK_U32(buf, msg->tx_pkt_failure_cnt_lo);

    return buf;
}

uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_stats_get_unpack(uint8_t *buf,
                                             mcs_ifa_msg_ctrl_stats_get_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_cnt_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_cnt_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->tx_pkt_cnt_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->tx_pkt_cnt_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->tx_record_cnt_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->tx_record_cnt_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_export_disabled_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_export_disabled_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_no_export_config_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_no_export_config_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_no_ipfix_config_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_no_ipfix_config_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_current_length_exceed_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_current_length_exceed_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_length_exceed_max_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_length_exceed_max_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_parse_error_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_parse_error_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_unknown_namespace_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_unknown_namespace_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_excess_rate_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_excess_rate_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_incomplete_metadata_discard_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->rx_pkt_incomplete_metadata_discard_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->tx_pkt_failure_cnt_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->tx_pkt_failure_cnt_lo);

    return buf;
}
