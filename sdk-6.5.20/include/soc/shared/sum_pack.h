/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    sum_pack.h
 */

#ifndef SHR_SUM_PACK_H_
#define SHR_SUM_PACK_H_

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif
#include <soc/shared/sum_msg.h>

uint8*
shr_sum_msg_ctrl_config_pack(uint8 *buf, shr_sum_msg_ctrl_config_t *msg);

uint8*
shr_sum_msg_ctrl_config_unpack(uint8 *buf, shr_sum_msg_ctrl_config_t *msg);

uint8*
shr_sum_msg_ctrl_stat_config_pack(uint8 *buf, shr_sum_msg_buf_info_t *msg);

uint8*
shr_sum_msg_ctrl_stat_config_unpack(uint8 *buf, shr_sum_msg_buf_info_t *msg);
#endif /* SHR_SUM_PACK_H_ */
