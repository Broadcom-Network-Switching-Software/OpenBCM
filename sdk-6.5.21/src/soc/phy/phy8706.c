
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8706.c
 * Purpose:    Phys Driver support for Broadcom 8706/8726 Serial 10Gig
 *             transceiver with XAUI interface.
 * Note:       To use this PHY driver, add phy_8706=1 config setting.
 *             PHY probing doesn't detect 8706 because the device ID is 
 *             the same as 8705.
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_8706)
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
#include "phy8706.h"

#define BCM8727_MICROCODE_LOAD_WAR

#ifdef INCLUDE_MACSEC
#include <soc/macsecphy.h>
#endif

/* typedefs */

typedef struct {
    uint16 tx_pol_ln0;
    uint16 tx_pol_ln1;
    uint16 tx_pol_ln2;
    uint16 tx_pol_ln3;
    uint16 rx_pol_ln0;
    uint16 rx_pol_ln1;
    uint16 rx_pol_ln2;
    uint16 rx_pol_ln3;
    uint16 tx_ln_swap;
    uint16 rx_ln_swap;
    uint16 preemph;
    uint16 inband_mdio;
} XGXS_CFG_t;

typedef struct {
    uint16 xgxsblk0_misccontrol1;
    uint16 rx_all_rx_control;
} PHY5942_CFG_t;

typedef struct {
    int cached_down_link;
} PHY8706_DEV_DESC_t;

/* device field macros */
#define CACHED_DOWN_LINK(_pc) (((PHY8706_DEV_DESC_t *)((_pc) + 1))->cached_down_link)

/* 
 * phy's revision info, device 1 
 * Revision     reg 0x0003      reg 0xC806
 *  A             0x6034            0x0010
 *  B0            0x6035            0x0010
 *  B1            0x6035            0x0011 
 *  C0            0x6035            0x0012 
 */

#define PHY87X6_REV_A       0x6034
#define PHY87X6_REV_BC      0x6035
#define PHY_ID1_8727        0x6036
#define PHY_ID1_8747        0x6037
#define PHY87X6_REV_ID_REG  0xc806

#define PHY_8706_PCS_LOOPBACK           1

#define IS_8727_TYPE(phyid1)    (((phyid1) == PHY_ID1_8727) || ((phyid1) == PHY_ID1_8747))

#define PHY_IS_BCM5942(_pc)     PHY_FLAGS_TST((_pc)->unit, (_pc)->port, \
                                                        PHY_FLAGS_SECONDARY_SERDES)

#if 0
#define PHY_8706_PMA_PMD_LOOPBACK       1
#define PHY_8706_SYSTEM_LOOPBACK        1
#define PHY_8706_XAUI_LOOPBACK          1
#endif

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

/* I2C related defines */
#define PHY_8727_BSC_XFER_MAX     0x1F9
#define PHY_8727_BSC_WR_MAX       16
#define PHY_8727_WRITE_START_ADDR 0x8007
#define PHY_8727_READ_START_ADDR  0x8007
#define PHY_8727_WR_FREQ_400KHZ   0x0100
#define PHY_8727_2W_STAT          0x000C
#define PHY_8727_2W_STAT_IDLE     0x0000
#define PHY_8727_2W_STAT_COMPLETE 0x0004
#define PHY_8727_2W_STAT_IN_PRG   0x0008
#define PHY_8727_2W_STAT_FAIL     0x000C
#define PHY_8727_BSC_WRITE_OP     0x22
#define PHY_8727_BSC_READ_OP      0x2
#define PHY_8727_I2CDEV_WRITE     0x1
#define PHY_8727_I2CDEV_READ      0x0
#define PHY_8727_I2C_8BIT         0
#define PHY_8727_I2C_16BIT        1     
#define PHY_8727_I2C_TEMP_RAM     0xE
#define PHY_8727_I2C_OP_TYPE(access_type,data_type) \
        ((access_type) | ((data_type) << 8))
#define PHY_8727_I2C_ACCESS_TYPE(op_type) ((op_type) & 0xff)
#define PHY_8727_I2C_DATA_TYPE(op_type)   (((op_type) >> 8) & 0xff)
#define PHY_8727_VERSION_C    7
#define PHY_8727_EDC_AUTO_CONFIG_MASK   (1<<10)
#define PHY_8727_EDC_MODE_MASK          0xff

/* SFP EEPROM defines */
#define EEPROM_I2C_ADDR 0x50
#define SFP_ID             0
#define SFP_TRCVER_CODE0   3
#define SFP_TRCVER_CODE3   6
#define SFP_TRCVER_CODE5   8
#define SFP_TYPE_SFP_PLUS  0x3
#define SFP_TRCVER_10G_ER  (1<<7)
#define SFP_TRCVER_10G_LRM (1<<6)
#define SFP_TRCVER_10G_LR  (1<<5)
#define SFP_TRCVER_10G_SR  (1<<4)
#define SFP_TRCVER_PASSIVE_CABLE  (1<<2)
#define SFP_TRCVER_ACTIVE_CABLE   (1<<3)
#define SFP_TRCVER_1000BASE_T    (1<<3)
#define SFP_TRCVER_1000BASE_CX   (1<<2)
#define SFP_TRCVER_1000BASE_LX   (1<<1)
#define SFP_TRCVER_1000BASE_SX   (1<<0)
#define SFP_8431_COMPLIANCE      (1<<0)
#define SFP_8431_LIMITED_COMPLIANCE  (1<<2)
#define SFP_8431_APPENDIX_E_COMPLIANCE  (1<<0)

#define NXT_PC(pc) ((phy_ctrl_t *)((pc)->driver_data))
#define PHYDRV_CALL_NOARG(pc,name) \
     do { \
        if (pc->driver_data) { \
            int rv; \
            phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
            EXT_PHY_SW_STATE(pc->unit,pc->port) = (phy_ctrl_t *)(pc->driver_data); \
            rv = name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port); \
            EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
            if (SOC_FAILURE(rv)) { \
                return rv; \
            } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG1(pc,name,arg0) \
    do { \
       if (pc->driver_data) { \
           int rv; \
           phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv = name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit, \
                (pc)->port,arg0); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
           if (SOC_FAILURE(rv)) { \
               return rv; \
           } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG2(pc,name,arg0,arg1) \
    do { \
       if (pc->driver_data) { \
           int rv; \
           phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv=name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port,arg0, \
                        arg1); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
           if (SOC_FAILURE(rv)) { \
               return rv; \
           } \
       } \
    } while(0)

#define PHYDRV_CALL_ARG3(pc,name,arg0,arg1,arg2) \
    do { \
       if (pc->driver_data) { \
           int rv; \
           phy_ctrl_t * tmp_pc = EXT_PHY_SW_STATE(pc->unit, pc->port); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = (phy_ctrl_t *)(pc->driver_data); \
           rv=name(((phy_ctrl_t *)(pc->driver_data))->pd,(pc)->unit,(pc)->port,arg0, \
                        arg1,arg2); \
           EXT_PHY_SW_STATE(pc->unit, pc->port) = tmp_pc; \
           if (SOC_FAILURE(rv)) { \
               return rv; \
           } \
       } \
    } while(0)

/* Local functions */
STATIC int _phy_8727_firmware_ram_set(int unit, int port, int offset, 
                                     uint8 *array,int datalen);
STATIC int _phy_8747_firmware_ram_set(int unit, int port, int offset, 
                                     uint8 *array,int datalen);
STATIC int _phy_8706_speed_set(int unit, soc_port_t port, int speed);

STATIC int phy_8706_speed_get(int unit, soc_port_t port, int *speed);

/* External functions and variables */

extern unsigned char phy8727c_ucode_bin[];
extern unsigned int phy8727c_ucode_bin_len;
extern unsigned char phy8747_ucode_bin[];
extern unsigned int phy8747_ucode_bin_len;

#define WR_TIMEOUT   1000000
static int write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, uint16 * rddata);

STATIC int _phy_8706_speed_get(int, soc_port_t, int *);
int phy_8727_i2cdev_read(int unit, soc_port_t port, int dev_addr, 
                     int offset, int nbytes,  uint8 *read_array);

STATIC int
_bcm8726_pmad_write_en(int unit, phy_ctrl_t *pc, int enable)
{
    if ( pc->phy_id1 == PHY87X6_REV_BC) {
        uint16 data;
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, PHY87X6_REV_ID_REG, &data));
        if ((data == 0x0011) || (data == 0x0012)) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8706_AN_REG(unit, pc, 0x800d, enable ? 0x400d : 0x4009));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_bcm8726_pmad_write_en: unit=%d port=%d\n"), unit, pc->port));
        }
    }
    return SOC_E_NONE;
}

/*
* Squelch enable/disable
* 1) 10G XAUI output squelching enable: 1.C842.12=1 AND 1.C842.11=0;
* 2) 1G XAUI output squelching enable: 1.C842.10=1
*/
STATIC int
_phy_8706_squelch_enable(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16 data16, mask16;
    int speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM5942(pc)) {
        /* no squelch support in reverse mode - do nothing */
        return SOC_E_NONE;
    }

    if (!IS_8727_TYPE(pc->phy_id1)) {
        /* no squelch support on non-8727/8728/8747 - do nothing */
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (phy_8706_speed_get(unit, port, &speed));

    if (speed == 10000) {
        mask16 = (1<<12);
        data16 = enable ? mask16 : 0;
        mask16 = ((1<<12) | (1<<11));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xC842, data16, mask16));
    } else {
        mask16 = (1<<10);
        data16 = enable ? mask16 : 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xC842, data16, mask16));
    }

    return SOC_E_NONE;
}

#ifndef BCM8727_MICROCODE_LOAD_WAR

STATIC int
_8727_rom_load(int unit, int port, phy_ctrl_t *pc)
{
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0001));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x008C));
    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca85, 0x0001));

    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x018A));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0188));

    /* Wait for atleast 100ms for code to download via SPI port*/
    sal_usleep(200000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca85, 0x0000));

    sal_usleep(100000);
    return SOC_E_NONE;
}

#else

STATIC int
_8727_rom_load_war(int unit, int port, phy_ctrl_t *pc)
{
    int delta1=0, delta2=0;
    uint16 status1, status2, rev, data16;
    soc_timeout_t  to;
    int            rv;

    SOC_IF_ERROR_RETURN
        (pc->read(unit, pc->phy_id, SOC_PHY_CLAUSE45_ADDR(0x1,0xc801), &rev));

    if ((rev == 0x2) || (rev == 0x3)) {
        /* revA or revB */ 
        if (pc->phy_id & 0x1) {
            /* We are in channel 2 */
            SOC_IF_ERROR_RETURN
                (pc->read(unit, pc->phy_id-1, SOC_PHY_CLAUSE45_ADDR(0x1,0xca1c), &status1));
            SOC_IF_ERROR_RETURN
                (pc->read(unit, pc->phy_id, SOC_PHY_CLAUSE45_ADDR(0x1,0xca1c), &status2));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Channel 2: PRTAD = %x status1 = %x Status2 = %x\n"),
                                 pc->phy_id, status1, status2)); 
            if ((status1 == 0x600d) && (status2 == 0x600d)) {
                return SOC_E_NONE;
            } else {
                delta2=0;
                delta1=-1;
            }
        } else {
            /* We are in channel 1 */
            SOC_IF_ERROR_RETURN
                (pc->read(unit, pc->phy_id, SOC_PHY_CLAUSE45_ADDR(0x1,0xca1c), &status1));
            SOC_IF_ERROR_RETURN
                (pc->read(unit, pc->phy_id+1, SOC_PHY_CLAUSE45_ADDR(0x1,0xca1c), &status2));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Channel 1: PRTAD = %x status1 = %x Status2 = %x\n"),
                                 pc->phy_id, status1, status2)); 
            if ((status1 == 0x600d) && (status2 == 0x600d)) {
                return SOC_E_NONE;
            } else {
                delta2=1;
                delta1=0;
            }
        } 

        /* Reset the device */

        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x1,0x0), MII_CTRL_RESET));

        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x3,0x0), MII_CTRL_RESET));

        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x4,0x0), MII_CTRL_RESET));

        /* Wait for device to come out of reset */
        soc_timeout_init(&to, 10000, 0);
        while (!soc_timeout_check(&to)) {
            rv = pc->read(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x4,0x0), &data16);
            if (((data16 & MII_CTRL_RESET) == 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        }
        if ((data16 & MII_CTRL_RESET) != 0) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8706/8726 reset failed: u=%d p=%d\n"),
                      unit, pc->port));
        }
        sal_usleep(10000);

        /* load microcode into channel 2 first */

        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x1,0xca10), 0x0001));
        sal_usleep(1000);

        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x1,0xca10), 0x008C));
        sal_usleep(1000);

        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x1,0xca85), 0x0001));

        sal_usleep(1000);
        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x1,0xca10), 0x018A));
        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x1,0xca10), 0x0188));

        /* Wait for atleast 100ms for code to download via SPI port*/
        sal_usleep(200000);

        SOC_IF_ERROR_RETURN
            (pc->write(unit, pc->phy_id+delta2, SOC_PHY_CLAUSE45_ADDR(0x1,0xca85), 0x0000));

        sal_usleep(100000);
    }

    /* now into channel 1 if revA or revB, if not do regular microcode load as delta1=0 */

    /* Reset the device */

    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x1,0x0), MII_CTRL_RESET));

    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x3,0x0), MII_CTRL_RESET));

    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x4,0x0), MII_CTRL_RESET));

    /* Wait for device to come out of reset */
    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = pc->read(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x4,0x0), &data16);
        if (((data16 & MII_CTRL_RESET) == 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((data16 & MII_CTRL_RESET) != 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8706/8726 reset failed: u=%d p=%d\n"),
                  unit, pc->port));
    }
    sal_usleep(10000);

    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x1,0xca10), 0x0001));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x1,0xca10), 0x008C));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x1,0xca85), 0x0001));

    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x1,0xca10), 0x018A));
    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x1,0xca10), 0x0188));

    /* Wait for atleast 100ms for code to download via SPI port*/
    sal_usleep(200000);

    SOC_IF_ERROR_RETURN
        (pc->write(unit, pc->phy_id+delta1, SOC_PHY_CLAUSE45_ADDR(0x1,0xca85), 0x0000));

    sal_usleep(100000);

    return SOC_E_NONE;
}

