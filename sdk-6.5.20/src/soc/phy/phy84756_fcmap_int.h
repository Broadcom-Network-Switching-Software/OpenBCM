
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PHY84756_INT_H
#define _PHY84756_INT_H

/****************************************************************************
 * Firmware related:
 */
#define BFCMAP_PHY84756_FW_CHECKSUM         0x600D
#define BFCMAP_PHY84756_FW_NUM_ITER         5 

#define BFCMAP_PHY84756_FW_DWNLD_DONE_MSG   0x4321
#define BFCMAP_PHY84756_FW_TOTAL_WR_BYTE    0x4000

#define BFCMAP_PHY84756_PMAD_M8051_MSGIN_REG     0xca12
#define BFCMAP_PHY84756_PMAD_M8051_MSGOUT_REG    0xca13

#define BFCMAP_PHY84756_IS_10G_CHANNEL(dev_port)              \
                    ((dev_port == 0) || (dev_port ==1))

#define BFCMAP_PHY84756_IS_GIG_ONLY_CHANNEL(dev_port)         \
                    ((dev_port == 2) || (dev_port ==3))


/* Channel 2 and 3 Device 1 SGMII : */

/*
 * MII Link Advertisment (Clause 37) 
 */
#define BFCMAP_PHY84756_DEV1_1000X_ANA_C37_FD          (1 << 5)
#define BFCMAP_PHY84756_DEV1_1000X_ANA_C37_HD          (1 << 6)
#define BFCMAP_PHY84756_DEV1_1000X_ANA_C37_PAUSE       (1 << 7)
#define BFCMAP_PHY84756_DEV1_1000X_ANA_C37_ASYM_PAUSE  (1 << 8)



/* MII Control register definations */
#define BFCMAP_PHY84756_SGMII_MII_CTRL_SS_MSB  (1 << 6) /* Speed select, MSb */
#define BFCMAP_PHY84756_SGMII_MII_CTRL_FD      (1 << 8) /* Full Duplex */
#define BFCMAP_PHY84756_SGMII_MII_CTRL_RAN     (1 << 9) /* Restart Autoneg*/
#define BFCMAP_PHY84756_SGMII_MII_CTRL_PD      (1 << 11) /* Power Down */
#define BFCMAP_PHY84756_SGMII_MII_CTRL_AE      (1 << 12) /* Autoneg enable */
#define BFCMAP_PHY84756_SGMII_MII_CTRL_SS_LSB  (1 << 13) /* Speed select, LSb */
#define BFCMAP_PHY84756_SGMII_MII_CTRL_LE      (1 << 14) /* Loopback enable */
#define BFCMAP_PHY84756_SGMII_MII_CTRL_RESET   (1 << 15) /* PHY reset */

#define BFCMAP_PHY84756_SGMII_MII_CTRL_SS(_x)    ((_x) &  \
                                     (BFCMAP_PHY84756_SGMII_MII_CTRL_SS_LSB | \
                                      BFCMAP_PHY84756_SGMII_MII_CTRL_SS_MSB))
#define BFCMAP_PHY84756_SGMII_MII_CTRL_SS_10          0
#define BFCMAP_PHY84756_SGMII_MII_CTRL_SS_100         \
                                    (BFCMAP_PHY84756_SGMII_MII_CTRL_SS_LSB)
#define BFCMAP_PHY84756_SGMII_MII_CTRL_SS_1000        \
                                    (BFCMAP_PHY84756_SGMII_MII_CTRL_SS_MSB)
#define BFCMAP_PHY84756_SGMII_MII_CTRL_SS_INVALID     \
                                    (BFCMAP_PHY84756_SGMII_MII_CTRL_SS_LSB | \
                                     BFCMAP_PHY84756_SGMII_MII_CTRL_SS_MSB)
#define BFCMAP_PHY84756_SGMII_MII_CTRL_SS_MASK        \
                                    (BFCMAP_PHY84756_SGMII_MII_CTRL_SS_LSB | \
                                     BFCMAP_PHY84756_SGMII_MII_CTRL_SS_MSB)

/* MII Status register definations */
#define BFCMAP_PHY84756_SGMII_MII_STAT_LINK_STATUS     (1 << 2)
#define BFCMAP_PHY84756_SGMII_MII_STAT_AN_DONE         (1 << 5)


/* 1000x Control1 Register definations */
#define BFCMAP_PHY84756_SGMII_BASE1000X_CTRL1r_FIBER_MODE     (1 << 0)
#define BFCMAP_PHY84756_SGMII_BASE1000X_CTRL1r_TBI_INT        (1 << 1)
#define BFCMAP_PHY84756_SGMII_BASE1000X_CTRL1r_SD_ENABLE      (1 << 2)
#define BFCMAP_PHY84756_SGMII_BASE1000X_CTRL1r_INVERT_SD      (1 << 3)
#define BFCMAP_PHY84756_SGMII_BASE1000X_CTRL1r_AUTODETECT_EN  (1 << 4)
#define BFCMAP_PHY84756_SGMII_BASE1000X_CTRL1r_SGMII_MASTER   (1 << 5)
#define BFCMAP_PHY84756_SGMII_BASE1000X_CTRL1r_DIS_PLL_PWRDN  (1 << 6)


/* 1000x Status1 Register definations */
#define BFCMAP_PHY84756_SGMII_BASE1000X_STAT1r_FIBER_MODE     (1 << 0)
#define BFCMAP_PHY84756_SGMII_BASE1000X_STAT1r_LINK_STATUS    (1 << 1)
#define BFCMAP_PHY84756_SGMII_BASE1000X_STAT1r_DUPLEX_STATUS  (1 << 2)
#define BFCMAP_PHY84756_SGMII_BASE1000X_STAT1r_SPEED_STATUS   ((1 << 3) | \
                                                               (1 << 4))
#define BFCMAP_PHY84756_SGMII_BASE1000X_STAT1r_PAUSE_TX       (1 << 5)
#define BFCMAP_PHY84756_SGMII_BASE1000X_STAT1r_PAUSE_RX       (1 << 6)


/* Speed Link status register defination */
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_LN_PDM_SPEED_10M    (1 << 0)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_LN_PDM_SPEED_100M   (1 << 1)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_LN_PDM_SPEED_1G     (1 << 2)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_LN_AN_COMPLETE      (1 << 6)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_LN_RX_SD            (1 << 7)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_SYS_PDM_SPEED_10M   (1 << 8)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_SYS_PDM_SPEED_100M  (1 << 9)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_SYS_PDM_SPEED_1G    (1 << 10)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_SYS_AN_COMPLETE     (1 << 14)
#define BFCMAP_PHY84756_SGMII_SPEED_LD_STATr_SYS_RX_SD           (1 << 15)

/* SGMII MACSEC Control register defination */
#define BFCMAP_PHY84756_SGMII_MACSEC_CTRL_BYPASS_MODE            (1 << 0)
#define BFCMAP_PHY84756_SGMII_MACSEC_CTRL_ENABLE_PWRDN_MACSEC    (1 << 8)
#define BFCMAP_PHY84756_SGMII_MACSEC_CTRL_UDSW_PWRDW_MACSEC      (1 << 11)
#define BFCMAP_PHY84756_SGMII_MACSEC_CTRL_UDSW_RESET_MACSEC      (1 << 15)



/************************************************************************
 *
 *                         Channel 0 and 1 : LINE : Device 1 (PMA/PMD)
 */

