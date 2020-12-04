/*
 * $Id: sum_pack.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        sum_pack.c
 * Purpose:     SUM pack and unpack routines for:
 *              - SUM Control messages
 *
 * SUM control messages
 *
 * SUM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The SUM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/sum.h>
#include <soc/shared/sum_msg.h>
#include <soc/shared/sum_pack.h>

/*
 * Create SUM config information - pack
 */
uint8*
shr_sum_msg_ctrl_config_pack(uint8 *buf, shr_sum_msg_ctrl_config_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->host_buf_addr);
    _SHR_PACK_U32(buf, msg->usec_sample_interval);
    _SHR_PACK_U32(buf, msg->max_history);
    _SHR_PACK_U32(buf, msg->sum_type);

    _SHR_PACK_U32(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U32(buf, msg->lports[i]);
        _SHR_PACK_U32(buf, msg->pports[i]);
    }
    _SHR_PACK_U32(buf, msg->mib_tbl_addr);
    _SHR_PACK_U8(buf, msg->mib_tbl_acc_type);
    _SHR_PACK_U8(buf, msg->sum_wb_indicate);

    return buf;
}

/*
 * Create SUM config information - unpack
 */
uint8*
shr_sum_msg_ctrl_config_unpack(uint8 *buf, shr_sum_msg_ctrl_config_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->host_buf_addr);
    _SHR_UNPACK_U32(buf, msg->usec_sample_interval);
    _SHR_UNPACK_U32(buf, msg->max_history);
    _SHR_UNPACK_U32(buf, msg->sum_type);

    _SHR_UNPACK_U32(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U32(buf, msg->lports[i]);
        _SHR_UNPACK_U32(buf, msg->pports[i]);
    }
    _SHR_UNPACK_U32(buf, msg->mib_tbl_addr);
    _SHR_UNPACK_U8(buf, msg->mib_tbl_acc_type);
    _SHR_UNPACK_U8(buf, msg->sum_wb_indicate);

    return buf;
}

/*
 * Create SUM stats config information - pack
 */
uint8*
shr_sum_msg_ctrl_stat_config_pack(uint8 *buf, shr_sum_msg_buf_info_t *msg)
{
    _SHR_PACK_U32(buf, msg->host_buf_cur_offset);
    _SHR_PACK_U32(buf, msg->host_buf_status);
    _SHR_PACK_U32(buf, msg->sample_interval);

    return buf;
}

/*
 * Create SUM stats config information - unpack
 */
uint8*
shr_sum_msg_ctrl_stat_config_unpack(uint8 *buf, shr_sum_msg_buf_info_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->host_buf_cur_offset);
    _SHR_UNPACK_U32(buf, msg->host_buf_status);
    _SHR_UNPACK_U32(buf, msg->sample_interval);

    return buf;
}