#endif

STATIC int
_8726_rom_load(int unit, int port, phy_ctrl_t *pc)
{
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca85, 0x0001));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0188));
    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x018A));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca1e, 0x73A0));
    sal_usleep(1000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0188));

    /* Wait for atleast 100ms for code to download via SPI port*/
    sal_usleep(200000);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca85, 0x0000));

    sal_usleep(1000);
    return SOC_E_NONE;
}

STATIC int
_8747_rom_load(int unit, int port, phy_ctrl_t *pc)
{
    uint16  phy_addr;
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
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x018f));

    /* Enable SPI */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xc843, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xc840, 0x0000));

    /* Remove M8051 resets */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca10, 0x0188));

    /* The rest is moved outside this routine. */

    return SOC_E_NONE;
}

#define NEW_NAME_ARRAY_SIZE 25
STATIC int
_8726_nxt_dev_probe(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t phy_ctrl;
    soc_phy_info_t phy_info;
    extern int _ext_phy_probe(int unit, soc_port_t port,soc_phy_info_t *pi, 
                              phy_ctrl_t *ext_pc);
    soc_phy_info_t *pi;
    int offset = 0;
    char new_name[NEW_NAME_ARRAY_SIZE];

    pc = EXT_PHY_SW_STATE(unit, port);

    /* initialize the phy_ctrl for the next device connected to the xaui port */
    sal_memset(&phy_ctrl, 0, sizeof(phy_ctrl_t));
    phy_ctrl.unit = unit;
    phy_ctrl.port = port;
    phy_ctrl.speed_max = pc->speed_max;
    phy_ctrl.read = pc->read;    /* use same MDIO read routine as this device's */
    phy_ctrl.write = pc->write;  /* use same MDIO write routine as this device's */ 

    /* get the mdio address of the next device */
    phy_ctrl.phy_id = soc_property_port_get(unit, port,
                                     spn_PORT_PHY_ADDR1, 0xff);

    if (phy_ctrl.phy_id == 0xff) {
        return SOC_E_NOT_FOUND;
    }

    /* probe the next device at the given address */
    SOC_IF_ERROR_RETURN
        (_ext_phy_probe(unit, port, &phy_info, &phy_ctrl));

    /* If we have probed BCM8706/8726/8727 then we won't allow 
     * to chain (physically its not possible) another 8706. The Probe depends
     * on spn_PORT_PHY_ADDR1 to probe 2nd level PHYs. It could so
     * happen that 8706/8726 could be chained with 8481. In such cases
     * probing could end up in a loop.
     */
    if ((phy_ctrl.pd == NULL) || ((phy_ctrl.phy_model == pc->phy_model) && 
                                  (phy_ctrl.phy_oui == pc->phy_oui))) {
        pc->driver_data = NULL;
        /* device not found */
        return SOC_E_NOT_FOUND;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "8726_nxt_dev_probe: found phy device"
                         " u=%d p=%d id0=0x%x id1=0x%x\n"), 
              unit, port,phy_ctrl.phy_id0,phy_ctrl.phy_id1));

    /* set the chained flag */
    PHY_FLAGS_SET(phy_ctrl.unit, phy_ctrl.port, PHY_FLAGS_CHAINED);

    /* Found device, allocate control structure */
    pc->driver_data = (void *)sal_alloc (sizeof (phy_ctrl_t) + phy_ctrl.size, phy_ctrl.pd->drv_name);
    if (pc->driver_data == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memcpy(pc->driver_data, &phy_ctrl, sizeof(phy_ctrl_t));

    /* We need to append the (chanined) PHY's name to the parent */
    pi = &SOC_PHY_INFO(unit, pc->port);
    offset = sal_strlen(pi->phy_name);
    /* size of new_name is 25 which is large enough */
    /* coverity[secure_coding] */
    sal_strcpy(new_name, (pi->phy_name));
    /* coverity[secure_coding] */
    sal_strcpy(new_name+offset, "->");
    offset = sal_strlen(new_name);
    if ((offset + sal_strlen(phy_info.phy_name)) < NEW_NAME_ARRAY_SIZE) {
        /* coverity[secure_coding] */
        sal_strcpy(new_name + offset, phy_info.phy_name);
    } else {
        return SOC_E_MEMORY;
    }
    pi->phy_name = sal_strdup(new_name);

    return SOC_E_NONE;
}
#if defined(NEW_NAME_ARRAY_SIZE)
    #undef NEW_NAME_ARRAY_SIZE
#endif

/* Function:
 *    phy_8727_opt_lvl_set
 * Purpose:
 *    Reads SOC property settings for bits whose polarity is
 *    affected by the OPINLVL and OPOUTLVL pins and flips
 *    bits as required.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 * Returns:
 *    SOC_E_NONE
 */
STATIC int
phy_8727_opt_lvl_set(int unit, soc_port_t port)
{
    phy_ctrl_t     *pc;
    int            rv;
    uint16         data16, original_data16;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_IDENTIFIERr(unit, pc, &data16));


    LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "8727: u=%d p=%d Initial value 0xc800 = 0x%x\n"),
                     unit, port, data16));

    original_data16 = data16;

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTTXENBLVL, -1);
    switch (rv) {
    case FALSE:
        data16 &= ~(1 << 15);
        break;
    case TRUE:
        data16 |= (1 << 15);
        break;
    default:    
        /* do nothing if property not set */
        break;
    }

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTTXRSTLVL, -1);
    switch (rv) {
    case FALSE:
        data16 &= ~(1 << 14);
        break;
    case TRUE:
        data16 |= (1 << 14);
        break;
    default:
        /* do nothing if property not set */
        break;
    }

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTBIASFLTLVL, -1);
    switch (rv) {
    case FALSE:
        data16 &= ~(1 << 13);
        break;
    case TRUE:
        data16 |= (1 << 13);
        break;
    default:
        /* do nothing if property not set */
        break;
    }

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTTEMPFLTLVL, -1);
    switch (rv) {
    case FALSE:
        data16 &= ~(1 << 12);
        break;
    case TRUE:
        data16 |= (1 << 12);
        break;
    default:
        /* do nothing if property not set */
        break;
    }

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTPRFLTLVL, -1);
    switch (rv) {
    case FALSE:
        data16 &= ~(1 << 11);
        break;
    case TRUE:
        data16 |= (1 << 11);
        break;
    default:
        /* do nothing if property not set */
        break;
    }

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTTXFLLVL, -1);
    switch (rv) {
    case FALSE:
        data16 &= ~(1 << 10);
        break;
    case TRUE:
        data16 |= (1 << 10);
        break;
    default:
        /* do nothing if property not set */
        break;
    }

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTRXLOSLVL, -1);
    switch (rv) {
    case FALSE:
        data16 &= ~(1 << 9);
        break;
    case TRUE:
        data16 |= (1 << 9);
        break;
    default:
        /* do nothing if property not set */
        break;
    }

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTRXFLTLVL, -1);
    switch (rv) {
    case 0:
        data16 &= ~(1 << 8);
        break;
    case 1:
        data16 |= (1 << 8);
        break;
    default:
        /* do nothing if property not set */
        break;
    }

    rv = soc_property_port_get(unit, port, spn_FORCE_OPTTXONLVL, -1);
    switch (rv) {
    case FALSE:
        data16 &= ~(1 << 7);
        break;
    case TRUE:
        data16 |= (1 << 7);
        break;
    default:
        /* do nothing if property not set */
        break;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "8727: u=%d p=%d Final value 0xc800 = 0x%x\n"),
                     unit, port, data16));
    if (data16 != original_data16) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_IDENTIFIERr(unit, pc, data16));
    }

    return SOC_E_NONE;
}


