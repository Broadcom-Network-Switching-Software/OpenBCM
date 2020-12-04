/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       phy84328.c
 * Purpose:    Phys Driver support for Broadcom 84328 40G phy
 * Note:      
 *
 * Specifications:

 * Repeater mode only, no retimer operation.

 * Supports the following data speeds:
 * 1.25 Gbps line rate (1 GbE data rate, 8x oversampled over 10 Gbps line rate)
 * 10.3125 Gbps line rate (10 GbE data rate)
 * 11.5 Gpbs line rate (for backplane application, proprietary data rate)
 * 4x10.3125 Gbps line rate (40 GbE data rate)
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
 * Single 125.00 MHz differential clock for 1.25 Gbps, 10.3125 Gbps and 11.5 Gbps
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

#if defined(INCLUDE_PHY_84328)   

static unsigned int phy84328_maj_rev = 1200;
static unsigned int phy84328_min_rev = 0;

#include <sal/types.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <shared/bsl.h>
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phydefs.h"      /* Must include before other phy related includes */
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyxehg.h"
#include "phy84328.h"

#define PHY84328_SINGLE_PORT_MODE(_pc)  \
(SOC_INFO((_pc)->unit).port_num_lanes[(_pc)->port] >= 4)
#define PHY84328_ID_84328 0x84328 /* Gallardo_oct_opt */
#define PHY84328_ID_84324 0x84324 /* Gallardo_opt     */
#define PHY84328_ID_84088 0x84088 /* Gallardo_oct_bp  */
#define PHY84328_ID_84024 0x84024 /* Gallardo_bp      */
/* PMD_CONTROL_REG - speed selection */
#define PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_MASK		\
	(PHY84328_DEV1_PMD_CONTROL_REGISTER_SPEED_SELECTIONLSB_MASK | \
	 PHY84328_DEV1_PMD_CONTROL_REGISTER_SPEED_SELECTIONMSB_MASK | \
	 PHY84328_DEV1_PMD_CONTROL_REGISTER_SPEED_SELECTION_MASK)

#define PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_SELECTIONLSB		\
	(1 << PHY84328_DEV1_PMD_CONTROL_REGISTER_SPEED_SELECTIONLSB_SHIFT)
#define PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_SELECTIONMSB		\
	(1 << PHY84328_DEV1_PMD_CONTROL_REGISTER_SPEED_SELECTIONMSB_SHIFT)

#define PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_10GB				\
	(PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_SELECTIONLSB |		\
	 PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_SELECTIONMSB)

#define PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_40GB				\
	(PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_SELECTIONLSB |		\
	 PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_SELECTIONMSB |		\
	 (2 << PHY84328_DEV1_PMD_CONTROL_REGISTER_SPEED_SELECTION_SHIFT))

#define PHY84328_PMD_CONTROL_REG_VAL_4_SPEED_1GB				\
	(1 << PHY84328_DEV1_PMD_CONTROL_REGISTER_SPEED_SELECTIONMSB_SHIFT)

#define PHY84328_DEV1_PMD_CONTROL_REGISTER_PMA_LOOPBACK  \
	(1 << PHY84328_DEV1_PMD_CONTROL_REGISTER_PMA_LOOPBACK_SHIFT) 

/* PMD_CONTROL_2_REG - PMA type selection */
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_LR4		0x23
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_SR4		0x22
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_CR4		0x21
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_KR4		0x20
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_10BASET	0x0f
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_100BASETX 0x0e
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_KX		0x0d
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_1000BASET	0x0c
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_KR		0x0b
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_KX4		0x0a
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_10GBASET	0x09
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_LRM		0x08
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_SR		0x07
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_LR		0x06
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_ER		0x05
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_LX4		0x04
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_SW		0x03
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_LW		0x02
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_EW		0x01
#define PHY84328_DEV1_PMD_CONTROL_2_REGISTER_PMA_TYPE_CX4		0x00

/* GP_REGISTER_0 new defined bits */
#define	PHY84328_DEV1_GP_REG_0_P4_5_CTRL                (1 << 1)
#define	PHY84328_DEV1_GP_REG_0_TX_GPIO                  (1 << 2)

/* GP_REGISTER_1 */
#define	PHY84328_DEV1_GP_REGISTER_1_LINE_TYPE_BKPLANE	(1 << PHY84328_DEV1_GP_REGISTER_1_LINE_TYPE_SHIFT)
#define	PHY84328_DEV1_GP_REGISTER_1_SYSTEM_TYPE_BKPLANE	(1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_TYPE_SHIFT)
#define	PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_COPPER	(1 << PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_SHIFT)
#define	PHY84328_DEV1_GP_REGISTER_1_SYSTEM_CU_TYPE_COPPER (1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_CU_TYPE_SHIFT)
#define PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE		(1 << PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_SHIFT)
#define PHY84328_DEV1_GP_REGISTER_1_SPEED_100G          (1 << PHY84328_DEV1_GP_REGISTER_1_SPEED_100G_SHIFT)
#define PHY84328_DEV1_GP_REGISTER_1_SPEED_42G           ((1 << PHY84328_DEV1_GP_REGISTER_1_SPEED_40G_SHIFT) |  \
                                                         (1 << PHY84328_DEV1_GP_REGISTER_1_SPEED_1G_SHIFT)  |  \
                                                         (1 << PHY84328_DEV1_GP_REGISTER_1_SPEED_10G_SHIFT))
#define PHY84328_DEV1_GP_REGISTER_1_SPEED_40G			(1 << PHY84328_DEV1_GP_REGISTER_1_SPEED_40G_SHIFT)
#define PHY84328_DEV1_GP_REGISTER_1_SPEED_10G			(1 << PHY84328_DEV1_GP_REGISTER_1_SPEED_10G_SHIFT)
#define PHY84328_DEV1_GP_REGISTER_1_SPEED_1G			(1 << PHY84328_DEV1_GP_REGISTER_1_SPEED_1G_SHIFT)

/* Datapath4 */
#define PHY84328_DEV1_GP_REGISTER_1_DATAPATH_MASK		0x0060
#define PHY84328_DEV1_GP_REGISTER_1_DATAPATH_4_DEPTH1	0x0040
#define PHY84328_DEV1_GP_REGISTER_1_DATAPATH_4_DEPTH2	0x0060

/* GP_REGISTER_3 - microcode updated */
#define PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK  0x0080

/* Rx polarity inversion */
#define PHY84328_DEV1_GP_REGISTER_RX_INV				0xc0ba
#define PHY84328_DEV1_GP_REGISTER_RX_INV_FORCE			(1 << 3)
#define PHY84328_DEV1_GP_REGISTER_RX_INV_INVERT			(1 << 2)

/* ANARXSTATUS - link status */
#define PHY84328_DEV1_ANARXSTATUS_RXSTATUS_SIGDET		(1 << 15)
#define PHY84328_DEV1_ANARXSTATUS_RXSTATUS_CDR			(1 << 12)
/* Available when PRBS status is selected in anaRxControl */
#define PHY84328_DEV1_ANARXSTATUS_RXSTATUS_RX_PRBS_LOCK	(1 << 15)
#define PHY84328_DEV1_ANARXSTATUS_RXSTATUS_RX_PRBS_ERR_MASK	(0x1fff)

/* Squelching done with lectrical idles */
#define PHY84328_DEV1_ANATXACONTROL2_TX_ELECTRAL_IDLE	0x0100

#define PHY84328_CSR_REG_MODULE_AUTO_DETECT		(1 << 4)
/* 
 * This bounds how long to wait until giving up on configuration updates
 * - Delay is given in us
 * - timeout = (MAX_RETRIES * DELAY)us
 */
#define PHY84328_UPDATE_CHECK_DELAY				20
#define PHY84328_UPDATE_CHECK_MAX_RETRIES		100


/* I2C related defines */
#define PHY84328_BSC_XFER_MAX     0x1F9
#define PHY84328_BSC_WR_MAX       16
#define PHY84328_WRITE_START_ADDR 0x8007
#define PHY84328_READ_START_ADDR  0x8007
#define PHY84328_WR_FREQ_400KHZ   0x0100
#define PHY84328_2W_STAT          0x000C
#define PHY84328_2W_STAT_IDLE     0x0000
#define PHY84328_2W_STAT_COMPLETE 0x0004
#define PHY84328_2W_STAT_IN_PRG   0x0008
#define PHY84328_2W_STAT_FAIL     0x000C
#define PHY84328_BSC_WRITE_OP     0x22
#define PHY84328_BSC_READ_OP      0x2
#define PHY84328_I2CDEV_WRITE     0x1
#define PHY84328_I2CDEV_READ      0x0
#define PHY84328_I2C_8BIT         0
#define PHY84328_I2C_16BIT        1
#define PHY84328_I2C_TEMP_RAM     0xE
#define PHY84328_I2C_OP_TYPE(access_type,data_type) \
        ((access_type) | ((data_type) << 8))
#define PHY84328_I2C_ACCESS_TYPE(op_type) ((op_type) & 0xff)
#define PHY84328_I2C_DATA_TYPE(op_type)   (((op_type) >> 8) & 0xff)
#define I2CM_IDLE_WAIT_MSEC        350
#define I2CM_IDLE_WAIT_CHUNK       10
#define I2CM_IDLE_WAIT_COUNT       (I2CM_IDLE_WAIT_MSEC / I2CM_IDLE_WAIT_CHUNK)



#define PHY84328_ALL_LANES						0x000f

#define PHY84328_PREEMPH_CTRL_FORCE_SHIFT		15
#define PHY84328_PREEMPH_CTRL_POST_TAP_SHIFT	10
#define PHY84328_PREEMPH_CTRL_MAIN_TAP_SHIFT	4
#define PHY84328_PREEMPH_CTRL_PRE_TAP_SHIFT		0

#define PHY84328_PREEMPH_FORCE_GET(_val)		(((_val) >> PHY84328_PREEMPH_CTRL_FORCE_SHIFT) & 0x1)
#define PHY84328_PREEMPH_POST_TAP_GET(_val)		(((_val) >> PHY84328_PREEMPH_CTRL_POST_TAP_SHIFT) & 0xf)
#define PHY84328_PREEMPH_MAIN_TAP_GET(_val)		(((_val) >> PHY84328_PREEMPH_CTRL_MAIN_TAP_SHIFT) & 0x1f)
#define PHY84328_PREEMPH_PRE_TAP_GET(_val)		(((_val) >> PHY84328_PREEMPH_CTRL_PRE_TAP_SHIFT) & 0x7)

#define PHY84328_PREEMPH_FORCE_SET(_val)		(((_val) & 1) << PHY84328_PREEMPH_CTRL_FORCE_SHIFT)
#define PHY84328_PREEMPH_POST_TAP_SET(_val)		(((_val) & 0xf) << PHY84328_PREEMPH_CTRL_POST_TAP_SHIFT)
#define PHY84328_PREEMPH_MAIN_TAP_SET(_val)		(((_val) & 0x1f) << PHY84328_PREEMPH_CTRL_MAIN_TAP_SHIFT)
#define PHY84328_PREEMPH_PRE_TAP_SET(_val)		(((_val) & 7) << PHY84328_PREEMPH_CTRL_PRE_TAP_SHIFT)

/* CL73 pause advertisement */
#define CL73_AN_ADV_PAUSE                       (1 << 10)
#define CL73_AN_ADV_ASYM_PAUSE                  (1 << 11)

/* Multicore port */
#define PHY84328_MAX_CORES                      3
#define PHY84328_CORE_STATE_SIZE                (sizeof(phy_ctrl_t) + sizeof(phy84328_dev_desc_t))

#define PHY84328_MULTI_CORE_MODE(_pc)           ((_pc)->phy_mode == PHYCTRL_MULTI_CORE_PORT)
#define PHY84328_VALID_CORE(_c)                 ((_c) < PHY84328_MAX_CORES)
#define PHY84328_CORES(_u, _p)                  ((SOC_INFO((_u)).port_num_lanes[(_p)] + 3) / 4)
#define PHY84328_CORE_PC(_pc, _c)               (phy_ctrl_t *) \
        (((uint8 *)(_pc)) + (PHY84328_CORE_STATE_SIZE * (_c)))

#define PHY84328_CORE_STATE_SET(unit, port, core, pc)  \
    do {\
        EXT_PHY_SW_STATE(unit, port) = PHY84328_CORE_PC(pc, core);\
        if (core > 0) { \
            (PHY84328_CORE_PC(pc, core))->flags  = (pc)->flags;\
        } \
    } while (0)

#define PHY84328_CALL_SET(_u, _p, _func) \
        do { \
            int _rv = SOC_E_NONE; \
            phy_ctrl_t *_pc = EXT_PHY_SW_STATE((_u), (_p)); \
            if (DBG_FLAGS(_pc) & PHY84328_DBG_F_API_SET) { \
                LOG_INFO(BSL_LS_SOC_PHY,\
                      (BSL_META_U(unit,\
                           "%s(%d, %d)\n"), FUNCTION_NAME(), (_u), (_p))); \
            } \
            if (! PHY84328_MULTI_CORE_MODE(_pc)) { \
                _rv = (_func); \
            } else { \
                int _c, _ac; \
                _ac = PHY84328_CORES((_u), (_p)); \
                for (_c = 0; _c < _ac; _c++) { \
                    PHY84328_CORE_STATE_SET((_u), (_p), _c, _pc); \
                    _rv = (_func); \
                    if (_rv != SOC_E_NONE) { \
                        break; \
                    } \
                } \
                PHY84328_CORE_STATE_SET((_u), (_p), 0, _pc); \
            } \
            return _rv; \
         } while (0)


#define PHY84328_CALL_GET(_u, _p, _func, _v, _pv) \
    do { \
        int _rv = SOC_E_NONE; \
        phy_ctrl_t *_pc = EXT_PHY_SW_STATE((_u), (_p)); \
        if (DBG_FLAGS(_pc) & PHY84328_DBG_F_API_GET) { \
            LOG_INFO(BSL_LS_SOC_PHY,\
                      (BSL_META_U(unit,\
                       "%s(%d, %d)\n"), FUNCTION_NAME(), (_u), (_p))); \
        } \
        if (! PHY84328_MULTI_CORE_MODE(_pc)) { \
            _rv = (_func); \
        } else { \
            int _c, _ac; \
            _ac = PHY84328_CORES((_u), (_p)); \
            for (_c = 0; _c < _ac; _c++) { \
                PHY84328_CORE_STATE_SET((_u), (_p), _c, _pc); \
                _rv = (_func); \
                if (_rv != SOC_E_NONE) { \
                    break; \
                } \
                if (_c == 0) { \
                    (_pv) = *(_v); \
                } else if (*(_v) != (_pv)) { \
                    LOG_ERROR(BSL_LS_SOC_PHY,\
                                  (BSL_META_U(unit,\
                              "84328 %s val does not match for all cores: u=%d p=%d\n"), \
                              FUNCTION_NAME(), (_u), (_p))); \
                } \
            } \
            PHY84328_CORE_STATE_SET((_u), (_p), 0, _pc); \
        } \
        return _rv; \
    } while (0)


#define PHY_84328_MICRO_PAUSE(_u, _p, _s) \
    do { \
        phy_ctrl_t *_pc; \
        _pc = EXT_PHY_SW_STATE((_u), (_p)); \
        if (DEVREV(_pc) == 0x00a0) { \
            _phy_84328_micro_pause((_u), (_p), (_s)); \
        } \
    } while (0) 

#define PHY_84328_MICRO_RESUME(_u, _p) \
    do { \
        phy_ctrl_t *_pc; \
        _pc = EXT_PHY_SW_STATE((_u), (_p)); \
        if (DEVREV(_pc) == 0x00a0) { \
            _phy_84328_micro_resume((_u), (_p)); \
        } \
    } while (0) 

/*
 * PHY system interface types 
 * These values are used when configuring system side interface with spn_PHY_SYS_INTERFACE 
 * Always add at the end but never change the order!! Adding also requires updating 
 * grog documentation
 */
enum {
	PHY_SYSTEM_INTERFACE_DFLT	= 0,
	PHY_SYSTEM_INTERFACE_KX		= 1,
	PHY_SYSTEM_INTERFACE_KR		= 2,
	PHY_SYSTEM_INTERFACE_SR		= 3,
	PHY_SYSTEM_INTERFACE_LR		= 4,
	PHY_SYSTEM_INTERFACE_CR		= 5,	/* System side as XFI-DFE, or system side DAC */
	PHY_SYSTEM_INTERFACE_KR4 	= 6,
	PHY_SYSTEM_INTERFACE_SR4 	= 7,
	PHY_SYSTEM_INTERFACE_LR4 	= 8,
	PHY_SYSTEM_INTERFACE_CR4 	= 9,    /* System side as XLAUI-DFE, or system side DAC */
	PHY_SYSTEM_INTERFACE_XFI 	= 10,
	PHY_SYSTEM_INTERFACE_XLAUI	= 11,
	PHY_SYSTEM_INTERFACE_INVALID	/* must be last */
}; 

soc_port_if_t phy_sys_to_port_if[] = {
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

/* interface params */
typedef struct phy84328_intf_cfg {
	int					speed;
	soc_port_if_t		type;
} phy84328_intf_cfg_t;

typedef	enum {
	PHY84328_DATAPATH_20,
	PHY84328_DATAPATH_4_DEPTH1,
	PHY84328_DATAPATH_4_DEPTH2
} phy84328_datapath_t;

typedef struct phy84328_counters {
	uint32				intf_updates;
	uint32				link_down;
	uint32				retry_serdes_link;
	uint32				speed_chk_err;
	uint32				intf_chk_err;
	uint32				side_sel_err;
	uint32				no_cdr;
	uint32				micro_nopause;
} phy84328_counters_t;

/* 
 * Software RX LOS (Loss of Signal) WAR
 * The workaround handles conditions when loss of signal is not reliably 
 * detected. A typical scenario is when the cable is still plugged to a module but
 * the link partner has disable its tx, in which case some modules do not correctly
 * report loss of signal.
 */
typedef enum {
    PHY84328_SW_RX_LOS_RESET,
    PHY84328_SW_RX_LOS_INITIAL_LINK,
    PHY84328_SW_RX_LOS_LINK,
    PHY84328_SW_RX_LOS_START_TIMER,
    PHY84328_SW_RX_LOS_RX_RESTART,
    PHY84328_SW_RX_LOS_IDLE
} phy84328_sw_rx_los_states_t;

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
const static char *sw_rx_los_state_names[] = {
    "RESET",
    "INITIAL_LINK",
    "LINK",
    "START_TIMER",
    "RX_RESTART",
    "IDLE"
};
#endif /* BROADCOM_DEBUG || DEBUG_PRINT */

typedef struct {
    uint8				cfg_enable;				/* configured with ph control or not */
    uint8				cur_enable;				/* currently enabled or not */
    uint8				sys_link;
	uint8               link_no_pcs;
    uint8				link_status;
	uint8               fault_report_dis;
	uint8				ls_ticks;
	uint32				restarts;	
	mac_driver_t		*macd;
    phy84328_sw_rx_los_states_t	state;
} phy84328_sw_rx_los_t;

#define PHY84328_LINK_DEBOUNCE	20
typedef struct {
	int					cur_enable;
	int					cfg_enable;
	int					debounce;
} phy84328_link_mon_t;

typedef struct {
	int					enable;
	int					count;					/* resume only if count==0 */
} phy84328_micro_ctrl_t;

#define PHY84328_NUM_LANES      4

#define PHY84328_DBG_F_REG      (1 << 0)
#define PHY84328_DBG_F_EYE      (1 << 1)
#define PHY84328_DBG_F_DWLD     (1 << 3)
#define PHY84328_DBG_F_API_SET  (1 << 4)
#define PHY84328_DBG_F_API_GET  (1 << 5)

typedef struct {
	uint32				devid;
	uint16				devrev;
    int                 core_num;
	uint32				dbg_flags;
	int					p2l_map[PHY84328_NUM_LANES];  /* index: physical lane, array element: */
	phy84328_intf_cfg_t line_intf;
	phy84328_intf_cfg_t sys_intf;
	phy84328_sw_rx_los_t sw_rx_los;					  /* sw_rx_los war */
    int                 fw_rx_los;                    /* rx LOS handled by firmware */
	phy84328_counters_t counters;
	int					cur_link;
	uint16				pol_tx_cfg;
	uint16				pol_rx_cfg;
	int					mod_auto_detect;			  /* module auto detect enabled */
	phy84328_datapath_t cfg_datapath;
	phy84328_datapath_t cur_datapath;
	phy84328_link_mon_t link_mon;
	int					sys_forced_cl72;
	int					an_en;
	int					force_20bit;
	int					sync_init;
	int					int_phy_re_en;
	soc_timeout_t       sync_to;
	uint16				logical_lane0;
	int					update_config;
	int					bypass_ss_tuning;
	phy84328_micro_ctrl_t micro_ctrl;
	int					cfg_sys_intf;
	int					port_enable_delay;
} phy84328_dev_desc_t;

#define DEVID(_pc)				(((phy84328_dev_desc_t *)((_pc) + 1))->devid)
#define DEVREV(_pc)				(((phy84328_dev_desc_t *)((_pc) + 1))->devrev)
#define CORE_NUM(_pc)           (((phy84328_dev_desc_t *)((_pc) + 1))->core_num)
#define DBG_FLAGS(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->dbg_flags)
#define P2L_MAP(_pc,_ix)		(((phy84328_dev_desc_t *)((_pc) + 1))->p2l_map[(_ix)])
#define LINE_INTF(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->line_intf)
#define SYS_INTF(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->sys_intf)
#define SW_RX_LOS(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->sw_rx_los)
#define FW_RX_LOS(_pc)          (((phy84328_dev_desc_t *)((_pc) + 1))->fw_rx_los)
#define COUNTERS(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->counters)
#define CUR_LINK(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->cur_link)
#define POL_TX_CFG(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->pol_tx_cfg)
#define POL_RX_CFG(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->pol_rx_cfg)
#define MOD_AUTO_DETECT(_pc)	(((phy84328_dev_desc_t *)((_pc) + 1))->mod_auto_detect)
#define CFG_DATAPATH(_pc)		(((phy84328_dev_desc_t *)((_pc) + 1))->cfg_datapath)
#define CUR_DATAPATH(_pc)		(((phy84328_dev_desc_t *)((_pc) + 1))->cur_datapath)
#define SYS_FORCED_CL72(_pc)	(((phy84328_dev_desc_t *)((_pc) + 1))->sys_forced_cl72)
#define AN_EN(_pc)				(((phy84328_dev_desc_t *)((_pc) + 1))->an_en)
#define FORCE_20BIT(_pc)        (((phy84328_dev_desc_t *)((_pc) + 1))->force_20bit)
#define FORCE_20BIT_LB          (1 << 0)
#define FORCE_20BIT_AN          (1 << 1)

#define LINK_MON(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->link_mon)
#define SYNC_INIT(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->sync_init)
#define SYNC_TO(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->sync_to)
#define INT_PHY_RE_EN(_pc)		(((phy84328_dev_desc_t *)((_pc) + 1))->int_phy_re_en)
#define LOGICAL_LANE0(_pc)		(((phy84328_dev_desc_t *)((_pc) + 1))->logical_lane0)
#define BYPASS_SS_TUNING(_pc)	(((phy84328_dev_desc_t *)((_pc) + 1))->bypass_ss_tuning)
#define MICRO_CTRL(_pc)			(((phy84328_dev_desc_t *)((_pc) + 1))->micro_ctrl)
#define CFG_SYS_INTF(_pc)		(((phy84328_dev_desc_t *)((_pc) + 1))->cfg_sys_intf)
#define PORT_ENABLE_DELAY(_pc)	(((phy84328_dev_desc_t *)((_pc) + 1))->port_enable_delay)

/* Polarity config */
#define POL_CONFIG_ALL_LANES			1
#define POL_CONFIG_LANE_WIDTH			0x4
#define POL_CONFIG_LANE_MASK(_ln)		(0xf << ((_ln) * POL_CONFIG_LANE_WIDTH))
/*
 * When a port is being flexed between 40G->10G, 
 * the polarity config syntax can be in
 * 40G syntax for first lane, so there is a need to parse 
 * both 10G and 40G polarity config syntax in 10G.
 */
#define PHY84328_LN(_pc)         ((_pc)->phy_id & 0x3)
#define PHY84328_POL_MASK(_pc)   POL_CONFIG_LANE_MASK(PHY84328_LN(_pc))
#define PHY84328_10G_POL_CFG_GET(_pc, _pol) \
(((_pol) == POL_CONFIG_ALL_LANES) ||  \
(((_pol) & PHY84328_POL_MASK(_pc)) ==  PHY84328_POL_MASK(_pc)))


typedef enum {
	PHY84328_INTF_SIDE_LINE,
	PHY84328_INTF_SIDE_SYS
} phy84328_intf_side_t;


STATIC int _phy_84328_chip_id_get(int unit, soc_port_t port, phy_ctrl_t *pc, uint32 *chip_id);
/* interface primitives */
STATIC int _phy_84328_intf_is_single_port(soc_port_if_t intf_type);
STATIC int _phy_84328_intf_is_quad_port(soc_port_if_t intf_type);
STATIC int _phy_84328_intf_type_reg_get(int unit, soc_port_t port, soc_port_if_t intf_type,
										 phy84328_intf_side_t side, uint16 *reg_data, uint16 *reg_mask);
STATIC int _phy_84328_intf_update(int unit, soc_port_t port, uint16 reg_data, uint16 reg_mask);
STATIC int _phy_84328_intf_line_sys_params_get(int unit, soc_port_t port);
STATIC int _phy_84328_intf_line_sys_update(int unit, soc_port_t port);
STATIC int _phy_84328_intf_line_sys_init(int unit, soc_port_t port);
STATIC int _phy_84328_intf_datapath_update(int unit,  soc_port_t port);
STATIC int _phy_84328_intf_speed_reg_get(int unit, soc_port_t port, int speed,
										 uint16 *reg_data, uint16 *reg_mask);

STATIC int _phy_84328_intf_link_get(int unit, soc_port_t port, uint16 *link);
STATIC int _phy_84328_intf_print(int unit, soc_port_t port, const char *msg);
STATIC void _phy_84328_intf_side_regs_select(int unit, soc_port_t port, phy84328_intf_side_t side);
STATIC phy84328_intf_side_t _phy_84328_intf_side_regs_get(int unit, soc_port_t);
STATIC int _phy_84328_intf_datapath_reg_get(int unit, soc_port_t port, phy84328_datapath_t datapath, 
											uint16 *reg_data, uint16 *reg_mask);
STATIC int _phy_84328_intf_line_forced(int unit, soc_port_t port, soc_port_if_t intf_type);
STATIC int _phy_84328_config_devid(int unit,soc_port_t port, phy_ctrl_t *pc, uint32 *devid);
STATIC int _phy_84328_speed_set(int unit, soc_port_t port, int speed);
STATIC int _phy_84328_polarity_flip(int unit, soc_port_t port, uint16 cfg_tx_pol, uint16 cfg_rx_pol);
STATIC int _phy_84328_intf_type_set(int unit, soc_port_t port, soc_port_if_t pif, int must_update);
STATIC int _phy_84328_txmode_manual_set(int unit, soc_port_t port, phy84328_intf_side_t side, int set);
STATIC int _phy_84328_tx_config(int unit, soc_port_t port);
STATIC int _phy_84328_tx_enable(int unit, soc_port_t port, phy84328_intf_side_t side, int enable);
STATIC int _phy_84328_sw_rx_los_pause(int unit, soc_port_t, int enable);
STATIC int _phy_84328_link_mon_pause(int unit, soc_port_t, int enable);
STATIC int _phy_84328_control_tx_driver_set(int unit, soc_port_t port, soc_phy_control_t type,
                                            phy84328_intf_side_t side, uint32 value);
STATIC int phy_84328_control_port_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value);
STATIC int phy_84328_control_port_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value);

/* Debug support for selective tracing mdio by this driver */
/* #define PHY84328_MDIO_TRACING 1 */
#ifdef PHY84328_MDIO_TRACING
STATIC int _phy_84328_dbg_read(int unit, phy_ctrl_t *pc, uint32 addr, uint16 *data);
STATIC int _phy_84328_dbg_write(int unit, phy_ctrl_t *pc, uint32 addr, uint16 data);
STATIC int _phy_84328_dbg_modify(int unit, phy_ctrl_t *pc, uint32 addr, uint16 data, uint16 mask);

#undef  READ_PHY_REG
#define READ_PHY_REG(_unit, _pc,  _addr, _value) \
            ((_phy_84328_dbg_read)((_pc->unit), _pc, (_addr), (_value)))
#undef  WRITE_PHY_REG
#define WRITE_PHY_REG(_unit, _pc, _addr, _value) \
            ((_phy_84328_dbg_write)((_pc->unit), _pc, (_addr), (_value)))
#undef  MODIFY_PHY_REG
#define MODIFY_PHY_REG(_unit, _pc, _addr, _data, _mask) \
			(_phy_84328_dbg_modify((_unit), (_pc), (_addr), (_data), (_mask)))
#endif /* PHY84328_MDIO_TRACING */

extern unsigned char phy84328_ucode_bin[];
extern unsigned int phy84328_ucode_bin_len;
extern unsigned char phy84328B0_ucode_bin[];
extern unsigned int phy84328B0_ucode_bin_len;
static char *dev_name_84328_a0 = "BCM84328_A0";
static char *dev_name_84328 = "BCM84328";
static char *dev_name_84324 = "BCM84324";
static char *dev_name_84088 = "BCM84088";
static char *dev_name_84024 = "BCM84024";

static const char *phy84328_intf_names[] = SOC_PORT_IF_NAMES_INITIALIZER;

/* callback to deliever the firmware by application 
 * return value: TRUE  successfully delivered
 *               FALSE fail to deliver
 *
 * Example:
 *
 * int user_firmware_send (int unit,int port,int flag,unsigned char **data,int *len)
 * {
 *     *data = phy84328_ucode_bin;
 *     *len  = phy84328_ucode_bin_len;
 *     return TRUE;
 * }
 * Then initialize the function pointer before the BCM port initialization: 
 * phy_84328_uc_firmware_hook = user_firmware_send;
 */
int (*phy_84328_uc_firmware_hook)(
       int unit, /* switch number */
       int port, /* port number */
       int flag, /* any information need to ba passed. Not used for now*/
       unsigned char **addr,  /* starting address of delivered firmware */
       int *len               /* length of the firmware */
       ) = NULL;          

STATIC int
_phy_84328_chip_id_get(int unit, soc_port_t port, phy_ctrl_t *pc, uint32 *chip_id)
{
	int rv = SOC_E_NONE;

    uint16 chip_id_lsb = 0, chip_id_msb = 0;

    *chip_id = 0;

    SOC_IF_ERROR_RETURN(
            READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_CHIP_ID0_REGISTER, &chip_id_lsb));
    SOC_IF_ERROR_RETURN(
            READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_CHIP_ID1_REGISTER, &chip_id_msb));

    if (chip_id_msb == 0x8) {
        if (chip_id_lsb == 0x4328) {
            *chip_id = PHY84328_ID_84328;
        } else if (chip_id_lsb == 0x4324) {
            *chip_id = PHY84328_ID_84324;
        } else if (chip_id_lsb == 0x4088) {
            *chip_id = PHY84328_ID_84088;
        } else if(chip_id_lsb == 0x4024) {
            *chip_id = PHY84328_ID_84024;
        } else {
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "PHY84328  bad chip id: u=%d p=%d chipid %x%x\n"),
                                              unit, port, chip_id_msb, chip_id_lsb));
			rv = SOC_E_BADID;
		}
    }  
    return rv;
}

STATIC void
_phy_84328_intf_side_regs_select(int unit, soc_port_t port, phy84328_intf_side_t side)
{
	int rv;
	uint16 data, mask;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	assert((side == PHY84328_INTF_SIDE_SYS) || (side == PHY84328_INTF_SIDE_LINE));
	data = (side == PHY84328_INTF_SIDE_SYS) ? (1 << PHY84328_DEV1_XPMD_REGS_SEL_SELECT_SYS_REGISTERS_SHIFT) : 0;
	mask = PHY84328_DEV1_XPMD_REGS_SEL_SELECT_SYS_REGISTERS_MASK;
 
    /* configure all  ports  */
	    rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_XPMD_REGS_SEL, data, mask);
	    if (SOC_FAILURE(rv)) {
		    phy84328_counters_t *counters = &(COUNTERS(pc));
		    counters->side_sel_err++;
	    }
}

STATIC phy84328_intf_side_t
_phy_84328_intf_side_regs_get(int unit, soc_port_t port)
{
	int rv;
	uint16 data;
	phy84328_intf_side_t side = PHY84328_INTF_SIDE_LINE;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_XPMD_REGS_SEL, &data);
	if (rv == SOC_E_NONE) {
		side = ((data & PHY84328_DEV1_XPMD_REGS_SEL_SELECT_SYS_REGISTERS_MASK) ==
				(1 << PHY84328_DEV1_XPMD_REGS_SEL_SELECT_SYS_REGISTERS_SHIFT)) ?
			PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE;
	}
	return side;
}

STATIC int
_phy_84328_intf_print(int unit, soc_port_t port, const char *msg)
{
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    phy_ctrl_t  *pc;
	phy84328_intf_cfg_t *line_intf, *sys_intf;

    pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));
	sys_intf = &(SYS_INTF(pc));

	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "%s: "), msg));
	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "[LINE:intf=%s,speed=%d], "), 
                             phy84328_intf_names[line_intf->type], line_intf->speed));
	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "[SYS :intf=%s,speed=%d]\n"), 
                             phy84328_intf_names[sys_intf->type], sys_intf->speed));
#endif /* BROADCOM_DEBUG || DEBUG_PRINT */
	
	return SOC_E_NONE;
}

STATIC int
_phy_84328_intf_is_single_port(soc_port_if_t intf_type)
{
	int rv;

	switch (intf_type) {
	case SOC_PORT_IF_CR4:
	case SOC_PORT_IF_KR4:
	case SOC_PORT_IF_XLAUI:
    case SOC_PORT_IF_LR4:
    case SOC_PORT_IF_SR4:
    case SOC_PORT_IF_CAUI:
/*****CR10 TBD ****
    case SOC_PORT_IF_CR10:
 *****CR10 TBD ****/
		rv = TRUE;
		break;
	default:
		rv = FALSE;
	}
	return rv;
}

