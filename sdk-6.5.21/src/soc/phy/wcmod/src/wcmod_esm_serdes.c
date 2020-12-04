/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *
 * Notes: Shim layer for wcmod driver to be used by ESM serdes 
 */ 

#include <sal/types.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include "phydefs.h"
#include <soc/phyreg.h>
#include "wcmod.h"
#include "wcmod_defines.h"
#include "wcmod_main.h"
#include "wcmod_extra.h"
#include "wcmod_phyreg.h"
#include "wcmod_address.h"

#define WCMOD_NUM_ESM_SERDES_DEV 6 /* 6 Serdes devices x 4 lanes each */
#define WCMOD_NUM_LANES_PER_SERDES_DEV 4
#define WCMOD_ESM_SERDES_MIN_LANE 0
#define WCMOD_ESM_SERDES_MAX_LANE 23
#define ESM_SERDES_MAX_FAKE_PORTS (WCMOD_NUM_ESM_SERDES_DEV * WCMOD_NUM_LANES_PER_SERDES_DEV)


/**************************************************************
 *  Port Block     Sub-port  MDIO   MDIO      MDIO   MDIO
 *                           Bus    Addr      Bus    Addr
 *                           (ext)  (ext)     (int)  (int)
 **************************************************************
    
    TCAM Serdes0    0           4   0x1 (1)     4   0x1 (1)
                    1           4   0x2 (2)     4   0x2 (2)
                    2           4   0x3 (3)     4   0x3 (3)
                    3           4   0x4 (4)     4   0x4 (4)
    TCAM Serdes1    4           4   0x5 (5)     4   0x5 (5)
                    5           4   0x6 (6)     4   0x6 (6)
                    6           4   0x7 (7)     4   0x7 (7)
                    7           4   0x8 (8)     4   0x8 (8)
    TCAM Serdes2    8           4   0x9 (9)     4   0x9 (9)
                    9           4   0xa (10)    4   0xa (10)
                    10          4   0xb (11)    4   0xb (11)
                    11          4   0xc (12)    4   0xc (12)
    TCAM Serdes3    12          4   0xd (13)    4   0xd (13)
                    13          4   0xe (14)    4   0xe (14)
                    14          4   0xf (15)    4   0xf (15)
                    15          4   0x10 (16)   4   0x10 (16)
    TCAM Serdes4    16          4   0x11 (17)   4   0x11 (17)
                    17          4   0x12 (18)   4   0x12 (18)
                    18          4   0x13 (19)   4   0x13 (19)
                    19          4   0x14 (20)   4   0x14 (20)
    TCAM Serdes5    20          4   0x15 (21)   4   0x15 (21)
                    21          4   0x16 (22)   4   0x16 (22)
                    22          4   0x17 (23)   4   0x17 (23)
                    23          4   0x18 (24)   4   0x18 (24)
 */
#if 0  /* Multi port - NOT recommended/Tested */
int _triumph3_esm_serdes_int_phy_addr[] = {
    0x181, /*TCAM SERDES 0 */
    0x182, /*TCAM SERDES 0 */
    0x183, /*TCAM SERDES 0 */
    0x184, /*TCAM SERDES 0 */
    0x185, /*TCAM SERDES 1 */
    0x186, /*TCAM SERDES 1 */
    0x187, /*TCAM SERDES 1 */
    0x188, /*TCAM SERDES 1 */
    0x189, /*TCAM SERDES 2 */
    0x18A, /*TCAM SERDES 2 */
    0x18B, /*TCAM SERDES 2 */
    0x18C, /*TCAM SERDES 2 */
    0x18D, /*TCAM SERDES 3 */
    0x18E, /*TCAM SERDES 3 */
    0x18F, /*TCAM SERDES 3 */
    0x190, /*TCAM SERDES 3 */
    0x191, /*TCAM SERDES 4 */
    0x192, /*TCAM SERDES 4 */
    0x193, /*TCAM SERDES 4 */
    0x194, /*TCAM SERDES 4 */
    0x195, /*TCAM SERDES 5 */
    0x196, /*TCAM SERDES 5 */
    0x197, /*TCAM SERDES 5 */
    0x198, /*TCAM SERDES 5 */
};
#else /* Use Lane 0 MDIO for each lane in a quad & AER addressing */
int _triumph3_esm_serdes_int_phy_addr[] = {
    0x181, /*TCAM SERDES 0 */
    0x181, /*TCAM SERDES 0 */
    0x181, /*TCAM SERDES 0 */
    0x181, /*TCAM SERDES 0 */
    0x185, /*TCAM SERDES 1 */
    0x185, /*TCAM SERDES 1 */
    0x185, /*TCAM SERDES 1 */
    0x185, /*TCAM SERDES 1 */
    0x189, /*TCAM SERDES 2 */
    0x189, /*TCAM SERDES 2 */
    0x189, /*TCAM SERDES 2 */
    0x189, /*TCAM SERDES 2 */
    0x18D, /*TCAM SERDES 3 */
    0x18D, /*TCAM SERDES 3 */
    0x18D, /*TCAM SERDES 3 */
    0x18D, /*TCAM SERDES 3 */
    0x191, /*TCAM SERDES 4 */
    0x191, /*TCAM SERDES 4 */
    0x191, /*TCAM SERDES 4 */
    0x191, /*TCAM SERDES 4 */
    0x195, /*TCAM SERDES 5 */
    0x195, /*TCAM SERDES 5 */
    0x195, /*TCAM SERDES 5 */
    0x195, /*TCAM SERDES 5 */
};


