/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * File:        phy84756.c
 * Purpose:     SDK PHY driver for BCM84756 (MACSec)
 *
 * Supported BCM546X Family of PHY devices:
 *
 *      Device  Ports   Media                           MAC Interface
 *      84756    4       2 10G SFP+, 2 1G SFP            XFI
 *
 * Workarounds:
 *
 * References:
 *     
 * Notes:
 */ 
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_84756)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */

#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#if defined(INCLUDE_MACSEC)

#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyfege.h"
#include "phyxehg.h"
#include "phy84756.h"
#include "phy84756_i2c.h"

#if defined(INCLUDE_MACSEC)
#include <soc/macsecphy.h>
#include <bmacsec.h>
#include <bmacsec_io.h>
#endif

#if defined(INCLUDE_FCMAP)
#include <soc/fcmapphy.h>
#include <bfcmap.h>
#include <bfcmap_io.h>
#endif

#define ADVERT_ALL_COPPER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_10MB | \
         SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB)

#define ADVERT_ALL_FIBER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_1000MB) 

#define ADVERT_ALL_10GFIBER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_10GB) 


static bmacsec_error_t error_tbl[] = {
                                SOC_E_NONE,
                                SOC_E_INTERNAL,
                                SOC_E_MEMORY,
                                SOC_E_PARAM,
                                SOC_E_EMPTY,
                                SOC_E_FULL,
                                SOC_E_NOT_FOUND,
                                SOC_E_EXISTS,
                                SOC_E_TIMEOUT,
                                SOC_E_FAIL,
                                SOC_E_DISABLED,
                                SOC_E_BADID,
                                SOC_E_RESOURCE,
                                SOC_E_CONFIG,
                                SOC_E_UNAVAIL,
                                SOC_E_INIT,
                                SOC_E_PORT,
                            };
/*
 * Convert BMACSEC return codes to SOC return code.
 */
#define SOC_ERROR(e)    error_tbl[(-e)]

#define PHY84756_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))

#define READ_PHY84756_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84756_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(1, (_reg)), (_val))

#define PHY84756_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
                WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
              
#define WRITE_PHY84756_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
                PHY84756_REG_WRITE((_unit), (_phy_ctrl), \
                  SOC_PHY_CLAUSE45_ADDR(1, (_reg)), (_val))

STATIC int phy_84756_init(int unit, soc_port_t port);
STATIC int phy_84756_link_get(int unit, soc_port_t port, int *link);
STATIC int phy_84756_enable_set(int unit, soc_port_t port, int enable);
STATIC int phy_84756_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_84756_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int phy_84756_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_84756_speed_get(int unit, soc_port_t port, int *speed);
#if 0
STATIC int phy_84756_master_set(int unit, soc_port_t port, int master);
STATIC int phy_84756_master_get(int unit, soc_port_t port, int *master);
#endif
STATIC int phy_84756_an_set(int unit, soc_port_t port, int autoneg);
STATIC int phy_84756_an_get(int unit, soc_port_t port,
                                int *autoneg, int *autoneg_done);
STATIC int phy_84756_lb_set(int unit, soc_port_t port, int enable);
STATIC int phy_84756_lb_get(int unit, soc_port_t port, int *enable);
STATIC int phy_84756_medium_config_set(int unit, soc_port_t port, 
                                      soc_port_medium_t  medium,
                                      soc_phy_config_t  *cfg);
STATIC int phy_84756_medium_config_get(int unit, soc_port_t port, 
                                      soc_port_medium_t medium,
                                      soc_phy_config_t *cfg);
STATIC int phy_84756_ability_advert_set(int unit, soc_port_t port, 
                                       soc_port_ability_t *ability);
STATIC int phy_84756_ability_advert_get(int unit, soc_port_t port,
                                       soc_port_ability_t *ability);
STATIC int phy_84756_ability_remote_get(int unit, soc_port_t port,
                                       soc_port_ability_t *ability);
STATIC int phy_84756_ability_local_get(int unit, soc_port_t port, 
                                soc_port_ability_t *ability);
STATIC int phy_84756_firmware_set(int unit, int port, int offset, 
                                 uint8 *data, int len);
STATIC int phy_84756_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value);
STATIC int phy_84756_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value);
STATIC int phy_84756_linkup(int unit, soc_port_t port);
STATIC int phy_84756_interface_get(int unit, soc_port_t port,
                                   soc_port_if_t *pif);

STATIC int phy_84756_reg_read(int unit, soc_port_t port, uint32 flags,
                                   uint32 phy_reg_addr, uint32 *phy_data);

STATIC int phy_84756_reg_write(int unit, soc_port_t port, uint32 flags,
                                   uint32 phy_reg_addr, uint32 phy_data);

typedef struct phy84756_data_s {
    int phy_ext_rom_boot;
    bmacsec_dev_addr_t phy_dev_addr;
    int line_mode;
    int macsec_enable;
    int fcmap_enable;
}phy84756_data_t;

#define PHY_EXT_ROM_BOOT(_pc)  (((phy84756_data_t *)((_pc) + 1))->phy_ext_rom_boot)
#define PHY_DEV_ADDR(_pc)      (((phy84756_data_t *)((_pc) + 1))->phy_dev_addr)
#define LINE_MODE(_pc)         (((phy84756_data_t *)((_pc) + 1))->line_mode)
#define MACSEC_ENABLE(_pc)     (((phy84756_data_t *)((_pc) + 1))->macsec_enable)
#define FCMAP_ENABLE(_pc)      (((phy84756_data_t *)((_pc) + 1))->fcmap_enable)

