/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  $Copyright: (c) 2012 Broadcom Corp.
 *  All Rights Reserved.$
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: Main header file for TSCMod.
 *---------------------------------------------------------------------*/

#ifndef _TSCMOD_MAIN_H_
#define _TSCMOD_MAIN_H_

#if defined (_DV_TRIDENT2) || defined(STANDALONE)
#else
#define _SDK_TSCMOD_
#endif

#ifdef _SDK_TSCMOD_
#include <sal/types.h>
#endif

#ifndef STATIC
#define STATIC static
#endif  /* STATIC */

#ifndef __KERNEL__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif
#include "tscmod_defines.h"

#ifndef STANDALONE
#if defined (_DV_TRIDENT2)
  #define printf io_printf
#endif
#endif

#ifdef CADENCE
#include <veriuser.h>
#endif /* CADENCE */


#ifdef SYNOPSYS
#include <vcsuser.h>
#endif /* SYNOPSYS */

#ifdef _SDK_TSCMOD_
#include <sal/types.h>
#include <soc/error.h>
#include <soc/cm.h>
#include <soc/util.h>
#include <soc/cmic.h>
#include <soc/phyreg.h>
#include <shared/phyreg.h>
#endif


#ifdef _SDK_TSCMOD_
  #define _SAL_TYPES_H
  #define printf bsl_printf
#endif

#ifndef  _SDK_TSCMOD_
typedef unsigned char           uint8;          /* 8-bit quantity  */
typedef unsigned short          uint16;         /* 16-bit quantity */
typedef unsigned int            uint32;         /* 32-bit quantity */


/* from "sal/core/time.h" */
typedef uint32 sal_usecs_t;

/* from "soc/types.h" */
typedef int     soc_port_t;

/* dev 2 is almost always the testbench. */
#define SOC_MAX_NUM_DEVICES         2


#define SOC_MAX_NUM_PORTS       72 
#define NUM_PORTS               SOC_MAX_NUM_PORTS
#endif /*#ifndef _SDK_TSCMOD_ */


#ifndef _SDK_TSCMOD_
#define SOC_E_MULT_REG        1    /* this means no errors */
#define SOC_E_FUNC_NOT_FOUND  3    /* function not found. Continue search */
#define SOC_E_NONE        0
#define SOC_E_UNAVAIL    -1
#define SOC_E_FAIL       -2    /* generic */
#define SOC_E_TIMEOUT    -3
#define SOC_E_ERROR      -4
#define SOC_E_FUNC_ERROR -5    /* function found. But did not execute well. */
#define SOC_E_PARAM      -6
#else 
#define SOC_E_MULT_REG        1    /* this means no errors */
#define SOC_E_FUNC_NOT_FOUND  3    /* function not found. Continue search */
#define SOC_E_ERROR SOC_E_INTERNAL
#endif

#define INT_PHY_SW_STATE(unit, port)  (int_phy_ctrl[unit][port])

#define TSC_REG_READ(_unit, _pc, _flags, _reg_addr, _val) \
        tscmod_reg_aer_read((_unit), (_pc),  (_reg_addr), (_val))

#define TSC_REG_WRITE(_unit, _pc, _flags, _reg_addr, _val) \
        tscmod_reg_aer_write((_unit), (_pc),  (_reg_addr), (_val))

#define TSC_REG_MODIFY(_unit, _pc, _flags, _reg_addr, _val, _mask) \
        tscmod_reg_aer_modify((_unit), (_pc),  (_reg_addr), (_val),(_mask))

#define TSC_UDELAY(_time) sal_udelay(_time)

#define PRINTF_ERROR(fmt)    printf fmt /* from bcmdrv/systemInit.h */

#ifndef _SDK_TSCMOD_
#define SOC_IF_ERROR_RETURN(op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) \
            { PRINTF_ERROR((__FILE__ " : " #op " %d\n", __LINE__)); \
                    return(__rv__);} } \
                        while(0)
#endif   /*_SDK_TSCMOD_ */

#define CHK_RET_VAL_FUNC(op) \
    { int rv; if ((rv=(op)) != 0) \
         printf("ERROR %s:%d. #op# returned %d\n", __FILE__,__LINE__,rv);}

#define TSC_AER_BCST_OFS_STRAP   0x1ff
#define PHYCTRL_MDIO_ADDR_SHARE (1 << 0) /*use phy base addr. to access lanes */
#define PHYCTRL_MDIO_CL45       (1 << 1) /*use MDIO CL45 to access device */
#define PHY_AER_REG_ADDR_AER(_addr)    (((_addr) >> 16) & 0x0000FFFF)
#define PHY_AER_REG_ADDR_BLK(_addr)    (((_addr) & 0x000FFF0))
#define PHY_AER_REG_ADDR_REGAD(_addr)  ((((_addr) & 0x00008000) >> 11) | \
                                        ((_addr) & 0x0000000F))

#define PHY_AER_REG        0xFFDE
#define PHY_AER_REG_ADDR_CL45_DEVID(_addr) (((_addr) >> 27) & 0x0000001F)
#define PHY_AER_REG_ADDR_LANE_NUM(_addr)   (((_addr) >> 16) & 0x07FF)
#define PHY_AER_REG_ADDR_CL45_REGAD(_addr) (((_addr) & 0x000FFFF))

#ifndef _SDK_TSCMOD_
#define DEV_CFG_PTR(_pc) (&(((TSC40_DEV_DESC_t *)((_pc)->driver_data))->cfg))
#define SOC_PHY_CLAUSE45_ADDR(_devad,_regad) \
  			   ((((_devad) & 0x003F) << 16) | ((_regad) & 0xFFFF))
#endif

#ifndef _MDK_TSCMOD_
  #ifndef _SDK_TSCMOD_
     #define FALSE 0
     #define TRUE  1
  #endif   /*_SDK_TSCMOD_ */
#else
int getLane0Phyad(int phyad);
#endif /* _MDK_TSCMOD_ */

/* Borrowed from SDK.  This struct controls iterations and time durations
 * while executing a 'wait' or a 'poll'. No doxygen documentation for this.
*/
#ifndef _SDK_TSCMOD_
typedef struct soc_timeout_s
{
    sal_usecs_t		expire;
    sal_usecs_t		usec;
    int			min_polls;
    int			polls;
    int			exp_delay;
} soc_timeout_t;
#endif  /* !_SDK_TSCMOD_ */

#define TSC40_XGXSBLK0_XGXSSTATUSr  0x8001
#define TSC40_PLL_WAIT              2500
#define TSC40_RX_X4_STATUS1_PMA_PMD_LIVE_STATUSr 0xc150
#define TSC40_RX_X4_CONTROL0_PMA_CONTROL_0r      0xc137
#define TSC40_DSC2B0_DSC_MISC_CTRL0r             0xc21e
#define TSC40_DSC1B0_UC_CTRLr                    0xc20e
#define TSC40_CL72_MISC1_CONTROLr                0xc253

#define TSC40_VER_NAME_LEN            3

#define NUMBER_PCS_FUNCTION_TABLE_ENTRIES 100

