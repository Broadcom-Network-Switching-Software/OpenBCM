/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phylocal.h
 * Purpose:     
 */

#ifndef   _PHYLOCAL_H_
#define   _PHYLOCAL_H_

/* microsec */
#define PHY_RESET_TIMEOUT_USEC \
    (SAL_BOOT_SIMULATION ? 10000000 : 10000)

/* Driver functions */
extern int phy_fe_ge_reset(int unit, soc_port_t port, void *user_arg);
extern int phy_fe_ge_enable_get(int unit, soc_port_t port, int *enable);
extern int phy_fe_ge_enable_set(int unit, soc_port_t port, int enable);
extern int phy_fe_init(int unit, soc_port_t port);
extern int phy_ge_init(int unit, soc_port_t port);
extern int phy_fe_ge_link_get(int unit, soc_port_t port, int *link);
extern int phy_fe_ge_duplex_set(int unit, soc_port_t port, int duplex);
extern int phy_fe_ge_duplex_get(int unit, soc_port_t port, int *duplex);
extern int phy_fe_ge_speed_set(int unit, soc_port_t port, int speed);
extern int phy_fe_ge_speed_get(int unit, soc_port_t port, int *speed);
extern int phy_fe_ge_master_set(int unit, soc_port_t port, int master);
extern int phy_fe_ge_master_get(int unit, soc_port_t port, int *master);
extern int phy_fe_ge_an_set(int unit, soc_port_t port, int an);
extern int phy_fe_ge_an_get(int unit, soc_port_t port, int *an,
                            int *an_done);
extern int phy_fe_adv_local_set(int unit, soc_port_t port,
                                soc_port_mode_t mode);
extern int phy_fe_adv_local_get(int unit, soc_port_t port,
                                soc_port_mode_t *mode);
extern int phy_fe_adv_remote_get(int unit, soc_port_t port,
                                 soc_port_mode_t *mode);
extern int phy_ge_adv_local_set(int unit, soc_port_t port,
                                soc_port_mode_t mode);
extern int phy_ge_adv_local_get(int unit, soc_port_t port,
                                soc_port_mode_t *mode);
extern int phy_ge_adv_remote_get(int unit, soc_port_t port,
                                 soc_port_mode_t *mode);
extern int phy_fe_ge_lb_set(int unit, soc_port_t port, int enable);
extern int phy_fe_ge_lb_get(int unit, soc_port_t port, int *enable);
extern int phy_fe_interface_set(int unit, soc_port_t port, soc_port_if_t pif);
extern int phy_fe_interface_get(int unit, soc_port_t port, soc_port_if_t *pif);
extern int phy_ge_interface_set(int unit, soc_port_t port, soc_port_if_t pif);
extern int phy_ge_interface_get(int unit, soc_port_t port, soc_port_if_t *pif);
extern int phy_fe_ability_get(int unit, soc_port_t port,
			      soc_port_mode_t *mode);
extern int phy_ge_ability_get(int unit, soc_port_t port,
			      soc_port_mode_t *mode);
extern int phy_fe_ge_mdix_set(int unit, soc_port_t port, 
                              soc_port_mdix_t mode);
extern int phy_fe_ge_mdix_get(int unit, soc_port_t port, 
                              soc_port_mdix_t *mode);
extern int phy_fe_ge_mdix_status_get(int unit, soc_port_t port, 
                                    soc_port_mdix_status_t *status);
extern int phy_fe_ge_medium_get(int unit, soc_port_t port,
                                soc_port_medium_t *medium);
extern int phy_fe_cable_diag(int unit, soc_port_t port,
                soc_port_cable_diag_t *status);
extern int phy_fe_ge_ability_advert_set(int unit, soc_port_t port,
                soc_port_ability_t *ability);
extern int phy_fe_ge_ability_advert_get(int unit, soc_port_t port,
                soc_port_ability_t *ability);
extern int phy_fe_ge_ability_remote_get(int unit, soc_port_t port,
                soc_port_ability_t *ability);

extern int phy_ge_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data);
extern int phy_ge_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data);
extern int phy_ge_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask);

#define _SOC_PHY_REG_DIRECT \
        ((SOC_PHY_REG_1000X << 1) | (SOC_PHY_REG_1000X >> 1))

/* PHY register access */
#define PHYFEGE_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, _reg_addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_reg_addr), (_val))
#define PHYFEGE_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _reg_bank, _reg_addr, _val) \
            phy_reg_ge_write((_unit), (_phy_ctrl), (_SOC_PHY_REG_DIRECT), \
                             (0), (_reg_addr), (_val))
#define PHYFEGE_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                           _reg_addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_reg_addr), (_val), (_mask))