/*
 * Function:
 *      _phy_84756_medium_config_update
 * Purpose:
 *      Update the PHY with config parameters
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      cfg - Config structure.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_84756_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{

    SOC_IF_ERROR_RETURN
        (phy_84756_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_84756_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_84756_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_84756_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/* Function:
 *    phy_84756_init
 * Purpose:    
 *    Initialize 84756 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_84756_init(int unit, soc_port_t port)
{
    phy_ctrl_t     *pc;
    int rv = SOC_E_NONE;

    pc     = EXT_PHY_SW_STATE(unit, port);

    MACSEC_ENABLE(pc) = 0;
    FCMAP_ENABLE(pc) = 0;

    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT ||
        PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1   ) {
        int fiber_preferred;
        int mmi_mdio_addr;

#if defined(INCLUDE_MACSEC)
        /* By default MACSEC is disable */
        /* Check if enable property is set */
        pc->macsec_enable = soc_property_port_get(unit, port, spn_MACSEC_ENABLE, 0);
        MACSEC_ENABLE(pc) = pc->macsec_enable;

        pc->macsec_dev_port = soc_property_port_get(unit, port, 
                                           spn_MACSEC_PORT_INDEX, -1);
        PHY_DEV_ADDR(pc) = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 

        /* Base PHY ID is used to access MACSEC core */
        mmi_mdio_addr = soc_property_port_get(unit, port, 
                                            spn_MACSEC_DEV_ADDR, -1);
        if (mmi_mdio_addr < 0) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_84756_init: "
                                 "MACSEC_DEV_ADDR property "
                                 "not configured for u=%d p=%d\n"), unit, port));
            return SOC_E_CONFIG;
        }

        pc->macsec_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, mmi_mdio_addr, 1);

        rv = bmacsec_phy84756_phy_mac_addr_set(PHY_DEV_ADDR(pc), 
                                              pc->macsec_dev_addr, 
                                              pc->macsec_dev_port,
                                              bmacsec_io_mmi1_cl45);
#endif

#if defined(INCLUDE_FCMAP)

        pc->fcmap_enable = soc_property_port_get(unit, port, spn_FCMAP_ENABLE, 0);
        FCMAP_ENABLE(pc) = pc->fcmap_enable;

        pc->fcmap_dev_port = soc_property_port_get(unit, port, 
                                           spn_FCMAP_PORT_INDEX, -1);
        PHY_DEV_ADDR(pc) = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);

        /* Base PHY ID is used to access MACSEC core */
        mmi_mdio_addr = soc_property_port_get(unit, port, 
                                            spn_FCMAP_DEV_ADDR, -1);
        if (mmi_mdio_addr < 0) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_84756_init: "
                                 "FCMAP_DEV_ADDR property "
                                 "not configured for u=%d p=%d\n"), unit, port));
            return SOC_E_CONFIG;
        }

        pc->fcmap_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, mmi_mdio_addr, 1);
        pc->fcmap_uc_dev_addr = PHY_DEV_ADDR(pc);

        rv = bmacsec_phy84756_phy_mac_addr_set(PHY_DEV_ADDR(pc), 
                                              pc->fcmap_dev_addr, 
                                              pc->fcmap_dev_port,
                                              bfcmap_io_mmi1_cl45);

#endif

        if(rv != BMACSEC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_84756_init: bmacsec_phy_mac_addr_set "
                                 "returned error for u=%d p=%d\n"), unit, port));
            return SOC_E_FAIL;
        }

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84756_init: u=%d p=%d\n"),unit, port));

        /* Preferred mode:
         * phy_fiber_pref = 1 ; Port is in SFI mode in 10G Default
         *                      when speed is changed to 1G, its 1000X
         * phy_fiber_pref = 0; Port is in SGMII mode, allows only 1G/100M/10M
         */
        fiber_preferred =
            soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 1);

        /* Initially configure for the preferred medium. */
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);

        if (fiber_preferred) {
            LINE_MODE(pc) = BMACSEC_PHY84756_INTF_SFI;
            pc->copper.enable = FALSE;
            pc->fiber.enable = TRUE;
            pc->interface = SOC_PORT_IF_XFI;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            pc->interface = SOC_PORT_IF_SGMII;
            LINE_MODE(pc) = BMACSEC_PHY84756_INTF_SGMII;
            pc->copper.enable = TRUE;
            pc->fiber.enable = FALSE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
        }
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_C45);

        pc->copper.preferred = !fiber_preferred;
        pc->copper.autoneg_enable = TRUE;
        pc->copper.autoneg_advert = ADVERT_ALL_COPPER;
        pc->copper.force_speed = 1000;
        pc->copper.force_duplex = TRUE;
        pc->copper.master = SOC_PORT_MS_AUTO;
        pc->copper.mdix = SOC_PORT_MDIX_AUTO;

        pc->fiber.preferred = TRUE;
        pc->fiber.autoneg_enable = FALSE;
        pc->fiber.autoneg_advert = ADVERT_ALL_FIBER;
        pc->fiber.force_speed = 10000;
        pc->fiber.force_duplex = TRUE;
        pc->fiber.master = SOC_PORT_MS_NONE;
        pc->fiber.mdix = SOC_PORT_MDIX_NORMAL;

        PHY_EXT_ROM_BOOT(pc) = soc_property_port_get(unit, port,
                                                spn_PHY_EXT_ROM_BOOT, 1);
        if ((!PHY_EXT_ROM_BOOT(pc)) && (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT)) {
                pc->flags |= PHYCTRL_MDIO_BCST;
                pc->dev_name = "BCM84756";
        }

        if ( PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT ) {
            /* indicate second pass of init is needed */
            PHYCTRL_INIT_STATE_SET(pc, PHYCTRL_INIT_STATE_PASS2);

            return SOC_E_NONE;
        }

    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        /* Init PHYS and MACs to defaults */
        rv = bmacsec_phy84756_init(PHY_DEV_ADDR(pc), 1, 
                                   LINE_MODE(pc), 
                                   MACSEC_ENABLE(pc), 
                                   FCMAP_ENABLE(pc), 
                                   PHY_EXT_ROM_BOOT(pc) ? 1 : (pc->flags & PHYCTRL_MDIO_BCST ? -1 : 0));

        if (rv != BMACSEC_E_NONE) {
            return SOC_E_FAIL;
        }


        /* Get Requested LED selectors (defaults are hardware defaults) */
        pc->ledmode[0] = soc_property_port_get(unit, port, spn_PHY_LED1_MODE, 0);
        pc->ledmode[1] = soc_property_port_get(unit, port, spn_PHY_LED2_MODE, 1);
        pc->ledmode[2] = soc_property_port_get(unit, port, spn_PHY_LED3_MODE, 3);
        pc->ledmode[3] = soc_property_port_get(unit, port, spn_PHY_LED4_MODE, 6);
        pc->ledctrl = soc_property_port_get(unit, port, spn_PHY_LED_CTRL, 0x8);
        pc->ledselect = soc_property_port_get(unit, port, spn_PHY_LED_SELECT, 0);

        /*
         * Some reasonable defaults
         */
