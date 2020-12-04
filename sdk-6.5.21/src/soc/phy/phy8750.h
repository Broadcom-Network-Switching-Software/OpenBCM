/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8750.h
 */

#ifndef   _PHY8750_H_
#define   _PHY8750_H_

#include <soc/phy.h>


/* 8750 Devices */
#define PHY8750_C45_DEV_PMA_PMD     0x01
#define PHY8750_C45_DEV_PCS         0x03
#define PHY8750_C45_DEV_AN          0x07

#define PHY8750_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8750_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8750_REG_MODIFY(_unit, _phy_ctrl, _addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val), (_mask))

#define PHY8750_XFI(_unit, _phy_ctrl)                          \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl),            \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, (0xffff)), 1)

#define PHY8750_MMF(_unit, _phy_ctrl)                          \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl),            \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, (0xffff)), 0)

/*
 * Line side
 */
/* PMA/PMD Device (Dev Addr 1) */
#define READ_PHY8750_MMF_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, (_reg)), (_val))
#define WRITE_PHY8750_MMF_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, (_reg)), (_val))
#define MODIFY_PHY8750_MMF_PMA_PMD_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, (_reg)), (_val), (_mask))

/* PCS Device (Dev Addr 3) */
#define READ_PHY8750_MMF_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val))
#define WRITE_PHY8750_MMF_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val))
#define MODIFY_PHY8750_MMF_PCS_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val), (_mask))

/* AN Device (Dev Addr 7) */
#define READ_PHY8750_MMF_AN_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_AN, (_reg)), (_val))
#define WRITE_PHY8750_MMF_AN_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_AN, (_reg)), (_val))
#define MODIFY_PHY8750_MMF_AN_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_AN, (_reg)), (_val), (_mask))


/*
 * XFI Side
 */
/* PMA Device (Dev Addr 1) */
#define READ_PHY8750_XFI_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val)             \
       PHY8750_REG_READ((_unit), (_phy_ctrl),                                 \
            SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, (_reg)), (_val))   \

#define WRITE_PHY8750_XFI_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val)            \
        PHY8750_REG_WRITE((_unit), (_phy_ctrl),                               \
             SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, (_reg)), (_val))  \

#define MODIFY_PHY8750_XFI_PMA_PMD_REG(_unit, _phy_ctrl, _reg, _val, _mask)   \
     PHY8750_REG_MODIFY((_unit), (_phy_ctrl),                                 \
      SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, (_reg)), (_val), (_mask))\

/* PCS Device (Dev Addr 3) */
#define READ_PHY8750_XFI_PCS_REG(_unit, _phy_ctrl, _reg,_val)             \
       PHY8750_REG_READ((_unit), (_phy_ctrl),                                 \
            SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val))   \

#define WRITE_PHY8750_XFI_PCS_REG(_unit, _phy_ctrl, _reg,_val)            \
        PHY8750_REG_WRITE((_unit), (_phy_ctrl),                               \
             SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val))  \

#define MODIFY_PHY8750_XFI_PCS_REG(_unit, _phy_ctrl, _reg, _val, _mask)   \
     PHY8750_REG_MODIFY((_unit), (_phy_ctrl),                                 \
      SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val), (_mask))\


/* AN Device (Dev Addr 1) */
#define READ_PHY8750_XFI_AN_REG(_unit, _phy_ctrl, _reg,_val)             \
       PHY8750_REG_READ((_unit), (_phy_ctrl),                                 \
            SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_AN, (_reg)), (_val))   \

#define WRITE_PHY8750_XFI_AN_REG(_unit, _phy_ctrl, _reg,_val)            \
        PHY8750_REG_WRITE((_unit), (_phy_ctrl),                               \
             SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_AN, (_reg)), (_val))  \

#define MODIFY_PHY8750_XFI_AN_REG(_unit, _phy_ctrl, _reg, _val, _mask)   \
     PHY8750_REG_MODIFY((_unit), (_phy_ctrl),                                 \
      SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_AN, (_reg)), (_val), (_mask))\



/* PMA/PMD registers */
#define    PHY8750_PMAD_CTRL_REG            0x00
#define    PHY8750_PMAD_STAT_REG            0x01
#define    PHY8750_PMAD_ID0_REG             0x02
#define    PHY8750_PMAD_ID1_REG             0x03
#define    PHY8750_PMAD_CTRL2_REG           0x07
#define    PHY8750_PMAD_STAT2_REG           0x08

#define    PHY8750_SPEED_LINK_DETECT_STAT_REG 0xc820
#define    PHY8750_PMAD_SPA_CTRL_REG       0xC848
#define    PHY8750_PMAD_RX_ALARM_STAT_REG  0x9003
#define    PHY8750_PMAD_M8051_MSGIN_REG    0xCA12
#define    PHY8750_PMAD_M8051_MSGOUT_REG   0xCA13

