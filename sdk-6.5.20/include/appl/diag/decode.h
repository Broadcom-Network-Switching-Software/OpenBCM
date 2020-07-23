/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This header file defines the interface to the packet decode routines.
 */

#include <bcm/pkt.h>

extern	int	d_packet_format(char *, int type, void *, int, void *);
extern	int	d_packet_vformat(char *, char *, int type, void *, int, void *);

/* Packet "type" on entry into decode routines */

#define	DECODE_ETHER	1
#define	DECODE_IP	2
#define DECODE_TCP	3
#define	DECODE_ARP	4
#define	DECODE_GB_ETHER	5


#if defined(BROADCOM_DEBUG)
extern void bcm_pkt_dump(int unit, bcm_pkt_t *pkt, int dump_data);
#endif  /* BROADCOM_DEBUG */
