/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy54880.c
 * Purpose:     PHY driver for BCM54880
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_54880)
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

#include "phyident.h"
#include "phyreg.h"
#include "phyfege.h"
#include "phynull.h"
#include "phy54880.h"

#define PHY_IEEE_MODE(unit, port)   (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_BR))
                                  

#define SOC_PORT_MEDIUM_IEEE        SOC_PORT_MEDIUM_COPPER
#define SOC_PORT_MEDIUM_BR          SOC_PORT_MEDIUM_FIBER
#define SOC_PA_MEDIUM_IEEE          SOC_PA_MEDIUM_COPPER

#define PHY_IS_BCM54880_A0(_pc)   (PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54880_OUI, \
                                                PHY_BCM54880_MODEL) \
                                   && ((_pc)->phy_rev == 0x0 ))

#define PHY_IS_BCM54880(_pc)   (PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54880_OUI, \
                                                PHY_BCM54880_MODEL))

#define PHY_IS_BCM54881(_pc)   (PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54881_OUI, \
                                                PHY_BCM54881_MODEL))

#define PHY_IS_BCM54811(_pc)   (PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54811_OUI, \
                                                PHY_BCM54811_MODEL) \
                                                && ((_pc)->phy_rev == 0x5 ))


#define PHY_IS_B0(_pc)   ((PHY_IS_BCM54880(_pc) || PHY_IS_BCM54881(_pc)) \
                                   && ((_pc)->phy_rev == 0x1))

#define PHY_BCM54880_A0_ID0    PHY_ID0(PHY_BCM54880_OUI,PHY_BCM54880_MODEL,0)
#define PHY_BCM54880_A0_ID1    PHY_ID1(PHY_BCM54880_OUI,PHY_BCM54880_MODEL,0)

#ifndef DISABLE_CLK125
#define DISABLE_CLK125 0
#endif

#ifndef AUTO_MDIX_WHEN_AN_DIS
#define AUTO_MDIX_WHEN_AN_DIS 0
#endif

#define DISABLE_TEST_PORT 1
/* #define A0_AUTO_FILL_PORT_INDEX 1 */

#define LONGREACH_ABILITY_54880 ( SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX )

#define LONGREACH_ABILITY_54881 ( SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX )

#define LONGREACH_ABILITY_54811 ( SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX |\
                                  SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX )


#define LONGREACH_ABILITY_ALL(_pc) ( PHY_IS_BCM54881((_pc)) ? LONGREACH_ABILITY_54881 : \
                                     ( PHY_IS_BCM54811((_pc)) ? LONGREACH_ABILITY_54811 : \
                                       LONGREACH_ABILITY_54880)) 

#define LDS_ADVA_MASK(_pc)   ( PHY_IS_BCM54881((_pc)) ?  \
                               LDS_ADVA_ASYM_PAUSE|LDS_ADVA_PAUSE|\
                               LDS_ADVA_2_50|LDS_ADVA_2_33|LDS_ADVA_2_25|LDS_ADVA_2_20|\
                               LDS_ADVA_1_50|LDS_ADVA_1_33|LDS_ADVA_1_25|LDS_ADVA_1_20|\
                               LDS_ADVA_1_100|LDS_ADVA_4_100|LDS_ADVA_2_100|LDS_ADVA_2_10|LDS_ADVA_1_10 :\
                               LDS_ADVA_ASYM_PAUSE|LDS_ADVA_PAUSE|\
                               LDS_ADVA_1_100|LDS_ADVA_4_100|LDS_ADVA_2_100|LDS_ADVA_2_10|LDS_ADVA_1_10 )

          

#define LONGREACH_ABILITY_10M  ( SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR | \
                                 SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR )
#define LONGREACH_ABILITY_20M  ( SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR | \
                                 SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR )
#define LONGREACH_ABILITY_25M  ( SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_1PAIR | \
                                 SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_2PAIR )
#define LONGREACH_ABILITY_33M  ( SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR | \
                                 SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR )
#define LONGREACH_ABILITY_50M  ( SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR | \
                                 SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR )
#define LONGREACH_ABILITY_100M ( SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR | \
                                 SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR | \
                                 SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR )

typedef struct _phy_br_config_t {
    int                 enable;
    int                 lds_enable;
    uint32              lds_advert_ability;
    int                 force_speed;
    int                 force_pairs;
    int                 master;
    int                 gain;
    int                 bypass_reset;
} phy_br_config_t;

#define SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR(op)                      \
    do {                                                                \
        int __rv__;                                                     \
        if (((__rv__ = (op)) < 0)) {                                    \
            if ((__rv__ == SOC_E_DISABLED)) {                           \
               __rv__ = SOC_E_NONE;                                     \
            }                                                           \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

STATIC int
_phy_54880_ieee_speed_set(int unit, soc_port_t port, int speed);
STATIC int
_phy_54880_ieee_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int
_phy_54880_ieee_master_set(int unit, soc_port_t port, int master);
STATIC int
_phy_54880_ieee_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int
_phy_54880_ieee_autoneg_set(int unit, soc_port_t port, int autoneg);
STATIC int
_phy_54880_ieee_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
STATIC int
_phy_54880_ieee_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int
_phy_54880_medium_change(int unit, soc_port_t port, int force_update);

STATIC int
_phy_54880_br_speed_set(int unit, soc_port_t port, int speed);
STATIC int
_phy_54880_br_pairs_set(int unit, soc_port_t port, int pairs);
STATIC int
_phy_54880_br_master_set(int unit, soc_port_t port, int master);
STATIC int
_phy_54880_br_ability_advert_set(int unit, soc_port_t port, uint32 ability);
STATIC int
_phy_54880_br_autoneg_set(int unit, soc_port_t port, int autoneg);
STATIC int
_phy_54880_br_gain_set(int unit, soc_port_t port, int gain);

STATIC int
phy_54880_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value);

extern int
phy_ecd_cable_diag_init(int unit, soc_port_t port);

extern int
phy_ecd_cable_diag(int unit, soc_port_t port,
            soc_port_cable_diag_t *status);

extern int
phy_acd_cable_diag_init(int unit, soc_port_t port);

extern int
phy_acd_cable_diag(int unit, soc_port_t port,
            soc_port_cable_diag_t *status);

STATIC int
_SW2IEEE(int unit,phy_ctrl_t *pc) 
{
    uint16              lds_acc;
    /* BCM54811 */
    if (PHY_IS_BCM54811(pc)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54811_EXP_BR_LDS_CHAN_STATUSr(unit, pc, 
                             (PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_EN
                              | PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_VAL), 
                             (PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_EN
                              | PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_VAL)));
        SOC_IF_ERROR_RETURN
            (READ_PHY54811_EXP_BR_LDS_CHAN_STATUSr(unit, pc, &lds_acc));
        if(lds_acc & PHY54811_LDS_BR_IEEE_REG_ACCESS_CTRL_STATUS) {
            return SOC_E_DISABLED;
        } 
        return 0;
    }


    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LDS_ACCr(unit, pc, 0x6));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LDS_ACCr((unit), (pc), &lds_acc));
    if (lds_acc & LDS_ACC_STAT) {
        return SOC_E_DISABLED;
    }
    return 0;
}

STATIC int
_SW2BR(int unit,phy_ctrl_t *pc) 
{
    uint16              lds_acc;    
    phy_br_config_t    *br_config;

    /* BCM54811 */
    if (PHY_IS_BCM54811(pc)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54811_EXP_BR_LDS_CHAN_STATUSr(unit, pc, 
                             PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_EN,
                              (PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_EN
                              | PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_VAL)));
        SOC_IF_ERROR_RETURN
            (READ_PHY54811_EXP_BR_LDS_CHAN_STATUSr(unit, pc, &lds_acc));
        if( 0 == (lds_acc & PHY54811_LDS_BR_IEEE_REG_ACCESS_CTRL_STATUS)) {
            return SOC_E_DISABLED;
        } 
        return 0;
    }

    br_config     = (phy_br_config_t *)  &pc->fiber;
    if (br_config->enable && !br_config->lds_enable) {
        return 0;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LDS_ACCr(unit, pc, 0x4));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LDS_ACCr((unit), (pc), &lds_acc));
    if ((lds_acc & LDS_ACC_STAT) == 0) {
        return SOC_E_DISABLED;
    }
    return 0;
}

/*
 * Function:
 *      phy_54880_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE.
 */
