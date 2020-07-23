/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       phy82328.c
 * Purpose:    Phys Driver support for Broadcom 82328 40G phy
 * Note:      
 *
 * Specifications:

 * Repeater mode only, no retimer operation.

 * Supports the following data speeds:
 * 1.25 Gbps line rate (1 GbE data rate, 8x oversampled over 10 Gbps line rate)
 * 10.3125 Gbps line rate (10 GbE data rate)
 * 11.5 Gpbs line rate (for backplane application, proprietary data rate)
 * 410.3125 Gbps line rate (40 GbE data rate)
 * 
 * Supports the following line-side connections:
 * 1 GbE and 10 GbE SFP+ SR and LR optical modules
 * 40 GbE QSFP SR4 and LR4 optical modules
 * 1 GbE and 10 GbE SFP+ CR (CX1) copper cable
 * 40 GbE QSFP CR4 copper cable
 * 10 GbE KR, 11.5 Gbps line rate and 40 GbE KR4 backplanes
 *
 * Operates with the following reference clocks:
 * Single 156.25 MHz differential clock for 1.25 Gbps, 10.3125 Gpbs and 11.5 Gbps
 * line rates
 *
 * Supports autonegotiation as follows:
 * Clause 73 only for starting Clause 72 and requesting FEC 
 * No speed resolution performed
 * No Clause 73 in 11.5 Gbps line rate, only Clause 72 supported
 * Clause 72 may be enabled standalone for close systems
 * Clause 37 is not supported
 * No on-chip FEC encoding/decoding, but shall pass-through FEC-encoded data. 
 */

#if defined(INCLUDE_PHY_82328) /* INCLUDE_PHY_82328 */
 
#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
 
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
 
#include "phydefs.h"      /* Must include before other phy related includes */
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyxehg.h"
#include "phy82328.h"
extern unsigned char phy82328_ucode_bin[];
extern unsigned int phy82328_ucode_bin_len;
static char *dev_name_82328_a0 = "BCM82328_A0";
static char *dev_name_82328f_a0 = "BCM82328F_A0";

extern unsigned char phy82328B0_ucode_bin[];
extern unsigned int phy82328B0_ucode_bin_len;
static char *dev_name_82328_b0 = "BCM82328_B0";
static char *dev_name_82322_b1 = "BCM82322_B1";
static char *dev_name_82328_b1 = "BCM82328_B1";
static char *dev_name_82328f_b0 = "BCM82328F_B0";
static char *dev_name_82328f_b1 = "BCM82328F_B1";
#define PHY82328B1_CHIP_REV    (0x00b1)
#define PHY82328B0_CHIP_REV    (0x00b0)
#define PHY82328A0_CHIP_REV    (0x00a0)
static char *dev_name_82322 = "BCM82322";
#define PHY82328_DEV_PMA_PMD                     1
#define PHY82328_DEV_AN                          2
#define PHY82328_ENABLE                          1
#define PHY82328_DISABLE                         0
#define PHY82328_TOTAL_WR_BYTE                   0x4000 
#define PHY82328_ID_82328                        0x82328 /* Gallardo_28nm_oct_opt */
#define PHY82328_ID_82328F                       0x8232f /* Gallardo_28nm_oct_opt */
#define PHY82328_ID_82322                        0x82322 /* Gallardo_28nm_oct_opt */
#define PHY82328_NUM_LANES                       4
#define PHY82328_20G_NUM_LANES                   2
#define PHY82328_ALL_LANES                       0xf
#define PHY82328_POL_DND                         0xFFFF
#define PHY82328_SPD_1G                          1000
#define PHY82328_SPD_10G                         10000
#define PHY82328_SPD_11G                         11000
#define PHY82328_SPD_40G                         40000
#define PHY82328_SPD_20G                         20000
#define PHY82328_SPD_42G                         42000
#define PHY82328_SPD_100G                        100000
#define PHY82328_GP_REG_1_SPD_100G               (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_100G_SHIFT)
#define PHY82328_GP_REG_1_SPD_42G                ((1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_40G_SHIFT)| \
                                                 (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_10G_SHIFT) | \
                                                 (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_1G_SHIFT))
#define PHY82328_GP_REG_1_SPD_40G                (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_40G_SHIFT)
#define PHY82328_GP_REG_1_SPD_11G                ((1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_40G_SHIFT)| \
                                                 (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_1G_SHIFT))
#define PHY82328_GP_REG_1_SPD_10G                (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_10G_SHIFT)
#define PHY82328_GP_REG_1_SPD_1G                 ((1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_1G_SHIFT) | \
                                                 (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_10G_SHIFT))

#define PHY82328_4BIT_DP_DEPTH1                  (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_ENABLE_ULTRA_LOW_LATENCY_DATAPATH_SHIFT)
#define PHY82328_4BIT_DP_DEPTH2                  ((1 << MGT_TOP_PMD_USR_GP_REGISTER_1_ENABLE_ULTRA_LOW_LATENCY_DATAPATH_SHIFT) | \
                                                 (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_ULL_DATAPATH_LATENCY_SHIFT))

#define PHY82328_SINGLE_PORT_MODE(_pc) \
    (SOC_INFO((_pc)->unit).port_num_lanes[(_pc)->port] >= 4)

#define PHY82328_AN_CTRL_REG                     0x0000
#define PHY82328_AN_STS_REG                      0x0001
#define PHY82328_AN_STS_LINK_STS_MSK             0x0004
#define PHY82328_AN_ENABLE_MSK                   (1<<12)
#define PHY82328_AN_COMPLETE_MSK                 (1<<5)

/* I2C related defines */
#define PHY82328_BSC_XFER_MAX     0x1F9
#define PHY82328_BSC_WR_MAX       16
#define PHY82328_WRITE_START_ADDR 0x8007
#define PHY82328_READ_START_ADDR  0x8007
#define PHY82328_WR_FREQ_400KHZ   0x0100
#define PHY82328_2W_STAT          0x000C
#define PHY82328_2W_STAT_IDLE     0x0000
#define PHY82328_2W_STAT_COMPLETE 0x0004
#define PHY82328_2W_STAT_IN_PRG   0x0008
#define PHY82328_2W_STAT_FAIL     0x000C
#define PHY82328_BSC_WRITE_OP     0x22
#define PHY82328_BSC_READ_OP      0x2
#define PHY82328_I2CDEV_WRITE     0x1
#define PHY82328_I2CDEV_READ      0x0
#define PHY82328_I2C_8BIT         0
#define PHY82328_I2C_16BIT        1
#define PHY82328_I2C_TEMP_RAM     0xE
#define PHY82328_I2C_OP_TYPE(access_type,data_type) \
        ((access_type) | ((data_type) << 8))
#define PHY82328_I2C_ACCESS_TYPE(op_type) ((op_type) & 0xff)
#define PHY82328_I2C_DATA_TYPE(op_type)   (((op_type) >> 8) & 0xff)
#define PHY82328_I2CM_IDLE_WAIT_MSEC                        350
#define PHY82328_I2CM_IDLE_WAIT_CHUNK                       10
#define PHY82328_I2CM_IDLE_WAIT_COUNT     (PHY82328_I2CM_IDLE_WAIT_MSEC / PHY82328_I2CM_IDLE_WAIT_CHUNK)

#define PHY82328_PREEMPHASIS_LANE_GET(_T, _L)           \
    if (_T == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {      \
        _L = 0; }                                       \
    else if (_T == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) { \
        _L = 1; }                                       \
    else if (_T == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) { \
        _L = 2; }                                       \
    else if (_T == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) { \
        _L = 3; }                                       \
    else {                                              \
        _L = PHY82328_ALL_LANES; }


static const char *phy82328_intf_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
int eye_consol_val[64][64];

/* interface params */
typedef struct phy82328_intf_cfg {
    int32          speed;
    soc_port_if_t  type;
} phy82328_intf_cfg_t;

typedef	enum {
    PHY82328_DATAPATH_20,
    PHY82328_DATAPATH_4_DEPTH_1,
    PHY82328_DATAPATH_4_DEPTH_2
}phy82328_datapath_t;

typedef enum {
    PHY82328_INTF_SIDE_LINE,
    PHY82328_INTF_SIDE_SYS
} phy82328_intf_side_t;

typedef struct {
    int32 enable;
    int32	count;					/* resume only if count==0 */
} phy82328_micro_ctrl_t;

typedef struct {
    uint32              devid;
    uint32              devrev;
    int32               p2l_map[PHY82328_NUM_LANES];  /* index: physical lane, array element: */
    phy82328_intf_cfg_t line_intf;
    phy82328_intf_cfg_t sys_intf;
    uint16              pol_tx_cfg;
    uint16              pol_rx_cfg;
    int32               mod_auto_detect;			  /* module auto detect enabled */
    phy82328_datapath_t cfg_datapath;
    phy82328_datapath_t cur_datapath;
    int32               an_en;
    uint16              logical_lane0;
    int32               sys_forced_cl72;
    int32               sync_init;
    phy82328_micro_ctrl_t  micro_ctrl;
    int32               fw_rx_los;
    int32               int_phy_re_en;
    soc_timeout_t       sync_to;
    int                 cfg_sys_intf;
    uint16              port_disabled;
} phy82328_dev_desc_t;

#define DEVID(_pc)              (((phy82328_dev_desc_t *)((_pc) + 1))->devid)
#define DEVREV(_pc)             (((phy82328_dev_desc_t *)((_pc) + 1))->devrev)
#define P2L_MAP(_pc,_ix)        (((phy82328_dev_desc_t *)((_pc) + 1))->p2l_map[(_ix)])
#define LINE_INTF(_pc)          (((phy82328_dev_desc_t *)((_pc) + 1))->line_intf)
#define SYS_INTF(_pc)           (((phy82328_dev_desc_t *)((_pc) + 1))->sys_intf)
#define POL_TX_CFG(_pc)	        (((phy82328_dev_desc_t *)((_pc) + 1))->pol_tx_cfg)
#define POL_RX_CFG(_pc)	        (((phy82328_dev_desc_t *)((_pc) + 1))->pol_rx_cfg)
#define MOD_AUTO_DETECT(_pc)    (((phy82328_dev_desc_t *)((_pc) + 1))->mod_auto_detect)
#define CFG_DATAPATH(_pc)       (((phy82328_dev_desc_t *)((_pc) + 1))->cfg_datapath)
#define CUR_DATAPATH(_pc)       (((phy82328_dev_desc_t *)((_pc) + 1))->cur_datapath)
#define AN_EN(_pc)              (((phy82328_dev_desc_t *)((_pc) + 1))->an_en)
#define SYS_FORCED_CL72(_pc)    (((phy82328_dev_desc_t *)((_pc) + 1))->sys_forced_cl72)
#define SYNC_INIT(_pc)	        (((phy82328_dev_desc_t *)((_pc) + 1))->sync_init)
#define MICRO_CTRL(_pc)	        (((phy82328_dev_desc_t *)((_pc) + 1))->micro_ctrl)
#define FW_RX_LOS(_pc)	        (((phy82328_dev_desc_t *)((_pc) + 1))->fw_rx_los)
#define LOGICAL_LANE0(_pc)		(((phy82328_dev_desc_t *)((_pc) + 1))->logical_lane0)
#define SYNC_TO(_pc)			(((phy82328_dev_desc_t *)((_pc) + 1))->sync_to)
#define INT_PHY_RE_EN(_pc)		(((phy82328_dev_desc_t *)((_pc) + 1))->int_phy_re_en)
#define CFG_SYS_INTF(_pc)		(((phy82328_dev_desc_t *)((_pc) + 1))->cfg_sys_intf)
#define PORT_DISABLED(_pc)		(((phy82328_dev_desc_t *)((_pc) + 1))->port_disabled)


#define PHY_82328_MICRO_PAUSE(_u, _p, _s) \
    do { \
        phy_ctrl_t *_pc; \
        _pc = EXT_PHY_SW_STATE((_u), (_p)); \
        if (DEVREV(_pc) == PHY82328A0_CHIP_REV || DEVREV(_pc) == PHY82328B0_CHIP_REV || DEVREV(_pc) == PHY82328B1_CHIP_REV) { \
            _phy_82328_micro_pause((_u), (_p), (_s)); \
        } \
    } while (0) 

#define PHY_82328_MICRO_RESUME(_u, _p)\
    do { \
        phy_ctrl_t *_pc; \
        _pc = EXT_PHY_SW_STATE((_u), (_p)); \
        if (DEVREV(_pc) == PHY82328A0_CHIP_REV || DEVREV(_pc) == PHY82328B0_CHIP_REV || DEVREV(_pc) == PHY82328B1_CHIP_REV) { \
            _phy_82328_micro_resume((_u), (_p)); \
        } \
    } while (0) 

#define POL_CONFIG_LANE_MASK(_l)           (1 << _l)

/* QSFI_RX :: anaRxStatus :: status_sel [02:00] */
#define QSFI_RX_ANARXSTATUS_SM_SEL_MASK            0x0007

/* QSFI_RX :: anaRxStatus :: status_sel [02:00] */
#define QSFI_RX_ANARXSTATUS_PMD_RX_PMD_LOCK_MASK   0x1000

/* REMOTE PHY ABLITY REG*/
#define PHY82328_RMT_PHY_ABLITY_REG                0xc450

/* REMOTE PHY VALID FLAG*/
#define PHY82328_RMT_PHY_ABLITY_VALID_MASK         0x8
#define PHY82328_RMT_PHY_ABLITY_KR_MASK            0x10
#define PHY82328_RMT_PHY_ABLITY_KR2_MASK           0x20
#define PHY82328_RMT_PHY_ABLITY_KR4_MASK           0x40
#define PHY82328_RMT_PHY_ABLITY_PAUSE_MASK         0x3800
#define PHY82328_RMT_PHY_ABLITY_FEC_MASK           0xC000

/* EYE SCAN RELATED MACROS & ENUMS*/
#define PHY82328_STS_IS_MICRO_STOPPED(u, p,  pc, sts)                         phy82328_serdes_rdbl_uc_var(u, p, pc, 0x11, &sts)
#define PHY82328_TRNSUM_TAP_5_TO_N2            0
#define PHY82328_TRNSUM_TAP_0                  4
#define PHY82328_DSC_STATE_UC_TUNE             7
#define PHY82328_RD_UC_DSC_RDY_FOR_CMD(u, pc, rdy)                         \
    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(u, pc, DSC_A_DSC_UC_CTRL, &rdy));      \
    rdy = (rdy & 0x0080) >> 7;

/** SERDES_PMD_UC_COMMANDS Enum */
typedef enum PHY82328_SERDES_PMD_UC_CMD {
  PHY82328_CMD_NULL                =  0,
  PHY82328_CMD_UC_CTRL             =  1,
  PHY82328_CMD_HEYE_OFFSET         =  2,
  PHY82328_CMD_VEYE_OFFSET         =  3,
  PHY82328_CMD_FREEZE_STEADY_STATE =  4,
  PHY82328_CMD_DIAG_EN             =  5,
  PHY82328_CMD_READ_UC_LANE_BYTE   =  6,
  PHY82328_CMD_WRITE_UC_LANE_BYTE  =  7,
  PHY82328_CMD_READ_UC_CORE_BYTE   =  8,
  PHY82328_CMD_WRITE_UC_CORE_BYTE  =  9,
  PHY82328_CMD_READ_UC_LANE_WORD   = 10,
  PHY82328_CMD_WRITE_UC_LANE_WORD  = 11,
  PHY82328_CMD_READ_UC_CORE_WORD   = 12,
  PHY82328_CMD_WRITE_UC_CORE_WORD  = 13,
  PHY82328_CMD_EVENT_LOG_CTRL      = 14,
  PHY82328_CMD_EVENT_LOG_READ      = 15
}PHY82328_SERDES_PMD_UC_CMD;

/** SERDES_UC_CTRL_COMMANDS Enum */
typedef enum PHY82328_SERDES_PMD_UC_CTRL_CMD {
  PHY82328_CMD_UC_CTRL_STOP_GRACEFULLY   = 0,
  PHY82328_CMD_UC_CTRL_STOP_IMMEDIATE    = 1,
  PHY82328_CMD_UC_CTRL_RESUME            = 2,
  PHY82328_CMD_UC_CTRL_STOP_HOLD         = 3,
  PHY82328_CMD_UC_CTRL_RELEASE_HOLD_ONCE = 4
}PHY82328_SERDES_PMD_UC_CTRL_CMD;

typedef enum PHY82328_TRNSUM_ERR_SEL {
  PHY82328_TRNSUM_ERR_SEL_EMUX = 0,
  PHY82328_TRNSUM_ERR_SEL_1    = 1,
  PHY82328_TRNSUM_ERR_SEL_M1   = 2,
  PHY82328_TRNSUM_ERR_SEL_P1   = 3
}PHY82328_TRNSUM_ERR_SEL;

soc_port_if_t phy82328_sys_to_port_if[] = {
    0,                                  /* PHY_SYSTEM_INTERFACE_DFLT    */
    SOC_PORT_IF_KX,                     /* PHY_SYSTEM_INTERFACE_KX      */
    SOC_PORT_IF_KR,                     /* PHY_SYSTEM_INTERFACE_KR      */
    SOC_PORT_IF_SR,                     /* PHY_SYSTEM_INTERFACE_SR      */
    SOC_PORT_IF_LR,                     /* PHY_SYSTEM_INTERFACE_LR      */
    SOC_PORT_IF_CR,                     /* PHY_SYSTEM_INTERFACE_CR      */
    SOC_PORT_IF_KR4,                    /* PHY_SYSTEM_INTERFACE_KR4     */
    SOC_PORT_IF_SR4,                    /* PHY_SYSTEM_INTERFACE_SR4     */
    SOC_PORT_IF_LR4,                    /* PHY_SYSTEM_INTERFACE_LR4     */
    SOC_PORT_IF_CR4,                    /* PHY_SYSTEM_INTERFACE_CR4     */
    SOC_PORT_IF_XFI,                    /* PHY_SYSTEM_INTERFACE_XFI     */
    SOC_PORT_IF_XLAUI,                  /* PHY_SYSTEM_INTERFACE_XLAUI   */
        
};

#ifdef PHY82328_MDIO_DEBUGGING
#undef  READ_PHY_REG
#define READ_PHY_REG(_unit, _pc,  _addr, _value) \
            ((_phy_82328_dbg_read)((_pc->unit), _pc, (_addr), (_value)))
#undef  WRITE_PHY_REG
#define WRITE_PHY_REG(_unit, _pc, _addr, _value) \
            ((_phy_82328_dbg_write)((_pc->unit), _pc, (_addr), (_value)))
#undef  MODIFY_PHY_REG
#define MODIFY_PHY_REG(_unit, _pc, _addr, _data, _mask) \
			(_phy_82328_dbg_modify((_unit), (_pc), (_addr), (_data), (_mask)))
#endif

/* dsc values */
typedef struct {       
    uint8 osr_mode;
    uint16 ucv_config;
    uint8 sig_det;
    uint8 rx_lock;
    int16 rx_ppm;
    int8 clk90;
    int8 clkp1;
    uint8 pf_main;
    uint8 pf_hiz;
    uint8 pf_bst;
    uint8 pf_low;
    uint8 vga;
    int8 dc_offset;
    uint8 p1_lvl_ctrl;
    uint8 dfe1;
    int8 dfe2;
    int8 dfe3;
    int8 dfe4;
    int8 dfe5;
    int8 dfe1_dcd;
    int8 dfe2_dcd;
    int8 pe;
    int8 ze;
    int8 me;
    int8 po;
    int8 zo;
    int8 mo;
    uint8 tx_ppm;
    uint8 txfir_pre; 
    uint8 txfir_main;
    uint8 txfir_post1; 
    uint8 txfir_post2;
    uint8 heye_left;
    uint8 heye_right;
    uint8 veye_upper;
    uint8 veye_lower;
    uint8 br_pd_en;
} phy82328_serdes_lane_state;

#define PHY82328_CONV_SIGNED_BYTE(_D, _M, _L)                \
        _D <<= (15 - (_M));                                  \
        _D >>= (15 - (_M) + (_L));                           \
        _D &= (0xFF);


STATIC int32
_phy_82328_channel_select(int32 unit, soc_port_t port, phy82328_intf_side_t side, int32 lane);
STATIC int32
_phy_82328_polarity_flip(int32 unit, soc_port_t port, uint16 cfg_tx_pol, uint16 cfg_rx_pol);
STATIC int32
phy_82328_control_get(int32 unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value);
STATIC int32
phy_82328_control_set(int32 unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value);
STATIC void
_phy_82328_micro_pause(int32 unit, soc_port_t port, const char *loc);
STATIC void
_phy_82328_micro_resume(int32 unit, soc_port_t port);
STATIC int32
_phy_82328_debug_info(int32 unit, soc_port_t port);
STATIC int32
_phy_82328_intf_line_sys_init(int32 unit, soc_port_t port);
STATIC int32
_phy_82328_intf_type_set(int32 unit, soc_port_t port, soc_port_if_t pif, int32 must_update);
STATIC int32
phy_82328_an_get(int32 unit, soc_port_t port, int32 *an, int32 *an_done);
STATIC int32
_phy_82328_intf_line_sys_update(int32 unit, soc_port_t port);

STATIC int32
_phy_82328_speed_set(int32 unit, soc_port_t port, int32 speed);

STATIC int32 
phy_82328_regbit_set_wait_check(int32 unit ,int32 port,
                                int32 reg, int32 bit_num,
                                int32 bitset, int32 timeout);
STATIC int32
_phy_82328_rx_los_control_set(int32 unit, soc_port_t port, uint32 value);


#ifdef PHY82328_MDIO_DEBUGGING
STATIC int32
_phy_82328_dbg_read(int32 unit, phy_ctrl_t *pc, uint32 addr, uint16 *data)
{
	int32 rv;
	rv = (pc->read)(unit, pc->phy_id, addr, data);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Read Addr=0x%04x Val=0x%04x\n"), addr, *data));
	return rv;
		
}

STATIC int32
_phy_82328_dbg_write(int32 unit, phy_ctrl_t *pc, uint32 addr, uint16 data)
{
	int32 rv;
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Write Addr=0x%04x Val=0x%04x\n"), addr, data));
	rv = (pc->write)(unit, pc->phy_id, addr, data);
	return rv;
}

STATIC int32
_phy_82328_dbg_modify(int32 unit, phy_ctrl_t *pc, uint32 addr, uint16 data, uint16 mask)
{
	int32 rv;
	rv = phy_reg_modify(unit, pc, addr, data, mask);
	rv = (pc->read)(unit, pc->phy_id, addr, &data);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Write Addr=0x%04x Val=0x%04x\n"), addr, data));
	return rv;
}
#endif

STATIC 
int32 phy82328_intf_side_reg_select(int unit, soc_port_t port, uint16 intf)
{
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

        SOC_IF_ERROR_RETURN
            (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL, intf));
    return SOC_E_NONE;
}

STATIC
int32 _phy_82328_modify_pma_pmd_reg (int32 unit, int32 port, phy_ctrl_t *pc, 
                               uint16 dev, uint16 address, uint8 msb_pos, 
                               uint8 lsb_pos, uint16 data)
{
    uint16 mask = 0, i =0;

    for (i = lsb_pos; i<=msb_pos; i++) {
        mask |= (1 << i);
    }
   
    data = data << lsb_pos; 
    if (dev == PHY82328_DEV_PMA_PMD) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, address,
                                        data, mask));
    } else { 
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY82328_MMF_AN_REG(unit, pc, address,
                                        data, mask));
    }
    return SOC_E_NONE;
}

STATIC
int32 _phy_82328_set_bcast_mode(int32 unit, int32 port,
                               int16 ena_dis)
{
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
         PHY82328_DEV_PMA_PMD, MGT_TOP_BROADCAST_CTRL,
         MGT_TOP_PMD_USR_BROADCAST_CTRL_BROADCAST_MODE_ENABLE_SHIFT, 
         MGT_TOP_PMD_USR_BROADCAST_CTRL_BROADCAST_MODE_ENABLE_SHIFT, ena_dis));

    return SOC_E_NONE;
}

STATIC
int32 _phy_82328_soft_reset(int32 unit, int32 port)
{
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
         PHY82328_DEV_PMA_PMD, IEEE0_BLK_PMA_PMD_PMD_CONTROL_REGISTER,
         QSFI_IEEE0_PMD_CONTROL_REGISTER_RESET_SHIFT, 
         QSFI_IEEE0_PMD_CONTROL_REGISTER_RESET_SHIFT, 1));

    return SOC_E_NONE;
}

STATIC 
int32 _phy_82328_rom_fw_dload (int32 unit, int32 port, int32 offset, 
                                uint8 *new_fw, uint32 datalen)
{
    uint32 wr_data = 0, j = 0;
    uint16 data16 = 0, temp = 0;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port); 
    if (offset == PHYCTRL_UCODE_BCST_SETUP) {
        /* Enable Bcast */
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82328 Enable Bcast u:%d p:%d\n"),
                             unit, port));
        SOC_IF_ERROR_RETURN (_phy_82328_set_bcast_mode(unit, port,
                          PHY82328_ENABLE));
        return SOC_E_NONE;
    } else if (offset == PHYCTRL_UCODE_BCST_uC_SETUP) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82328 uC setup u:%d p:%d\n"),
                             unit, port));

        /* uController reset */
        SOC_IF_ERROR_RETURN
            (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
             PHY82328_DEV_PMA_PMD, MGT_TOP_GEN_CTRL,
             MGT_TOP_CMN_CTLR_GEN_CTRL_UCRST_SHIFT, 
             MGT_TOP_CMN_CTLR_GEN_CTRL_UCRST_SHIFT, PHY82328_ENABLE));

        SOC_IF_ERROR_RETURN(_phy_82328_soft_reset(unit, port));
        sal_udelay(3000);
        return SOC_E_NONE;
    } else if (offset == PHYCTRL_UCODE_BCST_ENABLE) {
        /* Re Enable Bcast */
        SOC_IF_ERROR_RETURN (_phy_82328_set_bcast_mode(unit, port,
                          PHY82328_ENABLE));
        return SOC_E_NONE;
    } else if (offset == PHYCTRL_UCODE_BCST_LOAD) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82328 uCode load u:%d p:%d\n"),
                             unit, port));

        /*Select MDIO FOR ucontroller download */
        SOC_IF_ERROR_RETURN
         (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
          PHY82328_DEV_PMA_PMD, MGT_TOP_SPA_CONTROL_STATUS_REGISTER,
          MGT_TOP_PMD_USR_SPA_CONTROL_STATUS_REGISTER_SPI_PORT_USED_SHIFT, 
          MGT_TOP_PMD_USR_SPA_CONTROL_STATUS_REGISTER_SPI_PORT_USED_SHIFT, 
          PHY82328_DISABLE));
        
        /* Clear Download Done */
        SOC_IF_ERROR_RETURN
         (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
          PHY82328_DEV_PMA_PMD, MGT_TOP_SPA_CONTROL_STATUS_REGISTER,
          MGT_TOP_PMD_USR_SPA_CONTROL_STATUS_REGISTER_SPI_DWLD_DONE_SHIFT, 
          MGT_TOP_PMD_USR_SPA_CONTROL_STATUS_REGISTER_SPI_DWLD_DONE_SHIFT, 
          PHY82328_DISABLE));

        SOC_IF_ERROR_RETURN
         (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
         PHY82328_DEV_PMA_PMD, MGT_TOP_SPA_CONTROL_STATUS_REGISTER,
         MGT_TOP_PMD_USR_SPA_CONTROL_STATUS_REGISTER_SPI_BOOT_SHIFT, 
         MGT_TOP_PMD_USR_SPA_CONTROL_STATUS_REGISTER_SPI_BOOT_SHIFT, 
         PHY82328_ENABLE));

   
        /* Set Dload size to 32K */
        SOC_IF_ERROR_RETURN
         (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
          PHY82328_DEV_PMA_PMD, MGT_TOP_MISC_CTRL,
          MGT_TOP_CMN_CTLR_MISC_CTRL_SPI_DOWNLOAD_SIZE_SHIFT,
          MGT_TOP_CMN_CTLR_MISC_CTRL_SPI_DOWNLOAD_SIZE_SHIFT, 
          PHY82328_ENABLE));

        /* uController reset */
        SOC_IF_ERROR_RETURN
         (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
          PHY82328_DEV_PMA_PMD, MGT_TOP_GEN_CTRL,
          MGT_TOP_CMN_CTLR_GEN_CTRL_UCRST_SHIFT, 
          MGT_TOP_CMN_CTLR_GEN_CTRL_UCRST_SHIFT, PHY82328_DISABLE));

        sal_udelay(3000);    /*wait for 3ms to initialize the RAM*/

        /*Write Address*/
        SOC_IF_ERROR_RETURN
          (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_MSG_IN, 0x8000));

        /* Make sure Address word is read by the Micro*/
        sal_udelay(10);

        wr_data = (datalen/2);            /*16k ROM*/
        SOC_IF_ERROR_RETURN
            (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_MSG_IN, wr_data));
        sal_udelay(10);
   
        /* Fill in the SRAM */
        wr_data = (datalen - 1);
        for (j = 0; j < wr_data; j+=2) {
            /*Make sure the word is read by the Micro */
            sal_udelay(10);        
            data16 = (new_fw[j] << 8) | new_fw[j+1];
            SOC_IF_ERROR_RETURN
            (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_MSG_IN, data16));
        }
        return SOC_E_NONE;
    } else if(offset== PHYCTRL_UCODE_BCST_END) {
        int i = 0;
        sal_udelay(20);
        if ((pc->phy_id & 3) != 0) {
            return SOC_E_NONE;
        }
        temp = pc->phy_id;
        for (i = 0; i <= 3; i++) {
            pc->phy_id &= ~3;
            pc->phy_id |= i;
            SOC_IF_ERROR_RETURN
                (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_MSG_OUT, &data16));
    
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY82328 FW load done u:%d p:%d Read Done:0x%x\n"),
                                 unit, port, data16));

            sal_udelay(4000);
            SOC_IF_ERROR_RETURN
                (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_4, &data16));

            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY82328 FW load done u:%d p:%d chksum:0x%x\n"),
                                 unit, port, data16));

            if (data16 != 0x600D) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "Invalid Checksum:0x%x\n"), data16));
                pc->phy_id = temp;
                /* Disable Bcast */
                SOC_IF_ERROR_RETURN (_phy_82328_set_bcast_mode(unit, port,
                                      PHY82328_DISABLE));
                return SOC_E_INTERNAL;
            }
            /*Check the register to read*/
            SOC_IF_ERROR_RETURN
             (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, 0xc161, &data16)); 

            LOG_CLI((BSL_META_U(unit,
                                "PHY82328 u:%d p:%d Rom Version(Reg:0xc161):0x%x\n"), 
                     unit, port, (data16 & 0xFF)));

            /* Disable Bcast */
            SOC_IF_ERROR_RETURN (_phy_82328_set_bcast_mode(unit, port,
                              PHY82328_DISABLE));

            data16 = (PHY82328_SINGLE_PORT_MODE(pc)) ?
                     MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK : 0;
            SOC_IF_ERROR_RETURN
               (MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
               MGT_TOP_SINGLE_PMD_CTRL, data16,
               MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK));
            pc->phy_id = temp;
            if (PHY82328_SINGLE_PORT_MODE(pc)) {
                break;
            }
            /* Make sure micro completes its initialization */
            sal_udelay(5000);
        }
        pc->phy_id = temp;
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "u=%d p=%d PHY82328 firmware_bcst: invalid cmd 0x%x\n"),
                   unit, port, offset));
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_intf_is_single_port(soc_port_if_t intf_type)
{
	int32 rv;

    switch (intf_type) {
        case SOC_PORT_IF_CR4:
        case SOC_PORT_IF_KR4:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_LR4:
        case SOC_PORT_IF_SR4:
	        rv = TRUE;
		break;
        default:
		    rv = FALSE;
    }
    return rv;
}

