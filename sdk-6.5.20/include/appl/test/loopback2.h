/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Loopback Test definitions.
 */

#ifndef __LOOPBACK2TEST__H
#define __LOOPBACK2TEST__H

#include <sal/types.h>
#include <sal/compiler.h>

#include <soc/mem.h>
#include <soc/dma.h>
#include <soc/enet.h>
#include <soc/counter.h>
#include <soc/register.h>
#include <soc/memory.h>

#include <bcm/port.h>
#include <bcm/l2.h>
#include <bcm/rx.h>
#include <bcm/pkt.h>

#include <appl/diag/parse.h>

#define LB2_VLAN_ID             1
#define LB2_LINKSCAN_PERIOD     100000
#define LB2_LINK_TIMEOUT        10000000 /* Microseconds */

#define LB2_PACKET_TIMEOUT      5        /* Seconds */
#define LB2_PACKET_TIMEOUT_QT   60       /* Seconds */
#define LB2_PACKET_TIMEOUT_PLI  (3*60)   /* Seconds */
#define LB2_FFP_PARAM_BYTES     6

#define LB2_TH_ASCEND_PORT_MODE         0   /* ports begin with firt port in ascending order */
#define LB2_TH_DSCEND_PORT_MODE         1   /*ports begin with last port in descending order */
#define LB2_TH_MAX_PORTS_WITH_PROFILE   127 /* maximum ports supported for TH/TH+ due to profile management limitation */

enum lb2_test_type_e {
    LB2_TT_MAC = 0,
    LB2_TT_PHY,
    LB2_TT_EXT,
    LB2_TT_SNAKE,
    LB2_TT_NUM
};

/* Matching parse arrays are in lb_util.c */

/* MUST MATCH LB2_SPEED_INIT_STR ordering below used in lb_util.c */
enum lb2_speed_e {
    LB_SPEED_MAX = 0,
    LB_SPEED_10FD,
    LB_SPEED_100FD,
    LB_SPEED_1000FD,
    LB_SPEED_2500FD,
    LB_SPEED_3000FD,
    LB_SPEED_5000FD,
    LB_SPEED_10G_FD,
    LB_SPEED_11G_FD,
    LB_SPEED_12G_FD,
    LB_SPEED_13G_FD,
    LB_SPEED_15G_FD,
    LB_SPEED_16G_FD,
    LB_SPEED_20G_FD,
    LB_SPEED_21G_FD,
    LB_SPEED_23G_FD,
    LB_SPEED_24G_FD,
    LB_SPEED_25G_FD,
    LB_SPEED_30G_FD,
    LB_SPEED_32G_FD,
    LB_SPEED_40G_FD,
    LB_SPEED_42G_FD,
    LB_SPEED_50G_FD,
    LB_SPEED_100G_FD,
    LB_SPEED_106G_FD,
    LB_SPEED_120G_FD,
    LB_SPEED_127G_FD,
    LB_SPEED_200G_FD,
    LB_SPEED_400G_FD
};

#define LB2_SPEED_INIT_STR      \
    "MAXimum",                  \
    "10fullduplex",             \
    "100fullduplex",            \
    "1000fullduplex",           \
    "2500fullduplex",           \
    "3000fullduplex",           \
    "5000fullduplex",           \
    "10Gfullduplex",            \
    "11Gfullduplex",            \
    "12Gfullduplex",            \
    "13Gfullduplex",            \
    "15Gfullduplex",            \
    "16Gfullduplex",            \
    "20Gfullduplex",            \
    "21Gfullduplex",            \
    "23Gfullduplex",            \
    "24Gfullduplex",            \
    "25Gfullduplex",            \
    "30Gfullduplex",            \
    "32Gfullduplex",            \
    "40Gfullduplex",            \
    "42Gfullduplex",            \
    "50Gfullduplex",            \
    "100Gfullduplex",           \
    "106Gfullduplex",           \
    "120Gfullduplex",           \
    "127Gfullduplex",           \
    "200Gfullduplex",           \
    "400Gfullduplex"

/* MUST MATCH lb2_crc_mode ordering */
enum lb2_crc_mode_e {
    LB2_CRC_MODE_CPU_NONE = 0,
    LB2_CRC_MODE_CPU_APPEND,
    LB2_CRC_MODE_MAC_REGEN
};

/* MUST MATCH lb2_parse_snake ordering */
enum lb2_snake_path_e {
    LB2_SNAKE_INCR = 0,
    LB2_SNAKE_DECR,
    LB2_SNAKE_BOTH
};

/* MUST MATCH lb2_parse_lbmode ordering */
enum lb2_mode_e {
    LB2_MODE_MAC = 0,
    LB2_MODE_PHY,
    LB2_MODE_EXT
};

