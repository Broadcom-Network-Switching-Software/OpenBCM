/*! \file bcm56990_a0_testutil_max_power.c
 *
 * Chip-specific TRAFFIC data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_sleep.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <shr/shr_util.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>

#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/testcase/bcma_testcase_traffic_max_power.h>
#include <bcmlt/bcmlt.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmptm/bcmptm.h>

#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_power.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_traffic.h>

#include <bcma/test/chip/bcm56990_a0_testutil_traffic.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define TH4_PWRT_DEV_PORTS_PER_DEV           272
#define TH4_PWRT_DEV_PORTS_PER_PIPE           17
#define TH4_PWRT_PIPES_PER_DEV                16


#define TH4_PWRT_MAX_NUM_DEVICES 1

#define TH4_PWRT_EN_FULL_IFP  0
#define TH4_PWRT_EN_EM        1
#define TH4_PWRT_EN_VFP       2
#define TH4_PWRT_EN_EFP       3
#define TH4_PWRT_EN_FLX_CTR   4
#define TH4_PWRT_EN_TRAP      5
#define TH4_PWRT_EN_TCAM_NOISE      6
#define TH4_PWRT_EN_LAG_RES      7
#define TH4_PWRT_EN_RND_HDR      8
#define TH4_PWRT_EN_RND_INITFLEXCTR      9
#define TH4_PWRT_DIS_CUT_THROUGH        10
#define TH4_PWRT_DIS_SAME_SRC_DST       11
#define TH4_PWRT_EN_RND_PRI             12

#define TH4_PWRT_MPLS_IPV6         0
#define TH4_PWRT_MPLS_IPV4         1
#define TH4_PWRT_IPV6              2
#define TH4_PWRT_IPV4              3
#define TH4_PWRT_L2_ONLY           4

#define TH4_PWRT_L3_IIF_BASE      0xfc7
#define TH4_PWRT_VLAN_ID_BASE     0xe00
#define TH4_PWRT_MAC_BASE    {0x12, 0x34, 0x56, 0x78, 0x9a, 0x00}
#define TH4_PWRT_IP_DA       {0xa5a5a000, 0xa5a5a5a5, 0xabcde000, 0x12345678}
#define TH4_PWRT_IPV4_DA     0x12345600

#define TH4_PWRT_MPLS_LABEL  0x000a5000
#define TH4_PWRT_VRF_BASE    0x1f00

/* MAC + 3xMPLS + IPv6 + CRC = 18 + 3x4 + 40 + 4 */
#define TH4_PWRT_MPLS_IPV6_MIN_PKT_SIZE  74

#define TH4_PWRT_SET_FIELD(field, value) \
    memfld[0] = value; \
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, field, entry, memfld))

#define TH4_PWRT_PT_DYN_INFO(pt_dyn_info, id, inst)\
    do { \
        pt_dyn_info.index = id; \
        pt_dyn_info.tbl_inst = inst; \
    } while (0);


typedef uint8_t bcm_mac_t[6];


typedef struct power_test_s {
    uint32_t dst_port[TH4_PWRT_DEV_PORTS_PER_DEV];
    uint32_t num_flows;
    uint32_t start_flow;
    uint32_t src_port_flow_id[TH4_PWRT_DEV_PORTS_PER_DEV];   /* phy_src_port = f(flow_id) */
    uint32_t dst_port_flow_id[TH4_PWRT_DEV_PORTS_PER_DEV];   /* phy_dst_port = f(flow_id) */
    uint32_t p2l_mapping[TH4_PWRT_DEV_PORTS_PER_DEV];        /* dst_port = f(flow_id) */
    uint32_t dev_enabled_ports[TH4_PWRT_DEV_PORTS_PER_DEV];  /* indexed by dev_port; 1 if port enabled */
    uint32_t phy_enabled_ports[TH4_PWRT_DEV_PORTS_PER_DEV];  /* indexed by dev_port; 1 if port enabled */
    uint32_t phy_to_flow_id[TH4_PWRT_DEV_PORTS_PER_DEV];     /* indexed by phy_port; retrieves flow_id */
    uint32_t alpm_level;
    uint32_t enable_map;
    uint32_t enable_stress_tcam_param;
    uint32_t enable_ecmp_dlb;
    uint32_t enable_ecmp_dlb_debug;
    uint32_t enable_rnd_mac;
    uint32_t enable_rnd_mpls;
    uint32_t enable_rnd_ip;
    bcmdrd_pbmp_t pbmp_traffic;
    uint32_t pkt_flow;
    uint32_t lpm_en;
    int same_src_dst;
    int traffic_from_all;
    int verbose_level;
} power_test_t;

static power_test_t *power_test_parray[TH4_PWRT_MAX_NUM_DEVICES];

typedef struct th4_pwr_tsc_connect_s {
    int tsc_0;
    int tsc_1;
} th4_pwr_tsc_connect_t;

static const th4_pwr_tsc_connect_t
th4_pwr_tsc_connect[32] = {
    {  0,   3},
    {  1,   2},
    {  4,   6},
    {  5,   7},
    {  8,  10},
    {  9,  11},
    { 12,  14},
    { 13,  15},
    { 16,  18},
    { 17,  19},
    { 20,  22},
    { 21,  23},
    { 24,  26},
    { 25,  27},
    { 29,  30},
    { 28,  31},
    { 32,  35},
    { 33,  34},
    { 37,  39},
    { 36,  38},
    { 40,  42},
    { 41,  43},
    { 44,  46},
    { 45,  47},
    { 48,  50},
    { 49,  51},
    { 52,  54},
    { 53,  55},
    { 56,  58},
    { 57,  59},
    { 60,  63},
    { 61,  62}
};


static int
get_itm_num(int unit, int port)
{
    int pipe, itm;

    (void)(bcmtm_lport_pipe_get(unit, port, &pipe));
    itm = ((pipe >= 4) && (pipe <= 11)) ? 1 : 0;

    return itm;
}


static int
topology_get_port_pair(
    int unit,
    void *bp,
    bcmdrd_pbmp_t *left_ports_pbmp,
    int port,
    int *pair_port)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;
    int tsc, tsc_in, pair_tsc = 0, phy_port, subport, pair_phy_port;
    int p, itm, other_itm;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    if (tra_param->topology == 2) {
        /* tra_param->topology == 2
         * pair as per external loopback TSC mapping
         */
        SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, port, &phy_port));
        tsc_in = (phy_port - 1) / 4;
        subport = (phy_port - 1) % 4;
        for (tsc = 0; tsc < 32; tsc++) {
            if (th4_pwr_tsc_connect[tsc].tsc_0 == tsc_in) {
                 pair_tsc = th4_pwr_tsc_connect[tsc].tsc_1;
                 break;
            } else if (th4_pwr_tsc_connect[tsc].tsc_1 == tsc_in) {
                 pair_tsc = th4_pwr_tsc_connect[tsc].tsc_0;
                 break;
            }
        }
        pair_phy_port = 1 + (pair_tsc * 4) + subport;
        bcmtm_pport_lport_get(unit, pair_phy_port, pair_port);
    } else if (tra_param->topology == 1){
        /* tra_param->topology == 1
         * pair between two itms
         */
        itm = get_itm_num(unit, port);
        BCMDRD_PBMP_ITER(*left_ports_pbmp, p) {
            if (port != p) {
                other_itm = get_itm_num(unit, p);
                if (itm != other_itm) {
                    *pair_port = p;
                    break;
                }
            }
        }
    } else {
        /* tra_param->topology == 0
         * pair two contiguous ports
         */
        BCMDRD_PBMP_ITER(*left_ports_pbmp, p) {
            if (port != p) {
                *pair_port = p;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int
power_test_param_init(int unit, void *bp)
{
    power_test_t *power_test_p;
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;
    int port, phy_port, max_speed;
    uint32_t min_pkt_len, i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* Allocate and initialize local struct */
    power_test_p = power_test_parray[unit];
    power_test_p = sal_alloc(sizeof(power_test_t), "bcmaTestPower");
    sal_memset(power_test_p, 0, sizeof(power_test_t));
    power_test_parray[unit] = power_test_p;

    /* Filter out all non-active ports */
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
         SHR_IF_ERR_EXIT(bcmtm_lport_max_speed_get(unit, port, &max_speed));
         if ((max_speed > 0) &&
              bcmtm_lport_is_fp(unit, port) &&
              (port < TH4_PWRT_DEV_PORTS_PER_DEV)) {
             power_test_p->dev_enabled_ports[port] = 1;
             SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, port, &phy_port));
             power_test_p->phy_enabled_ports[phy_port] = 1;
             power_test_p->p2l_mapping[phy_port] = port;
         } else {
             BCMDRD_PBMP_PORT_REMOVE(tra_param->pbmp_all, port);
         }
    }

    /* Chip specific default params */
    power_test_p->same_src_dst = 0;
    power_test_p->traffic_from_all = 0;

    if (tra_param->enable_bmp == 0xffffffff) {
        tra_param->enable_bmp = (0x1 << TH4_PWRT_EN_FULL_IFP) |
                                (0x1 << TH4_PWRT_EN_EM) |
                                (0x1 << TH4_PWRT_EN_VFP) |
                                (0x1 << TH4_PWRT_EN_EFP) |
                                (0x1 << TH4_PWRT_EN_FLX_CTR) |
                                (0x1 << TH4_PWRT_EN_TRAP) |
                                (0x1 << TH4_PWRT_EN_TCAM_NOISE) | /* Not used */
                                (0x1 << TH4_PWRT_EN_LAG_RES) |
                                (0x1 << TH4_PWRT_EN_RND_HDR) | /* Not used */
                                (0x1 << TH4_PWRT_EN_RND_INITFLEXCTR) |
                                (0x1 << TH4_PWRT_DIS_CUT_THROUGH) |
                                (0x1 << TH4_PWRT_DIS_SAME_SRC_DST) |
                                (0x1 << TH4_PWRT_EN_RND_PRI);
    }
    power_test_p->enable_map = tra_param->enable_bmp;

    /* Set default */
    if (tra_param->topology == -1) {
        tra_param->topology = 0;
    }

    /* Assign power_test_p parameters */
    /* LPM Level 1,2 & 3 allowed in TH4 */
    if ((tra_param->lpm_level < 1) ||
        (tra_param->lpm_level > 3)) {
        tra_param->lpm_level = 3;
    }
    power_test_p->alpm_level = tra_param->lpm_level;
    power_test_p->enable_stress_tcam_param = 0;
    power_test_p->enable_ecmp_dlb = tra_param->enable_dlb;
    power_test_p->enable_ecmp_dlb_debug = 0;

    power_test_p->pkt_flow = TH4_PWRT_MPLS_IPV6;
    power_test_p->lpm_en = 1;

    power_test_p->verbose_level = 3;

    power_test_p->enable_rnd_mac  = tra_param->enable_rnd_hdr;
    power_test_p->enable_rnd_mpls = tra_param->enable_rnd_hdr;
    power_test_p->enable_rnd_ip   = tra_param->enable_rnd_hdr;

    /* External loopback: force topology = 2 always
     * independent of user preference
     */
    if (tra_param->loopback_mode == LB_TYPE_EXT) {
        tra_param->topology = 2;
        power_test_p->same_src_dst = 1;
        power_test_p->traffic_from_all = 0;
    } else {
        if (((power_test_p->enable_map >> TH4_PWRT_DIS_SAME_SRC_DST) & 0x1) == 0x1) {
            power_test_p->same_src_dst = 0;
        } else {
            power_test_p->same_src_dst = 1;
        }
        power_test_p->traffic_from_all = power_test_p->same_src_dst;
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit,
            "power_test_p->same_src_dst=%0d power_test_p->traffic_from_all=%0d\n"),
             power_test_p->traffic_from_all, power_test_p->same_src_dst));



    /* Make sure that pkt size set by parse param
     * is at least minimum required by the flow
     */
    min_pkt_len = TH4_PWRT_MPLS_IPV6_MIN_PKT_SIZE;
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        for (i = 0; i < tra_param->pkt_cnt[port]; i++) {
            if (tra_param->pkt_len[port][i] < min_pkt_len) {
                tra_param->pkt_len[port][i] = min_pkt_len;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
power_test_set_topology(int unit, void *bp)
{
    int port;
    uint32_t flow_id;
    power_test_t *power_test_p = power_test_parray[unit];
    int lport_src, lport_dst;
    int src_port, dst_port, pair_port = 0;
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;
    bcmdrd_pbmp_t left_ports_pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* Create the test traffic topology */
    flow_id = 0;
    BCMDRD_PBMP_CLEAR(power_test_p->pbmp_traffic);
    BCMDRD_PBMP_ASSIGN(left_ports_pbmp, tra_param->pbmp_all);
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        if (BCMDRD_PBMP_MEMBER(left_ports_pbmp, port)) {
            SHR_IF_ERR_EXIT(
                 topology_get_port_pair(unit, bp, &left_ports_pbmp,
                                        port, &pair_port));
            BCMDRD_PBMP_PORT_REMOVE(left_ports_pbmp, port);
            BCMDRD_PBMP_PORT_REMOVE(left_ports_pbmp, pair_port);

            SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, port, &src_port));
            SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, pair_port, &dst_port));

            power_test_p->dst_port[src_port] = dst_port;
            power_test_p->dst_port[dst_port] = src_port;

            power_test_p->src_port_flow_id[flow_id] = src_port;
            power_test_p->dst_port_flow_id[flow_id] = dst_port;
            power_test_p->phy_to_flow_id[src_port] = flow_id;

            power_test_p->src_port_flow_id[flow_id + 1] = dst_port;
            power_test_p->dst_port_flow_id[flow_id + 1] = src_port;
            power_test_p->phy_to_flow_id[dst_port] = flow_id + 1;
            flow_id += 2;
            BCMDRD_PBMP_PORT_ADD(power_test_p->pbmp_traffic, port);
            if (power_test_p->traffic_from_all == 1) {
                BCMDRD_PBMP_PORT_ADD(power_test_p->pbmp_traffic, pair_port);
            }
        }
    }
    power_test_p->start_flow = 0;
    power_test_p->num_flows = flow_id;


    for (flow_id = power_test_p->start_flow;
         flow_id < (power_test_p->start_flow + power_test_p->num_flows);
         flow_id++) {
        src_port = power_test_p->src_port_flow_id[flow_id];
        dst_port = power_test_p->dst_port_flow_id[flow_id];
        bcmtm_pport_lport_get(unit, src_port, &lport_src);
        bcmtm_pport_lport_get(unit, dst_port, &lport_dst);
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
               "flow_id=%3d dev_port_pair= { %3d <=> %3d}  phy_port_pair={ %3d <=> %3d}\n"),
               flow_id, lport_src, lport_dst, src_port, dst_port));
    }

exit:
    SHR_FUNC_EXIT();
}


static int
power_test_init(int unit, void *bp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(power_test_param_init(unit, bp));
    SHR_IF_ERR_EXIT(power_test_set_topology(unit, bp));

exit:
    SHR_FUNC_EXIT();
}


static void get_base_rand(uint32_t seed, uint32_t *out)
{
    uint32_t val;
    uint32_t seed_post;

    seed_post = seed & 0xffff;
    val = (seed_post * 16807) & 0xffff;
    *out = val << 16;
    val = (val * 16807) & 0xffff;
    *out |= val;
}

static void get_mac_base(uint32_t seed, uint32_t *out)
{
    uint32_t nz_seed, nz_seed_next;

    nz_seed = (seed + 1) * 53;
    get_base_rand(nz_seed, &nz_seed_next);
    nz_seed = nz_seed_next;
    /* Apply a 0 mask on bits [8:0] */
    out[0] = nz_seed & 0xfffffe00;
    get_base_rand(nz_seed, &nz_seed_next);
    nz_seed = nz_seed_next;
    out[1] = nz_seed & 0x0000feff;
}

static void pwr_get_mac_base(int unit, uint32_t seed, uint32_t *out)
{
    power_test_t *power_test_p = power_test_parray[unit];
    bcm_mac_t l_mac_addr = TH4_PWRT_MAC_BASE;

    if (power_test_p->enable_rnd_mac == 1) {
        get_mac_base(seed, out);
    } else {
        out[0] = (l_mac_addr[2] << 24) | (l_mac_addr[3] << 16) |
                 (l_mac_addr[4] << 8) | l_mac_addr[5];
        out[1] = (l_mac_addr[0] << 8) | l_mac_addr[1];
    }
}


static void pwr_get_mac_base_bcm_mac(int unit, uint32_t seed, bcm_mac_t mac_addr)
{
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];

    pwr_get_mac_base(unit, seed, memfld);

    mac_addr[0] = (memfld[1] >> 8) & 0xff;
    mac_addr[1] = (memfld[1] >> 0) & 0xff;
    mac_addr[2] = (memfld[0] >> 24) & 0xff;
    mac_addr[3] = (memfld[0] >> 16) & 0xff;
    mac_addr[4] = (memfld[0] >> 8) & 0xff;
    mac_addr[5] = (memfld[0] >> 0) & 0xff;
}


static void get_ipv6_base(uint32_t seed, uint32_t *out)
{
    uint32_t nz_seed, nz_seed_next;

    nz_seed = (seed + 1) * 61;
    get_base_rand(nz_seed, &nz_seed_next);
    nz_seed = nz_seed_next;
    out[0] = nz_seed & 0xfffffe00;
    get_base_rand(nz_seed, &nz_seed_next);
    nz_seed = nz_seed_next;
    out[1] = nz_seed;
    get_base_rand(nz_seed, &nz_seed_next);
    nz_seed = nz_seed_next;
    out[2] = nz_seed & 0xfffffe00;
    get_base_rand(nz_seed, &nz_seed_next);
    nz_seed = nz_seed_next;
    /* Apply a 0 mask on bits [11:0] & [75:64] */
    out[3] = (nz_seed & 0xfeffffff);
}

static void pwr_get_ipv6_base(int unit, uint32_t seed, uint32_t *out)
{
    power_test_t *power_test_p = power_test_parray[unit];
    uint32_t ip_addr[BCMDRD_MAX_PT_WSIZE] = TH4_PWRT_IP_DA;

    if (power_test_p->enable_rnd_ip == 1) {
        get_ipv6_base(seed, out);
        out[3] = ip_addr[3];
    } else {
        out[0] = ip_addr[0];
        out[1] = ip_addr[1];
        out[2] = ip_addr[2];
        out[3] = ip_addr[3];
    }
}

static void get_mpls_label_base(int label_num, uint32_t seed, uint32_t *out)
{
    uint32_t nz_seed, nz_seed_next;

    nz_seed = (seed + 1 + label_num) * 67;
    get_base_rand(nz_seed, &nz_seed_next);
    nz_seed = nz_seed_next;
    /* Apply a 0 mask on bits [31:20] & [8:0] */
    out[0] = nz_seed & 0x000ffe00;
}

static void pwr_get_mpls_label_base(int unit, int label_num, uint32_t seed, uint32_t *out)
{
    power_test_t *power_test_p = power_test_parray[unit];

    if (power_test_p->enable_rnd_mpls == 1) {
        get_mpls_label_base(label_num, seed, out);
    } else {
        out[0] = TH4_PWRT_MPLS_LABEL;
        out[0] |= label_num << 8;
    }
}


static uint32_t is_l2_only_en(
    int unit,
    int flow_id,
    int is_cpu)
{
    power_test_t *power_test_p = power_test_parray[unit];
    uint32_t l_is_en;

    l_is_en = 0;
    switch (power_test_p->pkt_flow) {
        case TH4_PWRT_MPLS_IPV6 :
        case TH4_PWRT_MPLS_IPV4 :
        case TH4_PWRT_IPV6 :
        case TH4_PWRT_IPV4 :
            l_is_en = 0;
            break;
        case TH4_PWRT_L2_ONLY:
            l_is_en = 1;
            break;
        default :
            l_is_en = 0;
            break;
    }
    return l_is_en;
}

static uint32_t is_mpls_en(
    int unit,
    int flow_id,
    int is_cpu)
{
    power_test_t *power_test_p = power_test_parray[unit];
    uint32_t l_is_en;

    l_is_en = 0;
    switch (power_test_p->pkt_flow) {
        case TH4_PWRT_MPLS_IPV6 :
        case TH4_PWRT_MPLS_IPV4 :
            l_is_en = 1;
            break;
        case TH4_PWRT_IPV6 :
        case TH4_PWRT_IPV4 :
        case TH4_PWRT_L2_ONLY:
            l_is_en = 0;
            break;
        default :
            l_is_en = 0;
            break;
    }
    return l_is_en;
}


static uint32_t is_l3_en(
    int unit,
    int flow_id,
    int is_cpu)
{
    power_test_t *power_test_p = power_test_parray[unit];
    uint32_t l_is_en;

    l_is_en = 0;
    switch (power_test_p->pkt_flow) {
        case TH4_PWRT_MPLS_IPV6 :
        case TH4_PWRT_MPLS_IPV4 :
        case TH4_PWRT_IPV6 :
        case TH4_PWRT_IPV4 :
            l_is_en = 1;
            break;
        case TH4_PWRT_L2_ONLY:
            l_is_en = 0;
            break;
        default :
            l_is_en = 0;
            break;
    }
    return l_is_en;
}

static uint32_t is_l3_ipv4(
    int unit,
    int flow_id,
    int is_cpu)
{
    power_test_t *power_test_p = power_test_parray[unit];
    uint32_t l_is_en;

    l_is_en = 0;
    switch (power_test_p->pkt_flow) {
        case TH4_PWRT_MPLS_IPV4 :
        case TH4_PWRT_IPV4 :
            l_is_en = 1;
            break;
        case TH4_PWRT_MPLS_IPV6 :
        case TH4_PWRT_IPV6 :
        case TH4_PWRT_L2_ONLY:
            l_is_en = 0;
            break;
        default :
            l_is_en = 0;
            break;
    }

    return l_is_en;
}


static uint32_t sal_rand32(void)
{
    return (((sal_rand() &  0xFF) << 24) |
            ((sal_rand() & 0xFFF) << 12) |
            ((sal_rand() & 0xFFF) <<  0) );
}

static int get_phy_src_port(
    int unit,
    int flow_id)
{
    power_test_t *power_test_p = power_test_parray[unit];
    int src_phy_port;

    src_phy_port = power_test_p->src_port_flow_id[flow_id];
    return src_phy_port;
}

static int get_phy_dst_port(
    int unit,
    int flow_id)
{
    power_test_t *power_test_p = power_test_parray[unit];
    int dst_phy_port;

    dst_phy_port = power_test_p->dst_port_flow_id[flow_id];
    return dst_phy_port;
}

static int get_dev_src_port(
    int unit,
    int flow_id)
{
    power_test_t *power_test_p = power_test_parray[unit];
    int src_phy_port;

    src_phy_port = power_test_p->src_port_flow_id[flow_id];
    return power_test_p->p2l_mapping[src_phy_port];
}

static int get_dev_dst_port(
    int unit,
    int flow_id)
{
    power_test_t *power_test_p = power_test_parray[unit];
    int dst_phy_port;

    dst_phy_port = power_test_p->dst_port_flow_id[flow_id];
    return power_test_p->p2l_mapping[dst_phy_port];
}


static int get_final_dst_dev_port(
    int unit,
    int flow_id)
{
    power_test_t *power_test_p = power_test_parray[unit];

    if (power_test_p->same_src_dst == 1) {
        return get_dev_src_port(unit, flow_id);
    } else {
        return get_dev_dst_port(unit, flow_id);
    }
}


static int get_alpm_level(
    int unit)
{
    power_test_t *power_test_p = power_test_parray[unit];
    return (power_test_p->alpm_level);
}

static uint32_t get_ip_lport_profile_index(
    int unit,
    int flow_id)
{
    return (get_dev_src_port(unit, flow_id) % TH4_PWRT_DEV_PORTS_PER_PIPE);
}


static uint32_t get_l3_iif(
    int unit,
    int flow_id)
{
    int final_flow_id;

    final_flow_id = flow_id;
    /* If L3 but no MPLS then L3_IIF is swapped between paired ports */
    if ((is_l3_en(unit, flow_id, 0) == 1) &&
        (is_mpls_en(unit, flow_id, 0) == 0)) {
        /* pick the other paired flow */
        final_flow_id = ((flow_id % 2) == 0) ? (flow_id + 1) :
                                               (flow_id - 1);
    }

    return (TH4_PWRT_L3_IIF_BASE + final_flow_id);
}


static uint32_t get_vlan_id(
    int unit,
    int flow_id)
{
    uint32_t phy_dst_port, phy_src_port;
    uint32_t smaller_phy_port;
    phy_src_port = get_phy_src_port(unit, flow_id);
    phy_dst_port = get_phy_dst_port(unit, flow_id);
    smaller_phy_port = (phy_src_port <= phy_dst_port) ? phy_src_port : phy_dst_port;

    if (is_l2_only_en(unit, flow_id, 0) == 1) {
        smaller_phy_port = phy_src_port;
    }

    return (TH4_PWRT_VLAN_ID_BASE + smaller_phy_port);
}

static uint32_t get_pipe_num(int dev_num) {
    return (dev_num / TH4_PWRT_DEV_PORTS_PER_PIPE);
}

static uint32_t get_ing_port_bitmap_profile_ptr(int flow_id)
{
    return (0x1dc + flow_id); /*MAXIDX => 1023*/
}

static uint32_t get_ing_stg(int flow_id){
    return (flow_id % 64); /* STG_TABm MAXIDX => 63*/
}

static uint32_t get_egr_stg(int flow_id){
    return (flow_id % 64); /* STG_TABm MAXIDX => 63*/
}

static uint32_t get_ing_vlan_profile_ptr(int flow_id){
    return (0); /* STG_TABm MAXIDX => 127*/
}



static void get_mac_sa(int unit,
    int flow_id,
    bcm_mac_t mac_addr)
{
    uint32_t phy_dst_port, phy_src_port;
    uint32_t smaller_phy_port;

    phy_src_port = get_phy_src_port(unit, flow_id);
    phy_dst_port = get_phy_dst_port(unit, flow_id);
    smaller_phy_port = (phy_src_port <= phy_dst_port) ? phy_src_port : phy_dst_port;
    /* phy_src_port can be in range 0:271  0x000 - 0x100*/
    pwr_get_mac_base_bcm_mac(unit, smaller_phy_port, mac_addr);
    mac_addr[5] |= (smaller_phy_port & 0xff);
    mac_addr[4] |= ((smaller_phy_port & 0x100) >> 8);
}

