/*! \file bcm56780_a0_ipep_intr.h
 *
 * IPEP interrupt sources for BCM56780_A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_IPEP_INTR_H
#define BCM56780_A0_IPEP_INTR_H

/*!
 * \name IPEP interrupt sources.
 *
 * The IPEP interrupts are organized into a single bit array, where the
 * interrupt bit order directly reflects the bit order in the hardware
 * registers. The bits are however shifted to accommodate interrupt
 * bits from three interrupts status registers in a single 32-bit
 * entity.
 *
 * The interrupt are organized follows:
 *  - Bits 0-12 are  EPIPE interrupts.
 *  - Bits 12-25 are IPIPE interrupts.
 *  - Bits 26-30 are IP_EP_COMMON interrupts.
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
 * EPIPE: EP_TO_CMIC
 */

/* EP_TO_CMIC_INTR_STATUS */
#define EP_TO_CMIC_BIT_BASE                             0
#define EP_TO_CMIC_NUM_BITS                             7
#define EP_TO_CMIC_DII                                  (EP_TO_CMIC_BIT_BASE + 0)
#define EP_TO_CMIC_DOI                                  (EP_TO_CMIC_BIT_BASE + 1)
#define EP_TO_CMIC_EP0                                  (EP_TO_CMIC_BIT_BASE + 2)
#define EP_TO_CMIC_EP1                                  (EP_TO_CMIC_BIT_BASE + 3)
#define EP_TO_CMIC_EP2                                  (EP_TO_CMIC_BIT_BASE + 4)
#define EP_TO_CMIC_EP3                                  (EP_TO_CMIC_BIT_BASE + 5)
#define EP_TO_CMIC_RDB                                  (EP_TO_CMIC_BIT_BASE + 6)

/* EPOST_EGR_INTR_STATUS */
#define EPOST_EGR_BIT_BASE                              (EP_TO_CMIC_BIT_BASE + EP_TO_CMIC_NUM_BITS)
#define EPOST_EGR_NUM_BITS                              1
#define EPOST_EGR_SER_FIFO_NON_EMPTY                    (EPOST_EGR_BIT_BASE + 0)

/* EGR_DOI_INTR_STATUS */
#define EGR_DOI_BIT_BASE                                (EPOST_EGR_BIT_BASE + EPOST_EGR_NUM_BITS)
#define EGR_DOI_NUM_BITS                                9
#define EGR_DOI_SER_FIFO_NON_EMPTY                      (EGR_DOI_BIT_BASE + 0)
#define EGR_DOI_EVENT_FIFO_0_READ_FAIL                  (EGR_DOI_BIT_BASE + 1)
#define EGR_DOI_EVENT_FIFO_0_WRITE_FAIL                 (EGR_DOI_BIT_BASE + 2)
#define EGR_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR        (EGR_DOI_BIT_BASE + 3)
#define EGR_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR     (EGR_DOI_BIT_BASE + 4)
#define EGR_DOI_EVENT_FIFO_1_READ_FAIL                  (EGR_DOI_BIT_BASE + 5)
#define EGR_DOI_EVENT_FIFO_1_WRITE_FAIL                 (EGR_DOI_BIT_BASE + 6)
#define EGR_DOI_CELL_QUEUE_1_UNCORRECTED_ECC_ERR        (EGR_DOI_BIT_BASE + 7)
#define EGR_DOI_SLOT_PIPELINE_1_UNCORRECTED_ECC_ERR     (EGR_DOI_BIT_BASE + 8)

/* EGR_DII_INTR_STATUS */
#define EGR_DII_BIT_BASE                                (EGR_DOI_BIT_BASE + EGR_DOI_NUM_BITS)
#define EGR_DII_NUM_BITS                                3
#define EGR_DII_MEM_RESET_COMPLETE                      (EGR_DII_BIT_BASE + 0)
#define EGR_DII_EVENT_FIFO_ERR                          (EGR_DII_BIT_BASE + 1)
#define EGR_DII_REFRESH_ERR                             (EGR_DII_BIT_BASE + 2)

/*
 * IPIPE: IP_TO_CMIC
 */