#endif

/* WCMOD control structure.
   This structure sets the context for 
   wcmod driver.
*/
wcmod_st wcmod_esm_serdes_ctrl_s;

/* WCMOD config structure
   This structure stores the config
   information for the esm serdes/wcmod
*/
WCMOD_DEV_CFG_t wcmod_cfg;

/*
 * Function:
 *      wcmod_esm_serdes_phy_addr_get
 * Purpose:
 *     Get the phy address for the given port
 *
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Logical port for serdes lanes #.
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_phy_addr_get(int unit, int port) 
{
    if (SOC_UNIT_VALID(unit) && ((port >=0) && (port < ESM_SERDES_MAX_FAKE_PORTS))) {
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            return (_triumph3_esm_serdes_int_phy_addr[port]);
        }
#endif
    }
    return 0;
}

/*
 * Function:
 *      wcmod_esm_serdes_config
 * Purpose:
 *      Gets the config settings for 
 *      esm_serdes
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Logical port for serdes lanes #.
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_config(int unit, int port)
{
#ifdef INCLUDE_XGXS_WCMOD
    uint16 num_core, i;

    /* Lane Polarity */
    wcmod_cfg.txpol = soc_property_get(unit, "esm_serdes_tx_polarity_flip", 0x0); 
    wcmod_cfg.rxpol = soc_property_get(unit, "esm_serdes_rx_polarity_flip", 0x0);
#ifdef BCM_TRIUMPH3_SUPPORT
    /* Lane analog transmit parameters */
    for (i = WCMOD_ESM_SERDES_MIN_LANE; i < WCMOD_ESM_SERDES_MAX_LANE; i++) {
        wcmod_cfg.idriver[i] = soc_property_suffix_num_get(unit, i, spn_ESM_SERDES_DRIVER_CURRENT,
                                                                                        "lane", 4);
        wcmod_cfg.pdriver[i] = soc_property_suffix_num_get(unit, i, spn_ESM_SERDES_PRE_DRIVER_CURRENT,
                                                                                        "lane", 4);
        wcmod_cfg.preemph[i] = soc_property_suffix_num_get(unit, i, spn_ESM_SERDES_PRECURSOR_TAP,
                                                                                        "lane", 0) & 0xf;
        wcmod_cfg.preemph[i] |= (soc_property_suffix_num_get(unit, i, spn_ESM_SERDES_MAIN_TAP,
                                                                                        "lane", 40) & 0x3f) << 4;
        wcmod_cfg.preemph[i] |= (soc_property_suffix_num_get(unit, i, spn_ESM_SERDES_POSTCURSOR_TAP,
                                                                                        "lane", 7) & 0x1f) << 10;
    }
#endif
    /* Lane mapping */
    num_core = WCMOD_NUM_ESM_SERDES_DEV;
    
    /* Tx lane map */
    for (i = 0; i < num_core; i++) {
        wcmod_cfg.txlane_map[i] = soc_property_port_suffix_num_get(unit, port, 
                                                i, spn_ESM_SERDES_TX_LANE_MAP,
                                                "core", 0x3210);
    }

    /* Rx lane map */   