static void get_mac_da(int unit,
    int flow_id,
    bcm_mac_t mac_addr)
{
    uint32_t phy_dst_port, phy_src_port;
    uint32_t bigger_phy_port;

    phy_src_port = get_phy_src_port(unit, flow_id);
    phy_dst_port = get_phy_dst_port(unit, flow_id);
    bigger_phy_port = (phy_src_port >= phy_dst_port) ? phy_src_port : phy_dst_port;
    pwr_get_mac_base_bcm_mac(unit, bigger_phy_port, mac_addr);
    /* phy_src_port can be in range 0:271  0x000 - 0x100*/
    mac_addr[4] |= ((bigger_phy_port & 0x100) >> 8);
    mac_addr[5] |= (bigger_phy_port & 0xff);
}

/*
 * Converts MAC from bcm_mac_t to memfld[]
 */
static void convert_mac(bcm_mac_t mac_addr, uint32_t *mem_buf)
{
    mem_buf[1] = (mac_addr[0] << 8) | (mac_addr[1] << 0);
    mem_buf[0] = (mac_addr[2] << 24) | (mac_addr[3] << 16) | (mac_addr[4] << 8) | (mac_addr[5] << 0);
}


static void get_port_bmp_pipe(int unit, int pipe, uint32_t *mem_bmp)
{
    int dev_port, l_dev_port, mem_bmp_indx, mem_bmp_offset;
    power_test_t *power_test_p = power_test_parray[unit];

    for (l_dev_port = 0 ; l_dev_port < (2 * TH4_PWRT_DEV_PORTS_PER_PIPE); l_dev_port++) {
        mem_bmp_indx = l_dev_port / 32;
        mem_bmp_offset = l_dev_port % 32;
        dev_port = l_dev_port + (2 * pipe * TH4_PWRT_DEV_PORTS_PER_PIPE);
        if (power_test_p->dev_enabled_ports[dev_port] == 1) {
            mem_bmp[mem_bmp_indx] |= (0x1 << mem_bmp_offset);
        }
    }
    /* Add CPU */
    if (pipe == 0) {
        mem_bmp[0] |= 0x1;
    }
}


static void get_device_whole_bmp(int unit, uint32_t *mem_bmp)
{
    power_test_t *power_test_p = power_test_parray[unit];
    int dev_port, mem_bmp_indx, mem_bmp_offset;

    /*
     * mem_bmp is an array such that
     * dev ports[31:0] are in mem_bmp[0]
     * dev ports[62:32] are in mem_bmp[1]
     * ...
     * dev ports[159:128] are in mem_bmp[4]
     * assumes that mem_bmp is initialized to zeros before call
     */

    for (dev_port = 0; dev_port < (TH4_PWRT_PIPES_PER_DEV * TH4_PWRT_DEV_PORTS_PER_PIPE); dev_port++) {
        mem_bmp_indx = dev_port / 32;
        mem_bmp_offset = dev_port % 32;
        if (power_test_p->dev_enabled_ports[dev_port] == 1) {
            mem_bmp[mem_bmp_indx] |= (0x1 << mem_bmp_offset);
        }
    }
    /* Add CPU */
    mem_bmp[0] |= 0x1;
}


static void get_mpls_label(
    int unit,
    int flow_id,
    int label_order,
    uint32_t *mpls_label,
    uint32_t *mpls_exp,
    uint32_t *mpls_ttl,
    int use_other_port)
{
    uint32_t phy_dst_port, phy_src_port;
    uint32_t smaller_phy_port;
    uint32_t bigger_phy_port;

    /* most TOP label: label_order = 1
     * middle label  : label_order = 2
     * BOS: bottom label: label_order = 3
     */

    phy_src_port = get_phy_src_port(unit, flow_id);
    phy_dst_port = get_phy_dst_port(unit, flow_id);
    smaller_phy_port = (phy_src_port <= phy_dst_port) ? phy_src_port : phy_dst_port;
    bigger_phy_port = (phy_src_port <= phy_dst_port) ? phy_dst_port : phy_src_port;

    *mpls_label = TH4_PWRT_MPLS_LABEL;
    *mpls_label |= label_order << 8;
    /* We may decide that twin flows to have same MPLs labels
     * mpls_label |= flow_id & 0xfe; 2*n & 2*n+1 flows are twin flows
     */
    if ((use_other_port == 1) &&
        (label_order == 3)) {
        pwr_get_mpls_label_base(unit, label_order, bigger_phy_port, mpls_label);
        *mpls_label |= bigger_phy_port & 0x1ff;
    } else {
        pwr_get_mpls_label_base(unit, label_order, smaller_phy_port, mpls_label);
        *mpls_label |= smaller_phy_port & 0x1ff;
    }

    *mpls_exp = 0x0;
    *mpls_ttl = 0xa5;
}

static uint32_t get_pkt_mpls(
    int unit,
    int flow_id,
    int label_order,
    int use_other_port)
{
    uint32_t mpls_header;
    uint32_t mpls_label;
    uint32_t mpls_exp;
    uint32_t mpls_ttl;
    uint32_t mpls_bos;

    get_mpls_label(unit, flow_id, label_order,
                   &mpls_label, &mpls_exp, &mpls_ttl, use_other_port);
    mpls_bos = (label_order == 3) ? 1 : 0;
    mpls_header = ((mpls_label & 0xfffff) << 12) |
                  ((mpls_exp   & 0x7)     <<  9) |
                  ((mpls_bos   & 0x1)     <<  8) |
                  ((mpls_ttl   & 0xff)    <<  0);

    return mpls_header;
}

static uint32_t get_l3_iif_profile_index(
    int unit,
    int flow_id)
{
    /* 8 bits: MAXBIT => 56, MINBIT => 49, */
    return (0x80 | flow_id);
}

static uint32_t get_vrf(
    int unit,
    int flow_id)
{
    /* 11 bits: MAXBIT => 67, MINBIT => 57, */
    return (TH4_PWRT_VRF_BASE + flow_id);
}

static uint32_t get_my_station_tcam_index(
    int unit,
    int flow_id,
    int is_cpu)
{
    /* 9 bits: MAXIDX => 511 */
    int index;

    index = (is_cpu == 1) ? 0 : (1 + (flow_id / 2));
    /*index = 1;*/
    return index;
}

static uint32_t get_l3_defip_table_index(
    int unit,
    int flow_id)
{
    /* MAXIDX => 2047 */
    return (flow_id);
}


static uint32_t get_ipv4_da(
    int unit,
    int flow_id)
{
    uint32_t ip_addr;
    uint32_t phy_dst_port, phy_src_port;
    uint32_t smaller_phy_port;

    phy_src_port = get_phy_src_port(unit, flow_id);
    phy_dst_port = get_phy_dst_port(unit, flow_id);
    smaller_phy_port = (phy_src_port <= phy_dst_port) ? phy_src_port : phy_dst_port;

    ip_addr = TH4_PWRT_IPV4_DA | smaller_phy_port;

    return ip_addr;
}

static uint32_t get_ipv4_sa(
    int unit,
    int flow_id)
{
    uint32_t ip_addr;
    uint32_t phy_dst_port, phy_src_port;
    uint32_t bigger_phy_port;

    phy_src_port = get_phy_src_port(unit, flow_id);
    phy_dst_port = get_phy_dst_port(unit, flow_id);
    bigger_phy_port = (phy_src_port >= phy_dst_port) ? phy_src_port : phy_dst_port;

    ip_addr = TH4_PWRT_IPV4_DA | bigger_phy_port;

    return ip_addr;
}



static void get_ip_addr_da(
    int unit,
    int flow_id,
    uint32_t *memfld)
{
    uint32_t ip_addr[BCMDRD_MAX_PT_WSIZE];
    uint32_t phy_dst_port, phy_src_port;
    uint32_t smaller_phy_port;

    /*
     * This function assumes ipv6_addr in this packing:
     * ipv6_addr[0]    31:0
     * ipv6_addr[1]   63:32
     * ipv6_addr[2]   95:64
     * ipv6_addr[3]  127:96
     */

    phy_src_port = get_phy_src_port(unit, flow_id);
    phy_dst_port = get_phy_dst_port(unit, flow_id);
    smaller_phy_port = (phy_src_port <= phy_dst_port) ? phy_src_port : phy_dst_port;

    pwr_get_ipv6_base(unit, smaller_phy_port, ip_addr);

    memfld[0] = ip_addr[0] | smaller_phy_port;
    memfld[1] = ip_addr[1];
    memfld[2] = ip_addr[2] | smaller_phy_port;
    memfld[3] = ip_addr[3];

    if (is_l3_ipv4(unit, flow_id, 0) == 1) {
        memfld[0] = get_ipv4_da(unit, flow_id);
    }
}

static void get_ip_addr_sa(
    int unit,
    int flow_id,
    uint32_t *memfld)
{
    uint32_t ip_addr[BCMDRD_MAX_PT_WSIZE];
    uint32_t phy_dst_port, phy_src_port;
    uint32_t bigger_phy_port;

    phy_src_port = get_phy_src_port(unit, flow_id);
    phy_dst_port = get_phy_dst_port(unit, flow_id);
    bigger_phy_port = (phy_src_port >= phy_dst_port) ? phy_src_port : phy_dst_port;

    pwr_get_ipv6_base(unit, bigger_phy_port, ip_addr);

    memfld[0] = ip_addr[0] | bigger_phy_port;
    memfld[1] = ip_addr[1];
    memfld[2] = ip_addr[2] | bigger_phy_port;
    memfld[3] = ip_addr[3];

    if (is_l3_ipv4(unit, flow_id, 0) == 1) {
        memfld[0] = get_ipv4_sa(unit, flow_id);
    }
}

static uint32_t get_ipv6_da(
    int unit,
    int flow_id,
    int slice_no)
{
    uint32_t ip_addr;
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];

    sal_memset(memfld, 0, sizeof(memfld));
    get_ip_addr_da(unit, flow_id, memfld);
    ip_addr = memfld[slice_no];

    return ip_addr;
}

static uint32_t get_next_hop_index(
    int unit,
    int flow_id)
{
    /* 15 bits */
    return (0x7f00 + flow_id);
}


static uint32_t get_cos_map_sel__selectf(
    int unit,
    int flow_id)
{
    /* Select one of four sections of COS_MAP table.*/
    return (flow_id % 4);
}

static uint32_t get_port_cos_map__cosf(
    int unit,
    int flow_id,
    int index)
{
    return 0;
}

static uint32_t get_egr_mac_da_profile_index(
    int unit,
    int flow_id)
{
    return get_dev_src_port(unit, flow_id);
}

static uint32_t get_egr_intf_num(
    int unit,
    int flow_id)
{
    uint32_t dev_src_port;
    dev_src_port = get_dev_src_port(unit, flow_id);

    /* INTF_NUM => {
     *       MAXBIT => 12, MINBIT => 0,
     */
    return (0x1e00 + dev_src_port);
}


static uint32_t get_egr_mpls_tunnel_index(
    int unit,
    int flow_id)
{
    uint32_t dev_src_port;

    /* MAXBIT => 13, MINBIT => 0, */
    dev_src_port = get_dev_src_port(unit, flow_id);
    return (dev_src_port << 3);
}

static uint32_t get_priority(
    int unit,
    int flow_id)
{
    return (0);
}

static int get_port_tab_class_id(
    int unit,
    int log_table)
{
    return 0x4;
}


static int get_udf_index(
    int unit,
    int flow_id)
{
    return (flow_id);
}


static int get_dlb_id_offset(
    int unit)
{
    /* Starting offset of ECMP_GROUP pointer with DLB_ID support */
    return 0xe00;
}

static int get_ecmp_level1_base_ptr(
    int unit,
    int flow_id,
    int is_cpu)
{
    int multiplier;

    multiplier = (is_cpu == 1) ? 0 :
        (flow_id + 1);
    return (0x3000 + multiplier);
}


static int get_ecmp_level1_ecmp_group_ptr(
    int unit,
    int flow_id,
    int is_cpu)
{
    int multiplier;

    multiplier = (is_cpu == 1) ? 0 :
        (flow_id + 1);
    return (0xc00 + multiplier);
}

static int get_ecmp_level1_ecmp_index(
    int unit,
    int flow_id,
    int is_cpu)
{
    return (get_ecmp_level1_base_ptr(unit, flow_id, is_cpu) + 0x0);
}


static uint32_t get_ether_type(
    int unit,
    int flow_id)
{
    uint32_t l_ether_type;
    power_test_t *power_test_p = power_test_parray[unit];

    l_ether_type = 0x8100;
    switch (power_test_p->pkt_flow) {
        /* MPLS unicast */
        case TH4_PWRT_MPLS_IPV6 :
        case TH4_PWRT_MPLS_IPV4 :
            l_ether_type = 0x8847;
            break;
        case TH4_PWRT_IPV6 :
            l_ether_type = 0x86dd;
            break;
        case TH4_PWRT_IPV4 :
            l_ether_type = 0x0800;
            break;
        case TH4_PWRT_L2_ONLY:
            l_ether_type = 0xffff; /* 0x8100 for VLAN tagged*/
            break;
        default :
            l_ether_type = 0x8100;
            break;
    }

    return l_ether_type;
}


static void set_dev_bmp_all_one(
    uint32_t *memfld)
{
    int i;

    /* TH4 has 272 ports */
    for (i = 0; i < 9; i++) {
        memfld[i] = (i == 8 ) ? 0x0000ffff : 0xffffffff;
    }
}


static int get_pp_map(
    int unit,
    uint32_t *pp_map)
{
    int pp_num, pipe;
    uint32_t pipe_map;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));

    *pp_map = 0;
    for (pipe = 0; pipe < TH4_PWRT_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            pp_num = pipe / 4;
            *pp_map |= (1 << pp_num);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static void set_rand_mem(
    int mem_width,
    uint32_t *memfld)
{
    int i, num_words, last_word_width;
    uint32_t word_mask;

    num_words = (mem_width + 31) / 32;
    last_word_width = (mem_width % 32);
    last_word_width = (last_word_width == 0) ? 32 : last_word_width;
    for (i = 0; i < num_words; i++) {
        word_mask = (i < (num_words - 1)) ?
                    0xffffffff :
                    (0xffffffff >> (32 - last_word_width));
        memfld[i] = sal_rand32() & word_mask;
    }
}


static int
th4_pwrt_field_set(int unit,
                   bcmdrd_sid_t sid,
                   bcmdrd_fid_t fid,
                   uint32_t *entbuf,
                   uint32_t *fbuf)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmdrd_pt_field_set(unit, sid, entbuf, fid, fbuf);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}


static int th4_pwrt_pt_write(int unit,
                           bcmdrd_sid_t sid,
                           bcmltd_sid_t lt_id,
                           void *pt_dyn_info,
                           uint32_t *ltmbuf)
{
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags;
    uint64_t in_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_write(unit, in_flags,
                               sid, pt_dyn_info,
                               NULL, ltmbuf, lt_id, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}


static int th4_pwrt_pt_insert(int unit,
                           bcmdrd_sid_t sid,
                           uint32_t *ltmbuf)
{
    int rv = SHR_E_NONE;
    bcmbd_pt_dyn_hash_info_t dyn_hash_info;
    bcmbd_tbl_resp_info_t resp_info;
    int wsize;

    SHR_FUNC_ENTER(unit);

    dyn_hash_info.bank = 0;
    dyn_hash_info.tbl_inst = 0;
    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    rv = bcmbd_pt_insert(unit, sid, &dyn_hash_info, NULL,
                                    ltmbuf, wsize, &resp_info, NULL);
    if (SHR_SUCCESS(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                "th4_pwrt_pt_insert() success for %s resp_info=0x%x\n"),
                bcmdrd_pt_sid_to_name(unit, sid), resp_info.resp_word));
    } else {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
            "th4_pwrt_pt_insert() %s could not be inserted\n"),
            bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}


static int power_test_set_key_attributes(int unit)
{
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = -1;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;

    SHR_FUNC_ENTER(unit);

    sid = L2_ENTRY_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x0);    /* L2_ENTRY_KEY_ATTRIBUTES__KEY_BASE_WIDTH_L2 */
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x0);   /* L2_ENTRY_KEY_ATTRIBUTES__DATA_BASE_WIDTH_L2 */
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0xf);         /* L2_ENTRY_KEY_ATTRIBUTES__KEY_WIDTH_L2 */
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);       /* L2_ENTRY_KEY_ATTRIBUTES__BUCKET_MODE_L2 */
    index = 0; /* L2_ENTRY_SINGLE__KEY_TYPE_BRIDGE */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    sid = L2_ENTRY_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x0);    /* L2_ENTRY_KEY_ATTRIBUTES__KEY_BASE_WIDTH_VLAN */
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x0);   /* L2_ENTRY_KEY_ATTRIBUTES__DATA_BASE_WIDTH_VLAN */
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0x6);         /* L2_ENTRY_KEY_ATTRIBUTES__KEY_WIDTH_VLAN */
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);       /* L2_ENTRY_KEY_ATTRIBUTES__BUCKET_MODE_VLAN */
    index = 1; /* L2_ENTRY_SINGLE__KEY_TYPE_SINGLE_CROSS_CONNECT */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    sid = L3_TUNNEL_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0x8);
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);
    index = 0; /* L3_TUNNEL_DOUBLE__KEY_TYPE_MPLS */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = L3_TUNNEL_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0x12);
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);
    index = 1; /* L3_TUNNEL_KEY_ATTRIBUTES__KEY_WIDTH_TERM_IPV4_1 */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = L3_TUNNEL_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x3);
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0x8);
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);
    index = 2; /* L3_TUNNEL_DOUBLE__KEY_TYPE_IPV6 */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = MPLS_ENTRY_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0x8);
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);
    index = 0; /* MPLS_ENTRY_SINGLE__KEY_TYPE_MPLS */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = EXACT_MATCH_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x1);
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0x3);
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);
    index = 0; /* EXACT_MATCH_ONLY_DOUBLE__KEY_TYPE_MODE128 */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = EXACT_MATCH_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x1);
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0xb);
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);
    index = 1; /* EXACT_MATCH_ONLY_DOUBLE__KEY_TYPE_MODE160 */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = EXACT_MATCH_KEY_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_LSB_OFFSETf, 0x0);
    TH4_PWRT_SET_FIELD(KEY_BASE_WIDTHf, 0x2);
    TH4_PWRT_SET_FIELD(DATA_BASE_WIDTHf, 0x1);
    TH4_PWRT_SET_FIELD(KEY_WIDTHf, 0x15);
    TH4_PWRT_SET_FIELD(BUCKET_MODEf, 0x0);
    index = 2; /* EXACT_MATCH_ONLY_QUAD__KEY_TYPE_MODE320 */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


