/*! \file bcma_bcmpkt_test.h
 *
 * General functions for Packet I/O test.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKT_TEST_H
#define BCMA_BCMPKT_TEST_H

#include <sal/sal_time.h>
#include <shr/shr_bitop.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/util/bcma_testutil_port.h>

/*!
 * \brief IFP info for packet I/O test.
 */
typedef struct bcma_bcmpkt_test_fp_cfg_s {
    /*! FP entry ID. */
    uint64_t entry_id;

    /*! FP group ID. */
    uint64_t grp_id;

    /*! FP rule ID. */
    uint64_t rule_id;

    /*! FP drop policy ID. */
    uint64_t drop_policy_id;

    /*! FP copy to CPU policy ID. */
    uint64_t copy_policy_id;
} bcma_bcmpkt_test_fp_cfg_t;

/*!
 * \brief Loopback mode for test.
 */
typedef enum bcma_test_lpbk_s
{
    /*! Normal mode. */
    BCMA_TEST_LPBK_NONE,

    /*! MAC loopback mode. */
    BCMA_TEST_LPBK_MAC,

    /*! PHY loopback mode. */
    BCMA_TEST_LPBK_PHY,

} bcma_test_lpbk_t;

/*!
 * \brief Parameters for packet I/O rx test.
 */
typedef struct bcma_bcmpkt_test_rx_param_s {
    /*! TRUE: initialized. */
    int init;

    /*! Enable rx packets counting. */
    uint32_t count_enable;

    /*! Counted rx packets. */
    uint64_t pkt_received;

    /*! Timestamp for the first packet. */
    sal_usecs_t time_start;

    /*! Timestamp for the last packet. */
    sal_usecs_t time_end;

    /*! Port number. */
    int port;

    /*! Packet I/O rx test duration. */
    uint32_t time;

    /*! Starting packet length. */
    uint32_t len_start;

    /*! Ending packet length. */
    uint32_t len_end;

    /*! Increasing step of packet length. */
    uint32_t len_inc;

    /*! Packet I/O packet structure. */
    bcmpkt_packet_t *packet;

    /*! FP config info. */
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg;
}bcma_bcmpkt_test_rx_param_t;

/*!
 * \brief Parameters for packet I/O tx test.
 */
typedef struct bcma_bcmpkt_test_tx_param_s {
    /*! TRUE: initialized. */
    int init;

    /*! Port number. */
    int port;

    /*! The number of packets to be sent. */
    int send_count;

    /*! Starting packet length. */
    uint32_t len_start;

    /*! Ending packet length. */
    uint32_t len_end;

    /*! Increasing step of packet length. */
    uint32_t len_inc;

    /*! Packet I/O packet structure. */
    bcmpkt_packet_t *packet;

    /*! FP config info. */
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg;
} bcma_bcmpkt_test_tx_param_t;

/*!
 * \brief Generate packet for test.
 *
 * \param [in] unit Unit number.
 * \param [in] packet The packet structure.
 * \param [in] packet_len Packet length.
 * \param [in] buf_size Buf size.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_packet_generate(int unit, bcmpkt_packet_t **packet,
                                 uint32_t packet_len, uint32_t buf_size,
                                 int port);

/*!
 * \brief Set port loopback mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] lb_mode Loopback mode.
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_port_lb_set(int unit, int port, bcma_test_lpbk_t lb_mode);

/*!
 * \brief Create traffic data path.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] redirect_port Redirect port number.
 * \param [in] ifp_cfg IFP cfg info.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_dpath_create(int unit, int port, int redirect_port,
                              bcma_bcmpkt_test_fp_cfg_t *ifp_cfg);

/*!
 * \brief Destroy traffic data path.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] ifp_cfg IFP cfg info.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_dpath_destroy(int unit, int port,
                               bcma_bcmpkt_test_fp_cfg_t *ifp_cfg);

/*!
 * \brief Parse parameter from CLI.
 *
 * \param [in] unit Unit number.
 * \param [in] cli CLI object.
 * \param [in] a Argument list.
 * \param [out] rx_param Packet I/O rx test parameter.
 * \param [in] show Print CLI parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_rx_parse_args(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                               bcma_bcmpkt_test_rx_param_t *rx_param, bool show);

/*!
 * \brief Parse parameter from CLI.
 *
 * \param [in] unit Unit number.
 * \param [in] cli CLI object.
 * \param [in] a Argument list.
 * \param [out] tx_param Packet I/O tx test parameter.
 * \param [in] show Print CLI parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_tx_parse_args(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                               bcma_bcmpkt_test_tx_param_t *tx_param, bool show);
/*!
 * \brief Packet I/O rx test procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] rx_param Packet I/O rx test parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_rx_run(int unit, bcma_bcmpkt_test_rx_param_t *rx_param);

/*!
 * \brief Packet I/O tx test procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] tx_param Packet I/O tx test parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_tx_run(int unit, bcma_bcmpkt_test_tx_param_t *tx_param);

/*!
 * \brief Packet I/O rx test cleanup procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] rx_param Packet I/O rx test parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern void
bcma_bcmpkt_test_rx_recycle(int unit, bcma_bcmpkt_test_rx_param_t *rx_param);

/*!
 * \brief Packet I/O tx test cleanup procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] tx_param Packet I/O tx test parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern void
bcma_bcmpkt_test_tx_recycle(int unit, bcma_bcmpkt_test_tx_param_t *tx_param);

/*!
 * \brief Cleanup packet I/O device.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_dev_cleanup(int unit);

/*!
 * \brief Init packet I/O device.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_dev_init(int unit);

/*!
 * \brief DMA channel mapping function.
 *
 * Mapping CPU CoS queues into receive packet DMA channel.
 *
 * \param [in] unit Switch unit number.
 * \param [in] chan_id Receive DMA channel ID.
 * \param [in] queue_bmp CPU CoS queues bitmap.
 * \param [in] num_queues Number of CPU CoS queues.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_chan_qmap_set(int unit, int chan_id, SHR_BITDCL *queue_bmp,
                               uint32_t num_queues);

#endif /* BCMA_BCMPKT_TEST_H */

