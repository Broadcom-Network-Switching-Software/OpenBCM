
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8072.c
 * Purpose:    Phys Driver support for Broadcom 8072 Serial 10Gig
 *             transceiver with XAUI interface.
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_8072)
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
#include "phy8072.h"

#define PHY_ID1_8072    0x6035
#define PHY_ID1_8073    0x6036
#define PHY_ID1_8074    0x6036
#define PHY_8073_A0     0x0000
#define PHY_8073_A1     0x0001

#define PHY_8072_PMA_PMD_LOOPBACK       1
#if 0
#define PHY_8072_PCS_LOOPBACK           1
#define PHY_8072_SYSTEM_LOOPBACK        1
#define PHY_8072_XAUI_LOOPBACK          1
#endif

#define APPLY_SNR_PATCH(dev, ver) \
    ((dev == PHY_8073_A1) && (ver == 0x0102)) ||     \
    ((dev == PHY_8073_A0) && ((ver == 0x0101) || (ver == 0x0102)))

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
static int write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, uint16 * rddata);

STATIC int phy_8072_ability_local_get(int, soc_port_t, soc_port_ability_t *);
STATIC int phy_8072_speed_get(int, soc_port_t, int *);
STATIC int phy_8072_ability_advert_set(int, soc_port_t, soc_port_ability_t *);
STATIC int phy_8072_an_get(int, soc_port_t, int *, int *);


STATIC int
_8072_rom_load(int unit,phy_ctrl_t * pc,int phy_id1)
{
    if (phy_id1 == PHY_ID1_8072) {
        /* reset the device */
        SOC_IF_ERROR_RETURN
             (MODIFY_PHY8072_PMA_PMD_CTRLr(unit, pc,
                                          MII_CTRL_RESET, MII_CTRL_RESET));
                                                                                     
        /* Wait 200ms after PCB reboot, for internal ROM load to finish */
        sal_usleep(200000);

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xca85, 0x0001));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN_CTRL_STAT_REG, 
                                       0x0188));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN_CTRL_STAT_REG, 
                                       0x018a));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN_CTRL_STAT_REG, 
                                       0x0188));
        /*
         * Wait for atleast 100ms for code to download via SPI port
         */
        sal_usleep(200000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xca85, 0x0000));
        sal_usleep(1000);
                                                                                
    } else if (phy_id1 == PHY_ID1_8073) {
        /* reset the device */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_PMA_PMD_CTRLr(unit, pc,
                                          MII_CTRL_RESET, MII_CTRL_RESET));

        /* no delay is needed for 8073 per chip designer*/                                              
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN_CTRL_STAT_REG, 
                                       0x0001));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN_CTRL_STAT_REG, 
                                       0x008C));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xCA85, 0x0001));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN_CTRL_STAT_REG, 
                                       0x018A));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN_CTRL_STAT_REG, 
                                       0x0188));

        /*
         * Wait for atleast 100ms for code to download via SPI port
         */
        sal_usleep(200000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xca85, 0x0000));
        sal_usleep(1000);
    }
    return SOC_E_NONE;
                                                                                
}

STATIC int
_8074_rom_load(int unit,phy_ctrl_t * pc,int phy_id1)
{
    uint16 phy_addr;
    int i;

    phy_addr = pc->phy_id & 0xfffc;
    for(i = 0; i < 4; i++) {
        /* Disable SPI for all ports, return value ignored on purpose */
        pc->write(unit, phy_addr+i,SOC_PHY_CLAUSE45_ADDR(1, 0xc843), 0x000f);
        pc->write(unit, phy_addr+i,SOC_PHY_CLAUSE45_ADDR(1, 0xc840), 0x000C);

        /* Set bit SPI Download (15), SER Boot (14) And SPI Port Enable (0) In C848, SPI_PORT_CTRL */
        pc->write(unit, phy_addr+i,SOC_PHY_CLAUSE45_ADDR(1, 0xc848), 0xC0F1);
    }

    /* Place Micro in reset */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xca10, 0x018f));

    /* Enable SPI */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xc843, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xc840, 0x0000));

    /* Remove M8051 resets */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xca10, 0x0188));

    /* The rest is moved outside this routine. */
    return SOC_E_NONE;
}

