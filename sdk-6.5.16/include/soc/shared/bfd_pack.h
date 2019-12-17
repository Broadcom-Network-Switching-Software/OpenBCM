/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bfd_pack.h
 * Purpose:     Interface to pack and unpack routines common to
 *              SDK and uKernel for:
 *              - BFD Control messages
 *              - Network Packet headers
 *
 *              This is to be shared between SDK host and uKernel.
 */

#ifndef   _SOC_SHARED_BFD_PACK_H_
#define   _SOC_SHARED_BFD_PACK_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/shared/bfd_msg.h>
#include <soc/shared/bfd_pkt.h>

/* BFD Control Messages pack/unpack */

extern uint8 *
shr_bfd_msg_ctrl_sess_set_pack(uint8 *buf,
                               shr_bfd_msg_ctrl_sess_set_t *msg);
extern uint8 *
shr_bfd_msg_ctrl_sess_set_unpack(uint8 *buf,
                                 shr_bfd_msg_ctrl_sess_set_t *msg);

extern uint8 *
shr_bfd_msg_ctrl_sess_get_pack(uint8 *buf,
                               shr_bfd_msg_ctrl_sess_get_t *msg);
extern uint8 *
shr_bfd_msg_ctrl_sess_get_unpack(uint8 *buf,
                                 shr_bfd_msg_ctrl_sess_get_t *msg);

extern uint8 *
shr_bfd_msg_ctrl_auth_sha1_pack(uint8 *buf,
                                shr_bfd_msg_ctrl_auth_sha1_t *msg);
extern uint8 *
shr_bfd_msg_ctrl_auth_sha1_unpack(uint8 *buf,
                                  shr_bfd_msg_ctrl_auth_sha1_t *msg);

extern uint8 *
shr_bfd_msg_ctrl_auth_sp_pack(uint8 *buf, shr_bfd_msg_ctrl_auth_sp_t *msg);
extern uint8 *
shr_bfd_msg_ctrl_auth_sp_unpack(uint8 *buf, shr_bfd_msg_ctrl_auth_sp_t *msg);

extern uint8 *
shr_bfd_msg_ctrl_stat_req_pack(uint8 *buf,
                               shr_bfd_msg_ctrl_stat_req_t *msg);
extern uint8 *
shr_bfd_msg_ctrl_stat_req_unpack(uint8 *buf,
                                 shr_bfd_msg_ctrl_stat_req_t *msg);

extern uint8 *
shr_bfd_msg_ctrl_stat_reply_pack(uint8 *buf,
                                 shr_bfd_msg_ctrl_stat_reply_t *msg);
extern uint8 *
shr_bfd_msg_ctrl_stat_reply_unpack(uint8 *buf,
                                   shr_bfd_msg_ctrl_stat_reply_t *msg);

/* BFD Network Packet headers pack/unpack */
extern uint8 *
shr_bfd_header_pack(uint8 *buf, shr_bfd_header_t *bfd);
extern uint8 *
shr_bfd_header_unpack(uint8 *buf, shr_bfd_header_t *bfd);

extern uint8 *
shr_bfd_auth_header_pack(uint8 *buf, shr_bfd_auth_header_t *auth);
extern uint8 *
shr_bfd_auth_header_unpack(uint8 *buf, shr_bfd_auth_header_t *auth);

#endif /* _SOC_SHARED_BFD_PACK_H_ */