STATIC int32
_phy_82328_intf_is_quad_port(soc_port_if_t intf_type)
{
	int32 rv;

    switch (intf_type) {
        case SOC_PORT_IF_SR:
        case SOC_PORT_IF_CR:
        case SOC_PORT_IF_KR:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SFI:
        case SOC_PORT_IF_LR:
        case SOC_PORT_IF_ZR:
        case SOC_PORT_IF_KX:
        case SOC_PORT_IF_GMII:
        case SOC_PORT_IF_SGMII:
            rv = TRUE;
		break;
        default:
		    rv = FALSE;
    }
    return rv;
}

STATIC int32
_phy_82328_intf_line_sys_params_get(int32 unit, soc_port_t port)
{
    phy82328_intf_cfg_t *line_intf;
    phy82328_intf_cfg_t *sys_intf;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    line_intf = &(LINE_INTF(pc));
    sys_intf = &(SYS_INTF(pc));

	/*
	 * system side interface was specified so make sure it's compatible with
	 * line side speed. If not, overwrite with default compatible interface
	 */
    if (line_intf->speed == PHY82328_SPD_40G) {
        sys_intf->speed = PHY82328_SPD_40G;
        line_intf->type = SOC_PORT_IF_SR4;
        if (!(_phy_82328_intf_is_single_port(sys_intf->type))) {
            /* system side interface is not compatible so overwrite with default */
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY82328 incompatible 40G system side interface, "
                                  "using default: u=%d p=%d\n"), unit, port));
            sys_intf->type = SOC_PORT_IF_XLAUI;
        }
    }
    else { /* 10G/1G */
        if (_phy_82328_intf_is_quad_port(sys_intf->type)) {
            if (sys_intf->type == SOC_PORT_IF_KX) {
                line_intf->speed = PHY82328_SPD_1G;
                line_intf->type = SOC_PORT_IF_GMII;
                sys_intf->speed = PHY82328_SPD_1G;
			} else {
                /* 10G system side so fix line side to be 10G compatible */
                line_intf->speed = PHY82328_SPD_10G;
                line_intf->type = SOC_PORT_IF_SR;
                sys_intf->speed = PHY82328_SPD_10G;
            }
        } else {
            /* system side interface is not compatible so overwrite with default */
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY82328 incompatible 10G/1G system side interface, "
                                  "using default: u=%d p=%d\n"), unit, port));
            /* default to 10G interface/speed on both sides */
            line_intf->speed = PHY82328_SPD_10G;
            line_intf->type = SOC_PORT_IF_SR;
            sys_intf->speed = PHY82328_SPD_10G;
            sys_intf->type =  SOC_PORT_IF_XFI;
        }
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_config_update(int32 unit, soc_port_t port)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    
    /* Change polarity got from config */
    return _phy_82328_polarity_flip(unit, port, POL_TX_CFG(pc), POL_RX_CFG(pc));
}

#ifdef BCM_WARM_BOOT_SUPPORT

STATIC int32
_phy_82328_intf_speed_from_reg(uint16 mode, int32 *speed)
{
    uint16 speed_reg = mode & (MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_1G_MASK |
                               MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_10G_MASK |
                               MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_40G_MASK |
                               MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_100G_MASK );
    *speed = 0;
    switch (speed_reg) {

    case PHY82328_GP_REG_1_SPD_42G:
        *speed = PHY82328_SPD_42G;
        break;

    case PHY82328_GP_REG_1_SPD_40G:
        *speed = PHY82328_SPD_40G;
        break;

    case PHY82328_GP_REG_1_SPD_10G:
        *speed = PHY82328_SPD_10G;
        break;
    case PHY82328_GP_REG_1_SPD_1G: 
        /********* 100, 10 not recoverable from PHY ********/
        *speed = PHY82328_SPD_1G;
        break;

    default:
        /* hardware has unexpected value */
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_intf_type_from_reg(uint16 mode, phy82328_intf_side_t side, int32 speed, int32 an,
                              soc_port_if_t *intf_type) 
{
    uint16 intf_reg = mode & ((side == PHY82328_INTF_SIDE_LINE) ? 0x5300 : 0xac00);

    if (side == PHY82328_INTF_SIDE_LINE) {
        if (intf_reg == 0) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_SR4;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_SR;
            } else if (speed == PHY82328_SPD_1G) {
                if (an) {
                    *intf_type = SOC_PORT_IF_GMII;
                } else {
                    *intf_type = SOC_PORT_IF_SGMII;
                }
            } else if (speed == 100 || speed == 10) {
                *intf_type = SOC_PORT_IF_SGMII;
            } 
            else {
                return SOC_E_FAIL;
            }
        } else if (intf_reg == 0x4200) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_XLAUI;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_SFI;
            } else {
                return SOC_E_FAIL;
            }
        } else if (intf_reg == 0x4000) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_LR4;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_LR;
            } else {
                return SOC_E_FAIL;
            }
        } else if (intf_reg == 0x200 || intf_reg == 0x1200) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_CR4;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_CR;
            }
            else {
                return SOC_E_FAIL;
            }
        } else if (intf_reg == 0x100) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_KR4;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_KR;
            } else if (speed == PHY82328_SPD_1G || speed == 100 || speed == 10) {
                *intf_type = SOC_PORT_IF_KX;
            }
            else {
                return SOC_E_FAIL;
            }
        } else if (intf_reg == 0x5000) {
            if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_ZR;
            } else {
                return SOC_E_FAIL;
            }
        } else {
            return SOC_E_FAIL;
        } /*  if (intf_reg) */
    } else {
        if (intf_reg == 0) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_SR4;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_SR;
            } else if (speed == PHY82328_SPD_1G) {
                if (an) {
                    *intf_type = SOC_PORT_IF_GMII;
                } else {
                    *intf_type = SOC_PORT_IF_SGMII;
                }
            } else if (speed == 100 || speed == 10) {
                *intf_type = SOC_PORT_IF_SGMII;
            }
            else {
                return SOC_E_FAIL;
            }
        } else if (intf_reg == 0x8800) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_XLAUI;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_XFI;
            } 
            else {
                return SOC_E_FAIL;
            }
        } else if (intf_reg == 0x800 || intf_reg == 0x2800) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_CR4;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_CR;
            }
            else {
                return SOC_E_FAIL;
            }
        } else if (intf_reg == 0x400) {
            if (speed == PHY82328_SPD_40G || speed == PHY82328_SPD_42G) {
                *intf_type = SOC_PORT_IF_KR4;
            } else if (speed == PHY82328_SPD_10G) {
                *intf_type = SOC_PORT_IF_KR;
            } else if (speed == PHY82328_SPD_1G || speed == 10 || speed == 100) {
                *intf_type = SOC_PORT_IF_KX;
            } 
            else {
                return SOC_E_FAIL;
            }
        } else {
           return SOC_E_FAIL;
        }
    } 
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_reinit_line_intf(int32 unit, soc_port_t port, int32 speed, int32 an, uint16 mode)
{
    phy_ctrl_t  *pc;
    phy82328_intf_cfg_t *line_intf;
    
    pc = EXT_PHY_SW_STATE(unit, port);
    line_intf = &(LINE_INTF(pc));

    line_intf->speed = speed;
    SOC_IF_ERROR_RETURN(_phy_82328_intf_type_from_reg(mode, PHY82328_INTF_SIDE_LINE, speed, an, &(line_intf->type)));

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_reinit_system_intf(int32 unit, soc_port_t port, int32 speed, int32 an, uint16 mode)
{
    phy_ctrl_t  *pc;
    phy82328_intf_cfg_t *system_intf;
    
    pc = EXT_PHY_SW_STATE(unit, port);
    system_intf = &(SYS_INTF(pc));

    system_intf->speed = speed;
    SOC_IF_ERROR_RETURN(_phy_82328_intf_type_from_reg(mode, PHY82328_INTF_SIDE_SYS, speed, an, &(system_intf->type)));

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_reinit_cur_datapath(int32 unit, soc_port_t port, uint16 mode)
{
    uint16 datapath_reg;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    datapath_reg = mode & (MGT_TOP_PMD_USR_GP_REGISTER_1_ENABLE_ULTRA_LOW_LATENCY_DATAPATH_MASK |
                           MGT_TOP_PMD_USR_GP_REGISTER_1_ULL_DATAPATH_LATENCY_MASK);

    if (datapath_reg == 0) {
        CUR_DATAPATH(pc) = PHY82328_DATAPATH_20;
    } else if (datapath_reg == PHY82328_4BIT_DP_DEPTH1) {
        CUR_DATAPATH(pc) = PHY82328_DATAPATH_4_DEPTH_1;
    } else if ( datapath_reg == PHY82328_4BIT_DP_DEPTH2) {
        CUR_DATAPATH(pc) = PHY82328_DATAPATH_4_DEPTH_2;
    } else {
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_reinit_from_mode(int32 unit, soc_port_t port)
{
    uint16 mode_reg = 0, side_reg, an_reg;
    int32 speed = 0, an =0, an_done = 0;
    phy_ctrl_t  *pc, *int_pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    if (int_pc == NULL) {
        return SOC_E_UNAVAIL;
    }
    /* 
     * Make sure line side registers are selected
     * if not, have to bail out since the system is in a bad state
     */
    SOC_IF_ERROR_RETURN(
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL, &side_reg));
    if (side_reg != 0) {
        return SOC_E_UNAVAIL;
    }

    /* Read the current mode */
    SOC_IF_ERROR_RETURN(
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_3, &mode_reg));

    /* Get speed */
    SOC_IF_ERROR_RETURN(_phy_82328_intf_speed_from_reg(mode_reg, &speed));

    /* Get an setting */
    if (speed < PHY82328_SPD_10G ) {
        SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_GET(int_pc->pd, unit, port, &an, &an_done));
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &speed));
    } else {
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_AN_REG(unit, pc,
                                                     PHY82328_AN_CTRL_REG, &an_reg));
        an = ((an_reg & PHY82328_AN_ENABLE_MSK) == PHY82328_AN_ENABLE_MSK);
    }
    
    AN_EN(pc) = an;
#if 1
    SOC_IF_ERROR_RETURN(
        _phy_82328_reinit_line_intf(unit, port, speed, an, mode_reg));
    SOC_IF_ERROR_RETURN(
        _phy_82328_reinit_system_intf(unit, port, speed, an, mode_reg));
#endif
    SOC_IF_ERROR_RETURN(
        _phy_82328_reinit_cur_datapath(unit, port, mode_reg));

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_reinit_fw_rx_los(int32 unit, soc_port_t port)
{
    uint16 data16 = 0;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, SIGDET_SIGDET_CTRL_1, &data16));

    FW_RX_LOS(pc) = (data16 & SIGDET_SIGDET_CTRL_1_SIGNAL_DETECT_FRC_MASK) >>
                      SIGDET_SIGDET_CTRL_1_SIGNAL_DETECT_FRC_SHIFT;

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_reinit(int32 unit, soc_port_t port)
{
	SOC_IF_ERROR_RETURN(
            _phy_82328_reinit_from_mode(unit , port));

    /* Reinit fw_rx_los */
    SOC_IF_ERROR_RETURN(
        _phy_82328_reinit_fw_rx_los(unit, port));

    return SOC_E_NONE;
}

#endif /*  BCM_WARM_BOOT_SUPPORT */

STATIC int32
_phy_82328_init_pass2(int32 unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    phy82328_intf_cfg_t *line_intf, *sys_intf;
    phy82328_micro_ctrl_t *micro_ctrl;
    uint16 if_sys_idx = 0;
    uint16 chan_data = 0, chan_mask = 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY82328 init pass2: u=%d p=%d\n"), unit, port));
    pc = EXT_PHY_SW_STATE(unit, port);
	/*
	 * - interfaces are KR4/KR/KX/XAUI/XFI/SR/LR/CR/SR4/LR4/CR4
	 * - Gallardo is capable of being configured symmetrically and can support all 
	 *   interfaces on both sides
	 * - system side interfaces cannot be changed dynamically
	 *   - configured in Gallardo via config variables
	 * - line side mode can be changed dynamically
	 *   - configured specifically with interface set and/or by matching speed
	 *   - automatically when module autodetect is enabled
	 *
	 *    port mode            system side                   line side
	 * ----------------  ------------------------  --------------------------------
	 * 40G(singleport)   dflt:XLAUI or configvar   dflt:XLAUI  or user set if/speed
	 * 1G/10G(quadport)  dflt:XFI or configvar     dflt:SFI    or user set if/speed
	 */
	
    line_intf = &(LINE_INTF(pc));
    sys_intf  = &(SYS_INTF(pc));
    micro_ctrl = &(MICRO_CTRL(pc));

    sal_memset(line_intf, 0, sizeof(phy82328_intf_cfg_t));
    sal_memset(sys_intf,  0, sizeof(phy82328_intf_cfg_t));
    sal_memset(micro_ctrl,  0, sizeof(phy82328_micro_ctrl_t));

    AN_EN(pc) = 0;
    SYNC_INIT(pc) = 1;
    FW_RX_LOS(pc) = 0;
    LOGICAL_LANE0(pc) = 0;
    micro_ctrl->enable = 1;
    micro_ctrl->count = 0;

    SYS_FORCED_CL72(pc) = soc_property_port_get(unit, port, spn_PORT_INIT_CL72, 0);
    pc->flags |= PHYCTRL_INIT_DONE;
	/* Get configured system interface */
    if_sys_idx = soc_property_port_get(unit, port, spn_PHY_SYS_INTERFACE, 0);
    if (if_sys_idx >= (sizeof(phy82328_sys_to_port_if) / sizeof(soc_port_if_t))) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "PHY82328 invalid system side interface: u=%d p=%d intf=%d\n"
                              "Using default interface\n"),
                   unit, port, if_sys_idx));
        if_sys_idx = 0;
	}
    sys_intf->type = phy82328_sys_to_port_if[if_sys_idx];
    if (PHY82328_SINGLE_PORT_MODE(pc)) {
        line_intf->speed = PHY82328_SPD_40G;
    }
    else {
        line_intf->speed = PHY82328_SPD_10G;
    }
    if (sys_intf->type == 0) {
		/* use defaults based on current line side */
        sys_intf->speed = line_intf->speed;
        if ( line_intf->speed == PHY82328_SPD_40G) {
            line_intf->type = SOC_PORT_IF_SR4;
            sys_intf->type = SOC_PORT_IF_XLAUI;
        }
        else {
            line_intf->type = SOC_PORT_IF_SR;
            sys_intf->type = SOC_PORT_IF_XFI;
        }
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "default sys and line intf are used\n")));
    } else {
        SOC_IF_ERROR_RETURN(_phy_82328_intf_line_sys_params_get(unit, port));
        LOG_CLI((BSL_META_U(unit,
                            "After updating intf based on sys config\n")));
        LOG_CLI((BSL_META_U(unit,
                            "SYS Intf:%s speed:%d\n"),
                 phy82328_intf_names[sys_intf->type],sys_intf->speed));
        LOG_CLI((BSL_META_U(unit,
                            "Line intf:%s speed:%d\n"),
                 phy82328_intf_names[line_intf->type],line_intf->speed));
    }
    CFG_SYS_INTF(pc) = sys_intf->type;
    CFG_DATAPATH(pc) = soc_property_port_get(unit, port, "82328_DATAPATH", PHY82328_DATAPATH_20);
    CUR_DATAPATH(pc) = CFG_DATAPATH(pc);
	
    /* Polarity configuration is applied every time interface is configured */
    POL_TX_CFG(pc) = soc_property_port_get(unit, port, spn_PHY_TX_POLARITY_FLIP, PHY82328_POL_DND);
    POL_RX_CFG(pc) = soc_property_port_get(unit, port, spn_PHY_RX_POLARITY_FLIP, PHY82328_POL_DND);

    PHY_82328_MICRO_PAUSE(unit, port, "polarity config in Init");

    /* Push configuration to the device */
    SOC_IF_ERROR_RETURN(_phy_82328_config_update(unit, port));
    
    PHY_82328_MICRO_RESUME(unit, port);
    /* Manually power down channel*/
    chan_data = (1 << MGT_TOP_PMD_USR_OPTICAL_CONFIGURATION_MAN_TXON_EN_SHIFT) |
        (1 << MGT_TOP_PMD_USR_OPTICAL_CONFIGURATION_TXOFFT_SHIFT);
    chan_mask = MGT_TOP_PMD_USR_OPTICAL_CONFIGURATION_MAN_TXON_EN_MASK |
        MGT_TOP_PMD_USR_OPTICAL_CONFIGURATION_TXOFFT_MASK;

    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_OPTICAL_CONFIGURATION,
                                       chan_data, chan_mask));
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        /* Do nothing */
    } else 
#endif
    {
        /* Initialize system and line side interfaces */
        SOC_IF_ERROR_RETURN(_phy_82328_intf_line_sys_init(unit, port));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return _phy_82328_reinit(unit, port);
    }
#endif
    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) {
        /* indicate third  pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS3);
    }
    return SOC_E_NONE;
}

STATIC int
_phy_82328_init_pass3(int unit, soc_port_t port)
{
    int rv;
    phy_ctrl_t  *pc, *int_pc;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY82328 init pass3: u=%d p=%d\n"), unit, port));
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    /* Complete  what we started from PASS2 in PASS3 */
    if (INT_PHY_RE_EN(pc)) {
        /* wait for timer to expire */
        while (!soc_timeout_check(&SYNC_TO(pc))) {
            sal_usleep(100);
        }

        /* Turn internal PHY back on now that the mode has been configured */
        SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, 1));
        INT_PHY_RE_EN(pc) = 0;
    }
	/* Logical lane0 used for auto-negotiation in 40G CR4 */
	if (PHY82328_SINGLE_PORT_MODE(pc)) {
		uint16 logical_lane0;
		
		logical_lane0 = soc_property_port_get(unit, port, spn_PHY_LANE0_L2P_MAP, 0);
		if (logical_lane0 > PHY82328_NUM_LANES) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY82328 bad auto-negotiation lane %d: u=%d p=%d"
                                  " lane must be 0..3\n"), (int)logical_lane0, unit, port));
		} else {
			SOC_IF_ERROR_RETURN(
				MODIFY_PHY82328_MMF_PMA_PMD_REG(
					unit, pc, MGT_TOP_SINGLE_PMD_CTRL, 
					logical_lane0 << MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_SHIFT, 
					MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_MASK));
			LOGICAL_LANE0(pc) = logical_lane0;
		}
	}
	
	/* If configured, enable module auto detection */
	MOD_AUTO_DETECT(pc) = soc_property_port_get(unit, port, spn_PHY_MOD_AUTO_DETECT, 0);
	if (MOD_AUTO_DETECT(pc)) {
		uint16 data, mask;
		
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82328 module auto detect enabled: u=%d p=%d\n"),
                  unit, port));

        /* Enables both SFP and QSFP module detect */
 		data = (MGT_TOP_CMN_CTLR_GP_REG_0_ENABLE_QSFP_MOD_DETECT_MASK |
                MGT_TOP_CMN_CTLR_GP_REG_0_ENABLE_SFP_MOD_DETECT_MASK);
		mask = (MGT_TOP_CMN_CTLR_GP_REG_0_ENABLE_QSFP_MOD_DETECT_MASK |
                MGT_TOP_CMN_CTLR_GP_REG_0_ENABLE_SFP_MOD_DETECT_MASK);
		
		rv = MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_0, data, mask);
		if (SOC_FAILURE(rv)) {
			MOD_AUTO_DETECT(pc) = 0;
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "PHY82328 setting module auto detect failed: u=%d p%d\n"),
                                   unit, port));
		} 
	}
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return _phy_82328_reinit(unit, port);
    }
#endif

    /* Cold boot code only below this  */
	return SOC_E_NONE;
}

/*
 * Check if the core has been initialized. This is done by
 * looking at all ports which belong to the same core and see
 * if any one of them have completed initialization.
 * The primary port is used to determine whether the port is
 * on the same core as all ports in a core share the primary port.
 */
STATIC int32
_phy_82328_core_init_done(int32 unit, struct phy_driver_s *pd, int32 primary_port)
{
    int32 rv;
    soc_port_t port;
    phy_ctrl_t *pc;
    uint32 core_primary_port;

    PBMP_ALL_ITER(unit, port) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc == NULL) {
            continue;
        }
        /* Make sure this port has a 82328 driver */
        if (pc->pd != pd) {
            continue;
        }
        rv = phy_82328_control_get(unit, port, SOC_PHY_CONTROL_PORT_PRIMARY,
                                   &core_primary_port);
        if (rv != SOC_E_NONE) {
            continue;
        }
        if ((primary_port == core_primary_port) && (pc->flags & PHYCTRL_INIT_DONE)) {
            return TRUE;
		}
	}
    return FALSE;
}


STATIC int32
_phy_82328_init_pass1(int32 unit, soc_port_t port)
{
    phy_ctrl_t			*pc;
    int32               offset;
    soc_port_t			primary_port;
    uint16              chip_rev;
		
    pc = EXT_PHY_SW_STATE(unit, port);
	
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY82328 init pass1: u=%d p=%d\n"), unit, port));

    SOC_IF_ERROR_RETURN(
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                      MGT_TOP_CHIP_REV_REGISTER,
				      &chip_rev));
    DEVREV(pc) = chip_rev;
    /* Set the primary port & offset */
    if (soc_phy_primary_and_offset_get(unit, port, &primary_port, &offset) != SOC_E_NONE) {
		/* 
		 * Derive primary and offset 
		 * There is an assumption lane 0 on first core on the the MDIO
		 * bus is a primary port
		 */
        if (PHY82328_SINGLE_PORT_MODE(pc)) {
            primary_port = port;
            offset = 0;
        } else {
            /*primary_port = port - (pc->phy_id & 0x3);*/
            if ((port-1) == 0) {
                primary_port = (((port)/4) * 4) + 1;
            } else {
                primary_port = (((port -1)/4) * 4) + 1;
            }
            offset = pc->phy_id & 0x3;
        }
    }
    /* set the default values that are valid for many boards */
    SOC_IF_ERROR_RETURN(phy_82328_control_set(unit, port, SOC_PHY_CONTROL_PORT_PRIMARY,
                                              primary_port));
	
    SOC_IF_ERROR_RETURN(phy_82328_control_set(unit, port,SOC_PHY_CONTROL_PORT_OFFSET,
                                              offset));

    /*
     * Since there is a single micro and therefore a single copy of the firmware
     * shared among the 4 channels in a core, do not download firmware if in quad
     * channel and the other channels are active.
     */
     if (PHY82328_SINGLE_PORT_MODE(pc) ||
        (!PHY82328_SINGLE_PORT_MODE(pc) && 
        (!_phy_82328_core_init_done(unit, pc->pd, primary_port)))) {
         uint16 sts_reg = 0;
         SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, 
                     MGT_TOP_SPA_CONTROL_STATUS_REGISTER, &sts_reg));

         if ((sts_reg & (1 << 13)) && 
             !(soc_property_port_get(unit, port,
                     spn_PHY_FORCE_FIRMWARE_LOAD, FALSE))) {
             pc->flags &= (~PHYCTRL_MDIO_BCST);
         } else {
             LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY82328 Bcast Enabled for Port:%d"), port));
             pc->flags |= PHYCTRL_MDIO_BCST;
	     }
    }
	
	/* indicate second pass of init is needed */
    PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_chip_id_get(int32 unit, soc_port_t port, phy_ctrl_t *pc, uint32 *chip_id)
{
	int32 rv = SOC_E_NONE;

    uint16 chip_id_lsb = 0, chip_id_msb = 0, chip_id_misc = 0;

    *chip_id = 0;

    SOC_IF_ERROR_RETURN(
            READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_CHIP_ID0_REGISTER, &chip_id_lsb));
    SOC_IF_ERROR_RETURN(
            READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_CHIP_ID1_REGISTER, &chip_id_msb));
    SOC_IF_ERROR_RETURN(
            READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_OTP_MISC_SIG4_REGISTER, &chip_id_misc));

    if (chip_id_msb == 0x8) {
        if (chip_id_lsb == 0x2328) {
            if (chip_id_misc & (1U << 12)) {
                *chip_id = PHY82328_ID_82328F;
            } else {
                *chip_id = PHY82328_ID_82328;
            }
        } else if (chip_id_lsb == 0x2322) {
            *chip_id = PHY82328_ID_82322;
        } else if (chip_id_lsb == 0x232F) {
            *chip_id = PHY82328_ID_82328F;
        } else {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY82328  bad chip id: u=%d p=%d chipid %x%x\n"),
                                  unit, port, chip_id_msb, chip_id_lsb));
            rv = SOC_E_BADID;
        }
    }
    return rv;
}

STATIC int32
_phy_82328_config_devid(int32 unit,soc_port_t port, phy_ctrl_t *pc, uint32 *devid)
{
    if (soc_property_port_get(unit, port, "phy_82328", FALSE)) {
        return *devid = PHY82328_ID_82328;
    } else if(soc_property_port_get(unit, port, "phy_82328f", FALSE)) {
        return *devid = PHY82328_ID_82328F;
    } else if(soc_property_port_get(unit, port, "phy_82322", FALSE)) {
        return *devid = PHY82328_ID_82322;
    }

    return _phy_82328_chip_id_get(unit, port, pc, devid);
}

