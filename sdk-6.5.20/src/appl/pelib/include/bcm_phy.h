/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bcm_phy.h
 */

#ifndef   _BCM_PHY_H_
#define   _BCM_PHY_H_

#define montreal2_RTS_MHOST_0_CR5_RST_CTRLr 0x00280000
#define montreal2_RTS_MHOST_1_CR5_RST_CTRLr 0x00380000
#define montreal2_RTS_MHOST_2_CR5_RST_CTRLr 0x00480000
#define montreal2_RTS_MHOST_3_CR5_RST_CTRLr 0x00580000

#define montreal2_TOP_SOFT_RESET_REGr 0x02000100
#define montreal2_TOP_SOFT_RESET_REG_2r 0x02000200

#define montreal2_TOP_MISC_CONTROL2r 0x02000500
#define montreal2_TOP_MISC_CONTROL3r 0x02000600

#define montreal2_TOP_PVTMON_CTRL_0r 0x02002400
#define montreal2_TOP_PVTMON_CTRL_1r 0x02002500
#define montreal2_TOP_TMON_CTRL_0r 0x02002600
#define montreal2_TOP_PVTMON_RESULT_0r 0x02002700
#define montreal2_TOP_PVTMON_RESULT_1r 0x02002800
#define montreal2_TOP_PVTMON_RESULT_2r 0x02002900
#define montreal2_TOP_TMON_RESULTr 0x02002a00

#define montreal2_TOP_RTS_MISC_CTRLr  0x02006f00

#define montreal2_AXI_CONTROL_F0r 0x02804600
#define montreal2_AXI_CONTROL_F1r 0x02806600

#define montreal2_AXI_STATUS_F0r 0x02804700
#define montreal2_AXI_STATUS_F1r 0x02806700

#define montreal2_MISC_CONTROLr 0x02808000

#define montreal2_AXI_ADDR_LOWER_F0r 0x02804100
#define montreal2_AXI_ADDR_LOWER_F1r 0x02806100
#define montreal2_AXI_ADDR_UPPER_F0r 0x02804000
#define montreal2_AXI_ADDR_UPPER_F1r 0x02806000

#define montreal2_AXI_DATA_BYTES_7_6_F0r 0x02804500
#define montreal2_AXI_DATA_BYTES_5_4_F0r 0x02804400
#define montreal2_AXI_DATA_BYTES_3_2_F0r 0x02804300
#define montreal2_AXI_DATA_BYTES_1_0_F0r 0x02804200

#define montreal2_AXI_DATA_BYTES_7_6_F1r 0x02806500
#define montreal2_AXI_DATA_BYTES_5_4_F1r 0x02806400
#define montreal2_AXI_DATA_BYTES_3_2_F1r 0x02806300
#define montreal2_AXI_DATA_BYTES_1_0_F1r 0x02806200

#define montreal2_SBUS_DATA_BYTES_15_14r 0x02802b00
#define montreal2_SBUS_DATA_BYTES_13_12r 0x02802a00
#define montreal2_SBUS_DATA_BYTES_11_10r 0x02802900
#define montreal2_SBUS_DATA_BYTES_9_8r 0x02802800
#define montreal2_SBUS_DATA_BYTES_7_6r 0x02802700
#define montreal2_SBUS_DATA_BYTES_5_4r 0x02802600
#define montreal2_SBUS_DATA_BYTES_3_2r 0x02802500
#define montreal2_SBUS_DATA_BYTES_1_0r 0x02802400

#define montreal2_SBUS_COMMAND_UPPERr 0x02802200
#define montreal2_SBUS_COMMAND_LOWERr 0x02802300
#define montreal2_SBUS_ADDR_UPPERr 0x02802000
#define montreal2_SBUS_ADDR_LOWERr 0x02802100

#define montreal2_SBUS_STATUSr 0x02802d00
#define montreal2_SBUS_CONTROLr 0x02802c00

#define montreal2_SW_TO_FW_INTERRUPTr 0x00808900
#define montreal2_FW_TO_SW_MESSAGEr   0x00808c00

#define SCHAN_MAX_POLLS  1000
#define AXI_MAX_POLLS    1000

#define SBUS_CTRL_GO     0x8000
#define SBUS_STATUS_DONE 0x8000
#define SBUS_STATUS_ERR  0x4000
#define SBUS_STATUS_NAK  0x0100