static int power_test_set_hash_control(int unit)
{
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = -1;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;
    int robust_hash_en;

    SHR_FUNC_ENTER(unit);

    robust_hash_en = 1;

    sid = L2_ENTRY_HASH_CONTROLm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_TABLE_BANK_CONFIGf, 0x3);
    TH4_PWRT_SET_FIELD(ROBUST_HASH_ENf, robust_hash_en);
    TH4_PWRT_SET_FIELD(ROBUST_HASH_INSTANCE_SELECTf, 0x0);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_0f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_1f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_TABLE_TEST_MODEf, 0x0);
    index = 0;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = L3_TUNNEL_HASH_CONTROLm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_TABLE_BANK_CONFIGf, 0xf);
    TH4_PWRT_SET_FIELD(ROBUST_HASH_ENf, robust_hash_en);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_0f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_1f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_2f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_3f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_TABLE_TEST_MODEf, 0x0);
    index = 0;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = MPLS_ENTRY_HASH_CONTROLm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_TABLE_BANK_CONFIGf, 0xf);
    TH4_PWRT_SET_FIELD(ROBUST_HASH_ENf, robust_hash_en);
    TH4_PWRT_SET_FIELD(ROBUST_HASH_INSTANCE_SELECTf, 0x0);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_0f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_1f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_2f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_OFFSET_DEDICATED_BANK_3f, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(HASH_TABLE_TEST_MODEf, 0x0);
    index = 0;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = EXACT_MATCH_HASH_CONTROLm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_TABLE_BANK_CONFIGf, 0xf);
    TH4_PWRT_SET_FIELD(ROBUST_HASH_ENf, robust_hash_en);
    TH4_PWRT_SET_FIELD(LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf, 0x0);
    TH4_PWRT_SET_FIELD(LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf, 0x1);
    TH4_PWRT_SET_FIELD(LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf, 0x2);
    TH4_PWRT_SET_FIELD(LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf, 0x3);
    TH4_PWRT_SET_FIELD(HASH_TABLE_TEST_MODEf, 0x0);
    index = 0;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = UFT_SHARED_BANKS_CONTROLm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(B7_HASH_OFFSETf, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(B6_HASH_OFFSETf, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(B5_HASH_OFFSETf, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(B4_HASH_OFFSETf, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(B3_HASH_OFFSETf, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(B2_HASH_OFFSETf, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(B1_HASH_OFFSETf, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(B0_HASH_OFFSETf, sal_rand() % 48);
    TH4_PWRT_SET_FIELD(DISABLE_XOR_WRITEf, 0x0);
    TH4_PWRT_SET_FIELD(FORCE_XOR_GENf, 0x0);
    index = 0;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


static int power_test_set_hash_remap_and_action(int unit)
{
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = -1;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int remap_table_size, action_table_size;
    int i, tab;

    static const bcmdrd_sid_t remap_tables[8] = {
        L2_ENTRY_REMAP_TABLE_Am,    L2_ENTRY_REMAP_TABLE_Bm,
        L3_TUNNEL_REMAP_TABLE_Am,   L3_TUNNEL_REMAP_TABLE_Bm,
        MPLS_ENTRY_REMAP_TABLE_Am,  MPLS_ENTRY_REMAP_TABLE_Bm,
        EXACT_MATCH_REMAP_TABLE_Am, EXACT_MATCH_REMAP_TABLE_Bm
    };

    static const bcmdrd_sid_t action_tables[8] = {
        L2_ENTRY_ACTION_TABLE_Am,    L2_ENTRY_ACTION_TABLE_Bm,
        L3_TUNNEL_ACTION_TABLE_Am,   L3_TUNNEL_ACTION_TABLE_Bm,
        MPLS_ENTRY_ACTION_TABLE_Am,  MPLS_ENTRY_ACTION_TABLE_Bm,
        EXACT_MATCH_ACTION_TABLE_Am, EXACT_MATCH_ACTION_TABLE_Bm
    };

    SHR_FUNC_ENTER(unit);

    remap_table_size = 256;
    for (tab = 0; tab < 8; tab++) {
        sid = remap_tables[tab];
        for (i = 0; i < remap_table_size; i++) {
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(REMAP_DATAf, sal_rand() & 0xfff);
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, i, 0);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    action_table_size = 4;
    for (tab = 0; tab < 8; tab++) {
        sid = action_tables[tab];
        for (i = 0; i < action_table_size; i++) {
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = ((sal_rand() % 0xff) << 24) | ((sal_rand() % 0xff) << 16) |
                        ((sal_rand() % 0xff) <<  8) | ((sal_rand() % 0xff) << 0);
            memfld[1] = ((sal_rand() % 0xff) << 24) | ((sal_rand() % 0xff) << 16) |
                        ((sal_rand() % 0xff) <<  8) | ((sal_rand() % 0xff) << 0);
            SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, ACTIONf, entry, memfld));
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, i, 0);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int setup_ing_ctrl_regs(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, port;

    SHR_FUNC_ENTER(unit);

    sid = ING_PROC_CONTROLr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(UC_DISABLE_SIP_LOOKUPf, 0);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = HASH_CONTROLr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    /* USE_16_MOD_10 - HASH_CONTROL__ECMP_HASH_FIELD_UPPER_BITS_COUNT_RESETVALUE*/
    TH4_PWRT_SET_FIELD(ECMP_HASH_FIELD_UPPER_BITS_COUNTf, 6);
    TH4_PWRT_SET_FIELD(ECMP_HASH_SELf, 3);
    TH4_PWRT_SET_FIELD(ECMP_HASH_USE_RTAG7f, 1);
    TH4_PWRT_SET_FIELD(NON_UC_TRUNK_HASH_SRC_ENABLEf, 1);
    TH4_PWRT_SET_FIELD(USE_TCP_UDP_PORTSf, 1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = CPU_CONTROL_0r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(UVLAN_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(PVLAN_VID_MISMATCH_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(PARITY_ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(MACSA_ALL_ZERO_DROPf, 1);
    TH4_PWRT_SET_FIELD(DOSATTACK_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(BFD_YOUR_DISCRIMINATOR_NOT_FOUND_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(BFD_UNKNOWN_VERSION_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(BFD_UNKNOWN_CONTROL_PACKET_TOCPUf, 1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = CPU_CONTROL_1r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(VXLT_MISS_TOCPUf, 0);
    TH4_PWRT_SET_FIELD(NONSTATICMOVE_TOCPUf, 0);
    TH4_PWRT_SET_FIELD(V6L3ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(V6L3ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(V6L3DSTMISS_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(V4L3ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(V4L3DSTMISS_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(UUCAST_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(UMC_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(TUNNEL_ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(STATICMOVE_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(SRCROUTE_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(NIP_L3ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(MC_INDEX_ERROR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(MARTIAN_ADDR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(L3_SLOWPATH_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(L3_MTU_FAIL_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(L3UC_TTL_ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(L3UC_TTL1_ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(IPMC_TTL_ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(IPMC_TTL1_ERR_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(IPMCPORTMISS_TOCPUf, 1);
    TH4_PWRT_SET_FIELD(IPMCERR_TOCPUf, 1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = LPM_LANE_CTRLr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(L3_DST_ENABLEf, 1);
    /*TH4_PWRT_SET_FIELD(ENABLEf, 0xf);*/
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ECMP_GROUP_DLB_ID_OFFSETr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(DLB_ID_OFFSETf, get_dlb_id_offset(unit));
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ING_HASH_CONFIG_0r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_HASHINGf, 0x1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    /* RTAG7 */

    /* RTAG7_HASH_CONTROL */
    /* Keep default values - all zeros */

    /* RTAG7_HASH_CONTROL_2 */
    sid = RTAG7_HASH_CONTROL_2r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ENABLE_BIN_12_OVERLAY_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_BIN_12_OVERLAY_Bf, 0x1);
    TH4_PWRT_SET_FIELD(MACRO_FLOW_HASH_BYTE_SELf, 0);
    TH4_PWRT_SET_FIELD(MACRO_FLOW_HASH_FUNC_SELf, 3 + (sal_rand() % 13));
    TH4_PWRT_SET_FIELD(USE_MPLS_STACK_FOR_HASHINGf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLOW_LABEL_IPV6_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLOW_LABEL_IPV6_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_GTP_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_GTP_Bf, 0x1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_HASH_CONTROL_3_64 */
    sid = RTAG7_HASH_CONTROL_3_64r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_10_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_10_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_11_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_11_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_12_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_12_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_13_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_13_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_1_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_1_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_2_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_2_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_3_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_3_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_4_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_4_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_5_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_5_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_6_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_6_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_7_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_7_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_8_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_8_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_9_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_FLEX_FIELD_9_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_UDF_FIELD_1_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_UDF_FIELD_1_Bf, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_UDF_FIELD_2_Af, 0x1);
    TH4_PWRT_SET_FIELD(ENABLE_UDF_FIELD_2_Bf, 0x1);
    TH4_PWRT_SET_FIELD(HASH_A0_FUNCTION_SELECTf, 3 + (sal_rand() % 13));
    TH4_PWRT_SET_FIELD(HASH_A1_FUNCTION_SELECTf, 3 + (sal_rand() % 13));
    TH4_PWRT_SET_FIELD(HASH_B0_FUNCTION_SELECTf, 3 + (sal_rand() % 13));
    TH4_PWRT_SET_FIELD(HASH_B1_FUNCTION_SELECTf, 3 + (sal_rand() % 13));
    TH4_PWRT_SET_FIELD(HASH_PRE_PROCESSING_ENABLE_Af, 0x0);
    TH4_PWRT_SET_FIELD(HASH_PRE_PROCESSING_ENABLE_Bf, 0x0);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_HASH_CONTROL_4 */
    /* Keep default values - VXLAN related */

    /* RTAG7_HASH_FIELD_BMAP_1 */
    sid = RTAG7_HASH_FIELD_BMAP_1r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(IPV4_FIELD_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(IPV4_FIELD_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_HASH_FIELD_BMAP_2 */
    sid = RTAG7_HASH_FIELD_BMAP_2r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(IPV6_FIELD_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(IPV6_FIELD_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_HASH_FIELD_BMAP_3 */
    sid = RTAG7_HASH_FIELD_BMAP_3r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(L2_FIELD_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(L2_FIELD_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_HASH_SEED_A */
    sid = RTAG7_HASH_SEED_Ar;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_SEED_Af, sal_rand32());
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_HASH_SEED_B */
    sid = RTAG7_HASH_SEED_Br;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(HASH_SEED_Bf, sal_rand32());
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_HASH_SEL */
    /* flw based vs port based selection - use port based: RTAG7_PORT_BASED_HASH */
    sid = RTAG7_HASH_SELr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_PORT_BASED_HASH - randomize */
    sid = RTAG7_PORT_BASED_HASHm;
    for (port = 0; port < TH4_PWRT_DEV_PORTS_PER_DEV; port++) {
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        set_rand_mem(100, entry);
        index = port;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

    /* RTAG7_HASH_VH_INITIALIZE */
    sid = RTAG7_HASH_VH_INITIALIZEr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(VH_INIT_VAL_0f, sal_rand32());
    TH4_PWRT_SET_FIELD(VH_INIT_VAL_1f, sal_rand32());
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_1 */
    sid = RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_1r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(IPV4_TCP_UDP_SRC_EQ_DST_FIELD_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(IPV4_TCP_UDP_SRC_EQ_DST_FIELD_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2 */
    sid = RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(IPV4_TCP_UDP_FIELD_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(IPV4_TCP_UDP_FIELD_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_1 */
    sid = RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_1r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(IPV6_TCP_UDP_SRC_EQ_DST_FIELD_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(IPV6_TCP_UDP_SRC_EQ_DST_FIELD_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_2 */
    sid = RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_2r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(IPV6_TCP_UDP_FIELD_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(IPV6_TCP_UDP_FIELD_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAP */
    sid = RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(MPLS_L3_PAYLOAD_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(MPLS_L3_PAYLOAD_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_MPLS_OUTER_HASH_FIELD_BMAP */
    sid = RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(MPLS_OUTER_BITMAP_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(MPLS_OUTER_BITMAP_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* RTAG7_MPLS_OUTER_HASH_FIELD_BMAP_EXTENDED */
    sid = RTAG7_MPLS_OUTER_HASH_FIELD_BMAP_EXTENDEDr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(MPLS_OUTER_BITMAP_EXTENDED_Af, 0x1fff);
    TH4_PWRT_SET_FIELD(MPLS_OUTER_BITMAP_EXTENDED_Bf, 0x1fff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    /* VFP_HASH_FIELD_BMAP_TABLE_A - all ones
     * VFP_HASH_FIELD_BMAP_TABLE_B
     */
    for (index = 0; index < 16; index++) {
        sid = VFP_HASH_FIELD_BMAP_TABLE_Am;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(BITMAPf, 0x1fff);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

        sid = VFP_HASH_FIELD_BMAP_TABLE_Bm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(BITMAPf, 0x1fff);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures ctrl tables:
 * LPM_IP_CONTROLm
 * EPC_LINK_BMAPm
 * ING_EN_EFILTER_BITMAPm
 * INSTRUMENTATION_TRIGGERS_ENABLEm
 */
static int setup_ing_ctrl_tables(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int alpm_level;
    bcmlt_entry_handle_t lt_entry = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    alpm_level = get_alpm_level(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LPM_IP_CONTROLm", &lt_entry));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DATA0_SEL", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DATA1_SEL", 4));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DATA2_SEL", 4));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DATA3_SEL", 4));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DB0_MODE", alpm_level));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DB1_MODE", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DB2_MODE", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DB3_MODE", 0));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_DATABASE_SEL_BLOCK_0", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_DATABASE_SEL_BLOCK_1", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_DATABASE_SEL_BLOCK_2", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_DATABASE_SEL_BLOCK_3", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_DATABASE_SEL_BLOCK_4", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_DATABASE_SEL_BLOCK_5", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_DATABASE_SEL_BLOCK_6", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_DATABASE_SEL_BLOCK_7", 0));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_KEY_INPUT_SEL_BLOCK_0", 8));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_KEY_INPUT_SEL_BLOCK_1", 1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_KEY_INPUT_SEL_BLOCK_2", 1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_KEY_INPUT_SEL_BLOCK_3", 1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_KEY_INPUT_SEL_BLOCK_4", 1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_KEY_INPUT_SEL_BLOCK_5", 1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_KEY_INPUT_SEL_BLOCK_6", 1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL1_KEY_INPUT_SEL_BLOCK_7", 1));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_BANK_CONFIG_BLOCK_0", 0x3));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_BANK_CONFIG_BLOCK_1", 0x4));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_BANK_CONFIG_BLOCK_2", 0x18));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_BANK_CONFIG_BLOCK_3", 0x20));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_DATABASE_SEL_BLOCK_0", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_DATABASE_SEL_BLOCK_1", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_DATABASE_SEL_BLOCK_2", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_DATABASE_SEL_BLOCK_3", 0));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_KEY_INPUT_SEL_BLOCK_0", 8));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_KEY_INPUT_SEL_BLOCK_1", 8));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_KEY_INPUT_SEL_BLOCK_2", 8));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL2_KEY_INPUT_SEL_BLOCK_3", 8));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_BANK_CONFIG_BLOCK_0", 0x3));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_BANK_CONFIG_BLOCK_1", 0xc));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_BANK_CONFIG_BLOCK_2", 0x30));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_BANK_CONFIG_BLOCK_3", 0xc0));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_DATABASE_SEL_BLOCK_0", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_DATABASE_SEL_BLOCK_1", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_DATABASE_SEL_BLOCK_2", 0));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_DATABASE_SEL_BLOCK_3", 0));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_KEY_INPUT_SEL_BLOCK_0", 8));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_KEY_INPUT_SEL_BLOCK_1", 8));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_KEY_INPUT_SEL_BLOCK_2", 8));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LEVEL3_KEY_INPUT_SEL_BLOCK_3", 8));


    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(lt_entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(lt_entry);

    sid = EPC_LINK_BMAPm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    /* TH4: all ones */
    set_dev_bmp_all_one(memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAPf, entry, memfld));
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    sid = ING_EN_EFILTER_BITMAPm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    /* TH4: all ones */
    set_dev_bmp_all_one(memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, BITMAPf, entry, memfld));
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    sid = INSTRUMENTATION_TRIGGERS_ENABLEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    /* TH4: all ones'  "Per ingress port instrumentation triggers enable control bitmap.",  */
    set_dev_bmp_all_one(memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, BITMAPf, entry, memfld));
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures flow independent tables:
 * STG_TABm  all indexes from 0 to 63: all ones
 * VLAN_PROFILE_2m:: index=0 only
 * VLAN_PROFILE_TABm:: index=0 only
 */
static int setup_ing_nonflow_tables(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;

    SHR_FUNC_ENTER(unit);

    sid = STG_TAB_Am;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xffffffff;
    memfld[1] = 0xffffffff;
    memfld[2] = 0x0000000f;
     /* SPLIT_DATA_P0 has 68 bits; 2 bist per port */
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SPLIT_DATA_P0f, entry, memfld));
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SPLIT_DATA_P1f, entry, memfld));
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SPLIT_DATA_P2f, entry, memfld));
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SPLIT_DATA_P3f, entry, memfld));
    /* all indexes from 0 to 63: all ones */
    for (index = 0; index < 64; index++) {
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = STG_TAB_Bm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xffffffff;
    memfld[1] = 0xffffffff;
    memfld[2] = 0x0000000f;
     /* SPLIT_DATA_P4 has 68 bits; 2 bist per port */
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SPLIT_DATA_P4f, entry, memfld));
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SPLIT_DATA_P5f, entry, memfld));
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SPLIT_DATA_P6f, entry, memfld));
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SPLIT_DATA_P7f, entry, memfld));
    /* all indexes from 0 to 63: all ones */
    for (index = 0; index < 64; index++) {
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = VLAN_PROFILE_TABm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(OUTER_TPID_INDEXf, 0x0);
    TH4_PWRT_SET_FIELD(L2_PFMf, 0x1);
    TH4_PWRT_SET_FIELD(L3_IPV4_PFMf, 0x1);
    TH4_PWRT_SET_FIELD(L3_IPV6_PFMf, 0x1);
    /* (configure just one entry = 0) */
    index  = get_ing_vlan_profile_ptr(0);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = VLAN_PROFILE_2m;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    /* all DATA =0; MAXBIT => 327, MINBIT => 0*/
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, DATAf, entry, memfld));
    /* (configure just one entry = 0) */
    index  = get_ing_vlan_profile_ptr(0);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* Dummy writes to indx 0 */
    sid = L3_IIFm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(IPV6L3_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(IPV4L3_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(IPMCV6_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(IPMCV4_ENABLEf, 0x1);
    index  = 0;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = L3_IIF_PROFILEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ALLOW_GLOBAL_ROUTEf, 0x1);
    index  = 0;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    for (index = 0; index < 8; index++) {
        sid = UDF_CONDITIONAL_CHECK_TABLE_CAMm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(VALIDf, 0x1);
        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = ((index & 0x7) << 0) | (sal_rand32() << 3);
        memfld[1] = sal_rand32();
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, KEYf, entry, memfld));
        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = 0x7;
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, MASKf, entry, memfld));
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

        sid = UDF_CONDITIONAL_CHECK_TABLE_RAMm;
        sal_memset(entry, 0, sizeof(entry));
        set_rand_mem(65, entry);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

        sid = UDF_CONDITIONAL_CHECK_TABLE_RAM_1m;
        sal_memset(entry, 0, sizeof(entry));
        set_rand_mem(208, entry);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * PORT_TABm
 * SOURCE_TRUNK_MAP_TABLEm
 * LPORT_TABm
 */
static int setup_ing_port_tabs(int unit,
    int flow_id, int is_cpu)
{
    power_test_t *power_test_p = power_test_parray[unit];
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int src_dev_port, pipe, index, pp_acc_type;
    uint32_t vlan_id, lport_profile_index;
    uint32_t table_id;
    uint32_t tag_action_profile_ptr;
    uint32_t enable;

    SHR_FUNC_ENTER(unit);

    if (is_cpu == 0) {
        src_dev_port = get_dev_src_port(unit, flow_id);
        vlan_id = get_vlan_id(unit, flow_id);
        lport_profile_index = get_ip_lport_profile_index(unit, flow_id);
        enable = 0x1;
    } else {
        src_dev_port = 0;
        vlan_id = TH4_PWRT_VLAN_ID_BASE;
        lport_profile_index = 0;
        enable = 0x0;
    }

    /* If L2 flow from front panel */
    if ((is_cpu == 0) &&
        (is_l2_only_en(unit, flow_id, 0) == 1)) {
        tag_action_profile_ptr = 1;
    } else {
        tag_action_profile_ptr = 0;
    }

    /* PORT_TABm */
    sid = PORT_TABm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(CML_FLAGS_MOVEf, 0x8);
    TH4_PWRT_SET_FIELD(CML_FLAGS_NEWf, 0x8);
    TH4_PWRT_SET_FIELD(EN_IFILTERf, 0x1);
    TH4_PWRT_SET_FIELD(FILTER_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(IPMC_DO_VLANf, enable);
    TH4_PWRT_SET_FIELD(MPLS_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(OUTER_TPID_ENABLEf, 0x1);
    /* Normal operation if L3 & not MPLS & CPU */
    /* In L3_ONLY flows from CPU, L3_IIF from VLAN_MPLS table is used */
    if ((is_l3_en(unit, flow_id, 0) == 1)   &&
        (is_mpls_en(unit, flow_id, 0) == 0) &&
        (is_cpu == 1) ) {
        TH4_PWRT_SET_FIELD(PORT_OPERATIONf, 0x0); /* Normal */
    } else if (is_l2_only_en(unit, flow_id, is_cpu) == 1) {
        TH4_PWRT_SET_FIELD(PORT_OPERATIONf, 0x0); /* Normal */
    } else {
        TH4_PWRT_SET_FIELD(PORT_OPERATIONf, (enable == 0x1) ? 0x2 : 0x0); /* l3_iif */
    }
    TH4_PWRT_SET_FIELD(ALLOW_NON_GSHf, 0x1);
    TH4_PWRT_SET_FIELD(PORT_TYPEf, 0x0); /* ETHERNET */
    TH4_PWRT_SET_FIELD(PORT_VIDf, vlan_id);
    TH4_PWRT_SET_FIELD(RTAG7_PORT_LBNf, 0x5); /* used for hash in ECMP */
    TH4_PWRT_SET_FIELD(TAG_ACTION_PROFILE_PTRf, tag_action_profile_ptr);
    TH4_PWRT_SET_FIELD(V4IPMC_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(V4L3_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(V6IPMC_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(V6L3_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(VFP_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(VFP_PORT_GROUP_IDf, 0x0);
    table_id = 0x1;
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTIONf, table_id);
    TH4_PWRT_SET_FIELD(IFP_KEY_SEL_CLASS_IDf, get_port_tab_class_id(unit, 0));

    if (((power_test_p->enable_map >> TH4_PWRT_EN_RND_PRI) & 0x1) == 0x1) {
        TH4_PWRT_SET_FIELD(TRUST_DOT1P_PTRf, flow_id & 0x3f);
        TH4_PWRT_SET_FIELD(DOT1P_REMAP_POINTERf, flow_id & 0x3f);
    }

    index  = src_dev_port;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    /* SOURCE_TRUNK_MAP_TABLEm */
    sid = SOURCE_TRUNK_MAP_TABLEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(L3_IIFf, get_l3_iif(unit, flow_id));
    TH4_PWRT_SET_FIELD(LPORT_PROFILE_IDXf, lport_profile_index);
    index  = src_dev_port;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* LPORT_PROFILE_TABLE */
    sid = LPORT_TABm;
    pipe = get_pipe_num(src_dev_port);
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(CML_FLAGS_MOVEf, 0x8);
    TH4_PWRT_SET_FIELD(CML_FLAGS_NEWf, 0x8);
    TH4_PWRT_SET_FIELD(EN_IFILTERf, 0x1);
    TH4_PWRT_SET_FIELD(FILTER_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(IPMC_DO_VLANf, enable);
    TH4_PWRT_SET_FIELD(MPLS_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(OUTER_TPID_ENABLEf, 0x1);
    if ((is_l3_en(unit, flow_id, 0) == 1)   &&
        (is_mpls_en(unit, flow_id, 0) == 0) &&
        (is_cpu == 1) ) {
        TH4_PWRT_SET_FIELD(PORT_OPERATIONf, 0x0); /*Normal */
    } else if (is_l2_only_en(unit, flow_id, is_cpu) == 1) {
        TH4_PWRT_SET_FIELD(PORT_OPERATIONf, 0x0); /* Normal */
    } else {
        TH4_PWRT_SET_FIELD(PORT_OPERATIONf, (enable == 0x1) ? 0x2 : 0x0); /* l3_iif */
    }
    TH4_PWRT_SET_FIELD(PORT_TYPEf, 0x0);      /* ETHERNET */
    TH4_PWRT_SET_FIELD(PORT_VIDf, vlan_id);
    TH4_PWRT_SET_FIELD(TAG_ACTION_PROFILE_PTRf, tag_action_profile_ptr);
    TH4_PWRT_SET_FIELD(V4IPMC_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(V4L3_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(V6IPMC_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(V6L3_ENABLEf, enable);
    TH4_PWRT_SET_FIELD(VFP_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(VFP_PORT_GROUP_IDf, 0x0);

    if (((power_test_p->enable_map >> TH4_PWRT_EN_RND_PRI) & 0x1) == 0x1) {
        TH4_PWRT_SET_FIELD(TRUST_DOT1P_PTRf, flow_id & 0x3f);
        TH4_PWRT_SET_FIELD(DOT1P_REMAP_POINTERf, flow_id & 0x3f);
    }

    index = lport_profile_index;
    pp_acc_type = pipe / 4;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, pp_acc_type);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* If L2 flow from front panel */
    if ((is_cpu == 0) &&
        (is_l2_only_en(unit, flow_id, 0) == 1)) {
        sid = ING_VLAN_TAG_ACTION_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(SOT_OTAG_ACTIONf, 0x2);
        index = tag_action_profile_ptr;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * VLAN_TABm
 * VLAN_2_TABm
 * VLAN_PROFILE_TABm
 * VLAN_PROFILE_2m
 */
static int setup_ing_vlan_tabs(int unit,
    int flow_id,
    int is_cpu)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;
    uint32_t vlan_id;
    uint32_t table_id;

    SHR_FUNC_ENTER(unit);

    if (is_cpu == 0) {
        vlan_id = get_vlan_id(unit, flow_id);
    } else {
        vlan_id = TH4_PWRT_VLAN_ID_BASE;
    }

    sid = VLAN_TABm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));

    TH4_PWRT_SET_FIELD(VALIDf, 0x1);
    TH4_PWRT_SET_FIELD(L2_ENTRY_KEY_TYPEf, 0x0); /* bridging */
    TH4_PWRT_SET_FIELD(VLAN_PROFILE_PTRf, get_ing_vlan_profile_ptr(flow_id));
    table_id = 2;
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTIONf, table_id);
    index = vlan_id;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = VLAN_2_TABm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    /* Profile pointer to egress port bitmap table. */
    TH4_PWRT_SET_FIELD(PORT_BITMAP_PROFILE_PTRf, get_ing_port_bitmap_profile_ptr(flow_id));
    /* Spanning Tree Group ID. */
    TH4_PWRT_SET_FIELD(STGf, get_ing_stg(flow_id));
    /* VLAN Ingress port membership bitmap : all ones  */
    sal_memset(memfld, 0, sizeof(memfld));
    get_device_whole_bmp(unit, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, ING_PORT_BITMAPf, entry, memfld));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(VALIDf, 0x1);
    index = vlan_id;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    sid = PORT_BITMAP_PROFILEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    get_device_whole_bmp(unit, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, BITMAPf, entry, memfld));
    index = get_ing_port_bitmap_profile_ptr(flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = VLAN_MPLSm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    if (is_mpls_en(unit, flow_id, is_cpu) == 1) {
        TH4_PWRT_SET_FIELD(MPLS_ENABLEf, 0x1);
    } else {
        TH4_PWRT_SET_FIELD(MPLS_ENABLEf, 0x0);
    }
    TH4_PWRT_SET_FIELD(L3_IIFf, get_l3_iif(unit, flow_id));
    index = vlan_id;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = L3_MTU_VALUESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(MTU_SIZEf, 0x3fff);
    index = vlan_id;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}



/*
 * This function configures:
 * L2_ENTRY_ONLY_SINGLEm
 * One entry per flow that has:
 * MAC = source port MAC
 * VLAN = flow vlan
 * port = src_port
 */
static int setup_ing_l2_entry(int unit,
    int flow_id,
    int is_cpu)
{
    bcmdrd_sid_t sid;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    bcm_mac_t mac_addr;
    uint32_t vlan_id;

    SHR_FUNC_ENTER(unit);

    vlan_id = (is_cpu == 1) ? TH4_PWRT_VLAN_ID_BASE :
        get_vlan_id(unit, flow_id);

    /* Setup (MAC_SA, VLAN_ID) entry */
    sid = L2_ENTRY_SINGLEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BASE_VALIDf, 0x1); /* START_OF_KEY_BM_0 */
    TH4_PWRT_SET_FIELD(KEY_TYPEf, 0x0);   /* BRIDGE Key = {OVID, MAC} */
    if (is_l2_only_en(unit, flow_id, 0) == 0) {
        TH4_PWRT_SET_FIELD(L2v_PORT_NUMf, get_dev_src_port(unit, flow_id));
        TH4_PWRT_SET_FIELD(L2v_VLAN_IDf, vlan_id);
        sal_memset(memfld, 0, sizeof(memfld));
        get_mac_sa(unit, flow_id, mac_addr);
        convert_mac(mac_addr, memfld);
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, L2v_MAC_ADDRf, entry, memfld));
    } else {
        TH4_PWRT_SET_FIELD(L2v_PORT_NUMf, get_dev_dst_port(unit, flow_id));
        TH4_PWRT_SET_FIELD(L2v_VLAN_IDf, vlan_id);
        sal_memset(memfld, 0, sizeof(memfld));
        get_mac_da(unit, flow_id, mac_addr);
        convert_mac(mac_addr, memfld);
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, L2v_MAC_ADDRf, entry, memfld));
    }
    SHR_IF_ERR_EXIT(th4_pwrt_pt_insert(unit, sid, entry));

    /* Setup (MAC_DA, VLAN_ID) entry */
    sid = L2_ENTRY_SINGLEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BASE_VALIDf, 0x1); /* START_OF_KEY_BM_0 */
    TH4_PWRT_SET_FIELD(KEY_TYPEf, 0x0);   /* BRIDGE Key = {OVID, MAC} */
    if (is_l2_only_en(unit, flow_id, 0) == 0) {
        TH4_PWRT_SET_FIELD(L2v_PORT_NUMf, get_dev_src_port(unit, flow_id));
        TH4_PWRT_SET_FIELD(L2v_VLAN_IDf, vlan_id);
        sal_memset(memfld, 0, sizeof(memfld));
        get_mac_da(unit, flow_id, mac_addr);
        convert_mac(mac_addr, memfld);
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, L2v_MAC_ADDRf, entry, memfld));
    } else {
        TH4_PWRT_SET_FIELD(L2v_PORT_NUMf, get_dev_dst_port(unit, flow_id));
        TH4_PWRT_SET_FIELD(L2v_VLAN_IDf, vlan_id);
        sal_memset(memfld, 0, sizeof(memfld));
        get_mac_sa(unit, flow_id, mac_addr);
        convert_mac(mac_addr, memfld);
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, L2v_MAC_ADDRf, entry, memfld));
    }
    SHR_IF_ERR_EXIT(th4_pwrt_pt_insert(unit, sid, entry));



exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * One MPLS label in L3_TUNNEL_SINGLEm; VRL label
 * Two MPLS labels in MPLS_ENTRY_SINGLEm
 */
static int setup_ing_mpls_entry(int unit,
    int flow_id,
    int is_cpu)
{
    power_test_t *power_test_p = power_test_parray[unit];
    bcmdrd_sid_t sid;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int label_order;
    uint32_t mpls_label, mpls_exp, mpls_ttl;
    int src_dev_port;
    int use_other_port;

    SHR_FUNC_ENTER(unit);

    src_dev_port = (is_cpu == 1) ?
        0 : get_dev_src_port(unit, flow_id);

    /* Special handling here:
     * For cpu & odd flow_id insert only BOS label
     */
    use_other_port = 0;
    if ((is_cpu == 1) &&
        ((flow_id % 2) == 1)) {
        use_other_port = 1;
    }

    /* Insert INNER - VRL label*/
    sid = L3_TUNNEL_SINGLEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BASE_VALIDf, 0x1); /* START_OF_KEY_BM_0 */
    TH4_PWRT_SET_FIELD(KEY_TYPEf, 0x0);   /* MPLS {MPLS, SGP}*/
    label_order = 3; /* BOS */
    get_mpls_label(unit, flow_id, label_order,
                   &mpls_label, &mpls_exp, &mpls_ttl, use_other_port);
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_LABELf, mpls_label);
    TH4_PWRT_SET_FIELD(MPLSv_PORT_NUMf, src_dev_port);
    TH4_PWRT_SET_FIELD(MPLSv_V4_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(MPLSv_V6_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_ACTION_IF_BOSf, 0x2); /* (0x2: l3_iif) */
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_ACTION_IF_NOT_BOSf, 0x0); /* (0x0: invalid) */
    TH4_PWRT_SET_FIELD(MPLSv_L3_IIFf, get_l3_iif(unit, flow_id));
    TH4_PWRT_SET_FIELD(MPLSv_FLEX_CTR_ACTION_SELf, 0x0); /* 0x4 */
    TH4_PWRT_SET_FIELD(MPLSv_FLEX_CTR_OBJECTf, 0x3a54);
    if (((power_test_p->enable_map >> TH4_PWRT_EN_RND_PRI) & 0x1) == 0x1) {
        TH4_PWRT_SET_FIELD(MPLSv_DECAP_USE_EXP_FOR_PRIf, 1);
        TH4_PWRT_SET_FIELD(MPLSv_EXP_MAPPING_PTRf, flow_id % 32);
    }

    SHR_IF_ERR_EXIT(th4_pwrt_pt_insert(unit, sid, entry));


    if (use_other_port == 1) {
        goto exit;
    }

    /* Insert MIDDLE label*/
    sid = MPLS_ENTRY_SINGLEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BASE_VALIDf, 0x1); /* START_OF_KEY_BM_0 */
    TH4_PWRT_SET_FIELD(KEY_TYPEf, 0x0);   /* MPLS {MPLS, SGP}*/
    label_order = 2; /* NOT BOS */
    get_mpls_label(unit, flow_id, label_order,
                   &mpls_label, &mpls_exp, &mpls_ttl, 0);
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_LABELf, mpls_label);
    TH4_PWRT_SET_FIELD(MPLSv_PORT_NUMf, src_dev_port);
    TH4_PWRT_SET_FIELD(MPLSv_V4_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(MPLSv_V6_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_ACTION_IF_BOSf, 0x0); /* (0x0: invalid) */
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_ACTION_IF_NOT_BOSf, 0x1); /* (0x1: POP) */
    TH4_PWRT_SET_FIELD(MPLSv_L3_IIFf, 0x1abc); /* don't care */
    TH4_PWRT_SET_FIELD(MPLSv_FLEX_CTR_ACTION_SELf, 0x1); /* 0x5 */
    TH4_PWRT_SET_FIELD(MPLSv_FLEX_CTR_OBJECTf, 0x3a55);

    SHR_IF_ERR_EXIT(th4_pwrt_pt_insert(unit, sid, entry));


    /* Insert TOP label*/
    sid = MPLS_ENTRY_SINGLEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BASE_VALIDf, 0x1); /* START_OF_KEY_BM_0 */
    TH4_PWRT_SET_FIELD(KEY_TYPEf, 0x0);   /* MPLS {MPLS, SGP}*/
    label_order = 1; /* NOT BOS */
    get_mpls_label(unit, flow_id, label_order,
                   &mpls_label, &mpls_exp, &mpls_ttl, 0);
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_LABELf, mpls_label);
    TH4_PWRT_SET_FIELD(MPLSv_PORT_NUMf, src_dev_port);
    TH4_PWRT_SET_FIELD(MPLSv_V4_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(MPLSv_V6_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_ACTION_IF_BOSf, 0x0); /* (0x0: invalid) */
    TH4_PWRT_SET_FIELD(MPLSv_MPLS_ACTION_IF_NOT_BOSf, 0x1); /* (0x1: POP) */
    TH4_PWRT_SET_FIELD(MPLSv_L3_IIFf, 0x1abc); /* don't care */
    TH4_PWRT_SET_FIELD(MPLSv_FLEX_CTR_ACTION_SELf, 0x2); /* 0x6 */
    TH4_PWRT_SET_FIELD(MPLSv_FLEX_CTR_OBJECTf, 0x3a56);

    SHR_IF_ERR_EXIT(th4_pwrt_pt_insert(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * L3_IIFm
 * L3_IIF_PROFILEm
 */
static int setup_ing_l3_iif(int unit,
    int flow_id)
{
    power_test_t *power_test_p = power_test_parray[unit];
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, table_id;
    int MMU_INT_STAT_REQ_0, MMU_INT_STAT_REQ_1;

    SHR_FUNC_ENTER(unit);

    sid = L3_IIFm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    index = get_l3_iif(unit, flow_id);
    TH4_PWRT_SET_FIELD(L3_IIF_PROFILE_INDEXf, get_l3_iif_profile_index(unit, flow_id));
    TH4_PWRT_SET_FIELD(VRFf, get_vrf(unit, flow_id));
    TH4_PWRT_SET_FIELD(IPMC_L3_IIFf, index);   /* don't care */
    TH4_PWRT_SET_FIELD(IPV6L3_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(IPV4L3_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(IPMCV6_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(IPMCV4_ENABLEf, 0x1);
    table_id = 3;
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTIONf, table_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = L3_IIF_PROFILEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    index = get_l3_iif_profile_index(unit, flow_id);
    TH4_PWRT_SET_FIELD(ALLOW_GLOBAL_ROUTEf, 0x1);
    MMU_INT_STAT_REQ_0 = ((sal_rand() & 0x1) == 0) ? 1 : 3;
    MMU_INT_STAT_REQ_1 = ((sal_rand() & 0x1) == 0) ? 1 : 3;
    TH4_PWRT_SET_FIELD(MMU_INT_STAT_REQ_0f, MMU_INT_STAT_REQ_0);
    TH4_PWRT_SET_FIELD(MMU_INT_STAT_REQ_1f, MMU_INT_STAT_REQ_1);
    /* Randomize the rest */
    TH4_PWRT_SET_FIELD(IPMC_MISS_AS_L2MCf, sal_rand() & 0x1);
    TH4_PWRT_SET_FIELD(UNKNOWN_IPV4_MC_TOCPUf, sal_rand() & 0x1);
    TH4_PWRT_SET_FIELD(UNRESOLVEDL3SRC_TOCPUf, 0x0);
    TH4_PWRT_SET_FIELD(RESERVEDf, sal_rand() & 0x1f);
    if (((power_test_p->enable_map >> TH4_PWRT_EN_RND_PRI) & 0x1) == 0x1) {
    TH4_PWRT_SET_FIELD(TRUST_DSCP_PTRf, flow_id % 64);
    }

    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


/* ALPM helper APIs
 */
static uint32_t get_alpm_data_bkt_ptr(
    int unit,
    int flow_id,
    int table_level)
{
    int bkt_ptr;
    int l2_bkt_offset, l3_bkt_offset;
    l2_bkt_offset = 0;
    l3_bkt_offset = 0;
    bkt_ptr = ((table_level == 1) ? l2_bkt_offset : l3_bkt_offset) + flow_id;
    return (bkt_ptr);
}


static uint32_t get_alpm_fmt(
    int unit,
    int flow_id,
    int table_alpm_level)
{
    int alpm_level;
    int fmt;

    alpm_level = get_alpm_level(unit);

    switch (table_alpm_level) {
        case 1 : fmt = 6; break; /* pivot */
        case 2 : fmt = (alpm_level == 3) ? 6 : 10; break; /* route */
        case 3 : fmt = 10;   /* route: 6 entries per bucket */
                 break;
        default : fmt = 10;
    }

    return (fmt);
}


static uint32_t get_alpm_kshift(
    int unit,
    int flow_id,
    int table_alpm_level)
{
    int kshift_level_1, kshift_level_2;
    kshift_level_1 = 4;
    kshift_level_2 = 4;

    if (table_alpm_level == 1) {
        return kshift_level_1;
    } else {
        return (kshift_level_1 + kshift_level_2);
    }
}



static void get_alpm_data(
    int unit,
    int flow_id,
    int table_alpm_level,
    uint32_t *memfld)
{
    /*                    ALPM1_DATA        ALPM2_DATA  */
    uint32_t DEFAULT_MISS;  /*  << 0 */      /*  << 0 */
    uint32_t DIRECT_ROUTE;  /*  << 1 */
    uint32_t BKT_PTR;       /*  << 2 */      /*  << 1 */
    uint32_t KSHIFT;        /*  << 12 */     /*  << 14 */
    uint32_t ROFS;          /*  << 21 */     /*  << 23 */
    uint32_t FMT0;          /*  << 24 */     /*  << 26 */
    uint32_t FMT1;          /*  << 28 */     /*  << 30 */
    uint32_t FMT2;          /*  << 32 */     /*  << 34 */
    uint32_t FMT3;          /*  << 36 */     /*  << 38 */
    uint32_t FMT4;          /*  << 40 */     /*  << 42 */
    uint32_t FMT5;                           /*  << 46 */
    uint32_t FMT6;                           /*  << 50 */
    uint32_t FMT7;                           /*  << 54 */
    int fmt;

    fmt = get_alpm_fmt(unit, flow_id, table_alpm_level + 1);

    /* ALPM1_DATA */
    if (table_alpm_level == 1) { /* Data used in  L3_DEFIP_ALPM_LEVEL1::ALPM_DATA */
        DEFAULT_MISS = 0x0;
        DIRECT_ROUTE = 0x0;
        BKT_PTR = get_alpm_data_bkt_ptr(unit, flow_id, table_alpm_level);
        KSHIFT = 13 + get_alpm_kshift(unit, flow_id, table_alpm_level); /* VRF_ID_WIDTH + kshift*/
        ROFS   = 0x0;
        FMT0 = fmt;
        FMT1 = fmt;
        FMT2 = fmt;
        FMT3 = fmt;
        FMT4 = fmt;
        FMT5 = fmt;
        memfld[0] = (DEFAULT_MISS << 0) | (DIRECT_ROUTE << 1) | (BKT_PTR << 2) |
                    (KSHIFT << 12) | (ROFS << 21) | (FMT0 << 24) | (FMT1 << 28);
        memfld[1] = (FMT2 << 0) | (FMT3 << 4) | (FMT4 << 8) | (FMT5 << 12);
    } else
     /* ALPM2_DATA */
    if (table_alpm_level == 2) {  /* Data used in  L3_DEFIP_ALPM_LEVEL2::ALPM_DATA */
        DEFAULT_MISS = 0x0;
        BKT_PTR = get_alpm_data_bkt_ptr(unit, flow_id, table_alpm_level);
        KSHIFT = 13 + get_alpm_kshift(unit, flow_id, table_alpm_level); /* VRF_ID_WIDTH + kshift*/
        ROFS   = 0x0;
        FMT0 = 0;   /* 0-3 EM, 4-7 alpm */
        FMT1 = 0;
        FMT2 = 0;
        FMT3 = 0;
        FMT4 = fmt;
        FMT5 = fmt;
        FMT6 = fmt;
        FMT7 = fmt;
        memfld[0] = (DEFAULT_MISS << 0) | (BKT_PTR << 1) |
                    (KSHIFT << 14) | (ROFS << 23) |
                    (FMT0 << 26) | (FMT1 << 30);
        memfld[1] = (FMT1 >> 2) | (FMT2 << 2) | (FMT3 << 6) | (FMT4 << 10) |
                    (FMT5 << 14) | (FMT6 << 18) | (FMT7 << 22);
    }
}


static uint32_t get_mem_bit(
    uint32_t *memfld,
    int bit_pos)
{
    /*
     * memfld[0] bits 127:96
     * memfld[1] bits  95:64
     * memfld[2] bits  63:32
     * memfld[3] bits  31:0
     */
    int word_indx, word_offset;
    word_indx   = (bit_pos / 32);
    word_offset = bit_pos % 32;
    return ((memfld[word_indx] >> word_offset) & 0x1);
}

static void set_mem_bit(
    uint32_t *memfld,
    int bit_pos,
    uint32_t val)
{
    int word_indx, word_offset;
    word_indx   = (bit_pos / 32);
    word_offset = bit_pos % 32;
    memfld[word_indx] |= (val << word_offset);
}

static void set_field_in_memfld(
    uint32_t *src_memfld,
    uint32_t *dst_memfld,
    int field_start_bit,
    int field_width)
{
    int i;
    uint32_t bitval;

    for (i = 0; i < field_width; i++) {
        bitval = get_mem_bit(src_memfld, i);
        set_mem_bit(dst_memfld, field_start_bit + i, bitval);
    }
}

static uint64_t get_mem_bit_64(
    uint64_t *memfld,
    int bit_pos)
{
    int word_indx, word_offset;
    word_indx   = (bit_pos / 64);
    word_offset = bit_pos % 64;
    return ((memfld[word_indx] >> word_offset) & 0x1);
}

static void set_mem_bit_64(
    uint64_t *memfld,
    int bit_pos,
    uint64_t val)
{
    int word_indx, word_offset;
    word_indx   = (bit_pos / 64);
    word_offset = bit_pos % 64;
    memfld[word_indx] |= (val << word_offset);
}

static void set_field_in_memfld_64(
    uint64_t *src_memfld,
    uint64_t *dst_memfld,
    int field_start_bit,
    int field_width)
{
    int i;
    uint64_t bitval;

    for (i = 0; i < field_width; i++) {
        bitval = get_mem_bit_64(src_memfld, i);
        set_mem_bit_64(dst_memfld, field_start_bit + i, bitval);
    }
}

static void get_alpm_prefix(
    int unit,
    int flow_id,
    int fmt,
    int bank_id,
    uint32_t *ipv6_addr,
    int prev_len_match, /* kshift */
    int key_len,
    int program_route_pivot,
    uint32_t *prefix)
{
    int prefix_width;
    int key_start;
    int key_end, i, j;
    uint32_t bit_val;

    /*
     * This function assumes ipv6_addr in this packing:
     * ipv6_addr[0]    31:0
     * ipv6_addr[1]   63:32
     * ipv6_addr[2]   95:64
     * ipv6_addr[3]  127:96
     */

    if (program_route_pivot == 0) {
        switch (fmt) {
            case 1  : prefix_width =  16; break;
            case 2  : prefix_width =  24; break;
            case 3  : prefix_width =  32; break;
            case 4  : prefix_width =  40; break;
            case 5  : prefix_width =  48; break;
            case 6  : prefix_width =  64; break;
            case 7  : prefix_width =  80; break;
            case 8  : prefix_width = 104; break;
            case 9  : prefix_width = 144; break;
            case 10 : prefix_width =  31; break;
            case 11 : prefix_width =  71; break;
            case 12 : prefix_width = 191; break;
            default : prefix_width =  34; break;
        }
    } else {
        switch (fmt) {
            case 1  : prefix_width =   6; break;
            case 2  : prefix_width =  22; break;
            case 3  : prefix_width =  46; break;
            case 4  : prefix_width =  86; break;
            case 5  : prefix_width = 166; break;
            case 6  : prefix_width =  13; break;
            case 7  : prefix_width =  53; break;
            case 8  : prefix_width = 133; break;
            default : prefix_width = 166; break;
        }
    }

    key_start = 127 - prev_len_match;
    key_end   = 127 - prev_len_match - prefix_width;

    j = 0;
    for (i = key_start; i > key_end; i--) {
        if (j < key_len) {
            bit_val = get_mem_bit(ipv6_addr, i);
            set_mem_bit(prefix, prefix_width-1-j, bit_val);
            /*prefix[prefix_width-1-j] = key[i];*/
        } else if (j == key_len) {
            set_mem_bit(prefix, prefix_width-1-j, 0x1);
            /*prefix[prefix_width-1-j] = 1;*/
        } else {
            set_mem_bit(prefix, prefix_width-1-j, 0x0);
            /*prefix[prefix_width-1-j] = 0;*/
        }
        j++;
    }
}

static void get_lpm_dest_type(
    int unit,
    int flow_id,
    uint32_t *dest_type,
    uint32_t *destination)
{
    power_test_t *power_test_p = power_test_parray[unit];

    if (power_test_p->enable_ecmp_dlb != 0) {
        *dest_type = 1;  /* 0: NHI; 1: ECMP_GROUP */
        *destination = get_ecmp_level1_ecmp_group_ptr(unit, flow_id, 0);
    } else {
        *dest_type = 0;
        *destination = get_next_hop_index(unit, flow_id);
    }
}

static void get_alpm_assoc_data(
    int unit,
    int flow_id,
    int is_pivot,
    int table_alpm_level,
    uint32_t *memfld)
{
    int alpm_level;
    uint32_t DEST_TYPE, DESTINATION, PRI, RPE, DST_DISCARD, CLASS_ID;
    uint32_t FLEX_CTR_ACTION_SEL, FLEX_CTR_OBJECT;
    uint32_t INT_ACTION_PROFILE_IDX, INT_FLOW_CLASS;
    int fmt, is_assoc_data_full;
    uint32_t l_destination;

    alpm_level = get_alpm_level(unit);
    fmt = get_alpm_fmt(unit, flow_id, table_alpm_level);

    is_assoc_data_full = 1;
    if (is_pivot == 1) {
        is_assoc_data_full = (fmt >= 6) ? 1 : 0;
    } else { /* route */
        is_assoc_data_full = (fmt >= 10) ? 1 : 0;
    }

    get_lpm_dest_type(unit, flow_id, &DEST_TYPE, &l_destination);
    if (is_assoc_data_full == 1) { /* ROUTE : ASSOC_DATA_FULL */
        if (table_alpm_level == alpm_level) {
            DESTINATION = l_destination;
        } else {
            DESTINATION = 0x5abc; /* random */
        }
        PRI = 0x0;
        RPE = 0x0;
        DST_DISCARD = 0x0;
        CLASS_ID = 0; /* L3 Classification id used for ingress FP. */
        FLEX_CTR_ACTION_SEL = 0x0;
        FLEX_CTR_OBJECT = 0;
        INT_ACTION_PROFILE_IDX = 1 + (sal_rand32() % 15);
        INT_FLOW_CLASS         = sal_rand32() & 0x1ff;
        memfld[0] = (FLEX_CTR_OBJECT << 18) |(DST_DISCARD << 17) |
                    (RPE << 16) | (DESTINATION << 1) | (DEST_TYPE << 0);
        memfld[1] = ((FLEX_CTR_OBJECT >> 13) << 0) | (FLEX_CTR_ACTION_SEL << 1) |
                    (CLASS_ID << 4) | (PRI << 10) |
                    (INT_ACTION_PROFILE_IDX << 13) | (INT_FLOW_CLASS << 4);
    }
    else { /* PIVOT : ASSOC_DATA_REDUCED */
        if (table_alpm_level == alpm_level) {
            DESTINATION = l_destination;
        } else {
            DESTINATION = 0x5abc; /* random */
        }
        memfld[0] = (DESTINATION << 1) | (DEST_TYPE << 0);
    }
}


static void get_alpm_full_data(
    int unit,
    int flow_id,
    int entry_number,
    int table_alpm_level,
    uint32_t *prefix,
    uint32_t *alpm_assoc_data,
    uint32_t *alpm_data,
    uint32_t *alpm_full_data)
{
    int i;
    int prefix_offset, assoc_offset, data_offset, data_end;
    int entry_width, entry_offset;
    uint32_t bit_val;
    int alpm_level;
    int fmt, is_route;

    /* Handles LEVEL 2 & 3 tables */

    alpm_level = get_alpm_level(unit);
    fmt = get_alpm_fmt(unit, flow_id, table_alpm_level);
    is_route = (table_alpm_level < alpm_level) ? 0 : 1;

    if (is_route == 1) {
        switch (fmt) {
            case 10 : entry_width  = 80;
                      prefix_offset = 0;
                      assoc_offset  = 31;
                      data_offset   = 80;
                      break;
            /* Add more if required */
            default :  entry_width  = 80;
                      prefix_offset = 0;
                      assoc_offset  = 31;
                      data_offset   = 80;
                      break;
        }
    } else { /* pivot */
        switch (fmt) {
            case 6 :  entry_width  = 120;
                      prefix_offset = 0;
                      assoc_offset  = 13;
                      data_offset   = 62;
                      break;
            /* Add more if required */
            default :  entry_width  = 120;
                      prefix_offset = 0;
                      assoc_offset  = 13;
                      data_offset   = 62;
                      break;
        }
    }

    entry_offset  = entry_number * entry_width;
    prefix_offset = entry_offset + prefix_offset;
    assoc_offset  = entry_offset + assoc_offset;
    data_offset   = entry_offset + data_offset;
    data_end      = entry_offset + entry_width;

    /* Copy prefix */
    for (i = prefix_offset; i < assoc_offset; i++) {
        bit_val = get_mem_bit(prefix, i - prefix_offset);
        set_mem_bit(alpm_full_data, i, bit_val);
    }

    /* Copy assoc_data */
    for (i = assoc_offset; i < data_offset; i++) {
        bit_val = get_mem_bit(alpm_assoc_data, i - assoc_offset);
        set_mem_bit(alpm_full_data, i, bit_val);
    }

    /* Copy data if pivot */
    if (is_route == 0) {
        for (i = data_offset; i < data_end; i++) {
            bit_val = get_mem_bit(alpm_data, i - data_offset);
            set_mem_bit(alpm_full_data, i, bit_val);
        }
    }
}



/*
 * This function configures:
 * L3_DEFIP_ALPM_LEVEL2m
 */
static int setup_alpm_level2(int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    /*bcmltd_sid_t ltid = -1;*/
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t ip_addr[BCMDRD_MAX_PT_WSIZE];
    uint32_t prefix[BCMDRD_MAX_PT_WSIZE];
    uint32_t alpm_assoc_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t alpm_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t alpm_full_data[BCMDRD_MAX_PT_WSIZE];
    int index;
    int fmt, bank_id, key_len, program_route_pivot;
    int alpm_level, table_alpm_level;
    int entry_number;
    int alpm1_data_kshift, alpm2_data_kshift;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(entry));
    sal_memset(ip_addr, 0, sizeof(ip_addr));
    sal_memset(prefix, 0, sizeof(prefix));
    sal_memset(alpm_assoc_data, 0, sizeof(alpm_assoc_data));
    sal_memset(alpm_data, 0, sizeof(alpm_data));
    sal_memset(alpm_full_data, 0, sizeof(alpm_full_data));

    table_alpm_level = 2;
    alpm_level = get_alpm_level(unit);
    fmt = get_alpm_fmt(unit, flow_id, table_alpm_level);
    alpm1_data_kshift = get_alpm_kshift(unit, flow_id, table_alpm_level - 1); /* K shift */
    alpm2_data_kshift = get_alpm_kshift(unit, flow_id, table_alpm_level); /* K shift */
    if (alpm_level == 2) {
        program_route_pivot = 0;
        key_len = 6;
    } else {
        program_route_pivot = 1;
        key_len = alpm2_data_kshift - alpm1_data_kshift;
    }
    bank_id = 0;
    entry_number = 0;

    get_ip_addr_da(unit, flow_id, ip_addr);
    get_alpm_prefix(unit, flow_id,
                    fmt,
                    bank_id,
                    ip_addr,
                    alpm1_data_kshift,
                    key_len,
                    program_route_pivot,
                    prefix);
    get_alpm_assoc_data(unit, flow_id, program_route_pivot, table_alpm_level, alpm_assoc_data);

    if (alpm_level == 3) {
        get_alpm_data(unit, flow_id, table_alpm_level, alpm_data);
    }

    get_alpm_full_data(unit, flow_id, entry_number, table_alpm_level,
                       prefix, alpm_assoc_data, alpm_data, alpm_full_data);

    sid = L3_DEFIP_ALPM_LEVEL2m;
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, TABLE_FIELDSf, entry, alpm_full_data));
    /* (l2_bank_id * `TABSIZE_L3_DEFIP_ALPM_LEVEL2/5) + alpm1_data.bkt_ptr */
    index = (bank_id * (6144/6)) + get_alpm_data_bkt_ptr(unit, flow_id, table_alpm_level - 1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    /* Use low level API to bypass ALPM_LEVEL2 range checks */
    SHR_IF_ERR_EXIT(bcmbd_pt_write(unit, sid, &pt_dyn_info, NULL, entry));

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * L3_DEFIP_ALPM_LEVEL3m
 */
static int setup_alpm_level3(int unit,
    int flow_id)
{
    const bcmdrd_sid_t sid = L3_DEFIP_ALPM_LEVEL3m;
    /*bcmltd_sid_t ltid = -1;*/
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t ip_addr[BCMDRD_MAX_PT_WSIZE];
    uint32_t prefix[BCMDRD_MAX_PT_WSIZE];
    uint32_t alpm_assoc_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t alpm_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t alpm_full_data[BCMDRD_MAX_PT_WSIZE];
    int index;
    int fmt, bank_id, key_len, program_route_pivot;
    int table_alpm_level;
    /*int alpm_level;*/
    int entry_number;
    int alpm2_data_kshift;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(entry));
    sal_memset(ip_addr, 0, sizeof(ip_addr));
    sal_memset(prefix, 0, sizeof(prefix));
    sal_memset(alpm_assoc_data, 0, sizeof(alpm_assoc_data));
    sal_memset(alpm_data, 0, sizeof(alpm_data));
    sal_memset(alpm_full_data, 0, sizeof(alpm_full_data));

    table_alpm_level = 3;
    /*alpm_level = get_alpm_level(unit);*/
    fmt = get_alpm_fmt(unit, flow_id, table_alpm_level);
    alpm2_data_kshift = get_alpm_kshift(unit, flow_id, table_alpm_level - 1); /* K shift */

    program_route_pivot = 0; /* no pivot in level 3 */

    bank_id = 4;
    key_len = 6;
    entry_number = 0;

    get_ip_addr_da(unit, flow_id, ip_addr);
    get_alpm_prefix(unit, flow_id,
                    fmt,
                    bank_id,
                    ip_addr,
                    alpm2_data_kshift,
                    key_len,
                    program_route_pivot,
                    prefix);
    get_alpm_assoc_data(unit, flow_id, program_route_pivot, table_alpm_level, alpm_assoc_data);

    get_alpm_full_data(unit, flow_id, entry_number, table_alpm_level, prefix, alpm_assoc_data, alpm_data, alpm_full_data);

    /*sid = L3_DEFIP_ALPM_LEVEL3m;*/
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, TABLE_FIELDSf, entry, alpm_full_data));
    /* (l2_bank_id * `TABSIZE_L3_DEFIP_ALPM_LEVEL2/5) + alpm1_data.bkt_ptr */
    index = (bank_id * (65536/8)) + get_alpm_data_bkt_ptr(unit, flow_id, table_alpm_level - 1);

    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);

    /* Use low level API to bypass ALPM_LEVEL3 range checks */
    SHR_IF_ERR_EXIT(bcmbd_pt_write(unit, sid, &pt_dyn_info, NULL, entry));

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * L3_DEFIP_LEVEL1m
 */
static int setup_ing_lpm(int unit,
    int flow_id)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;
    uint32_t KEY_MODE, KEY_TYPE, IP_ADDR_3, IP_ADDR_2, IP_ADDR_1, VRF_ID;
    uint32_t DEST_TYPE, DESTINATION, PRI, RPE, DST_DISCARD, CLASS_ID;
    int alpm_level, table_alpm_level, program_route_pivot;
    uint64_t val_64;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L3_DEFIP_LEVEL1m", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VALID0", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VALID1", 0x1));

    /* KEY0f has 48 bits*/
    KEY_MODE = 0x1; /*"Single Wide Entry." */
    KEY_TYPE = 0x1; /*"IPv6 Database."*/
    IP_ADDR_2 = get_ipv6_da(unit, flow_id, 2); /* ip bits 95:64 */
    IP_ADDR_1 = get_ipv6_da(unit, flow_id, 1); /* ip bits 63:32 */
    VRF_ID = get_vrf(unit, flow_id);
    sal_memset(memfld, 0, sizeof(memfld));
    /* KEY0: IP(45b)[95:51] */
    memfld[0] = ((IP_ADDR_2 & 0xffff) << 16)  | ((IP_ADDR_1 >> 19) << 3) | (KEY_TYPE << 2) | (KEY_MODE << 0);
    memfld[1] = (IP_ADDR_2 >> 16);
    shr_uint32_array_to_uint64(memfld, &val_64);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "KEY0", val_64));

    sal_memset(memfld, 0, sizeof(memfld));
    /* KEY1: VRF(13b)[12:0] , IP(32b)[127:96] */
    IP_ADDR_3 = get_ipv6_da(unit, flow_id, 3); /* ip bits 127:96 */
    memfld[0] = (IP_ADDR_3 << 3) | (KEY_TYPE << 2) | (KEY_MODE << 0);
    memfld[1] = (VRF_ID << 3) | (IP_ADDR_3 >> 29);
    shr_uint32_array_to_uint64(memfld, &val_64);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "KEY1", val_64));

    /* ASSOC_DATA0f has 48 bits*/
    get_lpm_dest_type(unit, flow_id, &DEST_TYPE, &DESTINATION);
    PRI = 0x0;
    RPE = 0x0;
    DST_DISCARD = 0x0;
    CLASS_ID = 0; /* L3 Classification id used for ingress FP. */

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = (CLASS_ID << 22) | (DST_DISCARD << 21) |
                (RPE << 20) | (PRI << 16) | (DESTINATION << 1) | (DEST_TYPE << 0);

    sal_memset(memfld, 0, sizeof(memfld));
    program_route_pivot = 1; /* get assoc_data_full */
    table_alpm_level = 1;
    get_alpm_assoc_data(unit, flow_id, program_route_pivot, table_alpm_level, memfld);

    shr_uint32_array_to_uint64(memfld, &val_64);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ASSOC_DATA0", val_64));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ASSOC_DATA1", val_64));

    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xffffffff;
    memfld[1] = 0x0000ffff;
    shr_uint32_array_to_uint64(memfld, &val_64);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MASK0", val_64));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MASK1", val_64));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_DATA0", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_DATA1", 0));

    sal_memset(memfld, 0, sizeof(memfld));
    alpm_level = get_alpm_level(unit);

    cli_out("setup_alpm_level1() alpm_level=%0d\n",
                  alpm_level);

    if (alpm_level >= 2) {
        table_alpm_level = 1;
        get_alpm_data(unit, flow_id, table_alpm_level, memfld);
    }

    shr_uint32_array_to_uint64(memfld, &val_64);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ALPM1_DATA0", val_64));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ALPM1_DATA1", val_64));
    index = get_l3_defip_table_index(unit, flow_id);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);


    /* - ALPM Level 2 - */
    if (alpm_level >= 2) {
         setup_alpm_level2(unit, flow_id);
    }

    /* - ALPM Level 3 - */
    if (alpm_level == 3) {
         setup_alpm_level3(unit, flow_id);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * ING_L3_NEXT_HOPm
 */
static int setup_ing_next_hop(int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;
    power_test_t *power_test_p = power_test_parray[unit];

    SHR_FUNC_ENTER(unit);

    sid = ING_L3_NEXT_HOPm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));

    TH4_PWRT_SET_FIELD(ENTRY_TYPEf, 0x0); /* regular L3 UC */
    TH4_PWRT_SET_FIELD(VLAN_IDf, get_vlan_id(unit, flow_id));
    if (((power_test_p->enable_map >> TH4_PWRT_EN_LAG_RES) & 0x1) != 0) {
        TH4_PWRT_SET_FIELD(Tf, 0x1);
        TH4_PWRT_SET_FIELD(TGIDf, flow_id);
    } else {
        TH4_PWRT_SET_FIELD(Tf, 0x0);
        /*TH4_PWRT_SET_FIELD(PORT_NUMf, get_dev_dst_port(unit, flow_id));*/
        TH4_PWRT_SET_FIELD(PORT_NUMf, get_final_dst_dev_port(unit, flow_id));
    }
    TH4_PWRT_SET_FIELD(DROPf, 0x0);
    TH4_PWRT_SET_FIELD(COPY_TO_CPUf, 0x0);
    index = get_next_hop_index(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * COS_MAP_SELm
 * PORT_COS_MAPm
 */
static int setup_ing_qos(int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, sel, pri;

    SHR_FUNC_ENTER(unit);

    sid = COS_MAP_SELm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(SELECTf, get_cos_map_sel__selectf(unit, flow_id));
    index = get_dev_src_port(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = PORT_COS_MAPm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    /* Index by COS_MAP_SEL.SELECT(2-bit), ingress port internal priority(4-bit). */
    for (sel = 0; sel < 4; sel++) {
        for (pri = 0; pri < 16; pri++) {
            index  = (sel * 16) + pri;
            sal_memset(entry, 0, sizeof(entry));
            TH4_PWRT_SET_FIELD(COSf, get_port_cos_map__cosf(unit, flow_id, index));
            TH4_PWRT_SET_FIELD(RQE_Q_NUMf, get_port_cos_map__cosf(unit, flow_id, index));
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int setup_udf(
    int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, i, dev_src_port;
    int pipe, ipep_acc_type;
    bcmlt_entry_handle_t lt_entry = BCMLT_INVALID_HDL;

    static const bcmdrd_fid_t udf_base_offset[16] = {
        UDF1_BASE_OFFSET_0f, UDF1_BASE_OFFSET_1f, UDF1_BASE_OFFSET_2f, UDF1_BASE_OFFSET_3f,
        UDF1_BASE_OFFSET_4f, UDF1_BASE_OFFSET_5f, UDF1_BASE_OFFSET_6f, UDF1_BASE_OFFSET_7f,
        UDF2_BASE_OFFSET_0f, UDF2_BASE_OFFSET_1f, UDF2_BASE_OFFSET_2f, UDF2_BASE_OFFSET_3f,
        UDF2_BASE_OFFSET_4f, UDF2_BASE_OFFSET_5f, UDF2_BASE_OFFSET_6f, UDF2_BASE_OFFSET_7f
    };
    static const bcmdrd_fid_t udf_offset[16] = {
        UDF1_OFFSET0f, UDF1_OFFSET1f, UDF1_OFFSET2f, UDF1_OFFSET3f,
        UDF1_OFFSET4f, UDF1_OFFSET5f, UDF1_OFFSET6f, UDF1_OFFSET7f,
        UDF2_OFFSET0f, UDF2_OFFSET1f, UDF2_OFFSET2f, UDF2_OFFSET3f,
        UDF2_OFFSET4f, UDF2_OFFSET5f, UDF2_OFFSET6f, UDF2_OFFSET7f
    };

    SHR_FUNC_ENTER(unit);

    dev_src_port = get_dev_src_port(unit, flow_id);
    SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, dev_src_port, &pipe));

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "FP_UDF_TCAMm", &lt_entry));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "VALID", 0x3));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "L2_ETHER_TYPE", get_ether_type(unit, flow_id)));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "OUTER_IP_TYPE", 0x2));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "L3_FIELDS", 0x60003));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "SOURCE_PORT_NUMBER", dev_src_port));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "L2_ETHER_TYPE_MASK", 0xffff));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "OUTER_IP_TYPE_MASK", 0x7));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "L3_FIELDS_MASK", 0xf000f));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "SOURCE_PORT_NUMBER_MASK", 0xff));
    index = get_udf_index(unit, flow_id);
    ipep_acc_type = pipe / 4;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INSTANCE", ipep_acc_type));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INDEX", index));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(lt_entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(lt_entry);

    sid = FP_UDF_OFFSETm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    for (i = 0; i < 16; i++) {
        TH4_PWRT_SET_FIELD(udf_base_offset[i], 1 + (sal_rand() % 4)); /* range 1:4 */
        TH4_PWRT_SET_FIELD(udf_offset[i],      1 + (sal_rand() % 5)); /* range 1:4 */
    }
    TH4_PWRT_SET_FIELD(HASH_MASK_1f, sal_rand32() & 0xffff);
    TH4_PWRT_SET_FIELD(HASH_MASK_2f, sal_rand32() & 0xffff);
    TH4_PWRT_SET_FIELD(ENABLE_UDF_CONDITIONAL_CHECKf, 0x1);
    /* get macda[15:0] on UDF_CONDITIONAL_CHECK_TABLE_CAM.key[15:0] */
    TH4_PWRT_SET_FIELD(UDF1_BASE_OFFSET_7f, 6);
    TH4_PWRT_SET_FIELD(UDF2_BASE_OFFSET_7f, 6);
    TH4_PWRT_SET_FIELD(UDF1_OFFSET7f, 2);
    TH4_PWRT_SET_FIELD(UDF2_OFFSET7f, 2);
    index = get_udf_index(unit, flow_id);
    ipep_acc_type = pipe / 4;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc_type);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * MY_STATION_TCAMm
 */
