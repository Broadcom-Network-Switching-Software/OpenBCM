/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8072.h
 */

#ifndef   _PHY8072_H_
#define   _PHY8072_H_

#include <soc/phy.h>

#define MII_CTRL_PMA_LOOPBACK      (1 << 0)

#define PHY8072_C45_DEV_PMA_PMD     0x01
#define PHY8072_C45_DEV_PCS         0x03
#define PHY8072_C45_DEV_PHYXS       0x04
#define PHY8072_C45_DEV_DTEXS       0x05
#define PHY8072_C45_DEV_AN          0x07

#define PHY8072_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8072_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8072_REG_MODIFY(_unit, _phy_ctrl, _addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val), (_mask))

/* PMA/PMD Device (Dev Addr 1) */

#define READ_PHY8072_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, (_reg)), (_val))
#define WRITE_PHY8072_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, (_reg)), (_val))
#define MODIFY_PHY8072_PMA_PMD_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, (_reg)), (_val), (_mask))

/* Control Register (Addr 0000h) */
#define READ_PHY8072_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0), (_val))
#define WRITE_PHY8072_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0), (_val))
#define MODIFY_PHY8072_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0), (_val), (_mask))

/* Status Register (Addr 0001h) */
#define READ_PHY8072_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 1), (_val))
#define WRITE_PHY8072_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 1), (_val))
#define MODIFY_PHY8072_PMA_PMD_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 1), (_val), (_mask))

/* ID0 Register (Addr 0002h) */
#define PMAD_ID0_REG    0x2
    
/* ID1 Register (Addr 0003h) */
#define PMAD_ID1_REG    0x3

/* Speed Ability Register (Addr 0004h) */
/* Devices in Package 1 Register (Addr 0005h) */
/* Devices in Package 2 Register (Addr 0006h) */

/* Control 2 Register (Addr 0007h) */
#define PMAD_CTRL2_REG   0x7

#define PMAD_CTRL2r_PMA_TYPE_MASK    0xF
#define PMAD_CTRL2r_PMA_TYPE_1G      0xD
#define PMAD_CTRL2r_PMA_TYPE_10G     0xB
 
/* Status 2 Register (Addr 0008h) */

/* Transmit Disable Register (Addr 0009h) */
#define READ_PHY8072_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 9), (_val))
#define WRITE_PHY8072_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 9), (_val))
#define MODIFY_PHY8072_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 9), (_val), (_mask))

/* Receive Signal Detect Register (Addr 000Ah) */
/* Organizationlly Unique Identifier 0 Register (Addr 000Eh) */
/* Organizationlly Unique Identifier 1 Register (Addr 000Fh) */

/* PMA/PMD device specific registers */

/* User Status Register. This register collects all speeds, link and
 * auto-neg status from 4 IEEE status registers
 */
#define PMAD_USR_STATUS_REG   0xC820

/* autoneg 10G link up */
#define PMAD_STATUS_AN_10G(_s)  (((_s) & 0xEC44) == 0x6C44)
#define PMAD_STATUS_AN_2p5G(_s)  (((_s) & 0x8C22) == 0x8C22)
#define PMAD_STATUS_MODE_2p5G      (1<<5)
#define PMAD_STATUS_1G_LINKDOWN    (1<<13)
#define PMAD_STATUS_2p5G_LINKDOWN  (1<<14)
#define PMAD_STATUS_10G_LINKDOWN   (1<<15)

/* Chip revision register */
#define PMAD_CHIP_REV_REG          0xC801

/* General control status register */
#define PMAD_GEN_CTRL_STAT_REG     0xCA10

/* General Purpose Register 2 */
#define PMAD_GEN2_REG              0xCA1A


/* PCS Device (Dev Addr 3) */
/* Control 1 Register (Addr 0000h) */
#define READ_PHY8072_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PCS, 0), (_val))
#define WRITE_PHY8072_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PCS, 0), (_val))
#define MODIFY_PHY8072_PCS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PCS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY8072_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PCS, 1), (_val))
#define WRITE_PHY8072_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PCS, 1), (_val))
#define MODIFY_PHY8072_PCS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PCS, 1), (_val), (_mask))

