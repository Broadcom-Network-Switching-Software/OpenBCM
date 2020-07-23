/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:        bfd_sdk_pack.c
 * Purpose:     BFD unpack routines for:
 *              - BFD Control messages
 */
#include <bcm_int/dpp/bfd_feature.h>
#include <bcm_int/dpp/bfd_sdk_pack.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/bfd_msg.h>

/***********************************************************
 * BFD Control Message Pack/Unpack
 *
 * Functions:
 *      bfd_sdk_msg_ctrl_<type>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      BFD control message into/from a given buffer
 * Parameters:
 *   _pack()
 *      buffer  - (OUT) Buffer where to pack message.
 *      msg     - (IN)  BFD control message to pack.
 *   _unpack()
 *      buffer  - (IN)  Buffer with message to unpack.
 *      msg     - (OUT) Returns BFD control message.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
bfd_sdk_dpp_msg_ctrl_init_pack(uint8 *buf, bfd_sdk_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->encap_size);
    _SHR_PACK_U32(buf, msg->num_auth_sha1_keys);
    _SHR_PACK_U32(buf, msg->num_auth_sp_keys);
    _SHR_PACK_U32(buf, msg->rx_channel);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_INIT_CONFIG_FLAGS)) {
        _SHR_PACK_U32(buf, msg->config_flags);
    }
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_PUNT_BUFFERS)) {
        _SHR_PACK_U32(buf, msg->num_punt_buffers);
    }

    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_PROTECTION_PKT)) {
        _SHR_PACK_U32(buf, msg->protection_packet_enable);
        _SHR_PACK_U32(buf, msg->protection_pkt_event_fifo_size);
        _SHR_PACK_U32(buf, msg->protection_pkt_event_fifo_timer);
    }

    return buf;
}

uint8 *
bfd_sdk_dpp_msg_ctrl_sess_set_pack(uint8 *buf,
                               bfd_sdk_msg_ctrl_sess_set_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->sess_id);             
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U8(buf, msg->passive);
    _SHR_PACK_U8(buf, msg->local_demand);
    _SHR_PACK_U8(buf, msg->local_diag);
    _SHR_PACK_U8(buf, msg->local_detect_mult);
    _SHR_PACK_U32(buf, msg->local_discriminator);
    _SHR_PACK_U32(buf, msg->remote_discriminator);
    _SHR_PACK_U32(buf, msg->local_min_tx);
    _SHR_PACK_U32(buf, msg->local_min_rx);
    _SHR_PACK_U32(buf, msg->local_min_echo_rx);
    _SHR_PACK_U8(buf, msg->auth_type);
    _SHR_PACK_U32(buf, msg->auth_key);
    _SHR_PACK_U32(buf, msg->xmt_auth_seq);
    _SHR_PACK_U8(buf, msg->encap_type);
    _SHR_PACK_U32(buf, msg->encap_length);
    for (i = 0; i < SHR_BFD_MAX_ENCAP_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->encap_data[i]);
    }
    _SHR_PACK_U16(buf, msg->lkey_etype);
    _SHR_PACK_U16(buf, msg->lkey_offset);
    _SHR_PACK_U16(buf, msg->lkey_length);
    _SHR_PACK_U32(buf, msg->mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->mep_id[i]);
    }
    for (i = 0; i < SHR_BFD_MPLS_LABEL_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->mpls_label[i]);
    }
    _SHR_PACK_U32(buf, msg->tx_port);
    _SHR_PACK_U32(buf, msg->tx_cos);
    _SHR_PACK_U32(buf, msg->tx_pri);
    _SHR_PACK_U32(buf, msg->tx_qnum);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_ECHO_MODE)) {
        _SHR_PACK_U8(buf, msg->local_echo);
    }
#ifdef SDK_45223_DISABLED
    _SHR_PACK_U32(buf, msg->remote_mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->remote_mep_id[i]);
    }
#endif
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_PUNT_MODE)) {
        _SHR_PACK_U8(buf, msg->sampling_ratio);
    }
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_SSO_UPDATE)) {
         _SHR_PACK_U32(buf, msg->actual_tx_interval);
         _SHR_PACK_U8(buf, msg->remote_detect_mult);
    }

    return buf;
}

uint8 *
bfd_sdk_dpp_msg_ctrl_sess_get_unpack(uint8 *buf,
                                     bfd_sdk_msg_ctrl_sess_get_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U8(buf, msg->enable);
    _SHR_UNPACK_U8(buf, msg->passive);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_POLL_BIT)) {
        _SHR_UNPACK_U8(buf, msg->poll);
    }
    _SHR_UNPACK_U8(buf, msg->local_demand);
    _SHR_UNPACK_U8(buf, msg->remote_demand);
    _SHR_UNPACK_U8(buf, msg->local_diag);
    _SHR_UNPACK_U8(buf, msg->remote_diag);
    _SHR_UNPACK_U8(buf, msg->local_sess_state);
    _SHR_UNPACK_U8(buf, msg->remote_sess_state);
    _SHR_UNPACK_U8(buf, msg->local_detect_mult);
    _SHR_UNPACK_U8(buf, msg->remote_detect_mult);
    _SHR_UNPACK_U32(buf, msg->local_discriminator);
    _SHR_UNPACK_U32(buf, msg->remote_discriminator); 
    _SHR_UNPACK_U32(buf, msg->local_min_tx);
    _SHR_UNPACK_U32(buf, msg->remote_min_tx);
    _SHR_UNPACK_U32(buf, msg->local_min_rx);
    _SHR_UNPACK_U32(buf, msg->remote_min_rx);
    _SHR_UNPACK_U32(buf, msg->local_min_echo_rx);
    _SHR_UNPACK_U32(buf, msg->remote_min_echo_rx);
    _SHR_UNPACK_U8(buf, msg->auth_type);
    _SHR_UNPACK_U32(buf, msg->auth_key);
    _SHR_UNPACK_U32(buf, msg->xmt_auth_seq);
    _SHR_UNPACK_U32(buf, msg->rcv_auth_seq);
    _SHR_UNPACK_U8(buf, msg->encap_type);
    _SHR_UNPACK_U32(buf, msg->encap_length); 
    for (i = 0; i < SHR_BFD_MAX_ENCAP_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->encap_data[i]);
    }
    _SHR_UNPACK_U16(buf, msg->lkey_etype);
    _SHR_UNPACK_U16(buf, msg->lkey_offset);
    _SHR_UNPACK_U16(buf, msg->lkey_length);
    _SHR_UNPACK_U32(buf, msg->mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->mep_id[i]);
    }
    for (i = 0; i < SHR_BFD_MPLS_LABEL_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->mpls_label[i]);
    }
    _SHR_UNPACK_U32(buf, msg->tx_port);
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U32(buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_ECHO_MODE)) {
        _SHR_UNPACK_U8(buf, msg->local_echo);
    }

#ifdef SDK_45223_DISABLED
    _SHR_UNPACK_U32(buf, msg->remote_mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->remote_mep_id[i]);
    }
    _SHR_UNPACK_U32(buf, msg->mis_conn_mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->mis_conn_mep_id[i]);
    }
#endif
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_PUNT_MODE)) {
        _SHR_UNPACK_U8(buf, msg->sampling_ratio);
    }

    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_SSO_UPDATE)) {
        _SHR_UNPACK_U32(buf, msg->actual_tx_interval);
    }

    return buf;
}

uint8* bfd_sdk_dpp_msg_ctrl_trace_log_enable_unpack(uint8 *buf,
                                                    bfd_sdk_msg_ctrl_trace_log_enable_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->addr_hi);
    _SHR_UNPACK_U32(buf, msg->addr_lo);

    return buf;
}

uint8 *
bfd_sdk_dpp_msg_ctrl_stat_req_pack(uint8 *buf,
                                   shr_bfd_msg_ctrl_stat_req_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->clear);

    return buf;
}

uint8* bfd_sdk_dpp_msg_ctrl_stat_reply_unpack(uint8 *buf,
                                             bfd_sdk_msg_ctrl_stat_reply_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->packets_in);
    _SHR_UNPACK_U32(buf, msg->packets_out);
    _SHR_UNPACK_U32(buf, msg->packets_drop);
    _SHR_UNPACK_U32(buf, msg->packets_auth_drop);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_ECHO_REPLY_PACKETS)) {
        _SHR_UNPACK_U32(buf, msg->packets_echo_reply);
        _SHR_UNPACK_U32(buf, msg->packets_echo_reply_hi);
    }

    return buf;
}

uint8 *
bfd_sdk_dpp_msg_ctrl_discard_stat_get_unpack(uint8 *buf,
                                         bfd_sdk_msg_ctrl_discard_stat_get_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->bfd_ver_err);
    _SHR_UNPACK_U32(buf, msg->bfd_len_err);
    _SHR_UNPACK_U32(buf, msg->bfd_detect_mult);
    _SHR_UNPACK_U32(buf, msg->bfd_my_disc);
    _SHR_UNPACK_U32(buf, msg->bfd_p_f_bit);
    _SHR_UNPACK_U32(buf, msg->bfd_m_bit);
    _SHR_UNPACK_U32(buf, msg->bfd_auth_type_mismatch);
    _SHR_UNPACK_U32(buf, msg->bfd_auth_simple_err);
    _SHR_UNPACK_U32(buf, msg->bfd_auth_m_sha1_err);
    _SHR_UNPACK_U32(buf, msg->bfd_sess_mismatch);

    return buf;
}

uint8 *
bfd_sdk_dpp_msg_ctrl_auth_sha1_pack(uint8 *buf,
                                    shr_bfd_msg_ctrl_auth_sha1_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->index);
    _SHR_PACK_U8(buf, msg->enable);
    _SHR_PACK_U32(buf, msg->sequence);
    for (i = 0; i < _SHR_BFD_AUTH_SHA1_KEY_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->key[i]);
    }

    return buf;
}

uint8 *
bfd_sdk_dpp_msg_ctrl_auth_sha1_unpack(uint8 *buf,
                                      shr_bfd_msg_ctrl_auth_sha1_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->index);
    _SHR_UNPACK_U8(buf, msg->enable);
    _SHR_UNPACK_U32(buf, msg->sequence);
    for (i = 0; i < _SHR_BFD_AUTH_SHA1_KEY_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->key[i]);
    }

    return buf;
}


uint8 *
bfd_sdk_dpp_msg_ctrl_auth_sp_pack(uint8 *buf,
                                  shr_bfd_msg_ctrl_auth_sp_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->index);
    _SHR_PACK_U8(buf, msg->length);
    for (i = 0; i < _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->password[i]);
    }

    return buf;
}

uint8 *
bfd_sdk_dpp_msg_ctrl_auth_sp_unpack(uint8 *buf,
                                    shr_bfd_msg_ctrl_auth_sp_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->index);
    _SHR_UNPACK_U8(buf, msg->length);
    for (i = 0; i < _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->password[i]);
    }

    return buf;
}
