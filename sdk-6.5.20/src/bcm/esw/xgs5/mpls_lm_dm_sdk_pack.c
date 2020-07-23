/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls_lm_dm_sdk_pack.c
 */
#if defined(INCLUDE_MPLS_LM_DM)
#include <bcm/types.h>
#include <shared/pack.h>
#include <bcm_int/esw/mpls_lm_dm_feature.h>
#include <bcm_int/esw/mpls_lm_dm_sdk_msg.h>

extern uint32 mpls_lm_dm_firmware_version;

uint8 *
mpls_lm_dm_sdk_msg_ctrl_init_pack(uint8 *buf, mpls_lm_dm_sdk_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    if (mpls_lm_dm_firmware_version > MPLS_LM_DM_UC_MIN_VERSION) {
        _SHR_PACK_U32(buf, msg->flags);
    }
    return buf;
}

uint8 *
mpls_lm_dm_sdk_msg_ctrl_loss_add_pack(uint8 *buf,
                                      mpls_lm_dm_sdk_msg_ctrl_loss_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);

    _SHR_PACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_PACK_U8 (buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<MPLS_LM_DM_OLP_HDR_LEN; i++) {
        _SHR_PACK_U8 (buf, msg->olp_encap_data[i]);
    }

    _SHR_PACK_U8 (buf, msg->ctr_size);
    for (i=0; i<msg->ctr_size; i++) {
        _SHR_PACK_U32(buf, msg->ctr_base_id[i]);
        _SHR_PACK_U8 (buf, msg->ctr_offset[i]);
        _SHR_PACK_U8 (buf, msg->ctr_action[i]);
    }
    if (msg->flags & SHR_MPLS_LM_DM_BYTE_COUNT_FLAG) {
        for (i=0; i<msg->ctr_size; i++) {
            _SHR_PACK_U8 (buf, msg->ctr_byte_offset[i]);
        }
    }

    if (MPLS_LM_DM_UC_FEATURE_CHECK(MPLS_LM_DM_FLEX_CTR_SESS_ID)) {
        _SHR_PACK_U32(buf, msg->session_id);
        _SHR_PACK_U8(buf, msg->session_num_entries);
    }

    return buf;
}

uint8 *
mpls_lm_dm_sdk_msg_ctrl_delay_add_pack(uint8 *buf,
                                       mpls_lm_dm_sdk_msg_ctrl_delay_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8(buf, msg->pkt_pri);
    _SHR_PACK_U8(buf, msg->dm_format);

    _SHR_PACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_PACK_U8(buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<MPLS_LM_DM_OLP_HDR_LEN; i++) {
        _SHR_PACK_U8(buf, msg->olp_encap_data[i]);
    }

    if (MPLS_LM_DM_UC_FEATURE_CHECK(MPLS_LM_DM_FLEX_CTR_SESS_ID)) {
        _SHR_PACK_U32(buf, msg->session_id);
        _SHR_PACK_U8(buf, msg->session_num_entries);
    }

    return buf;
}

uint8 *
mpls_lm_dm_sdk_msg_ctrl_delay_data_unpack(uint8 *buf,
        mpls_lm_dm_sdk_msg_ctrl_delay_data_t *msg)
{
    int i;
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->delay_seconds);
    _SHR_UNPACK_U32(buf, msg->delay_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->txf_seconds);
    _SHR_UNPACK_U32(buf, msg->txf_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rxf_seconds);
    _SHR_UNPACK_U32(buf, msg->rxf_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->txb_seconds);
    _SHR_UNPACK_U32(buf, msg->txb_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rxb_seconds);
    _SHR_UNPACK_U32(buf, msg->rxb_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->tx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);
    _SHR_UNPACK_U8 (buf, msg->dm_format);
    
    if (MPLS_LM_DM_UC_FEATURE_CHECK(MPLS_LM_DM_COUNTER_INFO_GET)) {
        _SHR_UNPACK_U32(buf, msg->olp_tx_hdr_length);

        for (i = 0; i < msg->olp_tx_hdr_length; i++) {
            _SHR_UNPACK_U8(buf, msg->olp_tx_hdr[i]);
        }
    }
    return buf;
}

#else /* INCLUDE_MPLS_LM_DM */
typedef int bcm_xgs5_mpls_lm_dm_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_MPLS_LM_DM */

