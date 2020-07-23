
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8750.c
 * Purpose:    Phys Driver support for Broadcom 8750 Serial 10Gig
 *             transceiver :XFI system side and SFI Line interface.
 * Note:       
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_8750)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */

#include <sal/types.h>
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
#include "phynull.h"
#include "phyxehg.h"
#include "phy8750.h"
/*
 * SPI-ROM Program related defs
 */
#define SPI_CTRL_1_L        0xC000
#define SPI_CTRL_1_H        0xC002
#define SPI_CTRL_2_L        0xC400
#define SPI_CTRL_2_H        0xC402
#define SPI_TXFIFO          0xD000
#define SPI_RXFIFO          0xD400
#define WR_CPU_CTRL_REGS    0x11
#define RD_CPU_CTRL_REGS    0xEE
#define WR_CPU_CTRL_FIFO    0x66

/*
 * SPI Controller Commands(Messages).
 */
#define MSGTYPE_HWR          0x40
#define MSGTYPE_HRD          0x80
#define WRSR_OPCODE          0x01
#define WR_OPCODE            0x02
#define WRDI_OPCODE          0x04
#define RDSR_OPCODE          0x05
#define WREN_OPCODE          0x06
#define WR_BLOCK_SIZE        0x40
#define TOTAL_WR_BYTE        0x4000
                                                                                
#define WR_TIMEOUT   1000000

/* typedefs */
typedef struct {
    int rx_los;
    int rx_los_invert;
    int mod_abs;
    int mod_abs_invert;
} PHY8750_DEV_DESC_t;

/* device field macros */
#define RX_LOS(_pc) (((PHY8750_DEV_DESC_t *)((_pc) + 1))->rx_los)
#define RX_LOS_INVERT(_pc) (((PHY8750_DEV_DESC_t *)((_pc) + 1))->rx_los_invert)
#define MOD_ABS(_pc) (((PHY8750_DEV_DESC_t *)((_pc) + 1))->mod_abs)
#define MOD_ABS_INVERT(_pc) (((PHY8750_DEV_DESC_t *)((_pc) + 1))->mod_abs_invert)

static int write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, 
                         uint16 *rddata);
                                                                                

STATIC int _phy8754_mdio_firmware_download(int unit, int port, int offset, uint8 *array,int datalen);
STATIC int phy_8750_speed_get(int, soc_port_t, int *);

extern unsigned char phy8754_ucode_bin[];
extern unsigned int phy8754_ucode_bin_len;


STATIC int
_phy8750_optical_sig_cfg(int unit, int port)
{
    uint16 mask16;
    uint16 data16;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    mask16 = PHY8750_RXLOS_OVERRIDE_MASK |
             PHY8750_MOD_ABS_OVERRIDE_MASK;
    data16 = 0;
    /* default: override RX_LOS signalling */
    if (!RX_LOS(pc)) {
        data16 |= PHY8750_RXLOS_OVERRIDE_MASK;
    }
    /* default: override MOD_ABS signalling */
    if (!MOD_ABS(pc)) {
        data16 |= PHY8750_MOD_ABS_OVERRIDE_MASK;
    }

    SOC_IF_ERROR_RETURN
       (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit,pc,
           PHY8750_PMAD_OPTICAL_CFG_REG, data16,mask16));

    mask16 = PHY8750_RXLOS_LVL_MASK | PHY8750_MOD_ABS_LVL_MASK;
    data16 = 0;
    /* default: RX_LOS level high */
    if (!RX_LOS_INVERT(pc)) {
        data16 |= PHY8750_RXLOS_LVL_MASK;
    }
    /* default: MOD_ABS level high */
    if (!MOD_ABS_INVERT(pc)) {
        data16 |= PHY8750_MOD_ABS_LVL_MASK;
    }
    SOC_IF_ERROR_RETURN
       (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit,pc,
                 PHY8750_PMAD_OPTICAL_SIG_LVL_REG,
                 data16, mask16));
    return SOC_E_NONE;
}

STATIC int
_phy8754_rom_load(int unit, int port, phy_ctrl_t *pc)
{

    uint8 phy_id, i;

    SOC_IF_ERROR_RETURN
        (_phy8750_optical_sig_cfg(unit,port));

    phy_id = pc->phy_id;
    /* configure all four ports  */
    for (i = 0; i < 4; i++) {
        pc->phy_id = (phy_id | 0x3) & ~i; /* Port 3, 2, 1 and 0 */

        /* Disable all port's SPI. bits 3:0 is GPIO port = input.
         * SPI control and debug port pin is multiplexed 
         */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC843, 0x000F));

        /* bits 3:2 is Mode control for SPI/Debug/GPIO pins */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC840, 0x000C));

        /* Set bit SPI Download (15), SER Boot (14) And SPI Port Enable (0):
         * enable SPI bus arbitor.
         */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0xC0F1));

    }
    /* restore the original phy_id */
    pc->phy_id = phy_id;

    /* place the port's uC in reset state */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA10, 0x018F));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC843, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC840, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA10, 0x0188));

    sal_usleep(100000); /* Sleep for at least 80ms */

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC843, 0x000F));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC840, 0x000C));

    return SOC_E_NONE;
}

