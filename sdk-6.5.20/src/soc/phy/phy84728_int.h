
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PHY84728_INT_H
#define _PHY84728_INT_H

/****************************************************************************
 * Firmware related:
 */
#define BSDK_PHY84728_FW_CHECKSUM         0x600D
#define BSDK_PHY84728_FW_NUM_ITER         5 

#define BSDK_PHY84728_FW_DWNLD_DONE_MSG   0x4321
#define BSDK_PHY84728_FW_TOTAL_WR_BYTE    0x4000

#define BSDK_PHY84728_PMAD_M8051_MSGIN_REG     0xca12
#define BSDK_PHY84728_PMAD_M8051_MSGOUT_REG    0xca13

/************************************************************************
 *
 *                         Channel 0 and 1 : LINE : Device 1 (PMA/PMD)
 */

/* Speed Link status register defination */
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_10M    (1 << 0)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_100M   (1 << 1)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_1G     (1 << 2)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_10G    (1 << 4)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_AN_COMPLETE      (1 << 6)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_RX_SD            (1 << 7)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_10M   (1 << 8)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_100M  (1 << 9)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_1G    (1 << 10)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_10G   (1 << 12)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_AN_COMPLETE     (1 << 14)
#define BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_RX_SD           (1 << 15)

#define BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_MASK                \
                    (BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_10M |  \
                     BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_100M | \
                     BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_1G |   \
                     BSDK_PHY84728_PMD_SPEED_LD_STAT_LN_PDM_SPEED_10G)

#define BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_MASK                \
                    (BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_10M |  \
                     BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_100M | \
                     BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_1G |   \
                     BSDK_PHY84728_PMD_SPEED_LD_STAT_SYS_PDM_SPEED_10G)


/* DEV1 MACSEC Control register defination */
#define BSDK_PHY84728_LN_DEV1_MACSEC_CTRL_BYPASS_MODE            (1 << 0)
#define BSDK_PHY84728_LN_DEV1_MACSEC_CTRL_ENABLE_PWRDN_MACSEC    (1 << 8)
#define BSDK_PHY84728_LN_DEV1_MACSEC_CTRL_UDSW_PWRDW_MACSEC      (1 << 11)
#define BSDK_PHY84728_LN_DEV1_MACSEC_CTRL_UDSW_RESET_MACSEC      (1 << 15)

/* Chip Mode Resister (Addr 0xc805) */
#define BSDK_PHY84728_LN_DEV1_CHIP_MODE_DAC_MASK                 (1 << 3)
#define BSDK_PHY84728_LN_DEV1_CHIP_MODE_BKPLANE_MASK             (1 << 2)



/* PMD Control Register definations */
#define BSDK_PHY84728_PMD_CTRL_PMS_LOOPBACK  (1 << 0) /* Loopback */
#define BSDK_PHY84728_PMD_CTRL_SS_MSB        (1 << 6) /* Speed select, MSb */
#define BSDK_PHY84728_PMD_CTRL_PD            (1 << 11) /* Power Down */
#define BSDK_PHY84728_PMD_CTRL_SS_LSB        (1 << 13) /* Speed select, LSb */
#define BSDK_PHY84728_PMD_CTRL_RESET         (1 << 15) /* PHY reset */

#define BSDK_PHY84728_PMD_CTRL_SS_SPEED_SEL   \
                                        (BSDK_PHY84728_PMD_CTRL_SS_LSB | \
                                         BSDK_PHY84728_PMD_CTRL_SS_MSB)
#define BSDK_PHY84728_PMD_CTRL_SS_MASK        \
                                        (BSDK_PHY84728_PMD_CTRL_SS_LSB | \
                                         BSDK_PHY84728_PMD_CTRL_SS_MSB)

#define BSDK_PHY84728_PMD_CTRL_10GSS_MASK     \
                                        (BSDK_PHY84728_PMD_CTRL_SS_LSB  | \
                                         BSDK_PHY84728_PMD_CTRL_SS_MSB  | \
                                         (1 << 2) | \
                                         (1 << 3) | \
                                         (1 << 4) | \
                                         (1 << 5))

#define BSDK_PHY84728_PMD_CTRL_SS_10       0
#define BSDK_PHY84728_PMD_CTRL_SS_100      \
                                             (BSDK_PHY84728_PMD_CTRL_SS_LSB)
#define BSDK_PHY84728_PMD_CTRL_SS_1000     (BSDK_PHY84728_PMD_CTRL_SS_MSB)
#define BSDK_PHY84728_PMD_CTRL_SS_10000    \
                                     (BSDK_PHY84728_PMD_CTRL_SS_SPEED_SEL | \
                                     (0x0000))


#define BSDK_PHY84728_PMD_CTRL_SS(_x)      \
                                       ((_x) &  \
                                        (BSDK_PHY84728_PMD_CTRL_SS_LSB | \
                                         BSDK_PHY84728_PMD_CTRL_SS_MSB))



/* PMD Status Register defination */
#define BSDK_PHY84728_PMD_STAT_RX_LINK_STATUS    (1 << 2)



/* PMD Control2 Resister definations */
#define BSDK_PHY84728_PMD_CTRL2_PMA_1GTPMD_TYPE     (0xc)
#define BSDK_PHY84728_PMD_CTRL2_PMA_10GLRMPMD_TYPE  (0x8)

#define BSDK_PHY84728_PMD_CTRL2_PMA_SELECT_MASK     (0xf)

/************************************************************************
 *
 *                         LINE : Device 3 (PCS)
 */
 /*PCS Control register definations */
 #define BSDK_PHY84728_PCS_PCS_CTRL_PD                     (1 << 11)


 /*PCS Status register definations */
 #define BSDK_PHY84728_PCS_PCS_STAT_RX_LINK_STATUS         (1 << 2)



/************************************************************************
 *
 *                         LINE : Device 7 (AN)
 */

/*
 * MII Link Advertisment (Clause 37) 
 */
#define BSDK_PHY84728_DEV7_1000X_ANA_C37_FD          (1 << 5)
#define BSDK_PHY84728_DEV7_1000X_ANA_C37_HD          (1 << 6)
#define BSDK_PHY84728_DEV7_1000X_ANA_C37_PAUSE       (1 << 7)
#define BSDK_PHY84728_DEV7_1000X_ANA_C37_ASYM_PAUSE  (1 << 8)


