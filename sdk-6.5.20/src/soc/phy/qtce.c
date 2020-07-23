/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        qsgmiie.c
 * Purpose:     Support Broadcom TSC/Eagle internal SerDes
 */

/*
 *   This module implements an NTSW SDK Phy driver for the TSC/Eagle Serdes.
 *  
 *   LAYERING.
 *
 *   This driver is built on top of the PHYMOD library, which is a PHY
 *   driver library that can operate on a family of PHYs, including
 *   TSC/Eagle.  PHYMOD can be built in a standalone enviroment and be
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
 *     APIs (nor ".pd_xx" calls, nor to .phy_qtce_per_lane_control_set and
 *     .phy_qtce_per_lane_control_get APIs.
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
 *   qtce_config_t
 *   Driver specific data.  This structure is used by qtce to hold
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
#if defined(INCLUDE_SERDES_QTCE)
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
#include <phymod/chip/bcmi_qtce_xgxs_defs.h>
#include <phymod/chip/qtce.h>

int (*_phy_qtce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int) = { NULL };

#define NUM_LANES             4    /* num of lanes per core */
#define MAX_NUM_LANES         4    /* max num of lanes per port */
#define QTCE_NO_CFG_VALUE     (-1)

/*
 * The DBG_OK macro will bypass the function body and return success
 * if the PHY simulator is enabled for a particular port. This allows
 * system initialization to complete even if the driver has not been
 * fully tested/debugged. As development progresses, we should be able
 * to remove all instances of this macro.
 */
#define DBG_OK() do { \
    if (soc_property_port_get(unit, port, "qtce_sim", 0)) { \
        return SOC_E_NONE; \
    } \
} while (0)


#define PHYMOD_IF_CR4    (1 << SOC_PORT_IF_CR4)


typedef struct qtce_speed_config_s {
    uint32  port_refclk_int;
    int     speed;
    int     port_num_lanes;
    int     port_is_higig;
    int     rxaui_mode;
    int     scrambler_en;
    int     line_interface;
    int     pcs_bypass;
    int     fiber_pref;
    int     phy_supports_dual_rate;
} qtce_speed_config_t;

#define NUM_PATTERN_DATA_INTS    8
typedef struct qtce_pattern_s {
    uint32 pattern_data[NUM_PATTERN_DATA_INTS];
    uint32 pattern_length;
} qtce_pattern_t;

#define QTCE_LANE_NAME_LEN   30

typedef struct {
    uint16 serdes_id0;
    char   name[QTCE_LANE_NAME_LEN];
} qtce_dev_info_t;

/* index to TX drive configuration table for each VCO setting.
 */
typedef enum txdrv_inxs {
    TXDRV_XFI_INX = 0,     /* 10G XFI */
    TXDRV_SFI_INX,     /* 10G SR fiber mode */
    TXDRV_SFIDAC_INX,  /* 10G SFI DAC mode */
    TXDRV_AN_INX,      /* a common entry for autoneg mode */
    TXDRV_DFT_INX,     /* temp for any missing speed modes */
    TXDRV_LAST_INX      /* always last */
} TXDRV_INXS_t;

#define TXDRV_ENTRY_NUM     (TXDRV_LAST_INX)



/*
   Config - per logical port
*/
typedef struct {
    phymod_polarity_t               phy_polarity_config;
    phymod_phy_reset_t              phy_reset_config;
    qtce_pattern_t                  pattern;
    qtce_speed_config_t             speed_config;

    int fiber_pref;                 /* spn_SERDES_FIBER_PREF */
    int cl73an;                     /* spn_PHY_AN_C73 */
    int cx4_10g;                    /* spn_10G_IS_CX4 */
    int pdetect1000x;
    int cl37an;                     /* spn_PHY_AN_C37 */
    int an_cl72;                    /* spn_PHY_AN_C72 */
    int forced_init_cl72;           /* spn_FORCED_INIT_CL72 */
    int hg_mode;                    /* higig port */
    int an_fec;                     /*enable FEC for AN mode */
    int sgmii_mstr;                 /* sgmii master mode */
    qtce_dev_info_t info;          /*serdes id info */
    phymod_tx_t tx_drive[TXDRV_ENTRY_NUM];
    int phy_sgmii_an;              /* phy_sgmii_autoneg */

} qtce_config_t;

#define QTCE_IF_NULL   (1 << SOC_PORT_IF_NULL)
#define QTCE_IF_SR     (1 << SOC_PORT_IF_SR)
#define QTCE_IF_KR     (1 << SOC_PORT_IF_KR)
#define QTCE_IF_KR4    (1 << SOC_PORT_IF_KR4)
#define QTCE_IF_CR     (1 << SOC_PORT_IF_CR)
#define QTCE_IF_CR4    (1 << SOC_PORT_IF_CR4)
#define QTCE_IF_XFI    (1 << SOC_PORT_IF_XFI)
#define QTCE_IF_SFI    (1 << SOC_PORT_IF_SFI)
#define QTCE_IF_XGMII  (1 << SOC_PORT_IF_XGMII)
#define QTCE_IF_SGMII   (1 << SOC_PORT_IF_SGMII)
#define QTCE_IF_GMII   (1 << SOC_PORT_IF_GMII)
#define QTCE_IF_QSGMII   (1 << SOC_PORT_IF_QSGMII)



#define TEMOD_CL73_CL37              0x5
#define TEMOD_CL73_HPAM              0x4
#define TEMOD_CL73_HPAM_VS_SW        0x8
#define TEMOD_CL73_WO_BAM            0x2
#define TEMOD_CL73_W_BAM             0x1
#define TEMOD_CL73_DISABLE           0x0

#define TEMOD_CL37_HR2SPM_W_10G 5
#define TEMOD_CL37_HR2SPM       4
#define TEMOD_CL37_W_10G     3
#define TEMOD_CL37_WO_BAM    2
#define TEMOD_CL37_W_BAM     1
#define TEMOD_CL37_DISABLE   0

STATIC int phy_qtce_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability);
STATIC int phy_qtce_an_set(int unit, soc_port_t port, int enable);

STATIC int phy_qtce_an_get(int unit, soc_port_t port, int *an, int *an_done);

/*
 * Function:
 *      _qtce_reg_read
 * Doc:
 *      register read operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to read
 *      val             - (OUT) read value
 */
STATIC int 
_qtce_reg_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    uint16 data16;
    int rv;
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;

    rv = core->read(core->unit, core_addr, reg_addr, &data16);
    *val = data16;

    return rv;
}

/*
 * Function:
 *      _qtce_reg_write
 * Doc:
 *      register write operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to write
 *      val             - (IN)  write value
 */
STATIC int 
_qtce_reg_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
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
    return core->write(core->unit, core_addr, reg_addr, data16);
}

/*
 * Function:
 *      tsce_firmware_loader
 * Purpose:
 *      Download PHY firmware using fast interface such as S-channel.
 * Parameters:
 *      pm_core         - (IN)  PHY core oject
 *      fw_len          - (IN)  Number of bytes to download
 *      fw_data         - (IN)  Firmware as byte array
 */
STATIC int
qtce_firmware_loader(const phymod_core_access_t *pm_core,
                     uint32 fw_len, const uint8 *fw_data)
{
    soc_phymod_core_t *core;
    int unit, port;

    if (pm_core == NULL) {
        return PHYMOD_E_PARAM;
    }

    /* Retrieve SOC information from PHYMOD object */
    core = (soc_phymod_core_t *)(pm_core->access.user_acc);

    if (core == NULL) {
        return PHYMOD_E_PARAM;
    }

    /* Get associated unit/port */
    unit = core->unit;
    port = core->port;

    return _phy_qtce_firmware_set_helper[unit](unit, port,
                                               (uint8 *)fw_data, fw_len); 
}

#define IS_DUAL_LANE_PORT(_pc) ((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT) 
#define IND_LANE_MODE(_pc) (((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT) || ((_pc)->phy_mode == PHYCTRL_ONE_LANE_PORT))  
#define MAIN0_SERDESID_REV_LETTER_SHIFT  (14)
#define MAIN0_SERDESID_REV_NUMBER_SHIFT  (11)

/*
 * Function:
 *      qtce_show_serdes_info
 * Purpose:
 *      Show SerDes information.
 * Parameters:
 *      pc             - (IN)  phyctrl  oject
 *      pInfo          - (IN)  device info object
 *      rev_id         - (IN)  serdes revid
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
qtce_show_serdes_info(phy_ctrl_t *pc, qtce_dev_info_t *pInfo, phymod_core_info_t *core_info)
{
    uint32_t serdes_id0, len;

    pInfo->serdes_id0 = core_info->serdes_id ;

    /* This is TSC/Eagle device */
    serdes_id0 = pInfo->serdes_id0;
    sal_strlcpy(pInfo->name,"QTCE-", sizeof(pInfo->name));
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

    /* phy_mode: 0 single port mode, port uses all four lanes of Warpcore
     *           1 dual port mode, port uses 2 lanes
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
    } else if (pc->phy_mode == PHYCTRL_QSGMII_CORE_PORT) {
        pInfo->name[len++] = pc->lane_num + '0';
    } else {
        /* pInfo->name[len++] = '4'; */
        if (pc->lane_num <= 9) {
            pInfo->name[len++] = pc->lane_num + '0';
        } else {
            pInfo->name[len++] = '1';
            pInfo->name[len++] = '0' + pc->lane_num - 10;
        }
    }
    pInfo->name[len] = 0;  /* string terminator */

    if (len > QTCE_LANE_NAME_LEN) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("QTCE info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                   len,QTCE_LANE_NAME_LEN, pc->unit, pc->port));
        return SOC_E_MEMORY;
    }
    return SOC_E_NONE;
}

STATIC int 
qtce_ref_clk_convert(int port_refclk_int, phymod_ref_clk_t *ref_clk)
{
    switch (port_refclk_int)
    {
        case 156:
            *ref_clk = phymodRefClk156Mhz;
            break;
        case 125:
            *ref_clk = phymodRefClk125Mhz;
            break;
        default:
            *ref_clk = phymodRefClk156Mhz;
            break;
    }

    return SOC_E_NONE;
}
/*
 * Function:
 *      qtce_speed_to_interface_config_get
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
qtce_speed_to_interface_config_get(qtce_speed_config_t* speed_config, phymod_phy_inf_config_t* interface_config)
{
    int fiber_pref;

    SOC_IF_ERROR_RETURN(phymod_phy_inf_config_t_init(interface_config));

    interface_config->data_rate = speed_config->speed;

    fiber_pref = speed_config->fiber_pref;
    if (fiber_pref) {
        PHYMOD_INTF_MODES_FIBER_SET(interface_config);
    } else {
        PHYMOD_INTF_MODES_BACKPLANE_SET(interface_config);
    }

    switch (speed_config->speed) {
        case 10:
            interface_config->interface_type = phymodInterfaceSGMII;
            break;
        case 100:
        case 1000:
            if (speed_config->fiber_pref) {
                interface_config->interface_type = phymodInterface1000X;
            } else {
                interface_config->interface_type = phymodInterfaceSGMII;
            }
            break;
        case 2500:
            interface_config->interface_type = phymodInterface1000X;
            break;
    default:
            if (speed_config->fiber_pref) {
                interface_config->interface_type = phymodInterface1000X;
            } else {
                interface_config->interface_type = phymodInterfaceSGMII;
            }
            break;
    }

    SOC_IF_ERROR_RETURN(
        qtce_ref_clk_convert(speed_config->port_refclk_int, &(interface_config->ref_clock)));
    
    return SOC_E_NONE;
}

/*
 * Function:
 *      qtce_config_init
 * Purpose:     
 *      Determine phy configuration data for purposes of PHYMOD initialization.
 * 
 *      A side effect of this procedure is to save some per-logical port
 *      information in (qtce_cfg_t *) &pc->driver_data;
 *
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 *      logical_lane_offset   - starting logical lane number
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
qtce_config_init(int unit, soc_port_t port, int logical_lane_offset,
                 phymod_core_init_config_t *core_init_config, 
                 phymod_phy_init_config_t  *pm_phy_init_config)
{
    phy_ctrl_t                   *pc;
    qtce_speed_config_t          *speed_config;
    qtce_config_t                *pCfg;
    phymod_tx_t                  *p_tx;
    int                          port_refclk_int;
    int                          port_num_lanes;
    int                          core_num;
    int                          phy_num_lanes;
    int                          port_is_higig;
    int                          phy_passthru;
    int                          phy_supports_dual_rate;
    int                          lane_map_rx, lane_map_tx;
    int                          i;
    int                          fiber_pref = 0;
    phymod_firmware_load_method_t fw_ld_method;

    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (qtce_config_t *) pc->driver_data;

    /* extract data from SOC_INFO */
    port_refclk_int = SOC_INFO(unit).port_refclk_int[port];
