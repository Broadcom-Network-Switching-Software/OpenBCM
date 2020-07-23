/*! \file bcma_testcase_traffic_max_power.h
 *
 * Test case for max power traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_TRAFFIC_MAX_POWER_H
#define BCMA_TESTCASE_TRAFFIC_MAX_POWER_H

#include <bcma/test/bcma_testcase.h>
#include <bcma/test/util/bcma_testutil_port.h>

/*! Brief description for max power traffic test case. */
#define BCMA_TESTCASE_TRAFFIC_MAX_POWER_DESC \
    "Intent to create maximum power usage for power measurement.\n"\
    "tr 517 is used for characterizing maximum power, it has to \n"\
    "run at appropriate voltage per chip lead's request. Moreover, \n"\
    "the test has to run with thermal control system to avoid \n"\
    "burning the device and damaging the board.\n"

/*! Flags for max power traffic test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_TRAFFIC_MAX_POWER_FLAGS (0)

/*! Max packet count for max power traffic test case.*/
#define TRAFFIC_MAX_POWER_PKT_CNT_MAX                (500)
/*! Max packet size for max power traffic test case.*/
#define TRAFFIC_MAX_POWER_PKT_SIZE_MAX               (9300)
/*! Pktdev transmit channel for max power traffic test case.*/
#define TRAFFIC_MAX_POWER_PKTDEV_CHAN_TX             (0)
/*! Pktdev receive channel for max power traffic test case.*/
#define TRAFFIC_MAX_POWER_PKTDEV_CHAN_RX             (1)
/*! Receive wait time for max power traffic test case.*/
#define TRAFFIC_MAX_POWER_DEFAULT_RX_CHECK_WAIT      (50*1000000) /* 50 sec */
/*! Speed check margin for max power traffic test case.*/
#define TRAFFIC_MAX_POWER_DEFAULT_CHECK_SPEED_MARGIN (5) /* pass tolerance is 5% */
/*! Netif ID for max power traffic test case.*/
#define TRAFFIC_MAX_POWER_DEFAULT_NETIF_ID           (1)

/*!
 * \brief Get max_power traffic test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_traffic_max_power_op_get(void);

/*! Packet length mode for power test*/
typedef enum traffic_power_pktlen_mode_e {
    POWER_PKTLEN_WORSTCASE = 0,
    POWER_PKTLEN_RANDOM,
    POWER_PKTLEN_USERDEFINED,
    POWER_PKTLEN_PATTERN
} traffic_power_pktlen_mode_t;


/*! Parameter struct for max power test*/
typedef struct traffic_max_power_param_s {
    /*! All testing ports */
    bcmdrd_pbmp_t pbmp_all;

    /*! Mirror ports */
    bcmdrd_pbmp_t mirror_pbmp;

    /*! Oversubscribed ports */
    bcmdrd_pbmp_t pbmp_oversub;

    /*! Packet length mode */
    traffic_power_pktlen_mode_t pkt_len_mode;

    /*! Packet length array for each testing port */
    uint32_t pkt_len[BCMDRD_CONFIG_MAX_PORTS][TRAFFIC_MAX_POWER_PKT_CNT_MAX];

    /*! Packet count array for each testing port  */
    uint32_t pkt_cnt[BCMDRD_CONFIG_MAX_PORTS];

    /*! Upper limit of cell numbers per packet */
    uint32_t pkt_cell_limit;

    /*! Running time (in sec) */
    uint32_t runtime;

    /*! Polling interval (in sec) */
    uint32_t interval;

    /*! Enabling l3 defip hit */
    int enable_defip;

    /*! Enabling rdb hit */
    int enable_rdb;

    /*! Enabling alpm hit */
    int enable_alpm;

    /*! Enabling dlb */
    int enable_dlb;

   /*! Enabling mirror */
    int enable_mirror;

    /*! Enabling ifp hit */
    int enable_ifp;

    /*! Enabling efp hit */
    int enable_efp;

    /*! Enabling random header */
    int enable_rnd_hdr;

    /*! Enabling flex ctr */
    int enable_flex_ctr;

    /*! Various enable features bmp
     * Every chip can have different meaning
     */
    uint32_t enable_bmp;

    /*! Various enable features bmp
     * Every chip can have different meaning
     */
    int lpm_level;

    /*! Various enable features bmp
     * Every chip can have different meaning
     */
    int topology;

    /*! Run until end even if error happen */
    int run_2_end;

    /*! Whether to check packet back to CPU after unlock loop */
    int need_check_rx;

    /*! Whether to check speed meet line rate */
    int need_check_speed;

    /*! config and send traffic */
    bool config_n_send;

    /*! check and stop traffic */
    bool check_n_stop;

    /*! Loopback Mode */
    testutil_port_lb_type_t loopback_mode;

    /*! pkt flow */
    int pkt_flow;


} traffic_max_power_param_t;

#endif /* BCMA_TESTCASE_TRAFFIC_MAX_POWER_H */