/* Function:
 *    phy_8706_init
 * Purpose:    
 *    Initialize 8706 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8706_init(int unit, soc_port_t port)
{
    uint16         phyid1;
    soc_timeout_t  to;
    int            rv;
    uint16         data16,ucode_ver;
    phy_ctrl_t     *pc;
    uint32         phy_ext_rom_boot; 
    char           *str;
    int            reg;
    uint16 rev_num;
    uint16 mask16;
    uint16      tmp;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45);

    phy_ext_rom_boot = soc_property_port_get(unit, port, 
                                            spn_PHY_EXT_ROM_BOOT, 1);
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, MII_PHY_ID1_REG, &phyid1));

#ifdef BCM8727_MICROCODE_LOAD_WAR
    if ((phyid1 != PHY_ID1_8727) || (!phy_ext_rom_boot))
#endif
    {
        /* Reset the device */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_CTRLr(unit, pc,
                                          MII_CTRL_RESET, MII_CTRL_RESET));

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PCS_CTRLr(unit, pc,
                                      MII_CTRL_RESET, MII_CTRL_RESET));

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PHYXS_CTRLr(unit, pc,
                                        MII_CTRL_RESET, MII_CTRL_RESET));

        /* Wait for device to come out of reset */
        soc_timeout_init(&to, 10000, 0);
        while (!soc_timeout_check(&to)) {
            rv = READ_PHY8706_PHYXS_CTRLr(unit, pc, &data16);
            if (((data16 & MII_CTRL_RESET) == 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        }
        if ((data16 & MII_CTRL_RESET) != 0) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8706/8726 reset failed: u=%d p=%d\n"),
                      unit, port));
        }
    }

    if (phy_ext_rom_boot) {
        if (phyid1 == PHY_ID1_8727) {
#ifdef BCM8727_MICROCODE_LOAD_WAR
            _8727_rom_load_war(unit, port,pc);
#else
            _8727_rom_load(unit, port,pc);
#endif
        } else if (phyid1 == PHY_ID1_8747) {
            _8747_rom_load(unit, port,pc);
        } else {
            _8726_rom_load(unit, port,pc);
        }
    } else {
        if (phyid1 == PHY_ID1_8727) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xc801, &rev_num));

            if ((rev_num & 0xF) == PHY_8727_VERSION_C) {
                rv = _phy_8727_firmware_ram_set(unit,port,0,phy8727c_ucode_bin,
                             phy8727c_ucode_bin_len);
                if (SOC_FAILURE(rv)) {
                    LOG_WARN(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "8706: p=%d RAM load didn't complete\n"), port));
                }
            }
        } else if (phyid1 == PHY_ID1_8747) {
            rv = _phy_8747_firmware_ram_set(unit,port,0,phy8747_ucode_bin,
                         phy8747_ucode_bin_len);
            if (SOC_FAILURE(rv)) {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "8706: p=%d RAM load didn't complete\n"), port));
            }
        }
    }

    /* read the microcode revision */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA1A,&ucode_ver));
 
    /* Make sure SPI-ROM load is complete */
    soc_timeout_init(&to, 600000, 0);
    do  {
        if (phyid1 == PHY_ID1_8727) {
            
            /* if microcode revision >  0xd101, then use the new method
             * to check the completion of the SPI-ROM loading
             */
            if (ucode_ver < 0xd101) {
                rv = (READ_PHY8706_GENREG1r(unit, pc, &data16));
                if (data16 == 0x0AA0 || SOC_FAILURE(rv)) {
                    break;
                }
            } else {
                rv = READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA1C,&data16);
                if (data16 == 0x600D || SOC_FAILURE(rv)) {
                    break;
                }
            }
        } else if (phyid1 == PHY_ID1_8747) {
            rv = READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA1C,&data16);
            if (data16 == 0x600D || SOC_FAILURE(rv)) {
                break;
            }
            /* Read from 0xca1a done outside the loop */
        } else {
            rv = (READ_PHY8706_GENREG1r(unit, pc, &data16));
            if ((data16 == 0x1234) || (data16 == 0x4321) || SOC_FAILURE(rv)) {
                break;
            }
        }
    } while (!soc_timeout_check(&to));

    if (phyid1 == PHY_ID1_8747) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xc843, 0x000f, 0x000f));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xc840, 0x000c, 0x000c));
    }

    if (soc_timeout_check(&to) || SOC_FAILURE(rv)) {
        if (phy_ext_rom_boot) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "8706: p=%d SPI-ROM load didn't complete (0x%x)\n"),
                                 port,data16));
        } else {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "8706: p=%d RAM boot didn't complete (0x%x)\n"),
                                 port,data16));
        }
    } else {
        /* re-read the microcode revision */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA1A,&ucode_ver));

        if (phy_ext_rom_boot) {
            LOG_CLI((BSL_META_U(unit,
                                "8706: u=%d p=%d external rom boot complete f/w ver. "
                                "0x%x: init.\n"), unit, port,ucode_ver));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "8706: u=%d p=%d internal RAM boot complete f/w ver. "
                                "0x%x: init.\n"), unit, port,ucode_ver));
        }
    }
    if (PHY_IS_CHAINED(unit, port) && (phyid1 == PHY_ID1_8747) 
                                   && (ucode_ver > 0x050d)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xc842, 0x1000));
    }
    SOC_IF_ERROR_RETURN
        (_bcm8726_pmad_write_en(unit, pc, FALSE));

    /* For rev B: do not initialize any registers and use the default values
     * if the configuration variable is not set 
     */

    /* Change XAUI TX Polarity */

    str = soc_property_port_get_str(unit, port, spn_PHY_TX_POLARITY_FLIP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) { 

        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port, 
                                    spn_PHY_TX_POLARITY_FLIP, 0);
        } else {  /* rev A */
            rv = 1;
        }
           
        mask16 = PHY8706_TX_POLARITY_FLIP_MASK;
        if (rv) {
            data16 = mask16;

            /* flip all lanes */
            if (rv == 1) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_TXA_CTRL_REG, 
                                             data16, mask16));

            } else { /* flip individual lanes */
                if ((rv & 0x000F) == 0x000F) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_TX0_CTRL_REG, 
                                                data16, mask16));
                }
                if ((rv & 0x00F0) == 0x00F0) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_TX1_CTRL_REG, 
                                                data16, mask16));
                }
                if ((rv & 0x0F00) == 0x0F00) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_TX2_CTRL_REG, 
                                                data16, mask16));
                }
                if ((rv & 0xF000) == 0xF000) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_TX3_CTRL_REG, 
                                                data16, mask16));
                }
            }
        } else {
            data16 = 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_TXA_CTRL_REG, 
                                                data16, mask16));
        }
    }

    /* Change XAUI RX Polarity */

    str = soc_property_port_get_str(unit, port, spn_PHY_RX_POLARITY_FLIP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_RX_POLARITY_FLIP, 0);
        } else {  /* rev A */
            rv = 0;
        }

        mask16 = PHY8706_RX_POLARITY_FLIP_MASK;
        if (rv) {
            data16 = mask16;

            /* flip all lanes */
            if (rv == 1) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_RXA_CTRL_REG, 
                                            data16, mask16));

            } else { /* flip individual lanes */
                if ((rv & 0x000F) == 0x000F) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_RX0_CTRL_REG, 
                                                data16, mask16));
                }
                if ((rv & 0x00F0) == 0x00F0) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_RX1_CTRL_REG, 
                                                data16, mask16));
                }
                if ((rv & 0x0F00) == 0x0F00) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_RX2_CTRL_REG, 
                                                data16, mask16));
                }
                if ((rv & 0xF000) == 0xF000) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_RX3_CTRL_REG, 
                                                data16, mask16));
                }
            }
        } else {
            data16 = 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8706_XS_REG(unit, pc, PHY8706_RXA_CTRL_REG, 
                                        data16, mask16));
        }
    }

    /* XAUI RX Lane Swap (Lane A, B, C, D changes to Lane D, C, B, A) */

    str = soc_property_port_get_str(unit, port, spn_PHY_XAUI_RX_LANE_SWAP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_XAUI_RX_LANE_SWAP, 0);
        } else {  /* rev A */
            rv = 1;
        }
                                                                                      
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_XS_REG(unit, pc, 0x8100, &data16));

        if (rv) {
            data16 |= (1 << 15);
        } else {
            data16 &= ~(1 << 15);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_XS_REG(unit, pc, 0x8100, data16));
    }
    
    /* XAUI TX Lane Swap (Lane A, B, C, D changes to Lane D, C, B, A) */

    str = soc_property_port_get_str(unit, port, spn_PHY_XAUI_TX_LANE_SWAP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_XAUI_TX_LANE_SWAP, 0);
        } else {  /* rev A */
            rv = 0;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_XS_REG(unit, pc, 0x8101, &data16));
        if (rv) {
            data16 |= (1 << 15);
        } else {
            data16 &= ~(1 << 15);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_XS_REG(unit, pc, 0x8101, data16));
    }

    /* Change PCS TX Polarity */

    str = soc_property_port_get_str(unit, port, spn_PHY_PCS_TX_POLARITY_FLIP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_PCS_TX_POLARITY_FLIP, 0);
        } else {  /* rev A */
            rv = 0;
        }

        if (IS_8727_TYPE(phyid1)) {
            reg = 0xcd08;
        } else {
            reg = 0xc808;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, reg, &data16));
        if (rv) {
            data16 |= (1 << 10);
        } else {
            data16 &= ~(1 << 10);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, reg, data16));
    }

    /* Change PCS RX Polarity */

    str = soc_property_port_get_str(unit, port, spn_PHY_PCS_RX_POLARITY_FLIP); 
    if ((phyid1 == PHY87X6_REV_A) || (str != NULL)) {
 
        /* always use the configuration if available */
        if (str != NULL) {
            rv = soc_property_port_get(unit, port,
                                    spn_PHY_PCS_RX_POLARITY_FLIP, 0);
        } else {  /* rev A */
            rv = 0;
        }

        if (IS_8727_TYPE(phyid1)) {
            reg = 0xcd08;
        } else {
            reg = 0xc808;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, reg, &data16));

        if (rv) {
            data16 |= (1 << 9);
        } else {
            data16 &= ~(1 << 9);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, reg, data16));
    }


    /* Make sure 8706 XAUI lanes are synchronized with the SOC XAUI */
    soc_timeout_init(&to, 100000, 0);
    while (!soc_timeout_check(&to)) {
        rv = (READ_PHY8706_PHYXS_XGXS_LANE_STATr(unit, pc, &data16));
        if (((data16 & 0x000f) == 0x000f) || SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((data16 & 0x000f) != 0x000f) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "8706: u=%d p=%d XAUI lane out of sync (0x%x)\n"),
                             unit, port, (data16 & 0x000f)));
    }

    /* default value for clause37 advertisement */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_AN_REG(unit, pc, 0xFFE4, MII_ANA_C37_FD));

    if (IS_8727_TYPE(phyid1)) {
       /* The register 0x7.0x8329 has been moved to 0x7.0x8309 
          since the microcode ver. D102. This version of microcode
          is obsolete now.
        */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x8309, 0));


        /* P_DOWN/OPTXRST1 */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_IDENTIFIERr(unit, pc, &tmp));

        if (soc_property_port_get(unit, port, spn_FORCE_OPTTXENBLVL, FALSE)) {
            tmp |= 0x8000; /* Set OPTXENB_LVL */
        } else {
            tmp &= ~0x8000;
        }
     
        if (soc_property_port_get(unit, port, spn_FORCE_OPTTXRSTLVL, TRUE)) {
            tmp |= 0x4000; /* Set OPTXRST_LVL */
        } else {
            tmp &= ~0x4000;
        }

        if (soc_property_port_get(unit, port, spn_FORCE_OPTBIASFLTLVL, TRUE)) {
            tmp |= 0x2000; /* Set OPBIASFLT_LVL */
        } else {
            tmp &= ~0x2000;
        }

        if (soc_property_port_get(unit, port, spn_FORCE_OPTTEMPFLTLVL, TRUE)) {
            tmp |= 0x1000; /* Set OPMPFLT_LVL */
        } else {
            tmp &= ~0x1000;
        }

        if (soc_property_port_get(unit, port, spn_FORCE_OPTPRFLTLVL, TRUE)) {
            tmp |= 0x0800; /* Set OPPRFLT_LVL */
        } else {
            tmp &= ~0x0800;
        }

        if (soc_property_port_get(unit, port, spn_FORCE_OPTTXFLLVL, TRUE)) {
            tmp |= 0x0400; /* Set OPTXFLT_LVL */
        } else {
            tmp &= ~0x0400;
        }

        if (soc_property_port_get(unit, port, spn_FORCE_OPTRXLOSLVL, TRUE)) {
            tmp |= 0x0200; /* Set OPRXLOS_LVL */
        } else {
            tmp &= ~0x0200;
        }

        if (soc_property_port_get(unit, port, spn_FORCE_OPTRXFLTLVL, TRUE)) {
            tmp |= 0x0100; /* Set OPRXFLT_LVL */
        } else {
            tmp &= ~0x0100;
        }

        if (soc_property_port_get(unit, port, spn_FORCE_OPTTXONLVL, TRUE)) {
            tmp |= 0x0080; /* Set OPTXON_LVL */
        } else {
            tmp &= ~0x0080;
        }

        /* P_DOWN/OPTXRST1 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_IDENTIFIERr(unit, pc, tmp));

    }

    /* fix for PHY-443 */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_XS_REG(unit, pc, 0x8111, 1 << 3, 1 << 3));

    /* Probe for external PHYs connected */
    if ((phyid1 == PHY87X6_REV_BC) || IS_8727_TYPE(phyid1)) {
        rv = _8726_nxt_dev_probe(unit, port);
        if (SOC_SUCCESS(rv)) {

            /* Initialize the next Device */
            PHYDRV_CALL_NOARG(pc,PHY_INIT);
        }
    }

    /* Enable Squelch */
    SOC_IF_ERROR_RETURN(_phy_8706_squelch_enable(unit, port, TRUE));

 
    return SOC_E_NONE;
}

STATIC int
_phy_8706_xgxs_cfg_save(int unit, soc_port_t port, XGXS_CFG_t *xgxs_cfg)
{
    phy_ctrl_t  *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, PHY8706_TX0_CTRL_REG,
                                         &xgxs_cfg->tx_pol_ln0));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, PHY8706_TX1_CTRL_REG,
                                         &xgxs_cfg->tx_pol_ln1));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, PHY8706_TX2_CTRL_REG,
                                         &xgxs_cfg->tx_pol_ln2));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, PHY8706_TX3_CTRL_REG,
                                         &xgxs_cfg->tx_pol_ln3));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, PHY8706_RX0_CTRL_REG,
                                &xgxs_cfg->rx_pol_ln0));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, PHY8706_RX1_CTRL_REG,
                                &xgxs_cfg->rx_pol_ln1));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, PHY8706_RX2_CTRL_REG,
                                &xgxs_cfg->rx_pol_ln2));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, PHY8706_RX3_CTRL_REG,
                                &xgxs_cfg->rx_pol_ln3));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, 0x8100,
                                &xgxs_cfg->rx_ln_swap));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, 0x8101,
                                &xgxs_cfg->tx_ln_swap));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_TXA_ACTRL_3r(unit, pc,
                                &xgxs_cfg->preemph));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, 0x8111,
                                &xgxs_cfg->inband_mdio));
    return SOC_E_NONE;
}