/*
 * MII Link Partner Advertisment (Clause 37) 
 */
#define BSDK_PHY84728_SGMII_ANP_SGMII_MODE    (1 << 0)
#define BSDK_PHY84728_SGMII_ANP_FD            (1 << 5)
#define BSDK_PHY84728_SGMII_ANP_HD            (1 << 6)
#define BSDK_PHY84728_SGMII_ANP_C37_PAUSE     (1 << 7)
#define BSDK_PHY84728_SGMII_ANPC37_ASYM_PAUSE (1 << 8)




/*AN MII control register definations */
#define BSDK_PHY84728_AN_MII_CTRL_SS_MSB    (1 << 6) /* Speed select, MSb */
#define BSDK_PHY84728_AN_MII_CTRL_FD        (1 << 8) /* Full Duplex */
#define BSDK_PHY84728_AN_MII_CTRL_RAN       (1 << 9) /* Restart Autoneg `*/
#define BSDK_PHY84728_AN_MII_CTRL_PD        (1 << 11) /* Power Down */
#define BSDK_PHY84728_AN_MII_CTRL_AE        (1 << 12) /* Autoneg enable */
#define BSDK_PHY84728_AN_MII_CTRL_SS_LSB    (1 << 13) /* Speed select, LSb */
#define BSDK_PHY84728_AN_MII_CTRL_LE        (1 << 14) /* Loopback enable */
#define BSDK_PHY84728_AN_MII_CTRL_RESET     (1 << 15) /* PHY reset */

#define BSDK_PHY84728_AN_MII_CTRL_SS(_x)    \
                                        ((_x) &  \
                                         (BSDK_PHY84728_AN_MII_CTRL_SS_LSB | \
                                          BSDK_PHY84728_AN_MII_CTRL_SS_MSB))
#define BSDK_PHY84728_AN_MII_CTRL_SS_10   0
#define BSDK_PHY84728_AN_MII_CTRL_SS_100  (BSDK_PHY84728_AN_MII_CTRL_SS_LSB)
#define BSDK_PHY84728_AN_MII_CTRL_SS_1000 (BSDK_PHY84728_AN_MII_CTRL_SS_MSB)
#define BSDK_PHY84728_AN_MII_CTRL_SS_INVALID \
                                        (BSDK_PHY84728_AN_MII_CTRL_SS_LSB | \
                                         BSDK_PHY84728_AN_MII_CTRL_SS_MSB)
#define BSDK_PHY84728_AN_MII_CTRL_SS_MASK   \
                                        (BSDK_PHY84728_AN_MII_CTRL_SS_LSB | \
                                         BSDK_PHY84728_AN_MII_CTRL_SS_MSB)

/* AN MII Status register definations */
#define BSDK_PHY84728_AN_MII_STAT_LINK_STATUS    (1 << 2)
#define BSDK_PHY84728_AN_MII_STAT_AN_DONE        (1 << 5)


/* AN 1000x Control1 Register definations */
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_FIBER_MODE         (1 << 0)
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_TBI_INT            (1 << 1)
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_SD_ENABLE          (1 << 2)
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_INVERT_SD          (1 << 3)
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_AUTODETECT_EN      (1 << 4)
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_SGMII_MASTER       (1 << 5)
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_DIS_PLL_PWRDN      (1 << 6)
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_CRC_CHK_DIS        (1 << 7)
#define BSDK_PHY84728_AN_BASE1000X_CTRL1_COMMA_DET_EN       (1 << 8)

/* 1000x Status1 Register definations */
#define BSDK_PHY84728_AN_BASE1000X_STAT1_SGMII_MODE      (1 << 0)
#define BSDK_PHY84728_AN_BASE1000X_STAT1_LINK_STATUS     (1 << 1)
#define BSDK_PHY84728_AN_BASE1000X_STAT1_DUPLEX_STATUS   (1 << 2)
#define BSDK_PHY84728_AN_BASE1000X_STAT1_SPEED_STATUS    ((1 << 3) | \
                                                             (1 << 4))
#define BSDK_PHY84728_AN_BASE1000X_STAT1_PAUSE_TX        (1 << 5)
#define BSDK_PHY84728_AN_BASE1000X_STAT1_PAUSE_RX        (1 << 6)


#define BSDK_PHY84728_AN_MISC2_RX_MII_GEN_SEL               (1 << 13)
#define BSDK_PHY84728_AN_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN  (1 << 5)

#define BSDK_PHY84728_AN_XGXS_CTRL_START_SEQ                (1 << 13)

/* AN Control Register */
#define BSDK_PHY84728_AN_AN_CTRL_RAN         (1 << 9) /* Restart AutoNeg */
#define BSDK_PHY84728_AN_AN_CTRL_AE          (1 << 12) /* Enable AutoNegn */
#define BSDK_PHY84728_AN_AN_CTRL_RESET       (1 << 15) /* Reset AutoNeg */

/* AN Status Register */
#define BSDK_PHY84728_AN_AN_STAT_AN_DONE     (1 << 5) 


extern int
bsdk_phy84728_reg_read(phy_ctrl_t * pc, uint32 flags, uint8 reg_bank,
                         uint16 reg_addr, uint16 *data);
extern int
bsdk_phy84728_reg_write(phy_ctrl_t * pc, uint32 flags, uint8 reg_bank,
                         uint16 reg_addr, uint16 data);
extern int
bsdk_phy84728_reg_modify(phy_ctrl_t * pc, uint32 flags, uint8 reg_bank,
                         uint16 reg_addr, uint16 data, uint16 mask);

/* Flag indicating that Line side Access or System side access */
#define BSDK_PHY84728_LINE_SIDE       0
#define BSDK_PHY84728_SYS_SIDE        1


#undef  READ_PHY_REG
#define READ_PHY_REG(_unit, _pc,  _addr, _value) \
            ((_pc->read)((_pc->unit), (_pc->phy_id), (_addr), (_value)))
#undef  WRITE_PHY_REG
#define WRITE_PHY_REG(_unit, _pc, _addr, _value) \
            ((_pc->write)((_pc->unit), (_pc->phy_id), (_addr), (_value)))


#define BSDK_PHY84728_IO_MDIO_READ(_phy_ctrl,  _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))