/* Speed Link status register defination */
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_10M    (1 << 0)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_100M   (1 << 1)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_1G     (1 << 2)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_2P5G   (1 << 3)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_10G    (1 << 4)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_AN_COMPLETE      (1 << 6)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_RX_SD            (1 << 7)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_10M   (1 << 8)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_100M  (1 << 9)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_1G    (1 << 10)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_2P5G  (1 << 11)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_10G   (1 << 12)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_AN_COMPLETE     (1 << 14)
#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_RX_SD           (1 << 15)

#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_MASK                \
                    (BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_10M |  \
                     BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_100M | \
                     BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_1G |   \
                     BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_2P5G | \
                     BFCMAP_PHY84756_PMD_SPEED_LD_STATr_LN_PDM_SPEED_10G)

#define BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_MASK                \
                    (BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_10M |  \
                     BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_100M | \
                     BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_1G |   \
                     BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_2P5G | \
                     BFCMAP_PHY84756_PMD_SPEED_LD_STATr_SYS_PDM_SPEED_10G)


/* DEV1 MACSEC Control register defination */
#define BFCMAP_PHY84756_LN_DEV1_MACSEC_CTRL_BYPASS_MODE            (1 << 0)
#define BFCMAP_PHY84756_LN_DEV1_MACSEC_CTRL_ENABLE_PWRDN_MACSEC    (1 << 8)
#define BFCMAP_PHY84756_LN_DEV1_MACSEC_CTRL_UDSW_PWRDW_MACSEC      (1 << 11)
#define BFCMAP_PHY84756_LN_DEV1_MACSEC_CTRL_UDSW_RESET_MACSEC      (1 << 15)



/* PMD Control Register definations */
#define BFCMAP_PHY84756_PMD_CTRL_PMS_LOOPBACK  (1 << 0) /* Loopback */
#define BFCMAP_PHY84756_PMD_CTRL_SS_MSB        (1 << 6) /* Speed select, MSb */
#define BFCMAP_PHY84756_PMD_CTRL_PD            (1 << 11) /* Power Down */
#define BFCMAP_PHY84756_PMD_CTRL_SS_LSB        (1 << 13) /* Speed select, LSb */
#define BFCMAP_PHY84756_PMD_CTRL_RESET         (1 << 15) /* PHY reset */

#define BFCMAP_PHY84756_PMD_CTRL_SS_SPEED_SEL   \
                                        (BFCMAP_PHY84756_PMD_CTRL_SS_LSB | \
                                         BFCMAP_PHY84756_PMD_CTRL_SS_MSB)
#define BFCMAP_PHY84756_PMD_CTRL_SS_MASK        \
                                        (BFCMAP_PHY84756_PMD_CTRL_SS_LSB | \
                                         BFCMAP_PHY84756_PMD_CTRL_SS_MSB)

#define BFCMAP_PHY84756_PMD_CTRL_10GSS_MASK     \
                                        (BFCMAP_PHY84756_PMD_CTRL_SS_LSB  | \
                                         BFCMAP_PHY84756_PMD_CTRL_SS_MSB  | \
                                         (1 << 2) | \
                                         (1 << 3) | \
                                         (1 << 4) | \
                                         (1 << 5))

#define BFCMAP_PHY84756_PMD_CTRL_SS_10       0
#define BFCMAP_PHY84756_PMD_CTRL_SS_100      \
                                             (BFCMAP_PHY84756_PMD_CTRL_SS_LSB)
#define BFCMAP_PHY84756_PMD_CTRL_SS_1000     (BFCMAP_PHY84756_PMD_CTRL_SS_MSB)
#define BFCMAP_PHY84756_PMD_CTRL_SS_10000    \
                                     (BFCMAP_PHY84756_PMD_CTRL_SS_SPEED_SEL | \
                                     (0x0000))


#define BFCMAP_PHY84756_PMD_CTRL_SS(_x)      \
                                       ((_x) &  \
                                        (BFCMAP_PHY84756_PMD_CTRL_SS_LSB | \
                                         BFCMAP_PHY84756_PMD_CTRL_SS_MSB))



/* PMD Status Register defination */
#define BFCMAP_PHY84756_PMD_STAT_RX_LINK_STATUS    (1 << 2)



/* PMD Control2 Resister definations */
#define BFCMAP_PHY84756_PMD_CTRL2_PMA_1GTPMD_TYPE     (0xc)
#define BFCMAP_PHY84756_PMD_CTRL2_PMA_10GLRMPMD_TYPE  (0x8)

#define BFCMAP_PHY84756_PMD_CTRL2_PMA_SELECT_MASK     (0xf)

/************************************************************************
 *
 *                         LINE : Device 3 (PCS)
 */
 /*PCS Control register definations */
 #define BFCMAP_PHY84756_PCS_PCS_CTRL_PD                     (1 << 11)


 /*PCS Status register definations */
 #define BFCMAP_PHY84756_PCS_PCS_STAT_RX_LINK_STATUS         (1 << 2)



/************************************************************************
 *
 *                         LINE : Device 7 (AN)
 */

/*
 * MII Link Advertisment (Clause 37) 
 */
#define BFCMAP_PHY84756_DEV7_1000X_ANA_C37_FD          (1 << 5)
#define BFCMAP_PHY84756_DEV7_1000X_ANA_C37_HD          (1 << 6)
#define BFCMAP_PHY84756_DEV7_1000X_ANA_C37_PAUSE       (1 << 7)
#define BFCMAP_PHY84756_DEV7_1000X_ANA_C37_ASYM_PAUSE  (1 << 8)


/*
 * MII Link Partner Advertisment (Clause 37) 
 */
#define BFCMAP_PHY84756_SGMII_ANP_SGMII_MODE    (1 << 0)
#define BFCMAP_PHY84756_SGMII_ANP_FD            (1 << 5)
#define BFCMAP_PHY84756_SGMII_ANP_HD            (1 << 6)
#define BFCMAP_PHY84756_SGMII_ANP_C37_PAUSE     (1 << 7)
#define BFCMAP_PHY84756_SGMII_ANPC37_ASYM_PAUSE (1 << 8)




/*AN MII control register definations */
#define BFCMAP_PHY84756_AN_MII_CTRL_SS_MSB    (1 << 6) /* Speed select, MSb */
#define BFCMAP_PHY84756_AN_MII_CTRL_FD        (1 << 8) /* Full Duplex */
#define BFCMAP_PHY84756_AN_MII_CTRL_RAN       (1 << 9) /* Restart Autoneg `*/
#define BFCMAP_PHY84756_AN_MII_CTRL_PD        (1 << 11) /* Power Down */
#define BFCMAP_PHY84756_AN_MII_CTRL_AE        (1 << 12) /* Autoneg enable */
#define BFCMAP_PHY84756_AN_MII_CTRL_SS_LSB    (1 << 13) /* Speed select, LSb */
#define BFCMAP_PHY84756_AN_MII_CTRL_LE        (1 << 14) /* Loopback enable */
#define BFCMAP_PHY84756_AN_MII_CTRL_RESET     (1 << 15) /* PHY reset */

#define BFCMAP_PHY84756_AN_MII_CTRL_SS(_x)    \
                                        ((_x) &  \
                                         (BFCMAP_PHY84756_AN_MII_CTRL_SS_LSB | \
                                          BFCMAP_PHY84756_AN_MII_CTRL_SS_MSB))