STATIC int
_phy8750_rom_load(int unit, int port, phy_ctrl_t *pc)
{
    uint16 data = 0;

    /* Read Chip IDs */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_PMA_PMD_CHIPIDr(unit, pc, &data));
    if (data == 0x8754) {
        SOC_IF_ERROR_RETURN
            (_phy8754_rom_load(unit, port, pc));
    } else {
        SOC_IF_ERROR_RETURN(
            READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xc848, &data));
        data |= 0xc000; /* Set SER boot and download via SPI */
        SOC_IF_ERROR_RETURN(
            WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xc848, data));

        /* Reset */;
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY8750_PMA_PMD_CTRLr(unit, pc, 0x8000, 0x8000));
    }

    /* Wait for atleast 100ms for code to download via SPI port*/
    sal_usleep(200000);

    return SOC_E_NONE;
}



STATIC int
_phy8750_adjust_xfi_tx_preemphasis(int unit, int port, phy_ctrl_t *pc)
{

    SOC_IF_ERROR_RETURN(
        PHY8750_XFI(unit, pc));

    SOC_IF_ERROR_RETURN(
        WRITE_PHY8750_XFI_PMA_PMD_REG(unit, pc, 0xc90b, 0xb000));

    SOC_IF_ERROR_RETURN(
        WRITE_PHY8750_XFI_PMA_PMD_REG(unit, pc, 0xc90c, 0x1052));

    SOC_IF_ERROR_RETURN(
        WRITE_PHY8750_XFI_PMA_PMD_REG(unit, pc, 0xc900, 0xec91));

    SOC_IF_ERROR_RETURN(
        PHY8750_MMF(unit, pc));

    return SOC_E_NONE;
}