#define BSDK_PHY84728_IO_MDIO_WRITE(_phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))

/* General - PHY register access */
#define BSDK_PHY84728_REG_RD(_pctrl, _flags, _reg_bank, _reg_addr, _val) \
        bsdk_phy84728_reg_read((_pctrl), (_flags), (_reg_bank),      \
                                 (_reg_addr), (_val))

#define BSDK_PHY84728_REG_WR(_pctrl, _flags, _reg_bank, _reg_addr, _val) \
        bsdk_phy84728_reg_write((_pctrl), (_flags), (_reg_bank),     \
                                       (_reg_addr), (_val))

#define BSDK_PHY84728_REG_MOD(_pctrl, _flags, _reg_bank, _reg_addr,      \
                                                            _val, _mask)    \
        bsdk_phy84728_reg_modify((_pctrl), (_flags), (_reg_bank),    \
                                        (_reg_addr), (_val), (_mask))


/* 
 * 10G/1G : Line Side PHY registers
 */

/* Device 1 */
/* PCS User Defined Registers */

/* Chip Revision Resister (Addr 0xc801) */
#define BSDK_RD_PHY84728_LN_DEV1_CHIP_REVr(_pctrl, _val)                  \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xc801, (_val))

/* Chip ID Resister (Addr 0xc802) */
#define BSDK_RD_PHY84728_LN_DEV1_CHIP_IDr(_pctrl, _val)                   \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xc802, (_val))

/* Speed Link detect Status Resister (Addr 0xc81f) */
#define BSDK_RD_PHY84728_LN_DEV1_SPEED_LINK_DETECT_STATr(_pctrl, _val)    \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xc81f, (_val))

/* Chip Mode Resister (Addr 0xc805) */
#define BSDK_RD_PHY84728_LN_DEV1_CHIP_MODEr(_pctrl, _val)        \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xc805, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_CHIP_MODEr(_pctrl, _val)        \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0xc805, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_CHIP_MODEr(_pctrl, _val, _mask)\
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0xc805,        \
                                     (_val), (_mask))

/* MACSEC Bypass Control Resister (Addr 0xc8f0) */
#define BSDK_RD_PHY84728_LN_DEV1_MACSEC_BYPASS_CTLRr(_pctrl, _val)        \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xc8f0, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_MACSEC_BYPASS_CTLRr(_pctrl, _val)        \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0xc8f0, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_MACSEC_BYPASS_CTLRr(_pctrl, _val, _mask)\
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0xc8f0,        \
                                     (_val), (_mask))

/* XPMD system Select Resister (Addr 0xc8f1) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_SYS_SELr(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xc8f1, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_SYS_SELr(_pctrl, _val)               \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0xc8f1, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_SYS_SELr(_pctrl, _val, _mask)       \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0xc8f1,        \
                                     (_val), (_mask))



/* PMD Standard Registers */

/* PMD Control Resister (Addr 0x0000) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0000, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0000, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRLr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0000,       \
                                     (_val), (_mask))

/* PMD Status Resister (Addr 0x0001) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0001, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0001, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_STATr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0001,       \
                                     (_val), (_mask))

/* PMD ID0 Resister (Addr 0x0002) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0002, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0002, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_ID0r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0002,       \
                                     (_val), (_mask))
/* PMD ID1 Resister (Addr 0x0003) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0003, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0003, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_ID1r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0003,       \
                                     (_val), (_mask))

/* PMD Speed Ability Resister (Addr 0x0004) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0004, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0004, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0004,         \
                                     (_val), (_mask))

/* PMD Devices in Package1 Resister (Addr 0x0005) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0005, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0005, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0005,         \
                                     (_val), (_mask))

/* PMD Devices in Package2 Resister (Addr 0x0006) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0006, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0006, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0006,         \
                                     (_val), (_mask))

/* PMD Control2 Resister (Addr 0x0007) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_CTRL2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0007, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_CTRL2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0007, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_CTRL2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0007,         \
                                     (_val), (_mask))

/* PMD Status2 Resister (Addr 0x0008) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_STAT2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0008, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_STAT2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0008, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_STAT2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0008,         \
                                     (_val), (_mask))

/* PMD Transmit Disable Resister (Addr 0x0009) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val)          \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x0009, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val)          \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x0009, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val, _mask)  \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x0009,         \
                                     (_val), (_mask))

/* PMD Receive Signal Detect Resister (Addr 0x000a) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_RX_SDr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x000a, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_RX_SDr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x000a, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_RX_SDr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x000a,         \
                                     (_val), (_mask))

/* PMD Extended Ability Resister (Addr 0x000b) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_EXT_ABILITYr(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x000b, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_EXT_ABILITYr(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x000b, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_EXT_ABILITYr(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x000b,         \
                                     (_val), (_mask))

/* PMD OUI ID0 Resister (Addr 0x000e) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_OUI_ID0r(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x000e, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_OUI_ID0r(_pctrl, _val)               \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x000e, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_OUI_ID0r(_pctrl, _val, _mask)       \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x000e,         \
                                     (_val), (_mask))
/* PMD OUI ID1 Resister (Addr 0x000e) */
#define BSDK_RD_PHY84728_LN_DEV1_PMD_OUI_ID1r(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x000f, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMD_OUI_ID1r(_pctrl, _val)               \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x000f, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMD_OUI_ID1r(_pctrl, _val, _mask)       \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x000f,         \
                                     (_val), (_mask))



/*
 * Device 3 registers 
 */
/* PCS Registers */

