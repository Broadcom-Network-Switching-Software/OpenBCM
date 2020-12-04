/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        oam_pm.c
 * Purpose:     OAM_PM pack and unpack routines for:
 *              - OAM_PM Control messages
 *              - Network Packet headers (PDUs)
 *
 *
 * OAM_PM control messages
 *
 * OAM_PM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The OAM_PM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/oam_pm_pack.h>


uint8 *
shr_oam_pm_msg_ctrl_pm_stats_get_pack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_pm_stats_get_t *msg)
{

    int i;
    _SHR_PACK_U32(buf,msg->flags);
    _SHR_PACK_U32(buf,msg->sess_id);
    _SHR_PACK_U32(buf,msg->far_loss_min);
    _SHR_PACK_U32(buf,msg->far_tx_min);
    _SHR_PACK_U32(buf,msg->far_loss_max);
    _SHR_PACK_U32(buf,msg->far_tx_max);
    _SHR_PACK_U32(buf,msg->far_loss);
    _SHR_PACK_U32(buf,msg->near_loss_min);
    _SHR_PACK_U32(buf,msg->near_tx_min);
    _SHR_PACK_U32(buf,msg->near_loss_max);
    _SHR_PACK_U32(buf,msg->near_tx_max);
    _SHR_PACK_U32(buf,msg->near_loss);
    _SHR_PACK_U32(buf,msg->lm_tx_count);
    _SHR_PACK_U32(buf,msg->DM_min);
    _SHR_PACK_U32(buf,msg->DM_max);
    _SHR_PACK_U32(buf,msg->DM_avg);
    _SHR_PACK_U32(buf,msg->dm_tx_count);
    _SHR_PACK_U8(buf,msg->profile_id);
    for (i = 0; i < (SHR_OAM_PM_MAX_PM_BIN_EDGES+1); i++) {
        _SHR_PACK_U32(buf, msg->bin_counters[i]);
    }

    return buf;
}

uint8 *
shr_oam_pm_msg_ctrl_pm_stats_get_unpack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_pm_stats_get_t *msg,
                                 uint8 pm_stat_extra_elem_feature)
{
    int i;

    _SHR_UNPACK_U32(buf,msg->flags);
    _SHR_UNPACK_U32(buf,msg->sess_id);
    _SHR_UNPACK_U32(buf,msg->far_loss_min);
    _SHR_UNPACK_U32(buf,msg->far_tx_min);
    _SHR_UNPACK_U32(buf,msg->far_loss_max);
    _SHR_UNPACK_U32(buf,msg->far_tx_max);
    _SHR_UNPACK_U32(buf,msg->far_loss);
    _SHR_UNPACK_U32(buf,msg->near_loss_min);
    _SHR_UNPACK_U32(buf,msg->near_tx_min);
    _SHR_UNPACK_U32(buf,msg->near_loss_max);
    _SHR_UNPACK_U32(buf,msg->near_tx_max);
    _SHR_UNPACK_U32(buf,msg->near_loss);
    _SHR_UNPACK_U32(buf,msg->lm_tx_count);
    _SHR_UNPACK_U32(buf,msg->DM_min);
    _SHR_UNPACK_U32(buf,msg->DM_max);
    _SHR_UNPACK_U32(buf,msg->DM_avg);
    _SHR_UNPACK_U32(buf,msg->dm_tx_count);
    _SHR_UNPACK_U8(buf,msg->profile_id);
    for (i = 0; i < (SHR_OAM_PM_MAX_PM_BIN_EDGES+1); i++) {
        _SHR_UNPACK_U32(buf, msg->bin_counters[i]);
    }

    if(pm_stat_extra_elem_feature) {
        _SHR_UNPACK_U32(buf, msg->lm_rx_count);
        _SHR_UNPACK_U32(buf, msg->dm_rx_count);
        _SHR_UNPACK_U32(buf, msg->far_total_tx_pkt_count);
        _SHR_UNPACK_U32(buf, msg->near_total_tx_pkt_count);
    }
    
    return buf;
}

uint8 *
shr_oam_pm_msg_ctrl_profile_attach_pack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_profile_attach_t *msg)
{

    int i;
    _SHR_PACK_U32(buf,msg->sess_id);
    _SHR_PACK_U32(buf,msg->profile_flags);
    _SHR_PACK_U8(buf,msg->profile_id);
    for (i = 0; i < SHR_OAM_PM_MAX_PM_BIN_EDGES; i++) {
        _SHR_PACK_U32(buf, msg->profile_edges[i]);
    }

    return buf;
}

uint8 *
shr_oam_pm_msg_ctrl_profile_attach_unpack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_profile_attach_t *msg)
{
    int i;
    _SHR_UNPACK_U32(buf,msg->sess_id);
    _SHR_UNPACK_U32(buf,msg->profile_flags);
    _SHR_UNPACK_U8(buf,msg->profile_id);
    for (i = 0; i < SHR_OAM_PM_MAX_PM_BIN_EDGES; i++) {
        _SHR_UNPACK_U32(buf, msg->profile_edges[i]);
    }
    
    return buf;
}

uint8 *
shr_oam_pm_msg_ctrl_raw_buffer_unpack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_raw_buffer_t *msg)
{
    int i, j;
    _SHR_UNPACK_U32(buf, msg->seq_num);
    for (i = 0; i < SHR_OAM_MAX_PM_ENDPOINTS; i++) {

        _SHR_UNPACK_U32(buf, msg->raw_sample[i].pm_id);
        _SHR_UNPACK_U32(buf, msg->raw_sample[i].lm_count);
        _SHR_UNPACK_U32(buf, msg->raw_sample[i].dm_count);
        for (j = 0; j < SHR_OAM_PM_RAW_DATA_MAX_SAMPLES; j++) {
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_counter[j].tx_fcf);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_counter[j].rx_fcf);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_counter[j].tx_fcb);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_counter[j].rx_fcb);
        }
        for (j = 0; j < SHR_OAM_PM_RAW_DATA_MAX_SAMPLES; j++) {
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].tx_tsf_secs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].tx_tsf_nsecs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].rx_tsf_secs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].rx_tsf_nsecs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].tx_tsb_secs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].tx_tsb_nsecs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].rx_tsb_secs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].rx_tsb_nsecs);
        }
    }

    return buf;
}
