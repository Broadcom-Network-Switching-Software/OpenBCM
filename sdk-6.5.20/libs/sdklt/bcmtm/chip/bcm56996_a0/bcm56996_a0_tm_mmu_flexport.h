/*! \file bcm56996_a0_tm_mmu_flexport.h
 *
 * File contains MMU init and flexport related defines for bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56996_A0_TM_MMU_FLEXPORT_H
#define BCM56996_A0_TM_MMU_FLEXPORT_H

#include <bcmtm/bcmtm_types.h>

#define TH4G_MMU_NUM_L0_NODES_PER_HSP_PORT 12
#define TH4G_MMU_NUM_L0_NODES_FOR_CPU_PORT 12
#define TH4G_MMU_NUM_L1_NODES_PER_HSP_PORT 2
#define TH4G_MMU_NUM_L1_NODES_FOR_CPU_PORT 48
#define TH4G_MMU_NUM_Q_PER_HSP_PORT 12
#define TH4G_MMU_NUM_Q_FOR_CPU_PORT 48

#define TH4G_MMU_BST_TRACK_MODE 0
#define TH4G_MMU_BST_WMARK_MODE 1
#define TH4G_MMU_NUM_PGS 8
#define TH4G_MMU_NUM_SPS 4
#define TH4G_MMU_NUM_COS 12

#define TH4G_MMU_FLUSH_OFF 0
#define TH4G_MMU_FLUSH_ON  1

/*fix for TH4G */
#define TH4G_INVALID_MMU_PORT 511
#define TH4G_INVALID_DEV_PORT 272
#define TH4G_INVALID_LOCAL_PHYS_PORT 63
#define TH4G_INVALID_PHYS_PORT 511
#define TH4G_MMU_PORT_PIPE_OFFSET 32

extern int
bcm56996_a0_tm_port_mmu_down(int unit, bcmtm_pport_t pport);

extern int
bcm56996_a0_tm_port_mmu_delete(int unit, bcmtm_pport_t pport);

extern int
bcm56996_a0_tm_port_mmu_add (int unit, int pport);

extern int
bcm56996_a0_tm_rqe_port_flush(int unit);
#endif /* BCM56996_A0_TM_MMU_FLEXPORT_H */
