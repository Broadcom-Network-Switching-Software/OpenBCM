/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        eth_lm_dm_pack.c
 * Purpose:     ETH_LM_DM pack and unpack routines for:
 *              - ETH_LM_DM Control messages
 *              - Network Packet headers (PDUs)
 *
 *
 * ETH_LM_DM control messages
 *
 * ETH_LM_DM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The ETH_LM_DM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/defs.h>
#include <soc/shared/olp_pkt.h>
#include <soc/shared/olp_pack.h>
#include <bcm_int/esw/eth_lm_dm.h>
#include <soc/shared/eth_lm_dm_msg.h>
#include <soc/shared/eth_lm_dm_pkt.h>
#include <soc/shared/eth_lm_dm_pack.h>
#include <bcm_int/esw/eth_lm_dm.h>


/***********************************************************
 * ETH_LM_DM Control Message Pack/Unpack
 *
 * Functions:
 *      shr_eth_lm_dm_msg_ctrl_<type>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      ETH_LM_DM control message into/from a given buffer
 * Parameters:
 *   _pack()
 *      buffer  - (OUT) Buffer where to pack message.
 *      msg     - (IN)  ETH_LM_DM control message to pack.
 *   _unpack()
 *      buffer  - (IN)  Buffer with message to unpack.
 *      msg     - (OUT) Returns ETH_LM_DM control message.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_eth_lm_dm_msg_ctrl_init_pack(uint8 *buf, shr_eth_lm_dm_msg_ctrl_init_t *msg)
{
   

    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_init_unpack(uint8 *buf, shr_eth_lm_dm_msg_ctrl_init_t *msg)
{
    

    _SHR_UNPACK_U32(buf, msg->num_sessions);
    _SHR_UNPACK_U32(buf, msg->rx_channel);
    return buf;
}


uint8 *
shr_eth_lm_dm_msg_ctrl_sess_delete_pack(uint8 *buf, shr_eth_lm_dm_msg_ctrl_sess_delete_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_sess_delete_unpack(uint8 *buf, shr_eth_lm_dm_msg_ctrl_sess_delete_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    return buf;
}

    uint8 *
shr_eth_lm_dm_msg_ctrl_sess_set_pack(uint8 *buf,
        shr_eth_lm_dm_msg_ctrl_sess_set_t *msg)
{
    uint16 i;
    uint32 l2_encap_length;

    /* Pack OLP and ETH-L2 in the same buffer. OLP Tx will be put into the Tx
     * Buffer in between these two L2 encap into the Tx_pkt kept in message
     * control structure while loss/delay add
     */
    l2_encap_length = msg->eth_l2_encap_length + sizeof(soc_olp_l2_hdr_t);

    _SHR_PACK_U32(buf, msg->sess_id);             
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U8(buf, msg->passive);
    _SHR_PACK_U8(buf, msg->mel);
    _SHR_PACK_U16(buf, msg->mep_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->eth_l2_encap_length);

    l2_encap_length = msg->eth_l2_encap_length + sizeof(soc_olp_l2_hdr_t);

    for (i = 0; i < l2_encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap_data[i]);
    }
    _SHR_PACK_U32(buf, msg->tx_port);

    _SHR_PACK_U32(buf, msg->lm_counter_index);
    _SHR_PACK_U16(buf, msg->vlan_id);
    _SHR_PACK_U16(buf, msg->outer_vlan_id);
    _SHR_PACK_U8(buf, msg->mep_type);
    _SHR_PACK_U16(buf, msg->group_id);

    return buf;
}

    uint8 *
