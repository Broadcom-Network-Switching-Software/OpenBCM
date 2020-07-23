/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        viper.c
 * Purpose:     Support Broadcom 28nm XGXS/SGMIIPLUS2 internal SerDes
 */

/*
 *   This module implements an NTSW SDK Phy driver for the VIPER Serdes.
 *  
 *   LAYERING.
 *
 *   This driver is built on top of the PHYMOD library, which is a PHY
 *   driver library that can operate on a family of PHYs, including
 *   VIPER.  PHYMOD can be built in a standalone enviroment and be
 *   provided independently from the SDK.  PHYMOD APIs consist of
 *   "core" APIs and "phy" APIs.
 *
 *
 *   KEY IDEAS AND MAIN FUNCTIONS
 *
 *   Some key ideas in this architecture are:
 *
 *   o A PHMOD "phy" consists of one more more lanes, all residing in a single
 *     core.  An SDK "phy" is a logical port, which can consist of one or
 *     more lanes in a single core, OR, can consist of multiples lanes in
 *     more than one core.
 *   o PHYMOD code is "stateless" in the sense that all calls to PHYMOD
 *     APIs are fully parameterized and no per-phy state is held by a PHYMOD
 *     driver.
 *   o PHYMOD APIs do not map 1-1 with SDK .pd_control_set or .pd_control_get
 *     APIs (nor ".pd_xx" calls, nor to .phy_viper_per_lane_control_set and
 *     .phy_viper_per_lane_control_get APIs.
 *
 *   The purpose of this code, then, is to provide the necessary translations
 *   between the SDK interfaces and the PHYMOD interfaces.  This includes:
 * 
 *   o Looping over the cores in a logical PHY in order to operate on the
 *     supporting PHMOD PHYs
 *   o Determining the configuration data for the phy, based on programmed
 *     defaults and SOC properties.
 *   o Managing the allocation and freeing of phy data structures required for
 *     working storage.  Locating these structures on procedure invocation.
 *   o Mapping SDK API concepts to PHYMOD APIs.  In some cases, local state is
 *     required in this module in order to present the legacy API.
 *
 * 
 *   MAIN DATA STRUCTURES
 * 
 *   phy_ctrl_t
 *   The PHY control structure defines the SDK notion of a PHY
 *   (existing) structure owned by SDK phy driver modules.  In order
 *   to support PHYMOD PHYs, one addition was made to this structure
 *   (soc_phymod_ctrl_t phymod_ctrl;)
 *
 *   viper_config_t
 *   Driver specific data.  This structure is used by viper to hold
 *   logical port specific working storage.
 *
 *   soc_phymod_ctrl_t
 *   PHYMOD drivers.  Resides in phy_ctrl_t specifies how many cores
 *   are in this phy and contains an array of pointers to
 *   soc_phymod_phy_t structures, which define a PHYMOD PHY.
 *
 *   soc_phymod_phy_t
 *   This structure contains a pointer to phymod_phy_access_t and a
 *   pointer to the associated soc_phymod_core_t.  Instances if this
 *   structure are created during device probe/init and are maintained
 *   by the SOC.
 *
 *   soc_phymod_core_t
 *   This structure contains per-core information.  Multiple PHYMOD
 *   PHYS can point to a single core.
 *
 *   phymod_phy_access_t
 *   This structure contains information about how to read/write PHY
 *   registers.  A required parameter for PHYMOD PHY APIs
 * 
 *   phymod_core_access_t
 *   This structure contains information about how to read/write PHY
 *   registers.  A required parameter for PHYMOD core APIs.
 */
#include "phydefs.h"      /* Must include before other phy related includes */ 
#if defined(INCLUDE_XGXS_VIPER)
#include "phyconfig.h"     /* Must include before other phy related includes */
#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
#include <soc/phy/phymod_ids.h>


#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "xgxs.h"
#include "serdesid.h"


#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_util.h>
#include <phymod/chip/bcmi_viper_xgxs_sym.h>
#include <phymod/chip/bcmi_sgmiip2x4_serdes_sym.h>
#include <phymod/chip/viper.h>


#include "../../../libs/phymod/chip/viper/tier1/viper_err_code.h"

#define NUM_LANES             4    /* num of lanes per core */
#define MAX_NUM_LANES         4    /* max num of lanes per port */
#define VIPER_NO_CFG_VALUE     (-1)

#define VIPERX          1

typedef struct viper_speed_config_s {
    uint32  port_refclk_int;
    int     speed;
    int     port_num_lanes;
    int     scrambler_en;  
    uint32  line_interface;
    int     fiber_pref;
    int     pll_divider_req;
} viper_speed_config_t;

#define NUM_PATTERN_DATA_INTS    8
typedef struct viper_pattern_s {
    uint32 pattern_data[NUM_PATTERN_DATA_INTS];
    uint32 pattern_length;
} viper_pattern_t;

#define VIPER_LANE_NAME_LEN   30

typedef struct {
    uint16 serdes_id0;
    char   name[VIPER_LANE_NAME_LEN];
} viper_dev_info_t;

/* index to TX drive configuration table for each VCO setting.
 */
typedef enum txdrv_inxs {
    TXDRV_XAUI_INX,     /* 10G XAUI mode  */
    TXDRV_SGMII_INX,    /* 1G SGMII mode  */
    TXDRV_AN_INX,       /* a common entry for autoneg mode */
    TXDRV_DFT_INX,      /* temp for any missing speed modes */
    TXDRV_LAST_INX      /* always last */
} TXDRV_INXS_t;

#define TXDRV_ENTRY_NUM     (TXDRV_LAST_INX)
#define MAX_NUM_LANES        4

/*
   Config - per logical port
*/
typedef struct {
    phymod_polarity_t               phy_polarity_config;
    phymod_phy_reset_t              phy_reset_config;
    viper_pattern_t                 pattern;
    viper_speed_config_t            speed_config;

    int fiber_pref;                 /* spn_SERDES_FIBER_PREF */
    int cl37an;                     /* spn_PHY_AN_C37 */
    int auto_medium;
    viper_dev_info_t info;          /*serdes id info */
    phymod_tx_t tx_drive[TXDRV_ENTRY_NUM];
    phymod_tx_t ln_txparam[MAX_NUM_LANES] ;

} viper_config_t;


#define VIPER_IF_NULL   (1 << SOC_PORT_IF_NULL)
#define VIPER_IF_SR     (1 << SOC_PORT_IF_SR)
#define VIPER_IF_KR     (1 << SOC_PORT_IF_KR)
#define VIPER_IF_KR4    (1 << SOC_PORT_IF_KR4)
#define VIPER_IF_CR     (1 << SOC_PORT_IF_CR)
#define VIPER_IF_CR4    (1 << SOC_PORT_IF_CR4)
#define VIPER_IF_XAUI   (1 << SOC_PORT_IF_XAUI)

#define VIPER_CL37_W_1G      3
#define VIPER_CL37_WO_BAM    2
#define VIPER_CL37_W_BAM     1
#define VIPER_CL37_DISABLE   0

STATIC int phy_viper_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_viper_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability);
STATIC int _phy_viper_enable_set(int unit, soc_port_t port, int enable);

/*
 * Function:
 *      _viper_reg_read
 * Doc:
 *      register read operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to read
 *      val             - (OUT) read value
 */
STATIC int 
_viper_reg_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    uint16 data16;
    int rv;
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;
    
    rv = core->read(core->unit, core_addr, reg_addr, &data16);
    *val = data16;
    PHYMOD_VDBG(VIPER_DBG_REGACC,NULL,("%-22s: core_addr: 0x%08x reg_addr: 0x%08x, data: 0x%04x\n", __func__, core_addr, reg_addr, *val ));
    return rv;
}

/*
 * Function:
 *      _viper_reg_write
 * Doc:
*      register write operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to write
 *      val             - (IN)  write value
 */
STATIC int 
_viper_reg_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;
    uint16 data16 = (uint16)val;
    uint16 mask = (uint16)(val >> 16);

    if (mask) {
        if (core->wrmask) {
            return core->wrmask(core->unit, core_addr, reg_addr, data16, mask);
        }
        (void)core->read(core->unit, core_addr, reg_addr, &data16);
        data16 &= ~mask;
        data16 |= (val & mask);
    }
        PHYMOD_VDBG(VIPER_DBG_REGACC,NULL,("-22%s: core_addr: 0x%08x reg_addr: 0x%08x, data: 0x%04x\n", __func__, core_addr, reg_addr, val ));

    return core->write(core->unit, core_addr, reg_addr, data16);
}



#define IS_DUAL_LANE_PORT(_pc) ((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT)
#define IND_LANE_MODE(_pc) ((_pc)->phy_mode == PHYCTRL_ONE_LANE_PORT)  
#define MAIN0_SERDESID_REV_LETTER_SHIFT  (14)
#define MAIN0_SERDESID_REV_NUMBER_SHIFT  (11)