STATIC int
_phy_8706_xgxs_cfg_restore(int unit, soc_port_t port, XGXS_CFG_t *xgxs_cfg)
{
    phy_ctrl_t  *pc;
    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, PHY8706_TX0_CTRL_REG,
                                         xgxs_cfg->tx_pol_ln0));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, PHY8706_TX1_CTRL_REG,
                                         xgxs_cfg->tx_pol_ln1));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, PHY8706_TX2_CTRL_REG,
                                         xgxs_cfg->tx_pol_ln2));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, PHY8706_TX3_CTRL_REG,
                                         xgxs_cfg->tx_pol_ln3));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, PHY8706_RX0_CTRL_REG,
                                xgxs_cfg->rx_pol_ln0));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, PHY8706_RX1_CTRL_REG,
                                xgxs_cfg->rx_pol_ln1));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, PHY8706_RX2_CTRL_REG,
                                xgxs_cfg->rx_pol_ln2));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, PHY8706_RX3_CTRL_REG,
                                xgxs_cfg->rx_pol_ln3));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, 0x8100,
                                xgxs_cfg->rx_ln_swap));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, 0x8101,
                                xgxs_cfg->tx_ln_swap));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_TXA_ACTRL_3r(unit, pc,
                                xgxs_cfg->preemph));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, 0x8111,
                                xgxs_cfg->inband_mdio));
    return SOC_E_NONE;
}

STATIC int
_phy_5942_pd_status_save(int unit, soc_port_t port, PHY5942_CFG_t *phy_5942_cfg)
{
    phy_ctrl_t  *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PHYXS_XGXSBLK0_MISCCONTROL1r(unit, pc,
               &phy_5942_cfg->xgxsblk0_misccontrol1));

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PHYXS_RX_ALL_RX_CONTROLr(unit, pc,
               &phy_5942_cfg->rx_all_rx_control));

    return SOC_E_NONE;
}



/*
 * Example configuration:  
 *
 * phy_led_select_tx_control2_xe0=0x8765
 */
#define PHY_8706_TX_CONTROL_X_SET(_pc, _str,_data16, _idx, _reg_addr,_def, _mask) \
do {                                                                      \
    _str = soc_property_port_suffix_num_get_str((_pc)->unit, (_pc)->port, \
                   _idx, spn_PHY_LED_SELECT, "tx_control" );              \
    if (_str != NULL) {                                                   \
        _data16 = soc_property_port_suffix_num_get((_pc)->unit,(_pc)->port,\
                   _idx, spn_PHY_LED_SELECT, "tx_control", _def );        \
        /* some bits are reserved */                                      \
        SOC_IF_ERROR_RETURN                                               \
            (MODIFY_PHY8706_PMA_PMD_REG((_pc)->unit, (_pc),               \
                                        _reg_addr, _data16, _mask));      \
    }                                                                     \
} while(0);


STATIC int
_phy_5942_pd_status_restore(int unit, soc_port_t port, PHY5942_CFG_t *phy_5942_cfg)
{
    uint16         data16;
    char           *str;

    phy_ctrl_t  *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PHYXS_XGXSBLK0_MISCCONTROL1r(unit, pc,
               phy_5942_cfg->xgxsblk0_misccontrol1));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PHYXS_RX_ALL_RX_CONTROLr(unit, pc,
               phy_5942_cfg->rx_all_rx_control));

    if (IS_8727_TYPE(pc->phy_id1)) {

        SOC_IF_ERROR_RETURN
            (phy_8727_opt_lvl_set(unit, port));

    /* see if platform wants to change initialization value of TX Control 0 Register */
        PHY_8706_TX_CONTROL_X_SET(pc, str, data16,
                                0, 0xca01, 0x8800, 0xff00);
    
    /* see if platform wants to change initialization value of TX Control 1 Register */
        PHY_8706_TX_CONTROL_X_SET(pc, str, data16,
                                1, 0xca02, 0xe000, 0xf800);
    
    /* see if platform wants to change initialization value of TX Control 2 Register */
        PHY_8706_TX_CONTROL_X_SET(pc, str, data16,
                                2, 0xca05, 0x8000, 0x80f0);

    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_an_get
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
_phy_8706_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16 an_status;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, 0xffe0, &an_status));
    *an = (an_status & AN_ENABLE)? TRUE: FALSE;

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, 0xffe1, &an_status));
    *an_done = (an_status & AN_DONE)? TRUE: FALSE;

    return SOC_E_NONE;
}

STATIC int
phy_8706_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG2(pc, PHY_AUTO_NEGOTIATE_GET, an, an_done);
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (_phy_8706_an_get(unit, port, an, an_done));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8726_an_set
 * Purpose:
 *      Enable or disabled auto-negotiation for 8726 port
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_8726_an_set(int unit, int port, int an)
{
    phy_ctrl_t  *pc;
    XGXS_CFG_t xgxs_cfg;

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8726_an_set: u=%d p=%d an=%d\n"),
                         unit, port, an));

    /* for 8726 device, switching autoneg On/Off may reset XGXS(device 4).
     * save/restore the software configuration for XGXS device in this case.
     */
    SOC_IF_ERROR_RETURN
        (_phy_8706_xgxs_cfg_save(unit,port, &xgxs_cfg));

    if (an) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x0011, 0x0020));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x8370, 0x040c));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_CTRLr(unit, pc, 0x0040));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_AN_REG(unit, pc, 0xFFE4, 0x0020, 0x0020));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0xFFE0, 0x1100));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x0000, 0x1000));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca62, 0x0004));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca62, 0x4004));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca42, 0x0f00));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xca42, 0x0e00));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x80f1, 0x9c40));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x80f1, 0x1c40));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0x0000, 0x3200));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_CTRLr(unit, pc, 0x2040));

        /* disable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0xFFE0, 0));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, AN_CTRL_REG, 0x0));
    }

    SOC_IF_ERROR_RETURN
        (_phy_8706_xgxs_cfg_restore(unit,port, &xgxs_cfg));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_an_set
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
phy_8706_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8706_an_set: u=%d p=%d an=%d\n"),
                         unit, port, an));

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc, PHY_AUTO_NEGOTIATE_SET, an);
        an = 0;
    }

    if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
        return phy_8726_an_set(unit, port ,an);
    }

    if (an) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_CTRLr(unit, pc, 0x0040));
                                      

        /* Enable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0xFFE0, 0x1300));

        if (IS_8727_TYPE(pc->phy_id1)) {
           /* The register 0x7.0x8329 has been moved to 0x7.0x8309 
              since the microcode ver. D102. This version of microcode
              is obsolete now.
            */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8706_AN_REG(unit, pc, 0x8309, 0));
        }
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_CTRLr(unit, pc, 0x2040));

        if (IS_8727_TYPE(pc->phy_id1)) {
           /* The register 0x7.0x8329 has been moved to 0x7.0x8309 
              since the microcode ver. D102. This version of microcode
              is obsolete now.
            */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8706_AN_REG(unit, pc, 0x8309, 0x0020));
        }

        /* disable Clause 37 AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, 0xFFE0, 0));

    }


    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_ability_advert_get
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
phy_8706_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    uint16           an_adv;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_GET,ability);
        return SOC_E_NONE;
    }
                                          
    /* Clause73 advert register 1 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, 0x11, &an_adv));

    mode = 0;
    mode |= (an_adv & 0x20) ? SOC_PA_SPEED_1000MB : 0;

    /* check Full Duplex advertisement on Clause 37 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, 0xFFE4,&an_adv));
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
                         "phy_8706_ability_advert_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
* Function:
*      phy_8706_ability_remote_get
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
phy_8706_ability_remote_get(int unit, soc_port_t port, 
                            soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint16      lp_abil;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
                                           
    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ABILITY_REMOTE_GET,ability);
        return SOC_E_NONE;
    }
                                          
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_AN_REG(unit, pc, 0xFFE5, &lp_abil));
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
        (READ_PHY8706_AN_REG(unit, pc, AN_STATUS_REG, &lp_abil));
    ability->flags     = (lp_abil & 0x1) ? SOC_PA_AUTONEG : 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8706_ability_remote_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8706_ability_advert_set
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
phy_8706_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    uint16           an_adv;
    phy_ctrl_t      *pc;                                                                                  
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ABILITY_ADVERT_SET,ability);
        return SOC_E_NONE;
    }

    /*
     * Set advertised duplex (only FD supported).
     */
    an_adv =  (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 0x20 : 0;

    /* CL73 advert register 1, advertising 1G only */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_AN_REG(unit, pc, 0x11, an_adv));

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
        (MODIFY_PHY8706_AN_REG(unit, pc, 0xFFE4, an_adv, 
                              MII_ANA_C37_ASYM_PAUSE | 
                              MII_ANA_C37_PAUSE |
                              MII_ANA_C37_FD |
                              MII_ANA_C37_HD ));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8706_ability_advert_set: u=%d p=%d pause=0x%08x adv_reg1=0x%04x\n"),
              unit, port, ability->pause, an_adv));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_ability_local_get
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
phy_8706_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);


    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8706_ability_local_get: u=%d p=%d\n"),
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
    ability->flags     = SOC_PA_AUTONEG;
                                                                               
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8706_ability_local_get: u=%d p=%d speed=0x%x\n"),
              unit, port, ability->speed_full_duplex));
                                                                               
    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_8706_link_get
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
_phy_8706_link_get(int unit, soc_port_t port, int *link)
{
    uint16      pma_mii_stat, pcs_mii_stat, pxs_mii_stat, link_stat;
    phy_ctrl_t *pc;
    int cur_speed = 0;
    int an = 0,an_done = 0;
    uint16 speed_val; 

    pc = EXT_PHY_SW_STATE(unit, port);

    _phy_8706_an_get(unit,port,&an,&an_done);

    /* return link false if in the middle of autoneg */
    if (an == TRUE && an_done == FALSE) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    /* AN only supports 1G */
    if (!an) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG, &speed_val));
        if ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK) !=
                                           PMAD_CTRL2r_PMA_TYPE_1G_KX) {
            cur_speed = 10000;
        }
    } else {
        cur_speed = 1000;
    }

    if (cur_speed == 10000) { /* check all 3 device's link status if 10G */

        /* Receive Link status */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PCS_STATr(unit, pc, &pcs_mii_stat));

        /* Transmit Link status */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PHYXS_STATr(unit, pc, &pxs_mii_stat));

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_STATr(unit, pc, &pma_mii_stat));

        link_stat = pma_mii_stat & pcs_mii_stat & pxs_mii_stat;

        *link = (link_stat == 0xffff) ? FALSE :
                (link_stat & MII_STAT_LA) ? TRUE : FALSE;
    } else {
        /* in 1G mode, the PMA/PMD and PCS status register's link bit always
         * show link down.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_AN_REG(unit, pc, AN_1G_STATUS_REG, &link_stat));
        if ((link_stat != 0xffff) && (link_stat & AN_1G_LINKUP) &&
            (!(link_stat & AN_1G_LINK_CHANGE)) ) {
            *link = TRUE;
        } else {
            *link = FALSE;
        }
    } 

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_8706_link_get: u=%d port%d: link:%s\n"),
                 unit, port, *link ? "Up": "Down"));

    return SOC_E_NONE;
}

STATIC int
_phy_5942_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc; 
    uint16      xgxsstatus1, status1000x1;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PHYXS_GP_STATUS_XGXSSTATUS1r(unit, pc, &xgxsstatus1));

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PHYXS_GP_STATUS_STATUS1000X1r(unit, pc, &status1000x1));

    if (xgxsstatus1 == 0xffff) {
      *link = 0;
    } else {
      *link =  ((xgxsstatus1 & (1U << 9)) | (status1000x1 & (1U << 1))) ? 1 : 0;
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_5942_link_get: u=%d port%d: link:%s\n"),
                 unit, port, *link ? "Up": "Down"));

    return SOC_E_NONE;
}


STATIC int
phy_8706_link_get(int unit, soc_port_t port, int *link)
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

    if (PHY_IS_BCM5942(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_5942_link_get(unit, port, link));
    } else {
        int down_link;
        if (NXT_PC(pc)) {
            PHYDRV_CALL_ARG1(pc, PHY_LINK_GET, &down_link);
            /* assumes that the above routine returns a latched link status */

            if (down_link && (down_link != CACHED_DOWN_LINK(pc))) {
                int down_speed;
                PHYDRV_CALL_ARG1(pc, PHY_SPEED_GET, &down_speed);
                SOC_IF_ERROR_RETURN
                    (_phy_8706_speed_set(unit, port, down_speed));
            }
            CACHED_DOWN_LINK(pc) = down_link;
            if ( !down_link ) {
                *link = FALSE;
                return SOC_E_NONE;
            }
        }

        SOC_IF_ERROR_RETURN
            (_phy_8706_link_get(unit, port, link));
    }

    return SOC_E_NONE;

}


