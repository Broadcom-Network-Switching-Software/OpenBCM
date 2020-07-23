/*! \file bcm56990_b0_tm_mmu_tdm.h
 *
 * File contains MMU init and flexport related defines for bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_TM_MMU_TDM_H
#define BCM56990_A0_TM_MMU_TDM_H

#include <bcmtm/bcmtm_types.h>


/*** START SDKLT API COMMON CODE ***/

typedef enum {
    TDM_MMU_MAIN_SCHED,
    TDM_MMU_EB_PORT_SCHED,
    TDM_MMU_EB_PKT_SCHED
} mmu_sched_inst_name_e;

extern int
th4_b0_mmu_tdm_general_init(int unit);

extern int
bcm56990_b0_tm_tdm_mmu_init(int unit);

extern int
th4_b0_mmu_tdm_port_delete(int unit, int lport);

extern int
th4_b0_mmu_tdm_port_add(int unit, int lport);

extern int
bcm56990_b0_tm_mmu_port_clear_edb_credit_counter(
    int unit,
    bcmtm_pport_t pport);

extern int
th4_b0_tdm_global_to_local_phy_num(int phy_port, uint32_t *local_phy_num);

/*** END SDKLT API COMMON CODE ***/
/*! @fn int th4_b0_mmu_get_speed_decode(int unit, int speed,
 *              int *speed_decode)
 *  @param unit Device number
 *  @param speed Actual speed value
 *  @param *speed_decode Decoded speed ID value
 *  @brief API to return the decoded speed value, used to program
 *         EBCTM registers
 */

extern int
th4_b0_mmu_get_speed_decode(int unit, int speed, uint32_t *speed_decode);


#endif /* BCM56990_A0_TM_MMU_TDM_H */
