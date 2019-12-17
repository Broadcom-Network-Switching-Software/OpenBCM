/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    st_sdk_msg.h
 */

#ifndef ST_SDK_MSG_H_
#define ST_SDK_MSG_H_

#include <sal/types.h>

/*
 * ST Initialization control message
 */
typedef struct st_sdk_msg_ctrl_init_s {
    /* Initialization flags */
    uint32 flags;

    /* Upper 32 bits of init time */
    uint32 init_time_u;

    /* Lower 32 bits of init time */
    uint32 init_time_l;

    /* Max length of the export packet */
    uint16 max_export_pkt_length;

    /* Number of instances */
    uint16 num_instances;

    /* Number of collectors */
    uint16 num_collectors;

    /* Maximum number of ports to be monitored */
    uint16 max_num_ports;
} st_sdk_msg_ctrl_init_t;

#endif /* ST_SDK_MSG_H_ */
