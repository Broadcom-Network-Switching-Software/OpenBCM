/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mpls_lm_dm_pack.c
 * Purpose:     MPLS_LM_DM pack and unpack routines for:
 *              - MPLS_LM_DM Control messages
 *              - Network Packet headers (PDUs)
 *
 *
 * MPLS_LM_DM control messages
 *
 * MPLS_LM_DM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The MPLS_LM_DM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/mpls_lm_dm_msg.h>
#include <soc/shared/mpls_lm_dm_pkt.h>
#include <soc/shared/mpls_lm_dm_pack.h>


/***********************************************************
 * MPLS_LM_DM Control Message Pack/Unpack
 *
 * Functions:
 *      shr_mpls_lm_dm_msg_ctrl_<type>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      MPLS_LM_DM control message into/from a given buffer
 * Parameters:
 *   _pack()
 *      buffer  - (OUT) Buffer where to pack message.
 *      msg     - (IN)  MPLS_LM_DM control message to pack.
 *   _unpack()
 *      buffer  - (IN)  Buffer with message to unpack.
 *      msg     - (OUT) Returns MPLS_LM_DM control message.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_mpls_lm_dm_msg_ctrl_init_pack(uint8 *buf, shr_mpls_lm_dm_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_init_unpack(uint8 *buf, shr_mpls_lm_dm_msg_ctrl_init_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->num_sessions);
    _SHR_UNPACK_U32(buf, msg->rx_channel);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_add_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_add_t *msg)
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
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_add_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_add_t *msg)
{
    int i;
    
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);

    _SHR_UNPACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_UNPACK_U8 (buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<(MPLS_LM_DM_OLP_HDR_LEN); i++) {
        _SHR_UNPACK_U8 (buf, msg->olp_encap_data[i]);
    }

    _SHR_UNPACK_U8 (buf, msg->ctr_size);
    for (i=0; i<msg->ctr_size; i++) {
        _SHR_UNPACK_U32(buf, msg->ctr_base_id[i]);
        _SHR_UNPACK_U8 (buf, msg->ctr_offset[i]);
        _SHR_UNPACK_U8 (buf, msg->ctr_action[i]);
    }
    if (msg->flags & SHR_MPLS_LM_DM_BYTE_COUNT_FLAG) {
        for (i=0; i<msg->ctr_size; i++) {
            _SHR_UNPACK_U8 (buf, msg->ctr_byte_offset[i]);
        }
    }
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_delete_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_delete_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_delete_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_delete_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_get_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_get_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_get_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_get_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_data_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_data_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->loss_threshold);
    _SHR_PACK_U32(buf, msg->loss_nearend);
    _SHR_PACK_U32(buf, msg->loss_farend);
    _SHR_PACK_U32(buf, msg->tx_nearend);
    _SHR_PACK_U32(buf, msg->rx_nearend);
    _SHR_PACK_U32(buf, msg->tx_farend);
    _SHR_PACK_U32(buf, msg->rx_farend);
    _SHR_PACK_U32(buf, msg->rx_oam_packets);
    _SHR_PACK_U32(buf, msg->tx_oam_packets);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);

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

        if (msg->flags & SHR_MPLS_LM_DM_64_BIT_FLAG) {
            _SHR_PACK_U32(buf, msg->loss_nearend_upper);
            _SHR_PACK_U32(buf, msg->loss_farend_upper);
            _SHR_PACK_U32(buf, msg->tx_nearend_upper);
            _SHR_PACK_U32(buf, msg->rx_nearend_upper);
            _SHR_PACK_U32(buf, msg->tx_farend_upper);
            _SHR_PACK_U32(buf, msg->rx_farend_upper);
        }
    }

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_data_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_data_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->loss_threshold);
    _SHR_UNPACK_U32(buf, msg->loss_nearend);
    _SHR_UNPACK_U32(buf, msg->loss_farend);
    _SHR_UNPACK_U32(buf, msg->tx_nearend);
    _SHR_UNPACK_U32(buf, msg->rx_nearend);
    _SHR_UNPACK_U32(buf, msg->tx_farend);
    _SHR_UNPACK_U32(buf, msg->rx_farend);
    _SHR_UNPACK_U32(buf, msg->rx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->tx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);

    _SHR_UNPACK_U8 (buf, msg->ctr_size);
    for (i=0; i<msg->ctr_size; i++) {
        _SHR_UNPACK_U32(buf, msg->ctr_base_id[i]);
        _SHR_UNPACK_U8 (buf, msg->ctr_offset[i]);
        _SHR_UNPACK_U8 (buf, msg->ctr_action[i]);
    }
    if (msg->flags & SHR_MPLS_LM_DM_BYTE_COUNT_FLAG) {
        for (i=0; i<msg->ctr_size; i++) {
            _SHR_UNPACK_U8 (buf, msg->ctr_byte_offset[i]);
        }

        if (msg->flags & SHR_MPLS_LM_DM_64_BIT_FLAG) {
            _SHR_UNPACK_U32(buf, msg->loss_nearend_upper);
            _SHR_UNPACK_U32(buf, msg->loss_farend_upper);
            _SHR_UNPACK_U32(buf, msg->tx_nearend_upper);
            _SHR_UNPACK_U32(buf, msg->rx_nearend_upper);
            _SHR_UNPACK_U32(buf, msg->tx_farend_upper);
            _SHR_UNPACK_U32(buf, msg->rx_farend_upper);
        }
    }

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_add_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);
    _SHR_PACK_U8 (buf, msg->dm_format);

    _SHR_PACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_PACK_U8 (buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<MPLS_LM_DM_OLP_HDR_LEN; i++) {
        _SHR_PACK_U8 (buf, msg->olp_encap_data[i]);
    }
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_add_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_add_t *msg)
{
    int i;
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);
    _SHR_UNPACK_U8 (buf, msg->dm_format);

    _SHR_UNPACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_UNPACK_U8 (buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<(MPLS_LM_DM_OLP_HDR_LEN); i++) {
        _SHR_UNPACK_U8 (buf, msg->olp_encap_data[i]);
    }
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_delete_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_delete_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_delete_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_delete_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_get_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_get_t *msg)
{

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_get_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_get_t *msg)
{

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_data_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_data_t *msg)
{

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->delay_seconds);
    _SHR_PACK_U32(buf, msg->delay_nanoseconds);
    _SHR_PACK_U32(buf, msg->txf_seconds);
    _SHR_PACK_U32(buf, msg->txf_nanoseconds);
    _SHR_PACK_U32(buf, msg->rxf_seconds);
    _SHR_PACK_U32(buf, msg->rxf_nanoseconds);
    _SHR_PACK_U32(buf, msg->txb_seconds);
    _SHR_PACK_U32(buf, msg->txb_nanoseconds);
    _SHR_PACK_U32(buf, msg->rxb_seconds);
    _SHR_PACK_U32(buf, msg->rxb_nanoseconds);
    _SHR_PACK_U32(buf, msg->rx_oam_packets);
    _SHR_PACK_U32(buf, msg->tx_oam_packets);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);
    _SHR_PACK_U8 (buf, msg->dm_format);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_data_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_data_t *msg)
{

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

    return buf;
}