/* Opcodes */
#define SBUS_READ_MEMORY_CMD_MSG           0x07
#define SBUS_READ_MEMORY_ACK_MSG           0x08
#define SBUS_WRITE_MEMORY_CMD_MSG          0x09
#define SBUS_WRITE_MEMORY_ACK_MSG          0x0a
#define SBUS_READ_REGISTER_CMD_MSG         0x0b
#define SBUS_READ_REGISTER_ACK_MSG         0x0c
#define SBUS_WRITE_REGISTER_CMD_MSG        0x0d
#define SBUS_WRITE_REGISTER_ACK_MSG        0x0e

#define montreal2_CMIC_BLOCK            32

/* Additional block types */
#define montreal2_BLKTYPE_AVS           0
#define montreal2_BLKTYPE_CMIC          1
#define montreal2_BLKTYPE_FCMAC         2
#define montreal2_BLKTYPE_LINESIDE_CL_PM 3
#define montreal2_BLKTYPE_LINE_FCPORT   4
#define montreal2_BLKTYPE_MAPPER        5
#define montreal2_BLKTYPE_MDIO          6
#define montreal2_BLKTYPE_PSM           7
#define montreal2_BLKTYPE_SYSSIDE_CL_PM 8
#define montreal2_BLKTYPE_SYS_FCPORT    9
/* #define montreal2_BLKTYPE_TOP           10 */
#define montreal2_BLKTYPE_TOP           34
#define montreal2_BLKTYPE_UCSS          11

#define PHY8806X_C45_DEV1           0x1
 
#define PHY8806X_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8806X_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8806X_REG_MODIFY(_unit, _phy_ctrl, _addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val), (_mask))

#define READ_PHY8806X_DEV1_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_READ((_unit), (_phy_ctrl), (_reg), (_val))
#define WRITE_PHY8806X_DEV1_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_WRITE((_unit), (_phy_ctrl), (_reg), (_val))
#define MODIFY_PHY8806X_DEV1_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
            PHY8806X_REG_MODIFY((_unit), (_phy_ctrl), (_reg), (_val), (_mask))

#define CDK_DEV_READ32(_u, addr, pval)  (*(pval) = *((uint32 *)addr))
#define CDK_DEV_WRITE32(_u, addr, val)  (*((uint32 *)addr) = val)

/* PHY Identifier Register 0 (Addr 0002h) */
#define READ_PHY8806X_DEV1_ID0r(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_DEV1_REG((_unit), (_phy_ctrl), 0x0002,(_val))

/* PHY Identifier Register 1 (Addr 0003h) */
#define READ_PHY8806X_DEV1_ID1r(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_DEV1_REG((_unit), (_phy_ctrl), 0x0003,(_val))

extern int _bcm_toplvl_sbus_write(int unit, phy_ctrl_t *pc, uint32 reg, uint32 arg[], int size);
extern int _bcm_toplvl_sbus_read(int unit, phy_ctrl_t *pc, uint32 reg, uint32 arg[], int size);

#define READ_TOP_SOFT_RESET_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_SOFT_RESET_REGr, (_val), 1)

#define WRITE_TOP_SOFT_RESET_REG(_unit, _phy_ctrl, _val) \
    { \
        uint32 temp = (_val); \
        int rv; \
        rv = _bcm_toplvl_sbus_write((unit), (_phy_ctrl), montreal2_TOP_SOFT_RESET_REGr, &temp, 1); \
        rv; \
    }

#define READ_TOP_SOFT_RESET_REG_2(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_SOFT_RESET_REG_2r, (_val), 1)

#define WRITE_TOP_SOFT_RESET_REG_2(_unit, _phy_ctrl, _val) \
    { \
        uint32 temp = (_val); \
        int rv; \
        rv = _bcm_toplvl_sbus_write((unit), (_phy_ctrl), montreal2_TOP_SOFT_RESET_REG_2r, &temp, 1); \
        rv; \
    }

#define READ_TOP_MISC_CONTROL2_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_MISC_CONTROL2r, (_val), 1)

#define WRITE_TOP_MISC_CONTROL2_REG(_unit, _phy_ctrl, _val) \
    { \
        uint32 temp = (_val); \
        int rv; \
        rv = _bcm_toplvl_sbus_write((unit), (_phy_ctrl), montreal2_TOP_MISC_CONTROL2r, &temp, 1); \
        rv; \
    }

#define READ_TOP_MISC_CONTROL3_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_MISC_CONTROL3r, (_val), 1)