STATIC int
_8073_A0_xaui_check(int unit, int port)
{
    uint8 count;
    uint16 data16;
    phy_ctrl_t *pc;
    soc_timeout_t  to;
    sal_usecs_t start, end;
    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Get the Chip revision or chip id
     * XAUI Workaround is not needed for Rev A1 and 8074 device
     */

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_CHIP_REV_REG, &data16));
    if (data16 == PHY_8073_A1) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit,pc,0xC802,&data16));
    if ( data16 == 0x8074) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_USR_STATUS_REG, &data16));

    if (!PMAD_STATUS_AN_10G(data16)) {
        return SOC_E_NONE;
    }

    start = sal_time_usecs();
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "xaui w/a in progress \n")));
    for (count = 0; count < 12; count++) {
        soc_timeout_init(&to, 5000000, 0);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             ".")));
        do {
            SOC_IF_ERROR_RETURN
                (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_USR_STATUS_REG, 
                              &data16));
        if (!(data16 & PMAD_STATUS_1G_LINKDOWN) || 
            !(data16 & PMAD_STATUS_2p5G_LINKDOWN)) {
                return SOC_E_NONE;
        } else if (!(data16 & PMAD_STATUS_10G_LINKDOWN)) {
                SOC_IF_ERROR_RETURN
                    (READ_PHY8072_PMA_PMD_REG(unit, pc, 0xC841, &data16));
                if (data16 & (1<<15)) {
                    end = sal_time_usecs();
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "\nxaui w/a completed successfully u=%d,p=%d"
                                         " time taken = %d\n"), unit, port,
                              SAL_USECS_SUB(end, start)));
                    return SOC_E_NONE;
                }
            }
        } while (!soc_timeout_check(&to));
    }

    LOG_WARN(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "\nWarning! : u=%d, p=%d:"
                         "8073A0 XAUI Work Around may not have completed\n"), unit, port));

    SOC_IF_ERROR_RETURN /* Restart AN ..HP*/
        (WRITE_PHY8072_AN_REG(unit, pc, AN_CTRL_REG, AN_ENABLE|AN_RESTART)); 

    return SOC_E_NONE;
}

/* Function:
 *    phy_8072_init
 * Purpose:    
 *    Initialize 8072 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8072_init(int unit, soc_port_t port)
{
    soc_timeout_t  to;
    int            rv;
    uint16         data16;
    phy_ctrl_t     *pc;
    uint32	   phy_ext_rom_boot;	 
    soc_port_ability_t  ability;
    int phy_8074;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45);

    phy_ext_rom_boot = soc_property_port_get(unit, port, spn_PHY_EXT_ROM_BOOT, 1);

    phy_8074 = FALSE;

    if (phy_ext_rom_boot) {

        if (pc->phy_id1 == PHY_ID1_8073) {
            uint16  data;
            /* Check register 0xc802 to find out if it is a real 8074 device. */
            SOC_IF_ERROR_RETURN
                (READ_PHY8072_PMA_PMD_REG(unit,pc,0xC802,&data));
            if ( data == 0x8074 ) {
                phy_8074 = TRUE;
                _8074_rom_load(unit,pc,pc->phy_id1);
            } else {
                _8072_rom_load(unit,pc,pc->phy_id1);
            }

        } else {
            _8072_rom_load(unit,pc,pc->phy_id1);
        }

        /* Make sure SPI-ROM load is complete */
        soc_timeout_init(&to, 500000, 0);
        do {
            rv = (READ_PHY8072_GENREG1r(unit, pc, &data16));
            if (((data16 & 0xFFF0) == PMDA_ROM_LOAD_8072ID) || 
                ((data16 & 0xFFF0) == PMDA_ROM_LOAD_8073ID) ||  
                 SOC_FAILURE(rv)) {
                break;
            }
        } while (!soc_timeout_check(&to));

        if (phy_8074) {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xc843, 0x000f, 0x000f));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xc840, 0x000c, 0x000c));
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY8072_GENREG1r(unit, pc,&data16));
        if (PMDA_ROM_LOAD_8072ID == (data16 & 0xFFF0) || 
            PMDA_ROM_LOAD_8073ID == (data16 & 0xFFF0)) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8072_PMA_PMD_REG(unit, pc, 0xCA1A,&data16));
            LOG_CLI((BSL_META_U(unit,
                                "8072: u=%d p=%d external rom boot complete f/w ver. "
                                "0x%x: init.\n"),
                     unit, port,data16));

        } else {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "ROM_CODE read : u=%d p=%d 0x%x Expected 0xBCB0\n"),
                      unit, port, data16));
        }

        if (pc->phy_id1 == PHY_ID1_8073) {
            uint16 dev_rev;
            uint16 snr_val;
            uint16 rom_ver;

            SOC_IF_ERROR_RETURN
                (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_CHIP_REV_REG,
                                          &dev_rev));
            SOC_IF_ERROR_RETURN
                (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN2_REG, &rom_ver));
            /*
             * Dis/En Tx to avoid Autoneg stuck for A1 devices.
             */
            if (dev_rev == PHY_8073_A1 || dev_rev == PHY_8073_A0) {
                SOC_IF_ERROR_RETURN
                   (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xCA02, 0x400, 0x400));
                sal_usleep(1000000);
                SOC_IF_ERROR_RETURN
                   (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xCA02, 0x000, 0x400));
            }

            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xCA1B, 0x1000, 0x1000));

            sal_usleep(10000);
            /*
             * SNR Improvement: Change FFE main cursor to 5.
             */
            if (APPLY_SNR_PATCH(dev_rev, rom_ver)) {
                snr_val = (dev_rev == PHY_8073_A0) ? 0xFA0C : 0xFB0C;
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xCA1B, snr_val));
            }
            SOC_IF_ERROR_RETURN /* FEC Advert Enable */
                (MODIFY_PHY8072_AN_REG(unit, pc, AN_ADVERT_2_REG, 
                                 AN_ADVERT_FEC, AN_ADVERT_FEC));
        }
    }

    SOC_IF_ERROR_RETURN /* Restart AN */
        (WRITE_PHY8072_AN_REG(unit, pc, AN_CTRL_REG, AN_ENABLE|AN_RESTART)); 

    if (pc->phy_id1 == PHY_ID1_8073) {
        sal_usleep(50000);

            /* XAUI Workaround */
        SOC_IF_ERROR_RETURN(_8073_A0_xaui_check(unit, port));
    }

    /* fix for PHY-443 */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8072_PHYXS_REG(unit, pc, 0x8111, 1 << 3, 1 << 3));

    /* Set Local Advertising Configuration */
    SOC_IF_ERROR_RETURN
        (phy_8072_ability_local_get(unit, port, &ability));
    SOC_IF_ERROR_RETURN
        (phy_8072_ability_advert_set(unit, port, &ability));
                                                                                
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "8072: u=%d port%d: init.\n"), unit, port));

    return SOC_E_NONE;
}


