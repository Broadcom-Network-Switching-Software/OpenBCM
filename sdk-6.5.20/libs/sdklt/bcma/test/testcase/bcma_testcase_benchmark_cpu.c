/*! \file bcma_testcase_benchmark_cpu.c
 *
 *  Test case for CPU benchmark.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <sal/sal_assert.h>
#include <sal/sal_sem.h>
#include <sal/sal_mutex.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_time.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_symbol_types.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/util/bcma_testutil_traffic.h>
#include <bcma/test/testcase/bcma_testcase_benchmark_cpu.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define BENCHMARK_CPU_REG_TEST_NUM        (2)
#define BENCHMARK_CPU_MEM_TEST_NUM        (3)

#define BENCHMARK_CPU_TF_SYSMEM           (1 << 0)
#define BENCHMARK_CPU_TF_LOCK             (1 << 1)
#define BENCHMARK_CPU_TF_REG              (1 << 2)
#define BENCHMARK_CPU_TF_MEM              (1 << 3)

#define BENCHMARK_CPU_DEFAULT_TRIALNUM    (5)
#define BENCHMARK_CPU_DEFAULT_ITERATION   (1000)
#define BENCHMARK_CPU_DEFAULT_MEMSIZE     (1)
#define BENCHMARK_CPU_DEFAULT_DMASIZE     (2)
#define BENCHMARK_CPU_DEFAULT_FLAGS       (BENCHMARK_CPU_TF_SYSMEM | \
                                           BENCHMARK_CPU_TF_LOCK | \
                                           BENCHMARK_CPU_TF_REG | \
                                           BENCHMARK_CPU_TF_MEM)
#define BENCHMARK_CPU_DEFAULT_TRAFFIC     (FALSE)
#define BENCHMARK_CPU_DEFAULT_ENTRYNUM    (0) /* 0 means test all entries */

#define BENCHMARK_TRAFFIC_VLAN_ID_BASE               (100)
#define BENCHMARK_TRAFFIC_VLAN_PROFILE_ID_BASE       (0)
#define BENCHMARK_TRAFFIC_L2_MC_GROUP_ID_BASE        (0)
#define BENCHMARK_TRAFFIC_TX_CHAN                    (0)
#define BENCHMARK_TRAFFIC_RX_CHAN                    (1)
#define BENCHMARK_TRAFFIC_PKT_MAX_FRAME_SIZE         (1536)
#define BENCHMARK_TRAFFIC_PKT_NUM                    (50)
#define BENCHMARK_TRAFFIC_PKT_PATTERN                (0xa5a4a3a2)
#define BENCHMARK_TRAFFIC_PKT_ETHERTYPE              (0xffff)
#define BENCHMARK_TRAFFIC_PORT_CPU                   (0)
#define BENCHMARK_TRAFFIC_PORT_0                     (1)
#define BENCHMARK_TRAFFIC_PORT_1                     (2)
#define BENCHMARK_TRAFFIC_POLLING_INTERVAL           (2)

static const shr_mac_t default_src_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const shr_mac_t default_dst_mac_u = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};
static const shr_mac_t default_dst_mac_m = {0x01, 0x77, 0x88, 0x99, 0xaa, 0xbb};

#define BENCHMARK_CPU_PARAM_HELP \
    "  TrialNum   - Testing trail number\n"\
    "  ITERation  - Testing iteration in each trial\n"\
    "  TRAffic    - Whether to introduce heavy traffic during benchmark\n"\
    "  MemSize    - Size of system memory (w/o DMA) testing. (in MB)\n"\
    "  DmaSize    - Size of system memory (w DMA) testing. (in MB)\n"\
    "  EntryNum   - Number of access entries in DMA benchmark\n"\
    "  REG1       - Testing register name.\n"\
    "  REG2         (Generally, one for SOC and another one for PCI reg)\n"\
    "  MEM1       - Testing memory name.\n"\
    "  MEM2         (Generally, we test three types: Index / Hash / TCAM)\n"\
    "  MEM3         \n"\
    "  ItemSelect - Select a single item to test.\n"\
    "               If ItemSelect is not specified, all test items will be\n"\
    "               run. The supported items are:\n"\
    "               sysmem: System memory.\n"\
    "               lock: System lock.\n"\
    "               reg: Device register.\n"\
    "               mem: Device memory.\n"\
    "\nExample:\n"\
    "    Run benchmark testing\n"\
    "      tr 21\n"\
    "    Run benchmark testing with 500 trials, 1000 iterations in each trial\n"\
    "      tr 21 tn=500 iter=1000\n"\
    "    Run benchmark testing with heavy traffic\n"\
    "      tr 21 tra=true\n"\
    "    Run benchmark testing with 2MB (w/o DMA) and 4MB (w/ DMA) in system memory test\n"\
    "      tr 21 ms=2 ds=4\n"\
    "    Run benchmark testing with the specific register or memory\n"\
    "      tr 21 reg1=VLAN_XLATE_CONTROL_0r reg2=VP_CAM_CONTROLr mem1=UDF_TCAMm\n"\
    "    Run benchmark testing for device memory with only 16 entries in DMA\n"\
    "      tr 21 is=mem en=16\n"\
    "    Run benchmark testing for lock performance only\n"\
    "      tr 21 is=lock iter=10000\n"\