#define BFCMAP_PHY84756_AN_MII_CTRL_SS_10   0
#define BFCMAP_PHY84756_AN_MII_CTRL_SS_100  (BFCMAP_PHY84756_AN_MII_CTRL_SS_LSB)
#define BFCMAP_PHY84756_AN_MII_CTRL_SS_1000 (BFCMAP_PHY84756_AN_MII_CTRL_SS_MSB)
#define BFCMAP_PHY84756_AN_MII_CTRL_SS_INVALID \
                                        (BFCMAP_PHY84756_AN_MII_CTRL_SS_LSB | \
                                         BFCMAP_PHY84756_AN_MII_CTRL_SS_MSB)
#define BFCMAP_PHY84756_AN_MII_CTRL_SS_MASK   \
                                        (BFCMAP_PHY84756_AN_MII_CTRL_SS_LSB | \
                                         BFCMAP_PHY84756_AN_MII_CTRL_SS_MSB)

/* AN MII Status register definations */
#define BFCMAP_PHY84756_AN_MII_STAT_LINK_STATUS    (1 << 2)
#define BFCMAP_PHY84756_AN_MII_STAT_AN_DONE        (1 << 5)


/* AN 1000x Control1 Register definations */
#define BFCMAP_PHY84756_AN_BASE1000X_CTRL1r_FIBER_MODE         (1 << 0)
#define BFCMAP_PHY84756_AN_BASE1000X_CTRL1r_TBI_INT            (1 << 1)
#define BFCMAP_PHY84756_AN_BASE1000X_CTRL1r_SD_ENABLE          (1 << 2)
#define BFCMAP_PHY84756_AN_BASE1000X_CTRL1r_INVERT_SD          (1 << 3)
#define BFCMAP_PHY84756_AN_BASE1000X_CTRL1r_AUTODETECT_EN      (1 << 4)
#define BFCMAP_PHY84756_AN_BASE1000X_CTRL1r_SGMII_MASTER       (1 << 5)
#define BFCMAP_PHY84756_AN_BASE1000X_CTRL1r_DIS_PLL_PWRDN      (1 << 6)


/* 1000x Status1 Register definations */
#define BFCMAP_PHY84756_AN_BASE1000X_STAT1r_SGMII_MODE      (1 << 0)
#define BFCMAP_PHY84756_AN_BASE1000X_STAT1r_LINK_STATUS     (1 << 1)
#define BFCMAP_PHY84756_AN_BASE1000X_STAT1r_DUPLEX_STATUS   (1 << 2)
#define BFCMAP_PHY84756_AN_BASE1000X_STAT1r_SPEED_STATUS    ((1 << 3) | \
                                                             (1 << 4))
#define BFCMAP_PHY84756_AN_BASE1000X_STAT1r_PAUSE_TX        (1 << 5)
#define BFCMAP_PHY84756_AN_BASE1000X_STAT1r_PAUSE_RX        (1 << 6)




/* AN Control Register */
#define BFCMAP_PHY84756_AN_AN_CTRL_RAN         (1 << 9) /* Restart AutoNeg */
#define BFCMAP_PHY84756_AN_AN_CTRL_AE          (1 << 12) /* Enable AutoNegn */
#define BFCMAP_PHY84756_AN_AN_CTRL_RESET       (1 << 15) /* Reset AutoNeg */

/* AN Status Register */
#define BFCMAP_PHY84756_AN_AN_STAT_AN_DONE     (1 << 5) 





extern int
bfcmap_phy84756_reg_read(phy_ctrl_t * pc, buint32_t flags, buint8_t reg_bank,
                         buint16_t reg_addr, buint16_t *data);
extern int
bfcmap_phy84756_reg_write(phy_ctrl_t * pc, buint32_t flags, buint8_t reg_bank,
                         buint16_t reg_addr, buint16_t data);
extern int
bfcmap_phy84756_reg_modify(phy_ctrl_t * pc, buint32_t flags, buint8_t reg_bank,
                         buint16_t reg_addr, buint16_t data, buint16_t mask);

/* Flag indicating that Line side Access or System side access */
#define BFCMAP_PHY84756_LINE_SIDE       0
#define BFCMAP_PHY84756_SYS_SIDE        1


#if 0
#define BFCMAP_READ_PHY54XX_MII_PHY_ID0r(_pctrl, _val) \
            BFCMAP_PHY54XX_REG_READ((_pctrl), 0x00, 0x0001, 0x02, (_val))
#define BFCMAP_WRITE_PHY54XX_MII_PHY_ID0r(_pctrl, _val) \
            BFCMAP_PHY54XX_REG_WRITE((_pctrl), 0x00, 0x0001, 0x02, (_val))
#define BFCMAP_MODIFY_PHY54XX_MII_PHY_ID0r(_pctrl, _val, _mask) \
            BFCMAP_PHY54XX_REG_MODIFY((_pctrl), 0x00, 0x0001, 0x02, \
                             (_val), (_mask))
#endif

#undef  READ_PHY_REG
#define READ_PHY_REG(_unit, _pc,  _addr, _value) \
            ((_pc->read)((_pc->unit), (_pc->phy_id), (_addr), (_value)))

#undef  WRITE_PHY_REG

#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
#define WRITE_PHY_REG(_unit, _pc, _addr, _value) \
     ((SOC_WARM_BOOT((_pc)->unit) || SOC_IS_RELOADING((_pc)->unit)) ?     \
            SOC_E_NONE :                                                  \
            ((_pc->write)((_pc->unit), (_pc->phy_id), (_addr), (_value))))
#else  /* BCM_WARM_BOOT_SUPPORT */
#define WRITE_PHY_REG(_unit, _pc, _addr, _value) \
            ((_pc->write)((_pc->unit), (_pc->phy_id), (_addr), (_value)))
#endif /* BCM_WARM_BOOT_SUPPORT */

#define BFCMAP_PHY84756_IO_MDIO_READ(_phy_ctrl,  _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))

#define BFCMAP_PHY84756_IO_MDIO_WRITE(_phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))

/* General - PHY register access */
#define BFCMAP_PHY84756_REG_RD(_pctrl, _flags, _reg_bank, _reg_addr, _val) \
        bfcmap_phy84756_reg_read((_pctrl), (_flags), (_reg_bank),      \
                                 (_reg_addr), (_val))

#define BFCMAP_PHY84756_REG_WR(_pctrl, _flags, _reg_bank, _reg_addr, _val) \
        bfcmap_phy84756_reg_write((_pctrl), (_flags), (_reg_bank),     \
                                       (_reg_addr), (_val))

#define BFCMAP_PHY84756_REG_MOD(_pctrl, _flags, _reg_bank, _reg_addr,      \
                                                            _val, _mask)    \
        bfcmap_phy84756_reg_modify((_pctrl), (_flags), (_reg_bank),    \
                                        (_reg_addr), (_val), (_mask))



/*
 * SGMII DEVICE 1 Line Side Registers
 */

/*
 * PMD Standard Registers
 */
/* PMD Control Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_PMD_CTRLr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0000, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_PMD_CTRLr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0000, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_PMD_CTRLr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0000,          \
                                     (_val), (_mask))

/* PMD Status Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_PMD_STATr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0001, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_PMD_STATr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0001, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_PMD_STATr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0001,          \
                                     (_val), (_mask))

/* PMD ID0 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_PMD_ID0r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0002, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_PMD_ID0r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0002, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_PMD_ID0r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0002,          \
                                     (_val), (_mask))

/* PMD ID1 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_PMD_ID1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0003, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_PMD_ID1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0003, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_PMD_ID1r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0003,          \
                                     (_val), (_mask))


/* 
 * PMD User registers
 */
