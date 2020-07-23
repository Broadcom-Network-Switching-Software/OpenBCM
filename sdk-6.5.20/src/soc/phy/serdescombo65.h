/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdescombo65.h
 * Purpose:     BCM5621X (Raptor) Internal Combo Port PHY Driver
 *              10/100/1000/2500
 */

#ifndef   _SERDES_COMBO65_H_
#define   _SERDES_COMBO65_H_

/* SERDES register access */
#define SERDESCOMBO65_REG_READ(_unit, _phy_ctrl, flags, \
                          _reg_addr, _val) \
            phy_reg_aer_read((_unit), (_phy_ctrl), \
                            (_reg_addr), (_val))
#define SERDESCOMBO65_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _reg_addr, _val) \
            phy_reg_aer_write((_unit), (_phy_ctrl), \
                             (_reg_addr), (_val))
#define SERDESCOMBO65_REG_MODIFY(_unit, _phy_ctrl, _flags, \
                            _reg_addr, _val, _mask) \
            phy_reg_aer_modify((_unit), (_phy_ctrl), \
                              (_reg_addr), (_val), (_mask))


/*
 * Register definitions for Internal SerDes 10/100/1000/2500 PHY with CL73
 */

/***************************/
/* Standard IEEE Registers */
/***************************/

/* MII Control (Addr 00h) */
#define READ_COMBO_IEEE0_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000FFE0, (_val))
#define WRITE_COMBO_IEEE0_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000FFE0, (_val))
#define MODIFY_COMBO_IEEE0_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000FFE0, \
                               (_val), (_mask))

/* MII Status (Addr 01h) */
#define READ_COMBO_IEEE0_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE1, (_val))
#define WRITE_COMBO_IEEE0_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE1, (_val))
#define MODIFY_COMBO_IEEE0_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0xFFE1, \
                               (_val), (_mask))

/* PHY ID 0 (Addr 02h) */
#define READ_COMBO_IEEE0_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE2, (_val))
#define WRITE_COMBO_IEEE0_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE2, (_val))
#define MODIFY_COMBO_IEEE0_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0xFFE2, \
                             (_val), (_mask))

/* PHY ID 1 (Addr 03h) */
#define READ_COMBO_IEEE0_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE3, (_val))
#define WRITE_COMBO_IEEE0_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE3, (_val))
#define MODIFY_COMBO_IEEE0_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0xFFE3, \
                             (_val), (_mask))

/* Autoneg Adv (Addr 04h) */
#define READ_COMBO_IEEE0_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE4, (_val))
#define WRITE_COMBO_IEEE0_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE4, (_val))
#define MODIFY_COMBO_IEEE0_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0xFFE4, \
                             (_val), (_mask))

/* Autoneg Link Partner Ability (Addr 05h) */
#define READ_COMBO_IEEE0_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                   0x00, 0xFFE5, (_val))
#define WRITE_COMBO_IEEE0_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                   0x00, 0xFFE5, (_val))
#define MODIFY_COMBO_IEEE0_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0xFFE5, \
                             (_val), (_mask))

/* Autoneg Expansion (Addr 06h) */
#define READ_COMBO_IEEE0_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE6, (_val))
#define WRITE_COMBO_IEEE0_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0xFFE6, (_val))
#define MODIFY_COMBO_IEEE0_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0xFFE6, \
                             (_val), (_mask))

/* Extended Status (Addr 0fh) */

/*****************************/ 
/* XGXS Block 0 (Blk 0x8000) */
/*****************************/ 
/* XGXS Control Register (Addr 8000h) */
#define READ_XGXS_BLK0_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008000, (_val))
#define WRITE_XGXS_BLK0_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008000, (_val))
#define MODIFY_XGXS_BLK0_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008000, \
                             (_val), (_mask))

/* XGXS Status Register (Addr 8001h) */
#define READ_XGXS_BLK0_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008001, (_val))
#define WRITE_XGXS_BLK0_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008001, (_val))
#define MODIFY_XGXS_BLK0_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008001, \
                             (_val), (_mask))

/*************************/
/* TX ALL (Block 0x80A0) */
/*************************/
/* TX AStatus 0 (Addr 80A0h) */
/* TX AControl 0 (Addr 80A1h) */
/* TX mdata 0 (Addr 80A2h) */
/* TX mdata 1 (Addr 80A3h) */
/* TX AStatus 1 (Addr 80A4h) */
/* TX BgVcm (Addr 80A5h) */
/* TX lbuff 1T2T (Addr 80A6h) */

/* TX Driver (Addr 80A7h) */
#define READ_TX_ALL_DRIVERr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x000080A7, (_val))
#define WRITE_TX_ALL_DRIVERr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x000080A7, (_val))
#define MODIFY_TX_ALL_DRIVERr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000080A7, \
                             (_val), (_mask))



/*************************/
/* SerDes Digital Block (Blk 0x8300) */
/*************************/
/* 1000X Control 1 (Addr 8300h) */
#define READ_SERDES_DIGITAL_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8300, (_val))
#define WRITE_SERDES_DIGITAL_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8300, (_val))
#define MODIFY_SERDES_DIGITAL_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8300, \
                             (_val), (_mask))

