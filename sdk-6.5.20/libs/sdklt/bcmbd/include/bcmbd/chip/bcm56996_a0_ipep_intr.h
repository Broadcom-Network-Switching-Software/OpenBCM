/*! \file bcm56996_a0_ipep_intr.h
 *
 * IPEP interrupt sources for BCM56996_A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56996_A0_IPEP_INTR_H
#define BCM56996_A0_IPEP_INTR_H

/*!
 * \name IPEP interrupt sources.
 *
 * The IPEP interrupts are organized into a single bit array, where the
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
 * EPIPE: EP_TO_CMIC_INTR
 */
/*
 * EP_TO_CMIC_INTR_STATUS.EARB
 * EGR_DII_INTR_STATUS
 */
#define EGR_DII_BIT_BASE                              0
#define EGR_DII_MEM_RESET_COMPLETE                    (EGR_DII_BIT_BASE + 0)
#define EGR_DII_EVENT_FIFO_0_ERR                      (EGR_DII_BIT_BASE + 1)
#define EGR_DII_EVENT_FIFO_1_ERR                      (EGR_DII_BIT_BASE + 2)
#define EGR_DII_EVENT_FIFO_2_ERR                      (EGR_DII_BIT_BASE + 3)
#define EGR_DII_EVENT_FIFO_3_ERR                      (EGR_DII_BIT_BASE + 4)
#define EGR_DII_REFRESH_ERR                           (EGR_DII_BIT_BASE + 5)
/* EVENT_FIFO_SER_ERR[3:0], each bit for a fifo */
#define EGR_DII_EVENT_FIFO0_SER_ERR                   (EGR_DII_BIT_BASE + 6)
#define EGR_DII_EVENT_FIFO1_SER_ERR                   (EGR_DII_BIT_BASE + 7)
#define EGR_DII_EVENT_FIFO2_SER_ERR                   (EGR_DII_BIT_BASE + 8)
#define EGR_DII_EVENT_FIFO3_SER_ERR                   (EGR_DII_BIT_BASE + 9)
#define EGR_DII_NUM_BITS                              10

/*
 * EP_TO_CMIC_INTR_STATUS.FLEX_COUNTER
 * FLEX_CTR_EGR_INTR_STATUS
 */
#define FLEX_CTR_EGR_BIT_BASE                         (EGR_DII_BIT_BASE + EGR_DII_NUM_BITS)
#define FLEX_CTR_EGR_SER_FIFO_NON_EMPTY               (FLEX_CTR_EGR_BIT_BASE + 0)
#define FLEX_CTR_EGR_NUM_BITS                         1

/*
 * EP_TO_CMIC_INTR_STATUS.EPOST
 * EGR_DOI_INTR_STATUS
 */
#define EGR_DOI_BIT_BASE                              (FLEX_CTR_EGR_BIT_BASE + FLEX_CTR_EGR_NUM_BITS)
#define EGR_DOI_SER_FIFO_NON_EMPTY                    (EGR_DOI_BIT_BASE + 0)
#define EGR_DOI_EVENT_FIFO_0_READ_FAIL                (EGR_DOI_BIT_BASE + 1)
#define EGR_DOI_EVENT_FIFO_0_WRITE_FAIL               (EGR_DOI_BIT_BASE + 2)
#define EGR_DOI_EVENT_FIFO_1_READ_FAIL                (EGR_DOI_BIT_BASE + 3)
#define EGR_DOI_EVENT_FIFO_1_WRITE_FAIL               (EGR_DOI_BIT_BASE + 4)
#define EGR_DOI_EVENT_FIFO_2_READ_FAIL                (EGR_DOI_BIT_BASE + 5)
#define EGR_DOI_EVENT_FIFO_2_WRITE_FAIL               (EGR_DOI_BIT_BASE + 6)
#define EGR_DOI_EVENT_FIFO_3_READ_FAIL                (EGR_DOI_BIT_BASE + 7)
#define EGR_DOI_EVENT_FIFO_3_WRITE_FAIL               (EGR_DOI_BIT_BASE + 8)
#define EGR_DOI_EVENT_FIFO_CTRL_0_UNCORRECTED_ECC_ERR (EGR_DOI_BIT_BASE + 9)
#define EGR_DOI_EVENT_FIFO_CTRL_1_UNCORRECTED_ECC_ERR (EGR_DOI_BIT_BASE + 10)
#define EGR_DOI_EVENT_FIFO_CTRL_2_UNCORRECTED_ECC_ERR (EGR_DOI_BIT_BASE + 11)
#define EGR_DOI_EVENT_FIFO_CTRL_3_UNCORRECTED_ECC_ERR (EGR_DOI_BIT_BASE + 12)
#define EGR_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR      (EGR_DOI_BIT_BASE + 13)
#define EGR_DOI_CELL_QUEUE_1_UNCORRECTED_ECC_ERR      (EGR_DOI_BIT_BASE + 14)
#define EGR_DOI_CELL_QUEUE_2_UNCORRECTED_ECC_ERR      (EGR_DOI_BIT_BASE + 15)
#define EGR_DOI_CELL_QUEUE_3_UNCORRECTED_ECC_ERR      (EGR_DOI_BIT_BASE + 16)
#define EGR_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR   (EGR_DOI_BIT_BASE + 17)
#define EGR_DOI_SLOT_PIPELINE_1_UNCORRECTED_ECC_ERR   (EGR_DOI_BIT_BASE + 18)
#define EGR_DOI_SLOT_PIPELINE_2_UNCORRECTED_ECC_ERR   (EGR_DOI_BIT_BASE + 19)
#define EGR_DOI_SLOT_PIPELINE_3_UNCORRECTED_ECC_ERR   (EGR_DOI_BIT_BASE + 20)
#define EGR_DOI_PKT_BUFFER_0_UNCORRECTED_ECC_ERR      (EGR_DOI_BIT_BASE + 21)
#define EGR_DOI_PKT_BUFFER_1_UNCORRECTED_ECC_ERR      (EGR_DOI_BIT_BASE + 22)
#define EGR_DOI_PKT_BUFFER_2_UNCORRECTED_ECC_ERR      (EGR_DOI_BIT_BASE + 23)
#define EGR_DOI_PKT_BUFFER_3_UNCORRECTED_ECC_ERR      (EGR_DOI_BIT_BASE + 24)
#define EGR_DOI_EVENT_FIFO_DATA_0_UNCORRECTED_ECC_ERR (EGR_DOI_BIT_BASE + 25)
#define EGR_DOI_EVENT_FIFO_DATA_1_UNCORRECTED_ECC_ERR (EGR_DOI_BIT_BASE + 26)
#define EGR_DOI_EVENT_FIFO_DATA_2_UNCORRECTED_ECC_ERR (EGR_DOI_BIT_BASE + 27)
#define EGR_DOI_EVENT_FIFO_DATA_3_UNCORRECTED_ECC_ERR (EGR_DOI_BIT_BASE + 28)
#define EGR_DOI_NUM_BITS                              29