#define MODEL_MASK            0x3f
#define MODEL_XGXS_10G_028    0x8
#define MODEL_SGMIIPLUS2_X4   0xf
/*
 * Function:
 *      viper_show_serdes_info
 * Purpose:
 *      Show Serdes information.
 * Parameters:
 *      pc             - (IN)  phyctrl  oject
 *      pInfo          - (IN)  device info object
 *      rev_id         - (IN)  serdes revid
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
viper_show_serdes_info(phy_ctrl_t *pc, viper_dev_info_t *pInfo, phymod_core_info_t *core_info)
{
    uint32_t serdes_id0, len;

    pInfo->serdes_id0 = core_info->serdes_id ;

    /* This is Viper device */
    serdes_id0 = pInfo->serdes_id0;
    if ((serdes_id0 & MODEL_MASK) == MODEL_XGXS_10G_028) {
        sal_strlcpy(pInfo->name,"VIPERX-", sizeof(pInfo->name)); 
    } else if ((serdes_id0 & MODEL_MASK) == MODEL_SGMIIPLUS2_X4){
        sal_strlcpy(pInfo->name,"VIPERG-", sizeof(pInfo->name));
    }
    len = sal_strlen(pInfo->name);
    /* add rev letter */
    pInfo->name[len++] = 'A' + ((serdes_id0 >>
                          MAIN0_SERDESID_REV_LETTER_SHIFT) & 0x3);
    /* add rev number */
    pInfo->name[len++] = '0' + ((serdes_id0 >>
                          MAIN0_SERDESID_REV_NUMBER_SHIFT) & 0x7);
    pInfo->name[len++] = '/';
    pInfo->name[len++] = (pc->chip_num / 10) % 10 + '0';
    pInfo->name[len++] = pc->chip_num % 10 + '0';
    pInfo->name[len++] = '/';

    /* phy_mode: 0 single port mode, port uses all four lanes 
     *           1 dual port mode, port uses 2 lane
     *           2 quad port mode, port uses 1 lane
     */
    if (IS_DUAL_LANE_PORT(pc)) { /* dual-xgxs mode */
        if (pc->lane_num < 2) { /* the first dual-xgxs port */
            pInfo->name[len++] = '0';
            pInfo->name[len++] = '-';
            pInfo->name[len++] = '1';
        } else {
            pInfo->name[len++] = '2';
            pInfo->name[len++] = '-';
            pInfo->name[len++] = '3';
        }
    } else if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT) {
        pInfo->name[len++] = pc->lane_num + '0';
    } else {
        pInfo->name[len++] = '4';
    }
    pInfo->name[len] = 0;  /* string terminator */

    if (len > VIPER_LANE_NAME_LEN) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("VIPER info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                   len,VIPER_LANE_NAME_LEN, pc->unit, pc->port));
        return SOC_E_MEMORY;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      viper_speed_to_interface_config_get
 * Purpose:     
 *      Convert speed to interface_config struct
 * Parameters:
 *      unit                - BCM unit number.
 *      port                - Port number.
 *      speed               - speed to convert
 *      interface_config    - output interface config struct
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int 
viper_speed_to_interface_config_get(const phymod_phy_access_t *phy, viper_speed_config_t* speed_config, phymod_phy_inf_config_t* interface_config, TXDRV_INXS_t* tx_index)
{
    int fiber_pref;
    fiber_pref = speed_config->fiber_pref;

    SOC_IF_ERROR_RETURN(phymod_phy_inf_config_t_init(interface_config));

    interface_config->interface_modes = 0;
    interface_config->data_rate       = speed_config->speed;
    interface_config->pll_divider_req = speed_config->pll_divider_req ;

    *tx_index = TXDRV_DFT_INX;

    if (fiber_pref) {
        PHYMOD_INTF_MODES_FIBER_SET(interface_config);
    }    
    switch (speed_config->speed) {
        case 10:
            interface_config->interface_type = phymodInterfaceSGMII;
            *tx_index = TXDRV_DFT_INX;
            break;
        case 100:
        case 1000:
            if (speed_config->fiber_pref) { 
                interface_config->interface_type = phymodInterface1000X;
                *tx_index = TXDRV_DFT_INX;
            } else {
                interface_config->interface_type = phymodInterfaceSGMII;
                *tx_index = TXDRV_SGMII_INX;
            }
            break;
        case 2500:  
             interface_config->interface_type = phymodInterface1000X; 
             *tx_index = TXDRV_DFT_INX; 
             break;
        case 10000:
            interface_config->interface_type = phymodInterfaceXAUI;
            *tx_index = TXDRV_XAUI_INX;
            break;
       default:
            return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      viper_config_init
 * Purpose:     
 *      Determine phy configuration data for purposes of PHYMOD initialization.
 * 
 *      A side effect of this procedure is to save some per-logical port
 *      information in (viper_cfg_t *) &pc->driver_data;
 *
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 *      logical_lane_offset   - starting logical lane number
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
viper_config_init(int unit, soc_port_t port, const phymod_phy_access_t* phy, 
                 int logical_lane_offset,
                 phymod_core_init_config_t *core_init_config, 
                 phymod_phy_init_config_t  *pm_phy_init_config)
{
    phy_ctrl_t *pc;
    viper_speed_config_t *speed_config;
    viper_config_t *pCfg;
    phymod_tx_t *p_tx;
    int port_refclk_int;
    int port_num_lanes;
    int core_num;
    int phy_num_lanes;
    int lane_map_rx = 0, lane_map_tx = 0;
    int i;
    int fiber_pref = 0;
    uint32_t preemphasis;
    TXDRV_INXS_t tx_index = TXDRV_DFT_INX;

    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (viper_config_t *) pc->driver_data;

    /* extract data from SOC_INFO */
    port_refclk_int = SOC_INFO(unit).port_refclk_int[port];
    port_refclk_int = 156;  
    port_refclk_int = soc_property_port_get(unit, port,spn_XGXS_LCPLL_XTAL_REFCLK, port_refclk_int);
    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];
    
    /* figure out how many lanes are in this phy */
    core_num = (port / 4);
    phy_num_lanes = port_num_lanes - logical_lane_offset;
    if (phy_num_lanes > MAX_NUM_LANES) {
       phy_num_lanes = MAX_NUM_LANES;
    }
    
    /* 
        CORE configuration
    */
    phymod_core_init_config_t_init(core_init_config);

    core_init_config->lane_map.num_of_lanes = NUM_LANES;
    core_init_config->flags = PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY;
    lane_map_rx = soc_property_port_get(unit, port,
                                        spn_XGXS_RX_LANE_MAP, 0x3210);

    for (i=0; i<NUM_LANES; i++) {
         core_init_config->lane_map.lane_map_rx[i] = (lane_map_rx >> (i * 4 /* 4 bit per lane */)) & 0xf;
    }

    lane_map_tx = soc_property_port_get(unit, port,
                                        spn_XGXS_TX_LANE_MAP, 0x3210);
    for (i=0; i<NUM_LANES; i++) {
         core_init_config->lane_map.lane_map_tx[i] = (lane_map_tx >> (i * 4 /*4 bit per lane*/)) & 0xf;
    }
    /* next get the tx/rx polairty info */
    core_init_config->polarity_map.tx_polarity = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_TX_POLARITY_FLIP, 0);
    core_init_config->polarity_map.rx_polarity = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_RX_POLARITY_FLIP, 0);
    speed_config = &(pCfg->speed_config);
    speed_config->port_refclk_int  = port_refclk_int;
    speed_config->speed  = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, pc->speed_max);
    speed_config->port_num_lanes   = phy_num_lanes;
    speed_config->line_interface   = soc_property_port_get(unit, port, spn_SERDES_IF_TYPE, 0);
    speed_config->fiber_pref = soc_property_port_get(unit, port, spn_SERDES_FIBER_PREF, speed_config->fiber_pref);
    speed_config->pll_divider_req      = soc_property_port_get(unit, port, spn_XGXS_PHY_PLL_DIVIDER, 0);

    SOC_IF_ERROR_RETURN
        (viper_speed_to_interface_config_get(phy, speed_config, &(core_init_config->interface), &tx_index));

    /* 
        PHY configuration
    */
    for (i=0; i<MAX_NUM_LANES; i++) {
        pCfg->ln_txparam[i+logical_lane_offset].main  = -1 ;
        pCfg->ln_txparam[i+logical_lane_offset].post  = -1 ;
    }

    for (i = 0; i < TXDRV_ENTRY_NUM; i++) {
        p_tx = &pCfg->tx_drive[i];
        SOC_IF_ERROR_RETURN(phymod_tx_t_init(p_tx));
    }
    /* set the default tx parameters */
    p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
    p_tx->main = 0x32;
    p_tx->post = 0;

    p_tx = &pCfg->tx_drive[TXDRV_AN_INX];
    p_tx->main = 0x32;
    p_tx->post = 0x0;

    p_tx = &pCfg->tx_drive[TXDRV_XAUI_INX];
    p_tx->main = 0x32;
    p_tx->post = 0x0;

    phymod_phy_init_config_t_init(pm_phy_init_config);
    for (i = 0; i < 4; i++) {
        pm_phy_init_config->tx[i].main = pCfg->tx_drive[tx_index].main;
        pm_phy_init_config->tx[i].post = pCfg->tx_drive[tx_index].post;
    }

    pm_phy_init_config->polarity.rx_polarity
                                 = soc_property_port_get(unit, port,
                                   spn_PHY_XAUI_RX_POLARITY_FLIP, 
                                   pm_phy_init_config->polarity.rx_polarity);
    pm_phy_init_config->polarity.tx_polarity
                                 = soc_property_port_get(unit, port,
                                   spn_PHY_XAUI_TX_POLARITY_FLIP,
                                   pm_phy_init_config->polarity.tx_polarity);
    pm_phy_init_config->rx_los.rx_los_en
                                 = soc_property_port_get(unit, port,
                                   spn_SERDES_RX_LOS, 0);
    pm_phy_init_config->rx_los.rx_los_invert_en
                                 = soc_property_port_get(unit, port,
                                   spn_SERDES_RX_LOS_INVERT, 0);

    pm_phy_init_config->rx_swing = soc_property_port_get(unit, port, spn_SERDES_RX_LARGE_SWING, 0);

    for (i = 0; i < MAX_NUM_LANES; i++) {
        /*first get the preemphasis */
        preemphasis = 0;
        preemphasis = soc_property_port_suffix_num_get(unit, port,
                                    i + core_num * 4, spn_SERDES_PREEMPHASIS,
                                    "lane", preemphasis); 
        pm_phy_init_config->tx[i].main = (preemphasis & 0xff00) >> 8;
        pm_phy_init_config->tx[i].post = (preemphasis & 0xff0000) >> 16;
    }


    pm_phy_init_config->an_en = TRUE;

    /* By default enable cl37 */
    pCfg->cl37an = TRUE;

    pCfg->cl37an = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C37, pCfg->cl37an); /* no L: C37, not CL37 */

    /* 
        phy_ctrl_t configuration (LOGICAL PORT BASED)
        Only do this once, for the first core of the logical port
    */
    if (core_num == 0) {
        /* phy_mode, PHYCTRL_MDIO_ADDR_SHARE, PHY_FLAGS_INDEPENDENT_LANE */
        if (port_num_lanes == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (port_num_lanes == 2) {
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (port_num_lanes == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }
        /* PHY_FLAGS_FIBER */
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        if (!PHY_EXTERNAL_MODE(unit, port)) {

            fiber_pref = FALSE;
            if (PHY_FIBER_MODE(unit, port) ||
                (!PHY_INDEPENDENT_LANE_MODE(unit, port)) ||
                (pc->speed_max >= 10000)) {
                fiber_pref = TRUE;
            }
            fiber_pref = soc_property_port_get(unit, port,
                                               spn_SERDES_FIBER_PREF, fiber_pref);
            if (fiber_pref) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            }
        }

        
        PHY_FLAGS_SET(unit,port,PHY_FLAGS_SERVICE_INT_PHY_LINK_GET);

    }

    return SOC_E_NONE;
}

STATIC int
_viper_device_print(soc_phymod_core_t *core)
{
    phymod_core_access_t *pm_core;
    phymod_access_t *pm_acc;
    
    pm_core = &core->pm_core;
    pm_acc = &pm_core->access;
     
    if(core) {
        PHYMOD_VDBG(VIPER_DBG_MEM, pm_acc, ("ref_cnt=%0d core_p=%p\n", core->ref_cnt, (void *)core)) ;
    } else {
        PHYMOD_VDBG(VIPER_DBG_MEM, pm_acc, ("Error: no defined phymod core.\n"));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_init
 *  
 *      An SDK "phy" is a logical port, which can consist of from 1-10 lanes,
 *          (A phy with more than 4 lanes requires more than one core).
 *      Per-logical port information is saved in (viper_config_t *) &pc->driver_data.
 *      An SDK phy is implemented as one or more PHYMOD "phy"s.
 *  
 *      A PHYMOD "phy" resides completely within a single core, which can be
 *      from 1 to 4 lanes.
 *      Per-phymod phy information is kept in (soc_phymod_ctrl_t) *pc->phymod_ctrl
 *      A phymod phy points to its core.  Up to 4 phymod phys can be on a single core
 *  
 * Purpose:     
 *      Initialize a viper phy
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy = NULL;
    soc_phymod_core_t *core;
    viper_config_t *pCfg;
    viper_speed_config_t *speed_config;
    viper_dev_info_t *pInfo;
    soc_phy_info_t *pi;
    phymod_phy_inf_config_t interface_config;
    phymod_core_status_t core_status;
    phymod_core_info_t core_info;
    int idx;
    int logical_lane_offset;
    soc_port_ability_t ability; 
    TXDRV_INXS_t tx_index = TXDRV_DFT_INX;


    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pc->driver_data = (void*)(pc+1);
    pmc = &pc->phymod_ctrl;
    pCfg = (viper_config_t *) pc->driver_data;
    pInfo = &pCfg->info; 
    
    sal_memset(pCfg, 0, sizeof(*pCfg));
    speed_config = &(pCfg->speed_config);

    sal_memset(&ability, 0, sizeof(ability));

    /* Loop through all phymod phys that support this SDK phy */
    logical_lane_offset = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;
        core->pm_core.access.lane_mask = phy->pm_phy.access.lane_mask;
        _viper_device_print(core) ;
        /* determine configuration data structure to default values, based on SOC properties */
        SOC_IF_ERROR_RETURN
            (viper_config_init(unit, port, &phy->pm_phy,
                              logical_lane_offset,
                              &core->init_config, &phy->init_config));

        core_status.pmd_active = 0;
        if (!SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN
                (phymod_core_init(&core->pm_core, &core->init_config, &core_status));

        }

        if (!SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN
                (phymod_phy_init(&phy->pm_phy, &phy->init_config));
        }


        /*read serdes id info */
        SOC_IF_ERROR_RETURN
            (phymod_core_info_get(&core->pm_core, &core_info)); 


        /* for multicore phys, need to ratchet up to the next batch of lanes */
        logical_lane_offset += core->init_config.lane_map.num_of_lanes; 
    }

    /* fill up the pInfo table for displaying the serdes driver info */
    SOC_IF_ERROR_RETURN
        (viper_show_serdes_info(pc, pInfo, &core_info));

    /* retrieve chip level information for serdes driver info */
    pi = &SOC_PHY_INFO(unit, port);

    if (!PHY_EXTERNAL_MODE(unit, port)) {
        pi->phy_name = pInfo->name;
    }

    if (SOC_WARM_BOOT(unit)) {
        return SOC_E_NONE;
    }

    speed_config->pll_divider_req = 40;

    /* set the port to its max or init_speed */
    SOC_IF_ERROR_RETURN
        (viper_speed_to_interface_config_get(&phy->pm_phy, speed_config, &interface_config, &tx_index));

    SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_set(&phy->pm_phy,
                                         0 /* flags */, &interface_config));

    /* setup the port's an cap */
    SOC_IF_ERROR_RETURN
        (phy_viper_ability_local_get(unit, port, &ability));
 
    SOC_IF_ERROR_RETURN
        (phy_viper_ability_advert_set(unit, port, &ability));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_viper_init: u=%d p=%d\n"), unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_link_get
 * Purpose:
 *      Get layer2 connection status.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      link - address of memory to store link up/down state.
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;

    *link = 0;
    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;

    SOC_IF_ERROR_RETURN
        (phymod_phy_link_status_get(pm_phy, (uint32_t *) link));
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_viper_enable_set
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_enable_set(int unit, soc_port_t port, int enable)
{
    return (_phy_viper_enable_set( unit,  port, enable));
}