STATIC int
_phy_84328_intf_is_quad_port(soc_port_if_t intf_type)
{
	int rv;

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

STATIC int
_phy_84328_intf_type_10000(soc_port_if_t intf_type)
{
	int rv;

	switch (intf_type) {
	case SOC_PORT_IF_SR:
	case SOC_PORT_IF_CR:
	case SOC_PORT_IF_KR:
	case SOC_PORT_IF_ZR:
	case SOC_PORT_IF_XFI:
	case SOC_PORT_IF_SFI:
    case SOC_PORT_IF_LR:
		rv = TRUE;
		break;
	default:
		rv = FALSE;
	}
	return rv;
}
STATIC int
_phy_84328_intf_type_1000(soc_port_if_t intf_type)
{
	int rv;

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
 * Return the settings in gp_register_1 for the line inteface type 
 *
 *    SOC_PORT_IF       SPEED      GP_REGISTER_1           
 *    ------------   -----------   -------------------------------------------
 *      XFI            10G(1G)     OPTICAL
 *      SR             10G(1G)     OPTICAL
 *      LR             10G(1G)     OPTICAL+LR
 *      CR             10G(1G)     OPTICAL+CU
 *      XLAUI          10G(1G)     OPTICAL
 *      SR4            40G         OPTICAL
 *      LR4            40G(1G)     OPTICAL+LR
 *      CR4            40G(1G)     OPTICAL+CU
 *      KX             1G          BACKPLANE
 *      KR             10G         BACKPLANE
 *      KR4            40G         BACKPLANE
 *      KR2            20G         BACKPLANE
 *      XFI2           20G(1G)     OPTICAL
 *      SR2            20G(1G)     OPTICAL
 *      CR2            20G         OPTICAL+CU
 */

STATIC int
_phy_84328_intf_type_reg_get(int unit, soc_port_t port, soc_port_if_t intf_type,
							 phy84328_intf_side_t side, uint16 *reg_data, uint16 *reg_mask)
{
	uint16 data = 0;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	assert((side == PHY84328_INTF_SIDE_LINE) || (side == PHY84328_INTF_SIDE_SYS));
	*reg_data = 0;
	*reg_mask = 0;

	switch (intf_type) {
    case SOC_PORT_IF_KX:
    case SOC_PORT_IF_KR:
    case SOC_PORT_IF_KR4:
		if (side == PHY84328_INTF_SIDE_SYS) {
			data = SYS_FORCED_CL72(pc) ? (1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_SHIFT) :
				(1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_TYPE_SHIFT);
		} else {
			/* line side */
			data = (1 << PHY84328_DEV1_GP_REGISTER_1_LINE_TYPE_SHIFT);
		}
        break;
    case SOC_PORT_IF_SR:
    case SOC_PORT_IF_SR4:
    case SOC_PORT_IF_GMII:
    case SOC_PORT_IF_SGMII:
        data = 0;
        break;
    case SOC_PORT_IF_ZR:
        data = (1 << PHY84328_DEV1_GP_REGISTER_1_LINE_LR_MODE_SHIFT) |
            (1 << PHY84328_DEV1_GP_REGISTER_1_LINE_FORCED_CL72_MODE_SHIFT);
        break;
    case SOC_PORT_IF_CR4:
		/* 
		 * If CR4 line side and autoneg enabled, then CR4, otherwise, DAC/XLAUI_DFE
		 */
		if (side == PHY84328_INTF_SIDE_LINE) {
			/* 
			 * If CR4 line side and autoneg enabled, then CR4, otherwise, DAC/XLAUI_DFE
			 */
			data = (AN_EN(pc)) ? (1 << PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_SHIFT) :
				((1 << PHY84328_DEV1_GP_REGISTER_1_LINE_FORCED_CL72_MODE_SHIFT) |
				 (1 << PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_SHIFT));
		} else {
			/* System side set to XLAUI_DFE */
			data = (1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_SHIFT) |
				(1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_CU_TYPE_SHIFT);
		}
        break;
    case SOC_PORT_IF_CR:
        data = (side == PHY84328_INTF_SIDE_SYS) ?
			((1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_SHIFT) |
			 (1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_CU_TYPE_SHIFT)) :
			(1 << PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_SHIFT);
        break;

    case SOC_PORT_IF_XFI:
    case SOC_PORT_IF_SFI:
    case SOC_PORT_IF_XLAUI:
    case SOC_PORT_IF_CAUI:
        data = (side == PHY84328_INTF_SIDE_SYS) ?
            ((1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_LR_MODE_SHIFT) |
             (1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_CU_TYPE_SHIFT)) :
            ((1 << PHY84328_DEV1_GP_REGISTER_1_LINE_LR_MODE_SHIFT) |
             (1 << PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_SHIFT));
        break;
    case SOC_PORT_IF_LR:
    case SOC_PORT_IF_LR4:
        data = (side == PHY84328_INTF_SIDE_SYS) ?
            (1 << PHY84328_DEV1_GP_REGISTER_1_SYSTEM_LR_MODE_SHIFT) :
            (1 << PHY84328_DEV1_GP_REGISTER_1_LINE_LR_MODE_SHIFT);
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "84328 Unsupported interface: u=%d p=%d\n"), unit, port));
        return SOC_E_UNAVAIL;
	};

    *reg_data |= (data | PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE);

    if (side == PHY84328_INTF_SIDE_LINE) {
        *reg_mask |= (PHY84328_DEV1_GP_REGISTER_1_LINE_LR_MODE_MASK |
                      PHY84328_DEV1_GP_REGISTER_1_LINE_FORCED_CL72_MODE_MASK |
                      PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_MASK |
                      PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK |
                      PHY84328_DEV1_GP_REGISTER_1_LINE_TYPE_MASK);

    } else {
        /* System side mask */
        *reg_mask |=  (PHY84328_DEV1_GP_REGISTER_1_SYSTEM_LR_MODE_MASK |
                       PHY84328_DEV1_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_MASK |
                       PHY84328_DEV1_GP_REGISTER_1_SYSTEM_CU_TYPE_MASK |
                       PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK |
                       PHY84328_DEV1_GP_REGISTER_1_SYSTEM_TYPE_MASK);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "84328 intf type register: u=%d, p=%d, reg=%04x/%04x, intf=%d\n"),
                         unit, port, *reg_data, *reg_mask, intf_type));

	return SOC_E_NONE;
}


STATIC int
_phy_84328_polarity_redo(int unit, soc_port_t port)
{
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	return _phy_84328_polarity_flip(unit, port, POL_TX_CFG(pc), POL_RX_CFG(pc));
}

STATIC void
_phy_84328_micro_pause(int unit, soc_port_t port, const char *loc)
{
	int rv = SOC_E_NONE;
    uint16 data, mask;
	int checks;
	uint16 csr;
	uint16 saved_side;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	phy84328_micro_ctrl_t *micro_ctrl = &(MICRO_CTRL(pc));
	phy84328_counters_t *counters = &(COUNTERS(pc));

	if (!micro_ctrl->enable) {
		return;
	}

	/* Access line side registers */
	saved_side = _phy_84328_intf_side_regs_get(unit, port);
	if (saved_side == PHY84328_INTF_SIDE_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	}

	micro_ctrl->count++;
    /* Quiet micro */
    data = 0;
	mask = 0xff00;
    rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, data, mask);
	if (rv != SOC_E_NONE) {
		goto fail;
	}
    sal_udelay(500);
	checks = 0;
	while (checks < 1000) {
		/* Make sure micro really paused */
		rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_2, &data);
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
		rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, &csr);
		if (rv != SOC_E_NONE) {
			goto fail;
		}
		LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "84328 microcode did not pause in %s: "
                                     "u=%d p%d 1.ca18/1.ca19=%04x/%04x checks=%d\n"), 
                          loc, unit, port, csr, data, checks));
        counters->micro_nopause++;
	}

fail:
	if (saved_side != PHY84328_INTF_SIDE_LINE) {
        _phy_84328_intf_side_regs_select(unit, port, saved_side);
	}
	return;
}

STATIC void
_phy_84328_micro_resume(int unit, soc_port_t port)
{
	phy84328_intf_side_t saved_side;
	int rv = SOC_E_NONE;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	phy84328_micro_ctrl_t *micro_ctrl = &(MICRO_CTRL(pc));

	if (!micro_ctrl->enable) {
		return;
	}

	/* Access line side registers */
	saved_side = _phy_84328_intf_side_regs_get(unit, port);
	if (saved_side == PHY84328_INTF_SIDE_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	}

	micro_ctrl->count--;
	if (micro_ctrl->count <= 0) {
		rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, 
											 0xff00, 0xff00);
		if (rv != SOC_E_NONE) {
			goto fail;
		}
		if (micro_ctrl->count < 0) {
			LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(unit,
                                                "84328 unmatched micro resume\n")));
			micro_ctrl->count = 0;;
		}
	}

fail:
	if (saved_side != PHY84328_INTF_SIDE_LINE) {
        _phy_84328_intf_side_regs_select(unit, port, saved_side);
	}
	return;
}

STATIC int
_phy_84328_config_update(int unit, soc_port_t port)
{
    int rv;
    phy84328_datapath_t save_dp;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	PHY_84328_MICRO_PAUSE(unit, port, "config update");

	SOC_IF_ERROR_RETURN(_phy_84328_tx_config(unit, port));

    save_dp = CUR_DATAPATH(pc);
    CUR_DATAPATH(pc) = PHY84328_DATAPATH_20;
    rv = _phy_84328_polarity_redo(unit, port);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "84328  failed updating config: u=%d port=%d\n"), unit, port));
    }

	if (save_dp !=  PHY84328_DATAPATH_20) {
		CUR_DATAPATH(pc) = PHY84328_DATAPATH_4_DEPTH1;
		rv = _phy_84328_polarity_redo(unit, port);
		if (rv != SOC_E_NONE) {
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "84328  failed updating config: u=%d port=%d\n"), unit, port));
		}
	}

    if (save_dp != CUR_DATAPATH(pc)) {
        CUR_DATAPATH(pc) = save_dp;
    }

	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
	/* Bypass steady state tuning */
	if (BYPASS_SS_TUNING(pc)) {
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY84328_MMF_PMA_PMD_REG(
				unit, pc, PHY84328_DEV1_TUNING_STATE_BYPASS, 
				PHY84328_DEV1_TUNING_STATE_BYPASS_BYPASS_STEADY_STATE_TUNING_MASK,
				PHY84328_DEV1_TUNING_STATE_BYPASS_BYPASS_STEADY_STATE_TUNING_MASK));
	} else {
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY84328_MMF_PMA_PMD_REG(
				unit, pc, PHY84328_DEV1_TUNING_STATE_BYPASS, 
				0,
				PHY84328_DEV1_TUNING_STATE_BYPASS_BYPASS_STEADY_STATE_TUNING_MASK));
	}
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);

	PHY_84328_MICRO_RESUME(unit, port);

    return SOC_E_NONE;
}

/* 
 * Updates are done to GP_REGISTER_1 which are line side registers only, ie. no
 * system side instances of these registers
 */
STATIC int
_phy_84328_intf_update(int unit, soc_port_t port, uint16 reg_data, uint16 reg_mask)
{
	uint16		ucode_csr_bef = 0, ucode_csr = 0, drv_csr = 0;
	phy84328_intf_side_t saved_side;
	soc_timeout_t to;
	int			rv = SOC_E_NONE;
	uint32		checks;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	phy84328_counters_t *counters = &(COUNTERS(pc));

	/* Access line side registers */
	saved_side = _phy_84328_intf_side_regs_get(unit, port);
	if (saved_side == PHY84328_INTF_SIDE_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	}

	/* Make sure ucode has acked */
	rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_3, &ucode_csr_bef);
	if (SOC_FAILURE(rv)) {
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328 failed reading ucode csr: u=%d p=%d err=%d\n"),
                                      unit, port, rv));
						 
		goto fail;
	}
	if ((ucode_csr_bef & PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK) ==
		PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK) {
		/* Cmd active and ucode acked, so let ucode know drv saw ack */
		rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1,
											 0,  PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK);
		if (SOC_FAILURE(rv)) {
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "84328 failed clearing ack: u=%d p=%d err=%d\n"),
                                              unit, port, rv));
			goto fail;
		}
		/* Wait for ucode to CTS */
		soc_timeout_init(&to, 1000000, 0);
		while (!soc_timeout_check(&to)) {
			rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_3, &ucode_csr);
			if (SOC_FAILURE(rv)) {
				LOG_ERROR(BSL_LS_SOC_PHY,
                                          (BSL_META_U(unit,
                                                      "84328 failed reading ucode csr: u=%d p=%d err=%d\n"),
                                                      unit, port, rv));
				goto fail;
			}
			if ((ucode_csr & PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK) == 0) {
				break;
			}
		}
		if ((ucode_csr & PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK) != 0) {
			(void) READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1, &drv_csr);
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "84328 ucode failed to clear to send: u=%d p=%d 1.%04x=%04x 1.%04x=%04x(%04x)\n"), 
                                   unit, port, 
                                   PHY84328_DEV1_GP_REGISTER_1, drv_csr,
                                   PHY84328_DEV1_GP_REGISTER_3, ucode_csr, ucode_csr_bef));
			rv = SOC_E_TIMEOUT;
			goto fail;
		}
	}

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "84328 intf update register: u=%d, p=%d, 1.%04x=%04x/%04x ucode_csr=%04x\n"),
                         unit, port, PHY84328_DEV1_GP_REGISTER_1, reg_data, reg_mask, ucode_csr));

	counters->intf_updates++;

	/* Send command */
	rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1,
										 reg_data, reg_mask);
	if (SOC_FAILURE(rv)) {
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328 failed sending command to ucode: u=%d p=%d err=%d\n"),
                                      unit, port, rv));
		goto fail;
	}

	/* Handshake with microcode by waiting for ack before moving on */
	checks = 0;
	while (checks < 10000) {
		rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_3, &ucode_csr);
		if (SOC_FAILURE(rv)) {
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "84328 failed reading ucode csr: u=%d p=%d err=%d\n"),
                                              unit, port, rv));
			goto fail;
		}
		if ((ucode_csr & PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK) == 
			PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK) {
			break;
		}
		sal_udelay(100);
		checks++;
	}
	if ((ucode_csr & PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK) != 
		PHY84328_DEV1_GP_REGISTER_3_FINISH_CHANGE_MASK) {
		uint16 micro_en;
		(void) READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1, &drv_csr);
		(void) READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, &micro_en);
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328: microcode failed to ack: u=%d p=%d 1.%04x=%04x 1.%04x=%04x "
                                      "1.%04x=%04x checks=%d\n"), 
                           unit, port,
                           PHY84328_DEV1_GP_REGISTER_1, drv_csr,
                           PHY84328_DEV1_GP_REGISTER_3, ucode_csr,
                           PHY84328_DEV1_GP_REG_0, micro_en,
                           checks));
		rv = SOC_E_TIMEOUT;
		goto fail;
	}

	/* Cmd active and ucode acked - let ucode know we saw ack */
	rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1, 
										 0,  PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK);
	if (rv != SOC_E_NONE) {
		goto fail;
	}
    
fail:
	if (saved_side == PHY84328_INTF_SIDE_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
	}

	return rv;
}
		
/*
 *  Populates interface type and speed for line and system side interfaces based on 
 *  line side interface speed and system side interface type
 *
 *  Assumption:
 *     line_intf->speed: must already be set based on port configuration
 *     sys_intf->type: must be set to either the configured _spn_PHY_SYS_INTERFACE value or default
 */
STATIC int
_phy_84328_intf_line_sys_params_get(int unit, soc_port_t port)
{
	phy84328_intf_cfg_t *line_intf;
	phy84328_intf_cfg_t *sys_intf;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	line_intf = &(LINE_INTF(pc));
	sys_intf = &(SYS_INTF(pc));

	/*
	 * System side interface was specified so make sure it's compatible with
	 * line side speed. If not, overwrite with default compatible interface
	 */
    if (PHY84328_MULTI_CORE_MODE(pc)) {
        sys_intf->speed = 100000;
		line_intf->type = SOC_PORT_IF_CAUI;
        sys_intf->type = SOC_PORT_IF_CAUI;
    } else if (line_intf->speed == 40000) {
		sys_intf->speed = 40000;
		line_intf->type = SOC_PORT_IF_SR4;
		if (!(_phy_84328_intf_is_single_port(sys_intf->type))) {
			/* system side interface is not compatible so overwrite with default */
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "PHY84328 incompatible 40G system side interface, "
                                              "using default: u=%d p=%d\n"), unit, port));
			sys_intf->type = SOC_PORT_IF_XLAUI;
		}
	} else { /* 10G/1G */
		if (_phy_84328_intf_is_quad_port(sys_intf->type)) {
			if (sys_intf->type == SOC_PORT_IF_KX) {
				line_intf->speed = 1000;
				line_intf->type = SOC_PORT_IF_GMII;
				sys_intf->speed = 1000;
			} else {
				/* 10G system side so fix line side to be 10G compatible */
				line_intf->speed = 10000;
				line_intf->type = SOC_PORT_IF_SR;
				sys_intf->speed = 10000;
			}
		} else {
			/* system side interface is not compatible so overwrite with default */
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "PHY84328 incompatible 10G/1G system side interface, "
                                              "using default: u=%d p=%d\n"), unit, port));
			/* default to 10G interface/speed on both sides */
			line_intf->speed = 10000;
			line_intf->type = SOC_PORT_IF_SR;
			sys_intf->speed = 10000;
			sys_intf->type =  SOC_PORT_IF_XFI;
		}
	}
	return SOC_E_NONE;
}

STATIC int
_phy_84328_intf_line_sys_update(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
	phy_ctrl_t *int_pc;
    int         int_phy_en = 0;
	uint16		data = 0, mask = 0;
	uint16		reg_data = 0, reg_mask = 0;
	phy84328_intf_cfg_t *line_intf, *sys_intf;

    pc = EXT_PHY_SW_STATE(unit, port);
	int_pc = INT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));
	sys_intf = &(SYS_INTF(pc));
	
	_phy_84328_intf_print(unit, port, "intf update");

	reg_data = 0;
	reg_mask = 0;
	SOC_IF_ERROR_RETURN(
		_phy_84328_intf_type_reg_get(unit, port, line_intf->type, 
									 PHY84328_INTF_SIDE_LINE, &data, &mask));
	reg_data |= data;
	reg_mask |= mask;

	SOC_IF_ERROR_RETURN(
		_phy_84328_intf_type_reg_get(unit, port, sys_intf->type, 
									 PHY84328_INTF_SIDE_SYS, &data, &mask));
	reg_data |= data;
	reg_mask |= mask;

	SOC_IF_ERROR_RETURN(
		_phy_84328_intf_speed_reg_get(unit, port, line_intf->speed, &data, &mask));
	reg_data |= data;	
	reg_mask |= mask;

	SOC_IF_ERROR_RETURN(
		_phy_84328_intf_datapath_reg_get(unit, port, CUR_DATAPATH(pc), &data, &mask));
	reg_data |= data;	
	reg_mask |= mask;

	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "84328  intf update: line=%s sys=%s speed=%d (1.%x = %04x/%04x): u=%d p=%d\n"), 
                  phy84328_intf_names[line_intf->type], phy84328_intf_names[sys_intf->type], 
                  line_intf->speed, PHY84328_DEV1_GP_REGISTER_1, reg_data, reg_mask,
                  unit, port));

	if (SYNC_INIT(pc) == 1) {
        SOC_IF_ERROR_RETURN(PHY_ENABLE_GET(int_pc->pd, unit, port, &int_phy_en));
        if (int_phy_en) {
            /* Turn off internal PHY while the mode changes */
            SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, 0));
        }
	}
	
	/* GP registers are only on the line side */
	SOC_IF_ERROR_RETURN(_phy_84328_intf_update(unit, port, reg_data, reg_mask));
	
    if (SYNC_INIT(pc) == 1) {
        if (int_phy_en) {
            /* If called from PASS2 we skip this and complete in PASS3 */
            if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2)) {
                INT_PHY_RE_EN(pc) = 1;
                /* start timer */
                soc_timeout_init(&SYNC_TO(pc), 10000, 0);
            } else {
                sal_usleep(10000);
                /* Turn internal PHY back on now that the mode has been configured */
                SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, 1));
            }
        }
    }

	return SOC_E_NONE;

}

/* 
 * Return the settings in gp_reg_1/gp_register_1 for the inteface speed 
 * Register data and mask are returned to modify the register. 
 */

STATIC int
_phy_84328_intf_speed_reg_get(int unit, soc_port_t port, int speed, uint16 *reg_data, uint16 *reg_mask)
{
	*reg_data = 0;
	*reg_mask = 0;
	switch (speed) {
	case 100000:
		*reg_data = PHY84328_DEV1_GP_REGISTER_1_SPEED_100G;
		break;
	case 42000:
		*reg_data = PHY84328_DEV1_GP_REGISTER_1_SPEED_42G;
		break;
	case 40000:
		*reg_data = PHY84328_DEV1_GP_REGISTER_1_SPEED_40G;
		break;
	case 10000:
		*reg_data = PHY84328_DEV1_GP_REGISTER_1_SPEED_10G;
		break;
	case 1000:
    case 100:
    case 10:
		/* This is a special mode that uses the 10G PLL but runs at 1G */
		*reg_data = (PHY84328_DEV1_GP_REGISTER_1_SPEED_10G |
					 PHY84328_DEV1_GP_REGISTER_1_SPEED_1G);
		break;

	default :
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328 invalid line speed %d: u=%d p=%d\n"),
                                      speed, unit, port));
		return SOC_E_CONFIG;
	}
			
	*reg_data |= PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE;
	*reg_mask |= (PHY84328_DEV1_GP_REGISTER_1_SPEED_1G_MASK |
				  PHY84328_DEV1_GP_REGISTER_1_SPEED_10G_MASK |
				  PHY84328_DEV1_GP_REGISTER_1_SPEED_40G_MASK |
				  PHY84328_DEV1_GP_REGISTER_1_SPEED_100G_MASK |
				  PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK);

	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "84328 speed set register: u=%d, p=%d, reg=%04x/%04x), speed=%d\n"),
                             unit, port, *reg_data, *reg_mask, speed));

	return SOC_E_NONE;
}

STATIC int
_phy_84328_intf_datapath_reg_get(int unit, soc_port_t port, phy84328_datapath_t datapath, 
								 uint16 *reg_data, uint16 *reg_mask)
{
	*reg_data = 0;
	*reg_mask = 0;

	switch (datapath) {
	case PHY84328_DATAPATH_20:
		*reg_data = 0;
		break;
	case PHY84328_DATAPATH_4_DEPTH1:
		*reg_data = PHY84328_DEV1_GP_REGISTER_1_DATAPATH_4_DEPTH1;
		break;
	case PHY84328_DATAPATH_4_DEPTH2:
		*reg_data = PHY84328_DEV1_GP_REGISTER_1_DATAPATH_4_DEPTH2;
		break;

	default :
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328 invalid datapath: u=%d p=%d datapath=%d\n"),
                                      unit, port, datapath));
		return SOC_E_CONFIG;
	}
	*reg_data |= PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE;	
	*reg_mask = (PHY84328_DEV1_GP_REGISTER_1_DATAPATH_MASK |
		PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK);

	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "84328 datapath set register: u=%d, p=%d, reg=%04x/%04x, datapath=%d\n"),
                             unit, port, *reg_data, *reg_mask, datapath));

	return SOC_E_NONE;
}

STATIC int
_phy_84328_intf_datapath_update(int unit,  soc_port_t port)
{
	phy_ctrl_t  *pc;
	uint16      data = 0, mask = 0;

	pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(
        _phy_84328_intf_datapath_reg_get(unit, port, CUR_DATAPATH(pc), &data, &mask));

	/* GP registers are only on the line side */
	SOC_IF_ERROR_RETURN(_phy_84328_intf_update(unit, port, data, mask));

	return SOC_E_NONE;
}

/*
 * Select channel with mapped access based on interface side and lane number
 * For details, see PHY-700
 */
STATIC int
_phy_84328_channel_mapped_select(int unit, soc_port_t port, phy84328_intf_side_t side, int lane)
{
    uint16 lane_sel;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    /* logical channel number for physical channel, indexed by lane/channel */
    static uint16 log_ch_map[] = { 0xe4, 0xe1, 0xc6, 0x27 }; 

    if (lane == PHY84328_ALL_LANES) {
        /* Restore 1:1 channel mappings */
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_LOGAD_CTRL, 0xe4, 0x00ff));
        lane_sel = 
            (LOGICAL_LANE0(pc) << PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_SHIFT) |
            PHY84328_DEV1_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK |
            PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_MASK;
    } else {
        /* System side interface always use first channel which then is mapped */
        if (side == PHY84328_INTF_SIDE_SYS) {
            lane_sel = 1;
        } else {
            /* Select channel in Single PMD 1.ca86[5:4] and [3..0] that corresponds to lane */
            lane_sel = (lane << PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_SHIFT) |
                ((1 << lane) & PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_MASK);
        }
    }

    SOC_IF_ERROR_RETURN(
        MODIFY_PHY84328_MMF_PMA_PMD_REG(
            unit, pc, PHY84328_DEV1_SINGLE_PMD_CTRL, lane_sel,
            PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_MASK |
            PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_MASK));
    
    if (side == PHY84328_INTF_SIDE_SYS) {
        /* Map the logical channel from physical channel */
        lane_sel = (lane == PHY84328_ALL_LANES) ? log_ch_map[0] :  log_ch_map[lane];
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY84328_MMF_PMA_PMD_REG(
                unit, pc, PHY84328_DEV1_LOGAD_CTRL, lane_sel, 0x00ff));

        /* Now go to system side to access register */
        _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
    }

    return SOC_E_NONE;
}

/*
 * Directly select channel to access based on interface side and lane number.
 */
STATIC int
_phy_84328_channel_direct_select(int unit, soc_port_t port, phy84328_intf_side_t side, int lane)
{
    uint16      lane_sel;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * If all lanes, restore single PMD access with lane 0 broadcast to all lanes
     * For single lane, select lane in Single PMD 1.ca86[5:4] and [3..0] that corresponds
     * to the lane with direct lane selection in 1.ca86[8]
     */
    if (lane == PHY84328_ALL_LANES) {
        lane_sel = 
            (LOGICAL_LANE0(pc) << PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_SHIFT) |
            PHY84328_DEV1_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK |
            PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_MASK;
    } else {
        lane_sel = 0x100 | (lane << PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_SHIFT) |
            ((1 << lane) & PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_MASK);
    }

    SOC_IF_ERROR_RETURN(
        MODIFY_PHY84328_MMF_PMA_PMD_REG(
            unit, pc, PHY84328_DEV1_SINGLE_PMD_CTRL, lane_sel,
            0x100 | PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_MASK |
            PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_BE_DISABLE_IN_SINGLE_PMD_MASK));

    if (side == PHY84328_INTF_SIDE_SYS) {
        /* Now go to system side to access register */
        _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
    }
    return SOC_E_NONE;
}

/*
 * Select channel to access based on interface side and lane number.
 * The side is selected before returning.
 */
STATIC int
_phy_84328_channel_select(int unit, soc_port_t port, phy84328_intf_side_t side, int lane)
{
    int rv;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if (!(PHY84328_SINGLE_PORT_MODE(pc))) {
        return SOC_E_PARAM;
    }

    if (!((lane == PHY84328_ALL_LANES) || ((lane >= 0) && (lane <= 3)))) {
        return SOC_E_PARAM;
    }

    _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);

    if ((side == PHY84328_INTF_SIDE_LINE) || (DEVREV(pc) != 0x00a0)) {
        rv  = _phy_84328_channel_direct_select(unit, port, side, lane);
    } else {
        rv  = _phy_84328_channel_mapped_select(unit, port, side, lane);
    }

    return rv;
}

/*
 *   20-bit: set txpol in line side 1.c061[5]
 *   4-bit:  set txpol in line side 1.c068[12]
 */
STATIC int
_phy_84328_polarity_flip_tx_set(int unit, soc_port_t port, int flip)
{	
	uint16		data, mask;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	/* Clear to flip, set to unflip */
	if (CUR_DATAPATH(pc) == PHY84328_DATAPATH_20) {
		data = flip ? 0 : PHY84328_DEV1_ANATXACONTROL_TXPOL_FLIP_MASK;
		mask = PHY84328_DEV1_ANATXACONTROL_TXPOL_FLIP_MASK;
        SOC_IF_ERROR_RETURN(
			MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL, data, mask));
	} else {
		data = flip ? 0 : PHY84328_DEV1_ANATXACONTROL4_XFI_LOWLATENCY_POLARITY_FLIP_MASK;
		mask = PHY84328_DEV1_ANATXACONTROL4_XFI_LOWLATENCY_POLARITY_FLIP_MASK;
         SOC_IF_ERROR_RETURN(
			MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL4, data, mask));
	}
	return SOC_E_NONE;
}

STATIC int
_phy_84328_polarity_flip_tx(int unit, soc_port_t port, uint16 cfg_pol)
{
	int			flip;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if ((PHY84328_SINGLE_PORT_MODE(pc))) {
		int	lane;
        for (lane = 0; lane < PHY84328_NUM_LANES; lane++) {
			/* Check for all lanes or whether this lane has polarity flipped */
			if ((cfg_pol == POL_CONFIG_ALL_LANES) || 
				((cfg_pol & POL_CONFIG_LANE_MASK(lane)) == POL_CONFIG_LANE_MASK(lane))) {
				flip = 1;
			} else {
				flip = 0;
			}
			LOG_INFO(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "84328 tx polarity flip=%d: u=%d p=%d lane=%d\n"), flip, unit, port, lane));

			/* Select the lane on the line side */
			SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE, lane));
			SOC_IF_ERROR_RETURN(_phy_84328_polarity_flip_tx_set(unit, port, flip));
		}
		/* Restore to default single port register access */
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE, PHY84328_ALL_LANES));
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
    }
    else {  /* quad mode */
		flip = PHY84328_10G_POL_CFG_GET(pc,cfg_pol);
		SOC_IF_ERROR_RETURN(_phy_84328_polarity_flip_tx_set(unit, port, flip));
		LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "84328  polarity flip: u=%d p=%d\n"), unit, port));
	}

	return SOC_E_NONE;
}


/* RX polarity in 20-bit line side with rx inversion in 1.c0ba[3:2] */
STATIC int
_phy_84328_polarity_flip_rx_20bit(int unit, soc_port_t port, int flip)
{
	uint16		data, mask;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
	
	/* Default is clear, set to flip it */
	data = flip ? (PHY84328_DEV1_GP_REGISTER_RX_INV_FORCE | PHY84328_DEV1_GP_REGISTER_RX_INV_INVERT) : 0;
	mask = (PHY84328_DEV1_GP_REGISTER_RX_INV_FORCE | PHY84328_DEV1_GP_REGISTER_RX_INV_INVERT);
 
    SOC_IF_ERROR_RETURN(
            MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_RX_INV, data, mask));

    return SOC_E_NONE;
}

/* RX polarity in 4-bit system side polarity in 1.c068[12] */
STATIC int
_phy_84328_polarity_flip_rx_4bit(int unit, soc_port_t port, int flip)
{
	uint16		data, mask;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    /* Default is set, clear to flip it */
	data = flip ? 0 : PHY84328_DEV1_ANATXACONTROL4_XFI_LOWLATENCY_POLARITY_FLIP_MASK;
	mask = PHY84328_DEV1_ANATXACONTROL4_XFI_LOWLATENCY_POLARITY_FLIP_MASK;

    SOC_IF_ERROR_RETURN(
            MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL4, data, mask));

    return SOC_E_NONE;
}
			
STATIC int
_phy_84328_polarity_flip_rx(int unit, soc_port_t port, uint16 cfg_pol)
{
	int			flip;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	/*
	 * RX polarity flipping in 20-bit done on the line side while 4-bit done on the system side
	 */
    if ((PHY84328_SINGLE_PORT_MODE(pc))) {
		int	lane;
		
		/* 4-bit polarity handled on system side */
		if (CUR_DATAPATH(pc) != PHY84328_DATAPATH_20) {
			PHY_84328_MICRO_PAUSE(unit, port, "polarity rx");
		}
        for (lane = 0; lane < PHY84328_NUM_LANES; lane++) {
			flip = 0;
			if ((cfg_pol == POL_CONFIG_ALL_LANES) || 
				((cfg_pol & POL_CONFIG_LANE_MASK(lane)) == POL_CONFIG_LANE_MASK(lane))) {
				flip = 1;
			}
			if (CUR_DATAPATH(pc) == PHY84328_DATAPATH_20) {
				SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE, lane));
				SOC_IF_ERROR_RETURN(_phy_84328_polarity_flip_rx_20bit(unit, port, flip));
			} else {
				SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_SYS, lane));
				SOC_IF_ERROR_RETURN(_phy_84328_polarity_flip_rx_4bit(unit, port, flip));
			}
			LOG_INFO(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "84328 rx polarity flip: u=%d p=%d lane=%d\n"), unit, port, lane));
		}
		/* Restore to default single port register access */
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE, PHY84328_ALL_LANES));
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
		if (CUR_DATAPATH(pc) != PHY84328_DATAPATH_20) {
			PHY_84328_MICRO_RESUME(unit, port);
		}

    }
    else {  /* quad mode */

		flip = PHY84328_10G_POL_CFG_GET(pc,cfg_pol);
		if (CUR_DATAPATH(pc) == PHY84328_DATAPATH_20) {
			_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
			SOC_IF_ERROR_RETURN(_phy_84328_polarity_flip_rx_20bit(unit, port, flip));
		} else {
			_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
			SOC_IF_ERROR_RETURN(_phy_84328_polarity_flip_rx_4bit(unit, port, flip));
			_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
		}
		LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "84328 rx polarity flip: u=%d p=%d\n"), unit, port));
	}


	return SOC_E_NONE;
}


/*                                                                                              
 * Flip configured tx/rx polarities
 *
 * Property values are either 1 for all lanes or
 * lane 0: 000f
 * lane 1: 00f0
 * lane 2: 0f00
 * lane 3: f000
 */
STATIC int
_phy_84328_polarity_flip(int unit, soc_port_t port, uint16 cfg_tx_pol, uint16 cfg_rx_pol)
{
	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "84328 polarity flip: u=%d p%d TX=%x RX=%x\n"), unit, port, 
                             cfg_tx_pol, cfg_rx_pol));

	SOC_IF_ERROR_RETURN(_phy_84328_polarity_flip_tx(unit, port, cfg_tx_pol));

	SOC_IF_ERROR_RETURN(_phy_84328_polarity_flip_rx(unit, port, cfg_rx_pol));

    return SOC_E_NONE;
}


STATIC int
_phy_84328_config_devid(int unit,soc_port_t port, phy_ctrl_t *pc, uint32 *devid)
{
    if (soc_property_port_get(unit, port, "phy_84328", FALSE)) {
        return *devid = PHY84328_ID_84328;
    }

	return _phy_84328_chip_id_get(unit, port, pc, devid);
}

