/*! \file bcm56880_a0_mmu_intr.h
 *
 * MMU interrupt sources for BCM56880_A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_MMU_INTR_H
#define BCM56880_A0_MMU_INTR_H

/*!
 * \name MMU interrupt sources.
 *
 * The MMU interrupts are organized into a single bit array, where the
 * interrupt bit order directly reflects the bit order in the hardware
 * registers. The bits are however shifted to accommodate interrupt
 * bits from three interrupts status registers in a single 32-bit
 * entity.
 *
 * The interrupt are organized follows:
 *  - Bits 0-7 are GLB interrupts.
 *  - Bits 8-15 are XPE interrupts.
 *  - Bits 16-31 are SC interrupts.
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

/* MMU_GLBCFG_CPU_INT */
#define GLB_BIT_BASE            0
#define GLB_NUM_BITS            5
#define MMU_INTR_GLB_MEM_PAR_ERR                (GLB_BIT_BASE + 0)
#define MMU_INTR_GLB_PPSCH                      (GLB_BIT_BASE + 1)
#define MMU_INTR_GLB_INTFO                      (GLB_BIT_BASE + 2)
#define MMU_INTR_GLB_TS_OVERFLOW                (GLB_BIT_BASE + 3)
#define MMU_INTR_GLB_TS_UNDERFLOW               (GLB_BIT_BASE + 4)

/* MMU_EBCFG_CPU_INT */
#define EB_BIT_BASE             (GLB_BIT_BASE + GLB_NUM_BITS)
#define EB_NUM_BITS             1
#define MMU_INTR_EB_MEM_PAR_ERR                 (EB_BIT_BASE + 0)

/* MMU_EBTOQ_CPU_INT */
#define EBTOQ_BIT_BASE          (EB_BIT_BASE + EB_NUM_BITS)
#define EBTOQ_NUM_BITS          4
#define MMU_INTR_EBTOQ_QUEUE_ENTRIES_FULL       (EBTOQ_BIT_BASE + 0)
#define MMU_INTR_EBTOQ_CBFP_FIFO_OVERFLOW       (EBTOQ_BIT_BASE + 1)
#define MMU_INTR_EBTOQ_CBFP_MEM_OVERFLOW        (EBTOQ_BIT_BASE + 2)
#define MMU_INTR_EBTOQ_ENQ_DEQ_EOP_PREF_COLL    (EBTOQ_BIT_BASE + 3)

/* MMU_ITMCFG_CPU_INT */
#define ITM_BIT_BASE            (EBTOQ_BIT_BASE + EBTOQ_NUM_BITS)
#define ITM_NUM_BITS            1
#define MMU_INTR_ITM_MEM_PAR_ERR                (ITM_BIT_BASE + 0)

/* MMU_THDR_QE_CPU_INT */
#define THDR_BIT_BASE           (ITM_BIT_BASE + ITM_NUM_BITS)
#define THDR_NUM_BITS           3
#define MMU_INTR_THDR_BST                       (THDR_BIT_BASE + 0)
#define MMU_INTR_THDR_CNT_OVERFLOW              (THDR_BIT_BASE + 1)
#define MMU_INTR_THDR_CNT_UNDERFLOW             (THDR_BIT_BASE + 2)

/* MMU_CFAP_INT */
#define CFAP_BIT_BASE           (THDR_BIT_BASE + THDR_NUM_BITS)
#define CFAP_NUM_BITS           1
#define MMU_INTR_CFAP_BST                       (CFAP_BIT_BASE + 0)

/* MMU_THDO_BST_CPU_INT */
#define THDO_BST_BIT_BASE       (CFAP_BIT_BASE + CFAP_NUM_BITS)
#define THDO_BST_NUM_BITS       3
#define MMU_INTR_THDO_BST                       (THDO_BST_BIT_BASE + 0)
#define MMU_INTR_THDO_EBST                      (THDO_BST_BIT_BASE + 1)
#define MMU_INTR_THDO_EBST_FIFO_FULL            (THDO_BST_BIT_BASE + 2)

/* MMU_THDO_CPU_INT */
#define THDO_BIT_BASE           (THDO_BST_BIT_BASE + THDO_BST_NUM_BITS)
#define THDO_NUM_BITS           3
#define MMU_INTR_THDO_SPID_MISMATCH             (THDO_BIT_BASE + 0)
#define MMU_INTR_THDO_CNT_OVERFLOW              (THDO_BIT_BASE + 1)
#define MMU_INTR_THDO_CNT_UNDERFLOW             (THDO_BIT_BASE + 2)

/* MMU_THDI_CPU_INT */
#define THDI_BIT_BASE           (THDO_BIT_BASE + THDO_NUM_BITS)
#define THDI_NUM_BITS           1
#define MMU_INTR_THDI_BST                       (THDI_BIT_BASE + 0)

#define MAX_MMU_INTR    (THDI_BIT_BASE + THDI_NUM_BITS)

/*! \} */

#endif /* BCM56880_A0_MMU_INTR_H */