STATIC int
phy_54880_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    *medium = SOC_PORT_MEDIUM_IEEE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_54880_ieee_medium_config_update
 * Purpose:
 *      Update the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_54880_ieee_medium_config_update(int unit, soc_port_t port, 
                           soc_phy_config_t  *cfg)
{
    SOC_IF_ERROR_RETURN
        (_phy_54880_ieee_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (_phy_54880_ieee_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (_phy_54880_ieee_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (_phy_54880_ieee_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (_phy_54880_ieee_autoneg_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (_phy_54880_ieee_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_medium_config_update(int unit, soc_port_t port, 
                           phy_br_config_t  *cfg)
{
    /* Notice the order. Speed, pairs and master could be written to the regs
       only if LDS is disabled. 
     */

    SOC_IF_ERROR_RETURN
        (_phy_54880_br_ability_advert_set(unit, port, cfg->lds_advert_ability));
    SOC_IF_ERROR_RETURN
        (_phy_54880_br_autoneg_set(unit, port, cfg->lds_enable));

    if ( !cfg->lds_enable ) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_speed_set(unit, port, cfg->force_speed));
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_pairs_set(unit, port, cfg->force_pairs));
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_master_set(unit, port, cfg->master));
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_gain_set(unit, port, cfg->gain));
    }

    return SOC_E_NONE;
}

STATIC int
phy_54880_medium_config_set(int unit, soc_port_t port,
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t        *pc;
    soc_port_ability_t ability;

    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Changes take effect immediately if the target medium is active.
     */
    if (medium == SOC_PORT_MEDIUM_COPPER) {

        sal_memcpy(&pc->copper, cfg, sizeof (pc->copper));

        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_ability_local_get(unit, port, &ability));

        pc->copper.advert_ability.speed_half_duplex &= ability.speed_half_duplex;
        pc->copper.advert_ability.speed_full_duplex &= ability.speed_full_duplex;
        pc->copper.advert_ability.pause &= ability.pause;

        if (PHY_IEEE_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (_phy_54880_ieee_medium_config_update(unit, port, &pc->copper));
        }
        return SOC_E_NONE;
    }

    return SOC_E_CONFIG;
}


/*
 * Function:
 *      phy_54880_medium_config_get
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
phy_54880_medium_config_get(int unit, soc_port_t port, 
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    phy_ctrl_t    *pc;

    COMPILER_REFERENCE(medium);

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memcpy(cfg, &pc->copper, sizeof (*cfg));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_54880_reset_setup
 * Purpose:
 *      Function to reset the PHY and set up initial operating registers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54880_reset_setup(int unit, soc_port_t port)
{
    phy_ctrl_t        *pc;
    uint16             tmp;
    uint16             phy_addr;
    uint16             id0, id1;
    int                index;

    pc     = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(phy_ge_init(unit, port));

    phy_addr = pc->phy_id;
    index = phy_addr & 7;

    /* set the default values that are valid for many boards */
    SOC_IF_ERROR_RETURN
        (phy_54880_control_set( unit, port, SOC_PHY_CONTROL_PORT_PRIMARY, 
        (port - index) < 0 ? 0:(port - index)));
    SOC_IF_ERROR_RETURN
        (phy_54880_control_set( unit, port, SOC_PHY_CONTROL_PORT_OFFSET, index ));

#ifdef DISABLE_TEST_PORT
    if (PHY_IS_BCM54880_A0(pc))  {
        /* Disable test port */

        phy_addr = pc->phy_id + 1;

        SOC_IF_ERROR_RETURN
            (pc->read(unit, phy_addr, MII_PHY_ID0_REG, &id0));

        SOC_IF_ERROR_RETURN
            (pc->read(unit, phy_addr, MII_PHY_ID1_REG, &id1));

        if ( ( id0 != PHY_BCM54880_A0_ID0 ) || ( id1 != PHY_BCM54880_A0_ID1 ) ) {

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x1f, 0xffd0));

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x1e, 0x001f));

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x1f, 0x8000));

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x1d, 0x4002));

            SOC_IF_ERROR_RETURN
                (pc->write(unit, phy_addr, 0x00, MII_CTRL_RESET));

#ifdef A0_AUTO_FILL_PORT_INDEX
            {
                int i;

                for (i = 0; i < 8; i++) {
                    SOC_IF_ERROR_RETURN
                        (phy_54880_control_set(unit, port - i, SOC_PHY_CONTROL_PORT_PRIMARY, port - 7));
                    SOC_IF_ERROR_RETURN
                        (phy_54880_control_set(unit, port - i, SOC_PHY_CONTROL_PORT_OFFSET, 7 - i ));
                }
            }
#endif
        }
    }
#endif

    /* remove power down */
    if (pc->copper.enable) {
        tmp = PHY_DISABLED_MODE(unit, port) ? MII_CTRL_PD : 0;
    } else {
        tmp = MII_CTRL_PD;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MII_CTRLr(unit, pc, tmp, MII_CTRL_PD));

#if DISABLE_CLK125
    /* Reduce EMI emissions by disabling the CLK125 pin if not used */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_SPARE_CTRL_3r(unit, pc, 0, 1));
#endif

    /* Configure Extended Control Register */
    /* Enable LEDs to indicate traffic status */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MII_ECRr(unit, pc, 0x0020, 0x0020));

#if AUTO_MDIX_WHEN_AN_DIS
    /* Enable Auto-MDIX When autoneg disabled */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MII_MISC_CTRLr(unit, pc, 0x0200, 0x0200));
#endif

    /* Enable extended packet length (4.5k through 25k) */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MII_AUX_CTRLr(unit, pc, 0x4000, 0x4000));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MISC_1000X_CTRL2r(unit, pc, 0x0001, 0x0001));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_AUX_CTRLr(unit, pc, 0x0002, 0x0002));

    /* Configure LED selectors */
    tmp = ((pc->ledmode[1] & 0xf) << 4) | (pc->ledmode[0] & 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LED_SELECTOR_1r(unit, pc, tmp));

    tmp = ((pc->ledmode[3] & 0xf) << 4) | (pc->ledmode[2] & 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LED_SELECTOR_2r(unit, pc, tmp));

    tmp = (pc->ledctrl & 0x3ff);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LED_CTRLr(unit, pc, tmp));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_EXP_LED_SELECTORr(unit, pc, pc->ledselect));

    /* LDS parameters */

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_EXP_BR_LDS_RX_CTRL1r(unit, pc, 0x0e1b));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_EXP_BR_LDS_RX_CTRL2r(unit, pc, 0x0306));

    /*
     * Configure Auxiliary control register to turn off
     * carrier extension.  The Intel 7131 NIC does not accept carrier
     * extension and gets CRC errors.
     */
    /* Disable carrier extension */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_AUX_CTRLr(unit, pc, 0x0040, 0x0040));

    if (PHY_IS_B0(pc)) {
        /* B0 errata */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL2r(unit, pc, 1U<<3, 1U<<3));
    }

    if ((PHY_IS_BCM54880(pc)) && ((pc->phy_rev & 0xf) > 0x1))  {
        SOC_IF_ERROR_RETURN(
            phy_ecd_cable_diag_init(unit, port));
    }

    return SOC_E_NONE;

}

/*
 * Function:  
 *      _phy_54811_reset_setup
 * Purpose:
 *      Function to reset the PHY and set up initial operating registers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54811_reset_setup(int unit, soc_port_t port)
{
    phy_ctrl_t        *pc;
    uint16             tmp;
    uint16             phy_addr;
    int                index;

    SOC_IF_ERROR_RETURN(phy_ge_init(unit, port));

    pc     = EXT_PHY_SW_STATE(unit, port);

    phy_addr = pc->phy_id;
    index = phy_addr & 7;

    /* set the default values that are valid for many boards */
    SOC_IF_ERROR_RETURN
        (phy_54880_control_set( unit, port, SOC_PHY_CONTROL_PORT_PRIMARY, 
        (port - index) < 0 ? 0:(port - index)));
    SOC_IF_ERROR_RETURN
        (phy_54880_control_set( unit, port, SOC_PHY_CONTROL_PORT_OFFSET, index ));

    /* remove power down */
    if (pc->copper.enable) {
        tmp = PHY_DISABLED_MODE(unit, port) ? MII_CTRL_PD : 0;
    } else {
        tmp = MII_CTRL_PD;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MII_CTRLr(unit, pc, tmp, MII_CTRL_PD));

    /* Configure Extended Control Register */
    /* Enable LEDs to indicate traffic status */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MII_ECRr(unit, pc, 0x0020, 0x0020));

#if AUTO_MDIX_WHEN_AN_DIS
    /* Enable Auto-MDIX When autoneg disabled */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MII_MISC_CTRLr(unit, pc, 0x0200, 0x0200));
#endif

    /* Enable extended packet length (4.5k through 25k) */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MII_AUX_CTRLr(unit, pc, 0x4000, 0x4000));

#if 0 

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_MISC_1000X_CTRL2r(unit, pc, 0x0001, 0x0001));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_AUX_CTRLr(unit, pc, 0x0002, 0x0002));
#endif

    /* Configure LED selectors */
    tmp = ((pc->ledmode[1] & 0xf) << 4) | (pc->ledmode[0] & 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LED_SELECTOR_1r(unit, pc, tmp));

    tmp = ((pc->ledmode[3] & 0xf) << 4) | (pc->ledmode[2] & 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LED_SELECTOR_2r(unit, pc, tmp));

    tmp = (pc->ledctrl & 0x3ff);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LED_CTRLr(unit, pc, tmp));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_EXP_LED_SELECTORr(unit, pc, pc->ledselect));