STATIC int
_phy_84328_tx_mode_set(int unit, soc_port_t port, phy84328_intf_side_t side, uint16 tx_mode)
{
    phy_ctrl_t *pc;
	phy84328_intf_side_t saved_side;

    /* Make sure tx_mode has acceptable values */
    if ((tx_mode != 0) && (tx_mode != 1) && (tx_mode != 4)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "PHY84328 invalid tx mode configuration - must be 0, 1, or 4: u%d p%d\n"),
                              unit, port));
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    saved_side = _phy_84328_intf_side_regs_get(unit, port);
	_phy_84328_intf_side_regs_select(unit, port, side);

    /* Set manual tx mode updates */
    SOC_IF_ERROR_RETURN(_phy_84328_txmode_manual_set(unit, port, side, TRUE));
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL2,
                                        tx_mode << PHY84328_DEV1_ANATXACONTROL2_TX_MODESELECT_SHIFT,
                                        PHY84328_DEV1_ANATXACONTROL2_TX_MODESELECT_MASK));
	if (saved_side != side) {
		_phy_84328_intf_side_regs_select(unit, port, saved_side);
	}

    return SOC_E_NONE;
}

STATIC int
_phy_84328_tx_mode_get(int unit, soc_port_t port, phy84328_intf_side_t side, uint16 *tx_mode)
{
    phy_ctrl_t *pc;
	uint16 data;
	phy84328_intf_side_t saved_side;

    pc = EXT_PHY_SW_STATE(unit, port);

    saved_side = _phy_84328_intf_side_regs_get(unit, port);
	_phy_84328_intf_side_regs_select(unit, port, side);

    SOC_IF_ERROR_RETURN(
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL2,
									  &data));

	/* Must exclude bit 13 from tx_mode */
	*tx_mode = (data >> PHY84328_DEV1_ANATXACONTROL2_TX_MODESELECT_SHIFT) & 0xd;

	if (saved_side != side) {
		_phy_84328_intf_side_regs_select(unit, port, saved_side);
	}

    return SOC_E_NONE;
}

STATIC int
_phy_84328_tx_config(int unit, soc_port_t port)
{
	int tx_mode = -1;
	int tx_idriver = -1;

	/* tx_modeselect: 0, 1, or 4 */
	tx_mode = soc_property_port_get(unit, port, "phy_line_tx_mode", -1);
	if (tx_mode != -1) {
		SOC_IF_ERROR_RETURN(_phy_84328_tx_mode_set(unit, port, PHY84328_INTF_SIDE_LINE,
												   (uint16) tx_mode));
	}
	tx_mode = soc_property_port_get(unit, port, "phy_system_tx_mode", -1);
	if (tx_mode != -1) {
		SOC_IF_ERROR_RETURN(_phy_84328_tx_mode_set(unit, port, PHY84328_INTF_SIDE_SYS,
												   (uint16) tx_mode));
	}

	/* tx idriver: 0..15 */
	tx_idriver = soc_property_port_get(unit, port, "phy_line_driver_current", -1);
	if (tx_idriver != -1) {
		SOC_IF_ERROR_RETURN(_phy_84328_control_tx_driver_set(unit, port, SOC_PHY_CONTROL_DRIVER_CURRENT,
															 PHY84328_INTF_SIDE_LINE, tx_idriver));
	}

	tx_idriver = soc_property_port_get(unit, port, "phy_system_driver_current", -1);
	if (tx_idriver != -1) {
		SOC_IF_ERROR_RETURN(_phy_84328_control_tx_driver_set(unit, port, SOC_PHY_CONTROL_DRIVER_CURRENT,
															 PHY84328_INTF_SIDE_SYS, tx_idriver));
	}

	return SOC_E_NONE;
}

STATIC phy84328_datapath_t
_phy_84328_datapath_get(int unit, soc_port_t port)
{
	phy84328_datapath_t dp = PHY84328_DATAPATH_20;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	switch (DEVREV(pc)) {
	case 0x00a0:
		dp = PHY84328_DATAPATH_20;
		break;
	case 0x00b0:
	default:
		dp = PHY84328_DATAPATH_4_DEPTH1;
		break;
	}
	return dp;
}

#if 1
STATIC int
_phy_84328_intf_sys_default(int unit, soc_port_t port)
{
	uint16 if_sys_idx = 0;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY84328_MULTI_CORE_MODE(pc)) {
        if_sys_idx = 0;
    } else {
        /* Get configured system interface */
        if_sys_idx = soc_property_port_get(unit, port, spn_PHY_SYS_INTERFACE, 0);
        if (if_sys_idx >= (sizeof(phy_sys_to_port_if) / sizeof(soc_port_if_t))) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                      "PHY84328 invalid system side interface: u=%d p=%d intf=%d\n"
                      "Using default interface\n"),
                      unit, port, if_sys_idx));
            if_sys_idx = 0;
        }
    }
    return if_sys_idx;
}

STATIC int
_phy_84328_intf_line_speed_default(int unit, soc_port_t port)
{
    int speed;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY84328_MULTI_CORE_MODE(pc)) {
        speed = 100000;
    } else if (PHY84328_SINGLE_PORT_MODE(pc)) {
        speed = 40000;
    } else {
        speed = 10000;
    }
    return speed;
}

STATIC soc_port_if_t
_phy_84328_intf_type_default(int speed, phy84328_intf_side_t side)
{
    soc_port_if_t intf;

    switch (speed) {
    case 100000:
        /**** SR10 on line side? */
        intf = (side == PHY84328_INTF_SIDE_LINE) ? SOC_PORT_IF_SR4 : SOC_PORT_IF_CAUI;
        break;
    case 40000:
        intf = (side == PHY84328_INTF_SIDE_LINE) ? SOC_PORT_IF_SR4 : SOC_PORT_IF_XLAUI;
        break;
    case 10000:
    default:
        intf = (side == PHY84328_INTF_SIDE_LINE) ? SOC_PORT_IF_SR : SOC_PORT_IF_XFI;
        break;
    }
    return intf;
}



STATIC int
_phy_84328_intf_default(int unit, soc_port_t port)
{
	phy84328_intf_cfg_t *line_intf, *sys_intf;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	line_intf = &(LINE_INTF(pc));
	sys_intf  = &(SYS_INTF(pc));
	sal_memset(line_intf, 0, sizeof(phy84328_intf_cfg_t));
	sal_memset(sys_intf,  0, sizeof(phy84328_intf_cfg_t));

	/*
	 * - interfaces are KR4/KR/KX/XAUI/XFI/SR/LR/CR/SR4/LR4/CR4/SR10/CR10
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
     * 100G(multicore)   dflt:CAUI                 dflt:SR10   or user set if/speed
	 * 40G(singleport)   dflt:XLAUI or configvar   dflt:XLAUI  or user set if/speed
	 * 1G/10G(quadport)  dflt:XFI or configvar     dflt:SFI    or user set if/speed
	 */

	/* Get configured system interface */
    sys_intf->type = phy_sys_to_port_if[_phy_84328_intf_sys_default(unit, port)];
	line_intf->speed = _phy_84328_intf_line_speed_default(unit, port);
	if (sys_intf->type == 0) {
		/* use defaults based on current line side */
		sys_intf->speed = line_intf->speed;
		line_intf->type = _phy_84328_intf_type_default(line_intf->speed, PHY84328_INTF_SIDE_LINE);
		sys_intf->type =  _phy_84328_intf_type_default(line_intf->speed, PHY84328_INTF_SIDE_SYS);
		_phy_84328_intf_print(unit, port, "default sys intf");
	} else {
		SOC_IF_ERROR_RETURN(_phy_84328_intf_line_sys_params_get(unit, port));
		_phy_84328_intf_print(unit, port, "after updating intf based on sys config");
	}
	CFG_SYS_INTF(pc) = sys_intf->type;

    return SOC_E_NONE;
}

/* 
 * TODO: how to handle configs for flex ports
 *    Issues: 
 *        - how is the polarity specified if the port number changes
 *        - what is the syntax to specify the lanes for 100G vs 40G vs 10G
 */
STATIC int
_phy_84328_polarity_cfg_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * The core aware property which specifies the core number must be
     * with multiple core ports
     *
     * Ex:
     *   Assuming port 4 is a 100G port, then if every other lane has tx
     *   polarity flipped:
     *        config add phy_tx_polarity_flip_core0_port4=0x0f0f
     *        config add phy_tx_polarity_flip_core1_port4=0x0f0f
     *        config add phy_tx_polarity_flip_core2_port4=0x000f
     *        
     *   Polarity configuration:
     *    1: all lanes
     *    nibble==0xF: each nibble specifies a lane - up to 4 lanes
     *
     * The polarity configuration is stored in the driver per core control block
 */
    if (PHY84328_MULTI_CORE_MODE(pc)) {
        POL_TX_CFG(pc) = soc_property_port_suffix_num_get(unit, port, CORE_NUM(pc),
                                                          spn_PHY_TX_POLARITY_FLIP,
                                                          "core", 0);
        POL_RX_CFG(pc) = soc_property_port_suffix_num_get(unit, port, CORE_NUM(pc),
                                                          spn_PHY_RX_POLARITY_FLIP,
                                                          "core", 0);
    } else {
        POL_TX_CFG(pc) = soc_property_port_get(unit, port, spn_PHY_TX_POLARITY_FLIP, 0);
        POL_RX_CFG(pc) = soc_property_port_get(unit, port, spn_PHY_RX_POLARITY_FLIP, 0);
    } 
    
    return SOC_E_NONE;
}	


#endif /* 100G */

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_phy_84328_intf_speed_from_reg(uint16 mode, int *speed)
{
    uint16 speed_reg = mode & (PHY84328_DEV1_GP_REGISTER_1_SPEED_1G_MASK |
                               PHY84328_DEV1_GP_REGISTER_1_SPEED_10G_MASK |
                               PHY84328_DEV1_GP_REGISTER_1_SPEED_40G_MASK |
                               PHY84328_DEV1_GP_REGISTER_1_SPEED_100G_MASK);
    *speed = 0;
    switch (speed_reg) {

    case PHY84328_DEV1_GP_REGISTER_1_SPEED_100G:
        *speed = 100000;
        break;

    case PHY84328_DEV1_GP_REGISTER_1_SPEED_42G:
        *speed = 42000;
        break;

    case PHY84328_DEV1_GP_REGISTER_1_SPEED_40G:
        *speed = 40000;
        break;

    case PHY84328_DEV1_GP_REGISTER_1_SPEED_10G:
        *speed = 10000;
        break;

    case PHY84328_DEV1_GP_REGISTER_1_SPEED_10G | PHY84328_DEV1_GP_REGISTER_1_SPEED_1G:
        /********* 100, 10 not recoverable from PHY ********/
        *speed = 1000;
        break;
    default:
        /* hardware has unexpected value */
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84328_intf_type_from_reg(uint16 mode, phy84328_intf_side_t side, int speed, int an,
                              soc_port_if_t *intf_type) 
{
    uint16 intf_reg = mode & ((side == PHY84328_INTF_SIDE_LINE) ? 0x5300 : 0xac00);

    if (side == PHY84328_INTF_SIDE_LINE) {
        switch (intf_reg) {
        case 0:
            switch (speed) {
            case 40000:
            case 42000:
            case 100000:
                *intf_type = SOC_PORT_IF_SR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_SR;
                break;
            case 1000:
                if (an) {
                    *intf_type = SOC_PORT_IF_GMII;
                } else {
                    *intf_type = SOC_PORT_IF_SGMII;
                }
                break;
            case 100:
            case 10:
                *intf_type = SOC_PORT_IF_SGMII;
                break;
            default:
                return SOC_E_FAIL;
            }

            break;
            
        case 0x4200:
            switch (speed) {
            case 100000:
                *intf_type = SOC_PORT_IF_CAUI;
                break;
            case 40000:
            case 42000:
                *intf_type = SOC_PORT_IF_XLAUI;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_SFI;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x4000:
            switch (speed) {
            case 40000:
            case 42000:
                *intf_type = SOC_PORT_IF_LR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_LR;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x200:
            switch (speed) {
            case 40000:
            case 42000:
            case 100000:
                *intf_type = SOC_PORT_IF_CR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_CR;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x100:
            switch (speed) {
            case 40000:
            case 42000:
                *intf_type = SOC_PORT_IF_KR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_KR;
                break;
            case 1000:
            case 100:
            case 10:
                *intf_type = SOC_PORT_IF_KX;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x1200:
            switch (speed) {
            case 40000:
            case 42000:
            case 100000:
                *intf_type = SOC_PORT_IF_CR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_CR;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x5000:
            switch (speed) {
            case 10000:
                *intf_type = SOC_PORT_IF_ZR;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x1000:
        default:
            return SOC_E_FAIL;
            break;
        } /*  switch (intf_reg) */
    } else {
        /* get sys intf type  */
        switch (intf_reg) {
        case 0:
            switch (speed) {
            case 40000:
            case 42000:
                *intf_type = SOC_PORT_IF_SR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_SR;
                break;
            case 1000:
                if (an) {
                    *intf_type = SOC_PORT_IF_GMII;
                } else {
                    *intf_type = SOC_PORT_IF_SGMII;
                }
                break;
            case 100:
            case 10:
                *intf_type = SOC_PORT_IF_SGMII;
                break;
            default:
                return SOC_E_FAIL;
            }

            break;
            
        case 0x8800:
            switch (speed) {
            case 100000:
                *intf_type = SOC_PORT_IF_CAUI;
                break;
            case 40000:
            case 42000:
                *intf_type = SOC_PORT_IF_XLAUI;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_XFI;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x800:
            switch (speed) {
            case 40000:
            case 42000:
            case 100000:
                *intf_type = SOC_PORT_IF_CR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_CR;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x400:
            switch (speed) {
            case 40000:
            case 42000:
                *intf_type = SOC_PORT_IF_KR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_KR;
                break;
            case 1000:
            case 100:
            case 10:
                *intf_type = SOC_PORT_IF_KX;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0x2800:
            switch (speed) {
            case 40000:
            case 42000:
            case 100000:
                *intf_type = SOC_PORT_IF_CR4;
                break;
            case 10000:
                *intf_type = SOC_PORT_IF_CR;
                break;
            default:
                return SOC_E_FAIL;
            }
            break;

        case 0xa000:
        case 0x2000:
        default:
            return SOC_E_FAIL;
            break;
        } /*  switch (intf_reg) */

    }
    return SOC_E_NONE;
}


STATIC int
_phy_84328_reinit_line_intf(int unit, soc_port_t port, int speed, int an, uint16 mode)
{
    phy_ctrl_t  *pc;
    phy84328_intf_cfg_t *line_intf;
    
    pc = EXT_PHY_SW_STATE(unit, port);
    line_intf = &(LINE_INTF(pc));

    line_intf->speed = speed;
    SOC_IF_ERROR_RETURN(_phy_84328_intf_type_from_reg(mode, PHY84328_INTF_SIDE_LINE, speed, an, &(line_intf->type)));

    if (AN_EN(pc) && line_intf->type  == SOC_PORT_IF_CR4) {
        if (POL_RX_CFG(pc)) {
            FORCE_20BIT(pc) |= FORCE_20BIT_AN ;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84328_reinit_system_intf(int unit, soc_port_t port, int speed, int an, uint16 mode)
{
    phy_ctrl_t  *pc;
    phy84328_intf_cfg_t *system_intf;
    
    pc = EXT_PHY_SW_STATE(unit, port);
    system_intf = &(SYS_INTF(pc));

    system_intf->speed = speed;
    SOC_IF_ERROR_RETURN(_phy_84328_intf_type_from_reg(mode, PHY84328_INTF_SIDE_SYS, speed, an, &(system_intf->type)));

    return SOC_E_NONE;
}

/*
 * For WarmBoot support: preserve SW RxLOS enable/disable status in register 1.0xC01A
 */
#define PHY84328_NO_PRESERVE_SW_RxLOS           0x100
#define PHY84328_DEV1_LANECTRL_RSV_SHIFT        12
STATIC int
_phy_84328_preserve_sw_rx_los(int unit, soc_port_t port, int enable)
{
    uint32                offset;
    uint16                sw_rx_los_enable;
    phy_ctrl_t           *pc = EXT_PHY_SW_STATE(unit, port);

    /* get the offset of this port/lane */
	SOC_IF_ERROR_RETURN(
	    phy_84328_control_port_get(unit, port, SOC_PHY_CONTROL_PORT_OFFSET, &offset) );
	offset = 1U << (offset + PHY84328_DEV1_LANECTRL_RSV_SHIFT);
    sw_rx_los_enable = (enable) ? offset : 0 ;

    /* register 1.0xC01A , use the reserved bits [15:12]                    *\
    \*                     to preserve SW RxLOS status for warmboot support */
    return  MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                    PHY84328_DEV1_LANECTRL, sw_rx_los_enable, (uint16) offset);
}

STATIC int  _phy_84328_sw_rx_los_control_set(int unit, soc_port_t port, uint32 value);
/*
 * During WarmBoot: retrieve SW RxLOS enable/disable status from register 1.0xC01A
 */
STATIC int
_phy_84328_reinit_sw_rx_los(int unit, soc_port_t port)
{
    uint32                offset;
    uint16                sw_rx_los_enable;
    phy84328_sw_rx_los_t *sw_rx_los;
    phy_ctrl_t           *pc = EXT_PHY_SW_STATE(unit, port);
    sw_rx_los = &(SW_RX_LOS(pc));

    /* get the offset of this port/lane */
	SOC_IF_ERROR_RETURN(
	    phy_84328_control_port_get(unit, port, SOC_PHY_CONTROL_PORT_OFFSET, &offset) );
	offset = 1U << (offset + PHY84328_DEV1_LANECTRL_RSV_SHIFT);

    /* register 1.0xC01A , use the reserved bits [15:12]               *\
    \*                     to preserve SW RxLOS status during warmboot */
    SOC_IF_ERROR_RETURN(
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                               PHY84328_DEV1_LANECTRL, &sw_rx_los_enable) );
    sw_rx_los->cfg_enable = (sw_rx_los_enable & offset) ? TRUE : FALSE;

    /* if SW RxLOS was enabled, hardcode the default RxLOS stable state */
    if ( sw_rx_los->cfg_enable ) {
        sw_rx_los->cur_enable       = 1;
        sw_rx_los->sys_link         = 1;
        sw_rx_los->link_no_pcs      = 0;
        sw_rx_los->link_status      = 0;
        sw_rx_los->fault_report_dis = 0;
        sw_rx_los->link_no_pcs      = 0;
        sw_rx_los->restarts         = 1;
        sw_rx_los->state            = PHY84328_SW_RX_LOS_IDLE;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84328_reinit_fw_rx_los(int unit, soc_port_t port)
{
    uint16 fw_rx_los, fw_rx_los_en_mask;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    fw_rx_los_en_mask = (1 << 15) | (1 << 14);

    SOC_IF_ERROR_RETURN(
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 0xc480, &fw_rx_los));

    FW_RX_LOS(pc) = (fw_rx_los & fw_rx_los_en_mask) == fw_rx_los_en_mask;

    return SOC_E_NONE;
}

STATIC int
_phy_84328_reinit_cur_datapath(int unit, soc_port_t port, uint16 mode)
{
    uint16 datapath_reg;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    datapath_reg = mode & PHY84328_DEV1_GP_REGISTER_1_DATAPATH_MASK;
    switch (datapath_reg) {
    case 0:
        CUR_DATAPATH(pc) = PHY84328_DATAPATH_20;
        break;
    case PHY84328_DEV1_GP_REGISTER_1_DATAPATH_4_DEPTH1:
        CUR_DATAPATH(pc) = PHY84328_DATAPATH_4_DEPTH1;
        break;
    case PHY84328_DEV1_GP_REGISTER_1_DATAPATH_4_DEPTH2:
        CUR_DATAPATH(pc) = PHY84328_DATAPATH_4_DEPTH2;
        break;
    default:
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


STATIC int
_phy_84328_reinit_from_mode(int unit, soc_port_t port)
{
    uint16 mode_reg;
    uint16 side_reg;
    uint16 an_reg;
    int speed;
    int an =0, an_done = 0;
    int int_phy_link = 0;
    phy_ctrl_t  *pc, *int_pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    /* 
     * Make sure line side registers are selected
     * if not, have to bail out since the system is in a bad state
     */
    SOC_IF_ERROR_RETURN(
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_XPMD_REGS_SEL, &side_reg));
    if (side_reg != 0) {
        return SOC_E_UNAVAIL;
    }

    /* Read the current mode */
    SOC_IF_ERROR_RETURN(
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1, &mode_reg));

    /* Get speed */
    SOC_IF_ERROR_RETURN(_phy_84328_intf_speed_from_reg(mode_reg, &speed));

    /* Get an setting */
    if (speed < 10000) {
      if (int_pc != NULL) {
        SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_GET(int_pc->pd, unit, port, &an, &an_done));
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &speed));
      }
    } else {
        SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_AN_REG(unit, pc,
                                                     PHY84328_DEV7_AN_CONTROL_REGISTER, &an_reg));
        an = (an_reg & PHY84328_DEV7_AN_CONTROL_REGISTER_AUTO_NEGOTIATIONENABLE_MASK) ==
            PHY84328_DEV7_AN_CONTROL_REGISTER_AUTO_NEGOTIATIONENABLE_MASK;
    }
    
    AN_EN(pc) = an;

    SOC_IF_ERROR_RETURN(
        _phy_84328_reinit_line_intf(unit, port, speed, an, mode_reg));

    SOC_IF_ERROR_RETURN(
        _phy_84328_reinit_system_intf(unit, port, speed, an, mode_reg));
    
    SOC_IF_ERROR_RETURN(
        _phy_84328_reinit_cur_datapath(unit, port, mode_reg));

    /* 
     * PCS from the internal PHY is used to determine link.
     */
    if (int_pc != NULL) {
        SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &int_phy_link));
        CUR_LINK(pc) = int_phy_link;
    } else {
        CUR_LINK(pc) = 0;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84328_reinit(int unit, soc_port_t port)
{
	SOC_IF_ERROR_RETURN(
            _phy_84328_reinit_from_mode(unit,port));

    /* Reinit Firmware RxLOS */
    SOC_IF_ERROR_RETURN(
        _phy_84328_reinit_fw_rx_los(unit, port));
    /* Reinit Software RxLOS */
    SOC_IF_ERROR_RETURN(
        _phy_84328_reinit_sw_rx_los(unit, port) );

    return SOC_E_NONE;
}
#endif /*  BCM_WARM_BOOT_SUPPORT */

STATIC int
_phy_84328_init_pass2(int unit, soc_port_t port)
{
	phy_ctrl_t  *pc;
	phy84328_intf_cfg_t *line_intf, *sys_intf;
	phy84328_counters_t *counters;
	phy84328_sw_rx_los_t *sw_rx_los;
	phy84328_link_mon_t *link_mon;
	phy84328_micro_ctrl_t *micro_ctrl;
	uint16	data, mask;
	uint16  chip_rev;
	int tx_gpio0 = 0;
	int link_mon_dflt, port_enable_delay_dflt;

	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84328 init pass2: u=%d p=%d\n"), unit, port));
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
	counters  = &(COUNTERS(pc));
	sw_rx_los = &(SW_RX_LOS(pc));
	link_mon  = &(LINK_MON(pc));
	micro_ctrl = &(MICRO_CTRL(pc));

	sal_memset(line_intf, 0, sizeof(phy84328_intf_cfg_t));
	sal_memset(sys_intf,  0, sizeof(phy84328_intf_cfg_t));
	sal_memset(counters,  0, sizeof(phy84328_counters_t));
	sal_memset(sw_rx_los, 0, sizeof(phy84328_sw_rx_los_t));
	sal_memset(link_mon,  0, sizeof(phy84328_link_mon_t));
	LOGICAL_LANE0(pc) = 0;
	AN_EN(pc) = 0;
	FORCE_20BIT(pc) = 0;
	CUR_LINK(pc) = 0;
	SYNC_INIT(pc) = 1;
	INT_PHY_RE_EN(pc) = 0;
	DBG_FLAGS(pc) = 0; 
    FW_RX_LOS(pc) = 0;
	BYPASS_SS_TUNING(pc) = soc_property_port_get(unit, port, "phy_84328_bypass_ss_tuning", 1);
	SYS_FORCED_CL72(pc) = soc_property_port_get(unit, port, spn_PORT_INIT_CL72, 0);
	micro_ctrl->enable = 1;
	micro_ctrl->count = 0;

	SOC_IF_ERROR_RETURN(
		READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
									  PHY84328_DEV1_CHIP_REV_REGISTER,
									  &chip_rev));
	DEVREV(pc) = chip_rev;
	port_enable_delay_dflt = (DEVREV(pc) == 0x00a0) ? 60 : 0;
	PORT_ENABLE_DELAY(pc) = soc_property_port_get(unit, port, 
												  "phy_84328_port_enable_delay",
												  port_enable_delay_dflt) * 1000;
	link_mon_dflt = (DEVREV(pc) == 0x00a0) ? 1 : 0;
	link_mon->cfg_enable = soc_property_port_get(unit, port, 
												 "phy_84328_link_monitor", 
												 link_mon_dflt);
	link_mon->cur_enable =  link_mon->cfg_enable;

	pc->flags |= PHYCTRL_INIT_DONE;


    SOC_IF_ERROR_RETURN(_phy_84328_intf_default(unit, port));
	
	/* Manually power channel down by default from driver - 
	 * it is upto the user to enable the port through CLI or API or
	 * by using correct compilation flags to ensure that port is enabled
	 * as part of initialization by upper layers.
	 */
     data = (1 << PHY84328_DEV1_OPTICAL_CONFIGURATION_MAN_TXON_EN_SHIFT) |
             (1 << PHY84328_DEV1_OPTICAL_CONFIGURATION_TXOFFT_SHIFT);
     mask = PHY84328_DEV1_OPTICAL_CONFIGURATION_MAN_TXON_EN_MASK |
            PHY84328_DEV1_OPTICAL_CONFIGURATION_TXOFFT_MASK;

     SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_OPTICAL_CONFIGURATION,
 	                                    data, mask));
	/*
	 * Initialize main firmware control register
	 * - First quiesce firmware and clear default settings
	 * - Enable all channels as there are race conditions if not all channels are 
	 *   enabled when changing to quad-channel mode
	 * - If tx_gpio0 is set, firmware drives line side tx enable/disable based on
	 *   the system side cdr state
	 */
	data = PHY84328_DEV1_GP_REG_0_ENABLE_LINE_3_0_MASK | 
		PHY84328_DEV1_GP_REG_0_ENABLE_SYSTEM_3_0_MASK;
	mask = PHY84328_DEV1_GP_REG_0_ENABLE_LINE_3_0_MASK | 
		PHY84328_DEV1_GP_REG_0_ENABLE_SYSTEM_3_0_MASK;
	tx_gpio0 = soc_property_port_get(unit, port, "phy_84328_tx_gpio0", 0);
	if (tx_gpio0 == 1) {
		data |= PHY84328_DEV1_GP_REG_0_TX_GPIO;
		mask |= data;
	}
    /* Update the core info in bit 6 and 5 */
    mask |= 0x3 << 5;
    if (CORE_NUM(pc)){
        data |= ((CORE_NUM(pc) % 3) + 1) << 5 ;
    }
	SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0,
														data, mask));
	
	/* Reset does not clear lane control2 */
	SOC_IF_ERROR_RETURN
		(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_LANECTRL2, 0,
										 PHY84328_DEV1_LANECTRL2_RLOOP1G_MASK));
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
	SOC_IF_ERROR_RETURN
		(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_LANECTRL2, 0,
										 PHY84328_DEV1_LANECTRL2_RLOOP1G_MASK));
	
	/* Make sure accessing line side registers and 1:1 logical channel mappings */
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_LOGAD_CTRL, 0xe4, 0x00ff));
	
	CFG_DATAPATH(pc) = _phy_84328_datapath_get(unit, port);
	CUR_DATAPATH(pc) = CFG_DATAPATH(pc);

	/* Polarity configuration is applied every time interface is configured */
    SOC_IF_ERROR_RETURN(_phy_84328_polarity_cfg_init(unit, port));


    /* Push configuration to the device */
    SOC_IF_ERROR_RETURN(_phy_84328_config_update(unit, port));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        /* Do nothing */
    } else 
#endif
    {
        /* Initialize system and line side interfaces */
        SOC_IF_ERROR_RETURN(_phy_84328_intf_line_sys_init(unit, port));
    }

    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) {
        /* indicate third  pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS3);
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84328_init_pass3(int unit, soc_port_t port)
{
    int rv;
    phy_ctrl_t  *pc, *int_pc;
    int	regulator_en = 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84328 init pass3: u=%d p=%d\n"), unit, port));
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


    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84328 init pass3 polarity verified: u=%d p=%d\n"), unit, port));

	regulator_en = soc_property_port_get(unit, port, spn_PHY_AUX_VOLTAGE_ENABLE, 0);
	if (regulator_en) {
		/* Clear 1.c850[13] to enable */
		SOC_IF_ERROR_RETURN
			(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_REGUALTOR_CTRL, 0,
											 PHY84328_DEV1_REGUALTOR_CTRL_PWRDN_MASK));
	}
	
	/* Logical lane0 used for auto-negotiation in 40G CR4 */
	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		uint16 logical_lane0;
		
		logical_lane0 = soc_property_port_get(unit, port, spn_PHY_LANE0_L2P_MAP, 0);
		if (logical_lane0 > PHY84328_NUM_LANES) {
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "PHY84328 bad auto-negotiation lane %d: u=%d p=%d"
                                              " lane must be 0..3\n"), (int)logical_lane0, unit, port));
		} else {
			SOC_IF_ERROR_RETURN(
				MODIFY_PHY84328_MMF_PMA_PMD_REG(
					unit, pc, PHY84328_DEV1_SINGLE_PMD_CTRL, 
					logical_lane0 << PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_SHIFT,
					PHY84328_DEV1_SINGLE_PMD_CTRL_PHY_CH_TO_ACCESS_IN_SINGLE_PMD_MASK));
			LOGICAL_LANE0(pc) = logical_lane0;
		}
	}
	
	/* If configured, enable module auto detection */
	MOD_AUTO_DETECT(pc) = soc_property_port_get(unit, port, spn_PHY_MOD_AUTO_DETECT, 0);
	if (MOD_AUTO_DETECT(pc)) {
		uint16 data, mask;
		
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "PHY84328 module auto detect enabled: u=%d p=%d\n"),
                                      unit, port));

        /* SFP MOD_DETECT enables both SFP and QSFP module detect */
		data = PHY84328_DEV1_GP_REG_0_ENABLE_SFP_MOD_DETECT_MASK;
		mask = PHY84328_DEV1_GP_REG_0_ENABLE_SFP_MOD_DETECT_MASK;
		
		rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, data, mask);
		if (SOC_FAILURE(rv)) {
			MOD_AUTO_DETECT(pc) = 0;
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "PHY84328 setting module auto detect failed: u=%d p%d\n"),
                                              unit, port));
		} 
	}

    if (soc_property_port_get(unit, port, "phy_84328_port_4_5_control", 0)) {
        rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, 
                                             PHY84328_DEV1_GP_REG_0_P4_5_CTRL,
                                             PHY84328_DEV1_GP_REG_0_P4_5_CTRL);
		if (SOC_FAILURE(rv)) {
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "PHY84328 setting p4_5 control failed: u=%d p%d\n"),
                                              unit, port));
		} 
    }

	
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return _phy_84328_reinit(unit, port);
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
STATIC int
_phy_84328_core_init_done(int unit, struct phy_driver_s *pd, int primary_port)
{
	int rv;
	soc_port_t port;
	phy_ctrl_t *pc;
	uint32 core_primary_port;

	PBMP_ALL_ITER(unit, port) {
		pc = EXT_PHY_SW_STATE(unit, port);
		if (pc == NULL) {
			continue;
		}
		/* Make sure this port has a 84328 driver */
		if (pc->pd != pd) {
			continue;
		}
		rv = phy_84328_control_port_get(unit, port, SOC_PHY_CONTROL_PORT_PRIMARY,
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

STATIC int
_phy_84328_init_pass1(int unit, soc_port_t port)
{
    phy_ctrl_t			*pc;
    int					offset;
    soc_port_t			primary_port;
		
	pc = EXT_PHY_SW_STATE(unit, port);

	
	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84328 init pass1: u=%d p=%d\n"), unit, port));
	
	/* Set the primary port & offset */
	if (soc_phy_primary_and_offset_get(unit, port, &primary_port, &offset) != SOC_E_NONE) {
		/* 
		 * Derive primary and offset 
		 * There is an assumption lane 0 on first core on the the MDIO
		 * bus is a primary port
		 */
		if (PHY84328_SINGLE_PORT_MODE(pc)) {
			primary_port = port;
			offset = 0;
		} else {
			primary_port = port - (pc->phy_id & 0x3);
			offset = pc->phy_id & 0x3;
		}
	}
	/* set the default values that are valid for many boards */
	SOC_IF_ERROR_RETURN(phy_84328_control_port_set(unit, port, SOC_PHY_CONTROL_PORT_PRIMARY,
											  primary_port));
	
	SOC_IF_ERROR_RETURN(phy_84328_control_port_set(unit, port,SOC_PHY_CONTROL_PORT_OFFSET,
											  offset));
	
	/*
	 * Since there is a single micro and therefore a single copy of the firmware
	 * shared among the 4 channels in a core, do not download firmware if in quad
	 * channel and the other channels are active.
	 */
	if (PHY84328_SINGLE_PORT_MODE(pc) || 
		(!PHY84328_SINGLE_PORT_MODE(pc) && 
		 (!_phy_84328_core_init_done(unit, pc->pd, primary_port)))) {
            uint16 csr_c848 = 0;

            /* read Ch. 0 1.C848, bit [13] is download done */
            SOC_IF_ERROR_RETURN(
                READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                                              PHY84328_DEV1_SPA_CONTROL_STATUS_REGISTER,
                                              &csr_c848));
            if ((csr_c848 & (1 << 13)) &&
               !(soc_property_port_get(unit, port,
                           spn_PHY_FORCE_FIRMWARE_LOAD, TRUE))) {
               	uint8 *fw_ptr = NULL;
                int fw_length = 0;
                uint16 chip_micro_ver = 0, file_micro_ver = 0;

                SOC_IF_ERROR_RETURN(
                     READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                                         PHY84328_SYS_DEV1_VERSION,
                                         &chip_micro_ver));
	            if (pc->dev_name == dev_name_84328_a0) {
                    fw_ptr = phy84328_ucode_bin;
                    fw_length = phy84328_ucode_bin_len;
                } else if (pc->dev_name == dev_name_84328) {
                    fw_ptr = phy84328B0_ucode_bin;
                    fw_length = phy84328B0_ucode_bin_len;
                } else if (pc->dev_name == dev_name_84088) {
                    fw_ptr = phy84328B0_ucode_bin;
                    fw_length = phy84328B0_ucode_bin_len;
                } else {
                    /* for other device donot take micro version*/
                    pc->flags &= (~PHYCTRL_MDIO_BCST);
                    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
                        /* indicate second pass of init is needed */
                        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
                    }
                    return SOC_E_NONE;
                }
                /* MICRO version is reterived from 4th and 3rd last byte of micro file*/
                file_micro_ver = (fw_ptr[fw_length-4] << 8) | (fw_ptr[fw_length-3]);
                if (file_micro_ver != chip_micro_ver) {
                    pc->flags |= PHYCTRL_MDIO_BCST;
                } else {
                    pc->flags &= (~PHYCTRL_MDIO_BCST);
                }
                
                /*
                LOG_CLI((BSL_META_U(unit,
                                    "84328 u=%d p=%d skip fw download\n"), unit, port));
                */
            } else {
                pc->flags |= PHYCTRL_MDIO_BCST;
            }
	}
	
	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_84328_init: u=%d p=%d setting primary=%d offset=%d\n"),
                             unit, port, primary_port, offset));
	
    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
        /* indicate second pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
    }

	return SOC_E_NONE;
}

/* Function:
 *    phy_84328_init
 * Purpose:    
 *    Initialize 84328 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_84328_init(int unit, soc_port_t port)
{
	int					ix;
	uint16				data;
	uint32				devid;
    phy_ctrl_t			*pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84328 init: u=%d p=%d state=%d\n"), 
                         unit, port,PHYCTRL_INIT_STATE(pc)));

	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

		/* Read the chip id to identify the device */
		SOC_IF_ERROR_RETURN(_phy_84328_config_devid(unit, port, pc, &devid));

		/* Save it in the device description structure for future use */  
		DEVID(pc) = devid;

		/* initialize default p2l map */
		for (ix = 0; ix < PHY84328_NUM_LANES; ix++) {
			P2L_MAP(pc,ix) = ix;
		}

        PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45 | PHY_FLAGS_REPEATER);
		data = (PHY84328_SINGLE_PORT_MODE(pc)) ? 
			PHY84328_DEV1_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK : 0;
		SOC_IF_ERROR_RETURN
			(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc,
											 PHY84328_DEV1_SINGLE_PMD_CTRL, data,
											 PHY84328_DEV1_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK));

        SOC_IF_ERROR_RETURN(_phy_84328_init_pass1(unit, port));

        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }

	}

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        SOC_IF_ERROR_RETURN( _phy_84328_init_pass2(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84328 init pass2 completed: u=%d p=%d\n"), unit, port));

        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        SOC_IF_ERROR_RETURN( _phy_84328_init_pass3(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84328 init pass3 completed: u=%d p=%d\n"), unit, port));
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_DEFAULT);
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84328_dev_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
	uint16 data;
	uint16 dev_an_done_mask; 
	phy_ctrl_t  *pc;

	pc = EXT_PHY_SW_STATE(unit, port);
	*an = 0;
	*an_done = 0;

	/*
	 * 7.0000[12] an enabled
	 * 1.0097[0]  an/training complete. For 40G, per lane status in bits 12,8,4,0
	 */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_AN_REG(unit, pc, 
												 PHY84328_DEV7_AN_CONTROL_REGISTER, &data));
	*an = (data & PHY84328_DEV7_AN_CONTROL_REGISTER_AUTO_NEGOTIATIONENABLE_MASK) ==
		PHY84328_DEV7_AN_CONTROL_REGISTER_AUTO_NEGOTIATIONENABLE_MASK;

	if (*an) {
		SOC_IF_ERROR_RETURN(
			READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_BASE_R_PMD_STATUS_REGISTER,
										  &data));
		dev_an_done_mask = PHY84328_SINGLE_PORT_MODE(pc) ? 0x1111 : 0x0001;
		*an_done = (data & dev_an_done_mask) == dev_an_done_mask;
	}

	return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84328_an_get
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

STATIC int
phy_84328_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
	phy_ctrl_t  *pc, *int_pc;
	phy84328_intf_cfg_t  *line_intf;

	pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

	if ((line_intf->type == SOC_PORT_IF_GMII) || (line_intf->type == SOC_PORT_IF_SGMII)) {
		SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_GET(int_pc->pd, unit, port, an, an_done));
	} else {
		switch (line_intf->type) {
		case SOC_PORT_IF_KX:
		case SOC_PORT_IF_KR:
		case SOC_PORT_IF_KR4:
		case SOC_PORT_IF_CR4:
			SOC_IF_ERROR_RETURN(_phy_84328_dev_an_get(unit, port, an, an_done));
			break;
		default:
			*an = 0;
			*an_done = 0;
			break;
		}
	}

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_an_get: u=%d p=%d an=%d\n"), unit, port, *an));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84328_an_set
 * Purpose:
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_84328_an_set(int unit, soc_port_t port, int an)
{
	int dev_an, dev_an_done;
	phy_ctrl_t  *pc, *int_pc;
	phy84328_intf_cfg_t  *line_intf;

	pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_an_set: u=%d p=%d an=%s\n"),
                         unit, port, an ? "enable" : "disable"));


	/*
	 * Determining line side AN status 
	 *  SR/LR/SR4/LR4/CR/CR2/SR2: no AN 
	 *  KR/KR4/KR2: line side: cl73/cl72, system side as forced cl72
	 *  CR4: if AN disabled, set interface to 40G CR4 DAC(aka XLAUI_DFE), otherwise, CR4
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
	SOC_IF_ERROR_RETURN(_phy_84328_intf_type_set(unit, port, line_intf->type, TRUE));

    /* If not passtrhu an, make sure firmware has enabled an */
    if (line_intf->speed > 1000) {
        _phy_84328_dev_an_get(unit, port, &dev_an, &dev_an_done);
        if (dev_an != AN_EN(pc)) {
     /*
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "84328 device autonegotiation mismatch: u=%d p=%d an=%s\n"),
                                  unit, port, AN_EN(pc) ? "en" : "dis"));
     */
        }
    } 
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84328_ability_advert_get
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

