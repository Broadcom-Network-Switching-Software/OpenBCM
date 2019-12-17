/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bfd_pack.c
 * Purpose:     BFD pack and unpack routines for:
 *              - BFD Control messages
 *              - Network Packet headers (PDUs)
 *
 *
 * BFD control messages
 *
 * BFD messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The BFD control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <shared/bfd.h>
#include <soc/shared/bfd_msg.h>
#include <soc/shared/bfd_pkt.h>
#include <soc/shared/bfd_pack.h>


uint8 *
shr_bfd_msg_ctrl_sess_set_pack(uint8 *buf,
                               shr_bfd_msg_ctrl_sess_set_t *msg)
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
#ifdef SDK_45223_DISABLED
    _SHR_PACK_U32(buf, msg->remote_mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->remote_mep_id[i]);
    }
#endif

    return buf;
}

uint8 *
shr_bfd_msg_ctrl_sess_set_unpack(uint8 *buf,
                                 shr_bfd_msg_ctrl_sess_set_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->sess_id);             
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U8(buf, msg->passive);
    _SHR_UNPACK_U8(buf, msg->local_demand);
    _SHR_UNPACK_U8(buf, msg->local_diag);
    _SHR_UNPACK_U8(buf, msg->local_detect_mult);
    _SHR_UNPACK_U32(buf, msg->local_discriminator);
    _SHR_UNPACK_U32(buf, msg->remote_discriminator);
    _SHR_UNPACK_U32(buf, msg->local_min_tx);
    _SHR_UNPACK_U32(buf, msg->local_min_rx);
    _SHR_UNPACK_U32(buf, msg->local_min_echo_rx);
    _SHR_UNPACK_U8(buf, msg->auth_type);
    _SHR_UNPACK_U32(buf, msg->auth_key);
    _SHR_UNPACK_U32(buf, msg->xmt_auth_seq);
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
#ifdef SDK_45223_DISABLED
    _SHR_UNPACK_U32(buf, msg->remote_mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->remote_mep_id[i]);
    }
#endif

    return buf;
}

uint8 *
shr_bfd_msg_ctrl_sess_get_pack(uint8 *buf,
                               shr_bfd_msg_ctrl_sess_get_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U8(buf, msg->enable);
    _SHR_PACK_U8(buf, msg->passive);
    _SHR_PACK_U8(buf, msg->local_demand);
    _SHR_PACK_U8(buf, msg->remote_demand);
    _SHR_PACK_U8(buf, msg->local_diag);
    _SHR_PACK_U8(buf, msg->remote_diag);
    _SHR_PACK_U8(buf, msg->local_sess_state);
    _SHR_PACK_U8(buf, msg->remote_sess_state);
    _SHR_PACK_U8(buf, msg->local_detect_mult);
    _SHR_PACK_U8(buf, msg->remote_detect_mult);
    _SHR_PACK_U32(buf, msg->local_discriminator);
    _SHR_PACK_U32(buf, msg->remote_discriminator); 
    _SHR_PACK_U32(buf, msg->local_min_tx);
    _SHR_PACK_U32(buf, msg->remote_min_tx);
    _SHR_PACK_U32(buf, msg->local_min_rx);
    _SHR_PACK_U32(buf, msg->remote_min_rx);
    _SHR_PACK_U32(buf, msg->local_min_echo_rx);
    _SHR_PACK_U32(buf, msg->remote_min_echo_rx);
    _SHR_PACK_U8(buf, msg->auth_type);
    _SHR_PACK_U32(buf, msg->auth_key);
    _SHR_PACK_U32(buf, msg->xmt_auth_seq);
    _SHR_PACK_U32(buf, msg->rcv_auth_seq);
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
#ifdef SDK_45223_DISABLED
    _SHR_PACK_U32(buf, msg->remote_mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->remote_mep_id[i]);
    }
    _SHR_PACK_U32(buf, msg->mis_conn_mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->mis_conn_mep_id[i]);
    }
#endif

    return buf;
}

uint8 *
shr_bfd_msg_ctrl_sess_get_unpack(uint8 *buf,
                                 shr_bfd_msg_ctrl_sess_get_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U8(buf, msg->enable);
    _SHR_UNPACK_U8(buf, msg->passive);
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

    return buf;
}

uint8 *
shr_bfd_msg_ctrl_auth_sha1_pack(uint8 *buf,
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
shr_bfd_msg_ctrl_auth_sha1_unpack(uint8 *buf,
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
shr_bfd_msg_ctrl_auth_sp_pack(uint8 *buf, shr_bfd_msg_ctrl_auth_sp_t *msg)
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
shr_bfd_msg_ctrl_auth_sp_unpack(uint8 *buf, shr_bfd_msg_ctrl_auth_sp_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->index);
    _SHR_UNPACK_U8(buf, msg->length);
    for (i = 0; i < _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->password[i]);
    }

    return buf;
}

uint8 *
shr_bfd_msg_ctrl_stat_req_pack(uint8 *buf,
                               shr_bfd_msg_ctrl_stat_req_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->clear);

    return buf;
}

uint8 *
shr_bfd_msg_ctrl_stat_req_unpack(uint8 *buf,
                                 shr_bfd_msg_ctrl_stat_req_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->clear);

    return buf;
}

