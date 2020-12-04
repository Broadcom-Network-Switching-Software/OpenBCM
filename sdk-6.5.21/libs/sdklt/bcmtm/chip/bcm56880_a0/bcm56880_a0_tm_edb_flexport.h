/*! \file bcm56880_a0_tm_edb_flexport.h
 *
 * File contains EDB init and flexport related defines for bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_EDB_FLEXPORT_H
#define BCM56880_A0_TM_EDB_FLEXPORT_H

/* Number of max cut thru classes. */
#define TD4_EDB_MAX_CT_CLASSES 16


/* EDB buffer drain time out. */
#ifdef DV_ONLY
    #define EDB_DRAIN_TIMEOUT_US 10000
#else
    #define EDB_DRAIN_TIMEOUT_US 70
#endif



extern int td4_port_edb_down(int unit, int *lport_bmp);
extern int td4_port_edb_up(int unit, int *lport_bmp);


#endif /* BCM56880_A0_TM_EDB_FLEXPORT_H */

