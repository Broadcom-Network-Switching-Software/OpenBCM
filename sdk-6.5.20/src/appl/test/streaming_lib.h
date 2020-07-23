/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Extern declarations for test functions in streaming_library.c
 */

#ifndef STREAMING_LIB_H
#define STREAMING_LIB_H

/* test type */
#define PKT_TYPE_L2 0
#define PKT_TYPE_IPMC 1
#define PKT_TYPE_L3UC 2
#define PKT_TYPE_L2_TS 3

/* tx/rx channel */
#define TX_CHAN 0
#define RX_CHAN 1

#define TPID 0x8100
#define NUM_BYTES_MAC_ADDR 6
#define NUM_BYTES_CRC 4

#define MIN_PKT_SIZE 64
#define MTU 9216
#define MTU_CELL_CNT 45
#define TARGET_CELL_COUNT 100

#define NUM_SUBP_OBM 4

/* packet size */
#define HG2_WC_PKT_SIZE 64
#define ENET_WC_PKT_SIZE 145
#define ENET_IPG 12
#define ENET_PREAMBLE 8
#define HG2_IPG 8
#define HG2_HDR 12

/* cell size */
#define CELL_SIZE 208
#define FIRST_CELL_ENET 144
#define FIRST_CELL_HG2 148

#define TXDMA_TIMEOUT 100000
#define RXDMA_TIMEOUT 100000

/* CMIC */
#define CMICM_MMU_BKP_OFF_THRESHOLD 0x20
#define CMICX_MMU_BKP_OFF_THRESHOLD 0xFF

/* ANCL bandwidth */
#define TH_MISC_BW 10000
#define TD2P_MISC_BW 10000
#define TH2_MISC_BW 25000
#define TD3_MISC_BW 30000

/* freq and bandwidth : TH */
#define TH_FREQ_850 850
#define TH_FREQ_765 765
#define TH_FREQ_672 672
#define TH_FREQ_645 645
#define TH_FREQ_545 545
#define TH_BW_850 561000
#define TH_BW_765 504000
#define TH_BW_672 442000
#define TH_BW_645 423000
#define TH_BW_545 358000

/* freq and bandwidth : TD2P */
#define TD2P_FREQ_793 793
#define TD2P_FREQ_635 635
#define TD2P_FREQ_529 529
#define TD2P_FREQ_421 421
#define TD2P_BW_793 500000
#define TD2P_BW_635 380000
#define TD2P_BW_529 340000
#define TD2P_BW_421 260000

/* freq and bandwidth : TH2 */
#define TH2_FREQ_1700 1700
#define TH2_FREQ_1625 1625
#define TH2_FREQ_1525 1525
#define TH2_FREQ_1425 1425
#define TH2_FREQ_1325 1325
#define TH2_FREQ_1275 1275
#define TH2_FREQ_1225 1225
#define TH2_FREQ_1125 1125
#define TH2_FREQ_1050 1050
#define TH2_FREQ_950 950
#define TH2_FREQ_850 850

/* freq and bandwidth : TD3 */
#define TD3_FREQ_1700 1700
#define TD3_FREQ_1625 1625
#define TD3_FREQ_1525 1525
#define TD3_FREQ_1425 1425
#define TD3_FREQ_1325 1325
#define TD3_FREQ_1275 1275
#define TD3_FREQ_1012 1012
#define TD3_FREQ_850 850
#define TD3_BW_1700 1142000
#define TD3_BW_1625 1092000
#define TD3_BW_1525 1024000
#define TD3_BW_1425 957000
#define TD3_BW_1325 890000
#define TD3_BW_1275 856000
#define TD3_BW_1012 680000
#define TD3_BW_850 571000

#define TD3_FREQ_1525_CAL_LEN 409
#define TD3_FREQ_1425_CAL_LEN 383
#define TD3_FREQ_1325_CAL_LEN 356
#define TD3_FREQ_1012_CAL_LEN 272
#define TD3_FREQ_850_CAL_LEN 228

#define TD3_ANCL_CELLS 12

#if defined(BCM_ESW_SUPPORT)

/*
 * Struct:
 *      stream_rate_t
 *
 * Description:
 */
typedef struct stream_rate_calc_s {
    pbmp_t pbmp;         /* port bitmap */
    uint32 mode;         /* 0-> check by max rx speed;
                            1-> check by actual tx speed */
    uint32 pkt_size;     /* packet size */
    uint32 interval_len; /* length of interval in second */
    uint32 tolerance_lr; /* rate tolerance percentage for linerate */
    uint32 tolerance_os; /* rate tolerance percentage for oversub */
    uint32 src_port[SOC_MAX_NUM_PORTS];/* source port array */
    uint32 num_rep[SOC_MAX_NUM_PORTS]; /* number of same-port replications */
    uint32 scaling_factor;             /* emulator speedup factor */
    uint32 emulation_param;             /* emulator speedup factor */
    uint32 traffic_load;
} stream_rate_t;

