/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_PTABLE_H
#define _SOC_PTABLE_H

/*
 * PTABLE bit definitions
 */

/* Values for SP_ST */

#define PVP_STP_DISABLED	0	/* Disabled */
#define PVP_STP_BLOCKING	1	/* Blocking/Listening */
#define PVP_STP_LEARNING	2	/* Learning */
#define PVP_STP_FORWARDING	3	/* Forwarding */

/*
 * Values for CML (control what happens on Source Lookup Failure packet).
 * The last two are for Draco only; on StrataSwitch CML is only 2 bits.
 */

#define	PVP_CML_SWITCH		0	/*   Learn ARL, !CPU,  Forward */
#define	PVP_CML_CPU		1	/*  !Learn ARL,  CPU, !Forward */
#define	PVP_CML_FORWARD		2	/*  !Learn ARL, !CPU,  Forward */
#define	PVP_CML_DROP		3	/*  !Learn ARL, !CPU, !Forward */
#define PVP_CML_CPU_SWITCH	4	/*   Learn ARL,  CPU,  Forward */
#define PVP_CML_CPU_FORWARD 	5	/*  !Learn ARL,  CPU,  Forward */

/* Egress port table redirection types */
#define SOC_EPT_REDIRECT_NOOP           0       /* NOOP - do not redirect */
#define SOC_EPT_REDIRECT_MIRROR         4       /* Mirror bitmap */
#define SOC_EPT_REDIRECT_SYS_PORT       5       /* System port */
#define SOC_EPT_REDIRECT_TRUNK          6       /* Trunk group */
#define SOC_EPT_REDIRECT_L2MC           7       /* L2MC group */

#endif	/* !_SOC_PTABLE_H */