#if 0 

    /* LDS parameters */

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_EXP_BR_LDS_RX_CTRL1r(unit, pc, 0x0e1b));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_EXP_BR_LDS_RX_CTRL2r(unit, pc, 0x0306));

    /*
     * Configure Auxiliary control register to turn off
     * carrier extension.  The Intel 7131 NIC does not accept carrier
     * extension and gets CRC errors.
     */
    /* Disable carrier extension */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_AUX_CTRLr(unit, pc, 0x0040, 0x0040));

    if (PHY_IS_B0(pc)) {
        /* B0 errata */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL2r(unit, pc, 1U<<3, 1U<<3));
    }

#endif

    return SOC_E_NONE;

}


/*
 * Function:
 *      phy_54880_init
 * Purpose:
 *      Init function for 54880 PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_54880_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;
    soc_phy_config_t   *copper_config;
    phy_br_config_t    *br_config;
    int                lr_initial_mode;
    int                lr_initial_ctrl;
    int                lr_initial_adva;

    pc    = EXT_PHY_SW_STATE(unit, port);

    copper_config = (soc_phy_config_t *) &pc->copper;
    br_config     = (phy_br_config_t *)  &pc->fiber;

    pc->automedium = FALSE;

    copper_config->enable = TRUE;
    copper_config->autoneg_enable = TRUE;
    if (PHY_IS_BCM54881(pc)) {
        copper_config->force_speed = 100;
    } else {
        copper_config->force_speed = 1000;
    }
    copper_config->force_duplex = TRUE;
    copper_config->master = SOC_PORT_MS_AUTO;
    copper_config->mdix = SOC_PORT_MDIX_AUTO;

    br_config->bypass_reset = 0;

    /* Initial Setting for BR mode */
    lr_initial_mode = 
        soc_property_port_get(unit, port, spn_PHY_LR_INITIAL_MODE, -1);
    lr_initial_ctrl = 
        soc_property_port_get(unit, port, spn_PHY_LR_INITIAL_CTRL, -1);
    lr_initial_adva = 
        soc_property_port_get(unit, port, spn_PHY_LR_INITIAL_ADVA, -1);

    /* Initial BR mode settings */
    if (lr_initial_mode != -1) {
        br_config->enable = 
            (lr_initial_mode & LR_INITIAL_MODE_LR_ENABLE) ? TRUE : FALSE; 
    } else {
        br_config->enable = FALSE;
    }

    /* Initial BR ctrl settings */
    if (lr_initial_ctrl != -1) {
        br_config->lds_enable = 
            (lr_initial_ctrl & LR_INITIAL_CTRL_LDS_ENABLE) ? TRUE : FALSE;

        if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_PS_MASK) == 
            LR_INITIAL_CTRL_LR_PS_1P) {
            br_config->force_pairs = 1;
        } else if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_PS_MASK) == 
                    LR_INITIAL_CTRL_LR_PS_2P) {
            br_config->force_pairs = 2;
        } else if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_PS_MASK) == 
                    LR_INITIAL_CTRL_LR_PS_4P) {
            br_config->force_pairs = 4;
        } else {
            br_config->force_pairs = 0;
        }

        br_config->master = 
            (lr_initial_ctrl & LR_INITIAL_CTRL_LR_MS) ? 
                        SOC_PORT_MS_MASTER : SOC_PORT_MS_SLAVE;

        if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_SS_MASK) == 
            LR_INITIAL_CTRL_LR_SS_10) {
            br_config->force_speed = 10;
        } else if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_SS_MASK) == 
                    LR_INITIAL_CTRL_LR_SS_20) {
            br_config->force_speed = 20;
        } else if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_SS_MASK) == 
                    LR_INITIAL_CTRL_LR_SS_25) {
            br_config->force_speed = 25;
        } else if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_SS_MASK) == 
                    LR_INITIAL_CTRL_LR_SS_33) {
            br_config->force_speed = 33;
        } else if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_SS_MASK) == 
                    LR_INITIAL_CTRL_LR_SS_50) {
            br_config->force_speed = 50;
        } else if ((lr_initial_ctrl & LR_INITIAL_CTRL_LR_SS_MASK) == 
                    LR_INITIAL_CTRL_LR_SS_100) {
            br_config->force_speed = 100;
        } else {
            br_config->force_speed = 100;
        }
            
    } else {
        br_config->lds_enable = TRUE;
        br_config->force_pairs = 0;
        br_config->master = SOC_PORT_MS_AUTO;
        br_config->force_speed = 100;
    }

    /* Initial advert ability, valid only if LDS is enabled */
    if (lr_initial_adva != -1) {
        br_config->lds_advert_ability = 0;
        if(lr_initial_adva & LR_INITIAL_ADVA_1_10) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_2_10) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_1_20) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_2_20) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_1_33) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_2_33) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_1_50) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_2_50) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_1_100) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_2_100) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_4_100) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_PAUSE_RX) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX;
        }
        if(lr_initial_adva & LR_INITIAL_ADVA_PAUSE_TX) {
            br_config->lds_advert_ability |= 
                SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX;
        }
    } else {
        br_config->lds_advert_ability = LONGREACH_ABILITY_ALL(pc);    
    }

    br_config->gain = 2;

    /* Initially configure for the preferred medium. */

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
    if (br_config->enable) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_BR);
    } else {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_BR);
    }

    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);

    /* Get Requested LED selectors (defaults are hardware defaults) */
    pc->ledmode[0] = soc_property_port_get(unit, port, spn_PHY_LED1_MODE, 0);
    pc->ledmode[1] = soc_property_port_get(unit, port, spn_PHY_LED2_MODE, 1);
    pc->ledmode[2] = soc_property_port_get(unit, port, spn_PHY_LED3_MODE, 3);
    pc->ledmode[3] = soc_property_port_get(unit, port, spn_PHY_LED4_MODE, 6);
    pc->ledctrl    = soc_property_port_get(unit, port, spn_PHY_LED_CTRL, 0x8);
    pc->ledselect  = soc_property_port_get(unit, port, spn_PHY_LED_SELECT, 0);

    /* Get half amplitude setting */
    /* pc->halfout = soc_property_port_get(unit, port, spn_PHY_HALF_POWER, 0); */
   
    if( PHY_IS_BCM54811(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_54811_reset_setup(unit, port));
        /* Disable BR by default */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54811_EXP_BR_LRE_MISC_CTRLr(unit, pc, 0x0, 0x1));
    } else { 
        if (!br_config->bypass_reset) {
            SOC_IF_ERROR_RETURN
                (_phy_54880_reset_setup(unit, port));
        }

        if (br_config->enable) {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL1r(unit, pc, 0x1, 0x1));
        } else {
            /* Disable BR by default */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL1r(unit, pc, 0x0, 0x1));
        }
    }
    if (PHY_IS_BCM54881(pc)) {
        /* Enabling half duplex in 54881 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_MISC_CTRLr(unit, pc, 1U<<7, 1U<<7));
    }

    /* Advertise all possible by default */
    SOC_IF_ERROR_RETURN
        (_phy_54880_ieee_ability_local_get(unit, port, &pc->copper.advert_ability));

    if (br_config->enable) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_medium_config_update(unit, port, br_config));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_medium_config_update(unit, port, &pc->copper)); 
    }

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54880_enable_set
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
phy_54880_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;
    uint16 power_down;

    pc = EXT_PHY_SW_STATE(unit, port);

    power_down = (enable) ? 0 : MII_CTRL_PD;

    if (pc->copper.enable) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_CTRLr(unit, pc, power_down, MII_CTRL_PD));
    }

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_enable_set(unit, port, enable));

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54880_enable_get
 * Purpose:
 *      Enable or disable the physical interface for a 54880 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54880_enable_get(int unit, soc_port_t port, int *enable)
{
    SOC_IF_ERROR_RETURN
        (phy_fe_ge_enable_get(unit, port, enable));

    return SOC_E_NONE;
}

STATIC int
_phy_54880_medium_check(int unit, soc_port_t port, int *medium)
{
    phy_ctrl_t   *pc;
    uint16    status;
    phy_br_config_t    *br_config;

    pc = EXT_PHY_SW_STATE(unit, port);

    br_config     = (phy_br_config_t *)  &pc->fiber;

    /* Force to br mode first to avoid locking up in ieee */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LDS_ACCr(unit, pc, 0x4));

    /* Force to auto mode */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LDS_ACCr(unit, pc, 0x0));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LDS_ACCr(unit, pc, &status));

    if ( status & LDS_ACC_STAT ) {
        *medium = SOC_PORT_MEDIUM_BR;
    } else {
        if (br_config->enable && !br_config->lds_enable) {
            *medium = SOC_PORT_MEDIUM_BR;
        } else {
            *medium = SOC_PORT_MEDIUM_IEEE;
        }
    }
    return SOC_E_NONE;
}


