/*
 * $Id: eth_oam_pack.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        eth_oam_pack.c
 * Purpose:     ETH-OAM pack and unpack routines for:
 *              - ETH-OAM Control messages
 *
 *
 * ETH-OAM control messages
 *
 * ETH-OAM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The ETH-OAM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/eth_oam.h>
#include <soc/shared/eth_oam_msg.h>
#include <soc/shared/eth_oam_pack.h>

uint8*
shr_eth_oam_msg_ctrl_init_pack(uint8 *buf, shr_eth_oam_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);

    _SHR_PACK_U16(buf, msg->max_lmeps);
    _SHR_PACK_U16(buf, msg->max_rmeps);
    _SHR_PACK_U16(buf, msg->max_ccm_sessions);
    _SHR_PACK_U16(buf, msg->max_slm_sessions);

    _SHR_PACK_U8(buf, msg->pm_mode);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_init_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_init_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);

    _SHR_UNPACK_U16(buf, msg->max_lmeps);
    _SHR_UNPACK_U16(buf, msg->max_rmeps);
    _SHR_UNPACK_U16(buf, msg->max_ccm_sessions);
    _SHR_UNPACK_U16(buf, msg->max_slm_sessions);

    _SHR_UNPACK_U8(buf, msg->pm_mode);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_lmep_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_set_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->update_flags);

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U16(buf, msg->port);
    _SHR_PACK_U16(buf, msg->svlan);
    _SHR_PACK_U16(buf, msg->cvlan);
    _SHR_PACK_U16(buf, msg->mep_id);
    _SHR_PACK_U16(buf, msg->ccm_offset);
    _SHR_PACK_U16(buf, msg->ccm_pkt_length);

    _SHR_PACK_U8(buf, msg->mdl);
    _SHR_PACK_U8(buf, msg->type);
    _SHR_PACK_U8(buf, msg->sticky_faults_clear);
    for (i = 0; i < SHR_ETH_OAM_MEG_ID_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->meg_id[i]);
    }
    for (i = 0; i < msg->ccm_pkt_length; i++) {
        _SHR_PACK_U8(buf, msg->ccm_pkt[i]);
    }
    _SHR_PACK_U8(buf, msg->ccm_period);
    _SHR_PACK_U8(buf, msg->alarm_mask);
    _SHR_PACK_U8(buf, msg->sw_rdi);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_lmep_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_set_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->update_flags);

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U16(buf, msg->port);
    _SHR_UNPACK_U16(buf, msg->svlan);
    _SHR_UNPACK_U16(buf, msg->cvlan);
    _SHR_UNPACK_U16(buf, msg->mep_id);
    _SHR_UNPACK_U16(buf, msg->ccm_offset);
    _SHR_UNPACK_U16(buf, msg->ccm_pkt_length);

    _SHR_UNPACK_U8(buf, msg->mdl);
    _SHR_UNPACK_U8(buf, msg->type);
    _SHR_UNPACK_U8(buf, msg->sticky_faults_clear);
    for (i = 0; i < SHR_ETH_OAM_MEG_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->meg_id[i]);
    }
    for (i = 0; i < msg->ccm_pkt_length; i++) {
        _SHR_UNPACK_U8(buf, msg->ccm_pkt[i]);
    }
    _SHR_UNPACK_U8(buf, msg->ccm_period);
    _SHR_UNPACK_U8(buf, msg->alarm_mask);
    _SHR_UNPACK_U8(buf, msg->sw_rdi);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_lmep_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_get_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);

    _SHR_PACK_U8(buf, msg->faults);
    _SHR_PACK_U8(buf, msg->sticky_faults);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_lmep_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_get_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);

    _SHR_UNPACK_U8(buf, msg->faults);
    _SHR_UNPACK_U8(buf, msg->sticky_faults);

    return buf;
}


uint8*
shr_eth_oam_msg_ctrl_lmep_del_pack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_del_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U16(buf, msg->port);
    _SHR_PACK_U16(buf, msg->svlan);
    _SHR_PACK_U16(buf, msg->cvlan);

    _SHR_PACK_U8(buf, msg->mdl);
    _SHR_PACK_U8(buf, msg->type);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_lmep_del_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_del_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U16(buf, msg->port);
    _SHR_UNPACK_U16(buf, msg->svlan);
    _SHR_UNPACK_U16(buf, msg->cvlan);

    _SHR_UNPACK_U8(buf, msg->mdl);
    _SHR_UNPACK_U8(buf, msg->type);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_rmep_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_rmep_set_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->update_flags);

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U16(buf, msg->lmep_id);
    _SHR_PACK_U16(buf, msg->mep_id);

    _SHR_PACK_U8(buf, msg->ccm_period);
    _SHR_PACK_U8(buf, msg->sticky_faults_clear);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_rmep_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_rmep_set_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->update_flags);

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U16(buf, msg->lmep_id);
    _SHR_UNPACK_U16(buf, msg->mep_id);

    _SHR_UNPACK_U8(buf, msg->ccm_period);
    _SHR_UNPACK_U8(buf, msg->sticky_faults_clear);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_rmep_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_rmep_get_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);

    _SHR_PACK_U8(buf, msg->faults);
    _SHR_PACK_U8(buf, msg->sticky_faults);
    for (i = 0; i < SHR_ETH_OAM_MAC_ADDR_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->src_mac[i]);
    }
    _SHR_PACK_U8(buf, msg->port_status);
    _SHR_PACK_U8(buf, msg->intf_status);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_rmep_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_rmep_get_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);

    _SHR_UNPACK_U8(buf, msg->faults);
    _SHR_UNPACK_U8(buf, msg->sticky_faults);
    for (i = 0; i < SHR_ETH_OAM_MAC_ADDR_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->src_mac[i]);
    }
    _SHR_UNPACK_U8(buf, msg->port_status);
    _SHR_UNPACK_U8(buf, msg->intf_status);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_slm_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_slm_set_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->update_flags);
    _SHR_PACK_U32(buf, msg->test_id);
    _SHR_PACK_U32(buf, msg->measurement_interval_msecs);

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U16(buf, msg->rmep_id);
    _SHR_PACK_U16(buf, msg->pkt_length);
    _SHR_PACK_U16(buf, msg->slm_offset);

    for (i = 0; i < msg->pkt_length; i++) {
        _SHR_PACK_U8(buf, msg->pkt[i]);
    }
    _SHR_PACK_U8(buf, msg->period);
    _SHR_PACK_U8(buf, msg->one_shot);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_slm_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_slm_set_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->update_flags);
    _SHR_UNPACK_U32(buf, msg->test_id);
    _SHR_UNPACK_U32(buf, msg->measurement_interval_msecs);

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U16(buf, msg->rmep_id);
    _SHR_UNPACK_U16(buf, msg->pkt_length);
    _SHR_UNPACK_U16(buf, msg->slm_offset);

    for (i = 0; i < msg->pkt_length; i++) {
        _SHR_UNPACK_U8(buf, msg->pkt[i]);
    }
    _SHR_UNPACK_U8(buf, msg->period);
    _SHR_UNPACK_U8(buf, msg->one_shot);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_slm_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_slm_get_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->tx_near_end);
    _SHR_PACK_U32(buf, msg->rx_far_end);
    _SHR_PACK_U32(buf, msg->tx_far_end);
    _SHR_PACK_U32(buf, msg->rx_near_end);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_slm_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_slm_get_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->tx_near_end);
    _SHR_UNPACK_U32(buf, msg->rx_far_end);
    _SHR_UNPACK_U32(buf, msg->tx_far_end);
    _SHR_UNPACK_U32(buf, msg->rx_near_end);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_trunk_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_trunk_set_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);

    _SHR_PACK_U16(buf, msg->tid);
    _SHR_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->ports[i]);
    }

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_trunk_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_trunk_set_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);

    _SHR_UNPACK_U16(buf, msg->tid);
    _SHR_UNPACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->ports[i]);
    }

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_trunk_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_trunk_get_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);

    _SHR_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->ports[i]);
    }

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_trunk_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_trunk_get_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);

    _SHR_UNPACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->ports[i]);
    }

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_event_masks_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_event_masks_set_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->lmep_mask);
    _SHR_PACK_U32(buf, msg->rmep_mask);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_event_masks_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_event_masks_set_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->lmep_mask);
    _SHR_UNPACK_U32(buf, msg->rmep_mask);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_pm_pstats_slm_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_pm_pstats_slm_get_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->max_flr_loss);
    _SHR_PACK_U32(buf, msg->max_flr_tx);
    _SHR_PACK_U32(buf, msg->min_flr_loss);
    _SHR_PACK_U32(buf, msg->min_flr_tx);
    _SHR_PACK_U32(buf, msg->max_nlr_loss);
    _SHR_PACK_U32(buf, msg->max_nlr_tx);
    _SHR_PACK_U32(buf, msg->min_nlr_loss);
    _SHR_PACK_U32(buf, msg->min_nlr_tx);
    _SHR_PACK_U32(buf, msg->total_far_loss);
    _SHR_PACK_U32(buf, msg->total_near_loss);
    _SHR_PACK_U32(buf, msg->total_tx);
    _SHR_PACK_U32(buf, msg->total_rx);
    _SHR_PACK_U32(buf, msg->num_intervals);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_pm_pstats_slm_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_pm_pstats_slm_get_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->max_flr_loss);
    _SHR_UNPACK_U32(buf, msg->max_flr_tx);
    _SHR_UNPACK_U32(buf, msg->min_flr_loss);
    _SHR_UNPACK_U32(buf, msg->min_flr_tx);
    _SHR_UNPACK_U32(buf, msg->max_nlr_loss);
    _SHR_UNPACK_U32(buf, msg->max_nlr_tx);
    _SHR_UNPACK_U32(buf, msg->min_nlr_loss);
    _SHR_UNPACK_U32(buf, msg->min_nlr_tx);
    _SHR_UNPACK_U32(buf, msg->total_far_loss);
    _SHR_UNPACK_U32(buf, msg->total_near_loss);
    _SHR_UNPACK_U32(buf, msg->total_tx);
    _SHR_UNPACK_U32(buf, msg->total_rx);
    _SHR_UNPACK_U32(buf, msg->num_intervals);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_pm_rstats_addr_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_pm_rstats_addr_set_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->addr);

    _SHR_PACK_U8(buf, msg->enable);

    return buf;
}

uint8*
shr_eth_oam_msg_ctrl_pm_rstats_addr_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_pm_rstats_addr_set_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->addr);

    _SHR_UNPACK_U8(buf, msg->enable);

    return buf;
}