/*
 * Function:
 *      phy_viper_enable_get
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;
    phymod_phy_power_t        phy_power;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;

    SOC_IF_ERROR_RETURN(phymod_phy_power_get(pm_phy, &phy_power));
    if (phy_power.tx & phy_power.rx) {
        *enable = 1;
    } else { 
        *enable = 0; 
    } 

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_viper_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_viper_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t                *pc;
    viper_config_t            *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       idx,ln;
    viper_speed_config_t      speed_config;
    phymod_phy_inf_config_t   interface_config;
    TXDRV_INXS_t tx_index = TXDRV_DFT_INX;
    phymod_tx_t phymod_tx;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane ;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    if (speed == 0) {
        return SOC_E_NONE; 
    }

    pmc = &pc->phymod_ctrl;
    pCfg = (viper_config_t *) pc->driver_data;

    /* take a copy of core and phy configuration values, and set the desired speed */
    sal_memcpy(&speed_config, &pCfg->speed_config, sizeof(speed_config));
    speed_config.speed = speed;

    SOC_IF_ERROR_RETURN(phymod_tx_t_init(&phymod_tx));

    /* determine the interface configuration */
    phy = pmc->phy[0];
    SOC_IF_ERROR_RETURN
        (viper_speed_to_interface_config_get(&phy->pm_phy, &speed_config, &interface_config, &tx_index));

    /* now loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, &phy->pm_phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (phymod_util_lane_config_get(&pm_phy_copy.access, &start_lane, &num_lane));

        for(ln=0; ln<num_lane; ln++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane +ln) ;

            phymod_tx.main  = pCfg->tx_drive[tx_index].main;
            phymod_tx.post  = pCfg->tx_drive[tx_index].post;

            if( pCfg->ln_txparam[4*idx+ln].main >=0) {
                phymod_tx.main = pCfg->ln_txparam[4*idx+ln].main;
            }
            if( pCfg->ln_txparam[4*idx+ln].post >=0) {
                phymod_tx.post = pCfg->ln_txparam[4*idx+ln].post;
            }

            SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
        }

        SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_set(&phy->pm_phy,
                                             0 /* flags */, &interface_config));
    }

    /* record success */
    pCfg->speed_config.speed = speed;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_viper_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    /*viper_config_t            *pCfg; */
    int                       flag = 0;
    phymod_ref_clk_t          ref_clock = phymodRefClk156Mhz;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    /*pCfg = (viper_config_t *) pc->driver_data;*/

    pmc = &pc->phymod_ctrl;

    /* initialize the data structure */
    interface_config.data_rate = 0;

    /* now loop through all cores */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&phy->pm_phy,
                                         flag, ref_clock, &interface_config));
    *speed = interface_config.data_rate;
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_viper_duplex_set
 * Purpose:
 *      Set PHY duplex mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      duplex - current duplex mode
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_viper_duplex_set(int unit, soc_port_t port, int duplex)
{
    phy_ctrl_t* pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;
    phymod_duplex_mode_t duplex_mode;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    switch (duplex) {
        case SOC_PORT_DUPLEX_HALF:
            duplex_mode = phymodDuplexHalf;
            break;
        case SOC_PORT_DUPLEX_FULL:
            duplex_mode = phymodDuplexFull;
            break;
        default:
            duplex_mode = phymodDuplexFull;
            break;
    }

    for (idx = 0; idx < pmc->num_phys; idx++) {
         pm_phy = &pmc->phy[idx]->pm_phy;
         SOC_IF_ERROR_RETURN(phymod_phy_duplex_set(pm_phy, duplex_mode));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_duplex_get
 * Purpose:
 *      Get PHY duplex mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      duplex - current duplex mode
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_viper_duplex_get(int unit, soc_port_t port, int *duplex)
{
    phy_ctrl_t* pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_duplex_mode_t duplex_mode;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;

    /* Check first PHY only */
    pm_phy = &pmc->phy[0]->pm_phy;
    SOC_IF_ERROR_RETURN
        (phymod_phy_duplex_get(pm_phy, &duplex_mode));
    switch (duplex_mode) {
        case phymodDuplexHalf:
            *duplex = SOC_PORT_DUPLEX_HALF;
            break;
        case phymodDuplexFull:
            *duplex = SOC_PORT_DUPLEX_FULL;
            break;
       default:
            *duplex = SOC_PORT_DUPLEX_FULL;
            break;
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_viper_an_set
 * Purpose:     
 *      Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      an   - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX  _soc_triumph_tx
 */
STATIC int
phy_viper_an_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    viper_config_t            *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_autoneg_control_t  an;
    soc_info_t *si;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_control_t_init(&an);
    pmc = &pc->phymod_ctrl;
    pCfg = (viper_config_t *) pc->driver_data;
    si = &SOC_INFO(unit);

    /* for legacy purpose autodet is 1 by defalut */
    pCfg->auto_medium = 1;
    pCfg->auto_medium = soc_property_port_get(unit, port,
                              spn_SERDES_AUTOMEDIUM, pCfg->auto_medium);

    /* only request autoneg on the first core */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    an.enable = enable;
    an.num_lane_adv = si->port_num_lanes[port];
    an.an_mode = phymod_AN_MODE_NONE;
    if (pCfg->cl37an) {
        an.an_mode = phymod_AN_MODE_CL37;
    } else {
        if(pCfg->speed_config.fiber_pref) {
            an.an_mode = phymod_AN_MODE_CL37;
        } else {
            an.an_mode = phymod_AN_MODE_SGMII;
        }
    }
    if (pCfg->auto_medium) {
        PHYMOD_AN_F_AUTO_MEDIUM_DETECT_SET(&an);
    }
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_set(&phy->pm_phy, &an));

    return (SOC_E_NONE);
}

/*
 * Function:    
 *      phy_viper_an_get
 * Purpose:     
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_viper_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_autoneg_control_t an_control;
    int idx, an_complete;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;

    sal_memset(&an_control, 0x0, sizeof(an_control));

    idx = pmc->main_phy;
    pm_phy = &pmc->phy[idx]->pm_phy;
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_get(pm_phy, &an_control, (uint32_t *) &an_complete)); 

    if (an_control.enable) {
        *an = 1;
        *an_done = an_complete;
    } else {
        *an = 0;
        *an_done = 0;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_viper_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      ability - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_viper_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    viper_config_t            *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);

    pmc = &pc->phymod_ctrl;
    pCfg = (viper_config_t *) pc->driver_data;

    /* only set abilities on the first core */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }


    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    case SOC_PA_PAUSE_RX:
        /* an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE; */
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    }

    /* Set 1000MB CL37*/
    if (pCfg->cl37an){
        if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
            PHYMOD_AN_CAP_CL37_SET(&phymod_autoneg_ability);
        }
    } else {
        /* set the sgmii speed */
        if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_1000M;
        } else if(ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_100M;
        } else if(ability->speed_full_duplex & SOC_PA_SPEED_10MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_10M;
        } else {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_1000M;
       }

    }

    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_set(&phy->pm_phy, &phymod_autoneg_ability));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      ability - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_viper_ability_advert_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       reg_ability;
    _shr_port_mode_t          speed_full_duplex;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 
    phymod_cl37_sgmii_speed_t cl37_sgmii_speed;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    /* only get abilities from the first core */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_get(&phy->pm_phy, &phymod_autoneg_ability));
    
    cl37_sgmii_speed = phymod_autoneg_ability.sgmii_speed;
    reg_ability = phymod_autoneg_ability.capabilities;
    speed_full_duplex = 0;

    /* retrieve CL37 abilities */
    if (reg_ability == PHYMOD_AN_CAP_CL37){
        speed_full_duplex|= SOC_PA_SPEED_1000MB;
    }

    /* retrieve sgmii speed */
    if (cl37_sgmii_speed == phymod_CL37_SGMII_1000M){
        speed_full_duplex|= SOC_PA_SPEED_1000MB;
    } else if (cl37_sgmii_speed == phymod_CL37_SGMII_100M){
        speed_full_duplex|= SOC_PA_SPEED_100MB;
    } else if (cl37_sgmii_speed == phymod_CL37_SGMII_10M){
        speed_full_duplex|= SOC_PA_SPEED_10MB;
    } else {
        speed_full_duplex|= 0;
    }

    /* retrieve "pause" abilities */
    ability->pause = 0;
    if (reg_ability == PHYMOD_AN_CAP_ASYM_PAUSE) {
        ability->pause = SOC_PA_PAUSE_TX;
    } else if (reg_ability == (PHYMOD_AN_CAP_SYMM_PAUSE|PHYMOD_AN_CAP_ASYM_PAUSE)) {
        ability->pause = SOC_PA_PAUSE_RX;
    } else if (reg_ability == PHYMOD_AN_CAP_SYMM_PAUSE) {
        ability->pause = (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX);
    }

    ability->speed_full_duplex = speed_full_duplex;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_ability_remote_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      ability - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_viper_ability_remote_get(int unit, soc_port_t port,
                               soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       reg37_ability;
    int                       reg_ability;
    _shr_port_mode_t          speed_full_duplex;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    /* only get abilities from the first core */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_remote_ability_get(&phy->pm_phy, &phymod_autoneg_ability));

    speed_full_duplex = 0;

    /* retrieve CL37 abilities */
    reg37_ability = phymod_autoneg_ability.an_cap;
    speed_full_duplex|= PHYMOD_AN_CAP_1G_KX_GET(reg37_ability)? SOC_PA_SPEED_1000MB : 0;
    speed_full_duplex|= SOC_PA_SPEED_1000MB ;

    /* retrieve "pause" abilities */
    reg_ability = phymod_autoneg_ability.capabilities;

    ability->pause = 0;
    if (reg_ability == PHYMOD_AN_CAP_ASYM_PAUSE) {
        ability->pause = SOC_PA_PAUSE_TX;
    } else if (reg_ability == (PHYMOD_AN_CAP_SYMM_PAUSE|PHYMOD_AN_CAP_ASYM_PAUSE)) {
        ability->pause = SOC_PA_PAUSE_RX;
    } else if (reg_ability == PHYMOD_AN_CAP_SYMM_PAUSE) {
        ability->pause = (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX);
    }

    ability->speed_full_duplex = speed_full_duplex;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_lb_set
 * Purpose:
 *      Enable/disable PHY loopback mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackGlobal, enable)); 
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_viper_lb_get
 * Purpose:
 *      Get current PHY loopback mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    uint32 lb_enable;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;

    *enable = 0;

    /* Check first PHY only */
    pm_phy = &pmc->phy[0]->pm_phy;
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(pm_phy, phymodLoopbackGlobal, &lb_enable));
    if (lb_enable) {
        *enable = 1;
    }
     
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_viper_ability_local_get
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      ability - xx
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t          *pc; 
    viper_config_t             *pCfg;

    pc = INT_PHY_SW_STATE(unit, port);
    pCfg = (viper_config_t *) pc->driver_data;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0, sizeof(*ability));

    ability->loopback  = SOC_PA_LB_PHY;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->flags     = SOC_PA_AUTONEG;

    if (IND_LANE_MODE(pc)) { 
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        if (pCfg->speed_config.fiber_pref) {
            ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
            if (pc->speed_max >= 2500) {
                ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
            }
            ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
        } else {
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB;
            ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
        } 
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->eee       = SOC_PA_ABILITY_NONE;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = SOC_PA_AUTONEG;
    } else {
        if (pCfg->speed_config.fiber_pref) {
            ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
        } else {
            ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
        }
        ability->speed_full_duplex = SOC_PA_SPEED_10GB;
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->eee       = SOC_PA_ABILITY_NONE;
        ability->flags     = SOC_PA_ABILITY_NONE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_viper_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));
    return (SOC_E_NONE);
}