#if 0
    
    port_refclk_int = soc_property_port_get(unit,port, spn_XGXS_PHY_VCO_FREQ, pCfg->refclk);
#endif
    port_refclk_int = soc_property_port_get(unit, port,spn_XGXS_LCPLL_XTAL_REFCLK, port_refclk_int);
    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];
    port_is_higig = PBMP_MEMBER(PBMP_HG_ALL(unit), port);
    phy_supports_dual_rate = PHY_IS_SUPPORT_DUAL_RATE(unit, port);

    /* figure out how many lanes are in this phy */
    core_num = (logical_lane_offset / 4);
    phy_num_lanes = (port_num_lanes - logical_lane_offset);
    if (phy_num_lanes > MAX_NUM_LANES) {
       phy_num_lanes = MAX_NUM_LANES;
    }
    
    /* 
        CORE configuration
    */
    phymod_core_init_config_t_init(core_init_config);
    fw_ld_method = phymodFirmwareLoadMethodInternal;
    if (_phy_qtce_firmware_set_helper[unit]) {
        core_init_config->firmware_loader = qtce_firmware_loader;
        fw_ld_method = phymodFirmwareLoadMethodExternal;
    }

    if (soc_property_port_get(pc->unit, pc->port, "qtce_sim", 0) || soc_property_port_get(pc->unit, pc->port, "eagle_sim", 0)) {
        fw_ld_method = phymodFirmwareLoadMethodNone;
    }
    

    core_init_config->firmware_load_method  = soc_property_port_get(unit, port, 
                                              spn_LOAD_FIRMWARE, fw_ld_method);
    core_init_config->firmware_load_method &= 0xff;   /* clear checksum bits */ 
    core_init_config->flags = PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY;
    core_init_config->lane_map.num_of_lanes = NUM_LANES;

    lane_map_rx = soc_property_port_suffix_num_get(unit, port, core_num,
                                        spn_XGXS_RX_LANE_MAP, "core", 0x3210);
    for (i=0; i<NUM_LANES; i++) {
        core_init_config->lane_map.lane_map_rx[i] = (lane_map_rx >> (i * 4 /*4 bit per lane*/)) & 0xf;
    }
 
    lane_map_tx = soc_property_port_suffix_num_get(unit, port, core_num,
                                        spn_XGXS_TX_LANE_MAP, "core", 0x3210);
    for (i=0; i<NUM_LANES; i++) {
        core_init_config->lane_map.lane_map_tx[i] = (lane_map_tx >> (i * 4 /*4 bit per lane*/)) & 0xf;
    }

    speed_config = &(pCfg->speed_config);
    speed_config->port_refclk_int  = port_refclk_int;
    speed_config->speed  = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, 0);
    speed_config->port_num_lanes   = phy_num_lanes;
    speed_config->port_is_higig    = port_is_higig;
    speed_config->rxaui_mode       = soc_property_port_get(unit, port, spn_SERDES_RXAUI_MODE, 1);
    speed_config->scrambler_en     = soc_property_port_get(unit, port, spn_SERDES_SCRAMBLER_ENABLE, 0);
    speed_config->line_interface   = soc_property_port_get(unit, port, spn_SERDES_IF_TYPE, 0);
    speed_config->fiber_pref = soc_property_port_get(unit, port, spn_SERDES_FIBER_PREF, fiber_pref);
    pCfg->fiber_pref = speed_config->fiber_pref;
    speed_config->pcs_bypass       = 0;
    speed_config->phy_supports_dual_rate = phy_supports_dual_rate;
    SOC_IF_ERROR_RETURN
        (qtce_speed_to_interface_config_get(speed_config, &(core_init_config->interface)));

    /* 
        PHY configuration
    */
    for (i = 0; i < TXDRV_ENTRY_NUM; i++) {
        SOC_IF_ERROR_RETURN(phymod_tx_t_init(&pCfg->tx_drive[i]));
    }
    /*set the default tx parameters */
    p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
    p_tx->amp = 0xc;
    p_tx->pre = 0x00;
    p_tx->main = 0x70;
    p_tx->post = 0x0;
    p_tx->post2 = 0x00;
    p_tx->post3 = 0x00;

    p_tx = &pCfg->tx_drive[TXDRV_XFI_INX];
    p_tx->amp = 0xc;
    p_tx->pre = 0x00;
    p_tx->main = 0x30;
    p_tx->post = 0x0;
    p_tx->post2 = 0x00;
    p_tx->post3 = 0x00;

    p_tx = &pCfg->tx_drive[TXDRV_SFIDAC_INX];
    p_tx->amp = 0xc;
    p_tx->pre = 0x00;
    p_tx->main = 0x18;
    p_tx->post = 0x18;
    p_tx->post2 = 0x00;
    p_tx->post3 = 0x00;

    p_tx = &pCfg->tx_drive[TXDRV_AN_INX];
    p_tx->amp = 0xc;
    p_tx->pre = 0x00;
    p_tx->main = 0x70;
    p_tx->post = 0x00;
    p_tx->post2 = 0x00;
    p_tx->post3 = 0x00;

    phymod_phy_init_config_t_init(pm_phy_init_config);
    /*initialize the tx taps and driver current*/
    for (i = 0; i < 4; i++) { 
        pm_phy_init_config->tx[i].pre = 0x00;
        pm_phy_init_config->tx[i].main = 0x70;
        pm_phy_init_config->tx[i].post = 0x0;
        pm_phy_init_config->tx[i].post2 = 0x0;
        pm_phy_init_config->tx[i].post3 = 0x0;
        pm_phy_init_config->tx[i].amp = 0xc;
    }
    pm_phy_init_config->polarity.rx_polarity 
                                 = soc_property_port_get(unit, port,
                                   spn_PHY_XAUI_RX_POLARITY_FLIP, FALSE);
    pm_phy_init_config->polarity.tx_polarity 
                                 = soc_property_port_get(unit, port,
                                   spn_PHY_XAUI_TX_POLARITY_FLIP, FALSE);

    for (i = 0; i < MAX_NUM_LANES; i++) {
        uint32_t preemphasis;
        preemphasis = soc_property_port_suffix_num_get(unit, port,
                                    i + core_num * 4, spn_SERDES_PREEMPHASIS,
                                    "lane", QTCE_NO_CFG_VALUE);
        if (preemphasis != QTCE_NO_CFG_VALUE) {
            pm_phy_init_config->tx[i].pre = preemphasis & 0xff;
            pm_phy_init_config->tx[i].main = (preemphasis & 0xff00) >> 8;
            pm_phy_init_config->tx[i].post = (preemphasis & 0xff0000) >> 16;
        }     
    }

    for (i = 0; i < MAX_NUM_LANES; i++) {
        pm_phy_init_config->tx[i].amp = soc_property_port_suffix_num_get(unit, port,
                                    i + core_num * 4, spn_SERDES_DRIVER_CURRENT,
                                    "lane", pm_phy_init_config->tx[i].amp);
    }

    

    
    pm_phy_init_config->cl72_en = soc_property_port_get(unit, port, spn_PHY_AN_C72, TRUE);
    pm_phy_init_config->an_en = TRUE;

    /* check the higig port mode, then set the default cl73 mode */
    if (port_is_higig) {
        pCfg->cl73an = FALSE;
    } else {
        pCfg->cl73an = PHYMOD_AN_CAP_CL73;
    }

    /* by default disable cl37 */
    pCfg->cl37an = FALSE;
    pCfg->an_cl72 = TRUE;
    pCfg->forced_init_cl72 = FALSE; 
    pCfg->phy_sgmii_an = TRUE;

    pCfg->cl73an  = soc_property_port_get(unit, port,
                                          spn_PHY_AN_C73, pCfg->cl73an); /* no L: C73, not CL73 */

    pCfg->cl37an = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C37, pCfg->cl37an); /* no L: C37, not CL37 */

    pCfg->an_cl72 = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C72, pCfg->an_cl72);
    pCfg->forced_init_cl72 = soc_property_port_get(unit, port,
                                        spn_PORT_INIT_CL72, pCfg->forced_init_cl72);
    pCfg->phy_sgmii_an =  soc_property_port_get(unit, port,
                             spn_PHY_SGMII_AUTONEG, pCfg->phy_sgmii_an);

    /* 
        phy_ctrl_t configuration (LOGICAL PORT BASED)
        Only do this once, for the first core of the logical port
    */
    if (core_num == 0) {
        
        /* pc->lane_num, pc->chip_num */
#if 0
        soc_port_info = &(SOC_DRIVER(unit)->port_info[phy_port]);
/* mark it for the assertion of lane_num and chip_num is assigned */
        pc->lane_num = soc_port_info->bindex;
        pc->chip_num = SOC_BLOCK_NUMBER(unit, soc_port_info->blk);
#endif

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

        /* PHY_FLAGS_PASSTHRU */
        phy_passthru = 0;
        if (PHY_EXTERNAL_MODE(unit, port)) {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
            phy_passthru = soc_property_port_get(unit, port,
                                                 spn_PHY_PCS_REPEATER, 0);
            if (phy_passthru) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_PASSTHRU);
            }
        }

        /* PHY_FLAGS_FIBER */
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        if (!PHY_EXTERNAL_MODE(unit, port)) {

            fiber_pref = FALSE;
            if (PHY_FIBER_MODE(unit, port) ||
                (phy_passthru) ||
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

        
        /* not used  */
        /* pCfg->cl73an = soc_property_port_get(unit, port, spn_PHY_AN_C73, TSCMOD_CL73_WO_BAM);

        if (!PHY_EXTERNAL_MODE(unit, port)) {
            if (pCfg->cl73an) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_C73);
            }
        }
        */

        
        if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
            phy_passthru) {
            pCfg->pdetect1000x = TRUE;
        } else {
            pCfg->pdetect1000x = FALSE;
        }

        pCfg->cx4_10g            = soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qtce_init
 *  
 *      An SDK "phy" is a logical port, which can consist of from 1-10 lanes,
 *          (A phy with more than 4 lanes requires more than one core).
 *      Per-logical port information is saved in (qtce_cfg_t *) &pc->driver_data.
 *      An SDK phy is implemented as one or more PHYMOD "phy"s.
 *  
 *      A PHYMOD "phy" resides completely within a single core, which can be
 *      from 1 to 4 lanes.
 *      Per-phymod phy information is kept in (soc_phymod_ctrl_t) *pc->phymod_ctrl
 *      A phymod phy points to its core.  Up to 4 phymod phys can be on a single core
 *  
 * Purpose:     
 *      Initialize a qtce phy
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_qtce_init(int unit, soc_port_t port)
{
    phy_ctrl_t                *pc; 
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy = NULL;
    soc_phymod_core_t         *core;
    qtce_config_t             *pCfg;
    qtce_speed_config_t       *speed_config;
    qtce_dev_info_t           *pInfo;
    soc_phy_info_t            *pi;
    phymod_phy_inf_config_t   interface_config;
    phymod_core_status_t      core_status;
    phymod_core_info_t        core_info;
    int idx;
    int logical_lane_offset;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 
    phymod_autoneg_control_t  an;

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);
    phymod_autoneg_control_t_init(&an);

    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pc->driver_data = (void*)(pc+1);
    pmc = &pc->phymod_ctrl;
    pCfg = (qtce_config_t *) pc->driver_data;
    pInfo = &pCfg->info; 
    
    sal_memset(pCfg, 0, sizeof(*pCfg));
    speed_config = &(pCfg->speed_config);

    /* Loop through all phymod phys that support this SDK phy */
    logical_lane_offset = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;

        

        /* determine configuration data structure to default values, based on SOC properties */
        SOC_IF_ERROR_RETURN
            (qtce_config_init(unit, port,
                              logical_lane_offset,
                              &core->init_config, &phy->init_config));

        
        if (!core->init) {
           core_status.pmd_active = 0;
           if (!SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN
               (phymod_core_init(&core->pm_core, &core->init_config, &core_status));
           }
           core->init = TRUE;
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

    /*fill up the pInfo table for displaying the serdes driver info */
    SOC_IF_ERROR_RETURN
        (qtce_show_serdes_info(pc, pInfo, &core_info));

    /* retrieve chip level information for serdes driver info */
    pi = &SOC_PHY_INFO(unit, port);

    if (!PHY_EXTERNAL_MODE(unit, port)) {
        pi->phy_name = pInfo->name;
    }

    if (SOC_WARM_BOOT(unit)) {
        return SOC_E_NONE;
    }
    /* set the port to its max or init_speed */
    SOC_IF_ERROR_RETURN
        (qtce_speed_to_interface_config_get(speed_config, &interface_config));
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_set(&phy->pm_phy,
                                         0 /* flags */, &interface_config));

    /* next check if qsgmii mode, enable  AN */
    if (PHYMOD_ACC_F_QMODE_GET(&phy->pm_phy.access) && pCfg ->phy_sgmii_an) {
         SOC_IF_ERROR_RETURN
            (phymod_phy_autoneg_ability_set(&phy->pm_phy, &phymod_autoneg_ability));
         /* enable AN */
         an.an_mode = phymod_AN_MODE_CL37;
         an.enable = 1;
         an.num_lane_adv = 1;
         SOC_IF_ERROR_RETURN
            (phymod_phy_autoneg_set(&phy->pm_phy, &an));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qtce_link_get
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
phy_qtce_link_get(int unit, soc_port_t port, int *link)
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
    pm_phy = &pmc->phy[0]->pm_phy;

    SOC_IF_ERROR_RETURN
        (phymod_phy_link_status_get(pm_phy, (uint32_t *) link));
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_qtce_enable_set
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
phy_qtce_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;
    phymod_phy_tx_lane_control_t tx_control;
    phymod_phy_rx_lane_control_t rx_control;

    int idx;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    if (enable) {
        tx_control = phymodTxSquelchOff;
        rx_control = phymodRxSquelchOff;
    } else {
        tx_control = phymodTxSquelchOn;
        rx_control = phymodRxSquelchOn;
    }

    pmc = &pc->phymod_ctrl;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_set(pm_phy, tx_control));
        SOC_IF_ERROR_RETURN
            (phymod_phy_rx_lane_control_set(pm_phy, rx_control));
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_qtce_enable_get
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
phy_qtce_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;
    phymod_phy_tx_lane_control_t tx_control;
    phymod_phy_rx_lane_control_t rx_control;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[0]->pm_phy;
    SOC_IF_ERROR_RETURN
        (phymod_phy_tx_lane_control_get(pm_phy, &tx_control));
    SOC_IF_ERROR_RETURN
        (phymod_phy_rx_lane_control_get(pm_phy, &rx_control));
    if ((tx_control == phymodTxSquelchOn) && (rx_control == phymodRxSquelchOn)) {
        *enable = 0;
    } else {
        *enable = 1;
    }
    return (SOC_E_NONE);
}