/*
 * Function:
 *    phy_8072_link_get
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
phy_8072_link_get(int unit, soc_port_t port, int *link)
{
    uint16      pma_mii_stat, pcs_mii_stat, link_stat,speed_val;
    phy_ctrl_t *pc;
    int an = 0,an_done = 0,cur_speed;

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_8072_an_get(unit,port,&an,&an_done);

    /* return link false if in the middle of autoneg */
    if (an == TRUE && an_done == FALSE) {
        *link = FALSE;
        return SOC_E_NONE;
    } 

    /* check if the current speed is set to 10G */
    cur_speed = 0;
    if (an) { /* autoneg is enabled */
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_AN_REG(unit, pc, AN_ETH_STATUS_REG, &speed_val));
        if (speed_val & AN_SPEED_10GBASE_KR) {
            cur_speed = 10000;
        }
    } else { /* force mode */
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG, &speed_val));
        if ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK)==PMAD_CTRL2r_PMA_TYPE_10G) {
            cur_speed = 10000;
        }
    }

    if (cur_speed == 10000) { /* check all 2 device's link status if 10G */
        /* Receive Link status */
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PCS_STATr(unit, pc, &pcs_mii_stat));
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_STATr(unit, pc, &pma_mii_stat));

    link_stat = (cur_speed == 10000)?
                 pma_mii_stat & pcs_mii_stat:
                 pma_mii_stat;

    *link = (link_stat & MII_STAT_LA) ? TRUE : FALSE;

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8072_enable_set
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
phy_8072_enable_set(int unit, soc_port_t port, int enable)
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
        (MODIFY_PHY8072_PMA_PMD_TX_DISABLEr(unit, pc, data, mask));
    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_8072_lb_set
 * Purpose:
 *    Put 8072 in PHY PCS/PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8072_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 
    uint16     tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

#if defined(PHY_8072_PCS_LOOPBACK)
    tmp = enable ? MII_CTRL_LE : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8072_PCS_CTRLr(unit, pc, tmp, MII_CTRL_LE));
#endif /* PHY_8072_PCS_LOOPBACK */

#if defined(PHY_8072_PMA_PMD_LOOPBACK)
    tmp = enable ? MII_CTRL_PMA_LOOPBACK : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8072_PMA_PMD_CTRLr(unit, pc,
                                      tmp, MII_CTRL_PMA_LOOPBACK));
#endif /* PHY_8072_PMA_PMD_LOOPBACK */

#if defined(PHY_8072_SYSTEM_LOOPBACK)

   tmp = enable? XGXS_MODE_SYSTEM_LOOPBACK | XGXS_CTRL_RLOOP
         :XGXS_MODE_NORMAL;    
   SOC_IF_ERROR_RETURN
       (MODIFY_PHY8072_PHYXS_REG(unit,pc,XGXS_MODE_CTRLr,
                     tmp,
                     XGXS_MODE_MASK | XGXS_CTRL_RLOOP)); 

#endif /* PHY_8072_SYSTEM_LOOPBACK */

#if defined(PHY_8072_XAUI_LOOPBACK)
   SOC_IF_ERROR_RETURN
        (READ_PHY8072_PHYXS_REG(unit, pc, PHYXS_MODE_CTRL_REG, &tmp));
    tmp &= ~(0xf << 8);
    tmp |= ((enable) ? 6 : 1) << 8;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PHYXS_REG(unit, pc, PHYXS_MODE_CTRL_REG, tmp));

    tmp = (enable) ? (0x00f0) : (0x0000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PHYXS_REG(unit, pc, 0x8017, tmp));
#endif /* PHY_8072_XAUI_LOOPBACK */

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8072_lb_get
 * Purpose:
 *    Get 8072 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8072_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      tmp;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

#if defined(PHY_8072_PCS_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PCS_CTRLr(unit, pc, &tmp));

    *enable = (tmp & MII_CTRL_LE) ? TRUE : FALSE;
