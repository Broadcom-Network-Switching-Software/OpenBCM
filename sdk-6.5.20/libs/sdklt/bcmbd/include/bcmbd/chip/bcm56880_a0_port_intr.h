/*! \file bcm56880_a0_port_intr.h
 *
 * Port interrupt sources for BCM56880_A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_PORT_INTR_H
#define BCM56880_A0_PORT_INTR_H

/*!
 * \name Port interrupt sources.
 *
 * The port interrupts are organized into a single bit array, and for
 * CDMAC/XLMAC interrupts, the bit order reflects the bit order in the
 * hardware registers. All CDMAC/XLMAC interrupts are instantiated per
 * MAC.
 *
 * For the CDPORT/XLPORT interrupts, some interrupt bits are
 * instantiated per MAC while others (TSC/PMD) are instantiated per
 * port macro (PM).
 *
 * On top of this, all port macros are connected to separate
 * low-priority (LP) interrupt lines, so with 32 CDPORTs each with 8
 * MACs, per-PM interrupts have 32 instances, and per-MAC interrupts
 * have 32 * 8 instances.
 *
 * The interrupt instance will be passed as a parameter to the
 * interrupt handler, and it will be provided as a physical port
 * number for convenience, irrespective of whether the interrupt is
 * per MAC or per PM.
 *
 * The PMD/TSC interrupts require further decoding via separate
 * interrupt registers, however this is assumed to be handled by the
 * SerDes driver/firmware.
 *
 * Please refer to the hardware documentation for a description of the
 * individual interrupt sources.
 *
 * \{
 */

#define CDPORT_INTR_BASE        0
/* These interrupts are instantiated per MAC unless otherwise noted */
#define CDPORT_INTR_LINK_DOWN                   (CDPORT_INTR_BASE + 0)
#define CDPORT_INTR_TSC_CORE0_ERR               (CDPORT_INTR_BASE + 1) /* PM */
#define CDPORT_INTR_TSC_CORE1_ERR               (CDPORT_INTR_BASE + 2) /* PM */
#define CDPORT_INTR_PMD_ERR                     (CDPORT_INTR_BASE + 3) /* PM */
#define CDPORT_INTR_FLOWCONTROL_REQ_FULL        (CDPORT_INTR_BASE + 4)
#define NUM_CDPORT_INTR                         5

#define CDMAC_INTR_BASE         (CDPORT_INTR_BASE + NUM_CDPORT_INTR)
/* These interrupts are all instantiated per MAC */
#define CDMAC_INTR_TX_PKT_UNDERFLOW             (CDMAC_INTR_BASE + 0)
#define CDMAC_INTR_TX_PKT_OVERFLOW              (CDMAC_INTR_BASE + 1)
#define CDMAC_INTR_LOCAL_FAULT_STATUS           (CDMAC_INTR_BASE + 2)
#define CDMAC_INTR_REMOTE_FAULT_STATUS          (CDMAC_INTR_BASE + 3)
#define CDMAC_INTR_LINK_INTERRUPTION_STATUS     (CDMAC_INTR_BASE + 4)
#define CDMAC_INTR_TX_CDC_SINGLE_BIT_ERR        (CDMAC_INTR_BASE + 5)
#define CDMAC_INTR_TX_CDC_DOUBLE_BIT_ERR        (CDMAC_INTR_BASE + 6)
#define CDMAC_INTR_MIB_COUNTER_SINGLE_BIT_ERR   (CDMAC_INTR_BASE + 7)
#define CDMAC_INTR_MIB_COUNTER_DOUBLE_BIT_ERR   (CDMAC_INTR_BASE + 8)
#define CDMAC_INTR_MIB_COUNTER_MULTIPLE_ERR     (CDMAC_INTR_BASE + 9)
#define NUM_CDMAC_INTR                          10

#define XLPORT_INTR_BASE        (CDMAC_INTR_BASE + NUM_CDMAC_INTR)
/* These interrupts are instantiated per MAC unless otherwise noted */
#define XLPORT_INTR_MIB_TX_MEM_ERR              (XLPORT_INTR_BASE + 0)
#define XLPORT_INTR_MIB_RX_MEM_ERR              (XLPORT_INTR_BASE + 1)
#define XLPORT_INTR_TSC_ERR                     (XLPORT_INTR_BASE + 2) /* PM */
#define XLPORT_INTR_RX_FLOWCONTROL_REQ_FULL     (XLPORT_INTR_BASE + 3)
#define XLPORT_INTR_LINK_DOWN                   (XLPORT_INTR_BASE + 4)
#define NUM_XLPORT_INTR                         5

#define XLMAC_INTR_BASE         (XLPORT_INTR_BASE + NUM_XLPORT_INTR)
/* These interrupts are all instantiated per MAC */
#define XLMAC_INTR_SUM_TX_PKT_UNDERFLOW         (XLMAC_INTR_BASE + 0)
#define XLMAC_INTR_SUM_TX_PKT_OVERFLOW          (XLMAC_INTR_BASE + 1)
#define XLMAC_INTR_SUM_TX_LLFC_MSG_OVERFLOW     (XLMAC_INTR_BASE + 2)
#define XLMAC_INTR_SUM_TX_TS_FIFO_OVERFLOW      (XLMAC_INTR_BASE + 3)
#define XLMAC_INTR_SUM_RX_PKT_OVERFLOW          (XLMAC_INTR_BASE + 4)
#define XLMAC_INTR_SUM_RX_MSG_OVERFLOW          (XLMAC_INTR_BASE + 5)
#define XLMAC_INTR_SUM_TX_CDC_SINGLE_BIT_ERR    (XLMAC_INTR_BASE + 6)
#define XLMAC_INTR_SUM_TX_CDC_DOUBLE_BIT_ERR    (XLMAC_INTR_BASE + 7)
#define XLMAC_INTR_SUM_RX_CDC_SINGLE_BIT_ERR    (XLMAC_INTR_BASE + 8)
#define XLMAC_INTR_SUM_RX_CDC_DOUBLE_BIT_ERR    (XLMAC_INTR_BASE + 9)
#define XLMAC_INTR_SUM_LOCAL_FAULT_STATUS       (XLMAC_INTR_BASE + 10)
#define XLMAC_INTR_SUM_REMOTE_FAULT_STATUS      (XLMAC_INTR_BASE + 11)
#define XLMAC_INTR_SUM_LINK_INTERRUPTION_STATUS (XLMAC_INTR_BASE + 12)
#define XLMAC_INTR_SUM_TS_ENTRY_VALID           (XLMAC_INTR_BASE + 13)
#define NUM_XLMAC_INTR                          14

#define MAX_PORT_INTR           (XLMAC_INTR_BASE + NUM_XLMAC_INTR)

/*! \} */

#endif /* BCM56880_A0_PORT_INTR_H */