static int setup_ing_my_station_tcam(int unit,
    int flow_id,
    int is_cpu)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int index;
    int src_dev_port;
    uint32_t vlan_id;
    bcm_mac_t mac_addr;
    uint64_t mac_64;
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    src_dev_port = (is_cpu == 1) ?
        0 : get_dev_src_port(unit, flow_id);
    vlan_id = (is_cpu == 1) ? TH4_PWRT_VLAN_ID_BASE :
        get_vlan_id(unit, flow_id);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "MY_STATION_TCAMm", &entry));

    index = get_my_station_tcam_index(unit, flow_id, is_cpu);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VALID", 0x3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MPLS_TERMINATION_ALLOWED", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IPV6_TERMINATION_ALLOWED", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IPV4_TERMINATION_ALLOWED", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ARP_RARP_TERMINATION_ALLOWED", sal_rand() & 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IPV4_MULTICAST_TERMINATION_ALLOWED", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VLAN_ID", vlan_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PORT_NUM", src_dev_port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "T", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VLAN_ID_MASK", 0xfff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "T_MASK", 0));
    get_mac_da(unit, flow_id, mac_addr);
    sal_memset(memfld, 0, sizeof(memfld));
    get_mac_da(unit, flow_id, mac_addr);
    convert_mac(mac_addr, memfld);
    shr_uint32_array_to_uint64(memfld, &mac_64);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MAC_ADDR", mac_64));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MAC_ADDR_MASK", 0xffffffffffff));

    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

