/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8706.h
 */

#ifndef   _PHY8706_H_
#define   _PHY8706_H_

#include <soc/phy.h>

#define MII_CTRL_PMA_LOOPBACK      (1 << 0)

#define PHY8706_C45_DEV_PMA_PMD     0x01
#define PHY8706_C45_DEV_PCS         0x03
#define PHY8706_C45_DEV_PHYXS       0x04
#define PHY8706_C45_DEV_DTEXS       0x05
#define PHY8706_C45_DEV_AN          0x07

#define PHY8706_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8706_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8706_REG_MODIFY(_unit, _phy_ctrl, _addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val), (_mask))

/* PMA/PMD Device (Dev Addr 1) */

#define READ_PHY8706_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, (_reg)), (_val))
#define WRITE_PHY8706_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, (_reg)), (_val))
#define MODIFY_PHY8706_PMA_PMD_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, (_reg)), (_val), (_mask))                                                                                

/* Control Register (Addr 0000h) */
#define READ_PHY8706_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0), (_val))
#define WRITE_PHY8706_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0), (_val))
#define MODIFY_PHY8706_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0), (_val), (_mask))

/* Status Register (Addr 0001h) */
#define READ_PHY8706_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 1), (_val))
#define WRITE_PHY8706_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 1), (_val))
#define MODIFY_PHY8706_PMA_PMD_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 1), (_val), (_mask))

/* ID0 Register (Addr 0002h) */
/* ID1 Register (Addr 0003h) */
/* Speed Ability Register (Addr 0004h) */
/* Devices in Package 1 Register (Addr 0005h) */
/* Devices in Package 2 Register (Addr 0006h) */
/* Control 2 Register (Addr 0007h) */

#define PMAD_CTRL2_REG               7
#define PMAD_CTRL2r_PMA_TYPE_MASK    0xF
#define PMAD_CTRL2r_PMA_TYPE_1G_KX   0xD
#define PMAD_CTRL2r_PMA_TYPE_10G_KR  0xB
#define PMAD_CTRL2r_PMA_TYPE_10G_LRM 0x8
 
/* Status 2 Register (Addr 0008h) */

/* Transmit Disable Register (Addr 0009h) */
#define READ_PHY8706_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 9), (_val))
#define WRITE_PHY8706_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 9), (_val))
#define MODIFY_PHY8706_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 9), (_val), (_mask))


/* Receive Signal Detect Register (Addr 000Ah) */
/* Organizationlly Unique Identifier 0 Register (Addr 000Eh) */
/* Organizationlly Unique Identifier 1 Register (Addr 000Fh) */

/* PCS Device (Dev Addr 3) */

#define READ_PHY8706_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, (_reg)), (_val))
#define WRITE_PHY8706_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, (_reg)), (_val))
#define MODIFY_PHY8706_PCS_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, (_reg)), (_val), (_mask))                                                                                

/* Control 1 Register (Addr 0000h) */
#define READ_PHY8706_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, 0), (_val))
#define WRITE_PHY8706_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, 0), (_val))
#define MODIFY_PHY8706_PCS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY8706_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, 1), (_val))
#define WRITE_PHY8706_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, 1), (_val))
#define MODIFY_PHY8706_PCS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PCS, 1), (_val), (_mask))

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

#define READ_PHY8706_XS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, (_reg)), (_val))
#define WRITE_PHY8706_XS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, (_reg)), (_val))
#define MODIFY_PHY8706_XS_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, (_reg)), (_val), (_mask))

/* Control 1 Register (Addr 0000h) */
#define READ_PHY8706_PHYXS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0), (_val))
#define WRITE_PHY8706_PHYXS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0), (_val))
#define MODIFY_PHY8706_PHYXS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY8706_PHYXS_STATr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 1), (_val))
#define WRITE_PHY8706_PHYXS_STATr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 1), (_val))
#define MODIFY_PHY8706_PHYXS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 1), (_val), (_mask))

/* PHY Identifier Register 0 (Addr 0002h) */
/* PHY Identifier Register 1 (Addr 0003h) */
/* Speed Ability (Addr 0004h) */
/* Device in Package 2 (Addr 0005h) */
/* Device in Package 1 (Addr 0006h) */
/* Status 2 Register (Addr 0008h) */
/* Organizatioally Unique Identifier 0 Register (Addr 000eh) */
/* Organizationally Unique Identifier 1 Register (Addr 000fh) */

/* XGXS Lane Status Register (Addr 0018h) */
#define READ_PHY8706_PHYXS_XGXS_LANE_STATr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x18), (_val))
#define WRITE_PHY8706_PHYXS_XGXS_LANE_STATr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x18), (_val))
#define MODIFY_PHY8706_PHYXS_XGXS_LANE_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x18), \
              (_val), (_mask))

/* XGXS Test Control Register (Addr 0019h) */

