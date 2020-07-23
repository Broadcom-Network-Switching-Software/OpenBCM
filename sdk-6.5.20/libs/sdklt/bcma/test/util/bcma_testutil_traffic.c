/*
 *
 *  Traffic test common functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_time.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmlt/bcmlt.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_qos.h>
#include <bcma/test/util/bcma_testutil_traffic.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST
#define INTER_PKT_GAP 20

/* Structure of traffic counter check */
typedef struct traffic_counter_checker_entry_s {
    /* port needs to be checked */
    int port;

    /* whether to check rx/tx or not */
    bool check_rx, check_tx;

    /* expected rate */
    uint64_t exp_rate;

    /* cache for last counter value */
    uint64_t rbyt_pre, rpkt_pre, tbyt_pre, tpkt_pre;

    /* show message */
    char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];

    /* timer */
    sal_usecs_t time_pre;

    /* next entry */
    struct traffic_counter_checker_entry_s *next;
} traffic_counter_checker_entry_t;

static bool traffic_counter_checker_init[BCMDRD_CONFIG_MAX_UNITS] = {false};
static bool traffic_counter_checker_start[BCMDRD_CONFIG_MAX_UNITS] = {false};
static traffic_counter_checker_entry_t
            *traffic_counter_checker[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

#define COUNTER_CHECK_UNIT(_u_)                              \
    do {                                                     \
        if ((_u_) < 0 || (_u_) >= BCMDRD_CONFIG_MAX_UNITS) { \
            SHR_ERR_EXIT(SHR_E_UNIT);                 \
        }                                                    \
    } while (0)

#define COUNTER_CHECK_STATUS(_expect_init_, _expect_start_)            \
    do {                                                               \
        if (traffic_counter_checker_init[unit] != (_expect_init_)) {   \
            if ((_expect_init_) == true) {                             \
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);                  \
            } else {                                                   \
                SHR_ERR_EXIT(SHR_E_EXISTS);                     \
            }                                                          \
        }                                                              \
        if (traffic_counter_checker_start[unit] != (_expect_start_)) { \
            if ((_expect_start_) == true) {                            \
                SHR_ERR_EXIT(SHR_E_CONFIG);                     \
            } else {                                                   \
                SHR_ERR_EXIT(SHR_E_BUSY);                       \
            }                                                          \
        }                                                              \
    } while (0)