/* Function:
 *    phy_8750_init
 * Purpose:    
 *    Initialize 8750 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8750_init(int unit, soc_port_t port)
{
    soc_timeout_t  to;
    int            rv = SOC_E_NONE;
    uint16         data16,ucode_ver;
    phy_ctrl_t     *pc;
    uint32         phy_ext_rom_boot, phy_long_xfi; 
    uint16         chip_id = 0xffff, chip_rev = 0xffff;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45);


    /* Read Chip IDs */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_PMA_PMD_CHIPIDr(unit, pc, &chip_id));
    /* Read Chip Revision */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_PMA_PMD_CHIPREVr(unit, pc, &chip_rev));
    chip_rev &= 0x000f;


    /* Support BCM8750, BCM8752 and BCM8754 PHYs */
    if ((chip_id != 0x8750) && (chip_id != 0x8752) && (chip_id != 0x8754)) {
        LOG_CLI((BSL_META_U(unit,
                            "Unsupported PHY: Chip Id = %x\n"), chip_id)); 
        return SOC_E_FAIL;
    }

    if (chip_id == 0x8754) {
        /* These two register writes allow the device boots and stays executing from
         * the internal ROM after the PMA/PMD MII_CTRL_RESET. Otherwise it will load
         * the content from SPI-ROM to RAM and executes from the RAM. In case the
         * SPI-ROM has not been programmed with any firmware, the uController will
         * not work properly.
         */  
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0, (1 << 14) |(1 << 13)));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xca85, 0, 1 ));
    }
     
    /* Reset the device */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_PMA_PMD_CTRLr(unit, pc,
                                      MII_CTRL_RESET, MII_CTRL_RESET));
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_PCS_CTRLr(unit, pc,
                                  MII_CTRL_RESET, MII_CTRL_RESET));

    /* Wait for device to come out of reset */
    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_PHY8750_PMA_PMD_CTRLr(unit, pc, &data16);
        if (((data16 & MII_CTRL_RESET) == 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if (((data16 & MII_CTRL_RESET) != 0) || SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8750/8726 reset failed: u=%d p=%d\n"),
                  unit, port));
        return SOC_E_FAIL;
    }

    /* check optical signaling configuration */
    RX_LOS(pc) = soc_property_port_get(unit, port,spn_PHY_RX_LOS, 0);
    MOD_ABS(pc) = soc_property_port_get(unit, port,spn_PHY_MOD_ABS, 0);
    RX_LOS_INVERT(pc) = soc_property_port_get(unit, port,spn_PHY_RX_LOS_INVERT, 0);
    MOD_ABS_INVERT(pc) = soc_property_port_get(unit, port,spn_PHY_MOD_ABS_INVERT, 0);

    phy_ext_rom_boot = soc_property_port_get(unit, port, 
                                            spn_PHY_EXT_ROM_BOOT, 1);
    if (phy_ext_rom_boot) {
        SOC_IF_ERROR_RETURN
            (_phy8750_rom_load(unit, port, pc));
    } else {
        /* MDIO loading */
        if (chip_id == 0x8754) {
            SOC_IF_ERROR_RETURN
                (_phy8754_mdio_firmware_download(unit, port, 0,phy8754_ucode_bin,
                                        phy8754_ucode_bin_len));
            SOC_IF_ERROR_RETURN
                (_phy8750_optical_sig_cfg(unit,port));
        }
    }

    /* read the microcode revision */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA1A, &ucode_ver));
 
    /* If microcode revision >  0x0301, then check the completion of 
     * the SPI-ROM load.
     */
    if (ucode_ver >= 0x0301) {
        rv = READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA1C, &data16);
        if (data16 != 0x600D || SOC_FAILURE(rv)) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "8750: p=%d SPI-ROM load: Bad Checksum (0x%x)\n"),
                                 port, data16));
             /* let it continue in case a blank SPI-ROM or allow other ports to 
              * initialize. Internal ROM code always show checksum as zero. 
              */
        }
    } else {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "8750: p=%d SPI-ROM load: Invalid Ucode version (0x%x)\n"),
                             port, ucode_ver));
             /* let it continue to allow other ports to initialize */
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "8750: u=%d port%d rom code ver: 0x%x: init.\n"),
                         unit, port, ucode_ver));

    /* By default XFI cannot drive long distance cables. TX premphasis needs
     * to be adjusted if long cables are used on the XFI side
     */
    phy_long_xfi = soc_property_port_get(unit, port, 
                                         spn_PHY_LONG_XFI, 1);
    if (phy_long_xfi) {
        SOC_IF_ERROR_RETURN(
            _phy8750_adjust_xfi_tx_preemphasis(unit, port, pc));
    }

    /* Disable SyncE */
    SOC_IF_ERROR_RETURN(
        PHY8750_MMF(unit, pc));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xc840, 0x0000, 0xffff));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xcd17, 0x0000, 0xffff));
    SOC_IF_ERROR_RETURN(
        PHY8750_XFI(unit, pc));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xcd17, 0x0000, 0xffff));
    SOC_IF_ERROR_RETURN(
        PHY8750_MMF(unit, pc));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 
          PHY8750_PMAD_OPTICAL_CFG_REG, (1U<<12), ((1U<<12) | (1U<<4))));

    LOG_CLI((BSL_META_U(unit,
                        "BCM%x (Rev. %d): u=%d port%d rom code ver. 0x%x\n"),
             chip_id, chip_rev, unit, port, ucode_ver));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8750_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8750_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16 an_status;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_AN_REG(unit, pc, 0xFFE0, &an_status));
    *an = (an_status & PHY8750_AN_ENABLE)? TRUE: FALSE;

    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_AN_REG(unit, pc, 0xFFE1, &an_status));
    *an_done = (an_status & PHY8750_AN_DONE)? TRUE: FALSE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8750_an_set
 * Purpose:
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_8750_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;

    int_pc = INT_PHY_SW_STATE(unit, port);
    if (NULL != int_pc) {
        SOC_IF_ERROR_RETURN(
        PHY_AUTO_NEGOTIATE_SET(int_pc->pd, unit, port, an));
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_an_set: u=%d p=%d an=%d\n"),
                         unit, port, an));

    if (an) {
        /* Clear Forced Speed selections in Miscellaneous 2 Register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_AN_REG(unit, pc, 0x8309, 0x0000));
        /* Enable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_AN_REG(unit, pc, 0xFFE0, 0x1300));
    } else {
        /* disable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_AN_REG(unit, pc, 0xFFE0, 0));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_AN_REG(unit, pc, 0x8309, 0x0020));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8750_ability_advert_get
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
phy_8750_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    uint16           an_adv;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_AN_REG(unit, pc, 0xFFE0, &an_adv));

    mode = 0;
    mode |= (an_adv & 0x1000) ? SOC_PA_SPEED_1000MB : 0;

    /* check Full Duplex advertisement on Clause 37 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_AN_REG(unit, pc, 0xFFE4, &an_adv));
    mode |= (an_adv & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

    ability->speed_full_duplex = mode;
    ability->pause = 0;

    switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
        case MII_ANA_C37_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_ability_advert_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
* Function:
*      phy_8750_ability_remote_get
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
phy_8750_ability_remote_get(int unit, soc_port_t port, 
                            soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint16      lp_abil;

    pc = EXT_PHY_SW_STATE(unit, port);
                                           
    if (NULL == ability) {
        return SOC_E_PARAM;
    }
                                          
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_AN_REG(unit, pc, 0xFFE5, &lp_abil));
    ability->speed_full_duplex  = (lp_abil & MII_ANA_C37_FD) ? 
                                   SOC_PA_SPEED_1000MB : 0;

    ability->pause = 0;
    switch (lp_abil & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
        case MII_ANP_C37_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANP_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_AN_REG(unit, pc, PHY8750_AN_STATUS_REG, &lp_abil));
    ability->flags     = (lp_abil & 0x1) ? SOC_PA_AUTONEG : 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_ability_remote_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8750_ability_advert_set
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
phy_8750_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    uint16           an_adv;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    /*
     * Set advertised duplex (only FD supported).
     */
    an_adv =  (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 
               MII_ANA_C37_FD : 0;

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
        case SOC_PA_PAUSE_TX:
            an_adv |= MII_ANA_C37_ASYM_PAUSE;
            break;
        case SOC_PA_PAUSE_RX:
            an_adv |= MII_ANA_C37_ASYM_PAUSE | MII_ANA_C37_PAUSE;
            break;
        case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
            an_adv |= MII_ANA_C37_PAUSE;
            break;
    }

    /* write Clause37 advertisement */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_MMF_AN_REG(unit, pc, 0xFFE4, an_adv, 
                              MII_ANA_C37_ASYM_PAUSE | 
                              MII_ANA_C37_PAUSE |
                              MII_ANA_C37_FD |
                              MII_ANA_C37_HD ));

    /*Set on the XFI side as well */
    PHY8750_XFI(unit, pc);
    /* write Clause37 advertisement */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_XFI_AN_REG(unit, pc, 0xFFE4, an_adv, 
                              MII_ANA_C37_ASYM_PAUSE | 
                              MII_ANA_C37_PAUSE |
                              MII_ANA_C37_FD |
                              MII_ANA_C37_HD ));
    PHY8750_MMF(unit, pc);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_ability_advert_set: u=%d p=%d pause=0x%08x adv_reg1=0x%04x\n"),
              unit, port, ability->pause, an_adv));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8750_ability_local_get
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
phy_8750_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_ability_local_get: u=%d p=%d\n"),
              unit, port));
                                                                               
    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
                                                                               
    switch(pc->speed_max) {
        case 10000:
        default:
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
            break;
    }
                                                                               
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM; 
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;
                                                                               
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_ability_local_get: u=%d p=%d speed=0x%x\n"),
              unit, port, ability->speed_full_duplex));
                                                                               
    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_8750_link_get
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
phy_8750_link_get(int unit, soc_port_t port, int *link)
{
    uint16      pma_mii_stat, pcs_mii_stat, link_stat;
    phy_ctrl_t *pc;
    int cur_speed = 0;
    int an = 0,an_done = 0;
    uint16 speed_val; 

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_8750_an_get(unit,port,&an,&an_done);

    /* return link false if in the middle of autoneg */
    if (an == TRUE && an_done == FALSE) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    /* AN only supports 1G */
    if (!an) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8750_MMF_PMA_PMD_REG(unit,pc, PHY8750_PMAD_CTRL2_REG, 
                                          &speed_val));
        if ((speed_val & PHY8750_PMAD_CTRL2r_PMA_TYPE_MASK) !=
                                       PHY8750_PMAD_CTRL2r_PMA_TYPE_1G_KX) {
            cur_speed = 10000;
        } else {
            cur_speed = 1000;
        }
    } else {
        cur_speed = 1000;
    }

    if (cur_speed == 10000) { /* check all 3 device's link status if 10G */

        /* Receive Link status */
        SOC_IF_ERROR_RETURN
            (READ_PHY8750_PCS_STATr(unit, pc, &pcs_mii_stat));

        SOC_IF_ERROR_RETURN
            (READ_PHY8750_PMA_PMD_STATr(unit, pc, &pma_mii_stat));

        /* MMF link status */
        link_stat = pma_mii_stat & pcs_mii_stat;
        *link = (link_stat & MII_STAT_LA) ? TRUE : FALSE;

    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY8750_MMF_AN_REG(unit, pc, 0xffe1,
                                     &link_stat));
        *link = (link_stat & MII_STAT_LA) ? TRUE : FALSE;
    } 

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_8750_link_get: u=%d port%d: link:%s\n"),
                 unit, port, *link ? "Up": "Down"));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8750_enable_set
 * Purpose:
 *    Enable/Disable phy 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8750_enable_set(int unit, soc_port_t port, int enable)
{
    uint16      data;       /* Holder for new value to write to PHY reg */
    uint16      mask;       /* Holder for bit mask to update in PHY reg */
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    data = 0;
    mask = 1 << 0; /* Global PMD transmit disable */
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        data = 1 << 0;  /* Global PMD transmit disable */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_PMA_PMD_TX_DISABLEr(unit, pc, data, mask));

    data = mask = 1 << 11; /* Disable clock for low power*/
    if (enable) {
        data = 0;
    }
    SOC_IF_ERROR_RETURN   
        (MODIFY_PHY8750_PMA_PMD_CTRLr(unit, pc, data, mask));
    
    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_8750_lb_set
 * Purpose:
 *    Put 8750 in PHY PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8750_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 
    uint16     tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

    tmp = enable ? PHY8750_MII_CTRL_PMA_LOOPBACK : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_PMA_PMD_CTRLr(unit, pc,
                                      tmp, PHY8750_MII_CTRL_PMA_LOOPBACK));
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8750_lb_get
 * Purpose:
 *    Get 8750 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8750_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      tmp;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8750_PMA_PMD_CTRLr(unit, pc, &tmp));
    *enable = (tmp & PHY8750_MII_CTRL_PMA_LOOPBACK) ? TRUE : FALSE;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_lb_get: u=%d port%d: loopback:%s\n"),
              unit, port, *enable ? "Enabled": "Disabled"));
    
    return SOC_E_NONE;
}

