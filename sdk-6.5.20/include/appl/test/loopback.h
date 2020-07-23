/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Loopback Test definitions.
 */
#ifndef __LOOPBACKTEST__H
#define __LOOPBACKTEST__H

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

#define LB_MAC_CHAIN    4       /* # DCBs in MAC loopback chain */
#define LB_PHY_CHAIN    4       /* # DCBs in PHY loopback chain */
#define LB_EXT_CHAIN    4       /* # DCBs in EXT loopback chain */
#define LB_SNAKE_CHAIN  1       /* # DCBs in SNAKE loopback chain */
#define LB_VLAN_ID      1
#define CHAN_CONFIG_COUNT         12  /* 4-choose-2 DMA config combos */
#define CMICX_CHAN_CONFIG_COUNT   56  /* 8-choose-2 DMA config combos */
#define LB_LINKSCAN_PERIOD  100000
#define LB_LINK_TIMEOUT     10000000 /* Microseconds */
#define LB_PACKET_TIMEOUT   5
#define LB_PACKET_TIMEOUT_QT    60
#define LB_PACKET_TIMEOUT_PLI   (3*60)
#define LB_FFP_PARAM_BYTES      6
#define LB_JUMBO_MAXSZ          0x3fe8  /* Maximum size of loopback packets */
#define LB_LEGACY_DMA_MODE      0
#define LB_CONTINUOUS_DMA_MODE  1

#define MH_BYTES        16

/* Specific checks for DMA modes */
#define LB_DMA_MODE_IS_LEGACY(m) (m == LB_LEGACY_DMA_MODE)
#define LB_DMA_MODE_IS_CONTINUOUS(m) (m == LB_CONTINUOUS_DMA_MODE)

/* Loopback test data, attributes of a given loopback test */
typedef struct loopback_testdata_s {
    pbmp_t      lp_pbm;                 /* Ports under test */
    pbmp_t      lp_pbm_tx;              /* Ports under test, TX only */
    pbmp_t      lp_ext_self_loop_pbm;   /* Ports which have external loopback cables */
    soc_port_t  lp_tx_port;             /* Current TX port */
    soc_port_t  lp_rx_port;             /* Current RX port */
    soc_port_t  lp_dst_inc;             /* Destination increment */
    pbmp_t      lp_ubm;                 /* Untag bit mask */
    uint32      lp_pattern;             /* Data Pattern */
    uint32      lp_pattern_inc;         /* Data Pattern increment  */
    uint32      lp_vlan;                /* Vlan tag */
    int         lp_d_mod;               /* Destination module, HiGig hdr */
    int         lp_d_mod_inc;           /* HiGig modid increment */
    uint32      lp_d_port;              /* Destination port, HiGig hdr */
    uint32      lp_opcode;              /* Opcode, HiGig hdr */
    int     lp_l_start;             /* Start length */
    int     lp_l_end;               /* End length */
    int     lp_l_inc;               /* Length increment */
    int     lp_c_start;             /* Start chain count */
    int     lp_c_end;               /* End chain count */
    int     lp_c_inc;               /* Chain count increment */
    int     lp_dv_start;            /* Starting # DV chains */
    int     lp_dv_end;              /* End # DV chains */
    int     lp_dv_inc;              /* Starting # DV chains */
    int     lp_ppc_start;           /* Packets per chain start */
    int     lp_ppc_end;             /* Packets per chain end */
    int     lp_ppc_inc;             /* Packets per chain increment */
    int     lp_dpp_start;           /* DCBs per packet start*/
    int     lp_dpp_end;             /* DCBs per packet end */
    int     lp_dpp_inc;             /* DCBs per packet increment */
    int     lp_sg;                  /* S/G if TRUE, lp_ppc_X != lp_c_X */
    int     lp_seed;                /* Random seed (0 == don't set) */
    int     lp_count;               /* # chains of each */
    int     lp_cos_start;           /* Starting COS value */
    int     lp_cos_end;             /* Ending COS value */
    int     lp_crc_mode;            /* CRC generation mode */
    int     lp_check_data;          /* Check data integrity on receive */
    int     lp_check_crc;           /* CRC data integrity on receive */
    int     lp_speed;               /* Speed to configure ports */
    int     lp_autoneg;             /* Enable autoneg (ext lb only) */
    sal_mac_addr_t lp_mac_src;      /* SRC Mac address under test */
    uint32  lp_mac_src_inc;         /* Source mac increment */
    sal_mac_addr_t lp_mac_dst;      /* SRC Mac address under test */
    uint32  lp_mac_dst_inc;         /* Destination mac increment */
    int     lp_duration;            /* Duration time for Snake Test */
    int     lp_two_way;             /* Both directions of snake path */
} loopback_testdata_t;


/* Loopback test structure, consolidated testdata for each
 * type of loopback test.
 */

