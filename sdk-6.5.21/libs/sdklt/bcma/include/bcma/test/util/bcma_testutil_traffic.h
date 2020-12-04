/*! \file bcma_testutil_traffic.h
 *
 * Traffic test utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_TRAFFIC_H
#define BCMA_TESTUTIL_TRAFFIC_H

#include <sal/sal_types.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcma/test/util/bcma_testutil_tm.h>
#include <bcma/test/util/bcma_testutil_ctr.h>

/*! Packet Flow type */
typedef enum testutil_traffic_pkt_flow_type_e {
    FLOW_TYPE_L2UC = 0,
    FLOW_TYPE_L2MC = 1
} testutil_traffic_pkt_flow_type_t;

/*! Count mode */
typedef enum testutil_traffic_cnt_mode_e {
    CNT_MODE_AUTO = 0,
    CNT_MODE_USER = 1,
} testutil_traffic_cnt_mode_t;

/*! Length mode */
typedef enum testutil_traffic_len_mode_e {
    LEN_MODE_FIXED = 0,
    LEN_MODE_RANDOM = 1,
} testutil_traffic_len_mode_t;

/*! Test mode */
typedef enum testutil_traffic_test_mode_e {
    TEST_MODE_COMPLETE = 0,
    TEST_MODE_TX_START = 1,
    TEST_MODE_TX_STOP = 2,
} testutil_traffic_test_mode_t;

/*!
 * \brief Traffic counter checker create
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_counter_checker_create(int unit);

/*!
 * \brief Traffic counter checker add tested port
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] check_rx Need to check rx or not
 * \param [in] check_tx Need to check tx or not
 * \param [in] exp_rate Expected rate
 * \param [in] show_msg Message shown in console
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_counter_checker_add(int unit, int port,
                                          bool check_rx, bool check_tx,
                                          uint64_t exp_rate,
                                          const char *show_msg);

/*!
 * \brief Traffic counter checker start
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_counter_checker_start(int unit);

/*!
 * \brief Traffic counter checker start without clearing stats
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_counter_checker_start_no_stat_clear(int unit);
/*!
 * \brief Traffic counter checker execute
 *
 * \param [in] unit Unit number.
 * \param [in] need_check_rate Need to check rate or not
 * \param [out] ret_result_pass Returned check result
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_counter_checker_execute(int unit, bool need_check_rate,
                                              bool *ret_result_pass);

/*!
 * \brief Traffic counter checker stop
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_counter_checker_stop(int unit);


/*!
 * \brief Traffic counter checker destroy
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_counter_checker_destroy(int unit);

/*! Traffic rx data checker mode */
typedef enum traffic_rxdata_checker_mode_e {
    TRAFFIC_RXDATA_CHECKER_MODE_ANY,  /* check rx data only */
    TRAFFIC_RXDATA_CHECKER_MODE_PORT, /* check rx data and ingress port */
    TRAFFIC_RXDATA_CHECKER_MODE_COUNT
} traffic_rxdata_checker_mode_t;

/*! The maximum length of showing message */
#define TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN  (50)

/*!
 * \brief Traffic rx data checker create
 *
 * \param [in] unit Unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] pktdev_id Packet device ID.
 * \param [in] mode Rx data checker mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_rxdata_checker_create(int unit,
                                            int netif_id, int pktdev_id,
                                            traffic_rxdata_checker_mode_t mode);

/*!
 * \brief Traffic rx data checker add tested port
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] databuf Data buffer need to be checked
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_rxdata_checker_add(int unit, int port,
                                         bcmpkt_data_buf_t *databuf);

/*!
 * \brief Traffic rx data checker start
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_rxdata_checker_start(int unit);

/*!
 * \brief Traffic rx data checker execute
 *
 * \param [in] unit Unit number.
 * \param [in] wait_time Duration of check
 * \param [out] ret_result_pass Returned check result
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_rxdata_checker_execute(int unit, int wait_time,
                                             bool *ret_result_pass);

/*!
 * \brief Traffic rx data checker stop
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_rxdata_checker_stop(int unit);

/*!
 * \brief Traffic rx data checker destroy
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_rxdata_checker_destroy(int unit);

/*! The length of timestamp shimlen */
#define TESTUTIL_LATENCY_TIMESTAMP_SHIMLEN    (14) /* 10 + 4-byte crc */