STATIC int
_phy_84328_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
	phy_ctrl_t  *pc, *int_pc;
    uint16 pause_adv;
	soc_port_mode_t mode = 0;
	phy84328_intf_cfg_t  *line_intf;

	if (ability == NULL) {
		return SOC_E_PARAM;
	}

	sal_memset(ability, 0, sizeof(soc_port_ability_t));

	pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));
    if (_phy_84328_intf_line_forced(unit, port, line_intf->type)) {
        return SOC_E_NONE;
    }

    if ((line_intf->type == SOC_PORT_IF_KX) || (line_intf->type == SOC_PORT_IF_GMII)) {
        int_pc = INT_PHY_SW_STATE(unit, port);
        if (int_pc) {
            SOC_IF_ERROR_RETURN(PHY_ABILITY_ADVERT_GET(int_pc->pd, unit, port, ability));
        }
        return SOC_E_NONE;
    }
     
    if(PHY84328_SINGLE_PORT_MODE(pc)) {
        ability->speed_full_duplex = SOC_PA_SPEED_40GB;
    }
    else {
        ability->speed_full_duplex = SOC_PA_SPEED_10GB;
    }
    SOC_IF_ERROR_RETURN
        (READ_PHY84328_MMF_AN_REG(unit, pc, PHY84328_DEV7_AN_ADVERTISEMENT_1_REGISTER, &pause_adv));
    
    mode = 0;
    switch (pause_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
    case CL73_AN_ADV_PAUSE:
        mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        break;
    case CL73_AN_ADV_ASYM_PAUSE:
        mode = SOC_PA_PAUSE_TX;
        break;
    case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
        mode = SOC_PA_PAUSE_RX;
        break;
    default:
        mode = 0;
        break;
    }
    ability->pause = mode;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_ability_advert_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
* Function:
*      _phy_84328_ability_remote_get
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
_phy_84328_ability_remote_get(int unit, soc_port_t port, 
                            soc_port_ability_t *ability)
{
    int an, an_done;
    uint16      pause_adv;
    soc_port_mode_t mode;
    phy_ctrl_t *pc, *int_pc;
	phy84328_intf_cfg_t  *line_intf;

    if (ability == NULL) {
        return SOC_E_PARAM;
    }

	/* Only firmware has visibility into remote ability */
	sal_memset(ability, 0, sizeof(soc_port_ability_t));

    pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

    if (_phy_84328_intf_line_forced(unit, port, line_intf->type)) {
        return SOC_E_NONE;
    }

    if ((line_intf->type == SOC_PORT_IF_KX) || (line_intf->type == SOC_PORT_IF_GMII)) {
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
    SOC_IF_ERROR_RETURN(phy_84328_an_get(unit, port, &an, &an_done));
    if (an && an_done) {
        if(PHY84328_SINGLE_PORT_MODE(pc)) {
            ability->speed_full_duplex = SOC_PA_SPEED_40GB;
        }
        else {
            ability->speed_full_duplex = SOC_PA_SPEED_10GB;
        }

        mode = 0;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_AN_REG(unit, pc, PHY84328_DEV7_AN_LP_BASE_PAGE_ABILITY_1_REG, &pause_adv));
        switch (pause_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
        break;
        }
        ability->pause = mode;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_ability_remote_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84328_ability_advert_set
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

STATIC int
_phy_84328_ability_advert_set(int unit, soc_port_t port,
							 soc_port_ability_t *ability)
{
    uint16 pause;
	phy_ctrl_t  *pc, *int_pc;
	phy84328_intf_cfg_t  *line_intf;

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

    /*
     * Advertisement mostly handled by firmware depending on the mode 
     * However, the firmware will honor the pause advertisement set by the driver
     */
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        pause = CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE;
        break;
    default:
        pause = 0;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84328_MMF_AN_REG(unit, pc, PHY84328_DEV7_AN_ADVERTISEMENT_1_REGISTER,
                                    pause, (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_ability_advert_set: u=%d p=%d speed(FD)=%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84328_ability_local_get
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
phy_84328_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
	uint32		pa_speed = 0;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
	
    if (ability == NULL) {
        return SOC_E_PARAM;
    }
	sal_memset(ability, 0, sizeof(soc_port_ability_t));

    if (PHY84328_MULTI_CORE_MODE(pc)) {
	    pa_speed = SOC_PA_SPEED_100GB;
    } else {
	pa_speed = PHY84328_SINGLE_PORT_MODE(pc) ?
        (SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB) :
		(SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB | SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB);
    }
        
	ability->speed_full_duplex = pa_speed;
	ability->speed_half_duplex = SOC_PA_ABILITY_NONE;
    ability->pause = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG; 

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_ability_local_get: u=%d p=%d speed=0x%x\n"),
              unit, port, ability->speed_full_duplex));
                                                                               
    return SOC_E_NONE;
}

STATIC int
_phy_84328_mod_auto_detect_speed_check(int unit, soc_port_t port, uint16 csr)
{
	uint16 csr_speed;
	phy_ctrl_t *pc;
	phy84328_intf_cfg_t *line_intf;
	static int speed_tbl[] = {	0, 1000, 10000, 1000, 40000, 0, 0 ,0, 10000 };

	pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

	csr_speed = csr & (PHY84328_DEV1_GP_REGISTER_1_SPEED_100G_MASK | 
					   PHY84328_DEV1_GP_REGISTER_1_SPEED_40G_MASK |
					   PHY84328_DEV1_GP_REGISTER_1_SPEED_10G_MASK |
					   PHY84328_DEV1_GP_REGISTER_1_SPEED_1G_MASK);
	if (line_intf->speed != speed_tbl[csr_speed]) {
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328 module auto detection unexpected: u=%d p=%d csr=0x%x "
                                      "csr table speed=%d line speed=%d\n"),
                           unit, port, csr, speed_tbl[csr_speed], line_intf->speed));
	}
	return (line_intf->speed == speed_tbl[csr_speed]);
}

STATIC int
_phy_84328_mod_auto_detect_intf_process(int unit, soc_port_t port, uint16 csr)
{
	int copper_mod = 0;
	int lr_mod = 0;
	phy_ctrl_t *pc;
	phy84328_intf_cfg_t *line_intf;
	int intf_update = 1;

	pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

	copper_mod = (csr & PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_MASK) ==
		PHY84328_DEV1_GP_REGISTER_1_LINE_CU_TYPE_MASK;

	lr_mod = (csr & PHY84328_DEV1_GP_REGISTER_1_LINE_LR_MODE_MASK) ==
		PHY84328_DEV1_GP_REGISTER_1_LINE_LR_MODE_MASK;

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
		SOC_IF_ERROR_RETURN(_phy_84328_intf_line_sys_update(unit, port));
	}
	return SOC_E_NONE;
}

STATIC int
_phy_84328_mod_auto_detect_process(int unit, soc_port_t port, uint16 csr) 
{

	LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "PHY84328 module detected: u=%d p=%d csr=%x\n"), 
                              unit, port, csr));

	/*
	 *  Speed cannot change with module detection so it must match the
	 *  currently configured speed
	 */
	if (!_phy_84328_mod_auto_detect_speed_check(unit, port, csr)) {
		return SOC_E_FAIL;
	}
		
	/*
	 * Get the line side interface based on the current interface and the 
	 * detected module
	 */
	SOC_IF_ERROR_RETURN(_phy_84328_mod_auto_detect_intf_process(unit, port, csr));

	return SOC_E_NONE;
}



STATIC int 
_phy_84328_mod_auto_detect_update(int unit, soc_port_t port) 
{
	phy_ctrl_t *pc;
	uint16 ucode_csr;
	uint16 drv_csr;

    pc = EXT_PHY_SW_STATE(unit, port);

	/* 
	 * New module insertion when 1.c843[4] == 1 and 1.c841[4] == 0
	 */ 
	SOC_IF_ERROR_RETURN
		(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_3, &ucode_csr));
	if (ucode_csr & PHY84328_CSR_REG_MODULE_AUTO_DETECT) {

		SOC_IF_ERROR_RETURN
			(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1, &drv_csr));
		if (!(drv_csr & PHY84328_CSR_REG_MODULE_AUTO_DETECT)) {
			SOC_IF_ERROR_RETURN(_phy_84328_mod_auto_detect_process(unit, port, ucode_csr));
			
			/* 
			 * Ack firmware that the module insertion has been updated 
			 * 1.c841[4] = 1, 1.c841[7] = 0
			 */
			SOC_IF_ERROR_RETURN
				(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1, 
												 PHY84328_CSR_REG_MODULE_AUTO_DETECT,
												 PHY84328_CSR_REG_MODULE_AUTO_DETECT |
												 PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK));
		}
	} else {
		/* 
		 * New module removal when 1.c843[4] == 0 and 1.c841[4] == 1
		 */
		SOC_IF_ERROR_RETURN
			(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1, &drv_csr));
		if (drv_csr & PHY84328_CSR_REG_MODULE_AUTO_DETECT) {

			/* 
			 * Ack firmware new module removal 1.c841[4] = 0 
			 */
			SOC_IF_ERROR_RETURN
				(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_1, 
												 0, PHY84328_CSR_REG_MODULE_AUTO_DETECT));
			
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "PHY84328 module removed u=%d p=%d\n"), unit, port));
		}
	}

	return SOC_E_NONE;
}

STATIC int
_phy_84328_do_rxseq_restart(int unit, soc_port_t port,phy84328_intf_side_t side)
{
	uint16 data;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	PHY_84328_MICRO_PAUSE(unit, port, "rxseq restart1");
    if (side == PHY84328_INTF_SIDE_SYS) {
	    _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
    }
        SOC_IF_ERROR_RETURN
		    (MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_DSC_MISC_CTRL0,
										 PHY84328_DEV1_DSC_MISC_CTRL0_RXSEQSTART_MASK,
										 PHY84328_DEV1_DSC_MISC_CTRL0_RXSEQSTART_MASK));
    if (side == PHY84328_INTF_SIDE_SYS) {
	    _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
    }
	PHY_84328_MICRO_RESUME(unit, port);

	sal_udelay(800);
	
	PHY_84328_MICRO_PAUSE(unit, port, "rxseq restart 2");
    if (side == PHY84328_INTF_SIDE_SYS) {
	    _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
    }
	    SOC_IF_ERROR_RETURN
		    (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_DSC_MISC_CTRL0,
			    						   &data));
	    if (data & PHY84328_DEV1_DSC_MISC_CTRL0_RXSEQSTART_MASK) {
		    /*
		     * Usually 1.c21e[15] rx seq start is cleared by the microcode but there are cases when 
		     * them micro won't clear it. 
		     */
		    SOC_IF_ERROR_RETURN
			    (MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
											 PHY84328_SYS_DEV1_DSC_MISC_CTRL0, 0,
											 PHY84328_DEV1_DSC_MISC_CTRL0_RXSEQSTART_MASK));
	    }
    if (side == PHY84328_INTF_SIDE_SYS) {
	    _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
    }
	PHY_84328_MICRO_RESUME(unit, port);

	return SOC_E_NONE;
}

/* 
 * Restart rx sequencer 
 *   no_cdr_lanes == 0: all lanes have cdr - rxseq being restarted because pcs not up
 *                != 0: bitmap of lanes which do not have cdr so restart only those lanes
 */
STATIC int
_phy_84328_rxseq_restart(int unit, soc_port_t port, int no_cdr_lanes)
{
	int lane;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
	phy84328_counters_t *counters =  &(COUNTERS(pc));

	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		if (no_cdr_lanes) {
			for (lane=0; lane < 4; lane++) {
				if (no_cdr_lanes & (1<<lane)) {
					counters->no_cdr++;
					/* Select the lane on the line side */
					SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, 
																  PHY84328_INTF_SIDE_LINE, lane));
					_phy_84328_do_rxseq_restart(unit, port,
                                                PHY84328_INTF_SIDE_LINE);
				}
			}
			SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE, 
														  PHY84328_ALL_LANES));
		}
	} 

	if (!no_cdr_lanes) {
		SOC_IF_ERROR_RETURN(_phy_84328_do_rxseq_restart(unit, port,
                                                    PHY84328_INTF_SIDE_LINE));
	}
	return SOC_E_NONE;
}


/*
 * Gets rx seq done status
 * Returns rx_seq_done status and  a lane bitmask for lanes which have sigdet but no rx seq done/cdr locked
 */
STATIC int
_phy_84328_rx_seq_done_cdr_lanes_get(int unit, soc_port_t port, phy84328_intf_side_t side,
									 uint32 *rx_seq_done, uint32 *no_cdr_lanes)
{
	uint16 data;
	int lane_end, lane;
	uint16  cdr_locked;
	phy_ctrl_t *pc;
	pc = EXT_PHY_SW_STATE(unit, port);
	cdr_locked = 0;
	*no_cdr_lanes = 0;

    if (side == PHY84328_INTF_SIDE_SYS) {
        PHY_84328_MICRO_PAUSE(unit, port, "cdr lanes get");
    }

	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		lane_end = PHY84328_NUM_LANES;
	}
    else {
		lane_end = 1;
		_phy_84328_intf_side_regs_select(unit, port, side);
	}

	for (lane = 0; lane < lane_end; lane++) {
		if (PHY84328_SINGLE_PORT_MODE(pc)) {
			/* Select the lane on the line side */
            SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, lane));
		}
		/* Select SigDet/CDR lock status to be read in anaRxStatus */
		SOC_IF_ERROR_RETURN(
			READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_ANARXCONTROL, &data));
		
		if ((data & PHY84328_SYS_DEV1_ANARXCONTROL_STATUS_SEL_MASK) != 0) {
			SOC_IF_ERROR_RETURN(
				MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
												PHY84328_SYS_DEV1_ANARXCONTROL, 0, 
												PHY84328_SYS_DEV1_ANARXCONTROL_STATUS_SEL_MASK)); 
		}

		SOC_IF_ERROR_RETURN(
			READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANARXSTATUS, &data));

		/* CDR==RxSeqDone */
		if ((data & PHY84328_DEV1_ANARXSTATUS_RXSTATUS_CDR) ==
			PHY84328_DEV1_ANARXSTATUS_RXSTATUS_CDR) {
			cdr_locked++;
		} else if (data & 0x8000) {
			/* Sigdet but no cdr lock */
			*no_cdr_lanes |= 1 << lane;
		}
    }
	/* Restore to default single port register access */
	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE, 
													  PHY84328_ALL_LANES));
	}
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
    if (side == PHY84328_INTF_SIDE_SYS) {
        PHY_84328_MICRO_RESUME(unit, port);
    }

	*rx_seq_done = (cdr_locked == lane_end);
	return SOC_E_NONE;
}

STATIC int
_phy_84328_rx_seq_done_get(int unit, soc_port_t port, phy84328_intf_side_t side, uint32 *rx_seq_done)
{
	uint32 no_cdr_lanes;

	return _phy_84328_rx_seq_done_cdr_lanes_get(unit, port, side, rx_seq_done, &no_cdr_lanes);
}

STATIC int
_phy_84328_sw_rx_los_check(int unit, soc_port_t port, uint16 link, uint16 *new_link)
{
	int pcs_link;
	uint8  sys_link;
	phy_ctrl_t *pc;
	phy84328_sw_rx_los_states_t rx_los_state;
	phy84328_sw_rx_los_t *sw_rx_los;
	uint32 no_cdr_lanes = 0;
	uint32 rx_seq_done = 0;

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
	int dbg_flag = 1;
#endif

	pc = EXT_PHY_SW_STATE(unit, port);
	sw_rx_los = &(SW_RX_LOS(pc));

#ifdef BCM_WARM_BOOT_SUPPORT
    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        *new_link = sw_rx_los->sys_link;
        return SOC_E_NONE;
    }
#endif

	pcs_link = link; 
	sys_link = sw_rx_los->sys_link;
	rx_los_state = sw_rx_los->state;
	sw_rx_los->link_status = 0;
	no_cdr_lanes = 0;
	rx_seq_done = 0;

	SOC_IF_ERROR_RETURN(
		_phy_84328_rx_seq_done_cdr_lanes_get(unit, port, PHY84328_INTF_SIDE_LINE,  
											 &rx_seq_done, &no_cdr_lanes));

	if (rx_los_state != PHY84328_SW_RX_LOS_IDLE) {
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
		LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "84328 SRL_IN: u=%d p=%d state=%s pcs_link=%d sys_link=%d link_status=%d "
                                        "rx_seq_done=%d no_cdr_lanes=%x\n"), 
                             unit, port, sw_rx_los_state_names[rx_los_state], pcs_link, sys_link, 
                             sw_rx_los->link_status, rx_seq_done, no_cdr_lanes));
#endif				 
	}

	/* 
	 * Current State: Don't Care
	 * Next State: IDLE
	 * If link was previously TRUE and is still true, 
	 * set the state to IDLE
	 */
	if ((sys_link == TRUE) && (pcs_link == TRUE)) {
		rx_los_state = PHY84328_SW_RX_LOS_IDLE;
	}

	/* 
	 * Current State: IDLE
	 * Next State: RESET
	 * If the state was IDLE(link up) but now link is down, 
	 * enter the RESET state 
	 */
	if ((rx_los_state == PHY84328_SW_RX_LOS_IDLE) && (pcs_link == FALSE)) {
		rx_los_state = PHY84328_SW_RX_LOS_RESET;
		sys_link = FALSE;
	} 

	/* 
	 * Current State: RESET
	 * Next State: INITIAL_LINK or START_TIMER
	 * If state is RESET, it's next state could be INITIAL LINK
	 * or START_TIMER depending upon the pcs_link state
	 */
	if (rx_los_state == PHY84328_SW_RX_LOS_RESET) {
		if ((pcs_link == TRUE) && rx_seq_done) {
			rx_los_state = PHY84328_SW_RX_LOS_INITIAL_LINK;
		} else if (rx_seq_done) {
			rx_los_state = PHY84328_SW_RX_LOS_START_TIMER;
			sw_rx_los->ls_ticks = 0;
		}
	}
	if (rx_los_state != PHY84328_SW_RX_LOS_IDLE) {
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)	
        if (dbg_flag == 1) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "84328 SRL_UPD: u=%d p=%d state=%s pcs_link=%d sys_link=%d link_status=%d "
                                    "rx_seq_done=%d\n====\n"), 
                         unit, port, sw_rx_los_state_names[rx_los_state], pcs_link, sys_link, 
                         sw_rx_los->link_status, rx_seq_done));
        }
#endif		
	}

	/* Take action depending upon the current state */
	switch (rx_los_state) {
	case PHY84328_SW_RX_LOS_IDLE:
		/* Do nothing */
		/* Link ON condition */
		break;
		
	case PHY84328_SW_RX_LOS_RESET:
#ifdef SW_RX_LOS_FLAP_CHECK
		if (sw_rx_los->fault_report_dis) {
                    SOC_IF_ERROR_RETURN
                        (MAC_CONTROL_SET(sw_rx_los->macd, unit, port,
                                         SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
                    sw_rx_los->fault_report_dis = 0;
		}
#endif
		sys_link = FALSE;
		break;
		
	case PHY84328_SW_RX_LOS_INITIAL_LINK:
#ifdef SW_RX_LOS_FLAP_CHECK
		/* Disable local fault reporting to avoid link flaps untill
		 *  link stablizes                                                                              
		 */
		SOC_IF_ERROR_RETURN
			(MAC_CONTROL_SET(sw_rx_los->macd, unit, port,
							 SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, FALSE));
		sw_rx_los->fault_report_dis = 1;
#endif
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
        if (dbg_flag == 1) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "84328 sw rx los: u=%d p=%d state=%s: restarting RxSeq\n"), 
                         unit, port, sw_rx_los_state_names[rx_los_state]));
        }
#endif
		sw_rx_los->restarts++;
		SOC_IF_ERROR_RETURN(_phy_84328_rxseq_restart(unit, port, no_cdr_lanes));

		/* Skip the link transition for this rx reset */
		sw_rx_los->link_status = 1;
		rx_los_state = PHY84328_SW_RX_LOS_LINK;
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)		
        if (dbg_flag == 1) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "84328 sw rx los: u=%d p=%d state:%s \n"), 
                         unit, port, sw_rx_los_state_names[rx_los_state]));
        }
#endif
		break;

	case PHY84328_SW_RX_LOS_LINK:

		if( pcs_link == TRUE) {
			sys_link = TRUE;
			sw_rx_los->link_no_pcs = 0;
			rx_los_state = PHY84328_SW_RX_LOS_IDLE;
		} else {
			sw_rx_los->link_no_pcs++;
			if (sw_rx_los->link_no_pcs > 20) {
				rx_los_state = PHY84328_SW_RX_LOS_RESET;
				sw_rx_los->link_no_pcs = 0;
			}
		}
#ifdef SW_RX_LOS_FLAP_CHECK
		/* Enable local fault reporting disabled earlier */
		SOC_IF_ERROR_RETURN
			(MAC_CONTROL_SET(sw_rx_los->macd, unit, port,
							 SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
		sw_rx_los->fault_report_dis = 0;
#endif
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
        if (dbg_flag == 1) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "84328 sw rx los: u=%d p=%d state:%s \n"), 
                         unit, port, sw_rx_los_state_names[rx_los_state]));
        }
#endif		
		break;

	case PHY84328_SW_RX_LOS_START_TIMER:
		sw_rx_los->ls_ticks++;
		if (pcs_link || (sw_rx_los->ls_ticks > 24)) {
			rx_los_state = PHY84328_SW_RX_LOS_RX_RESTART;  
			sw_rx_los->ls_ticks = 0;
		}
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)		
        if (dbg_flag == 1) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "84328 sw rx los: u=%d p=%d state:%s \n"), 
                         unit, port, sw_rx_los_state_names[rx_los_state]));
        }
#endif		
		break;

	case PHY84328_SW_RX_LOS_RX_RESTART:
#ifdef SW_RX_LOS_FLAP_CHECK
		/* 
		 * Disable local fault reporting to avoid link flaps until link stablizes
		 */
		SOC_IF_ERROR_RETURN
			(MAC_CONTROL_SET(sw_rx_los->macd, unit, port,
							 SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, FALSE));
		sw_rx_los->fault_report_dis = 1;
#endif
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
	    if (dbg_flag == 1) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "84328 sw rx los: u=%d p=%d state:%s -> restarting RxSeq\n"), 
                         unit, port, sw_rx_los_state_names[rx_los_state]));
        }
#endif
		sw_rx_los->restarts++;
		SOC_IF_ERROR_RETURN(_phy_84328_rxseq_restart(unit, port, no_cdr_lanes));

		if( pcs_link == FALSE) {
			rx_los_state = PHY84328_SW_RX_LOS_RESET;
		} else {
			rx_los_state = PHY84328_SW_RX_LOS_LINK;
		}
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)		
	    if (dbg_flag == 1) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "84328 sw rx los : u=%d p=%d state:%s \n"), 
                         unit, port, sw_rx_los_state_names[rx_los_state]));
        }
#endif
		/* Skip the link transition for this rx reset */
		sw_rx_los->link_status = 1;
		break;

	default:
		break;
	}

	sw_rx_los->sys_link = sys_link;;
	sw_rx_los->state = rx_los_state;
	*new_link = sys_link;

	return SOC_E_NONE;
}

/* Return contents of ANARXSTATUS */
STATIC int
_phy_84328_intf_link_get(int unit, soc_port_t port, uint16 *link)
{
	int			lane, max_lane;
	uint16		data;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	*link = 0;
    if (PHY84328_SINGLE_PORT_MODE(pc)) {
        max_lane = PHY84328_NUM_LANES;
    } 
    else {
        max_lane = 1;
    }
	for (lane = 0; lane < max_lane; lane++) {
		if (PHY84328_SINGLE_PORT_MODE(pc)) {
            /* Select the lane on the line side */
            SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE, lane));
        }
		/* Select SigDet/CDR lock status to be read in anaRxStatus */
		SOC_IF_ERROR_RETURN(
			READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_ANARXCONTROL, &data));
		if ((data & PHY84328_SYS_DEV1_ANARXCONTROL_STATUS_SEL_MASK) != 0) {
			SOC_IF_ERROR_RETURN(
				MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_ANARXCONTROL,
												0, PHY84328_SYS_DEV1_ANARXCONTROL_STATUS_SEL_MASK)); 
		}

		/* Check if rxSeq Done in any lane */
		SOC_IF_ERROR_RETURN(
			READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANARXSTATUS, &data));
		if ((data & PHY84328_DEV1_ANARXSTATUS_RXSTATUS_CDR) ==
            PHY84328_DEV1_ANARXSTATUS_RXSTATUS_CDR) {
			*link = 1;
			break;
		}
	}
    /* Restore to default single port register access */
    if (PHY84328_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE,
                                                      PHY84328_ALL_LANES));
    }
    _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);

	return SOC_E_NONE;
}

/* 
 * Called when there is no link so check different link components and attempt to 
 * recover by restarting interface if appropriate
 */
STATIC int
_phy_84328_link_recover(int unit, soc_port_t port, int line_link, int int_phy_link)
{
	phy_ctrl_t  *pc;
	phy84328_counters_t *counters;
	phy84328_link_mon_t *link_mon;

    pc = EXT_PHY_SW_STATE(unit, port);
	counters = &(COUNTERS(pc));
	link_mon = &(LINK_MON(pc));

	if (line_link && !int_phy_link) {
		if (link_mon->debounce > PHY84328_LINK_DEBOUNCE) {
			counters->retry_serdes_link++;
			LOG_INFO(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "84328 restarting intf: u=%d p=%d line=%04x internal=%04x debounce=%d\n"), 
                                  unit, port, line_link, int_phy_link, link_mon->debounce));
			SOC_IF_ERROR_RETURN(_phy_84328_intf_line_sys_update(unit, port));
			link_mon->debounce = 0;
		} else {
			link_mon->debounce++;
		}
	} else {
		link_mon->debounce = 0;
	}

	return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84328_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_84328_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *int_pc;
	uint16 line_link;
	int int_phy_link;
	phy84328_sw_rx_los_t *sw_rx_los;
	phy84328_link_mon_t *link_mon;
	phy84328_counters_t *counters;

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = 0;
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
	counters = &(COUNTERS(pc));

	/* 
	 * PCS from the internal PHY is used to determine link.
	 */
    if (int_pc != NULL) {
		SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &int_phy_link));
		*link = int_phy_link;
    } else {
		*link = 0;
	}

	sw_rx_los = &(SW_RX_LOS(pc));
	if (sw_rx_los->cur_enable) {
		uint16 new_link = 0;
		if (_phy_84328_sw_rx_los_check(unit, port, *link, &new_link) != SOC_E_NONE) {
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "84328  rx los check failed: u=%d port=%d\n"), 
                                              unit, port));
		}
		*link = new_link;
	}

	if (CUR_LINK(pc) && (*link == 0)) {
		counters->link_down++;
	}
	CUR_LINK(pc) = *link;

	link_mon = &(LINK_MON(pc));
	if (link_mon->cur_enable) {
		if (*link) {
			link_mon->debounce = 0;
		} else {
			SOC_IF_ERROR_RETURN(_phy_84328_intf_link_get(unit, port, &line_link));
			if (line_link) {
				_phy_84328_link_recover(unit, port, line_link, *link);
			} else {
				link_mon->debounce = 0;
			}
		}
	}

	if (MOD_AUTO_DETECT(pc)) {
		_phy_84328_mod_auto_detect_update(unit, port);
	}

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84328_enable_set
 * Purpose:
 *    Enable/Disable phy 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_84328_enable_set(int unit, soc_port_t port, int enable)
{
	uint16 chan_data, chan_mask;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	/* Manually power channel up/down */
    chan_data = enable ? 0 :
                (1 << PHY84328_DEV1_OPTICAL_CONFIGURATION_MAN_TXON_EN_SHIFT) |
                (1 << PHY84328_DEV1_OPTICAL_CONFIGURATION_TXOFFT_SHIFT);
    chan_mask = PHY84328_DEV1_OPTICAL_CONFIGURATION_MAN_TXON_EN_MASK |
                PHY84328_DEV1_OPTICAL_CONFIGURATION_TXOFFT_MASK;
        SOC_IF_ERROR_RETURN(
                MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_OPTICAL_CONFIGURATION,
                  chan_data, chan_mask));
	if (enable) {
        /* Let the micro settle down after powering back up */
        if (PORT_ENABLE_DELAY(pc)) {
            sal_udelay(PORT_ENABLE_DELAY(pc));
        }
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
	} else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    return SOC_E_NONE;
}

/*
 * The microcode automatically squelches tx in 1.c066[10] when  opposite side has 
 * no link. Setting 1.ca18[7] disables the microcode from squelching
 */