#define TSCMOD_UC_MACRO_SHIFT          8
#define TSCMOD_UC_INIT                 1
#define TSCMOD_UC_OFFSET               2
#define TSCMOD_UC_SIZE                 3
#define TSCMOD_UC_W_OP                 4
#define TSCMOD_UC_W_DATA               5
#define TSCMOD_UC_STOP                 6
#define TSCMOD_UC_LOAD_STATUS          7
#define TSCMOD_UC_NO_CKSUM             8
#define TSCMOD_UC_RELEASE              9
#define TSCMOD_UC_R_CKSUM             10
#define TSCMOD_UC_R_VER               11
#define TSCMOD_UC_EXT_ON              12
#define TSCMOD_UC_EXT_OFF             13
#define TSCMOD_UC_R_OP                14
#define TSCMOD_UC_R_DATA              15
#define TSCMOD_UC_W_RAM_DATA          16
#define TSCMOD_UC_W_RAM_OP            17


#define TSCMOD_ENABLE                 0x1
#define TSCMOD_DISABLE                0x0

#define TSCMOD_MODEL_TSC              0x11 

#define TSCMOD_DBG_FLOW               0x1
#define TSCMOD_DBG_REG                0x2
#define TSCMOD_DBG_MDIO               0x4  
#define TSCMOD_DBG_PRINT              0x8
#define TSCMOD_DBG_FUNC              0x10
#define TSCMOD_DBG_SUB               0x40
#define TSCMOD_DBG_SPD              0x100  
#define TSCMOD_DBG_AN               0x400
#define TSCMOD_DBG_DSC             0x1000
#define TSCMOD_DBG_TXDRV           0x2000
#define TSCMOD_DBG_LINK            0x4000
#define TSCMOD_DBG_PATH            0x8000
#define TSCMOD_DBG_TO             0x10000
#define TSCMOD_DBG_INIT           0x40000
#define TSCMOD_DBG_CFG           0x100000
#define TSCMOD_DBG_PAT           0x400000
#define TSCMOD_DBG_UC           0x1000000
#define TSCMOD_DBG_FSM          0x2000000
#define TSCMOD_DBG_SCAN         0x4000000
#define TSCMOD_DBG_FLEX         0x8000000
#define TSCMOD_DBG_LOCK        0x10000000
#define TSCMOD_DBG_FW_LOAD     0x20000000
#define TSCMOD_DBG_WB          0x40000000

#define USE_CJPAT  1
#define USE_CRPAT  2 

#define TSCMOD_CTL_AN_CHIP_INIT         1
#define TSCMOD_CTL_AN_MODE_INIT         2
#define TSCMOD_CTL_AN_MODE_SHARE        3

#define TSCMOD_MISC_CTL_MASK           0xff
#define TSCMOD_MISC_CTL_SHIFT             8
#define TSCMOD_MISC_CTL_PORT_MODE_SELECT  1
#define TSCMOD_MISC_CTL_CL72_STAT_GET     2 
#define TSCMOD_MISC_CTL_DISABLE_DSC_SM    3 
#define TSCMOD_MISC_CTL_ENABLE_DSC_SM     4
#define TSCMOD_MISC_CTL_DISABLE_PLL       5
#define TSCMOD_MISC_CTL_ENABLE_PLL        6
#define TSCMOD_MISC_CTL_ENABLE_PWR_TIMER  7 
#define TSCMOD_MISC_CTL_MAIN_TAP_ADJUST_EN_0   8 
#define TSCMOD_MISC_CTL_TUNING_SM_EN      9  
#define TSCMOD_MISC_CTL_PORT_CLEAN_UP     10  
#define TSCMOD_MISC_CTL_CLK_SEL_156       11  
#define TSCMOD_MISC_CTL_PWR_DOWN          12
#define TSCMOD_MISC_CTL_NULL              13
#define TSCMOD_MISC_CTL_DSC_HW_RESET      14
#define TSCMOD_MISC_CTL_DSC_HW_ENABLE     15
#define TSCMOD_MISC_CTL_LFCK_BYPASS_0     16
#define TSCMOD_MISC_CTL_TEMP_DEFAULT      17 
#define TSCMOD_MISC_CTL_NONCE_MATCH       18
#define TSCMOD_MISC_CTL_AN_RESTART        19
#define TSCMOD_MISC_CTL_PDET_COMPL        20
#define TSCMOD_MISC_CTL_PDET_KX_COMPL     21

#define TSCMOD_SRESET_SHIFT                8
#define TSCMOD_SRESET_MASK              0xff
#define TSCMOD_SRESET_AN_UNLOCK            0
#define TSCMOD_SRESET_DSC_HW               1
#define TSCMOD_SRESET_DSC_SM               2
#define TSCMOD_SRESET_FIRMWARE_MODE        3
#define TSCMOD_SRESET_PORT_MODE            4
#define TSCMOD_SRESET_ANATX_INPUT          5
#define TSCMOD_SRESET_AFE_ANALOG           6
#define TSCMOD_SRESET_AFE_TX_ANALOG        7
#define TSCMOD_SRESET_AFE_RX_ANALOG        8 
#define TSCMOD_SRESET_AFE_TX_VOLT          9
#define TSCMOD_SRESET_CL72_LINK_FAIL_TIMER 10 
#define TSCMOD_SRESET_CL72_MAX_WAIT_TIMER  11 
#define TSCMOD_SRESET_TAP_MUXSEL           12
#define TSCMOD_SRESET_UC_NO_RESP           13
#define TSCMOD_SRESET_RX_SIG_DET           14
#define TSCMOD_SRESET_AER_ALL_PROXY        15
#define TSCMOD_SRESET_AER_ALL_PROXY_BLK_ADR  16
#define TSCMOD_SRESET_BAM37_ACOL_SWAP      17
#define TSCMOD_SRESET_HP_BAM_AM            18
#define TSCMOD_SRESET_HP_BAM_SWM_TIMER     19
#define TSCMOD_SRESET_AN_MODES             20
#define TSCMOD_SRESET_CL72_FW_MODE         21

#define TSCMOD_DIAG_PRBS_LANE_EN_GET        1
#define TSCMOD_DIAG_PRBS_POLYNOMIAL_GET     2
#define TSCMOD_DIAG_PRBS_INVERT_DATA_GET    3
#define TSCMOD_DIAG_PRBS_MODE_GET           4
#define TSCMOD_DIAG_PRBS_RX_INVERT_DATA_GET 5
#define TSCMOD_DIAG_PRBS_CHECK_MODE_GET     6
#define TSCMOD_DIAG_PRBS_CTRL_GET           7


#define TSCMOD_DIAG_P_HELP                 0
#define TSCMOD_DIAG_P_VERB                 1
#define TSCMOD_DIAG_P_LINK_UP_BERT         2
#define TSCMOD_DIAG_P_RAM_WRITE            3
#define TSCMOD_DIAG_P_CTRL_TYPE            4
#define TSCMOD_DIAG_P_AN_CTL               5 
#define TSCMOD_DIAG_P_SILICON              6
#define TSCMOD_DIAG_P_FEC_CTL              7

#define TSCMOD_DIAG_LINK_LIVE              0x2
#define TSCMOD_DIAG_LINK_LATCH             0x1