#ifdef BCM_TRIUMPH3_SUPPORT
        /* Tx and Rx are asymmetric for TR3 - 24 Tx lanes and 12 Rx lanes
           Only Core 3,4,5 are used for Rx lanes but this information is 
           transparent to the customers. They would specifiy core 0,1 and 2. 
           S/w needs to apply the user Rx settings for 0,1 and 2 to cores 3,4 
           and 5.
       */ 
       if (SOC_IS_TRIUMPH3(unit)) {
           wcmod_cfg.rxlane_map[0] = 0x3210; 
           wcmod_cfg.rxlane_map[1] = 0x3210; 
           wcmod_cfg.rxlane_map[2] = 0x3210; 
           wcmod_cfg.rxlane_map[3] =  soc_property_port_suffix_num_get(unit, 
                                           port, 0, spn_ESM_SERDES_RX_LANE_MAP,
                                          "core", 0x3210);

           wcmod_cfg.rxlane_map[4] =  soc_property_port_suffix_num_get(unit, 
                                           port, 1, spn_ESM_SERDES_RX_LANE_MAP,
                                          "core", 0x3210);

           wcmod_cfg.rxlane_map[5] = soc_property_port_suffix_num_get(unit, 
                                           port, 2, spn_ESM_SERDES_RX_LANE_MAP,
                                          "core", 0x3210);
 
       } else  
#endif
       {
           for (i = 0; i < num_core; ++i) {
               wcmod_cfg.rxlane_map[i] = soc_property_port_suffix_num_get(unit, 
                                           port, i, spn_ESM_SERDES_RX_LANE_MAP,
                                          "core", 0x3210);
           }
       }
       
    return SOC_E_NONE;
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/
}


/*
 * Function:
 *      wcmod_esm_serdes_control_init
 * Purpose:
 *     Sets up the wcmod structure - initializes
 *     various fields. 
 *     Custom setting for ESM serdes
 *
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Logical port for serdes lanes #.
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_control_init(int unit, int port) {

#ifdef INCLUDE_XGXS_WCMOD
    wcmod_st            *ws;      /* wcmod structure    */
    uint16 core_num;
    ws = &wcmod_esm_serdes_ctrl_s;
    
    ws->unit = unit;
    ws->port = port; 
    ws->phy_ad = wcmod_esm_serdes_phy_addr_get(unit, port);
    ws->mdio_type = WCMOD_MDIO_CL22; 
    ws->port_type = WCMOD_INDEPENDENT;
    ws->verbosity = 0;
    ws->spd_intf = WCMOD_SPD_PCSBYP_6P25G;
    ws->lane_num_ignore = FALSE;
    ws->read = soc_esw_miim_read; 
    ws->write = soc_esw_miim_write;
    ws->tx_pol = (wcmod_cfg.txpol >> port) & 0x1;
    ws->rx_pol = (wcmod_cfg.rxpol >> port) & 0x1;

    core_num = port/WCMOD_NUM_LANES_PER_SERDES_DEV;
    ws->lane_swap = (wcmod_cfg.txlane_map[core_num] << 16) | (wcmod_cfg.rxlane_map[core_num]);

    ws->this_lane = (port >= WCMOD_NUM_LANES_PER_SERDES_DEV) ? 
                                (port % WCMOD_NUM_LANES_PER_SERDES_DEV) : port;
    switch(ws->this_lane) {
        case 0:
             ws->lane_select = WCMOD_LANE_0_0_0_1;
             break;
         case 1: 
             ws->lane_select = WCMOD_LANE_0_0_1_0;
             break;
         case 2: 
             ws->lane_select = WCMOD_LANE_0_1_0_0;
             break;
         case 3: 
             ws->lane_select = WCMOD_LANE_1_0_0_0;
             break;
        default:
             return SOC_E_PARAM;
    }

    return SOC_E_NONE;
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/
}