STATIC int
_phy_84328_micro_tx_squelch_enable(int unit, soc_port_t port, int enable)
{
	uint16 data;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
        /* 1.ca18[7] 0=micro squelching enabled, 1=micro squelching disabled */
        data = enable ? 0 : 0x80;
	    SOC_IF_ERROR_RETURN(
                MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, data, 0x80));
    return SOC_E_NONE;

}

/*
 * Squelch/unsquelch tx in 1.c066[8]
 */
STATIC int
_phy_84328_tx_squelch(int unit, soc_port_t port, phy84328_intf_side_t side, int squelch)
{
	uint16      data;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	_phy_84328_intf_side_regs_select(unit, port, side);

    /* 1.c066[8] 0=unsquelch, 1=squelch - electrical idle */
	data = squelch ? PHY84328_DEV1_ANATXACONTROL2_TX_ELECTRAL_IDLE : 0;

        SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL2,
										data,
										PHY84328_DEV1_ANATXACONTROL2_TX_ELECTRAL_IDLE));
	if (side != PHY84328_INTF_SIDE_LINE) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	}
	return SOC_E_NONE;
}

/*
 * Enable/disable transmitter
 */
STATIC int
_phy_84328_tx_enable(int unit, soc_port_t port, phy84328_intf_side_t side, int enable)
{
	uint16      data;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	_phy_84328_intf_side_regs_select(unit, port, side);		

	if (CUR_DATAPATH(pc) == PHY84328_DATAPATH_20) {
		/* 1.c06a[0] 0=tx enabled, 1=tx disabled */	
		data = enable ? 0 :
			(1 << PHY84328_DEV1_ANATXACONTROL6_DISABLE_TRANSMIT_SHIFT);
           
		    SOC_IF_ERROR_RETURN(
			    MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
					PHY84328_DEV1_ANATXACONTROL6, 
                    data, PHY84328_DEV1_ANATXACONTROL6_DISABLE_TRANSMIT_MASK));
	} else {
		/* 1.c068[13] 0=tx enabled, 1=tx disabled */
		data = enable ? 0 :  (1 << PHY84328_DEV1_ANATXACONTROL4_XFI_LOWLATENCY_FIFO_ZERO_OUT_SHIFT);
            SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(
                      unit, pc, PHY84328_DEV1_ANATXACONTROL4, data,
                      PHY84328_DEV1_ANATXACONTROL4_XFI_LOWLATENCY_FIFO_ZERO_OUT_MASK));
	}
	if (side != PHY84328_INTF_SIDE_LINE) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	}
	return SOC_E_NONE;
}

STATIC int
_phy_84328_data_path_check(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    phy84328_intf_cfg_t  *sys_intf;

    pc = EXT_PHY_SW_STATE(unit, port);
    sys_intf = &(SYS_INTF(pc));

    if (sys_intf->speed < 10000) {
        /* Speeds below 10G requires  20-bit datapath */
        if (CUR_DATAPATH(pc) != PHY84328_DATAPATH_20) {
            CUR_DATAPATH(pc) =  PHY84328_DATAPATH_20;
            SOC_IF_ERROR_RETURN(_phy_84328_intf_datapath_update(unit, port));
        }
    } else {
        /* Speeds 10G and above can use configured dpath, if !forced 20-bit */
        if (FORCE_20BIT(pc) != 0) {
            if (CUR_DATAPATH(pc) != PHY84328_DATAPATH_20) {
                CUR_DATAPATH(pc) =  PHY84328_DATAPATH_20;
                SOC_IF_ERROR_RETURN(_phy_84328_intf_datapath_update(unit, port));
            }
        } else {
            if (CUR_DATAPATH(pc) != CFG_DATAPATH(pc)) {
                CUR_DATAPATH(pc) =  CFG_DATAPATH(pc);
                SOC_IF_ERROR_RETURN(_phy_84328_intf_datapath_update(unit, port));
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84328_lb_set
 * Purpose:
 *    Put 84328 in PHY PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_84328_lb_set(int unit, soc_port_t port, int enable)
{
	int			rv = SOC_E_NONE;
	uint16		data, mask;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	SOC_IF_ERROR_RETURN(_phy_84328_sw_rx_los_pause(unit, port, enable));
	SOC_IF_ERROR_RETURN(_phy_84328_link_mon_pause(unit, port, enable));

    /* 
     * If enabling loopback
     *    disable tx line
     */
    if (enable) {
        SOC_IF_ERROR_RETURN(_phy_84328_tx_enable(unit, port, PHY84328_INTF_SIDE_LINE, FALSE  ));
    }

	/* Loopback requires 20-bit datapath */
    FORCE_20BIT(pc) &= ~(FORCE_20BIT_LB);
    FORCE_20BIT(pc) |= enable ? FORCE_20BIT_LB : 0;
    SOC_IF_ERROR_RETURN(_phy_84328_data_path_check(unit, port));

    /* 
     * If disabling loopback
     *    enable tx line
     */
    if (!enable) {
        SOC_IF_ERROR_RETURN(_phy_84328_tx_enable(unit, port, PHY84328_INTF_SIDE_LINE, TRUE   ));
    }

	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
	data = enable ? (1 << PHY84328_SYS_DEV1_ANATXACONTROL6_RLOOP_SHIFT) : 
		(1 << PHY84328_DEV1_ANATXACONTROL6_LOWLATENCY_PATH_SELECT_SHIFT);
	mask = PHY84328_SYS_DEV1_ANATXACONTROL6_RLOOP_MASK | 
		PHY84328_DEV1_ANATXACONTROL6_LOWLATENCY_PATH_SELECT_MASK;
        rv = MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_ANATXACONTROL6, data, mask);
        if (SOC_FAILURE(rv)) {
		    	LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "84328  failed setting loopback: u=%d port=%d\n"), unit, port));
	    } 
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);

 	return rv;
}

/*
 * Function:
 *    phy_84328_lb_get
 * Purpose:
 *    Get 84328 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
_phy_84328_lb_get(int unit, soc_port_t port, int *enable)
{
	int			rv = SOC_E_NONE;
	uint16		data;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	if (NULL == enable) {
		return SOC_E_PARAM;
	}
    *enable = 0xFFFF;
	/* Check if rloop in system side */
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
    
        rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_ANATXACONTROL6, &data);
        *enable &= ((data & PHY84328_SYS_DEV1_ANATXACONTROL6_RLOOP_MASK) == 
                  (1 << PHY84328_SYS_DEV1_ANATXACONTROL6_RLOOP_SHIFT));
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);

	return rv;
}

STATIC int
_phy_84328_control_preemphasis_lane_get(soc_phy_control_t type)
{
	int lane;

	switch (type) {
	case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
		lane = 0;
		break;
	case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
		lane = 1;
		break;
	case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
		lane = 2;
		break;
	case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
		lane = 3;
		break;
	default:
		lane = PHY84328_ALL_LANES;
		break;
	}
	return lane;
}

STATIC int
_phy_84328_control_preemphasis_set(int unit, soc_port_t port, 
								   soc_phy_control_t type, 
								   phy84328_intf_side_t side,
								   uint32 value)
{
	uint16		preemph_value;
	int         lane, lane_idx, lane_start, lane_end;
	uint16		tx_fir_sel, tx_fir_mask;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	lane = _phy_84328_control_preemphasis_lane_get(type);
	if (!PHY84328_SINGLE_PORT_MODE(pc) && (lane != PHY84328_ALL_LANES)) {
            if (lane != (pc->phy_id & 0x03)) {
			    return SOC_E_PARAM;
		    }
	}
    preemph_value = (uint16) (value & 0xffff);
	/*
	 * Value format:
	 * bit 15: if set, enable forced preemphasis control, otherwise, auto configured - see 1.c192[3:0] 
	 * bit 13:10: postcursor_tap   - see 1.c067[7:4]
	 * bit 08:04: main_tap         - see 1.c066[15:11]
	 * bit 02:00: precursor_tap    - see 1.c067[2:0]
	 */ 

	/*
	 * Single mode: if lane specified, enable only that lane, otherwise enable all lanes
	 * Quad mode: if specified lane matches lane for the channel, set the lane
	 */
	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		tx_fir_sel = (lane == PHY84328_ALL_LANES) ? 0xf : (1 << lane);
		tx_fir_mask = (lane == PHY84328_ALL_LANES) ?  PHY84328_DEV1_XFI_AFE_CTL2_TX_FIR_SEL_MASK :
			(1 << lane);
	} else {
		tx_fir_sel = 1 << (pc->phy_id & 0x03);
		tx_fir_mask = 1 << (pc->phy_id & 0x03);
	} 

	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		lane_start = (lane == PHY84328_ALL_LANES) ? 0 : lane;
		lane_end   = (lane == PHY84328_ALL_LANES) ? PHY84328_NUM_LANES : lane + 1;
	}
    else {
		/* Make sure specified lane matches lane for the channel */
		if (lane != PHY84328_ALL_LANES) {
			if (lane != (pc->phy_id & 0x03)) {
				return SOC_E_PARAM;
			}
		}
		lane_start = 0;
		lane_end   = 1;
	}

	if (side == PHY84328_INTF_SIDE_SYS) {
		PHY_84328_MICRO_PAUSE(unit, port, "preemphasis set");
	}

	if (!PHY84328_SINGLE_PORT_MODE(pc)) {
		_phy_84328_intf_side_regs_select(unit, port, side);
	}
	/* Update all requested lanes */
	for (lane_idx = lane_start; lane_idx < lane_end; lane_idx++) {
		if (PHY84328_SINGLE_PORT_MODE(pc)) {
			/* Select the lane and side to write */
			SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, lane_idx));
		} 
		/* Enable manual preemphasis */
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_XFI_AFE_CTL2,
											tx_fir_sel, tx_fir_mask));

		/* Main tap */
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL2,
											PHY84328_PREEMPH_MAIN_TAP_GET(preemph_value) << 
											PHY84328_DEV1_ANATXACONTROL2_MAIN_TAP_SHIFT,
											PHY84328_DEV1_ANATXACONTROL2_MAIN_TAP_MASK));
		
		/* Pre and post cursor */
		SOC_IF_ERROR_RETURN(
			MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL3,
											(PHY84328_PREEMPH_POST_TAP_GET(preemph_value) << 
											 PHY84328_DEV1_ANATXACONTROL3_POSTCURSOR_TAP_SHIFT) |
											(PHY84328_PREEMPH_PRE_TAP_GET(preemph_value) << 
											 PHY84328_DEV1_ANATXACONTROL3_PRECURSOR_TAP_SHIFT),
											(PHY84328_DEV1_ANATXACONTROL3_POSTCURSOR_TAP_MASK | 
											 PHY84328_DEV1_ANATXACONTROL3_PRECURSOR_TAP_MASK)));
	}

	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, PHY84328_ALL_LANES));
	}
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	if (side == PHY84328_INTF_SIDE_SYS) {
		PHY_84328_MICRO_RESUME(unit, port);
	}

	return SOC_E_NONE;
}

STATIC int
_phy_84328_control_preemphasis_get(int unit, soc_port_t port, soc_phy_control_t type, 
								   phy84328_intf_side_t side, uint32 *value)
{
	int			lane, lane_idx;
	uint16		forced;
	uint16		data;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	lane = _phy_84328_control_preemphasis_lane_get(type);
	lane_idx = (lane == PHY84328_ALL_LANES) ? 0 : lane;

	if (side == PHY84328_INTF_SIDE_SYS) {
		PHY_84328_MICRO_PAUSE(unit, port, "preemphasis get");
	}

	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, lane_idx));
	} else {
		_phy_84328_intf_side_regs_select(unit, port, side);
	}

	/*
	 * Value format:
	 * bit 15: if set, enable forced preemphasis control, otherwise, auto configured - see 1.c192[3:0] 
	 * bit 13:10: postcursor_tap   - see 1.c067[7:4]
	 * bit 08:04: main_tap         - see 1.c066[15:11]
	 * bit 02:00: precursor_tap    - see 1.c067[2:0]
	 */ 
	*value = 0;
	SOC_IF_ERROR_RETURN(
		READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_XFI_AFE_CTL2, &data));
	data &= 0xf;
	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		if (lane == PHY84328_ALL_LANES) {
			forced = (data == 0xf) ? 1 : 0;
		} else {
			forced = (data & (1 << lane)) ? 1 : 0;
		}
	} else {
		forced = (data & (1 << (pc->phy_id & 0x03))) ? 1 : 0;
	}
	*value = PHY84328_PREEMPH_FORCE_SET(forced);
    
    SOC_IF_ERROR_RETURN(
		READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL2, &data));
	*value |= PHY84328_PREEMPH_MAIN_TAP_SET((data & PHY84328_DEV1_ANATXACONTROL2_MAIN_TAP_MASK) >>
											PHY84328_DEV1_ANATXACONTROL2_MAIN_TAP_SHIFT);
	
	SOC_IF_ERROR_RETURN(
		READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL3, &data));
	*value |= PHY84328_PREEMPH_POST_TAP_SET((data & PHY84328_DEV1_ANATXACONTROL3_POSTCURSOR_TAP_MASK) >>
											PHY84328_DEV1_ANATXACONTROL3_POSTCURSOR_TAP_SHIFT);
	*value |= PHY84328_PREEMPH_PRE_TAP_SET((data & PHY84328_DEV1_ANATXACONTROL3_PRECURSOR_TAP_MASK) >>
											PHY84328_DEV1_ANATXACONTROL3_PRECURSOR_TAP_SHIFT);
    if (PHY84328_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, PHY84328_ALL_LANES));
    }
    _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	if (side == PHY84328_INTF_SIDE_SYS) {
		PHY_84328_MICRO_RESUME(unit, port);
	}

    return SOC_E_NONE;
}
 
STATIC int
_phy_84328_txmode_manual_set(int unit, soc_port_t port, phy84328_intf_side_t side, int set)
{
	uint16		data, mask;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	phy84328_intf_side_t cur_side;

	mask = (side == PHY84328_INTF_SIDE_LINE) ? (1 << 4) : (1 << 3);
	data = set ? mask : 0;

    cur_side = _phy_84328_intf_side_regs_get(unit, port);
    if (cur_side == PHY84328_INTF_SIDE_SYS) {
        _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
    }
        SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0,
														data, mask));
    if (cur_side == PHY84328_INTF_SIDE_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
	}
	return SOC_E_NONE;
}

STATIC int
_phy_84328_control_tx_driver_set(int unit, soc_port_t port, soc_phy_control_t type, 
								 phy84328_intf_side_t side, uint32 value)
{
	uint16      data, mask;
	int         lane, lane_idx, lane_start, lane_end;
	int			idriver = 0;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	switch (type) {
	case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
		lane = 0;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		idriver = 1;
		break;
	case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
		lane = 1;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		idriver = 1;
		break;
	case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
		lane = 2;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		idriver = 1;
		break;
	case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
		lane = 3;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		idriver = 1;
		break;
	case SOC_PHY_CONTROL_DRIVER_CURRENT:
		lane = PHY84328_ALL_LANES;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		idriver = 1;
		break;

	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
		lane = 0;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
		lane = 1;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
		lane = 2;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
		lane = 3;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
		lane = PHY84328_ALL_LANES;
		data = (uint16) (value & 0xf);
		data = data << PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	default:
		return SOC_E_PARAM;
	}

	/*                                                                                                
	 * Qualify lane                                                                                   
	 * - single mode: if lane specified, enable only that lane in Single_PMD_Ctrl 1.ca86[5:4],
	 *                otherwise program all lanes        
	 * - quad mode: make sure the lane corresponds to the channel, and program the lane
     * - 20G mode: if lane specified, enable only that lane, otherwise program all lanes
 	 */

	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		lane_start = (lane == PHY84328_ALL_LANES) ? 0 : lane;
		lane_end   = (lane == PHY84328_ALL_LANES) ? PHY84328_NUM_LANES : lane + 1;
	}
    else {
		/* Make sure specified lane matches lane for the channel */
		if (lane != PHY84328_ALL_LANES) {
			if (lane != (pc->phy_id & 0x03)) {
				return SOC_E_PARAM;
			}
		}
		lane_start = 0;
		lane_end   = 1;
	}

	/* 
	 * Driver current (idriver) and tx_mode are updated by the firmware by default.
	 * If driver current is programmed with value != 0, 
	 * the firmware will no longer update driver current and tx_mode.
	 * Setting driver current to 0 (re)enables firmware update of both driver current and
	 * tx_mode
	 */
	if (idriver) {
		if (data & PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK) {
			SOC_IF_ERROR_RETURN(_phy_84328_txmode_manual_set(unit, port, side, TRUE));
		} else {
			SOC_IF_ERROR_RETURN(_phy_84328_txmode_manual_set(unit, port, side, FALSE));
		}
	}
	if (side == PHY84328_INTF_SIDE_SYS) {
		PHY_84328_MICRO_PAUSE(unit, port, "tx driver set");
	}

	if (!PHY84328_SINGLE_PORT_MODE(pc)) {
        _phy_84328_intf_side_regs_select(unit, port, side);
    }
	/* Update all requested lanes */
	for (lane_idx = lane_start; lane_idx < lane_end; lane_idx++) {
		if (PHY84328_SINGLE_PORT_MODE(pc)) {
			/* Select the lane and side to write */
			SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, lane));
		}
		SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
															PHY84328_DEV1_ANATXACONTROL1,
															data, mask));
	}

	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, PHY84328_ALL_LANES));
	}
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	if (side == PHY84328_INTF_SIDE_SYS) {
		PHY_84328_MICRO_RESUME(unit, port);
	}

	return SOC_E_NONE;
}

STATIC int
_phy_84328_control_tx_driver_get(int unit, soc_port_t port, soc_phy_control_t type, 
								 phy84328_intf_side_t side, uint32 *value)
{
	int         rv;
	int         lane = 0;
	uint16      data, shift, mask;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	switch (type) {
	case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
		lane = 0;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
		lane = 1;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
		lane = 2;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
		lane = 3;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_DRIVER_CURRENT:
		/* Return value for lane 0 */
		lane = 0;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_SHIFT;
		mask = PHY84328_SYS_DEV1_ANATXACONTROL1_IDRIVER_MASK;
		break;

	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
		lane = 0;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
		lane = 1;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
		lane = 2;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
		lane = 3;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
		/* Return value for lane 0 */
		lane = 0;
		shift = PHY84328_SYS_DEV1_ANATXACONTROL1_IPREDRIVER_SHIFT;
		mask = PHY84328_DEV1_ANATXACONTROL1_IPREDRIVER_MASK;
		break;
	default:
		return SOC_E_PARAM;
	}

	if (side == PHY84328_INTF_SIDE_SYS) {	
		PHY_84328_MICRO_PAUSE(unit, port, "tx driver get");
	}

	/* Select the lane and side to access */
	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, lane)); 
	} else {
		_phy_84328_intf_side_regs_select(unit, port, side);
	}
	rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL1, &data);
	if (SOC_FAILURE(rv)) {
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328  tx driver get failed: u=%d p=%d\n"), unit, port));
		*value = 0xff;
	} else {
		*value = (data & mask) >> shift;
	}
	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		/* Restore back to channel 0 */
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, PHY84328_ALL_LANES));
	}
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);

	if (side == PHY84328_INTF_SIDE_SYS) {
		PHY_84328_MICRO_RESUME(unit, port);
	}
    return SOC_E_NONE;
}

STATIC int
_phy_84328_control_edc_mode_set(int unit, soc_port_t port, uint32 value)
{
    return SOC_E_UNAVAIL;
}

/*
 * enable/disable syncE recoverd clock for 10G/40G modes.
 */
STATIC int
_phy_84328_control_recovery_clock_set(int unit, soc_port_t port, int enable)
{
    return SOC_E_UNAVAIL;
}

/*
 * set the frequency of the syncE recoverd clock.
 */
STATIC int
_phy_84328_control_recovery_clock_freq_set(int unit, soc_port_t port, int freq)
{
    return SOC_E_UNAVAIL;
}

STATIC int
_phy_84328_control_prbs_tx_invert_data_set(int unit, soc_port_t port, int invert)
{
	uint16		data;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	data = invert ? (1 << PHY84328_SYS_DEV1_ANATXACONTROL7_NEW_PRBS_INV_SHIFT) : 0;
        /* tx prbs invert in anaTxAControl7 1.c06b */
        SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                    PHY84328_DEV1_ANATXACONTROL7,	data,
					PHY84328_SYS_DEV1_ANATXACONTROL7_NEW_PRBS_INV_MASK));

    return SOC_E_NONE;
}

STATIC int
_phy_84328_control_prbs_rx_invert_data_set(int unit, soc_port_t port, int invert)
{
	uint16		data;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    

	data = invert ? (1 << PHY84328_SYS_DEV1_ANARXCONTROL2_PRBS_INV_RX_R_SHIFT) : 0;
	    /* rx prbs invert in anaRxControl2 1.c0b6 */
	    SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_ANARXCONTROL2, data,
										PHY84328_SYS_DEV1_ANARXCONTROL2_PRBS_INV_RX_R_MASK));
    return SOC_E_NONE;
}

STATIC int
_phy_84328_control_prbs_polynomial_set(int unit, soc_port_t port, int poly_ctrl, int tx) 
{
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "84328  prbs polynomial set: u=%d p=%d polynomial=%d\n"),
                             unit, port, poly_ctrl));

	if (poly_ctrl < 0 || poly_ctrl > 3) {
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328  prbs invalid polynomial: u=%d p=%d polynomial=%d\n"),
                                      unit, port, poly_ctrl));
		return SOC_E_PARAM;
	}
	/* poly_ctrl: 0==prbs7, 1==prbs15, 2==prbs23, 3==prbs=31 */
	if (tx) {
            /* tx prbs polynomial in anaTxAControl7 1.c06b */
		    SOC_IF_ERROR_RETURN(
			    MODIFY_PHY84328_MMF_PMA_PMD_REG(
				unit, pc, 
				PHY84328_DEV1_ANATXACONTROL7,
				(poly_ctrl << PHY84328_DEV1_ANATXACONTROL7_LANE_PRBS_ORDER_NEW_B1_0_SHIFT),
				PHY84328_SYS_DEV1_ANATXACONTROL7_LANE_PRBS_ORDER_NEW_B1_0_MASK));
	} else {
            /* rx prbs polynomial in anaRxControl2 1.c0b6 */
		    SOC_IF_ERROR_RETURN(
			    MODIFY_PHY84328_MMF_PMA_PMD_REG(
				unit, pc, 
				PHY84328_SYS_DEV1_ANARXCONTROL2,
				(poly_ctrl << PHY84328_DEV1_ANARXCONTROL2_PRBS_ORDER_RX_R_SHIFT),
				PHY84328_SYS_DEV1_ANARXCONTROL2_PRBS_ORDER_RX_R_MASK));
	}
    return SOC_E_NONE;
}

STATIC int
_phy_84328_sw_rx_los_pause(int unit, soc_port_t port, int enable)
{
	phy_ctrl_t *pc;
	phy84328_sw_rx_los_t *sw_rx_los;

#ifdef BCM_WARM_BOOT_SUPPORT
    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }
#endif

	pc = EXT_PHY_SW_STATE(unit, port);
	sw_rx_los = &(SW_RX_LOS(pc));

	/* Nothing to do if RX LOS not configured */
	if (!sw_rx_los->cfg_enable) {
		return SOC_E_NONE;
	}

	sw_rx_los->cur_enable = !enable;

	return SOC_E_NONE;
}

STATIC int
_phy_84328_link_mon_pause(int unit, soc_port_t port, int enable)
{
	phy_ctrl_t *pc;
	phy84328_link_mon_t *link_mon;
	
	pc = EXT_PHY_SW_STATE(unit, port);
	link_mon = &(LINK_MON(pc));

	/* Nothing to do if LINK_MON not configured */
	if (!link_mon->cfg_enable) {
		return SOC_E_NONE;
	}

	link_mon->cur_enable = !enable;
	return SOC_E_NONE;
}


STATIC int
_phy_84328_control_prbs_enable_set(int unit, soc_port_t port, int enable)
{
	uint16	    tx_data, rx_data;
	uint16	    tx_mask, rx_mask;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_sw_rx_los_pause(unit, port, enable));
	SOC_IF_ERROR_RETURN(_phy_84328_link_mon_pause(unit, port, enable));

	/* Mask enable bits */
	tx_mask = (PHY84328_DEV1_ANATXACONTROL7_SELECT_NEW_PRBS_MASK | 
			  PHY84328_DEV1_ANATXACONTROL7_NEW_PRBS_EN_MASK);
	rx_mask = (PHY84328_DEV1_ANARXCONTROL2_PRBS_SEL_RX_R_MASK |
			  PHY84328_DEV1_ANARXCONTROL2_PRBS_EN_RX_R_MASK);

	/* 
	 * PRBS done with 20-bit datapath 
	 */
	if (enable) {
		tx_data = (1 << PHY84328_DEV1_ANATXACONTROL7_SELECT_NEW_PRBS_SHIFT) |
			(1 << PHY84328_DEV1_ANATXACONTROL7_NEW_PRBS_EN_SHIFT);
		rx_data = (1 << PHY84328_DEV1_ANARXCONTROL2_PRBS_EN_RX_R_SHIFT) |  
			(1 << PHY84328_DEV1_ANARXCONTROL2_PRBS_SEL_RX_R_SHIFT);
		if (CFG_DATAPATH(pc) != PHY84328_DATAPATH_20) {
			CUR_DATAPATH(pc) = PHY84328_DATAPATH_20;
		}
	} else {
		tx_data = 0;
		rx_data = 0;
		if (CFG_DATAPATH(pc) != PHY84328_DATAPATH_20) {
			CUR_DATAPATH(pc) = CFG_DATAPATH(pc);
		}
	}

    FORCE_20BIT(pc) &= ~(FORCE_20BIT_LB);
    FORCE_20BIT(pc) |= enable ? FORCE_20BIT_LB : 0;
    /* Update datapath */
    SOC_IF_ERROR_RETURN(_phy_84328_intf_datapath_update(unit, port));
    
        /* Enable tx prbs in anaTxAcontrol7 1.c06b[6..5] */
	    SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
														PHY84328_DEV1_ANATXACONTROL7,
														tx_data, tx_mask));
    	/* Enable rx prbs in anaRxControl2 1.c0b6[6..5] */
        SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
														PHY84328_SYS_DEV1_ANARXCONTROL2,
														rx_data, rx_mask));
	/* 
	 * If enabling prbs
	 *    disable micro tx squelch updates
	 *    unsquelch tx system
	 * else
	 *    enable micro tx squelch updates
	 */
	SOC_IF_ERROR_RETURN(_phy_84328_micro_tx_squelch_enable(unit, port, !enable));
	if (enable) {
		SOC_IF_ERROR_RETURN(_phy_84328_tx_squelch(unit, port, PHY84328_INTF_SIDE_SYS, 0));
	}

    return SOC_E_NONE;
}

STATIC int
_phy_84328_control_prbs_enable_get(int unit, soc_port_t port, uint32 *value)
{
	uint16	    tx_data, rx_data;
	uint16	    tx_en, rx_en;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	/* Enable tx prbs in anaTxAcontrol7 1.c06b[6..5] */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
													  PHY84328_DEV1_ANATXACONTROL7,
													  &tx_data));
	/* Enable rx prbs in anaRxControl2 1.c0b6[6..5] */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
													  PHY84328_SYS_DEV1_ANARXCONTROL2,
													  &rx_data));

	/* Mask enable bits */
	tx_data &= (PHY84328_DEV1_ANATXACONTROL7_SELECT_NEW_PRBS_MASK | 
				PHY84328_DEV1_ANATXACONTROL7_NEW_PRBS_EN_MASK);
	rx_data &= (PHY84328_DEV1_ANARXCONTROL2_PRBS_SEL_RX_R_MASK |
				PHY84328_DEV1_ANARXCONTROL2_PRBS_EN_RX_R_MASK);

	tx_en = (tx_data & 
			 ((1 << PHY84328_DEV1_ANATXACONTROL7_SELECT_NEW_PRBS_SHIFT) |
			  (1 << PHY84328_DEV1_ANATXACONTROL7_NEW_PRBS_EN_SHIFT))) ==
		((1 << PHY84328_DEV1_ANATXACONTROL7_SELECT_NEW_PRBS_SHIFT) |
		 (1 << PHY84328_DEV1_ANATXACONTROL7_NEW_PRBS_EN_SHIFT));

	rx_en = (rx_data & 
			 ((1 << PHY84328_DEV1_ANARXCONTROL2_PRBS_EN_RX_R_SHIFT) |  
			  (1 << PHY84328_DEV1_ANARXCONTROL2_PRBS_SEL_RX_R_SHIFT))) ==
		((1 << PHY84328_DEV1_ANARXCONTROL2_PRBS_EN_RX_R_SHIFT) |  
		 (1 << PHY84328_DEV1_ANARXCONTROL2_PRBS_SEL_RX_R_SHIFT));

	*value = tx_en && rx_en;

    return SOC_E_NONE;
}

STATIC int
_phy_84328_control_prbs_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
	uint16		data;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	*value = 0;
	/* tx prbs polynomial in anaTxAControl7 1.c06b */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
													  PHY84328_DEV1_ANATXACONTROL7,
													  &data));
	data &= PHY84328_SYS_DEV1_ANATXACONTROL7_LANE_PRBS_ORDER_NEW_B1_0_MASK;
	*value = data >> PHY84328_DEV1_ANATXACONTROL7_LANE_PRBS_ORDER_NEW_B1_0_SHIFT;

    return SOC_E_NONE;
}

STATIC int
_phy_84328_control_prbs_tx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
	uint16		data;
	phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	*value = 0;
	/* tx invert in anaTxAControl7 1.c06b */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
													  PHY84328_DEV1_ANATXACONTROL7,	&data));
	*value = (data & (1 << PHY84328_SYS_DEV1_ANATXACONTROL7_NEW_PRBS_INV_SHIFT)) ==
		(1 << PHY84328_SYS_DEV1_ANATXACONTROL7_NEW_PRBS_INV_SHIFT);

	return SOC_E_NONE;
}

STATIC int
_phy_84328_control_prbs_rx_status_get(int unit, soc_port_t port, phy84328_intf_side_t side, uint32 *value)
{
    uint16 data, tmp, lane_idx, lanes;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    if (PHY84328_SINGLE_PORT_MODE(pc)) {
        lanes = PHY84328_NUM_LANES;
    }
    else {
        lanes = 1;
    }

    *value = 0;

    for (lane_idx = 0; lane_idx < lanes; lane_idx++) {
        if (PHY84328_SINGLE_PORT_MODE(pc)) {
            /* Select the lane and side to access */
            SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, 
                                           side, lane_idx));
        }
        /* save anaRxControl 1.c0b1 in tmp*/
        SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                 PHY84328_SYS_DEV1_ANARXCONTROL, &tmp));

        /* Select PRBS status in anaRxControl 1.c0b1 */	
        SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                   PHY84328_SYS_DEV1_ANARXCONTROL, 7,
                                   PHY84328_DEV1_ANARXCONTROL_STATUS_SEL_MASK));

        /* Check for rx PRBS lock in anaRxStatus 1.c0b0 */
        SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                 PHY84328_DEV1_ANARXSTATUS, &data));

        /* Restore the old value into  anaRxControl 1.c0b1 */	
        SOC_IF_ERROR_RETURN(WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                   PHY84328_SYS_DEV1_ANARXCONTROL, tmp));

        if (data == PHY84328_DEV1_ANARXSTATUS_RXSTATUS_RX_PRBS_LOCK) {
            *value |= 0;		/* locked and no errors */
        } else if ((data & PHY84328_DEV1_ANARXSTATUS_RXSTATUS_RX_PRBS_LOCK) !=
                           PHY84328_DEV1_ANARXSTATUS_RXSTATUS_RX_PRBS_LOCK) {
            *value |= -1;	/* no lock */
        } else {
            /* locked with errors */
            *value |= data & PHY84328_DEV1_ANARXSTATUS_RXSTATUS_RX_PRBS_ERR_MASK;
        }
    }

    /* Restore to default single port register access */
    if (PHY84328_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port,
                             PHY84328_INTF_SIDE_LINE, PHY84328_ALL_LANES));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84328_control_edc_mode_get(int unit, soc_port_t port, uint32 *value)
{
    return SOC_E_UNAVAIL;
}

STATIC int
_phy_84328_remote_loopback_set(int unit, soc_port_t port, int intf, uint32 enable)
{    
	phy_ctrl_t  *pc;	/* PHY software state */
	uint16		data, mask;
    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * If enabling remote loopback
     *     disable tx system side
     */
    if (enable) {
        SOC_IF_ERROR_RETURN(_phy_84328_tx_enable(unit, port, PHY84328_INTF_SIDE_SYS, FALSE  ));
    }
	/* Loopback requires 20-bit datapath */
    FORCE_20BIT(pc) &= ~(FORCE_20BIT_LB);
    FORCE_20BIT(pc) |= enable ? FORCE_20BIT_LB : 0;
    SOC_IF_ERROR_RETURN(_phy_84328_data_path_check(unit, port));

    /* 
     * If disabling loopback
     *    enable tx system side
     */
    if (!enable) {
        SOC_IF_ERROR_RETURN(_phy_84328_tx_enable(unit, port, PHY84328_INTF_SIDE_SYS, TRUE   ));
    }
    data = enable ? (1 << PHY84328_SYS_DEV1_ANATXACONTROL6_RLOOP_SHIFT) : 0;
	mask = PHY84328_SYS_DEV1_ANATXACONTROL6_RLOOP_MASK;
 		SOC_IF_ERROR_RETURN(
                MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_ANATXACONTROL6, data, mask));
	return SOC_E_NONE;
}

STATIC int
_phy_84328_remote_loopback_get(int unit, soc_port_t port, int intf, uint32 *enable)
{    
	int			rv;
	phy_ctrl_t  *pc;	/* PHY software state */
	uint16		data;

    pc = EXT_PHY_SW_STATE(unit, port);

	rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_ANATXACONTROL6, &data);
	*enable =  (data & PHY84328_SYS_DEV1_ANATXACONTROL6_RLOOP_MASK) == 
		PHY84328_SYS_DEV1_ANATXACONTROL6_RLOOP_MASK;

	return rv;
}

