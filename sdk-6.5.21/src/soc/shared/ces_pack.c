/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ces_pack.c
 * Purpose:     CES pack and unpack routines for CES control messages.
 *
 * CES messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The CES control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */
#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif
#include <shared/pack.h>
#include <soc/shared/mos_msg_ces.h>
#include <soc/shared/ces_pack.h>

uint8 *
bcm_ces_crm_init_msg_pack(uint8 *buf, bcm_ces_crm_init_msg_t *msg)
{
    _SHR_PACK_U8(buf, msg->flags);

    return buf;
}

uint8 *
bcm_ces_crm_init_msg_unpack(uint8 *buf, bcm_ces_crm_init_msg_t *msg)
{
    _SHR_UNPACK_U8(buf, msg->flags);

    return buf;
}

uint8 *
bcm_ces_crm_cclk_config_msg_pack(uint8 *buf, bcm_ces_crm_cclk_config_msg_t *msg)
{
    _SHR_PACK_U8(buf, msg->flags);
    _SHR_PACK_U8(buf, msg->e_cclk_select);
    _SHR_PACK_U8(buf, msg->e_ref_clk_1_select);
    _SHR_PACK_U8(buf, msg->e_ref_clk_2_select);
    _SHR_PACK_U32(buf, msg->n_ref_clk_1_port);
    _SHR_PACK_U32(buf, msg->n_ref_clk_2_port);
    _SHR_PACK_U8(buf, msg->e_protocol);

    return buf;
}

uint8 *
bcm_ces_crm_cclk_config_msg_unpack(uint8 *buf, bcm_ces_crm_cclk_config_msg_t *msg)
{
    _SHR_UNPACK_U8(buf, msg->flags);
    _SHR_UNPACK_U8(buf, msg->e_cclk_select);
    _SHR_UNPACK_U8(buf, msg->e_ref_clk_1_select);
    _SHR_UNPACK_U8(buf, msg->e_ref_clk_2_select);
    _SHR_UNPACK_U32(buf, msg->n_ref_clk_1_port);
    _SHR_UNPACK_U32(buf, msg->n_ref_clk_2_port);
    _SHR_UNPACK_U8(buf, msg->e_protocol);

    return buf;
}

uint8 *
bcm_ces_crm_rclock_config_msg_pack(uint8 *buf, bcm_ces_crm_rclock_config_msg_t *msg)
{
    _SHR_PACK_U8(buf, msg->flags);
    _SHR_PACK_U8(buf, msg->b_enable);
    _SHR_PACK_U8(buf, msg->e_protocol);
    _SHR_PACK_U8(buf, msg->b_structured);
    _SHR_PACK_U8(buf, msg->output_brg);
    _SHR_PACK_U8(buf, msg->rclock);
    _SHR_PACK_U8(buf, msg->port);
    _SHR_PACK_U8(buf, msg->recovery_type);
    _SHR_PACK_U16(buf, msg->tdm_clocks_per_packet);
    _SHR_PACK_U8(buf, msg->service);
    /*Add new fields*/
    _SHR_PACK_U8(buf, msg->rcr_flags);
    _SHR_PACK_U32(buf, msg->ho_q_count);
    _SHR_PACK_U32(buf, msg->ho_a_threshold);
    _SHR_PACK_U32(buf, msg->ho_a_threshold);
    _SHR_PACK_U32(buf, msg->ho_a_threshold);
    return buf;
}

uint8 *
bcm_ces_crm_rclock_config_msg_unpack(uint8 *buf, bcm_ces_crm_rclock_config_msg_t *msg)
{
    _SHR_UNPACK_U8(buf, msg->flags);
    _SHR_UNPACK_U8(buf, msg->b_enable);
    _SHR_UNPACK_U8(buf, msg->e_protocol);
    _SHR_UNPACK_U8(buf, msg->b_structured);
    _SHR_UNPACK_U8(buf, msg->output_brg);
    _SHR_UNPACK_U8(buf, msg->rclock);
    _SHR_UNPACK_U8(buf, msg->port);
    _SHR_UNPACK_U8(buf, msg->recovery_type);
    _SHR_UNPACK_U16(buf, msg->tdm_clocks_per_packet);
    _SHR_UNPACK_U8(buf, msg->service);
    /*Add new fields*/
    _SHR_UNPACK_U8(buf, msg->rcr_flags);
    _SHR_UNPACK_U32(buf, msg->ho_q_count);
    _SHR_UNPACK_U32(buf, msg->ho_a_threshold);
    _SHR_UNPACK_U32(buf, msg->ho_a_threshold);
    _SHR_UNPACK_U32(buf, msg->ho_a_threshold);

    return buf;
}

uint8 *
bcm_ces_crm_status_msg_pack(uint8 *buf, bcm_ces_crm_status_msg_t *msg)
{
    _SHR_PACK_U8(buf, msg->rclock);
    _SHR_PACK_U8(buf, msg->clock_state);
    _SHR_PACK_U32(buf, msg->seconds_locked);
    _SHR_PACK_U32(buf, msg->seconds_active);
    _SHR_PACK_U32(buf, msg->calculated_frequency_w);
    _SHR_PACK_U32(buf, msg->calculated_frequency_f);

    return buf;
}

uint8 *
bcm_ces_crm_status_msg_unpack(uint8 *buf, bcm_ces_crm_status_msg_t *msg)
{
    _SHR_UNPACK_U8(buf, msg->rclock);
    _SHR_UNPACK_U8(buf, msg->clock_state);
    _SHR_UNPACK_U32(buf, msg->seconds_locked);
    _SHR_UNPACK_U32(buf, msg->seconds_active);
    _SHR_UNPACK_U32(buf, msg->calculated_frequency_w);
    _SHR_UNPACK_U32(buf, msg->calculated_frequency_f);

    return buf;
}
