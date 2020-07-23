/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       portphy.h
 */

#ifndef _TSC_FUNCTIONS_H_
#define _TSC_FUNCTIONS_H_

#include "types.h"
#include "error.h"
#include "bcm_utils.h"
#include "bcm_pe_fields.h"
#include "bcm_pe_api_uc_vars_rdwr_defns.h"


/**
 * Error-check a function call, returning error codes returned.
 *
 * Evaluates an expression (typically function call), stores its value into
 * `*(__ERR)' and returns it from a containing function if it is unequal to
 * `SOC_E_NONE'.
 *
 * EFUN() is intended for use in functions returning error codes directly to
 * check calls to functions also returning error codes directly, e.g.:
 *
 *     int foo(...) // remaining arguments elided
 *     {
 *         // ...
 *         EFUN(wrc_core_s_rstb(0x0));
 *         // ...
 *         return SOC_E_NONE;
 *     }
 */

#define EFUN(expr)                  \
    do  {                           \
        int __err;                  \
        *(__ERR) = (expr);          \
        if (*(__ERR) != SOC_E_NONE) \
            return (*(__ERR));      \
        (void)__err;                \
    }   while (0)


/**
 * Error-check a statement, returning error codes forwarded.
 *
 * Evaluates an expression (typically unterminated statement) that may modify
 * `*(__ERR)' and returns it from a containing function if it is unequal to
 * `SOC_E_NONE'.
 *
 * ESTM() is intended for use in functions returning error codes directly to
 * check calls to functions returning error codes indirectly, e.g.:
 *
 *     err_code_t foo(...) // remaining arguments elided
 *     {
 *         uint8 rst;
 *         // ...
 *         ESTM(rst = rdc_core_s_rstb());
 *         // ...
 *         return SOC_E_NONE;
 *     }
 */

#define ESTM(expr)                  \
    do  {                           \
        int __err;                  \
        *(__ERR) = SOC_E_NONE;      \
        (expr);                     \
        if (*(__ERR) != SOC_E_NONE) \
            return *(__ERR);        \
        (void)__err;                \
    }   while (0)


/**
 * Error-check a statement, defaulting when forwarding error codes forwarded.
 *
 * In a function taking an argument `int *err_code_p' in lieu of
 * returning an error code directly, evaluates an expression (typically
 * unterminated statement), stores its value into `*(__ERR)', combines this
 * (bitwise inclusive ore) into `*(err_code_p)', and returns a default value
 * if either `*(__ERR)' or `*(err_code_p)' is not `SOC_E_NONE'.
 *
 * EPSTM2() is intended for use in functions returning error codes indirectly
 * to check calls to functions also returning error codes indirectly, e.g.:
 *
 *     uint8 foo(int *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8 result;
 *         // ...
 *         EPSTM(result = rdc_core_s_rstb(), 0x1);
 *         // ...
 *         return result;
 *     }
 */

#define EPSTM2(expr, on_err)                  \
    do  {                                     \
        int __err;                            \
        *(__ERR) = SOC_E_NONE;                \
        (expr);                               \
        *(err_code_p) |= *(__ERR);            \
        if ((*(err_code_p ) != SOC_E_NONE)    \
            || (*(__ERR)      != SOC_E_NONE)) \
            return (on_err);                  \
        (void)__err;                          \
    }   while (0)


/**
 * Error-check a statement, defaulting to zero when forwarding error codes
 * forwarded.
 *
 * Supplies a default value of zero to EPSTM2() to reduce clutter in the most
 * common case.
 *
 * EPSTM() is intended for use in functions returning error codes indirectly
 * to check calls to functions also returning error codes indirectly, e.g.:
 *
 *     uint8 foo(int *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8 result;
 *         // ...
 *         EPSTM(result = rdc_core_s_rstb());
 *         // ...
 *         return result;
 *     }
 */

#define EPSTM(expr) EPSTM2((expr), 0)


/**
 * Error-check a function call, defaulting when forwarding error codes
 * returned.
 *
 * In a function taking an argument `int *err_code_p' in lieu of
 * returning an error code directly, evaluates an expression (typically
 * function call), stores its value into `*(__ERR)', combines this (bitwise
 * inclusive ore) into `*(err_code_p)', and returns a default value if either
 * `*(__ERR)' or `*(err_code_p)' is not `SOC_E_NONE'.
 *
 * EPFUN2() is intended for use in functions returning error codes indirectly
 * to check calls to functions returning error codes directly, e.g.:
 *
 *     uint8 foo(int *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8 result = 0x0;
 *         // ...
 *         EPFUN2(wrc_core_s_rstb(0x0), 0x1);
 *         // ...
 *         return result;
 *     }
 */

#define EPFUN2(expr, on_err)                  \
    do  {                                     \
        int __err;                            \
        *(__ERR) = (expr);                    \
        *(err_code_p) |= *(__ERR);            \
        if ((*(err_code_p) != SOC_E_NONE)     \
            || (*(__ERR)      != SOC_E_NONE)) \
            return (on_err);                  \
        (void)__err;                          \
    }   while (0)


/**
 * Error-check a function call, defaulting to zero when forwarding error codes
 * returned.
 *
 * Supplies a default value of zero to EPFUN2() to reduce clutter in the most
 * common case.
 *
 * EPFUN() is intended for use in functions returning error codes indirectly
 * to check calls to functions returning error codes directly, e.g.:
 *
 *     uint8 foo(int *err_code_p, ...) // remaining arguments elided
 *     {
 *         uint8 result;  // determined below, detail elided
 *         // ...
 *         EPFUN(wrc_core_s_rstb(0x0));
 *         // ...
 *         return result;
 *     }
 */

#define EPFUN(expr) EPFUN2((expr), 0)