/* EP_TO_CMIC_INTR_STATUS.EFPMOD for EGR_SER_FIFO_2m not empty only */
#define EP2C_EFPMOD_BIT_BASE                          (EGR_DOI_BIT_BASE + EGR_DOI_NUM_BITS)
#define EP2C_EFPMOD                                   (EP2C_EFPMOD_BIT_BASE + 0)
#define EP2C_EFPMOD_NUM_BITS                          1

/*
 * IPIPE: IP_TO_CMIC_PIPE_INTR
 */
/*
 * IP_TO_CMIC_INTR_STATUS.IP0
 * ING_DII_INTR_STATUS
 */
#define ING_DII_BIT_BASE                              (EP2C_EFPMOD_BIT_BASE + EP2C_EFPMOD_NUM_BITS)
#define ING_DII_MEM_RESET_COMPLETE                    (ING_DII_BIT_BASE + 0)
#define ING_DII_EVENT_FIFO_0_ERR                      (ING_DII_BIT_BASE + 1)
#define ING_DII_EVENT_FIFO_1_ERR                      (ING_DII_BIT_BASE + 2)
#define ING_DII_EVENT_FIFO_2_ERR                      (ING_DII_BIT_BASE + 3)
#define ING_DII_EVENT_FIFO_3_ERR                      (ING_DII_BIT_BASE + 4)
#define ING_DII_REFRESH_ERR                           (ING_DII_BIT_BASE + 5)
/* EVENT_FIFO_SER_ERR[3:0], each bit for a fifo */
#define ING_DII_EVENT_FIFO0_SER_ERR                   (ING_DII_BIT_BASE + 6)
#define ING_DII_EVENT_FIFO1_SER_ERR                   (ING_DII_BIT_BASE + 7)
#define ING_DII_EVENT_FIFO2_SER_ERR                   (ING_DII_BIT_BASE + 8)
#define ING_DII_EVENT_FIFO3_SER_ERR                   (ING_DII_BIT_BASE + 9)
#define ING_DII_NUM_BITS                              10

/*
 * IP_TO_CMIC_INTR_STATUS.IP4
 * IP4_INTR_STATUS.FLEX_CTR
 * FLEX_CTR_ING_INTR_STATUS
 */
#define FLEX_CTR_ING_BIT_BASE                         (ING_DII_BIT_BASE + ING_DII_NUM_BITS)
#define FLEX_CTR_ING_SER_FIFO_NON_EMPTY               (FLEX_CTR_ING_BIT_BASE + 0)
#define FLEX_CTR_ING_NUM_BITS                         1

/*
 * IP_TO_CMIC_INTR_STATUS.IP4
 * IP4_INTR_STATUS.IPOST
 * ING_DOI_INTR_STATUS
 */