/* DIGITAL_1000X Control 2 (Addr 8301h) */
#define READ_SERDES_DIGITAL_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8301, (_val))
#define WRITE_SERDES_DIGITAL_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8301, (_val))
#define MODIFY_SERDES_DIGITAL_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8301, \
                             (_val), (_mask))

/* DIGITAL_1000X Control 3 (Addr 8302h) */
#define READ_SERDES_DIGITAL_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8302, (_val))
#define WRITE_SERDES_DIGITAL_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8302, (_val))
#define MODIFY_SERDES_DIGITAL_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8302, \
                             (_val), (_mask))

/* DIGITAL_1000X Control 4 (Addr 8303h) */
#define READ_SERDES_DIGITAL_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8303, (_val))
#define WRITE_SERDES_DIGITAL_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8303, (_val))
#define MODIFY_SERDES_DIGITAL_CTRL4r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8303, \
                             (_val), (_mask))

/* DIGITAL_1000X Status 1 (Addr 8304h) */
#define READ_SERDES_DIGITAL_STAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8304, (_val))
#define WRITE_SERDES_DIGITAL_STAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8304, (_val))
#define MODIFY_SERDES_DIGITAL_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8304, \
                             (_val), (_mask))

/* DIGITAL_1000X Status 2 (Addr 8305h) */
#define READ_SERDES_DIGITAL_STAT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8305, (_val))
#define WRITE_SERDES_DIGITAL_STAT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8305, (_val))
#define MODIFY_SERDES_DIGITAL_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8305, \
                             (_val), (_mask))

/* DIGITAL_1000X Status 3 (Addr 8306h) */
#define READ_SERDES_DIGITAL_STAT3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8306, (_val))
#define WRITE_SERDES_DIGITAL_STAT3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8306, (_val))
#define MODIFY_SERDES_DIGITAL_STAT3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8306, \
                             (_val), (_mask))

/* Ber Crc Err Rx Pkt cntr (Addr 8307h) */
#define READ_SERDES_DIGITAL_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8307, (_val))
#define WRITE_SERDES_DIGITAL_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8307, (_val))
#define MODIFY_SERDES_DIGITAL_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8307, \
                             (_val), (_mask))

/* Misc1 (Addr 8308h) */
#define READ_SERDES_DIGITAL_MISC1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8308, (_val))
#define WRITE_SERDES_DIGITAL_MISC1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8308, (_val))
#define MODIFY_SERDES_DIGITAL_MISC1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8308, \
                             (_val), (_mask))

/* Misc2 (Addr 8309h) */
#define READ_SERDES_DIGITAL_MISC2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8309, (_val))
#define WRITE_SERDES_DIGITAL_MISC2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8309, (_val))
#define MODIFY_SERDES_DIGITAL_MISC2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8309, \
                             (_val), (_mask))

/* Pat Gen Control (Addr 830Ah) */
#define READ_SERDES_DIGITAL_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x830a, (_val))
#define WRITE_SERDES_DIGITAL_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x830a, (_val))
#define MODIFY_SERDES_DIGITAL_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x830a, \
                             (_val), (_mask))

/* Pat Gen Status (Addr 830Bh) */
#define READ_SERDES_DIGITAL_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x830b, (_val))
#define WRITE_SERDES_DIGITAL_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x830b, (_val))
#define MODIFY_SERDES_DIGITAL_PAT_GEN_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x830b, \
                             (_val), (_mask))

/* Test Mode (Addr 830Ch) */
#define READ_SERDES_DIGITAL_TEST_MODEr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x830c, (_val))
#define WRITE_SERDES_DIGITAL_TEST_MODEr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x830c, (_val))
#define MODIFY_SERDES_DIGITAL_TEST_MODEr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x830c, \
                             (_val), (_mask))

/* Tx Pkt Cnt (Addr 830Dh) */
#define READ_SERDES_DIGITAL_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x830d, (_val))
#define WRITE_SERDES_DIGITAL_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x830d, (_val))
#define MODIFY_SERDES_DIGITAL_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x830d, \
                             (_val), (_mask))

/* Rx Pkt Cnt (Addr 830Eh) */
#define READ_SERDES_DIGITAL_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x830e, (_val))
#define WRITE_SERDES_DIGITAL_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x830e, (_val))
#define MODIFY_SERDES_DIGITAL_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x830e, \
                             (_val), (_mask))

/************************/
/* Test (Blk 8310-831F) */
/************************/

/***************************/
/* Over 1G (Blk 8320-832F) */
/***************************/
#define READ_OVER_1G_DIGCTL_3_0r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008320, (_val))
#define WRITE_OVER_1G_DIGCTL_3_0r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008320, (_val))
#define MODIFY_OVER_1G_DIGCTL_3_0r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008320, \
                             (_val), (_mask))

#define READ_OVER_1G_DIGCTL_3_1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008321, (_val))
#define WRITE_OVER_1G_DIGCTL_3_1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008321, (_val))
#define MODIFY_OVER_1G_DIGCTL_3_1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008321, \
                             (_val), (_mask))

#define READ_OVER_1G_DIGCTL_3_2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008322, (_val))
#define WRITE_OVER_1G_DIGCTL_3_2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008322, (_val))
#define MODIFY_OVER_1G_DIGCTL_3_2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008322, \
                             (_val), (_mask))

