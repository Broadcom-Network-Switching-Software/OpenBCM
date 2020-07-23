/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy84740.h
 */

#ifndef   _PHY84740_H_
#define   _PHY84740_H_

#include <soc/phy.h>


/* 84740 Devices */
#define PHY84740_C45_DEV_PMA_PMD     0x01
#define PHY84740_C45_DEV_PCS         0x03
#define PHY84740_C45_DEV_AN          0x07

#define PHY84740_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY84740_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY84740_REG_MODIFY(_unit, _phy_ctrl, _addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val), (_mask))

#define PHY84740_XFI(_unit, _phy_ctrl)                          \
            PHY84740_REG_MODIFY((_unit), (_phy_ctrl),            \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, (0xffff)), 1, 1)

#define PHY84740_MMF(_unit, _phy_ctrl)                          \
            PHY84740_REG_MODIFY((_unit), (_phy_ctrl),            \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, (0xffff)), 0,1)

#define PHY84740_XFI_IF_INTF(unit, _phy_ctrl, intf, interface) {\
            if(intf == interface) {\
               SOC_IF_ERROR_RETURN \
                   (PHY84740_XFI(unit, _phy_ctrl));}}

#define PHY84740_MMF_IF_INTF(unit, _phy_ctrl, intf, interface) {\
            if(intf == interface) {\
               SOC_IF_ERROR_RETURN \
                   (PHY84740_MMF(unit, _phy_ctrl));}}

/*
 * Line side
 */
/* PMA/PMD Device (Dev Addr 1) */
#define READ_PHY84740_MMF_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, (_reg)), (_val))
#define WRITE_PHY84740_MMF_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, (_reg)), (_val))
#define MODIFY_PHY84740_MMF_PMA_PMD_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, (_reg)), (_val), (_mask))

/* PCS Device (Dev Addr 3) */
#define READ_PHY84740_MMF_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val))
#define WRITE_PHY84740_MMF_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val))
#define MODIFY_PHY84740_MMF_PCS_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val), (_mask))

/* AN Device (Dev Addr 7) */
#define READ_PHY84740_MMF_AN_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_AN, (_reg)), (_val))
#define WRITE_PHY84740_MMF_AN_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_AN, (_reg)), (_val))
#define MODIFY_PHY84740_MMF_AN_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_AN, (_reg)), (_val), (_mask))


/*
 * XFI Side
 */
/* PMA Device (Dev Addr 1) */
#define READ_PHY84740_XFI_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val)             \
       PHY84740_REG_READ((_unit), (_phy_ctrl),                                 \
            SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, (_reg)), (_val))   \

#define WRITE_PHY84740_XFI_PMA_PMD_REG(_unit, _phy_ctrl, _reg,_val)            \
        PHY84740_REG_WRITE((_unit), (_phy_ctrl),                               \
             SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, (_reg)), (_val))  \

#define MODIFY_PHY84740_XFI_PMA_PMD_REG(_unit, _phy_ctrl, _reg, _val, _mask)   \
     PHY84740_REG_MODIFY((_unit), (_phy_ctrl),                                 \
      SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, (_reg)), (_val), (_mask))\

/* PCS Device (Dev Addr 3) */
#define READ_PHY84740_XFI_PCS_REG(_unit, _phy_ctrl, _reg,_val)             \
       PHY84740_REG_READ((_unit), (_phy_ctrl),                                 \
            SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val))   \

#define WRITE_PHY84740_XFI_PCS_REG(_unit, _phy_ctrl, _reg,_val)            \
        PHY84740_REG_WRITE((_unit), (_phy_ctrl),                               \
             SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val))  \

#define MODIFY_PHY84740_XFI_PCS_REG(_unit, _phy_ctrl, _reg, _val, _mask)   \
     PHY84740_REG_MODIFY((_unit), (_phy_ctrl),                                 \
      SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val), (_mask))\


/* AN Device (Dev Addr 1) */
#define READ_PHY84740_XFI_AN_REG(_unit, _phy_ctrl, _reg,_val)             \
       PHY84740_REG_READ((_unit), (_phy_ctrl),                                 \
            SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_AN, (_reg)), (_val))   \