exit:
    SHR_FUNC_EXIT();
}


static int setup_ing_ecmp(
    int unit,
    int flow_id,
    int is_cpu)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;

    SHR_FUNC_ENTER(unit);

    sid = L3_ECMP_COUNTm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(MEMBER_COUNTf, 0); /* Modulo 1 is 0*/
    TH4_PWRT_SET_FIELD(BASE_PTRf, get_ecmp_level1_base_ptr(unit, flow_id, is_cpu));
    TH4_PWRT_SET_FIELD(LB_MODEf, 8); /* WECMP_0_127 */
    index = get_ecmp_level1_ecmp_group_ptr(unit, flow_id, is_cpu);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

    sid = L3_ECMPm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ENTRY_TYPEf, 1); /* WECMP */
    TH4_PWRT_SET_FIELD(WECMPv_NEXT_HOP_INDEX_Af, get_next_hop_index(unit, flow_id));
    TH4_PWRT_SET_FIELD(WECMPv_NEXT_HOP_INDEX_Bf, get_next_hop_index(unit, flow_id));
    TH4_PWRT_SET_FIELD(WECMPv_WEIGHTf, sal_rand32() & 0xfff);
    index = get_ecmp_level1_ecmp_index(unit, flow_id, is_cpu);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


static int setup_egr_ctrl_regs(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    uint32_t sel_val, sel_val_base;
    power_test_t *power_test_p = power_test_parray[unit];
    int dev_port, i;
    static const bcmdrd_sid_t rdbgc_regs[8] = {
        RDBGC0_SELECTr, RDBGC1_SELECTr,  RDBGC2_SELECTr, RDBGC3_SELECTr,
        RDBGC4_SELECTr, RDBGC5_SELECTr,  RDBGC6_SELECTr, RDBGC7_SELECTr
    };
    static const bcmdrd_sid_t tdbgc_regs[12] = {
        TDBGC0_SELECTr, TDBGC1_SELECTr,  TDBGC2_SELECTr,
        TDBGC3_SELECTr, TDBGC4_SELECTr,  TDBGC5_SELECTr,
        TDBGC6_SELECTr, TDBGC7_SELECTr,  TDBGC8_SELECTr,
        TDBGC9_SELECTr, TDBGC10_SELECTr, TDBGC11_SELECTr
    };

    SHR_FUNC_ENTER(unit);

    sid = EGR_MTUr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));

    /* Configure all enabled ports */
    TH4_PWRT_SET_FIELD(MTU_ENABLEf, 1);
    TH4_PWRT_SET_FIELD(MTU_SIZEf, 0x3fff);
    for (dev_port = 0 ; dev_port < TH4_PWRT_DEV_PORTS_PER_DEV; dev_port++) {
        if (power_test_p->dev_enabled_ports[dev_port] == 1) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, dev_port);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    /* for RDBGC */
    sel_val_base = 0x01001001;
    for (i = 0; i < 8; i++) {
        sid = rdbgc_regs[i];
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        sel_val = sel_val_base << i;
        TH4_PWRT_SET_FIELD(BITMAPf, sel_val);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

    /* for TDBGC */
    sel_val_base = 0x01001001;
    for (i = 0; i < 12; i++) {
        sid = tdbgc_regs[i];
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        sel_val = sel_val_base << i;
        TH4_PWRT_SET_FIELD(BITMAPf, sel_val);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

exit:
    SHR_FUNC_EXIT();
}



/*
 * This function configures:
 * EGR_VLANm
 * EGR_VLAN_STGm
 */
static int setup_egr_vlan_tabs(int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = -1;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t entry_b[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, i, src_port, dst_port, bit_pos;
    int flow_with_same_stg;

    SHR_FUNC_ENTER(unit);

    sid = EGR_VLANm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));

    TH4_PWRT_SET_FIELD(VALIDf, 0x1);
    sal_memset(memfld, 0, sizeof(memfld));
    get_port_bmp_pipe(unit, 0, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAP_P0f, entry, memfld));
    sal_memset(memfld, 0, sizeof(memfld));
    get_port_bmp_pipe(unit, 1, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAP_P1f, entry, memfld));
    sal_memset(memfld, 0, sizeof(memfld));
    get_port_bmp_pipe(unit, 2, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAP_P2f, entry, memfld));
    sal_memset(memfld, 0, sizeof(memfld));
    get_port_bmp_pipe(unit, 3, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAP_P3f, entry, memfld));
    sal_memset(memfld, 0, sizeof(memfld));
    get_port_bmp_pipe(unit, 4, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAP_P4f, entry, memfld));
    sal_memset(memfld, 0, sizeof(memfld));
    get_port_bmp_pipe(unit, 5, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAP_P5f, entry, memfld));
    sal_memset(memfld, 0, sizeof(memfld));
    get_port_bmp_pipe(unit, 6, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAP_P6f, entry, memfld));
    sal_memset(memfld, 0, sizeof(memfld));
    get_port_bmp_pipe(unit, 7, memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, PORT_BITMAP_P7f, entry, memfld));
    TH4_PWRT_SET_FIELD(STGf, get_egr_stg(flow_id)); /* one entry in vlan profile is used */
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTIONf, 0x3);
    index = get_vlan_id(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    sal_memset(entry, 0, sizeof(entry));
    sal_memset(entry_b, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    /* Except associated ports randomize the rest of the bitmap */
    for (i = 0; i < 8; i++) {
        entry[i] = sal_rand32();
        entry_b[i] = sal_rand32();
    }
    entry[i] = sal_rand32() & 0xffff;
    entry_b[i] = sal_rand32() & 0xffff;
    index = get_egr_stg(flow_id);
    memfld[0] = 0x3;
    for (i = 0; i < 4; i++) {
        flow_with_same_stg = (i * 64) + index;
        dst_port = get_dev_dst_port(unit, flow_with_same_stg);
        src_port = get_dev_src_port(unit, flow_with_same_stg);
        if (dst_port < (TH4_PWRT_DEV_PORTS_PER_DEV / 2)) {
            bit_pos = 2 * dst_port;
            set_field_in_memfld(memfld, entry, bit_pos, 2);
        } else {
            bit_pos = 2 * (dst_port - (TH4_PWRT_DEV_PORTS_PER_DEV / 2));
            set_field_in_memfld(memfld, entry_b, bit_pos, 2);
        }
        if (src_port < (TH4_PWRT_DEV_PORTS_PER_DEV / 2)) {
            bit_pos = 2 * src_port;
            set_field_in_memfld(memfld, entry, bit_pos, 2);
        } else {
            bit_pos = 2 * (src_port - (TH4_PWRT_DEV_PORTS_PER_DEV / 2));
            set_field_in_memfld(memfld, entry_b, bit_pos, 2);
        }
    }

    sid = EGR_VLAN_STG_Am;
    index = get_egr_stg(flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = EGR_VLAN_STG_Bm;
    index = get_egr_stg(flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry_b));

exit:
    SHR_FUNC_EXIT();
}


/*
 * This function configures:
 * EGR_L3_NEXT_HOPm
 * EGR_L3_NEXT_HOP_2m
 * EGR_L3_INTFm
 * EGR_MAC_DA_PROFILEm
 * EGR_IP_TUNNEL_MPLSm
 */
static int setup_egr_next_hop(int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = -1;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, label_order;
    uint32_t mpls_label, mpls_exp, mpls_ttl;
    bcm_mac_t mac_addr;

    SHR_FUNC_ENTER(unit);

    sid = EGR_L3_NEXT_HOPm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));

    if (is_mpls_en(unit, flow_id, 0) == 1) {
        TH4_PWRT_SET_FIELD(ENTRY_TYPEf, 0x1);  /* (MPLS) */
        TH4_PWRT_SET_FIELD(MPLSv_MPLS_LABEL_ACTIONf, 0x1); /* PUSH */
        label_order = 3; /* BOS */
        get_mpls_label(unit, flow_id, label_order,
                   &mpls_label, &mpls_exp, &mpls_ttl, 0);
        TH4_PWRT_SET_FIELD(MPLSv_MPLS_LABELf, mpls_label);
        TH4_PWRT_SET_FIELD(MPLSv_MPLS_EXPf, mpls_exp);
        TH4_PWRT_SET_FIELD(MPLSv_MPLS_TTLf, mpls_ttl);
        TH4_PWRT_SET_FIELD(MPLSv_MAC_DA_PROFILE_INDEXf,
                           get_egr_mac_da_profile_index(unit, flow_id));
        TH4_PWRT_SET_FIELD(MPLSv_MPLS_EXP_SELECTf, 0x0);
        TH4_PWRT_SET_FIELD(MPLSv_MPLS_EXP_MAPPING_PTRf, 0x0);
        TH4_PWRT_SET_FIELD(MPLSv_LOOPBACK_PROFILE_IDXf, 1 + (sal_rand() % 3));
    } else if (is_l3_en(unit, flow_id, 0) == 1) {
        TH4_PWRT_SET_FIELD(ENTRY_TYPEf, 0x0);  /*NORMAL L3*/
        TH4_PWRT_SET_FIELD(L3v_L3_UC_DA_DISABLEf, 0x0);
        TH4_PWRT_SET_FIELD(L3v_L3_UC_SA_DISABLEf, 0x0);
        TH4_PWRT_SET_FIELD(L3v_L3_UC_TTL_DISABLEf, 0x0);
        TH4_PWRT_SET_FIELD(L3v_L3_UC_VLAN_DISABLEf, 0x0);
        TH4_PWRT_SET_FIELD(L3v_CLASS_IDf, 0x0);
        TH4_PWRT_SET_FIELD(L3v_LOOPBACK_PROFILE_IDXf, 1 + (sal_rand() % 3));
        sal_memset(memfld, 0, sizeof(memfld));
        get_mac_da(unit, flow_id, mac_addr);
        convert_mac(mac_addr, memfld);
       SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, L3v_MAC_ADDRESSf, entry, memfld));
    }
    TH4_PWRT_SET_FIELD(MPLSv_FLEX_CTR_ACTION_SELf, 0x1);
    TH4_PWRT_SET_FIELD(MPLSv_FLEX_CTR_OBJECTf, 0x7fa1);
    index = get_next_hop_index(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    sid = EGR_L3_NEXT_HOP_2m;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(INTF_NUMf, get_egr_intf_num(unit, flow_id));
    index = get_next_hop_index(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    sid = EGR_L3_INTFm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));

    if (is_mpls_en(unit, flow_id, 0) == 1) {
        get_mac_sa(unit, flow_id, mac_addr);
        convert_mac(mac_addr, memfld);
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, MAC_ADDRESSf, entry, memfld));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(VIDf, get_vlan_id(unit, flow_id));
        TH4_PWRT_SET_FIELD(MPLS_TUNNEL_INDEXf, get_egr_mpls_tunnel_index(unit, flow_id));
        TH4_PWRT_SET_FIELD(TTL_THRESHOLDf, 0x1);
        TH4_PWRT_SET_FIELD(OPRIf, 0x1);
    } else if (is_l3_en(unit, flow_id, 0) == 1) {
        sal_memset(memfld, 0, sizeof(memfld));
        get_mac_sa(unit, flow_id, mac_addr);
        convert_mac(mac_addr, memfld);
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, MAC_ADDRESSf, entry, memfld));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(VIDf, get_vlan_id(unit, flow_id));
        TH4_PWRT_SET_FIELD(TTL_THRESHOLDf, 0x1);
    }
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTIONf, 0x2);
    index = get_egr_intf_num(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));


    if (is_mpls_en(unit, flow_id, 0) == 1) {
        sid = EGR_MAC_DA_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        get_mac_da(unit, flow_id, mac_addr);
        convert_mac(mac_addr, memfld);
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, MAC_ADDRESSf, entry, memfld));
        index = get_egr_mac_da_profile_index(unit, flow_id);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

        sid = EGR_IP_TUNNEL_MPLSm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(ENTRY_TYPEf, 0x3);  /* (MPLS) */
        /* "Push Two MPLS Labels - from this entry and (N+1)." */
        TH4_PWRT_SET_FIELD(MPLS_PUSH_ACTION_0f, 0x2);
        /* "Push ONE MPLS Label - from this entry." ,*/
        TH4_PWRT_SET_FIELD(MPLS_PUSH_ACTION_1f, 0x1);
        label_order = 2; /* NOT BOS - MIDDLE label */
        get_mpls_label(unit, flow_id, label_order,
                   &mpls_label, &mpls_exp, &mpls_ttl, 0);
        TH4_PWRT_SET_FIELD(MPLS_LABEL_0f, mpls_label);
        TH4_PWRT_SET_FIELD(MPLS_EXP_0f, mpls_exp);
        TH4_PWRT_SET_FIELD(MPLS_TTL_0f, mpls_ttl);
        TH4_PWRT_SET_FIELD(NEW_PRI_0f, get_priority(unit, flow_id));
        label_order = 1; /* NOT BOS - TOP label */
        get_mpls_label(unit, flow_id, label_order,
                   &mpls_label, &mpls_exp, &mpls_ttl, 0);
        TH4_PWRT_SET_FIELD(MPLS_LABEL_1f, mpls_label);
        TH4_PWRT_SET_FIELD(MPLS_EXP_1f, mpls_exp);
        TH4_PWRT_SET_FIELD(MPLS_TTL_1f, mpls_ttl);
        TH4_PWRT_SET_FIELD(NEW_PRI_1f, get_priority(unit, flow_id));

        index = get_egr_mpls_tunnel_index(unit, flow_id);
        index = index >> 3; /* Bits[13:3] are used to index EGR_IP_TUNNEL table */
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

exit:
    SHR_FUNC_EXIT();
}



/*
 * This function configures:
 * EGR_PORTm
 * EGR_ING_PORTm
 * EGR_GPP_ATTRIBUTESm
 */