#define READ_OVER_1G_DIGCTL_3_3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008323, (_val))
#define WRITE_OVER_1G_DIGCTL_3_3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008323, (_val))
#define MODIFY_OVER_1G_DIGCTL_3_3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008323, \
                             (_val), (_mask))

#define READ_OVER_1G_DIGCTL_3_4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008324, (_val))
#define WRITE_OVER_1G_DIGCTL_3_4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008324, (_val))
#define MODIFY_OVER_1G_DIGCTL_3_4r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008324, \
                             (_val), (_mask))

#define READ_OVER_1G_DIGCTL_3_5r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008325, (_val))
#define WRITE_OVER_1G_DIGCTL_3_5r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008325, (_val))
#define MODIFY_OVER_1G_DIGCTL_3_5r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008325, \
                             (_val), (_mask))

#define READ_OVER_1G_DIGCTL_3_6r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008326, (_val))
#define WRITE_OVER_1G_DIGCTL_3_6r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008326, (_val))
#define MODIFY_OVER_1G_DIGCTL_3_6r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008326, \
                             (_val), (_mask))

#define READ_OVER_1G_TPOUT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008327, (_val))
#define WRITE_OVER_1G_TPOUT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008327, (_val))
#define MODIFY_OVER_1G_TPOUT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008327, \
                             (_val), (_mask))

#define READ_OVER_1G_TPOUT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008328, (_val))
#define WRITE_OVER_1G_TPOUT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008328, (_val))
#define MODIFY_OVER_1G_TPOUT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008328, \
                             (_val), (_mask))

#define READ_OVER_1G_UP1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008329, (_val))
#define WRITE_OVER_1G_UP1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008329, (_val))
#define MODIFY_OVER_1G_UP1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008329, \
                             (_val), (_mask))

#define READ_OVER_1G_UP2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832A, (_val))
#define WRITE_OVER_1G_UP2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832A, (_val))
#define MODIFY_OVER_1G_UP2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000832A, \
                             (_val), (_mask))


#define READ_OVER_1G_UP3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832B, (_val))
#define WRITE_OVER_1G_UP3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832B, (_val))
#define MODIFY_OVER_1G_UP3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000832B, \
                             (_val), (_mask))

#define READ_OVER_1G_LP_UP1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832C, (_val))
#define WRITE_OVER_1G_LP_UP1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832C, (_val))
#define MODIFY_OVER_1G_LP_UP1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000832C, \
                             (_val), (_mask))

#define READ_OVER_1G_LP_UP2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832D, (_val))
#define WRITE_OVER_1G_LP_UP2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832D, (_val))
#define MODIFY_OVER_1G_LP_UP2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000832D, \
                             (_val), (_mask))

#define READ_OVER_1G_LP_UP3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832E, (_val))
#define WRITE_OVER_1G_LP_UP3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000832E, (_val))
#define MODIFY_OVER_1G_LP_UP3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000832E, \
                             (_val), (_mask))

/******************************/
/* Remote PHY (Blk 8330-833F) */
/******************************/

/*********************************/
/* BAM Next Page (Blk 8350-835F) */
/*********************************/

/*************************************/
/* Clause 73 User B0 (BLK 8370-837F) */
/*************************************/
/* CL73 UCtrl 1 (Addr 8370h) */
#define READ_CL73_USERB0_UCTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8370, (_val))
#define WRITE_CL73_USERB0_UCTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8370, (_val))
#define MODIFY_CL73_USERB0_UCTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8370, \
                             (_val), (_mask))

/* CL73_UStat1 (Addr 8371h) */
#define READ_CL73_USERB0_USTAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8371, (_val))
#define WRITE_CL73_USERB0_USTAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8371, (_val))
#define MODIFY_CL73_USERB0_USTAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8371, \
                             (_val), (_mask))

/* CL73_BAMCtrl1 (Addr 8372h) */
#define READ_CL73_USERB0_BAMCTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8372, (_val))
#define WRITE_CL73_USERB0_BAMCTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8372, (_val))
#define MODIFY_CL73_USERB0_BAMCTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8372, \
                             (_val), (_mask))

/* CL73_BAMCtrl2 (Addr 8373h) */
#define READ_CL73_USERB0_BAMCTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8373, (_val))
#define WRITE_CL73_USERB0_BAMCTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8373, (_val))
#define MODIFY_CL73_USERB0_BAMCTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8373, \
                             (_val), (_mask))

/* CL73_BAMCtrl3 (Addr 8374h) */
#define READ_CL73_USERB0_BAMCTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8374, (_val))
#define WRITE_CL73_USERB0_BAMCTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8374, (_val))
#define MODIFY_CL73_USERB0_BAMCTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8374, \
                             (_val), (_mask))

/* CL73_BAMStat1 (Addr 8375h) */
#define READ_CL73_USERB0_BAMSTAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8375, (_val))
#define WRITE_CL73_USERB0_BAMSTAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8375, (_val))
#define MODIFY_CL73_USERB0_BAMSTAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8375, \
                             (_val), (_mask))

/* CL73_BAMStat2 (Addr 8376h) */
#define READ_CL73_USERB0_BAMSTAT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8376, (_val))
#define WRITE_CL73_USERB0_BAMSTAT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8376, (_val))
#define MODIFY_CL73_USERB0_BAMSTAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8376, \
                             (_val), (_mask))

