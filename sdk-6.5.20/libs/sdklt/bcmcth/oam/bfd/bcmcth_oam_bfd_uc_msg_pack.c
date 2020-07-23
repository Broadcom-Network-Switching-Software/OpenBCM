/*! \file bcmcth_oam_bfd_uc_msg_pack.c
 *
 * Pack and unpack routines for the BFD embedded application messages.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_util_pack.h>
#include <bcmbd/mcs_shared/mcs_bfd_common.h>
#include <bcmcth/bcmcth_oam_bfd_uc_msg_pack.h>

/******************************************************************************
 * Public functions
 */
uint8_t *
bcmcth_oam_bfd_msg_ctrl_init_pack(uint8_t *buf, mcs_bfd_msg_ctrl_init_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->num_sessions);
    SHR_UTIL_PACK_U32(buf, msg->num_auth_sha1_keys);
    SHR_UTIL_PACK_U32(buf, msg->num_auth_sp_keys);
    SHR_UTIL_PACK_U16(buf, msg->max_pkt_size);

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_ep_set_pack(uint8_t *buf, mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    int i;

    SHR_UTIL_PACK_U32(buf, msg->endpoint_id);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->update_flags);
    SHR_UTIL_PACK_U32(buf, msg->local_discr);
    SHR_UTIL_PACK_U32(buf, msg->remote_discr);
    SHR_UTIL_PACK_U32(buf, msg->local_min_tx);
    SHR_UTIL_PACK_U32(buf, msg->local_min_rx);
    SHR_UTIL_PACK_U32(buf, msg->local_min_echo_rx);
    SHR_UTIL_PACK_U32(buf, msg->auth_key);
    SHR_UTIL_PACK_U32(buf, msg->xmt_auth_seq);
    SHR_UTIL_PACK_U32(buf, msg->mpls_label);

    SHR_UTIL_PACK_U16(buf, msg->tx_port);
    SHR_UTIL_PACK_U16(buf, msg->tx_cos);
    SHR_UTIL_PACK_U16(buf, msg->rx_pkt_vlan_id);
    SHR_UTIL_PACK_U16(buf, msg->lkey_etype);
    SHR_UTIL_PACK_U16(buf, msg->lkey_offset);
    SHR_UTIL_PACK_U16(buf, msg->lkey_length);

    SHR_UTIL_PACK_U8(buf, msg->encap_type);
    SHR_UTIL_PACK_U8(buf, msg->local_echo);
    SHR_UTIL_PACK_U8(buf, msg->passive);
    SHR_UTIL_PACK_U8(buf, msg->local_demand);
    SHR_UTIL_PACK_U8(buf, msg->local_diag);
    SHR_UTIL_PACK_U8(buf, msg->local_detect_mult);
    SHR_UTIL_PACK_U8(buf, msg->auth_type);

    SHR_UTIL_PACK_U16(buf, msg->encap_length);
    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->encap_data[i]);
    }

    SHR_UTIL_PACK_U16(buf, msg->source_mep_id_length);
    for (i = 0; i < msg->source_mep_id_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->source_mep_id[i]);
    }

    SHR_UTIL_PACK_U16(buf, msg->remote_mep_id_length);
    for (i = 0; i < msg->remote_mep_id_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->remote_mep_id[i]);
    }

    return buf;
}


