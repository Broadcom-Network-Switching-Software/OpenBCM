/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       phy8806x.h
 */

#ifndef   _PHY8806X_H_
#define   _PHY8806X_H_

#include <soc/phy.h>
#include <soc/util.h>
#include <soc/ll.h>

#define montreal2_TOP_DEV_REV_IDr 0x02000000

#define montreal2_TOP_SOFT_RESET_REGr 0x02000100
#define montreal2_TOP_RTS_MISC_CTRLr  0x02006f00
#define montreal2_TOP_SOFT_RESET_REG_2r 0x02000200
#define montreal2_TOP_FC_MODE_CONTROLr 0x02007300

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
#define montreal2_VS3_MISC_STATUS     0x00808400
#define montreal2_VS4_MISC_STATUS     0x00808500
#define montreal2_VS5_MISC_STATUS     0x00808600
#define montreal2_VS6_MISC_STATUS     0x00808700
#define montreal2_VS7_MISC_STATUS     0x00808800

#define montreal2_FCPORT_EXT_TX_DISABLE_CTRLr 0x02002d00

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
/* #define montreal2_BLKTYPE_LINE_FCPORT   4 */
#define montreal2_BLKTYPE_MAPPER        5
#define montreal2_BLKTYPE_MDIO          6
#define montreal2_BLKTYPE_PSM           7
#define montreal2_BLKTYPE_SYSSIDE_CL_PM 8
/* #define montreal2_BLKTYPE_SYS_FCPORT    9 */
/* #define montreal2_BLKTYPE_TOP           10 */
#define montreal2_BLKTYPE_UCSS          11

/* Define blktypes with Schan / Block Numbers */
#define montreal2_BLKTYPE_TOP           34
#define montreal2_BLKTYPE_LINE_FCPORT   3
#define montreal2_BLKTYPE_SYS_FCPORT    7

#define PHY8806X_C45_DEV1           0x1

/* PHY flag definitions */
#define PHY8806X_FWDLM          0x1 /* firmware download master */
#define PHY8806X_BMP0           0x2 /* boot master 0 */
#define PHY8806X_BMP1           0x4 /* boot master 1 */
#define PHY8806X_CHM            0x8 /* chip master */
#define PHY8806X_PM            0x10 /* passive mode */
#define PHY8806X_FLEX          0x20 /* flex reconfiguration */
#define PHY8806X_AC            0x40 /* download to acc chips */
#define PHY8806X_MODE_CHANGE   0x80 /* Mode change, Eth->FC or FC->Eth */
#define PHY8806X_PORTMOD      0x100 /* driver called from portmod  */
#define PHY8806X_FW_RESTART   0x200 /* Do not re-download f/w. Restart */
#define PHY8806X_ENCAP_CHANGE 0x400 /* HG->IEEE or IEEE->HG */

#define PHY8806X_FLAGS_BMP   0x1

#define NUM_PATTERN_DATA_INTS    8
typedef struct phy8806x_pattern_s {
    uint32 pattern_data[NUM_PATTERN_DATA_INTS];
    uint32 pattern_length;
} phy8806x_pattern_t;

typedef struct phy8806x_speed_config_s {
    uint32  port_refclk_int;
    int     speed;
    int     sys_lane_count;
    int     ln_lane_count;
    int     port_is_higig;
    int     line_interface;
    int     interface;
    int     pcs_bypass;
    int     fiber_pref;      /* line side */
    int     fiber_pref_sys;  /* Sys side */
    int     pll_divider_req;
    int     an_mode;
    int     an_fec;
    int     an_cl72;
    int     fs_cl72;
    int     fs_cl72_sys;
    int     port_mode;  /* Repeater or Retimer mode */
    int     fs_fec;
    int     fs_fec_sys;
    int     is_flex;
    int     quad_mode;
    int     repeater_an_phase;   /* Repeater mode AN state */
    int     repeater_an_resolved_speed;
    int     repeater_an_retry_phase0;
    int     repeater_an_retry_phase1;
    mac_driver_t  *macd;         /* Pointer to MAC */
    uint8   reset_trial;         /* Trial count */
} phy8806x_speed_config_t;

