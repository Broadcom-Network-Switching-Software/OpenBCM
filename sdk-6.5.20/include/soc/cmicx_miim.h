/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMICx MIIM (MDIO) functions
 */

#ifndef _SOC_CMICX_MIIM_H
#define _SOC_CMICX_MIIM_H

#include <sal/types.h>

#define SOC_CLAUSE_45 45
#define SOC_CLAUSE_22 22

#define CMICX_MIIM_RING_INDEX_START     0
#define CMICX_MIIM_RING_INDEX_END       7
#define CMICX_MIIM_12R_RING_INDEX_END   11

/* MDIO CYCLE types for iProc 15 and higher devices
 * with support for 12 MDIO rings */

#define MIIM_CYCLE_12R_C22_REG_WR           0x0
#define MIIM_CYCLE_12R_C22_REG_RD           0x1
#define MIIM_CYCLE_12R_C45_AUTO_WR          0x2
#define MIIM_CYCLE_12R_C45_AUTO_RD          0x3
#define MIIM_CYCLE_12R_C45_REG_AD           0x4
#define MIIM_CYCLE_12R_C45_REG_WR           0x5
#define MIIM_CYCLE_12R_C45_REG_RD           0x6
#define MIIM_CYCLE_12R_C45_REG_RD_ADINC     0x7

/*******************************************
* @function soc_cmicx_miim_init
* purpose Initialization required for MIIM module
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
* @comments This function give MDIO access to IPROC
 */
extern int soc_cmicx_miim_init(int unit);

/*******************************************
* @function soc_cmicx_miim_divider_set_ring
* purpose set MDIO frequency related properties for a given MIIM ring
*
* @param unit [in] unit #
* @param unit [in] ring_idx - MIIM ring index.
* @param unit [in] int_divider - The clock divider configuration register for Internal MDIO. use -1 to keep current configuration.
* @param unit [in] ext_divider - The clock divider configuration register for External MDIO. use -1 to keep current configuration.
* @param unit [in] out_delay - MDIO Output Delay. use -1 to keep current configuration.
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
* @comments This function configure all MDIO rings
 */
extern int soc_cmicx_miim_divider_set_ring(int unit, int ring_index, int int_divider, int ext_divider, int out_delay);

/*******************************************
* @function soc_cmicx_miim_divider_set_all
* purpose set MDIO frequency related properties for all MIIM rings (0 to 7)
*
* @param unit [in] unit #
* @param unit [in] int_divider - The clock divider configuration register for Internal MDIO. use -1 to keep current configuration.
* @param unit [in] ext_divider - The clock divider configuration register for External MDIO. use -1 to keep current configuration.
* @param unit [in] out_delay - MDIO Output Delay. use -1 to keep current configuration.
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
* @comments This function configure all MDIO rings
 */
extern int soc_cmicx_miim_divider_set_all(int unit, int int_divider, int ext_divider, int out_delay);

/*******************************************
* @function soc_cmicx_miim_operation
* purpose perform miim read or write operation
*
* @param unit [in] unit #
* @param unit [in] is_write - whether to perform read or write operation
* @param unit [in] clause - whether to perform clause 45 or clause 22 operation
* @param unit [in] phy_id - phy to access. phy_id encoding:
*                            bit7, 1: internal MDIO bus, 0: external MDIO bus
*                            bit9,8,6,5, mdio bus number
*                            bit4-0,   mdio addresses
* @param unit [in] phy_reg_addr - CL45/CL22 register address. For CL45, Encoded according to SOC_PHY_CLAUSE45_ADDR macro.
* @param unit [inout] phy_data - data to wrote in case is_write is true, data read from register otehrwise. 
* 
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
* @comments
*     This function takes MIIM lock
*     Waiting for MIIM done is done by polling (interrupt mode isn't supported)
 */
extern int soc_cmicx_miim_operation(int unit, int is_write, int clause, uint32 phy_id,
                  uint32 phy_reg_addr, uint16 *phy_data);

#endif  /* !_SOC_CMICX_MIIM_H */

