/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * File:        phy84334.c
 * Purpose:     PHY driver for BCM84334 (MACSec)
 *
 * References:
 *     
 *    +----------+   +---------------------------+
 *    |  XAUI /  |   |            +->Copper      |<--> Magnetic
 *    |  SGMII   |<->| XAUI/SGMII-+              |
 *    | (SerDes) |   |            |              |
 *    |          |   |            +->SGMII/      |<--> (PHY)SFP (10/100/1000)
 *    +----------+   |               1000BASE-X/ |<--> 1000BASE-X SFP
 *        Switch     |               XAUI        |
 *                   |                           |
 *                   |                           |
 *                   +---------------------------+
 *                               84334
 *
 * Notes:
 */ 
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_84334)
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

#include "phynull.h"
#include "phyxehg.h"

#ifdef INCLUDE_MACSEC
#include "phyident.h"
#include "phyreg.h"
#include "phyfege.h"
#include "phy84334.h"
#include <soc/macsecphy.h>

#define AUTO_MDIX_WHEN_AN_DIS   0       /* Non-standard-compliant option */
#define DISABLE_CLK125          0       /* Disable if unused to reduce */
                                        /*  EMI emissions */

/* Move flag to mac_ctrl.c */
#define MAC_CTRL_FLAG_MACSEC_XMAC_V2  (1<<1)

/* typedefs */
typedef struct {
    int           flags;
    soc_timeout_t to;
    uint8         *firmware;
    int           firmware_len;
    unsigned int  iter;
    int           phy_ext_boot;
    int           fiber_preferred;
} PHY8433_PRIV_t;

#define FLAGS(_pc) (((PHY8433_PRIV_t *)((_pc) + 1))->flags)
#define ITER(_pc) (((PHY8433_PRIV_t *)((_pc) + 1))->iter)
#define PHY_EXT_BOOT(_pc) (((PHY8433_PRIV_t *)((_pc) + 1))->phy_ext_boot)
#define FIBER_PREF(_pc) (((PHY8433_PRIV_t *)((_pc) + 1))->fiber_preferred)
#define TO(_pc) (((PHY8433_PRIV_t *)((_pc) + 1))->to)
#define FIRMWARE(_pc) (((PHY8433_PRIV_t *)((_pc) + 1))->firmware)
#define FIRMWARE_LEN(_pc) (((PHY8433_PRIV_t *)((_pc) + 1))->firmware_len)

extern buint8_t bcm84334_firmware[];
extern buint32_t bcm84334_firmware_length;

#define BMACSEC_PHY84334_MAP_UCODE_OFFSET(x)\
((x) == PHYCTRL_UCODE_BCST_SETUP     ? BMACSEC_PHY84334_UCODE_BCST_SETUP   :\
((x) == PHYCTRL_UCODE_BCST_uC_SETUP  ? BMACSEC_PHY84334_UCODE_BCST_uC_SETUP:\
((x) == PHYCTRL_UCODE_BCST_ENABLE    ? BMACSEC_PHY84334_UCODE_BCST_ENABLE  :\
((x) == PHYCTRL_UCODE_BCST_LOAD      ? BMACSEC_PHY84334_UCODE_BCST_LOAD   :\
                                       BMACSEC_PHY84334_UCODE_BCST_END  ))))





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

#define PHY84334_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))

#define READ_PHY84334_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84334_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(1, (_reg)), (_val))

#define MACSEC_READ_PHY_REG(_unit, _pc, _mdio, _reg, _value)     \
            ((_pc->read)((_unit), (_mdio), (_reg), (_value)))
#define MACSEC_WRITE_PHY_REG(_unit, _pc, _mdio, _reg, _value)     \
            ((_pc->write)((_unit), (_mdio), (_reg), (_value)))


STATIC int _phy_84334_medium_change(int unit, soc_port_t port, 
                          int force_update, soc_port_medium_t force_medium);
STATIC int phy_84334_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_84334_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_84334_master_set(int unit, soc_port_t port, int master);
STATIC int phy_84334_autoneg_set(int unit, soc_port_t port, int autoneg);
STATIC int phy_84334_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
STATIC int phy_84334_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_84334_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_84334_ability_local_get(int unit, soc_port_t port,soc_port_ability_t *ability);
STATIC int phy_84334_linkup(int unit, soc_port_t port);
int phy_84334_reset(int unit, soc_port_t port, void *user_arg);



/***********************************************************************
 *
 * HELPER FUNCTIONS
 */
