/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
/*
  Includes
*/
#include "furia_pkg_cfg.h"
#include "furia_types.h"
/*
* Defines
*/

/*
* Types
*/

/*
* Macros
*/

/*
* Global Variables
*/

    /* No package configuration defined. */
    /* Using Plain Duplex with no pkg swap */ 

    /* chip_id, pkg_lane_num, lane_type, die_addr, die_lane_num, slice_rd_val, slice_wr_val,
     * sideB(RX), sideA(TX), inv_in_data, inv_out_data
     */

const FURIA_PKG_LANE_CFG_t glb_lanes_82208[8] = {
    {0x82208, 0, SIMPLEX_TX, 0, 2, 2, 1 << 2, SYS, LINE, 1, 1},
    {0x82208, 1, SIMPLEX_TX, 0, 3, 3, 1 << 3, SYS, LINE, 0, 0},
    {0x82208, 2, SIMPLEX_TX, 0, 3, 3, 1 << 3, LINE, SYS, 1, 1},
    {0x82208, 3, SIMPLEX_TX, 0, 2, 2, 1 << 2, LINE, SYS, 1, 1},
    {0x82208, 4, SIMPLEX_TX, 0, 1, 1, 1 << 1, LINE, SYS, 1, 1},
    {0x82208, 5, SIMPLEX_TX, 0, 0, 0, 1 << 0, LINE, SYS, 0, 0},
    {0x82208, 6, SIMPLEX_TX, 0, 0, 0, 1 << 0, SYS, LINE, 1, 1},
    {0x82208, 7, SIMPLEX_TX, 0, 1, 1, 1 << 1, SYS, LINE, 0, 0}
};

const FURIA_PKG_LANE_CFG_t glb_lanes_82209[8] = {
    {0x82209, 0, SIMPLEX_TX, 0, 1, 1, 1 << 1, SYS, LINE, 0, 0},
    {0x82209, 1, SIMPLEX_TX, 0, 0, 0, 1 << 0, SYS, LINE, 0, 0},
    {0x82209, 2, SIMPLEX_TX, 0, 0, 0, 1 << 0, LINE, SYS, 1, 1},
    {0x82209, 3, SIMPLEX_TX, 0, 1, 1, 1 << 1, LINE, SYS, 1, 1},
    {0x82209, 4, SIMPLEX_TX, 0, 2, 2, 1 << 2, LINE, SYS, 0, 0},
    {0x82209, 5, SIMPLEX_TX, 0, 3, 3, 1 << 3, LINE, SYS, 0, 0},
    {0x82209, 6, SIMPLEX_TX, 0, 3, 3, 1 << 3, SYS, LINE, 0, 0},
    {0x82209, 7, SIMPLEX_TX, 0, 2, 2, 1 << 2, SYS, LINE, 1, 1}
};

const FURIA_PKG_LANE_CFG_t glb_lanes_82073[8] = {
    {0x82073, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82073, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82073, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82073, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
    {0x82073, 4, DUPLEX_XCVR, 1, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82073, 5, DUPLEX_XCVR, 1, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82073, 6, DUPLEX_XCVR, 1, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82073, 7, DUPLEX_XCVR, 1, 0, 0, 1<<0, SYS, LINE, 0, 0}
};

const FURIA_PKG_LANE_CFG_t glb_lanes_82381[8] = {
    {0x82381, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82381, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82381, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82381, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
    {0x82381, 4, DUPLEX_XCVR, 1, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82381, 5, DUPLEX_XCVR, 1, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82381, 6, DUPLEX_XCVR, 1, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82381, 7, DUPLEX_XCVR, 1, 0, 0, 1<<0, SYS, LINE, 0, 0}
};
const FURIA_PKG_LANE_CFG_t glb_lanes_82383[8] = {
    {0x82383, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82383, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82383, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82383, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
    {0x82383, 4, DUPLEX_XCVR, 1, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82383, 5, DUPLEX_XCVR, 1, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82383, 6, DUPLEX_XCVR, 1, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82383, 7, DUPLEX_XCVR, 1, 0, 0, 1<<0, SYS, LINE, 0, 0}
};

const FURIA_PKG_LANE_CFG_t glb_lanes_82385[8] = {
    {0x82385, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82385, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82385, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82385, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
    {0x82385, 4, DUPLEX_XCVR, 1, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82385, 5, DUPLEX_XCVR, 1, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82385, 6, DUPLEX_XCVR, 1, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82385, 7, DUPLEX_XCVR, 1, 0, 0, 1<<0, SYS, LINE, 0, 0}
};
const FURIA_PKG_LANE_CFG_t glb_lanes_82071[4] = {
    {0x82071, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82071, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82071, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82071, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
};
const FURIA_PKG_LANE_CFG_t glb_lanes_82380[4] = {
    {0x82380, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82380, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82380, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82380, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
};
const FURIA_PKG_LANE_CFG_t glb_lanes_82072[8] = {
    {0x82072, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82072, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82072, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82072, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
    {0x82072, 4, DUPLEX_XCVR, 1, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82072, 5, DUPLEX_XCVR, 1, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82072, 6, DUPLEX_XCVR, 1, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82072, 7, DUPLEX_XCVR, 1, 0, 0, 1<<0, SYS, LINE, 0, 0}
};
const FURIA_PKG_LANE_CFG_t glb_lanes_82212[12] = {
  {0x82212, 0, SIMPLEX_TX,  0, 0, 0, 1<<0, LINE, SYS, 0, 0 },
  {0x82212, 1, SIMPLEX_TX,  0, 1, 1, 1<<1, LINE, SYS, 0, 0 },
  {0x82212, 2, SIMPLEX_TX,  1, 0, 0, 1<<0, LINE, SYS, 1, 1 },
  {0x82212, 3, SIMPLEX_TX,  1, 1, 1, 1<<1, LINE, SYS, 0, 0 },
  {0x82212, 4, SIMPLEX_TX,  1, 2, 2, 1<<2, LINE, SYS, 1, 1 },
  {0x82212, 5, SIMPLEX_TX,  1, 3, 3, 1<<3, LINE, SYS, 0, 0 },
  {0x82212, 6, SIMPLEX_TX,  0, 1, 1, 1<<1, SYS,  LINE,1, 1 },
  {0x82212, 7, SIMPLEX_TX,  0, 0, 0, 1<<0, SYS,  LINE,0, 0 },
  {0x82212, 8, SIMPLEX_TX,  0, 2, 2, 1<<2, LINE, SYS, 1, 1 },
  {0x82212, 9, SIMPLEX_TX,  0, 3, 3, 1<<3, LINE, SYS, 0, 0 },
  {0x82212, 10, SIMPLEX_TX, 1, 3, 3, 1<<3, SYS,  LINE,1, 1 },
  {0x82212, 11, SIMPLEX_TX, 1, 2, 2, 1<<2, SYS,  LINE,0, 0 }
};


const FURIA_PKG_LANE_CFG_t* const glb_package_array[MAX_NUM_PACKAGES] = {
    glb_lanes_82208,
    glb_lanes_82209,
    glb_lanes_82073,
    glb_lanes_82381,
    glb_lanes_82385,
    glb_lanes_82071,
    glb_lanes_82380,
    glb_lanes_82072,
    glb_lanes_82212,
    glb_lanes_82383,
    NULL
}; 

FURIA_PHY_LIST_t glb_phy_list[MAX_NUM_PHYS];