/* IPOST_SER_STATUS_BLK_ING_INTR_STATUS */
#define IPOST_SER_STATUS_BLK_ING_BIT_BASE               (EGR_DII_BIT_BASE + EGR_DII_NUM_BITS)
#define IPOST_SER_STATUS_BLK_ING_NUM_BITS               1
#define IPOST_SER_STATUS_BLK_ING_SER_FIFO_NON_EMPTY     (IPOST_SER_STATUS_BLK_ING_BIT_BASE + 0)

/* LEARN_CACHE_CTRL */
#define LEARN_CACHE_CTRL_BIT_BASE                       (IPOST_SER_STATUS_BLK_ING_BIT_BASE + IPOST_SER_STATUS_BLK_ING_NUM_BITS)
#define LEARN_CACHE_CTRL_NUM_BITS                       1
#define LEARN_CACHE_INTR                                (LEARN_CACHE_CTRL_BIT_BASE + 0)

/* ING_DOI_INTR_STATUS */
#define ING_DOI_BIT_BASE                                (LEARN_CACHE_CTRL_BIT_BASE + LEARN_CACHE_CTRL_NUM_BITS)
#define ING_DOI_NUM_BITS                                9
#define ING_DOI_SER_FIFO_NON_EMPTY                      (ING_DOI_BIT_BASE + 0)
#define ING_DOI_EVENT_FIFO_0_READ_FAIL                  (ING_DOI_BIT_BASE + 1)
#define ING_DOI_EVENT_FIFO_0_WRITE_FAIL                 (ING_DOI_BIT_BASE + 2)
#define ING_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR        (ING_DOI_BIT_BASE + 3)
#define ING_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR     (ING_DOI_BIT_BASE + 4)
#define ING_DOI_EVENT_FIFO_1_READ_FAIL                  (ING_DOI_BIT_BASE + 5)
#define ING_DOI_EVENT_FIFO_1_WRITE_FAIL                 (ING_DOI_BIT_BASE + 6)
#define ING_DOI_CELL_QUEUE_1_UNCORRECTED_ECC_ERR        (ING_DOI_BIT_BASE + 7)
#define ING_DOI_SLOT_PIPELINE_1_UNCORRECTED_ECC_ERR     (ING_DOI_BIT_BASE + 8)

/* ING_DII_INTR_STATUS */
#define ING_DII_BIT_BASE                                (ING_DOI_BIT_BASE + ING_DOI_NUM_BITS)
#define ING_DII_NUM_BITS                                3
#define ING_DII_MEM_RESET_COMPLETE                      (ING_DII_BIT_BASE + 0)
#define ING_DII_EVENT_FIFO_ERR                          (ING_DII_BIT_BASE + 1)
#define ING_DII_REFRESH_ERR                             (ING_DII_BIT_BASE + 2)
/*
 * IP_EP_COMMON
 */
/* DLB_ECMP_INTR_STATUS */
#define DLB_ECMP_BIT_BASE                               (ING_DII_BIT_BASE + ING_DII_NUM_BITS)
#define DLB_ECMP_NUM_BITS                               1
#define DLB_ECMP_INTR                                   (DLB_ECMP_BIT_BASE + 0)

/* CENTRAL_CTR_EVICTION_INTR_STATUS */
#define CENTRAL_CTR_EVICTION_BIT_BASE                   (DLB_ECMP_BIT_BASE + DLB_ECMP_NUM_BITS)
#define CENTRAL_CTR_EVICTION_NUM_BITS                   3
#define CENTRAL_CTR_EVICTION_FIFO_NOT_EMPTY             (CENTRAL_CTR_EVICTION_BIT_BASE + 0)
#define CENTRAL_CTR_EVICTION_FIFO_OVERFLOW              (CENTRAL_CTR_EVICTION_BIT_BASE + 1)
#define CENTRAL_CTR_EVICTION_FIFO_PARITY_ERROR          (CENTRAL_CTR_EVICTION_BIT_BASE + 2)

/* FT_LEARN_INTR_STATUS */
#define FT_LEARN_BIT_BASE                               (CENTRAL_CTR_EVICTION_BIT_BASE + CENTRAL_CTR_EVICTION_NUM_BITS)
#define FT_LEARN_NUM_BITS                               1
#define FT_LEARN_INTR                                   (FT_LEARN_BIT_BASE + 0)

#define MAX_IPEP_INTR                                   (FT_LEARN_BIT_BASE + FT_LEARN_NUM_BITS)

/*! \} */

#endif /* BCM56780_A0_IPEP_INTR_H */