/* Parameter */
typedef struct benchmark_cpu_param_s {
    /* Trial number */
    uint32_t trial_num;

    /* Repeat iterations in each trial */
    uint32_t iteration;

    /* Test flags */
    uint32_t flags;

    /* Register testing list */
    bcmdrd_sid_t reglist[BENCHMARK_CPU_REG_TEST_NUM];

    /* SOC Memory testing list */
    bcmdrd_sid_t memlist[BENCHMARK_CPU_MEM_TEST_NUM];

    /* System memory testing size (in MB) */
    uint32_t memsize;

    /* System memory (DMA) testing size (in MB) */
    uint32_t dmasize;

    /* The number of access entries in DMA benchmark */
    uint32_t entrynum;

    /* introduce traffic or not */
    int run_traffic;

    /* the array of time stamp duing benchmark */
    int *time_diff;
} benchmark_cpu_param_t;

static bcma_cli_parse_enum_t benchmark_cpu_test_items[] = {
    { "Sysmem", BENCHMARK_CPU_TF_SYSMEM },
    { "Lock", BENCHMARK_CPU_TF_LOCK },
    { "Reg", BENCHMARK_CPU_TF_REG },
    { "Mem", BENCHMARK_CPU_TF_MEM },
    { NULL, 0 }
};

static int
benchmark_cpu_select(int unit)
{
    return true;
}