static int setup_egr_port_tables(int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;

    SHR_FUNC_ENTER(unit);

    sid = EGR_PORTm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(PORT_TYPEf, 0x0); /* ETHERNET */
    TH4_PWRT_SET_FIELD(EN_EFILTERf, 0x1);
    TH4_PWRT_SET_FIELD(EFP_FILTER_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(ALLOW_NON_GSHf, 0x1);
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTIONf, 0x0);
    /* Don't care, randomize for max toggling */
    TH4_PWRT_SET_FIELD(DCN_PROFILE_PTRf, flow_id % 4);
    TH4_PWRT_SET_FIELD(EGR_PORT_GROUP_IDf, sal_rand32() & 0x1ff);
    TH4_PWRT_SET_FIELD(LATENCY_ECN_PROFILE_PTRf, flow_id % 8);
    TH4_PWRT_SET_FIELD(MIRROR_ENCAP_INDEXf, sal_rand32() & 0x7);
    TH4_PWRT_SET_FIELD(VXLAN_VRF_LOOKUP_KEY_TYPEf, sal_rand32() & 0x1);
    index = get_dev_dst_port(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = EGR_ING_PORTm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(PORT_TYPEf, 0x0); /* ETHERNET */
    index = get_dev_src_port(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = EGR_GPP_ATTRIBUTESm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    index = get_dev_dst_port(unit, flow_id);
    /* all zeros */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


static int setup_ifp(
    int unit,
    int num_slices)
{
    bcmdrd_sid_t sid;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = -1;
    int ipep_acc;
    uint32_t pipe_map;
    uint32_t pp_map = 0;
    int pp_num, pipe;
    int ifp_num_slices, num_log_tables, i;
    int mindex, log_table;
    uint32_t counter_set, flex_ctr_action, flex_ctr_object;
    uint32_t INT_ACTION_PROFILE_IDX, LOOPBACK_PROFILE_INDEX;
    uint32_t MMU_INT_STAT_REQ_0, MMU_INT_STAT_REQ_1;
    power_test_t *power_test_p = power_test_parray[unit];
    uint32_t DEST_TYPE, DESTINATION, flex_ctr_indx;
    int flow_id, num_flows, start_flow;

    static const bcmdrd_fid_t logical_table_action_priority[16] = {
        LOGICAL_TABLE_0_ACTION_PRIORITYf, LOGICAL_TABLE_1_ACTION_PRIORITYf,
        LOGICAL_TABLE_2_ACTION_PRIORITYf, LOGICAL_TABLE_3_ACTION_PRIORITYf,
        LOGICAL_TABLE_4_ACTION_PRIORITYf, LOGICAL_TABLE_5_ACTION_PRIORITYf,
        LOGICAL_TABLE_6_ACTION_PRIORITYf, LOGICAL_TABLE_7_ACTION_PRIORITYf,
        LOGICAL_TABLE_8_ACTION_PRIORITYf, LOGICAL_TABLE_9_ACTION_PRIORITYf,
        LOGICAL_TABLE_10_ACTION_PRIORITYf, LOGICAL_TABLE_11_ACTION_PRIORITYf,
        LOGICAL_TABLE_12_ACTION_PRIORITYf, LOGICAL_TABLE_13_ACTION_PRIORITYf,
        LOGICAL_TABLE_14_ACTION_PRIORITYf, LOGICAL_TABLE_15_ACTION_PRIORITYf
    };

    bcmlt_entry_handle_t lt_entry = BCMLT_INVALID_HDL;
    uint64_t fld_memfld_64[BCMDRD_MAX_PT_WSIZE];
    uint64_t memfld_64[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    ifp_num_slices = num_slices;
    num_log_tables = num_slices;
    cli_out("setup_ifp() ifp_num_slices=%0d num_log_tables=%0d \n",
        ifp_num_slices, num_log_tables);

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));

    for (pipe = 0; pipe < TH4_PWRT_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            pp_num = pipe / 4;
            pp_map |= (1 << pp_num);
        }
    }

    /* IFP_LOGICAL_TABLE_SELECT_CONFIGr */
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            sid = IFP_LOGICAL_TABLE_SELECT_CONFIGr;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(SOURCE_CLASS_MODEf, 0);
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT
                (th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

            sid = EXACT_MATCH_LOGICAL_TABLE_SELECT_CONFIGr;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(SOURCE_CLASS_MODEf, 0);
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT
                (th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

            for (i = 0; i < ifp_num_slices; i++) {
                sid = IFP_CONFIGr;
                sal_memset(entry, 0, sizeof(entry));
                sal_memset(memfld, 0, sizeof(memfld));
                TH4_PWRT_SET_FIELD(IFP_SLICE_ENABLEf, 0x1);
                TH4_PWRT_SET_FIELD(IFP_SLICE_LOOKUP_ENABLEf, 0x1);
                TH4_PWRT_PT_DYN_INFO(pt_dyn_info, i, ipep_acc);
                SHR_IF_ERR_EXIT
                    (th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
            }
        }
    }

    /* One to one priority */
    /* IFP_LOGICAL_TABLE_ACTION_PRIORITYm */
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            sid = IFP_LOGICAL_TABLE_ACTION_PRIORITYm;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            for (i = 0; i < 16; i++) {
                TH4_PWRT_SET_FIELD(logical_table_action_priority[i], i);
            }
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit,
                                sid, ltid, &pt_dyn_info, entry));
        }
    }


    /* IFP_LOGICAL_TABLE_SELECTm */
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            for (log_table = 0; log_table < num_log_tables; log_table++) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_allocate(unit, "IFP_LOGICAL_TABLE_SELECTm", &lt_entry));
                SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "VALID", 0x3));
                SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "ENABLE", 0x1));
                SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LOGICAL_TABLE_ID", log_table));
                SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "KEY_GEN_PROGRAM_PROFILE_INDEX", log_table));
                SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LOGICAL_TABLE_CLASS_ID", 0x2));
                SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "SOURCE_CLASS_MASK", 0xff));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(lt_entry, "SOURCE_CLASS", get_port_tab_class_id(unit, log_table)));
                SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "DST_CONTAINER_0_SEL", 0x2));
                mindex = (log_table * 16) + 1;
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INSTANCE", ipep_acc));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INDEX", mindex));
                SHR_IF_ERR_EXIT
                    (bcmlt_pt_entry_commit(lt_entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
                bcmlt_entry_free(lt_entry);
            }
        }
    }


    /* IFP_LOGICAL_TABLE_CONFIG */
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            for (log_table = 0; log_table < num_log_tables; log_table++) {
                TH4_PWRT_PT_DYN_INFO(pt_dyn_info, log_table, ipep_acc);
                sid = IFP_LOGICAL_TABLE_CONFIGr;
                sal_memset(entry, 0, sizeof(entry));
                sal_memset(memfld, 0, sizeof(memfld));
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_MAPf, 0x1 << log_table);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_0f, 0x1);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_1f, 0x2);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_2f, 0x3);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_3f, 0x4);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_4f, 0x5);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_5f, 0x6);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_6f, 0x7);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_7f, 0x8);
                TH4_PWRT_SET_FIELD(LOGICAL_PARTITION_PRIORITY_8f, 0x9);
                SHR_IF_ERR_EXIT
                    (th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
            }
        }
    }

    /* IFP_KEY_GEN_PROGRAM_PROFILEm */
    /* IFP_KEY_GEN_PROGRAM_PROFILE2m */
    /* this selector settings will select Ether type from L2 Header */
    for (log_table = 0; log_table < num_log_tables; log_table++) {
        sid = IFP_KEY_GEN_PROGRAM_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        /*set_rand_mem(327, entry);*/
        TH4_PWRT_SET_FIELD(L1_C_E16_SEL_0f, 0x12);
        TH4_PWRT_SET_FIELD(L2_E16_SEL_0f, 0x4);
        mindex = log_table;
        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                TH4_PWRT_PT_DYN_INFO(pt_dyn_info, mindex, ipep_acc);
                SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit,
                    sid, ltid, &pt_dyn_info, entry));
            }
        }
        sid = IFP_KEY_GEN_PROGRAM_PROFILE2m;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        /*set_rand_mem(16, entry);*/
        TH4_PWRT_SET_FIELD(POST_EXTRACTOR_MUX_7_SELf, 0x1);
        mindex = log_table;
        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                TH4_PWRT_PT_DYN_INFO(pt_dyn_info, mindex, ipep_acc);
                SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit,
                    sid, ltid, &pt_dyn_info, entry));
            }
        }
    }

    num_flows = power_test_p->num_flows;
    start_flow = power_test_p->start_flow;

    for (flow_id = start_flow; flow_id < (start_flow + num_flows); flow_id++) {
        /* IFP_TCAM */
        for (log_table = 0; log_table < num_log_tables; log_table++) {

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "IFP_TCAMm", &lt_entry));
            SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "VALID", 0x3));

            sal_memset(memfld_64, 0, sizeof(memfld_64));
            sal_memset(fld_memfld_64, 0, sizeof(fld_memfld_64));
            fld_memfld_64[0] = get_ether_type(unit, 0);
            set_field_in_memfld_64(fld_memfld_64, memfld_64, 0, 16);
            get_lpm_dest_type(unit, flow_id, &DEST_TYPE, &DESTINATION);
            fld_memfld_64[0] = (DESTINATION << 0) | (DEST_TYPE << 15);
            set_field_in_memfld_64(fld_memfld_64, memfld_64, 124, 16);
            SHR_IF_ERR_EXIT(bcmlt_entry_field_array_add(lt_entry, "KEY", 0, memfld_64, 3));

            fld_memfld_64[0] = 0xffff;
            set_field_in_memfld_64(fld_memfld_64, memfld_64, 0, 16);
            set_field_in_memfld_64(fld_memfld_64, memfld_64, 124, 16);
            SHR_IF_ERR_EXIT(bcmlt_entry_field_array_add(lt_entry, "MASK", 0, memfld_64, 3));

            /* physical slices 0-5 are 256 entries, while slices 6-8 are 512 entries */
            mindex = (log_table <= 5) ? log_table << 8 :
                                        (6 << 8) + ((log_table % 6) << 9);
            mindex = mindex + (flow_id % 256);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INDEX", mindex));

            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    SHR_IF_ERR_EXIT
                        (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INSTANCE", ipep_acc));
                    SHR_IF_ERR_EXIT
                        (bcmlt_pt_entry_commit(lt_entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
                }
            }
            bcmlt_entry_free(lt_entry);
        }

        /* IFP_POLICY_TABLEm */
        for (log_table = 0; log_table < num_log_tables; log_table++) {
            sid = IFP_POLICY_TABLEm;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(TTL_OVERRIDE_SETf, 0x1);
            flex_ctr_indx = (flow_id + log_table) % 9;
            flex_ctr_action = (flex_ctr_indx == 0) ? 0 : 12 + flex_ctr_indx;
            flex_ctr_object = 0x3a0c + log_table;
            counter_set = (flex_ctr_action << 0) |
                          (flex_ctr_object << 5) |
                          (0x1 << 19);
            TH4_PWRT_SET_FIELD(COUNTER_SETf, counter_set);
            /* alternate between buf usage & buf avail*/
            MMU_INT_STAT_REQ_0 = 0; /* ((sal_rand() & 0x1) == 0) ? 1 : 3;*/
            MMU_INT_STAT_REQ_1 = 0; /*((sal_rand() & 0x1) == 0) ? 1 : 3;*/
            /* INT_ACTION_PROFILE_IDX !=0 */
            INT_ACTION_PROFILE_IDX = 0x0; /* (1 + (sal_rand32() % 15)); */
            LOOPBACK_PROFILE_INDEX = sal_rand() & 0x3;
            counter_set = (INT_ACTION_PROFILE_IDX << 2) |
                          (LOOPBACK_PROFILE_INDEX <<  24) |
                          (MMU_INT_STAT_REQ_0 << 26) |
                          (MMU_INT_STAT_REQ_1 << 29);
            TH4_PWRT_SET_FIELD(INBAND_TELEMETRY_SETf, counter_set);

            counter_set = (sal_rand32() & 0xfc); /* CHANGE_CPU_COS */
            TH4_PWRT_SET_FIELD(CHANGE_CPU_COS_SETf, counter_set);
            counter_set = (sal_rand32() & 0x1ff7f); /* DELAYED_REDIRECT_ENABLE=0 */
            TH4_PWRT_SET_FIELD(DELAYED_REDIRECT_SETf, counter_set);
            counter_set = (sal_rand32() & 0x7f); /* DELAYED_DROP_ENABLE = 0 */
            TH4_PWRT_SET_FIELD(DELAYED_DROP_SETf, counter_set);
            counter_set = (sal_rand32() & 0x3fe); /* DLB_ALTERNATE_PATH_ACTION_VALID = 0 */
            TH4_PWRT_SET_FIELD(DLB_ALTERNATE_PATH_CONTROL_SETf, counter_set);
            counter_set = (sal_rand32() & 0x3f); /* POLICY_MIRROR_FLOW_CLASS_ENABLE = 0 */
            TH4_PWRT_SET_FIELD(IFP_MULTIPLE_MIRROR_DESTINATION_SETf, counter_set);

            if (((power_test_p->enable_map >> TH4_PWRT_EN_RND_PRI) & 0x1) == 0x1) {
                /* CHANGE_INPUT_PRIORITY=1
                 * G_CHANGE_COS_OR_INT_PRI = CHANGE_UC_COS,
                 * G_CHANGE_INT_CN = 1
                 * G_DROP_PRECEDENCE = MARK_GREEN
                 * G_COS_INT_PRI = 0
                 * G_NEW_INT_CN
                 * NEW_INPUT_PRIORITY = 0
                 */
                counter_set = (0x1 << 0) | (0x8 << 1) | (0x1 << 5) | (0x1 << 14);
                TH4_PWRT_SET_FIELD(PROFILE_SET_1f, counter_set);
                /* G_CHANGE_DSCP_TOS = CHANGE_DSCP,
                 * G_CHANGE_ECN =1,
                 * G_CHANGE_PKT_PRI = 0
                 */
                counter_set = (0x3 << 0) | (0x1 << 3) | (0x5 << 4);
                TH4_PWRT_SET_FIELD(PROFILE_SET_2f, counter_set);
            }

            /* physical slices 0-5 are 256 entries, while slices 6-8 are 512 entries */
            mindex = (log_table <= 5) ? log_table << 8 :
                                   (6 << 8) + ((log_table % 6) << 9);
            mindex = mindex + (flow_id % 256);
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, mindex, ipep_acc);
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit,
                                sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int setup_vfp(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    uint32_t pp_map = 0;
    int ipep_acc;
    int index;
    bcm_mac_t mac_addr;
    power_test_t *power_test_p = power_test_parray[unit];
    int flow_id, num_flows, slice, start_flow;
    bcmlt_entry_handle_t lt_entry = BCMLT_INVALID_HDL;
    uint64_t memfld_64[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(get_pp_map(unit, &pp_map));

    sid = VFP_SLICE_CONTROLr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(LOOKUP_ENABLE_SLICE_0f, 0x1);
    TH4_PWRT_SET_FIELD(LOOKUP_ENABLE_SLICE_1f, 0x1);
    TH4_PWRT_SET_FIELD(LOOKUP_ENABLE_SLICE_2f, 0x1);
    TH4_PWRT_SET_FIELD(LOOKUP_ENABLE_SLICE_3f, 0x1);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = VFP_KEY_CONTROL_1r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(SLICE_0_F2f, 0x4);
    TH4_PWRT_SET_FIELD(SLICE_1_F2f, 0x4);
    TH4_PWRT_SET_FIELD(SLICE_2_F2f, 0x4);
    TH4_PWRT_SET_FIELD(SLICE_3_F2f, 0x4);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = VFP_SLICE_MAPr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBERf, 0x0);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBERf, 0x1);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBERf, 0x2);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBERf, 0x3);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUPf, 0x0);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUPf, 0x1);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUPf, 0x2);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUPf, 0x3);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }


    num_flows = power_test_p->num_flows;
    start_flow = power_test_p->start_flow;
    for (flow_id = start_flow; flow_id < (start_flow + num_flows); flow_id++) {
        if ((flow_id % 2 ) == 1) { continue; }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "VFP_TCAMm", &lt_entry));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(lt_entry, "VALID", 0x3));
        sal_memset(memfld_64, 0, sizeof(memfld_64));
        memfld_64[0] = 0x0;
        memfld_64[1] = 0x00ffffffffffff00;
        SHR_IF_ERR_EXIT(bcmlt_entry_field_array_add(lt_entry, "F2_MASK", 0, memfld_64, 2));
        get_mac_sa(unit, flow_id, mac_addr);
        memfld_64[1] = ((uint64_t)mac_addr[0] << 16) | ((uint64_t)mac_addr[1] <<  8) | ((uint64_t)mac_addr[2] << 0);
        memfld_64[1] = memfld_64[1] << 32;
        memfld_64[1] |= ((uint64_t)mac_addr[3] << 24) | ((uint64_t)mac_addr[4] << 16) | ((uint64_t)mac_addr[5] << 8);
        SHR_IF_ERR_EXIT(bcmlt_entry_field_array_add(lt_entry, "F2", 0, memfld_64, 2));
        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                for (slice = 0; slice < 8; slice++) {
                    index = (128 * slice) + (flow_id / 2);
                    SHR_IF_ERR_EXIT
                        (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INSTANCE", ipep_acc));
                    SHR_IF_ERR_EXIT
                        (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INDEX", index));
                    SHR_IF_ERR_EXIT
                        (bcmlt_pt_entry_commit(lt_entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
                }
            }
        }
        bcmlt_entry_free(lt_entry);

        sid = VFP_POLICY_TABLEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(USE_VFP_CLASS_ID_Hf, 0x1);
        TH4_PWRT_SET_FIELD(USE_VFP_CLASS_ID_Lf, 0x1);
        TH4_PWRT_SET_FIELD(VFP_CLASS_ID_Hf, 0x300 + flow_id); /* just dummy */
        TH4_PWRT_SET_FIELD(VFP_CLASS_ID_Lf, 0x200 + flow_id);
        TH4_PWRT_SET_FIELD(VFP_MATCHED_RULEf, 0x30+flow_id);
        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                for (slice = 0; slice < 8; slice++) {
                    index = (128 * slice) + (flow_id / 2);
                    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTIONf, 24 + (slice / 2));
                    TH4_PWRT_SET_FIELD(FLEX_CTR_OBJECTf, 0x3a80 + (slice / 2));
                    TH4_PWRT_SET_FIELD(HASH_FIELD_BITMAP_PTR_Af, 1 + (sal_rand() % 15));
                    TH4_PWRT_SET_FIELD(HASH_FIELD_BITMAP_PTR_Bf, 1 + (sal_rand() % 15));
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int setup_efp(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    uint32_t pp_map = 0;
    int index, ipep_acc;
    power_test_t *power_test_p = power_test_parray[unit];
    int flow_id, num_flows, slice, start_flow;
    bcm_mac_t mac_addr;
    bcmlt_entry_handle_t lt_entry = BCMLT_INVALID_HDL;
    uint64_t fld_memfld_64[BCMDRD_MAX_PT_WSIZE];
    uint64_t memfld_64[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(get_pp_map(unit, &pp_map));

    sid = EFP_SLICE_CONTROLr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(SLICE_ENABLE_SLICE_0f, 0x1);
    TH4_PWRT_SET_FIELD(SLICE_ENABLE_SLICE_1f, 0x1);
    TH4_PWRT_SET_FIELD(SLICE_ENABLE_SLICE_2f, 0x1);
    TH4_PWRT_SET_FIELD(SLICE_ENABLE_SLICE_3f, 0x1);
    TH4_PWRT_SET_FIELD(LOOKUP_ENABLE_SLICE_0f, 0x1);
    TH4_PWRT_SET_FIELD(LOOKUP_ENABLE_SLICE_1f, 0x1);
    TH4_PWRT_SET_FIELD(LOOKUP_ENABLE_SLICE_2f, 0x1);
    TH4_PWRT_SET_FIELD(LOOKUP_ENABLE_SLICE_3f, 0x1);
    TH4_PWRT_SET_FIELD(SLICE_0_MODEf, 0x1);
    TH4_PWRT_SET_FIELD(SLICE_1_MODEf, 0x1);
    TH4_PWRT_SET_FIELD(SLICE_2_MODEf, 0x1);
    TH4_PWRT_SET_FIELD(SLICE_3_MODEf, 0x1);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }


    /* All below are 0s - DON'T CARE
    EFP_CLASSID_SELECTORr
    EFP_KEY4_L3_CLASSID_SELECTORr
    EFP_KEY8_L3_CLASSID_SELECTORr
    EFP_KEY4_MDL_SELECTORr
    */

    sid = EFP_SLICE_MAPr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBERf, 0x0);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBERf, 0x1);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBERf, 0x2);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBERf, 0x3);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUPf, 0x0);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUPf, 0x1);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUPf, 0x2);
    TH4_PWRT_SET_FIELD(VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUPf, 0x3);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }


    num_flows = power_test_p->num_flows;
    start_flow = power_test_p->start_flow;
    for (flow_id = start_flow; flow_id < (start_flow + num_flows); flow_id++) {
        if ((flow_id % 2 ) == 1) { continue; }

        /*
         * KEYf  (240b)
         * key[`EFP_KEY4__KEY_MATCH_TYPE] = `EFP_KEY4__KEY_MATCH_TYPE_L2_SINGLE;
         * key[`EFP_KEY4__NUMBER_OF_TAGS] = 2;
         * key[`EFP_KEY4__ING_PORT_ID] = enet_src_port[flow_no];
         */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "EFP_TCAMm", &lt_entry));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(lt_entry, "VALID", 0x3));

        sal_memset(memfld_64, 0, sizeof(memfld_64));
        sal_memset(fld_memfld_64, 0, sizeof(fld_memfld_64));

        /* EFP_KEY4__KEY_MATCH_TYPE        3:0 */
        fld_memfld_64[0] = 5; /* MATCH_TYPE_L2_SINGLE */
        set_field_in_memfld_64(fld_memfld_64, memfld_64, 0, 4);
        sal_memset(fld_memfld_64, 0, sizeof(fld_memfld_64));
        /* EFP_KEY4__NUMBER_OF_TAGS        203:202 */
        fld_memfld_64[0] = 2; /* NUMBER OF TAGS */
        set_field_in_memfld_64(fld_memfld_64, fld_memfld_64, 202, 2);
        sal_memset(fld_memfld_64, 0, sizeof(fld_memfld_64));
        /* EFP_KEY4__MAC_SA        200:153 */
        get_mac_sa(unit, flow_id, mac_addr);
        fld_memfld_64[0] = (mac_addr[0] << 8) | (mac_addr[1] << 0);
        fld_memfld_64[0] = fld_memfld_64[0] << 32;
        fld_memfld_64[0] |= (mac_addr[2] << 24) | (mac_addr[3] << 16) | (mac_addr[4] << 8) | (mac_addr[5] << 0);
        set_field_in_memfld_64(fld_memfld_64, memfld_64, 153, 48);
        SHR_IF_ERR_EXIT(bcmlt_entry_field_array_add(lt_entry, "KEY", 0, memfld_64, 4));

        sal_memset(memfld_64, 0, sizeof(memfld_64));
        sal_memset(fld_memfld_64, 0, sizeof(fld_memfld_64));
        fld_memfld_64[0] = 0xf; /* MATCH_TYPE_L2_SINGLE */
        set_field_in_memfld_64(fld_memfld_64, memfld_64, 0, 4);
        sal_memset(fld_memfld_64, 0, sizeof(fld_memfld_64));
        fld_memfld_64[0] = 3; /* NUMBER OF TAGS */
        set_field_in_memfld_64(fld_memfld_64, fld_memfld_64, 202, 2);
        sal_memset(fld_memfld_64, 0, sizeof(fld_memfld_64));
        fld_memfld_64[0] = 0x0000ffffffffffff;
        set_field_in_memfld_64(fld_memfld_64, memfld_64, 153, 48);
        SHR_IF_ERR_EXIT(bcmlt_entry_field_array_add(lt_entry, "MASK", 0, memfld_64, 4));

        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                for (slice = 0; slice < 4; slice++) {
                    index = (128 * slice) + (flow_id / 2);
                    SHR_IF_ERR_EXIT
                        (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INSTANCE", ipep_acc));
                    SHR_IF_ERR_EXIT
                        (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INDEX", index));
                    SHR_IF_ERR_EXIT
                        (bcmlt_pt_entry_commit(lt_entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
                }
            }
        }
        bcmlt_entry_free(lt_entry);


        sid = EFP_POLICY_TABLEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        /* FLEX_CTR_ACTION
         * FLEX_CTR_OBJECT
         */
        TH4_PWRT_SET_FIELD(ENTRY_TYPEf, 0x0); /* DEFAULT */

        sal_memset(memfld, 0, sizeof(memfld));
        memfld[0] = (flow_id & 0xff) | ((flow_id & 0xff) << 8) |
                    ((flow_id & 0xff) << 16) | ((flow_id & 0xff) << 24);
        memfld[1] = 0x1f;
        SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, DEFAULTv_RESERVED_PADDINGf, entry, memfld));

        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                for (slice = 0; slice < 4; slice++) {
                    index = (128 * slice) + (flow_id / 2);
                    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTIONf, 0x8 + slice);
                    TH4_PWRT_SET_FIELD(FLEX_CTR_OBJECTf, 0x1f0 + slice);
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int setup_elephant_trap(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    uint32_t pp_map = 0;
    int ipep_acc, index, i;
    static const bcmdrd_sid_t etrap_flow_ctrl_tables[4] = {
        ETRAP_FLOW_CTRL_LEFT_TABLE_INST0m,
        ETRAP_FLOW_CTRL_RIGHT_TABLE_INST0m,
        ETRAP_FLOW_CTRL_LEFT_TABLE_INST1m,
        ETRAP_FLOW_CTRL_RIGHT_TABLE_INST1m
    };
    static const bcmdrd_sid_t etrap_event_fifo_ctrl_regs[2] = {
        ETRAP_EVENT_FIFO_CTRL_INST0r,
        ETRAP_EVENT_FIFO_CTRL_INST1r
    };


    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(get_pp_map(unit, &pp_map));

    sid = ETRAP_LKUP_EN_ING_PORTm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    set_dev_bmp_all_one(memfld);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, ING_PORT_BMP_ENf, entry, memfld));
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_MONITOR_CONFIG_INST0r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ETRAP_MONITOR_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(SEEDf, 0x123);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = ETRAP_MONITOR_CONFIG_INST1r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ETRAP_MONITOR_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(SEEDf, 0x123);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = ETRAP_LKUP_EN_INT_PRIr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(INT_PRI_BMP_ENf, 0xffff);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_LKUP_EN_PKT_TYPEr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(PKT_TYPE_OTHER_ENf, 0x1);
    TH4_PWRT_SET_FIELD(MPLS_ENf, 0x1);
    TH4_PWRT_SET_FIELD(IPV6_UDP_ENf, 0x1);
    TH4_PWRT_SET_FIELD(IPV6_TCP_ENf, 0x1);
    TH4_PWRT_SET_FIELD(IPV6_OTHER_ENf, 0x1);
    TH4_PWRT_SET_FIELD(IPV4_UDP_ENf, 0x1);
    TH4_PWRT_SET_FIELD(IPV4_TCP_ENf, 0x1);
    TH4_PWRT_SET_FIELD(IPV4_OTHER_ENf, 0x1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_PROC_ENr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ETRAP_ENf, 0x1);
    TH4_PWRT_SET_FIELD(BLOOM_FLOW_INS_ENf, 0x1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* ETRAP_PROC_EN_2r::COLOR_ACTION_ENf stays on 0 */

    sid = ETRAP_FILT_THRESHr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(THRESHOLD_BYTESf, 0xb);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_FLOW_ELEPH_THRESHOLDr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BYTE_THRESHOLDf, 0x18);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_FLOW_RESET_THRESHOLDr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BYTE_THRESHOLDf, 0x1b);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_FLOW_ELEPH_THR_YELr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BYTE_THRESHOLDf, 0x1b);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_FLOW_ELEPH_THR_REDr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BYTE_THRESHOLDf, 0x1b);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_FLOW_CFGr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(CRITICAL_TIME_PERIODf, 0x5); /* 500us */
    TH4_PWRT_SET_FIELD(ETRAP_INTERVALf, 0x1);  /* 2ms */
    TH4_PWRT_SET_FIELD(RIGHT_BANK_HASH_ROTRf, 0x2);
    TH4_PWRT_SET_FIELD(RIGHT_BANK_HASH_SELf, 0x0);
    TH4_PWRT_SET_FIELD(LEFT_BANK_HASH_ROTRf, 0x2);
    TH4_PWRT_SET_FIELD(LEFT_BANK_HASH_SELf, 0x0);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    sid = ETRAP_FILT_CFGr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(FILTER_0_HASH_ROTRf, 0x4);
    TH4_PWRT_SET_FIELD(FILTER_0_HASH_SELf, 0x0);
    TH4_PWRT_SET_FIELD(FILTER_1_HASH_ROTRf, 0x3);
    TH4_PWRT_SET_FIELD(FILTER_1_HASH_SELf, 0x1);
    TH4_PWRT_SET_FIELD(FILTER_2_HASH_ROTRf, 0x4);
    TH4_PWRT_SET_FIELD(FILTER_2_HASH_SELf, 0x3);
    TH4_PWRT_SET_FIELD(FILTER_3_HASH_ROTRf, 0x2);
    TH4_PWRT_SET_FIELD(FILTER_3_HASH_SELf, 0x2);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

    /* ETRAP_FLOW_CTRL_LEFT/RIGHT_TABLE_INST0/1 */
    if (0) {
        for (index = 0; index < 256; index ++) {
            for (i = 0; i < 4; i++) {
                sid = etrap_flow_ctrl_tables[i];
                sal_memset(entry, 0, sizeof(entry));
                sal_memset(memfld, 0, sizeof(memfld));
                set_rand_mem(35, entry);
                for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                    if (pp_map & (1 << ipep_acc)) {
                        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
                        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                            unit, sid, ltid, &pt_dyn_info, entry));
                    }
                }
            }
        }
    }

    /* Enable reporting in ETRAP FIFO */
    for (i = 0; i< 2; i++) {
        sid = etrap_event_fifo_ctrl_regs[i];
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(REPORT_ELEPHANTSf, 0x1);
        TH4_PWRT_SET_FIELD(REPORT_EVICTIONSf, 0x1);
        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
                SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                    unit, sid, ltid, &pt_dyn_info, entry));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int setup_exact_match(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    uint32_t pp_map = 0;
    int ipep_acc, index;
    int log_table = 0;
    int em0_key_profile_index;
    int em1_key_profile_index;
    bcmlt_entry_handle_t lt_entry = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(get_pp_map(unit, &pp_map));

    sid = EXACT_MATCH_LOGICAL_TABLE_SELECT_CONFIGr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(SOURCE_CLASS_MODEf, 0x0);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    /* ** EM lookup 0 */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EXACT_MATCH_LOGICAL_TABLE_SELECTm", &lt_entry));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "VALID", 0x3));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "ENABLE", 0x1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "KEY_TYPE", 0x0)); /* 128b */
    em0_key_profile_index = 0xd;
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "KEY_GEN_PROGRAM_PROFILE_INDEX", em0_key_profile_index));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LOGICAL_TABLE_ID", 0x8));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LOGICAL_TABLE_CLASS_ID", 0x1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "SOURCE_CLASS_MASK", 0xff));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "SOURCE_CLASS", get_port_tab_class_id(unit, log_table)));
    index = 2;
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INSTANCE", ipep_acc));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INDEX", index));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(lt_entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        }
    }
    bcmlt_entry_free(lt_entry);

    sid = EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(L1_C_E16_SEL_0f, 0x12);
    TH4_PWRT_SET_FIELD(L2_E16_SEL_0f, 0x4);
    index = em0_key_profile_index; /* matches KEY_GEN_PROGRAM_PROFILE_INDEXf */
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = EXACT_MATCH_KEY_GEN_MASKm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(SEARCH_MASKf, 0xffff);
    index = em0_key_profile_index; /* matches KEY_GEN_PROGRAM_PROFILE_INDEXf */
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = EXACT_MATCH_2m;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(KEY_TYPEf, 0x0); /* MODE128 */
    TH4_PWRT_SET_FIELD(BASE_VALID_0f, 0x1); /* START_OF_KEY_BM_0*/
    TH4_PWRT_SET_FIELD(BASE_VALID_1f, 0x2); /* key continues; no key_type in this entry */
    TH4_PWRT_SET_FIELD(DATA_TYPEf, 0x1); /* index in EXACT_MATCH_ACTION_PROFILEm */
    TH4_PWRT_SET_FIELD(MODE128v_KEY_0_ONLYf, get_ether_type(unit, 0));
    index = 0;  /* This will be insert */
    index = 0x1cbe;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_insert(unit, sid, entry));


    sid = EXACT_MATCH_ACTION_PROFILEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ACTION_SET_BITMAPf, 0x8);
    TH4_PWRT_SET_FIELD(COUNTER_SET_ENABLEf, 0x1);
    index = 1;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));


    /* ** EM lookup 1 */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EXACT_MATCH_LOGICAL_TABLE_SELECTm", &lt_entry));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "VALID", 0x3));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "ENABLE", 0x1));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "KEY_TYPE", 0x2)); /* 320b */
    em1_key_profile_index = 0x9;
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "KEY_GEN_PROGRAM_PROFILE_INDEX", em1_key_profile_index));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LOGICAL_TABLE_ID", 0x6));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "LOGICAL_TABLE_CLASS_ID", 0x3));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "SOURCE_CLASS_MASK", 0xff));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(lt_entry, "SOURCE_CLASS", get_port_tab_class_id(unit, log_table)));
    index = 16 + 5;
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INSTANCE", ipep_acc));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(lt_entry, "BCMLT_PT_INDEX", index));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(lt_entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        }
    }
    bcmlt_entry_free(lt_entry);

    sid = EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(L1_C_E16_SEL_0f, 0x14);
    TH4_PWRT_SET_FIELD(L1_C_E32_SEL_0f, 0x3);
    TH4_PWRT_SET_FIELD(L2_E16_SEL_4f, 0x4);
    index = em1_key_profile_index; /* matches KEY_GEN_PROGRAM_PROFILE_INDEXf */
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = EXACT_MATCH_KEY_GEN_MASKm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xfff0ffff;
    memfld[1] = 0xffffffff;
    memfld[2] = 0x000fffff;
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, SEARCH_MASKf, entry, memfld));
    index = em1_key_profile_index; /* matches KEY_GEN_PROGRAM_PROFILE_INDEXf */
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = EXACT_MATCH_4m;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(KEY_TYPEf, 0x0); /* MODE 320 */
    TH4_PWRT_SET_FIELD(BASE_VALID_0f, 0x1); /* start of key */
    TH4_PWRT_SET_FIELD(BASE_VALID_1f, 0x2); /* key continues; no key_type in this entry */
    TH4_PWRT_SET_FIELD(BASE_VALID_2f, 0x2); /* key continues; no key_type in this entry */
    TH4_PWRT_SET_FIELD(BASE_VALID_3f, 0x7); /* key continues; no key_type in this entry */
    TH4_PWRT_SET_FIELD(DATA_TYPEf, 0xf); /* index in EXACT_MATCH_ACTION_PROFILEm */
    sal_memset(memfld, 0, sizeof(memfld));
    memfld[0] = 0xfff0ffff;
    memfld[1] = 0xffffffff;
    memfld[2] = 0x000fffff;
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, MODE320v_KEY_0_ONLYf, entry, memfld));
    index = 0;  /* This will be insert */
    index = 0x1ecc;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_insert(unit, sid, entry));

    sid = EXACT_MATCH_ACTION_PROFILEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(ACTION_SET_BITMAPf, 0xc);
    TH4_PWRT_SET_FIELD(COPY_TO_CPU_SET_ENABLEf, 0x1);
    TH4_PWRT_SET_FIELD(COUNTER_SET_ENABLEf, 0x1);
    index = 0xf; /* EXACT_MATCH_4m::DATA_TYPE */
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