#define WRITE_PHY84740_XFI_AN_REG(_unit, _phy_ctrl, _reg,_val)            \
        PHY84740_REG_WRITE((_unit), (_phy_ctrl),                               \
             SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_AN, (_reg)), (_val))  \

#define MODIFY_PHY84740_XFI_AN_REG(_unit, _phy_ctrl, _reg, _val, _mask)   \
     PHY84740_REG_MODIFY((_unit), (_phy_ctrl),                                 \
      SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_AN, (_reg)), (_val), (_mask))\



/* PMA/PMD registers */
#define    PHY84740_PMAD_CTRL_REG            0x00
#define    PHY84740_PMAD_STAT_REG            0x01
#define    PHY84740_PMAD_ID0_REG             0x02
#define    PHY84740_PMAD_ID1_REG             0x03
#define    PHY84740_PMAD_CTRL2_REG           0x07
#define    PHY84740_PMAD_STAT2_REG           0x08

#define    PHY84740_SPEED_LINK_DETECT_STAT_REG 0xc820
#define    PHY84740_PMAD_BCST_REG            0xc8fe
#define    PHY84740_PMAD_CHIP_MODE_REG       0xc805
#define    PHY84740_PMAD_SPA_CTRL_REG        0xc848
#define    PHY84740_PMAD_MISC_CTRL1_REG      0xca85
#define    PHY84740_PMAD_M8051_MSGIN_REG     0xca12
#define    PHY84740_PMAD_M8051_MSGOUT_REG    0xca13
#define    PHY84740_PMAD_AER_ADDR_REG        0xc702
#define    PHY84740_PMAD_CHIP_MODE_MASK      0x3
#define    PHY84740_PMAD_DAC_MODE_MASK       (1 << 3)
#define    PHY84740_PMAD_BKPLANE_MODE_MASK   (1 << 2)
#define    PHY84740_PMAD_DAC_MODE            0x8
#define    PHY84740_PMAD_MODE_40G            0x1

/* logical lane0 to physical lane mapping register */
#define    PHY84740_PMAD_LANE0_L2P_REG       0xc701
#define    PHY84740_L2P_PHYSICAL_LANE_MASK   0x7    

/* chip intr value register */
#define    PHY84740_PMAD_INTR_VALUE_REG      0xc878
#define    PHY84740_AUTONEG_COMPLETE         (1 << 11)

/* Optical Configuration Register */
#define    PHY84740_PMAD_OPTICAL_CFG_REG     0xc8e4
#define    PHY84740_RXLOS_OVERRIDE_MASK      0xc0c0
#define    PHY84740_MOD_ABS_OVERRIDE_MASK    0x0808

/* Optical signal active level configuration   */
#define    PHY84740_PMAD_OPTICAL_SIG_LVL_REG 0xc800
#define    PHY84740_RXLOS_LVL_MASK           (1 << 9)
#define    PHY84740_MOD_ABS_LVL_MASK         (1 << 8)
 
/* Recovery clock enable control  */
#define    PHY84740_PMAD_RECOVERY_CLK_REG       0xcb27
#define    PHY84740_PMAD_RECOVERY_CLK_FREQ_REG  0xcb26

#define PHY84740_RECOVERY_CLK_ENABLE_MASK    (1 << 0)
#define PHY84740_RECOVERY_CLK_FREQ_SHIFT     13 
#define PHY84740_RECOVERY_CLK_FREQf(_f) ((_f) << PHY84740_RECOVERY_CLK_FREQ_SHIFT) 
#define PHY84740_RECOVERY_CLK_FREQ_MASK      (0x3 << PHY84740_RECOVERY_CLK_FREQ_SHIFT)

/* Optics digital control register */
#define PHY84740_DIG_CTRL_REG      0xcd08
#define PHY84740_TX_POLARITY_FLIP_MASK (1 << 10)
#define PHY84740_RX_POLARITY_FLIP_MASK (1 << 9)


/* PCS registers */
#define    PHY84740_PCS_CTRL_REG            0x00
#define    PHY84740_PCS_STAT_REG            0x01
#define    PHY84740_PCS_ID0_REG             0x02
#define    PHY84740_PCS_ID1_REG             0x03
#define    PHY84740_PCS_CTRL2_REG           0x07
#define    PHY84740_PCS_STAT2_REG           0x08

