/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * *
 * File:   epfc_sdk_msg.h
 */

#ifndef PFC_SDK_MSG_H_
#define PFC_SDK_MSG_H_
#include <sal/types.h>

/*
 * Encrypted PFC Initialization control message
 */
typedef struct pfc_sdk_msg_ctrl_init_s
{
    uint32 cpu_port;
} pfc_sdk_msg_ctrl_init_t;

/*
 * Encrypted PFC port stat message
 */
typedef struct pfc_sdk_msg_ctrl_stat_s
{
    uint32 port;
    uint8 pg_offset;
} pfc_sdk_msg_ctrl_stat_t;

/*
 * Encrypted PFC port stat reply message
 */
typedef struct pfc_sdk_msg_ctrl_stat_reply_s
{
    uint32 packets_out_lo;
    uint32 packets_out_hi;
} pfc_sdk_msg_ctrl_stat_reply_t;

/*
 * Encrypted PFC PG attributes
 */
typedef struct pfc_sdk_msg_ctrl_pg_attr_s
{
    uint8 tc_bit_map;
} pfc_sdk_msg_ctrl_pg_attr_t;

typedef struct pfc_sdk_msg_ctrl_port_get_reply_s
{
    uint8 tc_enable_bmp;
    uint8 src_mac[6];
    uint32 refresh_interval_usecs;
    uint32 encap_id;
} pfc_sdk_msg_ctrl_port_get_reply_t;

/*
 * Encrypted PFC port config message
 */
typedef struct pfc_sdk_msg_ctrl_port_config_s
{
    uint8 core;
    uint8 num_pg;
    uint8 tc_enable_bmp;
    uint32 vsq_id_base;
    uint32 refresh_interval_usecs;
    uint32 port;
    uint32 out_lif;
    uint32 speed;
    uint8 src_mac[6];
    pfc_sdk_msg_ctrl_pg_attr_t pg_attr[8];
} pfc_sdk_msg_ctrl_port_config_t;

typedef union pfc_msg_ctrl_s
{
    pfc_sdk_msg_ctrl_init_t init;
    pfc_sdk_msg_ctrl_stat_reply_t stat_reply;
    pfc_sdk_msg_ctrl_stat_t stat;
    pfc_sdk_msg_ctrl_port_config_t port_config;
    pfc_sdk_msg_ctrl_port_get_reply_t port_get_reply;
} pfc_msg_ctrl_t;

#endif /* PFC_SDK_MSG_H_ */