/*
 * Function:
 *    phy_8706_enable_set
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
_phy_8706_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_TX_DISABLEr(unit, pc, enable ? 0 : (1U << 0), (1U << 0)));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMAD_OPTICAL_CFG_REGr(unit, pc, enable ? 0 : (1 << 12), (1 << 12)));
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMAD_OPTICAL_CFG_REGr(unit, pc, enable ? 0 : (1 << 4), (1 << 4)));

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_ENABLE_SET,enable);
    }

    return (SOC_E_NONE);
}

STATIC int
_phy_5942_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    /* disable the Tx */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PHYXS_XGXSBLK0_MISCCONTROL1r(unit, pc,
               enable? 0:XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK,
               XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK));

    /* disable the Rx */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PHYXS_RX_ALL_RX_CONTROLr(unit, pc,
               enable? 0:DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK,
               DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK));

    return SOC_E_NONE;
}

STATIC int
phy_8706_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    if (PHY_IS_BCM5942(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_5942_enable_set(unit, port, enable));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_8706_enable_set(unit, port, enable));
    }

    if (enable) {
        sal_usleep(50);
        SOC_IF_ERROR_RETURN(_phy_8706_squelch_enable(unit, port, TRUE));
    }

    return SOC_E_NONE;

}


/*
 * Function:
 *    phy_8706_lb_set
 * Purpose:
 *    Put 8706 in PHY PCS/PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_8706_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 
    uint16     tmp;
    int  speed;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_LOOPBACK_SET,enable);
        return SOC_E_NONE;
    }


    /* loopback may not supported in the passthru mode on this phy.
     * So loopback from internal phy
     */

    SOC_IF_ERROR_RETURN
        (_phy_8706_speed_get(unit,port,&speed));

    if (speed < 10000) {
        phy_ctrl_t    *int_pc;   /* PHY software state */
        int rv;
                                                               
        int_pc = INT_PHY_SW_STATE(unit, port);
                                                                                
        if (NULL != int_pc) {
                rv = (PHY_LOOPBACK_SET(int_pc->pd, unit, port, enable));
            } else {
                rv = SOC_E_INTERNAL;
            }
    return rv;
    }

#if defined(PHY_8706_PCS_LOOPBACK)
    if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
        /* power down PCS RX clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xc808, (1<<13), (1<<13)));
    }

    /* lock detect bypass */
    if (IS_8727_TYPE(pc->phy_id1)) {

        tmp = enable ? (1<<14) : 0;

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xCA7B, tmp, (1<<14)));
    }

    tmp = enable ? MII_CTRL_LE : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PCS_CTRLr(unit, pc, tmp, MII_CTRL_LE));

    if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
        /* turn on PCS RX clock */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xc808, 0 , (1<<13)));
    }
#endif /* PHY_8706_PCS_LOOPBACK */

#if defined(PHY_8706_PMA_PMD_LOOPBACK)
    tmp = enable ? MII_CTRL_PMA_LOOPBACK : 0;

    SOC_IF_ERROR_RETURN
        (_bcm8726_pmad_write_en(unit,pc, TRUE));
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_CTRLr(unit, pc,
                                      tmp, MII_CTRL_PMA_LOOPBACK));
    SOC_IF_ERROR_RETURN
        (_bcm8726_pmad_write_en(unit, pc, FALSE));
#endif /* PHY_8706_PMA_PMD_LOOPBACK */

#if defined(PHY_8706_SYSTEM_LOOPBACK)

   tmp = enable? XGXS_MODE_SYSTEM_LOOPBACK | XGXS_CTRL_RLOOP
         :XGXS_MODE_NORMAL;
   SOC_IF_ERROR_RETURN
       (MODIFY_PHY8706_XS_REG(unit,pc,XGXS_MODE_CTRLr,
                     tmp,
                     XGXS_MODE_MASK | XGXS_CTRL_RLOOP));

#endif /* PHY_8706_SYSTEM_LOOPBACK */

#if defined(PHY_8706_XAUI_LOOPBACK)
   SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, 0x8000, &tmp));
    tmp &= ~(0xf << 8);
    tmp |= ((enable) ? 6 : 1) << 8;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, 0x8000, tmp));

    tmp = (enable) ? (0x00f0) : (0x0000);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_XS_REG(unit, pc, 0x8017, tmp));
#endif /* PHY_8706_XAUI_LOOPBACK */

    return SOC_E_NONE;
}

STATIC int
_phy_5942_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_XS_REG(unit, pc, 0, enable ? (1U << 14) : 0, (1U << 14)));

    return SOC_E_NONE;
}

STATIC int
phy_8706_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Disable(Enable) Squelch when we Enable(Disable) loopback */
    SOC_IF_ERROR_RETURN
        (_phy_8706_squelch_enable(unit, port, enable ? FALSE : TRUE));

    if (PHY_IS_BCM5942(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_5942_lb_set(unit, port, enable));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_8706_lb_set(unit, port, enable));
    }

    return SOC_E_NONE;

}

/*
 * Function:
 *    phy_8706_lb_get
 * Purpose:
 *    Get 8706 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_8706_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      tmp;
    phy_ctrl_t *pc;
    int  speed;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_LOOPBACK_GET,enable);
        return SOC_E_NONE;
    }


    SOC_IF_ERROR_RETURN
        (_phy_8706_speed_get(unit,port,&speed));

    if (speed < 10000) {
        phy_ctrl_t    *int_pc;   /* PHY software state */
        int rv;
                                                                                
        int_pc = INT_PHY_SW_STATE(unit, port);
                                                                                
        if (NULL != int_pc) {
                rv = (PHY_LOOPBACK_GET(int_pc->pd, unit, port, enable));
            } else {
                rv = SOC_E_INTERNAL;
            }
    return rv;
    }

#if defined(PHY_8706_PCS_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PCS_CTRLr(unit, pc, &tmp));

    /* ctrl != 0xffff check is to handle removable PHY daughter cards */
    *enable = ((tmp != 0xffff) && (tmp & MII_CTRL_LE)) ? TRUE : FALSE;
#endif /* PHY_8706_PCS_LOOPBACK */

#if defined(PHY_8706_PMA_PMD_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_CTRLr(unit, pc, &tmp));

    /* ctrl != 0xffff check is to handle removable PHY daughter cards */
    *enable = ((tmp != 0xffff) && (tmp & MII_CTRL_PMA_LOOPBACK)) ? TRUE : FALSE;
#endif /* PHY_8706_PMA_PMD_LOOPBACK */

#if defined(PHY_8706_SYSTEM_LOOPBACK)
   SOC_IF_ERROR_RETURN
       (READ_PHY8706_XS_REG(unit,pc,XGXS_MODE_CTRLr,&tmp));
    /* ctrl == 0xffff check is to handle removable PHY daughter cards */
   *enable = ((tmp == 0xffff) || (tmp & XGXS_MODE_MASK))? FALSE:TRUE;

#endif /* PHY_8706_SYSTEM_LOOPBACK */

#if defined(PHY_8706_XAUI_LOOPBACK)
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, 0x8000, &tmp));
    /* ctrl != 0xffff check is to handle removable PHY daughter cards */
    *enable =  ((tmp != 0xffff) && ((tmp & (0xf << 8)) == (6 << 8)));
#endif /* PHY_8706_XAUI_LOOPBACK */

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8706_lb_get: u=%d port%d: loopback:%s\n"),
              unit, port, *enable ? "Enabled": "Disabled"));
    
    return SOC_E_NONE;
}

STATIC int
_phy_5942_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc; 
    uint16 tmp;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_XS_REG(unit, pc, 0, &tmp));

    *enable = tmp & (1U << 14) ? TRUE : FALSE;

    return SOC_E_NONE;
}

STATIC int
phy_8706_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM5942(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_5942_lb_get(unit, port, enable));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_8706_lb_get(unit, port, enable));
    }

    return SOC_E_NONE;

}

STATIC int phy_8706_linkup(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_NOARG(pc, PHY_LINKUP_EVT);
    }

    return SOC_E_NONE;
}

STATIC int
_phy_8706_bit_order_msb2lsb(uint16 data, int len)
{
    int i;
    uint16 new_data = 0;

    for (i = len - 1; i >= 0; i--) {
        if (data & (1 << (len - 1 - i))) {
            new_data += (1 << i);
        }
    }
    return new_data;
}


STATIC int
_phy_8706_control_tx_driver_set(int unit, soc_port_t port,
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
         if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
             data = _phy_8706_bit_order_msb2lsb(data,4);
         }
         data = data << 12;
         mask = 0xf000;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data = (uint16)(value & 0xf);
         if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
             data = _phy_8706_bit_order_msb2lsb(data,4);
         }
         data = data << 8;
         mask = 0x0f00;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data = (uint16)(value & 0xf);
         if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
             data = _phy_8706_bit_order_msb2lsb(data,4);
         }
         data = data << 4;
         mask = 0x00f0;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_TXA_ACTRL_3r(unit, pc, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_8706_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16         data16;   /* Temporary holder of a reg value */
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    data16 = 0;
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_TXA_ACTRL_3r(unit, pc, &data16));
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
             *value = _phy_8706_bit_order_msb2lsb((data16 & 0xf000) >> 12,4);
         } else {
             *value = (data16 & 0xf000) >> 12;
         }
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
             *value = _phy_8706_bit_order_msb2lsb((data16 & 0x0f00) >> 8,4);
         } else {
             *value = (data16 & 0x0f00) >> 8;
         }
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
             *value = _phy_8706_bit_order_msb2lsb((data16 & 0x00f0) >> 4,4);
         } else {
             *value = (data16 & 0x00f0) >> 4;
         }
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}


STATIC int
_phy_8706_edc_mode_select(int unit, soc_port_t port, int *mode) 
{
    int edc_mode;
    uint8          eeprom[128];

    /* read module's eeprom */
    SOC_IF_ERROR_RETURN
        (phy_8727_i2cdev_read(unit,port,EEPROM_I2C_ADDR,0,128,eeprom));

    /* checks whether the module is either SFP or SFP+, addr0=(03h). */
    if (eeprom[SFP_ID] == SFP_TYPE_SFP_PLUS) {
        if (eeprom[SFP_TRCVER_CODE0] & SFP_TRCVER_10G_LRM) {
            /* set edc mode 22 */
            edc_mode = 0x22;
        } else if (eeprom[SFP_TRCVER_CODE0] & (SFP_TRCVER_10G_ER |
              SFP_TRCVER_10G_LR | SFP_TRCVER_10G_SR)) {
            /* set edc mode 44 */
            edc_mode = 0x44;
        } else {
            /* go check for CX1 cable */
            if ((eeprom[SFP_TRCVER_CODE5] & SFP_TRCVER_PASSIVE_CABLE) &&
                (eeprom[60] & SFP_8431_COMPLIANCE)) {
                    /* set mode to 55 */
                    edc_mode = 0x55;
            } else if ((eeprom[SFP_TRCVER_CODE5] & SFP_TRCVER_PASSIVE_CABLE) &&
                       !(eeprom[60] & SFP_8431_COMPLIANCE) &&
                       (eeprom[12] > 99)) {
                    /* set EDC 55 */
                    edc_mode = 0x55;
            } else if ((eeprom[SFP_TRCVER_CODE5] & SFP_TRCVER_ACTIVE_CABLE) &&
                (eeprom[60] & SFP_8431_LIMITED_COMPLIANCE)) {
                    /* set EDC 44 */
                    edc_mode = 0x44;
            } else if ((eeprom[SFP_TRCVER_CODE5] & SFP_TRCVER_ACTIVE_CABLE) &&
                (eeprom[60] & SFP_8431_APPENDIX_E_COMPLIANCE)) {
                    /* set EDC 55 */
                    edc_mode = 0x55;
            } else {
                /* check for 1G module */
                if (eeprom[SFP_TRCVER_CODE3] & (SFP_TRCVER_1000BASE_T |
                    SFP_TRCVER_1000BASE_CX | SFP_TRCVER_1000BASE_LX |
                    SFP_TRCVER_1000BASE_SX)) {
                    /* set mode 11 */
                    edc_mode = 0x11;
                } else {
                    /* set mode to 55 */
                    edc_mode = 0x55;
                }
            }
        }
    } else {
        /* set EDC mode 55 (CX1 passive) */
        edc_mode = 0x55;
    }
    *mode = edc_mode;
    return SOC_E_NONE;
}