/* the cmd value for eyescan API */
#define TSC_UTIL_2D_LIVELINK_EYESCAN               0
#define TSC_UTIL_2D_PRBS_EYESCAN                   1
#define TSC_UTIL_ENABLE_LIVELINK                   2
#define TSC_UTIL_DISABLE_LIVELINK                  3
#define TSC_UTIL_ENABLE_DEADLINK                   4
#define TSC_UTIL_DISABLE_DEADLINK                  5
#define TSC_UTIL_SET_VOFFSET                       6
#define TSC_UTIL_SET_HOFFSET                       7
#define TSC_UTIL_GET_MAX_VOFFSET                   8
#define TSC_UTIL_GET_MIN_VOFFSET                   9
#define TSC_UTIL_GET_MAX_LEFT_HOFFSET              10
#define TSC_UTIL_GET_MAX_RIGHT_HOFFSET             11
#define TSC_UTIL_START_LIVELINK                    12
#define TSC_UTIL_START_DEADLINK                    13
#define TSC_UTIL_STOP_LIVELINK                     14
#define TSC_UTIL_CLEAR_LIVELINK                    15
#define TSC_UTIL_READ_LIVELINK                     16
#define TSC_UTIL_READ_DEADLINK                     17
#define TSC_UTIL_GET_INIT_VOFFSET                  18
#define TSC_UTIL_VEYE_U                            19  
#define TSC_UTIL_VEYE_D                            20  
#define TSC_UTIL_HEYE_R                            21  
#define TSC_UTIL_HEYE_L                            22 

#define TSCMOD_CTRL_TYPE_DEFAULT          0xc0000000
#define TSCMOD_CTRL_TYPE_MASK             0x3fffffff
#define TSCMOD_CTRL_TYPE_LB                      0x1
#define TSCMOD_CTRL_TYPE_RMT_LB                  0x2
#define TSCMOD_CTRL_TYPE_HG                      0x4
#define TSCMOD_CTRL_TYPE_LINK_BER                0x8
#define TSCMOD_CTRL_TYPE_FW_LOADED              0x10
#define TSCMOD_CTRL_TYPE_FAULT_DIS              0x20
#define TSCMOD_CTRL_TYPE_FW_AVAIL               0x40
#define TSCMOD_CTRL_TYPE_PRBS_ON                0x80
#define TSCMOD_CTRL_TYPE_CL72_WAR_ON           0x100
#define TSCMOD_CTRL_TYPE_UC_STALL              0x200
#define TSCMOD_CTRL_TYPE_CL72_TR_DIS           0x400
#define TSCMOD_CTRL_TYPE_MSG                   0x800
#define TSCMOD_CTRL_TYPE_ERR                  0x1000
#define TSCMOD_CTRL_TYPE_LINK_DIS             0x2000
#define TSCMOD_CTRL_TYPE_SEMA_CHK             0x4000
#define TSCMOD_CTRL_TYPE_SEMA_ON              0x8000
#define TSCMOD_CTRL_TYPE_PCB_STALL0          0x10000
#define TSCMOD_CTRL_TYPE_PCB_STALL1          0x20000
#define TSCMOD_CTRL_TYPE_UC_CAP_RXP          0x40000
#define TSCMOD_CTRL_TYPE_UC_RXP              0x80000
#define TSCMOD_CTRL_TYPE_UC_CL72_FW         0x100000
#define TSCMOD_CTRL_TYPE_UC_WR_MASK_EN      0x200000
#define TSCMOD_CTRL_TYPE_ALL_PROXY          0x400000
#define TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR  0x800000
#define TSCMOD_CTRL_TYPE_SOME_PROXY        0x1000000
#define TSCMOD_CTRL_TYPE_HPAM_AUTODET      0x2000000
#define TSCMOD_CTRL_TYPE_WB_DIS            0x4000000
#define TSCMOD_CTRL_TYPE_SILICON           0x8000000
#define TSCMOD_CTRL_TYPE_TX_SQUELCH       0x10000000
#define TSCMOD_CTRL_TYPE_FS_FEC_ON        0x20000000

#define TSCMOD_MSG_FW_CHIP_VER_UNKNOWN              0x1
#define TSCMOD_MSG_PORT_MODE_SEL_RST                0x2
#define TSCMOD_MSG_UC_STALL                         0x4
#define TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK  0xf0
#define TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_SHIFT    4
#define TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK 0xf00
#define TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_SHIFT    8

#define TSCMOD_ERR_PLLDIV_DIFF                   0x1
#define TSCMOD_ERR_AN_PLL_DIFF                   0x2
#define TSCMOD_ERR_AN_BAD_CFG                    0x4
#define TSCMOD_ERR_FS_SPD_MODE                   0x8
#define TSCMOD_ERR_UC_SYNC_CMD_TO               0x10
#define TSCMOD_ERR_REG_UC_SYNC_CMD_TO           0x20
#define TSCMOD_ERR_PLLDIV_INIT_PLL           0x10000
#define TSCMOD_ERR_PLLDIV_SPD_SET            0x20000
#define TSCMOD_ERR_UC_FW_ARG                 0x40000
#define TSCMOD_ERR_UC_CLR_FAIL_CL72          0x80000
#define TSCMOD_ERR_UC_CLR_FAIL_RXP          0x100000

#define TSCMOD_CFG_CTRL_FLAG_WR_MASK_EN          0x1

#define TSCMOD_TX_LANE_TRAFFIC            0x10 
#define TSCMOD_TX_LANE_RESET              0x20

#define TSCMOD_UC_PROXY_WAIT_TIME         425000
#define TSCMOD_UC_PROXY_WAIT_TIME_STRIKE1 525000
#define TSCMOD_UC_PROXY_WAIT_TIME_STRIKE2 625000

#define TSCMOD_UC_PROXY_LOOP_CNT             500
#define TSCMOD_UC_PROXY_CNT_CAP                3

#define TSCMOD_UC_SYNC_REQ_MASK              0x1
#define TSCMOD_UC_SYNC_RDY_MASK              0x2
#define TSCMOD_UC_SYNC_BUSY_MASK             0x4
#define TSCMOD_UC_SYNC_RAM_MASK              0x8 
#define TSCMOD_UC_SYNC_WR_MASK              0x10 
#define TSCMOD_UC_SYNC_DONE_MASK          0x8000

#define TSCMOD_UC_SYNC_REQ_SHIFT             0
#define TSCMOD_UC_SYNC_RDY_SHIFT             1
#define TSCMOD_UC_SYNC_BUSY_SHIFT            2
#define TSCMOD_UC_SYNC_RAM_SHIFT             3 
#define TSCMOD_UC_SYNC_WR_SHIFT              4 
#define TSCMOD_UC_SYNC_DONE_SHIFT           15

/* TSCMOD HG2 CONTROL */
#define TSCMOD_HG2_CONTROL_DISABLE_ALL     0x0
#define TSCMOD_HG2_CONTROL_ENABLE_ALL      0x1
#define TSCMOD_HG2_CONTROL_DISABLE_CODEC   0x2
#define TSCMOD_HG2_CONTROL_ENABLE_CODEC    0x4
#define TSCMOD_HG2_CONTROL_READ_CODEC      0x8

/* TSCMOD PDET CONTROL */
#define TSCMOD_PDET_CONTROL_1G         0x1
#define TSCMOD_PDET_CONTROL_10G        0x2
#define TSCMOD_PDET_CONTROL_MASK       0x4
#define TSCMOD_PDET_CONTROL_SHIFT      0x2
#define TSCMOD_PDET_CONTROL_CMD_MASK   0x8
#define TSCMOD_PDET_CONTROL_CMD_SHIFT  0x3

/* for per_lane_control */
#define TSCMOD_UC_SYNC_RAM_MODE         0x1
#define TSCMOD_UC_SYNC_WR_MODE          0x2
#define TSCMOD_UC_SYNC_REG_MODE         0x0
#define TSCMOD_UC_SYNC_RD_MODE          0x0
#define TSCMOD_UC_SYNC_MASK_SHIFT         4

