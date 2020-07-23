/*! \file bcm56996_a0_testutil_traffic.c
 *
 * Chip-specific TRAFFIC data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/chip/bcm56996_a0_testutil_traffic.h>
#include <bcmlt/bcmlt.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define BCM56996_A0_TRAFFIC_MAX_PIPES               16
#define BCM56996_A0_TRAFFIC_PKT_HDR_LEN_ETH         20
#define BCM56996_A0_TRAFFIC_PKT_HDR_LEN_GSH         12
#define BCM56996_A0_TRAFFIC_CELL_SOP                206
#define BCM56996_A0_TRAFFIC_CELL_MOP                254

#define BCM56996_A0_TRAFFIC_MAX_FRAME_SIZE          9416
#define BCM56996_A0_TRAFFIC_MIN_FRAME_SIZE          64
#define BCM56996_A0_TRAFFIC_WORSTCASE_PKT_SIZE      64

#define BCM56996_A0_TRAFFIC_MAX_STRING_SIZE         32
#define BCM56996_A0_10G_MGMT_PORT_SPEED             10000
#define BCM56996_A0_40G_MGMT_PORT_SPEED             40000

/* Clock frequency name to value map */
static shr_enum_map_t bcm56996_a0_clk_freq_map[]={
    {"CLK_1325MHZ", 1325},
    {"CLK_1250MHZ", 1250},
    {"CLK_1175MHZ", 1175},
    {"CLK_1100MHZ", 1100},
    {"CLK_1025MHZ", 1025},
    {"CLK_950MHZ",   950},
    {NULL, 0}
};

/* Rate calculation struct */
typedef struct bcm56996_a0_rate_param_s {

    /* Active port bitmap */
    bcmdrd_pbmp_t active_port_bmp;

    /* Packet size */
    uint32_t pkt_size;

    /* Active pipe bitmap */
    uint32_t active_pipe_bmp;

    /* Number of cells per packet */
    uint32_t cells_per_pkt;

    /* PP clock frequency */
    uint32_t pp_clk_freq;

    /* Core clock frequency */
    uint32_t core_clk_freq;

    /* Auxiliary bandwidth in mega packet per second */
    uint32_t pipe_ancl_mpps[BCM56996_A0_TRAFFIC_MAX_PIPES];

    /* Auxiliary bandwidth in mega cells per second */
    uint32_t pipe_ancl_mcps[BCM56996_A0_TRAFFIC_MAX_PIPES];

    /* Mgmt port speed in Mbps */
    uint32_t mgmt_port_speed[BCM56996_A0_TRAFFIC_MAX_PIPES];

    /* Port speed in Mbps */
    uint32_t port_speed[BCMDRD_CONFIG_MAX_PORTS];

    /* Port packet header length in Byte */
    uint32_t port_pkt_hdr_len[BCMDRD_CONFIG_MAX_PORTS];

    /* Expected port rate in Mbps */
    uint64_t exp_port_rate[BCMDRD_CONFIG_MAX_PORTS];

} bcm56996_a0_rate_param_t;