STATIC int
_phy_8750_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    uint16       data;  /* Temporary holder of reg value to be written */
    uint16       mask;  /* Bit mask of reg value to be updated */
    phy_ctrl_t  *pc;    /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Dozen SerDes */
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         data = (uint16)(value & 0xf);
         data = data << 12;
         mask = 0xf000;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data = (uint16)(value & 0xf);
         data = data << 8;
         mask = 0x0f00;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data = (uint16)(value & 0xf);
         data = data << 4;
         mask = 0x00f0;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xca05, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_8750_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16         data16;   /* Temporary holder of a reg value */
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    data16 = 0;
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xca05, &data16));
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         *value = (data16 & 0xf000) >> 12;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         *value = (data16 & 0x0f00) >> 8;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         *value = (data16 & 0x00f0) >> 4;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8750_control_set
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
phy_8750_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;

    PHY_CONTROL_TYPE_CHECK(type);

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8750_control_tx_driver_set(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}
/*
 * Function:
 *      phy_8750_control_get
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
phy_8750_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;

    PHY_CONTROL_TYPE_CHECK(type);

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8750_control_tx_driver_get(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_8750_speed_set
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
phy_8750_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;
    int rv = SOC_E_NONE;


    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_speed_set: u=%d p=%d speed=%d\n"), 
                         unit, port,speed));

    if (!(speed == 10000 || speed == 1000)) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (speed == 10000) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit,pc, PHY8750_PMAD_CTRL_REG,
                                            MII_CTRL_SS_LSB,
                                            MII_CTRL_SS_LSB));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit,pc, PHY8750_PMAD_CTRL2_REG,
                                        PHY8750_PMAD_CTRL2r_PMA_TYPE_10G_LRM, 
                                        PHY8750_PMAD_CTRL2r_PMA_TYPE_MASK));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_AN_REG(unit, pc, PHY8750_AN_CTRL_REG,
                                      PHY8750_AN_ENABLE |
                                      PHY8750_AN_RESTART));
        sal_usleep(40000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_AN_REG(unit,pc, PHY8750_AN_CTRL_REG, 0x0));

    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit,pc, PHY8750_PMAD_CTRL2_REG,
                                        PHY8750_PMAD_CTRL2r_PMA_TYPE_1G_KX,
                                        PHY8750_PMAD_CTRL2r_PMA_TYPE_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit,pc, MII_CTRL_REG,
                                        0, MII_CTRL_SS_LSB));

    }
    /* need to set the internal phy's speed accordingly */

    int_pc = INT_PHY_SW_STATE(unit, port);
    if (NULL != int_pc) {
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
    }
    return rv;
}

