/*! \file bcm56990_b0_tm_edb_flexport.h
 *
 * File contains EDB init and flexport related defines for bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_TM_EDB_FLEXPORT_H
#define BCM56990_A0_TM_EDB_FLEXPORT_H

#include <bcmtm/bcmtm_types.h>


/*** START SDKLT API COMMON CODE ***/

/* Number of max cut thru classes. */
#define TH4_B0_EDB_MAX_CT_CLASSES 16


/* EDB buffer drain time out. */
#define EDB_DRAIN_TIMEOUT_US 70

/*
 * Number of PFC Optimized Config cases of xmit start count.
 *   General case + PFC_OPT cases
 */
#define TH4_B0_EDB_E2E_NUM_PFC_OPT_CFG  2
/* E2E table index value */
#define TH4_B0_EDB_E2E_TBL_SPD_LPBK     1
#define TH4_B0_EDB_E2E_TBL_SPD_CMIC     2
#define TH4_B0_EDB_E2E_TBL_SPD_MGMT_10G 3
#define TH4_B0_EDB_E2E_TBL_SPD_MGMT_40G 4
/* Number of valid cut thru classes: 1 SAF + 7 CT + 4 AUX. */
#define TH4_B0_EDB_E2E_TBL_ENTRIES 12


/* Transmit Start Count. */
typedef struct th4_b0_edb_xmit_start_count_s {
    /*! SAF value. */
    uint32_t saf;

    /*! CT value. */
    uint32_t ct;
} th4_b0_edb_xmit_start_count_t;

/* EDB E2E settings: ct_class, EP2MMU credit, xmit_cnt. */
typedef struct th4_b0_edb_e2e_setting_s {
    /*! Speed in Mbps. */
    int speed;

    /*! CT class. */
    int ct_class;

    /*! EP2MMU credit. */
    int mmu_credit;
    int mmu_credit_pfc_opt;

    /*! Xmit start count. [0]->general case, [1-N]-> OPT cases. */
    th4_b0_edb_xmit_start_count_t xmit_cnt[TH4_B0_EDB_E2E_NUM_PFC_OPT_CFG];
} th4_b0_edb_e2e_setting_t;


extern int
th4_b0_ep_get_ep2mmu_credit_per_speed(int unit, int speed, uint32_t *credit);

extern int
bcm56990_b0_tm_port_edb_add(int unit, bcmtm_pport_t pport);

extern int
bcm56990_b0_tm_port_edb_up(int unit, bcmtm_pport_t pport);

extern int
bcm56990_b0_tm_port_edb_down(int unit, bcmtm_pport_t pport);

extern int
bcm56990_b0_tm_port_edb_delete(int unit, bcmtm_pport_t pport);

/*** END SDKLT API COMMON CODE ***/

#endif /* BCM56990_A0_TM_EDB_FLEXPORT_H */