/*
   Config - per logical port
*/
typedef struct {
    phymod_polarity_t               phy_polarity_config;
    phymod_phy_reset_t              phy_reset_config;
    phy8806x_pattern_t              pattern;
    phy8806x_speed_config_t         speed_config;
    int fiber_pref;                 /* spn_SERDES_FIBER_PREF */
    int cl73an;                     /* spn_PHY_AN_C73 */
    int an_cl72;                    /* spn_PHY_AN_C72 */
    int forced_init_cl72;           /* spn_FORCED_INIT_CL72 */
    int hg_mode;                    /* higig port */
    int an_fec;                     /*enable FEC for AN mode */
    int sgmii_mstr;                 /* sgmii master mode */
    uint32 line_intf;               /*line interface info */
    int flags;                      /* PHY flags for internal use */
    soc_timeout_t to;               /* timer for multi pass initialization */
    uint8 *firmware;                /* firmware pointer */
    int  firmware_len;              /* firmware length in bytes */
    int  dl_dividend;               /* divident for MDC during firmware download */
    int  dl_divisor;                /* divisor for MDC during firmware download */
    uint64 inbctr[512];             /* Inband Counters Buffer */
    uint16 pmd_size;                /* PMD size */
    uint16 pmd_crc;                 /* PMD CRC */
    uint32 top_dev_rev_id;          /* cached TOP_DEV_REV_IDr */
    int user_set_tx;                /* this one will indicate if user set the tx or not */
    int led_speed;                  /* Speed */
    uint8 led_tx;                   /* Tx activity */
    uint8 led_rx;                   /* Rx activity */
} phy8806x_config_t;

typedef enum ether_mode_e {
    ETH_REPEATER,
    ETH_RETIMER,
    ETH_GEARBOX,
    ETH_ETHERRAM
} ether_mode_t;

#define FLAGS(_pc) (((phy8806x_config_t *)((_pc) + 1))->flags)

#define PHY8806X_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8806X_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8806X_REG_MODIFY(_unit, _phy_ctrl, _addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val), (_mask))
#define PHY8806X_REG_WB_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_WB_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))

#define READ_PHY8806X_DEV1_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8806X_C45_DEV1, (_reg)), (_val))
#define WRITE_PHY8806X_DEV1_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8806X_C45_DEV1, (_reg)), (_val))
#define MODIFY_PHY8806X_DEV1_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
            PHY8806X_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8806X_C45_DEV1, (_reg)), (_val), (_mask))

/* PHY Identifier Register 0 (Addr 0002h) */
#define READ_PHY8806X_DEV1_ID0r(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_DEV1_REG((_unit), (_phy_ctrl), 0x0002,(_val))

/* PHY Identifier Register 1 (Addr 0003h) */
#define READ_PHY8806X_DEV1_ID1r(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_DEV1_REG((_unit), (_phy_ctrl), 0x0003,(_val))

int
_phy_8806x_toplvl_sbus_write(int unit,phy_ctrl_t *pc, uint32 reg, uint32 arg[], int size);
int
_phy_8806x_toplvl_sbus_read(int unit,phy_ctrl_t *pc, uint32 reg, uint32 arg[], int size);
int 
_phy_8806x_write_top_soft_reset_reg(int unit, phy_ctrl_t *pc, uint32 val);
int
_phy_8806x_write_top_rts_misc_ctrl_reg(int unit, phy_ctrl_t *pc, uint32 val);
int 
_phy_8806x_write_top_soft_reset_reg_2(int unit, phy_ctrl_t *pc, uint32 val);

int 
_phy_8806x_write_top_fc_mode_control_reg(int unit, phy_ctrl_t *pc, uint32 val);
int
_phy_8806x_fcport_sbus_write(int unit,phy_ctrl_t *pc, int sys, uint32 reg, uint32 arg[], int size);
int
_phy_8806x_fcport_sbus_read(int unit,phy_ctrl_t *pc, int sys, uint32 reg, uint32 arg[], int size);
int 
_phy_8806x_write_fcport_ext_tx_disable_ctrl(int unit, phy_ctrl_t *pc, int sys, uint32 val);

#define READ_TOP_DEV_REV_ID_REG(_unit, _phy_ctrl, _val) \
    _phy_8806x_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_DEV_REV_IDr, (_val), 1)

#define READ_TOP_SOFT_RESET_REG(_unit, _phy_ctrl, _val) \
    _phy_8806x_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_SOFT_RESET_REGr, (_val), 1)