#define WRITE_TOP_MISC_CONTROL3_REG(_unit, _phy_ctrl, _val) \
    { \
        uint32 temp = (_val); \
        int rv; \
        rv = _bcm_toplvl_sbus_write((unit), (_phy_ctrl), montreal2_TOP_MISC_CONTROL3r, &temp, 1); \
        rv; \
    }

#define READ_TOP_RTS_MISC_CTRL_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_RTS_MISC_CTRLr, (_val), 1)

#define WRITE_TOP_RTS_MISC_CTRL_REG(_unit, _phy_ctrl, _val) \
    { \
        uint32 temp = (_val); \
        int rv; \
        rv = _bcm_toplvl_sbus_write((unit), (_phy_ctrl), montreal2_TOP_RTS_MISC_CTRLr, &temp, 1); \
        rv; \
    }

#define EXTRACT_REG(x) (((x) >> 8) & 0xffff)

#define READ_PHY8806X_MDIO_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_READ((_unit), (_phy_ctrl), (_reg), (_val))
#define WRITE_PHY8806X_MDIO_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_WRITE((_unit), (_phy_ctrl), (_reg), (_val))
#define MODIFY_PHY8806X_MDIO_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
            PHY8806X_REG_MODIFY((_unit), (_phy_ctrl), (_reg), (_val), (_mask))

#if 0
#define READ_PHY8806X_MDIO_REG_DF(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_READ((_unit), (_phy_ctrl), (_reg), (_val))
#define WRITE_PHY8806X_MDIO_REG_DF(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_WRITE((_unit), (_phy_ctrl), (_reg), (_val))
#endif

#define READ_PHY8806X_SBUS_STATUS_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_STATUSr), (_val))
#define WRITE_PHY8806X_SBUS_STATUS_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_STATUSr), (_val))
#define MODIFY_PHY8806X_SBUS_STATUS_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_STATUSr), (_val), (_mask))

#define READ_PHY8806X_SBUS_CONTROL_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_CONTROLr), (_val))
#define WRITE_PHY8806X_SBUS_CONTROL_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_CONTROLr), (_val))
#define MODIFY_PHY8806X_SBUS_CONTROL_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_CONTROLr), (_val), (_mask))

#define READ_PHY8806X_SBUS_COMMAND_UPPER_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_UPPERr), (_val))
#define WRITE_PHY8806X_SBUS_COMMAND_UPPER_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_UPPERr), (_val))
#define MODIFY_PHY8806X_SBUS_COMMAND_UPPER_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_UPPERr), (_val), (_mask))

#define READ_PHY8806X_SBUS_COMMAND_LOWER_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_LOWERr), (_val))
#define WRITE_PHY8806X_SBUS_COMMAND_LOWER_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_LOWERr), (_val))
#define MODIFY_PHY8806X_SBUS_COMMAND_LOWER_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_LOWERr), (_val), (_mask))

#define READ_PHY8806X_SBUS_ADDR_UPPER_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_UPPERr), (_val))
#define WRITE_PHY8806X_SBUS_ADDR_UPPER_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_UPPERr), (_val))
#define MODIFY_PHY8806X_SBUS_ADDR_UPPER_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_UPPERr), (_val), (_mask))

#define READ_PHY8806X_SBUS_ADDR_LOWER_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_LOWERr), (_val))
#define WRITE_PHY8806X_SBUS_ADDR_LOWER_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_LOWERr), (_val))
#define MODIFY_PHY8806X_SBUS_ADDR_LOWER_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_LOWERr), (_val), (_mask))

#define READ_PHY8806X_AXI_CONTROL_F0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F0r), (_val))
#define WRITE_PHY8806X_AXI_CONTROL_F0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F0r), (_val))
#define MODIFY_PHY8806X_AXI_CONTROL_F0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F0r), (_val), (_mask))

#define READ_PHY8806X_AXI_CONTROL_F1_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F1r), (_val))
#define WRITE_PHY8806X_AXI_CONTROL_F1_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F1r), (_val))
#define MODIFY_PHY8806X_AXI_CONTROL_F1_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F1r), (_val), (_mask))

#define READ_PHY8806X_AXI_STATUS_F0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F0r), (_val))
#define WRITE_PHY8806X_AXI_STATUS_F0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F0r), (_val))
#define MODIFY_PHY8806X_AXI_STATUS_F0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F0r), (_val), (_mask))