/* PCS Control Resister (Addr 0x0000) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0000, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0000, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_CTRLr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0000,       \
                                     (_val), (_mask))

/* PCS Status Resister (Addr 0x0001) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0001, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0001, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_STATr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0001,       \
                                     (_val), (_mask))

/* PCS ID0 Resister (Addr 0x0002) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0002, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0002, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_ID0r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0002,       \
                                     (_val), (_mask))
/* PCS ID1 Resister (Addr 0x0003) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0003, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0003, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_ID1r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0003,       \
                                     (_val), (_mask))

/* PCS Speed Ability Resister (Addr 0x0004) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0004, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0004, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0004,         \
                                     (_val), (_mask))

/* PCS Devices in Package1 Resister (Addr 0x0005) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0005, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0005, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0005,         \
                                     (_val), (_mask))

/* PCS Devices in Package2 Resister (Addr 0x0006) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0006, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0006, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0006,         \
                                     (_val), (_mask))

/* PCS Control2 Resister (Addr 0x0007) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_CTRL2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0007, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_CTRL2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0007, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_CTRL2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0007,         \
                                     (_val), (_mask))

/* PCS Status2 Resister (Addr 0x0008) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_STAT2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0008, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_STAT2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0008, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_STAT2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0008,         \
                                     (_val), (_mask))

/* PCS TenGBASE-X Status Register Resister (Addr 0x0018) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val)        \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0018, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val)        \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0018, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0018,         \
                                     (_val), (_mask))

/* PCS TenGBASE-R Status Register Resister (Addr 0x0020) */
#define BSDK_RD_PHY84728_LN_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val)        \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, 0x0020, (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val)        \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, 0x0020, (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, 0x0020,          \
                                     (_val), (_mask))


/*
 * Device 7 Registers
 */

/* AN User Defined registers */

/* XGXS control Register */
#define BSDK_RD_PHY84728_LN_DEV7_XGXS_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x7, 0x8000, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_XGXS_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x7, 0x8000, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_XGXS_CTRLr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x7, 0x8000,         \
                                     (_val), (_mask))

/* 1000X Control1 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x8300, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x8300, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x8300,         \
                                     (_val), (_mask))

/* 1000X Control2 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x8301, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x8301, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x8301,          \
                                     (_val), (_mask))

/* 1000X Control3 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x8302, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x8302, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x8302,         \
                                     (_val), (_mask))

/* 1000X Control4 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x8303, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x8303, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x8303,         \
                                     (_val), (_mask))

/* 1000X Status1 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_BASE1000X_STAT1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x8304, (_val)) 

/* 1000X Status2 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_BASE1000X_STAT2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x8305, (_val)) 

/* MISC1 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_MISC1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x8308, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_MISC1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x8308, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_MISC1r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x8308,         \
                                     (_val), (_mask))

/* MISC2 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_MSIC2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x8309, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_MSIC2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x8309, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_MSIC2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x8309,         \
                                     (_val), (_mask))

/* SGMII BASEPAGE Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_SGMII_BASEPAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x835c, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_SGMII_BASEPAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x835c, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_SGMII_BASEPAGEr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x835c,         \
                                     (_val), (_mask))


/* MII Control Register */

/* MII Control Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_MII_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0xffe0, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_MII_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0xffe0, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_MII_CTRLr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe0,         \
                                     (_val), (_mask))

/* MII Status Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_MII_STATr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0xffe1, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_MII_STATr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0xffe1, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_MII_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe1,         \
                                     (_val), (_mask))

/* AutoNeg Adv Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_ANAr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0xffe4, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_ANAr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0xffe4, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_ANAr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe4,         \
                                     (_val), (_mask))
/* Auto Neg Link Partner Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_ANPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0xffe5, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_ANPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0xffe5, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_ANPr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe5,         \
                                     (_val), (_mask))

/* Auto Neg Expansion Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_EXRr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0xffe6, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_EXRr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0xffe6, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_EXRr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe6,         \
                                     (_val), (_mask))

/* Auto Neg Next Page Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_NEXT_PAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0xffe7, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_NEXT_PAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0xffe7, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_NEXT_PAGEr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe7,         \
                                     (_val), (_mask))

/* Auto Neg Link Partner Ability 2 Register */
#define BSDK_RD_PHY84728_LN_DEV7_AN_ANP2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0xffe8, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_AN_ANP2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0xffe8, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_AN_ANP2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe8,         \
                                     (_val), (_mask))

/* Extended Status 2 Register */
#define BSDK_RD_PHY84728_LN_DEV7_ESPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0xffef, (_val)) 
#define BSDK_WR_PHY84728_LN_DEV7_ESPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0xffef, (_val)) 
#define BSDK_MOD_PHY84728_LN_DEV7_ESPr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0xffef,         \
                                     (_val), (_mask))


/* AN Standard registers */

/* AN Control Resister (Addr 0x0000) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0000, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0000, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_CTRLr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0000,       \
                                     (_val), (_mask))

/* AN Status Resister (Addr 0x0001) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0001, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0001, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_STATr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0001,       \
                                     (_val), (_mask))

/* AN ID0 Resister (Addr 0x0002) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0002, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0002, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_ID0r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0002,       \
                                     (_val), (_mask))
/* AN ID1 Resister (Addr 0x0003) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0003, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0003, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_ID1r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0003,       \
                                     (_val), (_mask))

/* AN Speed Ability Resister (Addr 0x0004) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_SPEED_ABILITYr(_pctrl, _val)          \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0004, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_SPEED_ABILITYr(_pctrl, _val)          \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0004, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_SPEED_ABILITYr(_pctrl, _val, _mask)  \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0004,         \
                                     (_val), (_mask))

/* AN Devices in Package1 Resister (Addr 0x0005) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val)            \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0005, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val)            \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0005, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val, _mask)    \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0005,         \
                                     (_val), (_mask))

/* AN Devices in Package2 Resister (Addr 0x0006) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val)            \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0006, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val)            \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0006, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val, _mask)    \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0006,         \
                                     (_val), (_mask))

/* AN Advertisement 1 Resister (Addr 0x0010) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_ADV1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0010, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_ADV1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0010, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_ADV1r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0010,       \
                                     (_val), (_mask))

/* AN Advertisement 2 Resister (Addr 0x0011) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_ADV2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0011, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_ADV2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0011, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_ADV2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0011,       \
                                     (_val), (_mask))

/* AN Advertisement 3 Resister (Addr 0x0012) */
#define BSDK_RD_PHY84728_LN_DEV7_AN_ADV3r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, 0x0012, (_val))
#define BSDK_WR_PHY84728_LN_DEV7_AN_ADV3r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, 0x0012, (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_AN_ADV3r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, 0x0012,       \
                                     (_val), (_mask))

#define BSDK_RD_PHY84728_LN_DEV1_PMDr(_pctrl, _reg, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, (_reg), (_val))
#define BSDK_WR_PHY84728_LN_DEV1_PMDr(_pctrl, _reg, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, (_reg), (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_PMDr(_pctrl, _reg, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, (_reg),         \
                                     (_val), (_mask))