/* Loopback test data, attributes of a given loopback test */
typedef struct loopback2_testdata_s {
    int         test_type;           /* Parameters for which test? */
    pbmp_t      pbm;                 /* Ports under test */
    pbmp_t      pbm_tx;              /* Ports under test, TX only */
    soc_port_t  dst_inc;             /* Destination increment */
    pbmp_t      ubm;                 /* Untag bit mask */
    uint32      pattern;             /* Data Pattern */
    uint32      pattern_inc;         /* Data Pattern increment  */
    uint32      vlan;                /* Vlan tag */
    int         d_mod;               /* Destination module, HiGig hdr */
    int         d_mod_inc;           /* HiGig modid increment */
    uint32      d_port;              /* Destination port, HiGig hdr */
    uint32      opcode;              /* Opcode, HiGig hdr */
    int         len_start;           /* Start length */
    int         len_end;             /* End length */
    int         len_inc;             /* Length increment */
    int         ppt_start;           /* Packets/trial, start */
    int         ppt_end;             /* Packets/trial, end */
    int         ppt_inc;             /* Packets/trial, increment */
    int         send_mech;           /* Sending mechanism per chain: */
#define LBD_SEND_SINGLE 0            /* Single step packets */
#define LBD_SEND_ARRAY  1            /* Send as array */
#define LBD_SEND_LLIST  2            /* Note Yet Supported */
    int         iterations;          /* Number of times to iterate test */
    int         cos_start;           /* Starting COS value */
    int         cos_end;             /* Ending COS value */
    int         crc_mode;            /* CRC generation mode */
    int         check_data;          /* Check data integrity on receive */
    int         check_crc;           /* CRC data integrity on receive */
    int         speed;               /* Speed to configure ports */
    int         autoneg;             /* Enable autoneg (ext lb only) */
    sal_mac_addr_t mac_src;          /* SRC Mac address under test */
    uint32      mac_src_inc;         /* Source mac increment */
    sal_mac_addr_t mac_dst;          /* SRC Mac address under test */
    uint32      mac_dst_inc;         /* Destination mac increment */
    int         duration;            /* Duration time for Snake Test */
    int         interval;            /* Check frequency for iSnake Test */
    int         two_way;             /* Both directions of snake path */
    int         snake_way;           /* Direction(s) of snake path */
    int         loopback;            /* Select loopback mode */
    int         inject;              /* Setup ISnake and TX only */
    int         port_select_mode;    /* Only valid for TH/TH+ of 128 ports
                                        0(default):use 127 ports begin with firt port in ascending order;
                                        1: use 127 ports begin with last port in descending order */
    pbmp_t      saved_pbm;             /* Saved ports under test */
#ifdef BCM_TOMAHAWK_SUPPORT
    int         line_rate;           /* line_rate snake test*/
    int         line_rate_count;     /* packet number in line rate mode */
#endif
} loopback2_testdata_t;

/* Info for forwarding a packet from one port to the next in snake */
typedef struct lb2s_port_connect_s {
    bcm_port_t  this_port;
    bcm_port_t  to_port;
    bcm_vlan_t  added_vlan;
    sal_mac_addr_t dst_mac;
    sal_mac_addr_t src_mac;
    int         dst_modid;
    int         src_modid;
    ip_addr_t   dst_ip;
    ip_addr_t   src_ip;
    int         valid_info;
} lb2s_port_connect_t;