STATIC int
phy_viper_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t      *pc;
    viper_config_t *pCfg;

    pc = INT_PHY_SW_STATE(unit, port);
    pCfg = (viper_config_t *) pc->driver_data;

    pCfg->speed_config.line_interface = (uint32) pif;

    if (pif == SOC_PORT_IF_GMII) {
        pCfg->speed_config.fiber_pref = 1;
    } else if (pif == SOC_PORT_IF_SGMII){
        pCfg->speed_config.fiber_pref = 0;
    }

    return SOC_E_NONE;
}

STATIC int
phy_viper_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t                  *pc;
    soc_phymod_ctrl_t           *pmc;
    soc_phymod_phy_t            *phy;
    phymod_phy_inf_config_t     interface_config;
    viper_config_t               *pCfg;
    int                         flag = 0;
    phymod_ref_clk_t            ref_clock = phymodRefClk156Mhz;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (viper_config_t *) pc->driver_data;

    pmc = &pc->phymod_ctrl;

    /* initialize the data structure */
    sal_memset(&interface_config, 0x0, sizeof(phymod_phy_inf_config_t));

    /* now loop through all cores */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&phy->pm_phy,
                                         flag, ref_clock, &interface_config));
    switch (interface_config.interface_type) {
    case phymodInterfaceXAUI:
        *pif = _SHR_PORT_IF_XAUI;
        break;
    case phymodInterface1000X:
        *pif = SOC_PORT_IF_GMII;
        break;
    case phymodInterfaceKX:
        *pif = SOC_PORT_IF_KX;
        break;
    case phymodInterfaceSGMII:
    {
        if(interface_config.data_rate == 1000) {
            if (PHY_EXTERNAL_MODE(unit, port) || !(pCfg->speed_config.fiber_pref)) {
                *pif = SOC_PORT_IF_SGMII;
            } else {
                *pif = SOC_PORT_IF_GMII;
            }
        } else {
            *pif = SOC_PORT_IF_SGMII;
        }
        break;
    }
    default:
        *pif =  SOC_PORT_IF_SGMII;
        break;
    }

    return (SOC_E_NONE);

}


/*
 * Function:
 *      viper_pcs_status_dump
 * Purpose:
 *      display all the serdes related parameters
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
viper_pcs_status_dump(int unit, soc_port_t port, void *arg)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_pcs_info_dump(pm_phy, arg)); 
    }
    return (SOC_E_NONE);
}


/* 
 * given a pc (phymod_ctrl_t) and logical lane number, 
 *    find the correct soc_phymod_phy_t object and lane
 */
STATIC int
_viper_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, uint32_t lane, 
                        soc_phymod_phy_t **p_phy, uint32 *lane_map)
{
    phymod_phy_access_t *pm_phy;
    int idx, lnx, ln_cnt, found;
    uint32 lane_map_copy;

    /* Traverse lanes belonging to this port */
    found = 0;
    ln_cnt = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        lane_map_copy = pm_phy->access.lane_mask;
        for (lnx = 0; lnx < 4; lnx++) {
            if ((1 << lnx) & lane_map_copy) {
                if (ln_cnt == lane) {
                    found = 1;
                    break;
                }
                ln_cnt++;
            }
        }
        if (found) {
            *p_phy = pmc->phy[idx];
            *lane_map = (1 << lnx);
            break;
        }
    } 

    if (!found) {
        /* No such lane */
        return SOC_E_PARAM;
    }
    return (SOC_E_NONE);
}

/* 
 * viper_per_lane_preemphasis_set
 */
STATIC int
viper_per_lane_preemphasis_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    phymod_tx.main = (value >> 8) & 0xff;
    phymod_tx.post = (value >> 16) & 0xff;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * viper_preemphasis_set
 */
STATIC int
viper_preemphasis_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.main = (value >> 8) & 0xff;
        phymod_tx.post = (value >> 16) & 0xff;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));

    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_fir_main_set
 */
STATIC int
viper_tx_fir_main_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.main = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_fir_post_set
 */
STATIC int
viper_tx_fir_post_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.post = value;  
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

#if 0
/* 
 * viper_tx_fir_post2_set
 */
STATIC int
viper_tx_fir_post2_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.post2 = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_fir_post3_set
 */
STATIC int
viper_tx_fir_post3_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        phymod_tx.post3 = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));
    }

    return(SOC_E_NONE);
}
#endif


/* 
 * viper_per_lane_rx_dfe_tap_control_set
 */
STATIC int
viper_per_lane_rx_dfe_tap_control_set(soc_phymod_ctrl_t *pmc, int lane, int tap, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        /* this can only happen with a coding error */
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    phymod_rx.dfe[tap].enable = enable;
    phymod_rx.dfe[tap].value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));

    return(SOC_E_NONE);
}


/* 
 * viper_tx_lane_squelch
 */
STATIC int
viper_tx_lane_squelch(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        if (value == 1) {
            tx_control = phymodTxSquelchOn;
        } else {
            tx_control = phymodTxSquelchOff;
        }
        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_set(pm_phy, tx_control));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_rx_peak_filter_set
 */
STATIC int
viper_per_lane_rx_peak_filter_set(soc_phymod_ctrl_t *pmc, int lane, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    phymod_rx.peaking_filter.enable = TRUE;
    phymod_rx.peaking_filter.value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(pm_phy, &phymod_rx));

    return(SOC_E_NONE);
}

/* 
 * viper_rx_peak_filter_set
 */
STATIC int 
viper_rx_peak_filter_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_rx_t         phymod_rx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_rx_get(pm_phy, &phymod_rx));
        phymod_rx.peaking_filter.enable = TRUE;
        phymod_rx.peaking_filter.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(pm_phy, &phymod_rx));
    }

    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_rx_vga_set
 */
STATIC int
viper_per_lane_rx_vga_set(soc_phymod_ctrl_t *pmc, int lane, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    phymod_rx.vga.enable = TRUE;
    phymod_rx.vga.value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(pm_phy, &phymod_rx));

    return(SOC_E_NONE);
}


STATIC int
viper_rx_tap_release(soc_phymod_ctrl_t *pmc, int tap)
{
    phymod_phy_access_t *pm_phy;
    phymod_rx_t         phymod_rx;
    int                 idx;

    /* bounds check "tap" */
    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_rx_get(pm_phy, &phymod_rx));
        phymod_rx.dfe[tap].enable = FALSE;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(pm_phy, &phymod_rx));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_rx_tap_set
 */
STATIC int 
viper_rx_tap_set(soc_phymod_ctrl_t *pmc, int tap, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_rx_t         phymod_rx;
    int                 idx;

    /* bounds check "tap" */
    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_rx_get(pm_phy, &phymod_rx));
        phymod_rx.dfe[tap].enable = TRUE;
        phymod_rx.dfe[tap].value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(pm_phy, &phymod_rx));
    }

    return(SOC_E_NONE);
}

/* 
 * viper_pi_control_set
 */
STATIC int 
viper_pi_control_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_tx_override_t tx_override;
    int                  idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        phymod_tx_override_t_init(&tx_override);
        tx_override.phase_interpolator.enable = (value == 0) ? 0 : 1;
        tx_override.phase_interpolator.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_override_set(pm_phy, &tx_override));
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_polarity_set
 */
STATIC int 
viper_tx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    int                  idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&polarity, cfg_polarity, sizeof(polarity));
        polarity.tx_polarity = value;
        SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(pm_phy, &polarity));

        /* after successfully setting the parity, update the configured value */
        cfg_polarity->tx_polarity = value;
    }

    return(SOC_E_NONE);
}

/* 
 * viper_rx_polarity_set
 */

STATIC int 
viper_rx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    int                  idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&polarity, cfg_polarity, sizeof(polarity));
        polarity.rx_polarity = value;
        SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(pm_phy, &polarity));

        /* after successfully setting the parity, update the configured value */
        cfg_polarity->rx_polarity = value;
    }

    return(SOC_E_NONE);
}

/* 
 * viper_rx_reset
 */
STATIC int
viper_rx_reset(soc_phymod_ctrl_t *pmc, phymod_phy_reset_t *cfg_reset, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_reset_t  reset;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&reset, cfg_reset, sizeof(reset));
        reset.rx = (phymod_reset_direction_t) value;
        SOC_IF_ERROR_RETURN(phymod_phy_reset_set(pm_phy, &reset));

        /* after successfully setting the parity, update the configured value */
        cfg_reset->rx = (phymod_reset_direction_t) value;
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_reset
 */
STATIC int
viper_tx_reset(soc_phymod_ctrl_t *pmc, phymod_phy_reset_t *cfg_reset, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_reset_t  reset;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&reset, cfg_reset, sizeof(reset));
        reset.tx = (phymod_reset_direction_t) value;
        SOC_IF_ERROR_RETURN(phymod_phy_reset_set(pm_phy, &reset));

        /* after successfully setting the parity, update the configured value */
        cfg_reset->tx = (phymod_reset_direction_t) value;
    }

    return(SOC_E_NONE);
}


STATIC int
viper_lane_map_set(soc_phymod_ctrl_t *pmc, uint32 value){
    int idx;
    phymod_lane_map_t lane_map;

    lane_map.num_of_lanes = NUM_LANES;
    if(pmc->phy[0] == NULL){
        return(SOC_E_INTERNAL);
    }
    for (idx=0; idx < NUM_LANES; idx++) {
        lane_map.lane_map_rx[idx] = (value >> (idx * 4 /* 4 bit per lane */)) & 0xf;
    }
    for (idx=0; idx < NUM_LANES; idx++) {
        lane_map.lane_map_tx[idx] = (value >> (16 + idx * 4 /* 4 bit per lane */)) & 0xf;
    }
    SOC_IF_ERROR_RETURN(phymod_core_lane_map_set(&pmc->phy[0]->core->pm_core, &lane_map));
    return(SOC_E_NONE);
}


STATIC int
viper_lane_map_get(soc_phymod_ctrl_t *pmc, uint32 *value){
    int idx;
    phymod_lane_map_t lane_map;

    *value = 0;
    if(pmc->phy[0] == NULL){
        return(SOC_E_INTERNAL);
    }
    SOC_IF_ERROR_RETURN(phymod_core_lane_map_get(&pmc->phy[0]->core->pm_core, &lane_map));
    if(lane_map.num_of_lanes != NUM_LANES){
        return SOC_E_INTERNAL;
    }
    for (idx=0; idx < NUM_LANES; idx++) {
        *value += (lane_map.lane_map_rx[idx] << (idx * 4));
    }
    for (idx=0; idx < NUM_LANES; idx++) {
        *value += (lane_map.lane_map_tx[idx]<< (idx * 4 + 16 ));
    }
    
    return(SOC_E_NONE);
}



STATIC int
viper_pattern_len_set(soc_phymod_ctrl_t *pmc, uint32_t value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_pattern_t       phymod_pattern;
    int                    idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        phymod_pattern_t_init(&phymod_pattern);
        SOC_IF_ERROR_RETURN
            (phymod_phy_pattern_config_get(pm_phy, &phymod_pattern));
        phymod_pattern.pattern_len = value;
        SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_set(pm_phy, &phymod_pattern));
    }
    return(SOC_E_NONE);
}