/*
 * Struct:
 *      stream_integrity_t
 *
 * Description:
 */
typedef struct stream_pkt_intg_s{
    int type;       /* packet type: PKT_TYPE_L2, PKT_TYPE_IPMC, etc. */
    int ipv6_en;    /* enable ipv6, 1->enable, 0->disable  */
    int sv_tag_en[SOC_MAX_NUM_PORTS];    /* enable sv_tag, 1->enable, 0->disable  */
    uint32 sv_tag[SOC_MAX_NUM_PORTS];  /* SV tag */
    pbmp_t tx_pbmp; /* tx port bitmap */
    pbmp_t rx_pbmp; /* rx port bitmap */
    uint32 tx_vlan[SOC_MAX_NUM_PORTS];        /* tx vlan array */
    uint32 rx_vlan[SOC_MAX_NUM_PORTS];        /* tx vlan array */
    uint32 port_pkt_seed[SOC_MAX_NUM_PORTS];  /* port packet seed array */
    uint32 port_flood_cnt[SOC_MAX_NUM_PORTS]; /* port flood count array */
    uint8 mac_da[SOC_MAX_NUM_PORTS][NUM_BYTES_MAC_ADDR];
    uint8 mac_sa[SOC_MAX_NUM_PORTS][NUM_BYTES_MAC_ADDR];
    uint32 ip_da[SOC_MAX_NUM_PORTS]; /* port ip DA array */
    uint32 ip_sa[SOC_MAX_NUM_PORTS]; /* port ip SA array */
} stream_integrity_t;

/*
 * Struct:
 *      stream_pkt_t
 *
 * Parameters:
 */
typedef struct stream_pkt_s{
    int port;
    int num_pkt;           /* expected number of tx/rx packets */
    int cnt_pkt;           /* actual number of tx/rx packets */
    uint32 pkt_seed;       /* random seed for pkt generation */
    uint32 pkt_size;       /* constant pkt size */
    int rand_pkt_size_en;  /* enable random pkt size */
    uint32 *rand_pkt_size; /* random pkt size array */
    int l3_en;             /* 1 -> enable, 0 -> disable (default) */
    uint8 ipv6_en;         /* 1 -> enable, 0 -> disable (default) */
    uint32 tx_vlan;
    uint32 rx_vlan;
    uint8 mac_da[NUM_BYTES_MAC_ADDR];
    uint8 mac_sa[NUM_BYTES_MAC_ADDR];
    uint32 ip_da;
    uint32 ip_sa;
    uint8 ttl;
    uint8 new_flow_en;  /* Enables MPLS, IPv4/6 & L2 pkts */
    uint8 l3_mpls_en;   /* 3 MPLS labels + L3 pkt: 1 -> enable, 0 -> disable (default) */
    /* mpls_labels[0] -  Top MPLS label Not-BOS
     * mpls_labels[1] -  Middle MPLS label Not-BOS
     * mpls_labels[2] -  BOS MPLS label
     */
    uint32 *mpls_labels; /* Top MPLS label Not-BOS */
    /*
     * ipv6_da[0]    31:0
     * ipv6_da[1]   63:32
     * ipv6_da[2]   95:64
     * ipv6_da[3]  127:96
     */
    uint32 *ipv6_da;
    uint32 *ipv6_sa;
    int sv_tag_en;    /* enable sv_tag, 1->enable, 0->disable  */
    uint32 sv_tag;  /* SV tag */

} stream_pkt_t;

extern void stream_print_port_config(int unit, pbmp_t pbmp);

extern uint32 stream_get_port_pipe(int unit, int port);
extern uint32 stream_get_pkt_cell_cnt(int unit, uint32 pkt_size, int port);
extern uint32 stream_get_safe_pkt_size(int unit, int ovs_ratio_x1000);
extern uint32 stream_get_ll_flood_cnt(int unit, int port, uint32 pkt_size,
                                      uint32 *pkt_size_rand);
extern uint32 stream_get_pipe_bandwidth(int unit, uint32 flag);
extern uint32 stream_get_ancl_bandwidth(int unit, uint32 flag);
extern uint32 stream_get_wc_pkt_size(int unit, int hg_en);

