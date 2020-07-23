/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdes.h
 * Purpose:     Defines common PHY driver routines for Broadcom XGXS core.
 */
#ifndef _PHY_SERDES_H
#define _PHY_SERDES_H

extern int
phy_serdes_link_get(int unit, soc_port_t port, int *link);

extern int
phy_serdes_duplex_set(int unit, soc_port_t port, int duplex);

extern int
phy_serdes_an_get(int unit, soc_port_t port, int *an, int *an_done);

extern int
phy_serdes_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode); 

extern int
phy_serdes_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode); 

extern int
phy_serdes_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode); 

extern int
phy_serdes_lb_set(int unit, soc_port_t port, int enable);

extern int
phy_serdes_lb_get(int unit, soc_port_t port, int *enable);

extern int
phy_serdes_interface_set(int unit, soc_port_t port, soc_port_if_t pif);

extern int
phy_serdes_interface_get(int unit, soc_port_t port, soc_port_if_t *pif);

extern int
phy_serdes_medium_config_get(int unit, soc_port_t port,
                             soc_port_medium_t medium,
                             soc_phy_config_t *cfg);

extern int
phy_serdes_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium);

extern int
phy_serdes_master_set(int unit, soc_port_t port, int master);

extern int
phy_serdes_master_get(int unit, soc_port_t port, int *master);

extern int
phy_serdes_reg_read(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 *phy_data);

extern int
phy_serdes_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data);

extern int
phy_serdes_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask);

/* SerDes IEEE Registers */

/* SerDes register access */
#define SERDES_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_serdes_read((_unit), (_phy_ctrl), (_reg_bank), \
                            (_reg_addr), (_val))
#define SERDES_REG_WRITE(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_serdes_write((_unit), (_phy_ctrl), (_reg_bank), \
                             (_reg_addr), (_val))
#define SERDES_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                            _reg_addr, _val, _mask) \
            phy_reg_serdes_modify((_unit), (_phy_ctrl), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))


/* MII Control (Addr 00h) */
#define READ_SERDES_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x00, (_val))
#define WRITE_SERDES_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x00, (_val))
#define MODIFY_SERDES_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* MII Status (Addr 01h) */
#define READ_SERDES_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x01, (_val))
#define WRITE_SERDES_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x01, (_val))
#define MODIFY_SERDES_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* PHY ID 0 (Addr 02h) */
#define READ_SERDES_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x02, (_val))
#define WRITE_SERDES_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x02, (_val))
#define MODIFY_SERDES_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* PHY ID 1 (Addr 03h) */
#define READ_SERDES_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x03, (_val))
#define WRITE_SERDES_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x03, (_val))
#define MODIFY_SERDES_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* Autoneg Adv (Addr 04h) */
#define READ_SERDES_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x04, (_val))
#define WRITE_SERDES_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x04, (_val))
#define MODIFY_SERDES_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* Autoneg Link Partner Ability (Addr 05h) */
#define READ_SERDES_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                   0x00, 0x0000, 0x05, (_val))
#define WRITE_SERDES_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                   0x00, 0x0000, 0x05, (_val))
#define MODIFY_SERDES_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* Autoneg Expansion (Addr 06h) */
#define READ_SERDES_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x06, (_val))
#define WRITE_SERDES_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x06, (_val))
#define MODIFY_SERDES_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* Autoneg Next Page (Addr 07h) */
/* Autoneg Link Partner Ability 2 (Addr 08h) */

/* Extended Status (Addr 0fh) */
#define READ_SERDES_MII_EXT_STATr(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x0f, (_val))
#define WRITE_SERDES_MII_EXT_STATr(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x09, (_val))
#define MODIFY_SERDES_MII_EXT_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/*************************/
/* Digital Block (Blk 0) */
/*************************/
/* 1000X Control 1 (Addr 10h) */
#define READ_SERDES_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x10, (_val))
#define WRITE_SERDES_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x10, (_val))
#define MODIFY_SERDES_1000X_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000X Control 2 (Addr 11h) */
#define READ_SERDES_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x11, (_val))
#define WRITE_SERDES_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x11, (_val))
#define MODIFY_SERDES_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))
/* 1000X Control 3 (Addr 12h) */
#define READ_SERDES_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x12, (_val))
#define WRITE_SERDES_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x12, (_val))
#define MODIFY_SERDES_1000X_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x12, \
                             (_val), (_mask))

/* 1000X Control 4 (Addr 13h) */
#define READ_SERDES_1000X_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x13, (_val))
#define WRITE_SERDES_1000X_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x13, (_val))
#define MODIFY_SERDES_1000X_CTRL4r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x13, \
                             (_val), (_mask))

/* 1000X Status 1 (Addr 14h) */
#define READ_SERDES_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x14, (_val))
#define WRITE_SERDES_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x14, (_val))
#define MODIFY_SERDES_1000X_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x14, \
                             (_val), (_mask))

/* 1000X Status 2 (Addr 15h) */
#define READ_SERDES_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x15, (_val))
#define WRITE_SERDES_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x15, (_val))
#define MODIFY_SERDES_1000X_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x15, \
                             (_val), (_mask))

/* 1000X Status 3 (Addr 16h) */
#define READ_SERDES_1000X_STAT3r(_unit, _phy_ctrl, _val) \
            SERDES_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x16, (_val))
#define WRITE_SERDES_1000X_STAT3r(_unit, _phy_ctrl, _val) \
            SERDES_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x16, (_val))
#define MODIFY_SERDES_1000X_STAT3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x16, \

/* Reigster Fields */
#define SERDES_1000X_STAT1_SGMII_MODE        (1 << 0)
#define SERDES_1000X_CTRL2_PAR_DET_EN        (1 << 0)
#endif /* _PHY_SERDES_H */