uint8_t *
bcmcth_oam_bfd_msg_ctrl_ep_status_get_unpack(uint8_t *buf,
                                             mcs_bfd_msg_ctrl_ep_status_get_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U32(buf, msg->local_discr);
    SHR_UTIL_UNPACK_U32(buf, msg->remote_discr);
    SHR_UTIL_UNPACK_U32(buf, msg->remote_min_tx);
    SHR_UTIL_UNPACK_U32(buf, msg->remote_min_rx);
    SHR_UTIL_UNPACK_U32(buf, msg->remote_min_echo_rx);
    SHR_UTIL_UNPACK_U32(buf, msg->xmt_auth_seq);
    SHR_UTIL_UNPACK_U32(buf, msg->rcv_auth_seq);

    SHR_UTIL_UNPACK_U8(buf, msg->poll);
    SHR_UTIL_UNPACK_U8(buf, msg->remote_demand);
    SHR_UTIL_UNPACK_U8(buf, msg->local_diag);
    SHR_UTIL_UNPACK_U8(buf, msg->remote_diag);
    SHR_UTIL_UNPACK_U8(buf, msg->local_state);
    SHR_UTIL_UNPACK_U8(buf, msg->remote_state);
    SHR_UTIL_UNPACK_U8(buf, msg->remote_detect_mult);

    SHR_UTIL_UNPACK_U16(buf, msg->mis_conn_mep_id_length);
    for (i = 0; i < msg->mis_conn_mep_id_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->mis_conn_mep_id[i]);
    }

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_ep_stats_set_pack(uint8_t *buf,
                                          mcs_bfd_msg_ctrl_ep_stats_set_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->endpoint_id);
    SHR_UTIL_PACK_U32(buf, msg->valid_bmp);
    SHR_UTIL_PACK_U32(buf, msg->pkts_in_hi);
    SHR_UTIL_PACK_U32(buf, msg->pkts_in_lo);
    SHR_UTIL_PACK_U32(buf, msg->pkts_out_hi);
    SHR_UTIL_PACK_U32(buf, msg->pkts_out_lo);
    SHR_UTIL_PACK_U32(buf, msg->pkts_drop_hi);
    SHR_UTIL_PACK_U32(buf, msg->pkts_drop_lo);
    SHR_UTIL_PACK_U32(buf, msg->pkts_auth_drop_hi);
    SHR_UTIL_PACK_U32(buf, msg->pkts_auth_drop_lo);
    SHR_UTIL_PACK_U32(buf, msg->pkts_echo_reply_hi);
    SHR_UTIL_PACK_U32(buf, msg->pkts_echo_reply_lo);

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_ep_stats_get_unpack(uint8_t *buf,
                                            mcs_bfd_msg_ctrl_ep_stats_get_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_in_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_in_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_out_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_out_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_drop_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_drop_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_auth_drop_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_auth_drop_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_echo_reply_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->pkts_echo_reply_lo);

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_global_stats_set_pack(uint8_t *buf,
                                              mcs_bfd_msg_ctrl_global_stats_set_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->valid_bmp);
    SHR_UTIL_PACK_U32(buf, msg->ver_err_hi);
    SHR_UTIL_PACK_U32(buf, msg->ver_err_lo);
    SHR_UTIL_PACK_U32(buf, msg->len_err_hi);
    SHR_UTIL_PACK_U32(buf, msg->len_err_lo);
    SHR_UTIL_PACK_U32(buf, msg->zero_detect_mult_hi);
    SHR_UTIL_PACK_U32(buf, msg->zero_detect_mult_lo);
    SHR_UTIL_PACK_U32(buf, msg->zero_my_disc_hi);
    SHR_UTIL_PACK_U32(buf, msg->zero_my_disc_lo);
    SHR_UTIL_PACK_U32(buf, msg->p_f_bit_hi);
    SHR_UTIL_PACK_U32(buf, msg->p_f_bit_lo);
    SHR_UTIL_PACK_U32(buf, msg->m_bit_hi);
    SHR_UTIL_PACK_U32(buf, msg->m_bit_lo);
    SHR_UTIL_PACK_U32(buf, msg->auth_type_mismatch_hi);
    SHR_UTIL_PACK_U32(buf, msg->auth_type_mismatch_lo);
    SHR_UTIL_PACK_U32(buf, msg->auth_sp_err_hi);
    SHR_UTIL_PACK_U32(buf, msg->auth_sp_err_lo);
    SHR_UTIL_PACK_U32(buf, msg->auth_sha1_err_hi);
    SHR_UTIL_PACK_U32(buf, msg->auth_sha1_err_lo);
    SHR_UTIL_PACK_U32(buf, msg->ep_not_found_hi);
    SHR_UTIL_PACK_U32(buf, msg->ep_not_found_lo);

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_global_stats_get_unpack(uint8_t *buf,
                                                mcs_bfd_msg_ctrl_global_stats_get_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->ver_err_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->ver_err_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->len_err_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->len_err_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->zero_detect_mult_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->zero_detect_mult_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->zero_my_disc_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->zero_my_disc_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->p_f_bit_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->p_f_bit_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->m_bit_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->m_bit_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->auth_type_mismatch_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->auth_type_mismatch_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->auth_sp_err_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->auth_sp_err_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->auth_sha1_err_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->auth_sha1_err_lo);
    SHR_UTIL_UNPACK_U32(buf, msg->ep_not_found_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->ep_not_found_lo);

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_auth_sha1_set_pack(uint8_t *buf,
                                           mcs_bfd_msg_ctrl_auth_sha1_set_t *msg)
{
    int i;

    SHR_UTIL_PACK_U32(buf, msg->index);
    for (i = 0; i < MCS_BFD_AUTH_SHA1_KEY_LENGTH; i++) {
        SHR_UTIL_PACK_U8(buf, msg->key[i]);
    }

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_auth_sp_set_pack(uint8_t *buf,
                                         mcs_bfd_msg_ctrl_auth_sp_set_t *msg)
{
    int i;

    SHR_UTIL_PACK_U32(buf, msg->index);
    SHR_UTIL_PACK_U8(buf, msg->length);
    for (i = 0; i < msg->length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->password[i]);
    }

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_global_param_set_pack(uint8_t *buf,
                                              mcs_bfd_msg_ctrl_global_param_set_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->param);
    SHR_UTIL_PACK_U32(buf, msg->value);

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_trace_log_enable_reply_unpack(uint8_t *buf,
                                                      mcs_bfd_msg_ctrl_trace_log_enable_reply_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->addr_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->addr_lo);

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_rxpmd_flex_format_set_pack(uint8_t *buf,
                                                   mcs_bfd_msg_ctrl_rxpmd_flex_format_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U8(buf, msg->num_fields);
    SHR_UTIL_PACK_U16(buf, msg->num_flow_ids);

    for (i = 0; i < msg->num_fields; i++) {
        for (j = 0; j < msg->num_flow_ids; j++) {
            SHR_UTIL_PACK_U16(buf, msg->offsets[i][j]);
            SHR_UTIL_PACK_U8(buf, msg->widths[i][j]);
        }
    }

    return buf;
}

uint8_t *
bcmcth_oam_bfd_msg_ctrl_match_id_set_pack(uint8_t *buf,
                                          mcs_bfd_msg_ctrl_match_id_set_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->num_fields);

    for (i = 0; i < msg->num_fields; i++) {
        SHR_UTIL_PACK_U16(buf, msg->offsets[i]);
        SHR_UTIL_PACK_U16(buf, msg->widths[i]);
        SHR_UTIL_PACK_U16(buf, msg->values[i]);
    }

    return buf;
}