#endif /* PHY_8072_PCS_LOOPBACK */



#if defined(PHY_8072_PMA_PMD_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_CTRLr(unit, pc, &tmp));

    *enable = (tmp & MII_CTRL_PMA_LOOPBACK) ? TRUE : FALSE;
#endif /* PHY_8072_PMA_PMD_LOOPBACK */

#if defined(PHY_8072_SYSTEM_LOOPBACK)
   SOC_IF_ERROR_RETURN
       (READ_PHY8072_PHYXS_REG(unit,pc,XGXS_MODE_CTRLr,&tmp));
   *enable = (tmp & XGXS_MODE_MASK)? FALSE:TRUE;
#endif /* PHY_8072_SYSTEM_LOOPBACK */

#if defined(PHY_8072_XAUI_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PHYXS_REG(unit, pc, PHYXS_MODE_CTRL_REG, &tmp));
    *enable = ((tmp & (0xf << 8)) == (6 << 8));
#endif /* PHY_8072_XAUI_LOOPBACK */

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_lb_get: u=%d port%d: loopback:%s\n"),
              unit, port, *enable ? "Enabled": "Disabled"));
    
    return SOC_E_NONE;
}


STATIC int
phy_8072_linkup_evt(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;    /* PHY software state */
    uint16 dev_rev;
    uint16 snr_val;
    uint16 rom_ver;
    uint16 link;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN(_8073_A0_xaui_check(unit, port));

    if (pc->phy_id1 == PHY_ID1_8072) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_CHIP_REV_REG, &dev_rev));
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_GEN2_REG, &rom_ver));
    if(APPLY_SNR_PATCH(dev_rev, rom_ver)) {
        /*
        * Change CDR Bandwidth.
        */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xCA46, 0x0333));
        /*
        * Change PLL Bandwidth.
        */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xCA1D, 0x26BC));
        /*
        * SNR Improvement.
        */
        snr_val = (dev_rev == PHY_8073_A0) ? 0xFA0C : 0xFB0C;
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xCA1B, snr_val));
    }
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_USR_STATUS_REG, &link));

    /* if autoneg to 10G, sync up the serdes's speed in case it's not done*/
    if (PMAD_STATUS_AN_10G(link)) {
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventSpeed, 10000));
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_linkup_evt: u=%d p=%d\n"),
              unit, port));
    return SOC_E_NONE;
}

STATIC int
_phy_8072_control_tx_driver_set(int unit, soc_port_t port,
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
        (MODIFY_PHY8072_TXA_ACTRL_3r(unit, pc, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_8072_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16         data16;   /* Temporary holder of a reg value */
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    data16 = 0;
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_TXA_ACTRL_3r(unit, pc, &data16));
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
 *      phy_8072_control_set
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
phy_8072_control_set(int unit, soc_port_t port,
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
        rv = _phy_8072_control_tx_driver_set(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}
/*
 * Function:
 *      phy_8072_control_get
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
phy_8072_control_get(int unit, soc_port_t port,
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
        rv = _phy_8072_control_tx_driver_get(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_8072_probe
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
phy_8072_probe(int unit, phy_ctrl_t *pc)
{
    uint16 data0 = 0;
    uint16 data1 = 0;

    if (soc_property_port_get(unit, pc->port, spn_PHY_8072, FALSE)) {
        return SOC_E_NONE;
    }

    /* device 8072 and 8073 has different id1, but uses the same driver. 
     * First check if the device's id1 matches 8073.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit,pc,PMAD_ID1_REG,&data1));

    if (data1 == PHY_ID1_8073) {

        /* PHY 8727 has the same phyid1, check register 0xc802 to find out if
         * it is a real 8073 or 8074 device.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PMA_PMD_REG(unit,pc,0xC802,&data1));
        if (data1 != 0x8073) {
            return SOC_E_NOT_FOUND;
        } else {
          /* found a 8073 */
          return SOC_E_NONE;
        }
    }

    /* 8072 has the same phy id0 and id1. The difference is the register
     * 0 device 7 reads as 0 for 87x6, 0x1000 for 8072 after powerup.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit,pc,0,&data0));

    /* needed for subsequent initialization after powerup */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_GENREG1r(unit,pc,&data1));

    /* the id after the SPI-ROM is loaded for 8072 */
    if ((data1 & 0xFFF0) == PMDA_ROM_LOAD_8072ID) {
        return SOC_E_NONE;
    }

    /* the id after the SPI-ROM is loaded for 8706 */
    if ((data1 == 0x4321) || (data1 == 0x1234)) {
        return SOC_E_NOT_FOUND;
    }

    /* powerup case */
    if (data0 == 0x1000) {
        return SOC_E_NONE;
    }

    return SOC_E_NOT_FOUND;
}