/*
 * Function:
 *      _phy_84334_medium_check
 * Purpose:
 *      Determine if chip should operate in copper or fiber mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) SOC_PORT_MEDIUM_XXX
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_84334_medium_check(int unit, soc_port_t port, int *medium)
{
    phy_ctrl_t    *pc = EXT_PHY_SW_STATE(unit, port);
    int rv;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84334_port_medium_t bmacsec_port_medium;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 

    rv = bmacsec_phy84334_medium_check(phy_dev_addr, &bmacsec_port_medium); 
    if(!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }

    *medium = SOC_PORT_MEDIUM_COPPER;

    if (pc->automedium) {
        if (pc->fiber.preferred) {
            if (bmacsec_port_medium == BMACSEC_PHY_MEDIUM_COPPER) {
                /* Only Copper is lined up */
                *medium = SOC_PORT_MEDIUM_COPPER;
            } else {
                *medium = SOC_PORT_MEDIUM_FIBER;
            }
        } else {
            if (bmacsec_port_medium == BMACSEC_PHY_MEDIUM_FIBER) {
                /* Only Fiber is linked up */
                *medium = SOC_PORT_MEDIUM_FIBER;
            } else {
                *medium = SOC_PORT_MEDIUM_COPPER;
            }
        }
    } else {
       *medium = pc->fiber.preferred ? SOC_PORT_MEDIUM_FIBER:
                  SOC_PORT_MEDIUM_COPPER; 
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_84334_medium_check: "
                            "u=%d p=%d fiber_pref=%d fiber=%d\n"),
                 unit, port, pc->fiber.preferred,
                 (*medium == SOC_PORT_MEDIUM_FIBER) ? 1 : 0));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84334_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_84334_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    if (PHY_COPPER_MODE(unit, port)) {
        *medium = SOC_PORT_MEDIUM_COPPER;
    } else {
        *medium = SOC_PORT_MEDIUM_FIBER;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84334_medium_config_update
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
_phy_84334_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_84334_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_84334_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_84334_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (phy_84334_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_84334_autoneg_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (phy_84334_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}

/***********************************************************************
 *
 * PHY84334 DRIVER ROUTINES
 */

/*
 * Function:
 *      phy_84334_medium_config_set
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
phy_84334_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;
    soc_phy_config_t *active_medium;  /* Currently active medium */
    soc_phy_config_t *change_medium;  /* Requested medium */
    soc_phy_config_t *other_medium;   /* The other medium */
    int               medium_update;
    int               rv;
    bmacsec_dev_addr_t phy_dev_addr;

    if (NULL == cfg) {
        return SOC_E_PARAM;
    }

    pc            = EXT_PHY_SW_STATE(unit, port);
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    medium_update = FALSE;

   switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (!pc->automedium && !PHY_COPPER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->copper;
        other_medium   = &pc->fiber;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->fiber;
        other_medium   = &pc->copper;
        break;
    default:
        return SOC_E_PARAM;
    }

    /*
     * Changes take effect immediately if the target medium is active or
     * the preferred medium changes.
     */
    if (change_medium->enable != cfg->enable) {
        medium_update = TRUE;
    }
    if (change_medium->preferred != cfg->preferred) {
        /* Make sure that only one medium is preferred */
        other_medium->preferred = !cfg->preferred;
        medium_update = TRUE;
    }

    sal_memcpy(change_medium, cfg, sizeof(*change_medium));

    rv = bmacsec_phy84334_port_priority_set(phy_dev_addr, pc->fiber.preferred ? BMACSEC_PHY_MEDIUM_FIBER : BMACSEC_PHY_MEDIUM_COPPER);

    if(!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }

    if (medium_update) {
        /* The new configuration may cause medium change. Check
         * and update medium.
         */
        SOC_IF_ERROR_RETURN
            (_phy_84334_medium_change(unit, port, TRUE, medium));
    } else {
        active_medium = (PHY_COPPER_MODE(unit, port)) ?  
                            &pc->copper : &pc->fiber;
        if (active_medium == change_medium) {
            /* If the medium to update is active, update the configuration */
            SOC_IF_ERROR_RETURN
                (_phy_84334_medium_config_update(unit, port, change_medium));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84334_medium_config_get
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
phy_84334_medium_config_get(int unit, soc_port_t port, 
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

#if 0
/*
 * Function:
 *      _phy_84334_no_reset_setup
 * Purpose:
 *      Setup the operating parameters of the PHY without resetting PHY
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_84334_no_reset_setup(int unit, soc_port_t port, int fiber)
{
    phy_ctrl_t    *int_pc;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    int rv = SOC_E_NONE;
    int fiber_capable = 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_84334_reset_setup: u=%d p=%d medium=%s\n"),
                         unit, port,
              PHY_COPPER_MODE(unit, port) ? "COPPER" : "FIBER"));

    pc      = EXT_PHY_SW_STATE(unit, port);
    int_pc  = INT_PHY_SW_STATE(unit, port);

    if (NULL != int_pc) {
        SOC_IF_ERROR_RETURN
            (PHY_INIT(int_pc->pd, unit, port));

        if (PHY_FIBER_MODE(unit, port)) {
           /* Interface to MAC or internal SerDes is always SGMII mode.
            * Typical internal SerDes driver set the interface to 1000BASE-X,
            * if the media is fiber. Therefore, we need to force the interface 
            * of internal SerDes Driver to SGMII. */
           SOC_IF_ERROR_RETURN
               (PHY_NOTIFY(int_pc->pd, unit, port, phyEventInterface,
                           SOC_PORT_IF_SGMII));
        } 

        if (PHY_COPPER_MODE(unit, port) && 
            !PHY_SGMII_AUTONEG_MODE(unit, port)) {
            /* Make sure internal SerDes is in power down state 
             * until the MAC is ready. */
            SOC_IF_ERROR_RETURN
                (PHY_NOTIFY(int_pc->pd, unit, port, 
                            phyEventStop, PHY_STOP_COPPER));
        }
    }
 
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_init(phy_dev_addr, 0,
                               pc->automedium,
                               fiber,
                               pc->fiber_detect,
                               pc->macsec_enable,
                               &fiber_capable, 1, NULL);
   if (rv != BMACSEC_E_NONE) {
       return SOC_E_FAIL;
   }
   return SOC_E_NONE;
}
#endif

/*
 * Function:
 *      phy_84334_init
 * Purpose:
 *      Init function for 84334 PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_84334_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    int        fiber_capable, rv;
    int        fiber_preferred;
    bmacsec_dev_addr_t  phy_dev_addr;
    int         mmi_mdio_addr, port_index, macsec_enable, phy_ext_boot;
    uint32      phy_mdi_pair_map;
    int         status;
    uint32      mac_config_flag = 0;
    uint16      temp1, temp2, saved_phy_addr;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84334_init: u=%d p=%d\n"),
                         unit, port));

    pc = EXT_PHY_SW_STATE(unit, port);

  if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT ||
      PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1   ) {

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_C45);
    rv = BMACSEC_E_NONE;

    pc->interface = SOC_PORT_IF_XGMII;


    mmi_mdio_addr = soc_property_port_get(unit, port, spn_MACSEC_DEV_ADDR, -1);
    phy_ext_boot = soc_property_port_get(unit, port,
                                            spn_PHY_EXT_ROM_BOOT, 1);
    PHY_EXT_BOOT(pc) = phy_ext_boot;

    if (mmi_mdio_addr == -1) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84334_init: "
                             "MACSEC_DEV_ADDR property "
                             "not configured for u=%d p=%d\n"), unit, port));
        return SOC_E_CONFIG;
    }

    port_index = soc_property_port_get(unit, port, spn_MACSEC_PORT_INDEX, -1);
    if (port_index == -1) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84334_init: "
                             "MACSEC_PORT_INDEX property not configured for "
                             "u=%d p=%d\n"), unit, port));
        return SOC_E_CONFIG;
    }

    pc->macsec_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, mmi_mdio_addr, 1);
    pc->macsec_dev_port = port_index;

    phy_dev_addr    = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 

    macsec_enable = soc_property_port_get(unit, port, spn_MACSEC_ENABLE, 0);
    pc->macsec_enable = macsec_enable;

    switch (PHY_MODEL(pc->phy_id0, pc->phy_id1)) {
        case PHY_BCM84844_MODEL:
        case PHY_BCM84846_MODEL:
        case PHY_BCM84848_MODEL:
            mac_config_flag |= MAC_CTRL_FLAG_MACSEC_XMAC_V2;
            break;
        default:
            mac_config_flag = 0;
            break;
    }

    rv = bmacsec_phy84334_phy_mac_addr_set(phy_dev_addr, pc->macsec_dev_addr, 
                                   pc->macsec_dev_port,
                                   bmacsec_io_mmi1_cl45_10gbt,
                                   mac_config_flag );

    if(rv != BMACSEC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84334_init: bmacsec_phy_mac_addr_set "
                             "returned error for u=%d p=%d\n"), unit, port));
        return SOC_E_FAIL;
    }

    fiber_capable = 1;

    fiber_preferred =
        soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 0);

    FIBER_PREF(pc) = fiber_preferred;

    /*
    pc->automedium =
        soc_property_port_get(unit, port, spn_PHY_AUTOMEDIUM, 1);
     */
    pc->automedium = TRUE;

    pc->fiber_detect =
        soc_property_port_get(unit, port, spn_PHY_FIBER_DETECT, 0);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84334_init: "
                         "u=%d p=%d type=84334%s automedium=%d fiber_pref=%d detect=%d\n"),
              unit, port, fiber_capable ? "S" : "",
              pc->automedium, fiber_preferred, pc->fiber_detect));

    ITER(pc) = 0 ;
    soc_timeout_init(&TO(pc), 0, 0);
    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
        /* indicate second pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
    }

#ifdef PHY84334_FIRMWARE_INIT_CHECK
    /* prepare for download(init check) */
    rv = bmacsec_phy84334_init_check(phy_dev_addr, &status);
    if(rv != BMACSEC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84334_init: bmacsec_phy84334_init_check "
                             "returned error %d for u=%d p=%d\n"), rv, unit, port));
        return SOC_E_FAIL;
    }

    if (!phy_ext_boot && status != BMACSEC_PHY84334_INIT_UCODE_ACTIVE) {
#else
    if (!phy_ext_boot) {
#endif

        /* read ID registers for address zero of the MDIO bus */
        saved_phy_addr = pc->phy_id;
        pc->phy_id &= ~0x1f;

        SOC_IF_ERROR_RETURN
            (READ_PHY84334_PMA_PMD_REG(unit, pc, 0x2, &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY84334_PMA_PMD_REG(unit, pc, 0x3, &temp2));

        pc->phy_id = saved_phy_addr;

        if(temp1 != 0xffff) {
            if((temp1 != pc->phy_id0) || (temp2 != pc->phy_id1)) {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "Another PHY already exist on same broadcast MDIO address. Doing unicast\n")));

                if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT) {
                    goto  skip_bcst;
                } else {
                    return SOC_E_NONE;
                }
            }
        }



        /* do MDIO download steps */
        ITER(pc) = 1 ;
        FIRMWARE(pc) = bcm84334_firmware ;
        FIRMWARE_LEN(pc) = bcm84334_firmware_length ;
        pc->flags |= PHYCTRL_MDIO_BCST;
        switch (PHY_MODEL(pc->phy_id0, pc->phy_id1)) {
            case PHY_BCM84844_MODEL:
            case PHY_BCM84846_MODEL:
            case PHY_BCM84848_MODEL:
                pc->dev_name = "BCM8484X" ;
                break;
            default:
                pc->dev_name = "BCM8433X" ;
                break;
        }

        soc_timeout_init(&TO(pc), 60000000, 0);
        /* indicate second pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);

        /* reset port */
        rv = bmacsec_phy84334_reset_no_wait(phy_dev_addr);
        if(rv != BMACSEC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_84334_init: bmacsec_phy84334_reset_no_wait "
                                 "returned error %d for u=%d p=%d\n"), rv, unit, port));
            return SOC_E_FAIL;
        }

        return SOC_E_NONE;
    }
  } 

