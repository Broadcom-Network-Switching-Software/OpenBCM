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
#include "huracan_cfg_seq.h"
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
    /*
      { 
            chip_id,
            pkg_lane_num, 
            lane_type,{
            
                        if_side_line { 
                                       
                                        tx {die,lane},
                                                      
                                        rx{die,lane}
                                     } ,
                                       
                        if_side_sys {
     
                                       tx{die,lane},
                                                       
                                       rx{die,lane}
                                                    
                                    }
                     }
                                   
       }
     */

const HURACAN_PKG_LANE_CFG_t glb_lanes_82109[] = {
                                   
                        /*<-------line--------->,<------system---------->*/
                              /*<-tx-> <-rx->*/      
    {0x82109,0,SIMPLEX_TX,{ { { {0,0}, {0,0} } },{ { {0,0}, {0,0} } } } },
    {0x82109,1,SIMPLEX_TX,{ { { {0,1}, {0,1} } },{ { {0,1}, {0,1} } } } },
    {0x82109,2,SIMPLEX_TX,{ { { {0,2}, {0,2} } },{ { {0,2}, {0,2} } } } },
    {0x82109,3,SIMPLEX_TX,{ { { {0,3}, {0,3} } },{ { {0,3}, {0,3} } } } },
    {0x82109,4,SIMPLEX_TX,{ { { {1,0}, {1,0} } },{ { {1,0}, {1,0} } } } },
    {0x82109,5,SIMPLEX_TX,{ { { {1,1}, {1,1} } },{ { {1,1}, {1,1} } } } },
    {0x82109,6,SIMPLEX_TX,{ { { {1,2}, {1,2} } },{ { {1,2}, {1,2} } } } },
    {0x82109,7,SIMPLEX_TX,{ { { {1,3}, {1,3} } },{ { {1,3}, {1,3} } } } },
};

const HURACAN_PKG_LANE_CFG_t glb_lanes_82181[8] = {
    {0x82181,0,DUPLEX_XCVR,{ { { {1,0}, {0,3} } },{ { {0,3}, {1,0} } } } },
    {0x82181,1,DUPLEX_XCVR,{ { { {1,1}, {0,2} } },{ { {0,2}, {1,1} } } } },
    {0x82181,2,DUPLEX_XCVR,{ { { {1,2}, {0,1} } },{ { {0,1}, {1,2} } } } },
    {0x82181,3,DUPLEX_XCVR,{ { { {1,3}, {0,0} } },{ { {0,0}, {1,3} } } } },
    {0x82181,4,DUPLEX_XCVR,{ { { {3,0}, {2,3} } },{ { {2,3}, {3,0} } } } },
    {0x82181,5,DUPLEX_XCVR,{ { { {3,1}, {2,2} } },{ { {2,2}, {3,1} } } } },
    {0x82181,6,DUPLEX_XCVR,{ { { {3,2}, {2,1} } },{ { {2,1}, {3,2} } } } },
    {0x82181,7,DUPLEX_XCVR,{ { { {3,3}, {2,0} } },{ { {2,0}, {3,3} } } } },
};
const HURACAN_PKG_LANE_CFG_t* const glb_pkg_array[MAX_NUM_PACKAGES] = {
    glb_lanes_82109,
    glb_lanes_82181,
    NULL
}; 

const chip_id_quad_info chipid_quad_info[MAX_NUM_PACKAGES-1] = {
    {
        0x82109,
        2,
        {0,1}
    },
      
    {
        0x82181,
        4,
        {0,1,2,3,}
    }
};


