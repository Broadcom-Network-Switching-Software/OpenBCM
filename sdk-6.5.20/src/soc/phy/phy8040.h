/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8040.h
 */

#ifndef   _phy8040_H_
#define   _phy8040_H_

#include <soc/phy.h>

/*
 * By default assume ports 0 <--> 1 are connected.
 */
#define PHY8040_DFLT_SWITCH_PORT    1
#define PHY8040_DFLT_MUX_PORT0      0
#define PHY8040_DFLT_MUX_PORT1      3

/*
 * Pick the same phy address as top level but different dev address 
 * for XGXS blocks.
 */
#define PHY8040_C45_DEV_TOPLVL     0x1
#define PHY8040_C45_DEV_XGXS0      0x3
#define PHY8040_C45_DEV_XGXS1      0x4
#define PHY8040_C45_DEV_XGXS2      0x6
#define PHY8040_C45_DEV_XGXS3      0x7

#define PHY8040_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8040_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8040_REG_MODIFY(_unit, _phy_ctrl, _addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val), (_mask))

/* Power down XGXS blocks (Dev Addr 1) */
#define PHY8040_PWRCTRL_RSVD      0x7830 /* only last nibble is writeable */
#define READ_PHY8040_PWR_CTLr(_unit, _phy_ctrl, _val) \
            PHY8040_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8040_C45_DEV_TOPLVL, 0x8001), (_val))
#define WRITE_PHY8040_PWR_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8040_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8040_C45_DEV_TOPLVL, 0x8001), (_val))

/* XGXS {_xgxs=0,1,2,3} Control Register (Addr 0000h) */
#define PHY8040_XGXS_CTRL_SRC(_mux)    (((_mux) & 0xf)<<12)
#define PHY8040_XGXS_CTRL_MDMX         (1<<11)
#define PHY8040_XGXS_CTRL_C45          (1<<10)
#define PHY8040_XGXS_CTRL_DEVID(_id)   (((_id) & 0x1f)<<5)
#define PHY8040_XGXS_CTRL_PRTAD(_ad)   ((_ad) & 0x1f)

#define READ_PHY8040_XGXSn_CTRLr(_unit, _phy_ctrl, _xgxs,_val) \
            PHY8040_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8040_C45_DEV_TOPLVL, (0x8010|(_xgxs))))
#define WRITE_PHY8040_XGXSn_CTRLr(_unit, _phy_ctrl, _xgxs, _val) \
            PHY8040_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8040_C45_DEV_TOPLVL, (0x8010|(_xgxs))), (_val))
#define MODIFY_PHY8040_XGXSn_CTRLr(_unit, _phy_ctrl, _xgxs, _val, _mask) \
            PHY8040_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8040_C45_DEV_TOPLVL, (0x8010|(_xgxs))), (_val), (_mask))

/* Generic XGXSn port/phy register reg access */
#define PHY8040_PRE_EMPHASIS_REG    0x80A7
#define PHY8040_PRE_EMPHASIS(_P)    (((_P) & 0xf) << 12)
#define PHY8040_DRIVER_STRENGTH(_C) (((_C) & 0xf) << 8)

#define PHY8040_LANE_STATUS_REG    0x0018

#define READ_PHY8040_XGXSn_REG(_unit, _phy_ctrl, _devid, _reg, _val)          \
            PHY8040_REG_READ((_unit), (_phy_ctrl),                            \
             SOC_PHY_CLAUSE45_ADDR((_devid), (_reg)), (_val))
#define WRITE_PHY8040_XGXSn_REG(_unit, _phy_ctrl, _devid, _reg, _val)         \
            PHY8040_REG_WRITE((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR((_devid), (_reg)), (_val))
#define MODIFY_PHY8040_XGXSn_REG(_unit, _phy_ctrl, _devid, _reg, _val, _mask) \
            PHY8040_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR((_devid), (_reg)), (_val), (_mask))

/* user registers */
#define PHY8040_XGXS_CTRL_REG       0x8000
#define PHY8040_LANE_CTRL1_REG      0x8016
#define PHY8040_LANE_CTRL2_REG      0x8017
#define PHY8040_TX0A_CTRL1_REG      0x80A7
#define PHY8040_RX_CTRL_REG         0x80F1
#define PHY8040_RX_STATUS_REG       0x80b0

/* RX status register */
#define PHY8040_1G_LINKUP_MASK      ((1 << 15) | (1 << 12))

#endif  /* _phy8705_H_ */
