/*! \file bcma_bcmpkt_test.c
 *
 * General functions for Packet I/O test.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <bcmlt/bcmlt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/io/bcma_io_file.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>
#include <bcma/bcmpkt/bcma_bcmpkt_rx.h>
#include "cmd/bcma_bcmpktcmd_internal.h"
#include <bcma/bcmpkt/bcma_bcmpkt_test_internal.h>
#include <bcmltd/chip/bcmltd_str.h>

/*! Device specific attach function type. */
typedef int (*bcma_bcmpkt_test_drv_attach_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { bcma_bcmpkt_test_##_bd##_drv_attach },
static struct {
    bcma_bcmpkt_test_drv_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#ifdef TIME_UPDATE_PERPACKET
#undef TIME_UPDATE_PERPACKET
#endif

static bcma_bcmpkt_test_drv_t *pkttest_drv = NULL;
static bool pkttest_drv_attached = FALSE;

#define DMAC_ADDR_BYTE_OFF_5       5
#define SMAC_ADDR_BYTE_OFF_5       11
#define ETHER_TYPE_BYTE_OFF_0      12
#define ETHER_TYPE_BYTE_OFF_1      13

typedef struct cpu_stats_s{
    uint64_t user;
    uint64_t kernel;
    uint64_t idle;
    uint64_t nice;
    uint64_t iowait;
    uint64_t irq;
    uint64_t softirq;
    uint64_t intr;
    uint64_t total;
} cpu_stats_t;

static int pkttest_max_frame_size = MAX_FRAME_SIZE_DEF;

#define MAX(a, b)              ((a) > (b) ? (a) : (b))

static char *
advance_to_next_number(char *str)
{
    if (str == NULL) {
        return NULL;
    }
    while (*str && (!sal_isdigit(*str))) {
        ++str;
    }
    return(*str)? str : NULL;
}

static int
cpu_stats_get(cpu_stats_t *cpu_stats)
{
    bcma_io_file_handle_t fh = NULL;
    char line[128] = {0};
    char *seperator = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    sal_memset(cpu_stats, 0, sizeof(*cpu_stats));

    if ((fh = bcma_io_file_open("/proc/stat", "r")) == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Open /proc/stat failed\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    bcma_io_file_gets(fh, line, sizeof(line));

    if ((seperator = sal_strstr(line, "cpu ")) == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    seperator = advance_to_next_number(seperator);
    cpu_stats->user = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->nice = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->kernel = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->idle = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->iowait = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->irq = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    seperator = advance_to_next_number(seperator);
    cpu_stats->softirq = (uint64_t)sal_strtoull(seperator, &seperator, 0);

    while (bcma_io_file_gets(fh, line, sizeof(line))) {
        if ((seperator = sal_strstr(line, "intr ")) != NULL) {
            seperator = advance_to_next_number(seperator);
            cpu_stats->intr = (uint64_t)sal_strtoull(seperator, &seperator, 0);
            break;
        }
    }

    cpu_stats->total += cpu_stats->user;
    cpu_stats->total += cpu_stats->kernel;
    cpu_stats->total += cpu_stats->nice;
    cpu_stats->total += cpu_stats->idle;
    cpu_stats->total += cpu_stats->iowait;
    cpu_stats->total += cpu_stats->irq;
    cpu_stats->total += cpu_stats->softirq;

exit:
    if (fh) {
        bcma_io_file_close(fh);
    }
    SHR_FUNC_EXIT();
}

static int
packet_measure(int unit, int netif_id, bcmpkt_packet_t *packet, void *cookie)
{
    bcma_bcmpkt_test_rx_param_t *rx_param = NULL;
    static bool count_start = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cookie, SHR_E_PARAM);

    rx_param = (bcma_bcmpkt_test_rx_param_t *)cookie;
    if (rx_param->count_enable) {
        if(!count_start) {
            rx_param->pkt_received = 0;
            rx_param->time_start = sal_time_usecs();
            rx_param->time_end = sal_time_usecs();
            count_start = TRUE;
        }
        rx_param->pkt_received++;
#ifdef PKTTEST_TIME_UPDATE_PERPACKET
        rx_param->time_end = sal_time_usecs();
#endif
    } else if (count_start) {
        rx_param->time_end = sal_time_usecs();
        count_start = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
test_stats_dump(int time_diff, uint64_t pkt_count, uint32_t pkt_len,
                cpu_stats_t *cpu_end, cpu_stats_t *cpu_start)
{
    int bps = 0;
    int pps = 0;
    int total_diff = 0;
    int user_diff = 0;
    int kernel_diff = 0;
    int idle_diff = 0;
    int nice_diff = 0;
    int iowait_diff = 0;
    int irq_diff = 0;
    int softirq_diff = 0;
    int intr_diff = 0;

    if (cpu_end != NULL && cpu_start != NULL) {
        total_diff = (int)(cpu_end->total - cpu_start->total);
    }
    if (total_diff > 0) {
        user_diff = (int)(cpu_end->user - cpu_start->user);
        kernel_diff = (int)(cpu_end->kernel - cpu_start->kernel);
        idle_diff = (int)(cpu_end->idle - cpu_start->idle);
        nice_diff = (int)(cpu_end->nice - cpu_start->nice);
        iowait_diff = (int)(cpu_end->iowait - cpu_start->iowait);
        irq_diff = (int)(cpu_end->irq - cpu_start->irq);
        softirq_diff = (int)(cpu_end->softirq - cpu_start->softirq);
        intr_diff = (int)(cpu_end->intr - cpu_start->intr);

        user_diff = (10000 * user_diff) / total_diff;
        kernel_diff = (10000 * kernel_diff) / total_diff;
        idle_diff = (10000 * idle_diff) / total_diff;
        nice_diff = (10000 * nice_diff) / total_diff;
        iowait_diff = (10000 * iowait_diff) / total_diff;
        irq_diff = (10000 * irq_diff) / total_diff;
        softirq_diff = (10000 * softirq_diff) / total_diff;
    }

    /* round to ms */
    time_diff = (time_diff + 500) / 1000;
    if (time_diff > 0) {
        pps = (int)((1000UL * pkt_count) / time_diff);
        bps = (int)((pkt_len * pkt_count) / time_diff);
    }

    cli_out("  %5"PRId32" |%4"PRId32".%03"PRId32" |%4"PRId32".%03"PRId32" |%8"PRId32" |%2"PRId32".%03"PRId32" |%7"PRId32" |%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32" |%2"PRId32".%02"PRId32"|%2"PRId32".%02"PRId32"",
            pkt_len,
            (pps / 1000), (pps % 1000),
            (bps / 1000), (bps % 1000),
            (int)pkt_count,
            time_diff / 1000, time_diff % 1000,
            intr_diff,
            (idle_diff / 100), (idle_diff % 100),
            (user_diff / 100), (user_diff % 100),
            (kernel_diff / 100), (kernel_diff % 100),
            (nice_diff / 100), (nice_diff % 100),
            (iowait_diff / 100), (iowait_diff % 100),
            (irq_diff / 100), (irq_diff % 100),
            (softirq_diff / 100), (softirq_diff % 100));
    cli_out("\n");

    return;
}

static int
packet_send(int unit, int netif_id, bcmpkt_packet_t *packet, int count)
{
    bcmpkt_packet_t *txpkt = NULL;
    int buf_unit = unit;
    bcmpkt_dev_drv_types_t dev_drv_type;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_drv_type_get(unit, &dev_drv_type));

    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* KNET mode, use shared buff pool. */
        buf_unit = BCMPKT_BPOOL_SHARED_ID;
    }

    SHR_IF_ERR_EXIT(bcmpkt_packet_clone(unit, packet, &txpkt));

    while(count--) {
        if (txpkt->data_buf) {
            bcmpkt_data_buf_free(txpkt->unit, txpkt->data_buf);
            txpkt->data_buf = NULL;
        }
        /* Copy current data buffer for this transmition. */
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_copy(buf_unit, packet->data_buf,
                                  &txpkt->data_buf));
        txpkt->unit = buf_unit;

        rv = bcmpkt_tx(unit, netif_id, txpkt);
        if (SHR_FAILURE(rv) && rv != SHR_E_RESOURCE) {
            /*
             * Underlying packet DMA driver reports SHR_E_RESOURCE
             * when TX HW temporary unavaialble. Skip the error code
             * and do the next loop.
             */
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    if (txpkt) {
        bcmpkt_free(txpkt->unit, txpkt);
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_drv_set(int unit, bcma_bcmpkt_test_drv_t *drv)
{
    pkttest_drv = drv;

    return SHR_E_NONE;
}

static int
lt_ifp_policy_update(int unit, uint64_t entry_id, uint64_t policy_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pkttest_drv, SHR_E_INIT);
    SHR_NULL_CHECK(pkttest_drv->lt_policy_update, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (pkttest_drv->lt_policy_update(unit, entry_id, policy_id));
exit:
    SHR_FUNC_EXIT();
}

static int
lt_port_linkscan_enable_set(int unit, int port, const char *scan_mode)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *cur_scan_mode;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_PORT_CONTROL", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));
    rv = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_get(ent_hdl, "LINKSCAN_MODE",
                                          &cur_scan_mode));
    }


    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "LINKSCAN_MODE", scan_mode));

    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL));
    } else if ((rv == SHR_E_NONE) &&
               (!sal_strncmp(cur_scan_mode, "NO_SCAN",
                             sal_strlen("NO_SCAN")))) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(ent_hdl);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_port_lpbk_mode_set(int unit, int port, const char *lb_mode_str)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;
    bcmlt_opcode_t opcode;
    uint64_t link_status;
    bool timeout = false;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Add port to linkscan which is mandatory for the port
     * link to be UP and switch traffic.
     */
    SHR_IF_ERR_EXIT
        (lt_port_linkscan_enable_set(unit, port, "SOFTWARE"));

    /* Set new loopback mode. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, PC_PORTs, &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, PORT_IDs, port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, LOOPBACK_MODEs, lb_mode_str));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Set new PAUSE modes. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, PC_MAC_CONTROLs, &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, PORT_IDs, port));

    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        opcode = BCMLT_OPCODE_INSERT;
    } else {
        SHR_IF_ERR_EXIT(status);
        opcode = BCMLT_OPCODE_UPDATE;
    }

    /*
     * Clear the entry handle, as the LT has read-only fields
     * which should not be touched during update or insert
     * operation.
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(ent_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, PORT_IDs, port));

    if (sal_strcasecmp(lb_mode_str, "PC_LPBK_NONE") == 0) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl, PAUSE_RXs, 0));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl, PAUSE_TXs, 0));
    }else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl, PAUSE_RXs, 1));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl, PAUSE_TXs, 1));
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, opcode,
                            BCMLT_PRIORITY_NORMAL));

    bcmlt_entry_free(ent_hdl);

    /* Exit if loopback type is NONE. */
    if (!sal_strcasecmp(lb_mode_str, "PC_LPBK_NONE")) {
        SHR_EXIT();
    }

    /* Wait for the Port link to be UP. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_LINK_STATE", &ent_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(ent_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, PORT_IDs, port));

    do {
        status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL);
        /*
         * When linkscan is enabled on a port, linkscan creates
         * the LM_LINK_STATE LT entry, when it detects the link
         * transition. It may take some time for this to happen,
         * sleeping for 1 sec.
         */

        if (status == SHR_E_NOT_FOUND) {
            if (!timeout) {
                sal_usleep(1000000);
                timeout = true;
                continue;
            } else {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
        } else if (SHR_FAILURE(status)) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        /* Get the LINK_STATE for link status. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ent_hdl, "LINK_STATE", &link_status));

        /*
         * Since loopback is set, the link_status should be UP (1),
         * if the link is not up wait for 1 sec and check again, if
         * the link is still not up return failure.
         */
        if (!link_status) {
            sal_usleep(2000000);
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(ent_hdl, "LINK_STATE",
                                       &link_status));
            if (!link_status) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            } else {
                SHR_EXIT();
            }
        }
        break;
    } while (1);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_port_link_status_get(int unit, int port, uint64_t *link_status)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t ent_info;
    bool clean_lm_port_control = 0;
    const char *cur_scan_mode;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Get the link status from the LM_LINK_STATE LT.
     * 1. Add the port to the LM_PORT_CONTROL with linkscan mode
     *    as SOFTWARE if not already added.
     * 2. Perform an lookup operation on the LM_LINK_STATE LT
     *    to get the link status from the LINK_STATE field.
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_PORT_CONTROL", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));
    rv = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_get(ent_hdl, "LINKSCAN_MODE",
                                          &cur_scan_mode));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "LINKSCAN_MODE",
                                      "SOFTWARE"));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL));
        clean_lm_port_control = 1;
    } else if ((rv == SHR_E_NONE) &&
               (!sal_strcasecmp(cur_scan_mode, "NO_SCAN"))) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(ent_hdl);

    sal_usleep(1000);

    /* Step 2. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_LINK_STATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));
    if (SHR_FAILURE(ent_info.status)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(ent_hdl, "LINK_STATE", link_status));

    bcmlt_entry_free(ent_hdl);

exit:
    if (clean_lm_port_control) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "LM_PORT_CONTROL", &ent_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_clear(ent_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL));
    }
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
dispatcher_delete(int unit, const bcma_bcmpkt_dispatch_info_t *dispatcher,
                  void *cb_data)
{
    return bcma_bcmpkt_rx_dispatcher_destroy(unit, dispatcher->netif_id);
}

int
bcma_bcmpkt_test_port_lb_set(int unit, int port, bcma_test_lpbk_t lb_mode)
{
    uint64_t link = 0;
    const char *lb_mode_str;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (lb_mode == BCMA_TEST_LPBK_MAC) {
        lb_mode_str = "PC_LPBK_MAC";
    } else if (lb_mode == BCMA_TEST_LPBK_PHY) {
        lb_mode_str = "PC_LPBK_PHY";
    } else if (lb_mode == BCMA_TEST_LPBK_NONE) {
        lb_mode_str = "PC_LPBK_NONE";
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(lt_port_lpbk_mode_set(unit, port, lb_mode_str));
    SHR_IF_ERR_EXIT(lt_port_link_status_get(unit, port, &link));
    if (!link && (lb_mode != BCMA_TEST_LPBK_NONE)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Port%d :link status is down\n"),
                  port));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_chan_qmap_set(int unit, int chan_id, SHR_BITDCL *queue_bmp,
                               uint32_t num_queues)
{

    return bcma_bcmpkt_chan_qmap_set(unit, chan_id, queue_bmp, num_queues);
}

int
bcma_bcmpkt_test_dev_cleanup(int unit)
{
    int rv;
    int actual_rv = SHR_E_NONE;

    /* Destroy all packet watchers on the unit */
    bcma_bcmpktcmd_watcher_destroy(unit, -1);

    /* Destroy all Rx dispatchers. */
    rv = bcma_bcmpkt_rx_dispatcher_traverse(unit, dispatcher_delete, NULL);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Rx dispatcher cleanup failed (%d)\n"),
                   rv));
        actual_rv = rv;
    }

    /* Packet device cleanup. */
    rv = bcmpkt_dev_cleanup(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Packet device cleanup failed (%d)\n"),
                   rv));
        if (actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
    }

    bcma_bcmpkt_netif_defid_set(unit, 0);

    return actual_rv;
}