/*
 * Function:
 *     wcmod_esm_serdes_probe 
 * Purpose:
 *     Probes and confirms if the device 
 *     is connected and is the right one
 *
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Logical port for serdes lanes #.
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_probe(int unit, int port) {
#ifdef INCLUDE_XGXS_WCMOD
    int rv;
    uint16      serdes_id0;
   
    wcmod_st *ws = &wcmod_esm_serdes_ctrl_s;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("REVID_READ", ws, &rv));

    serdes_id0 = ws->accData;
    
    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) != MODEL_ESM_SERDES) { 
      return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/
}

/*
 * Function:
 *     wcmod_esm_serdes_analog_param_set
 * Purpose:
 *      Settings addresses (lane number set through the AER):
 *	   Tap_main[5:0] -----> 0x806A[15:10]
 *	   Tap_pre[3:2] ------> 0x8063[15:14]
 *	   Tap_pre[1:0] ------> 0x8063[11:10]
 *	   Tap_post[4:0] -----> 0x806A[9:5]
 *	   I_Drive[3:0] ------> 0x8067[11:8]
 *	   I_Predrive[3:0] ---> 0x8067[7:4]
 *       In the wcmod_esm_serdes_config() function, the preemph member was used as:
 *	   preemph[3:0] = Tap_pre[3:0]
 *	   preemph[9:4] = Tap_main[5:0]
 *	   preemph[14:10] = Tap_post[4:0]
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Logical port for serdes lanes #
 *      ws - Warpcore context storing information
 * Returns:
 *      SOC_E_NONE
 */
#ifdef BCM_TRIUMPH3_SUPPORT
int
wcmod_esm_serdes_analog_param_set(int unit, int port, wcmod_st *ws){
    uint32 addr;
    uint16 data, mask;
    addr = AERBLK_AER_ADDR;
    data = ws->this_lane;
    mask = AERBLK_AER_ANALOG_TX_MASK;
    WC40_REG_MODIFY(ws->unit, ws, 0x00, addr, data, mask);
    addr = TX0_TX_DRIVER_ADDR + (0x10 * ws->this_lane);
    data = wcmod_cfg.idriver[port] << TX0_TX_DRIVER_IDRIVER_SHIFT;
    mask = TX0_TX_DRIVER_IDRIVER_MASK;
    WC40_REG_MODIFY(ws->unit, ws, 0x00, addr, data, mask);
    addr = TX0_TX_DRIVER_ADDR + (0x10 * ws->this_lane);
    data = wcmod_cfg.pdriver[port] << TX0_TX_DRIVER_IPREDRIVER_SHIFT;
    mask = TX0_TX_DRIVER_IPREDRIVER_MASK;
    WC40_REG_MODIFY(ws->unit, ws, 0x00, addr, data, mask);
    addr = TX0_ANATXMDATA1_ADDR + (0x10 * ws->this_lane);
    data = (((wcmod_cfg.preemph[port] & 0x000c) >>2) <<
           TX0_ANATXMDATA1_TXFIRTAPPRE_HIGH_SHIFT) |
           ((wcmod_cfg.preemph[port] & 0x0003) <<
           TX0_ANATXMDATA1_TXFIRTAPPRE_LOW_SHIFT);
    mask = TX0_ANATXMDATA1_TXFIRTAPPRE_MASK;
    WC40_REG_MODIFY(ws->unit, ws, 0x00, addr, data, mask);
    addr = TX0_ANATXACONTROL6_ADDR + (0x10 * ws->this_lane);
    data = ((wcmod_cfg.preemph[port] & 0x03f0) >> 4) <<
           TX0_ANATXACONTROL6_TXFIRTAPMAIN_SHIFT;
    mask = TX0_ANATXACONTROL6_TXFIRTAPMAIN_MASK;
    WC40_REG_MODIFY(ws->unit, ws, 0x00, addr, data, mask);
    addr = TX0_ANATXACONTROL6_ADDR + (0x10 * ws->this_lane);
    data = ((wcmod_cfg.preemph[port] & 0x7c00) >> 10) <<
            TX0_ANATXACONTROL6_TXFIRTAPPOST_SHIFT;
    mask = TX0_ANATXACONTROL6_TXFIRTAPPOST_MASK;
    WC40_REG_MODIFY(ws->unit, ws, 0x00, addr, data, mask);
    addr = AERBLK_AER_ADDR;
    data = 0;
    mask = AERBLK_AER_ANALOG_TX_MASK;
    WC40_REG_MODIFY(ws->unit, ws, 0x00, addr, data, mask);
    return SOC_E_NONE;
}
#endif
/*
 * Function:
 *     wcmod_esm_serdes_ind_init
 * Purpose:
 *     Init per independant lane 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Logical port for serdes lanes #.
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_ind_init(int unit, int port) {
#ifdef INCLUDE_XGXS_WCMOD
    wcmod_st * ws;
    int rv;
    ws = &wcmod_esm_serdes_ctrl_s;
    /* 
      Configure and initialize the resource shared by all 4 lanes.
     */
    if(ws->this_lane ==0) {
       /*
         Exercise it only once for a core. Since it is a simple and fixed
         configuration implementation for esm serdes, the simple way
         to accomplish this is to exercise it during lane0(this_lane==0) 
         init 
        */
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_ind_init_common(ws, 0)); /* Skip firmware loading */
    }

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_independent_lane_init(ws));
#ifdef BCM_TRIUMPH3_SUPPORT
    SOC_IF_ERROR_RETURN
        (wcmod_esm_serdes_analog_param_set(unit, port, ws));