/* Function:
 *    phy_82328_init
 * Purpose:
 *    Initialize 82328 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 * Returns:
 *    SOC_E_NONE
 */

STATIC
int32 phy_82328_init(int32 unit, soc_port_t port)
{
    int32					ix;
    uint32				devid;
    phy_ctrl_t			*pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY82328 init: u=%d p=%d state=%d\n"), 
                         unit, port,PHYCTRL_INIT_STATE(pc)));
    SOC_IF_ERROR_RETURN(
      phy82328_intf_side_reg_select (unit, port, PHY82328_INTF_SIDE_LINE));

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        /* Read the chip id to identify the device */
        SOC_IF_ERROR_RETURN(_phy_82328_config_devid(unit, port, pc, &devid));

        /* Save it in the device description structure for future use */  
        DEVID(pc) = devid;

        /* initialize default p2l map */
        for (ix = 0; ix < PHY82328_NUM_LANES; ix++) {
            P2L_MAP(pc,ix) = ix;
        }
        PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45 | PHY_FLAGS_REPEATER);
        SOC_IF_ERROR_RETURN(_phy_82328_init_pass1(unit, port));

        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        SOC_IF_ERROR_RETURN (_phy_82328_init_pass2(unit, port));
        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        SOC_IF_ERROR_RETURN( _phy_82328_init_pass3(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82328 init pass3 completed: u=%d p=%d\n"), unit, port));
        PHYCTRL_INIT_STATE_SET(pc, PHYCTRL_INIT_STATE_DEFAULT);
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

/* Return contents of ANARXSTATUS */
STATIC int32
_phy_82328_intf_link_get(int32 unit, soc_port_t port, int32 *link)
{
    int32 lane=0, max_lane=0;
    uint16 data16=0;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

        if (PHY82328_SINGLE_PORT_MODE(pc)) {
            max_lane = PHY82328_NUM_LANES;
        } 
        else {
           max_lane = 1;
        }
    for (lane = 0; lane < max_lane; lane++) {
        if (PHY82328_SINGLE_PORT_MODE(pc)) {
            /* Select the lane on the line side */
            SOC_IF_ERROR_RETURN(
               _phy_82328_channel_select(unit, port, PHY82328_INTF_SIDE_LINE, lane));
        }
        if (AN_EN(pc) == 1) { 
            /*anaRxStatusSel 1.C0B1.[2:0] 0*/
            SOC_IF_ERROR_RETURN(
                READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, PHY82328_AN_STS_REG,
                         &data16));
            if (data16 & PHY82328_AN_STS_LINK_STS_MSK) {
                *link &=1;
            } else {
                *link &=0;
            }
        } else {
            /*anaRxStatusSel 1.C0B1.[2:0] 0*/
            SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, RX_ANARXCONTROL,
                         0x0, QSFI_RX_ANARXSTATUS_SM_SEL_MASK));
            /*anaRxStatusSel 1.C0B0.12 1*/
            SOC_IF_ERROR_RETURN(
                READ_PHY82328_MMF_PMA_PMD_REG (unit, pc, RX_ANARXSTATUS, &data16));
        
            /* Check if rxSeq Done in any lane */
            if ((data16 & QSFI_RX_ANARXSTATUS_PMD_RX_PMD_LOCK_MASK) ==
                 QSFI_RX_ANARXSTATUS_PMD_RX_PMD_LOCK_MASK) {
                *link &= 1;
            } else {
                *link = 0;
            }
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "PhyID:%x link:%d"),
                         pc->phy_id,*link));
        }
#if 0
        if (LINE_INTF(pc).speed == 10000) {
            LOG_CLI((BSL_META_U(unit,
                                "Speed 10G: link:%d\n"), *link));
            break;
        }
#endif
    }
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "-->After MOD PhyID:%x\n"),
                     pc->phy_id));
 
    /* Restore to default single port register access */
    if (PHY82328_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_82328_channel_select(unit, port, PHY82328_INTF_SIDE_LINE,
											  PHY82328_ALL_LANES));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_82328_mod_auto_detect_speed_check(int unit, soc_port_t port, uint16 csr)
{
	uint16 csr_speed;
	phy_ctrl_t *pc;
	phy82328_intf_cfg_t *line_intf;
	static int speed_tbl[] = {	0, 1000, 10000, 0, 40000, 0, 0 ,0, 10000 };

	pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

	csr_speed = csr & (MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_100G_MASK | 
					   MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_40G_MASK |
					   MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_10G_MASK |
					   MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_1G_MASK);
	if (line_intf->speed != speed_tbl[csr_speed]) {
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "82328 module auto detection unexpected: u=%d p=%d speed=0x%x\n"),
                           unit, port, speed_tbl[csr_speed]));
	}
	return (line_intf->speed == speed_tbl[csr_speed]);
}

STATIC int
_phy_82328_mod_auto_detect_intf_process(int unit, soc_port_t port, uint16 csr)
{
	int copper_mod = 0;
	int lr_mod = 0;
	phy_ctrl_t *pc;
	phy82328_intf_cfg_t *line_intf;
	int intf_update = 1;

	pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

	copper_mod = (csr & MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_CU_TYPE_MASK) ==
		MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_CU_TYPE_MASK;

	lr_mod = (csr & MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_LR_MODE_MASK) ==
		MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_LR_MODE_MASK;

	/* Change interface if going from optical to copper */ 
	if (copper_mod) {
		switch (line_intf->type) {
		case SOC_PORT_IF_SR:
		case SOC_PORT_IF_SFI:
		case SOC_PORT_IF_XFI:
			line_intf->type = SOC_PORT_IF_CR;
			break;
		case SOC_PORT_IF_SR4:
		case SOC_PORT_IF_XLAUI:
			line_intf->type = SOC_PORT_IF_CR4;
			break;
		default:
			/* No interface change if not currently an optical interface */
			intf_update = 0;
			break;
		}
	} else {
		/* Change interface if going from optical SR to LR */
		if (lr_mod) {
			switch (line_intf->type) {
			case SOC_PORT_IF_SR:
			case SOC_PORT_IF_SFI:
			case SOC_PORT_IF_XFI:
				line_intf->type = SOC_PORT_IF_LR;
				break;
			case SOC_PORT_IF_SR4:
			case SOC_PORT_IF_XLAUI:
				line_intf->type = SOC_PORT_IF_LR4;
				break;
			default:
				/* No interface change */
				intf_update = 0;
				break;
			}
		}
		/* Change interface if going from copper to optical */
		switch (line_intf->type) {
		case SOC_PORT_IF_CR:
			line_intf->type = lr_mod ? SOC_PORT_IF_LR : SOC_PORT_IF_SR;
			break;
		case SOC_PORT_IF_CR4:
			line_intf->type = lr_mod ? SOC_PORT_IF_LR4 : SOC_PORT_IF_SR4;
			break;
		default:
			intf_update = 0;
			break;
		}
	}
	if (intf_update) {
		SOC_IF_ERROR_RETURN(_phy_82328_intf_line_sys_update(unit, port));
	}
	return SOC_E_NONE;
}

STATIC int
_phy_82328_mod_auto_detect_process(int unit, soc_port_t port, uint16 csr) 
{

	LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "PHY82328 module detected: u=%d p=%d csr=%x\n"), 
                   unit, port, csr));

	/*
	 *  Speed cannot change with module detection so it must match the
	 *  currently configured speed
	 */
	if (!_phy_82328_mod_auto_detect_speed_check(unit, port, csr)) {
		return SOC_E_FAIL;
	}
		
	/*
	 * Get the line side interface based on the current interface and the 
	 * detected module
	 */
	SOC_IF_ERROR_RETURN(_phy_82328_mod_auto_detect_intf_process(unit, port, csr));

	return SOC_E_NONE;
}

STATIC int 
_phy_82328_mod_auto_detect_update(int unit, soc_port_t port) 
{
	phy_ctrl_t *pc;
	uint16 ucode_csr;
	uint16 drv_csr;

    pc = EXT_PHY_SW_STATE(unit, port);

	/* 
	 * New module insertion when 1.c843[4] == 1 and 1.c841[4] == 0
	 */ 
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_3, &ucode_csr));
    if (ucode_csr & MGT_TOP_PMD_USR_GP_REGISTER_3_SPEED_OR_TYPE_CHANGE_MASK) {
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1, &drv_csr));
        if (!(drv_csr & MGT_TOP_PMD_USR_GP_REGISTER_3_SPEED_OR_TYPE_CHANGE_MASK)) {
            SOC_IF_ERROR_RETURN(_phy_82328_mod_auto_detect_process(unit, port, ucode_csr));
			
            /* 
             * Ack firmware that the module insertion has been updated 
             * 1.c841[4] = 1, 1.c841[7] = 0
             */
			SOC_IF_ERROR_RETURN
				(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1, 
												 MGT_TOP_PMD_USR_GP_REGISTER_3_SPEED_OR_TYPE_CHANGE_MASK,
												 MGT_TOP_PMD_USR_GP_REGISTER_3_SPEED_OR_TYPE_CHANGE_MASK |
												 MGT_TOP_PMD_USR_GP_REGISTER_3_FINISH_CHANGE_MASK));
        }
    } else {
		/* 
		 * New module removal when 1.c843[4] == 0 and 1.c841[4] == 1
		 */
        SOC_IF_ERROR_RETURN
			(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1, &drv_csr));
		if (drv_csr & MGT_TOP_PMD_USR_GP_REGISTER_3_SPEED_OR_TYPE_CHANGE_MASK) {
            /* 
             * Ack firmware new module removal 1.c841[4] = 0 
             */
            SOC_IF_ERROR_RETURN
                 (MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1, 
                       0, MGT_TOP_PMD_USR_GP_REGISTER_3_SPEED_OR_TYPE_CHANGE_MASK));
			
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY82328 module removed u=%d p=%d\n"), unit, port));
        }
    }

	return SOC_E_NONE;
}

/*
 *    phy_82328_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *      link - address of memory to store link up/down state.
 * Returns:
 *    SOC_E_NONE
 */
 
STATIC int32
phy_82328_link_get(int32 unit, soc_port_t port, int32 *link)
{
    phy_ctrl_t  *int_pc, *pc;
    int32 int_phy_link = 0;
	
    if (link == NULL) {
        return SOC_E_PARAM;
    }
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }
    int_pc = INT_PHY_SW_STATE(unit, port);   
    pc = EXT_PHY_SW_STATE(unit, port);   
    
    if (int_pc != NULL) {
        SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &int_phy_link));
        *link = int_phy_link;
    } else {
       *link = 1;
    }

    SOC_IF_ERROR_RETURN(
            _phy_82328_intf_link_get( unit, port, link));

    if (MOD_AUTO_DETECT(pc)) {
		SOC_IF_ERROR_RETURN(
                _phy_82328_mod_auto_detect_update(unit, port));
	}

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_82328_link_get: u=%d port%d: link:%s\n"),
                 unit, port, *link ? "Up": "Down"));
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_82328_enable_set
 * Purpose:
 *    Enable/Disable phy
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *      enable - on/off state to set
 * Returns:
 *    SOC_E_NONE
 */
 
STATIC int32
phy_82328_enable_set(int32 unit, soc_port_t port, int32 enable)
{
    uint16 chan_data, chan_mask;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    phy_ctrl_t  *int_pc = INT_PHY_SW_STATE(unit, port);

	/* Manually power channel up/down */
    chan_data = enable ? 0 :
        (1 << MGT_TOP_PMD_USR_OPTICAL_CONFIGURATION_MAN_TXON_EN_SHIFT) |
        (1 << MGT_TOP_PMD_USR_OPTICAL_CONFIGURATION_TXOFFT_SHIFT);
    chan_mask = MGT_TOP_PMD_USR_OPTICAL_CONFIGURATION_MAN_TXON_EN_MASK |
        MGT_TOP_PMD_USR_OPTICAL_CONFIGURATION_TXOFFT_MASK;
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_OPTICAL_CONFIGURATION,
                                       chan_data, chan_mask));
    {
		/* Port already disabled state and if tries to re-enable
         * than reconfigure the mode */
        if (enable && PORT_DISABLED(pc)) {
            SOC_IF_ERROR_RETURN (
             MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1,
                              MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_MASK,
                              MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_MASK));

            /* Handshake with microcode by waiting for ack before moving on */
            SOC_IF_ERROR_RETURN (
             phy_82328_regbit_set_wait_check (unit, port, MGT_TOP_GP_REGISTER_3,
                        MGT_TOP_PMD_USR_GP_REGISTER_3_FINISH_CHANGE_MASK, 1, 1000000));

            /* Cmd active and ucode acked - let ucode know we saw ack */
            SOC_IF_ERROR_RETURN (
                    MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1,
                    0,  MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_MASK));
        }
    }
    if (enable) {
        SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, 1));
        /* Let the micro settle down after powering back up */
        sal_udelay(100);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        PORT_DISABLED(pc) = 0;
    } else {
        SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, 0));
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
        PORT_DISABLED(pc) = 1;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_82328_regbit_set_wait_check
 * Purpose:
 *   Waits for a reg. bit to be set. 
 *	 Spins until bit is set or time out. 
 *   The time out is programmable.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    reg  - register to test
 *	  bit_num - register bit to be tested
 *    bitset - to test set/clear
 *    timeout - spin time out
 * Returns:
 *    SOC_E_NONE if no errors, SOC_E_ERROR else.
 */
STATIC
int32 phy_82328_regbit_set_wait_check(int32 unit ,int32 port,
                  int32 reg,              /* register to check */
                  int32 bit_num,          /* bit number to check */
                  int32 bitset,           /* check bit set or bit clear */
                  int32 timeout)          /* max wait time to check */
{                      
    uint16         data16 = 0;
    soc_timeout_t  to;
    int32            rv;
    int32            cond;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    /* override min_polls with iteration count for DV. */
    soc_timeout_init(&to, timeout, 0);
    do {     
        rv = READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, reg, &data16);
        cond = (bitset  && (data16 & bit_num)) ||
               (!bitset && !(data16 & bit_num));
        if (cond || (rv < 0)) {
            break;
        }
    } while (!soc_timeout_check(&to));

    /* Do one final check potentially after timeout as required by soc_timeout */
    if (rv >= 0) {
        rv = READ_PHY82328_MMF_PMA_PMD_REG(unit,pc,reg,&data16);
        cond = (bitset  && (data16 & bit_num)) ||
               (!bitset && !(data16 & bit_num));
    }
    return (cond? SOC_E_NONE: SOC_E_TIMEOUT);
}

STATIC phy82328_intf_side_t
_phy_82328_intf_side_regs_get(int32 unit, soc_port_t port)
{
    int32 rv=0;
    uint16 data=0;
    phy82328_intf_side_t side = PHY82328_INTF_SIDE_LINE;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);                
 
    rv = READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL, &data);
    if (rv == SOC_E_NONE) {
        side = (data & MGT_TOP_PMD_USR_XPMD_REGS_SEL_SELECT_SYS_REGISTERS_MASK) ? 
                PHY82328_INTF_SIDE_SYS : PHY82328_INTF_SIDE_LINE;
    }
    return side;
}

STATIC int32
_phy_82328_intf_update(int32 unit, soc_port_t port, uint16 reg_data, uint16 reg_mask)
{
    uint16 ucode_csr_bef;
    phy82328_intf_side_t saved_side;
    int32 rv = SOC_E_NONE;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
 
    /* Access line side registers */
    saved_side = _phy_82328_intf_side_regs_get(unit, port);
    if (saved_side == PHY82328_INTF_SIDE_SYS) {                
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    }
    /* Make sure ucode has acked */
    rv = READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_3, &ucode_csr_bef);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "82328 failed reading ucode csr: u=%d p=%d err=%d\n"),
                              unit, port, rv));
        goto fail;
    }
    if ((ucode_csr_bef & MGT_TOP_PMD_USR_GP_REGISTER_3_FINISH_CHANGE_MASK) ==
                MGT_TOP_PMD_USR_GP_REGISTER_3_FINISH_CHANGE_MASK) {

        /* Cmd active and ucode acked, so let ucode know drv saw ack */
        rv = MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1,
                       0x0,  MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_MASK);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 failed clearing ack: u=%d p=%d err=%d\n"),
                                  unit, port, rv));
            goto fail;
        }

        /*Spin for finish Flag to Clear*/
        rv = phy_82328_regbit_set_wait_check (unit,port,MGT_TOP_GP_REGISTER_3,
               MGT_TOP_PMD_USR_GP_REGISTER_3_FINISH_CHANGE_MASK,0,1000000);
        if (rv != SOC_E_NONE) {
            goto fail;
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328 intf update register: u=%d, p=%d, 1.%04x=%04x/%04x ucode_csr=%04x\n"),
                         unit, port, MGT_TOP_GP_REGISTER_1, reg_data, reg_mask, ucode_csr_bef));
 
    /* Send command */
    rv = MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1,
                                      reg_data, reg_mask);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "82328 failed sending command to ucode: u=%d p=%d err=%d\n"),
                   unit, port, rv));
        goto fail;
    }
    /* Handshake with microcode by waiting for ack before moving on */
    rv = phy_82328_regbit_set_wait_check (unit, port, MGT_TOP_GP_REGISTER_3,
                 MGT_TOP_PMD_USR_GP_REGISTER_3_FINISH_CHANGE_MASK, 1, 1000000);
				
    if (rv !=SOC_E_NONE) {
        goto fail;
    }
    /* Cmd active and ucode acked - let ucode know we saw ack */
    rv = MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REGISTER_1,
            0,  MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_MASK);
    if (rv != SOC_E_NONE) {
        goto fail;
    }
 
fail:
    if (saved_side == PHY82328_INTF_SIDE_SYS) {                
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select (unit, port, PHY82328_INTF_SIDE_SYS));
    }
    return rv;
}
						
STATIC int32
_phy_82328_intf_datapath_reg_get(int32 unit, soc_port_t port, phy82328_datapath_t datapath,
                                                                 uint16 *reg_data, uint16 *reg_mask)
{
    *reg_data = 0;
    *reg_mask = 0;
 
    switch (datapath) {
        case PHY82328_DATAPATH_20:
            *reg_data = 0;
        break;
        case PHY82328_DATAPATH_4_DEPTH_1:
            *reg_data = PHY82328_4BIT_DP_DEPTH1;
        break;
        case PHY82328_DATAPATH_4_DEPTH_2:
            *reg_data =	PHY82328_4BIT_DP_DEPTH2;
        break;
        default :
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 invalid datapath: u=%d p=%d datapath=%d\n"),
                                  unit, port, datapath));
            return SOC_E_CONFIG;
    }
    *reg_data |= (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_SHIFT);
    *reg_mask = (MGT_TOP_PMD_USR_GP_REGISTER_1_ENABLE_ULTRA_LOW_LATENCY_DATAPATH_MASK |
	             MGT_TOP_PMD_USR_GP_REGISTER_1_ULL_DATAPATH_LATENCY_MASK |
                 MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_MASK);
				 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328 datapath set register: u=%d, p=%d, reg=%04x/%04x, datapath=%d\n"),
                         unit, port, *reg_data, *reg_mask, datapath));
    return SOC_E_NONE;
}

/*
 * Return the settings in gp_reg_1/gp_register_1 for the inteface speed
 * Register data and mask are returned to modify the register.
 */ 
STATIC int32
_phy_82328_intf_speed_reg_get(int32 unit, soc_port_t port, int32 speed, uint16 *reg_data, uint16 *reg_mask)
{
    *reg_data = 0;
    *reg_mask = 0;		
    switch (speed) {
        case PHY82328_SPD_100G: /*1.C841.3 Not yet implemented */
	       *reg_data = PHY82328_GP_REG_1_SPD_100G; 
           break;
        case PHY82328_SPD_42G:/*1.C841.[2-0]*/
           *reg_data = PHY82328_GP_REG_1_SPD_42G; 
        break;
        case PHY82328_SPD_40G:/*1.C841.2*/
            *reg_data = PHY82328_GP_REG_1_SPD_40G;
        break;
		case PHY82328_SPD_11G: /*1.C841.0 & 1.C841.2*/
            *reg_data = PHY82328_GP_REG_1_SPD_11G;
        break;
        case PHY82328_SPD_10G: /*1.C841.1*/
            *reg_data = PHY82328_GP_REG_1_SPD_10G;
        break;
        case PHY82328_SPD_1G:/*1.C841.0*/
		case 100:
		case 10:
            *reg_data = PHY82328_GP_REG_1_SPD_1G;
        break;
        default :
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 invalid line speed %d: u=%d p=%d\n"),
                                  speed, unit, port));
            return SOC_E_CONFIG;
    }
    *reg_data |= (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_SHIFT);
    *reg_mask |= MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_MASK;
    *reg_mask |= (MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_1G_MASK |
                  MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_10G_MASK|
                  MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_40G_MASK |
                  MGT_TOP_PMD_USR_GP_REGISTER_1_SPEED_100G_MASK);
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328 speed set register: u=%d, p=%d, reg=%04x/%04x), speed=%d\n"),
                         unit, port, *reg_data, *reg_mask, speed));
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_intf_type_reg_get(int32 unit, soc_port_t port, soc_port_if_t intf_type,
                            phy82328_intf_side_t side, uint16 *reg_data, uint16 *reg_mask)
{
    uint16 data = 0;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
 
    assert((side == PHY82328_INTF_SIDE_LINE) || (side == PHY82328_INTF_SIDE_SYS));
    *reg_data = 0;
    *reg_mask = 0;
 
    switch (intf_type) {			
        case SOC_PORT_IF_KX:
        case SOC_PORT_IF_KR:
        case SOC_PORT_IF_KR4:
            data = (side == PHY82328_INTF_SIDE_SYS) ?
                   (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_TYPE_SHIFT):
                   (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_TYPE_SHIFT);
            *reg_mask |= (side == PHY82328_INTF_SIDE_SYS) ?
                (MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_TYPE_MASK):
                (MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_TYPE_MASK);
        break;
        case SOC_PORT_IF_SR:
        case SOC_PORT_IF_SFI:/*Nothing but 10G, but not in list*/
        case SOC_PORT_IF_GMII:
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_SR4: 
            data = 0;
        break;
        case SOC_PORT_IF_CR4: 
            /* If CR4 line side and autoneg enabled, then CR4, otherwise, XFI/XLAUI_DFE*/
            if (side == PHY82328_INTF_SIDE_LINE) {
                data = (AN_EN(pc)) ? (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_CU_TYPE_SHIFT) :
                          ((1 << MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_FORCED_CL72_MODE_SHIFT) |
                           (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_CU_TYPE_SHIFT));
            } else {
                /*System side set to XLAUI_DFE */
                /*CR4 System side is unusual taken  
                  CL72 not supported for Sys Side only DFE*/
                data = (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_SHIFT) |
                       (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_CU_TYPE_SHIFT);
            }
        break;			
       case SOC_PORT_IF_CR:
            data = (side == PHY82328_INTF_SIDE_SYS) ?
                   ((1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_SHIFT) |
                   (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_CU_TYPE_SHIFT)) :
                   (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_CU_TYPE_SHIFT);
        break;
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_XLAUI:
            data = (side == PHY82328_INTF_SIDE_SYS) ?
                   ((1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_LR_MODE_SHIFT) |
                   (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_CU_TYPE_SHIFT)) :
                   ((1 << MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_LR_MODE_SHIFT) |
                   (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_CU_TYPE_SHIFT));
        break;
			
        case SOC_PORT_IF_LR:
        case SOC_PORT_IF_LR4:
            data = (side == PHY82328_INTF_SIDE_SYS) ?
                   (1<< MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_LR_MODE_SHIFT) :
                   (1<< MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_LR_MODE_SHIFT);
        break;
        default:
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 Unsupported interface: u=%d p=%d\n"), unit, port));			
            return SOC_E_UNAVAIL;
    }
    *reg_data |= (data | (1 << MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_SHIFT));
    *reg_mask |=  MGT_TOP_PMD_USR_GP_REGISTER_1_FINISH_CHANGE_MASK;
    if (side == PHY82328_INTF_SIDE_LINE) {
        *reg_mask |= (MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_LR_MODE_MASK |
                      MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_FORCED_CL72_MODE_MASK |
                      MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_CU_TYPE_MASK |
                      MGT_TOP_PMD_USR_GP_REGISTER_1_LINE_TYPE_MASK);
    } else {
        /* System side mask */
        *reg_mask |=  (MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_LR_MODE_MASK |
                       MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_MASK |
                       MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_CU_TYPE_MASK | 
                       MGT_TOP_PMD_USR_GP_REGISTER_1_SYSTEM_TYPE_MASK);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328 intf type register: u=%d, p=%d, reg=%04x/%04x, intf=%d\n"),
                         unit, port, *reg_data, *reg_mask, intf_type));
 
    return SOC_E_NONE;	
}

STATIC int32
_phy_82328_intf_debug_print(int32 unit, soc_port_t port, const char *msg)
{
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    phy_ctrl_t  *pc;
    phy82328_intf_cfg_t *line_intf, *sys_intf;
 
    pc = EXT_PHY_SW_STATE(unit, port);
    line_intf = &(LINE_INTF(pc));
    sys_intf = &(SYS_INTF(pc));
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "%s: "), msg));
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "[LINE:intf=%s,speed=%d], "),
                         phy82328_intf_names[line_intf->type], line_intf->speed));
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "[SYS :intf=%s,speed=%d]\n"),
                         phy82328_intf_names[sys_intf->type], sys_intf->speed));
#endif /* BROADCOM_DEBUG || DEBUG_PRINT */
    return SOC_E_NONE;
}


STATIC int32
_phy_82328_intf_line_sys_update(int32 unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t *int_pc; 
    uint16      data = 0, mask = 0;
    uint16      reg_data = 0, reg_mask = 0;
    int32 int_phy_en = 0;
    phy82328_intf_cfg_t *line_intf, *sys_intf;
 
    pc = EXT_PHY_SW_STATE(unit, port);    
    int_pc = INT_PHY_SW_STATE(unit, port);    
    line_intf = &(LINE_INTF(pc));
    sys_intf = &(SYS_INTF(pc));
 
    _phy_82328_intf_debug_print(unit, port, "interface/Speed update");

    reg_data = 0;
    reg_mask = 0;
    SOC_IF_ERROR_RETURN(
        _phy_82328_intf_type_reg_get(unit, port, line_intf->type,
                                     PHY82328_INTF_SIDE_LINE, &data, &mask));
    reg_data |= data;
    reg_mask |= mask;

    SOC_IF_ERROR_RETURN(
        _phy_82328_intf_type_reg_get(unit, port, sys_intf->type,
                                     PHY82328_INTF_SIDE_SYS, &data, &mask));
    reg_data |= data;
    reg_mask |= mask;
	
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328  intf update: line=%s sys=%s speed=%d (1.%x = %04x/%04x): u=%d p=%d\n"),
              phy82328_intf_names[line_intf->type], phy82328_intf_names[sys_intf->type],
              line_intf->speed, MGT_TOP_GP_REGISTER_1, reg_data, reg_mask,
              unit, port));	
	
    SOC_IF_ERROR_RETURN(
        _phy_82328_intf_speed_reg_get(unit, port, line_intf->speed, &data, &mask));
    reg_data |= data;
    reg_mask |= mask;

    SOC_IF_ERROR_RETURN(
        _phy_82328_intf_datapath_reg_get(unit, port, CUR_DATAPATH(pc), &data, &mask));
    reg_data |= data;
    reg_mask |= mask;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328  intf update: line=%s sys=%s speed=%d (1.%x = %04x/%04x): u=%d p=%d\n"),
              phy82328_intf_names[line_intf->type], phy82328_intf_names[sys_intf->type],
              line_intf->speed, MGT_TOP_GP_REGISTER_1, reg_data, reg_mask,
              unit, port));

    if (SYNC_INIT(pc) == 1) {
        SOC_IF_ERROR_RETURN(PHY_ENABLE_GET(int_pc->pd, unit, port, &int_phy_en));
        if (int_phy_en) {
            /* Turn off internal PHY while the mode changes */
            SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, 0));
        }
    }
    /* GP registers are only on the line side */
    SOC_IF_ERROR_RETURN(_phy_82328_intf_update(unit, port, reg_data, reg_mask));

    if (SYNC_INIT(pc) == 1) {
        if (int_phy_en) {
            /* If called from PASS2 we skip this and complete in PASS3 */
            if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2)) {
                INT_PHY_RE_EN(pc) = 1;
                /* start timer */
                soc_timeout_init(&SYNC_TO(pc), 10000, 0);
            } else {
                sal_udelay(10000);
                /* Turn internal PHY back on now that the mode has been configured */
                SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, 1));
            }
        }
    }
    return SOC_E_NONE;
}


