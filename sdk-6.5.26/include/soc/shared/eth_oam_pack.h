/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    eth_oam_pack.h
 */

#ifndef SHR_ETH_OAM_PACK_H_
#define SHR_ETH_OAM_PACK_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif
#include <soc/shared/eth_oam_msg.h>


extern uint8*
shr_eth_oam_msg_ctrl_init_pack(uint8 *buf, shr_eth_oam_msg_ctrl_init_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_init_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_init_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_lmep_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_lmep_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_lmep_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_lmep_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_lmep_del_pack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_del_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_lmep_del_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_lmep_del_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_rmep_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_rmep_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_rmep_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_rmep_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_rmep_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_rmep_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_rmep_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_rmep_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_slm_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_slm_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_slm_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_slm_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_slm_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_slm_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_slm_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_slm_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_trunk_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_trunk_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_trunk_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_trunk_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_trunk_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_trunk_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_trunk_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_trunk_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_event_masks_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_event_masks_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_event_masks_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_event_masks_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_pm_pstats_slm_get_pack(uint8 *buf, shr_eth_oam_msg_ctrl_pm_pstats_slm_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_pm_pstats_slm_get_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_pm_pstats_slm_get_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_pm_rstats_addr_set_pack(uint8 *buf, shr_eth_oam_msg_ctrl_pm_rstats_addr_set_t *msg);

extern uint8*
shr_eth_oam_msg_ctrl_pm_rstats_addr_set_unpack(uint8 *buf, shr_eth_oam_msg_ctrl_pm_rstats_addr_set_t *msg);

#endif /* SHR_ETH_OAM_PACK_H_ */