#define BSDK_RD_PHY84728_LN_DEV3_PCSr(_pctrl, _reg, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0003, (_reg), (_val))
#define BSDK_WR_PHY84728_LN_DEV3_PCSr(_pctrl, _reg, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0003, (_reg), (_val))
#define BSDK_MOD_PHY84728_LN_DEV3_PCSr(_pctrl, _reg, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0003, (_reg),         \
                                     (_val), (_mask))

#define BSDK_RD_PHY84728_LN_DEV7_ANr(_pctrl, _reg, _val)                  \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0007, (_reg), (_val))
#define BSDK_WR_PHY84728_LN_DEV7_ANr(_pctrl, _reg, _val)                  \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0007, (_reg), (_val))
#define BSDK_MOD_PHY84728_LN_DEV7_ANr(_pctrl, _reg, _val, _mask)          \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0007, (_reg),         \
                                     (_val), (_mask))

/* 
 * 10G/1G : System Side PHY registers
 */

/* Device 1 */
/* PMD Control Resister (Addr 0x0000) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0000, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0000, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_CTRLr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0000,       \
                                     (_val), (_mask))

/* PMD Status Resister (Addr 0x0001) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0001, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0001, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_STATr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0001,       \
                                     (_val), (_mask))

/* PMD ID0 Resister (Addr 0x0002) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0002, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0002, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_ID0r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0002,       \
                                     (_val), (_mask))
/* PMD ID1 Resister (Addr 0x0003) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0003, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0003, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_ID1r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0003,       \
                                     (_val), (_mask))

/* PMD Speed Ability Resister (Addr 0x0004) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val)       \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0004, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val)       \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0004, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0004,       \
                                     (_val), (_mask))

/* PMD Devices in Package1 Resister (Addr 0x0005) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0005, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0005, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0005,       \
                                     (_val), (_mask))

/* PMD Devices in Package2 Resister (Addr 0x0006) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0006, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0006, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val, _mask)  \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0006,       \
                                     (_val), (_mask))

/* PMD Control 2 Resister (Addr 0x0007) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_CTRL2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0007, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_CTRL2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0007, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_CTRL2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0007,       \
                                     (_val), (_mask))
/* PMD Status 2 Resister (Addr 0x0007) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_STAT2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0008, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_STAT2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0008, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_STAT2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0008,       \
                                     (_val), (_mask))

/* PMD Transmit Disable Resister (Addr 0x0009) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x0009, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x0009, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x0009,       \
                                     (_val), (_mask))

/* PMD Receive Signal Detect Resister (Addr 0x000a) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_RX_SDr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x000a, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_RX_SDr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x000a, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_RX_SDr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x000a,       \
                                     (_val), (_mask))

/* PMD Extended Ability Resister (Addr 0x000b) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_EXT_ABILITYr(_pctrl, _val)          \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x000b, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_EXT_ABILITYr(_pctrl, _val)          \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x000b, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_EXT_ABILITYr(_pctrl, _val, _mask)  \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x000b,       \
                                     (_val), (_mask))

/* PMD OUI ID0 Resister (Addr 0x000e) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_OUI_ID0r(_pctrl, _val)              \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x000e, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_OUI_ID0r(_pctrl, _val)              \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x000e, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_OUI_ID0r(_pctrl, _val, _mask)      \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x000e,       \
                                     (_val), (_mask))
/* PMD OUI ID1 Resister (Addr 0x000e) */
#define BSDK_RD_PHY84728_SYS_DEV1_PMD_OUI_ID1r(_pctrl, _val)              \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0x000f, (_val))
#define BSDK_WR_PHY84728_SYS_DEV1_PMD_OUI_ID1r(_pctrl, _val)              \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0x000f, (_val))
#define BSDK_MOD_PHY84728_SYS_DEV1_PMD_OUI_ID1r(_pctrl, _val, _mask)      \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0x000f,       \
                                     (_val), (_mask))


#define BSDK_RD_PHY84728_PMD_MISC2_CTRLr(_pctrl, _val) \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0xca08, (_val))
#define BSDK_WR_PHY84728_PMD_MISC2_CTRLr(_pctrl, _val) \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0xca08, (_val))
#define BSDK_MOD_PHY84728_PMD_MISC2_CTRLr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0xca08,       \
                                     (_val), (_mask))

#define  PMD_MISC_CTRL_WAN_MODE     (1 << 0)
#define BSDK_RD_PHY84728_PMD_MISC_CTRLr(_pctrl, _val) \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0xc812, (_val))
#define BSDK_WR_PHY84728_PMD_MISC_CTRLr(_pctrl, _val) \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0xc812, (_val))
#define BSDK_MOD_PHY84728_PMD_MISC_CTRLr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0xc812,       \
                                     (_val), (_mask))

#define BSDK_RD_PHY84728_IDENTIFIERr(_pctrl, _val) \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0001, 0xc800, (_val))
#define BSDK_WR_PHY84728_IDENTIFIERr(_pctrl, _val) \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0001, 0xc800, (_val))
#define BSDK_MOD_PHY84728_IDENTIFIERr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0001, 0xc800,       \
                                     (_val), (_mask))

/* WIS Device (Dev Addr 2) */
/* Control 1 Register (Addr 0000h) */
#define BSDK_PHY84728_WIS_CTRL_RESET         (1 << 15) /* PHY reset */
#define BSDK_RD_PHY84728_WIS_CTRLr(_pctrl, _val) \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0002, 0x0000, (_val))
#define BSDK_WR_PHY84728_WIS_CTRLr(_pctrl, _val) \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0002, 0x0000, (_val))
#define BSDK_MOD_PHY84728_WIS_CTRLr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0002, 0x0000,       \
                                     (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define BSDK_RD_PHY84728_WIS_STATr(_pctrl, _val) \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0002, 0x0001, (_val))
#define BSDK_WR_PHY84728_WIS_STATr(_pctrl, _val) \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0002, 0x0001, (_val))
#define BSDK_MOD_PHY84728_WIS_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0002, 0x0001,       \
                                     (_val), (_mask))
/* ID0 Register (Addr 0002h) */
/* ID1 Register (Addr 0003h) */
/* Speed Ability Register (Addr 0004h) */