/*
 * Return whether AN is enabled or forced with internal SerDes on the system side
 */
STATIC int32
_phy_82328_intf_sys_forced(int32 unit, soc_port_t port, soc_port_if_t intf_type)
{
    int32 rv;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (intf_type) {
        case SOC_PORT_IF_SR:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_LR:
        case SOC_PORT_IF_LR4:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SFI:
        case SOC_PORT_IF_CR:
        case SOC_PORT_IF_CR4:
        case SOC_PORT_IF_ZR:
            rv = TRUE;
        break;
        case SOC_PORT_IF_GMII:
            rv = FALSE;
        break;
        case SOC_PORT_IF_KR:
        case SOC_PORT_IF_KR4:
            rv = SYS_FORCED_CL72(pc) ? TRUE : FALSE;
        break;
        default:
            rv = FALSE;
    }
    return rv;
}

STATIC int32
_phy_82328_speed_set(int32 unit, soc_port_t port, int32 speed)
{
    int rv = SOC_E_NONE;
    int cur_speed = 0;
    uint32  cur_fw_mode = 0;
    int32 cur_an, cur_an_done;
    soc_port_if_t cur_type, new_type;
    phy_ctrl_t  *pc, *int_pc;
    phy82328_intf_cfg_t  *line_intf;
    phy82328_intf_cfg_t  *sys_intf;
 
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    line_intf = &(LINE_INTF(pc));
    sys_intf = &(SYS_INTF(pc));
 
    line_intf->speed = speed;
    sys_intf->speed = speed;	
    
    if (int_pc != NULL) {
        if ((sys_intf->type == SOC_PORT_IF_GMII) || (sys_intf->type == SOC_PORT_IF_SGMII)) {
            SOC_IF_ERROR_RETURN(PHY_SPEED_SET(int_pc->pd, unit, port, sys_intf->speed));
        } else if (_phy_82328_intf_sys_forced(unit, port, sys_intf->type) ) {
            /*
             * When setting system side 40G DAC/XLAUI-DFE, tell internal SerDes to match
             * with XLAUI.
             *
             * If flexports, the internal SerDes may be out of sync until it gets a chance 
             *  to convert the port, so these operations may be best effort.
             */
            /* When the speed is 20G forcing interface to MLD irrespective of
             * remote PHY capability. When remote PHY capablity is enabled 
             * forcing interface and speed */
                new_type = (sys_intf->type == SOC_PORT_IF_CR4) ? SOC_PORT_IF_XLAUI : sys_intf->type;
                SOC_IF_ERROR_RETURN(PHY_INTERFACE_GET(int_pc->pd, unit, port, &cur_type));
                if (cur_type != new_type) {
                    (void) PHY_INTERFACE_SET(int_pc->pd, unit, port, new_type);
                }

            /* Check if it must change to forced */
            SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_GET(int_pc->pd, unit, port, &cur_an, &cur_an_done));
            if (cur_an == TRUE) {
                (void) PHY_AUTO_NEGOTIATE_SET(int_pc->pd, unit,port, FALSE);
            }
            if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2)) {
                /* Skip int_pc speed set during phy init */
            } else if ((rv = PHY_SPEED_GET(int_pc->pd, unit, port, &cur_speed))
                        != SOC_E_NONE) {
#if 0
                LOG_CLI((BSL_META_U(unit,
                                    "82328 u=%d p=%d speed get error\n"), unit, port));
#endif
                return rv;
            } else if (cur_speed  != sys_intf->speed) {
                (void) PHY_SPEED_SET(int_pc->pd, unit, port, sys_intf->speed);
            }

            /* For system intf=cr4 use XLAUI_DFE on Serdes (in forced mode)*/
            if (sys_intf->type != SOC_PORT_IF_CR4) {
                /* Skip int_pc fw mode set */
            } else if ((rv = PHY_CONTROL_GET(int_pc->pd, unit, port,
                             SOC_PHY_CONTROL_FIRMWARE_MODE, &cur_fw_mode))
                        != SOC_E_NONE) {                
                return rv;
            } else if (cur_fw_mode != SOC_PHY_FIRMWARE_FORCE_OSDFE) {
                (void) PHY_CONTROL_SET(int_pc->pd, unit, port,
                                        SOC_PHY_CONTROL_FIRMWARE_MODE,
                                        SOC_PHY_FIRMWARE_FORCE_OSDFE);
            }

        } else {
            SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET(int_pc->pd, unit,port, TRUE));
        }
    }
    /* Update interface type/speed */        
    return (_phy_82328_intf_line_sys_update(unit, port));
}
/*
 * Function:
 *    _phy_82328_intf_type_1G
 * Purpose:
 *   Check the interface for 1G
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    intf_type - Interface type to test
 * Returns:
 *    TRUE/FALSE
 */
STATIC int32
_phy_82328_intf_type_1G(soc_port_if_t intf_type)
{
    int32 rv;
 
    switch (intf_type) {
        case SOC_PORT_IF_KX:
        case SOC_PORT_IF_GMII:
        case SOC_PORT_IF_SGMII:
            rv = TRUE;
        break;
        default:
            rv = FALSE;
    }
    return rv;
}

/*
 * Function:
 *    _phy_82328_intf_type_10G
 * Purpose:
 *   Check the interface for 10G
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    intf_type - Interface type to test
 * Returns:
 *    TRUE/FALSE
 */
STATIC int32
_phy_82328_intf_type_10G(soc_port_if_t intf_type)
{
    int32 rv;
    
    switch (intf_type) {
        case SOC_PORT_IF_SR:
        case SOC_PORT_IF_CR:
		case SOC_PORT_IF_LR:
        case SOC_PORT_IF_KR:
        case SOC_PORT_IF_ZR:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SFI:
            rv = TRUE;
        break;
        default:
            rv = FALSE;
    }
    return rv;
}


STATIC int32
_phy_82328_intf_line_sys_init(int32 unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
	phy82328_intf_cfg_t  *line_intf;

    pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

	return _phy_82328_speed_set(unit, port, line_intf->speed);
}

/*
 * Function:
 *      phy_82328_speed_set
 * Purpose:
 *      Set PHY speed
 *      For 20G speed make sure serdes configured as 20G MLD
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */
 
STATIC int32
phy_82328_speed_set(int32 unit, soc_port_t port, int32 speed)
{
    int32 rv;
    phy_ctrl_t  *pc;
    phy82328_intf_cfg_t  *line_intf;
    phy82328_intf_cfg_t  *sys_intf;
 
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
 
    pc = EXT_PHY_SW_STATE(unit, port);
    line_intf = &(LINE_INTF(pc));
    sys_intf = &(SYS_INTF(pc));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_82328_speed_set: u=%d p=%d speed=%d\n"),
                         unit, port,speed));
    if (PHY82328_SINGLE_PORT_MODE(pc)) {
        if (!_phy_82328_intf_is_single_port(line_intf->type)) {
             LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 speed set does not match interface: "
                                  "u=%d p=%d speed=%d intf=%d\n"),
                       unit, port, speed, sys_intf->type));
             return SOC_E_PARAM;
        }
        /*Single port mode can supported 40/40HG/42HG/100G*/
        switch (speed) {
            case PHY82328_SPD_40G: 
                /* Both 40G And 40HG share same
                 * register config */
            break;
            case PHY82328_SPD_42G:
                if (! IS_HG_PORT(unit, port)) {
                    speed = PHY82328_SPD_40G;
                }
            break;
            case PHY82328_SPD_100G:  
            break;
            default:
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "82328  invalid speed Single Port: u=%d p=%d speed=%d\n"),
                           unit, port, speed));
            return SOC_E_PARAM;
        }
    } else {
        /*Check for the valid interface*/
        if(_phy_82328_intf_is_single_port(line_intf->type)){
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328  invalid intf in quad port: u=%d p=%d intf=%d\n"),
                       unit, port, line_intf->type));
            return SOC_E_PARAM;
		}
        switch (speed) {
            case PHY82328_SPD_1G:            
                if(!_phy_82328_intf_type_1G(line_intf->type)){
                    line_intf->type = SOC_PORT_IF_GMII;
                }
                /*
                 * Changing speed to 1000
                 * - if backplane interface(KR) then change to KX
                 * - if not KX, then set to GMII
                 */
                if (sys_intf->type == SOC_PORT_IF_KR) {
                    sys_intf->type = SOC_PORT_IF_KX;
                } else if (sys_intf->type != SOC_PORT_IF_KX) {
                    sys_intf->type = line_intf->type;
                }
            break;
            case 100:  
            case 10:
                if (line_intf->type != SOC_PORT_IF_SGMII) {
                    line_intf->type = SOC_PORT_IF_SGMII;
                }
                sys_intf->type = line_intf->type;
            break;
				
            /*10G/11G LR/SR/CR XFI/XFI DFE/Forced CL72*/
            case PHY82328_SPD_11G:
                /* 11G only valid for HG[11] */
                if (!IS_HG_PORT(unit, port)) {
                    speed = PHY82328_SPD_10G;
                }
            break;
            case PHY82328_SPD_10G:
                /* If changing from 1000 to 10000/11000, then adjust interface*/
                if (line_intf->speed <= PHY82328_SPD_1G) {
                    /* If changing speed from 1000 to 10000, set line interface to SR*/
                    if (!_phy_82328_intf_type_10G(line_intf->type)) {
                        line_intf->type = SOC_PORT_IF_SR;
                    }
                    if (!_phy_82328_intf_type_10G(sys_intf->type)) {
                        sys_intf->type = SOC_PORT_IF_XFI;
                    }
                }
            break;
            case PHY82328_SPD_40G:
            case PHY82328_SPD_42G:/*42G*/
            case PHY82328_SPD_100G: 
            default:
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "82328  invalid speed Quad Port: u=%d p=%d speed=%d\n"),
                           unit, port, speed));
            return SOC_E_PARAM;
        }
    }
	
    rv =  _phy_82328_speed_set(unit, port, speed);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "82328  %s failed: u=%d p=%d\n"), FUNCTION_NAME(), unit, port));
    }
    return rv;
}

/*
 * Function:
 *      phy_82328_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - current link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */
 
STATIC int32
phy_82328_speed_get(int32 unit, soc_port_t port, int32 *speed)
{
    phy82328_intf_cfg_t *line_intf;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	
    if ((pc !=NULL )&& (speed !=NULL)) {
        line_intf = &(LINE_INTF(pc));
        *speed = line_intf->speed;
    } else {
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

/*
 * Directly select channel to access based on interface side and lane number.
 */
STATIC int32
_phy_82328_channel_direct_select(int32 unit, soc_port_t port, phy82328_intf_side_t side, int32 lane)
{
    uint16      lane_sel;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * If all lanes, restore single PMD access with lane 0 broadcast to all lanes
     * For single lane, select lane in Single PMD 1.ca86[5:4] and [3..0] that corresponds
     * to the lane with direct lane selection in 1.ca86[8]
     */
    if (lane == PHY82328_ALL_LANES) {
        lane_sel = 
            (LOGICAL_LANE0(pc) << MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_SHIFT) |
            MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK |
            MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_MASK;
    } else {
        lane_sel = (lane << MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_SHIFT) |
            ((1 << lane) & MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_MASK);
    }
    SOC_IF_ERROR_RETURN(
        _phy_82328_modify_pma_pmd_reg (unit, port, pc, 
        PHY82328_DEV_PMA_PMD,  MGT_TOP_SINGLE_PMD_CTRL, 
        MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_SHIFT +3/*+ 5*/, 
        MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_SHIFT, lane_sel));
    READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, 0xca86, &lane_sel);
    return SOC_E_NONE;
}

/*
 * Select channel to access based on interface side and lane number.
 * The side is selected before returning.
 */
STATIC int32
_phy_82328_channel_select(int32 unit, soc_port_t port, phy82328_intf_side_t side, int32 lane)
{
    int32 rv;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if (!(PHY82328_SINGLE_PORT_MODE(pc))) {
        return SOC_E_PARAM;
    }

    if (!((lane == PHY82328_ALL_LANES) || ((lane >= 0) && (lane <= 3)))) {
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, side));

    rv  = _phy_82328_channel_direct_select(unit, port, side, lane);

    return rv;
}

/*
 *   20-bit: set txpol in line side 1.DOE3[0]
 *   4-bit:  set txpol in line side 1.D0A0[9]
 */
STATIC int32
_phy_82328_polarity_flip_tx_set(int32 unit, soc_port_t port, int32 flip)
{	
    uint16		data = 0;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	/* Clear to flip, set to unflip */
    if (CUR_DATAPATH(pc) == PHY82328_DATAPATH_20) {
        data = flip ? TLB_RX_TLB_RX_MISC_CONFIG_RX_PMD_DP_INVERT_MASK : 0;
        SOC_IF_ERROR_RETURN(
          _phy_82328_modify_pma_pmd_reg (unit, port, pc, 
          PHY82328_DEV_PMA_PMD,  TLB_TX_TLB_TX_MISC_CONFIG, 
          TLB_TX_TLB_TX_MISC_CONFIG_TX_PMD_DP_INVERT_SHIFT, 
          TLB_TX_TLB_TX_MISC_CONFIG_TX_PMD_DP_INVERT_SHIFT, data));
    } else {
        data = flip ? 1 : 0;
        SOC_IF_ERROR_RETURN(
          _phy_82328_modify_pma_pmd_reg (unit, port, pc, 
          PHY82328_DEV_PMA_PMD,  AMS_TX_TX_CTRL_0,
          AMS_TX_TX_CTRL_0_LL_POLARITY_FLIP_SHIFT, 
          AMS_TX_TX_CTRL_0_LL_POLARITY_FLIP_SHIFT, data));
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_polarity_flip_tx(int32 unit, soc_port_t port, phy82328_intf_side_t side, uint16 cfg_pol)
{
    int32			flip;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    int32	lane = 0;
    
    if (side != PHY82328_INTF_SIDE_LINE) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, side));
    }
    if ((PHY82328_SINGLE_PORT_MODE(pc))) {
        if (cfg_pol != PHY82328_POL_DND) {
            for (lane = 0; lane < PHY82328_NUM_LANES; lane++) {
	            /* Check for all lanes or whether this lane has polarity flipped */
                if ((cfg_pol & POL_CONFIG_LANE_MASK(lane)) == POL_CONFIG_LANE_MASK(lane)) {
				    flip = 1;
                } else {
                    flip = 0;
                }
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "82328 tx polarity flip=%d: u=%d p=%d" 
                                     "lane=%d\n"), flip, unit, port, lane));

			    /* Select the lane on the line side */
                SOC_IF_ERROR_RETURN
                    (_phy_82328_channel_select(unit, port, side, lane));
                SOC_IF_ERROR_RETURN
                    (_phy_82328_polarity_flip_tx_set(unit, port, flip));
            }
            /* Restore to default single port register access */
            SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, PHY82328_INTF_SIDE_LINE, 
                                PHY82328_ALL_LANES));
        }
    } 
    else {  /* quad mode */
        if (cfg_pol != PHY82328_POL_DND) {
            flip = (cfg_pol) ? 1 : 0;
            SOC_IF_ERROR_RETURN
                (_phy_82328_polarity_flip_tx_set(unit, port, flip));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "82328  polarity flip: u=%d p=%d\n"), unit, port));
        }
	}
    if (side != PHY82328_INTF_SIDE_LINE) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    }

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_polarity_flip_rx_20bit(int32 unit, soc_port_t port, int32 flip)
{
    uint16		data = 0;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    data = flip ? 1 : 0;
    SOC_IF_ERROR_RETURN(
          _phy_82328_modify_pma_pmd_reg (unit, port, pc, 
          PHY82328_DEV_PMA_PMD,  TLB_RX_TLB_RX_MISC_CONFIG, 
          TLB_RX_TLB_RX_MISC_CONFIG_RX_PMD_DP_INVERT_SHIFT, 
          TLB_RX_TLB_RX_MISC_CONFIG_RX_PMD_DP_INVERT_SHIFT, data));

    return SOC_E_NONE;
}
			
STATIC int32
_phy_82328_polarity_flip_rx(int32 unit, soc_port_t port, phy82328_intf_side_t side, uint16 cfg_pol)
{
    int32  flip;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    int32  lane;

    if (side != PHY82328_INTF_SIDE_LINE) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, side));
    }
    if ((PHY82328_SINGLE_PORT_MODE(pc))) {
        if (cfg_pol != PHY82328_POL_DND) {	
            /* 4-bit polarity handled on system side */
     		if (CUR_DATAPATH(pc) != PHY82328_DATAPATH_20) {
                PHY_82328_MICRO_PAUSE(unit, port, "polarity rx");
            }
            for (lane = 0; lane < PHY82328_NUM_LANES; lane++) {
                flip = 0;
                if ((cfg_pol & POL_CONFIG_LANE_MASK(lane)) == POL_CONFIG_LANE_MASK(lane)) {
                    flip = 1;
                }
                if (CUR_DATAPATH(pc) == PHY82328_DATAPATH_20) {
                    SOC_IF_ERROR_RETURN
                        (_phy_82328_channel_select(unit, port, side, lane));
                    SOC_IF_ERROR_RETURN
                        (_phy_82328_polarity_flip_rx_20bit(unit, port, flip));
                }
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "82328 rx polarity flip: u=%d p=%d lane=%d\n"), unit, port, lane));
            }
            /* Restore to default single port register access */
            SOC_IF_ERROR_RETURN
                (_phy_82328_channel_select(unit, port, PHY82328_INTF_SIDE_LINE, PHY82328_ALL_LANES));
            if (CUR_DATAPATH(pc) != PHY82328_DATAPATH_20) {
                PHY_82328_MICRO_RESUME(unit, port);
            }
        }
    }

    else {  /* quad mode */
        if (cfg_pol != PHY82328_POL_DND) {
            flip = (cfg_pol)? 1:0;
            SOC_IF_ERROR_RETURN(_phy_82328_polarity_flip_rx_20bit(unit, port, flip));
        }
    }
    
    if (side != PHY82328_INTF_SIDE_LINE) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328 rx polarity flip: u=%d p=%d\n"), unit, port));
    return SOC_E_NONE;
}

/*                                                                                              
 * Flip configured tx/rx polarities
 *
 * Property values are either 1 for all lanes or
 * lane 0: 0001
 * lane 1: 0002
 * lane 2: 0004
 * lane 3: 0008
 */
STATIC int32
_phy_82328_polarity_flip(int32 unit, soc_port_t port, uint16 cfg_tx_pol, uint16 cfg_rx_pol)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328 polarity flip: u=%d p%d TX=%x RX=%x\n"), unit, port, 
                         cfg_tx_pol, cfg_rx_pol));
    if (cfg_tx_pol != PHY82328_POL_DND) {
        SOC_IF_ERROR_RETURN(
            _phy_82328_polarity_flip_tx(unit, port, PHY82328_INTF_SIDE_LINE, cfg_tx_pol));
    }
    if (cfg_rx_pol != PHY82328_POL_DND) {
        SOC_IF_ERROR_RETURN(
            _phy_82328_polarity_flip_rx(unit, port, PHY82328_INTF_SIDE_LINE, cfg_rx_pol));
    }

    return SOC_E_NONE;
}

STATIC void
_phy_82328_micro_pause(int32 unit, soc_port_t port, const char *loc)
{
    int32 rv = SOC_E_NONE;
    uint16 data, mask;
    int32 checks;
    uint16 csr;
    uint16 saved_side;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    phy82328_micro_ctrl_t *micro_ctrl = &(MICRO_CTRL(pc));

    if (!micro_ctrl->enable) {
        return;
    }

    /* Access line side registers */
    saved_side = _phy_82328_intf_side_regs_get(unit, port);
    if (saved_side == PHY82328_INTF_SIDE_SYS) {
        WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL,PHY82328_INTF_SIDE_LINE);
    }

    micro_ctrl->count++;
    
    /* Quiet micro */
    data = 0;
    mask = 0xff00;
    rv = MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_0, data, mask);
    if (rv != SOC_E_NONE) {
        goto fail;
    }
    sal_udelay(500);
    checks = 0;
    while (checks < 1000) {
        /* Make sure micro really paused */
        rv = READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_2, &data);
        if (rv != SOC_E_NONE) {
            goto fail;
        }
        if ((data & mask) == 0) {
            break;
        }
        sal_udelay(100);
        checks++;
    }
    if (data & mask) {
        rv = READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_0, &csr);
        if (rv != SOC_E_NONE) {
            goto fail;
        }
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "82328 microcode did not pause in %s: "
                             "u=%d p%d 1.ca18/1.ca19=%04x/%04x checks=%d\n"), 
                  loc, unit, port, csr, data, checks));
    }

fail:
    if (saved_side != PHY82328_INTF_SIDE_LINE) {
        WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL, saved_side);
    }
    return;
}

STATIC void
_phy_82328_micro_resume(int32 unit, soc_port_t port)
{
    phy82328_intf_side_t saved_side;
    int32 rv = SOC_E_NONE;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    phy82328_micro_ctrl_t *micro_ctrl = &(MICRO_CTRL(pc));

    if (!micro_ctrl->enable) {
        return;
    }

    /* Access line side registers */
    saved_side = _phy_82328_intf_side_regs_get(unit, port);
    if (saved_side == PHY82328_INTF_SIDE_SYS) {
        WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL, PHY82328_INTF_SIDE_LINE);
	}

    micro_ctrl->count--;
    if (micro_ctrl->count <= 0) {
        rv = MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_0, 
                                             0xff00, 0xff00);
        if (rv != SOC_E_NONE) {
            goto fail;
        }
        if (micro_ctrl->count < 0) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "82328 unmatched micro resume\n")));
            micro_ctrl->count = 0;;
        }
    }

fail:
    if (saved_side != PHY82328_INTF_SIDE_LINE) {
        WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL, saved_side);
    }
    return;
}

/*
 * Function:
 *      phy_82328_an_set
 * Purpose:
 *      Configures auto-negotiation 
 *      Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable   - if true, auto-negotiation is enabled.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int32
phy_82328_an_set(int32 unit, soc_port_t port, int32 an)
{
	int32 dev_an, dev_an_done;
	phy_ctrl_t  *pc, *int_pc;
	phy82328_intf_cfg_t  *line_intf;

	pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_82328_an_set: u=%d p=%d an=%s\n"),
              unit, port, an ? "enable" : "disable"));

	/*
	 * Determining line side AN status 
	 *  SR/LR/SR4/LR4/SR2: no AN 
	 *  KR/KR4/KR2: line side: cl73/cl72, system side as forced cl72
	 *  CR4/CR: AN enabled in FW, to disable set interface to 40G CR4 DAC(aka XLAUI_DFE), otherwise, CR4
	 *  GMII: AN handled by internal SerDes
	 */
	switch (line_intf->type) {
	    case SOC_PORT_IF_GMII:
        case SOC_PORT_IF_SGMII:
	        AN_EN(pc) = an;
            SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET(int_pc->pd, unit, port, an));
		break;
	    case SOC_PORT_IF_KX:
        case SOC_PORT_IF_KR:
        case SOC_PORT_IF_KR4:
           AN_EN(pc) = 1;
		break;
        case SOC_PORT_IF_CR4:
           AN_EN(pc) = an;
		break;
        default:
		    /* Interface does not support AN in the firmware so it's always forced */
            AN_EN(pc) = FALSE;
	}
	SOC_IF_ERROR_RETURN(_phy_82328_intf_type_set(unit, port, line_intf->type, TRUE));

    /* If not passtrhu an, make sure firmware has enabled an */
    if (line_intf->speed > 1000) {
        SOC_IF_ERROR_RETURN(phy_82328_an_get(unit, port, &dev_an, &dev_an_done));
        if (dev_an != AN_EN(pc)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 device autonegotiation mismatch: u=%d p=%d an=%s\n"),
                       unit, port, AN_EN(pc) ? "en" : "dis"));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82328_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:
 *      SOC_E_XXX
 */
 
STATIC int32
phy_82328_an_get(int32 unit, soc_port_t port, int32 *an, int32 *an_done)
{
    uint16 data16 = 0;
    phy_ctrl_t  *pc, *int_pc;
    phy82328_intf_cfg_t  *line_intf;

	pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

	if ((line_intf->type == SOC_PORT_IF_GMII) || (line_intf->type == SOC_PORT_IF_SGMII)) {
		SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_GET(int_pc->pd, unit, port, an, an_done));
	} else {   
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_AN_REG(unit, pc,
                                     PHY82328_AN_CTRL_REG, &data16));
        *an = ((data16 & PHY82328_AN_ENABLE_MSK) == PHY82328_AN_ENABLE_MSK);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82328: AN Enable: 0x%x\n"),data16));
        if (*an) {
            SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_AN_REG(unit, pc,
                               PHY82328_AN_STS_REG, &data16));

            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY82328 AN Done: 0x%x\n"),data16));
           *an_done = ((data16 & PHY82328_AN_COMPLETE_MSK) == PHY82328_AN_COMPLETE_MSK);
        }
    }

    return SOC_E_NONE;
}

/*
 * Squelch/unsquelch tx in 1.reg.bit
 */
STATIC int32
_phy_82328_tx_squelch_enable(int32 unit, soc_port_t port, phy82328_intf_side_t side, uint8 squelch)
{
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN( 
          _phy_82328_modify_pma_pmd_reg (unit, port, pc, 
           PHY82328_DEV_PMA_PMD,  TX_ANATXACONTROL7, 
           QSFI_TX_ANATXACONTROL7_TX_DISABLE_FORCE_EN_SM_SHIFT,
           QSFI_TX_ANATXACONTROL7_TX_DISABLE_FORCE_EN_SM_SHIFT, 1));
 
    SOC_IF_ERROR_RETURN( 
          _phy_82328_modify_pma_pmd_reg (unit, port, pc, 
           PHY82328_DEV_PMA_PMD,  TX_ANATXACONTROL7, 
           QSFI_TX_ANATXACONTROL7_TX_DISABLE_FORCE_VAL_SM_SHIFT, 
           QSFI_TX_ANATXACONTROL7_TX_DISABLE_FORCE_VAL_SM_SHIFT, squelch? 1:0));

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_tx_squelch(int32 unit, soc_port_t port, phy82328_intf_side_t side, int32 cfg_sql)
{
    uint8  squelch = 0;
    int32	lane = 0;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, side));
    if (PHY82328_SINGLE_PORT_MODE(pc)) {
        for (lane = 0; lane < PHY82328_NUM_LANES; lane++) {
            /* Check for all lanes or whether this lane has to be squelched */
            if(cfg_sql & (1<<lane)){
                squelch = 1;
            } else {
                squelch = 0;
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "82328 tx squelch=%d: u=%d p=%d lane=%d\n"), squelch, unit, port, lane));

            /* Select the lane on the line side */
            SOC_IF_ERROR_RETURN(_phy_82328_channel_select (unit, port, side, lane));
            SOC_IF_ERROR_RETURN(_phy_82328_tx_squelch_enable (unit, port, side, squelch));
        }
        /* Restore to default single port register access */
        SOC_IF_ERROR_RETURN(_phy_82328_channel_select (unit, port, PHY82328_INTF_SIDE_LINE, PHY82328_ALL_LANES));
    } 
    else {  
        squelch = (cfg_sql) ? 1 : 0;
        SOC_IF_ERROR_RETURN(_phy_82328_tx_squelch_enable (unit, port, side, squelch));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "82328 squelch %s side: u=%d p=%d \n"),
                  side ? "SYSTEM" : "LINE", unit, port));
    }

    if (side != PHY82328_INTF_SIDE_LINE) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    _phy_82328_remote_loopback_get
 * Purpose:
 *    Get 82328 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    enable - (OUT) address of location to store binary value for on/off (1/0)
 * Returns:
 *    SOC_E_NONE
 */ 
 STATIC int32