/*
 * Function:
 *      phy_qtce_duplex_get
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
phy_qtce_duplex_get(int unit, soc_port_t port, int *duplex)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_autoneg_ability_t  phymod_autoneg_ability;

    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    
    pmc = &pc->phymod_ctrl;
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_get(&phy->pm_phy, &phymod_autoneg_ability));
    
    if(PHYMOD_AN_CAP_HALF_DUPLEX_GET(&phymod_autoneg_ability)) {
        *duplex = FALSE;
    } else {
        *duplex = TRUE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_qtce_qsgmii_an_get
 * Purpose:
 *      Get QSGMII AN mode enable or not info
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - QSGMII AN enabled or not.
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
_phy_qtce_qsgmii_an_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t                *pc;              
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       an = 0, an_done = 0;

    *enable = 0;
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    if (pmc == NULL) {
        return SOC_E_INTERNAL;
    }
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    if (PHYMOD_ACC_F_QMODE_GET(&phy->pm_phy.access)) {
        SOC_IF_ERROR_RETURN
            (phy_qtce_an_get(unit, port, &an, &an_done));
        if (an) {
            *enable = 1;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qtce_interface_set
 * Purpose:
 *      Set PHY interface type
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      pif   - Interface type
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_qtce_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t                *pc;
    qtce_config_t             *pCfg;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (qtce_config_t *) pc->driver_data;
    pCfg->speed_config.line_interface = (uint32) pif;

    if (pif == SOC_PORT_IF_GMII) {
        pCfg->speed_config.fiber_pref = 1;
    } else if (pif == SOC_PORT_IF_SGMII) {
        pCfg->speed_config.fiber_pref = 0;
    }

    return (SOC_E_NONE);
}
/*
 * Function:
 *      phy_qtce_interface_get
 * Purpose:
 *      Get PHY interface type
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      pif   - current interface type
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_qtce_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    int flag = 0;
    phymod_ref_clk_t ref_clk;
    qtce_config_t *pCfg;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;


    /* now loop through all cores */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (qtce_config_t *) pc->driver_data;
    SOC_IF_ERROR_RETURN(
        qtce_ref_clk_convert(pCfg->speed_config.port_refclk_int, &ref_clk));

    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&phy->pm_phy,
                                         flag, ref_clk, &interface_config));
    switch (interface_config.interface_type) {
        case phymodInterface1000X:
            *pif = SOC_PORT_IF_GMII;
            break;
        case phymodInterfaceSGMII:
            *pif = SOC_PORT_IF_SGMII;
            break;
        default:
            *pif =  SOC_PORT_IF_SGMII;
            break;
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_qtce_speed_set
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
phy_qtce_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t                *pc;
    qtce_config_t             *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       idx;
    qtce_speed_config_t       speed_config;
    phymod_phy_inf_config_t   interface_config;
    phymod_autoneg_control_t  an_control;
    int                       an_complete;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    if (speed == 0) {
        return SOC_E_NONE;
    }

    if (speed == 0) {
        /* coverity[dead_error_line] */
        return SOC_E_NONE; 
    }

    pmc = &pc->phymod_ctrl;
    pCfg = (qtce_config_t *) pc->driver_data;

    /* take a copy of core and phy configuration values, and set the desired speed */
    sal_memcpy(&speed_config, &pCfg->speed_config, sizeof(speed_config));
    speed_config.speed = speed;

    /* determine the interface configuration */
    SOC_IF_ERROR_RETURN
        (qtce_speed_to_interface_config_get(&speed_config, &interface_config));

    /* now loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            return SOC_E_INTERNAL;
        }

        /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
        SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_set(&phy->pm_phy,
                                             0 /* flags */, &interface_config));
    }

    /* record success */
    pCfg->speed_config.speed = speed;

    sal_memset(&an_control, 0x0, sizeof(an_control));
    idx = pmc->main_phy;
    phy = pmc->phy[idx];
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_get(&phy->pm_phy, &an_control, 
                                (uint32_t *) &an_complete));

    if (an_control.enable) {
        /* The autoneg might take up to 300 ms to link up. */
        sal_msleep(300);
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_qtce_speed_get
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
phy_qtce_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    int flag = 0;
    phymod_ref_clk_t ref_clk;
    qtce_config_t *pCfg;

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    /* initialize the data structure */
    interface_config.data_rate = 0;

    /* now loop through all cores */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (qtce_config_t *) pc->driver_data;
    SOC_IF_ERROR_RETURN(
        qtce_ref_clk_convert(pCfg->speed_config.port_refclk_int, &ref_clk));

    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&phy->pm_phy,
                                         flag, ref_clk, &interface_config));
    *speed = interface_config.data_rate;
    return (SOC_E_NONE);
}

/*
 * Function:    
 *      phy_qtce_an_set
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
phy_qtce_an_set(int unit, soc_port_t port, int enable)
{
#if 1   
    phy_ctrl_t                *pc;
    qtce_config_t             *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_autoneg_control_t  an;
#if 0
    soc_info_t *si;
#endif

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_control_t_init(&an);
    pmc = &pc->phymod_ctrl;
    pCfg = (qtce_config_t *) pc->driver_data;
#if 0
    si = &SOC_INFO(unit);
#endif

    /* only request autoneg on the first core */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    an.enable = enable;
    an.num_lane_adv = 1;
    an.an_mode = phymod_AN_MODE_NONE;
    /*first check if qsgmii mode */
    if (pCfg->fiber_pref) {
        an.an_mode = phymod_AN_MODE_CL37;
    } else {
        an.an_mode = phymod_AN_MODE_SGMII;
    }

    if (pCfg->cl37an ==  TEMOD_CL37_W_BAM) {
        an.an_mode = phymod_AN_MODE_CL37BAM;
    } 

    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_set(&phy->pm_phy, &an));
#endif
    return (SOC_E_NONE);
}

