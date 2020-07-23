/*! \file bcm56780_a0_tm_edb_flexport.h
 *
 * File contains EDB init and flexport related defines for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_TM_EDB_FLEXPORT_H
#define BCM56780_A0_TM_EDB_FLEXPORT_H

#define TD4_X9_EDB_MAX_CT_CLASSES 16

/* EDB buffer drain time out. */
#ifdef DV_ONLY
    #define EDB_DRAIN_TIMEOUT_US 10000
#else
    #define EDB_DRAIN_TIMEOUT_US 70
#endif

#endif /* BCM56780_A0_TM_EDB_FLEXPORT_H */