static int
benchmark_cpu_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                          benchmark_cpu_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    char *parse_reg_str[BENCHMARK_CPU_REG_TEST_NUM];
    bcmdrd_sid_t *param_reg_sid[BENCHMARK_CPU_REG_TEST_NUM];
    char *parse_mem_str[BENCHMARK_CPU_MEM_TEST_NUM];
    bcmdrd_sid_t *param_mem_sid[BENCHMARK_CPU_MEM_TEST_NUM];
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    for (i = 0; i < BENCHMARK_CPU_REG_TEST_NUM; i++) {
        parse_reg_str[i] = NULL;
        param_reg_sid[i] = &(param->reglist[i]);
    }
    for (i = 0; i < BENCHMARK_CPU_MEM_TEST_NUM; i++) {
        parse_mem_str[i] = NULL;
        param_mem_sid[i] = &(param->memlist[i]);
    }

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    for (i = 0; i < BENCHMARK_CPU_REG_TEST_NUM; i++) {
        char parse_name[5];

        sal_memcpy(parse_name, "REG", 3);
        if ((i + 1) >= 10) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        parse_name[3] = (i + 1) + '0';
        parse_name[4] = '\0';

        SHR_IF_ERR_EXIT
            (bcma_cli_parse_table_add(&pt, parse_name, "str",
                                      &parse_reg_str[i], NULL));
    }

    for (i = 0; i < BENCHMARK_CPU_MEM_TEST_NUM; i++) {
        char parse_name[5];

        sal_memcpy(parse_name, "MEM", 3);
        if ((i + 1) >= 10) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        parse_name[3] = (i + 1) + '0';
        parse_name[4] = '\0';

        SHR_IF_ERR_EXIT
            (bcma_cli_parse_table_add(&pt, parse_name, "str",
                                      &parse_mem_str[i], NULL));
    }

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TrialNum", "int",
                                  &param->trial_num, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "ITERation", "int",
                                  &param->iteration, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "MemSize", "int",
                                  &param->memsize, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "DmaSize", "int",
                                  &param->dmasize, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EntryNum", "int",
                                  &param->entrynum, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "ItemSelect", "enum",
                                  &param->flags, benchmark_cpu_test_items));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TRAffic", "bool",
                                  &param->run_traffic, NULL));

    /* parse start */
    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (show) {
        /* just show parsing result and exit function */
        bcma_cli_parse_table_show(&pt, NULL);
        SHR_EXIT();
    }

    /* Override parameter by user input */
    for (i = 0; i < BENCHMARK_CPU_REG_TEST_NUM; i++) {
        if (parse_reg_str[i] != NULL) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_name_to_sid(unit, parse_reg_str[i],
                                       param_reg_sid[i]));
        }
    }
    for (i = 0; i < BENCHMARK_CPU_MEM_TEST_NUM; i++) {
        if (parse_mem_str[i] != NULL) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_name_to_sid(unit, parse_mem_str[i],
                                       param_mem_sid[i]));
        }
    }

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                     uint32_t flags, void **bp)
{
    benchmark_cpu_param_t *bench_param = NULL;
    bool show = false;
    int i;
    bool regmem_assign = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(bench_param, sizeof(benchmark_cpu_param_t),
              "bcmaTestCaseBenchMarkPrm");
    SHR_NULL_CHECK(bench_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    bench_param->trial_num = BENCHMARK_CPU_DEFAULT_TRIALNUM;
    bench_param->iteration = BENCHMARK_CPU_DEFAULT_ITERATION;
    bench_param->flags = BENCHMARK_CPU_DEFAULT_FLAGS;
    bench_param->memsize = BENCHMARK_CPU_DEFAULT_MEMSIZE;
    bench_param->dmasize = BENCHMARK_CPU_DEFAULT_DMASIZE;
    bench_param->run_traffic = BENCHMARK_CPU_DEFAULT_TRAFFIC;
    bench_param->time_diff = NULL;
    bench_param->entrynum = BENCHMARK_CPU_DEFAULT_ENTRYNUM;
    for (i = 0; i < BENCHMARK_CPU_REG_TEST_NUM; i++) {
        bench_param->reglist[i] = INVALIDr;
    }
    for (i = 0; i < BENCHMARK_CPU_MEM_TEST_NUM; i++) {
        bench_param->memlist[i] = INVALIDm;
    }

    /* parse */
    SHR_IF_ERR_EXIT
        (benchmark_cpu_parse_param(unit, cli, a, bench_param, show));
    if (show == true) {
        SHR_FREE(bench_param);
        SHR_EXIT();
    }

    /* use default reg/mem testing list if the user does not assign */
    for (i = 0; i < BENCHMARK_CPU_REG_TEST_NUM; i++) {
        if (bench_param->reglist[i] != INVALIDr) {
            regmem_assign = true;
            break;
        }
    }
    for (i = 0; i < BENCHMARK_CPU_MEM_TEST_NUM; i++) {
        if (bench_param->memlist[i] != INVALIDm) {
            regmem_assign = true;
            break;
        }
    }
    if (regmem_assign == false) {
        const bcmdrd_sid_t *list_ptr = NULL;
        int list_num;

        if (SHR_SUCCESS(bcma_testutil_drv_benchmark_reglist_get
                            (unit, &list_ptr, &list_num))) {
            if (list_num > BENCHMARK_CPU_REG_TEST_NUM || list_ptr == NULL) {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
            sal_memcpy(bench_param->reglist, list_ptr,
                       sizeof(bcmdrd_sid_t) * list_num);
        }
        if (SHR_SUCCESS(bcma_testutil_drv_benchmark_memlist_get
                            (unit, &list_ptr, &list_num))) {
            if (list_num > BENCHMARK_CPU_MEM_TEST_NUM || list_ptr == NULL) {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
            sal_memcpy(bench_param->memlist, list_ptr,
                       sizeof(bcmdrd_sid_t) * list_num);
        }
    }

    *bp = (void *)bench_param;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(bench_param);
    }
    SHR_FUNC_EXIT();
}

static void
benchmark_cpu_help(int unit, void *bp)
{
    cli_out("%s", BENCHMARK_CPU_PARAM_HELP);
}

static void
benchmark_cpu_timer_show(uint64_t data_size, int time_diff,
                         const char *data_text, const char *data_subtext,
                         const char *data_unit, bool need_suffix)
{
#define TIMER_DATA_TEXT_STRLEN_MAX      "13"
#define TIMER_DATA_SUBTEXT_STRLEN_MAX   "5"
#define TIMER_DATA_UNIT_STRLEN_MAX      "4"

    cli_out("%"TIMER_DATA_TEXT_STRLEN_MAX"s : ", data_text);
    cli_out("%"TIMER_DATA_SUBTEXT_STRLEN_MAX"s ", data_subtext);
    bcma_testutil_stat_show(data_size * SECOND_USEC, time_diff, need_suffix);
    cli_out(" %"TIMER_DATA_UNIT_STRLEN_MAX"s/sec, ", data_unit);
    bcma_testutil_stat_show(time_diff, data_size, need_suffix);
    cli_out(" usec/%-"TIMER_DATA_UNIT_STRLEN_MAX"s\n", data_unit);
}

#define BENCHMARK_CPU_TIMER_START(_bench_param_,                 \
                                  _data_size_, _data_text_,      \
                                  _data_unit_, _need_suffix_)    \
    do {                                                         \
        int time_max = 0, time_sum = 0;                          \
        int i, j;                                                \
        int trial = (_bench_param_)->trial_num;                  \
        int iter = (_bench_param_)->iteration;                   \
        int *time_diff = (_bench_param_)->time_diff;             \
        uint64_t data_size = (_data_size_);                      \
        const char *data_text = (_data_text_);                   \
        const char *data_unit = (_data_unit_);                   \
        bool need_suffix = (_need_suffix_);                      \
        const char *empty_text = "";                             \
        const char *avg_text = "avg.";                           \
        const char *peak_text = "peak.";                         \
                                                                 \
        for (i = 0; i < trial; i++) {                            \
            sal_usecs_t benchmark_timer_start;                   \
            sal_usecs_t benchmark_timer_end;                     \
                                                                 \
            benchmark_timer_start = sal_time_usecs();            \
                                                                 \
            for (j = 0; j < iter; j++) {                         \

#define BENCHMARK_CPU_TIMER_STOP()                               \
            }                                                    \
            benchmark_timer_end = sal_time_usecs();              \
            time_diff[i] = (int)(benchmark_timer_end -           \
                                 benchmark_timer_start);         \
            time_sum += time_diff[i];                            \
            if (time_diff[i] > time_max) {                       \
                time_max = time_diff[i];                         \
            }                                                    \
        }                                                        \
        benchmark_cpu_timer_show(data_size * iter * trial,       \
                                 time_sum, data_text, avg_text,  \
                                 data_unit, need_suffix);        \
        benchmark_cpu_timer_show(data_size * iter,               \
                                 time_max, empty_text, peak_text,\
                                 data_unit, need_suffix);        \
    } while (0)

static int
benchmark_cpu_traffic_setup(int unit)
{
    int vid = BENCHMARK_TRAFFIC_VLAN_ID_BASE;
    int pid = BENCHMARK_TRAFFIC_VLAN_PROFILE_ID_BASE;
    int gid = BENCHMARK_TRAFFIC_L2_MC_GROUP_ID_BASE;
    int idx;
    bool need_l2_mc;
    bcmdrd_pbmp_t pbmp, ubmp;
    int netif_id, pktdev_id;
    bcmdrd_dev_type_t pkt_dev_type;
    bcmpkt_packet_t **packet_list = {NULL};
    bool rate_result;

    SHR_FUNC_ENTER(unit);

    /* setup loopback */
    BCMDRD_PBMP_CLEAR(ubmp);
    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, BENCHMARK_TRAFFIC_PORT_0);
    BCMDRD_PBMP_PORT_ADD(pbmp, BENCHMARK_TRAFFIC_PORT_1);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, LB_TYPE_MAC, pbmp));

    /* setup vlan */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_feature_get
            (unit, BCMA_TESTUTIL_FT_NEED_L2_MC_FOR_VLAN_FLOOD, &need_l2_mc));

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_create(unit, vid, pid));

    BCMDRD_PBMP_PORT_ADD(pbmp, BENCHMARK_TRAFFIC_PORT_CPU);
    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_port_add(unit, vid, pbmp, ubmp, true));

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_stp_set(unit, vid, pbmp,
                                    BCMA_TESTUTIL_STP_FORWARD));

    if (need_l2_mc == true) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_l2_multicast_add(unit, gid, pbmp,
                                            default_dst_mac_m, vid, 0));
    }

    /* setup packet device */
    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_init
            (unit, BENCHMARK_TRAFFIC_TX_CHAN, BENCHMARK_TRAFFIC_RX_CHAN,
             BENCHMARK_TRAFFIC_PKT_MAX_FRAME_SIZE, BENCHMARK_TRAFFIC_PKT_NUM,
             &netif_id, &pktdev_id));

    /* allocate packet buffer */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));

    SHR_ALLOC(packet_list,
              BENCHMARK_TRAFFIC_PKT_NUM * sizeof(bcmpkt_packet_t *),
              "bcmaTestCaseBenchMarkPkt");
    SHR_NULL_CHECK(packet_list, SHR_E_MEMORY);
    sal_memset(packet_list, 0,
               BENCHMARK_TRAFFIC_PKT_NUM * sizeof(bcmpkt_packet_t *));

    for (idx = 0; idx < BENCHMARK_TRAFFIC_PKT_NUM; idx++) {
        SHR_IF_ERR_EXIT
            (bcmpkt_alloc(pktdev_id, BENCHMARK_TRAFFIC_PKT_MAX_FRAME_SIZE,
                          BCMPKT_BUF_F_TX, &(packet_list[idx])));
    }

    /* send packet */
    for (idx = 0; idx < BENCHMARK_TRAFFIC_PKT_NUM; idx++) {

        bcmpkt_packet_t *packet = packet_list[idx];
        const shr_mac_t *dst_mac;

        if (need_l2_mc == true) {
            dst_mac = &default_dst_mac_m;
        } else {
            dst_mac = &default_dst_mac_u;
        }

        SHR_IF_ERR_EXIT
            (bcma_testutil_packet_fill
                (packet->data_buf, BENCHMARK_TRAFFIC_PKT_MAX_FRAME_SIZE,
                 default_src_mac, *dst_mac, true,
                 BENCHMARK_TRAFFIC_VLAN_ID_BASE,
                 BENCHMARK_TRAFFIC_PKT_ETHERTYPE,
                 BENCHMARK_TRAFFIC_PKT_PATTERN, 0));

        SHR_IF_ERR_EXIT
            (bcmpkt_fwd_port_set(pkt_dev_type,
                                 BENCHMARK_TRAFFIC_PORT_0, packet));

        SHR_IF_ERR_EXIT
            (bcmpkt_tx(unit, netif_id, packet));
     }

    /* show the current speed */
    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_create(unit));

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_add
            (unit, BENCHMARK_TRAFFIC_PORT_0, true, true, 0, ""));

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_start(unit));

    sal_sleep(BENCHMARK_TRAFFIC_POLLING_INTERVAL);

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_execute(unit, true,
                                                       &rate_result));

    if (rate_result == false) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "no traffic\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_stop(unit));

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_destroy(unit));