_phy_82328_remote_loopback_get(int32 unit, soc_port_t port, uint32 *enable)
{
    uint16 read_data=0,ret_val = 1;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
		
    /* Remote Loop Back Set <Line Side>*/
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));

    /*1 rmt_lpbk_config.rmt_lpbk_en 1.D0E2.0 1*/
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                  TLB_TX_RMT_LPBK_CONFIG, &read_data));	

    read_data &=TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_MASK;
    read_data = read_data >> TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_SHIFT;
    ret_val &=read_data;		
    
    /*2 tx_pi_control_5.tx_pi_pass_thru_sel 1.D075.2 0*/		
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                TX_PI_TX_PI_CONTROL_5, &read_data));

    read_data &=TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_MASK;
    read_data = read_data >> TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_SHIFT;
    ret_val &=(!read_data);													 
    
    /*3 tx_pi_control_0.tx_pi_ext_ctrl_en 1.D070.2 1*/
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                 TX_PI_TX_PI_CONTROL_0, &read_data));
    read_data &=TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_MASK;
    read_data = read_data >> TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_SHIFT;
    ret_val &=read_data;
		
    *enable = ret_val;		
    return SOC_E_NONE;
}

/*
 * Function:
 *    _phy_82328_remote_loopback_set
 * Purpose:
 *    Get 82328 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    enable - address of location to store binary value for on/off (1/0)
 * Returns:
 *    SOC_E_NONE
 */
 STATIC int32
_phy_82328_remote_loopback_set(int32 unit,soc_port_t port,int32 enable)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    /* Loopback requires 20-bit datapath */	
    if (CUR_DATAPATH(pc) != PHY82328_DATAPATH_20) {
        return SOC_E_RESOURCE;
    }

    /* Remote LoopBk Set <Line Side>*/
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
        SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                 MGT_TOP_GP_REG_0,
                 (enable << MGT_TOP_CMN_CTLR_GP_REG_0_DISABLE_TX_SQUELCHING_SHIFT),
                 MGT_TOP_CMN_CTLR_GP_REG_0_DISABLE_TX_SQUELCHING_MASK));
 
        if (enable) {
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                      TX_ANATXACONTROL7, 0,
                                                      QSFI_TX_ANATXACONTROL7_TX_DISABLE_FORCE_VAL_SM_MASK));

            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                      TX_ANATXACONTROL7, 0,
                                                      QSFI_TX_ANATXACONTROL7_TX_DISABLE_FORCE_EN_SM_MASK));

            /*1 rmt_lpbk_config.rmt_lpbk_en 1.D0E2.0 1*/
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                                TLB_TX_RMT_LPBK_CONFIG,
                                                                (1<<TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_SHIFT), 
                                                                TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_MASK));
            /*2 tx_pi_control_5.tx_pi_pass_thru_sel 1.D075.2 0*/		
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                                TX_PI_TX_PI_CONTROL_5,
                                                                0, 
                                                                TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_MASK));
            sal_udelay(100);
            /*3 tx_pi_control_0.tx_pi_ext_ctrl_en 1.D070.2 1*/
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                                TX_PI_TX_PI_CONTROL_0,
                                                                (1<<TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_SHIFT), 
                                                                TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_MASK));				
            SOC_IF_ERROR_RETURN
                 (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
                                                 PHY82328_DEV_PMA_PMD, TX_PI_TX_PI_CONTROL_0,
                                                 TX_PI_TX_PI_CONTROL_0_TX_PI_LOOP_TIMING_EN_SHIFT,
                                                 TX_PI_TX_PI_CONTROL_0_TX_PI_EN_SHIFT, 0x3));
        } else {
            /*1 rmt_lpbk_config.rmt_lpbk_en 1.D0E2.0 1*/
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                                TLB_TX_RMT_LPBK_CONFIG,
                                                                0, 
                                                                TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_MASK));
            /*2 tx_pi_control_5.tx_pi_pass_thru_sel 1.D075.2 0*/		
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                                TX_PI_TX_PI_CONTROL_5,
                                                                (1<<TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_SHIFT), 
                                                                TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_MASK));
            sal_udelay(100);
            /*3 tx_pi_control_0.tx_pi_ext_ctrl_en 1.D070.2 1*/
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                                TX_PI_TX_PI_CONTROL_0,
                                                                0, 
                                                                TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_MASK));	
        }
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_82328_lb_get
 * Purpose:
 *    Get 82328 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    enable - (OUT) address of location to store binary value for on/off (1/0)
 * Returns:
 *    SOC_E_NONE
 */ 
STATIC int32
phy_82328_lb_get(int32 unit, soc_port_t port, int32 *enable)
{
    uint16 read_data=0,ret_val=1;		
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    *enable = 0xFFFF;	
    /* LB Set API Supports System Side*/
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_SYS));

        /*1 rmt_lpbk_config.rmt_lpbk_en 1.D0E2.0 1*/
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG (unit, pc,
                         TLB_TX_RMT_LPBK_CONFIG, &read_data));
    
        read_data &=TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_MASK;
        read_data = read_data >> TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_SHIFT;
	    ret_val &=read_data;
    
        /*2 tx_pi_control_5.tx_pi_pass_thru_sel 1.D075.2 0*/
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                  TX_PI_TX_PI_CONTROL_5, &read_data));

        read_data &=TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_MASK;
        read_data = read_data >> TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_SHIFT;
        ret_val &=(!read_data);

        /*3 tx_pi_control_0.tx_pi_ext_ctrl_en 1.D070.2 1*/

        SOC_IF_ERROR_RETURN (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                            TX_PI_TX_PI_CONTROL_0, &read_data));

        read_data &=TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_MASK;
        read_data = read_data >> TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_SHIFT;
        ret_val &=read_data;
        
        *enable &= ret_val;

    SOC_IF_ERROR_RETURN(
       phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    return SOC_E_NONE;
}


STATIC int32
phy_82328_lb_set(int32 unit, soc_port_t port, int32 enable)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    /* Loopback requires 20-bit datapath */	
    if (CUR_DATAPATH(pc) != PHY82328_DATAPATH_20) {
        return SOC_E_RESOURCE;
    }
     /* LB Set API Supports System Side*/
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_SYS));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL, PHY82328_INTF_SIDE_LINE));
        SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
             MGT_TOP_GP_REG_0,
             (enable << MGT_TOP_CMN_CTLR_GP_REG_0_DISABLE_TX_SQUELCHING_SHIFT),
             MGT_TOP_CMN_CTLR_GP_REG_0_DISABLE_TX_SQUELCHING_MASK));
        
        SOC_IF_ERROR_RETURN
            (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_XPMD_REGS_SEL, PHY82328_INTF_SIDE_SYS));

        if (enable) {
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                  TX_ANATXACONTROL7, 0,
                                                  QSFI_TX_ANATXACONTROL7_TX_DISABLE_FORCE_VAL_SM_MASK));

            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                  TX_ANATXACONTROL7, 0,
                                                  QSFI_TX_ANATXACONTROL7_TX_DISABLE_FORCE_EN_SM_MASK));
             SOC_IF_ERROR_RETURN
             (_phy_82328_modify_pma_pmd_reg (unit, port, pc, 
                                             PHY82328_DEV_PMA_PMD, TX_PI_TX_PI_CONTROL_0,
                                             TX_PI_TX_PI_CONTROL_0_TX_PI_LOOP_TIMING_EN_SHIFT,
                                             TX_PI_TX_PI_CONTROL_0_TX_PI_EN_SHIFT, 0x3));

            /*1 rmt_lpbk_config.rmt_lpbk_en 1.D0E2.0 1*/
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                            TLB_TX_RMT_LPBK_CONFIG,
                                                            (1<<TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_SHIFT), 
                                                            TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_MASK));
            sal_udelay(100);
            /*2 tx_pi_control_5.tx_pi_pass_thru_sel 1.D075.2 0*/		
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                            TX_PI_TX_PI_CONTROL_5,
                                                            0, 
                                                            TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_MASK));

            /*3 tx_pi_control_0.tx_pi_ext_ctrl_en 1.D070.2 1*/
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                            TX_PI_TX_PI_CONTROL_0,
                                                            (1<<TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_SHIFT),
                                                            TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_MASK));
        } else {
            /*1 rmt_lpbk_config.rmt_lpbk_en 1.D0E2.0 1*/
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                            TLB_TX_RMT_LPBK_CONFIG,
                                                            0, 
                                                            TLB_TX_RMT_LPBK_CONFIG_RMT_LPBK_EN_MASK));
            /*2 tx_pi_control_5.tx_pi_pass_thru_sel 1.D075.2 0*/		
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                            TX_PI_TX_PI_CONTROL_5,
                                                            (1<<TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_SHIFT), 
                                                            TX_PI_TX_PI_CONTROL_5_TX_PI_PASS_THRU_SEL_MASK));
            sal_udelay(100);
            /*3 tx_pi_control_0.tx_pi_ext_ctrl_en 1.D070.2 1*/
            SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                                            TX_PI_TX_PI_CONTROL_0,
                                                            0, 
                                                            TX_PI_TX_PI_CONTROL_0_TX_PI_EXT_CTRL_EN_MASK));	
        }
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_intf_type_set(int32 unit, soc_port_t port, soc_port_if_t pif, int32 must_update)
{
    int32			rv = SOC_E_NONE;
    phy_ctrl_t      *pc = EXT_PHY_SW_STATE(unit, port);
    phy82328_intf_cfg_t *line_intf;
    int32			update = 0;
    uint16          reg_data = 0, reg_mask = 0;
    uint16          data = 0, mask = 0;

    line_intf = &(LINE_INTF(pc));
	
    reg_data = 0;
    reg_mask = 0;

    if (_phy_82328_intf_is_single_port(pif)) {
        if (!PHY82328_SINGLE_PORT_MODE(pc)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 invalid interface for quad port: u=%d p=%d\n"),
                                  unit, port));
            return SOC_E_CONFIG;
        }
        /* 
         * If rx polarity is flipped and the interface is CR4, then change the datapath
         * to 20-bit as rx polarity cannot be handle on the line side in 4-bit
         */
        if (pif == SOC_PORT_IF_CR4) {
            if (AN_EN(pc)) {
                if (POL_RX_CFG(pc) && (CUR_DATAPATH(pc) != PHY82328_DATAPATH_20)) {
                    CUR_DATAPATH(pc) = PHY82328_DATAPATH_20;
                    update = 1;
                }
            } else if (CUR_DATAPATH(pc) != CFG_DATAPATH(pc)) {
                CUR_DATAPATH(pc) = CFG_DATAPATH(pc);
                update = 1;
            }
            SOC_IF_ERROR_RETURN(
                _phy_82328_intf_datapath_reg_get(unit, port, CUR_DATAPATH(pc), &data, &mask));
            reg_data |= data;
            reg_mask |= mask;
        }
        if ((pif != line_intf->type) || must_update) {
            update = 1;
        }
    } else if (_phy_82328_intf_is_quad_port(pif)) {
        if (PHY82328_SINGLE_PORT_MODE(pc)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 invalid interface for single port: u=%d p=%d\n"),
                                  unit, port));
            return SOC_E_CONFIG;
        }

        if ((line_intf->type != pif) || must_update) {
            update = 1;
            SOC_IF_ERROR_RETURN(
               _phy_82328_intf_type_reg_get(unit, port, pif, PHY82328_INTF_SIDE_LINE,
                                           &reg_data, &reg_mask));
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "82328 invalid interface for port: u=%d p=%d intf=%d\n"),
                              unit, port, pif));
        return SOC_E_CONFIG;
    }
    if (update) {
        line_intf->type = pif;
        SOC_IF_ERROR_RETURN(
            _phy_82328_intf_type_reg_get(unit, port, pif, PHY82328_INTF_SIDE_LINE,
                                             &data, &mask));
        reg_data |= data;
        reg_mask |= mask;
        SOC_IF_ERROR_RETURN(_phy_82328_intf_update(unit, port, reg_data, reg_mask));
    }
    return rv;
}

STATIC int32
_phy_82328_intf_line_forced(int32 unit, soc_port_t port, soc_port_if_t intf_type)
{
    int32 rv;
    switch (intf_type) {
        case SOC_PORT_IF_SR:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_LR:
        case SOC_PORT_IF_ZR:
        case SOC_PORT_IF_LR4:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SFI:
        case SOC_PORT_IF_CR:
            rv = TRUE;
        break;
        default:
            rv = FALSE;
    }
    return rv;
}

/*
 * Function:
 *      phy_82328_interface_set
 * Purpose:
 *
 *
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_UNAVAIL - unsupported interface
 */
 
STATIC int32
phy_82328_interface_set(int32 unit, soc_port_t port, soc_port_if_t pif)
{
    int32 rv;

    LOG_CLI((BSL_META_U(unit,
                        "82328 interface set: u=%d p=%d pif=%s\n"), 
             unit, port, phy82328_intf_names[pif]));

    if (pif == SOC_PORT_IF_XGMII) {
        return phy_null_interface_set(unit, port, pif);
    }

    rv = _phy_82328_intf_type_set(unit, port, pif, FALSE);
    if (SOC_FAILURE(rv)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "82328  interface set check failed: u=%d p=%d\n"),
                             unit, port));
    }
    if (_phy_82328_intf_line_forced(unit, port, pif)) {
        SOC_IF_ERROR_RETURN(phy_82328_an_set(unit, port, 0));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82328_interface_get
 * Purpose:
 *      Get the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
phy_82328_interface_get(int32 unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    phy82328_intf_cfg_t *line_intf;

    line_intf = &(LINE_INTF(pc));
    *pif = line_intf->type;

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_micro_tx_squelch_enable(int32 unit, soc_port_t port, int32 enable)
{
    uint16 data;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    /* 1.ca18[7] 0=micro squelching enabled, 1=micro squelching disabled */
    data = enable ? 0 : 0x80;
        SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_0, data, 0x80));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_82328_control_prbs_enable_set
 * Purpose:
 *      Get the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - PRBS Generator Enable
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_prbs_enable_set(int32 unit, soc_port_t port, int32 enable)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    /* PRBS requires 20-bit datapath */	
    if (CUR_DATAPATH(pc) != PHY82328_DATAPATH_20) {
        return SOC_E_RESOURCE;
    }
        /* Disable Prbs Gen/Chkr First -> Refer Programming Guide 1.2*/
        /* Disable Prbs Gen in TLB_TX_PRBS_GEN_CONFIG 1.D0E1.1 */
        SOC_IF_ERROR_RETURN

            (MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                   TLB_TX_PRBS_GEN_CONFIG, 0, TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_EN_MASK));
	    														
        /* Disable Prbs Chkr in in TLB_RX_PRBS_CHK_CONFIG 1.D0D1.0*/	  
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, 
                   TLB_RX_PRBS_CHK_CONFIG, 0, TLB_RX_PRBS_CHK_CONFIG_PRBS_CHK_EN_MASK));

        if (enable) {
            /* Enable Prbs Gen in TLB_TX_PRBS_GEN_CONFIG 1.D0E1.0 */
            /*PRBS Gen Enable*/
            SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, TLB_TX_PRBS_GEN_CONFIG,
                    (1 << TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_EN_SHIFT), 
                    TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_EN_MASK));
	    			
            /*Prbs Checker En in TLB_RX_PRBS_CHK_CONFIG 1.D0D1.0*/
            /*PRBS Chk Enable*/
            SOC_IF_ERROR_RETURN (
               MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                TLB_RX_PRBS_CHK_CONFIG, (1 << TLB_RX_PRBS_CHK_CONFIG_PRBS_CHK_EN_SHIFT), 
                TLB_RX_PRBS_CHK_CONFIG_PRBS_CHK_EN_MASK));
        }
    /*
     * If enabling prbs
     *    disable micro tx squelch updates
     *    unsquelch tx system
     * else
     *    enable micro tx squelch updates
     */
    SOC_IF_ERROR_RETURN(_phy_82328_micro_tx_squelch_enable(unit, port, !enable));
    if (enable) {
        SOC_IF_ERROR_RETURN(_phy_82328_tx_squelch(unit, port, PHY82328_INTF_SIDE_SYS, 0));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_82328_control_prbs_invert_data_set
 * Purpose:
 *      Get the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      invert - PRBS Invert
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_prbs_invert_data_set(int32 unit, soc_port_t port, int32 invert)
{
    uint16          data;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
 
    data = invert ? (1 << TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_INV_SHIFT):0;
        /*PRBS Gen Invert*/
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, 
                  TLB_TX_PRBS_GEN_CONFIG, data,
                  TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_INV_MASK));

        /*PRBS Chk Invert*/
        SOC_IF_ERROR_RETURN(
             MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, 
                    TLB_RX_PRBS_CHK_CONFIG, data,
                    TLB_RX_PRBS_CHK_CONFIG_PRBS_CHK_INV_MASK));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_82328_control_prbs_polynomial_set
 * Purpose:
 *      Get the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      polynomial - PRBS Polynomial
 *       0==prbs7, 
 *       1==prbs=15 
 *       2==prbs=23 
 *       3==prbs=31 
 *
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_prbs_polynomial_set(int32 unit, soc_port_t port, int32 polynomial)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328  prbs polynomial set: u=%d p=%d polynomial=%d\n"),
                         unit, port, polynomial));
    switch(polynomial) {
        case 0: /* PRBS 7*/
            polynomial = 0;
            break;
        case 1: /*PRBS15*/
            polynomial = 3;
            break;
        case 2:/*PRBS23*/
            polynomial = 4;
            break;
        case 3:/*PRBS31*/
            polynomial = 5;
            break;
        case 4:/*PRBS 9*/
            polynomial = 1;
            break;
        case 5:/*PRBS 11*/
            polynomial = 2;
            break;
        case 6: /*PRBS58*/
            polynomial = 6;
            break;
        default:
            return SOC_E_PARAM;
    }
        /* Prbs Polynomial in TLB_TX_PRBS_GEN_CONFIG 1.D0E1.[3:1] */
        /*Set Gen Poly*/
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, TLB_TX_PRBS_GEN_CONFIG,
                    (polynomial << TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_MODE_SEL_SHIFT),
                    TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_MODE_SEL_MASK));
	    	
        /*Prbs Polynomial in TLB_RX_PRBS_CHK_CONFIG 1.D0D1.[3:1]*/
        /*Set Checker Poly*/
        SOC_IF_ERROR_RETURN(
             MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, TLB_RX_PRBS_CHK_CONFIG,
               (polynomial << TLB_RX_PRBS_CHK_CONFIG_PRBS_CHK_MODE_SEL_SHIFT),
               TLB_RX_PRBS_CHK_CONFIG_PRBS_CHK_MODE_SEL_MASK));

    return SOC_E_NONE;
}


int32 phy82328_validate_preemphasis(uint8 pre, uint8 main, uint8 post1, uint8 post2) 
{
    uint8 post2to1 = (post2==0);
    if (pre > 10) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid PRE CURSOR TAP. It should be less than 10\n")));
        return SOC_E_PARAM;
    }
    if (main > 60) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid MAIN CURSOR TAP. It should be less than 60\n")));
        return SOC_E_PARAM;
    }
    if ((!post2to1) && (post1 > 18)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid POST1 CURSOR TAP. It should be less than 18\n")));
        return SOC_E_PARAM;
    }
    if ((post2to1) && (post1 > 23)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid POST1 CURSOR TAP. It should be less than 23\n")));
        return SOC_E_PARAM;
    }
    if (post2 > 5) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid POST2 TAP. It should be less than 5\n")));
        return SOC_E_PARAM;
    }
    if (main < (pre + post1 + post2 + 1)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid MAIN TAP. It should be greater than (pre + post1 + post2 + 1)\n")));
        return SOC_E_PARAM;
    }
    if ((pre + main + post1 + post2) > 60) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid Value. pre + main + post1 + post2 should be less than 60\n")));
        return SOC_E_PARAM;
    }
    if ((!post2to1) && ((pre + post1) > 22)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid Value. pre + post1 greater than 22\n")));
        return SOC_E_PARAM;
    }
    if (post2to1 && ((pre + post1) > 27)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META("Invalid Value. pre + post1 greater than 27\n")));
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;  
}

STATIC int32
_phy_82328_control_preemphasis_get(int32 unit, soc_port_t port, soc_phy_control_t type, 
								   phy82328_intf_side_t side, uint32 *value)
{
	int32			lane, lane_idx;
    uint16      data16 = 0;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    /* coverity [mixed_enums:FALSE] */
	PHY82328_PREEMPHASIS_LANE_GET(type, lane);
	lane_idx = (lane == PHY82328_ALL_LANES) ? 0 : lane;


    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, side));

	if (PHY82328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, lane_idx));
	} 
    /*
	 * Value format:
	 * bit 21:18: post2_tap        - see 1.d111[10:7]
	 * bit 16:12: postcursor_tap   - see 1.d110[9:5]
	 * bit 10:05: main_tap         - see 1.d111[5:0]
	 * bit 03:00: precursor_tap    - see 1.d110[3:0]
	 */
    *value = 0;
	
	SOC_IF_ERROR_RETURN(
		READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_CONTROL2, &data16));
	
    *value = ((data16 & TX_FED_TXFIR_CONTROL2_TXFIR_POST2_MASK) >> 
              TX_FED_TXFIR_CONTROL2_TXFIR_POST2_SHIFT) << 18;
    *value |= ((data16 & TX_FED_TXFIR_CONTROL2_TXFIR_MAIN_OVERRIDE_MASK) << 5);
     
    SOC_IF_ERROR_RETURN(
		READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_CONTROL1, &data16));
	
    *value |= ((data16 & TX_FED_TXFIR_CONTROL1_TXFIR_POST_OVERRIDE_MASK) >> 
              TX_FED_TXFIR_CONTROL1_TXFIR_POST_OVERRIDE_SHIFT) << 12;
    *value |= (data16 & TX_FED_TXFIR_CONTROL1_TXFIR_PRE_OVERRIDE_MASK);

    if (PHY82328_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, PHY82328_ALL_LANES));
    }
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_control_preemphasis_set(int32 unit, soc_port_t port, 
								   soc_phy_control_t type, 
								   phy82328_intf_side_t side,
								   uint32 value)
{
    int32       lane, lane_idx, lane_start, lane_end;
	uint8       pre_cursor_tap = 0;
	uint8       main_tap = 0;
	uint8       post_cursor_tap = 0;
	uint8       post2_tap = 0;

	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    /* coverity [mixed_enums:FALSE] */
	PHY82328_PREEMPHASIS_LANE_GET(type, lane);
	if (!PHY82328_SINGLE_PORT_MODE(pc) && (lane != PHY82328_ALL_LANES)) {
		    if (lane != (pc->phy_id & 0x03)) {
		    	return SOC_E_PARAM;
		    }
	}

	/*
	 * Value format:
	 * bit 21:18: post2_tap        - see 1.d111[10:7]
	 * bit 16:12: postcursor_tap   - see 1.d110[9:5]
	 * bit 10:05: main_tap         - see 1.d111[5:0]
	 * bit 03:00: precursor_tap    - see 1.d110[3:0]
	 */
     pre_cursor_tap = (value & 0xf);
     main_tap = (value & 0x7E0) >> 5;
     post_cursor_tap = (value & 0x1F000) >> 12;
     post2_tap = (value & 0x3c0000) >> 18;
    
     SOC_IF_ERROR_RETURN(
         phy82328_validate_preemphasis(pre_cursor_tap, main_tap, 
                 post_cursor_tap, post2_tap));

    /*
	 * Single mode: if lane specified, enable only that lane, otherwise enable all lanes
	 * Quad mode: if specified lane matches lane for the channel, set the lane
	 */
	if (PHY82328_SINGLE_PORT_MODE(pc)) {
		lane_start = (lane == PHY82328_ALL_LANES) ? 0 : lane;
		lane_end   = (lane == PHY82328_ALL_LANES) ? PHY82328_NUM_LANES : lane + 1;
	}
    else {
		/* Make sure specified lane matches lane for the channel */
		if (lane != PHY82328_ALL_LANES) {
			if (lane != (pc->phy_id & 0x03)) {
				return SOC_E_PARAM;
			}
		}
		lane_start = 0;
		lane_end   = 1;
	}

    SOC_IF_ERROR_RETURN(
	 phy82328_intf_side_reg_select(unit, port, side));
	/* Update all requested lanes */
	for (lane_idx = lane_start; lane_idx < lane_end; lane_idx++) {
		if (PHY82328_SINGLE_PORT_MODE(pc)) {
			/* Select the lane and side to write */
			SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, lane_idx));
		} 
        SOC_IF_ERROR_RETURN(
			MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, AMS_TX_TX_CTRL_5,
                (((post2_tap == 0)?1:0) << AMS_TX_TX_CTRL_5_POST2TO1_SHIFT) ,
                AMS_TX_TX_CTRL_5_POST2TO1_MASK));

        /* Enable/Disable precursor */
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, AMS_TX_TX_CTRL_5,
                ((pre_cursor_tap?1:0) << AMS_TX_TX_CTRL_5_EN_PRE_SHIFT) ,
                AMS_TX_TX_CTRL_5_EN_PRE_MASK));
     	/* Set precursor */
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_CONTROL1,
              pre_cursor_tap , TX_FED_TXFIR_CONTROL1_TXFIR_PRE_OVERRIDE_MASK));

		/* Main tap */
        SOC_IF_ERROR_RETURN(
			MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_CONTROL2,
              main_tap, TX_FED_TXFIR_CONTROL2_TXFIR_MAIN_OVERRIDE_MASK));
        
        /* Enable/Disable postcursor */
		SOC_IF_ERROR_RETURN(
		    MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, AMS_TX_TX_CTRL_5,
              ((post_cursor_tap?1:0) << AMS_TX_TX_CTRL_5_EN_POST1_SHIFT) ,
              AMS_TX_TX_CTRL_5_EN_POST1_MASK));

     	/* Set postcursor */
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_CONTROL1,
             (post_cursor_tap << TX_FED_TXFIR_CONTROL1_TXFIR_POST_OVERRIDE_SHIFT), 
             TX_FED_TXFIR_CONTROL1_TXFIR_POST_OVERRIDE_MASK));
 
        /* Enable/Disable post2cursor */
		SOC_IF_ERROR_RETURN(
			    MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, AMS_TX_TX_CTRL_5,
                ((post2_tap?1:0) << AMS_TX_TX_CTRL_5_EN_POST2_SHIFT) ,
                AMS_TX_TX_CTRL_5_EN_POST2_MASK));

     	/* Set post2cursor */
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_CONTROL2,
             (post2_tap << TX_FED_TXFIR_CONTROL2_TXFIR_POST2_SHIFT),
             TX_FED_TXFIR_CONTROL2_TXFIR_POST2_MASK));
	}

	if (PHY82328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, PHY82328_ALL_LANES));
	}
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_rx_los_control_set(int32 unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc= EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));

        SOC_IF_ERROR_RETURN(               
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, SIGDET_SIGDET_CTRL_1, 
            (value?(1<<SIGDET_SIGDET_CTRL_1_SIGNAL_DETECT_FRC_SHIFT):0), 
             SIGDET_SIGDET_CTRL_1_SIGNAL_DETECT_FRC_MASK));
    FW_RX_LOS(pc) = value;
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_do_rxseq_restart(int32 unit, soc_port_t port, phy82328_intf_side_t side)
{
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
	PHY_82328_MICRO_PAUSE(unit, port, "rxseq restart1");
    if (side == PHY82328_INTF_SIDE_SYS) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_SYS));
    }
	SOC_IF_ERROR_RETURN
		(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, XGXS_BLK0_XGXSCONTROL,
					 0,	QSFI_XGXS_BLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    
    if (side == PHY82328_INTF_SIDE_SYS) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    }
	PHY_82328_MICRO_RESUME(unit, port);

	sal_udelay(800);
	
	PHY_82328_MICRO_PAUSE(unit, port, "rxseq restart 2");
    if (side == PHY82328_INTF_SIDE_SYS) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_SYS));
    }
    SOC_IF_ERROR_RETURN
		(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, 
							XGXS_BLK0_XGXSCONTROL, QSFI_XGXS_BLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                            QSFI_XGXS_BLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    if (side == PHY82328_INTF_SIDE_SYS) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    }
	PHY_82328_MICRO_RESUME(unit, port);

	return SOC_E_NONE;
}