/*
 * Function:
 *      phy_8750_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - current link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_8750_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;
    uint16       speed_val;
    int an = 0;
    int an_done = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_8750_an_get(unit,port,&an,&an_done);

    *speed = 0;
    if (an) { /* autoneg is enabled */
        if (an_done) { 
            *speed = 1000;  /* can only do 1G autoneg */
        }
    } else { /* autoneg is not enabled, forced speed */
        SOC_IF_ERROR_RETURN
            (READ_PHY8750_MMF_PMA_PMD_REG(unit,pc, PHY8750_PMAD_CTRL2_REG, &speed_val));
        if ((speed_val & PHY8750_PMAD_CTRL2r_PMA_TYPE_MASK) ==
                                                PHY8750_PMAD_CTRL2r_PMA_TYPE_1G_KX) {
            *speed = 1000;
        } else {
            *speed = 10000;
        }
    }    
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy8754_mdio_firmware_download
 * Purpose:
 *      write the given firmware to the internal RAM
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
_phy8754_mdio_firmware_download(int unit, int port, int offset, uint8 *array,int datalen)
{
    int j;
    phy_ctrl_t *pc;
    uint16 data16;
    uint16 mask16;

    if (array == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Programme SPA Control Register to Boot from MDIO 
     * bit 15, 0 MDIO for serial boot. bit 14, 1 serial boot
     */
    mask16 = (1 << 14) | (1 << 15);
    data16 = 1 << 14;
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, PHY8750_PMAD_SPA_CTRL_REG,
                                        data16,mask16));

    /* Reset, to download code from MDIO */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_PMA_PMD_CTRLr(unit, pc, &data16));

    data16 |= 0x8000;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_PMA_PMD_CTRLr(unit, pc, data16));

    sal_usleep(3000); /* Wait for 3ms */

    /* Write Starting Address, where the Code will reside in SRAM */
    data16 = 0xC000;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 
                              PHY8750_PMAD_M8051_MSGIN_REG, data16));
    sal_udelay(20); /* Wait for 15us */

    /* Write SPI SRAM Count Size */
    data16 = (datalen)/2;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 
                              PHY8750_PMAD_M8051_MSGIN_REG, data16));
    sal_udelay(20); /* Wait for 15us */

    /* Fill in the SPI RAM */
    for (j = 0; j < datalen/2; j++) {
        data16 = (array[2*j] << 8) | array[2*j+1];
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 
                               PHY8750_PMAD_M8051_MSGIN_REG, data16));

        /* Make sure the word is read by the Micro */
        sal_udelay(20); /* Wait for 15us */
    }

    /* Read Hand-Shake message (Done) from Micro */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 
                                 PHY8750_PMAD_M8051_MSGOUT_REG, &data16));

    if (data16 != 0x4321) {
        /* Download failure */
        LOG_CLI((BSL_META_U(unit,
                            "_phy8754_mdio_firmware_download fail,p=%d done msg 0x%x\n"),
                 port,data16));
        return SOC_E_FAIL;
    }

    /* Clear LASI status */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 
                                 PHY8750_PMAD_RX_ALARM_STAT_REG, &data16));

    /* Wait for LASI to be asserted when M8051 writes checksum to MSG_OUTr */
    sal_udelay(400); /* Wait for 300us */

    /* read message out reg */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 
                                 PHY8750_PMAD_M8051_MSGOUT_REG, &data16));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy8754_mdio_firmware_download: p=%d msgout 0x%x\n"), 
                         port,data16));

    /* clear LASI  */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 
                                 PHY8750_PMAD_RX_ALARM_STAT_REG, &data16));

    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA1C, &data16));

    /* check if checksum is correct */
    if (data16 != 0x600D) {
        /* Bad CHECKSUM */
        LOG_CLI((BSL_META_U(unit,
                            "_phy8754_mdio_firmware_download, bad cksum: "
                            "p=%d, cksum=0x%x\n"),
                 port, data16));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

/*
 *  Function:  *      phy_8750_rom_wait
 *
 *  Purpose:
 *      Wait for data to be written to the SPI-ROM.
 *  Input:
 *      unit, port
 */
STATIC int
phy_8750_rom_wait(int unit, int port)
{
    uint16     rd_data;     
    uint16     wr_data;
    int        count;     
    phy_ctrl_t *pc;
    soc_timeout_t to;     
    int        SPI_READY; 

    pc = EXT_PHY_SW_STATE(unit, port);

    rd_data = 0;
    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        /* Write SPI Control Register Read Command. */
        count = 1;
        wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        if (rd_data & 0x0100) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if (!(rd_data & 0x0100)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_8750_rom_program: u = %d p = %d "
                              "timeout 1\n"), unit, port));
        return SOC_E_TIMEOUT;
    }

    SPI_READY = 1;
    while (SPI_READY == 1) {
        /* Set-up SPI Controller To Receive SPI EEPROM Status. */
        count = 1;
        wr_data = ((WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = SPI_CTRL_2_H;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Fill-up SPI Transmit Fifo To check SPI Status. */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write Tx Fifo Register Address. */
        wr_data = SPI_TXFIFO;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-1. */
        wr_data = ((1 * 0x0100) | MSGTYPE_HRD);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Tx Fifo Control Word-2. */
        wr_data = RDSR_OPCODE;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control Register Write Command. */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-1. */
        wr_data = 0x0101;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control Register Write Command. */
        count = 1;
        wr_data = ((WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_CTRL_1_H;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0103;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Wait For 64 bytes To be written.   */
        rd_data = 0x0000;
        soc_timeout_init(&to, WR_TIMEOUT, 0);
        do {
            count = 1;
            wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
            SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
            wr_data = SPI_CTRL_1_L;
            SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
            if (rd_data & 0x0100) {
                break;
            }
        } while (!soc_timeout_check(&to));
        if (!(rd_data & 0x0100)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "phy_8750_rom_program: u = %d p = %d "
                                  "timeout 2\n"), unit, port));
            return SOC_E_TIMEOUT;
        }
        /* Write SPI Control Register Read Command. */
        count = 1;
        wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_RXFIFO;

        SOC_IF_ERROR_RETURN         
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA12, wr_data));
        SOC_IF_ERROR_RETURN
            (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));

        /* Clear LASI Message Out Status. */
        SOC_IF_ERROR_RETURN
            (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9003, &wr_data));

        if ((rd_data & 0x1) == 0) {
            SPI_READY = 0;
        }
    } /* SPI_READY  */
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8750_rom_write_enable_set
 *
 * Purpose:
 *      Enable disable protection on SPI_EEPROM
 *
 * Input:
 *      unit
 *      port
 *      enable
 * Output:
 *      SOC_E_xxx
 *
 * Notes:
 *          25AA256 256Kbit Serial EEPROM
 *          STATUS Register
 *          +------------------------------------------+
 *          | WPEN | x | x | x | BP1 | BP0 | WEL | WIP |
 *          +------------------------------------------+
 *      BP1 BP0  :   Protected Blocks
 *       0   0   :  Protect None
 *       1   1   :  Protect All
 *
 *      WEL : Write Latch Enable
 *       0  : Do not allow writes
 *       1  : Allow writes
 */