#define ING_DOI_BIT_BASE                              (FLEX_CTR_ING_BIT_BASE + FLEX_CTR_ING_NUM_BITS)
#define ING_DOI_SER_FIFO_NON_EMPTY                    (ING_DOI_BIT_BASE + 0)
#define ING_DOI_EVENT_FIFO_0_READ_FAIL                (ING_DOI_BIT_BASE + 1)
#define ING_DOI_EVENT_FIFO_0_WRITE_FAIL               (ING_DOI_BIT_BASE + 2)
#define ING_DOI_EVENT_FIFO_1_READ_FAIL                (ING_DOI_BIT_BASE + 3)
#define ING_DOI_EVENT_FIFO_1_WRITE_FAIL               (ING_DOI_BIT_BASE + 4)
#define ING_DOI_EVENT_FIFO_2_READ_FAIL                (ING_DOI_BIT_BASE + 5)
#define ING_DOI_EVENT_FIFO_2_WRITE_FAIL               (ING_DOI_BIT_BASE + 6)
#define ING_DOI_EVENT_FIFO_3_READ_FAIL                (ING_DOI_BIT_BASE + 7)
#define ING_DOI_EVENT_FIFO_3_WRITE_FAIL               (ING_DOI_BIT_BASE + 8)
#define ING_DOI_EVENT_FIFO_0_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 9)
#define ING_DOI_EVENT_FIFO_1_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 10)
#define ING_DOI_EVENT_FIFO_2_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 11)
#define ING_DOI_EVENT_FIFO_3_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 12)
#define ING_DOI_CELL_QUEUE_0_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 13)
#define ING_DOI_CELL_QUEUE_1_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 14)
#define ING_DOI_CELL_QUEUE_2_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 15)
#define ING_DOI_CELL_QUEUE_3_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 16)
#define ING_DOI_SLOT_PIPELINE_0_UNCORRECTED_ECC_ERR   (ING_DOI_BIT_BASE + 17)
#define ING_DOI_SLOT_PIPELINE_1_UNCORRECTED_ECC_ERR   (ING_DOI_BIT_BASE + 18)
#define ING_DOI_SLOT_PIPELINE_2_UNCORRECTED_ECC_ERR   (ING_DOI_BIT_BASE + 19)
#define ING_DOI_SLOT_PIPELINE_3_UNCORRECTED_ECC_ERR   (ING_DOI_BIT_BASE + 20)
#define ING_DOI_PKT_BUFFER_0_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 21)
#define ING_DOI_PKT_BUFFER_1_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 22)
#define ING_DOI_PKT_BUFFER_2_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 23)
#define ING_DOI_PKT_BUFFER_3_UNCORRECTED_ECC_ERR      (ING_DOI_BIT_BASE + 24)
#define ING_DOI_NUM_BITS                              25

/*
 * IP_TO_CMIC_INTR_STATUS.IP4
 * IP4_INTR_STATUS.ISW is for ING_SER_FIFOm not empty only
 */
#define IP4_ISW_BIT_BASE                              (ING_DOI_BIT_BASE + ING_DOI_NUM_BITS)
#define IP4_ISW_NUM_BITS                              1
#define IP4_ISW                                       (IP4_ISW_BIT_BASE + 0)

/* IP_TO_CMIC_INTR_STATUS */
#define IP_TO_CMIC_BIT_BASE                           (IP4_ISW_BIT_BASE + IP4_ISW_NUM_BITS)
#define IP_TO_CMIC_NUM_BITS                           3
/* Not full mask, should do field base read, change and write */
#define IP_TO_CMIC_IP1                                (IP_TO_CMIC_BIT_BASE + 0)
#define IP_TO_CMIC_IP2                                (IP_TO_CMIC_BIT_BASE + 1)
#define IP_TO_CMIC_IP3                                (IP_TO_CMIC_BIT_BASE + 2)

/*
 * IPEP_COMMON: DLB_CREDIT_TO_CMIC_INTR
 */
/* DLB_ECMP_INTR_STATUS */
#define DLB_ECMP_BIT_BASE                             (IP_TO_CMIC_BIT_BASE + IP_TO_CMIC_NUM_BITS)
#define DLB_ECMP_NUM_BITS                             1
#define DLB_ECMP_SER_FIFO_NON_EMPTY                   (DLB_ECMP_BIT_BASE + 0)

/*
 * IPEP_COMMON: CE_TO_CMIC_INTR
 */
/* CENTRAL_CTR_EVICTION_INTR_STATUS */
#define CENTRAL_CTR_EVICTION_BIT_BASE                 (DLB_ECMP_BIT_BASE + DLB_ECMP_NUM_BITS)
#define CENTRAL_CTR_EVICTION_NUM_BITS                 3
#define CENTRAL_CTR_EVICTION_FIFO_NOT_EMPTY           (CENTRAL_CTR_EVICTION_BIT_BASE + 0)
#define CENTRAL_CTR_EVICTION_FIFO_OVERFLOW            (CENTRAL_CTR_EVICTION_BIT_BASE + 1)
#define CENTRAL_CTR_EVICTION_FIFO_PARITY_ERROR        (CENTRAL_CTR_EVICTION_BIT_BASE + 2)

#define MAX_IPEP_INTR                                 (CENTRAL_CTR_EVICTION_BIT_BASE + CENTRAL_CTR_EVICTION_NUM_BITS)

/*! \} */

#endif /* BCM56996_A0_IPEP_INTR_H */