/* 1000X Control1 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x8300, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x8300, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_BASE1000X_CTRL1r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x8300,          \
                                     (_val), (_mask))

/* 1000X Control2 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_BASE1000X_CTRL2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x8301, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_BASE1000X_CTRL2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x8301, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_BASE1000X_CTRL2r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x8301,          \
                                     (_val), (_mask))

/* 1000X Control3 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_BASE1000X_CTRL3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x8302, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_BASE1000X_CTRL3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x8302, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_BASE1000X_CTRL3r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x8302,          \
                                     (_val), (_mask))

/* 1000X Control4 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x8303, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x8303, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x8303,          \
                                     (_val), (_mask))

/* 1000X Status1 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_BASE1000X_STAT1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x8304, (_val)) 

/* 1000X Status2 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_BASE1000X_STAT2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x8305, (_val)) 

/* 1000X Status3 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_BASE1000X_STAT3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x8306, (_val)) 

/* 1000X Control4 Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x8303, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x8303, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x8303,          \
                                     (_val), (_mask))

/* Speed Link Detect Status Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_SPEED_LINK_DETECT_STATr(_pctrl, _val)  \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc81f, (_val)) 

/* SGMII MACSEC Control Bypass Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MACSEC_BYPASS_CTRLr(_pctrl, _val)      \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc8f0, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MACSEC_BYPASS_CTRLr(_pctrl, _val)      \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xc8f0, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MACSEC_BYPASS_CTRLr(_pctrl, _val, _mask) \
        BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xc8f0,(_val), (_mask))

/* System Side Select Register */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_SYS_PMB_SELr(_pctrl, _val)             \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc8ff, (_val)) 
#define BFCMAP_WR_PHY84756_SGMII_DEV1_SYS_PMB_SELr(_pctrl, _val)             \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xc8ff, (_val)) 
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_SYS_PMB_SELr(_pctrl, _val, _mask)     \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xc8ff,          \
                                     (_val), (_mask))


/* MII Control Resister (Addr 0xffe0) */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MII_CTRLr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xffe0, (_val))
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MII_CTRLr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xffe0, (_val))
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MII_CTRLr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xffe0,          \
                               (_val), (_mask))

/* MII Status Resister (Addr 0xffe1) */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MII_STATr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xffe1, (_val))
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MII_STATr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xffe1, (_val))
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MII_STATr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xffe1,          \
                                     (_val), (_mask))

/* MII ID0 Resister (Addr 0xffe2) */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MII_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xffe2, (_val))
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MII_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xffe2, (_val))
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MII_ID0r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xffe2,          \
                                     (_val), (_mask))
/* MII ID1 Resister (Addr 0x0003) */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MII_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xffe3, (_val))
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MII_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xffe3, (_val))
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MII_ID1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xffe3,          \
                                     (_val), (_mask))

/* Auto Neg Adv Resister (Addr 0xffe4) */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MII_ANAr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xffe4, (_val))
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MII_ANAr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xffe4, (_val))
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MII_ANAr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xffe4,         \
                                     (_val), (_mask))

/* Auto Neg Link Partner Ability Register (Addr 0xffe5) */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MII_ANPr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xffe5, (_val))
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MII_ANPr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xffe5, (_val))
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MII_ANPr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xffe5,         \
                                     (_val), (_mask))

/* Auto Neg Expansion Resister (Addr 0xffe6) */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MII_AN_EXPr(_pctrl, _val)             \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xffe6, (_val))
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MII_AN_EXPr(_pctrl, _val)             \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xffe6, (_val))
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MII_AN_EXPr(_pctrl, _val, _mask)     \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xffe6,        \
                                     (_val), (_mask))

/* Extended Status Resister (Addr 0xffef) */
#define BFCMAP_RD_PHY84756_SGMII_DEV1_MII_ESRr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xffef, (_val))
#define BFCMAP_WR_PHY84756_SGMII_DEV1_MII_ESRr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xffef, (_val))
#define BFCMAP_MOD_PHY84756_SGMII_DEV1_MII_ESRr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xffef,        \
                                     (_val), (_mask))


/*
 * SGMII DEVICE 1 System Side Registers
 */

/* 1000X Control1 Register */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x8300, (_val)) 
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x8300, (_val)) 
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL1r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x8300,          \
                                     (_val), (_mask))

/* 1000X Control2 Register */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x8301, (_val)) 
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x8301, (_val)) 
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL2r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x8301,          \
                                     (_val), (_mask))

/* 1000X Control3 Register */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x8302, (_val)) 
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x8302, (_val)) 
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL3r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x8302,          \
                                     (_val), (_mask))

/* 1000X Control4 Register */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x8303, (_val)) 
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x8303, (_val)) 
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_BASE1000X_CTRL4r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x8303,          \
                                     (_val), (_mask))

/* 1000X Status1 Register */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_BASE1000X_STAT1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x8304, (_val)) 

/* 1000X Status2 Register */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_BASE1000X_STAT2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x8305, (_val)) 

/* 1000X Status3 Register */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_BASE1000X_STAT3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x8306, (_val)) 

/* Speed Link Detect Status Register */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_SPEED_LINK_DETECT_STATr(_pctrl, _val)  \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0xc81f, (_val)) 

/* MII Control Resister (Addr 0xffe0) */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_MII_CTRLr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0xffe0, (_val))
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_MII_CTRLr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0xffe0, (_val))
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_MII_CTRLr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0xffe0,          \
                               (_val), (_mask))

/* MII Status Resister (Addr 0xffe1) */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_MII_STATr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0xffe1, (_val))
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_MII_STATr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0xffe1, (_val))
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_MII_STATr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0xffe1,          \
                                     (_val), (_mask))

/* Auto Neg Adv Resister (Addr 0xffe4) */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_MII_ANAr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0xffe4, (_val))
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_MII_ANAr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0xffe4, (_val))
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_MII_ANAr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0xffe4,         \
                                     (_val), (_mask))

/* Auto Neg Link Partner Ability Register (Addr 0xffe5) */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_MII_ANPr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0xffe5, (_val))
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_MII_ANPr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0xffe5, (_val))
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_MII_ANPr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0xffe5,         \
                                     (_val), (_mask))

/* Auto Neg Expansion Resister (Addr 0xffe6) */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_MII_AN_EXPr(_pctrl, _val)             \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0xffe6, (_val))
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_MII_AN_EXPr(_pctrl, _val)             \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0xffe6, (_val))
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_MII_AN_EXPr(_pctrl, _val, _mask)     \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0xffe6,        \
                                     (_val), (_mask))

/* Extended Status Resister (Addr 0xffef) */
#define BFCMAP_RD_PHY84756_SYS_SGMII_DEV1_MII_ESRr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0xffef, (_val))
#define BFCMAP_WR_PHY84756_SYS_SGMII_DEV1_MII_ESRr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0xffef, (_val))
#define BFCMAP_MOD_PHY84756_SYS_SGMII_DEV1_MII_ESRr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0xffef,        \
                                     (_val), (_mask))




/* 
 * 10G/1G : Line Side PHY registers
 */

/* Device 1 */
/* PCS User Defined Registers */