int
bcma_bcmpkt_test_dev_init(int unit)
{
    bcmpkt_dev_drv_types_t dev_drv_type;
    int max_frame_size = MAX(pkttest_max_frame_size, MAX_FRAME_SIZE_DEF);
    
    static bcmpkt_dev_init_t cfg = {
        .cgrp_bmp = 0x1,
        .cgrp_size = 4,
        .mac_addr = BCMA_BCMPKT_DEF_NETIF_MAC,
    };
    bcmpkt_dma_chan_t chan = {
        .dir = BCMPKT_DMA_CH_DIR_TX,
        .ring_size = 64,
        .max_frame_size = max_frame_size,
    };
    bcmpkt_netif_t netif = {
        .mac_addr = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
        .max_frame_size = max_frame_size,
        .flags = BCMPKT_NETIF_F_RCPU_ENCAP,
    };
    bcmpkt_filter_t filter = {
        .type = BCMPKT_FILTER_T_RX_PKT,
        .priority = 255,
        .dma_chan = 1,
        .dest_type = BCMPKT_DEST_T_NETIF,
        .match_flags = 0,
    };
    bcmpkt_socket_cfg_t socket_cfg = {
        .rx_poll_timeout = 1000,
        .rx_buf_size = 0,
        .tx_buf_size = 0
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(sal_sprintf(cfg.name, "bcm%d", unit));
    SHR_IF_ERR_EXIT(bcmpkt_dev_init(unit, &cfg));

    SHR_IF_ERR_EXIT(bcmpkt_dev_drv_type_get(unit, &dev_drv_type));
    /* Create per device buffer pool for UNET only. */
    if (dev_drv_type == BCMPKT_DEV_DRV_T_UNET) {
        bcmpkt_bpool_create(unit, max_frame_size, BCMPKT_BPOOL_BCOUNT_DEF);
    }

    /* Configure TX channel*/
    chan.id = 0;
    chan.dir = BCMPKT_DMA_CH_DIR_TX;
    SHR_IF_ERR_EXIT(bcmpkt_dma_chan_set(unit, &chan));

    /* Configure RX channel*/
    chan.id = 1;
    chan.dir = BCMPKT_DMA_CH_DIR_RX;
    SHR_IF_ERR_EXIT(bcmpkt_dma_chan_set(unit, &chan));

    SHR_IF_ERR_EXIT(bcmpkt_dev_enable(unit));

    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* Create netif. */
        SHR_IF_ERR_EXIT(bcmpkt_netif_create(unit, &netif));

        /* Setup SOCKET. */
        SHR_IF_ERR_EXIT(bcmpkt_socket_create(unit, netif.id, &socket_cfg));

        /* Create filter to forward all packet to the netif. */
        filter.dest_id = netif.id;
        SHR_IF_ERR_EXIT(bcmpkt_filter_create(unit, &filter));

        SHR_IF_ERR_EXIT(bcma_bcmpkt_netif_defid_set(unit, netif.id));

        cli_out("Create SOCKET on network interface %d successfully!\n", netif.id);
    } else {
        SHR_IF_ERR_EXIT(bcma_bcmpkt_netif_defid_set(unit, 1));
        cli_out("Create UNET successfully!\n");
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcma_bcmpkt_test_drv_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(unit));
exit:
    SHR_FUNC_EXIT();
}