/*
 * Function:    
 *      phy_qtce_an_get
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
phy_qtce_an_get(int unit, soc_port_t port, int *an, int *an_done)
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
 *      phy_qtce_ability_advert_set
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
phy_qtce_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
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

    /* only set abilities on the first core */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    case SOC_PA_PAUSE_RX:
        /*an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE; */
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    }

    /* also set the sgmii speed */
    if(ability->speed_half_duplex) {
        if(ability->speed_half_duplex & SOC_PA_SPEED_1000MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_1000M;
        } else if(ability->speed_half_duplex & SOC_PA_SPEED_100MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_100M;
        } else if(ability->speed_half_duplex & SOC_PA_SPEED_10MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_10M;
        }
        PHYMOD_AN_CAP_HALF_DUPLEX_SET(&phymod_autoneg_ability);
    }
    if(ability->speed_full_duplex) {
        if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_1000M;
        } else if(ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_100M;
        } else if(ability->speed_full_duplex & SOC_PA_SPEED_10MB) {
            PHYMOD_AN_CAP_SGMII_SET(&phymod_autoneg_ability);
            phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_10M;
        }
        if(ability->speed_full_duplex & SOC_PA_SPEED_2500MB) {
            PHYMOD_AN_CAP_CL37BAM_SET(&phymod_autoneg_ability);
        }
        PHYMOD_AN_CAP_HALF_DUPLEX_CLR(&phymod_autoneg_ability);
    }
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_set(&phy->pm_phy, &phymod_autoneg_ability));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qtce_ability_advert_get
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
phy_qtce_ability_advert_get(int unit, soc_port_t port,
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
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_get(&phy->pm_phy, &phymod_autoneg_ability));

    speed_full_duplex = 0;

    reg37_ability = phymod_autoneg_ability.cl37bam_cap;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_2P5G_GET(reg37_ability)?       SOC_PA_SPEED_2500MB:0;
    
    reg_ability = phymod_autoneg_ability.capabilities & (PHYMOD_AN_CAP_SYMM_PAUSE|PHYMOD_AN_CAP_ASYM_PAUSE);

    ability->pause = 0;
    if (reg_ability == PHYMOD_AN_CAP_ASYM_PAUSE) {
        ability->pause = SOC_PA_PAUSE_TX;
    } else if (reg_ability == (PHYMOD_AN_CAP_SYMM_PAUSE|PHYMOD_AN_CAP_ASYM_PAUSE)) {
        ability->pause = SOC_PA_PAUSE_RX;
    } else if (reg_ability == PHYMOD_AN_CAP_SYMM_PAUSE) {
        ability->pause = (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX);
    }

    if(PHYMOD_AN_CAP_SGMII_GET(&phymod_autoneg_ability)) {
        
        if(phymod_autoneg_ability.sgmii_speed == phymod_CL37_SGMII_10M) {
            speed_full_duplex|= SOC_PA_SPEED_10MB ;
        }
        if(phymod_autoneg_ability.sgmii_speed == phymod_CL37_SGMII_100M) {
            speed_full_duplex|= SOC_PA_SPEED_100MB ;
        }
        if(phymod_autoneg_ability.sgmii_speed == phymod_CL37_SGMII_1000M) {
            speed_full_duplex|= SOC_PA_SPEED_1000MB ;
        }

        if(PHYMOD_AN_CAP_HALF_DUPLEX_GET(&phymod_autoneg_ability)) {
            ability->speed_full_duplex = 0 ;
            ability->speed_half_duplex = speed_full_duplex;
        } else {
            ability->speed_full_duplex = speed_full_duplex;
            ability->speed_half_duplex = 0;
        }
    } else { /* simplify CL37 */
        speed_full_duplex|= SOC_PA_SPEED_1000MB ;
        ability->speed_full_duplex = speed_full_duplex;
        ability->speed_half_duplex = 0;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qtce_ability_remote_get
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
phy_qtce_ability_remote_get(int unit, soc_port_t port,
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
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_remote_ability_get(&phy->pm_phy, &phymod_autoneg_ability));

    speed_full_duplex = 0;
    reg37_ability = phymod_autoneg_ability.cl37bam_cap;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_2P5G_GET(reg37_ability)? SOC_PA_SPEED_2500MB:0;
    

    reg_ability = phymod_autoneg_ability.capabilities & (PHYMOD_AN_CAP_SYMM_PAUSE|PHYMOD_AN_CAP_ASYM_PAUSE);
    ability->pause = 0;
    if (reg_ability == PHYMOD_AN_CAP_ASYM_PAUSE) {
        ability->pause = SOC_PA_PAUSE_TX;
    } else if (reg_ability == (PHYMOD_AN_CAP_SYMM_PAUSE|PHYMOD_AN_CAP_ASYM_PAUSE)) {
        ability->pause = SOC_PA_PAUSE_RX;
    } else if (reg_ability == PHYMOD_AN_CAP_SYMM_PAUSE) {
        ability->pause = (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX);
    }
    if(PHYMOD_AN_CAP_SGMII_GET(&phymod_autoneg_ability)) {
        if(phymod_autoneg_ability.sgmii_speed == phymod_CL37_SGMII_10M) {
            speed_full_duplex|= SOC_PA_SPEED_10MB ;
        }
        if(phymod_autoneg_ability.sgmii_speed == phymod_CL37_SGMII_100M) {
            speed_full_duplex|= SOC_PA_SPEED_100MB ;
        }
        if(phymod_autoneg_ability.sgmii_speed == phymod_CL37_SGMII_1000M) {
            speed_full_duplex|= SOC_PA_SPEED_1000MB ;
        }

        if(PHYMOD_AN_CAP_HALF_DUPLEX_GET(&phymod_autoneg_ability)) {
            ability->speed_full_duplex = 0 ;
            ability->speed_half_duplex = speed_full_duplex;
        } else {
            ability->speed_full_duplex = speed_full_duplex;
            ability->speed_half_duplex = 0;
        }
    } else { /* simplify CL37 */
        speed_full_duplex|= SOC_PA_SPEED_1000MB ;
        
        ability->speed_full_duplex = speed_full_duplex;
        ability->speed_half_duplex = 0;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qtce_lb_set
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
phy_qtce_lb_set(int unit, soc_port_t port, int enable)
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
 *      phy_qtce_lb_get
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
phy_qtce_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    uint32_t lb_enable = 0;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;

    *enable = 0;

    pm_phy = &pmc->phy[0]->pm_phy;
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(pm_phy, phymodLoopbackGlobal, &lb_enable));
    *enable = (int) lb_enable;
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_qtce_ability_local_get
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
phy_qtce_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    soc_phymod_phy_t    *phy;
    qtce_config_t    *pCfg;

    pc = INT_PHY_SW_STATE(unit, port);

    if (NULL == pc) {
        return SOC_E_INTERNAL;
    }

    pCfg = (qtce_config_t *) pc->driver_data;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }
    pmc = &pc->phymod_ctrl;
    if (NULL == pmc) {
        return SOC_E_INTERNAL;
    }
    phy = pmc->phy[pmc->num_phys - 1];
    if (NULL == phy) {
        return SOC_E_INTERNAL;
    }

    sal_memset(ability, 0, sizeof(*ability));

    ability->medium    = SOC_PA_MEDIUM_COPPER | SOC_PA_MEDIUM_FIBER;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->flags     = SOC_PA_AUTONEG;

    if (PHYMOD_ACC_F_QMODE_GET(&phy->pm_phy.access)) {
        ability->speed_half_duplex   = SOC_PA_SPEED_10MB |
                                       SOC_PA_SPEED_100MB|
                                       SOC_PA_SPEED_1000MB;

        ability->speed_full_duplex   = SOC_PA_SPEED_10MB |
                                       SOC_PA_SPEED_100MB|
                                       SOC_PA_SPEED_1000MB;
        /* QMODE doesn't support PHY loopback per subport */
    } else {
        if (pCfg->fiber_pref)   {
            ability->speed_full_duplex   = SOC_PA_SPEED_1000MB |
                                           SOC_PA_SPEED_2500MB;
        } else {
            ability->speed_half_duplex   = SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB|
                                           SOC_PA_SPEED_1000MB;

            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB|
                                           SOC_PA_SPEED_1000MB;
            if (pc->speed_max >= 2500) {
                ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
            }
        }
        ability->loopback  = SOC_PA_LB_PHY;
    }

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_SGMII | SOC_PA_INTF_GMII;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_qtce_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));
    return (SOC_E_NONE);
}

/*
 * Function:
 *      qtce_uc_status_dump
 * Purpose:
 *      display all the serdes related parameters
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
qtce_uc_status_dump(int unit, soc_port_t port, void *arg)
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
            (phymod_phy_pmd_info_dump(pm_phy, arg)); 
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      qtce_pcs_status_dump
 * Purpose:
 *      display all the serdes related parameters
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
qtce_pcs_status_dump(int unit, soc_port_t port, void *arg)
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
_qtce_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, uint32_t lane, 
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
 * qtce_per_lane_preemphasis_set
 */
STATIC int
qtce_per_lane_preemphasis_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    phymod_tx.pre = value;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * qtce_preemphasis_set
 */
STATIC int
qtce_preemphasis_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
        phymod_tx.pre = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));

        
    }

    return(SOC_E_NONE);
}

/* 
 * qtce_tx_fir_pre_set
 */
STATIC int
qtce_tx_fir_pre_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
        phymod_tx.pre = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * qtce_tx_fir_main_set
 */
STATIC int
qtce_tx_fir_main_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
 * qtce_tx_fir_post_set
 */
STATIC int
qtce_tx_fir_post_set(soc_phymod_ctrl_t *pmc, uint32 value)
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

/* 
 * qtce_tx_fir_post2_set
 */
STATIC int
qtce_tx_fir_post2_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
 * qtce_tx_fir_post3_set
 */
STATIC int
qtce_tx_fir_post3_set(soc_phymod_ctrl_t *pmc, uint32 value)
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


/* 
 * qtce_per_lane_driver_current_set
 */
STATIC int
qtce_per_lane_driver_current_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    phymod_tx.amp = value;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    

    return(SOC_E_NONE);
}

/* 
 * qtce_driver_current_set
 */
STATIC int
qtce_driver_current_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
        phymod_tx.amp = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(pm_phy, &phymod_tx));
    }

    

    return(SOC_E_NONE);
}

/* 
 * qtce_per_lane_rx_dfe_tap_control_set
 */
STATIC int
qtce_per_lane_rx_dfe_tap_control_set(soc_phymod_ctrl_t *pmc, int lane, int tap, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    if ((tap<0)||(tap>(COUNTOF(phymod_rx.dfe) - 1))) {
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
 * qtce_tx_lane_squelch
 */
STATIC int
qtce_tx_lane_squelch(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_power_t  phy_power;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        if (value == 1) {
            phy_power.tx = phymodPowerOff;
            phy_power.rx = phymodPowerNoChange;
        } else {
            phy_power.tx = phymodPowerOn;
            phy_power.rx = phymodPowerNoChange;
        }
        SOC_IF_ERROR_RETURN
            (phymod_phy_power_set(pm_phy, &phy_power));
    }
    return(SOC_E_NONE);
}

/* 
 * qtce_per_lane_rx_peak_filter_set
 */
STATIC int
qtce_per_lane_rx_peak_filter_set(soc_phymod_ctrl_t *pmc, int lane, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

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
 * qtce_rx_peak_filter_set
 */
STATIC int 
qtce_rx_peak_filter_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
 * qtce_per_lane_rx_vga_set
 */
STATIC int
qtce_per_lane_rx_vga_set(soc_phymod_ctrl_t *pmc, int lane, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

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

/* 
 * qtce_rx_vga_set
 */
STATIC int 
qtce_rx_vga_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
        phymod_rx.vga.enable = TRUE;
        phymod_rx.vga.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(pm_phy, &phymod_rx));
    }

    return(SOC_E_NONE);
}