#define READ_PHY8806X_AXI_STATUS_F1_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F1r), (_val))
#define WRITE_PHY8806X_AXI_STATUS_F1_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F1r), (_val))
#define MODIFY_PHY8806X_AXI_STATUS_F1_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F1r), (_val), (_mask))

#define READ_PHY8806X_MISC_CONTROL_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_MISC_CONTROLr), (_val))
#define WRITE_PHY8806X_MISC_CONTROL_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_MISC_CONTROLr), (_val))
#define MODIFY_PHY8806X_MISC_CONTROL_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_MISC_CONTROLr), (_val), (_mask))

#define READ_PHY8806X_AXI_ADDR_LOWER_F0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F0r), (_val))
#define WRITE_PHY8806X_AXI_ADDR_LOWER_F0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F0r), (_val))
#define MODIFY_PHY8806X_AXI_ADDR_LOWER_F0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F0r), (_val), (_mask))

#define READ_PHY8806X_AXI_ADDR_LOWER_F1_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F1r), (_val))
#define WRITE_PHY8806X_AXI_ADDR_LOWER_F1_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F1r), (_val))
#define MODIFY_PHY8806X_AXI_ADDR_LOWER_F1_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F1r), (_val), (_mask))

#define READ_PHY8806X_AXI_ADDR_UPPER_F0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r), (_val))
#define WRITE_PHY8806X_AXI_ADDR_UPPER_F0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r), (_val))
#define MODIFY_PHY8806X_AXI_ADDR_UPPER_F0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r), (_val), (_mask))

#define READ_PHY8806X_AXI_ADDR_UPPER_F1_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F1r), (_val))
#define WRITE_PHY8806X_AXI_ADDR_UPPER_F1_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F1r), (_val))
#define MODIFY_PHY8806X_AXI_ADDR_UPPER_F1_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F1r), (_val), (_mask))

#define READ_PHY8806X_SBUS_DATA_BYTES_1_0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_1_0r), (_val))
#define WRITE_PHY8806X_SBUS_DATA_BYTES_1_0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_1_0r), (_val))
#define MODIFY_PHY8806X_SBUS_DATA_BYTES_1_0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_1_0r), (_val), (_mask))

#define READ_PHY8806X_SBUS_DATA_BYTES_3_2_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_3_2r), (_val))
#define WRITE_PHY8806X_SBUS_DATA_BYTES_3_2_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_3_2r), (_val))
#define MODIFY_PHY8806X_SBUS_DATA_BYTES_3_2_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_3_2r), (_val), (_mask))

#define READ_PHY8806X_SBUS_DATA_BYTES_5_4_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_5_4r), (_val))
#define WRITE_PHY8806X_SBUS_DATA_BYTES_5_4_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_5_4r), (_val))
#define MODIFY_PHY8806X_SBUS_DATA_BYTES_5_4_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_5_4r), (_val), (_mask))

#define READ_PHY8806X_SBUS_DATA_BYTES_7_6_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_7_6r), (_val))
#define WRITE_PHY8806X_SBUS_DATA_BYTES_7_6_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_7_6r), (_val))
#define MODIFY_PHY8806X_SBUS_DATA_BYTES_7_6_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_7_6r), (_val), (_mask))

#define READ_PHY8806X_SBUS_DATA_BYTES_9_8_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_9_8r), (_val))
#define WRITE_PHY8806X_SBUS_DATA_BYTES_9_8_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_9_8r), (_val))
#define MODIFY_PHY8806X_SBUS_DATA_BYTES_9_8_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_9_8r), (_val), (_mask))

#define READ_PHY8806X_SBUS_DATA_BYTES_11_10_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_11_10r), (_val))
#define WRITE_PHY8806X_SBUS_DATA_BYTES_11_10_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_11_10r), (_val))
#define MODIFY_PHY8806X_SBUS_DATA_BYTES_11_10_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_11_10r), (_val), (_mask))

#define READ_PHY8806X_SBUS_DATA_BYTES_13_12_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_13_12r), (_val))
#define WRITE_PHY8806X_SBUS_DATA_BYTES_13_12_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_13_12r), (_val))
#define MODIFY_PHY8806X_SBUS_DATA_BYTES_13_12_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_13_12r), (_val), (_mask))