/* CL73_BAMStat3 (Addr 8377h) */
#define READ_CL73_USERB0_BAMSTAT3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x8377, (_val))
#define WRITE_CL73_USERB0_BAMSTAT3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x8377, (_val))
#define MODIFY_CL73_USERB0_BAMSTAT3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x8377, \
                             (_val), (_mask))

/*****************/
/* Clause 73 MMD */
/*****************/
/*********************/
/* CL73_IEEEB0 Block */
/*********************/
/* CL73 AN Control (Addr 0000h) */
#define READ_CL73_IEEEB0_AN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000000, (_val))
#define WRITE_CL73_IEEEB0_AN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000000, (_val))
#define MODIFY_CL73_IEEEB0_AN_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000000, \
                             (_val), (_mask))

/* CL73 AN Status (Addr 0001h) */
#define READ_CL73_IEEEB0_AN_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000001, (_val))
#define WRITE_CL73_IEEEB0_AN_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000001, (_val))
#define MODIFY_CL73_IEEEB0_AN_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000001, \
                             (_val), (_mask))

/* CL73 PhyIdMsb (Addr 0002h) */
#define READ_CL73_IEEEB0_PHY_ID_MSBr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000002, (_val))
#define WRITE_CL73_IEEEB0_PHY_ID_MSBr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000002, (_val))
#define MODIFY_CL73_IEEEB0_PHY_ID_MSBr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000002, \
                             (_val), (_mask))

/* CL73 PhyIdLsb (Addr 0003h) */
#define READ_CL73_IEEEB0_PHY_ID_LSBr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000003, (_val))
#define WRITE_CL73_IEEEB0_PHY_ID_LSBr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000003, (_val))
#define MODIFY_CL73_IEEEB0_PHY_ID_LSBr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000003, \
                             (_val), (_mask))

/* IeeeDevInPkg1 (Addr 0005h) */
#define READ_CL73_IEEEB0_DEV_IN_PKG1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000005, (_val))
#define WRITE_CL73_IEEEB0_DEV_IN_PKG1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000005, (_val))
#define MODIFY_CL73_IEEEB0_DEV_IN_PKG1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000005, \
                             (_val), (_mask))

/* IeeeDevInPkg2 (Addr 0006h) */
#define READ_CL73_IEEEB0_DEV_IN_PKG2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000006, (_val))
#define WRITE_CL73_IEEEB0_DEV_IN_PKG2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000006, (_val))
#define MODIFY_CL73_IEEEB0_DEV_IN_PKG2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000006, \
                             (_val), (_mask))

/*********************/
/* CL73 IEEEB1 Block */
/*********************/
/* AN adv1 (Addr 0010h) */
#define READ_CL73_IEEEB1_AN_ADV1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000010, (_val))
#define WRITE_CL73_IEEEB1_AN_ADV1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000010, (_val))
#define MODIFY_CL73_IEEEB1_AN_ADV1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000010, \
                             (_val), (_mask))

/* AN adv2 (Addr 0011h) */
#define READ_CL73_IEEEB1_AN_ADV2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000011, (_val))
#define WRITE_CL73_IEEEB1_AN_ADV2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000011, (_val))
#define MODIFY_CL73_IEEEB1_AN_ADV2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000011, \
                             (_val), (_mask))

/* AN adv3 (Addr 0012h) */
#define READ_CL73_IEEEB1_AN_ADV3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000012, (_val))
#define WRITE_CL73_IEEEB1_AN_ADV3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000012, (_val))
#define MODIFY_CL73_IEEEB1_AN_ADV3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000012, \
                             (_val), (_mask))

/* AN lp adv1 (Addr 0013h) */
#define READ_CL73_IEEEB1_LP_ADV1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000013, (_val))
#define WRITE_CL73_IEEEB1_LP_ADV1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000013, (_val))
#define MODIFY_CL73_IEEEB1_LP_ADV1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000013, \
                             (_val), (_mask))

/* AN lp adv2 (Addr 0014h) */
#define READ_CL73_IEEEB1_LP_ADV2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000014, (_val))
#define WRITE_CL73_IEEEB1_LP_ADV2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000014, (_val))
#define MODIFY_CL73_IEEEB1_LP_ADV2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000014, \
                             (_val), (_mask))

/* AN lp adv3 (Addr 0015h) */
#define READ_CL73_IEEEB1_LP_ADV3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000015, (_val))
#define WRITE_CL73_IEEEB1_LP_ADV3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000015, (_val))
#define MODIFY_CL73_IEEEB1_LP_ADV3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000015, \
                             (_val), (_mask))

/* AN np 1 (Addr 0016h) */ 
#define READ_CL73_IEEEB1_AN_NP1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000016, (_val))
#define WRITE_CL73_IEEEB1_AN_NP1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000016, (_val))
#define MODIFY_CL73_IEEEB1_AN_NP1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000016, \
                             (_val), (_mask))

/* AN np 2 (Addr 0017h) */
#define READ_CL73_IEEEB1_AN_NP2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000017, (_val))
#define WRITE_CL73_IEEEB1_AN_NP2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000017, (_val))
#define MODIFY_CL73_IEEEB1_AN_NP2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000017, \
                             (_val), (_mask))

