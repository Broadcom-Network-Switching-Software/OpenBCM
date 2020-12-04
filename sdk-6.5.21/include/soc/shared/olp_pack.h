/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        olp_pack.h
 * Purpose:     Interface to pack and unpack routines common to
 *              SDK and uKernel for:
 *              - OLP Control messages and Network packet headers
 *
 *              This is to be shared between SDK host and uKernel.
 */


#ifndef   _SOC_SHARED_OLP_PACK_H_
#define   _SOC_SHARED_OLP_PACK_H_

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

#include <soc/shared/olp_pkt.h>
extern uint8 *
shr_olp_tx_header_unpack(uint8 *bufp, soc_olp_tx_hdr_t *opt);

extern uint8 *
shr_olp_tx_header_pack(uint8 *bufp, soc_olp_tx_hdr_t *opt);

extern uint8 *
shr_olp_rx_header_unpack(uint8 *bufp, soc_olp_rx_hdr_t *opr);

extern uint8 *
shr_olp_rx_header_pack(uint8 *bufp, soc_olp_rx_hdr_t *opr);
#endif /* _SOC_SHARED_OLP_PACK_H_ */