exit:
    if (packet_list != NULL) {
        for (idx = 0; idx < BENCHMARK_TRAFFIC_PKT_NUM; idx++) {
            if (packet_list[idx] != NULL) {
                bcmpkt_free(pktdev_id, packet_list[idx]);
                packet_list[idx] = NULL;
            }
        }
        SHR_FREE(packet_list);
        packet_list = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_traffic_cleanup(int unit)
{
    bcmdrd_pbmp_t pbmp;
    int vid = BENCHMARK_TRAFFIC_VLAN_ID_BASE;
    int gid = BENCHMARK_TRAFFIC_L2_MC_GROUP_ID_BASE;
    bool need_l2_mc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcma_testutil_packet_dev_cleanup(unit));

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_feature_get
            (unit, BCMA_TESTUTIL_FT_NEED_L2_MC_FOR_VLAN_FLOOD, &need_l2_mc));

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, BENCHMARK_TRAFFIC_PORT_0);
    BCMDRD_PBMP_PORT_ADD(pbmp, BENCHMARK_TRAFFIC_PORT_1);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, LB_TYPE_MAC, pbmp, TRUE));

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_port_remove(unit, vid, pbmp));

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_destroy(unit, vid));

    if (need_l2_mc == true) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_l2_multicast_delete(unit, gid, pbmp,
                                               default_dst_mac_m, vid, 0));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test_sema(int unit, benchmark_cpu_param_t *bench_param)
{
    sal_sem_t sema = NULL;

    SHR_FUNC_ENTER(unit);

    sema = sal_sem_create("bcmaTestCasBenchmarkSema", SAL_SEM_BINARY, 1);
    if (sema == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    cli_out("-- Test semaphore --\n");

    /* lock/unlock benchmark */
    BENCHMARK_CPU_TIMER_START(bench_param, 1,
                              "lock/unlock", "lock", true) {
        SHR_IF_ERR_EXIT
            (sal_sem_take(sema, SAL_SEM_FOREVER));
        SHR_IF_ERR_EXIT
            (sal_sem_give(sema));
    } BENCHMARK_CPU_TIMER_STOP();

    cli_out("\n");

exit:
    if (sema != NULL) {
        sal_sem_destroy(sema);
        sema = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test_mutex(int unit, benchmark_cpu_param_t *bench_param)
{
    sal_mutex_t mutex = NULL;

    SHR_FUNC_ENTER(unit);

    mutex = sal_mutex_create("bcmaTestCasBenchmarkMutex");
    if (mutex == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    cli_out("-- Test mutex --\n");

    /* lock/unlock benchmark */
    BENCHMARK_CPU_TIMER_START(bench_param, 1,
                              "lock/unlock", "lock", true) {
        SHR_IF_ERR_EXIT
            (sal_mutex_take(mutex, SAL_SEM_FOREVER));
        SHR_IF_ERR_EXIT
            (sal_mutex_give(mutex));
    } BENCHMARK_CPU_TIMER_STOP();

    cli_out("\n");

exit:
    if (mutex != NULL) {
        sal_mutex_destroy(mutex);
        mutex = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test_spinlock(int unit, benchmark_cpu_param_t *bench_param)
{
    sal_spinlock_t spinlock = NULL;

    SHR_FUNC_ENTER(unit);

    spinlock = sal_spinlock_create("bcmaTestCasBenchmarkSpinlock");
    if (spinlock == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    cli_out("-- Test spinlock --\n");

    /* lock/unlock benchmark */
    BENCHMARK_CPU_TIMER_START(bench_param, 1,
                              "lock/unlock", "lock", true) {
        SHR_IF_ERR_EXIT
            (sal_spinlock_lock(spinlock));
        SHR_IF_ERR_EXIT
            (sal_spinlock_unlock(spinlock));
    } BENCHMARK_CPU_TIMER_STOP();

    cli_out("\n");

exit:
    if (spinlock != NULL) {
        sal_spinlock_destroy(spinlock);
        spinlock = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test_lock(int unit, benchmark_cpu_param_t *bench_param)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (benchmark_cpu_test_sema(unit, bench_param));
    SHR_IF_ERR_EXIT
        (benchmark_cpu_test_mutex(unit, bench_param));
    SHR_IF_ERR_EXIT
        (benchmark_cpu_test_spinlock(unit, bench_param));

exit:
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test_reg(int unit, benchmark_cpu_param_t *bench_param,
                       bcmdrd_sid_t reg_sid)
{
    bcmdrd_sym_info_t reg_info;
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t pt_odata[BCMDRD_MAX_PT_WSIZE];
    uint32_t pt_wdata[BCMDRD_MAX_PT_WSIZE], pt_size;
    uint32_t pt_rdata[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, reg_sid, &reg_info));

    if (!(reg_info.flags & BCMDRD_SYMBOL_FLAG_REGISTER)) {
        cli_out("%d is not a valid register.\n", reg_sid);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (reg_info.flags & BCMDRD_SYMBOL_FLAG_MEMMAPPED) {
        cli_out("-- Test cmicreg %s --\n", reg_info.name);
    } else {
        cli_out("-- Test socreg %s --\n", reg_info.name);
    }

    dyn_info.index = bcmdrd_pt_index_min(unit, reg_sid);
    dyn_info.tbl_inst = 0;
    pt_size = bcmdrd_pt_entry_wsize(unit, reg_sid);

    /* backup original data */
    SHR_IF_ERR_EXIT
        (bcmbd_pt_read(unit, reg_sid, &dyn_info, NULL, pt_odata, pt_size));

    /* fill wdata with all-masked value */
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_mask_get
            (unit, reg_sid, dyn_info.index, pt_wdata, pt_size, FALSE));

    /* write/read benchmark */
    BENCHMARK_CPU_TIMER_START(bench_param, 1, "read", "r", true) {
        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, reg_sid, &dyn_info, NULL, pt_rdata, pt_size));
    } BENCHMARK_CPU_TIMER_STOP();

    BENCHMARK_CPU_TIMER_START(bench_param, 1, "write", "w", true) {
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, reg_sid, &dyn_info, NULL, pt_wdata));
    } BENCHMARK_CPU_TIMER_STOP();

    /* recover original data */
    SHR_IF_ERR_EXIT
        (bcmbd_pt_write(unit, reg_sid, &dyn_info, NULL, pt_odata));

    cli_out("\n");

exit:
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test_socmem_dma(int unit, benchmark_cpu_param_t *bench_param,
                              bcmdrd_sid_t mem_sid)
{
#define DMABUF_MAGIC_DATA   0xa5

    char *dma_buf_r = NULL, *dma_buf_w = NULL;
    uint64_t dma_addr_r, dma_addr_w;
    bcmdrd_sym_info_t mem_info;
    uint32_t pt_size, pt_wsize;
    uint32_t dma_index_start, dma_index_end, dma_entry_num;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, mem_sid, &mem_info));

    pt_size = bcmdrd_pt_entry_size(unit, mem_sid);
    pt_wsize = bcmdrd_pt_entry_wsize(unit, mem_sid);

    dma_index_start = mem_info.index_min;
    dma_index_end = mem_info.index_max;
    if (bench_param->entrynum != 0 &&
        dma_index_end - dma_index_start + 1 > bench_param->entrynum) {
        dma_index_end = dma_index_start + bench_param->entrynum - 1;
    }
    dma_entry_num = dma_index_end - dma_index_start + 1;

    dma_buf_r = bcmdrd_hal_dma_alloc(unit,
                                     dma_entry_num * BCMDRD_WORDS2BYTES(pt_wsize),
                                     "bcmaTestCaseBenchmarkRangebufR",
                                     &dma_addr_r);
    if (dma_buf_r == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    dma_buf_w = bcmdrd_hal_dma_alloc(unit,
                                     dma_entry_num * BCMDRD_WORDS2BYTES(pt_wsize),
                                     "bcmaTestCaseBenchmarkRangebufW",
                                     &dma_addr_w);
    if (dma_buf_w == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* benchmark read */
    BENCHMARK_CPU_TIMER_START(bench_param,
                              dma_entry_num * BCMDRD_BYTES2BITS(pt_size),
                              "DMA read", "b", true) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_mem_range_read(unit, mem_sid,
                                              dma_index_start,
                                              dma_index_end, 0,
                                              dma_addr_r));
    } BENCHMARK_CPU_TIMER_STOP();

    /* benchmark write */
    sal_memset(dma_buf_w, DMABUF_MAGIC_DATA,
               dma_entry_num * BCMDRD_WORDS2BYTES(pt_wsize));

    BENCHMARK_CPU_TIMER_START(bench_param,
                              dma_entry_num * BCMDRD_BYTES2BITS(pt_size),
                              "DMA write", "b", true) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_mem_range_write(unit, mem_sid,
                                               dma_index_start,
                                               dma_index_end, 0,
                                               dma_addr_w));
    } BENCHMARK_CPU_TIMER_STOP();

    cli_out("   (DMA data transferred  %d * %d bits = %d bits)\n",
                dma_entry_num, BCMDRD_BYTES2BITS(pt_size),
                dma_entry_num * BCMDRD_BYTES2BITS(pt_size));

    /* recover original data */
    SHR_IF_ERR_EXIT
            (bcma_testutil_drv_mem_range_write(unit, mem_sid,
                                               dma_index_start,
                                               dma_index_end, 0,
                                               dma_addr_r));

exit:
    if (dma_buf_r != NULL) {
        bcmdrd_hal_dma_free(unit, dma_entry_num * BCMDRD_WORDS2BYTES(pt_wsize),
                            dma_buf_r, dma_addr_r);
    }
    if (dma_buf_w != NULL) {
        bcmdrd_hal_dma_free(unit, dma_entry_num * BCMDRD_WORDS2BYTES(pt_wsize),
                            dma_buf_w, dma_addr_w);
    }
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test_socmem(int unit, benchmark_cpu_param_t *bench_param,
                          bcmdrd_sid_t mem_sid, int dma_size)
{
    bcmdrd_sym_info_t mem_info;
    uint32_t pt_odata[BCMDRD_MAX_PT_WSIZE];
    uint32_t pt_wdata[BCMDRD_MAX_PT_WSIZE], pt_size;
    uint32_t pt_rdata[BCMDRD_MAX_PT_WSIZE];
    bool not_support_hash;
    bcmbd_pt_dyn_info_t dyn_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, mem_sid, &mem_info));

    if (!(mem_info.flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
        cli_out("%d is not a valid memory.\n", mem_sid);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (mem_info.flags & BCMDRD_SYMBOL_FLAG_HASHED) {
        cli_out("-- Test socmem %s (hash) --\n", mem_info.name);
    } else if (mem_info.flags & BCMDRD_SYMBOL_FLAG_CAM) {
        cli_out("-- Test socmem %s (tcam) --\n", mem_info.name);
    } else {
        cli_out("-- Test socmem %s (index) --\n", mem_info.name);
    }

    /******************************************
     *  write / read
 */
    dyn_info.index = bcmdrd_pt_index_min(unit, mem_sid);
    dyn_info.tbl_inst = 0;
    pt_size = bcmdrd_pt_entry_wsize(unit, mem_sid);

    /* backup original data */
    SHR_IF_ERR_EXIT
        (bcmbd_pt_read(unit, mem_sid, &dyn_info, NULL, pt_odata, pt_size));

    /* fill wdata with all-masked value */
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_mask_get
            (unit, mem_sid, dyn_info.index, pt_wdata, pt_size, FALSE));

    /* benchmark : write / read */
    BENCHMARK_CPU_TIMER_START(bench_param, 1, "read", "r", true) {
        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, mem_sid, &dyn_info, NULL,
                           pt_rdata, pt_size));
    } BENCHMARK_CPU_TIMER_STOP();

    BENCHMARK_CPU_TIMER_START(bench_param, 1, "write", "w", true) {
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, mem_sid, &dyn_info, NULL, pt_wdata));
    } BENCHMARK_CPU_TIMER_STOP();

    /* recover original data */
    SHR_IF_ERR_EXIT
        (bcmbd_pt_write(unit, mem_sid, &dyn_info, NULL, pt_odata));


    /******************************************
     *  insert / lookup /delete (if supported)
 */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_feature_get
            (unit, BCMA_TESTUTIL_FT_NOT_SUPPORT_PT_LOOKUP, &not_support_hash));

    if (mem_info.flags & BCMDRD_SYMBOL_FLAG_HASHED &&
        bcmdrd_feature_is_sim(unit) == false && not_support_hash == false) {
        /* hash table */
        bcmbd_pt_dyn_hash_info_t dyn_hash_info;
        bcmbd_tbl_resp_info_t resp_info;

        dyn_hash_info.bank = 0;
        dyn_hash_info.tbl_inst = 0;
        pt_size = bcmdrd_pt_entry_wsize(unit, mem_sid);
        if (pt_size == 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* fill wdata with all-masked value */
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_mask_get
                (unit, mem_sid, 0, pt_wdata, pt_size, FALSE));

        /* adjust testing data for hash operation if need */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcma_testutil_drv_mem_hash_data_adjust
                (unit, mem_sid, pt_wdata, pt_size), SHR_E_UNAVAIL);

        /* benchmark 1 : insert / delete */
        BENCHMARK_CPU_TIMER_START(bench_param, 1,
                                  "insert/delete", "i+d", true) {
            SHR_IF_ERR_EXIT
                (bcmbd_pt_insert(unit, mem_sid, &dyn_hash_info, NULL,
                                 pt_wdata, pt_size, &resp_info, NULL));

            SHR_IF_ERR_EXIT
                (bcmbd_pt_delete(unit, mem_sid, &dyn_hash_info, NULL,
                                 pt_wdata, pt_size, &resp_info, NULL));
        } BENCHMARK_CPU_TIMER_STOP();

        /* benchmark 2 : lookup */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_insert(unit, mem_sid, &dyn_hash_info, NULL,
                             pt_wdata, pt_size, &resp_info, NULL));

        BENCHMARK_CPU_TIMER_START(bench_param, 1,
                                  "lookup", "lkup", true) {
            SHR_IF_ERR_EXIT
                (bcmbd_pt_lookup(unit, mem_sid, &dyn_hash_info, NULL,
                                 pt_wdata, pt_size, &resp_info, NULL));

        } BENCHMARK_CPU_TIMER_STOP();

        /* recover original data */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_delete(unit, mem_sid, &dyn_hash_info, NULL,
                             pt_wdata, pt_size, &resp_info, NULL));
    }

    /******************************************
     *  DMA range access
 */
    if (bcmdrd_feature_is_sim(unit) == false) {
        SHR_IF_ERR_EXIT
            (benchmark_cpu_test_socmem_dma(unit, bench_param, mem_sid));
    }

    cli_out("\n");

