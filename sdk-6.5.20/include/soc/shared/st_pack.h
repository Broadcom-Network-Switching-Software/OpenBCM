/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    st_pack.h
 */

#ifndef SHR_ST_PACK_H_
#define SHR_ST_PACK_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif
#include <soc/shared/st_msg.h>


uint8*
shr_st_msg_ctrl_ports_add_pack(uint8 *buf, shr_st_msg_ctrl_ports_add_t *msg);

uint8*
shr_st_msg_ctrl_ports_add_unpack(uint8 *buf, shr_st_msg_ctrl_ports_add_t *msg);

uint8*
shr_st_msg_ctrl_ports_get_pack(uint8 *buf, shr_st_msg_ctrl_ports_get_t *msg);

uint8*
shr_st_msg_ctrl_ports_get_unpack(uint8 *buf, shr_st_msg_ctrl_ports_get_t *msg);

uint8*
shr_st_msg_ctrl_ports_delete_pack(uint8 *buf, shr_st_msg_ctrl_ports_delete_t *msg);

uint8*
shr_st_msg_ctrl_ports_delete_unpack(uint8 *buf, shr_st_msg_ctrl_ports_delete_t *msg);

uint8*
shr_st_msg_ctrl_instance_create_pack(uint8 *buf, shr_st_msg_ctrl_instance_create_t *msg);

uint8*
shr_st_msg_ctrl_instance_create_unpack(uint8 *buf, shr_st_msg_ctrl_instance_create_t *msg);

uint8*
shr_st_msg_ctrl_instance_get_pack(uint8 *buf, shr_st_msg_ctrl_instance_get_t *msg);

uint8*
shr_st_msg_ctrl_instance_get_unpack(uint8 *buf, shr_st_msg_ctrl_instance_get_t *msg);

uint8*
shr_st_msg_ctrl_instance_collector_attach_pack(uint8* buf,
                                               shr_st_msg_ctrl_instance_collector_attach_t *msg);

uint8*
shr_st_msg_ctrl_instance_collector_attach_unpack(uint8* buf,
                                                 shr_st_msg_ctrl_instance_collector_attach_t *msg);
uint8* shr_st_msg_system_id_pack(uint8 *buf, shr_st_msg_system_id_t *msg);

uint8* shr_st_msg_system_id_unpack(uint8 *buf, shr_st_msg_system_id_t *msg);

uint8* shr_st_msg_instance_export_stats_pack(uint8 *buf,
                                             shr_st_msg_instance_export_stats_t *msg);

uint8* shr_st_msg_instance_export_stats_unpack(uint8 *buf,
                                               shr_st_msg_instance_export_stats_t *msg);
uint8* shr_st_msg_ctrl_config_pack(uint8 *buf, shr_st_hw_config_t *msg);
uint8* shr_st_msg_ctrl_config_unpack(uint8 *buf, shr_st_hw_config_t *msg);

#endif /* SHR_ST_PACK_H_ */