static int
bcm56996_a0_rate_init_freq(int unit, bcm56996_a0_rate_param_t *rate_param)
{
    char pp_clk_freq_str[BCM56996_A0_TRAFFIC_MAX_STRING_SIZE];
    char core_clk_freq_str[BCM56996_A0_TRAFFIC_MAX_STRING_SIZE];
    uint32_t map_len, pp_clk_freq, core_clk_freq;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rate_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_core_clk_freq_get
            (unit, BCM56996_A0_TRAFFIC_MAX_STRING_SIZE, core_clk_freq_str));
    SHR_IF_ERR_EXIT
        (bcma_testutil_pp_clk_freq_get
            (unit, BCM56996_A0_TRAFFIC_MAX_STRING_SIZE, pp_clk_freq_str));

    map_len = COUNTOF(bcm56996_a0_clk_freq_map);
    core_clk_freq = bcma_testutil_name2id(bcm56996_a0_clk_freq_map, map_len,
                                          core_clk_freq_str);
    pp_clk_freq   = bcma_testutil_name2id(bcm56996_a0_clk_freq_map, map_len,
                                          pp_clk_freq_str);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rate_init: CORE_CLK_FREQ = %s | %0d\n"),
                 core_clk_freq_str, core_clk_freq));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rate_init: PP_CLK_FREQ   = %s | %0d\n"),
                 pp_clk_freq_str, pp_clk_freq));

    rate_param->core_clk_freq = core_clk_freq;
    rate_param->pp_clk_freq   = pp_clk_freq;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_rate_init(int unit, bcmdrd_pbmp_t pbmp, int pkt_size,
                      bcm56996_a0_rate_param_t *rate_param)
{
    uint32_t port, cells_per_pkt, ancl_mpps, ancl_mcps, active_pipe_bmp;
    uint32_t sop_size, mop_size;
    int pkt_size_tmp, pipe;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rate_param, SHR_E_PARAM);

    /* Init active port bitmap */
    BCMDRD_PBMP_ASSIGN(rate_param->active_port_bmp, pbmp);

    /* Init packet size */
    rate_param->pkt_size = pkt_size;

    /* Init port speeds */
    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_speed_list_get
            (unit, BCMDRD_CONFIG_MAX_PORTS, rate_param->port_speed));

    /* Init clock frequency */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_rate_init_freq(unit, rate_param));

    /* Init active_pipe_bmp */
    active_pipe_bmp = 0;
    BCMDRD_PBMP_ITER(rate_param->active_port_bmp, port) {
        pipe = bcmdrd_dev_logic_port_pipe(unit, port);
        if (pipe >= 0 && pipe < BCM56996_A0_TRAFFIC_MAX_PIPES) {
            active_pipe_bmp |= (1 << pipe);
        }
    }
    rate_param->active_pipe_bmp = active_pipe_bmp;

    /* Init pkt_hdr_len */
    BCMDRD_PBMP_ITER(rate_param->active_port_bmp, port) {
        rate_param->port_pkt_hdr_len[port] =
            BCM56996_A0_TRAFFIC_PKT_HDR_LEN_ETH;
    }

    /* Init cells_per_pkt */
    cells_per_pkt = 1;
    sop_size = BCM56996_A0_TRAFFIC_CELL_SOP;
    mop_size = BCM56996_A0_TRAFFIC_CELL_MOP;
    pkt_size_tmp = pkt_size - sop_size;
    while (pkt_size_tmp > 0) {
        cells_per_pkt++;
        pkt_size_tmp -= mop_size;
    }
    rate_param->cells_per_pkt = cells_per_pkt;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rate_init: pkt_size = %0d, sop_size = %0d,"
                            "mop_size = %0d, cells_per_pkt = %0d\n"),
                 pkt_size, sop_size, mop_size, cells_per_pkt));

    /* Init mgmt port speed */
    SHR_IF_ERR_EXIT
        (bcma_testutil_mgmt_port_speed_list_get
            (unit, BCM56996_A0_TRAFFIC_MAX_PIPES, rate_param->mgmt_port_speed));

    /* Init auxiliary bandwidth */
    for (pipe = 0; pipe < BCM56996_A0_TRAFFIC_MAX_PIPES; pipe++) {
        if (((1 << pipe) & active_pipe_bmp) == 0) {
            continue;
        }

        /* For the pipes contain mgmt port */
        if (rate_param->mgmt_port_speed[pipe] != 0) {
            ancl_mpps = rate_param->mgmt_port_speed[pipe] /
                        (pkt_size + BCM56996_A0_TRAFFIC_PKT_HDR_LEN_ETH) / 8;
            ancl_mcps = ancl_mpps / cells_per_pkt;
        } else {
            ancl_mpps = 0;
            ancl_mcps = 0;
        }
        rate_param->pipe_ancl_mpps[pipe] = ancl_mpps;
        rate_param->pipe_ancl_mcps[pipe] = ancl_mcps;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rate_init: pipe[%2d] "
                                "ancl_mpps = %0d, ancl_mcps = %0d\n"),
                     pipe, ancl_mpps, ancl_mcps));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_rate_set(int unit, bcm56996_a0_rate_param_t *rate_param)
{
    uint32_t pipe, port;
    uint32_t active_pipe_bmp;
    bcmdrd_pbmp_t active_port_bmp;
    uint32_t pipe_io_bw[BCM56996_A0_TRAFFIC_MAX_PIPES];
    uint32_t pkt_size, cells_per_pkt, pp_clk_freq, core_clk_freq;
    uint32_t exp_rate_by_mpps, exp_rate_by_mcps, exp_rate_by_ratio;
    uint32_t pipe_pp_bw_mbps, pipe_core_bw_mbps, pipe_avail_bw_mbps;

    uint64_t pipe_req_mpps_x1000[BCM56996_A0_TRAFFIC_MAX_PIPES];
    uint64_t pipe_req_mcps_x1000[BCM56996_A0_TRAFFIC_MAX_PIPES];
    uint64_t pipe_avail_mpps_x1000[BCM56996_A0_TRAFFIC_MAX_PIPES];
    uint64_t pipe_avail_mcps_x1000[BCM56996_A0_TRAFFIC_MAX_PIPES];
    uint32_t *port_speed = NULL;
    uint32_t *port_pkt_hdr_len = NULL;
    uint32_t *pipe_ancl_mpps = NULL;
    uint32_t *pipe_ancl_mcps = NULL;
    uint64_t *port_req_mpps_x1000 = NULL;
    uint64_t *port_req_mcps_x1000 = NULL;
    uint64_t *port_avail_mpps_x1000 = NULL;
    uint64_t *port_avail_mcps_x1000 = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rate_param, SHR_E_PARAM);

    SHR_ALLOC(port_req_mpps_x1000, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t),
              "bcm56996TestUtilTrafficReqMpps");
    SHR_NULL_CHECK(port_req_mpps_x1000, SHR_E_MEMORY);
    sal_memset(port_req_mpps_x1000, 0,
               BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t));

    SHR_ALLOC(port_req_mcps_x1000, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t),
              "bcm56996TestUtilTrafficReqMcps");
    SHR_NULL_CHECK(port_req_mcps_x1000, SHR_E_MEMORY);
    sal_memset(port_req_mcps_x1000, 0,
               BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t));

    SHR_ALLOC(port_avail_mpps_x1000, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t),
              "bcm56996TestUtilTrafficAvailMpps");
    SHR_NULL_CHECK(port_avail_mpps_x1000, SHR_E_MEMORY);
    sal_memset(port_avail_mpps_x1000, 0,
               BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t));

    SHR_ALLOC(port_avail_mcps_x1000, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t),
              "bcm56996TestUtilTrafficAvailMcps");
    SHR_NULL_CHECK(port_avail_mcps_x1000, SHR_E_MEMORY);
    sal_memset(port_avail_mcps_x1000, 0,
               BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t));

    sal_memset(pipe_io_bw, 0,
               BCM56996_A0_TRAFFIC_MAX_PIPES * sizeof(uint32_t));
    sal_memset(pipe_req_mpps_x1000, 0,
               BCM56996_A0_TRAFFIC_MAX_PIPES * sizeof(uint64_t));
    sal_memset(pipe_req_mcps_x1000, 0,
               BCM56996_A0_TRAFFIC_MAX_PIPES * sizeof(uint64_t));
    sal_memset(pipe_avail_mpps_x1000, 0,
               BCM56996_A0_TRAFFIC_MAX_PIPES * sizeof(uint64_t));
    sal_memset(pipe_avail_mcps_x1000, 0,
               BCM56996_A0_TRAFFIC_MAX_PIPES * sizeof(uint64_t));

    pp_clk_freq      = rate_param->pp_clk_freq;
    core_clk_freq    = rate_param->core_clk_freq;
    pkt_size         = rate_param->pkt_size;
    cells_per_pkt    = rate_param->cells_per_pkt;
    port_speed       = rate_param->port_speed;
    port_pkt_hdr_len = rate_param->port_pkt_hdr_len;
    pipe_ancl_mpps   = rate_param->pipe_ancl_mpps;
    pipe_ancl_mcps   = rate_param->pipe_ancl_mcps;
    active_pipe_bmp  = rate_param->active_pipe_bmp;
    BCMDRD_PBMP_ASSIGN(active_port_bmp, rate_param->active_port_bmp);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "pkt_size        = %0d\n"), pkt_size));
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "cells_per_pkt   = %0d\n"), cells_per_pkt));
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "core_clk_freq   = %0d\n"), core_clk_freq));
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "pp_clk_freq     = %0d\n"), pp_clk_freq));

    /* Set pipe_io_bw[] */
    BCMDRD_PBMP_ITER(active_port_bmp, port) {
        pipe = bcmdrd_dev_logic_port_pipe(unit, port);
        if (pipe < BCM56996_A0_TRAFFIC_MAX_PIPES) {
            pipe_io_bw[pipe] += port_speed[port];
        }
    }
    for (pipe = 0; pipe < BCM56996_A0_TRAFFIC_MAX_PIPES; pipe++) {
        if (((1 << pipe) & active_pipe_bmp)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "pipe[%0d]_io_bw = %0d\n"),
                      pipe, pipe_io_bw[pipe]/1000));
        }
    }

    /* Set pipe_avail_bw */
    pipe_pp_bw_mbps   = (pkt_size + BCM56996_A0_TRAFFIC_PKT_HDR_LEN_ETH) * 8 *
                        pp_clk_freq / 2;
    pipe_core_bw_mbps = (pkt_size + BCM56996_A0_TRAFFIC_PKT_HDR_LEN_ETH) * 8 *
                        core_clk_freq / cells_per_pkt;
    pipe_avail_bw_mbps= (pipe_core_bw_mbps < pipe_pp_bw_mbps) ?
                        pipe_core_bw_mbps : pipe_pp_bw_mbps;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "pipe_avail_bw = %0dG (pp_bw = %0d, core_bw = %0d)\n"),
              pipe_avail_bw_mbps/1000, pipe_pp_bw_mbps/1000,
              pipe_core_bw_mbps/1000));

    /* Set port_req_mpps[] and port_req_mcps[] */
    BCMDRD_PBMP_ITER(active_port_bmp, port) {
        port_req_mpps_x1000[port] = (uint64_t) 1000 * port_speed[port] /
                                    (pkt_size + port_pkt_hdr_len[port]) / 8;
        port_req_mcps_x1000[port] = port_req_mpps_x1000[port] *
                                    cells_per_pkt;

        pipe = bcmdrd_dev_logic_port_pipe(unit, port);
        if (pipe < BCM56996_A0_TRAFFIC_MAX_PIPES) {
            pipe_req_mpps_x1000[pipe] += port_req_mpps_x1000[port];
            pipe_req_mcps_x1000[pipe] += port_req_mcps_x1000[port];
        }
    }

    /* Set pipe_avail_mpps[] and pipe_avail_mcps[] */
    for (pipe = 0; pipe < BCM56996_A0_TRAFFIC_MAX_PIPES; pipe++) {
        if (((1 << pipe) & active_pipe_bmp) == 0) {
            continue;
        }
        pipe_avail_mpps_x1000[pipe] = (uint64_t) 1000 * (pp_clk_freq / 2 -
                                      pipe_ancl_mpps[pipe]);
        pipe_avail_mcps_x1000[pipe] = (uint64_t) 1000 * (core_clk_freq -
                                      pipe_ancl_mcps[pipe]);
    }

    /* Set port_avail_mpps and port_avail_mcps */
    BCMDRD_PBMP_ITER(active_port_bmp, port) {
        pipe = bcmdrd_dev_logic_port_pipe(unit, port);
        if (pipe < BCM56996_A0_TRAFFIC_MAX_PIPES) {
            if (pipe_avail_mpps_x1000[pipe] > pipe_req_mpps_x1000[pipe]) {
                port_avail_mpps_x1000[port] = port_req_mpps_x1000[port];
            } else {
                port_avail_mpps_x1000[port] = port_req_mpps_x1000[port] *
                    pipe_avail_mpps_x1000[pipe] / pipe_req_mpps_x1000[pipe];
            }

            if (pipe_avail_mcps_x1000[pipe] > pipe_req_mcps_x1000[pipe]) {
                port_avail_mcps_x1000[port] = port_req_mcps_x1000[port];
            } else {
                port_avail_mcps_x1000[port] = port_req_mcps_x1000[port] *
                    pipe_avail_mcps_x1000[pipe] / pipe_req_mcps_x1000[pipe];
            }

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "pipe[%0d]: port=%3d, "
                                 "req_mpps = %0d, avail_mpps = %0d, "
                                 "req_mcps = %0d, avail_mcps = %0d\n"),
                      pipe, port,
                      (uint32_t) port_req_mpps_x1000[port],
                      (uint32_t) port_avail_mpps_x1000[port],
                      (uint32_t) port_req_mcps_x1000[port],
                      (uint32_t) port_avail_mcps_x1000[port]));
        }
    }

    /* Calculate expected port rate */
    BCMDRD_PBMP_ITER(active_port_bmp, port) {
        uint32_t pipe_ovs_ratio_x1000;

        pipe = bcmdrd_dev_logic_port_pipe(unit, port);
        if (pipe < BCM56996_A0_TRAFFIC_MAX_PIPES) {
            pipe_ovs_ratio_x1000 = pipe_io_bw[pipe] < pipe_avail_bw_mbps ?
                                   1000 :
                                   (pipe_io_bw[pipe] * 1000) /
                                   pipe_avail_bw_mbps;

            exp_rate_by_mpps = port_avail_mpps_x1000[port] *
                               (pkt_size + port_pkt_hdr_len[port]) * 8 / 1000;
            exp_rate_by_mcps = port_avail_mcps_x1000[port] *
                               (pkt_size + port_pkt_hdr_len[port]) * 8 / 1000 /
                               cells_per_pkt;
            exp_rate_by_ratio= (port_speed[port] * 1000) /
                               pipe_ovs_ratio_x1000;

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "pipe[%0d]: port = %3d, speed = %3dMbps, "
                                 "exp_rate_by_mpps = %0dMbps, "
                                 "exp_rate_by_mcps = %0dMbps, "
                                 "exp_rate_by_ratio = %0dMbps\n"),
                      pipe, port, port_speed[port],
                      (uint32_t) exp_rate_by_mpps,
                      (uint32_t) exp_rate_by_mcps,
                      (uint32_t) exp_rate_by_ratio));

            if (exp_rate_by_mpps < exp_rate_by_mcps) {
                rate_param->exp_port_rate[port] = exp_rate_by_mpps;
            } else {
                rate_param->exp_port_rate[port] = exp_rate_by_mcps;
            }
        }
    }

    BCMDRD_PBMP_ITER(active_port_bmp, port) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "expected port rate: port = %3d, speed = %3dG, "
                             "exp_port_rate = %0dG\n"),
                  port, port_speed[port]/1000,
                  (uint32_t) (rate_param->exp_port_rate[port]/1000)));
    }