/* AN np 3 (Addr 0018h) */
#define READ_CL73_IEEEB1_AN_NP3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000018, (_val))
#define WRITE_CL73_IEEEB1_AN_NP3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000018, (_val))
#define MODIFY_CL73_IEEEB1_AN_NP3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000018, \
                             (_val), (_mask))

/* AN lp np 1 (Addr 0019h) */
#define READ_CL73_IEEEB1_LP_NP1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000010, (_val))
#define WRITE_CL73_IEEEB1_LP_NP1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000010, (_val))
#define MODIFY_CL73_IEEEB1_LP_NP1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000010, \
                             (_val), (_mask))

/* AN lp np 2 (Addr 001Ah) */
#define READ_CL73_IEEEB1_LP_NP2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x3800001A, (_val))
#define WRITE_CL73_IEEEB1_LP_NP2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x3800001A, (_val))
#define MODIFY_CL73_IEEEB1_LP_NP2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x3800001A, \
                             (_val), (_mask))

/* AN lp np 3 (Addr 001Bh) */
#define READ_CL73_IEEEB1_LP_NP3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x3800001B, (_val))
#define WRITE_CL73_IEEEB1_LP_NP3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x3800001B, (_val))
#define MODIFY_CL73_IEEEB1_LP_NP3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x3800001B, \
                             (_val), (_mask))

/*******************/
/* CL73 IEEEB3 Blk */
/*******************/
/* BPEther Stat (Addr 0030h) */
#define READ_CL73_IEEEB3_BPETHER_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x38000030, (_val))
#define WRITE_CL73_IEEEB3_BPETHER_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x38000030, (_val))
#define MODIFY_CL73_IEEEB3_BPETHER_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x38000030, \
                             (_val), (_mask))


/* SerDes Digital Block */
/* 1000X Control 1 Register */ 
#define CTRL1_FIBER_MODE                         (1 << 0)
#define CTRL1_TBI_INTERFACE                      (1 << 1)
#define CTRL1_SIGNAL_DETECT_EN                   (1 << 2)
#define CTRL1_INVERT_SIGNAL_DETECT               (1 << 3)
#define CTRL1_AUTODET_EN                         (1 << 4)
#define CTRL1_SGMII_MASTER_MODE                  (1 << 5)
#define CTRL1_DISABLE_PLL_PWRDWN                 (1 << 6)
#define CTRL1_CRC_CHECKER_DISABLE                (1 << 7)
#define CTRL1_COMMA_DET_EN                       (1 << 8)
#define CTRL1_ZERO_COMMA_DETECTOR_PHASE          (1 << 9)
#define CTRL1_REMOTE_LOOPBACK                    (1 << 10)
#define CTRL1_SEL_RX_PKTS_FOR_CNTR               (1 << 11)
#define CTRL1_SERDES_TX_AMPLITUDE_OVERRIDE       (1 << 12)
#define CTRL1_MASTER_MDIO_PHY_SEL                (1 << 13)
#define CTRL1_DISABLE_SIGNAL_DETECT_FILTER       (1 << 14)

/* 1000X Control 2 Register */
#define CTRL2_ENABLE_PARALLEL_DETECT             (1 << 0)
#define CTRL2_DISABLE_FALSE_LINK                 (1 << 1)
#define CTRL2_FILTER_FORCE_LINK                  (1 << 2)
#define CTRL2_ENABLE_AUTONEG_ERR_TIMER           (1 << 3)
#define CTRL2_DISABLE_REMOTE_ERR_TIMER           (1 << 4)
#define CTRL2_FORCE_XMIT_DATA_ON_TXSIDE          (1 << 5)
#define CTRL2_AUTONEG_FAST_TIMERS                (1 << 6)
#define CTRL2_DISABLE_CARRIER_EXTEND             (1 << 7)
#define CTRL2_DISABLE_TRRRR_GENERATION           (1 << 8)
#define CTRL2_BYPASS_PCS_RX                      (1 << 9)
#define CTRL2_BYPASS_PCS_TX                      (1 << 10)
#define CTRL2_TEST_CNTR                          (1 << 11)
#define CTRL2_TRANSMIT_PACKET_SEQ_TEST           (1 << 12)
#define CTRL2_TRANSMIT_IDLEJAM_SEQ_TEST          (1 << 13)
#define CTRL2_CLEAR_BER_COUNT                    (1 << 14)
#define CTRL2_DISABLE_EXTEND_FDX_ONLY            (1 << 15)

/* 1000X Control 3 Register */
#define CTRL3_TX_FIFO_RST                        (1 << 0)
#define CTRL3_FIFO_ELASTICITY_TX_RX              (3 << 1)
#define CTRL3_EARLY_PREAMBLE_TX                  (1 << 3)
#define CTRL3_EARLY_PREAMBLE_RX                  (1 << 4)
#define CTRL3_FREQ_LOCK_ELASTICITY_RX             (1 << 5)
#define CTRL3_FREQ_LOCK_ELASTICITY_TX            (1 << 6)
#define CTRL3_BYPASS_TXFIFO100                   (1 << 7)
#define CTRL3_FORCE_FIFO_ON                      (1 << 8)
#define CTRL3_BLOCK_TXEN_MODE                    (1 << 9)
#define CTRL3_JAM_FALSE_CARRIER_MODE             (1 << 10)
#define CTRL3_EXT_PHY_CRS_MODE                   (1 << 11)
#define CTRL3_INVERT_EXT_PHY_CRS                 (1 << 12)
#define CTRL3_DISABLE_TX_CRS                     (1 << 13)
#define CTRL3_RXFIFO_GMII_RESET                  (1 << 14)
#define CTRL3_DISABLE_PACKET_MISALIGN            (1 << 15)