#define WRITE_TOP_SOFT_RESET_REG(_unit, _phy_ctrl, _val) \
    _phy_8806x_write_top_soft_reset_reg((unit), (_phy_ctrl), (_val)) 


#define READ_TOP_RTS_MISC_CTRL_REG(_unit, _phy_ctrl, _val) \
    _phy_8806x_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_RTS_MISC_CTRLr, (_val), 1)

#define WRITE_TOP_RTS_MISC_CTRL_REG(_unit, _phy_ctrl, _val) \
    _phy_8806x_write_top_rts_misc_ctrl_reg((unit), (_phy_ctrl), (_val)) 

#define READ_TOP_SOFT_RESET_REG_2(_unit, _phy_ctrl, _val) \
    _phy_8806x_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_SOFT_RESET_REG_2r, (_val), 1)

#define WRITE_TOP_SOFT_RESET_REG_2(_unit, _phy_ctrl, _val) \
    _phy_8806x_write_top_soft_reset_reg_2((unit), (_phy_ctrl), (_val))

#define READ_TOP_FC_MODE_CONTROL_REG(_unit, _phy_ctrl, _val) \
    _phy_8806x_toplvl_sbus_read((unit), (_phy_ctrl), montreal2_TOP_FC_MODE_CONTROLr, (_val), 1)

#define WRITE_TOP_FC_MODE_CONTROL_REG(_unit, _phy_ctrl, _val) \
    _phy_8806x_write_top_fc_mode_control_reg((unit), (_phy_ctrl), (_val))

#define READ_SYS_FCPORT_EXT_TX_DISABLE_CTRL(_unit, _phy_ctrl, _val) \
    _phy_8806x_fcport_sbus_read((unit), (_phy_ctrl), 1, montreal2_FCPORT_EXT_TX_DISABLE_CTRLr, (_val), 1)

#define READ_LINE_FCPORT_EXT_TX_DISABLE_CTRL(_unit, _phy_ctrl, _val) \
    _phy_8806x_fcport_sbus_read((unit), (_phy_ctrl), 0, montreal2_FCPORT_EXT_TX_DISABLE_CTRLr, (_val), 1)

#define WRITE_SYS_FCPORT_EXT_TX_DISABLE_CTRL(_unit, _phy_ctrl, _val) \
    _phy_8806x_write_fcport_ext_tx_disable_ctrl((unit), (_phy_ctrl), 1, (_val))

#define WRITE_LINE_FCPORT_EXT_TX_DISABLE_CTRL(_unit, _phy_ctrl, _val) \
    _phy_8806x_write_fcport_ext_tx_disable_ctrl((unit), (_phy_ctrl), 0, (_val))

#define EXTRACT_REG(x) (((x) >> 8) & 0xffff)

#define READ_PHY8806X_MDIO_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(0x1, (_reg)), (_val))
#define WRITE_PHY8806X_MDIO_REG(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(0x1, (_reg)), (_val))
#define MODIFY_PHY8806X_MDIO_REG(_unit, _phy_ctrl, _reg, _val, _mask) \
            PHY8806X_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(0x1, (_reg)), (_val), (_mask))

#define WRITE_PHY8806X_MDIO_REG_WB(_unit, _phy_ctrl, _reg,_val) \
            PHY8806X_REG_WB_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(0x1, (_reg)), (_val))


#define READ_PHY8806X_SBUS_STATUS_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_STATUSr), (_val))
#define WRITE_PHY8806X_SBUS_STATUS_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_STATUSr), (_val))
#define MODIFY_PHY8806X_SBUS_STATUS_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_STATUSr), (_val), (_mask))

#define READ_PHY8806X_SBUS_CONTROL_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_CONTROLr), (_val))
#define WRITE_PHY8806X_SBUS_CONTROL_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_CONTROLr), (_val))
#define MODIFY_PHY8806X_SBUS_CONTROL_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_CONTROLr), (_val), (_mask))

#define READ_PHY8806X_SBUS_COMMAND_UPPER_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_UPPERr), (_val))
#define WRITE_PHY8806X_SBUS_COMMAND_UPPER_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_UPPERr), (_val))
#define MODIFY_PHY8806X_SBUS_COMMAND_UPPER_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_UPPERr), (_val), (_mask))

