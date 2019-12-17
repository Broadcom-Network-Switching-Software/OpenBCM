/****************************************************************************
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 *
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _PCIG3_PHY_ACC_H_
#define _PCIG3_PHY_ACC_H_

#define PCIE_MAX_CORES  (8)
#define PCIE_MAX_LANES_PER_CORE (4)

#include <merlin16_pcieg3_dependencies.h>
#include "merlin16_pcieg3_usr_includes.h"

extern int pcie_phy_diag_reg_read(srds_access_t *sa, uint16_t address, uint16_t *pdata);
extern int pcie_phy_diag_reg_write(srds_access_t *sa, uint16_t address, uint16_t data);
extern int pcie_phy_diag_reg_mwr(srds_access_t *sa, uint16_t address, uint16_t mask, uint8_t lsb, uint16_t val);
extern int pcie_phy_diag_pram_read(srds_access_t *sa, uint32_t address, uint32_t size);
extern int pcie_phy_diag_dsc(srds_access_t *sa, int array_size);
extern int pcie_phy_diag_state(srds_access_t *sa, int array_size);
extern int pcie_phy_diag_eyescan(srds_access_t *sa, int array_size);

#endif /* _PCIG3_PHY_ACC_H_ */