skip_bcst:

    phy_dev_addr    = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    phy_ext_boot    = PHY_EXT_BOOT(pc);
    fiber_preferred = FIBER_PREF(pc);

  if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
      (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

    do {
    /* Init PHYS and MACs to defaults */
    rv = bmacsec_phy84334_init(phy_dev_addr, 1,
                               pc->automedium,
                               fiber_preferred,
                               pc->fiber_detect,
                               pc->macsec_enable,
                               &fiber_capable, phy_ext_boot, 
                               ITER(pc) == 0 ? NULL : &ITER(pc) );

        if (rv != BMACSEC_E_NONE || ITER(pc) == 0) {
            break;
        }

    } while (!soc_timeout_check(&TO(pc)));

    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if (ITER(pc) != 0) {
        return SOC_E_TIMEOUT;
    }

    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) {
        /* indicate second pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS3);
        soc_timeout_init(&TO(pc), 700000, 0);
        return SOC_E_NONE;
    }
  }

  if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) ||
      (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

#if 1
    while (!soc_timeout_check(&TO(pc))) {
        rv = bmacsec_phy84334_handshake_status(phy_dev_addr, &status);
        if(rv != BMACSEC_E_NONE) {
            return SOC_E_FAIL;
        }
        if(status == BMACSEC_PHY84334_HS_STATUS_DONE) {
            break;
        }
    }
#endif

    /* software controlled medium selection is not available yet */
    pc->copper.enable         = TRUE;
    pc->copper.force_duplex    = TRUE;
    pc->copper.force_speed    = 10000;
    pc->copper.autoneg_enable = TRUE;
    pc->copper.master = SOC_PORT_MS_AUTO;
    pc->copper.mdix = SOC_PORT_MDIX_AUTO;
    pc->copper.preferred = !fiber_preferred;

    pc->fiber.enable          = TRUE;
    pc->fiber.force_speed     = 10000;
    pc->fiber.force_duplex    = TRUE;
    pc->fiber.autoneg_enable = FALSE;
    pc->fiber.preferred = fiber_preferred;


    /* Initially configure for the preferred medium. */
    PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);

    /*
     * Some reasonable defaults
     */
    pc->macsec_switch_fixed = 0;
    pc->macsec_switch_fixed_speed = 10000;
    pc->macsec_switch_fixed_duplex = 1;
    pc->macsec_switch_fixed_pause = 1;
    pc->macsec_pause_rx_fwd = 0;
    pc->macsec_pause_tx_fwd = 0;
    pc->macsec_line_ipg = 0xc;
    pc->macsec_switch_ipg = 0xc;

    /* Check config for MDI PAIR */
    phy_mdi_pair_map = soc_property_port_get(unit, port,
                                            spn_PHY_MDI_PAIR_MAP, 0);
    if (phy_mdi_pair_map) {
        rv = bmacsec_phy84334_phy_mdi_pair_set(phy_dev_addr, phy_mdi_pair_map);

        if(rv != BMACSEC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_84334_init: bmacsec_phy84334_phy_mdi_pair_set "
                                 "returned error %d for u=%d p=%d\n"), rv, unit, port));
            return SOC_E_FAIL;
        }
    }

    /* Set Local Advertising Configuration */
    SOC_IF_ERROR_RETURN
        (phy_84334_ability_local_get(unit, port, &pc->copper.advert_ability));

    SOC_IF_ERROR_RETURN
        (phy_84334_ability_advert_set(unit, port, &pc->copper.advert_ability));


#ifdef PHY84334_FIBER_1G_SUPPORT  /* Need to Revist this */
    if (!(PHY_FLAGS_TST(unit, port, PHY_FLAGS_FORCED_COPPER)) &&
          pc->fiber.preferred                                 &&
          pc->automedium  ){
        int            medium;

        /* Try 1G Fiber */
        pc->fiber.force_speed     = 1000;
        SOC_IF_ERROR_RETURN
            (_phy_84334_medium_change(unit, port, TRUE,
                                      SOC_PORT_MEDIUM_FIBER ));

        
        /* Wait to check if 1G Fiber is up */
        sal_usleep(400000);

        SOC_IF_ERROR_RETURN
            (_phy_84334_medium_check(unit, port, &medium));

        pc->fiber.force_speed     = 10000;
        if (medium != SOC_PORT_MEDIUM_FIBER) {
            /* Did not detect 1G Fiber - Try 10G Fiber */
            SOC_IF_ERROR_RETURN
                (_phy_84334_medium_change(unit, port, TRUE,
                                          SOC_PORT_MEDIUM_FIBER ));
            /* Wait to check if 10G Fiber is up */
            sal_usleep(400000);
        }
    }
#endif

    if (pc->macsec_enable) {
        switch (PHY_MODEL(pc->phy_id0, pc->phy_id1)) {
            case PHY_BCM84844_MODEL:
            case PHY_BCM84846_MODEL:
            case PHY_BCM84848_MODEL:
                rv = soc_macsecphy_init(unit, port, pc,
                    BMACSEC_CORE_BCM8484X, bmacsec_io_mmi1_cl45_10gbt);
                break;
            default:
                rv = soc_macsecphy_init(unit, port, pc,
                    BMACSEC_CORE_BCM8483X, bmacsec_io_mmi1_cl45_10gbt);
                break;
        }
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
  }
    return SOC_E_NONE;
}