typedef struct loopback_test_s {
    int     lw_set_up;      /* TRUE if lb_setup() done */
    loopback_testdata_t lw_lp_dma;   /* DMA loopback Parameters */
    loopback_testdata_t lw_lp_mac;   /* Mac Loopback Parameters */
    loopback_testdata_t lw_lp_phy;   /* PHY loopback Parameters */
    loopback_testdata_t lw_lp_ext;   /* EXT loopback Parameters */
    loopback_testdata_t lw_lp_snake;     /* EXT Snake Parameters */
    loopback_testdata_t lw_lp_sg_dma;    /* Scatter/Gather Loopback DMA */
    loopback_testdata_t lw_lp_sg_random; /* Scatter/Gather Random DMA */
    loopback_testdata_t *lw_lp;      /* Current lp parameters */
    int     lw_unit;         /* Unit # */
    bcm_port_info_t lw_save_port[SOC_MAX_NUM_PORTS]; /* Port information */
    uint32   lw_save_port_data[SOC_MAX_NUM_PORTS]; /* Port info to restore information (mostly used for SHADOW)*/

    int     lw_save_linkscan_period;
    int         lw_save_vlan_valid;
    bcm_stg_t   lw_save_vlan_stg;
    pbmp_t      lw_save_vlan_pbmp;
    pbmp_t      lw_save_vlan_ubmp;
    volatile int lw_eoc_tx;     /* TRUE --> End-of-chain seen */
    volatile int lw_eoc_rx;     /* TRUE --> End-of-chain seen */
    volatile dv_t *lw_tx_dv_chain_done; /* Pointer to dv waiting for */
    volatile dv_t *lw_rx_dv_chain_done; /* Pointer to dv waiting for */
    dv_t    **lw_rx_dv;     /* RX dv pointer */
    dv_t    **lw_tx_dv;     /* TX dv pointer */
    int     lw_dma_mode;    /* Legacy or Continuous Mode(CMICDV2) */
    int     lw_tx_started;  /* TRUE if Continuous Mode and TX has started */
    int     lw_rx_started;  /* TRUE if Continuous Mode and RX has started */
    volatile int lw_tx_dv_idx;   /* Index of TX DV in array Continuous Mode only */
    volatile int lw_rx_dv_idx;   /* Index of RX DV in array Continuous Mode only */
    int     lw_stats_init;      /* TRUE if stats initialized */
    int     lw_tx_seq;      /* Seq # in sent packet */
    int     lw_tx_total;        /* Total # to send (multiplied out)*/
    int     lw_tx_count;        /* Total packets sent so far */
    sal_time_t  lw_tx_stime;        /* Transmit start time */
    sal_time_t  lw_tx_rtime;        /* Time of next report */
    COMPILER_DOUBLE lw_tx_bytes;    /* Total bytes sent so far */
    int     lw_rx_count;        /* Total packets received so far */
    COMPILER_DOUBLE lw_rx_bytes;    /* Total bytes received so far */
    int     lw_rx_fill;     /* Fill RX buffer with 0xdeadbeef */

    sal_mac_addr_t lw_mac_src;      /* Source MAC address under test */
    uint32  lw_mac_src_inc;     /* Source MAC increment */
    sal_mac_addr_t lw_mac_dst;      /* Dest MAC address under test */
    uint32  lw_mac_dst_inc;     /* Dest MAC increment */

    sal_sem_t   lw_sema;        /* Sleep semaphore */
    volatile int lw_sema_woke;      /* Used to prevent excess semGives */
    uint32      lw_timeout_usec;        /* Timeout for packet operations */
    int     lw_tx_init_chn;     /* TX # in chain inited */
    int     lw_tx_init_len;     /* TX packet length inited */
    int     lw_tx_init_ppc;     /* TX packets/chain inited */

    int     lw_arl_src_delete;  /* TRUE --> Delete ARL when done */
    int     lw_arl_dst_delete;  /* TRUE --> Delete ARL when done */
    bcm_l2_addr_t    lw_arl_src;    /* Source port ARL entry */
    bcm_l2_addr_t    lw_arl_dst;    /* Destination port ARL entry */
    int     lw_tx_packet_cnt;   /* Allocate TX packets */
    int     lw_rx_packet_cnt;   /* Allocate RX packets */
    uint8   **lw_tx_packets;    /* Array of pointers to TX packets */
    uint8   **lw_rx_packets;    /* Array of pointers to RX packets */
    uint8   **lw_tx_dcbs;       /* Array of pointers to TX dcbs(only in cmicx) */
    sal_mac_addr_t lw_cur_mac_src;  /* SRC Mac address under test */
    sal_mac_addr_t lw_cur_mac_dst;  /* SRC Mac address under test */

    /* Snake test */

    int     lw_iPhase;      /* 0=none, 1=clean up FE, */
                    /*         2=clean up GE */
    int     lw_fwdPort[2];      /* forward port */
    int     lw_bwdPort[2];      /* backward port */
    int     lw_curC;        /* current Chain during setup */
    int     lw_ether_start;         /* Offset into start of eth hdr*/
    int         lw_c3_lrp_bypass;
    uint32  lw_tx_dcb_factor;       /* Number of tx dcbs per packet */
} loopback_test_t;


/*
 * A dma descriptor has two user-defined fields, one which we use
 * for packet data, the other for test data.
 */
#define dv_pckt dv_public1.u32      /* Which packet is being worked on */
#define dv_dsc  dv_public2.u32      /* RX: idx of next to look at */


extern loopback_test_t* lb_get_test(int unit);
extern loopback_testdata_t* lb_get_dma_testdata(int unit);

extern int lb_dma_testapi_init(int unit,
            uint32 pattern,
            uint32 patternIncrement,
            int startLength,
            int endLength,
            int lengthIncrement,
            int chainStart,
            int chainEnd,
            int chainIncrement,
            int cosStart,
            int cosEnd,
            int countPackets,
            int checkData);

extern int lb_dma_testapi_run(int unit,
               loopback_test_t  *lw,
               loopback_testdata_t  *lp);

extern int lb_dma_testapi_done(int unit);


#endif /*!__LOOPBACKTEST__H */