STATIC int
_phy_8706_control_edc_mode_set(int unit, soc_port_t port, uint32 value)
{
    uint16         data16;   
    uint16         mask16;   
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    /* EDC mode programming sequence*/
    mask16 = 1 << 9;

    /* induce LOS condition: toggle register bit 0xc800.9 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit,pc, 0xC800,&data16));
    /* only change toggled bit 9 */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_REG(unit,pc, 0xC800,~data16,mask16));

    /* program EDC mode */
    SOC_IF_ERROR_RETURN
      (MODIFY_PHY8706_PMA_PMD_REG(unit,pc, 0xCA1A,(uint16)value,
                                  PHY_8727_EDC_MODE_MASK));

    /* remove LOS condition: restore back original value of bit 0xc800.9 */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_REG(unit,pc, 0xC800,data16,mask16));

    return SOC_E_NONE;
}

STATIC int
_phy_8706_control_edc_mode_get(int unit, soc_port_t port, uint32 *value)
{
    uint16         data16;
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    /* program EDC mode */
    SOC_IF_ERROR_RETURN
      (READ_PHY8706_PMA_PMD_REG(unit,pc, 0xCA1A,&data16));

    *value = data16 & PHY_8727_EDC_MODE_MASK;  
    return SOC_E_NONE; 
}

STATIC int
_phy_8706_control_edc_config(int unit, soc_port_t port, uint32 value)
{
    uint16         data16;   
    uint16         mask16;   
    phy_ctrl_t    *pc;       /* PHY software state */
    int edc_mode = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

   
    mask16 = PHY_8727_EDC_AUTO_CONFIG_MASK;
    if (value == SOC_PHY_CONTROL_EDC_CONFIG_NONE) {

        /* disable hardware EDC auto config */
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc, 0xC82B,data16,mask16));
        return SOC_E_NONE;

    } else if (value == SOC_PHY_CONTROL_EDC_CONFIG_HARDWARE) {

       /* enable hardware EDC auto configuration */
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc, 0xC82B,data16,mask16));
        return SOC_E_NONE;

    } else if (value == SOC_PHY_CONTROL_EDC_CONFIG_SOFTWARE) {

        /* disable auto config, perform software EDC config */
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc, 0xC82B,data16,mask16));

        /* figure out the proper EDC mode based on module's type */
        SOC_IF_ERROR_RETURN
            (_phy_8706_edc_mode_select(unit,port,&edc_mode));

        SOC_IF_ERROR_RETURN
            (_phy_8706_control_edc_mode_set(unit,port,edc_mode));
        return SOC_E_NONE;

    }  else {
        return SOC_E_PARAM;
    }
}

STATIC int
_phy_8706_control_edc_config_get(int unit, soc_port_t port, uint32 *value)
{
    uint16         data16;   
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit,pc, 0xC82B,&data16));
    if (data16 & PHY_8727_EDC_AUTO_CONFIG_MASK) {
        *value = SOC_PHY_CONTROL_EDC_CONFIG_HARDWARE;
    } else {
        *value = SOC_PHY_CONTROL_EDC_CONFIG_NONE;
    }
    return SOC_E_NONE;
}
STATIC int
_phy_8706_control_linkdown_transmit_set(int unit, soc_port_t port, 
                                         uint32 value)
{
    uint16        data;
    uint16        speed, speed_val;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG, &speed_val));
    speed = ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK) == 
            PMAD_CTRL2r_PMA_TYPE_1G_KX) ? 1000 : 10000;


    if (speed == 10000) {
        /* link partner will not receive due to fault detection */
        return SOC_E_UNAVAIL;
    }

    if (value) {
        SOC_IF_ERROR_RETURN
            (PHY8706_REG_READ(unit, pc, 
                              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 
                                                    0x8301),
                              &data));
        data |= 0x0020;
        SOC_IF_ERROR_RETURN
            (PHY8706_REG_WRITE(unit, pc, 
                              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 
                                                    0x8301),
                              data));

        SOC_IF_ERROR_RETURN
            (PHY8706_REG_READ(unit, pc, 
                              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_AN, 
                                                    0x8301),
                              &data));
        data |= 0x0020;
        SOC_IF_ERROR_RETURN
            (PHY8706_REG_WRITE(unit, pc, 
                              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_AN, 
                                                    0x8301),
                              data));
    } else {
        
        SOC_IF_ERROR_RETURN
            (PHY8706_REG_READ(unit, pc, 
                              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 
                                                    0x8301),
                              &data));
        data &= ~0x0020;
        SOC_IF_ERROR_RETURN
            (PHY8706_REG_WRITE(unit, pc, 
                              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 
                                                    0x8301),
                              data));

        SOC_IF_ERROR_RETURN
            (PHY8706_REG_READ(unit, pc, 
                              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_AN, 
                                                    0x8301),
                              &data));
        data &= ~0x0020;
        SOC_IF_ERROR_RETURN
            (PHY8706_REG_WRITE(unit, pc, 
                              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_AN, 
                                                    0x8301),
                              data));

    }

    return SOC_E_NONE;
}

STATIC int
_phy_8706_control_linkdown_transmit_get(int unit, soc_port_t port, 
                                        uint32 *value)
{
    uint16        data;
    uint16        speed, speed_val;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG, &speed_val));
    speed = ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK) == 
            PMAD_CTRL2r_PMA_TYPE_1G_KX) ? 1000 : 10000;

    if (speed == 10000) {
        /* 10G transmits regardless of link */
        return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN
        (PHY8706_REG_READ(unit, pc, 
                          SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 
                                                0x8301),
                          &data));
    *value = (data & 0x0020) ? 1 : 0;

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8706_control_set
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
phy_8706_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG2(pc, PHY_CONTROL_SET, type, value);
        return SOC_E_NONE;
    }

    PHY_CONTROL_TYPE_CHECK(type);

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8706_control_tx_driver_set(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_EDC_CONFIG:
        rv = _phy_8706_control_edc_config(unit,port,value);
        break;
    case SOC_PHY_CONTROL_EDC_MODE:
        rv = _phy_8706_control_edc_mode_set(unit,port,value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_8706_control_linkdown_transmit_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xCA23, value ? (0x1 << 10) : 0, (0x1 << 10)));
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
        if (pc->phy_id1 == PHY_ID1_8747) {
            uint16 freq;
            switch (value) {
            case 125000:
                freq = 0x1;
                break;
            case 195312:
                freq = 0x2;
                break;
            case 156250:
                freq = 0x3;
                break;
            default:
                freq = 0x0;
                break;
            } 
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xCACE, (freq << 14), (0x3 << 14)));
            rv = SOC_E_NONE;
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}
/*
 * Function:
 *      phy_8706_control_get
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
phy_8706_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t    *pc;
    uint16 temp16;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG2(pc, PHY_CONTROL_GET, type, value);
        return SOC_E_NONE;
    }

    PHY_CONTROL_TYPE_CHECK(type);

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_8706_control_tx_driver_get(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_EDC_MODE:
        rv = _phy_8706_control_edc_mode_get(unit,port,value);
        break;
    case SOC_PHY_CONTROL_EDC_CONFIG:
        rv = _phy_8706_control_edc_config_get(unit,port,value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_8706_control_linkdown_transmit_get(unit, port, value);
        break;
   case SOC_PHY_CONTROL_CLOCK_ENABLE:
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA23, &temp16));
        *value = temp16 & (0x1<<10) ? TRUE : FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
        if (pc->phy_id1 == PHY_ID1_8747) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCACE, &temp16));
            switch ((temp16 >> 14) & 0x3) {
            case 0x1:
                *value = 125000;
                break;
            case 0x2:
                *value = 195312;
                break;
            case 0x3:
                *value = 156250;
                break;
            default:
                *value = 0x0;
                break;
            } 
            rv = SOC_E_NONE;
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}


/*
 * Function:
 *      phy_8706_speed_set
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
_phy_8706_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;
    XGXS_CFG_t xgxs_cfg;
    PHY5942_CFG_t phy5942_cfg;
    int rv = SOC_E_NONE;
    int type;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port); 

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8706_speed_set: u=%d p=%d speed=%d\n"), 
                         unit, port,speed));

    if (!(speed == 10000 || speed == 1000 || speed == 2500)) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    /* for 8726 device, speed change will reset XGXS(device 4). 
     * save/restore the software configuration for XGXS device
     */
    if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
        SOC_IF_ERROR_RETURN
            (_phy_8706_xgxs_cfg_save(unit,port, &xgxs_cfg));
    }
    if (PHY_IS_BCM5942(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_5942_pd_status_save(unit,port, &phy5942_cfg));
    }
    
    if (speed == 10000) {
        if (IS_8727_TYPE(pc->phy_id1)) {
            type = PMAD_CTRL2r_PMA_TYPE_10G_LRM;
        } else {
            type = PMAD_CTRL2r_PMA_TYPE_10G_KR;
        }
        SOC_IF_ERROR_RETURN
            (_bcm8726_pmad_write_en(unit, pc, TRUE));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc, MII_CTRL_REG,
                    MII_CTRL_SS_LSB,MII_CTRL_SS_LSB));

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG,
                    type,PMAD_CTRL2r_PMA_TYPE_MASK));
        SOC_IF_ERROR_RETURN
            (_bcm8726_pmad_write_en(unit, pc, FALSE));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit, pc, AN_CTRL_REG,
                          AN_ENABLE |
                          AN_RESTART));

        sal_usleep(40000);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_AN_REG(unit,pc, AN_CTRL_REG, 0x0));

    } else {
        SOC_IF_ERROR_RETURN
            (_bcm8726_pmad_write_en(unit, pc, TRUE));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG,
                    PMAD_CTRL2r_PMA_TYPE_1G_KX,PMAD_CTRL2r_PMA_TYPE_MASK));

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit,pc,MII_CTRL_REG,
                    0,MII_CTRL_SS_LSB));
        SOC_IF_ERROR_RETURN
            (_bcm8726_pmad_write_en(unit, pc, FALSE));
    }

    if (pc->phy_id1 == PHY87X6_REV_A || pc->phy_id1 == PHY87X6_REV_BC) {
        SOC_IF_ERROR_RETURN
            (_phy_8706_xgxs_cfg_restore(unit,port, &xgxs_cfg));
    }
    if (PHY_IS_BCM5942(pc)) {
        sal_usleep(40000);
        SOC_IF_ERROR_RETURN
            (_phy_5942_pd_status_restore(unit,port, &phy5942_cfg));
    }

    /* need to set the internal phy's speed accordingly */

    if (NULL != int_pc) {
        if (PHY_IS_BCM5942(pc)) {
            SOC_IF_ERROR_RETURN
                (PHY_INTERFACE_SET(int_pc->pd, unit, port, SOC_PORT_IF_SFI));
            SOC_IF_ERROR_RETURN
                (PHY_AUTO_NEGOTIATE_SET (int_pc->pd, unit, port, 0));
        }
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
    }

#if 0
    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_SPEED_SET,speed);
    }
#endif

    /* Enable Squelch */
    SOC_IF_ERROR_RETURN(_phy_8706_squelch_enable(unit, port, TRUE));

    return rv;
}

STATIC int
phy_8706_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_8706_speed_set (unit, port, speed));
    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_SPEED_SET,speed);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_speed_get
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
_phy_8706_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;
    uint16       speed_val;
    int an = 0;
    int an_done = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_SPEED_GET,speed);
        return SOC_E_NONE;
    }

    _phy_8706_an_get(unit,port,&an,&an_done);

    *speed = 10000;
    if (an) { /* autoneg is enabled */
        if (an_done) { 
            *speed = 1000;  /* can only do 1G autoneg */
        }
    } else { /* autoneg is not enabled, forced speed */

        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit,pc,PMAD_CTRL2_REG, &speed_val));
        if ((speed_val & PMAD_CTRL2r_PMA_TYPE_MASK) ==
                                                PMAD_CTRL2r_PMA_TYPE_1G_KX) {
            *speed = 1000;
        } else {
            *speed = 10000;
        }
    }    
    return SOC_E_NONE;
}