/*
 * Function: 	
 *	phy_84334_reset
 * Purpose:	
 *	Reset PHY and wait for it to come out of reset.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 * Returns:	
 *	SOC_E_XXX
 */

int
phy_84334_reset(int unit, soc_port_t port, void *user_arg)
{

    int rv;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_reset(phy_dev_addr);

    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_84334_enable_set
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
phy_84334_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84334_port_mode_t port_mode = 0;

    pc     = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_enable_set(phy_dev_addr, pc->automedium,
                                     port_mode, enable);
    if(rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if (pc->automedium || PHY_FIBER_MODE(unit, port)) {
        phy_ctrl_t  *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);
        if (NULL != int_pc) {
            SOC_IF_ERROR_RETURN
                (PHY_ENABLE_SET(int_pc->pd, unit, port, enable));
        }
    
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84334_enable_set: "
                             "Power %s fiber medium\n"), (enable) ? "up" : "down"));
    }

    /* Update software state */
    SOC_IF_ERROR_RETURN(phy_fe_ge_enable_set(unit, port, enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84334_enable_get
 * Purpose:
 *      Enable or disable the physical interface for a 84334 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84334_enable_get(int unit, soc_port_t port, int *enable)
{
    return phy_fe_ge_enable_get(unit, port, enable);
}

/*
 * Function:
 *      _phy_84334_medium_change
 * Purpose:
 *      Change medium when media change detected or forced
 * Parameters:
 *      unit         - StrataSwitch unit #.
 *      port         - StrataSwitch port #.
 *      force_update - Force update
 *      force_medium - Force update medium
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_84334_medium_change(int unit, soc_port_t port, int force_update,
                         soc_port_medium_t force_medium)
{
    phy_ctrl_t    *pc;
    int            medium = SOC_PORT_MEDIUM_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;
    int            rv;

    pc    = EXT_PHY_SW_STATE(unit, port);
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 

    if (force_update == TRUE) {
        medium = force_medium;
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_84334_medium_check(unit, port, &medium));
    }

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        if (!PHY_COPPER_MODE(unit, port) || force_update) { /* Was fiber */ 
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            rv = bmacsec_phy84334_port_mode_set(phy_dev_addr, BMACSEC_PHY_COPPER);
            if (!BMACSEC_E_SUCCESS(rv)) {
                return SOC_E_FAIL;
            }

            /* Do Not power up the interface if medium is disabled */
            if (pc->copper.enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_84334_medium_config_update(unit, port, &pc->copper));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_84334_link_auto_detect: u=%d p=%d [F->C]\n"),
                      unit, port));
        }
    } else {        /* Fiber */
        if (PHY_COPPER_MODE(unit, port) || force_update) { /* Was copper */
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            rv = bmacsec_phy84334_port_mode_set(phy_dev_addr, BMACSEC_PHY_FIBER);
            if (!BMACSEC_E_SUCCESS(rv)) {
                return SOC_E_FAIL;
            }

            /* Do Not power up the interface if medium is disabled */
            if (pc->fiber.enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_84334_medium_config_update(unit, port, &pc->fiber));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_84334_link_auto_detect: u=%d p=%d [C->F]\n"),
                      unit, port));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84334_link_get
 * Purpose:
 *      Determine the current link up/down status for a 84334 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      link - (OUT) Boolean, true indicates link established.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If using automedium, also switches the mode.
 */
STATIC int
phy_84334_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t    *pc;
    uint16         data; 
    int rv = SOC_E_NONE;
    bmacsec_phy84334_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;
    int bmacsec_link;


    pc    = EXT_PHY_SW_STATE(unit, port);
    *link = FALSE;      /* Default return */

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (!pc->fiber.enable && !pc->copper.enable) {
        return SOC_E_NONE;
    }

    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
    if (pc->automedium) {
        /* Check for medium change and update HW/SW accordingly. */
        SOC_IF_ERROR_RETURN
            (_phy_84334_medium_change(unit, port, FALSE, 0));
    }

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_link_get(phy_dev_addr, port_mode, &bmacsec_link);
    if (!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }
    if (bmacsec_link == 1) {
        *link = TRUE;
    } else {
        *link = FALSE;
    }

    /* If preferred medium is up, disable the other medium 
     * to prevent false link. */
    if (pc->automedium) {
        if (pc->copper.preferred) {
#if 0 /* Revisit this after implementing enable set for FIBER/COPPER */
            if (pc->fiber.enable) {
                /* Power down Serdes if Copper mode is up */
                data = (*link && PHY_COPPER_MODE(unit, port)) ? 0 : 1;
            } else {
                data = 0;

            }
            port_mode = BMACSEC_PHY_FIBER;
            (void)bmacsec_phy84334_enable_set(phy_dev_addr, pc->automedium,
                                                 port_mode, data);
#endif
        } else {  /* pc->fiber.preferred */
            if (pc->copper.enable) {
                data = (*link && PHY_FIBER_MODE(unit, port)) ? 0 : 1;
            } else {
                data = 0;
            }
            port_mode = BMACSEC_PHY_COPPER;
            (void)bmacsec_phy84334_enable_set(phy_dev_addr, pc->automedium,
                                             port_mode, data);
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_84334_link_get: u=%d p=%d mode=%s%s link=%d\n"),
                 unit, port,
                 PHY_COPPER_MODE(unit, port) ? "C" : "F",
                 PHY_FIBER_100FX_MODE(unit, port)? "(100FX)" : "",
                 *link));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_84334_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes:
 *      The duplex is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 *      Autonegotiation is not manipulated.
 */

STATIC int
phy_84334_duplex_set(int unit, soc_port_t port, int duplex)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_phy84334_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84334_duplex_t bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
        if (duplex == 0) {
            return SOC_E_UNAVAIL;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    if (duplex) {
        bmacsec_duplex = BMACSEC_PHY84334_FULL_DUPLEX;
    } else {
        bmacsec_duplex = BMACSEC_PHY84334_HALF_DUPLEX;
    }
    rv = bmacsec_phy84334_duplex_set(phy_dev_addr, port_mode, 
                                     bmacsec_duplex);

    if(BMACSEC_E_SUCCESS(rv)) {
        pc->copper.force_duplex = duplex;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84334_duplex_set: u=%d p=%d d=%d rv=%d\n"),
              unit, port, duplex, rv));
    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_84334_duplex_get
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
phy_84334_duplex_get(int unit, soc_port_t port, int *duplex)
{

    int rv = SOC_E_NONE;
    bmacsec_phy84334_port_mode_t port_mode = 0;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84334_duplex_t bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
        *duplex = TRUE;
        return SOC_E_NONE;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_duplex_get(phy_dev_addr, port_mode, 
                                     &bmacsec_duplex);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }
    if (bmacsec_duplex == BMACSEC_PHY84334_FULL_DUPLEX) {
        *duplex = TRUE;
    } else {
        *duplex = FALSE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84334_speed_set
 * Purpose:
 *      Set the current operating speed (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Requested speed, only 1000 supported.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The speed is set only for the ACTIVE medium.
 */

STATIC int
phy_84334_speed_set(int unit, soc_port_t port, int speed)
{
    int            rv; 
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84334_port_mode_t port_mode = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
        if (speed == 10) {
            return SOC_E_UNAVAIL;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy84334_speed_set(phy_dev_addr, port_mode, speed);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.force_speed = speed;
        } else {
            pc->fiber.force_speed = speed;
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84334_speed_set: u=%d p=%d s=%d fiber=%d rv=%d\n"),
              unit, port, speed, PHY_FIBER_MODE(unit, port), rv));

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84334_speed_get
 * Purpose:
 *      Get the current operating speed for a 84334 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The speed is retrieved for the ACTIVE medium.
 */

STATIC int
phy_84334_speed_get(int unit, soc_port_t port, int *speed)
{
    int rv = SOC_E_NONE;
    bmacsec_phy84334_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
            port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy84334_speed_get(phy_dev_addr, port_mode, speed);

    if(!BMACSEC_E_SUCCESS(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84334_speed_get: u=%d p=%d invalid speed\n"),
                  unit, port));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84334_speed_get: u=%d p=%d speed=%d"),
                  unit, port, *speed));
    }

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84334_master_set
 * Purpose:
 *      Set the current master mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The master mode is set only for the ACTIVE medium.
 */
