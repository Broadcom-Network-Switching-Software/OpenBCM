/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ifa2_pack.c
 * Purpose:     IFAv2.0 message packing functions.
 */

#if defined(INCLUDE_IFA)
#include <soc/shared/ifa2.h>
#include <soc/shared/ifa2_msg.h>
#include <shared/pack.h>
#include <bcm_int/esw/ifa2_pack.h>

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_init_pack(uint8 *buf,
                                  shr_ifa2_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->init_time_hi);
    _SHR_PACK_U32(buf, msg->init_time_lo);

    _SHR_PACK_U16(buf, msg->max_export_pkt_length);
    _SHR_PACK_U16(buf, msg->max_rx_pkt_length);

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_md_format_set_pack(uint8 *buf,
                                           shr_ifa2_msg_ctrl_md_format_set_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U16(buf, msg->num_elements);

    for (i = 0; i < msg->num_elements; i++) {
        _SHR_PACK_U16(buf, msg->elements[i]);
        _SHR_PACK_U16(buf, msg->element_widths[i]);
    }

    _SHR_PACK_U8(buf, msg->namespace);

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_config_update_pack(uint8 *buf,
                                           shr_ifa2_msg_ctrl_config_update_t *msg)
{
    int i, j;

    _SHR_PACK_U32(buf, msg->flags);

    _SHR_PACK_U16(buf, msg->num_params);

    for (i = 0; i < msg->num_params; i++) {
        _SHR_PACK_U32(buf, msg->params[i]);
        _SHR_PACK_U16(buf, msg->num_values[i]);
        for (j = 0; j < msg->num_values[i]; j++) {
            _SHR_PACK_U32(buf, msg->values[i][j]);
        }
    }

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_collector_create_pack(uint8 *buf,
                                              shr_ifa2_msg_ctrl_collector_create_t *msg)
{
    int idx;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->ip_base_checksum);
    _SHR_PACK_U32(buf, msg->udp_base_checksum);
    _SHR_PACK_U32(buf, msg->observation_domain_id);

    _SHR_PACK_U16(buf, msg->mtu);
    _SHR_PACK_U16(buf, msg->num_records);
    _SHR_PACK_U16(buf, msg->ip_offset);
    _SHR_PACK_U16(buf, msg->udp_offset);

    _SHR_PACK_U16(buf, msg->encap_length);
    for (idx = 0; idx < msg->encap_length; idx++) {
        _SHR_PACK_U8(buf, msg->encap[idx]);
    }

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_stats_set_pack(uint8 *buf,
                                       shr_ifa2_msg_ctrl_stats_set_t *msg)
{
    _SHR_PACK_U32(buf, msg->rx_pkt_cnt_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_cnt_lo);
    _SHR_PACK_U32(buf, msg->rx_pkt_excess_rate_discard_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_excess_rate_discard_lo);
    _SHR_PACK_U32(buf, msg->rx_pkt_export_disabled_discard_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_export_disabled_discard_lo);
    _SHR_PACK_U32(buf, msg->rx_pkt_no_export_config_discard_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_no_export_config_discard_lo);
    _SHR_PACK_U32(buf, msg->rx_pkt_current_length_exceed_discard_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_current_length_exceed_discard_lo);
    _SHR_PACK_U32(buf, msg->rx_pkt_length_exceed_max_discard_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_length_exceed_max_discard_lo);
    _SHR_PACK_U32(buf, msg->rx_pkt_parse_error_discard_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_parse_error_discard_lo);
    _SHR_PACK_U32(buf, msg->rx_pkt_unknown_namespace_discard_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_unknown_namespace_discard_lo);
    _SHR_PACK_U32(buf, msg->tx_pkt_cnt_hi);
    _SHR_PACK_U32(buf, msg->tx_pkt_cnt_lo);
    _SHR_PACK_U32(buf, msg->tx_record_cnt_hi);
    _SHR_PACK_U32(buf, msg->tx_record_cnt_lo);
    _SHR_PACK_U32(buf, msg->tx_pkt_failure_cnt_hi);
    _SHR_PACK_U32(buf, msg->tx_pkt_failure_cnt_lo);
    _SHR_PACK_U32(buf, msg->rx_pkt_no_template_config_discard_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_no_template_config_discard_lo);

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_stats_get_unpack(uint8 *buf,
                                         shr_ifa2_msg_ctrl_stats_get_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->rx_pkt_cnt_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_cnt_lo);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_excess_rate_discard_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_excess_rate_discard_lo);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_export_disabled_discard_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_export_disabled_discard_lo);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_no_export_config_discard_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_no_export_config_discard_lo);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_current_length_exceed_discard_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_current_length_exceed_discard_lo);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_length_exceed_max_discard_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_length_exceed_max_discard_lo);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_parse_error_discard_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_parse_error_discard_lo);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_unknown_namespace_discard_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_unknown_namespace_discard_lo);
    _SHR_UNPACK_U32(buf, msg->tx_pkt_cnt_hi);
    _SHR_UNPACK_U32(buf, msg->tx_pkt_cnt_lo);
    _SHR_UNPACK_U32(buf, msg->tx_record_cnt_hi);
    _SHR_UNPACK_U32(buf, msg->tx_record_cnt_lo);
    _SHR_UNPACK_U32(buf, msg->tx_pkt_failure_cnt_hi);
    _SHR_UNPACK_U32(buf, msg->tx_pkt_failure_cnt_lo);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_no_template_config_discard_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_no_template_config_discard_lo);

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_template_pack(uint8 *buf,
                                      shr_ifa2_msg_export_template_info_t *msg)
{
    int idx;

    _SHR_PACK_U32(buf, msg->template_id);
    _SHR_PACK_U32(buf, msg->num_export_elements);
    _SHR_PACK_U16(buf, msg->set_id);
    for (idx = 0; idx < msg->num_export_elements; idx++) {
        _SHR_PACK_U32(buf, msg->elements[idx].element);
        _SHR_PACK_U32(buf, msg->elements[idx].data_size);
        _SHR_PACK_U16(buf, msg->elements[idx].id);
        _SHR_PACK_U8(buf, msg->elements[idx].enterprise);
    }

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_template_unpack(uint8 *buf,
                                        shr_ifa2_msg_export_template_info_t *msg)
{
    int idx;

    _SHR_UNPACK_U32(buf, msg->template_id);
    _SHR_UNPACK_U32(buf, msg->num_export_elements);
    _SHR_UNPACK_U16(buf, msg->set_id);
    for (idx = 0; idx < msg->num_export_elements; idx++) {
        _SHR_UNPACK_U32(buf, msg->elements[idx].element);
        _SHR_UNPACK_U32(buf, msg->elements[idx].data_size);
        _SHR_UNPACK_U16(buf, msg->elements[idx].id);
        _SHR_UNPACK_U8(buf, msg->elements[idx].enterprise);
    }

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_template_xmit_pack(uint8 *buf,
                                           shr_ifa2_msg_ctrl_template_xmit_t *msg)
{
    int idx;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->template_id);
    _SHR_PACK_U32(buf, msg->collector_id);
    _SHR_PACK_U32(buf, msg->initial_burst);
    _SHR_PACK_U32(buf, msg->interval_secs);

    _SHR_PACK_U16(buf, msg->encap_length);
    for (idx = 0; idx < msg->encap_length; idx++) {
        _SHR_PACK_U8(buf, msg->encap[idx]);
    }

    return buf;
}

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_template_xmit_unpack(uint8 *buf,
                                             shr_ifa2_msg_ctrl_template_xmit_t *msg)
{
    int idx;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->template_id);
    _SHR_UNPACK_U32(buf, msg->collector_id);
    _SHR_UNPACK_U32(buf, msg->initial_burst);
    _SHR_UNPACK_U32(buf, msg->interval_secs);

    _SHR_UNPACK_U16(buf, msg->encap_length);
    for (idx = 0; idx < msg->encap_length; idx++) {
        _SHR_UNPACK_U8(buf, msg->encap[idx]);
    }

    return buf;
}

#else
typedef int bcm_make_iso_compilers_happy;
#endif /* INCLUDE_IFA */