shr_eth_lm_dm_msg_ctrl_sess_set_unpack(uint8 *buf,
        shr_eth_lm_dm_msg_ctrl_sess_set_t *msg)
{
    uint16 i;
    uint32 l2_encap_length;

    /* UnPack OLP and ETH-L2 in the same buffer. OLP Tx will be retrieved as
     * part of loss/delay get
     */

    _SHR_UNPACK_U32(buf, msg->sess_id);             
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U8(buf, msg->passive);
    _SHR_UNPACK_U8(buf, msg->mel);
    _SHR_UNPACK_U16(buf, msg->mep_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->eth_l2_encap_length);

    l2_encap_length = msg->eth_l2_encap_length + sizeof(soc_olp_l2_hdr_t);

    for (i = 0; i < l2_encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap_data[i]);
    }

    _SHR_UNPACK_U32(buf, msg->tx_port);

    _SHR_UNPACK_U32(buf, msg->lm_counter_index);
    _SHR_UNPACK_U16(buf, msg->vlan_id);
    _SHR_UNPACK_U16(buf, msg->outer_vlan_id);
    _SHR_UNPACK_U8(buf, msg->mep_type);
    _SHR_UNPACK_U16(buf, msg->group_id);
    return buf;
}



uint8 *
shr_eth_lm_dm_msg_ctrl_sess_get_pack(uint8 *buf,
                               shr_eth_lm_dm_msg_ctrl_sess_get_t *msg)
{
    int i;
    uint32 l2_encap_length;

    /* Pack OLP and ETH-L2 in the same buffer. OLP Tx will be put into the Tx
     * Buffer in between these two L2 encap into the Tx_pkt kept in message
     * control structure while loss/delay add
         */

    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U8(buf, msg->enable);
    _SHR_PACK_U8(buf, msg->passive);
    _SHR_PACK_U8(buf, msg->mel);
    _SHR_PACK_U16(buf, msg->mep_id);
    _SHR_PACK_U32(buf, msg->period);

    _SHR_PACK_U32(buf, msg->eth_l2_encap_length);
    l2_encap_length = msg->eth_l2_encap_length + sizeof(soc_olp_l2_hdr_t);

    for (i = 0; i < l2_encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap_data[i]);
    }

    _SHR_PACK_U32(buf, msg->tx_port);

	_SHR_PACK_U32(buf, msg->lm_counter_index);
	_SHR_PACK_U16(buf, msg->vlan_id);
	_SHR_PACK_U16(buf, msg->outer_vlan_id);
	_SHR_PACK_U8(buf, msg->mep_type);
	_SHR_PACK_U16(buf, msg->group_id);

    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_sess_get_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_sess_get_t *msg)
{
    int i;
    uint32 l2_encap_length;

    /* UnPack OLP and ETH-L2 in the same buffer. OLP Tx will be retrieved as
     * part of loss/delay get
     */

    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U8(buf, msg->enable);
    _SHR_UNPACK_U8(buf, msg->passive); 
    _SHR_UNPACK_U8(buf, msg->mel);
    _SHR_UNPACK_U16(buf, msg->mep_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->eth_l2_encap_length);
    l2_encap_length = msg->eth_l2_encap_length + sizeof(soc_olp_l2_hdr_t);

    for (i = 0; i < l2_encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap_data[i]);
    }

    _SHR_UNPACK_U32(buf, msg->tx_port);
    
    _SHR_UNPACK_U32(buf, msg->lm_counter_index);
	_SHR_UNPACK_U16(buf, msg->vlan_id);
	_SHR_UNPACK_U16(buf, msg->outer_vlan_id);
	_SHR_UNPACK_U8(buf, msg->mep_type);
	_SHR_UNPACK_U16(buf, msg->group_id);

    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_stat_req_pack(uint8 *buf,
                               shr_eth_lm_dm_msg_ctrl_stat_req_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->clear);

    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_stat_req_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_stat_req_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->clear);

    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_stat_reply_pack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_stat_reply_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->packets_in);
    _SHR_PACK_U32(buf, msg->packets_out);
    _SHR_PACK_U32(buf, msg->packets_drop);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_stat_reply_unpack(uint8 *buf,
                                   shr_eth_lm_dm_msg_ctrl_stat_reply_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->packets_in);
    _SHR_UNPACK_U32(buf, msg->packets_out);
    _SHR_UNPACK_U32(buf, msg->packets_drop);


    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_loss_add_pack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_loss_add_t *msg)
{
	int i;
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->tx_cos);
    _SHR_PACK_U8 (buf, msg->tx_pri);
    _SHR_PACK_U32(buf, msg->tx_qnum);
	for (i = 0; i < _ETH_LM_DM_MAC_ADDR_LENGTH; i++) {
		_SHR_PACK_U8(buf, msg->dst_mac[i]);
	}
    for (i = 0; i < SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH; i++) {
		_SHR_PACK_U8(buf, msg->olp_tx_hdr[i]);
	}
    _SHR_PACK_U32(buf, msg->olp_tx_hdr_length);
    _SHR_PACK_U32(buf, msg->remote_mep_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_loss_add_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_loss_add_t *msg)
{

	int i;
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U8 (buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
	for (i = 0; i < _ETH_LM_DM_MAC_ADDR_LENGTH; i++) {
		_SHR_UNPACK_U8(buf, msg->dst_mac[i]);
	}   
    for (i = 0; i < SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH; i++) {
		_SHR_UNPACK_U8(buf, msg->olp_tx_hdr[i]);
	}   
    _SHR_UNPACK_U32(buf, msg->olp_tx_hdr_length);
    _SHR_UNPACK_U32(buf, msg->remote_mep_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_loss_delete_pack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_loss_delete_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_loss_delete_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_loss_delete_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_loss_get_pack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_loss_get_t *msg)
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
    _SHR_PACK_U32(buf, msg->tx_cos);
    _SHR_PACK_U8 (buf, msg->tx_pri);
    _SHR_PACK_U32(buf, msg->tx_qnum);
	for (i = 0; i < _ETH_LM_DM_MAC_ADDR_LENGTH; i++) {
		_SHR_PACK_U8(buf, msg->dst_mac[i]);
	}
    for (i = 0; i < SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH; i++) {
		_SHR_PACK_U8(buf, msg->olp_tx_hdr[i]);
	}
    _SHR_PACK_U32(buf, msg->olp_tx_hdr_length);
    _SHR_PACK_U32(buf, msg->remote_mep_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_loss_get_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_loss_get_t *msg)
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
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U8 (buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
	for (i = 0; i < _ETH_LM_DM_MAC_ADDR_LENGTH; i++) {
		_SHR_UNPACK_U8(buf, msg->dst_mac[i]);
	}
    for (i = 0; i < SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH; i++) {
		_SHR_UNPACK_U8(buf, msg->olp_tx_hdr[i]);
	}
    _SHR_UNPACK_U32(buf, msg->olp_tx_hdr_length);
    _SHR_UNPACK_U32(buf, msg->remote_mep_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_delay_add_pack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_delay_add_t *msg)
{
	int i;
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->tx_cos);
    _SHR_PACK_U8 (buf, msg->tx_pri);
    _SHR_PACK_U32(buf, msg->tx_qnum);
    _SHR_PACK_U8 (buf, msg->dm_format); 
	for (i = 0; i < _ETH_LM_DM_MAC_ADDR_LENGTH; i++) {
		_SHR_PACK_U8(buf, msg->dst_mac[i]);
	}
    for (i = 0; i < SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH; i++) {
		_SHR_PACK_U8(buf, msg->olp_tx_hdr[i]);
	}
    _SHR_PACK_U32(buf, msg->olp_tx_hdr_length);
    _SHR_PACK_U32(buf, msg->remote_mep_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_delay_add_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_delay_add_t *msg)
{
    int i;
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U8 (buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
    _SHR_UNPACK_U8 (buf, msg->dm_format);
	for (i = 0; i < _ETH_LM_DM_MAC_ADDR_LENGTH; i++) {
		_SHR_UNPACK_U8(buf, msg->dst_mac[i]);
	}
    for (i = 0; i < SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH; i++) {
		_SHR_UNPACK_U8(buf, msg->olp_tx_hdr[i]);
	}
    _SHR_UNPACK_U32(buf, msg->olp_tx_hdr_length);
    _SHR_UNPACK_U32(buf, msg->remote_mep_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_delay_delete_pack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_delay_delete_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_delay_delete_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_delay_delete_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_delay_get_pack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_delay_get_t *msg)
{
	int i;
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
    _SHR_PACK_U32(buf, msg->tx_cos);
    _SHR_PACK_U8 (buf, msg->tx_pri);
    _SHR_PACK_U32(buf, msg->tx_qnum);
    _SHR_PACK_U8 (buf, msg->dm_format);
	for (i = 0; i < _ETH_LM_DM_MAC_ADDR_LENGTH; i++) {
		_SHR_PACK_U8(buf, msg->dst_mac[i]);
	}
    for (i = 0; i < SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH; i++) {
		_SHR_PACK_U8(buf, msg->olp_tx_hdr[i]);
	}
    _SHR_PACK_U32(buf, msg->olp_tx_hdr_length);
    _SHR_PACK_U32(buf, msg->remote_mep_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_msg_ctrl_delay_get_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_delay_get_t *msg)
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
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U8 (buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
    _SHR_UNPACK_U8 (buf, msg->dm_format);
	for (i = 0; i < _ETH_LM_DM_MAC_ADDR_LENGTH; i++) {
		_SHR_UNPACK_U8(buf, msg->dst_mac[i]);
	}
    for (i = 0; i < SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH; i++) {
		_SHR_UNPACK_U8(buf, msg->olp_tx_hdr[i]);
	}
    _SHR_UNPACK_U32(buf, msg->olp_tx_hdr_length);
    _SHR_UNPACK_U32(buf, msg->remote_mep_id);
    return buf;
}

/*********************************************************
 * ETH_LM_DM Network Packet Header Pack/Unpack
 *
 * Functions:
 *      shr_eth_lm_dm_<header>_pack/unpack
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
shr_eth_lm_dm_header_pack(uint8 *buf, shr_eth_lm_dm_header_t *eth_lm_dm)
{
    uint32 *ptr;

    ptr = (uint32 *)eth_lm_dm;

    _SHR_PACK_U32(buf, ptr[0]);
#if 0
    _SHR_PACK_U8(buf, eth_lm_dm->opcode);
    _SHR_PACK_U8(buf, eth_lm_dm->flags);
    _SHR_PACK_U8(buf, eth_lm_dm->tlv_offset);
#endif
    return buf;
}

uint8 *
shr_eth_lm_dm_header_unpack(uint8 *buf, shr_eth_lm_dm_header_t *eth_lm_dm)
{
    uint32 *ptr;

    ptr = (uint32 *)eth_lm_dm;

    _SHR_UNPACK_U32(buf, ptr[0]);
#if 0
    uint8 *byte;

    byte = (uint8 *)eth_lm_dm;

    _SHR_UNPACK_U8(buf, byte[0]);
    _SHR_UNPACK_U8(buf, eth_lm_dm->opcode);
    _SHR_UNPACK_U8(buf, eth_lm_dm->flags);
    _SHR_UNPACK_U8(buf, eth_lm_dm->tlv_offset);
#endif
    return buf;
}

/*********************************************************
 * ETH_LM_DM Network Packet SLM info Header Pack/Unpack
 *
 * Functions:
 *      shr_eth_lm_dm_<header>_pack/unpack
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
shr_eth_lm_dm_slm_info_header_pack(uint8 *buf, shr_eth_lm_dm_slm_info_header_t *eth_lm_dm_slm_info)
{
	_SHR_PACK_U16(buf, eth_lm_dm_slm_info->src_mep_id);
	_SHR_PACK_U16(buf, eth_lm_dm_slm_info->resp_mep_id);
	_SHR_PACK_U32(buf, eth_lm_dm_slm_info->test_id);
    return buf;
}

uint8 *
shr_eth_lm_dm_slm_info_header_unpack(uint8 *buf, shr_eth_lm_dm_slm_info_header_t *eth_lm_dm_slm_info)
{


	_SHR_UNPACK_U16(buf, eth_lm_dm_slm_info->src_mep_id);
	_SHR_UNPACK_U16(buf, eth_lm_dm_slm_info->resp_mep_id);
	_SHR_UNPACK_U32(buf, eth_lm_dm_slm_info->test_id);

    return buf;
}