STATIC int
_phy_54880_medium_change(int unit, soc_port_t port, int force_update)
{
    phy_ctrl_t       *pc;
    int               medium = 0;
    soc_phy_config_t *copper_config;
    phy_br_config_t  *br_config;

    pc    = EXT_PHY_SW_STATE(unit, port);

    copper_config = (soc_phy_config_t *) &pc->copper;
    br_config     = (phy_br_config_t *)  &pc->fiber;

    SOC_IF_ERROR_RETURN
        (_phy_54880_medium_check(unit, port, &medium));

    if (medium == SOC_PORT_MEDIUM_IEEE) {
        if ((!PHY_IEEE_MODE(unit, port)) || force_update) { /* Was BR */
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_BR);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);

            /* Do not power up the interface if medium is disabled. */
            if (copper_config->enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_54880_ieee_medium_config_update(unit, port,copper_config));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_54880_link_auto_detect: u=%d p=%d [F->X]\n"),
                      unit, port));
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
        }
    } else {
        if (PHY_IEEE_MODE(unit, port) || force_update) { /* Was IEEE */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_BR);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);

            if (br_config->enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_54880_br_medium_config_update(unit, port,br_config));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_54880_link_auto_detect: u=%d p=%d [C->X]\n"),
                      unit, port));
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
        }
    }

    return SOC_E_NONE;
}

int
_phy_54880_br_link_get(int unit, soc_port_t port, int *link)
{
    uint16              lre_ctrl, lre_stat;
    soc_timeout_t       to;
    phy_ctrl_t          *pc;

    *link = FALSE;              /* Default */

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_STATr(unit, pc, &lre_stat));

    if (!(lre_stat & LRE_STAT_LA)) {
        return SOC_E_NONE;
    }

    /* Link appears to be up; we are done if autoneg is off. */

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));

    if (!(lre_ctrl & LRE_CTRL_LDSE)) {
        *link = TRUE;
        return SOC_E_NONE;
    }

    soc_timeout_init(&to, SOC_PHY_INFO(unit, port).an_timeout, 0);
    for (;;) {
        SOC_IF_ERROR_RETURN
            (READ_PHY54880_LRE_STATr(unit, pc, &lre_stat));

        if (!(lre_stat & LRE_STAT_LA)) {
            return SOC_E_NONE;
        }

        if (lre_stat & LRE_STAT_LDS_DONE) {
            break;
        }

        if (soc_timeout_check(&to)) {
            return SOC_E_BUSY;
        }
    }

    /* Return link state at end of polling */

    *link = ((lre_stat & LRE_STAT_LA) != 0);

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54880_link_get
 * Purpose:
 *      Determine the current link up/down status for a 54880 device.
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
phy_54880_link_get(int unit, soc_port_t port, int *link)
{

    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (_phy_54880_medium_change(unit, port,FALSE));

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
            (_SW2IEEE(unit, pc));
        SOC_IF_ERROR_RETURN
            (phy_fe_ge_link_get(unit, port, link));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_link_get(unit, port, link));
    }

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54880_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_XXX
 *      SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes:
 *      The duplex is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 *      Autonegotiation is not manipulated.
 */

STATIC int
_phy_54880_ieee_duplex_set(int unit, soc_port_t port, int duplex)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_duplex_set(unit, port, duplex));
    pc->copper.force_duplex = duplex;

    return SOC_E_NONE;
}

STATIC int
phy_54880_duplex_set(int unit, soc_port_t port, int duplex)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_duplex_set(unit, port, duplex));
    } else {
        if ( !duplex ) {
            return SOC_E_CONFIG;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_duplex_get
 * Purpose:
 *      Get the current operating duplex mode. If autoneg is enabled,
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The duplex is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_54880_duplex_get(int unit, soc_port_t port, int *duplex)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IEEE_MODE(unit, port)) {

        SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
            (_SW2IEEE(unit, pc));

        SOC_IF_ERROR_RETURN
            (phy_fe_ge_duplex_get(unit, port, duplex));
    } else {
        *duplex = TRUE;
    }

    return SOC_E_NONE;
}

int
_phy_54880_br_speed_set(int unit, soc_port_t port, int speed)
{
    uint16      lre_ctrl;
    phy_ctrl_t  *pc;

    if (speed == 0) {
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));

    lre_ctrl &= ~LRE_CTRL_SS_MASK;
    switch(speed) {
    case 10:
        lre_ctrl |= LRE_CTRL_SS_10;
        break;
    case 20:
        lre_ctrl |= LRE_CTRL_SS_20;
        break;
    case 25:
        lre_ctrl |= LRE_CTRL_SS_25;
        break;
    case 33:
        lre_ctrl |= LRE_CTRL_SS_33;
        break;
    case 50:
        lre_ctrl |= LRE_CTRL_SS_50;
        break;
    case 100:
        lre_ctrl |= LRE_CTRL_SS_100;
        break;
    default:
        return(SOC_E_CONFIG);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LRE_CTRLr(unit, pc, lre_ctrl));

    return(SOC_E_NONE);
}


/*
 * Function:
 *      phy_54880_ieee_speed_set
 * Purpose:
 *      Set the current operating speed (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Requested speed.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The speed is set only for the IEEE mode.
 */

STATIC int
_phy_54880_ieee_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_speed_set(unit, port, speed));

    return SOC_E_NONE;
}

STATIC int
phy_54880_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    phy_br_config_t    *br_config;

    pc = EXT_PHY_SW_STATE(unit, port);
    br_config     = (phy_br_config_t *)  &pc->fiber;

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_speed_set(unit, port, speed));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_speed_set(unit, port, speed));
        if (br_config->force_pairs == 0) {
            SOC_IF_ERROR_RETURN
                (_phy_54880_br_pairs_set(unit, port, (speed==100) ? 4 : 2));
        }
    }

    return SOC_E_NONE;
}



int
_phy_54880_br_speed_get(int unit, soc_port_t port, int *speed)
{
    int         rv;
    uint16      lre_ctrl, lre_stat, br_lds_scan;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));

    if (lre_ctrl & LRE_CTRL_LDSE) {       /* LDS enabled */
        SOC_IF_ERROR_RETURN
            (READ_PHY54880_LRE_STATr(unit, pc, &lre_stat));
        if (!(lre_stat & LRE_STAT_LDS_DONE)) { /* LDS NOT complete */
            *speed = 0;
            rv = SOC_E_NONE;
        } else {
        /* Examine the BR EXP regs */
            SOC_IF_ERROR_RETURN
                (READ_PHY54880_EXP_BR_LDS_SCANr(unit, pc, &br_lds_scan));

            switch(br_lds_scan & PHY54880_EXP_BR_LDS_SCAN_SS_MASK) {

            case PHY54880_EXP_BR_LDS_SCAN_SS_10:
                *speed = 10;
                break;
            case PHY54880_EXP_BR_LDS_SCAN_SS_20:
                *speed = 20;
                break;
            case PHY54880_EXP_BR_LDS_SCAN_SS_25:
                *speed = 25;
                break;
            case PHY54880_EXP_BR_LDS_SCAN_SS_33:
                *speed = 33;
                break;
            case PHY54880_EXP_BR_LDS_SCAN_SS_50:
                *speed = 50;
                break;
            case PHY54880_EXP_BR_LDS_SCAN_SS_100:
                *speed = 100;
                break;
            default:
                *speed = 0;
                break;
            }
            rv = SOC_E_NONE;
        }
    } else {                            /* LDS disabled */
        /*
         * Simply pick up the values we force in CTRL register.
         */
        switch(lre_ctrl & LRE_CTRL_SS_MASK) {

        case LRE_CTRL_SS_10:
            *speed = 10;
            break;
        case LRE_CTRL_SS_20:
            *speed = 20;
            break;
        case LRE_CTRL_SS_25:
            *speed = 25;
            break;
        case LRE_CTRL_SS_33:
            *speed = 33;
            break;
        case LRE_CTRL_SS_50:
            *speed = 50;
            break;
        case LRE_CTRL_SS_100:
            *speed = 100;
            break;
        default:
            *speed = 0;
            break;
        }
        rv = SOC_E_NONE;
    }

    return(rv);
}

/*
 * Function:
 *      phy_54880_speed_get
 * Purpose:
 *      Get the current operating speed for a 54880 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The speed is retrieved for the ACTIVE medium.
 */

STATIC int
phy_54880_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IEEE_MODE(unit, port)) {

        SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
            (_SW2IEEE(unit, pc));

        SOC_IF_ERROR_RETURN(phy_fe_ge_speed_get(unit, port, speed));
    } else {
        SOC_IF_ERROR_RETURN(_phy_54880_br_speed_get(unit, port, speed));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_pairs_set(int unit, soc_port_t port, int pairs)
{
    uint16      lre_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));

    lre_ctrl &= ~LRE_CTRL_PS_MASK;
    switch(pairs) {
    case 1:
        lre_ctrl |= LRE_CTRL_PS_1P;
        break;
    case 2:
        lre_ctrl |= LRE_CTRL_PS_2P;
        break;
    case 4:
        lre_ctrl |= LRE_CTRL_PS_4P;
        break;
    default:
        return(SOC_E_CONFIG);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LRE_CTRLr(unit, pc, lre_ctrl));

    return(SOC_E_NONE);
}

