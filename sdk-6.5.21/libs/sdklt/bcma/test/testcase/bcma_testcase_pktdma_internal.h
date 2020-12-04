/*! \file bcma_testcase_pktdma_internal.h
 *
 * Packet DMA test common functions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_PKTDMA_INTERNAL_H
#define BCMA_TESTCASE_PKTDMA_INTERNAL_H

#include <sal/sal_types.h>

/*
 * DCB factor for different mode
 * Should not change
 */
#define PKTDMA_DCB_FACTOR_SG    2
#define PKTDMA_DCB_FACTOR_RL    2

#define PKTDMA_MAX_PKT_NUM      16
#define PKTDMA_MIN_PKT_SIZE     64
#define PKTDMA_MAX_PKT_SIZE     1518
#define PKTDMA_SG_MIN_DCB_SIZE  32 /*
                                    * should be less than PKTDMA_MIN_PKT_SIZE
                                    * in SG testing
                                    */
typedef enum {
    PKTDMA_TEST_TYPE_SIMPLE,
    PKTDMA_TEST_TYPE_SG,
    PKTDMA_TEST_TYPE_CHAIN,
    PKTDMA_TEST_TYPE_SG_CHAIN,
    PKTDMA_TEST_TYPE_RELOAD,
    PKTDMA_TEST_TYPE_NUM
} pktdma_test_type_t;

typedef struct pktdma_test_param_s {
    uint32_t tx_ch_bmp;     /* 16-bit bitmap of Tx channels */
    uint32_t rx_ch_bmp;     /* 16-bit bitmap of Rx channels */
    uint32_t pkt_size;      /* Packet size in bytes. 0 for random packet sizes */
    uint32_t num_pkts;      /* Number of packets for which descriptors are set up at one time */
    pktdma_test_type_t test_type;   /* 0: Simple Packet DMA
                                     * 1: Scatter/Gather without packet chaining
                                     * 2: Chained DMA
                                     * 3: Scatter/Gather with packet chaining
                                     * 4: Reload descriptor is inserted in the middle of chain
                                     */
    uint32_t check_pkt;     /* Enable packet data checks */
    uint32_t verbose;       /* Print descriptors and packet pointers */
    uint32_t intr_mode;     /* 0:Polling, 1:Intr, Default=0 */
    uint32_t continuous_mode; /* 0:non-continuous mode, 1:continuous mode, Default=0 */
    uint32_t cmic_lb;       /* CMIC loopback mode */

#define PKTDMA_F_SG     0x1
#define PKTDMA_F_CHAIN  0x2
#define PKTDMA_F_RELOAD 0x4
    uint32_t flags;             /* PKTDMA_F_* macros */
    uint32_t num_pairs;         /* Number of Tx/Rx channel pair */

    uint8_t  *txpkt[PKTDMA_MAX_PKT_NUM];            /* Pointer of each Tx packet data */
    uint64_t txpkt_dma_addr[PKTDMA_MAX_PKT_NUM];    /* DMA addr of each Tx packet data */
    uint8_t  *rxpkt[PKTDMA_MAX_PKT_NUM];            /* Pointer of each Rx packet data */
    uint64_t rxpkt_dma_addr[PKTDMA_MAX_PKT_NUM];    /* DMA addr of each Rx packet data */

    uint32_t num_dcbs;          /* Number of DCB needed for the test */
    void     *txdcb;            /* Pointor of Tx DCB */
    uint64_t txdcb_dma_addr;    /* DMA addr of Tx DCB */
    void     *rxdcb;            /* Pointor of Rx DCB */
    uint64_t rxdcb_dma_addr;    /* DMA addr of Rx DCB */

} pktdma_test_param_t;

/*!
 * \brief Packet DMA test packet header filling procedure.
 *
 * \param [in] packet Packet pointer.
 * \param [in] pkt_len Packet length.
 * \param [in] inc Increment value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_pktdma_packet_fill(uint8_t *packet, uint32_t pkt_len, int inc);

/*!
 * \brief Packet DMA test parser function.
 *
 * \param [in] unit Unit number.
 * \param [in] cli CLI object.
 * \param [in] a CLI arguments.
 * \param [in] flags Test engine flags.
 * \param [out] bp Buffer pointer for case-specific parameters.
 * \param [in] type Loopback type.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_pktdma_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                            uint32_t flags, void **bp);

/*!
 * \brief Packet DMA test help function.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 */
extern void
bcma_testcase_pktdma_help(int unit, void *bp);

/*!
 * \brief Packet DMA test packet buffer allocate function.
 *
 * \param [in] unit Unit number.
 * \param [in] p Pointer for parameters.
 * \param [in] bufsize Buffer size in bytes.
 */
extern int
bcma_testcase_pktdma_buf_alloc(int unit, pktdma_test_param_t *p, int bufsize);

/*!
 * \brief Packet DMA test packet buffer free function.
 *
 * \param [in] unit Unit number.
 * \param [in] p Pointer for parameters.
 * \param [in] bufsize Buffer size in bytes.
 */
extern void
bcma_testcase_pktdma_buf_free(int unit, pktdma_test_param_t *p, int bufsize);

/*!
 * \brief Packet DMA test recycle function.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 */
extern void
bcma_testcase_pktdma_recycle(int unit, void *bp);

/*!
 * \brief Packet DMA test vlan configuration procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_pktdma_vlan_set_cb(int unit, void *bp, uint32_t option);

/*!
 * \brief Packet DMA test L2 entry configuration procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_pktdma_l2_forward_add_cb(int unit, void *bp, uint32_t option);

/*!
 * \brief Packet DMA test cleanup procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_pktdma_cleanup_cb(int unit, void *bp, uint32_t option);
#endif /* BCMA_TESTCASE_PKTDMA_INTERNAL_H */