/* 1000X Control 4 Register */
#define CTRL4_MISC_RX_STAT_SEL                   (7 << 0)
#define CTRL4_NP_COUNT_CLRNRD                    (1 << 3)
#define CTRL4_NP_COUNT_CLRNBP                    (1 << 4)
#define CTRL4_LP_NEXT_PAGE_SEL                   (1 << 5)
#define CTRL4_LINK_FORCE                         (1 << 7) 
#define CTRL4_LATCH_LINKDOWN_ENABLE              (1 << 8)
#define CTRL4_CLEAR_LINKDOWN                     (1 << 9)
#define CTRL4_ZERO_RXDGMII                       (1 << 10)
#define CTRL4_TX_CONFIG_REG_SEL                  (1 << 11)
#define CTRL4_ENABLE_LAST_RESOLUTION_ERR         (1 << 12)
#define CTRL4_DISABLE_RESOLUTION_ERR_RESTART     (1 << 13)

/* 1000X STATUS 1 Register */
#define STAT1_SGMII_MODE                         (1 << 0)
#define STAT1_LINK_STATUS                        (1 << 1)
#define STAT1_DUPLEX_STATUS                      (1 << 2)
#define STAT1_SPEED_STATUS                       (3 << 3)
#define STAT1_SPEED_10MB                         (0 << 3)
#define STAT1_SPEED_100MB                        (1 << 3)
#define STAT1_SPEED_1000MB                       (2 << 3)
#define STAT1_SPEED_2P5GB                        (3 << 3)
#define STAT1_PAUSE_RESOLUTION_RXSIDE            (1 << 5)
#define STAT1_PAUSE_RESOLUTION_TXSIDE            (1 << 6)
#define STAT1_LINK_CHANGED                       (1 << 7)
#define STAT1_EARLY_END_EXTENSION_DETECTED       (1 << 8)
#define STAT1_CARRIER_EXTEND_ERR_DETECTED        (1 << 9)
#define STAT1_RX_ERR_DETECTED                    (1 << 10)
#define STAT1_TX_ERR_DETECTED                    (1 << 11)
#define STAT1_CRC_ERR_DETECTED                   (1 << 12)
#define STAT1_FALSE_CARRIER_DETECTED             (1 << 13)
#define STAT1_RXFIFO_ERR_DETECTED                (1 << 14)
#define STAT1_TXFIFO_ERR_DETECTED                (1 << 15)

/* 1000X STATUS 2 Register */
#define STAT2_AN_ENABLE_STATE                    (1 << 0)
#define STAT2_AN_ERROR_STATE                     (1 << 1)
#define STAT2_ABILITY_DETECT_STATE               (1 << 2)
#define STAT2_ACKNOWLEDGE_DETECT_STATE           (1 << 3)
#define STAT2_COMPLETE_ACKNOWLEDGE_STATE         (1 << 4)
#define STAT2_IDLE_DETECT_STATE                  (1 << 5)
#define STAT2_LINK_DOWN_LOSS_SYNC                (1 << 6)
#define STAT2_RUDI_INVALID                       (1 << 7)
#define STAT2_RUDI_L                             (1 << 8)
#define STAT2_RUDI_C                             (1 << 9)
#define STAT2_SYNC_STATUS_OK                     (1 << 10)
#define STAT2_SYNC_STATUS_FAIL                   (1 << 11)
#define STAT2_SGMII_SELECTOR_MISMATCH            (1 << 12)
#define STAT2_AUTONEG_RESOLUTION_ERR             (1 << 13)
#define STAT2_CONSISTENCY_MISTMATCH              (1 << 14)
#define STAT2_SGMII_MODE_CHANGE                  (1 << 15)

/* 1000X STATUS 3 Register */
#define STAT3_SD_FILTER_CHG	                 (1 << 7)
#define STAT3_SD_MUX                             (1 << 8)
#define STAT3_SD_FILTER                          (1 << 9)
#define STAT3_LATCH_LINKDOWN                     (1 << 10)
#define STAT3_REMOTE_PHY_AUTOSEL                 (1 << 11)
#define STAT3_PD_PARK_AN                         (1 << 12)

/* CRC Error/RX Packet Counter Register */
#define COUNTER_CRC_ERR                          (0xFF << 0)
#define COUNTER_RX_PKT                           (0xFF << 0)
#define COUNTER_BIT_ERR_RATE                     (0xFF << 8)