/* IEEE Standard Registers */
/* 1000BASE-T/100BASE-TX/10BASE-T MII Control Register (Addr 00h) */
#define READ_PHYFEGE_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val))
#define WRITE_PHYFEGE_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val)) 
#define MODIFY_PHYFEGE_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Status Register (ADDR 01h) */
#define READ_PHYFEGE_MII_STATr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define WRITE_PHYFEGE_MII_STATr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define MODIFY_PHYFEGE_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 02h) */
#define READ_PHYFEGE_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define WRITE_PHYFEGE_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define MODIFY_PHYFEGE_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 03h) */
#define READ_PHYFEGE_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define WRITE_PHYFEGE_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define MODIFY_PHYFEGE_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Advertisment Register (ADDR 04h) */
#define READ_PHYFEGE_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define WRITE_PHYFEGE_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define MODIFY_PHYFEGE_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Link Partner Ability (ADDR 05h) */
#define READ_PHYFEGE_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val))
#define WRITE_PHYFEGE_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val)) 
#define MODIFY_PHYFEGE_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Expansion Register (ADDR 06h) */
#define READ_PHYFEGE_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define WRITE_PHYFEGE_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define MODIFY_PHYFEGE_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Next Page Transmit Register (ADDR 07h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Link Partner Received Next Page (ADDR 08h) */

/* 1000BASE-T Control Register  (ADDR 09h) */
#define READ_PHYFEGE_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define WRITE_PHYFEGE_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define MODIFY_PHYFEGE_MII_GB_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/* 1000BASE-T Status Register (ADDR 0ah) */
#define READ_PHYFEGE_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val)) 
#define WRITE_PHYFEGE_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val))
#define MODIFY_PHYFEGE_MII_GB_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, \
                             (_val), (_mask))

/* (Addr 0ch-0eh) Reserved (Do not read/write to reserved registers. */

/* 1000BASE-T/100BASE-TX/10BASE-T IEEE Extended Status Register (ADDR 0fh) */
#define READ_PHYFEGE_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0f, (_val))
#define WRITE_PHYFEGE_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0f, (_val))
#define MODIFY_PHYFEGE_MII_ESRr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0f, \
                             (_val), (_mask))

/* None Standard Registers */
/* Extended Control Register (Addr 0x10) */
#define READ_PHYFEGE_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val))
#define WRITE_PHYFEGE_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val))
#define MODIFY_PHYFEGE_MII_ECRr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* SHA CD Control Register (Addr 0x13) */
/* SHA CD Sel Register (Addr 0x14) */

/* Auxiliary Control/Status Register (Addr 0x18) */
#define READ_PHYFEGE_MII_AUXr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define WRITE_PHYFEGE_MII_AUXr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define MODIFY_PHYFEGE_MII_AUXr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* Auxiliary Status Summary Register (Addr 0x19) */
#define READ_PHYFEGE_MII_ASSRr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x19, (_val))
#define WRITE_PHYFEGE_MII_ASSRr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x19, (_val))
#define MODIFY_PHYFEGE_MII_ASSRr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x19, \
                             (_val), (_mask))

/* SHA Auxiliary Status 2 Register (Addr 0x1b) */

/* General Status Register (BROADCOM) (Addr 0x1c) */
#define READ_PHYFEGE_MII_GSRr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1c, (_val))
#define WRITE_PHYFEGE_MII_GSRr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1c, (_val))
#define MODIFY_PHYFEGE_MII_GSRr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1c, \
                             (_val), (_mask))

/* Master/Slave Seed Register (BROADCOM) (Addr 0x1d) */
#define READ_PHYFEGE_MII_MSSEEDr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1d, (_val))
#define WRITE_PHYFEGE_MII_MSSEEDr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1d, (_val))
#define MODIFY_PHYFEGE_MII_MSSEEDr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1d, \
                             (_val), (_mask))

/* Auxiliary Multiple PHY (BROADCOM) (Addr 0x1e) */
#define READ_PHYFEGE_MII_AUX_MULTI_PHYr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define WRITE_PHYFEGE_MII_AUX_MULTI_PHYr(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define MODIFY_PHYFEGE_MII_AUX_MULTI_PHYr(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                             (_val), (_mask))

/* Test 2 Register (Addr 0x1f) */
#define READ_PHYFEGE_MII_TEST2r(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1f, (_val))
#define WRITE_PHYFEGE_MII_TEST2r(_unit, _phy_ctrl, _val) \
            PHYFEGE_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1f, (_val))
#define MODIFY_PHYFEGE_MII_TEST2r(_unit, _phy_ctrl, _val, _mask) \
            PHYFEGE_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1f, \
                             (_val), (_mask))

#endif /* _PHYFEGE_H_ */