STATIC int
phy_8706_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NXT_PC(pc)) {
        PHYDRV_CALL_ARG1(pc,PHY_SPEED_GET,speed);
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (_phy_8706_speed_get(unit, port, speed));

    return SOC_E_NONE;
}
/*
 * Function:
 *      _phy_8747_firmware_ram_set
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
_phy_8747_firmware_ram_set(int unit, int port, int offset, uint8 *array,int datalen)
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
     * bit 15, 0 MDIO for serial boot. bit 14, 1 serial boot, 
     * bit 13, 0, clear download done which is set by uC 
     */
    mask16 = (1 << 13) | (1 << 14) | (1 << 15);
    data16 = 1 << 14;
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, PHY8706_PMAD_SPA_CTRL_REG,
                                        data16,mask16));

    /* Reset, to download code from MDIO */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_CTRLr(unit, pc, &data16));

    data16 |= 0x8000;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_CTRLr(unit, pc, data16));

    sal_usleep(3000); /* Wait for 3ms */

    /* Write Starting Address, where the Code will reside in SRAM */
    data16 = 0xC000;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc,
                              PHY8706_PMAD_M8051_MSGIN_REG, data16));
    sal_udelay(20); /* Wait for 15us */

    /* Write SPI SRAM Count Size */
    data16 = (datalen)/2;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc,
                              PHY8706_PMAD_M8051_MSGIN_REG, data16));
    sal_udelay(20); /* Wait for 15us */

    /* Fill in the SPI RAM */
    for (j = 0; j < datalen/2; j++) {
        data16 = (array[2*j] << 8) | array[2*j+1];
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc,
                               PHY8706_PMAD_M8051_MSGIN_REG, data16));

        /* Make sure the word is read by the Micro */
        sal_udelay(20); /* Wait for 15us */
    }

    /* Read Hand-Shake message (Done) from Micro */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc,
                                 PHY8706_PMAD_M8051_MSGOUT_REG, &data16));

    if (data16 != 0x4321) {
        /* Download failure */
        LOG_CLI((BSL_META_U(unit,
                            "_phy8706_mdio_firmware_download fail,p=%d done msg 0x%x\n"),
                 port,data16));
        return SOC_E_FAIL;
    }

    /* Clear LASI status */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc,
                                 PHY8706_PMAD_RX_ALARM_STAT_REG, &data16));

    /* Wait for LASI to be asserted when M8051 writes checksum to MSG_OUTr */
    sal_udelay(400); /* Wait for 300us */

    /* read message out reg */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc,
                                 PHY8706_PMAD_M8051_MSGOUT_REG, &data16));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy8706_mdio_firmware_download: p=%d msgout 0x%x\n"),
                         port,data16));

    /* clear LASI  */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc,
                                 PHY8706_PMAD_RX_ALARM_STAT_REG, &data16));

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA1C, &data16));

    /* check if checksum is correct */
    if (data16 != 0x600D) {
        /* Bad CHECKSUM */
        LOG_CLI((BSL_META_U(unit,
                            "_phy8706_mdio_firmware_download, bad cksum: "
                            "p=%d, cksum=0x%x\n"),
                 port,data16));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_8727_firmware_ram_set
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
_phy_8727_firmware_ram_set(int unit, int port, int offset, uint8 *array,int datalen)
{
    int j;
    soc_timeout_t  to;
    int            rv; 
    phy_ctrl_t *pc;
    uint16 data16 = 0;

    if (array == NULL) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;     
    pc = EXT_PHY_SW_STATE(unit, port);

    /* Reset, to download code from MDIO */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_CTRLr(unit, pc, &data16));

    data16 |= 0x8000;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_CTRLr(unit, pc, data16));

    sal_usleep(500000); /* Wait for 500ms */

    SOC_IF_ERROR_RETURN
        (phy_8727_opt_lvl_set(unit, port));

    /* Read Hand-Shake message from Micro */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
    if (data16 != 0x101) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_ram_set: p=%d wrong msgout value (0x%x) vs 0x%x\n"),
                             port,data16,0x101));
        return SOC_E_FAIL;
    }

    /* writes 0x5555 */ 
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA12, 0x5555));
    sal_udelay(20); /* Wait for 15us */

    /* Read Hand-Shake message from Micro */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
    if (data16 != 0xabcd) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_ram_set: p=%d wrong msgout value (0x%x) vs 0x%x\n"),
                             port,data16,0xabcd));
        return SOC_E_FAIL;
    }
    
    /* Write Starting Address, where the Code will reside in SRAM */
    data16 = 0xC000;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA12, data16));
    sal_udelay(20); /* Wait for 15us */

    /* Read Hand-Shake message from Micro */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
    if (data16 != 0xadd) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_ram_set: p=%d wrong msgout value (0x%x) vs 0x%x\n"),
                             port,data16,0xadd));
        return SOC_E_FAIL;
    }

    /* Write SPI SRAM Count Size */
    data16 = (datalen)/2;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA12, data16));
    sal_udelay(20); /* Wait for 15us */

    /* Read Hand-Shake message from Micro */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
    if (data16 != 0xab00) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_ram_set: p=%d wrong msgout value (0x%x) vs 0x%x\n"),
                             port,data16,0xab00));
        return SOC_E_FAIL;
    }

    /* Fill in the SPI RAM */
    for (j = 0; j < datalen/2; j++) {
        data16 = array[2*j] | (array[2*j+1] << 8); 
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA12, data16));

        /* Make sure the word is read by the Micro */
        sal_udelay(20); /* Wait for 15us */
    }

    /* Read Hand-Shake message (Done) from Micro */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &data16));

    if (data16 != 0x4321) {
        /* Download failure */
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_ram_set: p=%d wrong msgout value (0x%x) vs 0x%x\n"),
                             port,data16,0x4321));
        return SOC_E_FAIL;
    }

    /* Clear LASI status */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &data16));

    /* Wait for LASI to be asserted when M8051 writes checksum to MSG_OUTr */
    sal_udelay(400); /* Wait for 300us */

    /* check if checksum is correct */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &data16));

    /* Need to check if checksum is correct */
    if (data16 != 0x300) {
        /* Bad CHECKSUM */
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_ram_set: p=%d wrong msgout value (0x%x) vs 0x%x\n"),
                             port,data16,0x300));
        return SOC_E_FAIL;
    }

    /* clear LASI  */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &data16));

    /* update SPI boot control bit, next time micro boots from SPI RAM */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xCA85, 0x1, 0x1));

    /* Apply soft reset to Micro and make sure SPI is not downloaded */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA10, 0x00C4));

    /* remove soft reset */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA10, 0x00C0));

    /* Wait for device to come out of reset */
    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_PHY8706_PMA_PMD_REG(unit, pc, 0xca13, &data16);
        if ((data16 == 0x202) || SOC_FAILURE(rv)) {
            break;
        }
    }
    if (data16 != 0x202) {
        rv = SOC_E_TIMEOUT;
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_ram_set fail: p=%d msgout 0x%x vs 0x%x\n"),
                             port,data16,0x202));
    } 
    return rv;
}

/*
 *  Function:  *      phy_8706_rom_wait
 *
 *  Purpose:
 *      Wait for data to be written to the SPI-ROM.
 *  Input:
 *      unit, port
 */
STATIC int
phy_8706_rom_wait(int unit, int port)
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
                              "phy_8706_rom_program: u = %d p = %d "
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
                                  "phy_8706_rom_program: u = %d p = %d "
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
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA12, wr_data));
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));

        /* Clear LASI Message Out Status. */
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &wr_data));

        if ((rd_data & 0x1) == 0) {
            SPI_READY = 0;
        }
    } /* SPI_READY  */
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8706_rom_write_enable_set
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
phy_8706_rom_write_enable_set(int unit, int port, int enable)
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
    SOC_IF_ERROR_RETURN(phy_8706_rom_wait(unit, port));

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_8706_firmware_set
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
phy_8706_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{

    uint16     rd_data;
    uint16     wr_data;
    uint16     tx_disable_reg;
    int        j;
    int        i = 0;
    int        count;
    phy_ctrl_t *pc;
    uint8       spi_values[WR_BLOCK_SIZE];
#ifdef BCM8727_MICROCODE_LOAD_WAR 
    uint16 rev; 
    uint16 phyid1; 
#endif 

    pc = EXT_PHY_SW_STATE(unit, port);

    /* save the PMAD TX disable reg. */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9, &tx_disable_reg));

    /* disable TX and bring the link down if it's up */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x9, 0x1));

    if (pc->phy_id1 == PHY_ID1_8747) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xc843, 0x0000, 0x000f));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xc840, 0x0000, 0x000c));
    }

    /*
     * Read LASI Status registers To clear initial Failure status.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9004, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9005, &rd_data));

    /*
     * Enable the LASI For Message out. 
     */
    if (IS_8727_TYPE(pc->phy_id1)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x9000, 0x0004));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x9000, 0x0400));
    }
    /*
     * 0x9002, bit 2 Is Rx Alarm enabled For LASI. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x9002, 0x0004));
    /*
     * Read Any Residual Message out register.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));
    /*
     * Clear LASI Message Out Status. 
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));

    /*
     * ser_boot pin HIGH
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xCA85, 0x1, 0x1));

     /* set SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(phy_8706_rom_write_enable_set(unit, port, 1));

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
        SOC_IF_ERROR_RETURN(phy_8706_rom_wait(unit,port));

    } /* SPI_WRITE */

    /* clear SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(phy_8706_rom_write_enable_set(unit, port, 0));

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
        (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xCA85, 0x0, 0x1)); 

    if (pc->phy_id1 == PHY_ID1_8747) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xc843, 0x000f, 0x000f));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8706_PMA_PMD_REG(unit, pc, 0xc840, 0x000c, 0x000c));
    }

    /* restore the PMAD TX disable reg. */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x9, tx_disable_reg));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8706_rom_program: u=%d p=%d done\n"), unit, port));

#ifdef BCM8727_MICROCODE_LOAD_WAR 
   /* 
    * After firmware in ROM is changed, enforce the microcode load 
    * status in the device as 0xbaad (!=0x600d) so that firmware written 
    * to ROM by this firmware_set is loaded to the device during port_init. 
    * else _8727_rom_load_war( ) will not load the firmware to device and 
    * device will run with previous firmware. 
    */ 
    SOC_IF_ERROR_RETURN 
        (READ_PHY8706_PMA_PMD_REG(unit, pc, MII_PHY_ID1_REG, &phyid1)); 
    if (phyid1 == PHY_ID1_8727) { 
        SOC_IF_ERROR_RETURN 
            (pc->read(unit, pc->phy_id, SOC_PHY_CLAUSE45_ADDR(0x1,0xc801), &rev)); 
        if ((rev == 0x2) || (rev == 0x3)) { 
            SOC_IF_ERROR_RETURN 
                (pc->write(unit, pc->phy_id, SOC_PHY_CLAUSE45_ADDR(0x1,0xca1c), 0xBAAD)); 
        } 
    } 
#endif 
    return SOC_E_NONE;
}

static int
write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, uint16 *rddata)
{

    uint16     tmp_data = 0;
    soc_timeout_t to;

    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    /* Clear any lingering LASI Message Out Status. */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &tmp_data));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0xCA12, wrdata));

    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9005, &tmp_data));
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
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    *rddata = tmp_data;
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x9003, &tmp_data));

    return SOC_E_NONE;
}

