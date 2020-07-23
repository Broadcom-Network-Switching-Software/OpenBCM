/*! \file bcm56990_b0_db_intr.h
 *
 * IDB-EDB interrupt sources for BCM56990_B0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_B0_DB_INTR_H
#define BCM56990_B0_DB_INTR_H

/*!
 * \name DB interrupt sources.
 *
 * The DB interrupts are organized into a single bit array, where the
 * interrupt bit order directly reflects the bit order in the hardware
 * registers.
 *
 * Please refer to the hardware documentation for a description of the
 * individual interrupt sources.
 *
 * Note that some interrupt sources have multiple instances because
 * the underlying interrupt status register has multiple
 * instances. For these interrupts, the instance will be passed as a
 * parameter to the interrupt handler.
 *
 * \{
 */

/*
 * IDB_EDB_TO_CMIC_PERR_INTR
 */
/* IDB_INTR_STATUS */
#define IDB_BIT_BASE                  0
#define IDB_NUM_BITS                  26
#define IDB_OBM0_ECC_ERR              (IDB_BIT_BASE + 0)
#define IDB_CA0_FIFO_ECC_ERR          (IDB_BIT_BASE + 1)
#define IDB_CA0_FIFO_ERR              (IDB_BIT_BASE + 2)
#define IDB_OBM1_ECC_ERR              (IDB_BIT_BASE + 3)
#define IDB_CA1_FIFO_ECC_ERR          (IDB_BIT_BASE + 4)
#define IDB_CA1_FIFO_ERR              (IDB_BIT_BASE + 5)
#define IDB_OBM2_ECC_ERR              (IDB_BIT_BASE + 6)
#define IDB_CA2_FIFO_ECC_ERR          (IDB_BIT_BASE + 7)
#define IDB_CA2_FIFO_ERR              (IDB_BIT_BASE + 8)
#define IDB_OBM3_ECC_ERR              (IDB_BIT_BASE + 9)
#define IDB_CA3_FIFO_ECC_ERR          (IDB_BIT_BASE + 10)
#define IDB_CA3_FIFO_ERR              (IDB_BIT_BASE + 11)
#define IDB_AUX_FIFO_ECC_ERR          (IDB_BIT_BASE + 12)
#define IDB_AUX_FIFO_ERR              (IDB_BIT_BASE + 13)
#define IDB_CA0_SOP_EOP_ERR           (IDB_BIT_BASE + 14)
#define IDB_CA0_CELL_LEN_ERR          (IDB_BIT_BASE + 15)
#define IDB_CA1_SOP_EOP_ERR           (IDB_BIT_BASE + 16)
#define IDB_CA1_CELL_LEN_ERR          (IDB_BIT_BASE + 17)
#define IDB_CA2_SOP_EOP_ERR           (IDB_BIT_BASE + 18)
#define IDB_CA2_CELL_LEN_ERR          (IDB_BIT_BASE + 19)
#define IDB_CA3_SOP_EOP_ERR           (IDB_BIT_BASE + 20)
#define IDB_CA3_CELL_LEN_ERR          (IDB_BIT_BASE + 21)
#define IDB_AUX_SOP_EOP_ERR           (IDB_BIT_BASE + 22)
#define IDB_AUX_CELL_LEN_ERR          (IDB_BIT_BASE + 23)
#define IDB_OBM_MONITOR_CONFIG_ERROR  (IDB_BIT_BASE + 24)
#define IDB_PFC_MON_ECC_ERR           (IDB_BIT_BASE + 25)

/* IDB_INTR_STATUS */
#define EDB_BIT_BASE                  (IDB_BIT_BASE + IDB_NUM_BITS)
#define EDB_NUM_BITS                  1
#define EDB_SER_FIFO_NON_EMPTY        (EDB_BIT_BASE + 0)

#define MAX_DB_INTR                   (EDB_BIT_BASE + EDB_NUM_BITS)

/*! \} */

#endif /* BCM56990_B0_DB_INTR_H */