STATIC int
_phy_84328_debug_info(int unit, soc_port_t port)
{
	phy_ctrl_t			*pc;     
	phy_ctrl_t			*int_pc; 
	phy84328_intf_cfg_t *intf;
	phy84328_intf_cfg_t *line_intf;
    phy84328_intf_cfg_t *sys_intf;
	phy84328_counters_t *counters;
	phy84328_sw_rx_los_t *sw_rx_los;
	phy84328_link_mon_t *link_mon;
	phy84328_intf_side_t side, saved_side;
    char                *rx_los_sts;
	soc_port_if_t		iif;
	int					lane;
	int					ian, ian_done, ilink, ispeed;
	uint16				link_sts[PHY84328_NUM_LANES];
	uint16				pol_sts[PHY84328_NUM_LANES];
	uint16				pol_inv_sts[PHY84328_NUM_LANES];
	uint16				tx_drv[PHY84328_NUM_LANES];
	uint16				preemph_main[PHY84328_NUM_LANES];
	uint16				preemph_pre_post[PHY84328_NUM_LANES];
	uint16				preemph_manual[PHY84328_NUM_LANES];
	uint16				data0, data1, data2, data3, data4, data5, data6, data7;
	uint32				die_temp;
	uint32				primary_port, offset;
    uint16              i = 0;
	pc = EXT_PHY_SW_STATE(unit, port);
	int_pc = INT_PHY_SW_STATE(unit, port);
	
	SOC_IF_ERROR_RETURN(phy_84328_control_port_get(unit, port, SOC_PHY_CONTROL_PORT_PRIMARY, 
											  &primary_port));

	SOC_IF_ERROR_RETURN(phy_84328_control_port_get(unit, port, SOC_PHY_CONTROL_PORT_OFFSET,
											  &offset));

	/* Access line side registers */
    saved_side = _phy_84328_intf_side_regs_get(unit, port);
    if (saved_side == PHY84328_INTF_SIDE_SYS) {
        _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
    }

	/* firware rev: 1.c1f0  */
	SOC_IF_ERROR_RETURN
		(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
									   PHY84328_DEV1_VERSION, &data0));

	/* micro enabled: 1.ca18 */
	SOC_IF_ERROR_RETURN
		(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
									   PHY84328_DEV1_GP_REG_0, &data1));

	/* die temperature in 1.c1fd */
	SOC_IF_ERROR_RETURN
		(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
									   PHY84328_DEV1_TEMPERATURE, &data2));
	die_temp = 418 - (((data2 & PHY84328_DEV1_TEMPERATURE_TEMPERATURE_MASK) * 5556) / 10000);

	sw_rx_los = &(SW_RX_LOS(pc));
	link_mon = &(LINK_MON(pc));
    rx_los_sts = "dis";
    if (sw_rx_los->cur_enable) {
        rx_los_sts = "sw";
    } else if (FW_RX_LOS(pc)) {
        rx_los_sts = "fw";
    }
        
	LOG_CLI((BSL_META_U(unit,
                            "Port %-2d: rev=%d:%d, chiprev=%04x, "
                            "micro ver(1.%04x)=%04x, micro(1.%04x)=%04x, die temp=%d(C)%s\n"
                            "     pri:offs=%d:%d, mdio=0x%x, datapath=%d, "
                            "rxLOS=%s, mod auto=%s, sync=%s, link mon=%s\n"),
                 port, phy84328_maj_rev, phy84328_min_rev,
                 DEVREV(pc),
                 PHY84328_DEV1_VERSION, data0,
                 PHY84328_DEV1_GP_REG_0, data1,
                 die_temp,
                 data2 & PHY84328_DEV1_TEMPERATURE_FORCE_TEMPERATURE_MASK ? 
                 "(forced)" : "",
                 primary_port, offset, pc->phy_id, 
                 CUR_DATAPATH(pc) == PHY84328_DATAPATH_20 ? 20 : 4,
                 rx_los_sts,
                 MOD_AUTO_DETECT(pc) ? "en" : "dis",
                 SYNC_INIT(pc) ? "en" : "dis",
                 link_mon->cur_enable ? "en" : "dis"));

	/* Single PMD control: 1.ca86 */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
													  PHY84328_DEV1_SINGLE_PMD_CTRL, &data0));

	/* Broadcast control: 1.c8fe */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
													  PHY84328_DEV1_BROADCAST_CTRL, &data1));

	/* AnaPllStatus 1.c050 */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
													  PHY84328_DEV1_ANAPLLASTATUS, &data2));

	/* AN 7.0000 */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_AN_REG(unit, pc, 
												 PHY84328_DEV7_AN_CONTROL_REGISTER, &data3));

	/* AN done 1.0097 */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
													  PHY84328_SYS_DEV1_BASE_R_PMD_STATUS_REGISTER,
													  &data4));

	/* Regulator control 1.c850 */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
													  PHY84328_DEV1_REGUALTOR_CTRL, &data5));

	/* Global intf type/speed - driver csr: 1.c841 */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
													  PHY84328_DEV1_GP_REGISTER_1, &data6));

	/* Micro csr 1.c843 */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
													  PHY84328_DEV1_GP_REGISTER_3, &data7));

	LOG_CLI((BSL_META_U(unit,
                            "     PMD(1.%04x)=%04x, bcctrl(1.%04x)=%04x, "
                            "pll(1.%04x)=%04x, an(7.%04x/1.%04x)=%04x/%04x\n"
                            "     regulator(1.%04x)=%04x, drvcsr(1.%04x)=%04x, "
                            "ucsr(1.%04x)=%04x\n"),
                 PHY84328_DEV1_SINGLE_PMD_CTRL, data0,
                 PHY84328_DEV1_BROADCAST_CTRL, data1, 
                 PHY84328_DEV1_ANAPLLASTATUS, data2,
                 PHY84328_DEV7_AN_CONTROL_REGISTER, 
                 PHY84328_SYS_DEV1_BASE_R_PMD_STATUS_REGISTER, data3, data4,
                 PHY84328_DEV1_REGUALTOR_CTRL, data5,
                 PHY84328_DEV1_GP_REGISTER_1, data6,
                 PHY84328_DEV1_GP_REGISTER_3, data7));

	counters = &(COUNTERS(pc));
	LOG_CLI((BSL_META_U(unit,
                            " Counters: link down=%d, intf updates=%d, "
                            "swRxLOS restarts=%d, no cdr=%d, micro nopause=%d\n"
                            "           retry internal=%d, intf chk(speed=%d, "
                            "type=%d, sel=%d)\n"),
                 counters->link_down, counters->intf_updates,
                 sw_rx_los->restarts, 
                 counters->no_cdr, counters->micro_nopause,
                 counters->retry_serdes_link,
                 counters->speed_chk_err, counters->intf_chk_err,
                 counters->side_sel_err));

	/* Interfaces: software, hardware, internal serdes */
	line_intf = &(LINE_INTF(pc));
    sys_intf = &(SYS_INTF(pc));

	for (side = PHY84328_INTF_SIDE_LINE; side <= PHY84328_INTF_SIDE_SYS; side++) {

		_phy_84328_intf_side_regs_select(unit, port, side);

		/* control: 1.0000 */
		SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
										   PHY84328_DEV1_PMD_CONTROL_REGISTER, &data1));
		
		/* PMD type: 1.0007 */
		SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
										  PHY84328_DEV1_PMD_CONTROL_2_REGISTER, &data2));

		/* Loopback 1.c06a */
		SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
														  PHY84328_SYS_DEV1_ANATXACONTROL6, &data3));

		/* Tx disable 1.c066 */
		SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_ANATXACONTROL2, 
														  &data4));
		
		/* Tx disable 1.c06a */
		SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
														  PHY84328_DEV1_ANATXACONTROL6,
														  &data5));
		if (PHY84328_SINGLE_PORT_MODE(pc)) {

			if (side == PHY84328_INTF_SIDE_SYS) {
				PHY_84328_MICRO_PAUSE(unit, port, "");
			}
			for (lane = 0; lane < PHY84328_NUM_LANES; lane++) {

				/* Select the lane and side to write */
                SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, lane));

				/* Link: 1.c0b0 */
				SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
																  PHY84328_DEV1_ANARXSTATUS, 
																  &(link_sts[lane])));

				if (CUR_DATAPATH(pc) == PHY84328_DATAPATH_20) {
					/* Polarity setting: 1.c061 */
					SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
																	  PHY84328_SYS_DEV1_ANATXACONTROL,
																	  &(pol_sts[lane])));
				} else {
					/* Polarity setting: 1.c068 */
					SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
																	  PHY84328_SYS_DEV1_ANATXACONTROL4,
																	  &(pol_sts[lane])));
				}

				if (side == PHY84328_INTF_SIDE_LINE) {
					/* Polarity inversion setting: 1.c0ba[3:2] in the line side */
					SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
																	  PHY84328_DEV1_GP_REGISTER_RX_INV,
																	  &(pol_inv_sts[lane])));
				}

				/* TX driver: 1.c065 */
				SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
																  PHY84328_SYS_DEV1_ANATXACONTROL1,
																  &(tx_drv[lane])));
				/* Preemphasis manual: 1.c192[15] */
				SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
																  PHY84328_SYS_DEV1_XFI_AFE_CTL2,
																  &(preemph_manual[lane])));

				/* Preemphasis main: 1.c066[15:11] */
				SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
																  PHY84328_SYS_DEV1_ANATXACONTROL2,
																  &(preemph_main[lane])));

				/* Preemphasis pre post: 1.c067 */
				SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
																  PHY84328_SYS_DEV1_ANATXACONTROL3,
																  &(preemph_pre_post[lane])));
			}
			/* Back to default channel selection through lane 0 */
			SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, PHY84328_ALL_LANES));
			_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
			if (side == PHY84328_INTF_SIDE_SYS) {
				PHY_84328_MICRO_RESUME(unit, port);
			}

		} else {
			    /* Link: 1.c0b0 */
			    SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
															  PHY84328_DEV1_ANARXSTATUS, 
															  &(link_sts[i])));
			    if (CUR_DATAPATH(pc) == PHY84328_DATAPATH_20) {
				    /* Polarity setting: 1.c061 */
				    SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
																  PHY84328_DEV1_ANATXACONTROL, 
																  &(pol_sts[i])));
			    } else {
				    /* Polarity setting: 1.c068 */
				    SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
																  PHY84328_DEV1_ANATXACONTROL4, 
																  &(pol_sts[i])));
			    }

			    if (side == PHY84328_INTF_SIDE_LINE) {
				    /* Polarity inversion setting: 1.c0ba[3:2] in the line side */
				    SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
																  PHY84328_DEV1_GP_REGISTER_RX_INV,
																  &(pol_inv_sts[i])));
			    }

			    /* TX driver: 1.c065 */
			    SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
															  PHY84328_SYS_DEV1_ANATXACONTROL1,
															  &(tx_drv[i])));
			    /* Preemphasis manual: 1.c192[15] */
			    SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
															  PHY84328_SYS_DEV1_XFI_AFE_CTL2,
															  &(preemph_manual[i])));
			
			    /* Preemphasis main: 1.c066[15:11] */
			    SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
															  PHY84328_SYS_DEV1_ANATXACONTROL2,
															  &(preemph_main[i])));
			
			    /* Preemphasis pre post: 1.c067 */
			    SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
															  PHY84328_SYS_DEV1_ANATXACONTROL3,
															  &(preemph_pre_post[i])));
		}

		intf = (side == PHY84328_INTF_SIDE_LINE) ? line_intf : sys_intf;			

		if (PHY84328_SINGLE_PORT_MODE(pc)) {
			LOG_CLI((BSL_META_U(unit,
                                            " %s: type=%s, speed=%d, forced cl72=%d, "
                                            "mode(1.%04x)=%04x\n"
                                            "           pmdtype(1.%04x)=%04x, "
                                            "lb(1.%04x)=%04x, txdis(1.%04x)=%04x/(1.%04x)=%04x\n"
                                            "           link(1.%04x)      : "
                                            "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"
                                            "           pol(1.%04x)       : "
                                            "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"),
                                 (side == PHY84328_INTF_SIDE_LINE) ?
                                 "Line    " : "System  ",
                                 phy84328_intf_names[intf->type],
                                 intf->speed, SYS_FORCED_CL72(pc),
                                 PHY84328_DEV1_PMD_CONTROL_REGISTER, data1,
                                 PHY84328_DEV1_PMD_CONTROL_2_REGISTER, data2,
                                 PHY84328_SYS_DEV1_ANATXACONTROL6, data3,
                                 PHY84328_DEV1_ANATXACONTROL2, data4,
                                 PHY84328_DEV1_ANATXACONTROL6, data5,
                                 PHY84328_DEV1_ANARXSTATUS, link_sts[0],
                                 link_sts[1], link_sts[2], link_sts[3],
                                 CUR_DATAPATH(pc) == PHY84328_DATAPATH_20 ?
                                 PHY84328_DEV1_ANATXACONTROL :
                                 PHY84328_DEV1_ANATXACONTROL4,
                                 pol_sts[0], pol_sts[1], pol_sts[2], pol_sts[3]));

			if (side == PHY84328_INTF_SIDE_LINE) {
				LOG_CLI((BSL_META_U(unit,
                                                    "           pol_inv(1.%04x)   : "
                                                    "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"),
                                         PHY84328_DEV1_GP_REGISTER_RX_INV,
                                         pol_inv_sts[0], pol_inv_sts[1],
                                         pol_inv_sts[2], pol_inv_sts[3]));
			}
			LOG_CLI((BSL_META_U(unit,
                                            "           txdrv(1.%04x)     : "
                                            "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"
                                            "           preman(1.%04x)    : "
                                            "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"
                                            "           premain(1.%04x)   : "
                                            "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"
                                            "           preprepos(1.%04x) : "
                                            "(ln0=%04x, ln1=%04x, ln2=%04x, ln3=%04x)\n"),
                                 PHY84328_DEV1_ANATXACONTROL1, tx_drv[0],
                                 tx_drv[1], tx_drv[2], tx_drv[3],
                                 PHY84328_SYS_DEV1_XFI_AFE_CTL2, preemph_manual[0],
                                 preemph_manual[1], preemph_manual[2], preemph_manual[3],
                                 PHY84328_DEV1_ANATXACONTROL2, preemph_main[0],
                                 preemph_main[1], preemph_main[2], preemph_main[3],
                                 PHY84328_DEV1_ANATXACONTROL3, preemph_pre_post[0],
                                 preemph_pre_post[1], preemph_pre_post[2],
                                 preemph_pre_post[3]));
		} else {
			    LOG_CLI((BSL_META_U(unit,
                                                "Lane:%d: %s: type=%s, speed=%d, "
                                                "forced cl72=%d, mode(1.%04x)=%04x\n"
                                                "           pmdtype(1.%04x)=%04x, "
                                                "lb(1.%04x)=%04x, txdis(1.%04x)=%04x/(1.%04x)=%04x\n"
                                                "           link(1.%04x)=%04x, "
                                                "txdrv(1.%04x)=%04x, pol(1.%04x)=%04x"), 
                                     i, (side == PHY84328_INTF_SIDE_LINE) ?
                                     "Line    " : "System  ",
                                     phy84328_intf_names[intf->type],
                                     intf->speed, SYS_FORCED_CL72(pc),
                                     PHY84328_DEV1_PMD_CONTROL_REGISTER, data1,
                                     PHY84328_DEV1_PMD_CONTROL_2_REGISTER, data2,
                                     PHY84328_SYS_DEV1_ANATXACONTROL6, data3,
                                     PHY84328_DEV1_ANATXACONTROL2, data4,
                                     PHY84328_DEV1_ANATXACONTROL6, data5,
                                     PHY84328_DEV1_ANARXSTATUS, link_sts[i],
                                     PHY84328_DEV1_ANATXACONTROL1, tx_drv[i],
                                     CUR_DATAPATH(pc) == PHY84328_DATAPATH_20 ?
                                     PHY84328_DEV1_ANATXACONTROL :
                                     PHY84328_DEV1_ANATXACONTROL4,
                                     pol_sts[i]));
			    if (side == PHY84328_INTF_SIDE_LINE) {
				    LOG_CLI((BSL_META_U(unit,
                                                        ", pol_inv(1.%04x)=%04x"),
                                             PHY84328_DEV1_GP_REGISTER_RX_INV,
                                             pol_inv_sts[i]));
			    }
			    LOG_CLI((BSL_META_U(unit,
                                                "\n"
                                     "           preman(1.%04x)=%04x,"
                                                "premain(1.%04x)=%04x, preprepos(1.%04x)=%04x\n"),
                                     PHY84328_SYS_DEV1_XFI_AFE_CTL2, preemph_manual[i], 
                                     PHY84328_DEV1_ANATXACONTROL2, preemph_main[i],
                                     PHY84328_DEV1_ANATXACONTROL3, preemph_pre_post[i]));
		}
	}

	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);

	if (int_pc) {
        int lb;
		SOC_IF_ERROR_RETURN(PHY_INTERFACE_GET(int_pc->pd, unit, port, &iif));
		SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &ilink));
        SOC_IF_ERROR_RETURN(_phy_84328_lb_get(unit, port, &lb));
        if (lb) {
            /* Clear the sticky bit if in loopback */
            SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &ilink));
        }
		SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &ispeed));
		SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_GET(int_pc->pd, unit, port, &ian, &ian_done));
		LOG_CLI((BSL_META_U(unit,
                                    " Internal: type=%s, speed=%d, link=%d, an=%d\n===\n"), 
                         phy84328_intf_names[iif], ispeed, ilink, ian));
	}
	if (saved_side == PHY84328_INTF_SIDE_SYS) {
        _phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
    }

	return SOC_E_NONE;
}

STATIC int
_phy_84328_control_driver_supply_set(int unit, soc_port_t port, phy84328_intf_side_t side,
									 uint32 value)
{
    soc_port_phy_control_driver_supply_t ds;
	uint16 tx_mode = 0;

    ds = (soc_port_phy_control_driver_supply_t) value;
	switch (ds) {
	case SOC_PHY_CONTROL_DRIVER_SUPPLY_1_5V_1_0V:
		tx_mode = 4;
		break;
	case SOC_PHY_CONTROL_DRIVER_SUPPLY_1_0V_700MV:
		tx_mode = 0;
		break;
	case SOC_PHY_CONTROL_DRIVER_SUPPLY_1_5V_700MV:
		tx_mode = 1;
		break;
	default:
		return SOC_E_PARAM;
	}

	return _phy_84328_tx_mode_set(unit, port, side, tx_mode);
}

STATIC int
_phy_84328_control_driver_supply_get(int unit, soc_port_t port, phy84328_intf_side_t side,
                                     uint32 *value)
{
	uint16 tx_mode = 0;
    soc_port_phy_control_driver_supply_t ds;

	SOC_IF_ERROR_RETURN(_phy_84328_tx_mode_get(unit, port, side, &tx_mode));

	switch (tx_mode) {
	case 4:
		ds = SOC_PHY_CONTROL_DRIVER_SUPPLY_1_5V_1_0V;
		break;
	case 0:
		ds = SOC_PHY_CONTROL_DRIVER_SUPPLY_1_0V_700MV;
		break;
	case 1:
		ds = SOC_PHY_CONTROL_DRIVER_SUPPLY_1_5V_700MV;
		break;
	default:
		/* The value is not reliable */
        *value = 0;
		return SOC_E_UNAVAIL;
	}

    *value = (uint32) ds;
	return SOC_E_NONE;
}

STATIC int
_phy_84328_sw_rx_los_control_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc= EXT_PHY_SW_STATE(unit, port);
    phy84328_sw_rx_los_t *sw_rx_los = &(SW_RX_LOS(pc));

#ifdef BCM_WARM_BOOT_SUPPORT
    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }
    /* preserve SW RxLOS status in register 1.0xC01A for warmboot support */
    SOC_IF_ERROR_RETURN( _phy_84328_preserve_sw_rx_los(unit, port, value) );
#endif

    sw_rx_los->cfg_enable = value;
    sw_rx_los->cur_enable = value;
    if (value) {
        sw_rx_los->sys_link = 0;
        sw_rx_los->state = PHY84328_SW_RX_LOS_RESET;
        sw_rx_los->link_status = 0;
        sw_rx_los->link_no_pcs = 0;
        sw_rx_los->restarts = 0;
#ifdef SW_RX_LOS_FLAP_CHECK
        sw_rx_los->fault_report_dis = 0;
        if (sw_rx_los->macd == NULL) {
            SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &(sw_rx_los->macd)));
        }
#endif
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84328_fw_rx_los_control_set(int unit, soc_port_t port, uint32 value)
{
    int lane_start, lane_end, lane;
    int fw_rx_los_en_data, fw_rx_los_en_mask;
    phy_ctrl_t *pc= EXT_PHY_SW_STATE(unit, port);
    uint32 lane_reg[PHY84328_NUM_LANES] = /* per lane registers timeout */
        { 0xca99, 0xca9b, 0xca9d, 0xca9f };

    FW_RX_LOS(pc) = value;

    fw_rx_los_en_mask = (1 << 15) | (1 << 14);
    if (value) {
        if (PHY84328_SINGLE_PORT_MODE(pc)) {
            lane_start = 0;
            lane_end = PHY84328_NUM_LANES;
        } 
        else {
            lane_start = pc->phy_id & 0x3;
            lane_end = lane_start + 1;
        }
        for (lane = lane_start; lane < lane_end; lane++) {
            /* Program the timeout value */
            SOC_IF_ERROR_RETURN(               
                MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, lane_reg[lane], 
                                                0xffff, 0xffff));
        }
        fw_rx_los_en_data = (1 << 15) | (1 << 14);
    } else {
        fw_rx_los_en_data = 0;
    }
        SOC_IF_ERROR_RETURN(               
            MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 0xc480, fw_rx_los_en_data, 
                                        fw_rx_los_en_mask));
    if (value){
	    /* Update interface type/speed */
        SOC_IF_ERROR_RETURN(_phy_84328_intf_line_sys_update(unit, port));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84328_rx_los_control_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    int rv  = SOC_E_NONE;
    int rv2 = SOC_E_NONE;

    switch (value) {
        case SOC_PHY_CONTROL_RX_LOS_NONE:
            rv  = _phy_84328_sw_rx_los_control_set(unit, port, 0);
            rv2 = _phy_84328_fw_rx_los_control_set(unit, port, 0);
            rv  = (rv == SOC_E_NONE) ? rv2 : rv;
            break;
        case SOC_PHY_CONTROL_RX_LOS_FIRMWARE:
            if (DEVREV(pc) == 0x00a0) {
                rv  = _phy_84328_sw_rx_los_control_set(unit, port, 1);
            } else {
                rv  = _phy_84328_sw_rx_los_control_set(unit, port, 0);
                rv2 = _phy_84328_fw_rx_los_control_set(unit, port, 1);
                rv  = (rv == SOC_E_NONE) ? rv2 : rv;
            }
            break;
        case SOC_PHY_CONTROL_RX_LOS_SOFTWARE:
        default:
            if (DEVREV(pc) == 0x00a0) {
                rv  = _phy_84328_sw_rx_los_control_set(unit, port, 1);
            } else {
                rv  = _phy_84328_fw_rx_los_control_set(unit, port, 0);
                rv2 = _phy_84328_sw_rx_los_control_set(unit, port, 1);
                rv  = (rv == SOC_E_NONE) ? rv2 : rv;
            }
            break;
    }

    return rv;
}

STATIC int
_phy_84328_rx_los_control_get(int unit, soc_port_t port, uint32 *value)
{
    phy84328_sw_rx_los_t *sw_rx_los;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    sw_rx_los = &(SW_RX_LOS(pc));

    if (sw_rx_los->cur_enable) {
        *value = SOC_PHY_CONTROL_RX_LOS_SOFTWARE;
    } else if (FW_RX_LOS(pc)) {
        *value = SOC_PHY_CONTROL_RX_LOS_FIRMWARE;
    } else {
        *value = SOC_PHY_CONTROL_RX_LOS_NONE;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84328_mod_auto_detect_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    uint16 data;
    uint16 rddata;
    uint16 count = 0;
    uint16 mask;

    pc = EXT_PHY_SW_STATE(unit, port);
    mask = PHY84328_DEV1_GP_REG_0_ENABLE_SFP_MOD_DETECT_MASK; 
    data = value ? mask : 0;

    SOC_IF_ERROR_RETURN(
        MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, data, 
                                        mask));
 
    if (!value) {
        /* Request is disabling mod auto detect, need to wait until I2CM FSM  becomes IDLE */
        /* Wait approx for 325 ms */ 
        while (count < I2CM_IDLE_WAIT_COUNT){
            SOC_IF_ERROR_RETURN(
                READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_I2C_CONTROL_REGISTER, &rddata));
            if ((rddata & 0xC) == 0) {
                break;
            }
            count ++;
            sal_usleep(I2CM_IDLE_WAIT_CHUNK * 1000);
        }
        if (count >= I2CM_IDLE_WAIT_COUNT) {
            return SOC_E_BUSY;
        }
    }
 
    /* Update the global database */
    MOD_AUTO_DETECT(pc) = value ? 1:0; 
    
    return SOC_E_NONE;
}

STATIC int
_phy_84328_mod_auto_detect_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    uint16 data;
 
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REG_0, &data));

    if( data & PHY84328_DEV1_GP_REG_0_ENABLE_SFP_MOD_DETECT_MASK ) {
        *value = 1;
    } else {
        *value = 0;
    } 
    return SOC_E_NONE;
}

STATIC int
_phy_84328_force_cl72_config(int unit, soc_port_t port, uint32 enable)
{
    uint16 intf_side = 0;
    uint16 data = 0 , mask = 0;
    phy_ctrl_t    *pc;       /* PHY software state */
    pc = EXT_PHY_SW_STATE(unit, port);

    intf_side = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE;
    if (intf_side == PHY84328_INTF_SIDE_SYS) {
        data = enable ? PHY84328_DEV1_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_MASK : 0;
        mask = PHY84328_DEV1_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_MASK;
    } else {
        data = enable ? PHY84328_DEV1_GP_REGISTER_1_LINE_FORCED_CL72_MODE_MASK : 0;
        mask = PHY84328_DEV1_GP_REGISTER_1_LINE_FORCED_CL72_MODE_MASK;
    }
    data |= PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK;
    mask |= PHY84328_DEV1_GP_REGISTER_1_FINISH_CHANGE_MASK; 
    SOC_IF_ERROR_RETURN(
            _phy_84328_intf_update(unit, port, data, mask));
    return SOC_E_NONE;
}