/**
 * Absolute value of an expression.
 *
 * @warning
 *
 *    May evaluate the given expression twice.
 */
#define _abs(a) (((a) > 0) ? (a) : (-(a)))

/** SERDES_UC_DIAG_COMMANDS */
#define CMD_UC_DIAG_DISABLE         3
#define CMD_UC_DIAG_PASSIVE         1
#define CMD_UC_DIAG_DENSITY         2
#define CMD_UC_DIAG_START_VSCAN_EYE 4
#define CMD_UC_DIAG_START_HSCAN_EYE 5
#define CMD_UC_DIAG_GET_EYE_SAMPLE  6

#define CMD_UC_DBG                  4
#define CMD_UC_DBG_DIE_TEMP         0
#define CMD_DIAG_EN                 5
#define CMD_READ_DIAG_DATA_WORD     18
#define CMD_UC_CTRL                 1
#define CMD_UC_CTRL_STOP_GRACEFULLY 0
#define CMD_UC_CTRL_STOP_IMMEDIATE  1
#define CMD_UC_CTRL_RESUME          2

#define DSC_A_DSC_UC_CTRL           0xd03d
#define DSC_E_RX_PI_CNT_BIN_D       0xd075
#define DSC_E_RX_PI_CNT_BIN_P       0xd076
#define DSC_E_RX_PI_CNT_BIN_L       0xd077
#define DSC_E_RX_PI_CNT_BIN_PD      0xd070
#define DSC_E_RX_PI_CNT_BIN_LD      0xd071
#define TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS 0xD16A
#define TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS 0xD16B

#define CMD_EVENT_LOG_READ          15
#define CMD_EVENT_LOG_READ_START    0
#define CMD_EVENT_LOG_READ_NEXT     1
#define CMD_EVENT_LOG_READ_DONE     2

/************************************************************************//**
* Direct RAM Access
* memory-mapped access to the firmware control/status RAM variables.
*//*************************************************************************/
#define CORE_VAR_RAM_BASE (0x400)
#define LANE_VAR_RAM_BASE (0x420)
#define LANE_VAR_RAM_SIZE (0x130)
#define CORE_VAR_RAM_SIZE (0x40)

/* PLL Lock and change Status Register define */
#define PLL_STATUS_ADDR 0xD148
/* PMD Lock and change Status Register define */
#define PMD_LOCK_STATUS_ADDR 0xD16C
/* Sigdet and change Status Register define */
#define SIGDET_STATUS_ADDR 0xD0E8

extern int bcm_pe_pmd_uc_cmd (phy_ctrl_t *pa, uint8 cmd, uint8 supp_info, uint32 timeout_ms);
extern int bcm_pe_pmd_uc_diag_cmd (phy_ctrl_t *pa, uint8 control, uint32 timeout_ms);
extern int bcm_pe_pmd_uc_control (phy_ctrl_t *pa, uint8 control, uint32 timeout_ms);
extern uint16 _bcm_pe_pmd_rde_field (phy_ctrl_t *pa, uint16 addr, uint8 shift_left, uint8 shift_right, int *err_code_p);
extern int16 _bcm_pe_pmd_rde_field_signed (phy_ctrl_t *pa, uint16 addr, uint8 shift_left, uint8 shift_right, int *err_code_p);
extern uint8 _bcm_pe_pmd_rde_field_byte (phy_ctrl_t *pa, uint16 addr, uint8 shift_left, uint8 shift_right, int *err_code_p);
extern int8 _bcm_pe_pmd_rde_field_signed_byte (phy_ctrl_t *pa, uint16 addr, uint8 shift_left, uint8 shift_right, int *err_code_p);
extern int _bcm_pe_pmd_mwr_reg_byte (phy_ctrl_t *pa, uint16 addr, uint16 mask, uint8 lsb, uint8 val);
extern uint16 _bcm_pe_pmd_rde_reg (phy_ctrl_t *pa, uint16 addr, int *err_code_p);
extern int bcm_pe_pmd_rdt_reg(phy_ctrl_t *pa, uint16 address, uint16 *val);
extern uint16 bcm_pe_rdwl_uc_var (phy_ctrl_t *pa, int *err_code_p, uint16 addr);
extern int16 bcm_pe_rdwls_uc_var (phy_ctrl_t *pa, int *err_code_p, uint16 addr);
extern uint8 bcm_pe_rdbl_uc_var (phy_ctrl_t *pa, int *err_code_p, uint16 addr);
extern int8 bcm_pe_rdbls_uc_var (phy_ctrl_t *pa, int *err_code_p, uint16 addr);
extern uint8 bcm_pe_rdbc_uc_var (phy_ctrl_t *pa, int *err_code_p, uint8 addr);
extern uint16 bcm_pe_rdwc_uc_var (phy_ctrl_t *pa, int *err_code_p, uint8 addr);
extern int _bcm_pe_pll_lock_status (phy_ctrl_t *pa, uint8 *pll_lock, uint8 *pll_lock_chg);
extern int _bcm_pe_pmd_lock_status (phy_ctrl_t *pa, uint8 *pmd_lock, uint8 *pmd_lock_chg);
extern int bcm_pe_prbs_chk_lock_state (phy_ctrl_t *pa, uint8 *chk_lock);
extern int bcm_pe_prbs_err_count_ll (phy_ctrl_t *pa, uint32 *prbs_err_cnt);
extern int bcm_pe_set_pll(uint8 pll_index);
extern uint8 bcm_pe_get_pll(void);
extern uint8 bcm_pe_get_core(void);
extern int bcm_pe_stop_rx_adaptation (phy_ctrl_t *pa, uint8 enable);
extern uint8 bcm_pe_tsc_get_lane(phy_ctrl_t *pa);

#endif /* _TSC_FUNCTIONS_H_ */