/*!
 * \brief Traffic latency checker create
 *
 * \param [in] unit Unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] pktlen Packet Length.
 * \param [in] pktcnt Packet Count.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_latency_checker_create(int unit, int netif_id,
                                             uint32_t pktlen, uint32_t pktcnt);

/*!
 * \brief Traffic latency checker execute
 *
 * \param [in] unit Unit number.
 * \param [in] wait_time Duration of check
 * \param [out] ret_result_pass Returned check result
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_latency_checker_execute(int unit, int wait_time,
                                              bool *ret_result_pass);


/*!
 * \brief Traffic latency checker destroy
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_latency_checker_destroy(int unit);

/*!
 * \brief Source received vs Destination Transmitted pakcet/byte check.
 *
 * \param [in] unit Unit number.
 * \param [in] src_port Port 1 of traffic pair.
 * \param [in] dst_port Port 2 of traffic pair.
 * \param [out] test_result Returned check result
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */

extern int
bcma_testutil_traffic_src_dst_packet_count_check(int unit, int src_port, int
        dst_port, bool *test_result);

/*!
 * \brief Port error packet check.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap of tested ports.
 * \param [out] test_result Returned check result
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */

extern int
bcma_testutil_traffic_port_error_packets_check(int unit, bcmdrd_pbmp_t pbmp,
        bool *test_result);

/*!
 * \brief Traffic Queue bandwidth checker create
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_queue_bw_checker_create(int unit);

/*!
 * \brief Traffic Queue bandwidth checker add tested port
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] q_id UC/MC queue Id.
 * \param [in] q_type UC/MC queue type.
 * \param [in] exp_rate Expected rate.
 * \param [in] rate_tol_perc Expected rate error tolerance %.
 * \param [in] ipg_bytes Inter packet gap.
 * \param [in] show_msg Message shown in console.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_queue_bw_checker_add(int unit,
                                           int port,
                                           int q_id,
                                           traffic_queue_type_t q_type,
                                           uint64_t exp_rate,
                                           int rate_tol_perc,
                                           int ipg_bytes,
                                           const char *show_msg);

/*!
 * \brief Traffic queue bandwidth checker start
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_queue_bw_checker_start(int unit);

/*!
 * \brief Traffic queue bandwidth checker execute
 *
 * \param [in] unit Unit number.
 * \param [in] need_check_rate Need to check rate or not
 * \param [out] ret_result_pass Returned check result
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_queue_bw_checker_execute(int unit, bool need_check_rate,
                                               bool *ret_result_pass);

/*!
 * \brief Traffic queue bandwidth checker stop
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_queue_bw_checker_stop(int unit);


/*!
 * \brief Traffic queue bandwidth checker destroy
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_queue_bw_checker_destroy(int unit);

/*!
 * \brief Traffic rx data checker execute by port
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] wait_time Duration of check
 * \param [out] ret_result_pass Returned check result
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_rxdata_checker_execute_per_port(int unit, int port,
                                                      int wait_time,
                                                      bool *ret_result_pass);


/*!
 * \brief Add check result to traffic checker display array.
 *
 * \param [in] unit Unit number.
 * \param [in] test_result Test result.
 * \param [in] show_msg Checker name.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_check_result_disp_add(int unit, bool test_result,
                                           const char *show_msg);
/*!
 * \brief Display different checker result.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_check_result_display(int unit);

/*!
 * \brief Traffic test end checker.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Input arguments.
 * \param [in] option Input options.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_traffic_check_cb(int unit, void *bp, uint32_t option);
#endif /* BCMA_TESTUTIL_TRAFFIC_H */