STATIC int
phy_84334_master_set(int unit, soc_port_t port, int master)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_phy84334_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_master_set(phy_dev_addr, port_mode, master);
    if(BMACSEC_E_SUCCESS(rv) && PHY_COPPER_MODE(unit, port)) {
        pc->copper.master = master;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84334_master_set: u=%d p=%d master=%d fiber=%d rv=%d\n"),
              unit, port, master, PHY_FIBER_MODE(unit, port), rv));

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84334_master_get
 * Purpose:
 *      Get the current master mode for a 84334 device.
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
phy_84334_master_get(int unit, soc_port_t port, int *master)
{
    int        rv;
    phy_ctrl_t *pc;
    bmacsec_phy84334_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_master_get(phy_dev_addr, port_mode, master);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84334_autoneg_set
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
phy_84334_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    int                 rv;
    phy_ctrl_t   *pc;
    bmacsec_phy84334_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;


    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_an_set(phy_dev_addr, port_mode,autoneg);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.autoneg_enable = autoneg ? TRUE : FALSE;
        } else {
            pc->fiber.autoneg_enable = autoneg ? TRUE : FALSE;
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84334_autoneg_set: u=%d p=%d autoneg=%d rv=%d\n"),
              unit, port, autoneg, rv));
    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84334_autoneg_get
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
phy_84334_autoneg_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{
    int           rv;
    phy_ctrl_t   *pc;
    bmacsec_phy84334_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    rv            = SOC_E_NONE;
    *autoneg      = FALSE;
    *autoneg_done = FALSE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_an_get(phy_dev_addr, port_mode, autoneg,
                                 autoneg_done);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_84334_ability_advert_get
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
phy_84334_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    bmacsec_phy84334_port_mode_t port_mode;
    bmacsec_phy84334_port_ability_t bmacsec_ability;
    bmacsec_dev_addr_t phy_dev_addr;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy84334_ability_advert_get(phy_dev_addr, port_mode,
                                        &bmacsec_ability);
    if (rv == BMACSEC_E_NONE) {
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_100MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_100MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_1000MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_1000MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10000MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_PAUSE_TX) {
            ability->pause |= SOC_PA_PAUSE_TX;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_PAUSE_RX) {
            ability->pause |= SOC_PA_PAUSE_RX;
        }

        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_100MB) {
            ability->eee |= SOC_PA_EEE_100MB_BASETX;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_1000MB) {
            ability->eee |= SOC_PA_EEE_1GB_BASET;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_10000MB) {
            ability->eee |= SOC_PA_EEE_10GB_BASET;
        }
    }

    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_84334_ability_advert_set
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
phy_84334_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    bmacsec_phy84334_port_mode_t port_mode;
    bmacsec_phy84334_port_ability_t bmacsec_ability;
    bmacsec_dev_addr_t phy_dev_addr;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);

    bmacsec_ability = 0;

    if (ability->speed_half_duplex & SOC_PA_SPEED_10MB)  {
        bmacsec_ability |= BMACSEC_PHY84334_PA_10MB_HD;
    }
    if (ability->speed_half_duplex & SOC_PA_SPEED_100MB) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_100MB_HD;
    }
    if (ability->speed_half_duplex & SOC_PA_SPEED_1000MB) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_1000MB_HD;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_10MB)  {
        bmacsec_ability |= BMACSEC_PHY84334_PA_10MB_FD;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_100MB_FD;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_1000MB_FD;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_10000MB_FD;
    }
    if (ability->pause & SOC_PA_PAUSE_TX) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_PAUSE_TX;
    }
    if (ability->pause & SOC_PA_PAUSE_RX) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_PAUSE_RX;
    }

    if (ability->eee & SOC_PA_EEE_100MB_BASETX) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_EEE_100MB;
    }
    if (ability->eee & SOC_PA_EEE_1GB_BASET) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_EEE_100MB;
    }
    if (ability->eee & SOC_PA_EEE_10GB_BASET) {
        bmacsec_ability |= BMACSEC_PHY84334_PA_EEE_10000MB;
    }

    rv = bmacsec_phy84334_ability_advert_set(phy_dev_addr, port_mode, bmacsec_ability);

    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_84334_ability_remote_get
 * Purpose:
 *      Get the current remoteisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remoteisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_84334_ability_remote_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    bmacsec_phy84334_port_mode_t port_mode;
    bmacsec_phy84334_port_ability_t bmacsec_ability = 0;
    bmacsec_dev_addr_t phy_dev_addr;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy84334_remote_ability_advert_get(phy_dev_addr, port_mode,
                                        &bmacsec_ability);
    if (rv == BMACSEC_E_NONE) {
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_100MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_100MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_1000MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_1000MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10000MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_PAUSE_TX) {
            ability->pause |= SOC_PA_PAUSE_TX;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_PAUSE_RX) {
            ability->pause |= SOC_PA_PAUSE_RX;
        }

        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_100MB) {
            ability->eee |= SOC_PA_EEE_100MB_BASETX;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_1000MB) {
            ability->eee |= SOC_PA_EEE_1GB_BASET;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_10000MB) {
            ability->eee |= SOC_PA_EEE_10GB_BASET;
        }
    }

    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_84334_ability_local_get
 * Purpose:
 *      Get the device's complete abilities.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - return device's abilities.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84334_ability_local_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    bmacsec_phy84334_port_mode_t port_mode;
    bmacsec_phy84334_port_ability_t bmacsec_ability;
    bmacsec_dev_addr_t phy_dev_addr;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy84334_ability_local_get(phy_dev_addr, port_mode,
                                        &bmacsec_ability);
    if (rv == BMACSEC_E_NONE) {
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_100MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_100MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_1000MB_HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_1000MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_10000MB_FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_PAUSE_TX) {
            ability->pause |= SOC_PA_PAUSE_TX;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_PAUSE_RX) {
            ability->pause |= SOC_PA_PAUSE_RX;
        }

        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_100MB) {
            ability->eee |= SOC_PA_EEE_100MB_BASETX;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_1000MB) {
            ability->eee |= SOC_PA_EEE_1GB_BASET;
        }
        if(bmacsec_ability & BMACSEC_PHY84334_PA_EEE_10000MB) {
            ability->eee |= SOC_PA_EEE_10GB_BASET;
        }
    }

    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_84334_lb_set
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
phy_84334_lb_set(int unit, soc_port_t port, int enable)
{
    int           rv;
    phy_ctrl_t    *pc;
    bmacsec_phy84334_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

#if AUTO_MDIX_WHEN_AN_DIS
    {
        /* Disable Auto-MDIX When autoneg disabled */
        /* Enable Auto-MDIX When autoneg disabled */
        data = (enable) ? 0x0000 : 0x0200;
        rv = BMACSEC_MODIFY_PHY_84334_MII_MISC_CTRLr(pc->phy_id, data, 0x0200);
        if(!BMACSEC_E_SUCCESS(rv)) {
            return SOC_ERROR(rv);
        }

    }
#endif

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_loopback_set(phy_dev_addr, port_mode, enable);

    if(!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }
    sal_usleep(1000000);

    if (enable) {
        SOC_IF_ERROR_RETURN
            (phy_84334_linkup(unit, port));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84334_lb_set: u=%d p=%d en=%d rv=%d\n"), 
              unit, port, enable, rv));

    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_84334_lb_get
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
phy_84334_lb_get(int unit, soc_port_t port, int *enable)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_phy84334_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_loopback_get(phy_dev_addr, port_mode, enable);

    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_84334_mdix_set
 * Description:
 *      Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_84334_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{

    int         rv;
    phy_ctrl_t  *pc;
    bmacsec_phy84334_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84334_port_mdix_t bmacsec_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    switch(mode) {
    case SOC_PORT_MDIX_AUTO:
        bmacsec_mode = BMACSEC_PHY84334_PORT_MDIX_AUTO;
        break;
    case SOC_PORT_MDIX_FORCE_AUTO:
        bmacsec_mode = BMACSEC_PHY84334_PORT_MDIX_FORCE_AUTO;
        break;
    case SOC_PORT_MDIX_NORMAL:
        bmacsec_mode = BMACSEC_PHY84334_PORT_MDIX_NORMAL;
        break;
    case SOC_PORT_MDIX_XOVER:
        bmacsec_mode = BMACSEC_PHY84334_PORT_MDIX_XOVER;
        break;
    default :
        return SOC_E_CONFIG;
    }

    rv = bmacsec_phy84334_mdix_set(phy_dev_addr, port_mode, bmacsec_mode);

    if(BMACSEC_E_SUCCESS(rv) && PHY_COPPER_MODE(unit, port)) {
        pc->copper.mdix = mode;
    }
    return SOC_ERROR(rv);
}        