extern void stream_set_vlan(int unit, bcm_vlan_t vlan, int enable);
extern void stream_set_vlan_valid(int unit, bcm_vlan_t vlan);
extern void stream_set_vlan_invalid(int unit, bcm_vlan_t vlan);
extern bcm_error_t stream_set_lpbk(int unit, pbmp_t pbmp, int loopback);
extern bcm_error_t stream_set_pmd_lpbk(int unit, bcm_port_t port,
                                       bcm_port_phy_control_t type,
                                       uint32 value);
extern void stream_set_mac_lpbk(int unit, pbmp_t pbmp);
extern void stream_set_phy_lpbk(int unit, pbmp_t pbmp);
extern void stream_set_no_lpbk(int unit, pbmp_t pbmp);

extern void stream_set_l3mtu_valid(int unit, bcm_vlan_t vlan);

extern void stream_turn_off_cmic_mmu_bkp(int unit);
extern void stream_turn_off_fc(int unit, pbmp_t pbmp);

extern void stream_gen_random_l2_pkt(uint8 *pkt_ptr,
                                     uint32 pkt_size,uint32 seq_id,
                                     uint8 mac_da[NUM_BYTES_MAC_ADDR],
                                     uint8 mac_sa[NUM_BYTES_MAC_ADDR],
                                     uint16 tpid, uint16 vlan_id);
extern void stream_gen_random_l3_pkt(uint8 *pkt_ptr, uint8 ipv6_en,
                                     uint32 pkt_size, uint32 seq_id,
                                     uint8 mac_da[NUM_BYTES_MAC_ADDR],
                                     uint8 mac_sa[NUM_BYTES_MAC_ADDR],
                                     uint16 vlan_id,
                                     uint32 ip_da, uint32 ip_sa,
                                     uint8 ttl, uint8 tos, uint8 sv_tag_en,
                                     uint32 sv_tag);

extern void stream_gen_random_mpls_l3_pkt(uint8 *pkt_ptr,
                                     stream_pkt_t *tx_pkt,
                                     uint32 pkt_size,
                                     uint32 seq_id,
                                     uint8 tos);

extern bcm_error_t stream_pktdma_tx(int unit, dv_t *dv, uint8 *pkt, uint16 cnt);
extern bcm_error_t stream_pktdma_rx(int unit, int rst_vlan, bcm_vlan_t vlan,
                                    dv_t *dv, uint8 *pkt, uint16 cnt);

extern bcm_error_t stream_tx_pkt(int unit, stream_pkt_t *tx_pkt);

extern bcm_error_t stream_chk_dma_chain_done(int unit, int vchan,
                                             soc_dma_poll_type_t type,
                                             int *detected);
extern bcm_error_t stream_chk_mib_counters(int unit, pbmp_t pbmp, int flag);
extern bcm_error_t stream_chk_port_rate(int unit, pbmp_t pbmp,
                                        stream_rate_t *rate_calc);
extern bcm_error_t stream_chk_pkt_integrity(int unit,
                                            stream_integrity_t *pkt_intg);
extern bcm_error_t stream_calc_exp_rate_by_rx(int unit, uint64 *port_rate_exp,
                                stream_rate_t *rate_calc);
extern bcm_error_t stream_get_reg_tpkt_tbyt(int unit, int port,
                                soc_reg_t* reg_tpkt, soc_reg_t* reg_tbyt);
extern bcm_error_t record_rate_information(int unit, uint32 emulation_param,
                pbmp_t pbmp, uint64 *time, uint64 *pkt_cnt, uint64 *byt_cnt);
extern void calc_act_port_rate(int unit, pbmp_t pbmp, uint64 *start_time,
                uint64 *end_time, uint64 *pkt_start, uint64 *pkt_end,
                uint64 *byt_start, uint64 *byt_end, uint64 *port_rate);
extern bcm_error_t compare_rates(int unit, pbmp_t pbmp, uint32 tolerance_lr,
              uint32 tolerance_os, uint64 *port_rate_exp, uint64 *port_rate_act);
extern int stream_get_exact_speed(int speed, int encap);
extern uint64 get_cur_time(int unit, uint32 emulation_param);
extern void start_cmic_timesync(int unit);
extern uint64 get_ts_count(int unit);


extern bcm_error_t stream_gen_ref_pkt(int unit, int port,
                                      stream_integrity_t *pkt_intg,
                                      uint8 *rx_pkt, uint8 *ref_pkt,
                                      uint32 *pkt_size);
extern void stream_set_l3mtu_valid(int unit, bcm_vlan_t vlan);


#endif /* BCM_ESW_SUPPORT */

#endif /* STREAMING_LIB_H */