STATIC int
phy_8074_probe(int unit, phy_ctrl_t *pc)
{
    uint16 data = 0;

    /* device 8072 and 8073/8074 have different id1, but use the same driver. 
     * First check if the device's id1 matches 8074.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit,pc,PMAD_ID1_REG,&data));

    if (data == PHY_ID1_8074) {
        /* PHYs 8727/8073/8074 have the same phyid1, check register 0xc802 to find out if
         * 8074 device.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PMA_PMD_REG(unit,pc,0xC802,&data));
        if (data == 0x8074) {
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *      phy_8072_speed_set
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
phy_8072_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;
    int rv = SOC_E_NONE;
                                                                                
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_speed_set: u=%d p=%d speed=%d\n"),
                         unit, port,speed));
                                                                                
    if (!(speed == 10000 || speed == 1000 || speed==2500)) {
        return SOC_E_PARAM;
    }
                                                                                
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
                                                                                
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0x0096, 0x0000));

    if (speed == 10000 || speed == 1000) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_AN_REG(unit,pc,0x8309,(1 << 5),(1 << 5)));
    }

    if (speed == 10000) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG,
                    PMAD_CTRL2r_PMA_TYPE_10G,PMAD_CTRL2r_PMA_TYPE_MASK));
                                                                                
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_PMA_PMD_REG(unit,pc, MII_CTRL_REG,
                    MII_CTRL_SS_LSB,MII_CTRL_SS_LSB));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit,pc,0x8308,0x1C));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit, pc, AN_CTRL_REG,
                          AN_ENABLE |
                          AN_RESTART));

        sal_usleep(40000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit,pc,AN_CTRL_REG,0));

    } else if (speed == 1000) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG,
                    PMAD_CTRL2r_PMA_TYPE_1G,PMAD_CTRL2r_PMA_TYPE_MASK));
                                                                                
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_PMA_PMD_REG(unit,pc,MII_CTRL_REG,
                    0,MII_CTRL_SS_LSB));
    } else if (speed == 2500) {

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG,
                    PMAD_CTRL2r_PMA_TYPE_1G,PMAD_CTRL2r_PMA_TYPE_MASK)); 

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit, pc, AN_CTRL_REG,
                          AN_ENABLE |
                          AN_RESTART));

        sal_usleep(40000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit,pc,AN_CTRL_REG,0));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit,pc,0xffe0,0));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_AN_REG(unit,pc,0x8309,0,(1 << 5)));
        sal_usleep(10000);

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit,pc,0x8308,0x10));
    }
 
                                                                   
    /* need to set the internal phy's speed accordingly */
                                                                                
    if (NULL != int_pc) {
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
    }
    return rv;
}