static int setup_lag_fast_trunk(int unit, int flow_id)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index;

    SHR_FUNC_ENTER(unit);

    sid = FAST_TRUNK_SIZEm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(TG_SIZEf, 0x0); /* always select TRUNK_MEMBER_0 */
    TH4_PWRT_SET_FIELD(TRUNK_MODEf, 0x0); /* RTAG7 HASH */
    index = flow_id;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

    sid = FAST_TRUNK_PORTS_1m;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    set_rand_mem(296, entry);
    TH4_PWRT_SET_FIELD(TRUNK_MEMBER_0f, get_final_dst_dev_port(unit, flow_id));
    index = flow_id;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

    sid = FAST_TRUNK_PORTS_2m;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    set_rand_mem(288, entry);
    index = flow_id;
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


static int setup_flex_ctr(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    uint32_t pp_map = 0;
    int ipep_acc;
    uint32_t ing_action_enable;
    uint32_t egr_action_enable;
    int pool_no, action, total_actions;
    int flex_ctr_pool_num, index;
    power_test_t *power_test_p = power_test_parray[unit];

    static const bcmdrd_sid_t ing_flex_ctr_update_ctrl[24] = {
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_0_INST0r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_1_INST0r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_2_INST0r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_3_INST0r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_4_INST0r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_5_INST0r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_6_INST0r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_7_INST0r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_8_INST0r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_9_INST0r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_10_INST0r,  FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_11_INST0r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_0_INST1r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_1_INST1r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_2_INST1r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_3_INST1r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_4_INST1r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_5_INST1r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_6_INST1r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_7_INST1r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_8_INST1r,   FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_9_INST1r,
        FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_10_INST1r,  FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_11_INST1r
    };

    static const bcmdrd_sid_t egr_flex_ctr_update_ctrl[16] = {
        FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_0_INST0r,   FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_1_INST0r,
        FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_2_INST0r,   FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_3_INST0r,
        FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_4_INST0r,   FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_5_INST0r,
        FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_6_INST0r,   FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_7_INST0r,
        FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_0_INST1r,   FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_1_INST1r,
        FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_2_INST1r,   FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_3_INST1r,
        FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_4_INST1r,   FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_5_INST1r,
        FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_6_INST1r,   FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_7_INST1r
    };

    static const bcmdrd_sid_t ing_flex_ctr_table[24] = {
        FLEX_CTR_ING_COUNTER_TABLE_0_INST0m,   FLEX_CTR_ING_COUNTER_TABLE_1_INST0m,
        FLEX_CTR_ING_COUNTER_TABLE_2_INST0m,   FLEX_CTR_ING_COUNTER_TABLE_3_INST0m,
        FLEX_CTR_ING_COUNTER_TABLE_4_INST0m,   FLEX_CTR_ING_COUNTER_TABLE_5_INST0m,
        FLEX_CTR_ING_COUNTER_TABLE_6_INST0m,   FLEX_CTR_ING_COUNTER_TABLE_7_INST0m,
        FLEX_CTR_ING_COUNTER_TABLE_8_INST0m,   FLEX_CTR_ING_COUNTER_TABLE_9_INST0m,
        FLEX_CTR_ING_COUNTER_TABLE_10_INST0m,  FLEX_CTR_ING_COUNTER_TABLE_11_INST0m,
        FLEX_CTR_ING_COUNTER_TABLE_0_INST1m,   FLEX_CTR_ING_COUNTER_TABLE_1_INST1m,
        FLEX_CTR_ING_COUNTER_TABLE_2_INST1m,   FLEX_CTR_ING_COUNTER_TABLE_3_INST1m,
        FLEX_CTR_ING_COUNTER_TABLE_4_INST1m,   FLEX_CTR_ING_COUNTER_TABLE_5_INST1m,
        FLEX_CTR_ING_COUNTER_TABLE_6_INST1m,   FLEX_CTR_ING_COUNTER_TABLE_7_INST1m,
        FLEX_CTR_ING_COUNTER_TABLE_8_INST1m,   FLEX_CTR_ING_COUNTER_TABLE_9_INST1m,
        FLEX_CTR_ING_COUNTER_TABLE_10_INST1m,  FLEX_CTR_ING_COUNTER_TABLE_11_INST1m
    };

    static const bcmdrd_sid_t egr_flex_ctr_table[16] = {
        FLEX_CTR_EGR_COUNTER_TABLE_0_INST0m,   FLEX_CTR_EGR_COUNTER_TABLE_1_INST0m,
        FLEX_CTR_EGR_COUNTER_TABLE_2_INST0m,   FLEX_CTR_EGR_COUNTER_TABLE_3_INST0m,
        FLEX_CTR_EGR_COUNTER_TABLE_4_INST0m,   FLEX_CTR_EGR_COUNTER_TABLE_5_INST0m,
        FLEX_CTR_EGR_COUNTER_TABLE_6_INST0m,   FLEX_CTR_EGR_COUNTER_TABLE_7_INST0m,
        FLEX_CTR_EGR_COUNTER_TABLE_0_INST1m,   FLEX_CTR_EGR_COUNTER_TABLE_1_INST1m,
        FLEX_CTR_EGR_COUNTER_TABLE_2_INST1m,   FLEX_CTR_EGR_COUNTER_TABLE_3_INST1m,
        FLEX_CTR_EGR_COUNTER_TABLE_4_INST1m,   FLEX_CTR_EGR_COUNTER_TABLE_5_INST1m,
        FLEX_CTR_EGR_COUNTER_TABLE_6_INST1m,   FLEX_CTR_EGR_COUNTER_TABLE_7_INST1m
    };


    SHR_FUNC_ENTER(unit);

    ing_action_enable = 0x0f0000ff; /* 12 actions to 12 counters */
    egr_action_enable = 0x00000f0f; /*  8 actions to  8 counters */

    SHR_IF_ERR_EXIT(get_pp_map(unit, &pp_map));

    sid = MPLS_ENTRY_FLEX_CTR_CONTROLr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTION_0f, 0x4);
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTION_1f, 0x5);
    TH4_PWRT_SET_FIELD(FLEX_CTR_ACTION_2f, 0x6);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

    sid = ALPM_FLEX_CTR_CONTROLr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(V6UC_FLEX_CTR_ACTION_0f, 0x7);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

    sid = FLEX_CTR_ING_COUNTER_ACTION_ENABLE_INST0r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BITMAPf, ing_action_enable);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = FLEX_CTR_ING_COUNTER_ACTION_ENABLE_INST1r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BITMAPf, ing_action_enable);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    total_actions = 32;
    pool_no = 0;
    for (action = 0; action < total_actions; action++) {
        if (ing_action_enable & (1 << action)) {
            sid = FLEX_CTR_ING_COUNTER_ACTION_TABLE_0_INST0m;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(POOL_BASEf, pool_no);
            TH4_PWRT_SET_FIELD(B_CVALUE_1_OPf, action);
            TH4_PWRT_SET_FIELD(A_CVALUE_1_OPf, action);
            TH4_PWRT_SET_FIELD(CINDEX_OPf, action);
            TH4_PWRT_SET_FIELD(CINDEX_BASEf, 0x0);
            TH4_PWRT_SET_FIELD(SIZE0_SEL_0f, 21); /* obj1 = src port num */
            TH4_PWRT_SET_FIELD(SIZE0_SEL_1f, 36); /* toggling */
            TH4_PWRT_SET_FIELD(SIZE0_SEL_2f, 56);
            TH4_PWRT_SET_FIELD(SIZE0_SEL_3f, 31);
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, action, ipep_acc);
                    sid = FLEX_CTR_ING_COUNTER_ACTION_TABLE_0_INST0m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                    sid = FLEX_CTR_ING_COUNTER_ACTION_TABLE_0_INST1m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
            pool_no++;
        }
    }

    for (action = 0; action < total_actions; action++) {
        if (ing_action_enable & (1 << action)) {
            sid = FLEX_CTR_ING_COUNTER_ACTION_TABLE_1_INST0m;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(B_UPDATE_FN_TRUEf, 2); /* INC_BY_CVALUE */
            TH4_PWRT_SET_FIELD(A_UPDATE_FN_TRUEf, 2); /* INC_BY_CVALUE */
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, action, ipep_acc);
                    sid = FLEX_CTR_ING_COUNTER_ACTION_TABLE_1_INST0m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                    sid = FLEX_CTR_ING_COUNTER_ACTION_TABLE_1_INST1m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }

    for (action = 0; action < total_actions; action++) {
        if (ing_action_enable & (1 << action)) {
            sid = FLEX_CTR_ING_COUNTER_OP_PROFILE_TABLE_INST0m;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(SEL_1f, 0x2); /* obj1 */
            TH4_PWRT_SET_FIELD(MASK_SIZE_1f, 0x8);
            TH4_PWRT_SET_FIELD(SEL_2f, 0x0);
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, action, ipep_acc);
                    sid = FLEX_CTR_ING_COUNTER_OP_PROFILE_TABLE_INST0m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                    sid = FLEX_CTR_ING_COUNTER_OP_PROFILE_TABLE_INST1m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }


    flex_ctr_pool_num = 12;
    for (pool_no = 0; pool_no < (2 * flex_ctr_pool_num); pool_no++) {
        sid = ing_flex_ctr_update_ctrl[pool_no];
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(COUNTER_POOL_ENABLEf, 0x1);
        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
                SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                    unit, sid, ltid, &pt_dyn_info, entry));
            }
        }
    }


    sid = EGR_NHOP_FLEX_CTR_CONTROLr;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(MPLS_FLEX_CTR_ACTION_0f, 0xf);
    TH4_PWRT_SET_FIELD(MPLS_FLEX_CTR_ACTION_1f, 0x1);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));


    sid = FLEX_CTR_EGR_COUNTER_ACTION_ENABLE_INST0r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BITMAPf, egr_action_enable);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    sid = FLEX_CTR_EGR_COUNTER_ACTION_ENABLE_INST1r;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(BITMAPf, egr_action_enable);
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

    total_actions = 16;
    pool_no = 0;
    for (action = 0; action < total_actions; action++) {
        if (egr_action_enable & (1 << action)) {
            sid = FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0_INST0m;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(POOL_BASEf, pool_no);
            TH4_PWRT_SET_FIELD(B_CVALUE_1_OPf, action);
            TH4_PWRT_SET_FIELD(A_CVALUE_1_OPf, action);
            TH4_PWRT_SET_FIELD(CINDEX_OPf, action);
            TH4_PWRT_SET_FIELD(CINDEX_BASEf, 0x0);
            TH4_PWRT_SET_FIELD(SIZE0_SEL_0f, 5); /* obj1 = dst port num */
            TH4_PWRT_SET_FIELD(SIZE0_SEL_1f, 12); /* toggling */
            TH4_PWRT_SET_FIELD(SIZE0_SEL_2f, 2);
            TH4_PWRT_SET_FIELD(SIZE0_SEL_3f, 1);
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, action, ipep_acc);
                    sid = FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0_INST0m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                    sid = FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0_INST1m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
            pool_no++;
        }
    }

    for (action = 0; action < total_actions; action++) {
        if (egr_action_enable & (1 << action)) {
            sid = FLEX_CTR_EGR_COUNTER_ACTION_TABLE_1_INST0m;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(B_UPDATE_FN_TRUEf, 2); /* INC_BY_CVALUE */
            TH4_PWRT_SET_FIELD(A_UPDATE_FN_TRUEf, 2); /* INC_BY_CVALUE */
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, action, ipep_acc);
                    sid = FLEX_CTR_EGR_COUNTER_ACTION_TABLE_1_INST0m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                    sid = FLEX_CTR_EGR_COUNTER_ACTION_TABLE_1_INST1m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }

    for (action = 0; action < total_actions; action++) {
        if (egr_action_enable & (1 << action)) {
            sid = FLEX_CTR_EGR_COUNTER_OP_PROFILE_TABLE_INST0m;
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            TH4_PWRT_SET_FIELD(SEL_1f, 0x2); /* obj1 */
            TH4_PWRT_SET_FIELD(MASK_SIZE_1f, 0x8);
            TH4_PWRT_SET_FIELD(SEL_2f, 0x0);
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, action, ipep_acc);
                    sid = FLEX_CTR_EGR_COUNTER_OP_PROFILE_TABLE_INST0m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                    sid = FLEX_CTR_EGR_COUNTER_OP_PROFILE_TABLE_INST1m;
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }

    flex_ctr_pool_num = 8;
    for (pool_no = 0; pool_no < (2 * flex_ctr_pool_num); pool_no++) {
        sid = egr_flex_ctr_update_ctrl[pool_no];
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(COUNTER_POOL_ENABLEf, 0x1);
        for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
            if (pp_map & (1 << ipep_acc)) {
                TH4_PWRT_PT_DYN_INFO(pt_dyn_info, 0, ipep_acc);
                SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                    unit, sid, ltid, &pt_dyn_info, entry));
            }
        }
    }

    /* Randomize pool counters */
    if (((power_test_p->enable_map >> TH4_PWRT_EN_RND_INITFLEXCTR) & 0x1) != 0) {
    flex_ctr_pool_num = 12;
    for (pool_no = 0; pool_no < (2 * flex_ctr_pool_num); pool_no++) {
        sid = ing_flex_ctr_table[pool_no];
        /* flex cntrs addreses will be dev_port num */
        for (index = 0; index < TH4_PWRT_DEV_PORTS_PER_DEV; index++) {
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = sal_rand32();
            memfld[1] = sal_rand32();
            memfld[2] = sal_rand32() & 0xff;
            SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, WIDE_COUNTERf, entry, memfld));
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }
    flex_ctr_pool_num = 8;
    for (pool_no = 0; pool_no < (2 * flex_ctr_pool_num); pool_no++) {
        sid = egr_flex_ctr_table[pool_no];
        /* flex cntrs addreses will be dev_port num */
        for (index = 0; index < TH4_PWRT_DEV_PORTS_PER_DEV; index++) {
            sal_memset(entry, 0, sizeof(entry));
            sal_memset(memfld, 0, sizeof(memfld));
            memfld[0] = sal_rand32();
            memfld[1] = sal_rand32();
            memfld[2] = sal_rand32() & 0xff;
            SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, WIDE_COUNTERf, entry, memfld));
            for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
                if (pp_map & (1 << ipep_acc)) {
                    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
                    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                        unit, sid, ltid, &pt_dyn_info, entry));
                }
            }
        }
    }
    }

exit:
    SHR_FUNC_EXIT();
}



static int setup_egr_misc(
    int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    uint32_t pp_map = 0;
    int index, ipep_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(get_pp_map(unit, &pp_map));

    sid = EGR_FRAGMENT_ID_TABLE_INST0m;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    TH4_PWRT_SET_FIELD(FRAGMENT_IDf, sal_rand32() & 0xffff);
    index = get_egr_mpls_tunnel_index(unit, flow_id) >> 2;
    for (ipep_acc = 0; ipep_acc < 4; ipep_acc++) {
        if (pp_map & (1 << ipep_acc)) {
            TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, ipep_acc);
            sid = EGR_FRAGMENT_ID_TABLE_INST0m;
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
            sid = EGR_FRAGMENT_ID_TABLE_INST1m;
            SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
                unit, sid, ltid, &pt_dyn_info, entry));
        }
    }

exit:
    SHR_FUNC_EXIT();
}



