/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        macsecphy.h
 *
 * Header file for MACSEC PHYs
 */
#ifndef SOC_MACSECPHY_H
#define SOC_MACSECPHY_H


#include <soc/phy/phyctrl.h>
#ifdef INCLUDE_MACSEC

#include <bmacsec.h>

typedef bmacsec_dev_addr_t soc_macsec_dev_addr_t;

extern int 
soc_macsecphy_miim_write(soc_macsec_dev_addr_t dev_addr, 
                         uint32 phy_reg_addr, uint16 data);

extern int 
soc_macsecphy_miim_read(soc_macsec_dev_addr_t dev_addr, 
                        uint32 phy_reg_addr, uint16 *data);

/*
 * Create a unique MACSEC portId for specified macsec port
 * connected to unit/port.
 */
#define SOC_MACSEC_PORTID(u, p) (((u) << 16) | ((p) & 0xff))

/*
 * Return port number within BCM unit from macsec portId.
 */
#define SOC_MACSEC_PORTID2UNIT(p)   ((p) >> 16)

/*
 * Return BCM unit number from macsec portId.
 */
#define SOC_MACSEC_PORTID2PORT(p)   ((p) & 0xff)

/*
 * Create MACSEC MDIO address.
 */
#ifdef BCM_SWITCHMACSEC_SUPPORT
/* nophy_macsec(i.e. switch_macsec) at bit 9 as a flag to indicate this 
 * MACSEC of this port is nophy_macsec. 
 *
 * Design for Switch-MACSEC on SOC_MACSECPHY_MDIO_ADDR() is to replace 
 *  'clause45' to 'flags'. This is for the flexible designing purpose and 
 *  compatiable with existing design for the PHY bounded MACSEC(PHY-MACSEC)
 *  solution. 
 *  - the compare between SOC_MACSECPHY_MDIO_ADDR(unit, mdio, flags) and 
 *      SOC_MACSECPHY_MDIO_ADDR(unit, mdio, clause45) are 
 *      1. 'CLAUSE45' information in both definitions are at bit8 in the 
 *          output MDIO_ADDR.
 *      2. 'NOPHY_MACSEC' information is at bit9 and is supported in  
 *          SOC_MACSECPHY_MDIO_ADDR(unit, mdio, flags) only.
 */

#define SOC_MACSECPHY_MDIO_FLAGS_CLAUSE45   (0x1)   /* flag : clause45 */
#define SOC_MACSECPHY_MDIO_FLAGS_NOPHY      (0x2)   /* flag : nophy_macsec */
#define SOC_MACSECPHY_MDIO_FLAGS_MASK   \
        (SOC_MACSECPHY_MDIO_FLAGS_CLAUSE45 | SOC_MACSECPHY_MDIO_FLAGS_NOPHY)
#define SOC_MACSECPHY_MDIO_ADDR(unit, mdio, flags) \
                        ((((unit) & 0xff) << 24)    |       \
                         (((mdio) & 0xff) << 0)     |       \
                         (((flags) & SOC_MACSECPHY_MDIO_FLAGS_MASK) << 8))
#else   /* BCM_SWITCHMACSEC_SUPPORT */
#define SOC_MACSECPHY_MDIO_ADDR(unit, mdio, clause45) \
                        ((((unit) & 0xff) << 24)    |       \
                         (((mdio) & 0x3ff) << 0)     |       \
                         (((clause45) & 0x1) << 11))
#endif  /* BCM_SWITCHMACSEC_SUPPORT */

#define SOC_MACSECPHY_ADDR2UNIT(a)  (((a) >> 24) & 0xff)

#define SOC_MACSECPHY_ADDR2MDIO(a)  ((a) & 0x3ff)

#define SOC_MACSECPHY_ADDR_IS_CLAUSE45(a)  (((a) >> 11) & 1)

#ifdef BCM_SWITCHMACSEC_SUPPORT
#define SOC_MACSECPHY_ADDR_FLAGS_GET(a)  \
        (((a) >> 8) & SOC_MACSECPHY_MDIO_FLAGS_MASK)
#define SOC_MACSECPHY_ADDR_IS_FLAG_CLAUSE45(a)  \
        (SOC_MACSECPHY_ADDR_FLAGS_GET(a) & SOC_MACSECPHY_MDIO_FLAGS_CLAUSE45)
#define SOC_MACSECPHY_ADDR_IS_FLAG_NOPHY(a)  \
        (SOC_MACSECPHY_ADDR_FLAGS_GET(a) & SOC_MACSECPHY_MDIO_FLAGS_NOPHY)



#define SOC_NOPHY_MACSEC_CORE_GET(unit, core, dev_port_cnt)   \
        do {    \
            (core) = BMACSEC_CORE_UNKNOWN; \
            (dev_port_cnt) = 0; \
        } while (0)
#define SOC_NOPHY_MACSEC_MAC_GET(unit, mac) \
        do {    \
            (mac) = BMACSEC_MAC_CORE_UNKNOWN;   \
        } while (0)
#define SOC_NOPHY_MACSEC_MMI_GET(unit, iofn)    \
        do {    \
            (iofn) = NULL; \
        } while (0)
#define SOC_NOPHY_MACSEC_DEVID_GET(unit, port, devid) \
        do {    \
            (devid) = -1; \
        } while (0)

#endif  /* BCM_SWITCHMACSEC_SUPPORT */

/* --- MACSEC device control type --- 
 *
 *  Types defined here are used for soc_macsecphy_dev_control_get()/set(), to 
 *  control whole MACSEC unit. For example, the request to enable or disable 
 *  MACSEC control path, to reset or power-down MACSEC unit.
 */
#define SOC_MACSECPHY_DEV_CONTROL_BYPASS    (0x1)
#define SOC_MACSECPHY_DEV_CONTROL_POWERDOWN (0x2)
#define SOC_MACSECPHY_DEV_CONTROL_RESET     (0x3)

/* --- MACSEC device information type --- 
 *
 *  Types defined here are used for soc_macsecphy_dev_info_get() for the 
 *  purpose to report the MACSEC related information.
 */
#define SOC_MACSECPHY_DEV_INFO_SWITCHMACSEC_ATTACHED    (0x1)


extern int
soc_macsecphy_init(int unit, soc_port_t port, phy_ctrl_t *pc, 
                   bmacsec_core_t core_type, bmacsec_dev_io_f iofn);

#ifdef BCM_SWITCHMACSEC_SUPPORT
extern int 
soc_switchmacsec_init(int unit, soc_port_t port);
#endif  /* BCM_SWITCHMACSEC_SUPPORT */

extern int 
soc_macsecphy_dev_control_set(int unit, soc_port_t port, 
                    int type, uint32 value);
extern int 
soc_macsecphy_dev_control_get(int unit, soc_port_t port, 
                    int type, uint32 *value);
extern int 
soc_macsecphy_dev_info_get(int unit, soc_port_t port, 
                    int type, uint32 *value);

#endif /* INCLUDE_MACSEC */
#endif /* SOC_MACSECPHY_H */