/* Misc Control Register (Addr 800eh) */
#define READ_PHY8706_PHYXS_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x800e), (_val))
#define WRITE_PHY8706_PHYXS_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x800e), (_val))
#define MODIFY_PHY8706_PHYXS_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x800e), (_val), (_mask))

/* XGXS control register */

#define XGXS_MODE_CTRLr            0x8000
#define XGXS_MODE_SHIFTER          8
#define XGXS_MODE_NORMAL           (0x1 << XGXS_MODE_SHIFTER)
#define XGXS_MODE_SYSTEM_LOOPBACK  (0x0 << XGXS_MODE_SHIFTER)
#define XGXS_MODE_MASK             (0xf << XGXS_MODE_SHIFTER)
#define XGXS_CTRL_RLOOP            (1 << 6)

/* XgxsBlk0 :: miscControl1 :: Global_PMD_tx_disable [11:11] */
#define XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK           0x0800
#define XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_ALIGN          0
#define XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_BITS           1
#define XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_SHIFT          11

#define READ_PHY8706_PHYXS_XGXSBLK0_MISCCONTROL1r(_unit, _pc, _val) \
             READ_PHY8706_XS_REG((_unit), (_pc), 0x0000800e, (_val))
#define WRITE_PHY8706_PHYXS_XGXSBLK0_MISCCONTROL1r(_unit, _pc, _val) \
             WRITE_PHY8706_XS_REG((_unit), (_pc), 0x0000800e, (_val))
#define MODIFY_PHY8706_PHYXS_XGXSBLK0_MISCCONTROL1r(_unit, _pc, _val, _mask) \
             MODIFY_PHY8706_XS_REG((_unit), (_pc), 0x0000800e, (_val), (_mask))

/* dsc_2_0 :: dsc_ctrl0 :: rxSeqStart [15:15] */
#define DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK                          0x8000
#define DSC_2_0_DSC_CTRL0_RXSEQSTART_ALIGN                         0
#define DSC_2_0_DSC_CTRL0_RXSEQSTART_BITS                          1
#define DSC_2_0_DSC_CTRL0_RXSEQSTART_SHIFT                         15

#define READ_PHY8706_PHYXS_RX_ALL_RX_CONTROLr(_unit, _pc, _val) \
             READ_PHY8706_XS_REG((_unit), (_pc), 0x000080f1, (_val))
#define WRITE_PHY8706_PHYXS_RX_ALL_RX_CONTROLr(_unit, _pc, _val) \
             WRITE_PHY8706_XS_REG((_unit), (_pc), 0x000080f1, (_val))
#define MODIFY_PHY8706_PHYXS_RX_ALL_RX_CONTROLr(_unit, _pc, _val, _mask) \
             MODIFY_PHY8706_XS_REG((_unit), (_pc), 0x000080f1, (_val), (_mask))

#define READ_PHY8706_PHYXS_GP_STATUS_XGXSSTATUS1r(_unit, _pc, _val) \
             READ_PHY8706_XS_REG((_unit), (_pc), 0x00008122, (_val))
#define WRITE_PHY8706_PHYXS_GP_STATUS_XGXSSTATUS1r(_unit, _pc, _val) \
             WRITE_PHY8706_XS_REG((_unit), (_pc),  0x00008122, (_val))
#define MODIFY_PHY8706_PHYXS_GP_STATUS_XGXSSTATUS1r(_unit, _pc, _val, _mask) \
             MODIFY_PHY8706_XS_REG((_unit), (_pc), 0x00008122, (_val), (_mask))

#define READ_PHY8706_PHYXS_GP_STATUS_STATUS1000X1r(_unit, _pc, _val) \
             READ_PHY8706_XS_REG((_unit), (_pc), 0x00008124, (_val))
#define WRITE_PHY8706_PHYXS_GP_STATUS_STATUS1000X1r(_unit, _pc, _val) \
             WRITE_PHY8706_XS_REG((_unit), (_pc), 0x00008124, (_val))
#define MODIFY_PHY8706_PHYXS_GP_STATUS_STATUS1000X1r(_unit, _pc, _val, _mask) \
             MODIFY_PHY8706_XS_REG((_unit), (_pc), 0x00008124, (_val), (_mask))

/* XGXG status 3 register */

/* GP_Status :: xgxsStatus3 :: link [15:15] */
#define GP_STATUS_XGXSSTATUS3_LINK_MASK                            0x8000
#define GP_STATUS_XGXSSTATUS3_LINK_ALIGN                           0
#define GP_STATUS_XGXSSTATUS3_LINK_BITS                            1
#define GP_STATUS_XGXSSTATUS3_LINK_SHIFT                           15

#define READ_PHY8706_PHYXS_GP_STATUS_XGXSSTATUS3r(_unit, _pc, _val) \
             READ_PHY8706_XS_REG((_unit), (_pc), 0x00008129, (_val))