/*
#define TSCMOD_UC_SYNC_CMDr             0xf010
#define TSCMOD_UC_SYNC_LADDr            0xf011
#define TSCMOD_UC_SYNC_HADDr            0xf015
#define TSCMOD_UC_SYNC_DATAr            0xffc3
*/
#define TSCMOD_UC_SYNC_BLOCK            0xffc0
#define TSCMOD_UC_SYNC_CMDr             0xffc8
#define TSCMOD_UC_SYNC_LADDr            0xffc0
#define TSCMOD_UC_SYNC_HADDr            0xffc1
#define TSCMOD_UC_SYNC_DATAr            0xffc3

#define TSCMOD_FAULT_CL49               0x1
#define TSCMOD_FAULT_CL48               0x2
#define TSCMOD_FAULT_CL82               0x4
#define TSCMOD_FAULT_RF                 0x1
#define TSCMOD_FAULT_LF                 0x2
#define TSCMOD_FAULT_LI                 0x4

#define TSCMOD_REG_RD    0
#define TSCMOD_REG_WR    1
#define TSCMOD_REG_MO    2

#define TSCMOD_CL72_AN_MASK             0xf0
#define TSCMOD_CL72_AN_FORCED_ENABLE    0x30
#define TSCMOD_CL72_AN_FORCED_DISABLE   0x70
#define TSCMOD_CL72_AN_NO_FORCED        0x10

#define TSCMOD_CL72_HW_MASK             0x0f
#define TSCMOD_CL72_HW_ENABLE           0x03
#define TSCMOD_CL72_HW_DISABLE          0x01
#define TSCMOD_CL72_HW_RESTART          0x08

#define TSCMOD_CL72_STATUS_RESET       0x100
#define TSCMOD_CL72_STATUS_READ        0x200

#define TSCMOD_SYMM_PAUSE      0x1
#define TSCMOD_ASYM_PAUSE      0x2

#define TSCMOD_ABILITY_SYMM_PAUSE   0x10000
#define TSCMOD_ABILITY_ASYM_PAUSE   0x20000

#define TSCMOD_AN_SET_RF_DISABLE   0x400 
#define TSCMOD_AN_SET_SGMII_SPEED  0x100 
#define TSCMOD_AN_SET_SGMII_MASTER 0x200 
#define TSCMOD_AN_SET_HG_MODE     0x1000
#define TSCMOD_AN_SET_FEC_MODE    0x2000
#define TSCMOD_AN_SET_CL72_MODE   0x4000
#define TSCMOD_AN_SET_CL37_ATTR   0x8000
#define TSCMOD_AN_SET_CL48_SYNC  0x10000
#define TSCMOD_AN_SET_LK_FAIL_INHIBIT_TIMER_NO_CL72  0x20000
#define TSCMOD_AN_SET_CL73_FEC_OFF                   0x40000
#define TSCMOD_AN_SET_CL73_FEC_ON                    0x80000


#define TSCMOD_COLD_RESET_MODE_MASK                0xff
#define TSCMOD_COLD_RESET_MODE_SHIFT                  8
#define TSCMOD_COLD_RESET_BASIC                     0x1
#define TSCMOD_AN_CTL_LINK_FAIL_CL72_WAIT_TIMER     0x2 
#define TSCMOD_AN_CTL_LINK_FAIL_NOCL72_WAIT_TIMER   0x4
#define TSCMOD_AN_CTL_CL73_ERROR_TIMER              0x8
#define TSCMOD_PLL_VCO_STEP                        0x10
#define TSCMOD_RAM_BASE_ADDR                       0x20


#define TSCMOD_AN_GET_CL73_ABIL      0xa
#define TSCMOD_AN_GET_CL37_ABIL      0xb
#define TSCMOD_AN_GET_MISC_ABIL      0xc
#define TSCMOD_AN_GET_LP_CL73_ABIL  0xa8
#define TSCMOD_AN_GET_LP_CL37_ABIL  0xb8
#define TSCMOD_AN_GET_LP_MISC_ABIL  0xc8
#define TSCMOD_AN_GET_LP_SGMII_ABIL 0xd8

#define TSCMOD_TAP_KX               0x1
#define TSCMOD_TAP_OS               0x2
#define TSCMOD_TAP_BR               0x4
#define TSCMOD_TAP_KR               0x8
#define TSCMOD_TAP_2P5             0x10
#define TSCMOD_TAP_REG             0x20


/* internal if macro */
#define TSCMOD_IIF_XLAUI           0x10
#define TSCMOD_IIF_SR4             0x20 
#define TSCMOD_IIF_SFPDAC          0x40 
#define TSCMOD_IIF_NOXFI           0x80 

#define TSCMOD_DIAG_CL72_DONE       0x0
#define TSCMOD_DIAG_CL72_START      0x1 
#define TSCMOD_DIAG_CL72_LOCK       0x2 
#define TSCMOD_DIAG_CL72_RX_TRAINED 0x4 

#define TSCMOD_DIAG_PMD_LOCK_LIVE   0x0
#define TSCMOD_DIAG_PMD_LOCK_LATCH  0x1 
#define TSCMOD_DIAG_PMD_INFO_LIVE   0x2
#define TSCMOD_DIAG_PMD_INFO_LATCH  0x4
#define TSCMOD_DIAG_PMD_INFO_LOCK_BIT   0x1
#define TSCMOD_DIAG_PMD_INFO_SDET_BIT   0x2

#define TSCMOD_DIAG_AN_MODE_CL73      0x1
#define TSCMOD_DIAG_AN_MODE_CL37      0x2
#define TSCMOD_DIAG_AN_MODE_CL73BAM   0x4
#define TSCMOD_DIAG_AN_MODE_CL37BAM   0x8
#define TSCMOD_DIAG_AN_MODE_HPAM     0x10
#define TSCMOD_DIAG_AN_MODE_SGMII    0x20

#define TSCMOD_FWMODE_RD            0x0
#define TSCMOD_FWMODE_WR            0x1

#define TSCMOD_RXP_REG_OFF          0x0
#define TSCMOD_RXP_REG_ON           0x1
#define TSCMOD_RXP_UC_OFF           0x2
#define TSCMOD_RXP_UC_ON            0x3
#define TSCMOD_RXP_REG_RD           0xa
#define TSCMOD_RXP_UC_RD            0xb
#define TSCMOD_RXP_UC_CLR           0xc

#define TSCMOD_UC_CAP_ALL           0x1
#define TSCMOD_UC_CAP_RXP           0x2

#define TSCMOD_UC_L0                0x0
#define TSCMOD_UC_L1                0x1
#define TSCMOD_UC_L2                0x2
#define TSCMOD_UC_L3                0x3
#define TSCMOD_UC_CORE              0x4
#define TSCMOD_UC_D0                0x5
#define TSCMOD_UC_D1                0x6

#define TSCMOD_UC_CL72_SWITCH_OVER  0x2
#define TSCMOD_UC_CREDIT_PROGRAM    0x3
#define TSCMOD_UC_CL37_HR2_WAR      0x4
#define TSCMOD_UC_CL72_CLEAR        0x5
#define TSCMOD_UC_CL72_WAIT_CLEAN   0x6
#define TSCMOD_UC_CL72_DEBUG_INFO   0x7