STATIC int
_phy_8727_bsc_rw(int unit, soc_port_t port, int dev_addr, int opr,
                    int addr, int count, void *data_array,uint32 ram_start)
{
    phy_ctrl_t  *pc;
    soc_timeout_t  to;
    uint16 data16;
    int i;
    sal_usecs_t start;
    sal_usecs_t end;
    int access_type;
    int data_type;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8727_bsc_read: u=%d p=%d addr=%04x\n"), 
                         unit, port, addr));

    if (!data_array) {
        return SOC_E_PARAM;
    }

    if (count > PHY_8727_BSC_XFER_MAX) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    data_type = PHY_8727_I2C_DATA_TYPE(opr);
    access_type = PHY_8727_I2C_ACCESS_TYPE(opr);

    if (access_type == PHY_8727_I2CDEV_WRITE) {
        for (i = 0; i < count; i++) {
            if (data_type == PHY_8727_I2C_8BIT) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8706_PMA_PMD_REG(unit, pc, ram_start + i,
                          ((uint8 *)data_array)[i]));
            } else {  /* 16 bit */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8706_PMA_PMD_REG(unit, pc, ram_start + i,
                          ((uint16 *)data_array)[i]));
            }
        }
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x8000, 0x8000));
    data16 = ram_start;
    SOC_IF_ERROR_RETURN         
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x8004, data16));
    SOC_IF_ERROR_RETURN         
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x8003, addr));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x8002, count)); 

    data16 = 1;
    data16 |= (dev_addr<<9);
    if (access_type == PHY_8727_I2CDEV_WRITE) { 
        data16 |= PHY_8727_WR_FREQ_400KHZ;
    } 

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit,pc,0x8005,data16)); 

    if (access_type == PHY_8727_I2CDEV_WRITE) {
        data16 =  0x8000 | PHY_8727_BSC_WRITE_OP; 
    } else {
        data16 =  0x8000 | PHY_8727_BSC_READ_OP; 
    }

    if (data_type == PHY_8727_I2C_16BIT) {
        data16 |= (1 << 12);
    }
        
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8706_PMA_PMD_REG(unit, pc, 0x8000, data16));

    start = sal_time_usecs();
    soc_timeout_init(&to, 1000000, 0);
    while (!soc_timeout_check(&to)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x8000, &data16));
        if (((data16 & PHY_8727_2W_STAT) == PHY_8727_2W_STAT_COMPLETE)) {
            break;
        }
    }
    end = sal_time_usecs();

    /* need some delays */
    sal_usleep(10000);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "BSC command status %d time=%d\n"), 
                         (data16 & PHY_8727_2W_STAT), SAL_USECS_SUB(end, start)));

    if (access_type == PHY_8727_I2CDEV_WRITE) {
        return SOC_E_NONE;
    }

    if ((data16 & PHY_8727_2W_STAT) == PHY_8727_2W_STAT_COMPLETE) {
        for (i = 0; i < count; i++) {
            SOC_IF_ERROR_RETURN 
                (READ_PHY8706_PMA_PMD_REG(unit, pc, (ram_start+i), &data16));
            if (data_type == PHY_8727_I2C_16BIT) {
                ((uint16 *)data_array)[i] = data16;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "%04x "), data16));
            } else {
                ((uint8 *)data_array)[i] = (uint8)data16;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "%02x "), data16));
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Read a slave device such as NVRAM/EEPROM connected to the 8727's I2C
 * interface. This function will be mainly used for diagnostic or workaround
 * purpose.
 * Note:
 * The size of read_array buffer must be greater than or equal to the 
 * parameter nbytes.
 * usage example:
 *   Retrieve the first 100 byte data of the non-volatile storage device with
 *   I2C bus device address 0x50(default SPF eeprom I2C address) on unit 0, 
 *   port 2.
 *   uint8 data8[100];
 *   phy_8727_i2cdev_read(0,2,0x50,0,100,data8);
 */
int
phy_8727_i2cdev_read(int unit, 
                     soc_port_t port, 
                     int dev_addr,  /* 7 bit I2C bus device address */
                     int offset,    /* starting data address to read */
                     int nbytes,    /* number of bytes to read */  
                     uint8 *read_array)   /* buffer to hold retrieved data */
{
    return _phy_8727_bsc_rw(unit, port, dev_addr,PHY_8727_I2CDEV_READ,
             offset, nbytes, (void *)read_array,PHY_8727_READ_START_ADDR); 

}

/*
 * Write to a slave device such as NVRAM/EEPROM connected to the 8727's I2C
 * interface. This function will be mainly used for diagnostic or workaround
 * purpose.
 * Note:
 * The size of write_array buffer should be equal to the parameter nbytes.
 * The EEPROM may limit the maximun write size to 16 bytes
 * usage example:
 *   Write to first 100 byte space of the non-volatile storage device with
 *   I2C bus device address 0x50(default SPF eeprom I2C address) on unit 0,
 *   port 2, with written data specified in array data8.
 *   uint8 data8[100];
 *   *** initialize the data8 array with written data ***
 *
 *   phy_8727_i2cdev_write(0,2,0x50,0,100,data8);
 */

int
phy_8727_i2cdev_write(int unit,
                     soc_port_t port,
                     int dev_addr,  /* I2C bus device address */
                     int offset,    /* starting data address to write to */
                     int nbytes,    /* number of bytes to write */
                     uint8 *write_array)   /* buffer to hold written data */
{
    int j;
    int rv = SOC_E_NONE;

    for (j = 0; j < (nbytes/PHY_8727_BSC_WR_MAX); j++) {
        rv = _phy_8727_bsc_rw(unit, port, dev_addr,PHY_8727_I2CDEV_WRITE,
                    offset + j * PHY_8727_BSC_WR_MAX, PHY_8727_BSC_WR_MAX,
                    (void *)(write_array + j * PHY_8727_BSC_WR_MAX),
                    PHY_8727_WRITE_START_ADDR);
        if (rv != SOC_E_NONE) {
            return rv;
        }
        sal_usleep(20000);
    }
    if (nbytes%PHY_8727_BSC_WR_MAX) {
        rv = _phy_8727_bsc_rw(unit, port, dev_addr,PHY_8727_I2CDEV_WRITE,
                offset + j * PHY_8727_BSC_WR_MAX, nbytes%PHY_8727_BSC_WR_MAX,
                (void *)(write_array + j * PHY_8727_BSC_WR_MAX),
                PHY_8727_WRITE_START_ADDR);
    }
    return rv;
}

/*
 * Function:
 *      phy_8706_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_8706_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data16;
    uint16               regdata;     
    phy_ctrl_t          *pc;      /* PHY software state */
    int rv = SOC_E_NONE;
    int rd_cnt;
    uint16 phyid1;
    uint16 rev_num;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, MII_PHY_ID1_REG, &phyid1));

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16)) {
        if (!IS_8727_TYPE(phyid1)) {
            return SOC_E_UNAVAIL;
        }
    }

    /* 8727 errata indicates 1 byte read is not working for B version.
     * The workaround is to read two bytes and return back one byte.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xc801, &rev_num));

    rd_cnt = ((rev_num & 0xF) < PHY_8727_VERSION_C)? 2: 1;

    if (flags & SOC_PHY_I2C_DATA8) {

        /* read two bytes and return the first byte */
        SOC_IF_ERROR_RETURN
            (phy_8727_i2cdev_read(unit, port, 
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
            (READ_PHY8706_PMA_PMD_REG(unit, pc, PHY_8727_I2C_TEMP_RAM, 
                   &regdata));
     
        rv = _phy_8727_bsc_rw(unit, port, 
               SOC_PHY_I2C_DEVAD(phy_reg_addr),
               PHY_8727_I2C_OP_TYPE(PHY_8727_I2CDEV_READ,PHY_8727_I2C_16BIT),
               SOC_PHY_I2C_REGAD(phy_reg_addr),1, 
               (void *)&data16,PHY_8727_I2C_TEMP_RAM);

        *phy_data = data16;

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, PHY_8727_I2C_TEMP_RAM,
                 regdata));
    } else {   
        SOC_IF_ERROR_RETURN
            (READ_PHY_REG(unit, pc, phy_reg_addr, &data16));
        *phy_data = data16;
    }

    return rv;
}

/*
 * Function:
 *      phy_8706_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_8706_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint8  data8[4];
    uint16 data16[2];  
    uint16 regdata[2]; 
    phy_ctrl_t          *pc;      /* PHY software state */
    int rv = SOC_E_NONE;
    uint16 phyid1;
    uint16 rev_num;
    int wr_cnt;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* This feature is only supported on 8727 for now*/
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, MII_PHY_ID1_REG, &phyid1));

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16)) {
        if (!IS_8727_TYPE(phyid1)) {
            return SOC_E_UNAVAIL;
        }
    }

    /* check which revision to implement rev. specific workaround */
    /* 8727 errata indicates 1 byte/word write is not working for B version.
     * The workaround is to send two bytes/words write command and actually
     * only one gets written.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY8706_PMA_PMD_REG(unit, pc, 0xc801, &rev_num));

    wr_cnt = ((rev_num & 0xF) < PHY_8727_VERSION_C)? 2: 1; 
      
    if (flags & SOC_PHY_I2C_DATA8) {
        data8[0] = (uint8)phy_data; 
        SOC_IF_ERROR_RETURN
            (phy_8727_i2cdev_write(unit, port,
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
            (READ_PHY8706_PMA_PMD_REG(unit, pc, PHY_8727_I2C_TEMP_RAM,
                 &regdata[0]));
        data16[0] = phy_data;
        rv = _phy_8727_bsc_rw(unit, port,
              SOC_PHY_I2C_DEVAD(phy_reg_addr),
              PHY_8727_I2C_OP_TYPE(PHY_8727_I2CDEV_WRITE,PHY_8727_I2C_16BIT),
              SOC_PHY_I2C_REGAD(phy_reg_addr),wr_cnt,
              (void *)data16,PHY_8727_I2C_TEMP_RAM);

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8706_PMA_PMD_REG(unit, pc, PHY_8727_I2C_TEMP_RAM,
                  regdata[0]));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, phy_reg_addr, (uint16)phy_data));
    }

    return rv;
}

/*
 * Function:
 *      phy_8706_probe
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
phy_8706_probe(int unit, phy_ctrl_t *pc)
{
    uint16 id0, id1;
    soc_phy_info_t *pi;

    if (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x2, &id0) < 0) {
        return SOC_E_NOT_FOUND;
    }
    if (READ_PHY8706_PMA_PMD_REG(unit, pc, 0x3, &id1) < 0) {
        return SOC_E_NOT_FOUND;
    }

    pi = &SOC_PHY_INFO(unit, pc->port);

    switch (PHY_MODEL(id0, id1)) {

    case PHY_BCM8747_MODEL:
        if (soc_property_port_suffix_num_get(unit, pc->port, 8706,
                                    spn_PORT_PHY_MODE_REVERSE, "phy", 0 )) {
            pi->phy_name = "BCM5942";
            PHY_FLAGS_SET(pc->unit, pc->port, PHY_FLAGS_SECONDARY_SERDES);
        }
    break;

    default:
    break;

    }
    pc->size = sizeof(PHY8706_DEV_DESC_t);
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8706_medium_get 
 * Purpose:
 *      Routine to get valid medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) SOC_PORT_MEDIUM_FIBER 
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8706_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    int rv;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    if (medium != NULL) {
        *medium = SOC_PORT_MEDIUM_FIBER;
        rv = SOC_E_NONE;
    } else {
        rv = SOC_E_PARAM;
    }

    return rv;
}

/*
 * Variable:
 *    phy_8706_drv
 * Purpose:
 *    Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_8706drv_xe = {
    "8706 10-Gigabit PHY Driver",
    phy_8706_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_8706_link_get,    /* Link get   */
    phy_8706_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_null_set,         /* Duplex set */
    phy_null_one_get,     /* Duplex get */
    phy_8706_speed_set,   /* Speed set  */
    phy_8706_speed_get,   /* Speed get  */
    phy_null_set,         /* Master set */
    phy_null_zero_get,    /* Master get */
    phy_8706_an_set,      /* ANA set */
    phy_8706_an_get,      /* ANA get */
    NULL,                 /* Local Advert set */
    NULL,                 /* Local Advert get */
    phy_null_mode_get,    /* Remote Advert get */
    phy_8706_lb_set,      /* PHY loopback set */
    phy_8706_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_xehg_interface_get, /* IO Interface get */
    NULL,   /* PHY abilities mask */
    phy_8706_linkup,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_8706_medium_get,     /* phy medium */
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8706_control_set,    /* phy_control_set */
    phy_8706_control_get,    /* phy_control_get */
    phy_8706_reg_read,       /* phy_reg_read */
    phy_8706_reg_write,      /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_8706_probe,          /* pd_probe  */
    phy_8706_ability_advert_set,  /* pd_ability_advert_set */
    phy_8706_ability_advert_get,  /* pd_ability_advert_get */
    phy_8706_ability_remote_get,  /* pd_ability_remote_get */
    phy_8706_ability_local_get,   /* pd_ability_local_get  */
    phy_8706_firmware_set         /* pd_firmware_set */
};

#else /* INCLUDE_PHY_8706 */
typedef int _phy_8706_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8706 */