/* Chip Revision Resister (Addr 0xc801) */
#define BFCMAP_RD_PHY84756_LN_DEV1_CHIP_REVr(_pctrl, _val)                  \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc801, (_val))

/* Chip ID Resister (Addr 0xc802) */
#define BFCMAP_RD_PHY84756_LN_DEV1_CHIP_IDr(_pctrl, _val)                   \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc802, (_val))

/* Speed Link detect Status Resister (Addr 0xc81f) */
#define BFCMAP_RD_PHY84756_LN_DEV1_SPEED_LINK_DETECT_STATr(_pctrl, _val)    \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc81f, (_val))

/* Chip Mode Resister (Addr 0xc805) */
#define BFCMAP_RD_PHY84756_LN_DEV1_CHIP_MODEr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc805, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_CHIP_MODEr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xc805, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_CHIP_MODEr(_pctrl, _val, _mask)\
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xc805,        \
                                     (_val), (_mask))

/* MACSEC Bypass Control Resister (Addr 0xc8f0) */
#define BFCMAP_RD_PHY84756_LN_DEV1_MACSEC_BYPASS_CTLRr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc8f0, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_MACSEC_BYPASS_CTLRr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xc8f0, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_MACSEC_BYPASS_CTLRr(_pctrl, _val, _mask)\
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xc8f0,        \
                                     (_val), (_mask))

/* XPMD system Select Resister (Addr 0xc8f1) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_SYS_SELr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xc8f1, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_SYS_SELr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xc8f1, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_SYS_SELr(_pctrl, _val, _mask)       \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xc8f1,        \
                                     (_val), (_mask))



/* PMD Standard Registers */

/* PMD Control Resister (Addr 0x0000) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0000, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0000, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_CTRLr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0000,       \
                                     (_val), (_mask))

/* PMD Status Resister (Addr 0x0001) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0001, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0001, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_STATr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0001,       \
                                     (_val), (_mask))

/* PMD ID0 Resister (Addr 0x0002) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0002, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0002, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_ID0r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0002,       \
                                     (_val), (_mask))
/* PMD ID1 Resister (Addr 0x0003) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0003, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0003, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_ID1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0003,       \
                                     (_val), (_mask))

/* PMD Speed Ability Resister (Addr 0x0004) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0004, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0004, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0004,         \
                                     (_val), (_mask))

/* PMD Devices in Package1 Resister (Addr 0x0005) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0005, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0005, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0005,         \
                                     (_val), (_mask))

/* PMD Devices in Package2 Resister (Addr 0x0006) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0006, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0006, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0006,         \
                                     (_val), (_mask))

/* PMD Control2 Resister (Addr 0x0007) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_CTRL2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0007, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_CTRL2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0007, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_CTRL2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0007,         \
                                     (_val), (_mask))

/* PMD Status2 Resister (Addr 0x0008) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_STAT2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0008, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_STAT2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0008, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_STAT2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0008,         \
                                     (_val), (_mask))

/* PMD Transmit Disable Resister (Addr 0x0009) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val)          \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x0009, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val)          \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x0009, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val, _mask)  \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x0009,         \
                                     (_val), (_mask))

/* PMD Receive Signal Detect Resister (Addr 0x000a) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_RX_SDr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x000a, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_RX_SDr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x000a, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_RX_SDr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x000a,         \
                                     (_val), (_mask))

/* PMD Extended Ability Resister (Addr 0x000b) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_EXT_ABILITYr(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x000b, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_EXT_ABILITYr(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x000b, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_EXT_ABILITYr(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x000b,         \
                                     (_val), (_mask))

/* PMD OUI ID0 Resister (Addr 0x000e) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_OUI_ID0r(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x000e, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_OUI_ID0r(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x000e, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_OUI_ID0r(_pctrl, _val, _mask)       \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x000e,         \
                                     (_val), (_mask))
/* PMD OUI ID1 Resister (Addr 0x000e) */
#define BFCMAP_RD_PHY84756_LN_DEV1_PMD_OUI_ID1r(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x000f, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMD_OUI_ID1r(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x000f, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMD_OUI_ID1r(_pctrl, _val, _mask)       \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x000f,         \
                                     (_val), (_mask))



/*
 * Device 3 registers 
 */
/* PCS Registers */

/* PCS Control Resister (Addr 0x0000) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0000, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0000, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_CTRLr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0000,       \
                                     (_val), (_mask))

/* PCS Status Resister (Addr 0x0001) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0001, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0001, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_STATr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0001,       \
                                     (_val), (_mask))

/* PCS ID0 Resister (Addr 0x0002) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0002, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0002, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_ID0r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0002,       \
                                     (_val), (_mask))
/* PCS ID1 Resister (Addr 0x0003) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0003, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0003, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_ID1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0003,       \
                                     (_val), (_mask))

/* PCS Speed Ability Resister (Addr 0x0004) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0004, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0004, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0004,         \
                                     (_val), (_mask))

/* PCS Devices in Package1 Resister (Addr 0x0005) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0005, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0005, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0005,         \
                                     (_val), (_mask))

/* PCS Devices in Package2 Resister (Addr 0x0006) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0006, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0006, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0006,         \
                                     (_val), (_mask))

/* PCS Control2 Resister (Addr 0x0007) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_CTRL2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0007, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_CTRL2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0007, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_CTRL2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0007,         \
                                     (_val), (_mask))

/* PCS Status2 Resister (Addr 0x0008) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_STAT2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0008, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_STAT2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0008, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_STAT2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0008,         \
                                     (_val), (_mask))

/* PCS TenGBASE-X Status Register Resister (Addr 0x0018) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0018, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0018, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0018,         \
                                     (_val), (_mask))

/* PCS TenGBASE-R Status Register Resister (Addr 0x0020) */
#define BFCMAP_RD_PHY84756_LN_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, 0x0020, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, 0x0020, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, 0x0020,          \
                                     (_val), (_mask))


/*
 * Device 7 Registers
 */

/* AN User Defined registers */

/* 1000X Control1 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x8300, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x8300, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x8300,         \
                                     (_val), (_mask))

/* 1000X Control2 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x8301, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x8301, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x8301,          \
                                     (_val), (_mask))

/* 1000X Control3 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x8302, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x8302, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x8302,         \
                                     (_val), (_mask))

/* 1000X Control4 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x8303, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x8303, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x8303,         \
                                     (_val), (_mask))

/* MISC2 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_MSIC2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x8309, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_MSIC2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x8309, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_MSIC2r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x8309,         \
                                     (_val), (_mask))

/* 1000X Status1 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_BASE1000X_STAT1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x8304, (_val)) 

/* 1000X Status2 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_BASE1000X_STAT2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x8305, (_val)) 


/* MII Control Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_MII_CTRLr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0xffe0, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_MII_CTRLr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0xffe0, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_MII_CTRLr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe0,         \
                                     (_val), (_mask))

/* MII Status Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_MII_STATr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0xffe1, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_MII_STATr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0xffe1, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_MII_STATr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe1,         \
                                     (_val), (_mask))

/* AutoNeg Adv Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_ANAr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0xffe4, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_ANAr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0xffe4, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_ANAr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe4,         \
                                     (_val), (_mask))
/* Auto Neg Link Partner Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_ANPr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0xffe5, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_ANPr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0xffe5, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_ANPr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe5,         \
                                     (_val), (_mask))

/* Auto Neg Expansion Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_EXRr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0xffe6, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_EXRr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0xffe6, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_EXRr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe6,         \
                                     (_val), (_mask))

/* Auto Neg Next Page Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_NEXT_PAGEr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0xffe7, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_NEXT_PAGEr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0xffe7, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_NEXT_PAGEr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe7,         \
                                     (_val), (_mask))

/* Auto Neg Link Partner Ability 2 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_ANP2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0xffe8, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_ANP2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0xffe8, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_ANP2r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0xffe8,         \
                                     (_val), (_mask))

/* Extended Status 2 Register */
#define BFCMAP_RD_PHY84756_LN_DEV7_ESPr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0xffef, (_val)) 
#define BFCMAP_WR_PHY84756_LN_DEV7_ESPr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0xffef, (_val)) 
#define BFCMAP_MOD_PHY84756_LN_DEV7_ESPr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0xffef,         \
                                     (_val), (_mask))


