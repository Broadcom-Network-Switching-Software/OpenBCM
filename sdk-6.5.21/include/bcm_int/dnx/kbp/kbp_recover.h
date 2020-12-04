/** \file kbp_recover.h
 *
 * Functions and defines for handling jericho2 kbp recover sequence.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_KBP)

#ifndef _KBP_RECOVER_INCLUDED__
/*
 * {
 */

#define _KBP_RECOVER_INCLUDED__

/*************
 * INCLUDES  *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * DEFINES   *
 */
/*
 * {
 */

#define DNX_KBP_CLAUSE45_ADDR(devad, regad)     \
            ((((devad) & 0x3F) << 16) |          \
             ((regad) & 0xFFFF))

#define DNX_KBP_TX_PCS_EN_REG    (0xF00F)

#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD0_REG    (0xF160)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD1_REG    (0xF161)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD2_REG    (0xF162)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD3_REG    (0xF163)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD4_REG    (0xF164)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD5_REG    (0xF165)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD6_REG    (0xF166)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD7_REG    (0xF167)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD8_REG    (0xF168)

#define DNX_KBP_TX_PHASE_MATCHING_FIFO_IN_RESET_VAL    (0x3037)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_OUT_OF_RESET_VAL    (0x3035)

#define DNX_KBP_RECOVER_LINK_UP_LOOP_TIMES    (0x64)
#define DNX_KBP_RECORD_REPLY_VALID_CHECK_TIMES    (5)

/*OP2 register*/
#define DNX_KBP_RX_LINK_CTRL_REG    (0x3002)
#define DNX_KBP_TX_LINK_CTRL_REG    (0x7002)

#define DNX_KBP_FEC_CTRL_REG        (0xF807)
#define DNX_KBP_RX_TX_PCS_CTRL_REG  (0xF801)
#define DNX_KBP_CDR_STATUS_REG      (0xF822)
#define DNX_KBP_LINKUP_STATUS_REG   (0x2829)

#define DNX_KBP_PHMF_FIFO_CTRL_REG  (0xF852)

#define DNX_KBP_CLEAR_ERR_0_CTRL_REG  (0x2850)
#define DNX_KBP_CLEAR_ERR_0_1_CTRL_REG  (0x2851)
#define DNX_KBP_CLEAR_ERR_1_CTRL_REG  (0x2003)
#define DNX_KBP_CLEAR_ERR_2_CTRL_REG  (0xF806)
#define DNX_KBP_CLEAR_ERR_3_CTRL_REG  (0x10)

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * ENUMS     *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * GLOBALS   *
 */
/*
 * {
 */
/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */
shr_error_e dnx_kbp_recover_run_recovery_sequence(
    int unit,
    uint32 core,
    uint32 mdio_id);

shr_error_e dnx_kbp_phase_matching_fifo_reset(
    int unit,
    uint32 mdio_id,
    int kbp_port_id,
    int reset);

shr_error_e dnx_kbp_op_link_stability_check(
    int unit,
    uint32 core,
    uint32 kbp_mdio_id,
    uint32 retries);

shr_error_e dnx_kbp_device_interface_enable_link(
    int unit,
    uint32 enable);

/*
 * }
 */
#endif /* __KBP_RECOVER_INCLUDED__ */
#endif /* defined(INCLUDE_KBP) */