static int
bcma_bcmpkt_test_drv_detach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_drv_set(unit, NULL));
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_dpath_create(int unit, int port, int redirect_port,
                              bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ifp_cfg, SHR_E_PARAM);

    if (!pkttest_drv_attached) {
        SHR_IF_ERR_EXIT
            (bcma_bcmpkt_test_drv_attach(unit));
        pkttest_drv_attached = TRUE;
    }

    SHR_NULL_CHECK(pkttest_drv, SHR_E_INIT);
    SHR_NULL_CHECK(pkttest_drv->dpath_create, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (pkttest_drv->dpath_create(unit, port, redirect_port, ifp_cfg));
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_dpath_destroy(int unit, int port,
                               bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ifp_cfg, SHR_E_PARAM);

    SHR_NULL_CHECK(pkttest_drv, SHR_E_INIT);
    SHR_NULL_CHECK(pkttest_drv->dpath_destroy, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (pkttest_drv->dpath_destroy(unit, port, ifp_cfg));

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_drv_detach(unit));
    pkttest_drv_attached = FALSE;

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Destroy data path failed\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_packet_generate(int unit, bcmpkt_packet_t **packet,
                                 uint32_t packet_len, uint32_t buf_size,
                                 int port)
{
    bcmpkt_packet_t *pkt = NULL;
    uint8_t *data = NULL;
    int buf_unit = unit;
    bcmdrd_dev_type_t dev_type;
    bcmpkt_dev_drv_types_t dev_drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_drv_type_get(unit, &dev_drv_type));
    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* KNET mode, use shared buff. */
        buf_unit = BCMPKT_BPOOL_SHARED_ID;
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(buf_unit, buf_size, BCMPKT_BUF_F_TX, &pkt));
    SHR_NULL_CHECK(pkt, SHR_E_FAIL);

    if (!bcmpkt_put(pkt->data_buf, packet_len)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    data = BCMPKT_PACKET_DATA(pkt);
    sal_memset(data, 0, packet_len);
    /* 00:00:00:00:00:11 */
    data[DMAC_ADDR_BYTE_OFF_5] = DA_BYTE_5;
    /* 00:00:00:00:00:22 */
    data[SMAC_ADDR_BYTE_OFF_5] = SA_BYTE_5;
    data[ETHER_TYPE_BYTE_OFF_0] = 0xFF;
    data[ETHER_TYPE_BYTE_OFF_1] = 0xFF;

    if (port > 0) {
        SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));
        SHR_IF_ERR_EXIT(bcmpkt_fwd_port_set(dev_type, port, pkt));
    }

    *packet = pkt;