uint8 *
shr_bfd_msg_ctrl_stat_reply_pack(uint8 *buf,
                                 shr_bfd_msg_ctrl_stat_reply_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->packets_in);
    _SHR_PACK_U32(buf, msg->packets_out);
    _SHR_PACK_U32(buf, msg->packets_drop);
    _SHR_PACK_U32(buf, msg->packets_auth_drop);

    return buf;
}

uint8 *
shr_bfd_msg_ctrl_stat_reply_unpack(uint8 *buf,
                                   shr_bfd_msg_ctrl_stat_reply_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->packets_in);
    _SHR_UNPACK_U32(buf, msg->packets_out);
    _SHR_UNPACK_U32(buf, msg->packets_drop);
    _SHR_UNPACK_U32(buf, msg->packets_auth_drop);

    return buf;
}


/*********************************************************
 * BFD Network Packet Header Pack/Unpack
 *
 * Functions:
 *      shr_bfd_<header>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      network packet header to/from given buffer.
 * Parameters:
 *   _pack()
 *      buffer   - (OUT) Buffer where to pack header.
 *      <header> - (IN)  Header to pack.
 *   _unpack()
 *      buffer   - (IN)  Buffer with data to unpack.
 *      <header> - (OUT) Returns unpack header.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_bfd_header_pack(uint8 *buf, shr_bfd_header_t *bfd)
{
    uint32 *word;

    word = (uint32 *)bfd;

    _SHR_PACK_U32(buf, word[0]);
    _SHR_PACK_U32(buf, bfd->my_discriminator);
    _SHR_PACK_U32(buf, bfd->your_discriminator);
    _SHR_PACK_U32(buf, bfd->desired_min_tx_interval);
    _SHR_PACK_U32(buf, bfd->required_min_rx_interval);
    _SHR_PACK_U32(buf, bfd->required_min_echo_rx_interval);

    return buf;
}

uint8 *
shr_bfd_header_unpack(uint8 *buf, shr_bfd_header_t *bfd)
{
    uint32 *word;

    word = (uint32 *)bfd;

    _SHR_UNPACK_U32(buf, word[0]);
    _SHR_UNPACK_U32(buf, bfd->my_discriminator);
    _SHR_UNPACK_U32(buf, bfd->your_discriminator);
    _SHR_UNPACK_U32(buf, bfd->desired_min_tx_interval);
    _SHR_UNPACK_U32(buf, bfd->required_min_rx_interval);
    _SHR_UNPACK_U32(buf, bfd->required_min_echo_rx_interval);

    return buf;
}

uint8 *
shr_bfd_auth_header_pack(uint8 *buf, shr_bfd_auth_header_t *auth)
{
    int i;

    _SHR_PACK_U8(buf, auth->type);
    _SHR_PACK_U8(buf, auth->length);

    if (auth->type == _SHR_BFD_AUTH_TYPE_SIMPLE_PASSWORD) {
        _SHR_PACK_U8(buf, auth->data.sp.key_id);
        for (i = 0;
             i < (auth->length - SHR_BFD_AUTH_SP_HEADER_START_LENGTH); i++) {
            _SHR_PACK_U8(buf, auth->data.sp.password[i]);
        }

    } else if ((auth->type == _SHR_BFD_AUTH_TYPE_KEYED_SHA1) ||
               (auth->type == _SHR_BFD_AUTH_TYPE_METICULOUS_KEYED_SHA1)) {
        _SHR_PACK_U8(buf, auth->data.sha1.key_id);
        _SHR_PACK_U8(buf, auth->data.sha1.reserved);
        _SHR_PACK_U32(buf, auth->data.sha1.sequence);
        for (i = 0; i < _SHR_BFD_AUTH_SHA1_KEY_LENGTH; i++) {
            _SHR_PACK_U8(buf, auth->data.sha1.key[i]);
        }
    }

    return buf;
}

uint8 *
shr_bfd_auth_header_unpack(uint8 *buf, shr_bfd_auth_header_t *auth)
{
    int i;

    _SHR_UNPACK_U8(buf, auth->type);
    _SHR_UNPACK_U8(buf, auth->length);

    if (auth->type == _SHR_BFD_AUTH_TYPE_SIMPLE_PASSWORD) {
        _SHR_UNPACK_U8(buf, auth->data.sp.key_id);
        for (i = 0;
             i < (auth->length - SHR_BFD_AUTH_SP_HEADER_START_LENGTH); i++) {
            _SHR_UNPACK_U8(buf, auth->data.sp.password[i]);
        }

    } else if ((auth->type == _SHR_BFD_AUTH_TYPE_KEYED_SHA1) ||
               (auth->type == _SHR_BFD_AUTH_TYPE_METICULOUS_KEYED_SHA1)) {
        _SHR_UNPACK_U8(buf, auth->data.sha1.key_id);
        _SHR_UNPACK_U8(buf, auth->data.sha1.reserved);
        _SHR_UNPACK_U32(buf, auth->data.sha1.sequence);
        for (i = 0; i < _SHR_BFD_AUTH_SHA1_KEY_LENGTH; i++) {
            _SHR_UNPACK_U8(buf, auth->data.sha1.key[i]);
        }
    }

    return buf;
}