#if defined(INCLUDE_MACSEC)
        pc->macsec_switch_fixed = 0;
        pc->macsec_switch_fixed_speed = 10000;
        pc->macsec_switch_fixed_duplex = 1;
        pc->macsec_switch_fixed_pause = 1;
        pc->macsec_pause_rx_fwd = 0;
        pc->macsec_pause_tx_fwd = 0;
        pc->macsec_line_ipg = 0xc;
        pc->macsec_switch_ipg = 0xc;
#endif

#if defined(INCLUDE_FCMAP)
        pc->fcmap_switch_fixed = 0;
        pc->fcmap_switch_fixed_speed = 10000;
        pc->fcmap_switch_fixed_duplex = 1;
        pc->fcmap_switch_fixed_pause = 1;
        pc->fcmap_pause_rx_fwd = 0;
        pc->fcmap_pause_tx_fwd = 0;
        pc->fcmap_line_ipg = 0xc;
        pc->fcmap_switch_ipg = 0xc;
#endif

        SOC_IF_ERROR_RETURN
            (_phy_84756_medium_config_update(unit, port,
                                        PHY_COPPER_MODE(unit, port) ?
                                        &pc->copper :
                                        &pc->fiber));

#if defined(INCLUDE_MACSEC)
        if (MACSEC_ENABLE(pc)) {
            rv = soc_macsecphy_init(unit, port, pc, 
                        BMACSEC_CORE_BCM84756, bmacsec_io_mmi1_cl45);
            if (!SOC_SUCCESS(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "soc_macsecphy_init: MACSEC init for"
                                  " u=%d p=%d FAILED "), unit, port));
            } else {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "soc_macsecphy_init: MACSEC init for"
                                 " u=%d p=%d SUCCESS "), unit, port));
            }
        }
#endif

#if defined(INCLUDE_FCMAP)
        if (FCMAP_ENABLE(pc)) {
            rv = soc_fcmapphy_init(unit, port, pc, 
                        BFCMAP_CORE_BCM84756, bfcmap_io_mmi1_cl45);
            if (!SOC_SUCCESS(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "soc_fcmapphy_init: FCMAP init for"
                                  " u=%d p=%d FAILED "), unit, port));
            } else {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "soc_fcmapphy_init: FCMAP init for"
                                 " u=%d p=%d SUCCESS "), unit, port));
            }
        }
#endif
        PHYCTRL_INIT_STATE_SET(pc, PHYCTRL_INIT_STATE_DEFAULT);

    }

    return SOC_E_NONE;
}


/*
 * Function:
 *    phy_84756_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_84756_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t    *pc;
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84756_intf_t bmacsec_mode;
    int bmacsec_link;

    pc    = EXT_PHY_SW_STATE(unit, port);
    *link = FALSE;      /* Default return */

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_line_intf_get(phy_dev_addr, 0, &bmacsec_mode);
    if (!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }

    if (bmacsec_mode == BMACSEC_PHY84756_INTF_SGMII) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
    }

    rv = bmacsec_phy84756_link_get(phy_dev_addr, &bmacsec_link);
    if (!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }
    if (bmacsec_link) {
        *link = TRUE;
    } else {
        *link = FALSE;
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_84756_link_get: u=%d p=%d link=%d\n"),
                 unit, port,
                 *link));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84756_enable_set
 * Purpose:
 *      Enable or disable the physical interface.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84756_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;

    pc     = EXT_PHY_SW_STATE(unit, port);

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_enable_set(phy_dev_addr, enable);

    if(rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84756_enable_set: "
                         "Power %s fiber medium\n"), (enable) ? "up" : "down"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84756_duplex_set
 * Purpose:
 *      Set the current duplex mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes:
 */