exit:
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test_sysmem(int unit, benchmark_cpu_param_t *bench_param,
                          int sysmem_size, int sysdma_size)
{
    char *sysmem = NULL, *sysmem_dst = NULL;
    char *sysdma = NULL, *sysdma_dst = NULL;
    int value_mb = 1024 * 1024;

    SHR_FUNC_ENTER(unit);

    cli_out("-- Test sysmem (w/o DMA) --\n");

    /* system memory (w/o DMA) benchmark - alloc / free */
    BENCHMARK_CPU_TIMER_START(bench_param, BCMDRD_BYTES2BITS(sysmem_size * value_mb),
                              "alloc/free", "b", true) {
        SHR_ALLOC(sysmem, sysmem_size * value_mb,
                  "bcmaTestCaseBenchmarkSysmem");
        SHR_NULL_CHECK(sysmem, SHR_E_MEMORY);
        SHR_FREE(sysmem);
    } BENCHMARK_CPU_TIMER_STOP();

    /* system memory (w/o DMA) benchmark - memset */
    SHR_ALLOC(sysmem, sysmem_size * value_mb, "bcmaTestCaseBenchmarkSysmem");
    SHR_NULL_CHECK(sysmem, SHR_E_MEMORY);

    BENCHMARK_CPU_TIMER_START(bench_param, BCMDRD_BYTES2BITS(sysmem_size * value_mb),
                              "memset", "b", true) {
        sal_memset(sysmem, 0, sysmem_size * value_mb);
    } BENCHMARK_CPU_TIMER_STOP();

    /* system memory (w/o DMA) benchmark - memcpy */
    SHR_ALLOC(sysmem_dst, sysmem_size * value_mb,
              "bcmaTestCaseBenchmarkSysmemDst");
    SHR_NULL_CHECK(sysmem_dst, SHR_E_MEMORY);
    BENCHMARK_CPU_TIMER_START(bench_param, BCMDRD_BYTES2BITS(sysmem_size * value_mb),
                              "memcpy", "b", true) {
        sal_memcpy(sysmem_dst, sysmem, sysmem_size * value_mb);
    } BENCHMARK_CPU_TIMER_STOP();

    cli_out("\n");

    cli_out("-- Test sysmem (w DMA) --\n");
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        cli_out("  (Built-in simulator does not support DMA)\n\n");
        SHR_EXIT();
    }
    /* system memory (w DMA) benchmark - alloc / free */
    BENCHMARK_CPU_TIMER_START(bench_param, BCMDRD_BYTES2BITS(sysdma_size * value_mb),
                              "alloc/free", "b", true) {
        sysdma = bcmdrd_hal_dma_alloc(unit, sysdma_size * value_mb,
                                      "bcmaTestCaseBenchmarkSysdma", NULL);
        if(sysdma == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        bcmdrd_hal_dma_free(unit, sysdma_size * value_mb, sysdma, 0);
        sysdma = NULL;
    } BENCHMARK_CPU_TIMER_STOP();

    /* system memory (w DMA) benchmark - memset */
    sysdma = bcmdrd_hal_dma_alloc(unit, sysdma_size * value_mb,
                                  "bcmaTestCaseBenchmarkSysdma", NULL);
    if(sysdma == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    BENCHMARK_CPU_TIMER_START(bench_param, BCMDRD_BYTES2BITS(sysdma_size * value_mb),
                              "memset", "b", true) {
        sal_memset(sysdma, 0, sysdma_size * value_mb);
    } BENCHMARK_CPU_TIMER_STOP();

    /* system memory (w DMA) benchmark - memcpy */
    sysdma_dst = bcmdrd_hal_dma_alloc(unit, sysdma_size * value_mb,
                                      "bcmaTestCaseBenchmarkSysdmaDst", NULL);
    if(sysdma_dst == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    BENCHMARK_CPU_TIMER_START(bench_param, BCMDRD_BYTES2BITS(sysdma_size * value_mb),
                              "memcpy", "b", true) {
        sal_memcpy(sysdma_dst, sysdma, sysdma_size * value_mb);
    } BENCHMARK_CPU_TIMER_STOP();

    cli_out("\n");

exit:
    SHR_FREE(sysmem);
    SHR_FREE(sysmem_dst);
    if (sysdma != NULL) {
        bcmdrd_hal_dma_free(unit, sysdma_size * value_mb, sysdma, 0);
    }
    if (sysdma_dst != NULL) {
        bcmdrd_hal_dma_free(unit, sysdma_size * value_mb, sysdma_dst, 0);
    }
    SHR_FUNC_EXIT();
}

static int
benchmark_cpu_test(int unit, void *bp, uint32_t option)
{
    benchmark_cpu_param_t *bench_param = (benchmark_cpu_param_t *)bp;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bench_param, SHR_E_PARAM);

    SHR_ALLOC(bench_param->time_diff, bench_param->trial_num * sizeof(int),
              "bcmaTestCaseBenchmarkTimediff");
    SHR_NULL_CHECK(bench_param->time_diff, SHR_E_MEMORY);

    if (bench_param->run_traffic == TRUE) {
        cli_out("introduce heavy traffic during testing...");
        SHR_IF_ERR_EXIT(benchmark_cpu_traffic_setup(unit));
        cli_out("\n");
    }

    if (bench_param->flags & BENCHMARK_CPU_TF_REG) {
        for (i = 0; i < BENCHMARK_CPU_REG_TEST_NUM; i++) {
            if (bench_param->reglist[i] == INVALIDr) {
                continue;
            }
            SHR_IF_ERR_EXIT
                (benchmark_cpu_test_reg(unit, bench_param,
                                        bench_param->reglist[i]));
        }
    }

    if (bench_param->flags & BENCHMARK_CPU_TF_MEM) {
        for (i = 0; i < BENCHMARK_CPU_MEM_TEST_NUM; i++) {
            if (bench_param->memlist[i] == INVALIDm) {
                continue;
            }
            SHR_IF_ERR_EXIT
                (benchmark_cpu_test_socmem(unit, bench_param,
                                           bench_param->memlist[i],
                                           bench_param->dmasize));
        }
    }

    if (bench_param->flags & BENCHMARK_CPU_TF_LOCK) {
        SHR_IF_ERR_EXIT
            (benchmark_cpu_test_lock(unit, bench_param));
    }

    if (bench_param->flags & BENCHMARK_CPU_TF_SYSMEM) {
        SHR_IF_ERR_EXIT
            (benchmark_cpu_test_sysmem(unit, bench_param,
                                       bench_param->memsize,
                                       bench_param->dmasize));
    }

    if (bench_param->run_traffic == TRUE) {
        cli_out("stop traffic\n");
        SHR_IF_ERR_EXIT(benchmark_cpu_traffic_cleanup(unit));
    }

exit:
    SHR_FREE(bench_param->time_diff);
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t benchmark_cpu_proc[] = {
    {
        .desc = "Run testing\n",
        .cb = benchmark_cpu_test,
    }
};

static void
benchmark_cpu_recycle(int unit, void *bp)
{
    /* tra_param would be freed in testdb_run_teardown */
    return;
}

static bcma_test_op_t benchmark_cpu_op = {
    .select = benchmark_cpu_select,
    .parser = benchmark_cpu_parser,
    .help = benchmark_cpu_help,
    .recycle = benchmark_cpu_recycle,
    .procs = benchmark_cpu_proc,
    .proc_count = COUNTOF(benchmark_cpu_proc)
};

bcma_test_op_t *
bcma_testcase_benchmark_cpu_op_get(void)
{
    return &benchmark_cpu_op;
}
