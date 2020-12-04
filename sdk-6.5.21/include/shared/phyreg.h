/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines common PHY register definition.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define PHY register definition.
 */

#ifndef _SHR_PHYREG_H
#define _SHR_PHYREG_H

#define _SHR_PORT_PHY_PHYMOD_IBLK_FORCE_LANE    0x8000000
#define _SHR_PORT_PHY_PHYMOD_ACCESS_FLAGS_SHIFT 24

#define _SHR_PORT_PHT_PHYMOD_FORCE_LANE_SET(_addr, _lane)  \
    _addr = ((_addr & ~(0x7 << _SHR_PORT_PHY_PHYMOD_ACCESS_FLAGS_SHIFT)) |   \
            _SHR_PORT_PHY_PHYMOD_IBLK_FORCE_LANE |                           \
             ((_lane & 0x7) <<  _SHR_PORT_PHY_PHYMOD_ACCESS_FLAGS_SHIFT))   

#define _SHR_PORT_PHY_SHAD_RETRY_CNT  (50)

#define _SHR_PORT_PHY_INTERNAL        0x00000001
#define _SHR_PORT_PHY_NOMAP           0x00000002
#define _SHR_PORT_PHY_CLAUSE45        0x00000004
#define _SHR_PORT_PHY_I2C_DATA8       0x00000008
#define _SHR_PORT_PHY_I2C_DATA16      0x00000010
#define _SHR_PORT_PHY_PVT_DATA        0x00000020
#define _SHR_PORT_PHY_BROADCAST_WRITE 0x00000040
#define _SHR_PORT_PHY_REG_RDB         0x02000000
#define _SHR_PORT_PHY_REG_MACSEC      0x04000000
#define _SHR_PORT_PHY_REG64_DATA_HI32 0x08000000

#define _SHR_PORT_PHY_CLAUSE45_ADDR(_devad, _regad)     \
            ((((_devad) & 0x3F) << 16) |                \
             ((_regad) & 0xFFFF))
#define _SHR_PORT_PHY_CLAUSE45_DEVAD(_reg_addr)         \
            (((_reg_addr) >> 16) & 0x3F)
#define _SHR_PORT_PHY_CLAUSE45_REGAD(_reg_addr)         \
            ((_reg_addr) & 0xFFFF)
#define _SHR_PORT_PHY_I2C_ADDR(_devad, _regad)          \
            ((((_devad) & 0xFF) << 16) |                \
             ((_regad) & 0xFFFF))
#define _SHR_PORT_PHY_I2C_DEVAD(_reg_addr)              \
            (((_reg_addr) >> 16) & 0xFF)
#define _SHR_PORT_PHY_I2C_REGAD(_reg_addr)              \
            ((_reg_addr) & 0xFFFF)

#define _SHR_PORT_PHY_REG_NO_RETRY_ACCESS   0x08000000
#define _SHR_PORT_PHY_REG_RESERVE_ACCESS    0x20000000
#define _SHR_PORT_PHY_REG_1000X             0x40000000
#define _SHR_PORT_PHY_REG_INDIRECT          0x80000000
#define _SHR_PORT_PHY_REG_DUMMY_ACCESS      0x10000000

#define _SHR_PORT_PHY_REG_FLAGS_MASK  0xFF000000
#define _SHR_PORT_PHY_REG_BANK_MASK   0x0000FFFF
#define _SHR_PORT_PHY_REG_ADDR_MASK   0x000000FF

#define _SHR_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_bank, _reg_addr) \
            ((((_flags) | _SHR_PORT_PHY_REG_INDIRECT) & \
              _SHR_PORT_PHY_REG_FLAGS_MASK) | \
             (((_reg_bank) & _SHR_PORT_PHY_REG_BANK_MASK) << 8) | \
             ((_reg_addr) & _SHR_PORT_PHY_REG_ADDR_MASK))
#define _SHR_PORT_PHY_REG_FLAGS(_reg_addr)                 \
            ((_reg_addr) & _SHR_PORT_PHY_REG_FLAGS_MASK)
#define _SHR_PORT_PHY_REG_BANK(_reg_addr)                 \
            (((_reg_addr) >> 8) & _SHR_PORT_PHY_REG_BANK_MASK) 
#define _SHR_PORT_PHY_REG_ADDR(_reg_addr)                 \
            ((_reg_addr) & _SHR_PORT_PHY_REG_ADDR_MASK)

#endif  /* !_SHR_PHYREG_H */