exit:
    SHR_FREE(port_req_mpps_x1000);
    SHR_FREE(port_req_mcps_x1000);
    SHR_FREE(port_avail_mpps_x1000);
    SHR_FREE(port_avail_mcps_x1000);
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_traffic_get_expeceted_rate(int unit, bcmdrd_pbmp_t pbmp,
                                       bcmdrd_pbmp_t pbmp_oversub,
                                       int pkt_size, int array_size,
                                       uint64_t *ret_exp_rate)
{
    bcm56996_a0_rate_param_t *rate_param = NULL;
    int port;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_exp_rate, SHR_E_PARAM);

    SHR_ALLOC(rate_param, sizeof(bcm56996_a0_rate_param_t),
              "bcm56996TestUtilTrafficRateParam");
    SHR_NULL_CHECK(rate_param, SHR_E_MEMORY);
    sal_memset(rate_param, 0, sizeof(bcm56996_a0_rate_param_t));

    /* Init rate parameters */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_rate_init(unit, pbmp, pkt_size, rate_param));

    /* Set expected port rate */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_rate_set(unit, rate_param));

    /* Set return values */
    BCMDRD_PBMP_ITER(pbmp, port) {
        if (port >= array_size) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        ret_exp_rate[port] = rate_param->exp_port_rate[port] * 1000000;
    }