/* PCS Identifier 0 Register (Addr 0002h) */

/* PCS Identifier 1 Register (Addr 0003h) */

/* Speed Ability (Addr 0004h) */
 
/* Devices in Package 1 (Addr 0005h) */

/* Devices in Package 2 (Addr 0006h) */

/* PCS Control 2 Register (Addr 0007h) */

/* PCS Status 2 Register (Addr 0008h) */

/* Organizationally Unique Identifier 0 Register (Addr 000eh) */
/* Organizationally Unique Identifier 1 Register (Addr 000fh) */
/* 10GBASE-X PCS Status Register (Addr 0x0018h) */
/* 10GBASE-R PCS Status Register (Addr 0x0020h) */
/* 10GBASE-R PCS Status 2 Register (Addr 0x0021h) */
/* 10GBASE-R PCS Jitter Test Seed A0 Register (Addr 0x0022h) */
/* 10GBASE-R PCS Jitter Test Seed A1 Register (Addr 0x0023h) */
/* 10GBASE-R PCS Jitter Test Seed A2 Register (Addr 0x0024h) */
/* 10GBASE-R PCS Jitter Test Seed A3 Register (Addr 0x0025h) */
/* 10GBASE-R PCS Jitter Test Seed B0 Register (Addr 0x0026h) */
/* 10GBASE-R PCS Jitter Test Seed B1 Register (Addr 0x0027h) */
/* 10GBASE-R PCS Jitter Test Seed B2 Register (Addr 0x0028h) */
/* 10GBASE-R PCS Jitter Test Seed B3 Register (Addr 0x0029h) */
/* 10GBASE-R PCS Jitter Test Control Register (Addr 0x002ah) */
/* 10GBASE-R PCS Jitter Test Error Counter Register (Addr 0x002Bh) */
/* PRBS23 WIN_2 Register (Addr = 0xc92dh) */

/****************************/
/* XGXS Device (Dev Addr 4) */
/****************************/
#define READ_PHY8072_PHYXS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, (_reg)), (_val))
#define WRITE_PHY8072_PHYXS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, (_reg)), (_val))
#define MODIFY_PHY8072_PHYXS_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, (_reg)), (_val), (_mask))

/* Control 1 Register (Addr 0000h) */
#define READ_PHY8072_PHYXS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0), (_val))
#define WRITE_PHY8072_PHYXS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0), (_val))
#define MODIFY_PHY8072_PHYXS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY8072_PHYXS_STATr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 1), (_val))
#define WRITE_PHY8072_PHYXS_STATr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 1), (_val))
#define MODIFY_PHY8072_PHYXS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 1), (_val), (_mask))

/* PHY Identifier Register 0 (Addr 0002h) */
/* PHY Identifier Register 1 (Addr 0003h) */
/* Speed Ability (Addr 0004h) */
/* Device in Package 2 (Addr 0005h) */
/* Device in Package 1 (Addr 0006h) */
/* Status 2 Register (Addr 0008h) */
/* Organizatioally Unique Identifier 0 Register (Addr 000eh) */
/* Organizationally Unique Identifier 1 Register (Addr 000fh) */

/* XGXS Lane Status Register (Addr 0018h) */
#define READ_PHY8072_PHYXS_XGXS_LANE_STATr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0x18), (_val))
#define WRITE_PHY8072_PHYXS_XGXS_LANE_STATr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0x18), (_val))
#define MODIFY_PHY8072_PHYXS_XGXS_LANE_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0x18), \
              (_val), (_mask))

/* XGXS Test Control Register (Addr 0019h) */

/* XGXS mode control register */

/* XGXS control register */

 #define XGXS_MODE_CTRLr           0x8000