exit:
    if (SHR_FUNC_ERR() && (pkt != NULL)) {
        bcmpkt_free(pkt->unit, pkt);
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_rx_parse_args(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                               bcma_bcmpkt_test_rx_param_t *rx_param, bool show)
{
    bcma_cli_parse_table_t pt;
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg= NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(rx_param, SHR_E_PARAM);

    /* Init defaults first time */
    if (!rx_param->init) {
        rx_param->init = TRUE;
        rx_param->port = 1;
        rx_param->len_start = 64;
        rx_param->len_end = 1536;
        rx_param->len_inc = 64;
        rx_param->time = 2;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Time", "int", &rx_param->time, NULL);
    bcma_cli_parse_table_add(&pt, "LengthStart", "int", &rx_param->len_start,
                             NULL);
    bcma_cli_parse_table_add(&pt, "LengthEnd", "int", &rx_param->len_end, NULL);
    bcma_cli_parse_table_add(&pt, "LengthInc", "int", &rx_param->len_inc, NULL);

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        bcma_cli_parse_table_done(&pt);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (show) {
        bcma_cli_parse_table_show(&pt, NULL);
    } else {
        SHR_ALLOC(ifp_cfg, sizeof(bcma_bcmpkt_test_fp_cfg_t),
                  "bcmaBcmpktTestRxIfpcfg");
        SHR_NULL_CHECK(ifp_cfg, SHR_E_MEMORY);
        sal_memset(ifp_cfg, 0, sizeof(bcma_bcmpkt_test_fp_cfg_t));
        rx_param->ifp_cfg = ifp_cfg;
    }
    bcma_cli_parse_table_done(&pt);

    /* reset to default value if inputs from CLI are invalid */
    if (rx_param->time < 1 || rx_param->time > 30) {
        rx_param->time = 2;
    }
    if (rx_param->len_start < ENET_MIN_PKT_SIZE) {
        rx_param->len_start = 64;
    }
    if (rx_param->len_start > MAX_JUMBO_FRAME_SIZE) {
        rx_param->len_start = MAX_JUMBO_FRAME_SIZE;
    }
    if (rx_param->len_end < ENET_MIN_PKT_SIZE) {
        rx_param->len_end = 1536;
    }
    if (rx_param->len_end > MAX_JUMBO_FRAME_SIZE) {
        rx_param->len_end = MAX_JUMBO_FRAME_SIZE;
    }
    if (rx_param->len_start > rx_param->len_end) {
        rx_param->len_end = rx_param->len_start;
    }
    if (rx_param->len_inc <= 0) {
        rx_param->len_inc = 64;
    }

    pkttest_max_frame_size = rx_param->len_end;

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_tx_parse_args(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                               bcma_bcmpkt_test_tx_param_t *tx_param, bool show)
{
    bcma_cli_parse_table_t pt;
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);

    /* Init defaults first time */
    if (!tx_param->init) {
        tx_param->init = TRUE;
        tx_param->port = 1;
        tx_param->len_start = 64;
        tx_param->len_end = 1536;
        tx_param->len_inc = 64;
        tx_param->send_count = 50000;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "LengthStart", "int", &tx_param->len_start,
                             NULL);
    bcma_cli_parse_table_add(&pt, "LengthEnd", "int", &tx_param->len_end, NULL);
    bcma_cli_parse_table_add(&pt, "LengthInc", "int", &tx_param->len_inc, NULL);
    bcma_cli_parse_table_add(&pt, "SendCount", "int", &tx_param->send_count,
                             NULL);

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        bcma_cli_parse_table_done(&pt);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (show) {
        bcma_cli_parse_table_show(&pt, NULL);
    } else {
        SHR_ALLOC(ifp_cfg, sizeof(bcma_bcmpkt_test_fp_cfg_t),
                  "bcmaBcmpktTestTxIfpcfg");
        SHR_NULL_CHECK(ifp_cfg, SHR_E_MEMORY);
        sal_memset(ifp_cfg, 0, sizeof(bcma_bcmpkt_test_fp_cfg_t));
        tx_param->ifp_cfg = ifp_cfg;
    }
    bcma_cli_parse_table_done(&pt);

    /* reset to default value if inputs from CLI are invalid */
    if (bcmdrd_feature_is_real_hw(unit)) {
        if (tx_param->send_count < 10000 || tx_param->send_count > 500000) {
            tx_param->send_count = 50000;
        }
    } else {
        /* Do less packets validation on simulation. */
        if (tx_param->send_count < 0 || tx_param->send_count > 10) {
            tx_param->send_count = 10;
        }
    }
    if (tx_param->len_start < ENET_MIN_PKT_SIZE) {
        tx_param->len_start = 64;
    }
    if (tx_param->len_start > MAX_JUMBO_FRAME_SIZE) {
        tx_param->len_start = MAX_JUMBO_FRAME_SIZE;
    }
    if (tx_param->len_end < ENET_MIN_PKT_SIZE) {
        tx_param->len_end = 1536;
    }
    if (tx_param->len_end > MAX_JUMBO_FRAME_SIZE) {
        tx_param->len_end = MAX_JUMBO_FRAME_SIZE;
    }
    if (tx_param->len_start > tx_param->len_end) {
        tx_param->len_end = tx_param->len_start;
    }
    if (tx_param->len_inc <= 0) {
        tx_param->len_inc = 64;
    }

    pkttest_max_frame_size = tx_param->len_end;

exit:
    SHR_FUNC_EXIT();
}

void
bcma_bcmpkt_test_rx_recycle(int unit, bcma_bcmpkt_test_rx_param_t *rx_param)
{
    if(rx_param == NULL) {
        return;
    }

    if (rx_param->packet != NULL) {
        bcmpkt_free(rx_param->packet->unit, rx_param->packet);
        rx_param->packet = NULL;
    }
    if (rx_param->ifp_cfg != NULL) {
        sal_free(rx_param->ifp_cfg);
        rx_param->ifp_cfg = NULL;
    }
    if (rx_param != NULL) {
        sal_free(rx_param);
        rx_param = NULL;
    }

    return;
}

void
bcma_bcmpkt_test_tx_recycle(int unit, bcma_bcmpkt_test_tx_param_t *tx_param)
{
    if(tx_param == NULL) {
        return;
    }

    if (tx_param->packet != NULL) {
        bcmpkt_free(tx_param->packet->unit, tx_param->packet);
        tx_param->packet = NULL;
    }
    if (tx_param->ifp_cfg != NULL) {
        sal_free(tx_param->ifp_cfg);
        tx_param->ifp_cfg = NULL;
    }
    if (tx_param != NULL) {
        sal_free(tx_param);
        tx_param= NULL;
    }

    return;
}

int
bcma_bcmpkt_test_rx_run(int unit, bcma_bcmpkt_test_rx_param_t *rx_param)
{
    uint32_t pkt_len = 0;
    int time_diff = 0;
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg = NULL;
    cpu_stats_t cpu_start;
    cpu_stats_t cpu_end;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rx_param, SHR_E_PARAM);
    SHR_NULL_CHECK(rx_param->ifp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(rx_param->packet, SHR_E_PARAM);

    /* Register Rx callback to counter received packets*/
    SHR_IF_ERR_EXIT
        (bcmpkt_rx_register(unit, netif_id, 0, packet_measure,
                            (void*)rx_param));

    ifp_cfg = rx_param->ifp_cfg;

    cli_out("\n"
            " Packet |        Rate         Total   |       |        |                     %%CPU                    \n"
            "  Len   |  KPkt/s |   MB/s  | packets | Time  |  Intr  |Idle |User |Kern |Nice |IOwait|IRQ  |softIRQ  \n"
            " -------+---------+---------+---------+-------+--------+-----+-----+-----+-----+------+-----+--------+\n");

    for (pkt_len = rx_param->len_start; pkt_len <= rx_param->len_end;) {
        SHR_IF_ERR_EXIT
            (lt_ifp_policy_update(unit, ifp_cfg->entry_id,
                                  ifp_cfg->copy_policy_id));
        /* Get the CPU usage info at the beginning of test. */
        SHR_IF_ERR_EXIT(cpu_stats_get(&cpu_start));

        SHR_IF_ERR_EXIT(packet_send(unit, netif_id, rx_param->packet, 1));

        rx_param->time_end = rx_param->time_start = 0;
        rx_param->pkt_received = 0;

        sal_usleep(100000);
        rx_param->count_enable = TRUE;
        sal_sleep(rx_param->time);
        rx_param->count_enable = FALSE;
        sal_usleep(100000);

        SHR_IF_ERR_EXIT
            (lt_ifp_policy_update(unit, ifp_cfg->entry_id,
                                  ifp_cfg->drop_policy_id));
        /* Get the CPU usage info at the end of test. */
        SHR_IF_ERR_EXIT(cpu_stats_get(&cpu_end));

        time_diff = (int)(rx_param->time_end - rx_param->time_start);
        test_stats_dump(time_diff, rx_param->pkt_received, pkt_len,
                        &cpu_end, &cpu_start);

        if ((pkt_len += rx_param->len_inc) > rx_param->len_end) {
            break;
        }

        if (!bcmpkt_put(rx_param->packet->data_buf, rx_param->len_inc)) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    /* Unregister Rx callback*/
    bcmpkt_rx_unregister(unit, netif_id, packet_measure, rx_param);
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_test_tx_run(int unit, bcma_bcmpkt_test_tx_param_t *tx_param)
{
    uint32_t pkt_len = 0;
    int time_diff = 0;
    sal_usecs_t time_start = 0;
    cpu_stats_t cpu_start;
    cpu_stats_t cpu_end;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);
    SHR_NULL_CHECK(tx_param->ifp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(tx_param->packet, SHR_E_PARAM);

    cli_out("\n"
            " Packet |        Rate         Total   |       |        |                     %%CPU                    \n"
            "  Len   |  KPkt/s |   MB/s  | packets | Time  |  Intr  |Idle |User |Kern |Nice |IOwait|IRQ  |softIRQ  \n"
            " -------+---------+---------+---------+-------+--------+-----+-----+-----+-----+------+-----+--------+\n");

    SHR_IF_ERR_EXIT
        (lt_ifp_policy_update(unit, tx_param->ifp_cfg->entry_id,
                              tx_param->ifp_cfg->drop_policy_id));
    for (pkt_len = tx_param->len_start; pkt_len <= tx_param->len_end;) {
        time_start = sal_time_usecs();
        SHR_IF_ERR_EXIT(cpu_stats_get(&cpu_start));
        SHR_IF_ERR_EXIT
            (packet_send(unit, netif_id, tx_param->packet,
                         tx_param->send_count));
        SHR_IF_ERR_EXIT(cpu_stats_get(&cpu_end));

        time_diff = (int)(sal_time_usecs() - time_start);

        sal_usleep(50000);
        test_stats_dump(time_diff, tx_param->send_count, pkt_len,
                        &cpu_end, &cpu_start);

        if ((pkt_len += tx_param->len_inc) > tx_param->len_end) {
            break;
        }

        if (!bcmpkt_put(tx_param->packet->data_buf, tx_param->len_inc)) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