STATIC int
_phy_82328_mod_auto_detect_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    uint16 data;
    uint16 rddata;
    uint16 count = 0;
    uint16 mask;

    pc = EXT_PHY_SW_STATE(unit, port);
    mask = (MGT_TOP_CMN_CTLR_GP_REG_0_ENABLE_QSFP_MOD_DETECT_MASK |
                MGT_TOP_CMN_CTLR_GP_REG_0_ENABLE_SFP_MOD_DETECT_MASK);
    data = value ? mask : 0;

    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_0, data, 
                                        mask));
 
    if (!value) {
        /* Request is disabling mod auto detect, need to wait until I2CM FSM  becomes IDLE */
        /* Wait approx for 325 ms */ 
        while (count < PHY82328_I2CM_IDLE_WAIT_COUNT){
            SOC_IF_ERROR_RETURN(
                READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_I2C_CONTROL_REGISTER, &rddata));
            if ((rddata & 0xC) == 0) {
                break;
            }
            count ++;
            sal_usleep(PHY82328_I2CM_IDLE_WAIT_CHUNK * 1000);
        }
        if (count >= PHY82328_I2CM_IDLE_WAIT_COUNT) {
            return SOC_E_BUSY;
        }
    }
 
    /* Update the global database */
    MOD_AUTO_DETECT(pc) = value ? 1:0; 
    
    return SOC_E_NONE;
}

STATIC int
_phy_82328_mod_auto_detect_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    uint16 data;
 
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_GP_REG_0, &data));

    if( data & (MGT_TOP_CMN_CTLR_GP_REG_0_ENABLE_QSFP_MOD_DETECT_MASK |
                MGT_TOP_CMN_CTLR_GP_REG_0_ENABLE_SFP_MOD_DETECT_MASK)) {
        *value = 1;
    } else {
        *value = 0;
    } 
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_82328_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_set(int32 unit, soc_port_t port, int32 intf, int32 lane,
                     soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    int32 rv = SOC_E_UNAVAIL;
 
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);
	
    if (intf == PHY_DIAG_INTF_SYS) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_SYS));
    }

    switch (type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
		    /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
		    rv = _phy_82328_control_preemphasis_set(unit, port, type, 
												(pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
												PHY82328_INTF_SIDE_SYS : PHY82328_INTF_SIDE_LINE, 
												value);
		    break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
            rv = SOC_E_UNAVAIL;
        break;

        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            rv = _phy_82328_control_prbs_polynomial_set(unit, port, value);					
        break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            rv = _phy_82328_control_prbs_invert_data_set(unit, port, value);					
        break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* fall through */
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            rv = _phy_82328_control_prbs_enable_set(unit, port, value);
        break;
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
            rv = _phy_82328_remote_loopback_set(unit, port, value);
        break;
        case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
		    /*Squelch Tx Line Side*/
            rv = _phy_82328_tx_squelch(unit,port,PHY82328_INTF_SIDE_LINE,value);
        break;
        case SOC_PHY_CONTROL_PORT_PRIMARY:
            rv = soc_phyctrl_primary_set(unit, port, (soc_port_t)value);
        break;
        case SOC_PHY_CONTROL_PORT_OFFSET:
            rv = soc_phyctrl_offset_set(unit, port, (int32)value);
        break;
        case SOC_PHY_CONTROL_RX_POLARITY:
            rv = _phy_82328_polarity_flip (unit, port, 
		          PHY82328_POL_DND, (uint16)value);
        break;
        case SOC_PHY_CONTROL_TX_POLARITY:
            rv = _phy_82328_polarity_flip (unit, port, 
		             (uint16)value, PHY82328_POL_DND);
        break;
        case SOC_PHY_CONTROL_DUMP:
            rv = _phy_82328_debug_info(unit, port);
        break;
        case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
            /* enable/disable RX los on line side*/
            rv = _phy_82328_rx_los_control_set(unit, port, value);
        break;
        case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
		/* value is not used since this is always a toggle ->1->0 */
		rv = _phy_82328_do_rxseq_restart(unit, port,
                            (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                            PHY82328_INTF_SIDE_SYS : PHY82328_INTF_SIDE_LINE);
		break;
        case SOC_PHY_CONTROL_MOD_AUTO_DETECT_ENABLE:
            rv = _phy_82328_mod_auto_detect_set(unit, port, value); 
        break;

        case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
            rv = SOC_E_UNAVAIL;
        break;
 
        default:
            return SOC_E_UNAVAIL;
    }
    return rv;
}

STATIC int32
phy_82328_control_set(int32 unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    SOC_IF_ERROR_RETURN(
            _phy_82328_control_set(unit, port, PHY_DIAG_INTF_LINE,
                                   PHY_DIAG_LN_DFLT, type, value));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_82328_control_prbs_enable_get
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. *      
 *      value - (OUT)Current setting for the PRBS Gen & Chkr
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_prbs_enable_get(int32 unit, soc_port_t port, uint32 *value)
{
    uint16  gen_data=0,chkr_data=0;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
 
    *value = 0;
    /*Prbs Gen in TLB_TX_PRBS_GEN_CONFIG 1.D0E1.0*/
    SOC_IF_ERROR_RETURN(
            READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
			  TLB_TX_PRBS_GEN_CONFIG, &gen_data));
    gen_data &= TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_EN_MASK;
    gen_data = gen_data >> TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_EN_SHIFT;
		
    /*Prbs Checker En in TLB_RX_PRBS_CHK_CONFIG 1.D0D1.0*/     
    SOC_IF_ERROR_RETURN(
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
            TLB_RX_PRBS_CHK_CONFIG, &chkr_data));
    chkr_data &= TLB_RX_PRBS_CHK_CONFIG_PRBS_CHK_EN_MASK;
    chkr_data = chkr_data >> TLB_RX_PRBS_CHK_CONFIG_PRBS_CHK_EN_SHIFT;
		
    *value = gen_data && chkr_data;
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_82328_control_prbs_rx_status_get
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. *      
 *      value - (OUT)PRBS Status Lock & Error Cntr 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_prbs_rx_status_get(int32 unit, soc_port_t port, uint16 intf, uint32 *value)
{
    uint16  lock = 0, err_lsb = 0, err_msb = 0, lanes = 0, lane_idx = 0;
    uint32  err_cnt = 0;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    
    if (PHY82328_SINGLE_PORT_MODE(pc)) {
        lanes = PHY82328_NUM_LANES;
    }
    else {
        lanes = 1;
    }
 
    *value = 0;
    for (lane_idx = 0; lane_idx < lanes; lane_idx++) {
        if (PHY82328_SINGLE_PORT_MODE(pc)) {
            /* Select the lane and side to access */
            SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, 
                                           (intf == PHY_DIAG_INTF_LINE) ? PHY82328_INTF_SIDE_LINE
                                           : PHY82328_INTF_SIDE_SYS , lane_idx));
        }
        
        /*Prbs Lock Status in TLB_RX_PRBS_CHK_LOCK_STATUS 1.D0D9.0*/
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, 
                  TLB_RX_PRBS_CHK_LOCK_STATUS, &lock));
        lock &= TLB_RX_PRBS_CHK_LOCK_STATUS_PRBS_CHK_LOCK_MASK;
	    	
        /*Prbs Err Cnt MSB in TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS 1.D0DA*/     
        SOC_IF_ERROR_RETURN(
             READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                  TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS, &err_msb));

        err_msb &= TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS_PRBS_CHK_ERR_CNT_MSB_MASK;
	    	
        /*Prbs Err Cnt LSB in TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS 1.D0DB*/     
        SOC_IF_ERROR_RETURN(
             READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                   TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS, &err_lsb));
	    	
	    err_cnt = (err_msb << TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS_PRBS_CHK_ERR_CNT_LSB_BITS) 
                  |err_lsb;
        
        if (lock && !err_cnt) { /*Locked and No errors*/
            *value |= 0;
        } else {
            if (!lock) { /*Not Locked*/	
                *value |= -1;	
            } else {
                /*Locked With Errors*/
                *value |= err_cnt;
            }
        }
    }
    
    /* Restore to default single port register access */
    if (PHY82328_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port,
                             PHY82328_INTF_SIDE_LINE, PHY82328_ALL_LANES));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_82328_control_prbs_tx_invert_data_get
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. *      
 *      value - (OUT)Current setting for the PRBS Invert
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_prbs_tx_invert_data_get(int32 unit, soc_port_t port, uint32 *value)
{
    uint16  data=0;		
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
 
    *value = 0;
    /*Prbs Invert in TLB_TX_PRBS_GEN_CONFIG 1.D0E1.4 */ 
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
              TLB_TX_PRBS_GEN_CONFIG, &data));

    data &= TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_INV_MASK;

    *value = data >> TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_INV_SHIFT;		
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_82328_control_prbs_polynomial_get
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. *      
 *      value - (OUT)Current setting for the PRBS Polynomial
 *       0=prbs7, 
 *       1=prbs=15 
 *       2=prbs=23 
 *       3=prbs=31 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_prbs_polynomial_get(int32 unit, soc_port_t port, uint32 *value)
{
    uint16  data;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
 
    *value = 0;
    /* Prbs Polynomial in TLB_TX_PRBS_GEN_CONFIG 1.D0E1.[3:1] */        
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
             TLB_TX_PRBS_GEN_CONFIG, &data));

    data &= TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_MODE_SEL_MASK;
    data = data >> TLB_TX_PRBS_GEN_CONFIG_PRBS_GEN_MODE_SEL_SHIFT;
    switch(data) {
        case 0: /* PRBS 7*/
            *value = 0;
        break;
        case 3: /*PRBS15*/
            *value = 1;
        break;
        case 4:/*PRBS23*/
            *value = 2;
        break;
        case 5:/*PRBS31*/
            *value = 3;
        break;
        case 1:/*PRBS 9*/
            *value = 4;
        break;
        case 2:/*PRBS 11*/
            *value = 5;
        break;
        case 6: /*PRBS58*/
            *value = 6;
        break;
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_82328_debug_info(int32 unit, soc_port_t port)
{
    phy_ctrl_t			*pc;     
    phy_ctrl_t			*int_pc; 
    phy82328_intf_cfg_t *intf;
    phy82328_intf_cfg_t *line_intf;
    phy82328_intf_cfg_t *sys_intf;
    phy82328_intf_side_t side, saved_side;
    soc_port_if_t		iif;
    int32					lane, die_temp = 0, i = 0;
    int32					ian, ian_done, ilink, ispeed;
    uint16				link_sts[PHY82328_NUM_LANES];
    uint16				pol_sts[PHY82328_NUM_LANES];
    uint16				data0, data1, data2, data3, data4;
    uint32				primary_port, offset;
    uint32              preemphasis[PHY82328_NUM_LANES];
    char *rx_los_sts;
 
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
	
    SOC_IF_ERROR_RETURN(phy_82328_control_get(unit, port, SOC_PHY_CONTROL_PORT_PRIMARY, 
                                             &primary_port));

    SOC_IF_ERROR_RETURN(phy_82328_control_get(unit, port, SOC_PHY_CONTROL_PORT_OFFSET,
                                              &offset));

	/* Access line side registers */
    saved_side = _phy_82328_intf_side_regs_get(unit, port);
    if (saved_side == PHY82328_INTF_SIDE_SYS) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    }

    /* firware rev: 1.c161  */
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                          0xc161, &data0));

    /* micro enabled: 1.ca18 */
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                          MGT_TOP_GP_REG_0, &data1));
	/* Link sts 1.c0b0*/
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                          RX_ANARXSTATUS, &data2));
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                          MGT_TOP_VTMON_STATUS, &data3));
    data3 &= MGT_TOP_CMN_CTLR_VTMON_STATUS_VTMON_DATA_MASK;
    rx_los_sts = "NONE";
    die_temp = 425 - ((49935 * data3)/100000);
    if ((FW_RX_LOS(pc) & 0x1) == 0x1) {
        rx_los_sts = "FW";
    }

    LOG_CLI((BSL_META_U(unit,
                        "Port %-2d: devid=%d, chiprev=%04x, "
                        "micro ver(1.%04x)=%04x, micro(1.%04x)=%04x, \n"
                        "     pri:offs=%d:%d, mdio=0x%x, datapath=%d, "
                        "Link (1.%04x)=%04x\n DieTemprature= %d*c,"
                        "        Rx LOS=%s\n"),
             port, DEVID(pc), DEVREV(pc), XGXS_BLK8_CL73CONTROL8, data0,
             MGT_TOP_GP_REG_0, data1,
             primary_port, offset, pc->phy_id, 
             CUR_DATAPATH(pc) == PHY82328_DATAPATH_20 ? 20 : 4,
             RX_ANARXSTATUS, data2, die_temp, rx_los_sts));
    LOG_CLI((BSL_META_U(unit,
                        "AN:%d\n"), AN_EN(pc)));
    /* Single PMD control: 1.ca86 */
    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                MGT_TOP_SINGLE_PMD_CTRL, &data0));

    /* Broadcast control: 1.c8fe */
    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                MGT_TOP_BROADCAST_CTRL, &data1));

    /* Global intf type/speed - driver csr: 1.c841 */
    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                               MGT_TOP_GP_REGISTER_1, &data2));

    /* Micro csr 1.c843 */
    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                               MGT_TOP_GP_REGISTER_3, &data3));

    LOG_CLI((BSL_META_U(unit,
                        "     PMD(1.%04x)=%04x, bcctrl(1.%04x)=%04x, \n"
                        "     drvcsr(1.%04x)=%04x, ucsr(1.%04x)=%04x\n"),
             MGT_TOP_SINGLE_PMD_CTRL , data0,
             MGT_TOP_BROADCAST_CTRL, data1, 
             MGT_TOP_GP_REGISTER_1, data2,
             MGT_TOP_GP_REGISTER_3, data3));

    /* Interfaces: software, hardware, internal serdes */
    line_intf = &(LINE_INTF(pc));
    sys_intf = &(SYS_INTF(pc));
    for (side = PHY82328_INTF_SIDE_LINE; side <= PHY82328_INTF_SIDE_SYS; side++) {

        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, side));

        /* control: 1.0000 */
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                           IEEE0_BLK_PMA_PMD_PMD_CONTROL_REGISTER, &data1));
		
        /* PMD type: 1.0007 */
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                         IEEE0_BLK_PMA_PMD_PMD_CONTROL_2_REGISTER, &data2));

        /* Loopback 1.d0e2 */
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, 
                                           TLB_TX_RMT_LPBK_CONFIG, &data3));

        /* Tx disable 1.c08b */
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_ANATXACONTROL7, 
                                           &data4));
        
        if (PHY82328_SINGLE_PORT_MODE(pc)) {
            if (side == PHY82328_INTF_SIDE_SYS) {
                PHY_82328_MICRO_PAUSE(unit, port, "");
            }
            for (lane = 0; lane < PHY82328_NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN
                  (_phy_82328_control_preemphasis_get(unit, port, 
                  (SOC_PHY_CONTROL_PREEMPHASIS_LANE0 + lane), side, &preemphasis[lane]));

                /* Select the lane and side to write */
                SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, lane));

                /* Link: 1.c0b0 */
                SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                    RX_ANARXSTATUS, &(link_sts[lane])));
             
                if (CUR_DATAPATH(pc) == PHY82328_DATAPATH_20) {
                    /* Polarity setting: 1.d0e3 */
                    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                       TLB_TX_TLB_TX_MISC_CONFIG, &(pol_sts[lane])));
                } else {
                    /* Polarity setting: 1.d0a0 */
                    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                        AMS_TX_TX_CTRL_0, &(pol_sts[lane])));
                }
            }
            /* Back to default channel selection through lane 0 */
            SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, PHY82328_ALL_LANES));
            SOC_IF_ERROR_RETURN(
             phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
            if (side == PHY82328_INTF_SIDE_SYS) {
                PHY_82328_MICRO_RESUME(unit, port);
            }
        } else {
                SOC_IF_ERROR_RETURN
                      (_phy_82328_control_preemphasis_get(unit, port, 
                        SOC_PHY_CONTROL_PREEMPHASIS, side, &preemphasis[i]));

                /* Link: 1.c0b0 */
                SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                    RX_ANARXSTATUS, &(link_sts[i])));
                if (CUR_DATAPATH(pc) == PHY82328_DATAPATH_20) {
                    /* Polarity setting: 1.doe3 */
                    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                        TLB_TX_TLB_TX_MISC_CONFIG, 
                                        &(pol_sts[i])));
                } else {
                    /* Polarity setting: 1.d0a0 */
                    SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, 
                                        AMS_TX_TX_CTRL_0, 
                                        &(pol_sts[i])));
            }
        }
        intf = (side == PHY82328_INTF_SIDE_LINE) ? line_intf : sys_intf;			
        if (PHY82328_SINGLE_PORT_MODE(pc)) {
            LOG_CLI((BSL_META_U(unit,
                                " %s: type=%s, speed=%d, "
                                "forced cl72=%d, mode(1.%04x)=%04x\n"
                                "           pmdtype(1.%04x)=%04x, "
                                "lb(1.%04x)=%04x, txdis(1.%04x)=%04x\n"
                                "           link(1.%04x)      : "
                                "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"
                                "           pol(1.%04x)       : "
                                "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"
                                "           Preemphasis       : "
                                "(ln0=%08x, ln1=%08x, ln2=%08x, ln3=%08x)\n"),
                     (side == PHY82328_INTF_SIDE_LINE) ?
                     "Line    " : "System  ",
                     phy82328_intf_names[intf->type], intf->speed,
                     SYS_FORCED_CL72(pc),
                     IEEE0_BLK_PMA_PMD_PMD_CONTROL_REGISTER, data1,
                     IEEE0_BLK_PMA_PMD_PMD_CONTROL_2_REGISTER, data2,
                     TLB_TX_RMT_LPBK_CONFIG, data3,
                     TX_ANATXACONTROL7, data4,
                     RX_ANARXSTATUS, link_sts[0], link_sts[1],
                     link_sts[2], link_sts[3],
                     CUR_DATAPATH(pc) == PHY82328_DATAPATH_20 ?
                     TLB_TX_TLB_TX_MISC_CONFIG : AMS_TX_TX_CTRL_0,
                     pol_sts[0], pol_sts[1], pol_sts[2], pol_sts[3],
                     preemphasis[0], preemphasis[1],
                     preemphasis[2], preemphasis[3]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                " %s: type=%s, speed=%d, "
                                "forced cl72=%d, mode(1.%04x)=%04x\n"
                                "           pmdtype(1.%04x)=%04x, "
                                "lb(1.%04x)=%04x, txdis(1.%04x)=%04x\n"),
                     (side == PHY82328_INTF_SIDE_LINE) ? 
                     "Line    " : "System  ",
                     phy82328_intf_names[intf->type], 
                     intf->speed, SYS_FORCED_CL72(pc),
                     IEEE0_BLK_PMA_PMD_PMD_CONTROL_REGISTER, data1,
                     IEEE0_BLK_PMA_PMD_PMD_CONTROL_2_REGISTER, data2,
                     TLB_TX_RMT_LPBK_CONFIG, data3,
                     TX_ANATXACONTROL7, data4));

                LOG_CLI((BSL_META_U(unit,
                                    "   Lane %d : link(1.%04x)=%04x,"
                                    "pol(1.%04x)=%04x, Preemphasis:%08x"),
                         i, RX_ANARXSTATUS, link_sts[i],
                         CUR_DATAPATH(pc) == PHY82328_DATAPATH_20 ? 
                         TLB_TX_TLB_TX_MISC_CONFIG : AMS_TX_TX_CTRL_0,
                         pol_sts[i], preemphasis[i]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
    SOC_IF_ERROR_RETURN(
     phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    if (int_pc) {
        int32 lb;
        SOC_IF_ERROR_RETURN(PHY_INTERFACE_GET(int_pc->pd, unit, port, &iif));
        SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &ilink));
        SOC_IF_ERROR_RETURN(phy_82328_lb_get(unit, port, &lb));
        if (lb) {
            /* Clear the sticky bit if in loopback */
            SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &ilink));
        }
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &ispeed));
        SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_GET(int_pc->pd, unit, port, &ian, &ian_done));
        LOG_CLI((BSL_META_U(unit,
                            " Internal: type=%s, speed=%d, link=%d, an=%d\n===\n"), 
                 phy82328_intf_names[iif], ispeed, ilink, ian));
    }
    if (saved_side == PHY82328_INTF_SIDE_SYS) {
        SOC_IF_ERROR_RETURN(
         phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_SYS));
    }

    return SOC_E_NONE;
}

STATIC int32
_phy_82328_rx_los_control_get(int32 unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc= EXT_PHY_SW_STATE(unit, port);
    *value = FW_RX_LOS(pc);
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82328_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to get
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_82328_control_get(int32 unit, soc_port_t port, int32 intf, int32 lane,
                     soc_phy_control_t type, uint32 *value)
{
    phy_ctrl_t *pc;
    soc_port_t offset, primary;
    int32 rv = SOC_E_UNAVAIL;    
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);
	
    if (intf == PHY_DIAG_INTF_SYS) {
       SOC_IF_ERROR_RETURN(
        phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_SYS));
    }
    rv = SOC_E_UNAVAIL;
    switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        /* fall through */
		rv = _phy_82328_control_preemphasis_get(unit, port, type,
												(pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
												PHY82328_INTF_SIDE_SYS : PHY82328_INTF_SIDE_LINE, 
												value);
		break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
            /* fall through */
            rv = SOC_E_UNAVAIL;
        break;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            rv = _phy_82328_control_prbs_polynomial_get(unit, port, value);
        break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            rv = _phy_82328_control_prbs_tx_invert_data_get(unit, port, value);
        break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
           /* fall through */
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            rv = _phy_82328_control_prbs_enable_get(unit, port, value);
        break;
        case SOC_PHY_CONTROL_PRBS_RX_STATUS:
            rv = _phy_82328_control_prbs_rx_status_get(unit, port, intf, value);
        break;
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
            rv = _phy_82328_remote_loopback_get(unit, port, value);
        break;
        case SOC_PHY_CONTROL_PORT_PRIMARY:
            SOC_IF_ERROR_RETURN
              (soc_phyctrl_primary_get(unit, port, &primary));
            *value = (uint32) primary;
            rv = SOC_E_NONE;
        break;
        case SOC_PHY_CONTROL_PORT_OFFSET:
            SOC_IF_ERROR_RETURN
              (soc_phyctrl_offset_get(unit, port, &offset));
            *value = (uint32) offset;
		    rv = SOC_E_NONE;
        break;
        case SOC_PHY_CONTROL_RX_SEQ_DONE:
           rv = _phy_82328_intf_link_get(unit, port, (int32*)value);
        break;
        case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
            rv = _phy_82328_rx_los_control_get(unit, port, value);
        break;

        case SOC_PHY_CONTROL_MOD_AUTO_DETECT_ENABLE:
            rv = _phy_82328_mod_auto_detect_get(unit, port, value);
        break;

        default:
            rv = SOC_E_UNAVAIL;
        break;
	}

    return rv;
}

STATIC int32
phy_82328_control_get(int32 unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    SOC_IF_ERROR_RETURN
        (_phy_82328_control_get (unit, port, 
                    PHY_DIAG_INTF_LINE, PHY_DIAG_LN_DFLT, type, value));
 
    return SOC_E_NONE;
}

STATIC int
_phy_82328_bsc_rw(int unit, soc_port_t port, int dev_addr, int opr,
                    int addr, int count, void *data_array,uint32 ram_start)
{
    phy_ctrl_t  *pc;
    soc_timeout_t  to;
    uint16 data16;
    int i;
    int access_type;
    int data_type;
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    sal_usecs_t start;
    sal_usecs_t end;
#endif

    if (!data_array) {
        return SOC_E_PARAM;
    }

    if (count > PHY82328_BSC_XFER_MAX) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    data_type = PHY82328_I2C_DATA_TYPE(opr);
    access_type = PHY82328_I2C_ACCESS_TYPE(opr);

    if (access_type == PHY82328_I2CDEV_WRITE) {
        for (i = 0; i < count; i++) {
            if (data_type == PHY82328_I2C_8BIT) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, ram_start + i,
                          ((uint8 *)data_array)[i]));
            } else {  /* 16 bit */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, ram_start + i,
                          ((uint16 *)data_array)[i]));
            }
        }
    }

    data16 = ram_start;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, 0x8004, data16));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, 0x8003, addr));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, 0x8002, count));

    data16 = 1;
    data16 |= (dev_addr<<9);
    if (access_type == PHY82328_I2CDEV_WRITE) {
        data16 |= PHY82328_WR_FREQ_400KHZ;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY82328_MMF_PMA_PMD_REG(unit,pc,0x8005,data16));

    if (access_type == PHY82328_I2CDEV_WRITE) {
        data16 =  0x8000 | PHY82328_BSC_WRITE_OP;
    } else {
        data16 =  0x8000 | PHY82328_BSC_READ_OP;
    }

    if (data_type == PHY82328_I2C_16BIT) {
        data16 |= (1 << 12);
    }

	/* Select first i2c port */
	if (PHY82328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN
			(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_MISC_CTRL,
											 0, MGT_TOP_CMN_CTLR_MISC_CTRL_I2C_PORT_SEL_MASK));
	} else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, MGT_TOP_MISC_CTRL,
            (pc->phy_id & 0x3), MGT_TOP_CMN_CTLR_MISC_CTRL_I2C_PORT_SEL_MASK));
    }
		
	SOC_IF_ERROR_RETURN
		(WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, 0x8000, data16));

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    start = sal_time_usecs();
#endif
    soc_timeout_init(&to, 1000000, 0);
    while (!soc_timeout_check(&to)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, 0x8000, &data16));
        if (((data16 & PHY82328_2W_STAT) == PHY82328_2W_STAT_COMPLETE)) {
            break;
        }
    }

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    end = sal_time_usecs();
#endif
    /* need some delays */
    sal_usleep(10000);

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "BSC command status %d time=%d\n"),
              (data16 & PHY82328_2W_STAT), SAL_USECS_SUB(end, start)));
#endif

    if (access_type == PHY82328_I2CDEV_WRITE) {
        return SOC_E_NONE;
    }

    if ((data16 & PHY82328_2W_STAT) == PHY82328_2W_STAT_COMPLETE) {
        for (i = 0; i < count; i++) {
            SOC_IF_ERROR_RETURN
                (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, (ram_start+i), &data16));
            if (data_type == PHY82328_I2C_16BIT) {
                ((uint16 *)data_array)[i] = data16;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "%04x "), data16));
            } else {
                ((uint8 *)data_array)[i] = (uint8)data16;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "%02x "), data16));
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Read a slave device such as NVRAM/EEPROM connected to the 82328's I2C
 * interface. This function will be mainly used for diagnostic or workaround
 * purpose.
 * Note:
 * The size of read_array buffer must be greater than or equal to the
 * parameter nbytes.
 * usage example:
 *   Retrieve the first 100 byte data of the non-volatile storage device with
 *   I2C bus device address 0x50(default SPF eeprom I2C address) on unit 0,
 *   port 2.
 *   uint8 data8[100];
 *   phy_82328_i2cdev_read(0,2,0x50,0,100,data8);
 */
int
phy_82328_i2cdev_read(int unit,
                     soc_port_t port,
                     int dev_addr,  /* 7 bit I2C bus device address */
                     int offset,    /* starting data address to read */
                     int nbytes,    /* number of bytes to read */
                     uint8 *read_array)   /* buffer to hold retrieved data */
{
    return _phy_82328_bsc_rw(unit, port, dev_addr, PHY82328_I2CDEV_READ,
             offset, nbytes, (void *)read_array, PHY82328_READ_START_ADDR);

}

/*
 * Write to a slave device such as NVRAM/EEPROM connected to the 82328's I2C
 * interface. This function will be mainly used for diagnostic or workaround
 * purpose.
 * Note:
 * The size of write_array buffer should be equal to the parameter nbytes.
 * The EEPROM may limit the maximun write size to 16 bytes
 * usage example:
 *   Write to first 100 byte space of the non-volatile storage device with
 *   I2C bus device address 0x50(default SPF eeprom I2C address) on unit 0,
 *   port 2, with written data specified in array data8.
 *   uint8 data8[100];
 *   *** initialize the data8 array with written data ***
 *
 *   phy_82328_i2cdev_write(0,2,0x50,0,100,data8);
 */

