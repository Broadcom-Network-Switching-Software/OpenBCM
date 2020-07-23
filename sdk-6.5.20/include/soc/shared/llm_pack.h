/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        llm_pack.h
 * Purpose:     Interface to pack and unpack LLM messages.
 *              This is to be shared between SDK host and uKernel.
 */

#ifndef   _SOC_SHARED_LLM_PACK_H_
#define   _SOC_SHARED_LLM_PACK_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/shared/mos_msg_common.h>
extern uint32 shr_max_buffer_get(void);
extern uint8 *shr_llm_msg_ctrl_init_pack    (uint8 *buf, shr_llm_msg_ctrl_init_t *msg);
extern uint8 *shr_llm_msg_ctrl_init_unpack  (uint8 *buf, shr_llm_msg_ctrl_init_t *msg);
extern uint8 *shr_llm_msg_pon_att_pack      (uint8 *buf, shr_llm_msg_pon_att_t *msg);
extern uint8 *shr_llm_msg_pon_att_unpack    (uint8 *buf, shr_llm_msg_pon_att_t *msg);
extern uint8 *shr_llm_msg_app_info_get_pack (uint8 *buf, shr_llm_msg_app_info_get_t *msg);
extern uint8 *shr_llm_msg_app_info_get_unpack(uint8 *buf, shr_llm_msg_app_info_get_t *msg);
extern uint8 *shr_llm_msg_ctrl_config_pack  (uint8 *buf, shr_llm_msg_ctrl_limit_value_t *msg);
extern uint8 *shr_llm_msg_ctrl_config_unpack(uint8 *buf, shr_llm_msg_ctrl_limit_value_t *msg);
extern uint8 *shr_llm_msg_pon_db_pack       (uint8 *buf, shr_llm_PON_ID_attributes_t *msg);
extern uint8 *shr_llm_msg_pon_db_unpack     (uint8 *buf, shr_llm_PON_ID_attributes_t *msg);
extern uint8 *shr_llm_msg_pointer_pool_pack (uint8 *buf, shr_llm_pointer_pool_t *msg);
extern uint8 *shr_llm_msg_pointer_pool_unpack(uint8 *buf, shr_llm_pointer_pool_t *msg);
extern uint8 *shr_llm_msg_pon_db_pack_port  (uint8 *buf, uint32 *port);
extern uint8 *shr_llm_msg_pon_db_unpack_port(uint8 *buf, uint32 *port);
extern uint8 *shr_llm_msg_pon_att_enable_pack(uint8 *buf, shr_llm_msg_pon_att_enable_t *msg);
extern uint8 *shr_llm_msg_pon_att_enable_unpack(uint8 *buf, shr_llm_msg_pon_att_enable_t *msg);
extern uint8 *shr_llm_msg_pon_att_update_pack(uint8 *buf, shr_llm_msg_pon_att_update_t *msg);
extern uint8 *shr_llm_msg_pon_att_update_unpack(uint8 *buf, shr_llm_msg_pon_att_update_t *msg);
extern uint8 *shr_llm_msg_pon_att_rx_mode_pack(uint8 *buf, shr_llm_msg_pon_att_rx_mode_t *msg);
extern uint8 *shr_llm_msg_pon_att_rx_mode_unpack(uint8 *buf, shr_llm_msg_pon_att_rx_mode_t *msg);
extern uint8 *shr_llm_msg_pon_att_mac_move_pack(uint8 *buf, shr_llm_msg_pon_att_mac_move_t *msg);
extern uint8 *shr_llm_msg_pon_att_mac_move_unpack(uint8 *buf, shr_llm_msg_pon_att_mac_move_t *msg);
extern uint8 *shr_llm_msg_pon_att_mac_move_event_pack(uint8 *buf, shr_llm_msg_pon_att_mac_move_event_t *msg);
extern uint8 *shr_llm_msg_pon_att_mac_move_event_unpack(uint8 *buf, shr_llm_msg_pon_att_mac_move_event_t *msg);
#endif /* _SOC_SHARED_LLM_PACK_H_ */