/* MISC 1 Register */
#define MISC1_FORCE_SPEED                        (0xF << 0)
#define MISC1_FORCE_SPEED_2P5GB                  (0 << 0)
#define MISC1_FORCE_SPEED_5GB                    (1 << 0)
#define MISC1_FORCE_SPEED_6GB                    (2 << 0)
#define MISC1_FORCE_SPEED_10GB                   (3 << 0)
#define MISC1_FORCE_SPEED_10GB_CX4               (4 << 0)
#define MISC1_FORCE_SPEED_12GB                   (5 << 0)
#define MISC1_FORCE_SPEED_12P5GB                 (6 << 0)
#define MISC1_FORCE_SPEED_13GB                   (7 << 0)
#define MISC1_FORCE_SPEED_15GB                   (8 << 0)
#define MISC1_FORCE_SPEED_16GB                   (9 << 0)
#define MISC1_FORCE_SPEED_SEL                    (1 << 4)
#define MISC1_FORCE_LN_MODE                      (1 << 5)
#define MISC1_TX_UNDERRUN_1000_DIS               (1 << 6)
#define MISC1_FORCE_TICK0_SW                     (1 << 7)
#define MISC1_FORCE_PLL_MODE_AFE                 (7 << 8)
#define MISC1_FORCE_PLL_MODE_AFE_SEL             (1 << 12)
#define MISC1_REFCLK_SEL                         (7 << 13)

/*
 * Useful constants for XGXS PHY chips
 */

/****************/
/* XGXS Block 0 */
/****************/
/* XGXS Control Register */
#define XGXS_CTRL_TXCKO_DIV                      (1 << 0)
#define XGXS_CTRL_AFRST_EN                       (1 << 1)
#define XGXS_CTRL_EDEN                           (1 << 2)
#define XGXS_CTRL_CDET_EN                        (1 << 3)
#define XGXS_CTRL_MDIO_CONT_EN                   (1 << 4)
#define XGXS_CTRL_HSTL                           (1 << 5)
#define XGXS_CTRL_RLOOP                          (1 << 6)
#define XGXS_CTRL_PLL_BYPASS                     (1 << 7)
#define XGXS_CTRL_MODE_10G                       (0xF << 8)
#define XGXS_CTRL_RESET_ANLG                     (1 << 12)
#define XGXS_CTRL_START_SEQUENCER                (1 << 13)
#define XGXS_CTRL_PCMP_EN                        (1 << 14)
#define XGXS_CTRL_PGEN_EN                        (1 << 15)
 
/*****************/
/* TX All Block */
/*****************/
/* TX Driver Register */
#define TX_DRIVER_ICBUF1T                        (1 << 0)
#define TX_DRIVER_IFULLSPD                       (7 << 1)
#define TX_DRIVER_IPREDRIVER                     (0xF << 4)
#define TX_DRIVER_IDRIVER                        (0xF << 8)
#define TX_DRIVER_PREEMPHASIS                    (0xF << 12)

/*****************/
/* Over 1G Block */
/*****************/
/* UP 1 Register */
#define UP1_ADV_2P5GB                            (1 << 0)
#define UP1_ADV_5GB                              (1 << 1)
#define UP1_ADV_6GB                              (1 << 2)
#define UP1_ADV_10GB                             (1 << 3)
#define UP1_ADV_10GB_CX4                         (1 << 4)
#define UP1_ADV_12GB                             (1 << 5)
#define UP1_ADV_12P5GB                           (1 << 6)
#define UP1_ADV_13GB                             (1 << 7)
#define UP1_ADV_15GB                             (1 << 8)
#define UP1_ADV_16GB                             (1 << 9)

/* LP_UP1 Register */
#define LP_UP1_ADV_2P5GB                            (1 << 0)
#define LP_UP1_ADV_5GB                              (1 << 1)
#define LP_UP1_ADV_6GB                              (1 << 2)
#define LP_UP1_ADV_10GB                             (1 << 3)
#define LP_UP1_ADV_10GB_CX4                         (1 << 4)
#define LP_UP1_ADV_12GB                             (1 << 5)
#define LP_UP1_ADV_12P5GB                           (1 << 6)
#define LP_UP1_ADV_13GB                             (1 << 7)
#define LP_UP1_ADV_15GB                             (1 << 8)
#define LP_UP1_ADV_16GB                             (1 << 9)


/*
 * Useful constants for CL73 autonegotion in XGXS PHY chips
 */

/****************/
/* CL73 UserB0 */
/****************/
/* CL73_UCtrl1 Register */
#define CL73_UCTRL1_FORCE_TX_OMUX_EN                     (1 << 0)
#define CL73_UCTRL1_USTAT1_MUXSEL                        (1 << 1)
#define CL73_UCTRL1_COUPLE_W_CL37_RESTART                (1 << 2)
#define CL73_UCTRL1_COUPLE_W_CL73_RESTART                (1 << 3)
#define CL73_UCTRL1_COUPLE_W_CL73_RESTART_WO_LINK_FAIL   (1 << 4)
#define CL73_UCTRL1_NONCE_MATCH_VAL                      (1 << 5)
#define CL73_UCTRL1_NONCE_MATCH_OVER                     (1 << 6)
#define CL73_UCTRL1_LINK_FAIL_TIMER_QUAL_EN              (1 << 7)
#define CL73_UCTRL1_LINK_FAIL_TIMER_DIS                  (1 << 8)
#define CL73_UCTRL1_LONG_PAR_DET_TIMER_DIS               (1 << 9)
#define CL73_UCTRL1_ALLOW_CL37_AN                        (1 << 10)
#define CL73_UCTRL1_PAR_DET_DIS                          (1 << 11)
#define CL73_UCTRL1_LOSS_OF_SYNC_FAIL_EN                 (1 << 12)

/* CL73_UStat1 Register */
#define CL73_USTAT1_ARB_FSM                              (0x3ff << 0)

