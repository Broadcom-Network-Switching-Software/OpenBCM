/*! \file bcm56880_a0_cmic_bcast.h
 *
 * Definitions for CMIC SBUS broadcast.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_CMIC_BCAST_H
#define BCM56880_A0_CMIC_BCAST_H

#include <bcmbd/bcmbd_cmicx.h>

/*! Number of CDPORT broadcast domains. */
#define CDPORT_NUM_BCAST        2

/*! Broadcast block id for CDPORT broadcast domain #0. */
#define CDPORT_BCST0_BLK        56

/*! Broadcast block id for CDPORT broadcast domain #1. */
#define CDPORT_BCST1_BLK        59

/*! First port in broadcast domain #0. */
#define CDPORT_BCST0_1ST_PORT   1

/*! First port in broadcast domain #1. */
#define CDPORT_BCST1_1ST_PORT   129

/*! Get broadcast block for a given broadcast domain. */
#define CDPORT_BCAST_BLK(_d) \
    (((_d) == 0) ? CDPORT_BCST0_BLK : CDPORT_BCST1_BLK)

/*! Get broadcast port for a given broadcast domain. */
#define CDPORT_BCAST_PORT(_d) \
    (BCMBD_CMICX_BCAST_BIT | CDPORT_BCAST_BLK(_d))

#endif /* BCM56880_A0_CMIC_BCAST_H */
