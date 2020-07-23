/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    int_pack.h
 */

#ifndef SHR_INT_PACK_H_
#define SHR_INT_PACK_H_

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif
#include <soc/shared/int_msg.h>

uint8*
shr_int_turnaround_msg_ctrl_init_pack(uint8 *buf,
        shr_int_turnaround_msg_ctrl_init_t *msg);

uint8*
shr_int_turnaround_msg_ctrl_init_unpack(uint8 *buf,
        shr_int_turnaround_msg_ctrl_init_t *msg);

uint8*
shr_int_turnaround_msg_ctrl_config_pack(uint8* buf,
        shr_int_turnaround_msg_ctrl_config_t *msg);

uint8*
shr_int_turnaround_msg_ctrl_config_unpack(uint8* buf,
        shr_int_turnaround_msg_ctrl_config_t *msg);

uint8*
shr_int_turnaround_msg_ctrl_stat_pack(uint8* buf,
        shr_int_turnaround_msg_ctrl_stat_t *msg);

uint8*
shr_int_turnaround_msg_ctrl_stat_unpack(uint8* buf,
        shr_int_turnaround_msg_ctrl_stat_t *msg);
#endif /* SHR_INT_PACK_H_ */