STATIC int
phy_84756_duplex_set(int unit, soc_port_t port, int duplex)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t           phy_dev_addr;
    bmacsec_phy84756_duplex_t    bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    if (duplex) {
        bmacsec_duplex = BMACSEC_PHY84756_FULL_DUPLEX;
    } else {
        bmacsec_duplex = BMACSEC_PHY84756_HALF_DUPLEX;
    }
    rv = bmacsec_phy84756_duplex_set(phy_dev_addr, bmacsec_duplex);

    if(BMACSEC_E_SUCCESS(rv)) {
        pc->copper.force_duplex = duplex;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84756_duplex_set: u=%d p=%d d=%d rv=%d\n"),
              unit, port, duplex, rv));
    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_84756_duplex_get
 * Purpose:
 *      Get the current operating duplex mode. If autoneg is enabled,
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The duplex is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_84756_duplex_get(int unit, soc_port_t port, int *duplex)
{

    int rv = SOC_E_NONE;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84756_duplex_t bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_duplex_get(phy_dev_addr, &bmacsec_duplex);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }
    if (bmacsec_duplex == BMACSEC_PHY84756_FULL_DUPLEX) {
        *duplex = TRUE;
    } else {
        *duplex = FALSE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84756_duplex_get: u=%d p=%d d=%d rv=%d\n"),
              unit, port, *duplex, rv));
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_84756_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_84756_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;
    phy_ctrl_t  *int_pc;
    int            rv; 
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if (speed > 10000) {
        return SOC_E_UNAVAIL;
    }

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    if(PHY_FIBER_MODE(unit, port)) {
        if (speed < 1000) {
            return SOC_E_UNAVAIL;
        }
    }

    rv = bmacsec_phy84756_speed_set(phy_dev_addr, speed);
    if(BMACSEC_E_SUCCESS(rv)) {
        if (NULL != int_pc) {
            PHY_AUTO_NEGOTIATE_SET (int_pc->pd, unit, port, 0);
            PHY_SPEED_SET(int_pc->pd, unit, port, speed);
        }
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.force_speed = speed;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.force_speed = speed;
            }
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84756_speed_set: u=%d p=%d s=%d fiber=%d rv=%d\n"),
              unit, port, speed, PHY_FIBER_MODE(unit, port), rv));

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84756_speed_get
 * Purpose:
 *      Get the current operating speed for a 84756 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - (OUT) Speed of the phy
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The speed is retrieved for the ACTIVE medium.
 */

STATIC int
phy_84756_speed_get(int unit, soc_port_t port, int *speed)
{
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_speed_get(phy_dev_addr, speed);

    if(!BMACSEC_E_SUCCESS(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84756_speed_get: u=%d p=%d invalid speed\n"),
                  unit, port));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84756_speed_get: u=%d p=%d speed=%d"),
                  unit, port, *speed));
    }

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84756_an_set
 * Purpose:
 *      Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is set only for the ACTIVE medium.
 */

STATIC int
phy_84756_an_set(int unit, soc_port_t port, int autoneg)
{
    int                 rv;
    phy_ctrl_t   *pc;
    int autoneg_advert = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_REPEATER) ) {
        soc_port_if_t  pif;
        SOC_IF_ERROR_RETURN( phy_84756_interface_get(unit, port, &pif) );
        if ( SOC_PORT_IF_SGMII == pif ) {
            /* in Repeater and SGMII mode,               *\
            \* AN need to be done by the Internal SerDes */
            phy_ctrl_t* int_pc = INT_PHY_SW_STATE(unit, port);
            rv = PHY_AUTO_NEGOTIATE_SET(int_pc->pd, unit, port, autoneg);
            return  rv;
        }
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        autoneg_advert = 0;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            autoneg_advert = pc->fiber.autoneg_advert;
        }
    }