/* Devices in Package 1 Register (Addr 0005h) */
#define BSDK_RD_PHY84728_WIS_DEV_IN_PKGr(_pctrl, _val) \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0002, 0x0005, (_val))
#define BSDK_WR_PHY84728_WIS_DEV_IN_PKGr(_pctrl, _val) \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0002, 0x0005, (_val))
#define BSDK_MOD_PHY84728_WIS_DEV_IN_PKGr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0002, 0x0005,       \
                                     (_val), (_mask))

/*
 * Device 3 registers 
 */
/* XFI PCS Registers */

/* PCS Control Resister (Addr 0x0000) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0000, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0000, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_CTRLr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0000,       \
                                     (_val), (_mask))

/* PCS Status Resister (Addr 0x0001) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0001, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0001, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_STATr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0001,       \
                                     (_val), (_mask))

/* PCS ID0 Resister (Addr 0x0002) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0002, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0002, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_ID0r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0002,       \
                                     (_val), (_mask))
/* PCS ID1 Resister (Addr 0x0003) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0003, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0003, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_ID1r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0003,       \
                                     (_val), (_mask))

/* PCS Speed Ability Resister (Addr 0x0004) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0004, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0004, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0004,       \
                                     (_val), (_mask))

/* PCS Devices in Package1 Resister (Addr 0x0005) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0005, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0005, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0005,       \
                                     (_val), (_mask))

/* PCS Devices in Package2 Resister (Addr 0x0006) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0006, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0006, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0006,       \
                                     (_val), (_mask))

/* PCS Control2 Resister (Addr 0x0007) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_CTRL2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0007, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_CTRL2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0007, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_CTRL2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0007,       \
                                     (_val), (_mask))

/* PCS Status2 Resister (Addr 0x0008) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_STAT2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0008, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_STAT2r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0008, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_STAT2r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0008,       \
                                     (_val), (_mask))

/* PCS TenGBASE-X Status Register Resister (Addr 0x0018) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val)       \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0018, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val)        \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0018, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0018,       \
                                     (_val), (_mask))

/* PCS TenGBASE-R Status Register Resister (Addr 0x0020) */
#define BSDK_RD_PHY84728_XFI_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val)        \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0003, 0x0020, (_val))
#define BSDK_WR_PHY84728_XFI_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val)        \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0003, 0x0020, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0003, 0x0020,       \
                                     (_val), (_mask))



/*
 * Device 7 Registers
 */

/* AN User Defined registers */

/* 1000X Control1 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x8300, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x8300, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x8300,         \
                                     (_val), (_mask))

/* 1000X Control2 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x8301, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x8301, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x8301,         \
                                     (_val), (_mask))

/* 1000X Control3 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x8302, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x8302, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x8302,         \
                                     (_val), (_mask))

/* 1000X Control4 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x8303, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x8303, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x8303,         \
                                     (_val), (_mask))
/* MISC2 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_MISC2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x8309, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_MISC2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x8309, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_MISC2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x8309,         \
                                     (_val), (_mask))

/* 1000X Status1 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_BASE1000X_STAT1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x8304, (_val)) 

/* 1000X Status2 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_BASE1000X_STAT2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x8305, (_val)) 

/* MII Control Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_MII_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0xffe0, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_MII_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0xffe0, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_MII_CTRLr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe0,         \
                                     (_val), (_mask))

/* MII Status Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_MII_STATr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0xffe1, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_MII_STATr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0xffe1, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_MII_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe1,         \
                                     (_val), (_mask))

/* AutoNeg Adv Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_ANAr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0xffe4, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_ANAr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0xffe4, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_ANAr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe4,         \
                                     (_val), (_mask))
/* Auto Neg Link Partner Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_ANPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0xffe5, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_ANPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0xffe5, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_ANPr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe5,         \
                                     (_val), (_mask))

/* Auto Neg Expansion Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_EXRr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0xffe6, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_EXRr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0xffe6, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_EXRr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe6,         \
                                     (_val), (_mask))

/* Auto Neg Next Page Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_NEXT_PAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0xffe7, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_NEXT_PAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0xffe7, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_NEXT_PAGEr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe7,         \
                                     (_val), (_mask))

/* Auto Neg Link Partner Ability 2 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_ANP2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0xffe8, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_AN_ANP2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0xffe8, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_ANP2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe8,         \
                                     (_val), (_mask))

/* Extended Status 2 Register */
#define BSDK_RD_PHY84728_XFI_DEV7_ESPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0xffef, (_val)) 
#define BSDK_WR_PHY84728_XFI_DEV7_ESPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0xffef, (_val)) 
#define BSDK_MOD_PHY84728_XFI_DEV7_ESPr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0xffef,         \
                                     (_val), (_mask))


/* AN Standard registers */

/* AN Control Resister (Addr 0x0000) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0000, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_CTRLr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0000, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_CTRLr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0000,       \
                                     (_val), (_mask))

/* AN Status Resister (Addr 0x0001) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0001, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_STATr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0001, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_STATr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0001,       \
                                     (_val), (_mask))

/* AN ID0 Resister (Addr 0x0002) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0002, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_ID0r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0002, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_ID0r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0002,       \
                                     (_val), (_mask))
/* AN ID1 Resister (Addr 0x0003) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0003, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_ID1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0003, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_ID1r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0003,       \
                                     (_val), (_mask))

/* AN Speed Ability Resister (Addr 0x0004) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_SPEED_ABILITYr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0004, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_SPEED_ABILITYr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0004, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_SPEED_ABILITYr(_pctrl, _val, _mask)  \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0004,       \
                                     (_val), (_mask))

/* AN Devices in Package1 Resister (Addr 0x0005) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0005, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0005, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0005,       \
                                     (_val), (_mask))

/* AN Devices in Package2 Resister (Addr 0x0006) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val)           \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0006, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val)           \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0006, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val, _mask)   \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0006,       \
                                     (_val), (_mask))

/* AN Advertisement 1 Resister (Addr 0x0010) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_ADV1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0010, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_ADV1r(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0010, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_ADV1r(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0010,       \
                                     (_val), (_mask))

/* AN Advertisement 2 Resister (Addr 0x0011) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_ADV2r(_pctrl, _val)                    \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0011, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_ADV2r(_pctrl, _val)                    \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0011, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_ADV2r(_pctrl, _val, _mask)            \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0011,           \
                                     (_val), (_mask))

/* AN Advertisement 3 Resister (Addr 0x0012) */
#define BSDK_RD_PHY84728_XFI_DEV7_AN_ADV3r(_pctrl, _val)                    \
            BSDK_PHY84728_REG_RD((_pctrl), 0x01, 0x0007, 0x0012, (_val))