#endif
    /* Set fixed speed - 6.25G */ 
    SOC_IF_ERROR_RETURN
        (wcmod_tier1_selector("SET_SPD_INTF", ws, &rv));

    return SOC_E_NONE;
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/
}


/*
 * Function:
 *     wcmod_esm_serdes_init
 * Purpose:
 *     Initializes all the esm serdes lanes
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Logical port for serdes lanes #.
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_init(int unit) {
#ifdef INCLUDE_XGXS_WCMOD
    int port_index = 0;
    int rv = SOC_E_NONE;
    rv = wcmod_esm_serdes_config(unit, port_index);
    if( SOC_FAILURE(rv)) {
       return rv; 
    }

    /* 1.  Initialize control structure - 'wcmod_st' for all the fake ports 
     *
     * PLEASE NOTE - Fake port is interchangeably used to address each lane of 
     * a serdes. Since every esm serdes lane has a different mdio address but 
     * no actual port assigned to it, hence it is also called a fake port. 
 */
    for(port_index=0; port_index < ESM_SERDES_MAX_FAKE_PORTS; ++port_index) {

        rv = wcmod_esm_serdes_control_init(unit, port_index); 
        if( SOC_FAILURE(rv)) {
            break;
        }

        rv = wcmod_esm_serdes_probe(unit, port_index);
        if( SOC_FAILURE(rv)) {
            break;
        }
        
        rv = wcmod_esm_serdes_ind_init(unit, port_index);
        if( SOC_FAILURE(rv)) {
            break;
        }

    }
    return rv;
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/
}

/*
 * Function:
 *     wcmod_esm_serdes_fifo_reset
 * Purpose:
 *     Resets the TX phases FIFO on all ESM serdes cores
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_fifo_reset(int unit) {
#ifdef INCLUDE_XGXS_WCMOD
    int         port_index = 0;
    int         rv = SOC_E_NONE; 
    wcmod_st    *temp_ws;

    for(port_index = 0; port_index < ESM_SERDES_MAX_FAKE_PORTS; port_index += 4) {
        rv = wcmod_esm_serdes_control_init(unit, port_index); 
        if(SOC_FAILURE(rv)) {
            break;
        }
        
        temp_ws = &wcmod_esm_serdes_ctrl_s;
        
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX0_ANATXACONTROL0r(unit, temp_ws,
                                             TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                             TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX0_ANATXACONTROL0r(unit, temp_ws, 0,
                                             TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX1_ANATXACONTROL0r(unit, temp_ws,
                                             TX1_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                             TX1_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX1_ANATXACONTROL0r(unit, temp_ws, 0,
                                             TX1_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX2_ANATXACONTROL0r(unit, temp_ws,
                                             TX2_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                             TX2_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX2_ANATXACONTROL0r(unit, temp_ws, 0,
                                             TX2_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX3_ANATXACONTROL0r(unit, temp_ws,
                                             TX3_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                             TX3_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX3_ANATXACONTROL0r(unit, temp_ws, 0,
                                             TX3_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
    }
   return rv; 
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/
}

/*
 * Function:
 *     wcmod_esm_serdes_control_set
 * Purpose:
 *
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      lane - Logical port for serdes lanes #.
 *      type - Control type
 *      value- Value
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_control_set(int unit, int lane, soc_phy_control_t type, uint32 *value) {
#ifdef INCLUDE_XGXS_WCMOD
    wcmod_st *ws;
    int rv = SOC_E_PARAM;
    int port = lane;

    if( (port < WCMOD_ESM_SERDES_MIN_LANE) || (port > WCMOD_ESM_SERDES_MAX_LANE)) {
        return SOC_E_PARAM;
    }

    rv = wcmod_esm_serdes_control_init(unit, port);

    ws = &wcmod_esm_serdes_ctrl_s;
    
    switch(type) {
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            rv = _phy_wcmod_control_prbs_polynomial_set(ws, *value);
            break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            rv = _phy_wcmod_control_prbs_tx_invert_data_set(ws, *value);
            break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
            /* TX_ENABLE does both tx and rx */
            rv = _phy_wcmod_control_prbs_enable_set(ws, *value);
            break; 
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            rv = SOC_E_NONE;
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;    
    }
    return rv;
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/
}