STATIC int
_phy_54880_br_pairs_get(int unit, soc_port_t port, int *pairs)
{
    int         rv;
    uint16      lre_ctrl, lre_stat, lds_exp;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_STATr(unit, pc, &lre_stat));

    if (lre_ctrl & LRE_CTRL_LDSE) {       /* LDS enabled */
        if (!(lre_stat & LRE_STAT_LDS_DONE)) { /* LDS NOT complete */
            *pairs = 0;
            rv = SOC_E_NONE;
        } else {
        /* Examine the LDS EXP reg (0xa) */
            SOC_IF_ERROR_RETURN
                (READ_PHY54880_LDS_EXPr(unit, pc, &lds_exp));

            switch(lds_exp & (0x3U<<12)) {
            case 0x0:
                *pairs = 1;
                break;
            case (0x1<<12):
                *pairs = 2;
                break;
            case (0x2<<12):
                *pairs = 4;
                break;
            default:
                *pairs = 0;
                break;
            }
            rv = SOC_E_NONE;
        }
    } else {                            /* LDS disabled */

        switch(lre_ctrl & LRE_CTRL_PS_MASK) {
        case LRE_CTRL_PS_1P:
            *pairs = 1;
            break;
        case LRE_CTRL_PS_2P:
            *pairs = 2;
            break;
        case LRE_CTRL_PS_4P:
            *pairs = 4;
            break;
        default:
            *pairs = 0;
            break;
        }
        rv = SOC_E_NONE;
    }

    return(rv);
}

STATIC int
_phy_54880_br_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if( PHY_IS_BCM54811(pc) ) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54811_EXP_BR_LRE_MISC_CTRLr(unit, pc, (enable) ? (1U<<0):0, (1U<<0)));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL1r(unit, pc, (enable) ? (1U<<0):0, (1U<<0)));
    }

    return SOC_E_NONE;

}

STATIC int
_phy_54880_br_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t    *pc;
    uint16 lre_misc_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);

    if( PHY_IS_BCM54811(pc)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY54811_EXP_BR_LRE_MISC_CTRLr(unit, pc, &lre_misc_ctrl));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY54880_EXP_BR_LRE_MISC_CTRL1r(unit, pc, &lre_misc_ctrl));
    }

    *enable = (lre_misc_ctrl & (1U<<0)) ? TRUE: FALSE;

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_active_get(int unit, soc_port_t port, int *active)
{
    *active = (PHY_IEEE_MODE(unit, port)) ? FALSE: TRUE;

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_gain_set(int unit, soc_port_t port, int gain)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch(gain) {
    case 0:
    case 1:
    case 2:
    case 4:
        if( PHY_IS_BCM54811(pc) ) {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54811_EXP_BR_LRE_MISC_CTRL2r(unit, pc, (gain&0x7)<<9, 0x7<<9));
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL2r(unit, pc, (gain&0x7)<<9, 0x7<<9));
        }
        break;
    default:
        return(SOC_E_CONFIG);
    }

    return(SOC_E_NONE);
}

STATIC int
_phy_54880_br_gain_get(int unit, soc_port_t port, int *gain)
{
    phy_ctrl_t  *pc;
    uint16 temp;

    pc = EXT_PHY_SW_STATE(unit, port);

    if( PHY_IS_BCM54811(pc) ) {
        SOC_IF_ERROR_RETURN
            (READ_PHY54811_EXP_BR_LRE_MISC_CTRL2r(unit, pc, &temp));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY54880_EXP_BR_LRE_MISC_CTRL2r(unit, pc, &temp));
    }

    *gain = (temp>>9) & 0x7;

    return(SOC_E_NONE);
}


/*
 * Function:
 *      phy_54880_master_set
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
_phy_54880_br_master_set(int unit, soc_port_t port, int master)
{
    uint16      lre_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));

    switch (master) {
    case SOC_PORT_MS_SLAVE:
        lre_ctrl &= ~LRE_CTRL_MS;
        break;
    case SOC_PORT_MS_MASTER:
        lre_ctrl |= LRE_CTRL_MS;
        break;
    case SOC_PORT_MS_AUTO:
        return(SOC_E_NONE);
    default:
        return(SOC_E_CONFIG);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54880_LRE_CTRLr(unit, pc, lre_ctrl));

    return(SOC_E_NONE);
}


STATIC int
_phy_54880_ieee_master_set(int unit, soc_port_t port, int master)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_master_set(unit, port, master));

    pc->copper.master = master;

    return SOC_E_NONE;
}

STATIC int
phy_54880_master_set(int unit, soc_port_t port, int master)
{
    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_master_set(unit, port, master));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_master_set(unit, port, master));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54880_master_get
 * Purpose:
 *      Get the current master mode for a 54880 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */
STATIC int
_phy_54880_br_master_get(int unit, soc_port_t port, int *master)
{
    uint16      lre_ctrl, lre_stat, lds_exp;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));

    if (lre_ctrl & LRE_CTRL_LDSE) {
        SOC_IF_ERROR_RETURN
            (READ_PHY54880_LRE_STATr(unit, pc, &lre_stat));
        if (!(lre_stat & LRE_STAT_LDS_DONE)) { /* LDS NOT complete */
            *master = SOC_PORT_MS_NONE;
        } else {
            /* Examine the LDS EXP reg (0xa) */
            SOC_IF_ERROR_RETURN
                (READ_PHY54880_LDS_EXPr(unit, pc, &lds_exp));
            *master = (lds_exp & (1U<<14)) ? SOC_PORT_MS_MASTER : SOC_PORT_MS_SLAVE;
        }
    } else if (lre_ctrl & LRE_CTRL_MS) {
        *master = SOC_PORT_MS_MASTER;
    } else {
        *master = SOC_PORT_MS_SLAVE;
    }

    return SOC_E_NONE;
}

STATIC int
phy_54880_master_get(int unit, soc_port_t port, int *master)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IEEE_MODE(unit, port)) {

        SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
            (_SW2IEEE(unit, pc));

        SOC_IF_ERROR_RETURN
            (phy_fe_ge_master_get(unit, port, master));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_master_get(unit, port, master));
    }

    return SOC_E_NONE;
}


STATIC int
_phy_54880_br_autoneg_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_LRE_CTRLr(unit, pc,
        an ? LRE_CTRL_LDSE|LRE_CTRL_RLDS : 0,
        LRE_CTRL_LDSE|LRE_CTRL_RLDS));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_autoneg_set
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
_phy_54880_ieee_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    int           rv;
    phy_ctrl_t    *pc;

    pc             = EXT_PHY_SW_STATE(unit, port);
    rv             = SOC_E_NONE;

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    /* Set auto-neg on PHY */
    rv = phy_fe_ge_an_set(unit, port, autoneg);
    if (SOC_SUCCESS(rv)) {
        pc->copper.autoneg_enable = autoneg ? 1 : 0;
    }

    return rv;
}

STATIC int
phy_54880_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    int rv  = SOC_E_NONE;

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_autoneg_set(unit, port, autoneg));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_autoneg_set(unit, port, autoneg));
    }

    return rv;
}

STATIC int
_phy_54880_br_autoneg_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16 lre_ctrl, lre_stat;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_STATr(unit, pc, &lre_stat));

    *an = (lre_ctrl & LRE_CTRL_LDSE) ? TRUE : FALSE;

    if ( an_done ) {
        *an_done = (lre_stat & LRE_STAT_LDS_DONE) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_autoneg_get
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
phy_54880_autoneg_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IEEE_MODE(unit, port)) {

        SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
            (_SW2IEEE(unit, pc));

        SOC_IF_ERROR_RETURN
            (phy_fe_ge_an_get(unit, port, autoneg, autoneg_done));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_autoneg_get(unit, port, autoneg, autoneg_done));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54880_ieee_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_ability_advert_set(unit, port, ability));

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_ability_advert_set(int unit, soc_port_t port, uint32 ability)
{
    phy_ctrl_t      *pc;
    uint16       lds_adv;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    ability &= LONGREACH_ABILITY_ALL(pc);

    lds_adv = 0;

    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR)  {
        lds_adv |= LDS_ADVA_1_10;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR)  {
        lds_adv |= LDS_ADVA_2_10;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR)  {
        lds_adv |= LDS_ADVA_1_20;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR)  {
        lds_adv |= LDS_ADVA_2_20;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_1PAIR)  {
        lds_adv |= LDS_ADVA_1_25;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_2PAIR)  {
        lds_adv |= LDS_ADVA_2_25;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR)  {
        lds_adv |= LDS_ADVA_1_33;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR)  {
        lds_adv |= LDS_ADVA_2_33;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR)  {
        lds_adv |= LDS_ADVA_1_50;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR)  {
        lds_adv |= LDS_ADVA_2_50;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR) {
        lds_adv |= LDS_ADVA_1_100;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR) {
        lds_adv |= LDS_ADVA_2_100;
    }
    if (ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR) {
        lds_adv |= LDS_ADVA_4_100;
    }

    switch (ability & (SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX | 
                       SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX)) {

    case SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX:
        lds_adv |= LDS_ADVA_ASYM_PAUSE;
        break;
    case SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX:
        lds_adv |= LDS_ADVA_PAUSE | LDS_ADVA_ASYM_PAUSE;
        break;
    case SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX |
         SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX:
        lds_adv |= LDS_ADVA_PAUSE;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_LDS_ADVAr(unit, pc, lds_adv, LDS_ADVA_MASK(pc)));

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_ability_advert_std_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    uint32 adv_ability = 0;

    if (ability->speed_full_duplex & SOC_PA_SPEED_10MB)  {
        adv_ability |= LONGREACH_ABILITY_10M;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_100MB)  {
        adv_ability |= LONGREACH_ABILITY_100M;
    }

    if (ability->pause & SOC_PA_PAUSE_TX)  {
        adv_ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX;
    }
    if (ability->pause & SOC_PA_PAUSE_RX)  {
        adv_ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX;
    }

    SOC_IF_ERROR_RETURN
        (_phy_54880_br_ability_advert_set(unit, port, adv_ability));

    return SOC_E_NONE;
}