STATIC int
viper_pattern_enable_set(soc_phymod_ctrl_t *pmc, uint32_t value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_pattern_t       phymod_pattern;
    int                    idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        phymod_pattern_t_init(&phymod_pattern);
        SOC_IF_ERROR_RETURN
            (phymod_phy_pattern_config_get(pm_phy, &phymod_pattern));
        SOC_IF_ERROR_RETURN(phymod_phy_pattern_enable_set(pm_phy,  value, &phymod_pattern));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_pattern_data_set 
 *    Sets 32 bits of the 256 bit data pattern.
 */
STATIC int
viper_pattern_data_set(int idx, viper_pattern_t *pattern, uint32 value)
{
    if ((idx<0) || (idx >= COUNTOF(pattern->pattern_data))) {
        return SOC_E_INTERNAL;
    }

    /* update pattern data */
    pattern->pattern_data[idx] = value;

    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_prbs_poly_set
 */
STATIC int
viper_per_lane_prbs_poly_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}


STATIC
int
viper_sdk_poly_to_phymod_poly(uint32 sdk_poly, phymod_prbs_poly_t *phymod_poly){
    switch(sdk_poly){
    case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1:
        *phymod_poly = phymodPrbsPoly31;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1:
        *phymod_poly = phymodPrbsPoly58;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/* 
 * viper_prbs_tx_poly_set
 */
STATIC int
viper_prbs_tx_poly_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    int                    idx;
    
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy,  flags, &prbs));
        SOC_IF_ERROR_RETURN(viper_sdk_poly_to_phymod_poly(value, &prbs.poly));
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_prbs_rx_poly_set
 */
STATIC int
viper_prbs_rx_poly_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    int                    idx;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));
        SOC_IF_ERROR_RETURN(viper_sdk_poly_to_phymod_poly(value, &prbs.poly));
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    }
    return(SOC_E_NONE);
}


/* 
 * viper_per_lane_prbs_tx_invert_data_set
 */
STATIC int
viper_per_lane_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_prbs_tx_invert_data_set
 */
STATIC int
viper_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    int                    idx;
    
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));
        prbs.invert = value;
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags,  &prbs));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_prbs_rx_invert_data_set
 */
STATIC int
viper_prbs_rx_invert_data_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    int                    idx;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags,  &prbs));
        prbs.invert = value;
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_prbs_tx_enable_set
 */
STATIC int
viper_per_lane_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}
/* 
 * viper_prbs_tx_enable_set
 */
STATIC int
viper_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    int                    idx;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(pm_phy, flags, value));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_prbs_rx_enable_set
 */
STATIC int
viper_prbs_rx_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    int                    idx;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(pm_phy, flags,  value));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_loopback_internal_set
 */
STATIC int 
viper_loopback_internal_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                    idx;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
    
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackGlobal, value));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_loopback_internal_pmd_set
 */
STATIC int 
viper_loopback_internal_pmd_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                    idx;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackGlobalPMD, value));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_loopback_remote_set
 */
STATIC int 
viper_loopback_remote_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                    idx;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackRemotePMD, value));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_loopback_remote_pcs_set
 */
STATIC int 
viper_loopback_remote_pcs_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                    idx;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackRemotePCS, value));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_fec_enable_set
 */
STATIC int 
viper_fec_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_set(pm_phy, value));
    }
    return(SOC_E_NONE);
}

/* 
 * viper_8b10b_set
 */
STATIC int
viper_8b10b_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_64b65b_set
 */
STATIC int
viper_64b65b_set(soc_phymod_ctrl_t *pmc, uint32 value)
{

   return(SOC_E_UNAVAIL);
}

/* 
 * viper_power_set
 */
STATIC int
viper_power_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        phymod_phy_power_t_init(&power);
        if (value) {
            power.tx = phymodPowerOn;
            power.rx = phymodPowerOn;
        } 
        else {
            power.tx = phymodPowerOff;
            power.rx = phymodPowerOff;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_power_set(pm_phy, &power));
    }

    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_rx_low_freq_filter_set
 */
STATIC int
viper_per_lane_rx_low_freq_filter_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_rx_low_freq_filter_set
 */
STATIC int
viper_rx_low_freq_filter_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;
    int                  idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_rx_get(pm_phy, &phymod_rx));

        phymod_rx.low_freq_peaking_filter.enable = TRUE;
        phymod_rx.low_freq_peaking_filter.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(pm_phy, &phymod_rx));
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_ppm_adjust_set
 */
STATIC int
viper_tx_ppm_adjust_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_tx_override_t tx_override;
    int                  idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        tx_override.phase_interpolator.enable = 1;
        tx_override.phase_interpolator.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_override_set(pm_phy, &tx_override));
    }
    return(SOC_E_NONE);
}

#if 0
/* 
 * viper_vco_freq_set
 */
STATIC int
viper__vco_freq_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    /*
The following 3 controls are used to override pre-defined speed in the phy.
What we discussed in SJ is that in case of rate which is not officially supported the driver will try to 
understand this parameters by itself.
Questions:
1. Do we want to support direct configuration of PLL\OS?
2. If yes - same API or new API

phymod_phy_interface_config_set (?)
    */

   return(SOC_E_UNAVAIL);
}
#endif
/* 
 * viper_pll_divider_set
 */
STATIC int
viper_pll_divider_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_oversample_mode_set
 */
STATIC int
viper_oversample_mode_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_ref_clk_set
 */
STATIC int
viper_ref_clk_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_rx_seq_toggle_set 
 */
STATIC int
viper_rx_seq_toggle_set(soc_phymod_ctrl_t *pmc)
{
    phymod_phy_access_t    *pm_phy;
    int                    idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }
        SOC_IF_ERROR_RETURN(phymod_phy_rx_restart(pm_phy));
    }

    return(SOC_E_NONE);
}

/* 
 * viper_eee_set 
 */
STATIC int
viper_eee_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t    *pm_phy;
    int                    idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }
        SOC_IF_ERROR_RETURN(phymod_phy_eee_set(pm_phy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
viper_control_linkdown_transmit_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t    *pm_phy;
    int                    idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }
        SOC_IF_ERROR_RETURN(phymod_phy_linkdown_transmit_set(pm_phy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
viper_control_linkdown_transmit_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
         return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_linkdown_transmit_get(pm_phy, value));

    return(SOC_E_NONE);
}

/* 
 * viper_driver_supply_set
 */
STATIC int
viper_driver_supply_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

#if 0
STATIC int
viper_user_speed_set(soc_phymod_ctrl_t *pmc, 
                    phymod_pcs_userspeed_mode_t mode, 
                    phymod_pcs_userspeed_param_t param, 
                    uint32 value)
{
    phymod_phy_access_t    *pm_phy;
    soc_phymod_phy_t       *p_phy;
    phymod_pcs_userspeed_config_t config ;
    int                    idx;
    uint32                 lane_map;
    
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, 0, &p_phy, &lane_map));
    config.mode          = mode ; 
    config.param         = param ;
    config.current_entry = (lane_map&0x1)? 0:((lane_map&0x2)? 1:((lane_map&0x4)? 2: ((lane_map&0x8)? 3:0)));
    config.value         = value ;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }
        
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }
        SOC_IF_ERROR_RETURN(phymod_phy_pcs_userspeed_set(pm_phy, &config));
    }
    
    return(SOC_E_NONE);
}