/* AN Standard registers */

/* AN Control Resister (Addr 0x0000) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0000, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0000, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_CTRLr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0000,       \
                                     (_val), (_mask))

/* AN Status Resister (Addr 0x0001) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0001, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0001, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_STATr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0001,       \
                                     (_val), (_mask))

/* AN ID0 Resister (Addr 0x0002) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0002, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0002, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_ID0r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0002,       \
                                     (_val), (_mask))
/* AN ID1 Resister (Addr 0x0003) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0003, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0003, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_ID1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0003,       \
                                     (_val), (_mask))

/* AN Speed Ability Resister (Addr 0x0004) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_SPEED_ABILITYr(_pctrl, _val)          \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0004, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_SPEED_ABILITYr(_pctrl, _val)          \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0004, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_SPEED_ABILITYr(_pctrl, _val, _mask)  \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0004,         \
                                     (_val), (_mask))

/* AN Devices in Package1 Resister (Addr 0x0005) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val)            \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0005, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val)            \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0005, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val, _mask)    \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0005,         \
                                     (_val), (_mask))

/* AN Devices in Package2 Resister (Addr 0x0006) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val)            \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0006, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val)            \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0006, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val, _mask)    \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0006,         \
                                     (_val), (_mask))

/* AN Advertisement 1 Resister (Addr 0x0010) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_ADV1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0010, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_ADV1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0010, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_ADV1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0010,       \
                                     (_val), (_mask))

/* AN Advertisement 2 Resister (Addr 0x0011) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_ADV2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0011, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_ADV2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0011, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_ADV2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0011,       \
                                     (_val), (_mask))

/* AN Advertisement 3 Resister (Addr 0x0012) */
#define BFCMAP_RD_PHY84756_LN_DEV7_AN_ADV3r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, 0x0012, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_AN_ADV3r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, 0x0012, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_AN_ADV3r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, 0x0012,       \
                                     (_val), (_mask))

#define BFCMAP_RD_PHY84756_LN_DEV1_PMDr(_pctrl, _reg, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, (_reg), (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_PMDr(_pctrl, _reg, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, (_reg), (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_PMDr(_pctrl, _reg, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, (_reg),         \
                                     (_val), (_mask))

#define BFCMAP_RD_PHY84756_LN_DEV3_PCSr(_pctrl, _reg, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0003, (_reg), (_val))
#define BFCMAP_WR_PHY84756_LN_DEV3_PCSr(_pctrl, _reg, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0003, (_reg), (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV3_PCSr(_pctrl, _reg, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0003, (_reg),         \
                                     (_val), (_mask))

#define BFCMAP_RD_PHY84756_LN_DEV7_ANr(_pctrl, _reg, _val)                  \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0007, (_reg), (_val))
#define BFCMAP_WR_PHY84756_LN_DEV7_ANr(_pctrl, _reg, _val)                  \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0007, (_reg), (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV7_ANr(_pctrl, _reg, _val, _mask)          \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0007, (_reg),         \

/* 
 * 10G/1G : System Side PHY registers
 */

/* Device 1 */
/* PMD Control Resister (Addr 0x0000) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0000, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0000, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_CTRLr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0000,       \
                                     (_val), (_mask))

/* PMD Status Resister (Addr 0x0001) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0001, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0001, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_STATr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0001,       \
                                     (_val), (_mask))

/* PMD ID0 Resister (Addr 0x0002) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0002, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0002, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_ID0r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0002,       \
                                     (_val), (_mask))
/* PMD ID1 Resister (Addr 0x0003) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0003, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0003, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_ID1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0003,       \
                                     (_val), (_mask))

/* PMD Speed Ability Resister (Addr 0x0004) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val)       \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0004, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val)       \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0004, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_SPEED_ABILITYr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0004,       \
                                     (_val), (_mask))

/* PMD Devices in Package1 Resister (Addr 0x0005) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0005, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0005, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_DEV_IN_PKG1r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0005,       \
                                     (_val), (_mask))

/* PMD Devices in Package2 Resister (Addr 0x0006) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0006, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0006, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_DEV_IN_PKG2r(_pctrl, _val, _mask)  \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0006,       \
                                     (_val), (_mask))

/* PMD Control 2 Resister (Addr 0x0007) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_CTRL2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0007, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_CTRL2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0007, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_CTRL2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0007,       \
                                     (_val), (_mask))
/* PMD Status 2 Resister (Addr 0x0007) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_STAT2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0008, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_STAT2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0008, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_STAT2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0008,       \
                                     (_val), (_mask))

/* PMD Transmit Disable Resister (Addr 0x0009) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x0009, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x0009, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_XMIT_DISABLEr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x0009,       \
                                     (_val), (_mask))

/* PMD Receive Signal Detect Resister (Addr 0x000a) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_RX_SDr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x000a, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_RX_SDr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x000a, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_RX_SDr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x000a,       \
                                     (_val), (_mask))

/* PMD Extended Ability Resister (Addr 0x000b) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_EXT_ABILITYr(_pctrl, _val)          \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x000b, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_EXT_ABILITYr(_pctrl, _val)          \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x000b, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_EXT_ABILITYr(_pctrl, _val, _mask)  \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x000b,       \
                                     (_val), (_mask))

/* PMD OUI ID0 Resister (Addr 0x000e) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_OUI_ID0r(_pctrl, _val)              \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x000e, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_OUI_ID0r(_pctrl, _val)              \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x000e, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_OUI_ID0r(_pctrl, _val, _mask)      \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x000e,       \
                                     (_val), (_mask))
/* PMD OUI ID1 Resister (Addr 0x000e) */
#define BFCMAP_RD_PHY84756_SYS_DEV1_PMD_OUI_ID1r(_pctrl, _val)              \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0001, 0x000f, (_val))
#define BFCMAP_WR_PHY84756_SYS_DEV1_PMD_OUI_ID1r(_pctrl, _val)              \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0001, 0x000f, (_val))
#define BFCMAP_MOD_PHY84756_SYS_DEV1_PMD_OUI_ID1r(_pctrl, _val, _mask)      \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0001, 0x000f,       \
                                     (_val), (_mask))


/*
 * Device 3 registers 
 */
/* XFI PCS Registers */