typedef struct loopback2_test_s {
    int         current_test_type;   /* Which test is active */
    int         set_up;              /* TRUE if lb_setup() done */
    loopback2_testdata_t params[LB2_TT_NUM];  /* Loopback Parameters */
    loopback2_testdata_t *cur_params;         /* Current lp parameters */
    int         unit;                /* Unit # */
    bcm_port_info_t save_port[SOC_MAX_NUM_PORTS]; /* Port information */
#ifdef BCM_TOMAHAWK3_SUPPORT
    bcm_port_resource_t port_info[SOC_MAX_NUM_PORTS]; /* Original FEC,
                                                       * phy lane, link
                                                       * training info to be
                                                       * saved and restored
                                                       */
#endif /* BCM_TOMAHAWK3_SUPPORT */
    int         save_linkscan_period;
    int         stats_init;          /* TRUE if stats initialized */
    bcm_port_t  tx_port;             /* Current TX port */
    bcm_port_t  rx_port;             /* Current RX port */
    int         tx_seq;              /* Seq # in sent packet */
    int         tx_total;            /* Total # to send (multiplied out)*/
    int         tx_count;            /* Total packets sent so far */
    sal_time_t  tx_stime;            /* Transmit start time */
    sal_time_t  tx_rtime;            /* Time of next report */
    uint32      tx_bytes;            /* Total bytes sent so far */
    int         rx_count;            /* Total packets received so far */
    uint32      rx_bytes;            /* Total bytes received so far */

    sal_mac_addr_t base_mac_src;     /* Source MAC address base for loop */
    sal_mac_addr_t base_mac_dst;     /* Dest MAC address base for loop */
    bcm_l2_addr_t l2_addr_scratch;   /* Working L2 entry */

    sal_sem_t   sema;                /* Sleep semaphore */
    volatile int sema_woke;          /* Used to prevent excess semGives */
    uint32      timeout_usec;        /* Timeout for packet operations */
    int         tx_len;              /* Current TX packet length */
    int         tx_cos;              /* Current TX COS */
    int         tx_ppt;              /* Current TX packets/chain */

    bcm_pkt_t    **tx_pkts;          /* Array of pointers to TX packets */
    bcm_pkt_t    *rx_pkts;           /* Array of RX packets */
    bcm_pkt_t    *rx_pkt_head;       /* Queue of rcvd packets */
    bcm_pkt_t    *rx_pkt_tail;

    volatile int expect_pkts;        /* TRUE --> Expecting packets */
    volatile int rx_pkt_cnt;         /* Number of packets received */

    /* Snake test */
    int          *tx_pkt_match;       /* TRUE --> Already matched tx pkt */

    lb2s_port_connect_t *port_cnct_f; /* Port interlink records, forward */
    lb2s_port_connect_t *port_cnct_r; /* Port interlink records, reverse */
    int         port_count;          /* Number of ports under test */
} loopback2_test_t;

#define LB2_RX_PKT(lw, idx)       (&(lw)->rx_pkts[idx])
#define LB2_RX_CUR_PKT(lw)        (&(lw)->rx_pkts[(lw)->rx_pkt_cnt])
#define LB2_PKT_NONE              0
#define LB2_PKT_EXPECTED          1
#define LB2_PKT_UNEXPECTED        2

#define LB2_ID_POS                24 /* L3 may need this larger */
#define LB2_SNAKE_TOLERANCE       50

/*
 * Port stats we monitor on a per-port basis while the test is running.
 * (Lifted from traffic.h)
 */
typedef struct {
    int      initialized;
    int      error;
    uint64   rbyt;
    uint64   rpkt;
    uint64   raln;
    uint64   rdisc;
    uint64   tbyt;
    uint64   tpkt;
    uint64   dropped;
#ifdef BCM_TOMAHAWK_SUPPORT
    uint64   rate;
#endif
} lb2_port_stat_t;

extern void lbu_pkt_param_add(int unit, parse_table_t *pt, 
                       loopback2_testdata_t *lp);
extern void lbu_port_param_add(int unit, parse_table_t *pt, 
                        loopback2_testdata_t *lp);
extern void lbu_other_param_add(int unit, parse_table_t *pt, 
                         loopback2_testdata_t *lp);

extern void lbu_setup(int unit, loopback2_test_t *lw);
extern int lbu_setup_arl_cmic(loopback2_test_t *lw);
extern void lbu_cleanup_arl(loopback2_test_t *lw);
extern int lbu_check_parms(loopback2_test_t *lw, loopback2_testdata_t *lp);
extern int lbu_init(loopback2_test_t *lw, loopback2_testdata_t *lp);
extern int lbu_snake_init(loopback2_test_t *lw, loopback2_testdata_t *lp);

extern void lbu_stats_init(loopback2_test_t *lw);
extern void lbu_stats_done(loopback2_test_t *lw);

extern int lbu_serial_txrx(loopback2_test_t *lw);
extern int lbu_snake_tx(loopback2_test_t *lw);
extern int lbu_snake_txrx(loopback2_test_t *lw, lb2_port_stat_t  *stats);

extern int lbu_done(loopback2_test_t *lw);
extern int lbu_snake_done(loopback2_test_t *lw);

extern int lbu_connect_ports(int unit, lb2s_port_connect_t *pc_info, int connect);
extern int lbu_port_monitor_task(int unit, bcm_rx_cb_f callback, void *cookie);
extern bcm_rx_t lbu_rx_callback(int unit, bcm_pkt_t *info, void *cookie);
extern int lbu_snake_stats(int unit, bcm_pbmp_t sel_pbm, int mac_lb, 
                    lb2_port_stat_t *stats);
extern void lbu_snake_dump_stats(int unit, bcm_pbmp_t sel_pbm,
                          lb2_port_stat_t *stats);
extern int lbu_snake_analysis(loopback2_test_t *lw);
#endif /*!__LOOPBACK2TEST__H */