#define BSDK_WR_PHY84728_XFI_DEV7_AN_ADV3r(_pctrl, _val)                    \
            BSDK_PHY84728_REG_WR((_pctrl), 0x01, 0x0007, 0x0012, (_val))
#define BSDK_MOD_PHY84728_XFI_DEV7_AN_ADV3r(_pctrl, _val, _mask)            \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x01, 0x0007, 0x0012,           \
                                     (_val), (_mask))


/* System side DEV4 XGXS registers */


/* 1000X Control1 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x8300, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0x8300, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL1r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0x8300,         \
                                     (_val), (_mask))

/* 1000X Control2 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x8301, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0x8301, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0x8301,         \
                                     (_val), (_mask))

/* 1000X Control3 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL3r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x8302, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL3r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0x8302, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL3r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0x8302,         \
                                     (_val), (_mask))

/* 1000X Control4 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL4r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x8303, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL4r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0x8303, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_BASE1000X_CTRL4r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0x8303,         \
                                     (_val), (_mask))
/* 1000X Status1 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_BASE1000X_STAT1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x8304, (_val)) 

/* 1000X Status2 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_BASE1000X_STAT2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x8305, (_val)) 

/* MISC1 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_MISC1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x8308, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_MISC1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0x8308, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_MISC1r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0x8308,         \
                                     (_val), (_mask))

/* MISC2 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_MISC2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x8309, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_MISC2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0x8309, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_MISC2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0x8309,         \
                                     (_val), (_mask))

/* SGMII BASEPAGE Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_SGMII_BASEPAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x835c, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_SGMII_BASEPAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0x835c, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_SGMII_BASEPAGEr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0x835c,         \
                                     (_val), (_mask))

/* XGXS  Status1 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXSSTATUS1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x0001, (_val)) 

/* MII Control Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_MII_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0xffe0, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_MII_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0xffe0, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_MII_CTRLr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0xffe0,         \
                                     (_val), (_mask))

/* MII Status Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_MII_STATr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0xffe1, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_MII_STATr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0xffe1, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_MII_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0xffe1,         \
                                     (_val), (_mask))

/* AutoNeg Adv Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_ANAr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0xffe4, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_ANAr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0xffe4, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_ANAr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0xffe4,         \
                                     (_val), (_mask))
/* Auto Neg Link Partner Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_ANPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0xffe5, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_ANPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0xffe5, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_ANPr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0xffe5,         \
                                     (_val), (_mask))

/* Auto Neg Expansion Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_EXRr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0xffe6, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_EXRr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0xffe6, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_EXRr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0xffe6,         \
                                     (_val), (_mask))

/* Auto Neg Next Page Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_NEXT_PAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0xffe7, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_NEXT_PAGEr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0xffe7, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_NEXT_PAGEr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0xffe7,         \
                                     (_val), (_mask))

/* Auto Neg Link Partner Ability 2 Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_ANP2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0xffe8, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_ANP2r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0xffe8, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_ANP2r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0xffe8,         \
                                     (_val), (_mask))

/* Extended Status 2 Register */
#define BSDK_RD_PHY84728_XGXS_DEV4_ESPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0xffef, (_val)) 
#define BSDK_WR_PHY84728_XGXS_DEV4_ESPr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0xffef, (_val)) 
#define BSDK_MOD_PHY84728_XGXS_DEV4_ESPr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0xffef,         \
                                     (_val), (_mask))


/*
 * Firmware Download related registers
 */

/* M8051 Message IN Resister (Addr 0xCA12) */
#define BSDK_RD_PHY84728_LN_DEV1_M8501_MSGINr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xCA12, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_M8501_MSGINr(_pctrl, _val)                 \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0xCA12, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_M8501_MSGINr(_pctrl, _val, _mask)         \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0xCA12,           \
                                     (_val), (_mask))

/* M8051 Message OUT Resister (Addr 0xCA13) */
#define BSDK_RD_PHY84728_LN_DEV1_M8501_MSGOUTr(_pctrl, _val)                \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xCA13, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_M8501_MSGOUTr(_pctrl, _val)                \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0xCA13, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_M8501_MSGOUTr(_pctrl, _val, _mask)        \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0xCA13,           \
                                     (_val), (_mask))

/* RX Alarm Resister (Addr 0x9003) */
#define BSDK_RD_PHY84728_LN_DEV1_RX_ALARMr(_pctrl, _val)                    \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x9003, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_RX_ALARMr(_pctrl, _val)                    \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x9003, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_RX_ALARMr(_pctrl, _val, _mask)            \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x9003,           \
                                     (_val), (_mask))

/* SPA Control and Status Resister (Addr 0xC848) */
#define BSDK_RD_PHY84728_LN_DEV1_SPI_CTRL_STATr(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xC848, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_SPI_CTRL_STATr(_pctrl, _val)               \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0xC848, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_SPI_CTRL_STATr(_pctrl, _val, _mask)       \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0xC848,           \
                                     (_val), (_mask))

/* SPA Control and Status Resister (Addr 0xCA1C) */
#define BSDK_RD_PHY84728_LN_DEV1_CHECKSUMr(_pctrl, _val)                    \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xCA1C, (_val))


/* SPI ROM Firmware update related */

/*
 * SPI-ROM Program related defs
 */
#define BSDK_SPI_CTRL_1_L        0xC000
#define BSDK_SPI_CTRL_1_H        0xC002
#define BSDK_SPI_CTRL_2_L        0xC400
#define BSDK_SPI_CTRL_2_H        0xC402
#define BSDK_SPI_TXFIFO          0xD000
#define BSDK_SPI_RXFIFO          0xD400
#define BSDK_WR_CPU_CTRL_REGS    0x11
#define BSDK_RD_CPU_CTRL_REGS    0xEE
#define BSDK_WR_CPU_CTRL_FIFO    0x66

/*
 * SPI Controller Commands(Messages).
 */