#define TSCMOD_FEC_VAL_MASK           0x0f
#define TSCMOD_FEC_CTRL_MASK          0xf0
#define TSCMOD_FEC_LANE_MASK         0xf00
#define TSCMOD_FEC_LANE_OVRW_MASK   0xf000
#define TSCMOD_FEC_SUB_MODE_MASK  0xff0000
#define TSCMOD_FEC_CTRL_SHIFT            4
#define TSCMOD_FEC_LANE_SHIFT            8
#define TSCMOD_FEC_LANE_OVRW_SHIFT      12
#define TSCMOD_FEC_SUB_MODE_SHIFT       16
#define TSCMOD_FEC_CTL_CNTR            0x0
#define TSCMOD_FEC_CTL_CC              0x1
#define TSCMOD_FEC_CTL_UC              0x2
#define TSCMOD_FEC_CTL_INJECT          0x3
#define TSCMOD_FEC_CTL_FS_RD           0x4
/* FS: forced speed */
#define TSCMOD_FEC_CTL_FS_SET_ON       0x5
#define TSCMOD_FEC_CTL_FS_SET_OFF      0x6
#define TSCMOD_FEC_CTL_AN_CL73_RD      0x7
#define TSCMOD_FEC_CTL_AN_CL37_RD      0x8
#define TSCMOD_FEC_SUB_MODE_ERR0       0x1

/* for cidL */
#define TSC623_SDK_42585_A1         0x1
#define TSC623_SDK_45197            0x2
#define TSC623_PHY_887              0x8
#define TSC623_PHY_891             0x10
#define TSC623_PHY_884             0x20 
#define TSC623_PHY_896             0x40 
#define TSC623_PHY_867             0x80 
#define TSC623_SDK_45418          0x200
#define TSC623_SDK_45283          0x400
#define TSC623_SDK_45731          0x800
#define TSC623_PHY_916           0x1000
#define TSC623_PHY_892           0x4000
#define TSC623_SDK_45682         0x8000
#define TSC623_SDK_45789_A1     0x10000
#define TSC623_SDK_46061        0x20000
#define TSC623_SDK_46060        0x40000
#define TSC623_PHY_917         0x200000
#define TSC623_PHY_863         0x400000
#define TSC623_PHY_864         0x800000
#define TSC623_SDK_46338      0x1000000
#define TSC623_SDK_43138      0x2000000
#define TSC623_SDK_46475      0x4000000
#define TSC623_SDK_46753      0x8000000
#define TSC623_SDK_46913     0x10000000
#define TSC623_SDK_46955     0x40000000

#define TSC623_SDK_46725_P0         0x1
#define TSC623_SDK_46725_P1         0x2
#define TSC623_SDK_47112           0x10
#define TSC623_SDK_47114           0x20
#define TSC623_SDK_46764           0x80
#define TSC623_SDK_46923          0x200
#define TSC623_PHY_999            0x400
#define TSC623_PHY_926            0x800
#define TSC623_SDK_47195         0x1000
#define TSC623_SDK_47058         0x2000
#define TSC623_SDK_47365         0x4000
#define TSC623_PHY_1015          0x8000
#define TSC623_SDK_46788        0x10000
#define TSC623_PHY_965          0x20000
#define TSC623_PHY_1023         0x80000
#define TSC623_SDK_46785       0x100000
#define TSC623_SDK_46786       0x200000
#define TSC623_PHY_1013        0x400000
#define TSC623_PHY_1073       0x1000000
#define TSC623_SDK_48904      0x2000000
#define TSC623_PHY_1100       0x4000000
#define TSC623_SDK_48705      0x8000000
#define TSC628_PHY_1095      0x10000000
#define TSC628_PHY_1108      0x20000000
#define TSC628_PHY_1105      0x80000000

#define TSC628_PHY_1111             0x1
#define TSC628_SDK_49139            0x2
#define TSC628_PHY_1116             0x4
#define TSC628_PHY_1119             0x8
#define TSC628_SDK_49889           0x10
#define TSC628_SDK_51300           0x20
#define TSC628_PHY_1092           0x100
#define TSC628_SDK_49465          0x200
#define TSC628_PHY_1123          0x1000
#define TSC628_SDK_51441         0x2000
#define TSC628_SDK_49835         0x4000
#define TSC628_SDK_51674         0x8000
#define TSC628_SDK_50095        0x10000
#define TSC628_PHY_1166         0x20000
#define TSC628_SDK_48752       0x100000
#define TSC628_SDK_47653       0x200000
#define TSC628_SDK_48449       0x400000
#define TSC628_SDK_51547       0x800000
#define TSC628_SDK_52038      0x1000000
#define TSC628_SDK_50212      0x2000000
#define TSC628_SDK_48449_P2   0x4000000
#define TSC628_SDK_52713      0x8000000
#define TSC628_SDK_52241     0x10000000
#define TSC628_SDK_52715     0x20000000
#define TSC628_SDK_52833     0x40000000
#define TSC628_SDK_53261     0x80000000

#define TSC640_SDK_39435            0x1
#define TSC640_SDK_53070            0x4
#define TSC640_SDK_53940            0x8
#define TSC640_SDK_54357           0x10
#define TSC640_SDK_54950           0x20
#define TSC640_SDK_48996           0x80
#define TSC640_SDK_54392          0x100
#define TSC640_SDK_54943          0x200
#define TSC640_SDK_54606          0x800
#define TSC640_SDK_53855         0x1000
#define TSC640_SDK_55448         0x2000
#define TSC640_SDK_56119         0x4000
#define TSC640_SDK_56749         0x8000
#define TSC640_SDK_55315        0x10000
#define TSC640_SDK_57764        0x20000
#define TSC640_SDK_58338        0x40000

#define TSCMOD_MODEL_REVID_A0        0x00
#define TSCMOD_MODEL_REVID_A1        0x01
#define TSCMOD_MODEL_REVID_A2        0x02

#define TSCMOD_MODEL_PROJ_TD2      0x0001  
#define TSCMOD_MODEL_PROJ_HR2      0x0002
#define TSCMOD_MODEL_TYPE_MASK     0x000f 
#define TSCMOD_MODEL_LET_MASK      0x00f0  
#define TSCMOD_MODEL_REV_MASK      0x0f00
#define TSCMOD_MODEL_PROJ_MASK     0xf000
#define TSCMOD_MODEL_LET_SHIFT      4
#define TSCMOD_MODEL_REV_SHIFT      8
#define TSCMOD_MODEL_PROJ_SHIFT    12
#define TSCMOD_MODEL_TYPE_IS(_tsc)   \
   ((_tsc)->model_type)

#define TSCMOD_MODEL_TYPE_GET(_tsc)  \
   ((_tsc->model_type)&TSCMOD_MODEL_TYPE_MASK)

#define TSCMOD_MODEL_TYPE_SET(_tsc,_val) \
   ((_tsc->model_type)=(((_tsc->model_type) & ~TSCMOD_MODEL_TYPE_MASK)|(_val&TSCMOD_MODEL_TYPE_MASK)))

#define TSCMOD_MODEL_LET_GET(_tsc) \
   (((_tsc->model_type)&TSCMOD_MODEL_LET_MASK)>>TSCMOD_MODEL_LET_SHIFT)

#define TSCMOD_MODEL_LET_SET(_tsc,_val) \
   ((_tsc->model_type)=(((_tsc->model_type)& ~TSCMOD_MODEL_LET_MASK)|((_val<<TSCMOD_MODEL_LET_SHIFT)&TSCMOD_MODEL_LET_MASK)))