/*
 * Function:
 *      phy_84334_mdix_get
 * Description:
 *      Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - (Out) One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_84334_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    if (mode == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(phy_84334_speed_get(unit, port, &speed));
        if (speed == 1000) {
           *mode = SOC_PORT_MDIX_AUTO;
        } else {
            *mode = pc->copper.mdix;
        }
    } else {
        *mode = SOC_PORT_MDIX_NORMAL;
    }

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_84334_mdix_status_get
 * Description:
 *      Get the current MDIX status on a port/PHY
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 *      status  - (OUT) One of:
 *              SOC_PORT_MDIX_STATUS_NORMAL
 *                      Straight connection
 *              SOC_PORT_MDIX_STATUS_XOVER
 *                      Crossover has been performed
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_84334_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    int         rv;
    phy_ctrl_t    *pc;

    bmacsec_phy84334_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy84334_port_mdix_status_t mdix_status;

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        port_mode = BMACSEC_PHY_FIBER;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy84334_mdix_status_get(phy_dev_addr, port_mode, 
                                          &mdix_status);
    if(BMACSEC_E_SUCCESS(rv)) {
        if(mdix_status == BMACSEC_PHY84334_PORT_MDIX_STATUS_NORMAL) {
            *status = SOC_PORT_MDIX_STATUS_NORMAL;
        }
        if(mdix_status == BMACSEC_PHY84334_PORT_MDIX_STATUS_XOVER) {
            *status = SOC_PORT_MDIX_STATUS_XOVER;
        }
        return SOC_E_NONE;
    }

    return SOC_ERROR(rv);
}    


/*
 * Function:
 *      phy_84334_timesync_config_set
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_84334_timesync_config_set(int unit, soc_port_t port, soc_port_phy_timesync_config_t *timesync_config)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
 
    rv = bmacsec_phy84334_timesync_config_set(phy_dev_addr, (bmacsec_port_phy_timesync_config_t *)timesync_config);
    return (SOC_ERROR(rv));

}


/*
 * Function:
 *     phy_84334_timesync_config_get 
 * Description:
 *     
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_84334_timesync_config_get(int unit, soc_port_t port, soc_port_phy_timesync_config_t *timesync_config)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
 
    rv = bmacsec_phy84334_timesync_config_get(phy_dev_addr, (bmacsec_port_phy_timesync_config_t *)timesync_config);
    return (SOC_ERROR(rv));

}


/*
 * Function:
 *      phy_84334_timesync_control_set
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_84334_timesync_control_set(int unit, soc_port_t port, soc_port_control_phy_timesync_t control_type, uint64 value)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    buint64_t btemp64;

    COMPILER_64_SET(btemp64, COMPILER_64_HI(value), COMPILER_64_LO(value)); 

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
 
    rv = bmacsec_phy84334_timesync_control_set(phy_dev_addr, control_type, btemp64);
    return (SOC_ERROR(rv));

}



/*
 * Function:
 *      phy_84334_timesync_control_get
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */
int
phy_84334_timesync_control_get(int unit, soc_port_t port, soc_port_control_phy_timesync_t control_type, uint64 *value)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    buint64_t btemp64;

    COMPILER_64_ZERO(btemp64);
    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
 
    rv = bmacsec_phy84334_timesync_control_get(phy_dev_addr, control_type, &btemp64);
  
    COMPILER_64_SET(*value, COMPILER_64_HI(btemp64), COMPILER_64_LO(btemp64));

    return (SOC_ERROR(rv));

}