#define READ_PHY8806X_SBUS_DATA_BYTES_15_14_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_15_14r), (_val))
#define WRITE_PHY8806X_SBUS_DATA_BYTES_15_14_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_15_14r), (_val))
#define MODIFY_PHY8806X_SBUS_DATA_BYTES_15_14_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_DATA_BYTES_15_14r), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_1_0_F0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F0r), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_1_0_F0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F0r), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_1_0_F0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F0r), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_3_2_F0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_3_2_F0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_3_2_F0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_5_4_F0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_5_4_F0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_5_4_F0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_7_6_F0_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_7_6_F0_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_7_6_F0_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_1_0_F1_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F1r), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_1_0_F1_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F1r), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_1_0_F1_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F1r), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_3_2_F1_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F1r), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_3_2_F1_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F1r), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_3_2_F1_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F1r), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_5_4_F1_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F1r), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_5_4_F1_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F1r), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_5_4_F1_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F1r), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_7_6_F1_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F1r), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_7_6_F1_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F1r), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_7_6_F1_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F1r), (_val), (_mask))

#define AXI_DELTA (EXTRACT_REG(montreal2_AXI_CONTROL_F1r) - EXTRACT_REG(montreal2_AXI_CONTROL_F0r))

#define READ_PHY8806X_AXI_ADDR_LOWER_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_ADDR_LOWER_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_ADDR_LOWER_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_ADDR_UPPER_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_ADDR_UPPER_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_ADDR_UPPER_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_CONTROL_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_CONTROL_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_CONTROL_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_STATUS_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_STATUS_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_STATUS_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_STATUS_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_1_0_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_1_0_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_1_0_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_SW_TO_FW_INTERRUPT_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SW_TO_FW_INTERRUPTr), (_val))
#define WRITE_PHY8806X_SW_TO_FW_INTERRUPT_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SW_TO_FW_INTERRUPTr), (_val))
#define MODIFY_PHY8806X_SW_TO_FW_INTERRUPT_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SW_TO_FW_INTERRUPTr), (_val), (_mask))

#define READ_PHY8806X_FW_TO_SW_MESSAGE_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_FW_TO_SW_MESSAGEr), (_val))
#define WRITE_PHY8806X_FW_TO_SW_MESSAGE_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_FW_TO_SW_MESSAGEr), (_val))
#define MODIFY_PHY8806X_FW_TO_SW_MESSAGE_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_FW_TO_SW_MESSAGEr), (_val), (_mask))

#define READ_PHY8806X_TOP_PVTMON_CTRL_0_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_PVTMON_CTRL_0r, (_val), 1)
#define WRITE_PHY8806X_TOP_PVTMON_CTRL_0_REG(_unit, _phy_ctrl, _val) \
    { \
        uint32 temp = (_val); \
        int rv; \
        rv = _bcm_toplvl_sbus_write((unit), (_phy_ctrl), montreal2_TOP_PVTMON_CTRL_0r, &temp, 1); \
        rv; \
    }

#define READ_PHY8806X_TOP_PVTMON_CTRL_1_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_PVTMON_CTRL_1r, (_val), 1)
#define WRITE_PHY8806X_TOP_PVTMON_CTRL_1_REG(_unit, _phy_ctrl, _val) \
    { \
        uint32 temp = (_val); \
        int rv; \
        rv = _bcm_toplvl_sbus_write((unit), (_phy_ctrl), montreal2_TOP_PVTMON_CTRL_1r, &temp, 1); \
        rv; \
    }

#define READ_PHY8806X_TOP_TMON_CTRL_0_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_TMON_CTRL_0r, (_val), 1)
#define WRITE_PHY8806X_TOP_TMON_CTRL_0_REG(_unit, _phy_ctrl, _val) \
    { \
        uint32 temp = (_val); \
        int rv; \
        rv = _bcm_toplvl_sbus_write((unit), (_phy_ctrl), montreal2_TOP_TMON_CTRL_0r, &temp, 1); \
        rv; \
    }

#define READ_PHY8806X_TOP_PVTMON_RESULT_0_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_PVTMON_RESULT_0r, (_val), 1)

#define READ_PHY8806X_TOP_PVTMON_RESULT_1_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_PVTMON_RESULT_1r, (_val), 1)

#define READ_PHY8806X_TOP_PVTMON_RESULT_2_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_PVTMON_RESULT_2r, (_val), 1)

#define READ_PHY8806X_TOP_TMON_RESULT_REG(_unit, _phy_ctrl, _val) \
    _bcm_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_TMON_RESULTr, (_val), 1)

#endif  /* _BCM_PHY_H_ */