int
phy_82328_i2cdev_write(int unit,
                     soc_port_t port,
                     int dev_addr,  /* I2C bus device address */
                     int offset,    /* starting data address to write to */
                     int nbytes,    /* number of bytes to write */
                     uint8 *write_array)   /* buffer to hold written data */
{
    int j;
    int rv = SOC_E_NONE;

    for (j = 0; j < (nbytes/PHY82328_BSC_WR_MAX); j++) {
        rv = _phy_82328_bsc_rw(unit, port, dev_addr,PHY82328_I2CDEV_WRITE,
                    offset + j * PHY82328_BSC_WR_MAX, PHY82328_BSC_WR_MAX,
                    (void *)(write_array + j * PHY82328_BSC_WR_MAX),
                    PHY82328_WRITE_START_ADDR);
        if (rv != SOC_E_NONE) {
            return rv;
        }
        sal_usleep(20000);
    }
    if (nbytes % PHY82328_BSC_WR_MAX) {
        rv = _phy_82328_bsc_rw(unit, port, dev_addr,PHY82328_I2CDEV_WRITE,
                offset + j * PHY82328_BSC_WR_MAX, (nbytes % PHY82328_BSC_WR_MAX),
                (void *)(write_array + j * PHY82328_BSC_WR_MAX),
                PHY82328_WRITE_START_ADDR);
    }
    return rv;
}

/*
 * Function:
 *      phy_82328_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int32
phy_82328_reg_read(int32 unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data16;
    phy_ctrl_t           *pc;      /* PHY software state */
    int32                rv = SOC_E_NONE;
    int32 rd_cnt = 1;
    uint16 regdata = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (flags & SOC_PHY_I2C_DATA8) {
        SOC_IF_ERROR_RETURN
            (phy_82328_i2cdev_read(unit, port,
                        SOC_PHY_I2C_DEVAD(phy_reg_addr),
                        SOC_PHY_I2C_REGAD(phy_reg_addr),
                        rd_cnt,
                        (uint8 *)&data16));
        *phy_data = *((uint8 *)&data16);
    } else if (flags & SOC_PHY_I2C_DATA16) {
        /* This operation is generally targeted to access 16-bit device,
         * such as PHY IC inside the SFP.  However there is no 16-bit
         * scratch register space on the device.  Use 1.800e
         * for this operation.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, PHY82328_I2C_TEMP_RAM,
                   &regdata));

        rv = _phy_82328_bsc_rw(unit, port,
               SOC_PHY_I2C_DEVAD(phy_reg_addr),
               PHY82328_I2C_OP_TYPE(PHY82328_I2CDEV_READ, PHY82328_I2C_16BIT),
               SOC_PHY_I2C_REGAD(phy_reg_addr), 1,
               (void *)&data16, PHY82328_I2C_TEMP_RAM);

        *phy_data = data16;

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, PHY82328_I2C_TEMP_RAM,
                 regdata));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY_REG(unit, pc, phy_reg_addr, &data16));
        *phy_data = data16;
    }
    return rv;
}

/*
 * Function:
 *      phy_82328_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int32
phy_82328_reg_write(int32 unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    phy_ctrl_t          *pc;      /* PHY software state */
    int32 rv = SOC_E_NONE;
    uint8  data8[4];
    uint16 data16[2];
    uint16 regdata[2];
    int32 wr_cnt = 1;

    pc = EXT_PHY_SW_STATE(unit, port);
 
    if (flags & SOC_PHY_I2C_DATA8) {
        data8[0] = (uint8)phy_data;
        SOC_IF_ERROR_RETURN
            (phy_82328_i2cdev_write(unit, port,
                        SOC_PHY_I2C_DEVAD(phy_reg_addr),
                        SOC_PHY_I2C_REGAD(phy_reg_addr),
                        wr_cnt,
                        data8));

    } else if (flags & SOC_PHY_I2C_DATA16) {
        /* This operation is generally targeted to access 16-bit device,
         * such as PHY IC inside the SFP.  However there is no 16-bit
         * scratch register space on the device.  Use 1.800e
         * for this operation.
         */
        /* save the temp ram register */
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, PHY82328_I2C_TEMP_RAM,
                 &regdata[0]));
        data16[0] = phy_data;
        rv = _phy_82328_bsc_rw(unit, port,
              SOC_PHY_I2C_DEVAD(phy_reg_addr),
              PHY82328_I2C_OP_TYPE(PHY82328_I2CDEV_WRITE, PHY82328_I2C_16BIT),
              SOC_PHY_I2C_REGAD(phy_reg_addr),wr_cnt,
              (void *)data16, PHY82328_I2C_TEMP_RAM);

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY82328_MMF_PMA_PMD_REG(unit, pc, PHY82328_I2C_TEMP_RAM,
                  regdata[0]));
    } else {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG (unit, pc, phy_reg_addr, 
                                      (uint16)phy_data));
    }
    return rv;
}

/*
 * Function:
 *      phy_82328_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */
STATIC int32
phy_82328_probe(int32 unit, phy_ctrl_t *pc)
{
    uint32 devid;
    uint16 chip_rev;
    SOC_IF_ERROR_RETURN(_phy_82328_config_devid(pc->unit, pc->port, pc, &devid));
    SOC_IF_ERROR_RETURN(
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                      MGT_TOP_CHIP_REV_REGISTER,
                                      &chip_rev));
    if (devid == PHY82328_ID_82328) {
        if (chip_rev == PHY82328A0_CHIP_REV) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY82328 Dev found")));
            pc->dev_name = dev_name_82328_a0;
        } else if (chip_rev == PHY82328B0_CHIP_REV) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY82328 B0 Dev found")));
            pc->dev_name = dev_name_82328_b0;
        } else if (chip_rev == PHY82328B1_CHIP_REV) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY82328 B1 Dev found")));
            pc->dev_name = dev_name_82328_b1;
        }

    } else if (devid == PHY82328_ID_82328F) {
        if (chip_rev == PHY82328A0_CHIP_REV) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY82328F A0 Dev found")));
            pc->dev_name = dev_name_82328f_a0;
        } else if (chip_rev == PHY82328B0_CHIP_REV) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                  "PHY82328F B0 Dev found")));
            pc->dev_name = dev_name_82328f_b0;
        } else if (chip_rev == PHY82328B1_CHIP_REV) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                  "PHY82328F B1 Dev found")));
            pc->dev_name = dev_name_82328f_b1;
        }

    } else if (devid == PHY82328_ID_82322) {
        if (chip_rev == PHY82328B1_CHIP_REV) { 
            pc->dev_name = dev_name_82322_b1;
        } else {
            pc->dev_name = dev_name_82322;
        }
    } else {  /* not found */
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "port %d: BCM82xxx type PHY device detected, please use "
                             "phy_82<xxx> config variable to select the specific type\n"),
                  pc->port));
        return SOC_E_NOT_FOUND;
    }
    pc->size = sizeof(phy82328_dev_desc_t);
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82328_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
 
STATIC int32
phy_82328_ability_advert_set(int32 unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    uint16 pause;
    phy_ctrl_t  *pc, *int_pc;
    phy82328_intf_cfg_t *line_intf;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "82328 Ablity advert Set\n")));
    if (ability == NULL) {
        return SOC_E_PARAM;
    }
	pc = EXT_PHY_SW_STATE(unit, port);
    
    line_intf = &(LINE_INTF(pc));
    if ((line_intf->type == SOC_PORT_IF_KX) || (line_intf->type == SOC_PORT_IF_GMII)) {
        int_pc = INT_PHY_SW_STATE(unit, port);
        if (int_pc) {
            SOC_IF_ERROR_RETURN(PHY_ABILITY_ADVERT_SET(int_pc->pd, unit, port, ability));
        }
        return SOC_E_NONE;
    }
	
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
        case SOC_PA_PAUSE_TX: /*7.0x0010.10 1*/
            pause = (1<<QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT);
		break;
        case SOC_PA_PAUSE_RX: /*7.0x0010.11:10 1*/
            pause = (1<<(QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT+1)) |
		            (1<<QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT);
        break;
        case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX: /*7.0x0010.11 1*/
            pause = (1<<(QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT+1));
        break;
        default:
            pause = 0;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY82328_MMF_AN_REG(unit, pc, 
         IEEE1_AN_BLK_AN_ADVERTISEMENT_1_REGISTER,
         pause, QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_MASK)); 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_82328_ability_advert_set: u=%d p=%d speed(FD)=%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82328_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
 
STATIC int32
phy_82328_ability_advert_get(int32 unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc, *int_pc;
    uint16 pause_adv;
    phy82328_intf_cfg_t *line_intf;

    soc_port_mode_t mode = 0;        
    if (ability == NULL) {
        return SOC_E_PARAM;
    } 
    sal_memset(ability, 0, sizeof(soc_port_ability_t)); 
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    line_intf = &(LINE_INTF(pc));

    if ((line_intf->type == SOC_PORT_IF_KX) || (line_intf->type == SOC_PORT_IF_GMII)) {
        int_pc = INT_PHY_SW_STATE(unit, port);
        if (int_pc) {
            SOC_IF_ERROR_RETURN(PHY_ABILITY_ADVERT_GET(int_pc->pd, unit, port, ability));
        }
        return SOC_E_NONE;
    }

    if(PHY82328_SINGLE_PORT_MODE(pc)) {
        ability->speed_full_duplex = SOC_PA_SPEED_40GB;
    }
    else {
        ability->speed_full_duplex = SOC_PA_SPEED_10GB;
    }
    
    SOC_IF_ERROR_RETURN
        (READ_PHY82328_MMF_AN_REG
         (unit, pc, IEEE1_AN_BLK_AN_ADVERTISEMENT_1_REGISTER, &pause_adv)); 
    mode = 0;
    
    switch (pause_adv & QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_MASK) {		
        case (1<<QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT):  /*7.0x0010.10 1*/
			mode = SOC_PA_PAUSE_TX;		
        break;
        case (1<<(QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT+1)): /*7.0x0010.11 1*/
			mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        break;		
        case (1<<(QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT+1)) |
				(1<<QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT):/*7.0x0010.11:10 1*/
            mode = SOC_PA_PAUSE_RX;
        break;		
        default:
            mode = 0;
        break;
    }
    ability->pause = mode;	 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_82328_ability_advert_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82328_ability_local_get
 * Purpose:
 *      Get the device's complete abilities.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - return device's abilities.
 * Returns:
 *      SOC_E_XXX
 */
 
STATIC int32
phy_82328_ability_local_get(int32 unit, soc_port_t port, soc_port_ability_t *ability)
{
    uint32		pa_speed = 0;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
	
    if (ability == NULL) {
        return SOC_E_PARAM;
    }
    sal_memset(ability, 0, sizeof(soc_port_ability_t));
    pa_speed = PHY82328_SINGLE_PORT_MODE(pc) ? (SOC_PA_SPEED_40GB|SOC_PA_SPEED_42GB) :
               (SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB );
    ability->speed_full_duplex = pa_speed;
    ability->speed_half_duplex = SOC_PA_ABILITY_NONE;
    ability->pause = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_82328_ability_local_get: u=%d p=%d speed=0x%x\n"),
              unit, port, ability->speed_full_duplex));
                                                                               
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82328_firmware_set
 * Purpose:
 *      program the given firmware into the SPI-ROM
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      offset - offset to the data stream
 *      array  - the given data
 *      datalen- the data length
 * Returns:
 *      SOC_E_NONE
 */
 
STATIC int32
phy_82328_firmware_set(int32 unit, int32 port, int32 offset, uint8 *array,int32 datalen)
{
    /* overload this function a littl bit if array == NULL 
     * special handling for init. uCode broadcast. Internal use only 
     */
     uint16 chip_rev;
     phy_ctrl_t *pc = (phy_ctrl_t*)NULL;
     pc = EXT_PHY_SW_STATE(unit, port); 
     chip_rev = DEVREV(pc);
     if (array == NULL) {
        if(chip_rev == PHY82328B0_CHIP_REV || chip_rev == PHY82328B1_CHIP_REV) {
           return _phy_82328_rom_fw_dload (unit, port, offset, 
                            phy82328B0_ucode_bin, phy82328B0_ucode_bin_len);
        } else if(chip_rev == PHY82328A0_CHIP_REV) {
           return _phy_82328_rom_fw_dload (unit, port, offset, 
                            phy82328_ucode_bin, phy82328_ucode_bin_len);
        } else {
           return SOC_E_UNAVAIL; 
        }
    } else {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d firmware download method not supported\n"),
                             unit, port));
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

int32 phy82328_uc_dsc_rdy_tim_out(int32 unit, phy_ctrl_t *pc,  uint16 timeout_ms) 
{
    uint16 tim_out = 0, rdy = 0;
    for (tim_out = 0; tim_out < 100; tim_out ++) {
        PHY82328_RD_UC_DSC_RDY_FOR_CMD(unit, pc, rdy);
        if (rdy) {
            return SOC_E_NONE;
        }
        sal_udelay(10 * timeout_ms);
    }
    LOG_CLI((BSL_META_U(unit,
                        "UC DSC rdy timeout\n")));
    return SOC_E_INTERNAL;

}
int32 phy82328_serdes_pmd_uc_cmd(int32 unit, soc_port_t port, phy_ctrl_t *pc, uint16 cmd, uint8 supp_info, uint16 timeout) 
{

    /* Poll for uc_dsc_ready_for_cmd = 1 to indicate serdes ready for command */
    SOC_IF_ERROR_RETURN(phy82328_uc_dsc_rdy_tim_out(unit, pc, timeout));
   
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_DSC_UC_CTRL, 
            (supp_info << DSC_A_DSC_UC_CTRL_UC_DSC_SUPP_INFO_SHIFT), DSC_A_DSC_UC_CTRL_UC_DSC_SUPP_INFO_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_DSC_UC_CTRL, 
            0x0, DSC_A_DSC_UC_CTRL_UC_DSC_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_DSC_UC_CTRL,
            cmd ,DSC_A_DSC_UC_CTRL_UC_DSC_GP_UC_REQ_MASK));

    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_DSC_UC_CTRL, 
            0x0, DSC_A_DSC_UC_CTRL_UC_DSC_READY_FOR_CMD_MASK));
  
    /* Poll for uc_dsc_ready_for_cmd = 1 to indicate serdes ready for command */
    SOC_IF_ERROR_RETURN(phy82328_uc_dsc_rdy_tim_out(unit, pc, timeout));

    return SOC_E_NONE;
}

/* Micro RAM Lane Byte Read */
int32 phy82328_serdes_rdbl_uc_var(int32 unit, soc_port_t port, phy_ctrl_t *pc,  uint8 s_info, uint16 *data16) 
{
    /* Each lane can have a maximum of 208 bytes in Gallardo */
    if (s_info > 207) {
        return SOC_E_INTERNAL;
    }
    *data16 = 0;

    /* Use Serdes FW DSC Command Interface for reading RAM */
    SOC_IF_ERROR_RETURN(
            phy82328_serdes_pmd_uc_cmd(unit, port, pc, PHY82328_CMD_READ_UC_LANE_BYTE, s_info, 10));
    SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_DSC_SCRATCH, data16));

    return SOC_E_NONE;
}

int32 phy82328_restore_trnsum_ctrls(int32 unit, phy_ctrl_t *pc) 
{
    /* DSC_D_TRNSUM_CTL_1 - Trnsum Ctl 1 */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_EN_MASK));
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_ERR_SEL_MASK));
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_RANDOM_TAPSEL_DISABLE_MASK));
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_INV_PATTERN_EN_MASK));
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_PATTERN_FULL_CHECK_OFF_MASK));
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_GAIN_MASK));
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_EYE_CLOSURE_EN_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_CDR_QPHASE_MULT_EN_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_TAP_RANGE_SEL_MASK));

    /* DSC_D_TRNSUM_CTL_2 - Trnsum Ctl 2 */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_2, 0, DSC_D_TRNSUM_CTL_2_TRNSUM_PATTERN_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_2, 0, DSC_D_TRNSUM_CTL_2_TRNSUM_PATTERN_BIT_EN_MASK));

    /* DSC_D_TRNSUM_CTL_3 - Trnsum Ctl 3 */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_3, 0, DSC_D_TRNSUM_CTL_3_TRNSUM_TAP_EN_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_3, 0, DSC_D_TRNSUM_CTL_3_TRNSUM_TAP_SIGN_MASK));

    /* DSC_D_TRNSUM_CTL_4 - Trnsum Ctl 4 */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_4, 0, DSC_D_TRNSUM_CTL_4_TDR_TRNSUM_EN_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_4, 0, DSC_D_TRNSUM_CTL_4_TDR_CYCLE_BIN_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_4, 0, DSC_D_TRNSUM_CTL_4_TDR_CYCLE_SEL_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_4, 0, DSC_D_TRNSUM_CTL_4_TDR_BIT_SEL_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_4, 0, DSC_D_TRNSUM_CTL_4_TRNSUM_UNSIGNED_FLIP_MASK));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_4, 0, DSC_D_TRNSUM_CTL_4_TRNSUM_UNSIGNED_CORR_MASK));

    return SOC_E_NONE;
}

/* Configures trnsum block before calibrating the P1 slicers */
/* Desired equation is Sum of [ p(n) ] */
int32 phy82328_cfg_trnsum_for_eyediag(int32 unit, soc_port_t port, uint8 timeout) 
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
 
    SOC_IF_ERROR_RETURN(phy82328_restore_trnsum_ctrls(unit, pc));
    
    /* Select d[0] */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_3, 
                (PHY82328_TRNSUM_TAP_0 << DSC_D_TRNSUM_CTL_3_TRNSUM_TAP_EN_SHIFT), DSC_D_TRNSUM_CTL_3_TRNSUM_TAP_EN_MASK));

    /* Delay range 5 to -2 */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 
                (PHY82328_TRNSUM_TAP_5_TO_N2 << DSC_D_TRNSUM_CTL_1_TRNSUM_TAP_RANGE_SEL_SHIFT), DSC_D_TRNSUM_CTL_1_TRNSUM_TAP_RANGE_SEL_MASK));

    /* Select error from only P1 slicer */
     SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 
                (PHY82328_TRNSUM_ERR_SEL_P1 << DSC_D_TRNSUM_CTL_1_TRNSUM_ERR_SEL_SHIFT) , DSC_D_TRNSUM_CTL_1_TRNSUM_ERR_SEL_MASK));

    /* Comparison */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_4,
            1, DSC_D_TRNSUM_CTL_4_TRNSUM_UNSIGNED_CORR_MASK));

    /* Not equals */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_4, 
            (1 << DSC_D_TRNSUM_CTL_4_TRNSUM_UNSIGNED_FLIP_SHIFT) , DSC_D_TRNSUM_CTL_4_TRNSUM_UNSIGNED_FLIP_MASK));

    /* Enable the trnsum function (posedge clears) */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 
            (1 << DSC_D_TRNSUM_CTL_1_TRNSUM_EN_SHIFT), DSC_D_TRNSUM_CTL_1_TRNSUM_EN_MASK));

    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_B_DSC_SM_CTRL_5, 
                timeout , DSC_B_DSC_SM_CTRL_5_MEASURE_TIMEOUT_MASK));
    return SOC_E_NONE;
}


STATIC int32
_phy_82328_diag_eyescan_livelink_en(int32 unit, soc_port_t port, int32 en)
{
    uint16 is_micro_stopped = 0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY82328_STS_IS_MICRO_STOPPED(unit, port, pc, is_micro_stopped);
    if (!is_micro_stopped) {
        SOC_IF_ERROR_RETURN(
            phy82328_serdes_pmd_uc_cmd(unit, port, pc, PHY82328_CMD_UC_CTRL, PHY82328_CMD_UC_CTRL_STOP_GRACEFULLY, 2000));
    }
     /* Enable tuning by uC */
    SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_B_DSC_SM_CTRL_0, 
            (en << DSC_B_DSC_SM_CTRL_0_UC_TUNE_EN_SHIFT), DSC_B_DSC_SM_CTRL_0_UC_TUNE_EN_MASK));
    if (en) {
        SOC_IF_ERROR_RETURN(phy82328_cfg_trnsum_for_eyediag(unit, port, 31));
    } else {
        SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 0, DSC_D_TRNSUM_CTL_1_TRNSUM_EN_MASK));
        SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_B_DSC_SM_CTRL_0, 0, DSC_B_DSC_SM_CTRL_0_UC_TRNSUM_EN_MASK));
        
        if (!is_micro_stopped) {
            SOC_IF_ERROR_RETURN(
                phy82328_serdes_pmd_uc_cmd(unit, port, pc, PHY82328_CMD_UC_CTRL, PHY82328_CMD_UC_CTRL_RESUME, 2000));
        }
    }

    return SOC_E_NONE;
}

int32 phy82328_move_clkp1_offset(int32 unit, phy_ctrl_t *pc , int8 delta) 
{
    uint8 cnt = 0;

    /* Select p1 slicer to adjust */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_RX_PI_CONTROL, 
            (2 << DSC_A_RX_PI_CONTROL_PI_SLICERS_EN_SHIFT), DSC_A_RX_PI_CONTROL_PI_SLICERS_EN_MASK));

    /* 1 for positive step   */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_RX_PI_CONTROL, 
            ((delta >0) << DSC_A_RX_PI_CONTROL_PI_PHASE_STEP_DIR_SHIFT) , DSC_A_RX_PI_CONTROL_PI_PHASE_STEP_DIR_MASK));

    /* Step set to 1 */
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_RX_PI_CONTROL, 
                1, DSC_A_RX_PI_CONTROL_PI_PHASE_STEP_CNT_MASK));
    if (delta < 0) {
        delta = -(delta);
    }
  
    for (cnt=0; cnt < delta; cnt++) {
        /* Increments/Decrements by 1 every strobe */
        SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_RX_PI_CONTROL, 
                (1 << DSC_A_RX_PI_CONTROL_PI_MANUAL_STROBE_SHIFT), DSC_A_RX_PI_CONTROL_PI_MANUAL_STROBE_MASK));
    }
    return SOC_E_NONE;
}

STATIC 
int32 phy82328_serdes_poll_dsc_state_equals_uc_tune(int32 unit, phy_ctrl_t *pc, uint32 timeout_ms) 
{
  
    uint16 loop, data16 = 0;
    for (loop = 0; loop < 110; loop++) {
        SOC_IF_ERROR_RETURN(READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
              DSC_B_DSC_SM_STATUS_DSC_STATE, &data16));
        data16 = (data16 & DSC_B_DSC_SM_STATUS_DSC_STATE_DSC_STATE_MASK) >> DSC_B_DSC_SM_STATUS_DSC_STATE_DSC_STATE_SHIFT;
        if (data16 == PHY82328_DSC_STATE_UC_TUNE) {
            return (SOC_E_NONE);
        }
        sal_udelay(10 * timeout_ms);
    }
    LOG_CLI((BSL_META_U(unit,
                        "uc Tune State not occured:Timeout:%d\n"),data16));
    return SOC_E_INTERNAL;
}    

STATIC int32
_phy_82328_diag_eyescan_livelink_read(int32 unit, soc_port_t port, int32 *err_counter)
{
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    uint16 data1 = 0, data2 = 0;

    SOC_IF_ERROR_RETURN (
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 
                0, DSC_D_TRNSUM_CTL_1_TRNSUM_EN_MASK));

    SOC_IF_ERROR_RETURN (
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 
            (1 << DSC_D_TRNSUM_CTL_1_TRNSUM_EN_SHIFT), DSC_D_TRNSUM_CTL_1_TRNSUM_EN_MASK));

    SOC_IF_ERROR_RETURN (
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_B_DSC_SM_CTRL_0, 
            (1 << DSC_B_DSC_SM_CTRL_0_UC_TRNSUM_EN_SHIFT) ,DSC_B_DSC_SM_CTRL_0_UC_TRNSUM_EN_MASK));

    SOC_IF_ERROR_RETURN(
              phy82328_serdes_poll_dsc_state_equals_uc_tune(unit, pc, 2000));
    
    SOC_IF_ERROR_RETURN (
            READ_PHY82328_MMF_PMA_PMD_REG (unit, pc, DSC_D_TRNSUM_STS_5, &data1));
    SOC_IF_ERROR_RETURN (
            READ_PHY82328_MMF_PMA_PMD_REG (unit, pc, DSC_D_TRNSUM_STS_6, &data2));
    data2 &= 0x3ff;
    *err_counter = (data1 << 10) | data2;
    SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_RX_PI_CONTROL, 
                (1 << DSC_A_RX_PI_CONTROL_PI_MANUAL_STROBE_SHIFT), DSC_A_RX_PI_CONTROL_PI_MANUAL_STROBE_MASK));
    return SOC_E_NONE;

}

STATIC int32
_phy_82328_diag_eyescan_hoffset_set(int32 unit, soc_port_t port, int32 hoffset)
{
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    
    /* Setup the P1 slicer vertical level  */
    /* Configure dfe_vga_write_tapsel to p1_eyediag mode  */
    SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_C_DFE_VGA_OVERRIDE, 
                (0xd << DSC_C_DFE_VGA_OVERRIDE_DFE_VGA_WRITE_TAPSEL_SHIFT) , DSC_C_DFE_VGA_OVERRIDE_DFE_VGA_WRITE_TAPSEL_MASK));

    /* dfe_vga_write_val[8:3] are used to drive the analog control port. */
    SOC_IF_ERROR_RETURN(
               MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_C_DFE_VGA_OVERRIDE,  
                   ((-(hoffset))<< 3), DSC_C_DFE_VGA_OVERRIDE_DFE_VGA_WRITE_VAL_MASK));

     /* Enable f/w to write the tap values */
    SOC_IF_ERROR_RETURN(
               MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_C_DFE_VGA_OVERRIDE, 
                   (1 << DSC_C_DFE_VGA_OVERRIDE_DFE_VGA_WRITE_EN_SHIFT), DSC_C_DFE_VGA_OVERRIDE_DFE_VGA_WRITE_EN_MASK));
    
    SOC_IF_ERROR_RETURN(phy82328_move_clkp1_offset(unit, pc, -32));
    SOC_IF_ERROR_RETURN(phy82328_move_clkp1_offset(unit, pc, 1));

	return SOC_E_NONE;
}

STATIC int32
_phy_82328_diag_eyescan(int32 unit, soc_port_t port, uint32 inst)
{
    int32 lane;
    int32 intf;
    phy82328_intf_side_t side;
	int32 rv = SOC_E_NONE, hoffset = 0, voffset = 0, err = 0;
    int32 limits[7] = {1835008, 183501, 18350, 1835, 184, 18, 2};
    int8 num_limit = 0;

    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    intf = PHY_DIAG_INST_INTF(inst);
    if (intf == PHY_DIAG_INTF_SYS) {
        side = PHY82328_INTF_SIDE_SYS;
        SOC_IF_ERROR_RETURN(
		   phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_SYS));
	} else {
        side = PHY82328_INTF_SIDE_LINE;
    }
    lane = PHY_DIAG_INST_LN(inst);
    if (PHY82328_SINGLE_PORT_MODE(pc) && (lane != 0)) {
        SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, lane));
    }
    
    SOC_IF_ERROR_RETURN(
        _phy_82328_diag_eyescan_livelink_en(unit, port, 1));

    LOG_CLI((BSL_META_U(unit,
                        "Fast eyescan under Processing")));
    for (hoffset = 31; hoffset>=-31; hoffset--) {
        rv = _phy_82328_diag_eyescan_hoffset_set(unit, port, hoffset);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "82328 failed in HOFFSET SET: u=%d p=%d err=%d\n"),
                                  unit, port, rv));
            goto eye_fail;
        }

        for (voffset=-31; voffset < 32; voffset++) {
            rv = _phy_82328_diag_eyescan_livelink_read(unit, port, &err);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "82328 failed in Read BER: u=%d p=%d err=%d\n"),
                                      unit, port, rv));
                goto eye_fail;
            }
            eye_consol_val[hoffset+31][voffset+31] = err;
        }
        LOG_CLI((BSL_META_U(unit,
                            ".")));
        SOC_IF_ERROR_RETURN(phy82328_move_clkp1_offset(unit, pc, -32));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n\n")));
    LOG_CLI((BSL_META_U(unit,
                        "-30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30\n")));
    LOG_CLI((BSL_META_U(unit,
                        "-|----|----|----|----|----|----|----|----|----|----|----|----|-\n")));
    
    for (hoffset = 31; hoffset >=-31; hoffset = hoffset - 1) {
        for (voffset = -31; voffset < 32; voffset++) {
            err = eye_consol_val[hoffset+31][voffset+31];
            for (num_limit = 0; num_limit < 7; num_limit++) {
                if (err >= limits[num_limit]) {
                     LOG_CLI((BSL_META_U(unit,
                                         "%c"), '0' + num_limit + 1));
                     break;
                }
            }
            if (num_limit == 7) {
                 if ((voffset % 5 )== 0 && (hoffset % 5)==0) {
                     LOG_CLI((BSL_META_U(unit,
                                         "+")));
                 } else if ((voffset % 5)!=0 && (hoffset % 5)==0) {
                     LOG_CLI((BSL_META_U(unit,
                                         "-")));
                 } else if ((voffset % 5)==0 && (hoffset % 5)!=0) {
                     LOG_CLI((BSL_META_U(unit,
                                         ":")));
                 } else {
                     LOG_CLI((BSL_META_U(unit,
                                         " ")));
                 }
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n-|----|----|----|----|----|----|----|----|----|----|----|----|-\n")));
    LOG_CLI((BSL_META_U(unit,
                        "-30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30\n")));

eye_fail:
    SOC_IF_ERROR_RETURN(MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_B_DSC_SM_CTRL_0, 
            0, DSC_B_DSC_SM_CTRL_0_UC_TUNE_EN_MASK));
    SOC_IF_ERROR_RETURN (
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_TRNSUM_CTL_1, 
                0, DSC_D_TRNSUM_CTL_1_TRNSUM_EN_MASK));
    SOC_IF_ERROR_RETURN (
            MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_B_DSC_SM_CTRL_0, 
            (0 << DSC_B_DSC_SM_CTRL_0_UC_TRNSUM_EN_SHIFT) ,DSC_B_DSC_SM_CTRL_0_UC_TRNSUM_EN_MASK));
    SOC_IF_ERROR_RETURN(
            phy82328_serdes_pmd_uc_cmd(unit, port, pc, PHY82328_CMD_UC_CTRL, 
                PHY82328_CMD_UC_CTRL_RESUME, 2000));
    if (PHY82328_SINGLE_PORT_MODE(pc) && (lane != 0)) {
        SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, PHY82328_INTF_SIDE_LINE,
                                                      PHY82328_ALL_LANES));
    }
    if (intf == PHY_DIAG_INTF_SYS) {
        SOC_IF_ERROR_RETURN(
		 phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
	}
	return rv;
}