STATIC int
qtce_rx_tap_release(soc_phymod_ctrl_t *pmc, int tap)
{
    phymod_phy_access_t *pm_phy;
    phymod_rx_t         phymod_rx;
    int                 idx;

    /* bounds check "tap" */
    if ((tap < 0) || (tap >= COUNTOF(phymod_rx.dfe))) {
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
 * qtce_rx_tap_set
 */
STATIC int 
qtce_rx_tap_set(soc_phymod_ctrl_t *pmc, int tap, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_rx_t         phymod_rx;
    int                 idx;

    /* bounds check "tap" */
    if ((tap < 0) || (tap > COUNTOF(phymod_rx.dfe) - 1)) {
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
 * qtce_pi_control_set
 */
STATIC int 
qtce_pi_control_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
 * qtce_tx_polarity_set
 */
STATIC int 
qtce_tx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, uint32 value)
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
 * qtce_rx_polarity_set
 */

STATIC int 
qtce_rx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, uint32 value)
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
 * qtce_rx_reset
 */
STATIC int
qtce_rx_reset(soc_phymod_ctrl_t *pmc, phymod_phy_reset_t *cfg_reset, uint32 value)
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
 * qtce_tx_reset
 */
STATIC int
qtce_tx_reset(soc_phymod_ctrl_t *pmc, phymod_phy_reset_t *cfg_reset, uint32 value)
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

/* 
 * qtce_cl72_enable_set
 */
STATIC int
qtce_cl72_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_cl72_set(pm_phy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
qtce_lane_map_set(soc_phymod_ctrl_t *pmc, uint32 value){
    int idx;
    phymod_lane_map_t lane_map;

    lane_map.num_of_lanes = NUM_LANES;
    if(pmc->phy[0] == NULL){
        return(SOC_E_INTERNAL);
    }
    for (idx=0; idx < NUM_LANES; idx++) {
        lane_map.lane_map_rx[idx] = (value >> (idx * 4 /*4 bit per lane*/)) & 0xf;
    }
    for (idx=0; idx < NUM_LANES; idx++) {
        lane_map.lane_map_tx[idx] = (value >> (16 + idx * 4 /*4 bit per lane*/)) & 0xf;
    }
    SOC_IF_ERROR_RETURN(phymod_core_lane_map_set(&pmc->phy[0]->core->pm_core, &lane_map));
    return(SOC_E_NONE);
}


STATIC int
qtce_lane_map_get(soc_phymod_ctrl_t *pmc, uint32 *value){
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
        *value += (lane_map.lane_map_rx[idx]<< (idx * 4 + 16));
    }
    
    return(SOC_E_NONE);
}


STATIC int
qtce_pattern_len_set(soc_phymod_ctrl_t *pmc, uint32_t value)
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
qtce_pattern_enable_set(soc_phymod_ctrl_t *pmc, uint32_t value)
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
 * qtce_pattern_data_set 
 *    Sets 32 bits of the 256 bit data pattern.
 */
STATIC int
qtce_pattern_data_set(int idx, qtce_pattern_t *pattern, uint32 value)
{
    if ((idx<0) || (idx >= COUNTOF(pattern->pattern_data))) {
        return SOC_E_INTERNAL;
    }

    /* update pattern data */
    pattern->pattern_data[idx] = value;

    return(SOC_E_NONE);
}

/* 
 * qtce_per_lane_prbs_poly_set
 */
STATIC int
qtce_per_lane_prbs_poly_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}


STATIC
int
qtce_sdk_poly_to_phymod_poly(uint32 sdk_poly, phymod_prbs_poly_t *phymod_poly){
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
 * qtce_prbs_tx_poly_set
 */
STATIC int
qtce_prbs_tx_poly_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
    SOC_IF_ERROR_RETURN(qtce_sdk_poly_to_phymod_poly(value, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    return(SOC_E_NONE);
}

/* 
 * qtce_prbs_rx_poly_set
 */
STATIC int
qtce_prbs_rx_poly_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags,  &prbs));
    SOC_IF_ERROR_RETURN(qtce_sdk_poly_to_phymod_poly(value, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    return(SOC_E_NONE);
}


/* 
 * qtce_per_lane_prbs_tx_invert_data_set
 */
STATIC int
qtce_per_lane_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_prbs_tx_invert_data_set
 */
STATIC int
qtce_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
    prbs.invert = value;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    return(SOC_E_NONE);
}

/* 
 * qtce_prbs_rx_invert_data_set
 */
STATIC int
qtce_prbs_rx_invert_data_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
    prbs.invert = value;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags,  &prbs));
    return(SOC_E_NONE);
}

/* 
 * qtce_per_lane_prbs_tx_enable_set
 */
STATIC int
qtce_per_lane_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}
/* 
 * qtce_prbs_tx_enable_set
 */
STATIC int
qtce_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(pm_phy, flags, value));
    return(SOC_E_NONE);
}

/* 
 * qtce_prbs_rx_enable_set
 */
STATIC int
qtce_prbs_rx_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(pm_phy, flags, value));
    return(SOC_E_NONE);
}

/* 
 * qtce_loopback_internal_set
 */
STATIC int 
qtce_loopback_internal_set(soc_phymod_ctrl_t *pmc, uint32 value)
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

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(pm_phy, phymodLoopbackGlobal, value));

    return(SOC_E_NONE);
}

/* 
 * qtce_loopback_internal_pmd_set
 */
STATIC int 
qtce_loopback_internal_pmd_set(soc_phymod_ctrl_t *pmc, uint32 value)
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

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(pm_phy, phymodLoopbackGlobalPMD, value));

    return(SOC_E_NONE);
}

/* 
 * qtce_loopback_remote_set
 */
STATIC int 
qtce_loopback_remote_set(soc_phymod_ctrl_t *pmc, uint32 value)
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

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(pm_phy, phymodLoopbackRemotePMD, value));

    return(SOC_E_NONE);
}

/* 
 * qtce_loopback_remote_pcs_set
 */
STATIC int 
qtce_loopback_remote_pcs_set(soc_phymod_ctrl_t *pmc, uint32 value)
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

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(pm_phy, phymodLoopbackRemotePCS, value));

    return(SOC_E_NONE);
}

/* 
 * qtce_fec_enable_set
 */
STATIC int 
qtce_fec_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
 * qtce_scrambler_set
 */
STATIC int
qtce_scrambler_set(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_inf_config_t config;
    int                     idx;
    phy_ctrl_t              *pc;
    qtce_config_t        *pCfg;
    phymod_ref_clk_t        ref_clk;

    pc = INT_PHY_SW_STATE(pmc->unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (qtce_config_t *) pc->driver_data;
    SOC_IF_ERROR_RETURN(
        qtce_ref_clk_convert(pCfg->speed_config.port_refclk_int, &ref_clk));

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(pm_phy, 0 /* flags */, ref_clk, &config));
        PHYMOD_INTF_MODES_SCR_SET(&config);
        SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(pm_phy, PHYMOD_INTF_F_DONT_OVERIDE_TX_PARAMS, &config));
    }

#if 0
   /* tscmod had the following side effect; assume this is not needed */
   DEV_CFG_PTR(pc)->scrambler_en = value? TRUE: FALSE;
#endif

    return(SOC_E_NONE);
}

/* 
 * qtce_8b10b_set
 */
STATIC int
qtce_8b10b_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
#if 0
In discussion: whether encoding API should be separate
Setting interface and speed:
typedef struct phymod_phy_inf_config_s {
    phymod_interface_t interface_type;          
    uint32                      data_rate;          /*Use PHYMOD_DEFAULT_RATE of interface default*/
    uint32                      interface_modes;    
} phymod_phy_inf_config_t;

#define PHYMOD_DEFAULT_RATE 0xFFFFFFFF

int phymod_phy_interface_config_set(int unit, uint32 phy_id, uint32 flags, const phymod_phy_inf_config_t* config);
int phymod_phy_interface_config_get(int unit, uint32 phy_id, uint32 flags, phymod_phy_inf_config_t* config);
#endif

   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_64b65b_set
 */
STATIC int
qtce_64b65b_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
#if 0
In discussion: whether encoding API should be separate
Setting interface and speed:
typedef struct phymod_phy_inf_config_s {
    phymod_interface_t interface_type;          
    uint32                      data_rate;          /*Use PHYMOD_DEFAULT_RATE of interface default*/
    uint32                      interface_modes;    
} phymod_phy_inf_config_t;

#define PHYMOD_DEFAULT_RATE 0xFFFFFFFF

int phymod_phy_interface_config_set(int unit, uint32 phy_id, uint32 flags, const phymod_phy_inf_config_t* config);
int phymod_phy_interface_config_get(int unit, uint32 phy_id, uint32 flags, phymod_phy_inf_config_t* config);
#endif

   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_power_set
 */
STATIC int
qtce_power_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
 * qtce_per_lane_rx_low_freq_filter_set
 */
STATIC int
qtce_per_lane_rx_low_freq_filter_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_rx_low_freq_filter_set
 */
STATIC int
qtce_rx_low_freq_filter_set(soc_phymod_ctrl_t *pmc, uint32 value)
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
 * qtce_tx_ppm_adjust_set
 */
STATIC int
qtce_tx_ppm_adjust_set(soc_phymod_ctrl_t *pmc, uint32 value)
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

/* 
 * qtce_vco_freq_set
 */
STATIC int
qtce_vco_freq_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
#if 0
The following 3 controls are used to override pre-defined speed in the phy.
What we discussed in SJ is that in case of rate which is not officially supported the driver will try to 
understand this parameters by itself.
Questions:
1. Do we want to support direct configuration of PLL\OS?
2. If yes - same API or new API

phymod_phy_interface_config_set (?)
#endif

   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_pll_divider_set
 */
STATIC int
qtce_pll_divider_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
#if 0
phymod_phy_interface_config_set (?)
#endif

   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_oversample_mode_set
 */
STATIC int
qtce_oversample_mode_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
#if 0
phymod_phy_interface_config_set (?)
#endif

   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_ref_clk_set
 */
STATIC int
qtce_ref_clk_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
#if 0
The control stores the ref clock in SW DB. 
As discussed the phymod will be stateless. 
So itll be added to interface_config_set as input parameter.

phymod_phy_interface_config_set
#endif

   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_rx_seq_toggle_set 
 */
STATIC int
qtce_rx_seq_toggle_set(soc_phymod_ctrl_t *pmc)
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
 * qtce_eee_set 
 */
STATIC int
qtce_eee_set(soc_phymod_ctrl_t *pmc, uint32 value)
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

/* 
 * qtce_driver_supply_set
 */
STATIC int
qtce_driver_supply_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
#if 0
Can be added as init parameter.
Do you think set\get API are required?
#endif

   return(SOC_E_UNAVAIL);
}

