/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	socend.h
 * Purpose:	Defines SOC private device control structure for
 *		VxWorks SENS driver.
 */
#ifndef __SOCEND_H
#define __SOCEND_H

#include <bcm/rx.h>
#include <bcm/tx.h>
#include <bcm/pkt.h>

#define	SOC_END_STRING	"Broadcom StrataSwitch (BCM56XX) SENS Driver"

/*
 * Typedef:	socend_t
 * Purpose:	Describes the control structure for 1 END device on the
 *		SOC.
 */
typedef struct socend_s {
    struct end_object	se_eo;		/* SENS: END_OBJ */
    sal_mac_addr_t	se_mac;		/* SENS: Interface Mac address */
    bcm_vlan_t		se_vlan;	/* SENS: Interface VLAN ID */
    int			se_unit;	/* SOC Unit # */
    struct socend_s	*se_next;	/* Linked list of devices off Unit */
} socend_t;

/*
 * TX and RX packet snooping/filter typedefs; see socend.c.
 */

typedef void (*socend_snoop_ip_tx_t)(int unit, void *packet, int len);
typedef bcm_rx_t (*socend_snoop_ip_rx_t)(int unit, bcm_pkt_t *pkt, int *enqueue);

/*
 * Defines: SOC_END_XXX
 * Purpose: SOC End Driver memory buffer allocation parameters. 
 */
#define	SOC_END_PK_SZ	1600		/* Packet Size */
#define	SOC_END_CLBUFS	(128)		/* Cluster Buffers */
#define	SOC_END_CLBLKS	256		/* Total Cluster Headers */
#define	SOC_END_MBLKS	64		/* Total Packet Headers */
#define	SOC_END_SPEED	10000000	/* 10 Mb/s */

#define SOCEND_COS_DEFAULT 3
#define SOCEND_COS_MAP_DEFAULT {0, 0, 0, 0, 0, 0, 3, 3}

/*
 * Defines: SOC_END_FLG_XXX
 * Purpose: Various driver mode and configuration flags.
 */
#define SOC_END_FLG_MGMT   (1 << 1)
#define SOC_END_FLG_BCAST  (1 << 2)

/* BSP support routine: return switch address */
IMPORT STATUS sysSwitchAddrGet(char *mac);

/* StrataSwitch (TM) SOC Driver API Initialization */
IMPORT int socdrv(void);

#endif /* __SOCEND_H */