/*
 * Function:
 *      _phy_82328_diag_dsc 
 * Purpose:
 *      Diag function for DSC .
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      intf - line/system
 *      lane - lane number 
 *      type  - Control to update
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_82328_diag_dsc(int32 unit, soc_port_t port, int32 intf, int32 lane)
{
    int32	 i, lanes;
    uint16 data16 = 0;
    int16 temp = 0;
    uint16 is_micro_stopped = 0;
    phy_ctrl_t *pc;
	phy82328_intf_side_t side;
	phy82328_serdes_lane_state *dsc_cbp;
	phy82328_serdes_lane_state dsc_cb[PHY82328_NUM_LANES];
    pc = EXT_PHY_SW_STATE(unit, port);

	if (PHY82328_SINGLE_PORT_MODE(pc)) {
        lanes = PHY82328_NUM_LANES;
    }
    else {
        lanes = 1;
    }
	side =  (intf == PHY_DIAG_INTF_LINE) ? PHY82328_INTF_SIDE_LINE :
		PHY82328_INTF_SIDE_SYS;
    sal_memset(dsc_cb, 0, (sizeof(dsc_cb)));
    PHY82328_STS_IS_MICRO_STOPPED(unit, port, pc, is_micro_stopped);
    if (!is_micro_stopped) {
        SOC_IF_ERROR_RETURN(
            phy82328_serdes_pmd_uc_cmd(unit, port, pc, PHY82328_CMD_UC_CTRL, PHY82328_CMD_UC_CTRL_STOP_GRACEFULLY, 2000));
    }

    SOC_IF_ERROR_RETURN(
	  phy82328_intf_side_reg_select(unit, port, side));
    for (i = 0; i < lanes; i++) {
		if (PHY82328_SINGLE_PORT_MODE(pc)) {
            /* Select the lane and side to access */
            SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, i));
        }
        dsc_cbp = &(dsc_cb[i]);
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, CKRST_CTRL_OSR_MODE_STATUS, &data16));
        dsc_cbp->osr_mode = data16 & CKRST_CTRL_OSR_MODE_STATUS_ST_OSR_MODE_MASK;
 
        SOC_IF_ERROR_RETURN 
           (phy82328_serdes_rdbl_uc_var (unit, port, pc, 0x0, &data16));
        dsc_cbp->ucv_config = (data16 & 0xff) << 8;
        SOC_IF_ERROR_RETURN 
           (phy82328_serdes_rdbl_uc_var (unit, port, pc, 0x1, &data16));
        dsc_cbp->ucv_config |= (data16 & 0xff);

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, SIGDET_SIGDET_STATUS_0, &data16));
        dsc_cbp->sig_det = (data16 & SIGDET_SIGDET_STATUS_0_SIGNAL_DETECT_MASK); 
        
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TLB_RX_PMD_RX_LOCK_STATUS, &data16));
        dsc_cbp->rx_lock = (data16 & TLB_RX_PMD_RX_LOCK_STATUS_PMD_RX_LOCK_MASK);

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_CDR_STATUS_INTEG_REG, &data16));
        temp = (int16) data16;
        temp <<= (15 -15); 
        temp >>= (15 - 15 + 0);

        dsc_cbp->rx_ppm = (temp/84);
        
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_RX_PI_DIFF_BIN, &data16));
        dsc_cbp->clk90 = (data16 & DSC_A_RX_PI_DIFF_BIN_CNT_D_MINUS_M1_MASK); 
        dsc_cbp->clkp1 = (data16 & DSC_A_RX_PI_DIFF_BIN_CNT_D_MINUS_P1_MASK) 
                         >> DSC_A_RX_PI_DIFF_BIN_CNT_D_MINUS_P1_SHIFT;
                         
        /* drop the MSB, the result is actually valid modulo 128 */
        /* Also flip the sign to account for d-m1, versus m1-d */
        dsc_cbp->clk90 = dsc_cbp->clk90 << 1;
        dsc_cbp->clk90 = -(dsc_cbp->clk90 >> 1);
        dsc_cbp->clkp1 = dsc_cbp->clkp1 << 1;
        dsc_cbp->clkp1 = -(dsc_cbp->clkp1 >> 1);

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_A_CDR_CONTROL_0, &data16));
        dsc_cbp->br_pd_en = (data16 & DSC_A_CDR_CONTROL_0_BR_PD_EN_MASK) >>
                         DSC_A_CDR_CONTROL_0_BR_PD_EN_SHIFT;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_PF_CTRL, &data16));
        dsc_cbp->pf_main = (data16 & DSC_E_DSC_E_PF_CTRL_PF_CTRL_MASK);

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_CTRL, &data16));
        dsc_cbp->pf_hiz = (data16 & DSC_E_DSC_E_CTRL_PF_HIZ_MASK) >>
                           DSC_E_DSC_E_CTRL_PF_HIZ_SHIFT;
        
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_PF2_LOWP_CTRL, &data16));
        dsc_cbp->pf_low = (data16 & DSC_E_DSC_E_PF2_LOWP_CTRL_PF2_LOWP_CTRL_MASK);

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_VGA_P1EYEDIAG_STS, &data16));
        dsc_cbp->vga = (data16 & DSC_D_VGA_P1EYEDIAG_STS_VGA_BIN_MASK);
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 13, 8);
        dsc_cbp->p1_lvl_ctrl = -(temp);

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_DC_OFFSET, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 6, 0);
        dsc_cbp->dc_offset = temp;
        
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, AMS_RX_RX_CTRL_5, &data16));
        dsc_cbp->pf_bst = ((data16 & AMS_RX_RX_CTRL_5_PKBST_MASK) >> AMS_RX_RX_CTRL_5_PKBST_SHIFT);

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_DFE_1_STS, &data16));

        dsc_cbp->dfe1 = (((data16 & DSC_D_DFE_1_STS_DFE_1_E_MASK) >> DSC_D_DFE_1_STS_DFE_1_E_SHIFT) +
                         (data16 & DSC_D_DFE_1_STS_DFE_1_CMN_MASK));
        dsc_cbp->dfe1_dcd = (((data16 & DSC_D_DFE_1_STS_DFE_1_E_MASK) >> DSC_D_DFE_1_STS_DFE_1_E_SHIFT) -
                             ((data16 & DSC_D_DFE_1_STS_DFE_1_O_MASK) >> DSC_D_DFE_1_STS_DFE_1_O_SHIFT)); 
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_DFE_2_STS, &data16));

        dsc_cbp->dfe2 = (((data16 & DSC_D_DFE_2_STS_DFE_2_SE_MASK) >> DSC_D_DFE_2_STS_DFE_2_SE_SHIFT) ?
                        -(((data16 & DSC_D_DFE_2_STS_DFE_2_E_MASK) >> DSC_D_DFE_2_STS_DFE_2_E_SHIFT) +
                                                           (data16 & DSC_D_DFE_2_STS_DFE_2_CMN_MASK)) : 
                        (((data16 & DSC_D_DFE_2_STS_DFE_2_E_MASK) >> DSC_D_DFE_2_STS_DFE_2_E_SHIFT) +
                                                          (data16 & DSC_D_DFE_2_STS_DFE_2_CMN_MASK)));


        dsc_cbp->dfe2_dcd = (((data16 & DSC_D_DFE_2_STS_DFE_2_SE_MASK) >> DSC_D_DFE_2_STS_DFE_2_SE_SHIFT) ?
                            -((data16 & DSC_D_DFE_2_STS_DFE_2_E_MASK) >> DSC_D_DFE_2_STS_DFE_2_E_SHIFT) :
                             ((data16 & DSC_D_DFE_2_STS_DFE_2_E_MASK) >> DSC_D_DFE_2_STS_DFE_2_E_SHIFT)) - 
                            (((data16 & DSC_D_DFE_2_STS_DFE_2_SO_MASK) >> DSC_D_DFE_2_STS_DFE_2_SO_SHIFT) ?
                            -((data16 & DSC_D_DFE_2_STS_DFE_2_O_MASK) >> DSC_D_DFE_2_STS_DFE_2_O_SHIFT) :
                             ((data16 & DSC_D_DFE_2_STS_DFE_2_O_MASK) >> DSC_D_DFE_2_STS_DFE_2_O_SHIFT));

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_DFE_3_4_5_STS, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 5, 0);
        dsc_cbp->dfe3 = temp;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_DFE_3_4_5_STS, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 10, 6);
        dsc_cbp->dfe4 = temp;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_D_DFE_3_4_5_STS, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 15, 11);
        dsc_cbp->dfe5 = temp;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_OFFSET_ADJ_P1_EVEN, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 5, 0);
        dsc_cbp->pe = (4*temp) - 30;
        
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_OFFSET_ADJ_DATA_EVEN, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 5, 0);
        dsc_cbp->ze = (4*temp) - 30;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_OFFSET_ADJ_M1_EVEN, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 5, 0);
        dsc_cbp->me = (4*temp) - 30;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_OFFSET_ADJ_P1_ODD, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 5, 0);
        dsc_cbp->po = (4*temp) - 30;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_OFFSET_ADJ_DATA_ODD, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 5, 0);
        dsc_cbp->zo = (4*temp) - 30;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, DSC_E_DSC_E_OFFSET_ADJ_M1_ODD, &data16));
        temp = (int16)data16;
        PHY82328_CONV_SIGNED_BYTE(temp, 5, 0);
        dsc_cbp->mo = (4*temp) - 30;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_PI_TX_PI_STATUS_1, &data16));
        dsc_cbp->tx_ppm = data16/21;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_STATUS1, &data16));
        dsc_cbp->txfir_pre = data16 & TX_FED_TXFIR_STATUS1_TXFIR_PRE_AFTER_OVR_MASK;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_STATUS2, &data16));
        dsc_cbp->txfir_main = data16 & TX_FED_TXFIR_STATUS2_TXFIR_MAIN_AFTER_OVR_MASK;

        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_STATUS1, &data16));
        dsc_cbp->txfir_post1 = (data16 & TX_FED_TXFIR_STATUS1_TXFIR_POST_AFTER_OVR_MASK) >>
                              TX_FED_TXFIR_STATUS1_TXFIR_POST_AFTER_OVR_SHIFT;
        
        SOC_IF_ERROR_RETURN
            (READ_PHY82328_MMF_PMA_PMD_REG(unit, pc, TX_FED_RPTR_TXFIR_STATUS1, &data16));
        dsc_cbp->txfir_post2 = (data16 & TX_FED_TXFIR_CONTROL2_TXFIR_POST2_MASK) >>
                               TX_FED_TXFIR_CONTROL2_TXFIR_POST2_SHIFT;

        SOC_IF_ERROR_RETURN
            (phy82328_serdes_rdbl_uc_var(unit, port, pc, 0xd, &data16));
        dsc_cbp->heye_left = data16;

        SOC_IF_ERROR_RETURN
            (phy82328_serdes_rdbl_uc_var(unit, port, pc, 0xe, &data16));
        dsc_cbp->heye_right = data16;
        SOC_IF_ERROR_RETURN
            (phy82328_serdes_rdbl_uc_var(unit, port, pc, 0xf, &data16));
        dsc_cbp->veye_upper = data16;
        SOC_IF_ERROR_RETURN
            (phy82328_serdes_rdbl_uc_var(unit, port, pc, 0x10, &data16));
        dsc_cbp->veye_lower = data16;
    }
	/* Restore to default single port register access */
	if (PHY82328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_82328_channel_select(unit, port, side, PHY82328_ALL_LANES));
	}
    SOC_IF_ERROR_RETURN(
        phy82328_intf_side_reg_select(unit, port, PHY82328_INTF_SIDE_LINE));
    if (!is_micro_stopped) {
            SOC_IF_ERROR_RETURN(
                phy82328_serdes_pmd_uc_cmd(unit, port, pc, PHY82328_CMD_UC_CTRL, PHY82328_CMD_UC_CTRL_RESUME, 2000));
        }

    /* display */
    LOG_CLI((BSL_META_U(unit,
                        "\nDSC parameters for port %d\n"), port));

    LOG_CLI((BSL_META_U(unit,
                        "SNO OSR SIGDET UCV_CON RXLCK RXPPM CLK90 CLKP1 "
                        "PFMAIN PFLOW DCOFFSET VGA P1LVLCTRL DFE1 DFE2 "
                        "DFE3 DFE4 DFE5 PE  ZE  ME  PO  ZO  MO TXPPM PRE "
                        "MAIN POST1 POST2 \n")));
    for (i = 0; i < lanes; i++) {
        dsc_cbp = &(dsc_cb[i]);
        LOG_CLI((BSL_META_U(unit,
                            "%03d %03d %06d %07d %05d %05d %05d %05d %06d %05d %08d "
                            "%03d %09d %04d %04d %04d %04d %04d %03d %03d %03d %03d"
                            "%03d %03d %04d %03d %04d %04d %04d \n"), 
                 i, dsc_cbp->osr_mode, dsc_cbp->sig_det, dsc_cbp->ucv_config,
                 dsc_cbp->rx_lock, dsc_cbp->rx_ppm,
                 dsc_cbp->clk90, dsc_cbp->clkp1,
                 dsc_cbp->pf_main, dsc_cbp->pf_low,
                 dsc_cbp->dc_offset,dsc_cbp->vga,dsc_cbp->p1_lvl_ctrl,
                 dsc_cbp->dfe1, dsc_cbp->dfe2,
                 dsc_cbp->dfe3,dsc_cbp->dfe4,dsc_cbp->dfe5,
                 dsc_cbp->pe,dsc_cbp->ze,dsc_cbp->me, dsc_cbp->po,
                 dsc_cbp->zo, dsc_cbp->mo,
                 dsc_cbp->tx_ppm, dsc_cbp->txfir_pre, dsc_cbp->txfir_main,
                 dsc_cbp->txfir_post1, dsc_cbp->txfir_post2));
    }
    return SOC_E_NONE;
}


STATIC int32
phy_82328_diag_ctrl(int32 unit, soc_port_t port, uint32 inst, int32 op_type,  int32 op_cmd,  
					void *arg)   
{
    int32 lane;
    int32 intf;
    int32 rv = SOC_E_NONE;

    lane = PHY_DIAG_INST_LN(inst);
    intf = PHY_DIAG_INST_INTF(inst);
    if (intf == PHY_DIAG_INTF_DFLT) {
        intf = PHY_DIAG_INTF_LINE;
    }

    switch (op_cmd) {
	    case PHY_DIAG_CTRL_DSC:
            rv = _phy_82328_diag_dsc(unit, port, intf, lane);
        break;
        case PHY_DIAG_CTRL_EYE_ENABLE_LIVELINK:
        case PHY_DIAG_CTRL_EYE_DISABLE_LIVELINK:
        case PHY_DIAG_CTRL_EYE_CLEAR_LIVELINK:
        case PHY_DIAG_CTRL_EYE_START_LIVELINK:
        case PHY_DIAG_CTRL_EYE_STOP_LIVELINK:
        case PHY_DIAG_CTRL_EYE_READ_LIVELINK:
        case PHY_DIAG_CTRL_EYE_GET_INIT_VOFFSET:
        case PHY_DIAG_CTRL_EYE_SET_HOFFSET:
        case PHY_DIAG_CTRL_EYE_SET_VOFFSET:
        case PHY_DIAG_CTRL_EYE_GET_MAX_LEFT_HOFFSET:
        case PHY_DIAG_CTRL_EYE_GET_MAX_RIGHT_HOFFSET:
        case PHY_DIAG_CTRL_EYE_GET_MIN_VOFFSET:
        case PHY_DIAG_CTRL_EYE_GET_MAX_VOFFSET:
        case PHY_DIAG_CTRL_EYE_MARGIN_VEYE:
        case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_LEFT:
        case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_RIGHT:
            rv = SOC_E_UNAVAIL;
        break;
        case PHY_DIAG_CTRL_START_FAST_EYESCAN:
            rv =  _phy_82328_diag_eyescan(unit, port, inst);
        break;

        default:
           if (op_type == PHY_DIAG_CTRL_SET) {
               SOC_IF_ERROR_RETURN
                   (_phy_82328_control_set(unit, port, intf, lane, op_cmd, PTR_TO_INT(arg)));
           } else if (op_type == PHY_DIAG_CTRL_GET) {
               SOC_IF_ERROR_RETURN
                   (_phy_82328_control_get(unit, port, intf, lane, op_cmd, (uint32 *)arg));
           } else {
               LOG_ERROR(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "82328 diag_ctrl bad operation: u=%d p=%d ctrl=0x%x\n"), 
                                     unit, port, op_cmd));
               rv = SOC_E_UNAVAIL;
        }
        break;
    }
    return rv;
}

/*
* Function:
*      phy_82328_ability_remote_get
* Purpose:
*      Get the device's complete abilities.
* Parameters:
*      unit - StrataSwitch unit #.
*      port - StrataSwitch port #.
*      ability - return device's abilities.
* Returns:
*      SOC_E_XXX
*/

STATIC int
phy_82328_ability_remote_get(int unit, soc_port_t port, 
                            soc_port_ability_t *ability)
{
    int an, an_done;
    uint16      pause_adv;
    soc_port_mode_t mode;
    phy_ctrl_t *pc, *int_pc;
	phy82328_intf_cfg_t  *line_intf;

    if (ability == NULL) {
        return SOC_E_PARAM;
    }
	
    /* Only firmware has visibility into remote ability */
	sal_memset(ability, 0, sizeof(soc_port_ability_t));

    pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));
        if (  (line_intf->type != SOC_PORT_IF_CR4)
              && (line_intf->type != SOC_PORT_IF_CR)
              && (line_intf->type != SOC_PORT_IF_KR4)
              && (line_intf->type != SOC_PORT_IF_KR)
              && (line_intf->type != SOC_PORT_IF_GMII)
              && (line_intf->type != SOC_PORT_IF_SGMII)) {
            return SOC_E_NONE;
        }

    if (line_intf->type == SOC_PORT_IF_GMII) {
        int_pc = INT_PHY_SW_STATE(unit, port);
        if (int_pc) {
            SOC_IF_ERROR_RETURN(PHY_ABILITY_REMOTE_GET(int_pc->pd, unit, port, ability));
        }
        return SOC_E_NONE;
    }

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;

    mode = 0;
	/* Return what the firmware is setting */
    /* If an done, then return the speed for the port */
    SOC_IF_ERROR_RETURN(phy_82328_an_get(unit, port, &an, &an_done));
    if (an && an_done) {
        if(PHY82328_SINGLE_PORT_MODE(pc)) {
            ability->speed_full_duplex = SOC_PA_SPEED_40GB;
        }
        else {
            ability->speed_full_duplex = SOC_PA_SPEED_10GB;
        }
        
        mode = 0;
            SOC_IF_ERROR_RETURN
              (READ_PHY82328_MMF_AN_REG(unit, pc, IEEE1_AN_BLK_AN_LP_BASE_PAGE_ABILITY_1_REG, &pause_adv));
        switch (pause_adv & QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_MASK) {		
            case (1<<QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT):  /*7.0x0010.10 1*/
			    mode = SOC_PA_PAUSE_TX;		
            break;
            case (1<<(QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT+1)): /*7.0x0010.11 1*/
                mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;		
            case (1<<(QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT+1)) |
				(1<<QSFI_AN_IEEE1_AN_ADVERTISEMENT_1_REGISTER_PAUSE_SHIFT):/*7.0x0010.11:10 1*/
                mode = SOC_PA_PAUSE_RX;
            break;		
            default:
                mode = 0;
            break;
        }
        ability->pause = mode;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_82328_ability_remote_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82328_precondition_before_probe
 * Purpose:
 *      When the device is in 40G, ie. after a power reset, and the 10G
 *      port being probed does not have the 40G address in PRTAD, the
 *      device will not respond until the port that has the 40G address
 *      is probed. To avoid not programming ports correctly, make
 *      sure the device is in quad (4x10G) PMD mode when the SDK port is 10G.
 *      Need to set "port_phy_precondition_before_probe_xe0/4" in config file.
 * Parameters: 
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */

STATIC int32
phy_82328_precondition_before_probe(int32 unit, phy_ctrl_t *pc)
{
    int32 ids;
    int32 rv = SOC_E_NOT_FOUND;
    uint16 pmd_ctrl;
    uint16 id0, id1;
    uint16 saved_phy_id;
    /* Not required when the port is single PMD */
    if (PHY82328_SINGLE_PORT_MODE(pc)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PMD must not be called when in single PMD\n")));
        return rv;
    }
    saved_phy_id = pc->phy_id;

    /* Look for first address that responds */
    for (ids = 0; ids < 4; ids++) {

        pc->phy_id = (saved_phy_id & ~0x3) + ids;
         LOG_INFO(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Try device with address %x\n"), pc->phy_id));

        /*
         * Make sure the device has the correct dev IDs
         * Note that this is being called either because the soc probe found
         * the ids or there was an id override. In the later case, the device
         * might not have the 82328 ids so this PMD setup will not be supported
         */
        id0 = 0;
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                      IEEE0_BLK_PMA_PMD_PMD_IDENTIFIER_REGISTER_0,
                                      &id0);
        if (id0 != 0xae02) {
            continue;
        }
        id1 = 0;
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                      IEEE0_BLK_PMA_PMD_PMD_IDENTIFIER_REGISTER_1,
                                      &id1);
        if (id1 != 0x5210) {
            continue;
        }

        /* Read device ids correctly so use this address to make sure device is in 10G */
        pmd_ctrl = 0;
        READ_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                      MGT_TOP_SINGLE_PMD_CTRL, &pmd_ctrl);
        if (pmd_ctrl & MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Device responded to address %x. Setting quad PMD mode\n"),
                      pc->phy_id));
            pmd_ctrl = 0;   /* Clear single PMD */
            SOC_IF_ERROR_RETURN(
                MODIFY_PHY82328_MMF_PMA_PMD_REG(unit, pc,
                                            MGT_TOP_SINGLE_PMD_CTRL , pmd_ctrl,
                                            MGT_TOP_CMN_CTLR_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK));
        }
        /* Found the address that responds and the device now is in 4x10G */
        rv = SOC_E_NONE;
        break;
    }

    pc->phy_id = saved_phy_id;

    return rv;
}


/*
 * Function:
 *     phy_82328_multi_get
 * Purpose:
 *      Multi data read interface through device I2C
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      flags - flags passed in the transfer
 *      dev_adr - Device address from which data need to be read.
 *      offset - Starting addres from which data read starts.
 *      max_size - Maximum Number of bytes to read
 *      data - data buffer to receive the data
 *      actual_size - Actual received number of byte
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_82328_multi_get(int unit, soc_port_t port, uint32 flags,
                    uint32 dev_addr, uint32 offset, int max_size, uint8 *data,
                    int *actual_size)
{
    int rv;
    rv = phy_82328_i2cdev_read(unit, port, (int)dev_addr, (int)offset, max_size,
                                   data);
    if (SOC_FAILURE(rv)) {
        *actual_size = 0;
        return SOC_E_FAIL;
    }
    *actual_size = max_size;
    return SOC_E_NONE;
}

/*
 * Variable:   phy_82328drv_xe
 * Purpose:    Phy driver callouts for a  phy 82328
 * Notes:     
 */
phy_driver_t phy_82328drv_xe = {
    "82328 40-Gigabit PHY Driver",
    phy_82328_init,              /* phy_init                     */
    phy_null_reset,              /* phy_reset                    */
    phy_82328_link_get,          /* phy_link_get                 */
    phy_82328_enable_set,        /* phy_enable_set               */
    phy_null_enable_get,         /* phy_enable_get               */
    phy_null_duplex_set,         /* phy_duplex_set               */
    phy_null_duplex_get,         /* phy_duplex_get               */
    phy_82328_speed_set,         /* phy_speed_set                */
    phy_82328_speed_get,         /* phy_speed_get                */
    phy_null_set,                /* phy_master_set               */
    phy_null_zero_get,           /* phy_master_get               */
    phy_82328_an_set,            /* phy_an_set                   */
    phy_82328_an_get,            /* phy_an_get                   */
    phy_null_mode_set,           /* phy_adv_local_set            */
    phy_null_mode_get,           /* phy_adv_local_get            */
    phy_null_mode_get,           /* phy_adv_remote_get           */
    phy_82328_lb_set,            /* phy_lb_set                   */
    phy_82328_lb_get,            /* phy_lb_get                   */
    phy_82328_interface_set,     /* phy_interface_set            */
    phy_82328_interface_get,     /* phy_interface_get            */
    phy_null_mode_get,           /* phy_ability                  */
    NULL,                        /* phy_linkup_evt               */
    NULL,                        /* phy_linkdn_evt               */
    phy_null_mdix_set,           /* phy_mdix_set                 */
    phy_null_mdix_get,           /* phy_mdix_get                 */
    phy_null_mdix_status_get,    /* phy_mdix_status_get          */
#if 1
    NULL,                        /* phy_medium_config_set        */
    NULL,                        /* phy_medium_config_get        */
    phy_null_medium_get,         /* phy_medium_get               */
#endif
    NULL,                        /* phy_cable_diag               */
    NULL,                        /* phy_link_change              */
    phy_82328_control_set,       /* phy_control_set              */
    phy_82328_control_get,       /* phy_control_get              */
    phy_82328_reg_read,          /* phy_reg_read                 */
    phy_82328_reg_write,         /* phy_reg_write                */
    NULL,                        /* phy_reg_modify               */
    NULL,                        /* phy_notify                   */
    phy_82328_probe,             /* phy_probe                    */
    phy_82328_ability_advert_set,/* phy_ability_advert_set       */
    phy_82328_ability_advert_get,/* phy_ability_advert_get       */
    phy_82328_ability_remote_get,/* phy_ability_remote_get       */
    phy_82328_ability_local_get, /* phy_ability_local_get        */
    phy_82328_firmware_set,      /* phy_firmware_set             */
    NULL,                        /* phy_timesync_config_set      */
    NULL,                        /* phy_timesync_config_get      */
    NULL,                        /* phy_timesync_control_set     */
    NULL,                        /* phy_timesync_control_get     */
    phy_82328_diag_ctrl,         /* phy_diag_ctrl                */
    NULL,                        /* pd_lane_control_set */
    NULL,                        /* pd_lane_control_get */
    NULL,                        /* pd_lane_reg_read */
    NULL,                        /* pd_lane_reg_write */
    NULL,                        /* pd_oam_config_set */
    NULL,                        /* pd_oam_config_get */
    NULL,                        /* pd_oam_control_set */
    NULL,                        /* pd_oam_control_get */
    NULL,                        /* pd_timesync_enhanced_capture_get */
    phy_82328_multi_get,         /* pd_multi_get */
    phy_82328_precondition_before_probe /* pd_precondition_before_probe */

};
#else /* INCLUDE_PHY_82328 */
typedef int _phy_82328_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_82328 */ 