#define BSDK_MSGTYPE_HWR          0x40
#define BSDK_MSGTYPE_HRD          0x80
#define BSDK_WRSR_OPCODE          0x01
#define BSDK_WR_OPCODE            0x02
#define BSDK_WRDI_OPCODE          0x04
#define BSDK_RDSR_OPCODE          0x05
#define BSDK_WREN_OPCODE          0x06
#define BSDK_WR_BLOCK_SIZE        0x40
#define BSDK_TOTAL_WR_BYTE        0x4000
                                                                                
#define BSDK_WR_TIMEOUT      100
#define BSDK_WR_ITERATIONS   10000


/* Misc Control 2 Resister(SPI ROM FW Upload control) (Addr 0xCA85) */
#define BSDK_RD_PHY84728_LN_DEV1_MISC_CNTL2r(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0xCA85, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_MISC_CNTL2r(_pctrl, _val)               \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0xCA85, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_MISC_CNTL2r(_pctrl, _val, _mask)       \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0xCA85,           \
                                     (_val), (_mask))

/* RX Alarm Control Resister (Addr 0x9000) */
#define BSDK_RD_PHY84728_LN_DEV1_RX_ALARM_CNTLr(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x9000, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_RX_ALARM_CNTLr(_pctrl, _val)               \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x9000, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_RX_ALARM_CNTLr(_pctrl, _val, _mask)       \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x9000,           \
                                     (_val), (_mask))

/* TX Alarm Control Resister (Addr 0x9001) */
#define BSDK_RD_PHY84728_LN_DEV1_TX_ALARM_CNTLr(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x9001, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_TX_ALARM_CNTLr(_pctrl, _val)               \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x9001, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_TX_ALARM_CNTLr(_pctrl, _val, _mask)       \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x9001,           \
                                     (_val), (_mask))

/* LASI Control Resister (Addr 0x9002) */
#define BSDK_RD_PHY84728_LN_DEV1_LASI_CNTLr(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x9002, (_val))
#define BSDK_WR_PHY84728_LN_DEV1_LASI_CNTLr(_pctrl, _val)               \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0001, 0x9002, (_val))
#define BSDK_MOD_PHY84728_LN_DEV1_LASI_CNTLr(_pctrl, _val, _mask)       \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0001, 0x9002,           \
                                     (_val), (_mask))

/* RX Alarm Status Register (Addr 0x9003) */
#define BSDK_RD_PHY84728_LN_DEV1_RX_ALARM_STATr(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x9003, (_val))

/* TX Alarm Status Register (Addr 0x9003) */
#define BSDK_RD_PHY84728_LN_DEV1_TX_ALARM_STATr(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x9004, (_val))

/* LASI Status Register (Addr 0x9003) */
#define BSDK_RD_PHY84728_LN_DEV1_LASI_STATr(_pctrl, _val)               \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0001, 0x9005, (_val))

/* XGXS clause 45 registers */

/* XGXS control Register */
#define BSDK_RD_PHY84728_SYS_DEV4_XGXS_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x4, 0x8000, (_val)) 
#define BSDK_WR_PHY84728_SYS_DEV4_XGXS_CTRLr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x4, 0x8000, (_val)) 
#define BSDK_MOD_PHY84728_SYS_DEV4_XGXS_CTRLr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x4, 0x8000,         \
                                     (_val), (_mask))

/* XgxsBlk0 :: miscControl1 :: Global_PMD_tx_disable [11:11] */
#define XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK           0x0800
#define XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_ALIGN          0
#define XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_BITS           1
#define XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_SHIFT          11
/* XGXS control Register */
#define BSDK_RD_PHY84728_XGXSBLK0_MISCCONTROL1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x4, 0x800e, (_val)) 
#define BSDK_WR_PHY84728_XGXSBLK0_MISCCONTROL1r(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x4, 0x800e, (_val)) 
#define BSDK_MOD_PHY84728_XGXSBLK0_MISCCONTROL1r(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x4, 0x800e,         \
                                     (_val), (_mask))


/* XGXS Lane status Register */
#define BSDK_RD_PHY84728_XGXS_DEV4_XS_LANE_STATr(_pctrl, _val)         \
            BSDK_PHY84728_REG_RD((_pctrl), 0x00, 0x0004, 0x18, (_val)) 
#define BSDK_WR_PHY84728_XGXS_DEV4_XS_LANE_STATr(_pctrl, _val)         \
            BSDK_PHY84728_REG_WR((_pctrl), 0x00, 0x0004, 0x18, (_val)) 
#define BSDK_MOD_PHY84728_XGXS_DEV4_XS_LANE_STATr(_pctrl, _val, _mask) \
            BSDK_PHY84728_REG_MOD((_pctrl), 0x00, 0x0004, 0x18,         \
                                     (_val), (_mask))

/* I2C related defines */
#define BSDK_PHY84728_BSC_XFER_MAX     0x1F9
#define BSDK_PHY84728_BSC_WR_MAX       16
#define BSDK_PHY84728_WRITE_START_ADDR 0x8007
#define BSDK_PHY84728_READ_START_ADDR  0x8007
#define BSDK_PHY84728_WR_FREQ_400KHZ   0x0100
#define BSDK_PHY84728_2W_STAT          0x000C
#define BSDK_PHY84728_2W_STAT_IDLE     0x0000
#define BSDK_PHY84728_2W_STAT_COMPLETE 0x0004
#define BSDK_PHY84728_2W_STAT_IN_PRG   0x0008
#define BSDK_PHY84728_2W_STAT_FAIL     0x000C
#define BSDK_PHY84728_BSC_WRITE_OP     0x22
#define BSDK_PHY84728_BSC_READ_OP      0x2
#define BSDK_PHY84728_I2CDEV_WRITE     0x1
#define BSDK_PHY84728_I2CDEV_READ      0x0
#define BSDK_PHY84728_I2C_8BIT         0
#define BSDK_PHY84728_I2C_16BIT        1
#define BSDK_PHY84728_I2C_TEMP_RAM     0xE
#define BSDK_PHY84728_I2C_OP_TYPE(access_type,data_type) \
        ((access_type) | ((data_type) << 8))
#define BSDK_PHY84728_I2C_ACCESS_TYPE(op_type) ((op_type) & 0xff)
#define BSDK_PHY84728_I2C_DATA_TYPE(op_type)   (((op_type) >> 8) & 0xff)

#endif /* _PHY84728_INT_H */