#define TSCMOD_MODEL_REV_GET(_tsc) \
   (((_tsc->model_type)&TSCMOD_MODEL_REV_MASK)>>TSCMOD_MODEL_REV_SHIFT)

#define TSCMOD_MODEL_REV_SET(_tsc,_val) \
   ((_tsc->model_type)=(((_tsc->model_type)& ~TSCMOD_MODEL_REV_MASK)|((_val<<TSCMOD_MODEL_REV_SHIFT)&TSCMOD_MODEL_REV_MASK)))

#define TSCMOD_MODEL_REVID_GET(_tsc) \
   ((((_tsc->model_type)&TSCMOD_MODEL_REV_MASK)>>TSCMOD_MODEL_REV_SHIFT)|((_tsc->model_type)&TSCMOD_MODEL_LET_MASK))

#define TSCMOD_MODEL_PROJ_GET(_tsc) \
   (((_tsc->model_type)&TSCMOD_MODEL_PROJ_MASK)>>TSCMOD_MODEL_PROJ_SHIFT)

#define TSCMOD_MODEL_PROJ_SET(_tsc,_val) \
   ((_tsc->model_type)=(((_tsc->model_type)& ~TSCMOD_MODEL_PROJ_MASK)|((_val<<TSCMOD_MODEL_PROJ_SHIFT)&TSCMOD_MODEL_PROJ_MASK)))

#define TSCMOD_TX_PI_MODE_MASK     0xf
#define TSCMOD_TX_PI_PPM_RESET     0x1
#define TSCMOD_TX_PI_PPM_SET       0x2
#define TSCMOD_TX_PI_PPM_RD        0x3

#define TSCMOD_LPI_BYPASS 0x10 /**< enable/disable LPI Bypass the SerDes */
#define TSCMOD_LPI_BYPASS_SET(enable) (enable |= TSCMOD_LPI_BYPASS)
#define TSCMOD_LPI_BYPASS_CLR(enable) (enable &= ~TSCMOD_LPI_BYPASS)
#define TSCMOD_LPI_BYPASS_GET(enable) (enable & TSCMOD_LPI_BYPASS ? 1 : 0)

#define TSCMOD_DIAG_PRBS_POLY7        0
#define TSCMOD_DIAG_PRBS_POLY9        1
#define TSCMOD_DIAG_PRBS_POLY11       2
#define TSCMOD_DIAG_PRBS_POLY15       3
#define TSCMOD_DIAG_PRBS_POLY23       4
#define TSCMOD_DIAG_PRBS_POLY31       5
#define TSCMOD_DIAG_PRBS_POLY58       6

/*
#define BRCM_TSCMOD_DEBUG  1
*/

/* This struct is a building block of a string associated function table.
 * It is used to override native PCS Tier1 functions. No doxygen documentation
 * for this.
 */
typedef struct strToPCSFunction {
  char* p;
  int (*fp)(tscmod_st*);
} str2PCSFn;

#include "tscmod.h"


extern str2PCSFn str2PCSFunc;

extern void              tscmod_init_called(int x);
extern void              tscmod_init(void);
extern void              tscmod_tier1_selector(const char* s,tscmod_st* c, int *retVal);
extern void              tscmod_read_cfg(const char* fileName);
extern void              print_config(int unit, int port);
extern int               tscmod_searchPCSFuncTable(const char* selStr, tscmod_st* ws); 
extern void              copy_tscmod_st(tscmod_st *from, tscmod_st *to);
/* extern int               isLaneEnabled(tscmod_lane_select ls, int lane); */
/* extern tscmod_model_type  getGenericModelType(tscmod_model_type model); */
extern const char*       getLaneStr(tscmod_lane_select ls);
extern tscmod_spd_intfc_type tscmod_spd_intf_s2e(char* s);
extern const char*       tscmod_aspd_e2s(tscmod_aspd_type aspd_e) ;
extern const char*       tscmod_aspd_vec2s(int vec) ;
extern tscmod_aspd_type  tscmod_spd2aspd(tscmod_st* ws) ;  
extern int               getLaneSelect(int lane) ;