STATIC int
_phy_84328_force_cl72_status(int unit, soc_port_t port, uint32 *value)
{
    uint16 intf_side = 0, data = 0;
    phy_ctrl_t    *pc;       /* PHY software state */
    pc = EXT_PHY_SW_STATE(unit, port);

    intf_side = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE;

    SOC_IF_ERROR_RETURN(
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GP_REGISTER_3, &data));
    *value = 0;

    if (intf_side == PHY84328_INTF_SIDE_SYS) {
        *value = (data & PHY84328_DEV1_GP_REGISTER_1_SYSTEM_FORCED_CL72_MODE_MASK) ? 1 : 0;
    } else {
        *value = (data & PHY84328_DEV1_GP_REGISTER_1_LINE_FORCED_CL72_MODE_MASK) ? 1 : 0;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84328_control_set
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
STATIC int
_phy_84328_control_set(int unit, soc_port_t port, int intf, int lane,
                     soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);

    rv = SOC_E_UNAVAIL;
    if (intf == PHY_DIAG_INTF_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
        pc->flags |= PHYCTRL_SYS_SIDE_CTRL ;
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
		rv = _phy_84328_control_preemphasis_set(unit, port, type, 
												(pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
												PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE, 
												value);
		break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        rv = _phy_84328_control_tx_driver_set(unit, port, type,
											  (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
											  PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE, 
											  value);
        break;
    case SOC_PHY_CONTROL_EDC_MODE:
        rv = _phy_84328_control_edc_mode_set(unit,port,value);
        break;
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        rv = _phy_84328_control_recovery_clock_set(unit,port,value);
        break;
    case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
        rv = _phy_84328_control_recovery_clock_freq_set(unit,port,value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_84328_control_prbs_polynomial_set(unit, port, value, TRUE);
		if (SOC_SUCCESS(rv)) {
			rv = _phy_84328_control_prbs_polynomial_set(unit, port,value, FALSE);
		}
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_84328_control_prbs_tx_invert_data_set(unit, port, value);
		if (SOC_SUCCESS(rv)) {
			rv = _phy_84328_control_prbs_rx_invert_data_set(unit, port, value);
		}
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* fall through */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        /* tx/rx is enabled at the same time. no seperate control */
        rv = _phy_84328_control_prbs_enable_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = _phy_84328_remote_loopback_set(unit, port, intf, value);
        break;
    case SOC_PHY_CONTROL_PORT_PRIMARY:
        rv = soc_phyctrl_primary_set(unit, port, (soc_port_t)value);
        break;
    case SOC_PHY_CONTROL_PORT_OFFSET:
		rv = soc_phyctrl_offset_set(unit, port, (int)value);
        break;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
		rv = _phy_84328_rx_los_control_set(unit, port, value);
		break;
	case SOC_PHY_CONTROL_DUMP:
		/* in control set for consistency with other drivers */
		rv = _phy_84328_debug_info(unit, port);
		break;
	case SOC_PHY_CONTROL_DRIVER_SUPPLY:
		rv = _phy_84328_control_driver_supply_set(unit, port,
												  (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
												  PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE, 
												  value);
		break;
	case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
   	    /* value is not used since this is always a toggle ->1->0 */
        rv = _phy_84328_do_rxseq_restart(unit, port,
                            (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                            PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE);
        break;
    case SOC_PHY_CONTROL_MOD_AUTO_DETECT_ENABLE:
        rv = _phy_84328_mod_auto_detect_set(unit,port,value); 
        break;
    case SOC_PHY_CONTROL_CL72:
        rv = _phy_84328_force_cl72_config(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PCS_MODE:
        {
            phy_ctrl_t *int_pc;
            phy84328_intf_cfg_t *line_intf;
            rv = SOC_E_NONE;
            int_pc = INT_PHY_SW_STATE(unit, port);
	        line_intf = &(LINE_INTF(pc));
            if (int_pc) {
                SOC_IF_ERROR_RETURN(
                    PHY_CONTROL_SET(int_pc->pd, unit, port, type, value));

                SOC_IF_ERROR_RETURN(PHY_SPEED_SET(int_pc->pd, unit, port, 
                     line_intf->speed));
            } else {
                rv = SOC_E_INTERNAL;
            }
        }
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    if (intf == PHY_DIAG_INTF_SYS) {
        /* if it is targeted to the system side, switch back */
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
        pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL ;
    }
    return rv;
}

STATIC int
phy_84328_control_port_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    SOC_IF_ERROR_RETURN(_phy_84328_control_set(unit, port, PHY_DIAG_INTF_LINE,
											   PHY_DIAG_LN_DFLT, type, value));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84328_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_phy_84328_control_get(int unit, soc_port_t port, int intf, int lane,
                     soc_phy_control_t type, uint32 *value)
{
    phy_ctrl_t    *pc;       /* PHY software state */
    int rv, offset;
    soc_port_t primary;
    pc = EXT_PHY_SW_STATE(unit, port);


    PHY_CONTROL_TYPE_CHECK(type);

    if (intf == PHY_DIAG_INTF_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
        pc->flags |= PHYCTRL_SYS_SIDE_CTRL ;
    }
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
		rv = _phy_84328_control_preemphasis_get(unit, port, type,
												(pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
												PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE, 
												value);
		break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        rv = _phy_84328_control_tx_driver_get(unit, port, type,
											  (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
											  PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE, 
											  value);
        break;
    case SOC_PHY_CONTROL_EDC_MODE:
        rv = _phy_84328_control_edc_mode_get(unit,port,value);
        break;
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_84328_control_prbs_polynomial_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_84328_control_prbs_tx_invert_data_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* fall through */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_84328_control_prbs_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = _phy_84328_control_prbs_rx_status_get(unit, port,
                (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE, 
                value);
        break;
    case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
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
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = _phy_84328_remote_loopback_get(unit, port, intf, value);
        break;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
        rv = _phy_84328_rx_los_control_get(unit, port, value);
		break;
	case SOC_PHY_CONTROL_DUMP:
		*value = 0;
		rv = SOC_E_NONE;
		break;
	case SOC_PHY_CONTROL_DRIVER_SUPPLY:
		rv = _phy_84328_control_driver_supply_get(unit, port,
												  (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
												  PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE, 
												  value);
		break;
	case SOC_PHY_CONTROL_RX_SEQ_DONE:
		rv = _phy_84328_rx_seq_done_get(unit, port, 
										(pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
										PHY84328_INTF_SIDE_SYS : PHY84328_INTF_SIDE_LINE, 
										value);
		break;
    case SOC_PHY_CONTROL_MOD_AUTO_DETECT_ENABLE:
        rv = _phy_84328_mod_auto_detect_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_CL72_STATUS:
        rv = _phy_84328_force_cl72_status(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PCS_MODE:
        {
            phy_ctrl_t *int_pc;
            int_pc = INT_PHY_SW_STATE(unit, port);
            if (int_pc) {
                rv = PHY_CONTROL_GET(int_pc->pd, unit, port, type, value);
            } else {
                rv = SOC_E_INTERNAL;
            }
        }
        break;

    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    if (intf == PHY_DIAG_INTF_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
        pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL ;
    }
    return rv;
}
STATIC int
phy_84328_control_port_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int intf;

    intf = PHY_DIAG_INTF_LINE;
    SOC_IF_ERROR_RETURN
        (_phy_84328_control_get(unit, port, intf, PHY_DIAG_LN_DFLT, type, value));

    return SOC_E_NONE;
}

/* Returns whether interface has cl72 */
STATIC int
_phy_84328_cl72_en(int unit, soc_port_t port, phy84328_intf_side_t side)
{
	int cl72 = FALSE;
    phy_ctrl_t *pc;
    phy84328_intf_cfg_t *ifc;

    pc = EXT_PHY_SW_STATE(unit, port);
    ifc = (side == PHY84328_INTF_SIDE_LINE) ? &(LINE_INTF(pc)) : &(SYS_INTF(pc));
    switch (ifc->type) {
    case SOC_PORT_IF_KR:
    case SOC_PORT_IF_KR4:
        cl72 = TRUE;
        break;
    case SOC_PORT_IF_CR4:
        /* System side is always DAC */
        if (side == PHY84328_INTF_SIDE_SYS) {
            cl72 = FALSE;
        } else {
            /* If no AN, it's really DAC */
            cl72 = AN_EN(pc);
        }
        break;
    default:
        cl72 = FALSE;
        break;
    }
    return cl72;
}

/* dsc values */
struct phy84328_dsc_cb {
    int tx_pre_cursor;
    int tx_main;
    int tx_post_cursor;
    char *vga_bias_reduced;
    int postc_metric;
    int pf_ctrl;
    int vga_sum;
    int dfe1_bin;
    int dfe2_bin;
    int dfe3_bin;
    int dfe4_bin;
    int dfe5_bin;
    int integ_reg;
    int integ_reg_xfer;
    int clk90_offset;
    int slicer_target;
    int offset_pe;
    int offset_ze;
    int offset_me;
    int offset_po;
    int offset_zo;
    int offset_mo;
};

STATIC int
_phy_84328_diag_dsc(int unit, soc_port_t port, int intf, int lane)
{
    int	 i, reg, lanes, regval;
    uint16 data16;
    phy_ctrl_t *pc;
	phy84328_intf_side_t side;
	struct phy84328_dsc_cb *dsc_cbp;
	struct phy84328_dsc_cb dsc_cb[PHY84328_NUM_LANES];

    pc = EXT_PHY_SW_STATE(unit, port);

	if (PHY84328_SINGLE_PORT_MODE(pc)) {
        lanes = PHY84328_NUM_LANES ;
    } 
    else {
        lanes = 1;
    }
	side =  (intf == PHY_DIAG_INTF_LINE) ? PHY84328_INTF_SIDE_LINE :
		PHY84328_INTF_SIDE_SYS;
    sal_memset((char *)&dsc_cb[0], 0, (sizeof(dsc_cb)));

	PHY_84328_MICRO_PAUSE(unit, port, "dsc");
	_phy_84328_intf_side_regs_select(unit, port, side);
    for (i = 0; i < lanes; i++) {
		if (PHY84328_SINGLE_PORT_MODE(pc)) {
            /* Select the lane and side to access */
            SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, i));
        }
        dsc_cbp = &(dsc_cb[i]);

		/* tx drive pre, main, post cursor taps are read differently for cl72 */
		if (_phy_84328_cl72_en(unit, port, side)) {
			uint16 saved_reg;

			/* 1.c063[15:14]=1 to get port cl72 tap values in 1.c060 */
			reg = 0xc063;
			SOC_IF_ERROR_RETURN
				(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
			saved_reg = data16;
			regval = 0x4000 | (data16 & 0x3fff);
			SOC_IF_ERROR_RETURN
				(WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, data16));
			reg = 0xc060;
			SOC_IF_ERROR_RETURN
				(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));

			/* tx_pre_cursor */
			regval = data16 & 0xf;
			dsc_cbp->tx_pre_cursor = regval;

			/* tx_post_cursor */
			regval = (data16 >> 10) & 0x1f;
			dsc_cbp->tx_post_cursor = regval;

			/* tx_main */
			regval = (data16 >> 4) & 0x3f;
			dsc_cbp->tx_main = regval;

			/* Restore 1.c063 to original values used to access 1.c060 */
			SOC_IF_ERROR_RETURN
				(WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, saved_reg));
		} else {
			reg = 0xc067;
			SOC_IF_ERROR_RETURN
				(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));

			/* tx_pre_cursor */
			regval = data16 & 0x7;
			dsc_cbp->tx_pre_cursor = regval;

			/* tx_post_cursor */
			regval = (data16 >> 4) & 0x1F;
			dsc_cbp->tx_post_cursor = regval;

			/* tx_main */
			reg = 0xc066;
			SOC_IF_ERROR_RETURN
				(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
			regval = (data16 >> 11) & 0x1F;
			dsc_cbp->tx_main = regval;
		}

        /* vga_bias_reduced */ 
        reg = 0xc0BD;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16 & (1 << 4);
        if (regval) {
            dsc_cbp->vga_bias_reduced = "88%";
        } else {
            dsc_cbp->vga_bias_reduced = "100%";
        }

        /* post2 tap - c068[10:8] */
        reg = 0xc068;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = (data16 >> 8) & 0x7;
        dsc_cbp->postc_metric = regval;


        /* pf DSC_3_ADDR=0xc220 + 11 */
        reg = 0xc220 + 11;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = (data16 & 0xf);
        dsc_cbp->pf_ctrl = regval;

        /* vga sum DSC_3_ADDR=0xc220 + 5 */
        reg = 0xc220 + 5;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = (data16 & 63);
        dsc_cbp->vga_sum = regval;

        /* dfe1_bin DSC_3_ADDR=0xc220 + 5 */
		reg = 0xc220 + 5;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval /= 64; 
        regval &= 63;
        dsc_cbp->dfe1_bin = regval;

        /* dfe2_bin  DSC_3_ADDR=0xc220 + 6 */
        reg = 0xc220 + 6;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        dsc_cbp->dfe2_bin = regval;
        
        /* dfe3_bin DSC_3_ADDR=0xc220 + 6 */
        reg = 0xc220 + 6;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval /= 64;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        dsc_cbp->dfe3_bin = regval;
        
        /* dfe4_bin DSC_3_ADDR=0xc220 + 7 */
        reg = 0xc220 + 7;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval &= 31;
        if (regval >= 16) {
            regval -=32;
        }
        dsc_cbp->dfe4_bin = regval;
  
        /* dfe5_bin DSC_3_ADDR=0xc220 + 7  */
        reg = 0xc220 + 7;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval /= 32;
        regval &= 31;
        if (regval >= 16) {
            regval -=32;
        }
        dsc_cbp->dfe5_bin = regval;

        /* integ_reg(lane) = rd22_integ_reg (phy, lane) DSC_3_ADDR=0xc220  */
        reg = 0xc220;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval &= 65535;
        if (regval >= 32768) {
            regval -=65536;
        }
        regval /= 84;
        dsc_cbp->integ_reg = regval;

        /* integ_reg_xfer(lane) = rd22_integ_reg_xfer (phy, lane)   */
        reg = 0xc221;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval &= 65535;
        if (regval >= 32768) {
            regval -=65536;
        }
        dsc_cbp->integ_reg_xfer = regval;

        /* clk90_offset(lane) = rd22_clk90_phase_offset (phy, lane)   */
        reg = 0xc223;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval /= 128;
        regval &= 127;
        dsc_cbp->clk90_offset = regval;

        /* slicer_target(lane) = ((25*rd22_rx_thresh_sel (phy, lane)) + 125)   */
        reg = 0xc21c;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval /= 32;
        regval &= 3;
        dsc_cbp->slicer_target = regval * 25 + 125;

        /* offset_pe(lane) = rd22_slicer_offset_pe (phy, lane)   */
        reg = 0xc22c;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        dsc_cbp->offset_pe = regval;

        /* offset_ze(lane) = rd22_slicer_offset_ze (phy, lane)   */
        reg = 0xc22d;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        dsc_cbp->offset_ze = regval;

        /* offset_me(lane) = rd22_slicer_offset_me (phy, lane)   */
        reg = 0xc22e;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        dsc_cbp->offset_me = regval;

        /* offset_po(lane) = rd22_slicer_offset_po (phy, lane) */
        reg = 0xc22c;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval /= 64;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        dsc_cbp->offset_po = regval;

        /* offset_zo(lane) = rd22_slicer_offset_zo (phy, lane) */
        reg = 0xc22d;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval /= 64;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        dsc_cbp->offset_zo = regval;

        /* offset_mo(lane) = rd22_slicer_offset_mo (phy, lane) */
        reg = 0xc22e;
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
        regval = data16;
        regval /= 64;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        dsc_cbp->offset_mo = regval;
    }
	/* Restore to default single port register access */
	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, PHY84328_ALL_LANES));
	}
	_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	PHY_84328_MICRO_RESUME(unit, port);

    /* display */
    LOG_CLI((BSL_META_U(unit,
                        "\nDSC parameters for port %d\n"), port));

    LOG_CLI((BSL_META_U(unit,
                        "LN  PPM PPM_XFR clk90_ofs PF SL_TRGT VGA BIAS DFE1 DFE2 "
                        "DFE3 DFE4 DFE5 PREC MAIN POSTC MTRC "
                        "PE   ZE   ME  PO  ZO  MO\n")));
    for (i = 0; i < lanes; i++) {
        dsc_cbp = &(dsc_cb[i]);
        LOG_CLI((BSL_META_U(unit,
                            "%02d %04d %07d %09d %04d %04d %04d %4s %04d %04d %04d %04d "
                            "%04d %04d %04d %04d  %04d %04d %04d %2d %3d %3d %2d\n"),
                 i, dsc_cbp->integ_reg,dsc_cbp->integ_reg_xfer,
                 dsc_cbp->clk90_offset,
                 dsc_cbp->pf_ctrl, dsc_cbp->slicer_target,
                 dsc_cbp->vga_sum, dsc_cbp->vga_bias_reduced,
                 dsc_cbp->dfe1_bin, dsc_cbp->dfe2_bin,
                 dsc_cbp->dfe3_bin, dsc_cbp->dfe4_bin,
                 dsc_cbp->dfe5_bin, dsc_cbp->tx_pre_cursor,
                 dsc_cbp->tx_main,dsc_cbp->tx_post_cursor,
                 dsc_cbp->postc_metric, dsc_cbp->offset_pe,
                 dsc_cbp->offset_ze,dsc_cbp->offset_me,
                 dsc_cbp->offset_po, dsc_cbp->offset_zo, dsc_cbp->offset_mo));

    }

    return SOC_E_NONE;
}


STATIC int
_phy_84328_diag_eyescan_uc_ready(int unit, soc_port_t port)
{
	int rv = SOC_E_NONE;
	uint16 data;
	soc_timeout_t to;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "    _phy_84328_diag_eyescan_uc_ready\n")));
	}

	/* Wait for uC ready */
	soc_timeout_init(&to, 250000, 0);
	while (!soc_timeout_check(&to)) {
		rv = READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL, &data);
		if (rv != SOC_E_NONE) {
			break;
		}
		if (data & PHY84328_DEV1_UC_CTRL_READY_FOR_CMD_MASK) {
			break;
		}
	}

	if ((rv == SOC_E_NONE) &&
		(data & PHY84328_DEV1_UC_CTRL_READY_FOR_CMD_MASK)) {
		rv = SOC_E_NONE;
	} else {
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328 EYE: uController not ready pass 1!: u=%d, p=%d "
                                      "uc_ctrl(1.%04x)=%04x\n"), unit, port, 
                           PHY84328_SYS_DEV1_UC_CTRL, data));
		rv = SOC_E_TIMEOUT;
	}

    return rv;
}

STATIC int
_phy_84328_diag_eyescan_offset(int unit, soc_port_t port, int16 supp_data, int req)
{
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "   _phy_84328_diag_eyescan_offset\n")));
	}

    /* offset 1.c20e */
    SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL,
										supp_data << PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
										PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL,
										0, PHY84328_SYS_DEV1_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL,
										0, PHY84328_SYS_DEV1_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL,
										0, PHY84328_SYS_DEV1_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL,
										(req | 
										 (supp_data << PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
										PHY84328_SYS_DEV1_UC_CTRL_GP_UC_REQ_MASK |
										PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_MASK));

    /* wait for uC ready for command */
	return _phy_84328_diag_eyescan_uc_ready(unit, port);
}

STATIC uint16
_phy_84328_diag_eyescan_diag_ctrl_get(int unit, soc_port_t port)
{
	uint16 data;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
													  PHY84328_SYS_DEV1_DSC_DIAG_CTRL0,
													  &data));
	return data;
}

STATIC int
_phy_84328_diag_eyescan_livelink_en(int unit, soc_port_t port, int en)
{
	uint16 data, mask;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_livelink_en: en=%d\n"), en));
	}

	data = 0x2;
	mask = data;
	SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
														PHY84328_SYS_DEV1_DSC_DIAG_CTRL0,
														data, mask));

	data = en ? PHY84328_SYS_DEV1_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_BITS : 0;
	mask = PHY84328_SYS_DEV1_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK;

	SOC_IF_ERROR_RETURN(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
														PHY84328_SYS_DEV1_DSC_DIAG_CTRL0,
														data, mask));
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_livelink_en: en=%d [%04x]\n"), 
                         en, _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}

    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_livelink_clear(int unit, soc_port_t port, int unused)
{
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port,  0x3, 0x6));
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_livelink_clear: [%04x]\n"), 
                         _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}
    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_livelink_start(int unit, soc_port_t port, int unused)
{
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port,  0x0, 0x6));
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_livelink_start: [%04x]\n"), 
                         _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}
    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_livelink_stop(int unit, soc_port_t port, int unused)
{
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port, 0x1, 0x6));
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_livelink_stop: [%04x]\n"), 
                         _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	} 
    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_livelink_read(int unit, soc_port_t port, int *err_counter)
{
	int i;
	uint16 data;
	uint32 counter;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	counter = 0;
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_livelink_read: ")));
	}
	for (i = 0; i < 4; i++) {
		SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port, 0x2, 0x6));

		/* read back the maximum offset */
		SOC_IF_ERROR_RETURN(
			READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL, &data));
		data &= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_MASK;
		data >>= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
		if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
			LOG_CLI((BSL_META_U(unit,
                                            "[%d]=%d "), i, data));
		}
		counter += (data << (8 * i));
	} 
	*err_counter = counter;

	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "\n_phy_84328_diag_eyescan_livelink_read: err_counter=%d\n"),
                         *err_counter));
	}
    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_init_voffset_get(int unit, soc_port_t port, int *voffset)
{
    uint16 data;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	*voffset = 0;
	/* 1.c21c[15:10] */
	SOC_IF_ERROR_RETURN(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
													  PHY84328_DEV1_DSC_ANA_CTRL3,
													  &data));
    *voffset = (data & PHY84328_DEV1_DSC_ANA_CTRL3_RSVD_FOR_ECO_MASK) >>
		PHY84328_DEV1_DSC_ANA_CTRL3_RSVD_FOR_ECO_SHIFT;

	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_init_voffset_get: "
                                    "voffset=%x [%04x]\n"), 
                         *voffset,
                         _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}
    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_hoffset_set(int unit, soc_port_t port, int *hoffset)
{
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port, *hoffset, 0x2));
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_hoffset_set: hoffset=%x [%04x]\n"), 
                         *hoffset, _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}
    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_voffset_set(int unit, soc_port_t port, int *voffset)
{
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port, *voffset, 0x3));
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_voffset_set: voffset=%x [%04x]\n"), 
                         *voffset, _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}
    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_max_left_hoffset_get(int unit, soc_port_t port, int *left_hoffset)
{
    uint16 data;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port, 127, 0x2));
    /* read back the maximum offset */
    SOC_IF_ERROR_RETURN(
		READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL, &data));
    data &= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
	*left_hoffset = -data;

	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_max_left_hoffset_get: "
                                    "max_left_hoffset=%d [%04x]\n"), 
                         *left_hoffset,
                         _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}

    return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_max_right_hoffset_get(int unit, soc_port_t port, int *right_hoffset)
{
    uint16 data;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port, -128, 0x2));
    /* read back the right offset */
    SOC_IF_ERROR_RETURN(
		READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL, &data));
    data &= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
	*right_hoffset = data;
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_max_right_hoffset_get: "
                                    "max right hoffset=%d [%04x]\n"), 
                         *right_hoffset,
                         _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}
	return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_min_voffset_get(int unit, soc_port_t port, int *min_voffset)
{
    uint16 data;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port, -128, 0x3));

    /* read back the min offset */
    SOC_IF_ERROR_RETURN(
		READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL, &data));
    data &= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
	*min_voffset = data - 256;

	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_min_voffset_get: "
                                    "min_voffset=%d [%04x]\n"), 
                         *min_voffset,
                         _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}
	return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan_max_voffset_get(int unit, soc_port_t port, int *max_voffset)
{
    uint16 data;
	phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

	SOC_IF_ERROR_RETURN(_phy_84328_diag_eyescan_offset(unit, port, 127, 0x3));
    /* read back the max voffset */
    SOC_IF_ERROR_RETURN(
		READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_SYS_DEV1_UC_CTRL, &data));
    data &= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= PHY84328_SYS_DEV1_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
	*max_voffset = data;
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_EYE) {
		LOG_CLI((BSL_META_U(unit,
                                    "_phy_84328_diag_eyescan_max_voffset_get: "
                                    "max_voffset=%d [%04x]\n"), 
                         *max_voffset,
                         _phy_84328_diag_eyescan_diag_ctrl_get(unit, port)));
	}
	return SOC_E_NONE;
}

STATIC int
_phy_84328_diag_eyescan(int unit, soc_port_t port, uint32 inst, int op_cmd, void *arg)
{
    int lane;
    int intf;
    phy84328_intf_side_t side;
	int rv = SOC_E_NONE;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    intf = PHY_DIAG_INST_INTF(inst);
    if (intf == PHY_DIAG_INTF_SYS) {
        side = PHY84328_INTF_SIDE_SYS;
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_SYS);
	} else {
        side = PHY84328_INTF_SIDE_LINE;
    }
    lane = PHY_DIAG_INST_LN(inst);
    if (PHY84328_SINGLE_PORT_MODE(pc) && (lane != 0)) {
        SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, side, lane));
    }
	switch (op_cmd) {
	case PHY_DIAG_CTRL_EYE_ENABLE_LIVELINK:
		rv = _phy_84328_diag_eyescan_livelink_en(unit, port, 1);
		break;
	case PHY_DIAG_CTRL_EYE_DISABLE_LIVELINK:
		rv = _phy_84328_diag_eyescan_livelink_en(unit, port, 0);
		break;
	case PHY_DIAG_CTRL_EYE_CLEAR_LIVELINK:
		rv = _phy_84328_diag_eyescan_livelink_clear(unit, port, 0);
		break;
	case PHY_DIAG_CTRL_EYE_START_LIVELINK:
		rv = _phy_84328_diag_eyescan_livelink_start(unit, port, 0);
		break;
	case PHY_DIAG_CTRL_EYE_STOP_LIVELINK:
		rv = _phy_84328_diag_eyescan_livelink_stop(unit, port, 0);
		break;
	case PHY_DIAG_CTRL_EYE_READ_LIVELINK:
		rv = _phy_84328_diag_eyescan_livelink_read(unit, port, (int *) arg);
		break;
	case PHY_DIAG_CTRL_EYE_GET_INIT_VOFFSET:
		rv = _phy_84328_diag_eyescan_init_voffset_get(unit, port, (int *)arg);
		break;
	case PHY_DIAG_CTRL_EYE_SET_HOFFSET:
		rv = _phy_84328_diag_eyescan_hoffset_set(unit, port, (int *)arg);
		break;
	case PHY_DIAG_CTRL_EYE_SET_VOFFSET:
		rv = _phy_84328_diag_eyescan_voffset_set(unit, port, (int *)arg);
		break;
	case PHY_DIAG_CTRL_EYE_GET_MAX_LEFT_HOFFSET:
		rv = _phy_84328_diag_eyescan_max_left_hoffset_get(unit, port, (int *)arg);
		break;
	case PHY_DIAG_CTRL_EYE_GET_MAX_RIGHT_HOFFSET:
		rv = _phy_84328_diag_eyescan_max_right_hoffset_get(unit, port, (int *)arg);
		break;
	case PHY_DIAG_CTRL_EYE_GET_MIN_VOFFSET:
		rv = _phy_84328_diag_eyescan_min_voffset_get(unit, port, (int *)arg);
		break;
	case PHY_DIAG_CTRL_EYE_GET_MAX_VOFFSET:
		rv = _phy_84328_diag_eyescan_max_voffset_get(unit, port, (int *)arg);
		break;
	}
    if (PHY84328_SINGLE_PORT_MODE(pc) && (lane != 0)) {
        SOC_IF_ERROR_RETURN(_phy_84328_channel_select(unit, port, PHY84328_INTF_SIDE_LINE,
                                                      PHY84328_ALL_LANES));
    }
    if (intf == PHY_DIAG_INTF_SYS) {
		_phy_84328_intf_side_regs_select(unit, port, PHY84328_INTF_SIDE_LINE);
	}

	return rv;
}

STATIC int
_phy_84328_diag_ctrl(int unit, soc_port_t port, uint32 inst, int op_type,  int op_cmd,  
					void *arg)   
{
    int lane;
    int intf;
	int rv = SOC_E_NONE;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "84328 diag_ctrl: u=%d p=%d ctrl=0x%x\n"), unit, port, op_cmd));
   
    lane = PHY_DIAG_INST_LN(inst);
    intf = PHY_DIAG_INST_INTF(inst);
    if (intf == PHY_DIAG_INTF_DFLT) {
        intf = PHY_DIAG_INTF_LINE;
    }

	switch (op_cmd) {
	case PHY_DIAG_CTRL_DSC:
		SOC_IF_ERROR_RETURN(_phy_84328_diag_dsc(unit, port, intf, lane));
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
		rv = _phy_84328_diag_eyescan(unit, port, inst, op_cmd, arg);
		break;
	case PHY_DIAG_CTRL_EYE_MARGIN_VEYE:
	case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_LEFT:
	case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_RIGHT:
		rv = SOC_E_UNAVAIL;
        break;

	default:
		if (op_type == PHY_DIAG_CTRL_SET) {
			SOC_IF_ERROR_RETURN
				(_phy_84328_control_set(unit, port, intf, lane, op_cmd, PTR_TO_INT(arg)));
		} else if (op_type == PHY_DIAG_CTRL_GET) {
			SOC_IF_ERROR_RETURN
				(_phy_84328_control_get(unit, port, intf, lane, op_cmd, (uint32 *)arg));
		} else {
			LOG_INFO(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "84328 diag_ctrl bad operation: u=%d p=%d ctrl=0x%x\n"), 
                                             unit, port, op_cmd));
			rv = SOC_E_UNAVAIL;
		}
		break;
	}

    return rv;
}

/*
 * Return whether AN is enabled or forced with internal SerDes on the system side
 */
STATIC int
_phy_84328_intf_sys_forced(int unit, soc_port_t port, soc_port_if_t intf_type)
{
    int rv;
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
    case SOC_PORT_IF_CAUI:
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

STATIC int
_phy_84328_intf_line_forced(int unit, soc_port_t port, soc_port_if_t intf_type)
{
	int rv;

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
    case SOC_PORT_IF_CAUI:
		rv = TRUE;
		break;
	default:
		rv = FALSE;
	}
	return rv;
}

STATIC int
__phy_84328_speed_set(int unit, soc_port_t port, int speed)
{
	int rv = SOC_E_NONE;
	int cur_an, cur_an_done;
    int cur_speed = 0;
    uint32  cur_fw_mode = 0;
	soc_port_if_t cur_type, new_type;
    phy_ctrl_t  *pc, *int_pc;
	phy84328_intf_cfg_t  *line_intf;
	phy84328_intf_cfg_t  *sys_intf;

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));
	sys_intf = &(SYS_INTF(pc));

	line_intf->speed = speed;
	sys_intf->speed = speed;

    /* Make sure datapath is correct one for this speed */
	SOC_IF_ERROR_RETURN(_phy_84328_data_path_check(unit, port));

	if (int_pc != NULL) {
		if ((sys_intf->type == SOC_PORT_IF_GMII) || (sys_intf->type == SOC_PORT_IF_SGMII)) {
			SOC_IF_ERROR_RETURN(PHY_SPEED_SET(int_pc->pd, unit, port, sys_intf->speed));
		} else if (_phy_84328_intf_sys_forced(unit, port, sys_intf->type) ) {
			/*
			 * When setting system side 40G DAC/XLAUI-DFE, tell internal SerDes to match
			 * with XLAUI.
			 *
			 * If flexports, the internal SerDes may be out of sync until it gets a chance 
			 *  to convert the port, so these operations may be best effort.
			 */
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
                /*
                LOG_CLI((BSL_META_U(unit,
                                    "84328 u=%d p=%d speed get error\n"), unit, port));
                */
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
                /*
                LOG_CLI((BSL_META_U(unit,
                                    "84328 u=%d p=%d fwmode get error\n"), unit, port));
                */
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
	return _phy_84328_intf_line_sys_update(unit, port);
}

STATIC int
_phy_84328_intf_line_sys_init(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
	phy84328_intf_cfg_t  *line_intf;

    pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));

	return __phy_84328_speed_set(unit, port, line_intf->speed);
}

/*
 * Function:
 *      phy_84328_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
_phy_84328_speed_set(int unit, soc_port_t port, int speed)
{
	int rv;
    phy_ctrl_t  *pc;
	phy84328_intf_cfg_t  *line_intf;
	phy84328_intf_cfg_t  *sys_intf;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port); 

    pc = EXT_PHY_SW_STATE(unit, port);
	line_intf = &(LINE_INTF(pc));
	sys_intf = &(SYS_INTF(pc));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_speed_set: u=%d p=%d speed=%d\n"), 
                         unit, port,speed));
    if (PHY84328_SINGLE_PORT_MODE(pc)) {
		if (! _phy_84328_intf_is_single_port(line_intf->type)) {
			LOG_INFO(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "84328 speed set does not match interface: "
                                             "u=%d p=%d speed=%d intf=%d\n"),
                                  unit, port, speed, sys_intf->type));
			return SOC_E_PARAM;
		}
		switch (speed) {
        case 100000:
		case 40000:
			break;
		case 42000:
			/* 42G only valid for HG[42] */
			if (! IS_HG_PORT(unit, port)) {
				speed = 40000;
			}
			break;
		default:
			return SOC_E_PARAM;
		}
    } else {
		if (_phy_84328_intf_is_single_port(line_intf->type)) {
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "84328  invalid intf in quad port: u=%d p=%d intf=%d\n"),
                                              unit, port, line_intf->type));
			return SOC_E_PARAM;
		} 
		switch (speed) {
		case 1000:
            if (!_phy_84328_intf_type_1000(line_intf->type)) {
                line_intf->type = SOC_PORT_IF_GMII;
            }
            /*
			 * Changing speed to 1000 
			 * - if backplane interface then change to KX
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
		case 10000:
			/* 
			 * If changing from 1000 to 10000, then adjust interface 
			 */
			if (line_intf->speed <= 1000) {
				/* 
				 * If changing speed from 1000 to 10000, set line interface to SFI
				 */
				if (!_phy_84328_intf_type_10000(line_intf->type)) {
					line_intf->type = SOC_PORT_IF_SR;
				}
				/*
				 * Restore the configured system side interface
				 */
				sys_intf->type = CFG_SYS_INTF(pc);
				if (!_phy_84328_intf_type_10000(sys_intf->type)) {
					sys_intf->type = SOC_PORT_IF_XFI;
				}
			}
			break;
		case 40000:
		default:
			LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "84328  invalid speed: u=%d p=%d speed=%d\n"),
                                              unit, port, speed));
			return SOC_E_PARAM;
		}
	}
	rv =  __phy_84328_speed_set(unit, port, speed);
	if (SOC_FAILURE(rv)) {
		LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84328  %s failed: u=%d p=%d\n"), FUNCTION_NAME(), unit, port));
	}
	return rv;
}

/*
 * Function:
 *      phy_84328_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - current link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
_phy_84328_speed_get(int unit, soc_port_t port, int *speed)
{
	phy84328_intf_cfg_t *line_intf;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

	line_intf = &(LINE_INTF(pc));
	*speed = line_intf->speed;

    return SOC_E_NONE;
}


STATIC int
_phy_84328_intf_type_set(int unit, soc_port_t port, soc_port_if_t pif, int must_update)
{
	int			rv = SOC_E_NONE;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	phy84328_intf_cfg_t *line_intf;
	int			update = 0;
	uint16		reg_data = 0, reg_mask = 0;
    uint16      data = 0, mask = 0;

	line_intf = &(LINE_INTF(pc));
	
	reg_data = 0;
	reg_mask = 0;

    FORCE_20BIT(pc) &= ~(FORCE_20BIT_AN);

    if (_phy_84328_intf_is_single_port(pif)) {

        if (!PHY84328_SINGLE_PORT_MODE(pc)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "84328 invalid interface for quad port: u=%d p=%d\n"),
                                  unit, port));
            return SOC_E_CONFIG;
        }
		/* 
		 * If rx polarity is flipped and the interface is CR4, then change the datapath
         * to 20-bit as rx polarity cannot be handle on the line side in 4-bit
		 */
        if (pif == SOC_PORT_IF_CR4) {
			if (AN_EN(pc)) {
				if (POL_RX_CFG(pc) && (CUR_DATAPATH(pc) != PHY84328_DATAPATH_20)) {
					CUR_DATAPATH(pc) = PHY84328_DATAPATH_20;
					update = 1;
				}
                if (POL_RX_CFG(pc)) {
                    FORCE_20BIT(pc) |= FORCE_20BIT_AN ;
                }
			} else if (FORCE_20BIT(pc) == 0  && 
                       CUR_DATAPATH(pc) != CFG_DATAPATH(pc)) {
				CUR_DATAPATH(pc) = CFG_DATAPATH(pc);
				update = 1;
			}

			SOC_IF_ERROR_RETURN(
				_phy_84328_intf_datapath_reg_get(unit, port, CUR_DATAPATH(pc), &data, &mask));
			reg_data |= data;
			reg_mask |= mask;
        }
		if ((pif != line_intf->type) || must_update) {
            update = 1;

		}
    } else if (_phy_84328_intf_is_quad_port(pif)) {
        if (PHY84328_SINGLE_PORT_MODE(pc)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "84328 invalid interface for single port: u=%d p=%d\n"),
                                  unit, port));
            return SOC_E_CONFIG;
        }

        if ((line_intf->type != pif) || must_update) {
            update = 1;
            SOC_IF_ERROR_RETURN(
                _phy_84328_intf_type_reg_get(unit, port, pif, PHY84328_INTF_SIDE_LINE,
                                             &reg_data, &reg_mask));

        }
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "84328 invalid interface for port: u=%d p=%d intf=%d\n"),
                              unit, port, pif));
        return SOC_E_CONFIG;
    }

	if (update) {
		line_intf->type = pif;

        SOC_IF_ERROR_RETURN(
            _phy_84328_intf_type_reg_get(unit, port, pif, PHY84328_INTF_SIDE_LINE,
                                             &data, &mask));
        reg_data |= data;
        reg_mask |= mask;

		SOC_IF_ERROR_RETURN(_phy_84328_intf_update(unit, port, reg_data, reg_mask));

	}

	return rv;
}

/*
 * Function:
 *      phy_84328_interface_set
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

STATIC int
_phy_84328_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
	int rv;

	LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "84328 interface set: u=%d p=%d pif=%s\n"), 
                             unit, port, phy84328_intf_names[pif]));


	if (pif == SOC_PORT_IF_MII) {
		return phy_null_interface_set(unit, port, pif);
	}

	if (pif == SOC_PORT_IF_XGMII) {
		return phy_null_interface_set(unit, port, pif);
	}

	rv = _phy_84328_intf_type_set(unit, port, pif, FALSE);
	if (SOC_FAILURE(rv)) {
		LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "84328  interface set check failed: u=%d p=%d\n"),
                                     unit, port));
	}

	if (_phy_84328_intf_line_forced(unit, port, pif)) {
		SOC_IF_ERROR_RETURN(_phy_84328_an_set(unit, port, 0));
	}

	return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84328_interface_get
 * Purpose:
 *      Get the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
_phy_84328_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
	phy84328_intf_cfg_t *line_intf;

	/* interface set now operates on line side interface */
	line_intf = &(LINE_INTF(pc));
	*pif = line_intf->type;

    return SOC_E_NONE;
}

STATIC int
_phy84328_init_ucode_bcst_load(int unit, int port)
{
	uint16 data16, mask16, num_words;
	uint8 *fw_ptr;
	int j, fw_length;
	phy_ctrl_t *pc;
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
	int dbg_flag = 1;
#endif

	pc = EXT_PHY_SW_STATE(unit, port);

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
	if (dbg_flag) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84328 bcst download device name %s: u=%d p=%d\n"),
                      pc->dev_name? pc->dev_name: "NULL", unit, port));
	}				 
#endif
	/* find the right firmware */
	if (pc->dev_name == dev_name_84328_a0) {
        fw_ptr = phy84328_ucode_bin;
        fw_length = phy84328_ucode_bin_len;
    } else if (pc->dev_name == dev_name_84328) {
        fw_ptr = phy84328B0_ucode_bin;
        fw_length = phy84328B0_ucode_bin_len;
    } else if (pc->dev_name == dev_name_84088) {
        fw_ptr = phy84328B0_ucode_bin;
        fw_length = phy84328B0_ucode_bin_len;
	} else {
		/* invalid device name */
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)		
		LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "firmware_bcst,invalid device name %s: u=%d p=%d\n"),
                          pc->dev_name? pc->dev_name: "NULL", unit, port));
#endif						 
		return SOC_E_NONE;
	}
	
	/* clear SPA ctrl reg bit 15 and bit 13.
	 * bit 15, 0-use MDIO download to SRAM, 1 SPI-ROM download to SRAM
	 * bit 13, 0 clear download done status, 1 skip download
	 */
	mask16 = PHY84328_DEV1_SPA_CONTROL_STATUS_REGISTER_SPI_PORT_USED_MASK 
		| PHY84328_DEV1_SPA_CONTROL_STATUS_REGISTER_SPI_DWLD_DONE_MASK;
	data16 = 0;
	SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
										PHY84328_DEV1_SPA_CONTROL_STATUS_REGISTER,
										data16,mask16));
	
	/* set SPA ctrl reg bit 14, 1 RAM boot, 0 internal ROM boot */
	mask16 = PHY84328_DEV1_SPA_CONTROL_STATUS_REGISTER_SPI_BOOT_MASK;
	data16 = mask16;
	SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
										PHY84328_DEV1_SPA_CONTROL_STATUS_REGISTER,
										data16,mask16));
	
	/* misc_ctrl1 reg bit 3 to 1 for 32k downloading size */
	mask16 = PHY84328_DEV1_MISC_CTRL_SPI_DOWNLOAD_SIZE_MASK;
	data16 = mask16;
	SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
										PHY84328_DEV1_MISC_CTRL,
										data16,mask16));
	
	/* deassert ucrst 1.ca10 bit 2=0 */
	mask16 = PHY84328_DEV1_GEN_CTRL_UCRST_MASK;
	data16 = 0x00; /*clear; not self-clearing*/
	SOC_IF_ERROR_RETURN(
		MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GEN_CTRL,
										data16,mask16));
	
	/* wait for 2ms for M8051 start and 5ms to initialize the RAM */
	sal_usleep(10000); /* Wait for 10ms */
	
	/* Write Starting Address, where the Code will reside in SRAM */
	data16 = 0x8000;
	SOC_IF_ERROR_RETURN(
		WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
									   PHY84328_DEV1_MSG_IN, data16));
	
	/* make sure address word is read by the micro */
	sal_udelay(10); /* Wait for 10us */
	
	/* Write SPI SRAM Count Size */
	data16 = (fw_length)/2;
	SOC_IF_ERROR_RETURN(
		WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
									   PHY84328_DEV1_MSG_IN, data16));
	
	/* make sure read by the micro */
	sal_udelay(10); /* Wait for 10us */
    
    if (pc->addr_write != NULL) { 
        SOC_IF_ERROR_RETURN(
            WRITE_ADDR_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                                           PHY84328_C45_DEV_PMA_PMD,
                                           PHY84328_DEV1_MSG_IN));
	}
	/* Fill in the SRAM */
	num_words = (fw_length - 1);
	for (j = 0; j < num_words; j+=2) {

		data16 = (fw_ptr[j] << 8) | fw_ptr[j+1];
    
        if (pc->data_write != NULL) {    
    		SOC_IF_ERROR_RETURN(
	    		WRITE_DATA_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
		    								PHY84328_C45_DEV_PMA_PMD, data16));
        } else {

    		/* Make sure the word is read by the Micro */
	        sal_udelay(10);
            SOC_IF_ERROR_RETURN(
                WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84328_DEV1_MSG_IN, data16));
        }

	}
	return SOC_E_NONE;
}