#define XGXS_MODE_SHIFTER          8
#define XGXS_MODE_NORMAL           (0x1 << XGXS_MODE_SHIFTER)
#define XGXS_MODE_SYSTEM_LOOPBACK  (0x0 << XGXS_MODE_SHIFTER)
#define XGXS_MODE_MASK             (0xf << XGXS_MODE_SHIFTER)
#define XGXS_CTRL_RLOOP            (1 << 6) 

/************************************************/
/* User Defined Registers (Devices 1, 2, and 3) */
/************************************************/

#define READ_PHY8072_IDENTIFIERr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xc800), (_val))
#define WRITE_PHY8072_IDENTIFIERr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xc800), (_val))
#define MODIFY_PHY8072_IDENTIFIERr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xc800), \
              (_val), (_mask))

#define READ_PHY8072_PMD_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xca0a), (_val))
#define WRITE_PHY8072_PMD_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xca0a), (_val))
#define MODIFY_PHY8072_PMD_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xca0a), \
              (_val), (_mask))

#define  READ_PHY8072_TXA_ACTRL_3r(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0x80a7), (_val))
#define WRITE_PHY8072_TXA_ACTRL_3r(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0x80a7), (_val))
#define MODIFY_PHY8072_TXA_ACTRL_3r(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PHYXS, 0x80a7), \
              (_val), (_mask))

/* General Purpose Register 1 */
#define  READ_PHY8072_GENREG1r(_unit, _phy_ctrl, _val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xca19), (_val))
#define WRITE_PHY8072_GENREG1r(_unit, _phy_ctrl, _val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xca19), (_val))
#define MODIFY_PHY8072_GENREG1r(_unit, _phy_ctrl, _val, _mask) \
            PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_PMA_PMD, 0xca19), \
              (_val), (_mask))
#define PMDA_ROM_LOAD_8072ID    0xbcb0
#define PMDA_ROM_LOAD_8073ID    0xbaa0

/*   Auto-Neotiation device */

#define READ_PHY8072_AN_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8072_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_AN, (_reg)), (_val))
#define WRITE_PHY8072_AN_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8072_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_AN, (_reg)), (_val))
#define MODIFY_PHY8072_AN_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8072_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8072_C45_DEV_AN, (_reg)), (_val), (_mask))

/* autoneg control register */

#define AN_CTRL_REG        0
#define AN_EXT_NXT_PAGE    (1 << 13)
#define AN_ENABLE          (1 << 12)
#define AN_RESTART         (1 << 9)

/* autoneg status register */

#define AN_STATUS_REG      1
#define AN_LP_AN_ABILITY   (1 << 0)
#define AN_LINK            (1 << 2)
#define AN_DONE            (1 << 5)

/* autoneg advertisement register 0 */

#define AN_ADVERT_0_REG        0x10
#define AN_ADVERT_PAUSE        (1 << 10)
#define AN_ADVERT_PAUSE_ASYM   (1 << 11)

/* autoneg advertisement register 1 */

#define AN_ADVERT_1_REG        0x11
#define AN_ADVERT_10G          (1 << 7)
#define AN_ADVERT_1G           (1 << 5)

/* autoneg advertisement register 2 */

#define AN_ADVERT_2_REG        0x12
#define AN_ADVERT_FEC          (1 << 15)

/* Link Partner base ability page 0 */

#define AN_LP_ABILITY_0_REG    0x13

/* Link Partner base ability page 1 */

#define AN_LP_ABILITY_1_REG    0x14

/* Link Partner base ability page 2 */

#define AN_LP_ABILITY_2_REG    0x15
 
/* BP ethernet status register */

#define AN_ETH_STATUS_REG      0x30
#define AN_SPEED_1000BASE_KX   (1 << 1)
#define AN_SPEED_10GBASE_KR    (1 << 3)

/* misc. registers */
#define AN_MISC1_REG    0x8308
#define AN_MISC2_REG    0x8309
#define PMD_FORCED_SP_MASK   (1 << 5)
#define FORCED_SP_SEL_MASK   (1 << 4)

#endif  /* _PHY8072_H_ */