STATIC int
phy_8750_rom_write_enable_set(int unit, int port, int enable)
{
    uint16     rd_data;
    uint16     wr_data;
    uint8      wrsr_data;
    int        count;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Write SPI Control Register Write Command.
     */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Address.
     */
    wr_data = SPI_CTRL_2_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Word-1.
     */
    wr_data = 0x8200;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Word-2.
     */
    wr_data = 0x0100;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Fill-up SPI Transmit Fifo With SPI EEPROM Messages.
     * Write SPI Control Register Write Command.
     */
    count = 4;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write Tx Fifo Register Address.
     */
    wr_data = SPI_TXFIFO;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-1.
     */
    wr_data = ((1 * 0x0100) | MSGTYPE_HWR);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-2.
     */
    wr_data = ((MSGTYPE_HWR * 0x0100) | WREN_OPCODE);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-3.
     */
    wr_data = ((WRSR_OPCODE * 0x100) | (0x2));
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-4.
     */
    wrsr_data = enable ? 0x2 : 0xc;
    wr_data = ((wrsr_data * 0x0100) | wrsr_data);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control Register Write Command.
     */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Address.
     */
    wr_data = SPI_CTRL_1_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Word-1.
     */
    wr_data = 0x0101;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Word-2.
     */
    wr_data = 0x0003;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Wait For WRSR Command To be written.
     */
    SOC_IF_ERROR_RETURN(phy_8750_rom_wait(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8750_firmware_set
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
phy_8750_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{

    uint16     rd_data;
    uint16     wr_data;
    int        j;
    int        i = 0;
    int        count;
    uint8      spi_values[WR_BLOCK_SIZE];
    uint8      phy_id;
    uint16     chip_id;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    phy_id = pc->phy_id;

    /* Read Chip IDs */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_PMA_PMD_CHIPIDr(unit, pc, &chip_id));
    if (chip_id == 0x8754) {
        /*
         * Place all Micros in reset, GEN_CTRL
         * CAVEAT:
         *      To eliminate the interference with SPI Programming process
         *      it is required to reset the MicroController in adjacent odd port.
         *      i.e. if you are programming channel-0(2) of the 8754 then you need
         *      to reset the microcontroller of channel-1(3).
         *      The instruction below reset all channels in 8754.
         */
        phy_id = pc->phy_id;
        for (i = 0; i < 4; i++) {
            pc->phy_id = (phy_id & (~3)) + i; /* Port 3, 2, 1 and 0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA10, 0x018f));
        }
        pc->phy_id = phy_id;  /* restore original phy_id */

        /*
         * Disable all port's SPI
         * CAVEAT:
         *      Always disable all ports associated with the 
         *      SPI you are programming.
         */
        /*
         * Write to register 0x1.C843 bits 3:0 to 4'b1111
         */
        phy_id = pc->phy_id;
        for (i = 0; i < 4; i++) {
            pc->phy_id = (phy_id & (~3)) + i; /* Port 3, 2, 1 and 0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC843, 0x000f));
        }
        pc->phy_id = phy_id;  /* restore original phy_id */

        /*
         * Write to register 0x1.C840 bits 3:2 to 2'b11.
         */
        phy_id = pc->phy_id;
        for (i = 0; i < 4; i++) {
            pc->phy_id = (phy_id & (~3)) + i; /* Port 3, 2, 1 and 0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC840, 0x000c));
        }
        pc->phy_id = phy_id;

        /*
         * Enable SPI for Port0
         */    
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC843, 0x0000));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC840, 0x0000));
        /*
         * Remove M8051 resets for port 0.
         * and wait for Micro to complete Init.
         */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA10, 0x0188));
        sal_usleep(10000);

        /*
         * Set bit 0 (spa enable) in C848 for master channel.
         */ 
        phy_id = pc->phy_id;
        for (i = 0; i < 4; i++) {
            pc->phy_id = (phy_id & (~3)) + i; /* Port 3, 2, 1 and 0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0x80f9));
        }
        pc->phy_id = phy_id;
    }

    /* 
     * Enable SPI ROM access in SPI control
     */
    if (chip_id == 0x8754) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0xc0fd));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0x80fd));
    }

    /*
     * ser_boot pin HIGH
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA85, 0x1, 0x1)); 
    /*
     * Read LASI Status registers To clear initial Failure status.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9004, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9005, &rd_data));

    /*
     * Enable the LASI For Message out. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9000, 0x0004));
    /*
     * 0x9002, bit 2 Is Rx Alarm enabled For LASI. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9002, 0x0004));
    /*
     * Read Any Residual Message out register.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));
    /*
     * Clear LASI Message Out Status. 
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));

    /* set SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(phy_8750_rom_write_enable_set(unit, port, 1));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "init0: u=%d p=%d\n"),
                         unit, port));

    for (j = 0; j < datalen; j += WR_BLOCK_SIZE) {
        /*
         * Setup SPI Controller. 
         */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Address.*/
        wr_data = SPI_CTRL_2_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Word-1. */
        wr_data = 0x8200;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Word-2. */
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Fill-up SPI Transmit Fifo.
         * Write SPI Control Register Write Command.
         */
        count = 4 + (WR_BLOCK_SIZE / 2);
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write Tx Fifo Register Address. */
        wr_data = SPI_TXFIFO;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-1. */
        wr_data = ((1 * 0x0100) | MSGTYPE_HWR);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-2. */
        wr_data = ((MSGTYPE_HWR * 0x0100) | WREN_OPCODE);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-3. */
        wr_data = ((WR_OPCODE * 0x0100) | (0x3 + WR_BLOCK_SIZE));
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-4. */
        wr_data = (((j & 0x00FF) * 0x0100) | ((j & 0xFF00) / 0x0100));
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "loop: u=%d p=%d,inxj: %d,inxi:%d\n"),
                             unit, port,j,i));

        if (datalen < (j + WR_BLOCK_SIZE)) {   /* last block */ 
            sal_memset(spi_values,0,WR_BLOCK_SIZE);
            sal_memcpy(spi_values,&array[j],datalen - j);

            for (i = 0; i < WR_BLOCK_SIZE; i += 2) {
                /* Write SPI Tx Fifo Data Word-4. */
                wr_data = ((spi_values[i+1] * 0x0100) | spi_values[i]);
                SOC_IF_ERROR_RETURN(write_message(unit, pc,wr_data, &rd_data));
            }           
        } else {        
            for (i = 0; i < WR_BLOCK_SIZE; i += 2) {
                /* Write SPI Tx Fifo Data Word-4. */
                wr_data = ((array[j+i+1] * 0x0100) | array[j+i]);
                SOC_IF_ERROR_RETURN(write_message(unit, pc,wr_data, &rd_data));
            }
        }

        /* 
         * Set-up SPI Controller To Transmit.
         * Write SPI Control Register Write Command.
         */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -1 Register Word-1. */
        wr_data = 0x0501;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0003;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Wait For 64 bytes To be written.   */
        SOC_IF_ERROR_RETURN(phy_8750_rom_wait(unit,port));

    } /* SPI_WRITE */

    /* clear SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(phy_8750_rom_write_enable_set(unit, port, 0));

    /* Disable SPI EEPROM. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Address. */
    wr_data = SPI_CTRL_2_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-1. */
    wr_data = 0x8200;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-2. */
    wr_data = 0x0100;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));


    /* Fill-up SPI Transmit Fifo With SPI EEPROM Messages. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write Tx Fifo Register Address. */
    wr_data = SPI_TXFIFO;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-1. */
    wr_data = ((0x1*0x0100) | MSGTYPE_HWR);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-2. */
    wr_data = WRDI_OPCODE;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control Register Write Command. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Address. */
    wr_data = SPI_CTRL_1_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Word-1. */
    wr_data = 0x0101;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Word-2. */
    wr_data = 0x0003;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * ser_boot pin LOW
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA85, 0x0, 0x1)); 
    /* 
     * Disable SPI ROM access in SPI control
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0xc0fd, 0xffff)); 

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8750_rom_program: u=%d p=%d done\n"), unit, port));

    return SOC_E_NONE;
}

static int
write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, uint16 *rddata)
{

    uint16     tmp_data = 0;
    soc_timeout_t to;

    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA12, wrdata));

    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        SOC_IF_ERROR_RETURN
            (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9005, &tmp_data));
        if (tmp_data & 0x4)
            break;
    } while (!soc_timeout_check(&to));
    if (!(tmp_data & 0x4)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "write_message failed: wrdata %04x\n"), wrdata));
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    *rddata = tmp_data;
    SOC_IF_ERROR_RETURN
        (READ_PHY8750_MMF_PMA_PMD_REG(unit, pc, 0x9003, &tmp_data));

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_8750_probe
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
phy_8750_probe(int unit, phy_ctrl_t *pc)
{
    pc->size = sizeof(PHY8750_DEV_DESC_t);
    return SOC_E_NONE;
}


/*
 * Variable:
 *    phy_8750_drv
 * Purpose:
 *    Phy Driver for 10G XFI to SFI PHY.
 */