/* PCS Control Resister (Addr 0x0000) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0000, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0000, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_CTRLr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0000,       \
                                     (_val), (_mask))

/* PCS Status Resister (Addr 0x0001) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0001, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0001, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_STATr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0001,       \
                                     (_val), (_mask))

/* PCS ID0 Resister (Addr 0x0002) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0002, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0002, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_ID0r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0002,       \
                                     (_val), (_mask))
/* PCS ID1 Resister (Addr 0x0003) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0003, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0003, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_ID1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0003,       \
                                     (_val), (_mask))

/* PCS Speed Ability Resister (Addr 0x0004) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0004, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0004, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_SPEED_ABILITYr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0004,       \
                                     (_val), (_mask))

/* PCS Devices in Package1 Resister (Addr 0x0005) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0005, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0005, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_DEV_IN_PKG1r(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0005,       \
                                     (_val), (_mask))

/* PCS Devices in Package2 Resister (Addr 0x0006) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0006, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0006, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_DEV_IN_PKG2r(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0006,       \
                                     (_val), (_mask))

/* PCS Control2 Resister (Addr 0x0007) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_CTRL2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0007, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_CTRL2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0007, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_CTRL2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0007,       \
                                     (_val), (_mask))

/* PCS Status2 Resister (Addr 0x0008) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_STAT2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0008, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_STAT2r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0008, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_STAT2r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0008,       \
                                     (_val), (_mask))

/* PCS TenGBASE-X Status Register Resister (Addr 0x0018) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val)       \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0018, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0018, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_10GBASE_X_STATr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0018,       \
                                     (_val), (_mask))

/* PCS TenGBASE-R Status Register Resister (Addr 0x0020) */
#define BFCMAP_RD_PHY84756_XFI_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0003, 0x0020, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val)        \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0003, 0x0020, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV3_PCS_10GBASE_R_STATr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0003, 0x0020,       \
                                     (_val), (_mask))



/*
 * Device 7 Registers
 */

/* AN User Defined registers */

/* 1000X Control1 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x8300, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x8300, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x8300,         \
                                     (_val), (_mask))

/* 1000X Control2 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x8301, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x8301, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL2r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x8301,         \
                                     (_val), (_mask))

/* 1000X Control3 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x8302, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x8302, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL3r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x8302,         \
                                     (_val), (_mask))

/* 1000X Control4 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x8303, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x8303, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_BASE1000X_CTRL4r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x8303,         \
                                     (_val), (_mask))

/* PRBS Generator Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_PRBS_GEN(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x0, 0x0007, 0x8019, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_PRBS_GEN(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x0, 0x0007, 0x8019, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_PRBS_GEN(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x0, 0x0007, 0x8019,         \
                                     (_val), (_mask))

/* Lane Control 0 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_LANE_CTRL0(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x0, 0x0007, 0x8015, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_LANE_CTRL0(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x0, 0x0007, 0x8015, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_LANE_CTRL0(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x0, 0x0007, 0x8015,         \
                                     (_val), (_mask))

/* Lane Control 2 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_LANE_CTRL2(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x0, 0x0007, 0x8017, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_LANE_CTRL2(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x0, 0x0007, 0x8017, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_LANE_CTRL2(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x0, 0x0007, 0x8017,         \
                                     (_val), (_mask))

/* XGXS Status Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_XGXS_CTRL(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x0, 0x0007, 0x8000, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_XGXS_CTRL(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x0, 0x0007, 0x8000, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_XGXS_CTRL(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x0, 0x0007, 0x8000,         \
                                     (_val), (_mask))

/* PRBS Checker Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_PRBS_CHK(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x0, 0x0007, 0x80B0, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_PRBS_CHK(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x0, 0x0007, 0x80B0, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_PRBS_CHK(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x0, 0x0007, 0x80B0,         \
                                     (_val), (_mask))
/* PRBS Checker Lock Err Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_PRBS_LOCK_ERR(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x0, 0x0007, 0x80B1, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_PRBS_LOCK_ERR(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x0, 0x0007, 0x80B1, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_PRBS_LOCK_ERR(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x0, 0x0007, 0x80B1,         \
                                     (_val), (_mask))

/* MISC2 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_MISC2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x8309, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_MISC2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x8309, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_MISC2r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x8309,         \
                                     (_val), (_mask))

/* 1000X Status1 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_BASE1000X_STAT1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x8304, (_val)) 

/* 1000X Status2 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_BASE1000X_STAT2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x8305, (_val)) 

/* MII Control Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_MII_CTRLr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0xffe0, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_MII_CTRLr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0xffe0, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_MII_CTRLr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe0,         \
                                     (_val), (_mask))

/* MII Status Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_MII_STATr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0xffe1, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_MII_STATr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0xffe1, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_MII_STATr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe1,         \
                                     (_val), (_mask))

/* AutoNeg Adv Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_ANAr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0xffe4, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_ANAr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0xffe4, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_ANAr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe4,         \
                                     (_val), (_mask))
/* Auto Neg Link Partner Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_ANPr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0xffe5, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_ANPr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0xffe5, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_ANPr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe5,         \
                                     (_val), (_mask))

/* Auto Neg Expansion Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_EXRr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0xffe6, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_EXRr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0xffe6, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_EXRr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe6,         \
                                     (_val), (_mask))

/* Auto Neg Next Page Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_NEXT_PAGEr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0xffe7, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_NEXT_PAGEr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0xffe7, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_NEXT_PAGEr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe7,         \
                                     (_val), (_mask))

/* Auto Neg Link Partner Ability 2 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_ANP2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0xffe8, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_ANP2r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0xffe8, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_ANP2r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0xffe8,         \
                                     (_val), (_mask))

/* Extended Status 2 Register */
#define BFCMAP_RD_PHY84756_XFI_DEV7_ESPr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0xffef, (_val)) 
#define BFCMAP_WR_PHY84756_XFI_DEV7_ESPr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0xffef, (_val)) 
#define BFCMAP_MOD_PHY84756_XFI_DEV7_ESPr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0xffef,         \
                                     (_val), (_mask))


/* AN Standard registers */

/* AN Control Resister (Addr 0x0000) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0000, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_CTRLr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0000, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_CTRLr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0000,       \
                                     (_val), (_mask))

/* AN Status Resister (Addr 0x0001) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0001, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_STATr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0001, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_STATr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0001,       \
                                     (_val), (_mask))

/* AN ID0 Resister (Addr 0x0002) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0002, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_ID0r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0002, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_ID0r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0002,       \
                                     (_val), (_mask))
/* AN ID1 Resister (Addr 0x0003) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0003, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_ID1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0003, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_ID1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0003,       \
                                     (_val), (_mask))

/* AN Speed Ability Resister (Addr 0x0004) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_SPEED_ABILITYr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0004, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_SPEED_ABILITYr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0004, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_SPEED_ABILITYr(_pctrl, _val, _mask)  \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0004,       \
                                     (_val), (_mask))

/* AN Devices in Package1 Resister (Addr 0x0005) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0005, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0005, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_DEV_IN_PKG1r(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0005,       \
                                     (_val), (_mask))

/* AN Devices in Package2 Resister (Addr 0x0006) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0006, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val)           \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0006, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_DEV_IN_PKG2r(_pctrl, _val, _mask)   \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0006,       \
                                     (_val), (_mask))

/* AN Advertisement 1 Resister (Addr 0x0010) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_ADV1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0010, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_ADV1r(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0010, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_ADV1r(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0010,       \
                                     (_val), (_mask))

/* AN Advertisement 2 Resister (Addr 0x0011) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_ADV2r(_pctrl, _val)                    \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0011, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_ADV2r(_pctrl, _val)                    \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0011, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_ADV2r(_pctrl, _val, _mask)            \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0011,           \
                                     (_val), (_mask))

/* AN Advertisement 3 Resister (Addr 0x0012) */
#define BFCMAP_RD_PHY84756_XFI_DEV7_AN_ADV3r(_pctrl, _val)                    \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x0012, (_val))
#define BFCMAP_WR_PHY84756_XFI_DEV7_AN_ADV3r(_pctrl, _val)                    \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x0012, (_val))
#define BFCMAP_MOD_PHY84756_XFI_DEV7_AN_ADV3r(_pctrl, _val, _mask)            \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x0012,           \
                                     (_val), (_mask))


