/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:        bfd_sdk_pack.c
 * Purpose:     BFD unpack routines for:
 *              - BFD Control messages
 */
#if defined(INCLUDE_BFD)

#include <bcm_int/esw/bfd_sdk_pack.h>
#include <soc/shared/bfd_msg.h>

uint8 *
bfd_sdk_msg_ctrl_init_pack(uint8 *buf, bfd_sdk_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->encap_size);
    _SHR_PACK_U32(buf, msg->num_auth_sha1_keys);
    _SHR_PACK_U32(buf, msg->num_auth_sp_keys);
    _SHR_PACK_U32(buf, msg->rx_channel);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_INIT_CONFIG_FLAGS)) {
        _SHR_PACK_U32(buf, msg->config_flags);
    }

    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_DIP_BASED_LOOKUP)) {
        _SHR_PACK_U32(buf, msg->dip_based_lookup_enable);
    }

    return buf;
}

uint8 *
bfd_sdk_msg_ctrl_stat_reply_unpack(uint8 *buf, bfd_sdk_msg_ctrl_stat_reply_t *msg,
                                   uint8 stat64bit)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->packets_in);
    _SHR_UNPACK_U32(buf, msg->packets_out);
    _SHR_UNPACK_U32(buf, msg->packets_drop);
    _SHR_UNPACK_U32(buf, msg->packets_auth_drop);
    if (stat64bit && BFD_UC_FEATURE_CHECK(BFD_FEATURE_64BIT_STAT)) {
        _SHR_UNPACK_U32(buf, msg->packets_in_hi);
        _SHR_UNPACK_U32(buf, msg->packets_out_hi);
        _SHR_UNPACK_U32(buf, msg->packets_drop_hi);
        _SHR_UNPACK_U32(buf, msg->packets_auth_drop_hi);
    }
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_ECHO_REPLY_PACKETS)) {
        _SHR_UNPACK_U32(buf, msg->packets_echo_reply);
        _SHR_UNPACK_U32(buf, msg->packets_echo_reply_hi);
    }
    return buf;
}


uint8 *
bfd_sdk_msg_ctrl_stat_req_pack(uint8 *buf,
                               bfd_sdk_msg_ctrl_stat_req_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->options);

    return buf;
}

uint8 *
bfd_sdk_version_exchange_msg_unpack(uint8 *buf, bfd_sdk_version_exchange_msg_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bfd_sdk_version_exchange_msg_pack(uint8 *buf, bfd_sdk_version_exchange_msg_t *msg)
{
    _SHR_PACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bfd_sdk_msg_ctrl_sess_set_pack(uint8 *buf,
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
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_REMOTE_MEP_ID)) {
        _SHR_PACK_U32(buf, msg->remote_mep_id_length);
        for (i = 0; i < msg->remote_mep_id_length; i++) {
             _SHR_PACK_U8(buf, msg->remote_mep_id[i]);
        }
    }
    if(BFD_UC_FEATURE_CHECK(BFD_FEATURE_RX_PKT_VLAN_ID)) {
        _SHR_PACK_U16(buf, msg->rx_pkt_vlan_id);
    }
    return buf;
}

uint8 *
bfd_sdk_msg_ctrl_sess_get_unpack(uint8 *buf,
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
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_REMOTE_MEP_ID)) {
        _SHR_UNPACK_U32(buf, msg->remote_mep_id_length);
        for (i = 0; i < msg->remote_mep_id_length; i++) {
             _SHR_UNPACK_U8(buf, msg->remote_mep_id[i]);
        }
        _SHR_UNPACK_U32(buf, msg->mis_conn_mep_id_length);
        for (i = 0; i < msg->mis_conn_mep_id_length; i++) {
             _SHR_UNPACK_U8(buf, msg->mis_conn_mep_id[i]);
        }
    }

    if(BFD_UC_FEATURE_CHECK(BFD_FEATURE_RX_PKT_VLAN_ID)) {
        _SHR_UNPACK_U16(buf, msg->rx_pkt_vlan_id);
    }

    return buf;
}

uint8 *
bfd_sdk_msg_ctrl_discard_stat_get_unpack(uint8 *buf,
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
bfd_sdk_msg_ctrl_auth_sha1_pack(uint8 *buf,
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
bfd_sdk_msg_ctrl_auth_sha1_unpack(uint8 *buf,
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
bfd_sdk_msg_ctrl_auth_sp_pack(uint8 *buf, shr_bfd_msg_ctrl_auth_sp_t *msg)
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
bfd_sdk_msg_ctrl_auth_sp_unpack(uint8 *buf, shr_bfd_msg_ctrl_auth_sp_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->index);
    _SHR_UNPACK_U8(buf, msg->length);
    for (i = 0; i < _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->password[i]);
    }

    return buf;
}

#if defined(BCM_TOMAHAWK_SUPPORT)
uint8 *
bfd_sdk_msg_port_txqueue_map_pack(uint8 *buf,
                                  bfd_sdk_msg_port_txqueue_map_t *msg) {
    int i = 0;

    _SHR_PACK_U16(buf, msg->max_num_port);
    for (i = 0; i <= msg->max_num_port; i++) {
        _SHR_PACK_U16(buf, msg->tx_queue[i]);
    }

    return buf;
}
#endif

uint8* bfd_sdk_msg_ctrl_trace_log_enable_unpack(uint8 *buf,
                                              bfd_sdk_msg_ctrl_trace_log_enable_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->addr_hi);
    _SHR_UNPACK_U32(buf, msg->addr_lo);

    return buf;
}

#if defined(BCM_TRIDENT3_SUPPORT)
uint8 *
bfd_sdk_msg_match_id_map_pack(uint8 *buf,
                              bfd_sdk_msg_match_id_map_t *msg) {
    int i = 0;

    _SHR_PACK_U32(buf, msg->match_id_types);
    _SHR_PACK_U32(buf, msg->match_id_length);
    for (i = 0; i <= msg->match_id_types; i++) {
        _SHR_PACK_U32(buf, msg->match_id_fields[i].offset);
        _SHR_PACK_U32(buf, msg->match_id_fields[i].width);
        _SHR_PACK_U32(buf, msg->match_id_fields[i].value);
    }

    return buf;
}
#endif

#else /* INCLUDE_BFD */
typedef int bcm_xgs5_bfd_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_BFD */