/* CL73_BAMCtrl1 Register */
#define CL73_BAMCTRL1_UD_CODE_41_32                      (0x3ff << 0)
#define CL73_BAMCTRL1_TEST_MP5_HALT_STEP                 (1 << 11)
#define CL73_BAMCTRL1_TEST_MP5_HALT_EN                   (1 << 12)
#define CL73_BAMCTRL1_BAMNP_AFTER_BP_EN                  (1 << 13)
#define CL73_BAMCTRL1_STATION_MNGR_EN                    (1 << 14)
#define CL73_BAMCTRL1_BAMEN                              (1 << 15)

/* CL73_BAMCtrl2 Register */
#define CL73_BAMCTRL2_UD_CODE_31_16                      (0xffff) 

/* CL73_BAMCtrl3 Register */ 
#define CL73_BAMCTRL3_USE_CL73_HCD_MR                    (1 << 0)
#define CL73_BAMCTRL3_UD_CODE_15_1                       (0x7fff << 1)

/* CL73_BAMStat1 Register */
#define CL73_BAMSTAT1_LP_UD_CODE_41_32                   (0x3ff << 0)

/* CL73_BAMStat2 Register */
#define CL73_BAMSTAT2_LP_UD_CODE_31_16                   (0xffff)

/* CL73_BAMStat3 Register */
#define CL73_BAMSTAT3_USE_CL73_HCD_LP                    (1 << 0)
#define CL73_BAMSTAT3_LP_UD_CODE_15_1                    (0x7fff << 1)

/*********************/
/* CL73_IEEEB0 Block */
/*********************/
/* CL73 AN Control Register */
#define CL73_AN_CTRL_RES_NEGOTIATION                     (1 << 9)
#define CL73_AN_CTRL_AUTONEG_EN                          (1 << 12)
#define CL73_AN_CTRL_MAIN_RESET                          (1 << 15)

/* CL73 AN Status Register */
#define CL73_AN_STAT_LP_AN_ABLE                          (1 << 0)
#define CL73_AN_STAT_LINK_STAT                           (1 << 2)
#define CL73_AN_STAT_AN_ABILITY                          (1 << 3)
#define CL73_AN_STAT_RF                                  (1 << 4)
#define CL73_AN_STAT_AN_DONE                             (1 << 5)
#define CL73_AN_STAT_PAGE_RECEIVED                       (1 << 6)
#define CL73_AN_STAT_EXT_NP_STAT                         (1 << 7)
#define CL73_AN_STAT_PARALLEL_DETECT_FAULT               (1 << 9)

/* CL73 PHY ID MSB */
/* CL73 PHY ID LSB */
/* IEEE Dev In Pkg 1 */
/* IEEE Dev In Pkg 2 */

/*********************/
/* CL73_IEEEB1 Block */
/*********************/
/* AN_adv1 Register */
#define CL73_AN_ADV1_LD_SELECTOR                         (0x1f << 0)
#define CL73_AN_ADV1_LD_ECHOED_NONCE                     (0x1f << 5)
#define CL73_AN_ADV1_LD_PAUSE                            (7 << 10)
#define CL73_AN_ADV1_LD_RF                               (1 << 13)
#define CL73_AN_ADV1_LD_ACK                              (1 << 14)
#define CL73_AN_ADV1_LD_NP                               (1 << 15)

/* AN_adv2 Register */
#define CL73_AN_ADV2_LD_TX_NONCE                         (0x1f << 0)
#define CL73_AN_ADV2_LD_TECH_ABILITY_10_0                (0x7ff << 5)

/* AN_adv3 Register */
#define CL73_AN_ADV2_LD_TECH_ABILITY_26_11               (0xffff)

/* AN_lp_adv1 Register */
#define CL73_AN_ADV1_LP_SELECTOR                         (0x1f << 0)
#define CL73_AN_ADV1_LP_ECHOED_NONCE                     (0x1f << 5)
#define CL73_AN_ADV1_LP_PAUSE                            (1 << 10)
#define CL73_AN_ADV1_LP_ASYM_PAUSE                       (1 << 11)
#define CL73_AN_ADV1_LP_RF                               (1 << 13)
#define CL73_AN_ADV1_LP_ACK                              (1 << 14)
#define CL73_AN_ADV1_LP_NP                               (1 << 15)

/* AN_lp_adv2 Register */
#define CL73_AN_ADV2_LP_TX_NONCE                         (0x1f << 0)
#define CL73_AN_ADV2_LP_TECH_ABILITY_10_0                (0x7ff << 5)
#define CL73_AN_ADV2_LP_TECH_1G_KX                       (1 << 5)
#define CL73_AN_ADV2_LP_TECH_10G_KX4                     (1 << 6)
#define CL73_AN_ADV2_LP_TECH_10G_KR                      (1 << 7)

/* AN_lp_adv3 Register */
#define CL73_AN_ADV2_LP_TECH_ABILITY_26_11               (0xffff)

/* AN_np1 Register */
/* AN_np2 Register */
/* AN_np3 Register */
/* AN_lp_np1 Register */
/* AN_lp_np2 Register */
/* AN_lp_np3 Register */

/*********************/
/* CL73_IEEEB2 Block */
/*********************/


#endif /* _SERDES_COMBO65_H_ */