/*
 * Function:
 *      phy_84334_control_set
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
phy_84334_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv, pause_fwd = 0, temp;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);


    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 

    rv = BMACSEC_E_NONE;    
    switch(type) {
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED:
            pc->macsec_switch_fixed = value;
            if(value) {
                rv = bmacsec_phy84334_switch_side_set_params(phy_dev_addr, 
                                BMACSEC_BCM84334_SWITCH_FIXED, 
                                pc->macsec_switch_fixed_speed,
                                pc->macsec_switch_fixed_duplex,
                                pc->macsec_switch_fixed_pause);
            } else { /* Switch side follows line side */
                rv = bmacsec_phy84334_switch_side_set_params(phy_dev_addr, 
                                BMACSEC_BCM84334_SWITCH_FOLLOWS_LINE, 
                                -1, -1, -1);
            }

            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED:
            /* Switch fixed speed is allowed only to be 1000Mbps */
            if (value != 1000) {
                return SOC_E_CONFIG;
            }
            if(pc->macsec_switch_fixed_speed != value) {
                int line_speed;

                pc->macsec_switch_fixed_speed = value;

                SOC_IF_ERROR_RETURN
                    (phy_84334_speed_get(unit, port, &line_speed));
                pc->macsec_switch_fixed_speed = line_speed;

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

            rv = bmacsec_phy84334_line_side_pause_forward(phy_dev_addr,
                                                          pause_fwd);
            if (rv == BMACSEC_E_NONE) {
                rv = bmacsec_phy84334_switch_side_pause_forward(
                                                     phy_dev_addr, pause_fwd);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD:
            pc->macsec_pause_tx_fwd = value;
            pause_fwd = (value == 1) ? 1 : 0;

            rv = bmacsec_phy84334_line_side_pause_forward(phy_dev_addr,
                                                          pause_fwd);
            if (rv == BMACSEC_E_NONE) {
                rv = bmacsec_phy84334_switch_side_pause_forward(phy_dev_addr,
                                                                pause_fwd);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_LINE_IPG:
            pc->macsec_line_ipg = value;
            rv = bmacsec_phy84334_line_side_ipg_set(phy_dev_addr, value);
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_IPG:
            pc->macsec_switch_ipg = value;
            rv = bmacsec_phy84334_switch_side_ipg_set(phy_dev_addr, value);
            break;




    case SOC_PHY_CONTROL_EEE:
        if (value) {
                SOC_IF_ERROR_RETURN
                    (bmacsec_phy84334_eee_mode_set(phy_dev_addr, 1, 0x0000, 0x0000, 0x0000 ));

            pc->copper.advert_ability.eee |= (SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
            temp = pc->copper.autoneg_enable;
            /* Initiate AN */
            SOC_IF_ERROR_RETURN
                (phy_84334_autoneg_set(unit, port, 1));
            pc->copper.autoneg_enable = temp;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_EEE_ENABLED);
            PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_MODE);
        } else {
                SOC_IF_ERROR_RETURN
                    (bmacsec_phy84334_eee_mode_set(phy_dev_addr, 0, 0x0000, 0x0000, 0x0000 ));
            pc->copper.advert_ability.eee &= ~(SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
            SOC_IF_ERROR_RETURN
                (phy_84334_autoneg_set(unit, port, pc->copper.autoneg_enable ? 1 : 0));
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_EEE_ENABLED);
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO:
        if (value) {
                SOC_IF_ERROR_RETURN
                    (bmacsec_phy84334_eee_mode_set(phy_dev_addr, 2, 0x0000, 0x047f, 0x047e ));

            pc->copper.advert_ability.eee |= (SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
            temp = pc->copper.autoneg_enable;
            /* Initiate AN */
            SOC_IF_ERROR_RETURN
                (phy_84334_autoneg_set(unit, port, 1));
            pc->copper.autoneg_enable = temp;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_EEE_ENABLED);
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_MODE);
        } else {
           SOC_IF_ERROR_RETURN
               (bmacsec_phy84334_eee_mode_set(phy_dev_addr, 0, 0x0000, 0x0000, 0x0000 ));
            pc->copper.advert_ability.eee &= ~(SOC_PA_EEE_10GB_BASET | SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
            SOC_IF_ERROR_RETURN
                (phy_84334_autoneg_set(unit, port, pc->copper.autoneg_enable ? 1 : 0));
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_EEE_ENABLED);
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
        { 
            uint16 mode=0, latency=0, th_high=0, th_low=0;
            SOC_IF_ERROR_RETURN
                (bmacsec_phy84334_eee_mode_get(phy_dev_addr, &mode, &th_high, &th_low, &latency ));
            /* Fixed latency can have only 0 or 1 value  */
            if ((value != FALSE) && (value != TRUE)) {
                return SOC_E_PARAM;
            }
            if (value == FALSE) {
                /* Variable latency  */
                SOC_IF_ERROR_RETURN
                    (bmacsec_phy84334_eee_mode_set(phy_dev_addr, 3, th_high, th_low, latency ));
            } else {
                /* Fixed latency  */ 
                SOC_IF_ERROR_RETURN
                    (bmacsec_phy84334_eee_mode_set(phy_dev_addr, 2, th_high, th_low, latency ));
            }
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
        {
            uint16 mode, buffer_len;
            SOC_IF_ERROR_RETURN
                (bmacsec_phy84334_eee_mode_get(phy_dev_addr, &mode, NULL, NULL, &buffer_len ));
            if ( (mode != 2) || (buffer_len > value)) {
                return SOC_E_PARAM;
            }
            SOC_IF_ERROR_RETURN
                (bmacsec_phy84334_eee_mode_set(phy_dev_addr, 2, (uint16)((value >> 16) & 0xffff), (uint16)(value & 0xffff), buffer_len ));
        }
        break;

    case SOC_PHY_CONTROL_EEE_STATISTICS_CLEAR:
        SOC_IF_ERROR_RETURN
            (bmacsec_phy84334_eee_statistics_clear(phy_dev_addr));

        break;

    case SOC_PHY_CONTROL_EEE_AUTO_BUFFER_LIMIT:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_WAKE_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_WAKE_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_SLEEP_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_SLEEP_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_QUIET_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_QUIET_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_REFRESH_TIME:
            return SOC_E_UNAVAIL;


        default:
            return SOC_E_UNAVAIL;
    }

    if (rv != BMACSEC_E_NONE) {
        return SOC_ERROR(rv);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84334_control_get
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
phy_84334_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    buint16_t mode, temp, temp1;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 

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


    case SOC_PHY_CONTROL_EEE:
        *value = (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_ENABLED) && 
                 !PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_MODE)) ? 1 : 0;
        break;

    case SOC_PHY_CONTROL_EEE_AUTO:
        *value = (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_ENABLED) && 
                  PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_MODE)) ? 1 : 0;
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
        { 
            uint16 mode=0 ,latency=0;
            SOC_IF_ERROR_RETURN
                (bmacsec_phy84334_eee_mode_get(phy_dev_addr, &mode, NULL, NULL, &latency));
            if (mode == 2 || mode == 6 || mode == 0xa) {
                 *value = 1; 
            }
            else if (mode == 3 || mode == 7 || mode == 0xb) {
                 *value = 0; 
            }
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
        SOC_IF_ERROR_RETURN
            (bmacsec_phy84334_eee_mode_get(phy_dev_addr, &mode, &temp1, &temp, NULL));
        if (mode != 2) {
            return SOC_E_UNAVAIL;
        }
        *value = (((uint32) temp1) << 16) | temp;
        break;

    case SOC_PHY_CONTROL_EEE_TRANSMIT_EVENTS:
        SOC_IF_ERROR_RETURN
            (bmacsec_phy84334_eee_tx_events_get(phy_dev_addr, value));
        break;

    case SOC_PHY_CONTROL_EEE_TRANSMIT_DURATION:
        SOC_IF_ERROR_RETURN
            (bmacsec_phy84334_eee_tx_duration_get(phy_dev_addr, value));
        break;

    case SOC_PHY_CONTROL_EEE_RECEIVE_EVENTS:
        SOC_IF_ERROR_RETURN
            (bmacsec_phy84334_eee_rx_events_get(phy_dev_addr, value));
        break;

    case SOC_PHY_CONTROL_EEE_RECEIVE_DURATION:
        SOC_IF_ERROR_RETURN
            (bmacsec_phy84334_eee_tx_duration_get(phy_dev_addr, value));
        break;

        default:
            rv = SOC_E_UNAVAIL;
            break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_84334_linkup
 * Purpose:
 *      Link up handler
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_84334_linkup(int unit, soc_port_t port)
{
    int rv;
    int line_speed;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;


    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_84334_speed_get(unit, port, &line_speed));

    switch (line_speed) {

    case 10000:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_XAUI));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventSpeed, 10000));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventResume, 1));
        break;

    case 1000:
        if (PHY_COPPER_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventSpeed, 1000));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_XAUI));
        }
        break;

    case 100:
    case 10:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventSpeed, line_speed));
        break;

    default:
        break;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);

    /* Call BMACSEC linkup routine */
    rv = bmacsec_84334_phy_linkup(phy_dev_addr, pc->macsec_enable);

    if (BMACSEC_E_FAILURE(rv)) {
        return SOC_E_FAIL;
    }

    if (pc->macsec_enable != 1) {
        return SOC_E_NONE;
    }

    pc->macsec_switch_fixed_speed = line_speed;


    /* wait for Power Down of unused MACSEC sub-blocks to complete */
    sal_usleep(400000);

    rv = bmacsec_phy84334_switch_side_set_params(phy_dev_addr, 
                                      BMACSEC_BCM84334_SWITCH_FIXED,
                                      pc->macsec_switch_fixed_speed,
                                      pc->macsec_switch_fixed_duplex,
                                      pc->macsec_switch_fixed_pause);
    if (BMACSEC_E_FAILURE(rv)) {
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84334_firmware_set
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
phy_84334_firmware_set(int unit, int port, int offset, uint8 *data,int len)
{
    int            rv;
    phy_ctrl_t *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);

    offset = (data == 0) ? BMACSEC_PHY84334_MAP_UCODE_OFFSET(offset) : 0 ;
    len    = (data == 0) ? (PHY_MODEL(pc->phy_id0, pc->phy_id1))     : len;

    rv = bmacsec_phy84334_firmware_set(phy_dev_addr, offset, data, len);
    if (rv != BMACSEC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84334 firmware upgrade possibly failed:"
                                 "u=%d p=%d\n"), unit, port));
        return (SOC_E_FAIL);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84334 firmware upgrade successful:"
                         "u=%d p=%d\n"), unit, port));