/* Optical Configuration Register */
#define    PHY8750_PMAD_OPTICAL_CFG_REG     0xc8e4
#define    PHY8750_RXLOS_OVERRIDE_MASK      0xc0c0
#define    PHY8750_MOD_ABS_OVERRIDE_MASK    0x0808

/* Optical signal active level configuration   */
#define    PHY8750_PMAD_OPTICAL_SIG_LVL_REG 0xc800
#define    PHY8750_RXLOS_LVL_MASK           (1 << 9)
#define    PHY8750_MOD_ABS_LVL_MASK         (1 << 8)

/* PCS registers */
#define    PHY8750_PCS_CTRL_REG            0x00
#define    PHY8750_PCS_STAT_REG            0x01
#define    PHY8750_PCS_ID0_REG             0x02
#define    PHY8750_PCS_ID1_REG             0x03
#define    PHY8750_PCS_CTRL2_REG           0x07
#define    PHY8750_PCS_STAT2_REG           0x08

/* AN Registers */
#define    PHY8750_AN_CTRL_REG            0xffe0
#define    PHY8750_AN_STAT_REG            0xffe1


/* PMA/PMD Standard registers definations */
/* Control Register */
#define PHY8750_MII_CTRL_PMA_LOOPBACK      (1 << 0)

/* PMA/PMD User define registers  definations */
/* Speed Link Detect status register definations */
#define    PHY8750_1G_MODE                0x0021


/* AN registers definations */
/* AN Status Register definations */
#define    PHY8750_AN_STAT_LA                0x0004



/* Chip ID register */
#define READ_PHY8750_PMA_PMD_CHIPIDr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 0xc802), (_val))

/* Chip Revision register */
#define READ_PHY8750_PMA_PMD_CHIPREVr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 0xc801), (_val))

/* Control Register (Addr 0000h) */
#define READ_PHY8750_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 0), (_val))
#define WRITE_PHY8750_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 0), (_val))
#define MODIFY_PHY8750_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 0), (_val), (_mask))

/* Status Register (Addr 0001h) */
#define READ_PHY8750_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 1), (_val))
#define WRITE_PHY8750_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 1), (_val))
#define MODIFY_PHY8750_PMA_PMD_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 1), (_val), (_mask))

/* ID0 Register (Addr 0002h) */
/* ID1 Register (Addr 0003h) */
/* Speed Ability Register (Addr 0004h) */
/* Devices in Package 1 Register (Addr 0005h) */
/* Devices in Package 2 Register (Addr 0006h) */
/* Control 2 Register (Addr 0007h) */

#define PHY8750_PMAD_CTRL2r_PMA_TYPE_MASK    0xF
#define PHY8750_PMAD_CTRL2r_PMA_TYPE_1G_KX   0xD
#define PHY8750_PMAD_CTRL2r_PMA_TYPE_10G_KR  0xB
#define PHY8750_PMAD_CTRL2r_PMA_TYPE_10G_LRM 0x8
 
/* Status 2 Register (Addr 0008h) */

/* Transmit Disable Register (Addr 0009h) */
#define READ_PHY8750_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 9), (_val))
#define WRITE_PHY8750_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 9), (_val))
#define MODIFY_PHY8750_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val, _mask) \
            PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PMA_PMD, 9), (_val), (_mask))


/* Receive Signal Detect Register (Addr 000Ah) */
/* Organizationlly Unique Identifier 0 Register (Addr 000Eh) */
/* Organizationlly Unique Identifier 1 Register (Addr 000Fh) */

/* PCS Device (Dev Addr 3) */
#define READ_PHY8750_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val))
#define WRITE_PHY8750_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val))
#define MODIFY_PHY8750_PCS_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, (_reg)), (_val), (_mask))

/* Control 1 Register (Addr 0000h) */
#define READ_PHY8750_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, 0), (_val))
#define WRITE_PHY8750_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, 0), (_val))
#define MODIFY_PHY8750_PCS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY8750_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, 1), (_val))
#define WRITE_PHY8750_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY8750_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, 1), (_val))
#define MODIFY_PHY8750_PCS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8750_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8750_C45_DEV_PCS, 1), (_val), (_mask))

/* autoneg control register */
#define PHY8750_AN_EXT_NXT_PAGE    (1 << 13)
#define PHY8750_AN_ENABLE          (1 << 12) 
#define PHY8750_AN_RESTART         (1 << 9)
                                                                               
/* autoneg status register */
#define PHY8750_AN_STATUS_REG      1
#define PHY8750_AN_LINK            (1 << 2)
#define PHY8750_AN_DONE            (1 << 5)
                                                                               
/* 1G status register for both autoneg mode and forced mode */
#define AN_1G_STATUS_REG       0x8304
#define AN_1G_LINKUP           0x2
#define AN_1G_LINK_CHANGE      0x80

#endif  /* _phy8750_H_ */