#define WRITE_PHY8706_PHYXS_GP_STATUS_XGXSSTATUS3r(_unit, _pc, _val) \
             WRITE_PHY8706_XS_REG((_unit), (_pc), 0x00008129, (_val))
#define MODIFY_PHY8706_PHYXS_GP_STATUS_XGXSSTATUS3r(_unit, _pc, _val, _mask) \
             MODIFY_PHY8706_XS_REG((_unit), (_pc), 0x00008129, (_val), (_mask))





#define READ_PHY8706_PMAD_OPTICAL_CFG_REGr(_unit, _phy_ctrl, _val) \
             PHY8706_REG_READ((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xc8e4), (_val))

#define WRITE_PHY8706_PMAD_OPTICAL_CFG_REGr(_unit, _phy_ctrl, _val) \
             PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xc8e4), (_val))

#define MODIFY_PHY8706_PMAD_OPTICAL_CFG_REGr(_unit, _phy_ctrl, _val, _mask) \
             PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xc8e4), \
             (_val), (_mask))

/************************************************/
/* User Defined Registers (Devices 1, 2, and 3) */
/************************************************/

#define READ_PHY8706_IDENTIFIERr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xc800), (_val))
#define WRITE_PHY8706_IDENTIFIERr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xc800), (_val))
#define MODIFY_PHY8706_IDENTIFIERr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xc800), \
              (_val), (_mask))

#define READ_PHY8706_PMD_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xca0a), (_val))
#define WRITE_PHY8706_PMD_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xca0a), (_val))
#define MODIFY_PHY8706_PMD_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xca0a), \
              (_val), (_mask))

#define  READ_PHY8706_TXA_ACTRL_3r(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x80a7), (_val))
#define WRITE_PHY8706_TXA_ACTRL_3r(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x80a7), (_val))
#define MODIFY_PHY8706_TXA_ACTRL_3r(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PHYXS, 0x80a7), \
              (_val), (_mask))

/* General Purpose Register 1 */
#define  READ_PHY8706_GENREG1r(_unit, _phy_ctrl, _val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xca19), (_val))
#define WRITE_PHY8706_GENREG1r(_unit, _phy_ctrl, _val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xca19), (_val))
#define MODIFY_PHY8706_GENREG1r(_unit, _phy_ctrl, _val, _mask) \
            PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_PMA_PMD, 0xca19), \
              (_val), (_mask))

#define    PHY8706_PMAD_SPA_CTRL_REG       0xC848
#define    PHY8706_PMAD_RX_ALARM_STAT_REG  0x9003
#define    PHY8706_PMAD_M8051_MSGIN_REG    0xCA12
#define    PHY8706_PMAD_M8051_MSGOUT_REG   0xCA13

/************************************************/
/* User Defined Registers (Devices 4)           */
/************************************************/

#define  PHY8706_TXA_CTRL_REG   0x80a1
#define  PHY8706_TX0_CTRL_REG   0x8061
#define  PHY8706_TX1_CTRL_REG   0x8071
#define  PHY8706_TX2_CTRL_REG   0x8081
#define  PHY8706_TX3_CTRL_REG   0x8091
#define  PHY8706_TX_POLARITY_FLIP_MASK (1 << 5)

#define  PHY8706_RXA_CTRL_REG   0x80fa
#define  PHY8706_RX0_CTRL_REG   0x80ba
#define  PHY8706_RX1_CTRL_REG   0x80ca
#define  PHY8706_RX2_CTRL_REG   0x80da
#define  PHY8706_RX3_CTRL_REG   0x80ea
#define  PHY8706_RX_POLARITY_FLIP_MASK (3 << 2)


/*   Auto-Neotiation device */
                                                                                
#define READ_PHY8706_AN_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8706_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_AN, (_reg)), (_val))
#define WRITE_PHY8706_AN_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8706_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_AN, (_reg)), (_val))
#define MODIFY_PHY8706_AN_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8706_REG_MODIFY((_unit), (_phy_ctrl), \
	SOC_PHY_CLAUSE45_ADDR(PHY8706_C45_DEV_AN, (_reg)), (_val), (_mask))

/* autoneg control register */
                                                                               
#define AN_CTRL_REG        0
#define AN_EXT_NXT_PAGE    (1 << 13)
#define AN_ENABLE          (1 << 12) 
#define AN_RESTART         (1 << 9)
                                                                               
/* autoneg status register */
                                                                               
#define AN_STATUS_REG      1
#define AN_LINK            (1 << 2)
#define AN_DONE            (1 << 5)
                                                                               
/* BP ethernet status register */
#define AN_ETH_STATUS_REG      0x30
#define AN_SPEED_1000BASE_KX   (1 << 1)
#define AN_SPEED_10GBASE_KR    (1 << 3)

/* 1G status register for both autoneg mode and forced mode */
#define AN_1G_STATUS_REG       0x8304
#define AN_1G_LINKUP           0x2
#define AN_1G_LINK_CHANGE      0x80

#endif  /* _phy8706_H_ */