/*
 * Function:
 *      phy_8072_speed_get
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
phy_8072_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;
    uint16       speed_val;
    int an = 0;
    int an_done = 0;                                        
    uint16 an_cl37 = 0;
    uint16 status = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    speed_val = 0;
    *speed = 10000;
                                                                                
    phy_8072_an_get(unit, port, &an, &an_done);
    if (pc->phy_id1 == PHY_ID1_8073) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PMA_PMD_REG(unit, pc, PMAD_USR_STATUS_REG, 
              &status));
    }

    if (an) { /* autoneg is enabled */
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_AN_REG(unit, pc, AN_ETH_STATUS_REG, &speed_val));
        if (speed_val & AN_SPEED_10GBASE_KR) {
            *speed = 10000;
        } else if (PMAD_STATUS_AN_2p5G(status)) {
            *speed = 2500;
        } else if (speed_val & AN_SPEED_1000BASE_KX) {
            *speed = 1000;
        } else {
            uint16 phy_id1;
            int link_cl37;

            SOC_IF_ERROR_RETURN
                (READ_PHY8072_PMA_PMD_REG(unit,pc,PMAD_ID1_REG,&phy_id1));

            /* check CL37 link status */
            SOC_IF_ERROR_RETURN
                (READ_PHY8072_AN_REG(unit, pc, 0x8304, &an_cl37));
            link_cl37 = an_cl37 & 0x2; 
           
            /* check if CL37 enabled. automatically enabled for 8073 */ 
            if (phy_id1 == PHY_ID1_8072) { 
                SOC_IF_ERROR_RETURN
                    (READ_PHY8072_AN_REG(unit, pc, 0xFFE0, &an_cl37));
                link_cl37 = link_cl37 && (an_cl37 & AN_ENABLE);
            }

            if (link_cl37) {
                *speed = 1000;
            }
        }                        
    } else { /* autoneg is not enabled, forced speed */

        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PMA_PMD_REG(unit,pc,PMAD_USR_STATUS_REG,&speed_val));
        if (speed_val & PMAD_STATUS_MODE_2p5G) {
                *speed = 2500;
        } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG, &speed_val));
        if ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK) ==
                                            PMAD_CTRL2r_PMA_TYPE_1G) {
            *speed = 1000;
        } else if ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK) == 
                               PMAD_CTRL2r_PMA_TYPE_10G) {
            *speed = 10000;
        }
        }
    }
                                                                       
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_speed_get: u=%d p=%d speed=%d\n"),
              unit, port, *speed));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8072_an_get
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
phy_8072_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16 an_status;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit, pc, AN_CTRL_REG, &an_status));
    *an = (an_status & AN_ENABLE)? TRUE: FALSE;

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit, pc, AN_STATUS_REG, &an_status));
    *an_done = (an_status & AN_DONE)? TRUE: FALSE;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8072_an_set
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
phy_8072_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16 phy_id1;

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit,pc,PMAD_ID1_REG,&phy_id1));
                                                                                
    if (an) {
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0x0096, 0x0002));

        if (phy_id1 == PHY_ID1_8072) {
            /* Allow Clause 37 through Clause 73 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8072_AN_REG(unit, pc, 0x8370,0x040c));
                                                                                
        }

        /* Enable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit, pc, 0xFFE0, 0x1100));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit, pc, AN_CTRL_REG, 
                          AN_EXT_NXT_PAGE |
                          AN_ENABLE |
                          AN_RESTART));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0x0096, 0x0000));

        if (phy_id1 == PHY_ID1_8072) {
            /* no Clause 37 through Clause 73 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8072_AN_REG(unit, pc, 0x8370,0x0));
                                                                                
        }  
        /* disable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit, pc, 0xFFE0, 0x0));
                                                             
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit, pc, AN_CTRL_REG, 0x0));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8072_ability_advert_get
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
phy_8072_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           pause;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
                                                                                
    pc = EXT_PHY_SW_STATE(unit, port);

    /* advert register 1 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit, pc, AN_ADVERT_1_REG, &an_adv));

    mode = 0;
    pause = 0;
    mode |= (an_adv & AN_ADVERT_10G) ? SOC_PA_SPEED_10GB : 0;
    mode |= (an_adv & AN_ADVERT_1G) ? SOC_PA_SPEED_1000MB : 0;

    /* check Full Duplex advertisement on Clause 37 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit, pc, 0xFFE4,&an_adv));
                                                                                
    mode |= (an_adv & 0x20) ? SOC_PA_SPEED_1000MB : 0;

    if (pc->phy_id1 == PHY_ID1_8073) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_AN_REG(unit, pc, 0x8329,&an_adv));
        mode |= (an_adv & 0x1) ? SOC_PA_SPEED_2500MB : 0;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit, pc, AN_ADVERT_0_REG, &pause));
                                                                                
    ability->speed_full_duplex = mode;
    ability->pause = 0;
    switch (pause & (AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM)) {
        case AN_ADVERT_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case AN_ADVERT_PAUSE_ASYM:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_ability_advert_get: u=%d p=%d speed=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8072_ability_advert_set
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
phy_8072_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{   
    uint16           an_adv;
    uint16           an_adv_cl37;
    phy_ctrl_t      *pc;                                                                                 
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
                                                                                
    pc = EXT_PHY_SW_STATE(unit, port);
                                                                                
    /*
     * Set advertised duplex (only FD supported).
     */
    an_adv =  (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 
              AN_ADVERT_1G : 0;
                                                                                
    /* write Full Duplex advertisement on Clause 37 */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_AN_REG(unit, pc, 0xFFE4,an_adv));

    an_adv =  (ability->speed_full_duplex & 
               (SOC_PA_SPEED_1000MB | SOC_PA_SPEED_2500MB)) ? 
               AN_ADVERT_1G : 0;
    an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ? 
               AN_ADVERT_10G : 0; 

    /* advert register 1 */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_AN_REG(unit, pc, AN_ADVERT_1_REG, an_adv));

    if (pc->phy_id1 == PHY_ID1_8073) {
        an_adv =  (ability->speed_full_duplex & SOC_PA_SPEED_2500MB) ? 0x1 : 0;
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8072_AN_REG(unit, pc, 0x8329, an_adv));
    }

    an_adv = an_adv_cl37 = 0;
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
        case SOC_PA_PAUSE_TX:
            an_adv = AN_ADVERT_PAUSE_ASYM;
            an_adv_cl37 = MII_ANA_C37_ASYM_PAUSE;
            break;
        case SOC_PA_PAUSE_RX:
            an_adv = AN_ADVERT_PAUSE_ASYM | AN_ADVERT_PAUSE;
            an_adv_cl37 = MII_ANA_C37_ASYM_PAUSE | MII_ANA_C37_PAUSE;
            break;
        case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
            an_adv_cl37 = MII_ANA_C37_PAUSE;
            an_adv = AN_ADVERT_PAUSE;
            break;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8072_AN_REG(unit, pc, AN_ADVERT_0_REG, an_adv,
                               AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM));
    /* write Pause advertisement on Clause 37 */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8072_AN_REG(unit, pc, 0xFFE4, an_adv_cl37,
                            MII_ANA_C37_ASYM_PAUSE | MII_ANA_C37_PAUSE));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_ability_advert_set: u=%d p=%d pause=0x%08x adv_reg1=0x%04x\n"),
              unit, port, ability->pause, an_adv));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8072_ability_local_get
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
phy_8072_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
                                                                                      
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_ability_local_get: u=%d p=%d\n"),
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
            if (pc->phy_id1 == PHY_ID1_8073) {
                ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
            }
            break;
    }

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM; 
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_ability_local_get: u=%d p=%d speed=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex,ability->pause));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8072_ability_remote_get
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
phy_8072_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint16      lp_abil, lp_abil_cl37;
    soc_port_mode_t  mode;
    int speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_ability_remote_get: u=%d p=%d\n"),
              unit, port));

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    mode = 0;
    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit, pc, AN_LP_ABILITY_1_REG, &lp_abil));
    mode |= (lp_abil & AN_ADVERT_1G) ? SOC_PA_SPEED_1000MB : 0;
    mode |= (lp_abil & AN_ADVERT_10G) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex  = mode;

    if (pc->phy_id1 == PHY_ID1_8073) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_AN_REG(unit, pc, 0x832C, &lp_abil));
        if (lp_abil &  0x1) {
            ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        }
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit, pc, AN_LP_ABILITY_0_REG, &lp_abil));

    ability->pause = 0;
    switch (lp_abil & (AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM)) {
        case AN_ADVERT_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case AN_ADVERT_PAUSE_ASYM:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    SOC_IF_ERROR_RETURN
        (phy_8072_speed_get(unit,port,&speed));

    if (speed == 1000) {  /* add clause37 attributes */
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_AN_REG(unit, pc, 0xFFE5, &lp_abil_cl37));
        ability->speed_full_duplex |= 
                   (lp_abil_cl37 & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

        switch (lp_abil_cl37 & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
            case MII_ANP_C37_PAUSE:
                ability->pause |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANP_C37_ASYM_PAUSE:
                ability->pause |= SOC_PA_PAUSE_TX;
                break;
            case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
                ability->pause |= SOC_PA_PAUSE_RX;
                break;
        }
    }
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_AN_REG(unit, pc, AN_STATUS_REG, &lp_abil));
    ability->flags     = (lp_abil & AN_LP_AN_ABILITY) ? SOC_PA_AUTONEG : 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_ability_remote_get: u=%d p=%d speed=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));

    return (SOC_E_NONE);
}