/*!
\mainpage  TSCMOD Documentation (Beta)

This is the documentation site for TSCMod (<b>T</b>DM <b>S</b>erdes controller</b> <b>C</b>onfiguration <b>MOD</b>ule)

\version 0.2

\author TSCMod Team

\date 06/25/10

<B>Introduction</B><br> 

TDM Serdes controller Configuration Module (TSCMod)  is a source-code/binary distributable 
multi tiered library that defines a hardware abstraction layer for various
configuration modes of the broadcom TDM serdes IP. This document expects
the reader to be familiar with the TSC architecture.

The tiered structure progressively encapsulates register accesses and sequence
hiding complexities like indirect addressing (Address extension registers or
AER) and access protocol (MDIO CL22/CL45) implementation. The Tier1 layer,
also called TSCMod Procedures are building blocks to construct comprehensive
TSC Tier2 layers also called TSCMod sequences.

Tier1 procedures are fixed sequences of register accesses, accomplishing a
specific sub-configuration.  In general they are mode aware (combo or
independent) and only affect independent ports.

The full list of Tier1 procedures are tabulated below.

TSCMod architecture documentation is created using doxygen and so follows the
code structure. Please use the following guidelines to use the documentation.

The documentation is stored in tabs. If you are reading this you are in the
main tab. (See the top left corner of our browser). The other tabs you will
see are 'classes' where the #tscmod_st struct is visible and 'files', where following files are visible.

\li tscmod_cfg_seq.c
\li tscmod_enum_defines.c
\li tscmod_main.c
\li tscmod_phyreg.c

\li tscmod_defines.h
\li tscmod_enum_defines.h
\li tscmod_main.h
\li tscmod_phyreg.h

All Tier1 procedures have a fixed signature. They return an error code and
accept a reference to a C struct (called #tscmod_st). #tscmod_st is used to pass
input parameters to the Tier1 procedures and occassionally stores a return
value from the Tier1 procedure. #tscmod_st remembers the context of one TSC
(not one port or one lane). If the system has multiple TSCs, the software
platform must create and maintain that many #tscmod_st structures.

TSCs can be programmed per lane or per port (independent lane mode or
combo mode). Platforms control the lanes on which Tier1 procedures operate by
setting the #tscmod_st.lane_select element. The lane specific information is set
in the #tscmod_st.per_lane_control element which has fields dedicated to each of
the 4 lanes. In combo mode, the lane_select element should be set to zero and
any inputs to the Tier1 functions should be set in the lane 0 fields.

Tier1 procedures have no programmatic order dependencies. This implies that the
effect of a Tier1 is constant regardless of the state of the TSC.

The exceptions are 
\li #tscmod_set_port_mode function must be called before others.
\li Order mandated by TSC architecture.
\li Aggregate registers which control features of multiple lanes in TSC.

<B>TSCMod interface to System Platforms</B><br> 
     
TSCMod has an unambiguous interface mechanism to system platforms. Platforms can
be software systems like MDK, and SDK or customer specific software. It can
also be validation platforms including HDL (verilog/systemVerilog/Vera) based
testbenches.

TSCMod functions (tier1 and tier2) operates on a single TSC at a time. It
can be used to access multiple lanes within the TSC, but cannot access
multiple TSCs simultaneously. To access different TSCs multiple calls
to tier1 procedures are needed.

TSCMod defines a structure (called tscmod_st) which encapsulates all information
of a TSC needed by TSCMod. This structure should be created and maintained
by system platforms, one per TSC.  Typically an array of tscmod_st structs
is created, accessible by the TSC's unique physical address.

All Tier1 procedures are called with a reference to a tscmod_st struct. of the
TSC being accessed. TSCMod will read and write information to the struct.
as needed. Adopting platforms should treat this struct as read only and should
not modify it.

TSC lanes can architecturally be accessed in two ways. 
\li 1.If the 'prt_ad' straps of  the TSC are strapped to a specific
value, say 'lane_add' and the multiport_en strap is enabled, lanes are accessed
as lane_add, lane_add+1, lane_add+2 and lane_add+3. This  method is not
currently supported by TSCMod.
\li 2.Lanes can be accessed by using the AER offset method. This is the
preferred method and is currently supported by TSCMod.

Register access within lanes have the following pattern.

\li In independent mode tier1 procedures can be called for multiple lanes
lanes simultaneously. The lanes are chosen using #tscmod_st.lane_select
\li In dual xgxs mode two lanes form a logical port. So Tier1 procedures are
called for each dual port. Individual lanes are still accessible.
\li In combo mode, Tier1 procedures are called for a TSC. Register access
for individual lanes are still available via #tscmod_st.lane_select.

<B>Input/Output for Tier1 procedures</B><br> 

As previously mentioned the only input to all tier1 procedures is #tscmod_st.
All Tier1 procedures return a status value. Certain procedures, such as
'read_rev_id' which have to return a value modify a general purpose element
#tscmod_st.accData.  Platforms that don't use returned information can ignore
this element. Others can pick up the information from #tscmod_st::accData

<B>TSCMod Register Access Methods</B><br> 

The virtual drivers in TSCMod will eventually attach to platform specific
drivers. But prior to that, the registers, fields, and thier address (which
depends on the addressing mode and cl22/cl45 considerations) are disambiguated.
The psueodo code below illustrates this breakdown.

\code
TSC40_REG_READ(unit, pc, flags, reg_addr, val);
* unit ignored 
* pc is tscmod_st 
* flags ignored 
* reg_addr is the arch. address (i.e. needs to decoded) 
* val is a short int  pointer  to store the read value. 
  
  tscmod_reg_aer_read(unit, pc, reg_addr, val);
  * pc is the same tscmod_st 
  * reg_addr is unresolved addr 
  * val is the same short int  pointer  to store the read value. 

  * We fix address for dual XGXS mode. 
  * We fix address for dual strap offsets, multiport_addr_en port etc. 
  * i.e. reg_addr converted to phy_reg_addr 
  * call cl22 or cl45 drivers as needed. Assume we go cl22 way here. 

    tscmod_aer_cl22_read(pc, phy_reg_addr, val)
    * pc is the same tscmod_st 
    * phy_reg_addr is the resolved addr 
    * val is the same short int  pointer  to store the read value. 

    * Figure out broadcast/per lane.  
    * Adjust address again for per lane.  i.e. write lane num to bits 16,17 
    * Conduct a series of reg. accesses based on indirect (AER) reg. access 
    * For example, write the offset to reg. 0x1f etc. 

      tscmod_cl22_read(pc, phy_reg_addr, val)
      * pc is the same tscmod_st 
      * phy_reg_addr is the resolved addr 
      * val is the same short int  pointer  to store the read value. 

        * call the physical driver. (compiled for target platform)
        * In TSC verilog, we call a DPI which calls the verilog task
        * In Trident verilog, we call a DPI which directly access the Parallel

        * control bus of the MDIO ( for simulation speedup)
        * In MDK (vxworks vers.), we call our cdk_miim_read(), BRCM medium
        * independent driver
        * In SDK(all versions) we call a function ptr. that is provided by
        * the broadcom software team. 
\endcode

\note The TSCMod and documentation is work in progress. Table below 

<table cellspacing=0>

<tr><td colspan=3><B>'Current Status' bit-mappings</B></td></tr>

<tr><td><B>Tier1 </B>                  </td><td><B>Handle</B>            </td><td>PCS or PMA</td><td><B>Status</B></td></tr>
<tr><td>tscmod_aer_lane_select         </td><td> AER_LANE_SELECT         </td><td> PMA/PMD  </td><td>-      <td></tr>
<tr><td>tscmod_tx_lane_disable         </td><td> TX_LANE_DISABLE         </td><td> PMA/PMD  </td><td>-      <td></tr>
<tr><td>tscmod_autoneg_page_set        </td><td> AUTONEG_PAGE_SET        </td><td> PMA/PMD  </td><td>-      <td></tr>
<tr><td>tscmod_cjpat_crpat_control     </td><td> CJPAT_CRPAT_CONTROL     </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_lane_swap               </td><td> LANE_SWAP               </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>tscmod_pll_lock_wait           </td><td> PLL_LOCK_WAIT           </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>tscmod_wait_pmd_lock           </td><td> WAIT_PMD_LOCK           </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>tscmod_prbs_check              </td><td> PRBS_CHECK              </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>tscmod_print_tscmod_st         </td><td> PRINT_TSCMOD_ST          </td><td> NONE     </td><td>-     <td></tr>
<tr><td>tscmod_prog_data               </td><td> PROG_DATA               </td><td> PCS      </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_reg_read                </td><td> REG_READ                </td><td> PCS      </td><td>-   <td></tr>
<tr><td>tscmod_reg_write               </td><td> REG_WRITE               </td><td> PCS      </td><td>-   <td></tr>
<tr><td>tscmod_revid_read              </td><td> REVID_READ              </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_rx_loopback_control     </td><td> RX_LOOPBACK_CONTROL     </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_rx_seq_control          </td><td> RX_SEQ_CONTROL          </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>tscmod_set_port_mode           </td><td> SET_PORT_MODE           </td><td> BOTH     </td><td>DONE   <td></tr>
<tr><td>tscmod_soft_reset              </td><td> SOFT_RESET              </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_tx_bert_control         </td><td> TX_BERT_CONTROL         </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_tx_loopback_control     </td><td> TX_LOOPBACK_CONTROL     </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_tx_rx_packets_check     </td><td> TX_RX_PACKETS_CHECK     </td><td> PCS      </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_tx_rx_polarity          </td><td> TX_RX_POLARITY          </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_100fx_control           </td><td> 100FX_CONTROL           </td><td> PCS      </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_autoneg_set             </td><td> AUTONEG_SET             </td><td> PCS      </td><td>INPROG <td></tr>
<tr><td>tscmod_autoneg_get             </td><td> AUTONEG_GET             </td><td> PCS      </td><td>INPROG <td></tr>
<tr><td>tscmod_autoneg_control         </td><td> AUTONEG_CONTROL         </td><td> PCS      </td><td>INPROG <td></tr>
<tr><td>tscmod_cjpat_crpat_check       </td><td> CJPAT_CRPAT_CHECK       </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_clause72_control        </td><td> CLAUSE72_CONTROL        </td><td> PCS      </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_core_reset              </td><td> CORE_RESET              </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>tscmod_fwmode_control          </td><td> FWMODE_CONTROL          </td><td> PMD      </td><td>DONE   <td></tr>
<tr><td>tscmod_duplex_control          </td><td> DUPLEX_CONTROL          </td><td> PCS      </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_parallel_detect_control </td><td> PARALLEL_DETECT_CONTROL </td><td> PCS      </td><td>NOTDONE <td></tr>
<tr><td>tscmod_pll_sequencer_control   </td><td> PLL_SEQUENCER_CONTROL   </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>tscmod_port_stat_display       </td><td> PORT_STAT_DISPLAY       </td><td> BOTH     </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_prbs_control            </td><td> PRBS_CONTROL            </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>tscmod_set_spd_intf            </td><td> SET_SPEED_INTF          </td><td> BOTH    </td><td>INPROG <td></tr>
<tr><td> .SPEED_10M                    </td><td>  SPD_10_SGMII           </td><td>DONE <td></tr>
<tr><td> .SPEED_100M                   </td><td>  SPD_100_SGMII          </td><td>DONE <td></tr>
<tr><td> .SPEED_1000M                  </td><td>  SPD_1000_SGMII         </td><td>DONE <td></tr>
<tr><td> .SPEED_2p5G_X1                </td><td>  SPD_2500               </td><td>DONE <td></tr>
<tr><td> .SPEED_5G_X4                  </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_6G_X4                  </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_X4                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_CX4                </td><td>  SPD_10000/SPD_10000_HI </td><td>DONE<td></tr>
<tr><td> .SPEED_12G_X4                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_12p5G_X4               </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_13G_X4                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_15G_X4                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_16G_X4                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_1G_KX1                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_KX4                </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_KR1                </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_5G_X1                  </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_6p36G_X1               </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_20G_CX4                </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_21G_X4                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_25p45G_X4              </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_X2_NOSCRAMBLE      </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_CX2_NOSCRAMBLE     </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10p5G_X2               </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10p5G_CX2_NOSCRAMBLE   </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_12p7G_X2               </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_12p7G_CX2              </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_X1                 </td><td>  SPD_10000_XFI          </td><td>DONE <td></tr>
<tr><td> .SPEED_40G_X4                 </td><td>  SPD_40G_X4             </td><td>DONE <td></tr>
<tr><td> .SPEED_20G_X2                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_20G_CX2                </td><td>  SPD_20G_DXGXS          </td><td>DONE <td></tr>
<tr><td> .SPEED_10G_SFI                </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_31p5G_X4               </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_32p7G_X4               </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_20G_X4                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_X2                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_CX2                </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_12G_SCO_R2             </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_SCO_X2             </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_40G_KR4                </td><td>  SPD_40G_XLAUI          </td><td>DONE <td></tr>
<tr><td> .SPEED_40G_CR4                </td><td>  SPD_40G_XLAUI          </td><td>DONE <td></tr>
<tr><td> .SPEED_100G_CR10              </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_5G_X2                  </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_15p75G_X2              </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_2G_FC                  </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_4G_FC                  </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_8G_FC                  </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_10G_CX1                </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_1G_CX1                 </td><td>                         </td><td>     <td></tr>
<tr><td> .SPEED_20G_KR2                </td><td>  SPD_20G_MLD_DXGXS      </td><td>DONE <td></tr>
<tr><td> .SPEED_20G_CR2                </td><td>  SPD_20G_MLD_DXGXS      </td><td>DONE <td></tr>
<tr><td>tscmod_Idriver_Ipredriver_set  </td><td> IDRIVER_IPREDRIVER_SET  </td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_FEC_control             </td><td> FEC_CONTROL             </td><td> PCS      </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_firmware_set            </td><td> FIRMWARE_SET            </td><td> PCS      </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_packet_size_IPG_set     </td><td> PACKET_SIZE_IPG_SET     </td><td> PCS      </td><td>NOTDONE<td></tr>
<tr><td>tscmod_power_control           </td><td> POWER_CONTROL           </td><td> PMA/PMD  </td><td>DONE<td></tr>
<tr><td>tscmod_rx_loopback_sfpplus     </td><td> RX_LOOPBACK_SFPPLUS     </td><td> PCS      </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_scrambler_control       </td><td> SCRAMBLER_CONTROL       </td><td> PCS      </td><td>NOTDONE<td></tr>
<tr><td>tscmod_set_BRCM64B66B          </td><td> SET_BRCM64B66B          </td><td> PCS      </td><td>NOTDONE<td></tr>
<tr><td>tscmod_set_rx_DFE_tap1_override</td><td> SET_RX_DFE_TAP1_OVERRIDE</td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_set_rx_DFE_tap2_override</td><td> SET_RX_DFE_TAP2_OVERRIDE</td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_set_rx_DFE_tap3_override</td><td> SET_RX_DFE_TAP3_OVERRIDE</td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_set_rx_DFE_tap4_override</td><td> SET_RX_DFE_TAP4_OVERRIDE</td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_set_rx_DFE_tap5_override</td><td> SET_RX_DFE_TAP5_OVERRIDE</td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_set_rx_PF_override      </td><td> SET_RX_PF_OVERRIDE      </td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_set_rx_VGA_override     </td><td> SET_RX_VGA_OVERRIDE     </td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_tx_tap_control          </td><td> TX_TAP_CONTROL          </td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_rx_p1_slicer_control    </td><td> RX_P1_SLICER_CONTROL    </td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_rx_m1_slicer_control    </td><td> RX_M1_SLICER_CONTROL    </td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_rx_d_slicer_control     </td><td> RX_D_SLICER_CONTROL     </td><td> PMA/PMD  </td><td>NOTDONE   <td></tr>
<tr><td>tscmod_diag                    </td><td> TSCMOD_DIAG             </td><td> BOTH     </td><td>INPROG <td></tr>
<tr><td>tscmod_misc_control            </td><td> MISC_CONTROL            </td><td> BOTH     </td><td>INPROG <td></tr>
<tr><td>tscmod_firmware_set            </td><td> FIRMWARE_SET            </td><td> UCODE    </td><td>DONE <td></tr>
</table>

<B>Overriding native with Customer PCS Functions.</B><br> 

Customers can attach custom functions to TSCMod. Though it seems to indicate
replacement or augmentation of Tier1 functions accessing registers in the PCS
domain only (PCS Functions), any TSCMod function can be selectively replaced
with this methodology. This makes TSCMod more versatile in handling PHY modules
with different PCS blocks and also locale specific interpretation of tier1
functions. The flow chart provided below shows how overriding is possible.

TSCMod provides a structure, #str2PCSFn, to store a function pointer and
string pointers to identify it. A function Pointer Table #pcsFuncTable is
an array of these structures. The following steps populate #pcsFuncTable table.
\li 1. Initialize each element's string pointer to an identifying string
\li 2. Initialize same element's function pointer to a function
\li 3. Create one element for each overriding function 
\li 4. The last table entry must have string pointer and function pointer NULLed

<B>Function Execution.</B><br>
TSCMod user interface provides unique strings to identify all tier1 functions.
 Execution of a Tier1 function involves the following steps.

\li 1. Use input string and query Customer PCS Function Table. 
\li 2. If the string matches and the associated function pointer is NULLed, we
    assume user does not want this Tier1 function.
\li 3. If a non-NULL function pointer is found, we execute it and evaluate the return code.
\li 4. If return code is zero, we exit the loop. Any negative return value is
  treated as an error. If return code is 1 (Augmentation mode), we continue to
search for this function in local table and execute it if found.
\li 5. If function is not found we try to find it in the local function table.

<IMG SRC="../../doc/customerPCSsupport.jpg">

*/ 

#endif   /*  _TSCMOD_MAIN_H_ */