STATIC int
phy_54880_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_ability_advert_set(unit, port, ability));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_ability_advert_std_set(unit, port, ability));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54880_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
_phy_54880_ieee_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_ability_advert_get(unit, port, ability));

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_ability_advert_get(int unit, soc_port_t port, uint32 *ability)
{
    phy_ctrl_t *pc;
    uint16      lds_adv;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LDS_ADVAr(unit, pc, &lds_adv));

    *ability = 0;

    if (lds_adv & LDS_ADVA_1_10) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_10) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_20) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_20) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_25) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_25) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_33) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_33) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_50) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_50) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_100) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_100) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_4_100) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR;
    }

    switch (lds_adv & (LDS_ADVA_PAUSE | LDS_ADVA_ASYM_PAUSE)) {
    case LDS_ADVA_PAUSE:
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX | 
                     SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX;
        break;
    case LDS_ADVA_ASYM_PAUSE:
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX;
        break;
    case LDS_ADVA_PAUSE | LDS_ADVA_ASYM_PAUSE:
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX;
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_ability_advert_std_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    uint32 adv_ability;

    sal_memset(ability, 0, sizeof(*ability));
    adv_ability = 0;

    SOC_IF_ERROR_RETURN
        (_phy_54880_br_ability_advert_get(unit, port, &adv_ability));

    if (adv_ability & LONGREACH_ABILITY_10M) {
      ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if (adv_ability & LONGREACH_ABILITY_100M) {
      ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }

    if (adv_ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX) {
      ability->pause |= SOC_PA_PAUSE_TX;
    }

    if (adv_ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX) {
      ability->pause |= SOC_PA_PAUSE_RX;
    }

    return SOC_E_NONE;
}


STATIC int
phy_54880_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_ability_advert_get(unit, port, ability));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_ability_advert_std_get(unit, port, ability));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_ability_remote_get
 * Purpose:
 *      Get partners current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remote advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. If Autonegotiation is
 *      disabled or in progress, this routine will return an error.
 */

STATIC int
_phy_54880_ieee_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t       *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_ability_remote_get(unit, port, ability));

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_ability_remote_get(int unit, soc_port_t port, uint32 *ability)
{
    phy_ctrl_t *pc;
    uint16      lds_adv;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LDS_LPABr(unit, pc, &lds_adv));

    *ability = 0;

    if (lds_adv & LDS_ADVA_1_10) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_10) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_20) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_20) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_25) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_25) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_33) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_33) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_50) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_50) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_1_100) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR;
    }
    if (lds_adv & LDS_ADVA_2_100) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR;
    }
    if (lds_adv & LDS_ADVA_4_100) {
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR;
    }

    switch (lds_adv & (LDS_ADVA_PAUSE | LDS_ADVA_ASYM_PAUSE)) {
    case LDS_ADVA_PAUSE:
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX | 
                     SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX;
        break;
    case LDS_ADVA_ASYM_PAUSE:
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX;
        break;
    case LDS_ADVA_PAUSE | LDS_ADVA_ASYM_PAUSE:
        *ability |= SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX;
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_ability_remote_std_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{

    uint32 rem_ability;

    sal_memset(ability, 0, sizeof(*ability));
    rem_ability = 0;

    SOC_IF_ERROR_RETURN
        (_phy_54880_br_ability_remote_get(unit, port, &rem_ability));

    if (rem_ability & LONGREACH_ABILITY_10M) {
      ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if (rem_ability & LONGREACH_ABILITY_100M) {
      ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }

    if (rem_ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX) {
      ability->pause |= SOC_PA_PAUSE_TX;
    }

    if (rem_ability & SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX) {
      ability->pause |= SOC_PA_PAUSE_RX;
    }

    return SOC_E_NONE;

}

STATIC int
phy_54880_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_ability_remote_get(unit, port, ability));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_ability_remote_std_get(unit, port, ability));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_54880_ieee_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM54881(pc)) {
        ability->speed_full_duplex  = SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
    } else {
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB |SOC_PA_SPEED_100MB | 
                                      SOC_PA_SPEED_10MB;
    }
    ability->speed_half_duplex  = SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_SGMII;
    ability->medium    = SOC_PA_MEDIUM_IEEE; 
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
#if 1
    ability->speed_full_duplex  = SOC_PA_SPEED_100MB |SOC_PA_SPEED_10MB; 
#endif
    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_SGMII;
    ability->medium    = SOC_PA_MEDIUM_IEEE; 
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_ability_local_get
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
phy_54880_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_ability_local_get(unit, port, ability));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_ability_local_get(unit, port, ability));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_ability_current_get(int unit, soc_port_t port, uint32 *ability)
{
    uint16      lre_ctrl, lre_stat;
    phy_ctrl_t  *pc;
    uint32 ability_local, ability_remote;
    int speed, pairs;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));

    if (lre_ctrl & LRE_CTRL_LDSE) {
        SOC_IF_ERROR_RETURN
            (READ_PHY54880_LRE_STATr(unit, pc, &lre_stat));
        if (!(lre_stat & LRE_STAT_LDS_DONE)) { /* LDS NOT complete */
            *ability = 0;
        } else {
            ability_local = 0;
            ability_remote = 0;

            SOC_IF_ERROR_RETURN
                (_phy_54880_br_ability_advert_get(unit, port, &ability_local));
            SOC_IF_ERROR_RETURN
                (_phy_54880_br_ability_remote_get(unit, port, &ability_remote));
            SOC_IF_ERROR_RETURN
                (_phy_54880_br_speed_get(unit, port, &speed));
            SOC_IF_ERROR_RETURN
                (_phy_54880_br_pairs_get(unit, port, &pairs));
            *ability = (ability_local & ability_remote & (3U<<13)) |
                       (((speed == 100) ? (1U << 10): 
                         (speed == 50)  ? (1U << 8): 
                         (speed == 33)  ? (1U << 6): 
                         (speed == 25)  ? (1U << 4): 
                         (speed == 20)  ? (1U << 2): 
                         (speed == 10)  ? (1U << 0):0) 
                         << (((unsigned) pairs) >> 1));
        }
    } else {
        *ability = 0;
    }


    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_lb_set
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
_phy_54880_ieee_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

#if AUTO_MDIX_WHEN_AN_DIS
        {
        uint16          tmp;

        /* Disable Auto-MDIX When autoneg disabled */
        /* Enable Auto-MDIX When autoneg disabled */
        tmp = (enable) ? 0x0000: 0x0200;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_MISC_CTRLr(unit, pc, tmp, 0x0200));
        }
#endif
    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_lb_set(unit, port, enable));

    return SOC_E_NONE;
}

STATIC int
_phy_54880_br_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54880_LRE_CTRLr(unit, pc, (enable ? MII_CTRL_LE : 0), MII_CTRL_LE));

    return SOC_E_NONE;
}

STATIC int
phy_54880_lb_set(int unit, soc_port_t port, int enable)
{
    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_lb_set(unit, port, enable));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_lb_set(unit, port, enable));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_lb_get
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
_phy_54880_br_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      lre_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2BR(unit, pc));

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_LRE_CTRLr(unit, pc, &lre_ctrl));

    *enable = (lre_ctrl & MII_CTRL_LE) ? 1 : 0;

    return SOC_E_NONE;
}