/*
 *  Function:
 *      phy_8072_rom_wait
 *
 *  Purpose:
 *      Wait for data to be written to the SPI-ROM.
 *  Input:
 *      unit, port
 */
STATIC int
phy_8072_rom_wait(int unit, int port)
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
        /*
         * Write SPI Control Register Read Command.
         */
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
                              "phy 8072 : u = %d p = %d timeout 1\n"),
                              unit, port));
        SOC_IF_ERROR_RETURN(SOC_E_TIMEOUT);
    }

    SPI_READY = 1;
    while (SPI_READY == 1) {
        /*
         * Set-up SPI Controller To Receive SPI EEPROM Status.
         */
        count = 1;
        wr_data = ((WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = SPI_CTRL_2_H;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /*
         * Fill-up SPI Transmit Fifo To check SPI Status.
         */
        count = 2;
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
        wr_data = ((1 * 0x0100) | MSGTYPE_HRD);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /*
         * Write SPI Tx Fifo Control Word-2.
         */
        wr_data = RDSR_OPCODE;
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
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /*
         * Write SPI Control Register Write Command.
         */
        count = 1;
        wr_data = ((WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /*
         * Write SPI Control -1 Register Address.
         */
        wr_data = SPI_CTRL_1_H;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /*
         * Write SPI Control -1 Register Word-2.
         */
        wr_data = 0x0103;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /*
         * Wait For 64 bytes To be written.
         */
        rd_data = 0x0000;
        soc_timeout_init(&to, WR_TIMEOUT, 0);
        do {
            count = 1;
            wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
            SOC_IF_ERROR_RETURN(write_message
                                (unit, pc, wr_data, &rd_data));
            wr_data = SPI_CTRL_1_L;
            SOC_IF_ERROR_RETURN(write_message
                                (unit, pc, wr_data, &rd_data));
            if (rd_data & 0x0100) {
                break;
            }
        } while (!soc_timeout_check(&to));
        if (!(rd_data & 0x0100)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "phy 8072 : u = %d p = %d timeout 2\n"),
                                  unit, port));
            SOC_IF_ERROR_RETURN(SOC_E_TIMEOUT);
        }
        /*
         * Write SPI Control Register Read Command.
         */
        count = 1;
        wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /*
         * Write SPI Control -1 Register Address.
         */
        wr_data = SPI_RXFIFO;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        if ((rd_data & 0x1) == 0) {
            SPI_READY = 0;
        }
    }                           /* SPI_READY */
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8072_rom_write_enable_set
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
phy_8072_rom_write_enable_set(int unit, int port, int enable)
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
    SOC_IF_ERROR_RETURN(phy_8072_rom_wait(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8072_firmware_set
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
phy_8072_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{

    uint16     rd_data;
    uint16     wr_data;
    int        j;
    int        i = 0;
    int        count;
    int     phy_8074;
    phy_ctrl_t *pc;
    uint8       spi_values[WR_BLOCK_SIZE];

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_8074 = FALSE;

    if (pc->phy_id1 == PHY_ID1_8073) {
        uint16     data;

        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PMA_PMD_REG(unit,pc,0xC802,&data));
        if ( data == 0x8074 ) {
            phy_8074 = TRUE;
            /* Enable the SPI */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xc843, 0x0000, 0x000f));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xc840, 0x0000, 0x000c));
        }
    }

    /*
     * ser_boot pin HIGH
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xCA85, 0x1, 0x1)); 
    /*
     * Read LASI Status registers To clear initial Failure status.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, 0x9004, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, 0x9005, &rd_data));

    /*
     * Enable the LASI For Message out. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0x9000, 0x0004));
    /*
     * 0x9002, bit 2 Is Rx Alarm enabled For LASI. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0x9002, 0x0004));
    /*
     * Read Any Residual Message out register.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));
    /*
     * Clear LASI Message Out Status. 
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));

    /* set SPI-ROM write enable */ 
    SOC_IF_ERROR_RETURN(phy_8072_rom_write_enable_set(unit, port, 1));

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
        SOC_IF_ERROR_RETURN(phy_8072_rom_wait(unit, port));
    }

    /* clear SPI-ROM write enable */ 
    SOC_IF_ERROR_RETURN(phy_8072_rom_write_enable_set(unit, port, 0));

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
        (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xCA85, 0x0, 0x1)); 

    if (phy_8074) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xc843, 0x000f, 0x000f));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8072_PMA_PMD_REG(unit, pc, 0xc840, 0x000c, 0x000c));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_rom_program: u=%d p=%d done\n"), unit, port));

    return SOC_E_NONE;
}