#if 0
    if (data == 0 && offset == BMACSEC_PHY84334_UCODE_BCST_END) {
        soc_timeout_init(&TO(pc), 2000000, 0);
    }
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84334_probe
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
phy_84334_probe(int unit, phy_ctrl_t *pc)
{
    uint16  id0, id1;

    if (READ_PHY84334_PMA_PMD_REG(unit, pc, 0x2, &id0) < 0) {
        return SOC_E_NOT_FOUND;
    }

    if (READ_PHY84334_PMA_PMD_REG(unit, pc, 0x3, &id1) < 0) {
        return SOC_E_NOT_FOUND;
    }

    switch (PHY_MODEL(id0, id1)) {

    case PHY_BCM84334_MODEL:
        if (id1 & 0x8) {
            if (!soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0)) {
                break;
            }
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
            pc->size = sizeof(PHY8433_PRIV_t);
            return SOC_E_NONE;
        }
    break;

    case PHY_BCM84333_MODEL:
    case PHY_BCM84336_MODEL:
        if (id1 & 0x8) {
            if (!soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0)) {
                break;
            }
            pc->size = sizeof(PHY8433_PRIV_t);
            return SOC_E_NONE;
        }
    break;

    case PHY_BCM84844_MODEL:
    case PHY_BCM84848_MODEL:
#if 1
        if (!soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0)) {
            break;
        }
#endif
        PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_FORCED_COPPER);
        pc->size = sizeof(PHY8433_PRIV_t);
        return SOC_E_NONE;
    break;

    case PHY_BCM84846_MODEL:
#if 1
        if (!soc_property_port_get(unit, pc->port, spn_MACSEC_ENABLE, 0)) {
            break;
        }
#endif
        pc->size = sizeof(PHY8433_PRIV_t);
        return SOC_E_NONE;
    break;

    default:
    break;
    }
    return SOC_E_NOT_FOUND;
}


#ifdef BROADCOM_DEBUG

/*
 * Function:
 *      phy_84334_shadow_dump
 * Purpose:
 *      Debug routine to dump all shadow registers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 */

void
phy_84334_shadow_dump(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    uint16      tmp;
    int         i;

    pc       = EXT_PHY_SW_STATE(unit, port);

    /* Register 0x18 Shadows */
    for (i = 0; i <= 7; i++) {
        WRITE_PHY_REG(unit, pc, 0x18, (i << 12) | 0x7);
        READ_PHY_REG(unit, pc, 0x18, &tmp);
        if ((tmp & ~7) == 0x0000) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "0x18[0x%x]=0x%04x\n"), i, tmp));
    }

    /* Register 0x1c Shadows */
    for (i = 0; i <= 0x1f; i++) {
        WRITE_PHY_REG(unit, pc, 0x1c, i << 10);
        READ_PHY_REG(unit, pc, 0x1c, &tmp);
        if ((tmp & ~0x7c00) == 0x0000) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "0x1c[0x%x]=0x%04x\n"), i, tmp));
    }

    /* Register 0x17/0x15 Shadows */
    for (i = 0; i <= 0x13; i++) {
        WRITE_PHY_REG(unit, pc, 0x17, 0xf00 | i);
        READ_PHY_REG(unit, pc, 0x15, &tmp);
        if (tmp  == 0x0000) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "0x17[0x%x]=0x%04x\n"), i, tmp));
    }
}

#endif /* BROADCOM_DEBUG */

/*
 * Variable:    phy_84334drv_ge
 * Purpose:     PHY driver for 84334
 */
phy_driver_t phy_84334drv_xe = {
    "84334 (MACSec PHY) Gigabit PHY Driver",
    phy_84334_init,
    phy_84334_reset,
    phy_84334_link_get,
    phy_84334_enable_set,
    phy_84334_enable_get,
    phy_84334_duplex_set,
    phy_84334_duplex_get,
    phy_84334_speed_set,
    phy_84334_speed_get,
    phy_84334_master_set,
    phy_84334_master_get,
    phy_84334_autoneg_set,
    phy_84334_autoneg_get,
    NULL,
    NULL,
    NULL,
    phy_84334_lb_set,
    phy_84334_lb_get,
    phy_null_interface_set,
    phy_xehg_interface_get,
    NULL,
    phy_84334_linkup,            /* Phy link up event */
    NULL,                        /* Phy link down event */
    phy_84334_mdix_set,
    phy_84334_mdix_get,
    phy_84334_mdix_status_get,
    phy_84334_medium_config_set,
    phy_84334_medium_config_get,
    phy_84334_medium_status,
    NULL,                       /* cable diag */
    NULL,                       /* phy_link_change */
    phy_84334_control_set,      /* phy_control_set */ 
    phy_84334_control_get,      /* phy_control_get */
    NULL,            /* phy_ge_reg_read */
    NULL,           /* phy_ge_reg_write */
    NULL,          /* phy_ge_reg_modify */
    NULL,          /* Phy event notify */
    phy_84334_probe,          /* pd_probe  */
    phy_84334_ability_advert_set,  /* pd_ability_advert_set */
    phy_84334_ability_advert_get,  /* pd_ability_advert_get */
    phy_84334_ability_remote_get,  /* pd_ability_remote_get */
    phy_84334_ability_local_get,   /* pd_ability_local_get  */
    phy_84334_firmware_set,
    phy_84334_timesync_config_set,
    phy_84334_timesync_config_get,
    phy_84334_timesync_control_set,
    phy_84334_timesync_control_get
};
#endif /* INCLUDE_MACSEC */

#else /* INCLUDE_PHY_84334 */
typedef int _soc_phy_84334_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_84334 */