STATIC int
phy_54880_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IEEE_MODE(unit, port)) {

        SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
            (_SW2IEEE(unit, pc));

        SOC_IF_ERROR_RETURN
            (phy_fe_ge_lb_get(unit, port, enable));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_lb_get(unit, port, enable));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_interface_set
 * Purpose:
 *      Set the current operating mode of the internal PHY.
 *      (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54880_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(pif);

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_54880_interface_get
 * Purpose:
 *      Get the current operating mode of the internal PHY.
 *      (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54880_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM54881(pc)) {
        *pif = SOC_PORT_IF_MII;
    } else {
        *pif = SOC_PORT_IF_SGMII;
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_54880_mdix_set
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
_phy_54880_ieee_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    switch (mode) {
    case SOC_PORT_MDIX_AUTO:
        /* Clear bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_ECRr(unit, pc, 0, 0x4000));

        /*
         * Write the result in the register 0x18, shadow copy 7
         */
        /* Clear bit 9 to disable forced auto MDI xover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_MISC_CTRLr(unit, pc, 0, 0x0200));
        break;

    case SOC_PORT_MDIX_FORCE_AUTO:
        /* Clear bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_ECRr(unit, pc, 0, 0x4000));

        /*
         * Write the result in the register 0x18, shadow copy 7
         */
        /* Set bit 9 to force automatic MDI crossover */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_MISC_CTRLr(unit, pc, 0x0200, 0x0200));
        break;

    case SOC_PORT_MDIX_NORMAL:
        SOC_IF_ERROR_RETURN(phy_54880_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
            /* Set bit 14 for manual MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54880_MII_ECRr(unit, pc, 0x4000, 0x4000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY54880_TEST1r(unit, pc, 0));
        } else {
            return SOC_E_UNAVAIL;
        }
        break;

    case SOC_PORT_MDIX_XOVER:
        SOC_IF_ERROR_RETURN(phy_54880_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
             /* Set bit 14 for manual MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54880_MII_ECRr(unit, pc, 0x4000, 0x4000));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY54880_TEST1r(unit, pc, 0x0080));
        } else {
            return SOC_E_UNAVAIL;
        }
        break;

    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;

}        

STATIC int
phy_54880_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_mdix_set(unit, port, mode));
        pc->copper.mdix = mode;
    } else {
        return(SOC_E_CONFIG);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_mdix_get
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
_phy_54880_ieee_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_NOT_DISABLED_RETURN_ERROR
        (_SW2IEEE(unit, pc));

    SOC_IF_ERROR_RETURN(phy_54880_speed_get(unit, port, &speed));
    if (speed == 1000) {
       *mode = SOC_PORT_MDIX_AUTO;
    } else {
        *mode = pc->copper.mdix;
    }

    return SOC_E_NONE;
}    

STATIC int
phy_54880_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    if (PHY_IEEE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_54880_ieee_mdix_get(unit, port, mode));
    } else {
        *mode = SOC_PORT_MDIX_AUTO;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_mdix_status_get
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
phy_54880_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    phy_ctrl_t    *pc;
    uint16               tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY54880_MII_ESRr(unit, pc, &tmp));
    if (tmp & 0x2000) {
        *status = SOC_PORT_MDIX_STATUS_XOVER;
    } else {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
    }

    return SOC_E_NONE;
}    

STATIC int
phy_54880_link_up(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    int pairs, master;
    int speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_54880_speed_get(unit, port, &speed));
    
    if (PHY_IS_B0(pc)) {
        if (PHY_BR_MODE(unit, port)) {
            /* B0 errata */
            SOC_IF_ERROR_RETURN
                (_phy_54880_br_pairs_get(unit, port, &pairs));
            SOC_IF_ERROR_RETURN
                (_phy_54880_br_master_get(unit, port, &master));

            switch(pairs) {
            case 4:
                if (master == SOC_PORT_MS_MASTER) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL3r(unit, pc, 1U<<15, 1U<<15));
                }
                break;

            case 2:
            case 1:
                SOC_IF_ERROR_RETURN
                    (PHY54880_REG_WRITE(unit, pc, 0x00, 0x0ffd, 0x15, 0x0038));
                SOC_IF_ERROR_RETURN
                    (PHY54880_REG_WRITE(unit, pc, 0x00, 0x0ffe, 0x15, 0x0038));
                break;

            default:
                break;
            }
        }

    }

    /* Notify interface to internal PHY */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));

    /* Situation observed with Saber devices XGXS16g1l+BCM54640E
       Linkscan is not updating speed for internal phy xgxs16g1l. 
       It was observed that when speed is set to 100M the int phy 
       is still set to 1G and traffic does not flow through untill 
       int phy speed is also update to 100M. This applies to other 
       speeds as well. 
       Explicitly send speed notify to int phy from ext phy.
    */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventSpeed, speed));

    return SOC_E_NONE;
}