static int
write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, uint16 *rddata)
{

    uint16     tmp_data = 0;
    soc_timeout_t to;

    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8072_PMA_PMD_REG(unit, pc, 0xCA12, wrdata));

    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        SOC_IF_ERROR_RETURN
            (READ_PHY8072_PMA_PMD_REG(unit, pc, 0x9005, &tmp_data));
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
        (READ_PHY8072_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    *rddata = tmp_data;
    SOC_IF_ERROR_RETURN
        (READ_PHY8072_PMA_PMD_REG(unit, pc, 0x9003, &tmp_data));

    return SOC_E_NONE;
}

/*
 * Variable:
 *    phy_8072_drv
 * Purpose:
 *    Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_8072drv_xe = {
    "8072 10-Gigabit PHY Driver",
    phy_8072_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_8072_link_get,    /* Link get   */
    phy_8072_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_null_set,         /* Duplex set */
    phy_null_one_get,     /* Duplex get */
    phy_8072_speed_set,   /* Speed set  */
    phy_8072_speed_get,   /* Speed get  */
    phy_null_set,         /* Master set */
    phy_null_zero_get,    /* Master get */
    phy_8072_an_set,         /* ANA set */
    phy_8072_an_get,      /* ANA get */
    NULL,                 /* Local Advert set */
    NULL,                 /* Local Advert get */
    NULL,                 /* Remote Advert get */
    phy_8072_lb_set,      /* PHY loopback set */
    phy_8072_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_xehg_interface_get, /* IO Interface get */
    NULL,                   /* pd_ability, deprecated */
    phy_8072_linkup_evt,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8072_control_set,    /* phy_control_set */
    phy_8072_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_8072_probe,          /* pd_probe  */
    phy_8072_ability_advert_set,  /* pd_ability_advert_set */
    phy_8072_ability_advert_get,  /* pd_ability_advert_get */
    phy_8072_ability_remote_get,  /* pd_ability_remote_get */
    phy_8072_ability_local_get,   /* pd_ability_local_get  */
    phy_8072_firmware_set
};

phy_driver_t phy_8074drv_xe = {
    "8072 10-Gigabit PHY Driver",
    phy_8072_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_8072_link_get,    /* Link get   */
    phy_8072_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_null_set,         /* Duplex set */
    phy_null_one_get,     /* Duplex get */
    phy_8072_speed_set,   /* Speed set  */
    phy_8072_speed_get,   /* Speed get  */
    phy_null_set,         /* Master set */
    phy_null_zero_get,    /* Master get */
    phy_8072_an_set,         /* ANA set */
    phy_8072_an_get,      /* ANA get */
    NULL,                 /* Local Advert set */
    NULL,                 /* Local Advert get */
    NULL,                 /* Remote Advert get */
    phy_8072_lb_set,      /* PHY loopback set */
    phy_8072_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_xehg_interface_get, /* IO Interface get */
    NULL,                   /* pd_ability, deprecated */
    phy_8072_linkup_evt,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8072_control_set,    /* phy_control_set */
    phy_8072_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_8074_probe,          /* pd_probe  */
    phy_8072_ability_advert_set,  /* pd_ability_advert_set */
    phy_8072_ability_advert_get,  /* pd_ability_advert_get */
    phy_8072_ability_remote_get,  /* pd_ability_remote_get */
    phy_8072_ability_local_get,   /* pd_ability_local_get  */
    phy_8072_firmware_set
};

#else /* INCLUDE_PHY_8072 */
typedef int _phy_8072_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8072 */