phy_driver_t phy_8750drv_xe = {
    "8750 10-Gigabit PHY Driver",
    phy_8750_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_8750_link_get,    /* Link get   */
    phy_8750_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_null_set,         /* Duplex set */
    phy_null_one_get,     /* Duplex get */
    phy_8750_speed_set,   /* Speed set  */
    phy_8750_speed_get,   /* Speed get  */
    phy_null_set,         /* Master set */
    phy_null_zero_get,    /* Master get */
    phy_8750_an_set,      /* ANA set */
    phy_8750_an_get,      /* ANA get */
    NULL,                 /* Local Advert set */
    NULL,                 /* Local Advert get */
    phy_null_mode_get,    /* Remote Advert get */
    phy_8750_lb_set,      /* PHY loopback set */
    phy_8750_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_xehg_interface_get, /* IO Interface get */
    NULL,   /* PHY abilities mask */
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8750_control_set,    /* phy_control_set */
    phy_8750_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_8750_probe,         /* pd_probe  */
    phy_8750_ability_advert_set,  /* pd_ability_advert_set */
    phy_8750_ability_advert_get,  /* pd_ability_advert_get */
    phy_8750_ability_remote_get,  /* pd_ability_remote_get */
    phy_8750_ability_local_get,   /* pd_ability_local_get  */
    phy_8750_firmware_set         /* pd_firmware_set */
};

#else /* INCLUDE_PHY_8750 */
typedef int _phy_8750_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8750 */