/*
 * Function:
 *     wcmod_esm_serdes_control_get
 * Purpose:
 *
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      lane - Logical port for serdes lanes #.
 *      type - Control type
 *      value- Value
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_control_get(int unit, int lane, soc_phy_control_t type, uint32 *value) {
#ifdef INCLUDE_XGXS_WCMOD
    wcmod_st *ws;
    int rv;
    int port = lane;

    if( (port < WCMOD_ESM_SERDES_MIN_LANE) || (port > WCMOD_ESM_SERDES_MAX_LANE)) {
        return SOC_E_PARAM;
    }
    
    rv = wcmod_esm_serdes_control_init(unit, port);

    ws = &wcmod_esm_serdes_ctrl_s;

    switch(type) {
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            rv = _phy_wcmod_control_prbs_polynomial_get(ws, value);
            break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            rv = _phy_wcmod_control_prbs_tx_invert_data_get(ws, value);
            break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE: /* fall through */
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE: /* fall through */
            rv = _phy_wcmod_control_prbs_enable_get(ws, value);
            break;
        case SOC_PHY_CONTROL_PRBS_RX_STATUS:
            rv = _phy_wcmod_control_prbs_rx_status_get(ws, value);
            break;
        case SOC_PHY_CONTROL_DUMP:
           rv = wcmod_uc_status_dump (unit, port, NULL);
           break;     
        default:
            rv = SOC_E_UNAVAIL;
            break;    
    }
    return rv;
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/

}

/*
 * Function:
 *     wcmod_esm_serdes_decouple_prbs_set
 * Purpose:
 *     Decoupled prbs - separately configure
 *     prbs settings on tx & rx lane separately
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      lane - Logical port for serdes lanes #.
 *      tx   - logical tx lane number 
 *      rx   - logical rx lane number
 *      poly - Polynomial
 *      invert  
 * Returns:
 *      SOC_E_NONE
 */
int
wcmod_esm_serdes_decouple_prbs_set(int unit, int lane, int tx, int rx, int poly, int invert) {

#ifdef INCLUDE_XGXS_WCMOD

/* Per lane control bit mask : 
  This bit mask should match the internal handling logic of the
  driver function */
#define ESM_SERDES_DECOUPLE_PRBS_TX_LANE    0x80
#define ESM_SERDES_DECOUPLE_PRBS_RX_LANE    0x40
#define ESM_SERDES_DECOUPLE_PRBS_DCPL_EN    0x20
#define ESM_SERDES_DECOUPLE_PRBS_EN         0x10
#define ESM_SERDES_DECOUPLE_PRBS_INV        0x08
#define ESM_SERDES_DECOUPLE_PRBS_ORDER      0x07

    wcmod_st *ws;
    int rv;
    int port = lane;
    int value = ESM_SERDES_DECOUPLE_PRBS_DCPL_EN | ESM_SERDES_DECOUPLE_PRBS_EN;

    if( (port < WCMOD_ESM_SERDES_MIN_LANE) || (port > WCMOD_ESM_SERDES_MAX_LANE)) {
        return SOC_E_PARAM;
    }
    
    rv = wcmod_esm_serdes_control_init(unit, port);

    ws = &wcmod_esm_serdes_ctrl_s;

    if(tx) {
        value |= ESM_SERDES_DECOUPLE_PRBS_TX_LANE; 
    }
    if(rx) {
        value |= ESM_SERDES_DECOUPLE_PRBS_RX_LANE; 
    }
    if(invert) {
        value |= ESM_SERDES_DECOUPLE_PRBS_INV; 
    }

    /* polynomial */
    value |= (poly & ESM_SERDES_DECOUPLE_PRBS_ORDER);

    /* lane should be between [0,3], or ws->per_lane_control will have undefined behaviour by left-shifting for more than 31 bits */
    if(lane>=0 && lane<=3){
        ws->per_lane_control = value << (lane * 8);
    }
    else{
        return SOC_E_ERROR;
    }
    
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_DECOUPLE_CONTROL", ws, &rv));

    return SOC_E_NONE;
#else /*INCLUDE_XGXS_WCMOD*/
   return SOC_E_UNAVAIL;
#endif /*INCLUDE_XGXS_WCMOD*/

}