STATIC int
viper_user_speed_get(soc_phymod_ctrl_t *pmc, 
                    phymod_pcs_userspeed_mode_t mode, 
                    phymod_pcs_userspeed_param_t param, 
                    uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    soc_phymod_phy_t       *p_phy;
    phymod_pcs_userspeed_config_t config ;
    int                    idx;
    uint32                 lane_map;
    
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, 0, &p_phy, &lane_map));
    config.mode          = mode ; 
    config.param         = param ;
    config.current_entry = (lane_map&0x1)? 0:((lane_map&0x2)? 1:((lane_map&0x4)? 2: ((lane_map&0x8)? 3:0)));
    config.value         = 0 ;

    *value = 0 ;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }
        
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }
        
        SOC_IF_ERROR_RETURN(phymod_phy_pcs_userspeed_get(pm_phy, &config));
        *value = config.value ;
    }
    
    return(SOC_E_NONE);
}
#endif
/*
 * Function:
 *      phy_viper_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    viper_config_t       *pCfg;

    rv = SOC_E_UNAVAIL;

    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    /* locate phy control, phymod control, and the configuration data */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    pCfg = (viper_config_t *) pc->driver_data;

    switch(type) {

    case SOC_PHY_CONTROL_TX_FIR_PRE:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        rv = viper_tx_fir_main_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        rv = viper_tx_fir_post_set(pmc, value);
        break;
#if 0
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        rv = viper_tx_fir_post2_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        rv = viper_tx_fir_post3_set(pmc, value);
        break;
#endif
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = viper_per_lane_preemphasis_set(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = viper_per_lane_preemphasis_set(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = viper_per_lane_preemphasis_set(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = viper_per_lane_preemphasis_set(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = viper_preemphasis_set(pmc, value);
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;

    /* PRE_DRIVER CURRENT  not supported anymore */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;

    /* POST2_DRIVER CURRENT not supported anymore */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DUMP:
        /* "N/A: will be part of common phymod application" */
        /*     was rv = tscmod_uc_status_dump(unit, port); */
        break;

    /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = viper_tx_lane_squelch(pmc, value);
        break;

    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = viper_rx_peak_filter_set(pmc, value);
        break;

    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        rv = SOC_E_UNAVAIL;
        break;

    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = viper_rx_tap_set(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = viper_rx_tap_set(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = viper_rx_tap_set(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = viper_rx_tap_set(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = viper_rx_tap_set(pmc, 4, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:       /* $$$ tbd $$$ */
       rv = viper_rx_tap_release(pmc, 0);
       break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:       /* $$$ tbd $$$ */
       rv = viper_rx_tap_release(pmc, 1);
       break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:       /* $$$ tbd $$$ */
       rv = viper_rx_tap_release(pmc, 2);
       break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:       /* $$$ tbd $$$ */
       rv = viper_rx_tap_release(pmc, 3);
       break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:       /* $$$ tbd $$$ */
       rv = viper_rx_tap_release(pmc, 4);
       break;
    /* RX SLICER */
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_RX_D_SLICER:
        rv = SOC_E_UNAVAIL;
        break;

    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
        rv = viper_pi_control_set(pmc, value);
        break;

    /* POLARITY */
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = viper_rx_polarity_set(pmc, &pCfg->phy_polarity_config, value);;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = viper_tx_polarity_set(pmc, &pCfg->phy_polarity_config, value);
        break;

    /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        rv = viper_rx_reset(pmc, &pCfg->phy_reset_config, value);
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        rv = viper_tx_reset(pmc, &pCfg->phy_reset_config, value);
        break;
    /* LANE SWAP */
    case SOC_PHY_CONTROL_LANE_SWAP:
        rv = viper_lane_map_set(pmc, value);
        break;
    /* TX PATTERN */
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        rv = viper_pattern_len_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        rv = viper_pattern_enable_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        rv = viper_pattern_data_set(0, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        rv = viper_pattern_data_set(1, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        rv = viper_pattern_data_set(2, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        rv = viper_pattern_data_set(3, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        rv = viper_pattern_data_set(4, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        rv = viper_pattern_data_set(5, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        rv = viper_pattern_data_set(6, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        rv = viper_pattern_data_set(7, &pCfg->pattern, value);
        break;

    /* decoupled PRBS */
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        rv = viper_prbs_tx_poly_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        rv = viper_prbs_tx_invert_data_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        rv = viper_prbs_tx_enable_set(pmc, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        rv = viper_prbs_rx_poly_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        rv = viper_prbs_rx_invert_data_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        rv = viper_prbs_rx_enable_set(pmc, value);
        break;
    /* for legacy prbs usage mainly set both tx/rx the same */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = viper_prbs_tx_poly_set(pmc, value);
        rv = viper_prbs_rx_poly_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = viper_prbs_tx_invert_data_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = viper_prbs_tx_enable_set(pmc, value);
        rv = viper_prbs_rx_enable_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = viper_prbs_tx_enable_set(pmc, value);
        rv = viper_prbs_rx_enable_set(pmc, value);
        break;

    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        rv = viper_loopback_internal_set(pmc, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
        rv = viper_loopback_internal_pmd_set(pmc, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = viper_loopback_remote_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        rv = viper_loopback_remote_pcs_set(pmc, value);
        break;

    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        rv = viper_fec_enable_set(pmc, value);
        break;

    /* CUSTOM */
    case SOC_PHY_CONTROL_CUSTOM1:
       /* Obsolete ??
       DEV_CFG_PTR(pc)->custom1 = value? TRUE: FALSE ;
       rv = SOC_E_NONE; */
       rv = SOC_E_UNAVAIL;
       break;
    /* 8B10B */
    case SOC_PHY_CONTROL_8B10B:
        rv = viper_8b10b_set(pmc, value);
        break;

    /* 64B65B */
    case SOC_PHY_CONTROL_64B66B:
        rv = viper_64b65b_set(pmc, value);
        break;

    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = viper_power_set(pmc, value);
       break;

    /* RX_LOW_FREQ_PEAK_FILTER */
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
       rv = viper_rx_low_freq_filter_set(pmc, value);
       break;

    /* TX_PPM_ADJUST */
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
       rv = viper_tx_ppm_adjust_set(pmc, value);
       break;
#if 0
    /* VCO_FREQ */
    case SOC_PHY_CONTROL_VCO_FREQ:
       rv = viper_vco_freq_set(pmc, value);
       break;
#endif
    /* PLL_DIVIDER */
    case SOC_PHY_CONTROL_PLL_DIVIDER:
       rv = viper_pll_divider_set(pmc, value);
       break;

    /* OVERSAMPLE_MODE */
    case SOC_PHY_CONTROL_OVERSAMPLE_MODE:
       rv = viper_oversample_mode_set(pmc, value);
       break;

    /* REF_CLK */
    case SOC_PHY_CONTROL_REF_CLK:
       rv = viper_ref_clk_set(pmc, value);
       break;

    /* RX_SEQ_TOGGLE */
    case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
       rv = viper_rx_seq_toggle_set(pmc);
       break;

    /* DRIVER_SUPPLY */
    case SOC_PHY_CONTROL_DRIVER_SUPPLY:
       rv = viper_driver_supply_set(pmc, value);
       break;
    /* EEE */
    case SOC_PHY_CONTROL_EEE:
       rv = viper_eee_set(pmc, value);
       break;

    /* EEE */
#ifdef TSC_EEE_SUPPORT
    case SOC_PHY_CONTROL_EEE:
    case SOC_PHY_CONTROL_EEE_AUTO:
        rv = SOC_E_NONE;   /* not supported */
        break;
#endif
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
       rv = viper_control_linkdown_transmit_set(pmc, value);
       break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}


/* 
 * viper_tx_fir_pre_get
 */
STATIC int
viper_tx_fir_pre_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        *value = phymod_tx.pre;
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_fir_main_get
 */
STATIC int
viper_tx_fir_main_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        *value = phymod_tx.main;
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_fir_post_get
 */
STATIC int
viper_tx_fir_post_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        *value = phymod_tx.post;
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_fir_post2_get
 */
STATIC int
viper_tx_fir_post2_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        *value = phymod_tx.post2;
    }

    return(SOC_E_NONE);
}

/* 
 * viper_tx_fir_post3_get
 */
STATIC int
viper_tx_fir_post3_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
        *value = phymod_tx.post3;
    }

    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_preemphasis_get
 */
STATIC int
viper_per_lane_preemphasis_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.pre  | (phymod_tx.main << 8) | (phymod_tx.post << 16);

    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_driver_current_get
 */
STATIC int
viper_per_lane_driver_current_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.amp;

    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_prbs_poly_get
 */
STATIC int
viper_per_lane_prbs_poly_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_per_lane_prbs_tx_invert_data_get
 */
STATIC int
viper_per_lane_prbs_tx_invert_data_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_per_lane_prbs_tx_enable_get
 */
STATIC int
viper_per_lane_prbs_tx_enable_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_per_lane_prbs_rx_enable_get
 */
STATIC int
viper_per_lane_prbs_rx_enable_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_per_lane_prbs_rx_status_get
 */
STATIC int
viper_per_lane_prbs_rx_status_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   /* $$$ Need to add diagnostic API */
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_per_lane_rx_peak_filter_get
 */
STATIC int
viper_per_lane_rx_peak_filter_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    if (phymod_rx.peaking_filter.enable)
        *value = phymod_rx.peaking_filter.value;

    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_rx_vga_get
 */
STATIC int
viper_per_lane_rx_vga_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    if (phymod_rx.vga.enable)
        *value = phymod_rx.vga.value;

    return(SOC_E_NONE);
}

/* 
 * viper_per_lane_rx_dfe_tap_control_get
 */
STATIC int
viper_per_lane_rx_dfe_tap_control_get(soc_phymod_ctrl_t *pmc, int lane, int tap, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_viper_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        /* this can only happen with a coding error */
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    if (phymod_rx.dfe[tap].enable)
       *value = phymod_rx.dfe[tap].value;

    return(SOC_E_NONE);
}


/* 
 * viper_per_lane_rx_low_freq_filter_get
 */
STATIC int
viper_per_lane_rx_low_freq_filter_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_rx_peak_filter_get
 */
STATIC int 
viper_rx_peak_filter_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(pm_phy, &phymod_rx));
    *value = phymod_rx.peaking_filter.value;

    return(SOC_E_NONE);
}


/* 
 * viper_rx_tap_get
 */
STATIC int 
viper_rx_tap_get(soc_phymod_ctrl_t *pmc, int tap, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;

    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        /* this can only happen with a coding error */
        return SOC_E_INTERNAL;
    }
    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(pm_phy, &phymod_rx));
    *value = phymod_rx.dfe[tap].value;

    return(SOC_E_NONE);
}

/* 
 * viper_pi_control_get
 */
STATIC int 
viper_pi_control_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_tx_override_t tx_override;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_tx_override_get(pm_phy, &tx_override));
    *value = tx_override.phase_interpolator.value;

    return(SOC_E_NONE);
}

/* 
 * viper_rx_signal_detect_get
 */
STATIC int
viper_rx_signal_detect_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
   /* $$$ Need to add diagnostic API */
   return(SOC_E_UNAVAIL);
}

/* 
 * viper_rx_seq_done_get
 */
STATIC int
viper_rx_seq_done_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;

    /* just take the value from the first phy */
    if (pmc->phy[pmc->main_phy] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;

    if (pm_phy == NULL) {
         return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_pmd_locked_get(pm_phy, value));

    return(SOC_E_NONE);
}

/* 
 * viper_eee_get
 */
STATIC int
viper_eee_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
         return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_eee_get(pm_phy, value));

    return(SOC_E_NONE);
}


/* 
 * viper_rx_ppm_get
 */
STATIC int
viper_rx_ppm_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    /* $$$ Need to add diagnostic API */
    return(SOC_E_UNAVAIL);
}

/* 
 * viper_prbs_tx_poly_get
 */
STATIC int
viper_prbs_tx_poly_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));

    *value = (int) prbs.poly;

    /* convert from PHYMOD enum to SDK enum */
    switch(prbs.poly){
    case phymodPrbsPoly7:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
        break;
    case phymodPrbsPoly9:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
        break;
    case phymodPrbsPoly15:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
        break;
    case phymodPrbsPoly23:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
        break;
    case phymodPrbsPoly31:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
        break;
    case phymodPrbsPoly11:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
        break;
    case phymodPrbsPoly58:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1; 
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}
/* 
 * viper_prbs_rx_poly_get
 */
STATIC int
viper_prbs_rx_poly_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));

    *value = (int) prbs.poly;

    /* convert from PHYMOD enum to SDK enum */
    switch(prbs.poly){
    case phymodPrbsPoly7:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
        break;
    case phymodPrbsPoly9:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
        break;
    case phymodPrbsPoly15:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
        break;
    case phymodPrbsPoly23:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
        break;
    case phymodPrbsPoly31:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
        break;
    case phymodPrbsPoly11:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
        break;
    case phymodPrbsPoly58:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/* 
 * viper_prbs_tx_invert_data_get
 */
STATIC int
viper_prbs_tx_invert_data_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));
    *value = prbs.invert;

    return(SOC_E_NONE);
}

/* 
 * viper_prbs_rx_invert_data_get
 */
STATIC int
viper_prbs_rx_invert_data_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));
    *value = prbs.invert;

    return(SOC_E_NONE);
}


/* 
 * viper_prbs_tx_enable_get
 */
STATIC int
viper_prbs_tx_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(pm_phy, flags, value));

    return(SOC_E_NONE);
}

/* 
 * viper_prbs_rx_enable_get
 */
STATIC int
viper_prbs_rx_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(pm_phy, flags, value));

    return(SOC_E_NONE);
}

/* 
 * viper_prbs_rx_status_get
 */
STATIC int
viper_prbs_rx_status_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_prbs_status_t   prbs_tmp;
    int idx, prbs_lock, lock_loss, error_count ;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }

    prbs_lock   = 1 ; 
    lock_loss   = 0 ;
    error_count = 0 ;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        /* $$$ Need to add diagnostic API */
        SOC_IF_ERROR_RETURN
            (phymod_phy_prbs_status_get(pm_phy, 0, &prbs_tmp));

        if(prbs_tmp.prbs_lock==0) {
            prbs_lock = 0 ;
        } else {
            if(prbs_tmp.prbs_lock_loss) {
                lock_loss = 1 ;
            } else {
                error_count += prbs_tmp.error_count;
            }
        }
    }
    
    if (prbs_lock == 0) {
        *value = -1;
    } else if ((lock_loss == 1) && (prbs_lock == 1)) {
        *value = -2;
    } else {
        *value = error_count;
    }
    return(SOC_E_NONE);
}

/* 
 * viper_loopback_internal_pmd_get (this is the PMD global loopback)
 */
STATIC int 
viper_loopback_internal_pmd_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackGlobalPMD, &enable));
    *value = enable;
    return(SOC_E_NONE);
}


/* 
 * viper_loopback_remote_get
 */
STATIC int 
viper_loopback_remote_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackRemotePMD, &enable));
    *value = enable;
    return(SOC_E_NONE);
}

/* 
 * viper_loopback_remote_pcs_get
 */
STATIC int 
viper_loopback_remote_pcs_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackRemotePCS, &enable));
    *value = enable;
    return(SOC_E_NONE);
}

/* 
 * viper_fec_get
 */
STATIC int 
viper_fec_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_get(pm_phy,  &enable));
    *value = enable;

    return(SOC_E_NONE);
}
/* 
 * viper_pattern_256bit_get 
 *  
 * CHECK SEMANTICS: assumption is that this procedure will retrieve the pattern 
 * from Tier1 and return whether the tx pattern is enabled.  Note that this 
 * is different from the 20 bit pattern retrieval semantics 
 */
/* 
 * viper_pattern_256bit_get 
 *  
 * CHECK SEMANTICS: assumption is that this procedure will retrieve the pattern 
 * from Tier1 and return whether the tx pattern is enabled.  Note that this 
 * is different from the 20 bit pattern retrieval semantics 
 */
STATIC int
viper_pattern_get(soc_phymod_ctrl_t *pmc, viper_pattern_t* cfg_pattern)
{
    phymod_phy_access_t    *pm_phy;
    phymod_pattern_t       phymod_pattern;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_get(pm_phy, &phymod_pattern));
    sal_memcpy(cfg_pattern , phymod_pattern.pattern, sizeof(*cfg_pattern));
    return(SOC_E_NONE);
}

STATIC int
viper_pattern_len_get(soc_phymod_ctrl_t *pmc, uint32_t *value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_pattern_t       phymod_pattern;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_get(pm_phy, &phymod_pattern));
    *value = phymod_pattern.pattern_len;
    return(SOC_E_NONE);
}

/* 
 * viper_scrambler_get
 */
