/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        fcmapphy.h
 *
 * Header file for FCMAP PHYs
 */
#ifndef SOC_FCMAPPHY_H
#define SOC_FCMAPPHY_H


#include <soc/phy/phyctrl.h>
#ifdef INCLUDE_FCMAP

#include <bfcmap.h>

typedef bfcmap_dev_addr_t soc_fcmap_dev_addr_t;

extern int 
soc_fcmapphy_miim_write(soc_fcmap_dev_addr_t dev_addr, 
                         uint32 phy_reg_addr, uint16 data);

extern int 
soc_fcmapphy_miim_read(soc_fcmap_dev_addr_t dev_addr, 
                        uint32 phy_reg_addr, uint16 *data);

/*
 * Create a unique FCMAP portId for specified fcmap port
 * connected to unit/port.
 */
#define SOC_FCMAP_PORTID(u, p) (((u) << 16) | ((p) & 0xff))

/*
 * Return port number within BCM unit from fcmap portId.
 */
#define SOC_FCMAP_PORTID2UNIT(p)   ((p) >> 16)

/*
 * Return BCM unit number from fcmap portId.
 */
#define SOC_FCMAP_PORTID2PORT(p)   ((p) & 0xff)

/*
 * Create FCMAP MDIO address.
 */
#define SOC_FCMAPPHY_MDIO_ADDR(unit, mdio, clause45) \
                        ((((unit) & 0xff) << 24)    |       \
                         (((mdio) & 0xff) << 0)     |       \
                         (((clause45) & 0x1) << 8))

#define SOC_FCMAPPHY_ADDR2UNIT(a)  (((a) >> 24) & 0xff)

#define SOC_FCMAPPHY_ADDR2MDIO(a)  ((a) & 0xff)

#define SOC_FCMAPPHY_ADDR_IS_CLAUSE45(a)  (((a) >> 8) & 1)

extern int
soc_fcmapphy_init(int unit, soc_port_t port, phy_ctrl_t *pc, 
                   bfcmap_core_t core_type, bfcmap_dev_io_f iofn);
extern int
soc_fcmapphy_uninit(int unit, soc_port_t port);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
soc_fcmapphy_blmi_enable(int enable);
#endif


#endif /* INCLUDE_FCMAP */
#endif /* SOC_FCMAPPHY_H */