exit:
    SHR_FREE(rate_param);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_traffic_get_cellnums(int unit, uint32_t pkt_len,
                                 uint32_t *ret_cellnums)
{
    uint32_t i = BCM56996_A0_TRAFFIC_CELL_SOP;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_cellnums, SHR_E_PARAM);

    *ret_cellnums = 1;

    while (i < pkt_len) {
        (*ret_cellnums)++;
        i += BCM56996_A0_TRAFFIC_CELL_MOP;
    }

exit:
    SHR_FUNC_EXIT();
}

#define BCM56996_A0_TARGET_CELL_COUNT (300)

int
bcm56996_a0_traffic_get_fixed_packet_list(int unit, uint32_t pkt_len,
                                          uint32_t *ret_pkt_cnt)
{
    uint32_t cellnums;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_pkt_cnt, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm56996_a0_traffic_get_cellnums(unit, pkt_len, &cellnums));

    *ret_pkt_cnt = BCM56996_A0_TARGET_CELL_COUNT / cellnums;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_traffic_get_random_packet_list(int unit, uint32_t cellnums_limit,
                                           uint32_t array_size,
                                           uint32_t *ret_pkt_len,
                                           uint32_t *ret_pkt_cnt)
{
    uint32_t total_cellnums = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_pkt_len, SHR_E_PARAM);
    SHR_NULL_CHECK(ret_pkt_cnt, SHR_E_PARAM);

    *ret_pkt_cnt = 0;

    while (1) {
        uint32_t cellnums;
        int pkt_len;

        do {
            /* coverity[dont_call : FALSE] */
            pkt_len = (sal_rand() % (BCM56996_A0_TRAFFIC_MAX_FRAME_SIZE -
                      BCM56996_A0_TRAFFIC_MIN_FRAME_SIZE + 1))
                      + BCM56996_A0_TRAFFIC_MIN_FRAME_SIZE;
            SHR_IF_ERR_EXIT
                (bcm56996_a0_traffic_get_cellnums(unit, pkt_len, &cellnums));
        } while (cellnums > cellnums_limit);

        if (total_cellnums + cellnums > BCM56996_A0_TARGET_CELL_COUNT) {
            break;
        }

        if (*ret_pkt_cnt >= array_size) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        ret_pkt_len[*ret_pkt_cnt] = pkt_len;
        total_cellnums += cellnums;
        (*ret_pkt_cnt)++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_traffic_get_worstcase_pktlen(int unit, int port,
                                         uint32_t *ret_pkt_len)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_pkt_len, SHR_E_PARAM);

    *ret_pkt_len = BCM56996_A0_TRAFFIC_WORSTCASE_PKT_SIZE;

exit:
    SHR_FUNC_EXIT();
}


