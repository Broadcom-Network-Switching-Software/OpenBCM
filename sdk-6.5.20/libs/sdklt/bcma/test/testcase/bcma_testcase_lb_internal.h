/*! \file bcma_testcase_lb_internal.h
 *
 * Loopback test framework functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_LB_INTERNAL_H
#define BCMA_TESTCASE_LB_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_time.h>
#include <sal/sal_sem.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_traffic.h>

/*! Number of chan for one CMC */
#define LB_CMC_CHAN_NUM 4

#define LB_PKT_MAX_FRAME_SIZE (1536)

#define LB_DEFAULT_TX_CHAN (0)
#define LB_DEFAULT_RX_CHAN (1)

/*!
 * \brief Parameters for loopback test.
 */
typedef struct lb_param_s {
    /*! Port bitmap. */
    bcmdrd_pbmp_t pbmp;

    /*! 2nd Port bitmap. Only for external loopback testing */
    bcmdrd_pbmp_t pbmp2;

    /*! packet device type */
    bcmdrd_dev_type_t pkt_dev_type;

    /*! Test iteration. */
    int iteration;

    /*! Source mac address. */
    shr_mac_t src_mac;

    /*! Destination mac address. */
    shr_mac_t dst_mac;

    /*! VLAN id. */
    int vid;

    /*! Packet pattern. */
    uint32_t pkt_pattern;

    /*! Packet starting length. */
    int pkt_len_start;

    /*! Packet ending length. */
    int pkt_len_end;

    /*! Packet increment length. */
    int pkt_len_inc;

    /*! Starting cosq. */
    int cos_start;

    /*! Ending cosq. */
    int cos_end;

    /*! Check link status after testing */
    int check_post_link;

    /*! TX/RX State. */
    int tx_rx_state;

    /*! TX time. */
    sal_time_t tx_time;

    /*! RX time. */
    sal_time_t rx_time;

    /*! Time for report progress. */
    sal_time_t report_time;

    /*! Total TX packets. */
    int tx_total;

    /*! TX packets. */
    int tx_pkts;

    /*! TX bytes. */
    int tx_bytes;

    /*! RX packets. */
    int rx_pkts;

    /*! RX bytes. */
    int rx_bytes;

    /*! PacketIO packet. */
    bcmpkt_packet_t *packet;

    /*! Expected incoming port */
    int port_rx_expect;

    /*! Semaphore for waiting RX callback done. */
    sal_sem_t sema;

    /*! Time for waiting RX packets. */
    int timeout;

    /*! Net device id. */
    int netif_id;

    /*! Loopback type. */
    testutil_port_lb_type_t lb_type;
} lb_param_t;

/*!
 * \brief Initialize TX/RX.
 *
 * \param [in] unit Unit number.
 * \param [in] lb_param Loopback parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_lb_txrx_init(int unit, lb_param_t *lb_param);

/*!
 * \brief Send and receive traffic.
 *
 * \param [in] unit Unit number.
 * \param [in] lb_param Loopback parameter.
 * \param [in] tx_chan TX chan number.
 * \param [in] rx_chan RX chan number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_lb_txrx(int unit, lb_param_t *lb_param, int tx_chan, int rx_chan);

/*!
 * \brief Cleanup TX/RX.
 *
 * \param [in] unit Unit number.
 * \param [in] lb_param Loopback parameter.
 */
extern void
bcma_testcase_lb_txrx_cleanup(int unit, lb_param_t *lb_param);

/*!
 * \brief Loopback test select function.
 *
 * \param [in] unit Unit number.
 *
 * \return true if test case is selected on this device, otherwise false.
 */
extern int
bcma_testcase_lb_select(int unit);

/*!
 * \brief Loopback test parser function.
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
bcma_testcase_lb_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                        uint32_t flags, void **bp, testutil_port_lb_type_t type);

/*!
 * \brief Loopback test help function.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 */
extern void
bcma_testcase_lb_help(int unit, void *bp);

/*!
 * \brief Loopback test recycle function.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 */
extern void
bcma_testcase_lb_recycle(int unit, void *bp);

/*!
 * \brief Loopback test vlan configuration procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_lb_vlan_set_cb(int unit, void *bp, uint32_t option);

/*!
 * \brief Loopback test port configuration procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_lb_port_set_cb(int unit, void *bp, uint32_t option);

/*!
 * \brief Loopback test L2 entry configuration procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_lb_l2_forward_add_cb(int unit, void *bp, uint32_t option);

/*!
 * \brief Loopback test TX/RX procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_lb_txrx_cb(int unit, void *bp, uint32_t option);

/*!
 * \brief Loopback test cleanup procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Buffer pointer for case-specific parameters.
 * \param [in] option Test database option.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testcase_lb_cleanup_cb(int unit, void *bp, uint32_t option);

#endif /* BCMA_TESTCASE_LB_INTERNAL_H */