int
bcma_testutil_traffic_counter_checker_create(int unit)
{
    SHR_FUNC_ENTER(unit);
    COUNTER_CHECK_UNIT(unit);
    COUNTER_CHECK_STATUS(false, false);

    traffic_counter_checker_init[unit] = true;

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_counter_checker_add(int unit, int port,
                                          bool check_rx, bool check_tx,
                                          uint64_t exp_rate,
                                          const char *show_msg)
{
    traffic_counter_checker_entry_t *entry_new = NULL;
    size_t show_msg_len;

    SHR_FUNC_ENTER(unit);
    COUNTER_CHECK_UNIT(unit);
    COUNTER_CHECK_STATUS(true, false);

    /* allocate new entry */
    SHR_ALLOC(entry_new, sizeof(traffic_counter_checker_entry_t),
              "bcmaTestCaseTrafficCounterCheck");
    SHR_NULL_CHECK(entry_new, SHR_E_MEMORY);
    sal_memset(entry_new, 0,
               sizeof(traffic_counter_checker_entry_t));

    entry_new->port = port;
    entry_new->check_rx = check_rx;
    entry_new->check_tx = check_tx;
    entry_new->exp_rate = exp_rate;
    show_msg_len = sal_strlen(show_msg);
    if (show_msg_len >= TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sal_memcpy(entry_new->show_msg, show_msg, show_msg_len + 1);

    /* insert into linked list */
    if (traffic_counter_checker[unit] == NULL) {
        traffic_counter_checker[unit] = entry_new;
    } else {
        traffic_counter_checker_entry_t *entry_ptr = NULL;

        entry_ptr = traffic_counter_checker[unit];
        while (entry_ptr->next != NULL) {
            entry_ptr = entry_ptr->next;
        }
        entry_ptr->next = entry_new;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(entry_new);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_counter_checker_start(int unit)
{
    traffic_counter_checker_entry_t *entry_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    COUNTER_CHECK_UNIT(unit);
    COUNTER_CHECK_STATUS(true, false);

    /* clear stat for each port */
    entry_ptr = traffic_counter_checker[unit];
    while (entry_ptr != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_stat_clear(unit, entry_ptr->port));
        entry_ptr->rbyt_pre = 0;
        entry_ptr->tbyt_pre = 0;
        entry_ptr->rpkt_pre = 0;
        entry_ptr->tpkt_pre = 0;
        entry_ptr->time_pre = sal_time_usecs();
        entry_ptr = entry_ptr->next;
    }

    /* start process okay */
    traffic_counter_checker_start[unit] = true;

 exit:
    SHR_FUNC_EXIT();
}

/* Only */
int
bcma_testutil_traffic_counter_checker_start_no_stat_clear(int unit)
{
    traffic_counter_checker_entry_t *entry_ptr = NULL;
    bcma_testutil_stat_t *stat_array = NULL;
    int stat_size = 0;

    SHR_FUNC_ENTER(unit);
    COUNTER_CHECK_UNIT(unit);
    COUNTER_CHECK_STATUS(true, false);

    /* start process okay */
    traffic_counter_checker_start[unit] = true;
    entry_ptr = traffic_counter_checker[unit];
    while (entry_ptr != NULL) {
        uint32_t port = entry_ptr->port;

        /* counter polling */
        SHR_IF_ERR_EXIT
            (bcma_testutil_stat_mac_get(unit, port, &stat_array, &stat_size));
        entry_ptr->rbyt_pre = stat_array[RX_BYTES].value;
        entry_ptr->rpkt_pre = stat_array[RX_PKT].value;
        entry_ptr->tbyt_pre = stat_array[TX_BYTES].value;
        entry_ptr->tpkt_pre = stat_array[TX_PKT].value;

        entry_ptr->time_pre = sal_time_usecs();
        entry_ptr = entry_ptr->next;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_counter_checker_execute(int unit, bool need_check_rate,
                                              bool *ret_result_pass)
{
    traffic_counter_checker_entry_t *entry_ptr = NULL;
    bcma_testutil_stat_t *stat_array = NULL;
    int stat_size = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_result_pass, SHR_E_PARAM);
    COUNTER_CHECK_UNIT(unit);
    COUNTER_CHECK_STATUS(true, true);

    *ret_result_pass = true;

    /* iterate each port to poll counter */
    entry_ptr = traffic_counter_checker[unit];
    while (entry_ptr != NULL) {

        uint64_t rbyt, rpkt, tbyt, tpkt;
        uint64_t rbyt_diff, rpkt_diff, tbyt_diff, tpkt_diff;
        uint64_t r_rate, t_rate;
        uint32_t port = entry_ptr->port;
        sal_usecs_t time = sal_time_usecs();
        int time_diff;
        size_t i;

        /* counter polling */
        SHR_IF_ERR_EXIT
            (bcma_testutil_stat_mac_get(unit, port, &stat_array, &stat_size));
        rbyt = stat_array[RX_BYTES].value;
        rpkt = stat_array[RX_PKT].value;
        tbyt = stat_array[TX_BYTES].value;
        tpkt = stat_array[TX_PKT].value;

        rbyt_diff = rbyt - entry_ptr->rbyt_pre;
        rpkt_diff = rpkt - entry_ptr->rpkt_pre;
        tbyt_diff = tbyt - entry_ptr->tbyt_pre;
        tpkt_diff = tpkt - entry_ptr->tpkt_pre;

        entry_ptr->rbyt_pre = rbyt;
        entry_ptr->rpkt_pre = rpkt;
        entry_ptr->tbyt_pre = tbyt;
        entry_ptr->tpkt_pre = tpkt;

        time_diff = (int)(time - entry_ptr->time_pre);
        entry_ptr->time_pre = time;

        r_rate = 8 * (rbyt_diff + (rpkt_diff * INTER_PKT_GAP)) * SECOND_USEC / time_diff;
        t_rate = 8 * (tbyt_diff + (tpkt_diff * INTER_PKT_GAP)) * SECOND_USEC / time_diff;

        /* output TX status */
        cli_out("%s", entry_ptr->show_msg);
        cli_out("TX Packets +");
        bcma_testutil_stat_show(tpkt_diff, 1, true);
        cli_out(", Bytes +");
        bcma_testutil_stat_show(tbyt_diff, 1, true);
        cli_out(" [");
        bcma_testutil_stat_show(t_rate, 1, true);
        cli_out("b/s]\n");

        /* print spaces to align the first line */
        for (i = 0; i < sal_strlen(entry_ptr->show_msg); i++) {
            cli_out(" ");
        }

        /* output RX status */
        cli_out("RX Packets +");
        bcma_testutil_stat_show(rpkt_diff, 1, true);
        cli_out(", Bytes +");
        bcma_testutil_stat_show(rbyt_diff, 1, true);
        cli_out(" [");
        bcma_testutil_stat_show(r_rate, 1, true);
        cli_out("b/s]\n");

        /* check counter if need */
        if ((entry_ptr->check_rx == true
                && (rbyt_diff == 0 || rpkt_diff == 0))||
            (entry_ptr->check_tx == true
                && (tbyt_diff == 0 || tpkt_diff == 0))) {

            *ret_result_pass = false;
            cli_out("Error: Port %3"PRId32" traffic stop.\n", port);
        }

        /* check rate if need */
        if (need_check_rate == TRUE) {

            if ((entry_ptr->check_rx == true && r_rate < entry_ptr->exp_rate)||
                (entry_ptr->check_tx == true && t_rate < entry_ptr->exp_rate)) {

                *ret_result_pass = false;
                cli_out("Error: Port %3"PRId32" traffic too slow.\n", port);
                cli_out("  ==> expected rate is : ");
                bcma_testutil_stat_show(entry_ptr->exp_rate, 1, true);
                cli_out("b/s\n");
            }
        }

        entry_ptr = entry_ptr->next;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_counter_checker_stop(int unit)
{
    SHR_FUNC_ENTER(unit);
    COUNTER_CHECK_UNIT(unit);
    COUNTER_CHECK_STATUS(true, true);

    /* stop process okay */
    traffic_counter_checker_start[unit] = false;

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_counter_checker_destroy(int unit)
{
    traffic_counter_checker_entry_t *entry_ptr = NULL;
    traffic_counter_checker_entry_t *entry_next = NULL;

    SHR_FUNC_ENTER(unit);
    COUNTER_CHECK_UNIT(unit);
    COUNTER_CHECK_STATUS(true, false);

    entry_ptr = traffic_counter_checker[unit];
    while (entry_ptr != NULL) {
        entry_next = entry_ptr->next;
        sal_free(entry_ptr);
        entry_ptr = entry_next;
    }
    traffic_counter_checker[unit] = NULL;
    traffic_counter_checker_init[unit] = false;

 exit:
    SHR_FUNC_EXIT();
}

/* Structure of traffic rx data checker */
typedef struct traffic_rxdata_databuf_s {
    /* data buffer */
    uint8_t *data;

    /* data size */
    uint32_t data_len;

    /* next */
    struct traffic_rxdata_databuf_s *next;
} traffic_rxdata_databuf_t;

typedef struct traffic_rxdata_checker_entry_s {
    /* databuf array (to be checked) for each port */
    traffic_rxdata_databuf_t *databuf_list[BCMDRD_CONFIG_MAX_PORTS];

    /* databuf array (to be freed) for each port */
    traffic_rxdata_databuf_t *databuf_list_freed[BCMDRD_CONFIG_MAX_PORTS];

    /* the number of matched packet */
    uint32_t match_num[BCMDRD_CONFIG_MAX_PORTS];

    /* the number of received packet */
    uint32_t rx_num[BCMDRD_CONFIG_MAX_PORTS];

    /* mode */
    traffic_rxdata_checker_mode_t mode;

    /* packet device id */
    int pktdev_id;

    /* packet device type */
    bcmdrd_dev_type_t pktdev_type;

    /* netif id */
    int netif_id;

    /* rx cb found something wrong */
    bool rx_something_wrong;

    /* semaphore */
    sal_sem_t sema;
} traffic_rxdata_checker_entry_t;

static traffic_rxdata_checker_entry_t
            *traffic_rxdata_checker[BCMDRD_CONFIG_MAX_UNITS] = {NULL};
static bool traffic_rxdata_checker_start[BCMDRD_CONFIG_MAX_UNITS] = {false};

#define RXDATA_CHECK_UNIT(_u_)                               \
    do {                                                     \
        if ((_u_) < 0 && (_u_) >= BCMDRD_CONFIG_MAX_UNITS) { \
            SHR_ERR_EXIT(SHR_E_UNIT);                 \
        }                                                    \
    } while (0)

#define RXDATA_CHECK_STATUS(_expect_init_, _expect_start_)                     \
    do {                                                                       \
        if (traffic_rxdata_checker[unit] == NULL && (_expect_init_) == true) { \
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);                          \
        }                                                                      \
        if (traffic_rxdata_checker[unit] != NULL && (_expect_init_) == false) {\
                SHR_ERR_EXIT(SHR_E_EXISTS);                             \
        }                                                                      \
        if (traffic_rxdata_checker_start[unit] != (_expect_start_)) {          \
            if ((_expect_start_) == true) {                                    \
                SHR_ERR_EXIT(SHR_E_CONFIG);                             \
            } else {                                                           \
                SHR_ERR_EXIT(SHR_E_BUSY);                               \
            }                                                                  \
        }                                                                      \
    } while (0)

int
bcma_testutil_traffic_rxdata_checker_create(int unit,
                                            int netif_id, int pktdev_id,
                                            traffic_rxdata_checker_mode_t mode)
{
    traffic_rxdata_checker_entry_t *entry_new = NULL;

    SHR_FUNC_ENTER(unit);
    RXDATA_CHECK_UNIT(unit);
    RXDATA_CHECK_STATUS(false, false);

    SHR_ALLOC(entry_new, sizeof(traffic_rxdata_checker_entry_t),
              "bcmaTestCaseTrafficRxDataCheck");
    SHR_NULL_CHECK(entry_new, SHR_E_MEMORY);
    sal_memset(entry_new, 0, sizeof(traffic_rxdata_checker_entry_t));

    entry_new->netif_id = netif_id;
    entry_new->pktdev_id = pktdev_id;
    entry_new->rx_something_wrong = false;
    entry_new->mode = mode;
    entry_new->sema = sal_sem_create("rxcheck_sema", SAL_SEM_BINARY, 0);
    if (entry_new->sema == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &entry_new->pktdev_type));

    /* create process success */
    traffic_rxdata_checker[unit] = entry_new;

 exit:
    if (SHR_FUNC_ERR()) {
        if (entry_new != NULL) {
            if (entry_new->sema != NULL) {
                sal_sem_destroy(entry_new->sema);
                entry_new->sema = NULL;
            }
            sal_free(entry_new);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_rxdata_checker_add(int unit, int port,
                                         bcmpkt_data_buf_t *databuf)
{
    traffic_rxdata_checker_entry_t *entry = NULL;
    traffic_rxdata_databuf_t *databuf_new = NULL;
    int port_index;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(databuf, SHR_E_PARAM);
    RXDATA_CHECK_UNIT(unit);
    RXDATA_CHECK_STATUS(true, false);

    entry = traffic_rxdata_checker[unit];

    if (entry->mode == TRAFFIC_RXDATA_CHECKER_MODE_ANY) {
        if (port != -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        port_index = 0; /* insert to list 0 if dont care ingress port number */
    } else {
        if (port < 0 || port >= BCMDRD_CONFIG_MAX_PORTS) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        port_index = port;
    }

    /* allocate a new databuf, and copy from the user */
    SHR_ALLOC(databuf_new, sizeof(traffic_rxdata_databuf_t),
              "bcmaTestCaseTrafficRx");
    SHR_NULL_CHECK(databuf_new, SHR_E_MEMORY);
    sal_memset(databuf_new, 0, sizeof(traffic_rxdata_databuf_t));

    databuf_new->data_len = databuf->data_len;
    SHR_ALLOC(databuf_new->data, databuf_new->data_len,
              "bcmaTestCaseTrafficRxData");
    SHR_NULL_CHECK(databuf_new->data, SHR_E_MEMORY);
    sal_memcpy(databuf_new->data, databuf->data, databuf->data_len);

    /* insert into linked list */
    if (entry->databuf_list[port_index] == NULL) {
        entry->databuf_list[port_index] = databuf_new;
    } else {
        traffic_rxdata_databuf_t *databuf_itr = NULL;

        databuf_itr = entry->databuf_list[port_index];
        while (databuf_itr->next != NULL) {
            databuf_itr = databuf_itr->next;
        }
        databuf_itr->next = databuf_new;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        if (databuf_new != NULL) {
            if (databuf_new->data != NULL) {
                SHR_FREE(databuf_new->data);
            }
            SHR_FREE(databuf_new);
        }
    }
    SHR_FUNC_EXIT();
}

static int
databuf_compare(const traffic_rxdata_databuf_t *buf1, const bcmpkt_data_buf_t *buf2)
{
    if (!buf1 || !buf2) {
        return -1;
    }

    if (buf1->data_len != buf2->data_len) {
        return -1;
    }

    return sal_memcmp(buf1->data, buf2->data, buf1->data_len);
}

static void
databuf_print(int unit, const bcmpkt_data_buf_t *buf, shr_pb_t *pb)
{
    uint8_t *pbuf;
    uint32_t idx;

    pbuf = buf->data;
    shr_pb_printf(pb,"packet content:\n");
    for (idx = 0; idx < buf->data_len; idx++) {
        shr_pb_printf(pb,"%02x ", pbuf[idx]);
    }
    shr_pb_printf(pb, "\n----------------------------------------------------------------------------------\n");
    shr_pb_printf(pb,"\n");
}

static int
bcma_testutil_traffic_rxdata_checker_cb(int unit, int netif_id,
                                        bcmpkt_packet_t *packet, void *cookie)
{
    traffic_rxdata_checker_entry_t *entry = (traffic_rxdata_checker_entry_t *)cookie;
    traffic_rxdata_databuf_t *databuf_itr = NULL;
    traffic_rxdata_databuf_t *databuf_pre = NULL;
    bool current_packet_pass = false;
    int check_port_idx;
    uint32_t src_port;
    shr_pb_t *pb;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(entry->sema, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Receive a packet...\n")));

    /* get ingress port number from packet metadata */
    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_field_get(entry->pktdev_type, packet->pmd.rxpmd,
                                BCMPKT_RXPMD_SRC_PORT_NUM,
                                (uint32_t *)&src_port));

    if (entry->mode == TRAFFIC_RXDATA_CHECKER_MODE_PORT) {
        check_port_idx = (int)src_port;
    } else {
        check_port_idx = 0;
    }

    if (check_port_idx < 0 || check_port_idx >= BCMDRD_CONFIG_MAX_PORTS) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    entry->rx_num[check_port_idx]++;

    /* log packet content if needs */
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE) && packet->data_buf != NULL) {
        pb = shr_pb_create();
        databuf_print(unit, packet->data_buf, pb);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    /* check packet content */
    databuf_pre = NULL;
    databuf_itr = entry->databuf_list[check_port_idx];

    while (databuf_itr != NULL) {
        if (databuf_itr->data == NULL || packet->data_buf == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (databuf_compare(databuf_itr, packet->data_buf) == 0) {
            /*
             * if this packet is matched,
             *  remove this databuf from the checked list
             */
            traffic_rxdata_databuf_t *databuf_next = databuf_itr->next;

            current_packet_pass = true;
            entry->match_num[check_port_idx]++;

            LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Port %d, matched %d packets\n"),
                                 src_port,
                                 entry->match_num[check_port_idx]));

            if (databuf_pre == NULL) {
                entry->databuf_list[check_port_idx] = databuf_next;
            } else {
                databuf_pre->next = databuf_next;
            }

            /* move this databuf into the head of the freed list instead */
            if (entry->databuf_list_freed[check_port_idx] == NULL) {
                entry->databuf_list_freed[check_port_idx] = databuf_itr;
                databuf_itr->next = NULL;
            } else {
                databuf_itr->next = entry->databuf_list_freed[check_port_idx];
                entry->databuf_list_freed[check_port_idx] = databuf_itr;
            }

            /* final result pass if all packets have been received well.
             * However, we still wait a moment to see if there is any other
             * unexpected out-of-number packet.
             * (hence we do not give rx_sema back right now)
             */
            break;
        }

        databuf_pre = databuf_itr;
        databuf_itr = databuf_itr->next;
    }

    if (current_packet_pass == false) {
        /* cannot find the match packet in rx_packet */
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Unexpected packet found, rx %d packets\n"),
                             entry->rx_num[check_port_idx]));
    }

 exit:
    if (current_packet_pass == false) {
        /* give back sema immediately if any unexpected error happened */
        entry->rx_something_wrong = true;
        sal_sem_give(entry->sema);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_rxdata_checker_start(int unit)
{
    traffic_rxdata_checker_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);
    RXDATA_CHECK_UNIT(unit);
    RXDATA_CHECK_STATUS(true, false);

    entry = traffic_rxdata_checker[unit];

    entry->rx_something_wrong = false;

    SHR_IF_ERR_EXIT
        (bcmpkt_rx_register(unit, entry->netif_id, 0,
                            bcma_testutil_traffic_rxdata_checker_cb,
                            (void*)entry));

    traffic_rxdata_checker_start[unit] = true;

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_rxdata_checker_execute(int unit, int wait_time,
                                             bool *ret_result_pass)
{
    traffic_rxdata_checker_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_result_pass, SHR_E_PARAM);
    RXDATA_CHECK_UNIT(unit);
    RXDATA_CHECK_STATUS(true, true);

    entry = traffic_rxdata_checker[unit];

    cli_out("rx check....\n");

    *ret_result_pass = false;
    sal_sem_take(entry->sema, wait_time);

    if (entry->rx_something_wrong == true) {
        /* rx cb found there is something wrong */
        cli_out("something wrong in rx cb\n");
        SHR_EXIT();
    } else {
        /* after timer expired,
         * check whether all expected packet has been received well
         */
        int i;

        for (i = 0; i < BCMDRD_CONFIG_MAX_PORTS; i++) {
            if (entry->databuf_list[i] != NULL) {
                cli_out("port %d not match all packets (matched %d, rx %d)\n",
                        i, entry->match_num[i], entry->rx_num[i]);
                SHR_EXIT();
            }
        }
    }

    *ret_result_pass = true;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Rxdata check passed\n")));

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_rxdata_checker_execute_per_port(int unit, int port,
                                                      int wait_time,
                                                      bool *ret_result_pass)
{
    traffic_rxdata_checker_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_result_pass, SHR_E_PARAM);
    RXDATA_CHECK_UNIT(unit);
    RXDATA_CHECK_STATUS(true, true);

    entry = traffic_rxdata_checker[unit];

    cli_out("Data Integrity check for port: %d....\n", port);

    *ret_result_pass = false;
    sal_sem_take(entry->sema, wait_time);

    if (entry->rx_something_wrong == true) {
        /* rx cb found there is something wrong */
        cli_out("something wrong in rx cb\n");
        SHR_EXIT();
    } else {
        /* after timer expired,
         * check whether all expected packet has been received well
         */
       if (entry->databuf_list[port] != NULL) {
           cli_out("port %d not receive all packet (only %d)\n",
                   port, entry->match_num[port]);
           SHR_EXIT();
       }
    }

    *ret_result_pass = true;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Rxdata check passed for port: %d\n"), port));

 exit:
    SHR_FUNC_EXIT();
}


int
bcma_testutil_traffic_rxdata_checker_stop(int unit)
{
    traffic_rxdata_checker_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);
    RXDATA_CHECK_UNIT(unit);
    RXDATA_CHECK_STATUS(true, true);

    entry = traffic_rxdata_checker[unit];

    SHR_IF_ERR_EXIT
        (bcmpkt_rx_unregister(unit, entry->netif_id,
                              bcma_testutil_traffic_rxdata_checker_cb,
                              (void*)entry));

    traffic_rxdata_checker_start[unit] = false;

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_rxdata_checker_destroy(int unit)
{
    traffic_rxdata_checker_entry_t *entry = NULL;
    int i;

    SHR_FUNC_ENTER(unit);
    RXDATA_CHECK_UNIT(unit);
    RXDATA_CHECK_STATUS(true, false);

    entry = traffic_rxdata_checker[unit];

    /* free sema */
    if (entry->sema != NULL) {
        sal_sem_destroy(entry->sema);
        entry->sema = NULL;
    }

    /* free databuf in two lists */
    for (i = 0; i < BCMDRD_CONFIG_MAX_PORTS; i++) {

        traffic_rxdata_databuf_t *databuf_entry;
        traffic_rxdata_databuf_t *databuf_next;

        databuf_entry = entry->databuf_list[i];
        while (databuf_entry != NULL) {
            databuf_next = databuf_entry->next;
            SHR_FREE(databuf_entry->data);
            SHR_FREE(databuf_entry);
            databuf_entry = databuf_next;
        }

        databuf_entry = entry->databuf_list_freed[i];
        while (databuf_entry != NULL) {
            databuf_next = databuf_entry->next;
            SHR_FREE(databuf_entry->data);
            SHR_FREE(databuf_entry);
            databuf_entry = databuf_next;
        }
    }

    /* destroy process success */
    sal_free(entry);
    traffic_rxdata_checker[unit] = NULL;

 exit:
    SHR_FUNC_EXIT();
}

typedef struct traffic_latency_checker_entry_s {
    /* the length of the original sending packet */
    uint32_t pktlen;

    /* the number of the original sending packet */
    uint32_t pktcnt;

    /* netif id */
    int netif_id;

    /* rx cb found something wrong */
    bool rx_something_wrong;

    /* semaphore */
    sal_sem_t sema;

    /* recevied packet num */
    uint32_t rx_num;

    /* The minimum of latency */
    uint64_t latency_min;

    /* The maximum of latency */
    uint64_t latency_max;

    /* The total sumary of latency */
    uint64_t latency_sum;
} traffic_latency_checker_entry_t;

static traffic_latency_checker_entry_t
            *traffic_latency_checker[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

#define LATENCY_CHECK_UNIT(_u_)                              \
    do {                                                     \
        if ((_u_) < 0 && (_u_) >= BCMDRD_CONFIG_MAX_UNITS) { \
            SHR_ERR_EXIT(SHR_E_UNIT);                        \
        }                                                    \
    } while (0)

#define LATENCY_CHECK_STATUS(_expect_init_)                                     \
    do {                                                                        \
        if (traffic_latency_checker[unit] == NULL && (_expect_init_) == true) { \
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);                                  \
        }                                                                       \
        if (traffic_latency_checker[unit] != NULL && (_expect_init_) == false) {\
                SHR_ERR_EXIT(SHR_E_EXISTS);                                     \
        }                                                                       \
    } while (0)

static int
bcma_testutil_traffic_latency_checker_cb(int unit, int netif_id,
                                         bcmpkt_packet_t *packet, void *cookie)
{
    traffic_latency_checker_entry_t *entry = (traffic_latency_checker_entry_t *)cookie;
    bool current_packet_pass = true;
    shr_pb_t *pb = NULL;
    uint64_t ts1, ts2, latency;
    uint8_t *ts1_data, *ts2_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(entry->sema, SHR_E_PARAM);

    pb = shr_pb_create();
    if (pb == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    databuf_print(unit, packet->data_buf, pb);

    if (packet->data_buf->data_len !=
        (entry->pktlen + 2 * TESTUTIL_LATENCY_TIMESTAMP_SHIMLEN)) {
        cli_out("Receive a unexpected packet\n");
        current_packet_pass = false;
    } else {
        entry->rx_num++;
        ts1_data = packet->data_buf->data + (entry->pktlen + 4);
        ts2_data = packet->data_buf->data + (entry->pktlen + 18);

        ts1 = ((uint64_t)ts1_data[0] << 40) +
              ((uint64_t)ts1_data[1] << 32) +
              ((uint64_t)ts1_data[2] << 24) +
              ((uint64_t)ts1_data[3] << 16) +
              ((uint64_t)ts1_data[4] << 8) +
              ((uint64_t)ts1_data[5]);

        ts2 = ((uint64_t)ts2_data[0] << 40) +
              ((uint64_t)ts2_data[1] << 32) +
              ((uint64_t)ts2_data[2] << 24) +
              ((uint64_t)ts2_data[3] << 16) +
              ((uint64_t)ts2_data[4] << 8) +
              ((uint64_t)ts2_data[5]);

        if (ts2 < ts1) {
            latency = ts2 + 0x1ffffffffffff - ts1;
        } else {
            latency = ts2 - ts1;
        }
        if (latency < entry->latency_min) {
            entry->latency_min = latency;
        }
        if (latency > entry->latency_max) {
            entry->latency_max = latency;
        }
        entry->latency_sum += latency;

        shr_pb_printf(pb,"timestamp 1: %02"PRIx32" %02"PRIx32" %02"PRIx32" %02"PRIx32" %02"PRIx32" %02"PRIx32"\n",
                      ts1_data[0], ts1_data[1], ts1_data[2],
                      ts1_data[3], ts1_data[4], ts1_data[5]);
        shr_pb_printf(pb,"timestamp 2: %02"PRIx32" %02"PRIx32" %02"PRIx32" %02"PRIx32" %02"PRIx32" %02"PRIx32" (diff : %"PRIu64")\n",
                      ts2_data[0], ts2_data[1], ts2_data[2],
                      ts2_data[3], ts2_data[4], ts2_data[5], latency);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Received pkt: %s\n"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

 exit:
    if (current_packet_pass == false) {
        /* give back sema immediately if any unexpected error happened */
        entry->rx_something_wrong = true;
        sal_sem_give(entry->sema);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_latency_checker_create(int unit, int netif_id,
                                             uint32_t pktlen, uint32_t pktcnt)
{
    traffic_latency_checker_entry_t *entry_new = NULL;

    SHR_FUNC_ENTER(unit);
    LATENCY_CHECK_UNIT(unit);
    LATENCY_CHECK_STATUS(false);

    /* allocate new entry */
    SHR_ALLOC(entry_new, sizeof(traffic_latency_checker_entry_t),
              "bcmaTestCaseTrafficLatencyCheck");
    SHR_NULL_CHECK(entry_new, SHR_E_MEMORY);
    sal_memset(entry_new, 0,
               sizeof(traffic_latency_checker_entry_t));

    entry_new->netif_id = netif_id;
    entry_new->pktlen = pktlen;
    entry_new->pktcnt = pktcnt;
    entry_new->rx_something_wrong = false;
    entry_new->sema = sal_sem_create("latency_sema", SAL_SEM_BINARY, 0);
    entry_new->rx_num = 0;
    entry_new->latency_min = -1;
    entry_new->latency_max = 0;
    entry_new->latency_sum = 0;
    if (entry_new->sema == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_rx_register(unit, entry_new->netif_id, 0,
                            bcma_testutil_traffic_latency_checker_cb,
                            (void*)entry_new));

    /* create process success */
    traffic_latency_checker[unit] = entry_new;

 exit:
    if (SHR_FUNC_ERR()) {
        if (entry_new != NULL) {
            if (entry_new->sema != NULL) {
                sal_sem_destroy(entry_new->sema);
                entry_new->sema = NULL;
            }
            sal_free(entry_new);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_latency_checker_execute(int unit, int wait_time,
                                              bool *ret_result_pass)
{
    traffic_latency_checker_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_result_pass, SHR_E_PARAM);
    LATENCY_CHECK_UNIT(unit);
    LATENCY_CHECK_STATUS(true);

    *ret_result_pass = false;
    entry = traffic_latency_checker[unit];

    sal_sem_take(entry->sema, wait_time);

    if (entry->rx_something_wrong == true) {
        /* rx cb found there is something wrong */
        cli_out("something wrong in rx cb\n");
        SHR_EXIT();
    } else if (entry->rx_num != entry->pktcnt) {
        cli_out("Only receive %d packets (should be %d)\n",
                entry->rx_num, entry->pktcnt);
        SHR_EXIT();
    }
    cli_out("Latency:max ");
    bcma_testutil_stat_show(entry->latency_max, 1, true);
    cli_out(" ns, min ");
    bcma_testutil_stat_show(entry->latency_min, 1, true);
    cli_out(" ns, average ");
    bcma_testutil_stat_show(entry->latency_sum, entry->pktcnt, true);
    cli_out(" ns\n");
    *ret_result_pass = true;

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_latency_checker_destroy(int unit)
{
    traffic_latency_checker_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);
    LATENCY_CHECK_UNIT(unit);
    LATENCY_CHECK_STATUS(true);

    entry = traffic_latency_checker[unit];

    SHR_IF_ERR_EXIT
        (bcmpkt_rx_unregister(unit, entry->netif_id,
                              bcma_testutil_traffic_latency_checker_cb,
                              (void*)entry));

    if (entry->sema != NULL) {
        sal_sem_destroy(entry->sema);
        entry->sema = NULL;
    }

    /* destroy process success */
    sal_free(entry);
    traffic_latency_checker[unit] = NULL;

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_src_dst_packet_count_check(int unit, int src_port, int
    dst_port, bool *test_result)
{
    bcma_testutil_stat_t *stat_array = NULL;
    int stat_size = 0;
    uint64_t src_rbyt, src_rpkt, src_tbyt, src_tpkt;
    uint64_t dst_rbyt, dst_rpkt, dst_tbyt, dst_tpkt;
    uint64_t tot_rbyt = 0, tot_rpkt = 0, tot_tbyt = 0, tot_tpkt = 0;

    SHR_FUNC_ENTER(unit);

    *test_result = true;

    /* Get source port counters */
    SHR_IF_ERR_EXIT
        (bcma_testutil_stat_mac_get(unit, src_port, &stat_array, &stat_size));
    src_rbyt = stat_array[RX_BYTES].value;
    src_rpkt = stat_array[RX_PKT].value;
    src_tbyt = stat_array[TX_BYTES].value;
    src_tpkt = stat_array[TX_PKT].value;

    tot_rbyt += src_rbyt;
    tot_rpkt += src_rpkt;
    tot_tbyt += src_tbyt;
    tot_tpkt += src_tpkt;

    if (src_port != dst_port) {
        /* Get dest port counters */
        SHR_IF_ERR_EXIT
            (bcma_testutil_stat_mac_get(unit, dst_port, &stat_array,
                                        &stat_size));
        dst_rbyt = stat_array[RX_BYTES].value;
        dst_rpkt = stat_array[RX_PKT].value;
        dst_tbyt = stat_array[TX_BYTES].value;
        dst_tpkt = stat_array[TX_PKT].value;

        tot_rbyt += dst_rbyt;
        tot_rpkt += dst_rpkt;
        tot_tbyt += dst_tbyt;
        tot_tpkt += dst_tpkt;
    }

    if (tot_rpkt != tot_tpkt) {
        cli_out(" FAIL: RPKT vs TPKT mismatch \n");
        cli_out("Port 1:%d port 2: %d total rpkt : %"PRIu64" total tpkt: %"PRIu64" \n",
                src_port, dst_port, tot_rpkt, tot_tpkt);
        *test_result = false;
    } else {
        cli_out(" PASS: RPKT vs TPKT \n");
        cli_out("Port 1:%d port 2: %d total rpkt : %"PRIu64" total tpkt: %"PRIu64" \n",
                src_port, dst_port, tot_rpkt, tot_tpkt);
    }

    if (tot_rbyt != tot_tbyt) {
        cli_out(" FAIL: RBYT vs TBYT mismatch \n");
        cli_out("Port 1:%d port 2: %d total rbyt : %"PRIu64" total tbyt: %"PRIu64" \n",
                src_port, dst_port, tot_rbyt, tot_tbyt);
        *test_result = false;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_port_error_packets_check(int unit, bcmdrd_pbmp_t pbmp,
    bool *test_result)
{
    bcma_testutil_stat_t *stat_array = NULL;
    int stat_size = 0;
    int stat_idx;
    int port;
    uint64_t err_pkts;

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_stat_macerr_get(unit, port, &stat_array,
                                           &stat_size));

        for (stat_idx = 0; stat_idx < stat_size; stat_idx++) {
            if (stat_idx == RX_PROMISCUOUS_PKT) {
                continue;
            }
            err_pkts = stat_array[stat_idx].value;
            if (err_pkts != 0) {
                cli_out("FAIL: Port: %d %s: %"PRIu64" \n", port,
                        stat_array[stat_idx].name, err_pkts);
                err_pkts = 0;
                *test_result = false;
            }
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*-----------    Per-queue bandwidth checker  -----------------*/

/* Structure of traffic Queue Bandwidth check */
typedef struct traffic_queue_bw_checker_entry_s {
    /* port needs to be checked */
    int port;

    /* Queue_id to be checked*/
    int queue_id;

    /*Queue type to be checked */
    traffic_queue_type_t queue_type;

    /* expected rate */
    uint64_t exp_rate;

    /* expected rate error tolerance perc */
    int rate_tol_perc;

    /* cache for last counter value */
    uint64_t q_byt_pre, q_pkt_pre;

    /*IPG bytes*/
    int ipg_bytes;

    /* show message */
    char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];

    /* timer */
    sal_usecs_t time_pre;

    /* next entry */
    struct traffic_queue_bw_checker_entry_s *next;
} traffic_queue_bw_checker_entry_t;

static bool traffic_queue_bw_checker_init[BCMDRD_CONFIG_MAX_UNITS] = {false};
static bool traffic_queue_bw_checker_start[BCMDRD_CONFIG_MAX_UNITS] = {false};
static traffic_queue_bw_checker_entry_t
            *traffic_queue_bw_checker[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

#define QUEUE_BW_CHECK_UNIT(_u_)                              \
    do {                                                     \
        if ((_u_) < 0 || (_u_) >= BCMDRD_CONFIG_MAX_UNITS) { \
            SHR_ERR_EXIT(SHR_E_UNIT);                 \
        }                                                    \
    } while (0)

#define QUEUE_BW_CHECK_STATUS(_expect_init_, _expect_start_)            \
    do {                                                               \
        if (traffic_queue_bw_checker_init[unit] != (_expect_init_)) {   \
            if ((_expect_init_) == true) {                             \
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);                  \
            } else {                                                   \
                SHR_ERR_EXIT(SHR_E_EXISTS);                     \
            }                                                          \
        }                                                              \
        if (traffic_queue_bw_checker_start[unit] != (_expect_start_)) { \
            if ((_expect_start_) == true) {                            \
                SHR_ERR_EXIT(SHR_E_CONFIG);                     \
            } else {                                                   \
                SHR_ERR_EXIT(SHR_E_BUSY);                       \
            }                                                          \
        }                                                              \
    } while (0)

int
bcma_testutil_traffic_queue_bw_checker_create(int unit)
{
    SHR_FUNC_ENTER(unit);
    QUEUE_BW_CHECK_UNIT(unit);
    QUEUE_BW_CHECK_STATUS(false, false);

    traffic_queue_bw_checker_init[unit] = true;

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_queue_bw_checker_add(int unit, int port,
                                           int q_id, traffic_queue_type_t q_type,
                                           uint64_t exp_rate, int rate_tol_perc,
                                           int ipg_bytes,
                                           const char *show_msg)
{
    traffic_queue_bw_checker_entry_t *queue_bw_new = NULL;
    size_t show_msg_len;

    SHR_FUNC_ENTER(unit);
    QUEUE_BW_CHECK_UNIT(unit);
    QUEUE_BW_CHECK_STATUS(true, false);

    /* allocate new entry */
    SHR_ALLOC(queue_bw_new, sizeof(traffic_queue_bw_checker_entry_t),
              "bcmaTestCaseQueueBWCheck");
    SHR_NULL_CHECK(queue_bw_new, SHR_E_MEMORY);
    sal_memset(queue_bw_new, 0,
               sizeof(traffic_queue_bw_checker_entry_t));

    queue_bw_new->port = port;
    queue_bw_new->queue_id = q_id;
    queue_bw_new->queue_type = q_type;
    queue_bw_new->exp_rate = exp_rate;
    queue_bw_new->rate_tol_perc = rate_tol_perc;
    queue_bw_new->ipg_bytes = ipg_bytes;
    show_msg_len = sal_strlen(show_msg);
    if (show_msg_len >= TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sal_memcpy(queue_bw_new->show_msg, show_msg, show_msg_len + 1);

    /* insert into linked list */
    if (traffic_queue_bw_checker[unit] == NULL) {
        traffic_queue_bw_checker[unit] = queue_bw_new;
    } else {
        traffic_queue_bw_checker_entry_t *queue_bw_ptr = NULL;

        queue_bw_ptr = traffic_queue_bw_checker[unit];
        while (queue_bw_ptr->next != NULL) {
            queue_bw_ptr = queue_bw_ptr->next;
        }
        queue_bw_ptr->next = queue_bw_new;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(queue_bw_new);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_queue_bw_checker_start(int unit)
{
    traffic_queue_bw_checker_entry_t *queue_bw_ptr = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value[1] = {1};

    SHR_FUNC_ENTER(unit);
    QUEUE_BW_CHECK_UNIT(unit);
    QUEUE_BW_CHECK_STATUS(true, false);

    /* start counter polling thread for more accurate statistic */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_CONTROL", &entry_hdl));
    queue_bw_ptr = traffic_queue_bw_checker[unit];
    /* clear stat for each port */
    while (queue_bw_ptr != NULL) {
        int port = queue_bw_ptr->port;
        SHR_IF_ERR_EXIT
            (bcma_testutil_ctr_egr_perq_stat_clear(unit,
                                                   queue_bw_ptr->port,
                                                   queue_bw_ptr->queue_id,
                                                   queue_bw_ptr->queue_type));
        queue_bw_ptr->q_byt_pre = 0;
        queue_bw_ptr->q_pkt_pre = 0;
        queue_bw_ptr->time_pre = sal_time_usecs();
        queue_bw_ptr = queue_bw_ptr->next;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl, "PORT_ID", port, value, 1));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "COLLECTION_ENABLE", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));


    /* start process okay */
    traffic_queue_bw_checker_start[unit] = true;

 exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_queue_bw_checker_execute(int unit, bool need_check_rate,
                                               bool *ret_result_pass)
{
    traffic_queue_bw_checker_entry_t *queue_bw_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_result_pass, SHR_E_PARAM);
    QUEUE_BW_CHECK_UNIT(unit);
    QUEUE_BW_CHECK_STATUS(true, true);

    *ret_result_pass = true;

    /* iterate each port to poll counter */
    queue_bw_ptr = traffic_queue_bw_checker[unit];
    while (queue_bw_ptr != NULL) {

        uint64_t perq_byt, perq_pkt;
        uint64_t perq_byt_diff, perq_pkt_diff;
        uint64_t rate;
        int port = queue_bw_ptr->port;
        sal_usecs_t time = sal_time_usecs();
        int time_diff;
        int err_perc;

        /* counter polling */
        SHR_IF_ERR_EXIT
            (bcma_testutil_ctr_egr_perq_stat_get
                (unit, port, queue_bw_ptr->queue_id, queue_bw_ptr->queue_type,
                 true, &perq_pkt));

        SHR_IF_ERR_EXIT
            (bcma_testutil_ctr_egr_perq_stat_get
                (unit, port, queue_bw_ptr->queue_id, queue_bw_ptr->queue_type,
                 false, &perq_byt));

        perq_byt_diff = perq_byt - queue_bw_ptr->q_byt_pre;
        perq_pkt_diff = perq_pkt - queue_bw_ptr->q_pkt_pre;

        queue_bw_ptr->q_byt_pre = perq_byt;
        queue_bw_ptr->q_pkt_pre = perq_pkt;

        time_diff = (int)(time - queue_bw_ptr->time_pre);
        queue_bw_ptr->time_pre = time;

        rate = 8 * (perq_byt_diff+ (perq_pkt_diff * queue_bw_ptr->ipg_bytes))
            * SECOND_USEC / time_diff;

        /* output TX status */
        cli_out("%s", queue_bw_ptr->show_msg);
        cli_out("TX Packets +");
        bcma_testutil_stat_show(perq_pkt_diff, 1, true);
        cli_out(", Bytes +");
        bcma_testutil_stat_show(perq_byt_diff, 1, true);
        cli_out(" [");
        bcma_testutil_stat_show(rate, 1, true);
        cli_out("b/s]\n");

        /* check rate if need */
        if (need_check_rate == true) {
            if (rate < queue_bw_ptr->exp_rate) {
                err_perc = (int)((queue_bw_ptr->exp_rate - rate) * 100 /
                        queue_bw_ptr->exp_rate);
                if (err_perc >= queue_bw_ptr->rate_tol_perc) {
                    *ret_result_pass = false;
                    cli_out("Error: %s traffic too slow.\n",
                            queue_bw_ptr->show_msg);
                    cli_out("  ==> expected rate is : ");
                    bcma_testutil_stat_show(queue_bw_ptr->exp_rate, 1, true);
                    cli_out("b/s\n");
                }
            }
        }

        queue_bw_ptr = queue_bw_ptr->next;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_queue_bw_checker_stop(int unit)
{

    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    traffic_queue_bw_checker_entry_t *queue_bw_ptr = NULL;
    uint64_t value[1] = {0};

    SHR_FUNC_ENTER(unit);
    QUEUE_BW_CHECK_UNIT(unit);
    QUEUE_BW_CHECK_STATUS(true, true);

    queue_bw_ptr = traffic_queue_bw_checker[unit];
    /* stop counter polling thread for more accurate statistic */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_CONTROL", &entry_hdl));
    while (queue_bw_ptr != NULL) {
        int port = queue_bw_ptr->port;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl, "PORT_ID", port, value, 1));
        queue_bw_ptr = queue_bw_ptr->next;
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "COLLECTION_ENABLE", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
     /* stop process okay */
    traffic_queue_bw_checker_start[unit] = false;

 exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_traffic_queue_bw_checker_destroy(int unit)
{
    traffic_queue_bw_checker_entry_t *queue_bw_ptr = NULL;
    traffic_queue_bw_checker_entry_t *queue_bw_next = NULL;

    SHR_FUNC_ENTER(unit);
    QUEUE_BW_CHECK_UNIT(unit);
    QUEUE_BW_CHECK_STATUS(true, false);

    queue_bw_ptr = traffic_queue_bw_checker[unit];
    while (queue_bw_ptr != NULL) {
        queue_bw_next = queue_bw_ptr->next;
        sal_free(queue_bw_ptr);
        queue_bw_ptr = queue_bw_next;
    }
    traffic_queue_bw_checker[unit] = NULL;
    traffic_queue_bw_checker_init[unit] = false;

 exit:
    SHR_FUNC_EXIT();
}

/*-------------------------------------------------------------------------*/

/* Structure of traffic Queue Bandwidth check */
typedef struct traffic_check_result_disp_s {
    /* show message */
    char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];

    /* test result */
    bool test_result;

    /* next entry */
    struct traffic_check_result_disp_s *next;
} traffic_check_result_disp_t;

static traffic_check_result_disp_t
            *traffic_check_result_display[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

int
bcma_testutil_traffic_check_result_disp_add (int unit, bool test_result,
                                           const char *show_msg)
{
    traffic_check_result_disp_t *check_disp = NULL;
    size_t show_msg_len;

    SHR_FUNC_ENTER(unit);

    /* allocate new entry */
    SHR_ALLOC(check_disp, sizeof(traffic_check_result_disp_t),
              "bcmaTrafficCheckResultDisplay");
    SHR_NULL_CHECK(check_disp, SHR_E_MEMORY);
    sal_memset(check_disp, 0,
               sizeof(traffic_check_result_disp_t));

    check_disp->test_result = test_result;
    show_msg_len = sal_strlen(show_msg);
    if (show_msg_len >= TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sal_memcpy(check_disp->show_msg, show_msg, show_msg_len + 1);

    /* insert into linked list */
    if (traffic_check_result_display[unit] == NULL) {
        traffic_check_result_display[unit] = check_disp;
    } else {
        traffic_check_result_disp_t *check_disp_ptr = NULL;

        check_disp_ptr = traffic_check_result_display[unit];
        while (check_disp_ptr->next != NULL) {
            check_disp_ptr = check_disp_ptr->next;
        }
        check_disp_ptr->next = check_disp;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(check_disp);
    }
    SHR_FUNC_EXIT();
}

static void
bcma_testutil_traffic_check_result_disp_destroy(int unit)
{
    traffic_check_result_disp_t *check_ptr = NULL;
    traffic_check_result_disp_t *check_next = NULL;


    check_ptr = traffic_check_result_display[unit];
    while (check_ptr != NULL) {
        check_next = check_ptr->next;
        sal_free(check_ptr);
        check_ptr = check_next;
    }
    traffic_check_result_display[unit] = NULL;

}


int
bcma_testutil_traffic_check_result_display (int unit)
{
    traffic_check_result_disp_t *check_disp = NULL;

    if (traffic_check_result_display[unit] != NULL) {
        check_disp = traffic_check_result_display[unit];
        while (check_disp->next != NULL) {
            cli_out("%s", check_disp->show_msg);
            if (check_disp->test_result == true) {
                cli_out(" : PASS \n");
            } else {
                cli_out(" : FAIL \n");
            }
            check_disp = check_disp->next;
        }
        /* Display last checker of linked list */
        cli_out("%s", check_disp->show_msg);
        if (check_disp->test_result == true) {
            cli_out(" : PASS \n");
        } else {
            cli_out(" : FAIL \n");
        }
    }

    bcma_testutil_traffic_check_result_disp_destroy(unit);
    return SHR_E_NONE;
}

int
bcma_testutil_traffic_check_cb(int unit, void *bp, uint32_t option)
{
    bool test_result;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcma_testutil_drv_check_health(unit, &test_result);
    if (rv == SHR_E_UNAVAIL) {
        /* Ignore if check health is not available for a device */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                           "Check Health"));

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                           "Test Result"));

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_check_result_display(unit));
    if (test_result == false) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
exit:
    SHR_FUNC_EXIT();
}