/*
 * Function:
 *      phy_qtce_control_set
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
phy_qtce_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    qtce_config_t       *pCfg;

    rv = SOC_E_UNAVAIL;

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    /* locate phy control, phymod control, and the configuration data */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    pCfg = (qtce_config_t *) pc->driver_data;

    switch(type) {

    case SOC_PHY_CONTROL_TX_FIR_PRE:
        rv = qtce_tx_fir_pre_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        rv = qtce_tx_fir_main_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        rv = qtce_tx_fir_post_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        rv = qtce_tx_fir_post2_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        rv = qtce_tx_fir_post3_set(pmc, value);
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = qtce_per_lane_preemphasis_set(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = qtce_per_lane_preemphasis_set(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = qtce_per_lane_preemphasis_set(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = qtce_per_lane_preemphasis_set(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = qtce_preemphasis_set(pmc, value);
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = qtce_per_lane_driver_current_set(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = qtce_per_lane_driver_current_set(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = qtce_per_lane_driver_current_set(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = qtce_per_lane_driver_current_set(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = qtce_driver_current_set(pmc, value);
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
        rv = qtce_tx_lane_squelch(pmc, value);
        break;

    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = qtce_rx_peak_filter_set(pmc, value);
        break;

    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        rv = qtce_rx_vga_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:
       /* $$$ tbd $$$ */
       break;

    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = qtce_rx_tap_set(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = qtce_rx_tap_set(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = qtce_rx_tap_set(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = qtce_rx_tap_set(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = qtce_rx_tap_set(pmc, 4, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:       /* $$$ tbd $$$ */
       rv = qtce_rx_tap_release(pmc, 0);
       break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:       /* $$$ tbd $$$ */
       rv = qtce_rx_tap_release(pmc, 1);
       break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:       /* $$$ tbd $$$ */
       rv = qtce_rx_tap_release(pmc, 2);
       break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:       /* $$$ tbd $$$ */
       rv = qtce_rx_tap_release(pmc, 3);
       break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:       /* $$$ tbd $$$ */
       rv = qtce_rx_tap_release(pmc, 4);
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
        rv = qtce_pi_control_set(pmc, value);
        break;

    /* POLARITY */
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = qtce_rx_polarity_set(pmc, &pCfg->phy_polarity_config, value);;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = qtce_tx_polarity_set(pmc, &pCfg->phy_polarity_config, value);
        break;

    /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        rv = qtce_rx_reset(pmc, &pCfg->phy_reset_config, value);
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        rv = qtce_tx_reset(pmc, &pCfg->phy_reset_config, value);
        break;

    /* CL72 ENABLE */
    case SOC_PHY_CONTROL_CL72:
        rv = qtce_cl72_enable_set(pmc, value);
        break;

    /* LANE SWAP */
    case SOC_PHY_CONTROL_LANE_SWAP:
        rv = qtce_lane_map_set(pmc, value);
        break;

    /* TX PATTERN */
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        rv = qtce_pattern_len_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        rv = qtce_pattern_enable_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        rv = qtce_pattern_data_set(0, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        rv = qtce_pattern_data_set(1, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        rv = qtce_pattern_data_set(2, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        rv = qtce_pattern_data_set(3, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        rv = qtce_pattern_data_set(4, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        rv = qtce_pattern_data_set(5, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        rv = qtce_pattern_data_set(6, &pCfg->pattern, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        rv = qtce_pattern_data_set(7, &pCfg->pattern, value);
        break;

    /* decoupled PRBS */
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        rv = qtce_prbs_tx_poly_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        rv = qtce_prbs_tx_invert_data_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        rv = qtce_prbs_tx_enable_set(pmc, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        rv = qtce_prbs_rx_poly_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        rv = qtce_prbs_rx_invert_data_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        rv = qtce_prbs_rx_enable_set(pmc, value);
        break;
    /*for legacy prbs usage mainly set both tx/rx the same */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = qtce_prbs_tx_poly_set(pmc, value);
        rv = qtce_prbs_rx_poly_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = qtce_prbs_tx_invert_data_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = qtce_prbs_tx_enable_set(pmc, value);
        rv = qtce_prbs_rx_enable_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = qtce_prbs_tx_enable_set(pmc, value);
        rv = qtce_prbs_rx_enable_set(pmc, value);
        break;

    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        rv = qtce_loopback_internal_set(pmc, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
        rv = qtce_loopback_internal_pmd_set(pmc, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = qtce_loopback_remote_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        rv = qtce_loopback_remote_pcs_set(pmc, value);
        break;

    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        rv = qtce_fec_enable_set(pmc, value);
        break;

    /* CUSTOM */
    case SOC_PHY_CONTROL_CUSTOM1:
       /* Obsolete ??
       DEV_CFG_PTR(pc)->custom1 = value? TRUE: FALSE ;
       rv = SOC_E_NONE; */
       rv = SOC_E_UNAVAIL;
       break;

    /* SCRAMBLER */
    case SOC_PHY_CONTROL_SCRAMBLER:
        rv = qtce_scrambler_set(pmc, port, value);
        break;

    /* 8B10B */
    case SOC_PHY_CONTROL_8B10B:
        rv = qtce_8b10b_set(pmc, value);
        break;

    /* 64B65B */
    case SOC_PHY_CONTROL_64B66B:
        rv = qtce_64b65b_set(pmc, value);
        break;

    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = qtce_power_set(pmc, value);
       break;

    /* RX_LOW_FREQ_PEAK_FILTER */
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
       rv = qtce_rx_low_freq_filter_set(pmc, value);
       break;

    /* TX_PPM_ADJUST */
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
       rv = qtce_tx_ppm_adjust_set(pmc, value);
       break;

    /* VCO_FREQ */
    case SOC_PHY_CONTROL_VCO_FREQ:
       rv = qtce_vco_freq_set(pmc, value);
       break;

    /* PLL_DIVIDER */
    case SOC_PHY_CONTROL_PLL_DIVIDER:
       rv = qtce_pll_divider_set(pmc, value);
       break;

    /* OVERSAMPLE_MODE */
    case SOC_PHY_CONTROL_OVERSAMPLE_MODE:
       rv = qtce_oversample_mode_set(pmc, value);
       break;

    /* REF_CLK */
    case SOC_PHY_CONTROL_REF_CLK:
       rv = qtce_ref_clk_set(pmc, value);
       break;

    /* RX_SEQ_TOGGLE */
    case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
       rv = qtce_rx_seq_toggle_set(pmc);
       break;

    /* DRIVER_SUPPLY */
    case SOC_PHY_CONTROL_DRIVER_SUPPLY:
       rv = qtce_driver_supply_set(pmc, value);
       break;

    /* EEE */
    case SOC_PHY_CONTROL_EEE:
       rv = qtce_eee_set(pmc, value);
       break;
#ifdef TSC_EEE_SUPPORT
    case SOC_PHY_CONTROL_EEE_AUTO:
        rv = SOC_E_NONE;   /* not supported */
        break;
#endif
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}


/* 
 * qtce_tx_fir_pre_get
 */
STATIC int
qtce_tx_fir_pre_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_tx_fir_main_get
 */
STATIC int
qtce_tx_fir_main_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_tx_fir_post_get
 */
STATIC int
qtce_tx_fir_post_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_tx_fir_post2_get
 */
STATIC int
qtce_tx_fir_post2_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_tx_fir_post3_get
 */
STATIC int
qtce_tx_fir_post3_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_per_lane_preemphasis_get
 */
STATIC int
qtce_per_lane_preemphasis_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.pre;

    return(SOC_E_NONE);
}

STATIC int
qtce_driver_current_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;

    pm_phy = &pmc->phy[0]->pm_phy;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
    *value = phymod_tx.amp;

    return(SOC_E_NONE);
}


/* 
 * qtce_per_lane_driver_current_get
 */
STATIC int
qtce_per_lane_driver_current_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.amp;

    return(SOC_E_NONE);
}

/* 
 * qtce_per_lane_prbs_poly_get
 */
STATIC int
qtce_per_lane_prbs_poly_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_per_lane_prbs_tx_invert_data_get
 */
STATIC int
qtce_per_lane_prbs_tx_invert_data_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_per_lane_prbs_tx_enable_get
 */
STATIC int
qtce_per_lane_prbs_tx_enable_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_per_lane_prbs_rx_enable_get
 */
STATIC int
qtce_per_lane_prbs_rx_enable_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_per_lane_prbs_rx_status_get
 */
STATIC int
qtce_per_lane_prbs_rx_status_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   /* $$$ Need to add diagnostic API */
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_per_lane_rx_peak_filter_get
 */
STATIC int
qtce_per_lane_rx_peak_filter_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

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
 * qtce_per_lane_rx_vga_get
 */
STATIC int
qtce_per_lane_rx_vga_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

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
 * qtce_per_lane_rx_dfe_tap_control_get
 */
STATIC int
qtce_per_lane_rx_dfe_tap_control_get(soc_phymod_ctrl_t *pmc, int lane, int tap, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_qtce_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    if ((tap<0)||(tap>(COUNTOF(phymod_rx.dfe) -1 ))) {
        /* this can only happen with a coding error */
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    if (phymod_rx.dfe[tap].enable)
       *value = phymod_rx.dfe[tap].value;

    return(SOC_E_NONE);
}


/* 
 * qtce_per_lane_rx_low_freq_filter_get
 */
STATIC int
qtce_per_lane_rx_low_freq_filter_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_rx_peak_filter_get
 */
STATIC int 
qtce_rx_peak_filter_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_rx_vga_set
 */
STATIC int 
qtce_rx_vga_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
    *value = phymod_rx.vga.value;

    return(SOC_E_NONE);
}

/* 
 * qtce_rx_tap_get
 */
STATIC int 
qtce_rx_tap_get(soc_phymod_ctrl_t *pmc, int tap, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;

    if ((tap < 0)||(tap > (COUNTOF(phymod_rx.dfe) - 1))) {
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
 * qtce_pi_control_get
 */
STATIC int 
qtce_pi_control_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_rx_signal_detect_get
 */
STATIC int
qtce_rx_signal_detect_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
   /* $$$ Need to add diagnostic API */
   return(SOC_E_UNAVAIL);
}

/* 
 * qtce_rx_seq_done_get
 */
STATIC int
qtce_rx_seq_done_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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

    SOC_IF_ERROR_RETURN(phymod_phy_rx_pmd_locked_get(pm_phy, value));

    return(SOC_E_NONE);
}

/* 
 * qtce_eee_get
 */
STATIC int
qtce_eee_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_rx_ppm_get
 */
STATIC int
qtce_rx_ppm_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    /* $$$ Need to add diagnostic API */
    return(SOC_E_UNAVAIL);
}

/* 
 * qtce_cl72_enable_get
 */
STATIC int
qtce_cl72_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_cl72_status_t status;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
         return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_cl72_status_get(pm_phy, &status));
    *value = status.enabled;

    return(SOC_E_NONE);
}

/* 
 * qtce_cl72_status_get
 */
STATIC int
qtce_cl72_status_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_cl72_status_t status;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_cl72_status_get(pm_phy, &status));
    *value = status.locked;

    return(SOC_E_NONE);
}

/* 
 * qtce_prbs_tx_poly_get
 */
STATIC int
qtce_prbs_tx_poly_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
    case phymodPrbsPoly58:
        *value = 6; 
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}
/* 
 * qtce_prbs_rx_poly_get
 */
STATIC int
qtce_prbs_rx_poly_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
    case phymodPrbsPoly11:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
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
    case phymodPrbsPoly58:
        *value = 6; 
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/* 
 * qtce_prbs_tx_invert_data_get
 */
STATIC int
qtce_prbs_tx_invert_data_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_prbs_rx_invert_data_get
 */
STATIC int
qtce_prbs_rx_invert_data_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_prbs_tx_enable_get
 */
STATIC int
qtce_prbs_tx_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_prbs_rx_enable_get
 */
STATIC int
qtce_prbs_rx_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_prbs_rx_status_get
 */
STATIC int
qtce_prbs_rx_status_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_prbs_status_t   prbs_tmp;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    /* $$$ Need to add diagnostic API */
    SOC_IF_ERROR_RETURN
        (phymod_phy_prbs_status_get(pm_phy, 0, &prbs_tmp));
    if (prbs_tmp.prbs_lock == 0) {
        *value = -1;
    } else if ((prbs_tmp.prbs_lock_loss == 1) && (prbs_tmp.prbs_lock == 1)) {
        *value = -2;
    } else {
        *value = prbs_tmp.error_count;
    }     

    return(SOC_E_NONE);
}

/* 
 * qtce_loopback_internal_pmd_get (this is the PMD global loopback)
 */
STATIC int 
qtce_loopback_internal_pmd_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_loopback_remote_get
 */
STATIC int 
qtce_loopback_remote_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_loopback_remote_pcs_get
 */
STATIC int 
qtce_loopback_remote_pcs_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_fec_get
 */
STATIC int 
qtce_fec_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
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
 * qtce_pattern_256bit_get 
 *  
 * CHECK SEMANTICS: assumption is that this procedure will retrieve the pattern 
 * from Tier1 and return whether the tx pattern is enabled.  Note that this 
 * is different from the 20 bit pattern retrieval semantics 
 */
/* 
 * qtce_pattern_256bit_get 
 *  
 * CHECK SEMANTICS: assumption is that this procedure will retrieve the pattern 
 * from Tier1 and return whether the tx pattern is enabled.  Note that this 
 * is different from the 20 bit pattern retrieval semantics 
 */
STATIC int
qtce_pattern_get(soc_phymod_ctrl_t *pmc, qtce_pattern_t* cfg_pattern)
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
qtce_pattern_len_get(soc_phymod_ctrl_t *pmc, uint32_t *value)
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
 * qtce_scrambler_get
 */
STATIC int
qtce_scrambler_get(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_inf_config_t config;
    phy_ctrl_t              *pc;
    qtce_config_t        *pCfg;
    phymod_ref_clk_t        ref_clk;


    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    pc = INT_PHY_SW_STATE(pmc->unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (qtce_config_t *) pc->driver_data;
    SOC_IF_ERROR_RETURN(
        qtce_ref_clk_convert(pCfg->speed_config.port_refclk_int, &ref_clk));


    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(pm_phy, 0 /* flags */, ref_clk, &config));
    *value = PHYMOD_INTF_MODES_SCR_GET(&config);

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_qtce_control_get
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
phy_qtce_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    qtce_config_t       *pCfg;

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pCfg = (qtce_config_t *) pc->driver_data;

    switch(type) {

    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = qtce_per_lane_preemphasis_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = qtce_per_lane_preemphasis_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = qtce_per_lane_preemphasis_get(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = qtce_per_lane_preemphasis_get(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        /* assume they are all the same as lane 0 */
        rv = qtce_tx_fir_pre_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        /* assume they are all the same as lane 0 */
        rv = qtce_tx_fir_main_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        /* assume they are all the same as lane 0 */
        rv = qtce_tx_fir_post_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        /* assume they are all the same as lane 0 */
        rv = qtce_tx_fir_post2_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        /* assume they are all the same as lane 0 */
        rv = qtce_tx_fir_post3_get(pmc, value);
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = qtce_per_lane_driver_current_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = qtce_per_lane_driver_current_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = qtce_per_lane_driver_current_get(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = qtce_per_lane_driver_current_get(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = qtce_driver_current_get(pmc, value);
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
        rv = qtce_rx_peak_filter_get(pmc, value);
        break;

    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        rv = qtce_rx_vga_get(pmc, value);
        break;

    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = qtce_rx_tap_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = qtce_rx_tap_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = qtce_rx_tap_get(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = qtce_rx_tap_get(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = qtce_rx_tap_get(pmc, 4, value);
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
        rv = qtce_pi_control_get(pmc, value);
        break;

    /* RX SIGNAL DETECT */
    case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
      rv = qtce_rx_signal_detect_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_RX_SEQ_DONE:
      rv = qtce_rx_seq_done_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_RX_PPM:
      rv = qtce_rx_ppm_get(pmc, value);
      break;

    /* CL72 */
    case SOC_PHY_CONTROL_CL72:
      rv = qtce_cl72_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_CL72_STATUS:
      rv = qtce_cl72_status_get(pmc, value);
      break;

    /* PRBS */
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
      rv = qtce_prbs_tx_poly_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
      rv = qtce_prbs_tx_invert_data_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
      rv = qtce_prbs_tx_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
      rv = qtce_prbs_rx_poly_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
      rv = qtce_prbs_rx_invert_data_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
      rv = qtce_prbs_rx_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
      rv = qtce_prbs_tx_poly_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
      rv = qtce_prbs_tx_invert_data_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
      rv = qtce_prbs_tx_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
      rv = qtce_prbs_rx_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
      rv = qtce_prbs_rx_status_get(pmc, value);
      break;

    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
       rv = qtce_loopback_remote_get(pmc, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
       rv = qtce_loopback_remote_pcs_get(pmc, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
       rv = qtce_loopback_internal_pmd_get(pmc, value);
       break;

    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
      rv = qtce_fec_enable_get(pmc, value);
      break;

    /* TX PATTERN */
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
       rv = qtce_pattern_get(pmc, &pCfg->pattern);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
       rv = qtce_pattern_len_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
       rv = qtce_pattern_get(pmc, &pCfg->pattern);
      break;

    /* SCRAMBLER */
    case SOC_PHY_CONTROL_SCRAMBLER:
       rv = qtce_scrambler_get(pmc, port, value);
       break;
    case SOC_PHY_CONTROL_LANE_SWAP:
        rv = qtce_lane_map_get(pmc, value);
        break;
        
    /* EEE */
    case SOC_PHY_CONTROL_EEE:
        rv = qtce_eee_get(pmc, value);
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
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
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
 *      phy_qtce_per_lane_control_set
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
phy_qtce_per_lane_control_set(int unit, soc_port_t port, int lane, soc_phy_control_t type, uint32 value)
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

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = qtce_per_lane_preemphasis_set(pmc, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = qtce_per_lane_driver_current_set(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 0 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 0 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 1 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 1 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 2 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 2 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 3 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 3 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 4 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
        rv = qtce_per_lane_rx_dfe_tap_control_set (pmc, lane, 4 /* tap */, 0 /* release */, 0x8000);
        break;

    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = qtce_per_lane_prbs_poly_set(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = qtce_per_lane_prbs_tx_invert_data_set(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* TX_ENABLE does both tx and rx */
        rv = qtce_per_lane_prbs_tx_enable_set(pmc, lane, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = qtce_per_lane_rx_peak_filter_set(pmc, lane, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = qtce_per_lane_rx_low_freq_filter_set(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA:
        rv = qtce_per_lane_rx_vga_set(pmc, lane, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:
        rv = qtce_per_lane_rx_vga_set(pmc, lane, 0 /* release */, 0x8000);
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
 *      phy_qtce_per_lane_control_get
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
phy_qtce_per_lane_control_get(int unit, soc_port_t port, int lane,
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

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    switch(type) {

    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = qtce_per_lane_preemphasis_get(pmc, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = qtce_per_lane_driver_current_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = qtce_per_lane_prbs_poly_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = qtce_per_lane_prbs_tx_invert_data_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = qtce_per_lane_prbs_tx_enable_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = qtce_per_lane_prbs_rx_enable_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = qtce_per_lane_prbs_rx_status_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = qtce_per_lane_rx_peak_filter_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA:
        rv = qtce_per_lane_rx_vga_get(pmc, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = qtce_per_lane_rx_dfe_tap_control_get(pmc, lane, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = qtce_per_lane_rx_dfe_tap_control_get(pmc, lane, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = qtce_per_lane_rx_dfe_tap_control_get(pmc, lane, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = qtce_per_lane_rx_dfe_tap_control_get(pmc, lane, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = qtce_per_lane_rx_dfe_tap_control_get(pmc, lane, 4, value);
        break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = qtce_per_lane_rx_low_freq_filter_get(pmc, lane, value);
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
 *      phy_qtce_reg_read
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
phy_qtce_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;

    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[0]->pm_phy;
    SOC_IF_ERROR_RETURN
        (phymod_phy_reg_read(pm_phy, phy_reg_addr, phy_reg_data));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qtce_reg_write
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
phy_qtce_reg_write(int unit, soc_port_t port, uint32 flags,
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
 *      phy_qtce_reg_modify
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
phy_qtce_reg_modify(int unit, soc_port_t port, uint32 flags,
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
phy_qtce_cleanup(soc_phymod_ctrl_t *pmc)
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
                core->init = FALSE;
                soc_phymod_core_destroy(pmc->unit, core);
            }
        }

        /* Destroy phy object */
        if (phy) {
            soc_phymod_phy_destroy(pmc->unit, phy);
        }
    }
    pmc->num_phys = 0;
}

STATIC void
phy_qtce_core_init(phy_ctrl_t *pc, soc_phymod_core_t *core,
                   phymod_bus_t *core_bus, uint32 core_addr)
{
    phymod_core_access_t *pm_core;
    phymod_access_t *pm_acc;

    core->unit = pc->unit;
    core->read = pc->read;
    core->write = pc->write;
    core->wrmask = pc->wrmask;
    core->port = pc->port;

    pm_core = &core->pm_core;
    phymod_core_access_t_init(pm_core);
    pm_acc = &pm_core->access;
    phymod_access_t_init(pm_acc);
    PHYMOD_ACC_USER_ACC(pm_acc) = core;
    PHYMOD_ACC_BUS(pm_acc) = core_bus;
    PHYMOD_ACC_ADDR(pm_acc) = core_addr;

    if (soc_property_port_get(pc->unit, pc->port, "qtce_sim", 0) == 45) {
        PHYMOD_ACC_F_CLAUSE45_SET(pm_acc);
    }

    return;
}

/*
 * Function:
 *      phy_qtce_probe
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
phy_qtce_probe(int unit, phy_ctrl_t *pc)
{
    int rv, idx;
    uint32 lane_map, num_phys, core_id, phy_id, found;
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
#if 0  /* for QSGMII */
    int array_max = 8;
    int array_size = 0;
#endif
    int port;
    int phy_port;  /* physical port number */
    int qmode = 0;

    rv = 0;

    /* Initialize PHY bus */
    SOC_IF_ERROR_RETURN(phymod_bus_t_init(&core_bus));
    core_bus.bus_name = "qtce_sim"; 
    core_bus.read = _qtce_reg_read; 
    core_bus.write = _qtce_reg_write;

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
    pmc->cleanup = phy_qtce_cleanup;
    /*pmc->symbols = &bcmi_qtce_serdes_symbols; */

    
    /* if (SOC_IS_GREYHOUND(unit)) */ {
        int i, blk;
        blk = -1;
        for (i=0; i< SOC_DRIVER(unit)->port_num_blktype; i++){
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            if (PBMP_MEMBER(SOC_BLOCK_BITMAP(unit, blk), port)){
                break;
            }
        }
        if (blk == -1) {
            pc->chip_num = -1;
            return SOC_E_NOT_FOUND;
        } else {
            pc->chip_num = SOC_BLOCK_NUMBER(unit, blk);
        }

        /* next check if Qmode or sgmii mode */
        if (si->port_num_lanes[port] == 1) {
            qmode = 1;
            /* current lane number for QSGMII-Eagle is 0-15 */
            /* In QSGMII mode, GH2 has two qtce cores. Each core has 16 lanes
             * Block GPORT3-4 is core0. Block GPORT5-6 is core1 */
            if (SOC_IS_GREYHOUND2(unit)) {
                pc->lane_num = (SOC_PORT_BINDEX(unit, phy_port) +
                    ((pc->chip_num % 2) ? 0 : 8)) % 16;
                pc->chip_num = (pc->chip_num - 3)/2;
            }
            else {
                pc->lane_num = (SOC_PORT_BINDEX(unit, phy_port) +
                    ((pc->chip_num % 2) ? 8 : 0)) % 16;
            }
        } else {
            /* current lane number for QSGMII-Eagle is 0-15 */
            /* In SGMII mode, GH2 has two qtce cores. Each core has 4 lanes
             * Block GPORT3 is core0. Block GPORT5 is core1 */
            if (SOC_IS_GREYHOUND2(unit)) {
                pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
                pc->chip_num = (pc->chip_num - 3)/2;
            }
            else {
                pc->lane_num = (SOC_PORT_BINDEX(unit, phy_port) +
                    ((pc->chip_num > 0) ? 8 : 0)) % 16;
                
                pc->lane_num %= 8;
            }
        }
    }

    /* request memory for the configuration structure */
    pc->size = sizeof(qtce_config_t);

    num_phys = 1;
    pc->phy_mode = PHYCTRL_QSGMII_CORE_PORT;
    lane_map = 1 << pc->lane_num;
    core_info[0].mdio_addr = pc->phy_id;

    phy_type = phymodDispatchTypeQtce;

    /* Probe cores */
    for (idx = 0; idx < num_phys ; idx++) {
        phy_qtce_core_init(pc, &core_probe, &core_bus,
                           core_info[idx].mdio_addr);
        /* Check that core is indeed an QTC/Eagle core */
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

    for (idx = 0; idx < num_phys ; idx++) {
        /* Set core and phy IDs based on PHY address */
        core_id = pc->phy_id + idx;
        phy_id = (lane_map << 16) | core_id;

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
        phy_qtce_cleanup(pmc);
        return rv;
    }

    for (idx = 0; idx < pmc->num_phys ; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;
        pm_core = &core->pm_core;

        /* Initialize core object if newly created */
        if (core->ref_cnt == 0) {
            sal_memcpy(&core->pm_bus, &core_bus, sizeof(core->pm_bus));
            phy_qtce_core_init(pc, core, &core->pm_bus,
                               core_info[idx].mdio_addr);
            /* Set dispatch type */
            pm_core->type = phy_type;
            if (qmode) {
                PHYMOD_ACC_F_QMODE_SET(&pm_core->access);
            }
        }
        core->ref_cnt++;

        /* Initialize phy access based on associated core */
        pm_acc = &phy->pm_phy.access;
        sal_memcpy(pm_acc, &pm_core->access, sizeof(*pm_acc));
        phy->pm_phy.type = phy_type;
        PHYMOD_ACC_LANE_MASK(pm_acc) = lane_map;
    }

    return SOC_E_NONE;
}


/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _qtce_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) serdesqtce is an intermediate PHY.
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
_qtce_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      _qtce_stop
 * Purpose:
 *      Put serdesqtce SERDES in or out of reset depending on conditions
 */

STATIC int
_qtce_stop(int unit, soc_port_t port)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_phy_power_t phy_power;
    int  stop, copper;
    int  enable = 0;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[0]->pm_phy;

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
                         "qtce_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));
    if (stop) {
        phy_power.tx = phymodPowerOff; 
        /* phy_power.tx = phymodPowerOn; */
        phy_power.rx = phymodPowerNoChange; 
    } else {
        phy_power.tx = phymodPowerOn;
        phy_power.rx = phymodPowerOn;
    }

    SOC_IF_ERROR_RETURN
        (_phy_qtce_qsgmii_an_get(unit, port, &enable)) ;
    
    if(!enable) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_power_set(pm_phy, &phy_power));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _qtce_notify_stop
 * Purpose:
 *      Add a reason to put serdesqtce PHY in reset.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_qtce_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;
    return _qtce_stop(unit, port);
}

/*  
 * Function:
 *      _qtce_notify_resume
 * Purpose:
 *      Remove a reason to put serdesqtce PHY in reset.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_qtce_notify_resume(int unit, soc_port_t port, uint32 flags)
{   
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
    return _qtce_stop(unit, port);
}

/*
 * Function:
 *      _qtce_notify_speed
 * Purpose:
 *      Program duplex if (and only if) serdesqtce is an intermediate PHY.
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
_qtce_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    phy_ctrl_t* pc; 
    qtce_config_t *pCfg;
    int  fiber;
    int  enable = 0;

    pc = INT_PHY_SW_STATE(unit, port);
    pCfg = (qtce_config_t *) pc->driver_data;
    fiber = pCfg->fiber_pref;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "_qtce_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (_phy_qtce_qsgmii_an_get(unit, port, &enable)) ;
    
    if(!enable) { /* to avoid the link flap for qsgmii */
        /* Put SERDES PHY in reset */
        SOC_IF_ERROR_RETURN
            (_qtce_notify_stop(unit, port, PHY_STOP_SPEED_CHG));
        
        /* Update speed */
        SOC_IF_ERROR_RETURN
            (phy_qtce_speed_set(unit, port, speed));
        
        /* Take SERDES PHY out of reset */
        SOC_IF_ERROR_RETURN
            (_qtce_notify_resume(unit, port, PHY_STOP_SPEED_CHG));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      qtce_media_setup
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
_qtce_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      _qtce_notify_mac_loopback
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
_qtce_notify_mac_loopback(int unit, soc_port_t port, uint32 enable)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      _qtce_notify_link_wait
 * Purpose:
 *      Waiting the link between external PHY and Serdes
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      timeout - timeout in micro-second; 0:use default value.
 * Returns:
 *      SOC_E_XXX
 */
#define QTC_WAIT_LINK_TIMEOUT   (500000)    /* 500 ms */
#define QTC_POLL_LINK_INTERVAL  (50000)     /* 50 ms */
STATIC int
_qtce_notify_link_wait(int unit, soc_port_t port, uint32 timeout)
{
    int rv = SOC_E_NONE;
    soc_timeout_t to;
    uint32 to_us = (timeout>0 ? timeout : QTC_WAIT_LINK_TIMEOUT);
    int link = 0;

    soc_timeout_init(&to, to_us, 0);

    for (;;) {
        rv = phy_qtce_link_get(unit, port, &link);
        if (link == 1 || SOC_FAILURE(rv)) {
            break;
        }

        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            break;
        }

        sal_usleep(QTC_POLL_LINK_INTERVAL);
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "rv=%d u=%d p=%d link=%d\n"),
              rv, unit, port, link));

    return rv;
}

STATIC int
phy_qtce_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;
    rv = SOC_E_NONE ;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    switch(event) {
    case phyEventInterface:
        rv = (_qtce_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_qtce_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_qtce_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_qtce_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_qtce_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
#if 0
        rv = (_qtce_an_set(unit, port, value));
#endif
        break;
    case phyEventMacLoopback:
        rv = (_qtce_notify_mac_loopback(unit, port, value));
        break;
    case phyEventLpiBypass:
        PHYMOD_LPI_BYPASS_SET(value);
        rv = phy_qtce_control_set(unit, port, SOC_PHY_CONTROL_EEE, value);
        break;
    case phyEventLinkWait:
        rv = (_qtce_notify_link_wait(unit, port, value));
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_qtce_diag_ctrl
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_qtce_diag_ctrl(
    int unit,        /* unit */
    soc_port_t port, /* port */
    uint32 inst,     /* the specific device block the control action directs to */
    int op_type,     /* operation types: read,write or command sequence */
    int op_cmd,      /* command code */
    void *arg)       /* command argument based on op_type/op_cmd */
{
    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_tscmod_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_DSC));
            SOC_IF_ERROR_RETURN(qtce_uc_status_dump(unit, port, arg));
            break;
        case PHY_DIAG_CTRL_PCS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_temod_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_PCS));
			SOC_IF_ERROR_RETURN(qtce_pcs_status_dump(unit, port, arg));
            break;
        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                SOC_IF_ERROR_RETURN(phy_qtce_control_set(unit,port,op_cmd,PTR_TO_INT(arg)));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                SOC_IF_ERROR_RETURN(phy_qtce_control_get(unit,port,op_cmd,(uint32 *)arg));
            }
            break ;
        }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_tsce_master_set
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
phy_qtce_master_set(int unit, soc_port_t port, int master)
{
    if (master != SOC_PORT_MS_SLAVE)
        return SOC_E_UNAVAIL;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tsce_master_get
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
phy_qtce_master_get(int unit, soc_port_t port, int *master)
{
    *master = SOC_PORT_MS_SLAVE;

    return SOC_E_NONE;
}

#ifdef BROADCOM_DEBUG
void
qtce_state_dump(int unit, soc_port_t port)
{
    /* Show PHY configuration summary */
    /* Lane status */
    /* Show Counters */
    /* Dump Registers */
}
#endif /* BROADCOM_DEBUG */

STATIC int
phy_qtce_synce_clk_ctrl_set(int unit, int port, uint32 mode0, uint32 mode1, uint32 sdm_value)
{
    phy_ctrl_t* pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_synce_clk_ctrl_t phy_synce_cfg;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[0]->pm_phy;

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);
    phy_synce_cfg.stg0_mode = mode0;
    phy_synce_cfg.stg1_mode = mode1;
    phy_synce_cfg.sdm_val = sdm_value;

    SOC_IF_ERROR_RETURN(phymod_phy_synce_clk_ctrl_set(pm_phy, phy_synce_cfg));

    return SOC_E_NONE;
}

STATIC int
phy_qtce_synce_clk_ctrl_get(int unit, int port, uint32 *mode0, uint32 *mode1, uint32 *sdm_value)
{
    phy_ctrl_t* pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_synce_clk_ctrl_t phy_synce_cfg;

    pc = INT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[0]->pm_phy;

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);

    SOC_IF_ERROR_RETURN(phymod_phy_synce_clk_ctrl_get(pm_phy, &phy_synce_cfg));

    *mode0 = phy_synce_cfg.stg0_mode;
    *mode1 = phy_synce_cfg.stg1_mode;
    *sdm_value = phy_synce_cfg.sdm_val;

    return SOC_E_NONE;
}

/*
 * Variable:
 *      qtce_drv
 * Purpose:
 *      Phy Driver for TSC/Eagle 
 */
phy_driver_t phy_qtce_drv = {
    /* .drv_name                      = */ "TSC/Eagle PHYMOD PHY Driver",
    /* .pd_init                       = */ phy_qtce_init,
    /* .pd_reset                      = */ phy_null_reset,
    /* .pd_link_get                   = */ phy_qtce_link_get,
    /* .pd_enable_set                 = */ phy_qtce_enable_set,
    /* .pd_enable_get                 = */ phy_qtce_enable_get,
    /* .pd_duplex_set                 = */ phy_null_set,
    /* .pd_duplex_get                 = */ phy_qtce_duplex_get,
    /* .pd_speed_set                  = */ phy_qtce_speed_set,
    /* .pd_speed_get                  = */ phy_qtce_speed_get,
    /* .pd_master_set                 = */ phy_qtce_master_set,
    /* .pd_master_get                 = */ phy_qtce_master_get,
    /* .pd_an_set                     = */ phy_qtce_an_set,
    /* .pd_an_get                     = */ phy_qtce_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */ 
    /* .pd_lb_set                     = */ phy_qtce_lb_set,
    /* .pd_lb_get                     = */ phy_qtce_lb_get,
    /* .pd_interface_set              = */ phy_qtce_interface_set,
    /* .pd_interface_get              = */ phy_qtce_interface_get,
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
    /* .pd_control_set                = */ phy_qtce_control_set,
    /* .pd_control_get                = */ phy_qtce_control_get,
    /* .pd_reg_read                   = */ phy_qtce_reg_read,
    /* .pd_reg_write                  = */ phy_qtce_reg_write,
    /* .pd_reg_modify                 = */ phy_qtce_reg_modify,
    /* .pd_notify                     = */ phy_qtce_notify,
    /* .pd_probe                      = */ phy_qtce_probe,
    /* .pd_ability_advert_set         = */ phy_qtce_ability_advert_set, 
    /* .pd_ability_advert_get         = */ phy_qtce_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_qtce_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_qtce_ability_local_get,
    /* .pd_firmware_set               = */ NULL,
    /* .pd_timesync_config_set        = */ NULL,
    /* .pd_timesync_config_get        = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_qtce_diag_ctrl,
    /* .pd_lane_control_set           = */ phy_qtce_per_lane_control_set,    
    /* .pd_lane_control_get           = */ phy_qtce_per_lane_control_get,
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
    /* .pd_synce_clk_ctrl_set              = */ phy_qtce_synce_clk_ctrl_set,
    /* .pd_synce_clk_ctrl_get              = */ phy_qtce_synce_clk_ctrl_get,
    /* .pd_fec_error_inject_set              = */ NULL,
    /* .pd_fec_error_inject_get              = */ NULL

};

#else
typedef int _qtce_not_empty; /* Make ISO compilers happy. */
#endif /*  INCLUDE_SERDES_QTCE */