#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_an_set(phy_dev_addr, autoneg, autoneg_advert);
    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.autoneg_enable = autoneg ? TRUE : FALSE;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.autoneg_enable = autoneg ? TRUE : FALSE;
            }
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84756_an_set: u=%d p=%d autoneg=%d rv=%d\n"),
              unit, port, autoneg, rv));
    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84756_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - (OUT) if true, auto-negotiation is enabled.
 *      autoneg_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if autoneg == FALSE.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_84756_an_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{
    int           rv;
    phy_ctrl_t   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    rv            = SOC_E_NONE;
    *autoneg      = FALSE;
    *autoneg_done = FALSE;

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_an_get(phy_dev_addr, autoneg, autoneg_done);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84756_lb_set
 * Purpose:
 *      Set the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_84756_lb_set(int unit, soc_port_t port, int enable)
{
    int           rv;
#if 0
    int           link = 0;
    soc_timeout_t  to;
#endif
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_loopback_set(phy_dev_addr, enable);

    if (rv == BMACSEC_E_NONE) {
        /* wait for link up when loopback is enabled */
        
        if (enable) {
            sal_usleep(2000000);
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84756_lb_set: u=%d p=%d en=%d rv=%d\n"), 
              unit, port, enable, rv));

    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_84756_lb_get
 * Purpose:
 *      Get the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_84756_lb_get(int unit, soc_port_t port, int *enable)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_loopback_get(phy_dev_addr, enable);

    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_84756_medium_config_set
 * Purpose:
 *      Set the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84756_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;
#if 0
    soc_phy_config_t *active_medium;  /* Currently active medium */
    int               medium_update = FALSE;
#endif
    soc_phy_config_t *change_medium;  /* Requested medium */
    soc_phy_config_t *other_medium;   /* The other medium */
    soc_port_mode_t   advert_mask;

    if (NULL == cfg) {
        return SOC_E_PARAM;
    }

    pc            = EXT_PHY_SW_STATE(unit, port);

   switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (!pc->automedium && !PHY_COPPER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->copper;
        other_medium   = &pc->fiber;
        advert_mask    = ADVERT_ALL_COPPER;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->fiber;
        other_medium   = &pc->copper;
        advert_mask    = ADVERT_ALL_FIBER;
        break;
    default:
        return SOC_E_PARAM;
    }

    /*
     * Changes take effect immediately if the target medium is active or
     * the preferred medium changes.
     */
#if 0
    if (change_medium->enable != cfg->enable) {
        medium_update = TRUE;
    }
#endif
    if (change_medium->preferred != cfg->preferred) {
        /* Make sure that only one medium is preferred */
        other_medium->preferred = !cfg->preferred;
#if 0
        medium_update = TRUE;
#endif
    }

    sal_memcpy(change_medium, cfg, sizeof(*change_medium));
    change_medium->autoneg_advert &= advert_mask;

#if 0
    if (medium_update) {
        /* The new configuration may cause medium change. Check
         * and update medium.
         */
        SOC_IF_ERROR_RETURN
            (_phy_84756_medium_change(unit, port, TRUE, medium));
    } else {
        active_medium = (PHY_COPPER_MODE(unit, port)) ?  
                            &pc->copper : &pc->fiber;
        if (active_medium == change_medium) {
            /* If the medium to update is active, update the configuration */
            SOC_IF_ERROR_RETURN
                (_phy_84756_medium_config_update(unit, port, change_medium));
        }
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84756_medium_config_get
 * Purpose:
 *      Get the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - (OUT) Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84756_medium_config_get(int unit, soc_port_t port, 
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    int            rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (pc->automedium || PHY_COPPER_MODE(unit, port)) {
            sal_memcpy(cfg, &pc->copper, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (pc->automedium || PHY_FIBER_MODE(unit, port)) {
            sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_84756_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
STATIC int
phy_84756_ability_advert_set(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    int        rv;
    phy_ctrl_t *pc;
    bmacsec_phy84756_port_ability_t local_ability;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        pc->copper.advert_ability = *ability;
    } else {
        pc->fiber.advert_ability = *ability;
    }

    local_ability = 0;

    if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
        local_ability |= BMACSEC_PHY84756_PA_1000MB_FD;
    }
    if(ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
        local_ability |= BMACSEC_PHY84756_PA_10000MB_FD;
    }

    if(ability->pause & SOC_PA_PAUSE_TX) {
        local_ability |= BMACSEC_PHY84756_PA_PAUSE_TX;
    }
    if(ability->pause & SOC_PA_PAUSE_RX) {
        local_ability |= BMACSEC_PHY84756_PA_PAUSE_RX;
    }
    if(ability->pause & SOC_PA_PAUSE_ASYMM) {
        local_ability |= BMACSEC_PHY84756_PA_PAUSE_ASYMM;
    }

    if(ability->loopback & SOC_PA_LB_NONE) {
        local_ability |= BMACSEC_PHY84756_PA_LB_NONE;
    }
    if(ability->loopback & SOC_PA_LB_PHY) {
        local_ability |= BMACSEC_PHY84756_PA_LB_PHY;
    }
    if(ability->flags & SOC_PA_AUTONEG) {
        local_ability |= BMACSEC_PHY84756_PA_AN;
    }

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_ability_advert_set(phy_dev_addr, local_ability);


    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84756_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
STATIC int
phy_84756_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    int        rv;
    bmacsec_phy84756_port_ability_t local_ability;
    bmacsec_dev_addr_t phy_dev_addr;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    local_ability = 0;

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_ability_advert_get(phy_dev_addr, &local_ability);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(local_ability & BMACSEC_PHY84756_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(local_ability & BMACSEC_PHY84756_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(local_ability & BMACSEC_PHY84756_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(local_ability & BMACSEC_PHY84756_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(local_ability & BMACSEC_PHY84756_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }

    if(local_ability & BMACSEC_PHY84756_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(local_ability & BMACSEC_PHY84756_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(local_ability & BMACSEC_PHY84756_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84756_ability_remote_get
 * Purpose:
 *      Get the current remote advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
STATIC int
phy_84756_ability_remote_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    int        rv;
    bmacsec_phy84756_port_ability_t remote_ability;
    bmacsec_dev_addr_t phy_dev_addr;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    remote_ability = 0;

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_remote_ability_advert_get(phy_dev_addr,
                                                   &remote_ability);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(remote_ability & BMACSEC_PHY84756_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(remote_ability & BMACSEC_PHY84756_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(remote_ability & BMACSEC_PHY84756_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(remote_ability & BMACSEC_PHY84756_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(remote_ability & BMACSEC_PHY84756_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }

    if(remote_ability & BMACSEC_PHY84756_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(remote_ability & BMACSEC_PHY84756_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(remote_ability & BMACSEC_PHY84756_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}



/*
 * Function:
 *      phy_84756_ability_local_get
 * Purpose:
 *      Get local abilities 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The ability is retrieved only for the ACTIVE medium.
 */

STATIC int
phy_84756_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int         rv = SOC_E_NONE;
    phy_ctrl_t *pc;
    bmacsec_phy84756_port_ability_t local_ability = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_ability_local_get(phy_dev_addr, &local_ability);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(local_ability & BMACSEC_PHY84756_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(local_ability & BMACSEC_PHY84756_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(local_ability & BMACSEC_PHY84756_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(local_ability & BMACSEC_PHY84756_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(local_ability & BMACSEC_PHY84756_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }


    if(local_ability & BMACSEC_PHY84756_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(local_ability & BMACSEC_PHY84756_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(local_ability & BMACSEC_PHY84756_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84756_firmware_set
 * Purpose:
 *      program the given firmware into the SPI-ROM
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      offset - offset to the data stream
 *      array  - the given data
 *      datalen- the data length
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_84756_firmware_set(int unit, int port, int offset, uint8 *data,int len)
{
    int            rv;
    phy_ctrl_t *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif

    rv = bmacsec_phy84756_spi_firmware_update(phy_dev_addr, offset, data, len);
    if (rv != BMACSEC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84756 firmware upgrade possibly failed:"
                                 "u=%d p=%d\n"), unit, port));
        return (SOC_E_FAIL);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84756 firmware upgrade successful:"
                         "u=%d p=%d\n"), unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84756_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_XAUI
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_84756_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *medium = SOC_PORT_MEDIUM_FIBER;

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_84756_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:    
 *      SOC_E_NONE
 */
STATIC int
phy_84756_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv = BMACSEC_E_NONE; 

#ifdef INCLUDE_MACSEC
#ifdef PHY84756_SWITCH_FIXED_MODE
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84756_intf_t line_mode;
    int pause_fwd =0, line_speed=0, line_duplex=0, line_pause=1;
    phy_ctrl_t    *pc;

    rv = BMACSEC_E_NONE;    
    pc = EXT_PHY_SW_STATE(unit, port);

/* Disable switch fixed speed modes and supporting functions
   Code not deleted to save the work already done if feature is enabled later
*/
    if (pc->macsec_enable != 1) {
        return SOC_E_UNAVAIL;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84756_line_intf_get(phy_dev_addr,
                                       pc->macsec_dev_port, &line_mode);
    if (rv != BMACSEC_E_NONE) {
        return SOC_ERROR(rv);
    }
    switch(type) {
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED:
            pc->macsec_switch_fixed = value;
            if(value) {
                rv = bmacsec_phy84756_switch_side_set_params(phy_dev_addr, 
                                BMACSEC_BCM84756_SWITCH_FIXED, 
                                pc->macsec_switch_fixed_speed,
                                pc->macsec_switch_fixed_duplex,
                                pc->macsec_switch_fixed_pause);
            } else { /* Switch side follows line side */
                SOC_IF_ERROR_RETURN
                    (phy_84756_speed_get(unit, port, &line_speed));
                SOC_IF_ERROR_RETURN
                    (phy_84756_duplex_get(unit, port, &line_duplex));
#if 0 
                SOC_IF_ERROR_RETURN
                    (phy_84756_pause_get(unit, port, &line_pause));
#endif
                rv = bmacsec_phy84756_switch_side_set_params(phy_dev_addr, 
                                BMACSEC_BCM84756_SWITCH_FOLLOWS_LINE, 
                                line_speed, line_duplex, line_pause);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED:
            /* Switch fixed speed is allowed only to be 1000Mbps */
            if (pc->macsec_dev_port > 2) {
                if (value != 1000) {
                    return SOC_E_CONFIG;
                }
            } else {
                if (line_mode == BMACSEC_PHY84756_INTF_1000X) {
                    if (value != 1000) {
                        return SOC_E_CONFIG;
                    }
                } else {
                    if (value != 10000) {
                        return SOC_E_CONFIG;
                    }
                }
            } 

            if(pc->macsec_switch_fixed_speed != value) {
                pc->macsec_switch_fixed_speed = value;
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX:
            /* Switch fixed Duplex is allowed only to be Full Duplex mode */
            if (value != 1) {
                return SOC_E_CONFIG;
            }
            if(pc->macsec_switch_fixed_duplex != value) {
                pc->macsec_switch_fixed_duplex = value;
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE:
            if(pc->macsec_switch_fixed_pause != value) {
                pc->macsec_switch_fixed_pause = value;
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD:
            pc->macsec_pause_rx_fwd = value;
            pause_fwd = (value == 1) ? 1 : 0;

            rv = bmacsec_phy84756_line_side_pause_forward(phy_dev_addr,
                                                         pause_fwd);
            if (rv == BMACSEC_E_NONE) {
                rv = bmacsec_phy84756_switch_side_pause_forward(
                                                     phy_dev_addr, pause_fwd);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD:
            pc->macsec_pause_tx_fwd = value;
            pause_fwd = (value == 1) ? 1 : 0;

            rv = bmacsec_phy84756_line_side_pause_forward(phy_dev_addr,
                                                          pause_fwd);
            if (rv == BMACSEC_E_NONE) {
                rv = bmacsec_phy84756_switch_side_pause_forward(phy_dev_addr,
                                                                pause_fwd);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_LINE_IPG:
            pc->macsec_line_ipg = value;
            rv = bmacsec_phy84756_line_side_ipg_set(phy_dev_addr, value);
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_IPG:
            pc->macsec_switch_ipg = value;
            rv = bmacsec_phy84756_switch_side_ipg_set(phy_dev_addr, value);
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }
#endif /* PHY84756_SWITCH_FIXED_MODE */
#else
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84756_intf_t line_mode;
    int pause_fwd =0, line_speed=0, line_duplex=0, line_pause=1;

    pause_fwd = 0;
    line_speed = 0;
    line_duplex = 0;
    line_pause = 1;
    phy_dev_addr = 0;
    line_mode = 0;

    rv = SOC_E_UNAVAIL;
#endif

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84756_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_84756_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv = SOC_E_NONE;
#ifdef INCLUDE_MACSEC
#ifdef PHY84756_SWITCH_FIXED_MODE
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

/* Disable switch fixed speed modes and supporting functions
   Code not deleted to save the work already done if feature is enabled later
*/

    if (pc->macsec_enable != 1) {
        return SOC_E_UNAVAIL;
    }

    if (NULL == value) {
        return SOC_E_PARAM;
    }

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(type) {
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED:
            *value = pc->macsec_switch_fixed;
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED:
            *value = pc->macsec_switch_fixed_speed;
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX:
            *value = pc->macsec_switch_fixed_duplex;
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE:
            *value = pc->macsec_switch_fixed_pause;
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD:
            *value = pc->macsec_pause_rx_fwd;
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD:
            *value = pc->macsec_pause_tx_fwd;
            break;
        case SOC_PHY_CONTROL_MACSEC_LINE_IPG:
            *value = pc->macsec_line_ipg;
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_IPG:
            *value = pc->macsec_switch_ipg;
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break; 
    }
#endif /* PHY84756_SWITCH_FIXED_MODE */
#else
    rv = SOC_E_UNAVAIL;
#endif
    return rv;
}

/*
 * Function:
 *      phy_84756_linkup
 * Purpose:
 *      Link up handler
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_84756_linkup(int unit, soc_port_t port)
{
    int rv = SOC_E_NONE;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    pc = EXT_PHY_SW_STATE(unit, port);

#ifdef INCLUDE_MACSEC
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);

    rv = bmacsec_84756_phy_linkup(phy_dev_addr, pc->macsec_enable);
#else
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);

    rv = bmacsec_84756_phy_linkup(phy_dev_addr, pc->fcmap_enable);
#endif

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84756_interface_get
 * Purpose:
 *      Get interface on a given port.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      pif   - Interface.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_84756_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    *pif = PHY_COPPER_MODE(unit, port) ? SOC_PORT_IF_SGMII : SOC_PORT_IF_XFI;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84756_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */
STATIC int
phy_84756_probe(int unit, phy_ctrl_t *pc)
{
    uint16  data1, data2;

    if (READ_PHY84756_PMA_PMD_REG(unit, pc, 0xc802, &data1) < 0) {
        return SOC_E_NOT_FOUND;
    }

    if (READ_PHY84756_PMA_PMD_REG(unit, pc, 0xc803, &data2) < 0) {
        return SOC_E_NOT_FOUND;
    }

    /* Use of Standalone MACSEC driver is selected by fcmap_enable=0xff */
    if (0xff == soc_property_port_get(unit, pc->port, spn_FCMAP_ENABLE, 0)) {
        pc->size = sizeof(phy84756_data_t);
        LOG_CLI((BSL_META_U(unit,
                            "u=%d p=%d Using Standalone PHY driver for BCM84756\n"),
                 unit, pc->port));
        return SOC_E_NONE;
    }
    return SOC_E_NOT_FOUND;

}

/*
 * Function:
 *      phy_84756_timesync_config_set
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_84756_timesync_config_set(int unit, soc_port_t port, soc_port_phy_timesync_config_t *timesync_config)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
 
    rv = bmacsec_phy84756_timesync_config_set(phy_dev_addr, (bmacsec_port_phy_timesync_config_t *)timesync_config);
    return (SOC_ERROR(rv));

}

/*
 * Function:
 *     phy_84756_timesync_config_get 
 * Description:
 *     
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_84756_timesync_config_get(int unit, soc_port_t port, soc_port_phy_timesync_config_t *timesync_config)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
 
    rv = bmacsec_phy84756_timesync_config_get(phy_dev_addr, (bmacsec_port_phy_timesync_config_t *)timesync_config);
    return (SOC_ERROR(rv));

}

/*
 * Function:
 *      phy_84756_timesync_control_set
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_84756_timesync_control_set(int unit, soc_port_t port, soc_port_control_phy_timesync_t control_type, uint64 value)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    buint64_t btemp64;

    COMPILER_64_SET(btemp64, COMPILER_64_HI(value), COMPILER_64_LO(value)); 

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
 
    rv = bmacsec_phy84756_timesync_control_set(phy_dev_addr, control_type, btemp64);
    return (SOC_ERROR(rv));

}

/*
 * Function:
 *      phy_84756_timesync_control_get
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */
int
phy_84756_timesync_control_get(int unit, soc_port_t port, soc_port_control_phy_timesync_t control_type, uint64 *value)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    buint64_t btemp64;

    COMPILER_64_ZERO(btemp64);
    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
#if defined(INCLUDE_MACSEC)
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
#if defined(INCLUDE_FCMAP)
    phy_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
#endif
 
    rv = bmacsec_phy84756_timesync_control_get(phy_dev_addr, control_type, &btemp64);
  
    COMPILER_64_SET(*value, COMPILER_64_HI(btemp64), COMPILER_64_LO(btemp64));

    return (SOC_ERROR(rv));

}


STATIC int 
phy_84756_reg_read(int unit, soc_port_t port, uint32 flags,
                        uint32 phy_reg_addr, uint32 *phy_data)
{
	uint16               data16;
    uint16               regdata;
    phy_ctrl_t          *pc;      /* PHY software state */
    int rv = SOC_E_NONE;
	int	rd_cnt;

    pc = EXT_PHY_SW_STATE(unit, port);

    rd_cnt = 1;

    if (flags & SOC_PHY_I2C_DATA8) {

        SOC_IF_ERROR_RETURN
            (phy_84756_i2cdev_read(pc,
                        SOC_PHY_I2C_DEVAD(phy_reg_addr),
                        SOC_PHY_I2C_REGAD(phy_reg_addr),
                        rd_cnt,
                        (uint8 *)&data16));
        *phy_data = *((uint8 *)&data16);

    } else if (flags & SOC_PHY_I2C_DATA16) {
        /* This operation is generally targeted to access 16-bit device,
         * such as PHY IC inside the SFP.  However there is no 16-bit
         * scratch register space on the device.  Use 1.800e
         * for this operation.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY84756_PMA_PMD_REG(unit, pc, PHY84756_I2C_TEMP_RAM,
                   &regdata));

        rv = _phy_84756_bsc_rw(pc,
               SOC_PHY_I2C_DEVAD(phy_reg_addr),
               PHY84756_I2C_OP_TYPE(PHY84756_I2CDEV_READ,PHY84756_I2C_16BIT),
               SOC_PHY_I2C_REGAD(phy_reg_addr),1,
               (void *)&data16,PHY84756_I2C_TEMP_RAM);

        *phy_data = data16;

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84756_PMA_PMD_REG(unit, pc, PHY84756_I2C_TEMP_RAM,
                 regdata));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY_REG(unit, pc, phy_reg_addr, &data16));
        *phy_data = data16;
    } 

    return rv;

}



STATIC int
phy_84756_reg_write(int unit, soc_port_t port, uint32 flags,
                        uint32 phy_reg_addr, uint32 phy_data)
{
    uint8  data8[4];
    uint16 data16[2];
    uint16 regdata[2];
    phy_ctrl_t          *pc;      /* PHY software state */
    int rv = SOC_E_NONE;
    int wr_cnt;

    pc = EXT_PHY_SW_STATE(unit, port);

    wr_cnt = 1;

    if (flags & SOC_PHY_I2C_DATA8) {
        data8[0] = (uint8)phy_data;
        SOC_IF_ERROR_RETURN
            (phy_84756_i2cdev_write(pc,
                        SOC_PHY_I2C_DEVAD(phy_reg_addr),
                        SOC_PHY_I2C_REGAD(phy_reg_addr),
                        wr_cnt,
                        data8));
    } else if (flags & SOC_PHY_I2C_DATA16) {

     /* This operation is generally targeted to access 16-bit device,
         * such as PHY IC inside the SFP.  However there is no 16-bit
         * scratch register space on the device.  Use 1.800e
         * for this operation.
         */
        /* save the temp ram register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84756_PMA_PMD_REG(unit, pc, PHY84756_I2C_TEMP_RAM,
                 &regdata[0]));
        data16[0] = phy_data;
        rv = _phy_84756_bsc_rw(pc,
              SOC_PHY_I2C_DEVAD(phy_reg_addr),
              PHY84756_I2C_OP_TYPE(PHY84756_I2CDEV_READ,PHY84756_I2C_16BIT),
              SOC_PHY_I2C_REGAD(phy_reg_addr),wr_cnt,
              (void *)data16,PHY84756_I2C_TEMP_RAM);

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84756_PMA_PMD_REG(unit, pc, PHY84756_I2C_TEMP_RAM,
                  regdata[0]));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, phy_reg_addr, (uint16)phy_data));
    }
    return rv;
}



/*
 * Variable:
 *    phy_84756_drv
 * Purpose:
 *    Phy Driver for BCM84756 MACSEC PHY
 */

phy_driver_t phy_84756drv_xe = {
    "84756 1G/10-Gigabit PHY Driver",
    phy_84756_init,        /* Init */
    phy_null_reset,       /* Reset */
    phy_84756_link_get,    /* Link get   */
    phy_84756_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_84756_duplex_set,  /* Duplex set */
    phy_84756_duplex_get,  /* Duplex get */
    phy_84756_speed_set,   /* Speed set  */
    phy_84756_speed_get,   /* Speed get  */
    phy_null_set,          /* Master set */
    phy_null_zero_get,     /* Master get */
    phy_84756_an_set,      /* ANA set */
    phy_84756_an_get,      /* ANA get */
    NULL,                 /* Local Advert set, deprecated */
    NULL,                 /* Local Advert get, deprecated */
    NULL,                 /* Remote Advert get, deprecated */
    phy_84756_lb_set,      /* PHY loopback set */
    phy_84756_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_84756_interface_get, /* IO Interface get */
    NULL,                   /* pd_ability, deprecated */
    phy_84756_linkup,
    NULL,
    phy_null_mdix_set,        /* phy_84756_mdix_set */
    phy_null_mdix_get,        /* phy_84756_mdix_get */
    phy_null_mdix_status_get, /* phy_84756_mdix_status_get */
    phy_84756_medium_config_set, /* medium config setting set */
    phy_84756_medium_config_get, /* medium config setting get */
    phy_84756_medium_status,        /* active medium */
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_84756_control_set,    /* phy_control_set */
    phy_84756_control_get,    /* phy_control_get */
    phy_84756_reg_read,       /* phy_reg_read */
    phy_84756_reg_write,      /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_84756_probe,         /* pd_probe  */
    phy_84756_ability_advert_set,  /* pd_ability_advert_set */
    phy_84756_ability_advert_get,  /* pd_ability_advert_get */
    phy_84756_ability_remote_get,  /* pd_ability_remote_get */
    phy_84756_ability_local_get,   /* pd_ability_local_get  */
    phy_84756_firmware_set,
    phy_84756_timesync_config_set,
    phy_84756_timesync_config_get,
    phy_84756_timesync_control_set,
    phy_84756_timesync_control_get
};

#endif /* INCLUDE_MACSEC */
#else /* INCLUDE_PHY_84756 */
typedef int _phy_84756_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_84756 */