/* AN Registers */
#define    PHY84740_AN_CTRL_REG            0xffe0
#define    PHY84740_AN_STAT_REG            0xffe1


/* PMA/PMD Standard registers definations */
/* Control Register */
#define PHY84740_MII_CTRL_PMA_LOOPBACK      (1 << 0)

/* PMA/PMD User define registers  definations */
/* Speed Link Detect status register definations */
#define    PHY84740_1G_MODE                0x0021


/* AN registers definations */
/* AN Status Register definations */
#define    PHY84740_AN_STAT_LA                0x0004



/* Chip ID register */
#define READ_PHY84740_PMA_PMD_CHIPIDr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 0xc802), (_val))

/* Chip Revision register */
#define READ_PHY84740_PMA_PMD_CHIPREVr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 0xc801), (_val))

/* Control Register (Addr 0000h) */
#define READ_PHY84740_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 0), (_val))
#define WRITE_PHY84740_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 0), (_val))
#define MODIFY_PHY84740_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 0), (_val), (_mask))

/* Status Register (Addr 0001h) */
#define READ_PHY84740_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 1), (_val))
#define WRITE_PHY84740_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 1), (_val))
#define MODIFY_PHY84740_PMA_PMD_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 1), (_val), (_mask))

/* ID0 Register (Addr 0002h) */
/* ID1 Register (Addr 0003h) */
/* Speed Ability Register (Addr 0004h) */
/* Devices in Package 1 Register (Addr 0005h) */
/* Devices in Package 2 Register (Addr 0006h) */
/* Control 2 Register (Addr 0007h) */

#define PHY84740_PMAD_CTRL2r_PMA_TYPE_MASK    0xF
#define PHY84740_PMAD_CTRL2r_PMA_TYPE_1G_KX   0xD
#define PHY84740_PMAD_CTRL2r_PMA_TYPE_10G_KR  0xB
#define PHY84740_PMAD_CTRL2r_PMA_TYPE_10G_LRM 0x8
 
/* Status 2 Register (Addr 0008h) */

/* Transmit Disable Register (Addr 0009h) */
#define READ_PHY84740_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 9), (_val))
#define WRITE_PHY84740_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 9), (_val))
#define MODIFY_PHY84740_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val, _mask) \
            PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PMA_PMD, 9), (_val), (_mask))


/* Receive Signal Detect Register (Addr 000Ah) */
/* Organizationlly Unique Identifier 0 Register (Addr 000Eh) */
/* Organizationlly Unique Identifier 1 Register (Addr 000Fh) */

/* PCS Device (Dev Addr 3) */
#define READ_PHY84740_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val))
#define WRITE_PHY84740_PCS_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val))
#define MODIFY_PHY84740_PCS_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
        PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
        SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, (_reg)), (_val), (_mask))

/* Control 1 Register (Addr 0000h) */
#define READ_PHY84740_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, 0), (_val))
#define WRITE_PHY84740_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, 0), (_val))
#define MODIFY_PHY84740_PCS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY84740_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, 1), (_val))
#define WRITE_PHY84740_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY84740_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, 1), (_val))
#define MODIFY_PHY84740_PCS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY84740_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY84740_C45_DEV_PCS, 1), (_val), (_mask))

/* CL73 autoneg control register */

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

/* autoneg control register */
#define PHY84740_AN_EXT_NXT_PAGE    (1 << 13)
#define PHY84740_AN_ENABLE          (1 << 12) 
#define PHY84740_AN_RESTART         (1 << 9)
                                                                               
/* autoneg status register */
#define PHY84740_AN_STATUS_REG      1
#define PHY84740_AN_LINK            (1 << 2)
#define PHY84740_AN_DONE            (1 << 5)
                                                                               
/* 1G status register for both autoneg mode and forced mode */
#define AN_1G_STATUS_REG       0x8304
#define AN_1G_LINKUP           0x2
#define AN_1G_LINK_CHANGE      0x80

#endif  /* _phy84740_H_ */