static int setup_egr_misc_global(
    int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, i;

    SHR_FUNC_ENTER(unit);

    for (index = 1; index < 16; index ++) {
        sid = EGR_INT_ACTION_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(EGR_METADATA_FIFO_RW_ENABLEf, 0x1);
        TH4_PWRT_SET_FIELD(MD_HDR_TYPEf, sal_rand32() & 0x7);
        TH4_PWRT_SET_FIELD(MD_HDR_LEN_GRANULARITYf, 0);
        TH4_PWRT_SET_FIELD(IFA_FLAGSf, sal_rand32() & 0xff);
        TH4_PWRT_SET_FIELD(MAX_OR_REMAINING_LENf, sal_rand32() & 0xffff);
        TH4_PWRT_SET_FIELD(IP_HDR_TTLf, sal_rand32() & 0x3fff);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    for (index = 0; index < 4; index ++) {
        sid = EGR_DCN_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(DCN_PKT_SIZEf,  sal_rand32() & 0x3);
        TH4_PWRT_SET_FIELD(DCN_PKT_TOSf,  sal_rand32() & 0xff);
        TH4_PWRT_SET_FIELD(DCN_PKT_TOS_MASKf,  sal_rand32() & 0xff);
        TH4_PWRT_SET_FIELD(DCN_PKT_UPDATE_TOSf,  sal_rand32() & 0x1);
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    for (index = 0; index < 4; index ++) {
        sid = EGR_LOOPBACK_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        /* just randomize the entry */
        entry[0] = sal_rand32() & 0xfff;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    for (index = 0; index < 8; index ++) {
        sid = EGR_MD_HDR_ATTRSm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        /* just randomize the entry */
        entry[0] = sal_rand32();
        entry[1] = sal_rand32() & 0x1ffff;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    for (index = 0; index < 8; index ++) {
        sid = EGR_MD_HDR_CONST_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        for (i = 0; i < 24; i++) {
            memfld[0] = ((sal_rand32() & 0x3f) << 1) | 0x1;
            set_field_in_memfld(memfld, entry, i*7, 7);
        }
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    for (index = 0; index < 8; index ++) {
        sid = EGR_MD_HDR_FS_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        /* FIELD_16BIT_0_INDEX
         * FIELD_16BIT_0_SHIFT
         * FIELD_16BIT_0_SIZE
         * FIELD_16BIT_0_START
         */
        for (i = 0; i < 12; i++) {
            memfld[0] = ((4 + (sal_rand() % 17)) << 0) |
                        (0  << 6) |
                        (15 << 10) |
                        (0  << 15);
            set_field_in_memfld(memfld, entry, i*19, 19);
        }
        /* FIELD_32BIT_0_INDEX
         * FIELD_32BIT_0_SHIFT
         * FIELD_32BIT_0_SIZE
         * FIELD_32BIT_0_START
         */
        for (i = 0; i < 12; i++) {
            memfld[0] = ((2 + (sal_rand() % 9)) << 0) |
                        (0  << 5) |
                        (31 << 10) |
                        (0  << 16);
            set_field_in_memfld(memfld, entry, 228 + (i*21), 21);
        }
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    for (index = 0; index < 512; index ++) {
        /* assoc_data.INT_FLOW_CLASS[8:0] */
        sid = EGR_METADATA_PROFILEm;
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(OPAQUE_FIELD_1f, sal_rand32());
        TH4_PWRT_SET_FIELD(OPAQUE_FIELD_2f, sal_rand32());
        TH4_PWRT_SET_FIELD(OPAQUE_FIELD_3f, sal_rand32());
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

exit:
    SHR_FUNC_EXIT();
}


static int setup_ing_pri_dscp_cn(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, i;

    SHR_FUNC_ENTER(unit);

    sid = DSCP_TABLEm;
    for (i = 0; i < 64; i++) {
        sal_memset(entry, 0, sizeof(entry));
        /* {CNG/DSCP/PRI} = random */
        set_rand_mem(12, entry);
        index = i*64;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = ING_MPLS_EXP_MAPPINGm;
    for (i = 0; i < 32; i++) {
        sal_memset(entry, 0, sizeof(entry));
        /* {CNG/DSCP/PRI} = random */
        set_rand_mem(12, entry);
        index = i*8;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = ING_UNTAGGED_PHBm;
    for (i = 0; i < 4; i++) {
        sal_memset(entry, 0, sizeof(entry));
        /* {CNG/PRI} = random */
        set_rand_mem(6, entry);
        index = i*16;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = IP_TO_INT_CN_MAPPING_PROFILE_SELECT_1m;
    for (i = 0; i < 16; i++) {
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(INT_CN_PROFILE_PTRf, i & 0x1);
        index = i;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = IP_TO_INT_CN_MAPPING_PROFILE_SELECT_2m;
    for (i = 0; i < 16; i++) {
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(INT_CN_PROFILE_PTRf, i & 0x1);
        index = i;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = IP_TO_INT_CN_MAPPING_1m;
    for (i = 0; i < 16; i++) {
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(INT_CNf, i & 0x3);
        index = i;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = ING_OUTER_DOT1P_MAPPING_TABLEm;
    for (i = 0; i < 64; i++) {
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(NEW_CFIf, 0x0);
        TH4_PWRT_SET_FIELD(NEW_DOT1Pf, i & 0x7);
        index = i * 16;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

    sid = ING_TUNNEL_ECN_DECAPm;
    for (i = 0; i < 8; i++) {
        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));
        TH4_PWRT_SET_FIELD(CHANGE_INNER_ECNf, sal_rand() & 0x1);
        TH4_PWRT_SET_FIELD(INCREMENT_ECN_COUNTERf, sal_rand() & 0x1);
        TH4_PWRT_SET_FIELD(INNER_ECNf, sal_rand() & 0x3);
        TH4_PWRT_SET_FIELD(COPY_TO_CPUf, 0);
        TH4_PWRT_SET_FIELD(DROPf, 0);
        index = i * 16;
        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
            unit, sid, ltid, &pt_dyn_info, entry));
    }

exit:
    SHR_FUNC_EXIT();
}


static int setup_cpu_port(int unit,
    int flow_id)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    int index, cpu_port;

    SHR_FUNC_ENTER(unit);

    cpu_port = 0;
    index = 0;
    sid = CPU_PBMm;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));
    set_mem_bit(memfld, cpu_port, 0x1);
    SHR_IF_ERR_EXIT(th4_pwrt_field_set(unit, sid, BITMAPf, entry, memfld));
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(
        unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}



static int setup_idb_ct(int unit)
{
    power_test_t *power_test_p = power_test_parray[unit];
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    int phy_port, lport;
    int pm_num;
    int subp;
    int pipe_num;
    uint32_t ct_disable;
    uint32_t flow_id;
    size_t wsize;
    bcmdrd_sym_info_t sinfo;
    static const bcmdrd_fid_t ca_ct_disable_fields[] = {
        PORT0_CT_DISABLEf, PORT1_CT_DISABLEf,
        PORT2_CT_DISABLEf, PORT3_CT_DISABLEf
    };
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    ct_disable = (((power_test_p->enable_map >> TH4_PWRT_DIS_CUT_THROUGH) & 0x1) == 0x1) ? 1 : 0;
    sid = IDB_CA_CONTROL_2r;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));
    wsize = sinfo.entry_wsize;

    for (flow_id = power_test_p->start_flow;
         flow_id < power_test_p->start_flow + power_test_p->num_flows;
         flow_id++) {

        sal_memset(entry, 0, sizeof(entry));
        sal_memset(memfld, 0, sizeof(memfld));

        phy_port = power_test_p->src_port_flow_id[flow_id];
        lport = power_test_p->p2l_mapping[phy_port];
        SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, lport, &pipe_num));
        pm_num = ((phy_port - 1) & 0xf) >> 2;
        subp = ((phy_port - 1) & 0x3);

        TH4_PWRT_PT_DYN_INFO(pt_dyn_info, pm_num, pipe_num);

        SHR_IF_ERR_EXIT(bcmbd_pt_read(unit, sid, &pt_dyn_info, NULL, entry, wsize));
        fid = ca_ct_disable_fields[subp];
        TH4_PWRT_SET_FIELD(fid, ct_disable);

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
            "setup_idb_ct() flow_id=%0d lport=%0d phy_port=%0d, pipe_num=%0d pm_num=%0d subp=%0d ct_disable=%0d entry[0]=0x%x\n"),
            flow_id, lport, phy_port, pipe_num, pm_num, subp, ct_disable, entry[0]));
        SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));
    }

exit:
    SHR_FUNC_EXIT();
}


static int power_test_setup_ipep(int unit)
{
    int flow_id;
    power_test_t *power_test_p = power_test_parray[unit];
    int num_flows, start_flow;
    int num_slices;

    SHR_FUNC_ENTER(unit);

    num_flows = power_test_p->num_flows;
    start_flow = power_test_p->start_flow;

    /* Set hash tables attributes & controls */
    SHR_IF_ERR_EXIT(power_test_set_key_attributes(unit));
    SHR_IF_ERR_EXIT(power_test_set_hash_control(unit));
    SHR_IF_ERR_EXIT(power_test_set_hash_remap_and_action(unit));

    SHR_IF_ERR_EXIT(setup_ing_ctrl_regs(unit));
    SHR_IF_ERR_EXIT(setup_ing_ctrl_tables(unit));
    SHR_IF_ERR_EXIT(setup_ing_nonflow_tables(unit));

    cli_out("\nSTART power_test_setup_ipep() IPIPE\n");
    for (flow_id = start_flow; flow_id < (start_flow + num_flows); flow_id++) {
        SHR_IF_ERR_EXIT(setup_ing_port_tabs(unit, flow_id, 0));
        SHR_IF_ERR_EXIT(setup_ing_vlan_tabs(unit, flow_id, 0));
        cli_out("flow_id=%0d setup_ing_l2_entry is_l2_only_en=%0d\n",
                 flow_id, is_l2_only_en(unit, flow_id, 0));
        if (is_l2_only_en(unit, flow_id, 0) == 0) {
            if ((flow_id % 2) == 0){
                SHR_IF_ERR_EXIT(setup_ing_l2_entry(unit, flow_id, 0));
            }
        } else {
            SHR_IF_ERR_EXIT(setup_ing_l2_entry(unit, flow_id, 0));
        }
        if ((flow_id % 2) == 0){
            SHR_IF_ERR_EXIT(setup_ing_my_station_tcam(unit, flow_id, 0));
        }
        if (is_mpls_en(unit, flow_id, 0) == 1) {
            SHR_IF_ERR_EXIT(setup_ing_mpls_entry(unit, flow_id, 0));
        }
        SHR_IF_ERR_EXIT(setup_ing_l3_iif(unit, flow_id));
        if (is_l2_only_en(unit, flow_id, 0) == 0) {
            if (power_test_p->lpm_en == 1) {
                if (power_test_p->verbose_level >= 1) {
                    cli_out("flow_id=%0d setup_ing_lpm()\n", flow_id);
                }
                SHR_IF_ERR_EXIT(setup_ing_lpm(unit, flow_id));
            }
        }
        if (is_l2_only_en(unit, flow_id, 0) == 0) {
            SHR_IF_ERR_EXIT(setup_ing_next_hop(unit, flow_id));
            if (0) {SHR_IF_ERR_EXIT(setup_ing_qos(unit, flow_id)); }
            SHR_IF_ERR_EXIT(setup_udf(unit, flow_id));
        }
        if (((power_test_p->enable_map >> TH4_PWRT_EN_LAG_RES) & 0x1) != 0) {
            SHR_IF_ERR_EXIT(setup_lag_fast_trunk(unit, flow_id));
        }
    }

    if (((power_test_p->enable_map >> TH4_PWRT_EN_RND_PRI) & 0x1) == 0x1) {
        SHR_IF_ERR_EXIT(setup_ing_pri_dscp_cn(unit));
    }

    cli_out("START power_test_setup_ipep() EPIPE\n");
    SHR_IF_ERR_EXIT(setup_egr_ctrl_regs(unit));
    for (flow_id = start_flow; flow_id < (start_flow + num_flows); flow_id++) {
        cli_out("Calling EPIPE setup function flow_id=%0d()\n", flow_id);
        SHR_IF_ERR_EXIT(setup_egr_vlan_tabs(unit, flow_id));
        SHR_IF_ERR_EXIT(setup_egr_next_hop(unit, flow_id));
        SHR_IF_ERR_EXIT(setup_egr_port_tables(unit, flow_id));
        SHR_IF_ERR_EXIT(setup_egr_misc(unit, flow_id));
    }
    SHR_IF_ERR_EXIT(setup_egr_misc_global(unit));

    /* CPU setup */
    cli_out("START power_test_setup_ipep() CPU\n");
    SHR_IF_ERR_EXIT(setup_cpu_port(unit, 0));
    SHR_IF_ERR_EXIT(setup_ing_port_tabs(unit, 0, 1));
    if (power_test_p->enable_ecmp_dlb != 0) {
        if (power_test_p->verbose_level >= 1) {
            cli_out("START power_test_setup_ecmp_dlb()\n");
        }
        for (flow_id = start_flow; flow_id < (start_flow + num_flows); flow_id++) {
            SHR_IF_ERR_EXIT(setup_ing_ecmp(unit, flow_id, 0));
        }
    }

    cli_out("START power_test_setup_ipep() MISC\n");
    if (((power_test_p->enable_map >> TH4_PWRT_EN_FULL_IFP) & 0x1) != 0) {
        num_slices = 9;
    } else {
        num_slices = 1;
    }
    if (power_test_p->verbose_level >= 1) {
        cli_out("START setup_ifp num_slices=%0d\n", num_slices);
    }
    SHR_IF_ERR_EXIT(setup_ifp(unit, num_slices));

    if (((power_test_p->enable_map >> TH4_PWRT_EN_EM) & 0x1) != 0) {
        if (power_test_p->verbose_level >= 1) {
            cli_out("START setup_exact_match\n");
        }
        SHR_IF_ERR_EXIT(setup_exact_match(unit));
    }

    if (((power_test_p->enable_map >> TH4_PWRT_EN_VFP) & 0x1) != 0) {
        if (power_test_p->verbose_level >= 1) {
            cli_out("START setup_vfp\n");
        }
        SHR_IF_ERR_EXIT(setup_vfp(unit));
    }
    if (((power_test_p->enable_map >> TH4_PWRT_EN_EFP) & 0x1) != 0) {
        if (power_test_p->verbose_level >= 1) {
            cli_out("START setup_efp\n");
        }
        SHR_IF_ERR_EXIT(setup_efp(unit));
    }
    if (((power_test_p->enable_map >> TH4_PWRT_EN_TRAP) & 0x1) != 0) {
        if (power_test_p->verbose_level >= 1) {
            cli_out("START setup_elephant_trap\n");
        }
        SHR_IF_ERR_EXIT(setup_elephant_trap(unit));
    }
    if (((power_test_p->enable_map >> TH4_PWRT_EN_FLX_CTR) & 0x1) != 0) {
        if (power_test_p->verbose_level >= 1) {
            cli_out("START setup_flex_ctr\n");
        }
        SHR_IF_ERR_EXIT(setup_flex_ctr(unit));
    }

    setup_idb_ct(unit);

    cli_out("FINISH power_test_setup_ipep()\n");

exit:
    SHR_FUNC_EXIT();
}


static int tomahawk4_power_test_reroute_to_cpu_ing_nhi_per_port(
    int unit,
    int port)
{
    int flow_id, index, phy_port;
    power_test_t *power_test_p = power_test_parray[unit];
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid;
    uint32_t entry[BCMDRD_MAX_PT_WSIZE];
    uint32_t memfld[BCMDRD_MAX_PT_WSIZE];
    bcmltd_sid_t ltid = -1;
    size_t wsize;
    bcmdrd_sym_info_t sinfo;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, port, &phy_port));
    flow_id = power_test_p->phy_to_flow_id[phy_port];

    sid = ING_L3_NEXT_HOPm;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));
    wsize = sinfo.entry_wsize;

    sal_memset(entry, 0, sizeof(entry));
    sal_memset(memfld, 0, sizeof(memfld));

    index = get_next_hop_index(unit, flow_id);
    TH4_PWRT_PT_DYN_INFO(pt_dyn_info, index, 0);

    SHR_IF_ERR_EXIT(bcmbd_pt_read(unit, sid, &pt_dyn_info, NULL, entry, wsize));
    TH4_PWRT_SET_FIELD(DROPf, 0x1);
    TH4_PWRT_SET_FIELD(COPY_TO_CPUf, 0x1);

    SHR_IF_ERR_EXIT(th4_pwrt_pt_write(unit, sid, ltid, &pt_dyn_info, entry));

exit:
    SHR_FUNC_EXIT();
}


static int tomahawk4_power_test_setup_ipep_wrap(
    int unit)
{
    power_test_t *power_test_p;

    SHR_FUNC_ENTER(unit);

    power_test_p = power_test_parray[unit];

    cli_out("START power_test_setup_ipep() start_flow=%0d num_flows=%0d alpm_level=%0d\n",
        power_test_p->start_flow, power_test_p->num_flows, power_test_p->alpm_level);
    SHR_IF_ERR_EXIT(power_test_setup_ipep(unit));
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "FINISH power_test_setup_ipep()")));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_power_fill_pkt(
    int unit,
    bcmpkt_data_buf_t *buf,
    bcma_testutil_pkt_attr_t pkt_attr)
{
    bcma_testutil_pkt_attr_t l_pkt_attr;
    int flow_id, i;
    bcm_mac_t mac_addr;
    uint32_t ip_addr[BCMDRD_MAX_PT_WSIZE];
    int phy_port;
    power_test_t *power_test_p;

    SHR_FUNC_ENTER(unit);

    l_pkt_attr.port   = pkt_attr.port;
    l_pkt_attr.id     = pkt_attr.id;
    l_pkt_attr.size   = pkt_attr.size;

    l_pkt_attr.l3_en       = 1;
    l_pkt_attr.ipv6_en     = 1;
    l_pkt_attr.l3_mpls_en  = 1;

    power_test_p = power_test_parray[unit];
    SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, pkt_attr.port, &phy_port));

    flow_id = power_test_p->phy_to_flow_id[phy_port];

    get_mac_da(unit, flow_id, mac_addr);
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        l_pkt_attr.mac_da[i] = mac_addr[i];
    }
    get_mac_sa(unit, flow_id, mac_addr);
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        l_pkt_attr.mac_sa[i] = mac_addr[i];
    }
    l_pkt_attr.vlan_id = get_vlan_id(unit, flow_id);
    l_pkt_attr.mpls_label[0] = get_pkt_mpls(unit, flow_id, 1, 0);
    l_pkt_attr.mpls_label[1] = get_pkt_mpls(unit, flow_id, 2, 0);
    l_pkt_attr.mpls_label[2] = get_pkt_mpls(unit, flow_id, 3, 0);

    get_ip_addr_da(unit, flow_id, ip_addr);
    l_pkt_attr.ip_da[0] = ip_addr[0];
    l_pkt_attr.ip_da[1] = ip_addr[1];
    l_pkt_attr.ip_da[2] = ip_addr[2];
    l_pkt_attr.ip_da[3] = ip_addr[3];
    get_ip_addr_sa(unit, flow_id, ip_addr);
    l_pkt_attr.ip_sa[0] = ip_addr[0];
    l_pkt_attr.ip_sa[1] = ip_addr[1];
    l_pkt_attr.ip_sa[2] = ip_addr[2];
    l_pkt_attr.ip_sa[3] = ip_addr[3];
    l_pkt_attr.ttl        = 0xff;
    l_pkt_attr.tos        = 0;

    l_pkt_attr.seed = 11 + (pkt_attr.id * 512) + pkt_attr.port;

    SHR_IF_ERR_EXIT
        (bcma_testutil_mpls_packet_fill(unit, buf, l_pkt_attr));

exit:
    SHR_FUNC_EXIT();
}



static void  print_pkt(bcmpkt_packet_t *packet)
{
    uint32_t i;
    cli_out("PKT unit=%0d, flags=0x%x type=0x%x \n", packet->unit, packet->flags, packet->type);

    cli_out("PKT len=%0d, data_len=0%0d ref_count=%0d \n",
        packet->data_buf->len, packet->data_buf->data_len, packet->data_buf->ref_count);

    for (i = 0; i < packet->data_buf->data_len; i++){
        cli_out("%02x ", packet->data_buf->data[i]);
        if ((i % 16) == 15) {
            cli_out("\n");
        }
    }
    cli_out("\n");
}



int
bcm56990_a0_power_init(int unit, void *bp)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "bcm56990_a0_power_init() START \n")));

    sal_srand(0x12347);
    power_test_init(unit, bp);

exit:
    SHR_FUNC_EXIT();
}


#define BCM56990_A0_TRAFFIC_MAX_POWER_PARAM_HELP \
    "\n\n"\
    "  TH4 DevSpecific PARAM details:\n"\
    "  LpmLevel         - TH4 Alpm level. Admissable values: 1,2,3; default=1\n"\
    "  Topology         - TH4 Port traffic topology. default=0. Admissable values:\n"\
    "                     0 - connects two adjacent ports \n"\
    "                     1 - connects two ports from different ITMs \n"\
    "                     2 - only for EXT loopback; no need to specify if LoopbackMode=EXT\n"\
    "  EnableBmp        - TH4 Power test flags; for debug only.\n"


int
bcm56990_a0_power_help(int unit)
{
    cli_out("%s", BCM56990_A0_TRAFFIC_MAX_POWER_PARAM_HELP);
    return SHR_E_NONE;
}

int
bcm56990_a0_power_port_set(int unit, void *bp)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* Place holder for now */
    cli_out("bcm56990_a0_power_port_set CALLED\n");

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_power_config_set(int unit, void *bp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(tomahawk4_power_test_setup_ipep_wrap(unit));

exit:
    SHR_FUNC_EXIT();
}


int
bcm56990_a0_power_txrx(int unit, void *bp)
{
    bcmpkt_packet_t **packet_list[BCMDRD_CONFIG_MAX_PORTS] = {NULL};
    int netif_id, pktdev_id;
    bcmdrd_dev_type_t pkt_dev_type;
    int pkt_cnt_all;
    int port, pkt_len;
    uint32_t idx;
    bool rxdata_checker_create = false, rxdata_checker_start = false;
    bool counter_checker_create = false, counter_checker_start = false;
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;
    int wait_time;
    bool test_result = false;
    uint64_t *expeceted_rate = NULL;
    power_test_t *power_test_p = power_test_parray[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    cli_out("bcm56990_a0_power_txrx() CALLED \n");
    pkt_len = tra_param->pkt_len[1][0];

    /* setup packet device */
    pkt_cnt_all = 0;
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        pkt_cnt_all += tra_param->pkt_cnt[port];
    }

    bcma_testutil_packet_init();

    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_init
            (unit, TRAFFIC_MAX_POWER_PKTDEV_CHAN_TX, TRAFFIC_MAX_POWER_PKTDEV_CHAN_RX,
             TRAFFIC_MAX_POWER_PKT_SIZE_MAX,
             pkt_cnt_all,
             &netif_id, &pktdev_id));


    /* allocate packet buffer */
    BCMDRD_PBMP_ITER(power_test_p->pbmp_traffic, port) {
        uint32_t pkt_cnt = tra_param->pkt_cnt[port];
        SHR_ALLOC(packet_list[port], pkt_cnt * sizeof(bcmpkt_packet_t *),
                  "bcmaTestCaseTrafficStreamPkt");
        SHR_NULL_CHECK(packet_list[port], SHR_E_MEMORY);
        sal_memset(packet_list[port], 0, pkt_cnt * sizeof(bcmpkt_packet_t *));
    }


    if (tra_param->need_check_speed) {
        /* determine target speed : minimun port speed * 95% */
        SHR_ALLOC(expeceted_rate, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t),
              "bcmaTestCaseTrafficStreamExpRate");
        SHR_NULL_CHECK(expeceted_rate, SHR_E_MEMORY);
        sal_memset(expeceted_rate, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t));
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_traffic_expeceted_rate_get
                (unit, tra_param->pbmp_all, tra_param->pbmp_oversub,
                 pkt_len, BCMDRD_CONFIG_MAX_PORTS, expeceted_rate));

        BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
            expeceted_rate[port] =
                (expeceted_rate[port] *
                 (100 - TRAFFIC_MAX_POWER_DEFAULT_CHECK_SPEED_MARGIN)) / 100;
        }

        /* print information */
        BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
            cli_out("port %d target speed : ", port);
            bcma_testutil_stat_show(expeceted_rate[port], 1, true);
            cli_out("b/s \n");
        }

        /* setup counter checker */
        SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_create(unit));
        counter_checker_create = true;

        BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
            char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];
            sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                     "Port %3"PRId32" :", port);
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_add
                    (unit, port, true, true, expeceted_rate[port], show_msg));
        }
    }

    /* inject packet, and setup rxdataa checker if need */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));
    if (tra_param->need_check_rx == TRUE) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_create
                (unit, netif_id, pktdev_id, TRAFFIC_RXDATA_CHECKER_MODE_PORT));
        rxdata_checker_create = true;
    }

    if (tra_param->need_check_speed) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_start(unit));
        counter_checker_start = true;
    }

    if (tra_param->config_n_send) {
        BCMDRD_PBMP_ITER(power_test_p->pbmp_traffic, port) {
            bcma_testutil_pkt_attr_t pkt_attr;
            pkt_attr.port = port;

            cli_out("port=%0d inject %d packets.\n    (size_pattern :",
                     port, tra_param->pkt_cnt[port]);
            for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
                 if (idx > 5) { break;}
                 cli_out(" %d", tra_param->pkt_len[port][idx]);
            }
            cli_out(" ...)\n");

            for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
                bcmpkt_packet_t *packet = packet_list[port][idx];
                pkt_attr.id = idx;
                pkt_attr.size = tra_param->pkt_len[port][idx];
                SHR_IF_ERR_EXIT
                       (bcmpkt_alloc(pktdev_id, tra_param->pkt_len[port][idx],
                             BCMPKT_BUF_F_TX, &packet));
                SHR_NULL_CHECK(packet, SHR_E_MEMORY);

                SHR_IF_ERR_EXIT
                    (bcm56990_a0_power_fill_pkt
                        (unit, packet->data_buf, pkt_attr));
                SHR_IF_ERR_EXIT
                    (bcmpkt_fwd_port_set(pkt_dev_type, port, packet));
                if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                    print_pkt(packet);
                }
                SHR_IF_ERR_EXIT
                    (bcmpkt_tx(unit, TRAFFIC_MAX_POWER_DEFAULT_NETIF_ID, packet));
                bcmpkt_free(pktdev_id, packet);
                packet = NULL;
            }
        }
    }

    sal_sleep(tra_param->interval);

    if (tra_param->check_n_stop) {
        if (tra_param->need_check_speed) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_execute
                    (unit, false, &test_result));
        }

        cli_out("bcm56990_a0_power_txrx() run for %0d seconds \n", tra_param->runtime);
        sal_sleep(tra_param->runtime);
        if (tra_param->need_check_speed) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_execute
                    (unit, true, &test_result));
        }

        if (tra_param->need_check_rx == TRUE) {
            BCMDRD_PBMP_ITER(power_test_p->pbmp_traffic, port) {
                bcma_testutil_pkt_attr_t pkt_attr;
                pkt_attr.port = port;
                for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
                    bcmpkt_packet_t *packet;
                    SHR_IF_ERR_EXIT
                       (bcmpkt_alloc(pktdev_id, tra_param->pkt_len[port][idx],
                                 BCMPKT_BUF_F_TX, &(packet_list[port][idx])));
                    packet = packet_list[port][idx];
                    pkt_attr.id = idx;
                    pkt_attr.size = tra_param->pkt_len[port][idx];
                    SHR_IF_ERR_EXIT
                        (bcm56990_a0_power_fill_pkt
                            (unit, packet->data_buf, pkt_attr));
                    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
                        print_pkt(packet);
                    }
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_traffic_rxdata_checker_add
                            (unit, port, packet->data_buf));
                }
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_rxdata_checker_start(unit));
                rxdata_checker_start = true;
                /* redirect all packet back to CPU and check */
                SHR_IF_ERR_EXIT(tomahawk4_power_test_reroute_to_cpu_ing_nhi_per_port(unit, port));

                /* check */
                wait_time = TRAFFIC_MAX_POWER_DEFAULT_RX_CHECK_WAIT/50;
                LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "rxdata_checker port=%0d wait time is %0dms\n"), port, wait_time/1000));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_rxdata_checker_execute_per_port
                        (unit, port, wait_time, &test_result));
                if (test_result == false) {
                    cli_out("bcm56990_a0_power_txrx() PORT= %0d rx_data check CHECK_RESULT= %0d\n", port, test_result);
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }
                if (rxdata_checker_start == true) {
                    bcma_testutil_traffic_rxdata_checker_stop(unit);
                    rxdata_checker_start = false;
                }
                for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
                    if (packet_list[port][idx] != NULL) {
                        bcmpkt_free(pktdev_id, packet_list[port][idx]);
                        packet_list[port][idx] = NULL;
                    }
                }
            }
        }
    }

    cli_out("bcm56990_a0_power_txrx() FINISHED \n");

exit:
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        if (packet_list[port] != NULL) {
            for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
                if (packet_list[port][idx] != NULL) {
                    bcmpkt_free(pktdev_id, packet_list[port][idx]);
                    packet_list[port][idx] = NULL;
                }
            }
            SHR_FREE(packet_list[port]);
        }
    }
    if (expeceted_rate != NULL) {
        SHR_FREE(expeceted_rate);
    }
    if (counter_checker_start == true) {
        bcma_testutil_traffic_counter_checker_stop(unit);
    }
    if (counter_checker_create == true) {
        bcma_testutil_traffic_counter_checker_destroy(unit);
    }
    if (rxdata_checker_start == true) {
        bcma_testutil_traffic_rxdata_checker_stop(unit);
    }
    if (rxdata_checker_create == true) {
        bcma_testutil_traffic_rxdata_checker_destroy(unit);
    }
    (void)bcma_testutil_packet_dev_cleanup(unit);

    SHR_FUNC_EXIT();
}