STATIC int
__phy84328_init_ucode_bcst(int unit, int port, int cmd)
{
    uint16 data16;
    uint16 mask16;
    phy_ctrl_t *pc;
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
	int dbg_flag = 1;
#endif
    pc = EXT_PHY_SW_STATE(unit, port);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }
#endif

    if (cmd == PHYCTRL_UCODE_BCST_SETUP) {
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)	
        if (dbg_flag == 1) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84328 bcst setup: u=%d p=%d\n"), unit, port));
        }
#endif		
        /* Enable broadcast mode so all lanes get properly reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                    PHY84328_DEV1_BROADCAST_CTRL,
                    PHY84328_DEV1_BROADCAST_CTRL_BROADCAST_MODE_ENABLE_MASK,
                    PHY84328_DEV1_BROADCAST_CTRL_BROADCAST_MODE_ENABLE_MASK));
        return SOC_E_NONE;
    } else if (cmd == PHYCTRL_UCODE_BCST_uC_SETUP) {
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
        if (dbg_flag == 1) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84328 micro setup: u=%d p=%d\n"), unit, port));
        }
#endif
        /* apply bcst Reset to start download code from MDIO */
        mask16 = PHY84328_DEV1_GEN_CTRL_UCRST_MASK;
        data16 = mask16; /*set*/
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_GEN_CTRL,
                                           data16,mask16));

        /* clear : 1.ca18 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                           PHY84328_DEV1_GP_REG_0, 0));
        /* clear : 1.ca19 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                           PHY84328_DEV1_GP_REG_1, 0));
        /* clear : 1.ca1a */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                           PHY84328_DEV1_GP_REG_2, 0));

        mask16 = PHY84328_DEV1_PMD_CONTROL_REGISTER_RESET_MASK;
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_PMD_CONTROL_REGISTER,
                                           data16,mask16));

        /* Wait for 2.8 msec for 8051 to start */
        sal_udelay(2800);

        return SOC_E_NONE;
    } else if (cmd == PHYCTRL_UCODE_BCST_ENABLE) {
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
        if (dbg_flag == 1) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84328 bcst enable: u=%d p=%d\n"), unit, port));
        }
#endif
        /* In case reset clears bcst register */
        /* restore bcst register after reset */
        /* Enable broadcast mode */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                    PHY84328_DEV1_BROADCAST_CTRL,
                    PHY84328_DEV1_BROADCAST_CTRL_BROADCAST_MODE_ENABLE_MASK,
                    PHY84328_DEV1_BROADCAST_CTRL_BROADCAST_MODE_ENABLE_MASK));

        return SOC_E_NONE;
    } else if (cmd == PHYCTRL_UCODE_BCST_LOAD) {

		return _phy84328_init_ucode_bcst_load(unit, port);

    } else if (cmd == PHYCTRL_UCODE_BCST_END) {
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
        if (dbg_flag == 1) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84328 bcst end: u=%d p=%d\n"), unit, port));
        }
#endif
        /* make sure last code word is read by the micro */
        sal_udelay(20);


        /* Read Hand-Shake message (Done) from Micro */
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                                           PHY84328_DEV1_MSG_OUT, &data16));

        /* Download done message */
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)		
        if (dbg_flag == 1) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84328 MDIO firmware download done message=0x%x: u=%d p=%d\n"),
                      data16, unit, port));
        }
#endif
        sal_udelay(100); /* Wait for 100 usecs */

        /* Read checksum for Micro.
           The expected value is 0x0300 , 
           where upper_byte=03 means mdio download is done
           and lower byte=00 means no checksum error */
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                                           PHY84328_DEV1_MSG_OUT, &data16));

        /* Download done message */
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)		
        if (dbg_flag == 1) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84328 MDIO firmware download status message state 0x%x checksum 0x%x: "
                                 "u=%d p=%d\n"), (data16 >> 8) & 0xff, data16 & 0xff, unit, port));
        }
#endif
        /* Need to check if checksum is correct */
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                                           PHY84328_DEV1_GP_REG_4, &data16));

        if (data16 == 0x600D) {
			/* read Rev-ID */
			SOC_IF_ERROR_RETURN
				(READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
											   PHY84328_DEV1_VERSION, &data16));
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)											   
			if (dbg_flag == 1) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "PHY84328 Firmware revID=0x%x: u=%d p=%d \n"),
                                     data16, unit, port));
            }
#endif			
        }

        /* Disable broadcast mode */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                    PHY84328_DEV1_BROADCAST_CTRL,
                    0,
                    PHY84328_DEV1_BROADCAST_CTRL_BROADCAST_MODE_ENABLE_MASK));

		/* Go back to proper PMD mode */
		data16 = (PHY84328_SINGLE_PORT_MODE(pc)) ?
			PHY84328_DEV1_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK : 0;
		SOC_IF_ERROR_RETURN
			(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc,
											 PHY84328_DEV1_SINGLE_PMD_CTRL, data16,
											 PHY84328_DEV1_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK));

		/* Make sure micro completes its initialization */
		sal_udelay(5000);
        return SOC_E_NONE;
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "u=%d p=%d firmware_bcst: invalid cmd 0x%x\n"),
                   unit, port,cmd));
    }

    return SOC_E_FAIL;
}


/*
 * Function:
 *      phy_84328_precondition_before_probe
 * Purpose:
 *      When the device is in 40G, ie. after a power reset, and the 10G 
 *      port being probed does not have the 40G address in PRTAD, the 
 *      device will not respond until the port that has the 40G address
 *      is probed. To avoid not programming ports correctly, make
 *      sure the device is in quad (4x10G) PMD mode when the SDK port is 10G.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */

STATIC int
phy_84328_precondition_before_probe(int unit, phy_ctrl_t *pc)
{
    int ids;
    int rv = SOC_E_NOT_FOUND;
    uint16 pmd_ctrl;
    uint16 id0, id1;
    uint16 saved_phy_id;

    /* Not required when the port is single PMD */
    if (PHY84328_SINGLE_PORT_MODE(pc)) {
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
         * might not have the 84328 ids so this PMD setup will not be supported
         */
        id0 = 0;
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                                      PHY84328_DEV1_PMD_IDENTIFIER_REGISTER_0,
                                      &id0);
        if (id0 != 0x600d) {
            continue;
        }
        id1 = 0;
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 
                                      PHY84328_DEV1_PMD_IDENTIFIER_REGISTER_1,
                                      &id1);
        if (id1 != 0x8500) {
            continue;
        }

        /* Read device ids correctly so use this address to make sure device is in 10G */
        pmd_ctrl = 0;
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                      PHY84328_DEV1_SINGLE_PMD_CTRL, &pmd_ctrl);
        if (pmd_ctrl & PHY84328_DEV1_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Device responded to address %x. Setting quad PMD mode\n"), 
                      pc->phy_id));
            pmd_ctrl = 0;   /* Clear single PMD */
            SOC_IF_ERROR_RETURN(
                    MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84328_DEV1_SINGLE_PMD_CTRL, pmd_ctrl, 
                                            PHY84328_DEV1_SINGLE_PMD_CTRL_SINGLE_PMD_MODE_MASK));
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
 *      phy_84328_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */
STATIC int
_phy_84328_probe(int unit, phy_ctrl_t *pc)
{
    uint32 devid;
    uint16 chip_rev;

    SOC_IF_ERROR_RETURN(_phy_84328_config_devid(pc->unit, pc->port, pc, &devid));
    SOC_IF_ERROR_RETURN(
        READ_PHY84328_MMF_PMA_PMD_REG(unit, pc,
                                      PHY84328_DEV1_CHIP_REV_REGISTER,
                                      &chip_rev));

    if (devid == PHY84328_ID_84328) {
        if (chip_rev == 0x00a0) {
            pc->dev_name = dev_name_84328_a0;
        } else {
            pc->dev_name = dev_name_84328;
        }
    } else if (devid == PHY84328_ID_84324){
        pc->dev_name = dev_name_84324;
    } else if (devid == PHY84328_ID_84088){
        pc->dev_name = dev_name_84088;
    } else if (devid == PHY84328_ID_84024){
        pc->dev_name = dev_name_84024;
    } else {  /* not found */
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "port %d: BCM84xxx type PHY device detected, please use "
                             "phy_84<xxx> config variable to select the specific type\n"),
                  pc->port));
        return SOC_E_NOT_FOUND;
    }
    pc->size = sizeof(phy84328_dev_desc_t);
    return SOC_E_NONE;
}

/* 
 * Return the phy_id for the core 
 *
 * Currently assumes the addresses for the cores are 
 * 4 apart, e.g. core0_addr=n, core1_addr=n+4; core2_addr=n+8
 *
 * This can be modified to specify the address via a config property.
 * The config property spn_PORT_PHY_ADDR is already available to
 * specify the address for the port, which in a multicore port, 
 * refers to core0.
 */
STATIC uint16 
_phy_84328_phy_id_get(phy_ctrl_t *pc, int core)
{
    int phy_id = 0xff;
    int id_map[PHY84328_MAX_CORES] = { 0xff , 0xff, 0xff };

    id_map[core%PHY84328_MAX_CORES] =
        soc_property_port_suffix_num_get(
                    pc->unit, pc->port, 
                    core, spn_PORT_PHY_ADDR,
                    "core", id_map[core % PHY84328_MAX_CORES]);

    phy_id = 
        soc_property_port_get(
                    pc->unit, pc->port, spn_PORT_PHY_ADDR, phy_id);

    if (phy_id != id_map[core % PHY84328_MAX_CORES]) {
        return id_map[core % PHY84328_MAX_CORES];
    } else {
        return pc->phy_id + (core * 4);
    }
}


STATIC int
_phy_84328_bsc_rw(int unit, soc_port_t port, int dev_addr, int opr,
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

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_bsc_read: u=%d p=%d addr=%04x\n"),
                         unit, port, addr));

    if (!data_array) {
        return SOC_E_PARAM;
    }

    if (count > PHY84328_BSC_XFER_MAX) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    data_type = PHY84328_I2C_DATA_TYPE(opr);
    access_type = PHY84328_I2C_ACCESS_TYPE(opr);

    if (access_type == PHY84328_I2CDEV_WRITE) {
        for (i = 0; i < count; i++) {
            if (data_type == PHY84328_I2C_8BIT) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, ram_start + i,
                          ((uint8 *)data_array)[i]));
            } else {  /* 16 bit */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, ram_start + i,
                          ((uint16 *)data_array)[i]));
            }
        }
    }

    data16 = ram_start;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, 0x8004, data16));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, 0x8003, addr));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, 0x8002, count));

    data16 = 1;
    data16 |= (dev_addr<<9);
    if (access_type == PHY84328_I2CDEV_WRITE) {
        data16 |= PHY84328_WR_FREQ_400KHZ;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84328_MMF_PMA_PMD_REG(unit,pc,0x8005,data16));

    if (access_type == PHY84328_I2CDEV_WRITE) {
        data16 =  0x8000 | PHY84328_BSC_WRITE_OP;
    } else {
        data16 =  0x8000 | PHY84328_BSC_READ_OP;
    }

    if (data_type == PHY84328_I2C_16BIT) {
        data16 |= (1 << 12);
    }

	/* Select first i2c port */
	if (PHY84328_SINGLE_PORT_MODE(pc)) {
		SOC_IF_ERROR_RETURN
			(MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_MISC_CTRL,
											 0, PHY84328_DEV1_MISC_CTRL_I2C_PORT_SEL_MASK));
	} else {
        SOC_IF_ERROR_RETURN
             (MODIFY_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_DEV1_MISC_CTRL,
             (pc->phy_id & 0x3), PHY84328_DEV1_MISC_CTRL_I2C_PORT_SEL_MASK));
    }
		
	SOC_IF_ERROR_RETURN
		(WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, 0x8000, data16));

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    start = sal_time_usecs();
#endif
    soc_timeout_init(&to, 1000000, 0);
    while (!soc_timeout_check(&to)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, 0x8000, &data16));
        if (((data16 & PHY84328_2W_STAT) == PHY84328_2W_STAT_COMPLETE)) {
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
                         (data16 & PHY84328_2W_STAT), SAL_USECS_SUB(end, start)));
#endif

    if (access_type == PHY84328_I2CDEV_WRITE) {
        return SOC_E_NONE;
    }

    if ((data16 & PHY84328_2W_STAT) == PHY84328_2W_STAT_COMPLETE) {
        for (i = 0; i < count; i++) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, (ram_start+i), &data16));
            if (data_type == PHY84328_I2C_16BIT) {
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
 * Read a slave device such as NVRAM/EEPROM connected to the 84328's I2C
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
 *   phy_84328_i2cdev_read(0,2,0x50,0,100,data8);
 */
int
phy_84328_i2cdev_read(int unit,
                     soc_port_t port,
                     int dev_addr,  /* 7 bit I2C bus device address */
                     int offset,    /* starting data address to read */
                     int nbytes,    /* number of bytes to read */
                     uint8 *read_array)   /* buffer to hold retrieved data */
{
    return _phy_84328_bsc_rw(unit, port, dev_addr,PHY84328_I2CDEV_READ,
             offset, nbytes, (void *)read_array,PHY84328_READ_START_ADDR);

}

/*
 * Write to a slave device such as NVRAM/EEPROM connected to the 84328's I2C
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
 *   phy_84328_i2cdev_write(0,2,0x50,0,100,data8);
 */

int
phy_84328_i2cdev_write(int unit,
                     soc_port_t port,
                     int dev_addr,  /* I2C bus device address */
                     int offset,    /* starting data address to write to */
                     int nbytes,    /* number of bytes to write */
                     uint8 *write_array)   /* buffer to hold written data */
{
    int j;
    int rv = SOC_E_NONE;

    for (j = 0; j < (nbytes/PHY84328_BSC_WR_MAX); j++) {
        rv = _phy_84328_bsc_rw(unit, port, dev_addr,PHY84328_I2CDEV_WRITE,
                    offset + j * PHY84328_BSC_WR_MAX, PHY84328_BSC_WR_MAX,
                    (void *)(write_array + j * PHY84328_BSC_WR_MAX),
                    PHY84328_WRITE_START_ADDR);
        if (rv != SOC_E_NONE) {
            return rv;
        }
        sal_usleep(20000);
    }
    if (nbytes%PHY84328_BSC_WR_MAX) {
        rv = _phy_84328_bsc_rw(unit, port, dev_addr,PHY84328_I2CDEV_WRITE,
                offset + j * PHY84328_BSC_WR_MAX, nbytes%PHY84328_BSC_WR_MAX,
                (void *)(write_array + j * PHY84328_BSC_WR_MAX),
                PHY84328_WRITE_START_ADDR);
    }
    return rv;
}

/*
 * Function:
 *      _phy_84328_reg_read
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
STATIC int
_phy_84328_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
	uint16               data16;
    uint16               regdata;
    phy_ctrl_t          *pc;      /* PHY software state */
    int rv = SOC_E_NONE;
	int	rd_cnt;

    pc = EXT_PHY_SW_STATE(unit, port);

    rd_cnt = 1;

    if (flags & SOC_PHY_I2C_DATA8) {

        SOC_IF_ERROR_RETURN
            (phy_84328_i2cdev_read(unit, port,
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
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_I2C_TEMP_RAM,
                   &regdata));

        rv = _phy_84328_bsc_rw(unit, port,
               SOC_PHY_I2C_DEVAD(phy_reg_addr),
               PHY84328_I2C_OP_TYPE(PHY84328_I2CDEV_READ,PHY84328_I2C_16BIT),
               SOC_PHY_I2C_REGAD(phy_reg_addr),1,
               (void *)&data16,PHY84328_I2C_TEMP_RAM);

        *phy_data = data16;

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_I2C_TEMP_RAM,
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
 *      _phy_84328_reg_write
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
STATIC int
_phy_84328_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint8  data8[4];
    uint16 data16[2];
    uint16 regdata[2];
    phy_ctrl_t          *pc;      /* PHY software state */
    int rv = SOC_E_NONE;
    int wr_cnt;
    pc = EXT_PHY_SW_STATE(unit, port);

    wr_cnt = 1;

    if (flags & SOC_PHY_I2C_DATA8) {
        data8[0] = (uint8)phy_data;
        SOC_IF_ERROR_RETURN
            (phy_84328_i2cdev_write(unit, port,
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
            (READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_I2C_TEMP_RAM,
                 &regdata[0]));
        data16[0] = phy_data;
        rv = _phy_84328_bsc_rw(unit, port,
              SOC_PHY_I2C_DEVAD(phy_reg_addr),
              PHY84328_I2C_OP_TYPE(PHY84328_I2CDEV_WRITE,PHY84328_I2C_16BIT),
              SOC_PHY_I2C_REGAD(phy_reg_addr),wr_cnt,
              (void *)data16,PHY84328_I2C_TEMP_RAM);

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, PHY84328_I2C_TEMP_RAM,
                  regdata[0]));
    } else {
            SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, phy_reg_addr, (uint16)phy_data));
    }

    return rv;
}

void
_phy_84328_dbg_reg_read(int unit, soc_port_t port, int reg)
{
    uint16 data;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return;
    }
    READ_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, &data);
    LOG_CLI((BSL_META_U(unit,
                        "1.%04x=0x%04x\n"), reg, data));
}

void
_phy_84328_dbg_reg_write(int unit, soc_port_t port, int reg, uint16 val)
{
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return;
    }
    WRITE_PHY84328_MMF_PMA_PMD_REG(unit, pc, reg, val);
    _phy_84328_dbg_reg_read(unit, port, reg);
}

#ifdef PHY84328_MDIO_TRACING
STATIC int
_phy_84328_dbg_read(int unit, phy_ctrl_t *pc, uint32 addr, uint16 *data)
{
	int rv;
	rv = (pc->read)(unit, pc->phy_id, addr, data);

	if (DBG_FLAGS(pc) & PHY84328_DBG_F_REG) {
		LOG_CLI((BSL_META_U(unit,
                                    "p%d(%d): READ addr=%04x data=%04x\n"),
                         pc->port, pc->phy_id, addr, *data));
	}
	return rv;
		
}

STATIC int
_phy_84328_dbg_write(int unit, phy_ctrl_t *pc, uint32 addr, uint16 data)
{
	int rv;
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_REG) {
		LOG_CLI((BSL_META_U(unit,
                                    "p%d(%d): WRITE addr=%04x data=%04x\n"),
                         pc->port, pc->phy_id, addr, data));
	}
	rv = (pc->write)(unit, pc->phy_id, addr, data);
	return rv;
}

STATIC int
_phy_84328_dbg_modify(int unit, phy_ctrl_t *pc, uint32 addr, uint16 data, uint16 mask)
{
	int rv;
	if (DBG_FLAGS(pc) & PHY84328_DBG_F_REG) {
		LOG_CLI((BSL_META_U(unit,
                                    "p%d(%d): MODIFY addr=%04x data=%04x mask=%04x\n"),
                         pc->port, pc->phy_id, 
                         addr, data, mask));
	}
	rv = phy_reg_modify(unit, pc, addr, data, mask);
	return rv;
}
#endif /* PHY84328_MDIO_TRACING */



/******************************************************************************/


/*
 * Function:
 *      phy_84328_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */
STATIC int
phy_84328_probe(int unit, phy_ctrl_t *pc)
{
    int rv;
    char *dev_name = "";
    uint16 prim_phy_id;
    int core, all_cores;
    soc_port_t port;

    pc->size = 0;
    port = pc->port;
    all_cores = PHY84328_CORES(unit, port);
    if (all_cores > PHY84328_MAX_CORES) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                  "u=%d p=%d: too many cores %d\n"),
                  unit, port, all_cores));
        return SOC_E_NOT_FOUND;
    }
    prim_phy_id = pc->phy_id;
    for (core = 0; core < all_cores; core++) {
        
        /* 
         * During probe, reuse the pc for the port (first core only) as there are no
         * pc's allocated yet for the other cores
         */ 
        pc->phy_id = _phy_84328_phy_id_get(pc, core);

        rv = _phy_84328_probe(unit, pc);
        pc->phy_id = prim_phy_id;
        if (rv != SOC_E_NONE) {
            return rv;
        }
       
        /* 
         * Make sure the probed device name for all cores matches the first
         * core device  name 
         */
        if (core == 0) {
            dev_name = pc->dev_name;
        } else if (pc->dev_name != dev_name) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                      "u=%d p=%d: all cores must have same device: core0=%s != core%d=%s\n"),
                      unit, port, dev_name, core, pc->dev_name));
            return SOC_E_NOT_FOUND;
        }
    }

    /* 
     * Allocate one pc + one control block for each core
     */
    pc->size = sizeof(phy84328_dev_desc_t) + ((all_cores - 1) * PHY84328_CORE_STATE_SIZE);

    return SOC_E_NONE;
}

STATIC void
_phy_84328_init_setup(int unit, soc_port_t port)
{
    int core, all_cores;
    phy_ctrl_t *core_pc;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    
    /*	DBG_FLAGS(pc) = 0; */
	DBG_FLAGS(pc) = PHY84328_DBG_F_API_SET | PHY84328_DBG_F_DWLD;
	
    all_cores = PHY84328_CORES(unit, port);
    for (core = 0; core < all_cores; core++) {
        core_pc = PHY84328_CORE_PC(pc, core);

        if (core > 0) {
            /* Copy state from core 0 pc to other core pcs */
            sal_memcpy(core_pc, pc , sizeof(phy_ctrl_t));

            /* Populate phy_id for core1..core2 */
            core_pc->phy_id = _phy_84328_phy_id_get(pc, core);
        }


       CORE_NUM(core_pc) = core;
        if (all_cores > 1) {
            core_pc->phy_mode = PHYCTRL_MULTI_CORE_PORT;
        }
    }
}

STATIC int
phy_84328_init(int unit, soc_port_t port)
{
    int rv;
    int pass;
    int core, all_cores;
    phy_ctrl_t *core_pc;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    pass = PHYCTRL_INIT_STATE(pc);

    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
        _phy_84328_init_setup(unit, port);
    }

    /*
    if (DBG_FLAGS(pc) & PHY84328_DBG_F_API_SET) {
        bsl_normal(BSL_APPL_SHELL, unit,
                   "%s(%d, %d)\n", FUNCTION_NAME(), unit, port);
    }
    */

    all_cores = PHY84328_CORES(unit, port);
    for (core = 0; core < all_cores; core++) {
        core_pc = PHY84328_CORE_PC(pc, core);

        PHY84328_CORE_STATE_SET(unit, port, core, pc);
        /* copy init state to core_pc */
        PHYCTRL_INIT_STATE_SET(core_pc, pass);
        rv = _phy_84328_init(unit, port);
        PHY84328_CORE_STATE_SET(unit, port, 0, pc);

        if (rv != SOC_E_NONE) {
            return rv;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy84328_init_ucode_bcst(int unit, int port, int cmd)
{
    int rv;
    int core, all_cores;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    all_cores = PHY84328_CORES(unit, port);

    /*
     * The actual load needs to happen only on the first core as the
     * other cores will be in broadcast mode
     */
    if (cmd == PHYCTRL_UCODE_BCST_LOAD) {
        all_cores = 1;
    }
    for (core = 0; core < all_cores; core++) {
        PHY84328_CORE_STATE_SET(unit, port, core, pc);
        rv = __phy84328_init_ucode_bcst(unit, port, cmd);
        PHY84328_CORE_STATE_SET(unit, port, 0, pc);
        if (rv != SOC_E_NONE) {
            return rv;
        }
    }

    return SOC_E_NONE;
}

STATIC int
phy_84328_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{
    /*
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    */

    /* 
     * When array==NULL, this function is used to broadcast the firmware.
     * array==NULL should be specified only for internal use
     */
    /*
    if (DBG_FLAGS(pc) & PHY84328_DBG_F_API_SET) {
        bsl_normal(BSL_APPL_SHELL, unit,
                   "%s(%d, %d)\n", FUNCTION_NAME(), unit, port);
    }
    */

    if (array == NULL) {
        return _phy84328_init_ucode_bcst(unit, port, offset);
    } else {
        /*
        bsl_warning(BSL__X, unit,
                    "u=%d p=%d firmware download method not supported\n",
                    unit, port);
        */
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

STATIC int
phy_84328_enable_set(int unit, soc_port_t port, int enable)
{
    PHY84328_CALL_SET(unit, port, (_phy_84328_enable_set(unit, port, enable)));
}

STATIC int
phy_84328_speed_set(int unit, soc_port_t port, int speed)
{
    PHY84328_CALL_SET(unit, port, (_phy_84328_speed_set(unit, port, speed)));
}

STATIC int
phy_84328_speed_get(int unit, soc_port_t port, int *speed)
{
    int port_speed = 0;

    PHY84328_CALL_GET(unit, port, (_phy_84328_speed_get(unit, port, speed)), 
                      speed, port_speed);
}

STATIC int
phy_84328_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    PHY84328_CALL_SET(unit, port, (_phy_84328_interface_set(unit, port, pif)));
}

STATIC int
phy_84328_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    soc_port_if_t port_pif = 0;

    PHY84328_CALL_GET(unit, port, (_phy_84328_interface_get(unit, port, pif)), 
                      pif, port_pif);
}

STATIC int
phy_84328_lb_set(int unit, soc_port_t port, int enable)
{
    PHY84328_CALL_SET(unit, port, (_phy_84328_lb_set(unit, port, enable)));
}

STATIC int
phy_84328_lb_get(int unit, soc_port_t port, int *enable)
{
    int port_enable = 0;

    PHY84328_CALL_GET(unit, port, (_phy_84328_lb_get(unit, port, enable)), 
                      enable, port_enable);
}

STATIC int
phy_84328_link_get(int unit, soc_port_t port, int *link)
{
    int port_link = 0;

    PHY84328_CALL_GET(unit, port, (_phy_84328_link_get(unit, port, link)), 
                      link, port_link);
}


STATIC int
phy_84328_an_set(int unit, soc_port_t port, int an)
{
    PHY84328_CALL_SET(unit, port, (_phy_84328_an_set(unit, port, an)));
}


STATIC int
phy_84328_control_set(int unit, soc_port_t port,
                      soc_phy_control_t type, uint32 value)
{
    PHY84328_CALL_SET(unit, port,
                      (phy_84328_control_port_set(unit, port, type, value)));
}

STATIC int
phy_84328_control_get(int unit, soc_port_t port, 
                       soc_phy_control_t type, uint32 *value)
{
    uint32 port_value = 0;

    PHY84328_CALL_GET(unit, port,
                      (phy_84328_control_port_get(unit, port, type, value)),
                      value, port_value);
}

STATIC int
phy_84328_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }
#endif
    PHY84328_CALL_SET(unit, port,
                      (_phy_84328_reg_write(unit, port, 
                       flags, phy_reg_addr, phy_data)));
}

STATIC int
phy_84328_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint32 port_phy_data  = 0;

    PHY84328_CALL_GET(unit, port,
                      (_phy_84328_reg_read(
                          unit, port, flags, phy_reg_addr, phy_data)),
                      phy_data, port_phy_data);
}

STATIC int
phy_84328_ability_advert_set(int unit, soc_port_t port,
							 soc_port_ability_t *ability)
{
    PHY84328_CALL_SET(unit, port,
                      (_phy_84328_ability_advert_set(unit, port, ability)));
}

STATIC int
phy_84328_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    int rv;
    int core, all_cores;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    soc_port_ability_t port_ability;


    if (DBG_FLAGS(pc) & PHY84328_DBG_F_API_SET) {
        LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                   "%s(%d, %d)\n"), FUNCTION_NAME(), unit, port));
    }

    all_cores = PHY84328_CORES(unit, port);
    for (core = 0; core < all_cores; core++) {

        PHY84328_CORE_STATE_SET(unit, port, core, pc);
        rv = _phy_84328_ability_advert_get(unit, port, ability);
        PHY84328_CORE_STATE_SET(unit, port, 0, pc);

        if (rv != SOC_E_NONE) {
            return rv;
        }

        if (core > 0) {
            if (sal_memcmp((const void *) &port_ability,
                           (const void *) ability,
                           sizeof(soc_port_ability_t))){
                /*
                bsl_error(BSL__X, unit,
                          "84328 %s : ability does not match "
                          "for all cores: u=%d p=%d\n",
                          FUNCTION_NAME(), unit, port );
                */
            }
        } else if (all_cores > 1) {
            sal_memcpy(&port_ability, ability , sizeof(soc_port_ability_t));
        }
    }

    return SOC_E_NONE;
}


STATIC int
phy_84328_diag_ctrl(int unit, soc_port_t port, uint32 inst, int op_type,  int op_cmd,  void *arg)
{
	int rv = SOC_E_NONE;
    int core, all_cores;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    uint32 port_arg;


    if (DBG_FLAGS(pc) & PHY84328_DBG_F_API_SET) {
        LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                   "%s(%d, %d)\n"), FUNCTION_NAME(), unit, port));
    }

    all_cores = PHY84328_CORES(unit, port);
    for (core = 0; core < all_cores; core++) {

        PHY84328_CORE_STATE_SET(unit, port, core, pc);

        switch(op_type)
        {
            case  PHY_DIAG_CTRL_SET:
            case  PHY_DIAG_CTRL_CMD:
                rv =
                (_phy_84328_diag_ctrl(unit, port, inst, op_type,  op_cmd, arg));
                break;

            case  PHY_DIAG_CTRL_GET:
                rv =
                (_phy_84328_diag_ctrl(unit, port, inst, op_type,  op_cmd, arg));

                if (core > 0) {
                    if (sal_memcmp((const void *) &port_arg,
                                   (const void *) arg,
                                   sizeof(port_arg))){
                        LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                  "84328 %s : arg does not match "
                                  "for all cores: u=%d p=%d\n"),
                                  FUNCTION_NAME(), unit, port ));
                    }
                } else if (all_cores > 1) {
                    sal_memcpy(&port_arg, arg , sizeof(port_arg));
                }
                break;

            default:
                LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                           "84328 diag_ctrl bad operation: u=%d p=%d ctrl=0x%x\n"), 
                           unit, port, op_cmd));
                rv = SOC_E_UNAVAIL;
                break;
        }

        PHY84328_CORE_STATE_SET(unit, port, 0, pc);

        if (rv != SOC_E_NONE) {
            return rv;
        }

    }
    return SOC_E_NONE;
}


STATIC int
phy_84328_ability_remote_get(int unit, soc_port_t port, 
                            soc_port_ability_t *ability)
{
    int rv;
    int core, all_cores;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    soc_port_ability_t port_ability;


    if (DBG_FLAGS(pc) & PHY84328_DBG_F_API_SET) {
        LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                   "%s(%d, %d)\n"), FUNCTION_NAME(), unit, port));
    }

    all_cores = PHY84328_CORES(unit, port);
    for (core = 0; core < all_cores; core++) {

        PHY84328_CORE_STATE_SET(unit, port, core, pc);
        rv = _phy_84328_ability_remote_get(unit, port, ability);
        PHY84328_CORE_STATE_SET(unit, port, 0, pc);

        if (rv != SOC_E_NONE) {
            return rv;
        }

        if (core > 0) {
            if (sal_memcmp((const void *) &port_ability,
                           (const void *) ability,
                           sizeof(soc_port_ability_t))){
                /*
                LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                          "84328 %s : ability does not match "
                          "for all cores: u=%d p=%d\n"),
                          FUNCTION_NAME(), unit, port ));
                */
            }
        } else if (all_cores > 1) {
            sal_memcpy(&port_ability, ability , sizeof(soc_port_ability_t));
        }
    }

    return SOC_E_NONE;
}

/*
 * Variable:
 *    phy_84328_drv
 * Purpose:
 *    Phy Driver for 40G PHY.
 */
phy_driver_t phy_84328drv_xe = {
    "84328 40-Gigabit PHY Driver",
    phy_84328_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_84328_link_get,    /* Link get   */
    phy_84328_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_null_set,         /* Duplex set */
    phy_null_one_get,     /* Duplex get */
    phy_84328_speed_set,   /* Speed set  */
    phy_84328_speed_get,   /* Speed get  */
    phy_null_set,         /* Master set */
    phy_null_zero_get,    /* Master get */
    phy_84328_an_set,      /* ANA set */
    phy_84328_an_get,      /* ANA get */
    NULL,                 /* Local Advert set */
    NULL,                 /* Local Advert get */
    phy_null_mode_get,    /* Remote Advert get */
    phy_84328_lb_set,      /* PHY loopback set */
    phy_84328_lb_get,      /* PHY loopback set */
    phy_84328_interface_set, /* IO Interface set */
    phy_84328_interface_get, /* IO Interface get */
    NULL,   /* PHY abilities mask */
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_84328_control_set,    /* phy_control_set */
    phy_84328_control_get,    /* phy_control_get */
    phy_84328_reg_read,       /* phy_reg_read */
    phy_84328_reg_write,      /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    phy_84328_probe,         /* pd_probe  */
    phy_84328_ability_advert_set,  /* pd_ability_advert_set */
    phy_84328_ability_advert_get,  /* pd_ability_advert_get */
    phy_84328_ability_remote_get,  /* pd_ability_remote_get */
    phy_84328_ability_local_get,   /* pd_ability_local_get  */
    phy_84328_firmware_set,        /* pd_firmware_set */
    NULL,                          /* pd_timesync_config_set */
    NULL,                          /* pd_timesync_config_get */
    NULL,                          /* pd_timesync_control_set */
    NULL,                          /* pd_timesync_control_get */
    phy_84328_diag_ctrl,            /* .pd_diag_ctrl */
    NULL,                          /* pd_lane_control_set */
    NULL,                          /* pd_lane_control_get */
    NULL,                          /* pd_lane_reg_read */
    NULL,                          /* pd_lane_reg_write */
    NULL,                          /* pd_oam_config_set */
    NULL,                          /* pd_oam_config_get */
    NULL,                          /* pd_oam_control_set */
    NULL,                          /* pd_oam_control_get */
    NULL,                          /* pd_timesync_enhanced_capture_get */
    NULL,                          /* pd_multi_get */
    phy_84328_precondition_before_probe /* pd_precondition_before_probe */
};
#else /* INCLUDE_PHY_84328 */
typedef int _phy_84328_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_84328 */