STATIC int
viper_scrambler_get(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_inf_config_t config;
    phymod_ref_clk_t        ref_clock = phymodRefClk156Mhz;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    
    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(pm_phy, 0 /* flags */, ref_clock, &config));
    *value = PHYMOD_INTF_MODES_SCR_GET(&config);

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_viper_control_get
 * Purpose:
 *      Get current control settings of the PHY. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - (OUT) Current setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    viper_config_t       *pCfg;

    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pCfg = (viper_config_t *) pc->driver_data;

    switch(type) {

    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = viper_per_lane_preemphasis_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = viper_per_lane_preemphasis_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = viper_per_lane_preemphasis_get(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = viper_per_lane_preemphasis_get(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        /* assume they are all the same as lane 0 */
        rv = viper_tx_fir_pre_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        /* assume they are all the same as lane 0 */
        rv = viper_tx_fir_main_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        /* assume they are all the same as lane 0 */
        rv = viper_tx_fir_post_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        /* assume they are all the same as lane 0 */
        rv = viper_tx_fir_post2_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        /* assume they are all the same as lane 0 */
        rv = viper_tx_fir_post3_get(pmc, value);
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = viper_per_lane_driver_current_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = viper_per_lane_driver_current_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = viper_per_lane_driver_current_get(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = viper_per_lane_driver_current_get(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = viper_per_lane_driver_current_get(pmc, 0, value);
        break;

    /* PRE-DRIVER CURRENT */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;

    /* POST2_DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;

    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = viper_rx_peak_filter_get(pmc, value);
        break;

    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        rv = SOC_E_UNAVAIL;
        break;

    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = viper_rx_tap_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = viper_rx_tap_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = viper_rx_tap_get(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = viper_rx_tap_get(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = viper_rx_tap_get(pmc, 4, value);
        break;

    /* RX SLICER */
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_RX_D_SLICER:
        rv = SOC_E_UNAVAIL;
        break;

    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
        rv = viper_pi_control_get(pmc, value);
        break;

    /* RX SIGNAL DETECT */
    case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
      rv = viper_rx_signal_detect_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_RX_SEQ_DONE:
      rv = viper_rx_seq_done_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_RX_PPM:
      rv = viper_rx_ppm_get(pmc, value);
      break;
    /* PRBS */
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
      rv = viper_prbs_tx_poly_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
      rv = viper_prbs_tx_invert_data_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
      rv = viper_prbs_tx_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
      rv = viper_prbs_rx_poly_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
      rv = viper_prbs_rx_invert_data_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
      rv = viper_prbs_rx_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
      rv = viper_prbs_tx_poly_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
      rv = viper_prbs_tx_invert_data_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
      rv = viper_prbs_tx_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
      rv = viper_prbs_rx_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
      rv = viper_prbs_rx_status_get(pmc, value);
      break;

    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
       rv = viper_loopback_remote_get(pmc, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
       rv = viper_loopback_remote_pcs_get(pmc, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
       rv = viper_loopback_internal_pmd_get(pmc, value);
       break;

    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
      rv = viper_fec_enable_get(pmc, value);
      break;

    /* TX PATTERN */
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
       rv = viper_pattern_get(pmc, &pCfg->pattern);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
       rv = viper_pattern_len_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
       rv = viper_pattern_get(pmc, &pCfg->pattern);
      break;

    /* SCRAMBLER */
    case SOC_PHY_CONTROL_SCRAMBLER:
       rv = viper_scrambler_get(pmc, port, value);
       break;
    case SOC_PHY_CONTROL_LANE_SWAP:
        rv = viper_lane_map_get(pmc, value);
        break;
    /* EEE */
    case SOC_PHY_CONTROL_EEE:
        rv = viper_eee_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
       rv = viper_control_linkdown_transmit_get(pmc, value);
       break;

    /* UNAVAIL */

#if 0
    /* BERT */
    case SOC_PHY_CONTROL_BERT_TX_PACKETS:       /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_PACKETS:       /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_BITS:    /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_BYTES:   /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_PACKETS: /* fall through */
    case SOC_PHY_CONTROL_BERT_PATTERN:          /* fall through */
    case SOC_PHY_CONTROL_BERT_PACKET_SIZE:      /* fall through */
    case SOC_PHY_CONTROL_BERT_IPG:              /* fall through */
        rv = SOC_E_NONE;
        break;

    /* CUSTOM */
    case SOC_PHY_CONTROL_CUSTOM1:
      /* Obsolete ??
      *value = DEV_CFG_PTR(pc)->custom1;
      rv = SOC_E_NONE; */
      break;

    /* SOFTWARE RX LOS */
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
       /* Was 
       *value = DEV_CFG_PTR(pc)->sw_rx_los.enable ; */
       rv = SOC_E_NONE ;
       break ;

    /* UNAVAIL */
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
       rv = SOC_E_UNAVAIL;
       break;
#endif
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}     

/*
 * Function:
 *      phy_viper_per_lane_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_per_lane_control_set(int unit, soc_port_t port, int lane, soc_phy_control_t type, uint32 value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;

    /* locate phy control, phymod control, and the configuration data */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = viper_per_lane_preemphasis_set(pmc, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 0 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 0 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 1 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 1 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 2 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 2 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 3 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 3 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 4 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
        rv = viper_per_lane_rx_dfe_tap_control_set (pmc, lane, 4 /* tap */, 0 /* release */, 0x8000);
        break;

    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = viper_per_lane_prbs_poly_set(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = viper_per_lane_prbs_tx_invert_data_set(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* TX_ENABLE does both tx and rx */
        rv = viper_per_lane_prbs_tx_enable_set(pmc, lane, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = viper_per_lane_rx_peak_filter_set(pmc, lane, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = viper_per_lane_rx_low_freq_filter_set(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA:
        rv = viper_per_lane_rx_vga_set(pmc, lane, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:
        rv = viper_per_lane_rx_vga_set(pmc, lane, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
        rv = SOC_E_UNAVAIL;
        break; 
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
        rv = SOC_E_UNAVAIL;
        break; 
    case SOC_PHY_CONTROL_RX_D_SLICER:
        rv = SOC_E_UNAVAIL;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }

    return rv;
}

/*
 * Function:
 *      phy_viper_per_lane_control_get
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_viper_per_lane_control_get(int unit, soc_port_t port, int lane,
                     soc_phy_control_t type, uint32 *value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;

    /* locate phy control, phymod control, and the configuration data */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    switch(type) {

    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = viper_per_lane_preemphasis_get(pmc, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = viper_per_lane_prbs_poly_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = viper_per_lane_prbs_tx_invert_data_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = viper_per_lane_prbs_tx_enable_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = viper_per_lane_prbs_rx_enable_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = viper_per_lane_prbs_rx_status_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = viper_per_lane_rx_peak_filter_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA:
        rv = viper_per_lane_rx_vga_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = viper_per_lane_rx_dfe_tap_control_get(pmc, lane, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = viper_per_lane_rx_dfe_tap_control_get(pmc, lane, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = viper_per_lane_rx_dfe_tap_control_get(pmc, lane, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = viper_per_lane_rx_dfe_tap_control_get(pmc, lane, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = viper_per_lane_rx_dfe_tap_control_get(pmc, lane, 4, value);
        break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = viper_per_lane_rx_low_freq_filter_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
        rv = SOC_E_UNAVAIL;
        break; 
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
        rv = SOC_E_UNAVAIL;
        break; 
    case SOC_PHY_CONTROL_RX_D_SLICER:
        rv = SOC_E_UNAVAIL;
        break; 
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_viper_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      unit         - BCM unit number
 *      port         - Port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_viper_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;

    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    idx = pmc->main_phy ;
    pm_phy = &pmc->phy[idx]->pm_phy;

    return phymod_phy_reg_read(pm_phy, phy_reg_addr, phy_reg_data);
}

/*
 * Function:
 *      phy_viper_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_viper_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;

    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_reg_write(pm_phy, phy_reg_addr, phy_reg_data));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Note:
 *      This function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_viper_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    uint32 data32, tmp;
    int idx;

    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_reg_read(pm_phy, phy_reg_addr, &data32));
        tmp = data32;
        data32 &= ~(phy_data_mask);
        data32 |= (phy_data & phy_data_mask);
        if (data32 != tmp) {
            SOC_IF_ERROR_RETURN
                (phymod_phy_reg_write(pm_phy, phy_reg_addr, data32));
        }
    }
    return SOC_E_NONE;
}

STATIC void
phy_viper_cleanup(soc_phymod_ctrl_t *pmc)
{
    int idx;
    soc_phymod_phy_t *phy;
    soc_phymod_core_t *core;

    for (idx = 0; idx < pmc->num_phys ; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(pmc->unit,
                                 "phy object is empty")));
            continue;
        }

        core = phy->core;
        /* Destroy core object if not used anymore */
        if (core && core->ref_cnt) {
            if (--core->ref_cnt == 0) {
                PHYMOD_VDBG(VIPER_DBG_MEM, NULL,("clean_up core_p=%p\n", (void *)core)) ;
                soc_phymod_core_destroy(pmc->unit, core);
            }
        }

        /* Destroy phy object */
        if (phy) {
            PHYMOD_VDBG(VIPER_DBG_MEM, NULL,("clean_up phy=%p\n", (void *)phy)) ;
            soc_phymod_phy_destroy(pmc->unit, phy);
        }
    }
    pmc->num_phys = 0;
}

STATIC void
phy_viper_core_init(phy_ctrl_t *pc, soc_phymod_core_t *core,
                   phymod_bus_t *core_bus, uint32 core_addr)
{
    phymod_core_access_t *pm_core;
    phymod_access_t *pm_acc;

    core->unit = pc->unit;
    core->port = pc->port;
    core->read = pc->read;
    core->write = pc->write;
    core->wrmask = pc->wrmask;

    pm_core = &core->pm_core;
    phymod_core_access_t_init(pm_core);
    pm_acc = &pm_core->access;
    phymod_access_t_init(pm_acc);
    PHYMOD_ACC_USER_ACC(pm_acc) = core;
    PHYMOD_ACC_BUS(pm_acc) = core_bus;
    PHYMOD_ACC_ADDR(pm_acc) = core_addr;
#if 0
    if (soc_property_port_get(pc->unit, pc->port, "viper_sim", 0) == 45) {
        PHYMOD_ACC_F_CLAUSE45_SET(pm_acc);
    }
#endif
    return;
}


/*
 * Function:
 *      phy_viper_probe
 * Purpose:
 *      xx
 * Parameters:
 *      pc->phy_id   (IN)
 *      pc->unit     (IN)
 *      pc->port     (IN)
 *      pc->size     (OUT) - memory required by phy port
 *      pc->dev_name (OUT) - set to port device name
 *  
 * Note:
 */
STATIC int
phy_viper_probe(int unit, phy_ctrl_t *pc)
{
    int rv, idx;
    uint32 lane_map[3], num_phys, core_id, phy_id, found;
    phymod_bus_t core_bus;
    phymod_dispatch_type_t phy_type;
    phymod_core_access_t *pm_core;
    phymod_phy_access_t *pm_phy;
    phymod_access_t *pm_acc;
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy;
    soc_phymod_core_t *core;
    soc_phymod_core_t core_probe;
    soc_info_t *si;
    phyident_core_info_t core_info[8];  
    int array_max = 8;
    int array_size = 0;
    int port;
    int phy_port;  /* physical port number */

    /* Initialize PHY bus */
    SOC_IF_ERROR_RETURN(phymod_bus_t_init(&core_bus));
    core_bus.bus_name = "viper_sim"; 
    core_bus.read = _viper_reg_read; 
    core_bus.write = _viper_reg_write;

    /* Configure PHY bus properties */
    if (pc->wrmask) {
        PHYMOD_BUS_CAP_WR_MODIFY_SET(&core_bus);
        PHYMOD_BUS_CAP_LANE_CTRL_SET(&core_bus);
    }

    port = pc->port;
    pmc = &pc->phymod_ctrl;
    si = &SOC_INFO(unit);
    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = si->port_l2p_mapping[port];
    } else {
        phy_port = port;
    }

    /* Install clean up function */
    pmc->unit = pc->unit;
    pmc->cleanup = phy_viper_cleanup;
    if(SOC_IS_GREYHOUND2(unit) || soc_feature(unit, soc_feature_wh2)) {
        pmc->symbols = &bcmi_sgmiip2x4_serdes_symbols;
    }
    else {
        pmc->symbols = &bcmi_viper_xgxs_symbols;
    }
    pmc->main_phy= 0;

    if(SOC_IS_SABER2(unit)) { 
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));
    }
    else if (SOC_IS_GREYHOUND2(unit)){
        /* every block has 2 SGMIIPlus2 cores, every core has 4 lanes */
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port) % 4;
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port)) * 2 + SOC_PORT_BINDEX(unit, phy_port) / 4;
    }
    else if (soc_feature(unit, soc_feature_wh2)) {
        /* viper ports belong to  gport3 and gport4 block of wolfhound2 */
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port)) - 3;
    }
    /* request memory for the configuration structure */
    pc->size = sizeof(viper_config_t);

    /* Bit N corresponds to lane N in lane_map */
    lane_map[0] = 0xf;
    lane_map[1] = 0 ;
    lane_map[2] = 0 ;
    num_phys = 1;
    switch (si->port_num_lanes[port]) {
    case 4:
        pc->phy_mode = PHYCTRL_QUAD_LANE_PORT; 
        break;
    case 2:
        lane_map[0] = 0x3;
        pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
        break;
    case 1:
    case 0:
        lane_map[0] = 0x1;
        pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
        break;
    default:
        return SOC_E_CONFIG;
    }
    lane_map[0] <<= pc->lane_num;

    /* we need to get the other core id if more than 1 core per port */
    if (num_phys > 1) {
        /* get the other core address */
        SOC_IF_ERROR_RETURN
            (soc_phy_addr_multi_get(unit, port, array_max, &array_size, &core_info[0]));
    } else {
        core_info[0].mdio_addr = pc->phy_id;
    }

    phy_type = phymodDispatchTypeViper;

    /* Probe cores */
    for (idx = 0; idx < num_phys ; idx++) {
        phy_viper_core_init(pc, &core_probe, &core_bus,
                           core_info[idx].mdio_addr);
        /* Check that core is indeed an VIPER core */
        pm_core = &core_probe.pm_core;
        pm_core->type = phy_type;
        rv = phymod_core_identify(pm_core, 0, &found);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
        if (!found) {
            return SOC_E_NOT_FOUND;
        }
    }

    rv = SOC_E_NONE;
    for (idx = 0; idx < num_phys ; idx++) {
        /* Set core and phy IDs based on PHY address */
        core_id = pc->phy_id + idx;
        phy_id = (lane_map[idx] << 16) | core_id;

        /* Create PHY object */
        rv = soc_phymod_phy_create(unit, phy_id, &pmc->phy[idx]);
        if (SOC_FAILURE(rv)) {
            break;
        }
        pmc->num_phys++;

        /* Initialize phy object */
        phy = pmc->phy[idx];
        pm_phy = &phy->pm_phy;
        phymod_phy_access_t_init(pm_phy);

        /* Find or create associated core object */
        rv = soc_phymod_core_find_by_id(unit, core_id, &phy->core);
        if (rv == SOC_E_NOT_FOUND) {
            rv = soc_phymod_core_create(unit, core_id, &phy->core);
        }
        if (SOC_FAILURE(rv)) {
            break;
        }        
    }
    if (SOC_FAILURE(rv)) {
        phy_viper_cleanup(pmc);
        return rv;
    }

    for (idx = 0; idx < pmc->num_phys ; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;
        pm_core = &core->pm_core;

        /* Initialize core object if newly created */
        if (core->ref_cnt == 0) {
            sal_memcpy(&core->pm_bus, &core_bus, sizeof(core->pm_bus));
            phy_viper_core_init(pc, core, &core->pm_bus,
                               core_info[idx].mdio_addr);
            /* Set dispatch type */
            pm_core->type = phy_type;
        }
        core->ref_cnt++;        

        /* Initialize phy access based on associated core */
        pm_acc = &phy->pm_phy.access;
        sal_memcpy(pm_acc, &pm_core->access, sizeof(*pm_acc));
        phy->pm_phy.type = phy_type;
        PHYMOD_ACC_LANE_MASK(pm_acc) = lane_map[idx];
    }

    return SOC_E_NONE;
}


/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _viper_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) serdesphy is an intermediate PHY.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      duplex - Boolean, TRUE indicates full duplex, FALSE
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_viper_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    SOC_IF_ERROR_RETURN
        (phy_viper_duplex_set(unit, port, duplex));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _viper_stop
 * Purpose:
 *      Put serdesviper SERDES in or out of reset depending on conditions
 */

STATIC int
_viper_stop(int unit, soc_port_t port)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_phy_power_t phy_power;
    int  stop, copper, speed;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;

    if (pc->phy_mode != PHYCTRL_ONE_LANE_PORT) {
        return SOC_E_NONE;
    }

    /* 'Stop' only for speeds < 10G. */ 
    SOC_IF_ERROR_RETURN
        (phy_viper_speed_get(unit,port,&speed));
    if(10000 <= speed) {
        return SOC_E_NONE;
    }
    copper = (pc->stop &
              PHY_STOP_COPPER) != 0;

    stop = ((pc->stop &
             (PHY_STOP_PHY_DIS |
              PHY_STOP_DRAIN)) != 0 ||
            (copper &&
             (pc->stop &
              (PHY_STOP_MAC_DIS |
               PHY_STOP_DUPLEX_CHG |
               PHY_STOP_SPEED_CHG)) != 0));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "qsgmiie_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));
    if (stop) {
        phy_power.tx = phymodPowerOff;
        phy_power.rx = phymodPowerOff;
    } else {
        phy_power.tx = phymodPowerOn;
        phy_power.rx = phymodPowerOn;
    }
    SOC_IF_ERROR_RETURN
        (phymod_phy_power_set(pm_phy, &phy_power));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _viper_notify_stop
 * Purpose:
 *      Add a reason to put serdesviper PHY in reset.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_viper_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;
    return _viper_stop(unit, port);
}

/*  
 * Function:
 *      _viper_notify_resume
 * Purpose:
 *      Remove a reason to put serdesviper PHY in reset.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_viper_notify_resume(int unit, soc_port_t port, uint32 flags)
{   
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
    return _viper_stop(unit, port);
}

/*
 * Function:
 *      _viper_notify_speed
 * Purpose:
 *      Program duplex if (and only if) serdesviper is an intermediate PHY.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      speed - Speed to program.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_viper_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    phy_ctrl_t* pc; 
    viper_config_t *pCfg;
    int  fiber;

    pc = INT_PHY_SW_STATE(unit, port);
    pCfg = (viper_config_t *) pc->driver_data;
    fiber = pCfg->fiber_pref;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "_qsgmiie_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_viper_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (phy_viper_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_viper_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_viper_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      viper_media_setup
 * Purpose:     
 *      Configure 
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      fiber_mode - Configure for fiber mode
 *      fiber_pref - Fiber preferrred (if fiber mode)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
_viper_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      _viper_notify_mac_loopback
 * Purpose:
 *      Turn on rx clock compensation in mac loopback mode for
 *      applicable XGXS devices
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      enable - TRUE: In MAC loopback mode, FALSE: Not in MAC loopback mode 
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_viper_notify_mac_loopback(int unit, soc_port_t port, uint32 enable)
{
    return SOC_E_NONE;
}

STATIC int
phy_viper_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;
    rv = SOC_E_NONE ;
    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    switch(event) {
    case phyEventInterface:
        rv = (_viper_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_viper_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_viper_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_viper_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_viper_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
#if 0
        rv = (_viper_an_set(unit, port, value));
#endif
        break;
    case phyEventMacLoopback:
        rv = (_viper_notify_mac_loopback(unit, port, value));
        break;
    case phyEventLpiBypass:
        PHYMOD_LPI_BYPASS_SET(value);
        rv = phy_viper_control_set(unit, port, SOC_PHY_CONTROL_EEE, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}


/*
 * Function:
 *      phy_viper_master_set
 * Purpose:
 *      Configure master mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - Master mode.
 * Returns:
 *      SOC_E_NONE/SOC_E_UNAVAIL
 */
STATIC int
phy_viper_master_set(int unit, soc_port_t port, int master)
{
    if (master != SOC_PORT_MS_SLAVE)
        return SOC_E_UNAVAIL;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_master_get
 * Purpose:
 *      this function is meant for 1000Base-T PHY. Added here to support
 *      internal PHY regression test
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */
STATIC int
phy_viper_master_get(int unit, soc_port_t port, int *master)
{
    *master = SOC_PORT_MS_SLAVE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_viper_diag_ctrl
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_viper_diag_ctrl(
    int unit,        /* unit */
    soc_port_t port, /* port */
    uint32 inst,     /* the specific device block the control action directs to */
    int op_type,     /* operation types: read,write or command sequence */
    int op_cmd,      /* command code */
    void *arg)       /* command argument based on op_type/op_cmd */
{
    switch(op_cmd) {
        case PHY_DIAG_CTRL_PCS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_temod_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_PCS));
			SOC_IF_ERROR_RETURN(viper_pcs_status_dump(unit, port, arg));
            break;
        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                SOC_IF_ERROR_RETURN(phy_viper_control_set(unit,port,op_cmd,PTR_TO_INT(arg)));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                SOC_IF_ERROR_RETURN(phy_viper_control_get(unit,port,op_cmd,(uint32 *)arg));
            }
            break ;
        }
    return (SOC_E_NONE);
}


#ifdef BROADCOM_DEBUG
void
viper_state_dump(int unit, soc_port_t port)
{
    /* Show PHY configuration summary */
    /* Lane status */
    /* Show Counters */
    /* Dump Registers */
}
#endif /* BROADCOM_DEBUG */

/*
 * Variable:
 *      viper_drv
 * Purpose:
 *      Phy Driver for VIPER 
 */
phy_driver_t phy_viper_drv = {
    /* .drv_name                      = */ "VIPER PHYMOD PHY Driver",
    /* .pd_init                       = */ phy_viper_init,
    /* .pd_reset                      = */ phy_null_reset,
    /* .pd_link_get                   = */ phy_viper_link_get,
    /* .pd_enable_set                 = */ phy_viper_enable_set,
    /* .pd_enable_get                 = */ phy_viper_enable_get,
    /* .pd_duplex_set                 = */ phy_viper_duplex_set,
    /* .pd_duplex_get                 = */ phy_viper_duplex_get,
    /* .pd_speed_set                  = */ phy_viper_speed_set,
    /* .pd_speed_get                  = */ phy_viper_speed_get,
    /* .pd_master_set                 = */ phy_viper_master_set,
    /* .pd_master_get                 = */ phy_viper_master_get,
    /* .pd_an_set                     = */ phy_viper_an_set,
    /* .pd_an_get                     = */ phy_viper_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */ 
    /* .pd_lb_set                     = */ phy_viper_lb_set,
    /* .pd_lb_get                     = */ phy_viper_lb_get,
    /* .pd_interface_set              = */ phy_viper_interface_set,
    /* .pd_interface_get              = */ phy_viper_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */ 
    /* .pd_linkup_evt                 = */ NULL,
    /* .pd_linkdn_evt                 = */ NULL,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ NULL,
    /* .pd_medium_config_get          = */ NULL,
    /* .pd_medium_get                 = */ phy_null_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_viper_control_set,
    /* .pd_control_get                = */ phy_viper_control_get,
    /* .pd_reg_read                   = */ phy_viper_reg_read,
    /* .pd_reg_write                  = */ phy_viper_reg_write,
    /* .pd_reg_modify                 = */ phy_viper_reg_modify,
    /* .pd_notify                     = */ phy_viper_notify,
    /* .pd_probe                      = */ phy_viper_probe,
    /* .pd_ability_advert_set         = */ phy_viper_ability_advert_set, 
    /* .pd_ability_advert_get         = */ phy_viper_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_viper_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_viper_ability_local_get,
    /* .pd_firmware_set               = */ NULL,
    /* .pd_timesync_config_set        = */ NULL,
    /* .pd_timesync_config_get        = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_viper_diag_ctrl,
    /* .pd_lane_control_set           = */ phy_viper_per_lane_control_set,    
    /* .pd_lane_control_get           = */ phy_viper_per_lane_control_get,
    /* .pd_lane_reg_read              = */ NULL,
    /* .pd_lane_reg_write             = */ NULL,
    /* .pd_oam_config_set             = */ NULL,
    /* .pd_oam_config_get             = */ NULL,
    /* .pd_oam_control_set            = */ NULL,
    /* .pd_oam_control_get            = */ NULL,
    /* .pd_timesync_enhanced_capture_get   = */ NULL,
    /* .pd_multi_get                       = */ NULL,
    /* .pd_precondition_before_probe       = */ NULL,
    /* .pd_linkfault_get                   = */ NULL,
    /* .pd_synce_clk_ctrl_set              = */ NULL,
    /* .pd_synce_clk_ctrl_get              = */ NULL,
    /* .pd_fec_error_inject_set              = */ NULL,
    /* .pd_fec_error_inject_get              = */ NULL
};

STATIC int
_phy_viper_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;
    phymod_phy_power_t        phy_power;   

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;

    if (enable) {
        phy_power.tx = phymodPowerOn; 
        phy_power.rx = phymodPowerOn; 
    } else {
        phy_power.tx = phymodPowerOff; 
        phy_power.rx = phymodPowerOff; 
    }

    SOC_IF_ERROR_RETURN(phymod_phy_power_set(pm_phy, &phy_power));
    /*viper_phy_power_set(&pm_phy->access, &power); */ 

    return SOC_E_NONE;
}





#else
typedef int _viper_not_empty; /* Make ISO compilers happy. */
#endif /*  INCLUDE_XGXS_VIPER */