/*
 * Firmware Download related registers
 */

/* M8051 Message IN Resister (Addr 0xCA12) */
#define BFCMAP_RD_PHY84756_LN_DEV1_M8501_MSGINr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xCA12, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_M8501_MSGINr(_pctrl, _val)                 \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xCA12, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_M8501_MSGINr(_pctrl, _val, _mask)         \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xCA12,           \
                                     (_val), (_mask))

/* M8051 Message OUT Resister (Addr 0xCA13) */
#define BFCMAP_RD_PHY84756_LN_DEV1_M8501_MSGOUTr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xCA13, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_M8501_MSGOUTr(_pctrl, _val)                \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xCA13, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_M8501_MSGOUTr(_pctrl, _val, _mask)        \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xCA13,           \
                                     (_val), (_mask))

/* RX Alarm Resister (Addr 0x9003) */
#define BFCMAP_RD_PHY84756_LN_DEV1_RX_ALARMr(_pctrl, _val)                    \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x9003, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_RX_ALARMr(_pctrl, _val)                    \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x9003, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_RX_ALARMr(_pctrl, _val, _mask)            \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x9003,           \
                                     (_val), (_mask))

/* SPA Control and Status Resister (Addr 0xC848) */
#define BFCMAP_RD_PHY84756_LN_DEV1_SPI_CTRL_STATr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xC848, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_SPI_CTRL_STATr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xC848, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_SPI_CTRL_STATr(_pctrl, _val, _mask)       \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xC848,           \
                                     (_val), (_mask))

/* SPA Control and Status Resister (Addr 0xCA1C) */
#define BFCMAP_RD_PHY84756_LN_DEV1_CHECKSUMr(_pctrl, _val)                    \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xCA1C, (_val))


/* SPI ROM Firmware update related */

/*
 * SPI-ROM Program related defs
 */
#define BFCMAP_SPI_CTRL_1_L        0xC000
#define BFCMAP_SPI_CTRL_1_H        0xC002
#define BFCMAP_SPI_CTRL_2_L        0xC400
#define BFCMAP_SPI_CTRL_2_H        0xC402
#define BFCMAP_SPI_TXFIFO          0xD000
#define BFCMAP_SPI_RXFIFO          0xD400
#define BFCMAP_WR_CPU_CTRL_REGS    0x11
#define BFCMAP_RD_CPU_CTRL_REGS    0xEE
#define BFCMAP_WR_CPU_CTRL_FIFO    0x66

/*
 * SPI Controller Commands(Messages).
 */
#define BFCMAP_MSGTYPE_HWR          0x40
#define BFCMAP_MSGTYPE_HRD          0x80
#define BFCMAP_WRSR_OPCODE          0x01
#define BFCMAP_WR_OPCODE            0x02
#define BFCMAP_WRDI_OPCODE          0x04
#define BFCMAP_RDSR_OPCODE          0x05
#define BFCMAP_WREN_OPCODE          0x06
#define BFCMAP_WR_BLOCK_SIZE        0x40
#define BFCMAP_TOTAL_WR_BYTE        0x4000
                                                                                
#define BFCMAP_WR_TIMEOUT      100
#define BFCMAP_WR_ITERATIONS   10000


/* Misc Control 2 Resister(SPI ROM FW Upload control) (Addr 0xCA85) */
#define BFCMAP_RD_PHY84756_LN_DEV1_MISC_CNTL2r(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0xCA85, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_MISC_CNTL2r(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0xCA85, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_MISC_CNTL2r(_pctrl, _val, _mask)       \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0xCA85,           \
                                     (_val), (_mask))

/* RX Alarm Control Resister (Addr 0x9000) */
#define BFCMAP_RD_PHY84756_LN_DEV1_RX_ALARM_CNTLr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x9000, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_RX_ALARM_CNTLr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x9000, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_RX_ALARM_CNTLr(_pctrl, _val, _mask)       \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x9000,           \
                                     (_val), (_mask))

/* TX Alarm Control Resister (Addr 0x9001) */
#define BFCMAP_RD_PHY84756_LN_DEV1_TX_ALARM_CNTLr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x9001, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_TX_ALARM_CNTLr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x9001, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_TX_ALARM_CNTLr(_pctrl, _val, _mask)       \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x9001,           \
                                     (_val), (_mask))

/* LASI Control Resister (Addr 0x9002) */
#define BFCMAP_RD_PHY84756_LN_DEV1_LASI_CNTLr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x9002, (_val))
#define BFCMAP_WR_PHY84756_LN_DEV1_LASI_CNTLr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x00, 0x0001, 0x9002, (_val))
#define BFCMAP_MOD_PHY84756_LN_DEV1_LASI_CNTLr(_pctrl, _val, _mask)       \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x00, 0x0001, 0x9002,           \
                                     (_val), (_mask))

/* RX Alarm Status Register (Addr 0x9003) */
#define BFCMAP_RD_PHY84756_LN_DEV1_RX_ALARM_STATr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x9003, (_val))

/* TX Alarm Status Register (Addr 0x9003) */
#define BFCMAP_RD_PHY84756_LN_DEV1_TX_ALARM_STATr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x9004, (_val))

/* LASI Status Register (Addr 0x9003) */
#define BFCMAP_RD_PHY84756_LN_DEV1_LASI_STATr(_pctrl, _val)               \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x00, 0x0001, 0x9005, (_val))

/* XGXS clause 45 registers */

/* AN User Defined registers */

/* 1000X Control1 Register */
#define BFCMAP_RD_PHY84756_XGXS_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0007, 0x8300, (_val)) 
#define BFCMAP_WR_PHY84756_XGXS_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0007, 0x8300, (_val)) 
#define BFCMAP_MOD_PHY84756_XGXS_DEV7_AN_BASE1000X_CTRL1r(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0007, 0x8300,         \
                                     (_val), (_mask))

/* XGXS control Register */
#define BFCMAP_RD_PHY84756_XGXS_CTRLr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x4, 0x8000, (_val)) 
#define BFCMAP_WR_PHY84756_XGXS_CTRLr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x4, 0x8000, (_val)) 
#define BFCMAP_MOD_PHY84756_XGXS_CTRLr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x4, 0x8000,         \
                                     (_val), (_mask))

/* XGXS Lane status Register */
#define BFCMAP_RD_PHY84756_XGXS_DEV4_XS_LANE_STATr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_RD((_pctrl), 0x01, 0x0004, 0x18, (_val)) 
#define BFCMAP_WR_PHY84756_XGXS_DEV4_XS_LANE_STATr(_pctrl, _val)         \
            BFCMAP_PHY84756_REG_WR((_pctrl), 0x01, 0x0004, 0x18, (_val)) 
#define BFCMAP_MOD_PHY84756_XGXS_DEV4_XS_LANE_STATr(_pctrl, _val, _mask) \
            BFCMAP_PHY84756_REG_MOD((_pctrl), 0x01, 0x0004, 0x18,         \
                                     (_val), (_mask))
                                     
#endif /* _PHY84756_INT_H */