#define READ_PHY8806X_SBUS_COMMAND_LOWER_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_LOWERr), (_val))
#define WRITE_PHY8806X_SBUS_COMMAND_LOWER_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_LOWERr), (_val))
#define MODIFY_PHY8806X_SBUS_COMMAND_LOWER_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_COMMAND_LOWERr), (_val), (_mask))

#define READ_PHY8806X_SBUS_ADDR_UPPER_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_UPPERr), (_val))
#define WRITE_PHY8806X_SBUS_ADDR_UPPER_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_UPPERr), (_val))
#define MODIFY_PHY8806X_SBUS_ADDR_UPPER_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_UPPERr), (_val), (_mask))

#define READ_PHY8806X_SBUS_ADDR_LOWER_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_LOWERr), (_val))
#define WRITE_PHY8806X_SBUS_ADDR_LOWER_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SBUS_ADDR_LOWERr), (_val))
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
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_ADDR_LOWER_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_LOWER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_ADDR_UPPER_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_ADDR_UPPER_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_ADDR_UPPER_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_ADDR_UPPER_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))


#define READ_PHY8806X_AXI_CONTROL_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_CONTROL_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_CONTROL_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
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
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_1_0_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_1_0_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))


#define READ_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_3_2_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_5_4_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))

#define READ_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define WRITE_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val))
#define MODIFY_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_AXI_DATA_BYTES_7_6_F0r) + (((_phy_ctrl)->phy_id & 0x4) ? AXI_DELTA : 0), (_val), (_mask))



#define READ_PHY8806X_SW_TO_FW_INTERRUPT_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SW_TO_FW_INTERRUPTr), (_val))
#define WRITE_PHY8806X_SW_TO_FW_INTERRUPT_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG_WB((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SW_TO_FW_INTERRUPTr), (_val))
#define MODIFY_PHY8806X_SW_TO_FW_INTERRUPT_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_SW_TO_FW_INTERRUPTr), (_val), (_mask))

#define READ_PHY8806X_FW_TO_SW_MESSAGE_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_FW_TO_SW_MESSAGEr), (_val))
#define WRITE_PHY8806X_FW_TO_SW_MESSAGE_REG(_unit, _phy_ctrl, _val) \
            WRITE_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_FW_TO_SW_MESSAGEr), (_val))
#define MODIFY_PHY8806X_FW_TO_SW_MESSAGE_REG(_unit, _phy_ctrl, _val, _mask) \
            MODIFY_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_FW_TO_SW_MESSAGEr), (_val), (_mask))
#define READ_PHY8806X_VS5_MISC_STATUS_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_VS5_MISC_STATUS), (_val))
#define READ_LINK_STATUS_REG READ_PHY8806X_VS5_MISC_STATUS_REG
#define VS_MISC_STATUS_LL_STATUS_BIT_MASK      0x8
#define VS_MISC_STATUS_LIVE_STATUS_BIT_MASK    0x4
#define VS_MISC_STATUS_STATUS_BIT_MASK         0x1

#define READ_PHY8806X_VS6_MISC_STATUS_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_VS6_MISC_STATUS), (_val))
#define READ_FC_LINK_STATUS_REG READ_PHY8806X_VS6_MISC_STATUS_REG

#define READ_PHY8806X_VS7_MISC_STATUS_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_VS7_MISC_STATUS), (_val))
#define READ_FC_LINK_FAULT_REG READ_PHY8806X_VS7_MISC_STATUS_REG

#define READ_PHY8806X_VS3_MISC_STATUS_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_VS3_MISC_STATUS), (_val))
#define READ_ETH_LOCAL_FAULT_REG READ_PHY8806X_VS3_MISC_STATUS_REG

#define READ_PHY8806X_VS4_MISC_STATUS_REG(_unit, _phy_ctrl, _val) \
            READ_PHY8806X_MDIO_REG((_unit), (_phy_ctrl), EXTRACT_REG(montreal2_VS4_MISC_STATUS), (_val))
#define READ_ETH_REMOTE_FAULT_REG READ_PHY8806X_VS4_MISC_STATUS_REG

/* 13 bit Error code - field width in xmod status for 
   Error code from Firmware to sdk 
 */
#define BFCMAP_ERROR_CODE_MASK 0x1FFF

#endif  /* _PHY8806X_H_ */


