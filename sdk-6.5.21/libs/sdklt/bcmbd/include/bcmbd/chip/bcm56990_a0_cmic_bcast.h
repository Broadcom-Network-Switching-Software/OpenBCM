/*! \file bcm56990_a0_cmic_bcast.h
 *
 * Definitions for CMIC SBUS broadcast.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_CMIC_BCAST_H
#define BCM56990_A0_CMIC_BCAST_H

#include <bcmbd/bcmbd_cmicx.h>

/*! Number of CDPORT broadcast domains. */
#define CDPORT_NUM_BCAST        4

/*! Broadcast block for CDPORT broadcast domain #0. */
#define CDPORT_BCST0_BLK        80

/*! Broadcast block for CDPORT broadcast domain #1. */
#define CDPORT_BCST1_BLK        81

/*! Broadcast block for CDPORT broadcast domain #2. */
#define CDPORT_BCST2_BLK        82

/*! Broadcast block for CDPORT broadcast domain #3. */
#define CDPORT_BCST3_BLK        83

/*! First port in broadcast domain #0. */
#define CDPORT_BCST0_1ST_PORT   1

/*! First port in broadcast domain #1. */
#define CDPORT_BCST1_1ST_PORT   65

/*! First port in broadcast domain #2. */
#define CDPORT_BCST2_1ST_PORT   129

/*! First port in broadcast domain #3. */
#define CDPORT_BCST3_1ST_PORT   193

/*! Get broadcast block for a given broadcast domain. */
#define CDPORT_BCAST_BLK(_d)    (CDPORT_BCST0_BLK + (_d))

/*! Get broadcast domain for a given physical port number. */
#define CDPORT_BCAST_DOMAIN(_p) \
    (((_p) < CDPORT_BCST1_1ST_PORT) ? 0 : \
     ((_p) < CDPORT_BCST2_1ST_PORT) ? 1 : \
     ((_p) < CDPORT_BCST3_1ST_PORT) ? 2 : 3)

/*! Get broadcast port for a given broadcast domain. */
#define CDPORT_BCAST_PORT(_d) \
    (BCMBD_CMICX_BCAST_BIT | CDPORT_BCAST_BLK(_d))

#endif /* BCM56990_A0_CMIC_BCAST_H */