STATIC int
phy_54880_link_down(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_B0(pc)) {
        if (PHY_BR_MODE(unit, port)) {
            /* B0 errata */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL3r(unit, pc, 0, 1U<<15));
            SOC_IF_ERROR_RETURN
                (PHY54880_REG_WRITE(unit, pc, 0x00, 0x0ffd, 0x15, 0));
            SOC_IF_ERROR_RETURN
                (PHY54880_REG_WRITE(unit, pc, 0x00, 0x0ffe, 0x15, 0));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_54880_power_mode_set (int unit, soc_port_t port, int mode)
{
    phy_ctrl_t    *pc;

    pc       = EXT_PHY_SW_STATE(unit, port);

    if (pc->power_mode == mode) {
        return SOC_E_NONE;
    }

    if (mode == SOC_PHY_CONTROL_POWER_LOW) {
        /* enable dsp clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_AUX_CTRLr(unit,pc,0x0c00,0x0c00));

        /* enable low power 136 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_AUTO_POWER_DOWNr(unit,pc,0x80,0x80));

        /* reduce tx bias current to -20% */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x17, 0x0f75));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x15, 0x1555));

        /* disable dsp clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_AUX_CTRLr(unit,pc,0x0400,0x0c00));
        pc->power_mode = mode;

    } else if (mode == SOC_PHY_CONTROL_POWER_FULL) {

        /* back to normal mode */
        /* enable dsp clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_AUX_CTRLr(unit,pc,0x0c00,0x0c00));

        /* disable low power 136 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_AUTO_POWER_DOWNr(unit,pc,0x00,0x80));

        /* set tx bias current to nominal */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x17, 0x0f75));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x15, 0x0));

        /* disable dsp clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_MII_AUX_CTRLr(unit,pc,0x0400,0x0c00));

        /* disable the auto power mode */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_AUTO_POWER_DOWNr(unit,pc,
                        0,
                        PHY_54880_AUTO_PWRDWN_EN));
        pc->power_mode = mode;
    } else if (mode == SOC_PHY_CONTROL_POWER_AUTO) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_AUTO_POWER_DOWNr(unit,pc,
                        PHY_54880_AUTO_PWRDWN_EN,
                        PHY_54880_AUTO_PWRDWN_EN));
        pc->power_mode = mode;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54880_control_set
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
phy_54880_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;
    phy_br_config_t  *br_config;
    uint16 data;
    soc_port_t primary;
    int offset;

    PHY_CONTROL_TYPE_CHECK(type);

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    br_config     = (phy_br_config_t *)  &pc->fiber;

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_POWER:
        rv = _phy_54880_power_mode_set(unit,port,value);
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
        if (value <= PHY_54880_AUTO_PWRDWN_WAKEUP_MAX) {

            /* at least one unit */
            if (value < PHY_54880_AUTO_PWRDWN_WAKEUP_UNIT) {
                value = PHY_54880_AUTO_PWRDWN_WAKEUP_UNIT;
            }
        } else { /* anything more then max, set to the max */
            value = PHY_54880_AUTO_PWRDWN_WAKEUP_MAX;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_AUTO_POWER_DOWNr(unit,pc,
                      value/PHY_54880_AUTO_PWRDWN_WAKEUP_UNIT,
                      PHY_54880_AUTO_PWRDWN_WAKEUP_MASK));
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
        /* sleep time configuration is either 2.7s or 5.4 s, default is 2.7s */
        if (value < PHY_54880_AUTO_PWRDWN_SLEEP_MAX) {
            data = 0; /* anything less than 5.4s, default to 2.7s */
        } else {
            data = PHY_54880_AUTO_PWRDWN_SLEEP_MASK;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54880_AUTO_POWER_DOWNr(unit,pc,
                      data,
                      PHY_54880_AUTO_PWRDWN_SLEEP_MASK));
        break;

    case SOC_PHY_CONTROL_LONGREACH_SPEED:
        rv = _phy_54880_br_speed_set(unit, port, value);
        br_config->force_speed = value;
        break;

    case SOC_PHY_CONTROL_LONGREACH_PAIRS:
        rv = _phy_54880_br_pairs_set(unit, port, value);
        br_config->force_pairs = value;
        break;

    case SOC_PHY_CONTROL_LONGREACH_GAIN:
        rv = _phy_54880_br_gain_set(unit, port, value);
        br_config->gain = value;
        break;

    case SOC_PHY_CONTROL_LONGREACH_AUTONEG:
        rv = _phy_54880_br_autoneg_set(unit, port, value);
        br_config->lds_enable = value;
        break;

    case SOC_PHY_CONTROL_LONGREACH_LOCAL_ABILITY:
        rv = _phy_54880_br_ability_advert_set(unit, port, value);
        SOC_IF_ERROR_RETURN
            (_phy_54880_br_autoneg_set(unit, port, br_config->lds_enable));
        br_config->lds_advert_ability = value & LONGREACH_ABILITY_ALL(pc);
        break;

    case SOC_PHY_CONTROL_LONGREACH_MASTER:
        rv = _phy_54880_br_master_set(unit, port, value);
        br_config->master = value;
        break;

    case SOC_PHY_CONTROL_LONGREACH_ENABLE:
        rv = _phy_54880_br_enable_set(unit, port, value);
        br_config->enable = value;
        break;

    case SOC_PHY_CONTROL_LONGREACH_REMOTE_ABILITY:
    case SOC_PHY_CONTROL_LONGREACH_CURRENT_ABILITY:
    case SOC_PHY_CONTROL_LONGREACH_ACTIVE:
        /* Do nothing for the read only commands */
        rv = SOC_E_CONFIG;
        break;

    case SOC_PHY_CONTROL_PORT_PRIMARY:
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_primary_set(unit, port, (soc_port_t)value));
        break;

        case SOC_PHY_CONTROL_PORT_OFFSET:
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_offset_set(unit, port, (int)value));
        break;

    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_primary_get(unit, port, &primary));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_toplvl_reg_read(unit, port, primary, 0x0, &data));
        if ( value ) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_toplvl_reg_write(unit, port, primary, 0x0,
                     ( data & 0xf0 ) | ( offset & 0x7 )));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_toplvl_reg_write(unit, port, primary, 0x0,
                     ( data & 0xf0 ) | 0x8 ));
        }
        break;

    case SOC_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_primary_get(unit, port, &primary));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_toplvl_reg_read(unit, port, primary, 0x0, &data));
        if ( value ) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_toplvl_reg_write(unit, port, primary, 0x0,
                     ( data & 0x0f ) | (( offset & 0x7 )<<4 )));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_toplvl_reg_write(unit, port, primary, 0x0,
                     ( data & 0x0f ) | (0x8<<4) ));
        }
        break;

    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        /* Not supported for 10/100/1000BASE-T interfaces */
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
 *      phy_54880_control_get
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
phy_54880_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t *pc;
    int temp;
    uint16 data;
    soc_port_t primary;
    int offset;

    PHY_CONTROL_TYPE_CHECK(type);

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    temp = 0;

    switch(type) {

    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_POWER:
        *value = pc->power_mode;
        rv = SOC_E_NONE;
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
        SOC_IF_ERROR_RETURN
            (READ_PHY54880_AUTO_POWER_DOWNr(unit,pc, &data));

        if (data & PHY_54880_AUTO_PWRDWN_SLEEP_MASK) {
            *value = PHY_54880_AUTO_PWRDWN_SLEEP_MAX;
        } else {
            *value = 2700;
        }
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
        SOC_IF_ERROR_RETURN
            (READ_PHY54880_AUTO_POWER_DOWNr(unit,pc, &data));

        data &= PHY_54880_AUTO_PWRDWN_WAKEUP_MASK;
        *value = data * PHY_54880_AUTO_PWRDWN_WAKEUP_UNIT;
        break;

    case SOC_PHY_CONTROL_LONGREACH_SPEED:
        rv = _phy_54880_br_speed_get(unit, port, &temp);
        *value = (uint32)temp;
        break;

    case SOC_PHY_CONTROL_LONGREACH_PAIRS:
        rv = _phy_54880_br_pairs_get(unit, port, &temp);
        *value = (uint32)temp;
        break;

    case SOC_PHY_CONTROL_LONGREACH_GAIN:
        rv = _phy_54880_br_gain_get(unit, port, &temp);
        *value = (uint32)temp;
        break;

    case SOC_PHY_CONTROL_LONGREACH_AUTONEG:
        rv = _phy_54880_br_autoneg_get(unit, port, &temp, NULL);
        *value = (uint32)temp;
        break;

    case SOC_PHY_CONTROL_LONGREACH_LOCAL_ABILITY:
        rv = _phy_54880_br_ability_advert_get(unit, port, value);
        break;

    case SOC_PHY_CONTROL_LONGREACH_REMOTE_ABILITY:
        rv = _phy_54880_br_ability_remote_get(unit, port, value);
        break;

    case SOC_PHY_CONTROL_LONGREACH_CURRENT_ABILITY:
        rv = _phy_54880_br_ability_current_get(unit, port, value);
        break;

    case SOC_PHY_CONTROL_LONGREACH_MASTER:
        rv = _phy_54880_br_master_get(unit, port, &temp);
        *value = (uint32)temp;
        break;

    case SOC_PHY_CONTROL_LONGREACH_ACTIVE:
        rv = _phy_54880_br_active_get(unit, port, &temp);
        *value = (uint32)temp;
        break;

    case SOC_PHY_CONTROL_LONGREACH_ENABLE:
        rv = _phy_54880_br_enable_get(unit, port, &temp);
        *value = (uint32)temp;
        break;

    case SOC_PHY_CONTROL_PORT_PRIMARY:
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_primary_get(unit, port, &primary));
            *value = (uint32) primary;
        break;

    case SOC_PHY_CONTROL_PORT_OFFSET:
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_offset_get(unit, port, &offset));
            *value = (uint32) offset;
        break;

    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_primary_get(unit, port, &primary));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        if (soc_phyctrl_toplvl_reg_read(unit, port, primary, 0x0, &data) == SOC_E_NONE) {
            if (( data & 0x8 ) || (( data & 0x7 ) != offset)) {
                *value = FALSE;
            } else {
                *value = TRUE;
            }
        }
        break;

    case SOC_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_primary_get(unit, port, &primary));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        if (soc_phyctrl_toplvl_reg_read(unit, port, primary, 0x0, &data) == SOC_E_NONE) {
            if (( data & (0x8<<4) ) || (( data & (0x7<<4) ) != (offset<<4) )) {
                *value = FALSE;
            } else {
                *value = TRUE;
            }
        }
        break;

    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        /* Not supported for 10/100/1000BASE-T interfaces */
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
 *      phy_54880_cable_diag
 * Purpose:
 *      Run 546x cable diagnostics
 * Parameters:
 *      unit - device number
 *      port - port number
 *      status - (OUT) cable diagnotic status structure
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_54880_cable_diag(int unit, soc_port_t port,
                    soc_port_cable_diag_t *status)
{
    int                 rv, rv2, i;

    extern int phy_5464_cable_diag_sw(int, soc_port_t ,
                                      soc_port_cable_diag_t *);

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    status->state = SOC_PORT_CABLE_STATE_OK;
    status->npairs = 4;
    status->fuzz_len = 0;
    for (i = 0; i < 4; i++) {
        status->pair_state[i] = SOC_PORT_CABLE_STATE_OK;
    }

    MIIM_LOCK(unit);    /* this locks out linkscan, essentially */
    rv = phy_5464_cable_diag_sw(unit,port, status);
    MIIM_UNLOCK(unit);
    rv2 = 0;
    if (rv <= 0) {      /* don't reset if > 0 -- link was up */
        rv2 = _phy_54880_reset_setup(unit, port);
    }
    if (rv >= 0 && rv2 < 0) {
        return rv2;
    }
    return rv;
}

STATIC int
phy_54880_cable_diag_dispatch(int unit, soc_port_t port,
            soc_port_cable_diag_t *status)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ((PHY_IS_BCM54880(pc)) && ((pc->phy_rev & 0xf) > 0x1))  {
        SOC_IF_ERROR_RETURN(
            phy_ecd_cable_diag(unit, port, status));
    } else {
        SOC_IF_ERROR_RETURN(
            phy_54880_cable_diag(unit, port, status));
    }
    SOC_IF_ERROR_RETURN(
        _phy_54880_reset_setup(unit, port));

    return SOC_E_NONE;
}

/*
 * Variable:    phy_54880drv_ge
 * Purpose:     PHY driver for 54880
 */

phy_driver_t phy_54880drv_ge = {
    "54880 Gigabit PHY Driver",
    phy_54880_init,
    phy_fe_ge_reset,
    phy_54880_link_get,
    phy_54880_enable_set,
    phy_54880_enable_get,
    phy_54880_duplex_set,
    phy_54880_duplex_get,
    phy_54880_speed_set,
    phy_54880_speed_get,
    phy_54880_master_set,
    phy_54880_master_get,
    phy_54880_autoneg_set,
    phy_54880_autoneg_get,
    NULL,
    NULL,
    NULL,
    phy_54880_lb_set,
    phy_54880_lb_get,
    phy_54880_interface_set,
    phy_54880_interface_get,
    NULL,                       /* Deprecated */
    phy_54880_link_up,          /* Link up event */
    phy_54880_link_down,        /* Link down event */
    phy_54880_mdix_set,
    phy_54880_mdix_get,
    phy_54880_mdix_status_get,
    phy_54880_medium_config_set,
    phy_54880_medium_config_get,
    phy_54880_medium_status,
    phy_54880_cable_diag_dispatch,
    NULL,                        /* phy_link_change */
    phy_54880_control_set,       /* phy_control_set */ 
    phy_54880_control_get,       /* phy_control_get */
    phy_ge_reg_read,
    phy_ge_reg_write,
    phy_ge_reg_modify,
    NULL,                        /* Phy notify event */    
    NULL,                        /* pd_probe  */
    phy_54880_ability_advert_set,/* pd_ability_advert_set */
    phy_54880_ability_advert_get,/* pd_ability_advert_get */
    phy_54880_ability_remote_get,/* pd_ability_remote_get */
    phy_54880_ability_local_get  /* pd_ability_local_get  */
};

#else /* INCLUDE_PHY_54880_ESW */
typedef int _phy_54880_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_54880_ESW */

