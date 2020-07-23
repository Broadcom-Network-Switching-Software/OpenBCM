/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * socdiag tx (transmit) and rx (receive) commands
 */

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/thread.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/higig.h>
#include <soc/dma.h>
#include <soc/dcb.h>
#include <soc/drv.h>

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#else
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <bcm/tx.h>
#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <appl/diag/system.h>
#include <appl/diag/dport.h>
#include <appl/diag/visibility.h>

#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif
#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/common/multicast.h>
#endif

#if defined(BCM_XGS_SUPPORT) && defined(BCM_CPU_TX_PROC_SUPPORT)
#include <soc/proc_hdr.h>
#endif

/* These modes must correspond with the order of strings below */
#define ENCAP_IEEE      0
#define ENCAP_HIGIG     1
#define ENCAP_B5632     2
#define ENCAP_HIGIG2    3 /* Note: no BCM_PORT_ENCAP_ equivalent */
#define ENCAP_RAW       4 /* Note: no BCM_PORT_ENCAP_ equivalent */
#define ENCAP_COUNT     5 /* Number of CLI encapsulation modes */

static char *hdr_modes[] = {
    "Ieee",
    "Higig",
    "B5632",
    "Higig2",
    "Raw",
    NULL};

typedef struct xd_s {                   /* TX/RX description */
    int         xd_unit;                /* Unit # */
    int         xd_tx_unit;             /* TX device */
    int         xd_ppsm;                /* true --> per port src macs */
    enum {
        XD_IDLE,                        /* Not doing anything */
        XD_RUNNING,                     /* Running */
        XD_STOP                         /* Stop requested */
    }           xd_state;
    uint32      xd_tot_cnt;             /* # to send/receive */
    uint32      xd_cur_cnt;             /* # sent/received */
    int         xd_pkt_len;             /* Packet length */
    char        *xd_file;               /* File name of packet data */
    sal_mac_addr_t  xd_mac_dst;         /* Destination mac address */
    sal_mac_addr_t  xd_mac_src;         /* Source mac address */
    sal_mac_addr_t  xd_mac_src_base;    /* Source mac address for pps */
                                        /* port source mac */
    uint32      xd_mac_dst_inc;         /* Destination mac increment */
    uint32      xd_mac_src_inc;         /* Source mac increment */
    uint32      xd_pat;                 /* XMIT pattern */
    uint32      xd_pat_inc;             /* Pattern Increment */
    int         xd_pat_random;          /* Use Random Pattern */
    pbmp_t      xd_ppsm_pbm;            /* Saved port bit map for per */
                                        /* port source mac */
    uint32      xd_vlan;                /* vlan id (0 if untagged) */
    uint32      xd_prio;                /* vlan prio */
    uint32      xd_prio_int;            /* Internal priority */
    uint32      xd_cong_int;            /* Internal congestion */
    uint32      xd_crc;

    /* Packet info is now in bcm_pkt_t structure */
    bcm_pkt_t   pkt_info;

    /* XGS Header Fields.  Most are in pkt_info */
    int         hdr_mode;               /* Mode type */
    int         ether_pkt;              /* Use Ethernet packet */
    uint32      w0,w1,w2,w3,w4;         /* Raw mode words */
    uint32      xd_hg_dmod;             /* Higig Dest Mod */
    uint32      xd_hg_dport;            /* Higig Dest Port */
    uint32      xd_hg_smod;             /* Higig Src Mod */
    uint32      xd_hg_sport;            /* Higig Src Port */
    uint32      xd_hg_opcode;           /* Higig Opcode */
    uint32      xd_hg_cos;              /* Higig Class of Service */
    uint32      xd_hg_pfm;              /* Higig Port Filtering Mode */
#ifdef BCM_HIGIG2_SUPPORT
    uint32      xd_hg_mgid;             /* Higig2 Multicast Group ID */
    uint32      xd_hg_lbid;             /* Higig2 Load Balancing ID */
    uint32      xd_hg_dp;               /* Higig2 Drop Precendece */
#endif /* BCM_HIGIG2_SUPPORT */
    uint32      xd_s_portid;            /* BCM5632 Source port ID */
    uint32      xd_d_portid;            /* BCM5632 Destination port ID */
    uint32      xd_length;              /* BCM5632 Length Field */
    uint32      xd_start;               /* BCM5632 Start Field */
    int         xd_untag;               /* FB/ER Send Untagged */
    int         xd_purge;               /* FB/ER Send Purge */
    char        *xd_data;               /* packet data */
#if defined(BCM_TRIUMPH3_SUPPORT)
    int         xd_rpl_type;            /* replacement type */
    int         xd_rpl_offset;          /* replacement offset */
    int         xd_lm_ctr_index;          /* LM counter Index */
#endif

#ifdef BCM_INSTRUMENTATION_SUPPORT
    int         xd_vis_options;
    int         xd_vis_sport;
#endif /* BCM_INSTRUMENTATION_SUPPORT */
#ifdef BCM_CPU_TX_PROC_SUPPORT
    int         xd_ph_destination_type; /* CPU TX Proc Destination type */
    int         xd_ph_destination;      /* CPU TX Proc Destination */
    int         xd_ph_destination_port; /* CPU TX Proc Destination Port */
    int         xd_ph_source_port;      /* CPU TX Proc Source Port */
    int         xd_ph_routed_pkt;       /* CPU TX Proc Routed Packet */
    int         xd_ph_qos_fields_valid; /* CPU TX Proc QoS Fields Valid */
    int         xd_ph_dp;               /* CPU TX Proc Drop Precedence */
    int         xd_ph_mcast_lb_index_valid; /* CPU TX Proc Mcast LB index valid */
    int         xd_ph_mcast_lb_index;       /* CPU TX Proc Mcast LB index */
    int         xd_ph_ecmp_group_index;     /* CPU TX Proc Group index */
    int         xd_ph_ecmp_member_index;    /* CPU TX Proc Member index */
#endif /* BCM_CPU_TX_PROC_SUPPORT */
    int         xd_mc_qtype;            /*To MC queue(SOBMH) */
    int         xd_rqe_q_num;            /*RQE Queue num(SOBMH) */
    int         xd_link_down_tx;         /* Allow tx to link down port */
    int         xd_spid;                    /* Service Pool id */
    int         xd_spid_override;           /* Use the priority from the packet */
} xd_t;

static xd_t     *xd_units[SOC_MAX_NUM_DEVICES];

#define _XD_INIT(unit, xd) do { \
        if (xd_units[unit] == NULL) \
            _xd_init(unit); \
        if (xd_units[unit] == NULL) \
            return CMD_FAIL; \
        xd = xd_units[unit]; } while (0)

#define XD_FILE(xd)     ((xd)->xd_file != NULL && (xd)->xd_file[0] != 0)
#define XD_STRING(xd)   ((xd)->xd_data != NULL && (xd)->xd_data[0] != 0)

#define UNTAGGED_PACKET_LENGTH         1024

#if  defined(BCM_HIGIG2_SUPPORT) || defined(BCM_CPU_TX_PROC_SUPPORT)
STATIC char *drop_precedence[] = {
    "Green", "Red", "Invalid", "Yellow", NULL
};
#endif /* BCM_HIGIG2_SUPPORT ||  BCM_CPU_TX_PROC_SUPPORT */

#define XD_PRIO_INT_INVALID 0xffffffff 
#define XD_CONG_INT_INVALID 0xffffffff

#if  defined(BCM_CPU_TX_PROC_SUPPORT)
#define XD_DEST_TYPE_INVALID 0xff
#endif

STATIC void
_xd_init(int unit)
{
    xd_t        *xd;
    int         port;
    sal_mac_addr_t default_mac_src ={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    sal_mac_addr_t default_mac_dst ={ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15 };
    bcm_port_config_t pcfg;

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("WARNING: bcm ports not initialized\n");
        return;
    }

    xd = xd_units[unit];
    if (xd == NULL) {
        xd = sal_alloc(sizeof(xd_t), "xd");
        if (xd == NULL) {
            cli_out("WARNING: xd memory allocation failed\n");
            return;
        }
        sal_memset(xd, 0, sizeof(xd_t));
    }
#ifdef  BCM_XGS_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        xd->xd_hg_smod = 0;
    } else 
    if (bcm_stk_my_modid_get(unit, (int *)&xd->xd_hg_smod) < 0 ) {
        cli_out("WARNING: getting my_modid failed\n");
        if (xd_units[unit] == NULL) {
            sal_free(xd);
        }
        /* coverity[leaked_storage : FALSE] */
        return;
    }
    xd->xd_hg_dmod      = 0;
    xd->xd_hg_dport     = 0;
    xd->xd_hg_opcode    = SOC_HIGIG_OP_CPU;
    BCM_PBMP_ITER(pcfg.cpu, port) {
        xd->xd_hg_sport = port;
        break;
    }
    xd->xd_hg_pfm       = 0;
#endif  /* BCM_XGS_SUPPORT */
    
    /* get the encapsulation of the first higig port or default to IEEE */
    xd->hdr_mode = ENCAP_IEEE;
    BCM_PBMP_ITER(pcfg.stack_ext, port) {
        if (bcm_port_encap_get(unit, port, &xd->hdr_mode) < 0) {
            xd->hdr_mode = ENCAP_IEEE;
        }
        break;
    }

    xd->xd_unit         = unit;
    xd->xd_tx_unit      = unit;
    xd->xd_state        = XD_IDLE;
    xd->xd_file         = NULL;
    xd->xd_data         = NULL;
    xd->xd_pkt_len      = 68;
    xd->xd_pat          = 0x12345678;
    xd->xd_pat_inc      = 1;
    xd->xd_pat_random   = 0;
    BCM_PBMP_ASSIGN(xd->pkt_info.tx_pbmp, pcfg.all);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(xd->xd_unit)) {
        SOC_PBMP_CLEAR(xd->pkt_info.tx_pbmp);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    BCM_PBMP_ASSIGN(xd->xd_ppsm_pbm, pcfg.all);
    xd->xd_vlan         = 0x1;
    xd->xd_prio         = 0;
    xd->xd_prio_int     = XD_PRIO_INT_INVALID;
    xd->xd_cong_int     = XD_CONG_INT_INVALID;
    xd->xd_ppsm         = 0;

#if defined(BCM_CPU_TX_PROC_SUPPORT)
    xd->xd_ph_destination_type = XD_DEST_TYPE_INVALID;
#endif

    ENET_SET_MACADDR(xd->xd_mac_dst, default_mac_dst);
    ENET_SET_MACADDR(xd->xd_mac_src, default_mac_src);
    xd->xd_crc      = 1;                /* Re-gen CRC by default */

    if (xd->pkt_info.pkt_data) { /* Has been setup before */
        soc_cm_sfree(unit, xd->pkt_info._pkt_data.data);
        xd->pkt_info.pkt_data = NULL;
    }
    xd->pkt_info.flags = 0;

    if ((xd->pkt_info._pkt_data.data = (uint8 *)soc_cm_salloc(unit,
                                                              xd->xd_pkt_len,
                                                              "xd tx")) == NULL) {
        cli_out("WARNING: xd tx packet memory allocation failed\n");
        xd->pkt_info.pkt_data = NULL;
        xd->pkt_info._pkt_data.len = 0;
    } else {
        xd->pkt_info.pkt_data = &xd->pkt_info._pkt_data;
        xd->pkt_info.blk_count = 1;
        xd->pkt_info._pkt_data.len = xd->xd_pkt_len;
    }

    xd_units[unit] = xd;
}

bcm_pkt_t *
txrx_pkt_alloc(int unit, int nblocks, int *sizes, int flags)
{
    bcm_pkt_t *pkt;
    int i, j;

    if (!(pkt = sal_alloc(sizeof(bcm_pkt_t), "txrx pkt"))) {
        return NULL;
    }

    pkt->blk_count = nblocks;
    if (nblocks == 1) {
        pkt->pkt_data = &pkt->_pkt_data;
    } else {
        if (!(pkt->pkt_data = sal_alloc(sizeof(bcm_pkt_blk_t) * nblocks,
                                        "tx pdata"))) {
            sal_free(pkt);
            return NULL;
        }
    }

    for (i = 0; i < nblocks; i++) {
        pkt->pkt_data[i].len = sizes[i];
        if (!(pkt->pkt_data[i].data = soc_cm_salloc(unit, sizes[i],
                                                    "txrx data"))) {
            for (j = 0; j < i; j++) {
                soc_cm_sfree(unit, pkt->pkt_data[j].data);
            }
            if (nblocks > 1) {
                sal_free(pkt->pkt_data);
            }
            sal_free(pkt);
            return NULL;
        }
    }

    pkt->unit = unit;
    pkt->flags = flags;
    return pkt;
}

void
txrx_pkt_free(int unit, bcm_pkt_t *pkt)
{
    int i;

    for (i = 0; i < pkt->blk_count; i++) {
        soc_cm_sfree(unit, pkt->pkt_data[i].data);
    }

    if (pkt->pkt_data != &pkt->_pkt_data) {
        sal_free(pkt->pkt_data);
    }

    sal_free(pkt);
}

cmd_result_t
tx_parse(int u, args_t *a, xd_t *xd)
/*
 * Function:    txrx_parse
 * Purpose:     Parse input parameters into a xd structure.
 * Parameters:  u - unit #.
 *              a - pointer to arguments
 *              xd - pointer to xd structure to fill in.
 * Returns:     0 - success, -1 failed.
 */
{
    char *crc_list[] = {"None", "Recompute", "Append", NULL};
    parse_table_t       pt;
    int                 min_len, tagged;

    /* First arg is count */

    if (!ARG_CNT(a) || !isint(ARG_CUR(a))) {
        return(CMD_USAGE);
    }

    _XD_INIT(u, xd);

    xd->xd_tot_cnt = parse_integer(ARG_GET(a));

    parse_table_init(u, &pt);

    /* Add XGS Ethernet/BCM5632/HIGIG Options */
    if (xd->hdr_mode == ENCAP_IEEE) {
        if (SOC_IS_XGS3_SWITCH(u)) {
            parse_table_add(&pt, "Untagged", PQ_DFL|PQ_BOOL, 0,
                            &xd->xd_untag, NULL);
            parse_table_add(&pt, "HGSrcMod",        PQ_DFL|PQ_INT,  0,
                            &xd->xd_hg_smod,            NULL);
        }
        xd->pkt_info.flags &= (~(BCM_TX_HG_READY));
    } else if (xd->hdr_mode == ENCAP_HIGIG ||
               xd->hdr_mode == ENCAP_HIGIG2) {
        if (SOC_IS_XGS_FABRIC(u) || SOC_IS_XGS3_SWITCH(u)) {
            xd->pkt_info.flags |= (BCM_TX_HG_READY);
        }
        parse_table_add(&pt, "HGDestMod",       PQ_DFL|PQ_INT,  0,
                        &xd->xd_hg_dmod,            NULL);
        parse_table_add(&pt, "HGDestPort",      PQ_DFL|PQ_INT,  0,
                        &xd->xd_hg_dport, NULL);
        parse_table_add(&pt, "HGOpcode",        PQ_DFL|PQ_INT,  0,
                        &xd->xd_hg_opcode,          NULL);
        parse_table_add(&pt, "HGSrcMod",        PQ_DFL|PQ_INT,  0,
                        &xd->xd_hg_smod,            NULL);
        parse_table_add(&pt, "HGSrcPort",       PQ_DFL|PQ_INT,  0,
                        &xd->xd_hg_sport, NULL);
        parse_table_add(&pt, "PFM",             PQ_DFL|PQ_INT,  0,
                        &xd->xd_hg_pfm, NULL);
        parse_table_add(&pt, "EthernetPacket",  PQ_DFL|PQ_BOOL, 0,
                        &xd->ether_pkt,             NULL);
#ifdef BCM_HIGIG2_SUPPORT
        if (xd->hdr_mode == ENCAP_HIGIG2 &&
            soc_feature(u, soc_feature_higig2)) {
            /* Map traffic class to internal priority */
            parse_table_add(&pt, "TrafficClass",     PQ_DFL|PQ_INT,     0,
                            &xd->xd_prio_int, NULL);
            parse_table_add(&pt, "McastGroupID",     PQ_DFL|PQ_INT,     0,
                            &xd->xd_hg_mgid, NULL);
            parse_table_add(&pt, "LoadBalID",        PQ_DFL|PQ_INT,     0,
                            &xd->xd_hg_lbid, NULL);
            parse_table_add(&pt, "DropPrecedence",   PQ_DFL|PQ_MULTI,   0,
                            &xd->xd_hg_dp, drop_precedence);
        }
#endif /* BCM_HIGIG2_SUPPORT */
    } else if (xd->hdr_mode == ENCAP_B5632) {
        parse_table_add(&pt, "SourcePortID",    PQ_DFL|PQ_INT,  0,
                        &xd->xd_s_portid,           NULL);
        parse_table_add(&pt, "DestPortID",      PQ_DFL|PQ_INT,  0,
                        &xd->xd_d_portid,           NULL);
        parse_table_add(&pt, "Start",   PQ_DFL|PQ_INT,0,
                        &xd->xd_start, NULL);
        parse_table_add(&pt, "Length",  PQ_DFL|PQ_INT,0,
                            &xd->xd_length, NULL);
    } else if (xd->hdr_mode == ENCAP_RAW) {
        parse_table_add(&pt, "Word0",   PQ_DFL|PQ_INT,0,
                        &xd->w0, NULL);
        parse_table_add(&pt, "Word1",   PQ_DFL|PQ_INT,0,
                        &xd->w1, NULL);
        parse_table_add(&pt, "Word2",   PQ_DFL|PQ_INT,0,
                        &xd->w2, NULL);

#if defined(BCM_HIGIG2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
        if (soc_feature(u, soc_feature_higig2) || SOC_IS_TOMAHAWK3(u)) {
            parse_table_add(&pt, "Word3",   PQ_DFL|PQ_INT,0,
                            &xd->w3, NULL);
            parse_table_add(&pt, "Word4",   PQ_DFL|PQ_INT,0,
                            &xd->w4, NULL);
        }
#endif /* BCM_HIGIG2_SUPPORT */
    }

    parse_table_add(&pt, "TXUnit", PQ_DFL|PQ_INT, 0, &xd->xd_tx_unit, NULL);
    parse_table_add(&pt, "PortBitMap",      PQ_DFL|PQ_PBMP|PQ_BCM, 0,
                    &xd->pkt_info.tx_pbmp,      NULL);

#ifdef BCM_INSTRUMENTATION_SUPPORT
    parse_table_add(&pt, "VisibilityOptions",    PQ_DFL|PQ_INT,  0,
                    &xd->xd_vis_options,         NULL);
    parse_table_add(&pt, "VisibilitySourcePort",    PQ_DFL|PQ_PORT,  0,
                    &xd->xd_vis_sport,           NULL);
#endif /* BCM_INSTRUMENTATION_SUPPORT */

#ifdef BCM_CPU_TX_PROC_SUPPORT
    if (soc_feature(u, soc_feature_cpu_tx_proc)) {
        parse_table_add(&pt, "TxProcMhDestinationType",              PQ_DFL|PQ_INT,    0,
                        &xd->xd_ph_destination_type,                 NULL);
        parse_table_add(&pt, "TxProcMhSourcePort",                   PQ_DFL|PQ_PORT,   0,
                        &xd->xd_ph_source_port,                      NULL);
        parse_table_add(&pt, "TxProcMhDestinationPort",              PQ_DFL|PQ_PORT,   0,
                        &xd->xd_ph_destination_port,                 NULL);
        parse_table_add(&pt, "TxProcMhDestination",                  PQ_DFL|PQ_INT,    0,
                        &xd->xd_ph_destination,                      NULL);
        parse_table_add(&pt, "TxProcMhMcastLbIndexValid",            PQ_DFL|PQ_BOOL,   0,
                        &xd->xd_ph_mcast_lb_index_valid,             NULL);
        parse_table_add(&pt, "TxProcMhMcastLbIndex",                 PQ_DFL|PQ_INT,    0,
                        &xd->xd_ph_mcast_lb_index,                   NULL);
        parse_table_add(&pt, "TxProcMhQosFieldsValid",               PQ_DFL|PQ_BOOL,   0,
                        &xd->xd_ph_qos_fields_valid,                 NULL);
        parse_table_add(&pt, "TxProcMhDropPrecedence",               PQ_DFL|PQ_MULTI,  0,
                        &xd->xd_ph_dp,                               drop_precedence);
        parse_table_add(&pt, "TxProcMhInternalPriority",             PQ_DFL|PQ_INT,    0,
                        &xd->xd_prio_int,                            NULL);
        parse_table_add(&pt, "TxProcMhInternalCongestion",           PQ_DFL|PQ_INT,    0,
                        &xd->xd_cong_int,                            NULL);
        parse_table_add(&pt, "TxProcMhRoutedPkt",                    PQ_DFL|PQ_BOOL,   0,
                        &xd->xd_ph_routed_pkt,                       NULL);
        parse_table_add(&pt, "TxProcMhEcmpGroup",                    PQ_DFL|PQ_INT,    0,
                        &xd->xd_ph_ecmp_group_index,                 NULL);
        parse_table_add(&pt, "TxProcMhEcmpMember",                   PQ_DFL|PQ_INT,    0,
                        &xd->xd_ph_ecmp_member_index,                NULL);
    }
#endif /* BCM_CPU_TX_PROC_SUPPORT */

    if (!SOC_IS_XGS_FABRIC(u)) {
        parse_table_add(&pt, "UntagBitMap", PQ_DFL|PQ_PBMP|PQ_BCM, 0,
                        &xd->pkt_info.tx_upbmp,         NULL);
    }

    if (!SOC_IS_XGS3_SWITCH(u)) {
        if (SOC_IS_XGS_SWITCH(u)) {
            parse_table_add(&pt, "L3BitMap",    PQ_DFL|PQ_PBMP|PQ_BCM, 0,
                            &xd->pkt_info.tx_l3pbmp,    NULL);
        }
    }

    parse_table_add(&pt, "DATA",        PQ_DFL|PQ_STRING,0,
                    &xd->xd_data,           NULL);
    parse_table_add(&pt, "Filename",    PQ_DFL|PQ_STRING,0,
                    &xd->xd_file,           NULL);
    parse_table_add(&pt, "Length",      PQ_DFL|PQ_INT,  0,
                    &xd->xd_pkt_len,    NULL);
    parse_table_add(&pt, "VLantag",     PQ_DFL|PQ_HEX,  0,
                    &xd->xd_vlan,           NULL);
    parse_table_add(&pt, "VlanPrio",    PQ_DFL|PQ_INT,  0,
                    &xd->xd_prio,           NULL);
    parse_table_add(&pt, "PrioInt",     PQ_DFL|PQ_INT,  0,
                    &xd->xd_prio_int,       NULL);
    parse_table_add(&pt, "Pattern",     PQ_DFL|PQ_HEX,  0,
                    &xd->xd_pat,            NULL);
    parse_table_add(&pt, "PatternInc",  PQ_DFL|PQ_INT,  0,
                    &xd->xd_pat_inc,    NULL);
    parse_table_add(&pt, "PatternRandom", PQ_DFL|PQ_BOOL, 0,
                    &xd->xd_pat_random,    NULL);
    parse_table_add(&pt, "PerPortSrcMac",PQ_DFL|PQ_INT, 0,
                    &xd->xd_ppsm,    NULL);
    parse_table_add(&pt, "SourceMac",   PQ_DFL|PQ_MAC,  0,
                    &xd->xd_mac_src,    NULL);
    parse_table_add(&pt, "SourceMacInc",PQ_DFL|PQ_INT,  0,
                    &xd->xd_mac_src_inc,NULL);
    parse_table_add(&pt, "DestMac",     PQ_DFL|PQ_MAC,  0,
                    &xd->xd_mac_dst,    NULL);
    parse_table_add(&pt, "DestMacInc",  PQ_DFL|PQ_INT,  0,
                    &xd->xd_mac_dst_inc,NULL);
    parse_table_add(&pt, "LinkDownTX",        PQ_DFL|PQ_INT,  0,
                    &xd->xd_link_down_tx,       NULL);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(u)) {
        parse_table_add(&pt, "PurGe", PQ_DFL|PQ_BOOL, 0,
                        &xd->xd_purge, NULL);
        parse_table_add(&pt, "McQType",       PQ_DFL|PQ_BOOL,   0,
                        &xd->xd_mc_qtype, NULL);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    parse_table_add(&pt, "COSqueue",        PQ_DFL|PQ_INT,  0,
                    &xd->xd_hg_cos, NULL);
    parse_table_add(&pt, "CRC",         PQ_DFL|PQ_MULTI,0,
                    &xd->xd_crc,            crc_list);
    parse_table_add(&pt, "RQEnum",       PQ_DFL|PQ_INT,  0,
                    &xd->xd_rqe_q_num, NULL);
#if defined(BCM_TRIUMPH3_SUPPORT)
    parse_table_add(&pt, "RplType",        PQ_DFL|PQ_INT,  0,
                    &xd->xd_rpl_type, NULL);
    parse_table_add(&pt, "RplOffset",      PQ_DFL|PQ_INT,  0,
                    &xd->xd_rpl_offset, NULL);
    parse_table_add(&pt, "LmCtrIdx",       PQ_DFL|PQ_INT,  0,
                    &xd->xd_lm_ctr_index, NULL);
#endif /* BCM_TRIUMPH3_SUPPORT */
    parse_table_add(&pt, "Spid",     PQ_DFL|PQ_INT,  0,
                    &xd->xd_spid,       NULL);
    parse_table_add(&pt, "SpidOverride",     PQ_DFL|PQ_INT,  0,
                    &xd->xd_spid_override,       NULL);

    if (SOC_IS_XGS_FABRIC(u)) {
        SOC_PBMP_PORT_REMOVE(xd->pkt_info.tx_pbmp, CMIC_PORT(u));
    }

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(a, &pt)) {
        cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }
    {
        char *xfile = NULL, *xdata = NULL;
        if (xd->xd_file) {
            xfile = sal_strdup(xd->xd_file);
        }
        if (xd->xd_data) {
            xdata = sal_strdup(xd->xd_data);
        }
        parse_arg_eq_done(&pt);
        xd->xd_file = xfile;
        xd->xd_data = xdata;
    }

    if (SOC_IS_XGS12_FABRIC(u)) {
        tagged = TRUE;
        min_len = ((xd->hdr_mode == ENCAP_HIGIG) ? 60 : 64);
        if (xd->hdr_mode == ENCAP_HIGIG) {
            BCM_PKT_HGHDR_REQUIRE(&xd->pkt_info);
        }
    } else {
        tagged = (xd->xd_vlan != 0);
        min_len = (tagged ? 68 : 64);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(u)) {
        if (SOC_PBMP_IS_NULL(xd->pkt_info.tx_pbmp)) { 
            xd->pkt_info.flags |= (BCM_TX_ETHER); /* Mapped mode */
            if (xd->ether_pkt) {
                xd->pkt_info.flags &= (~BCM_TX_HG_READY);
            }
        } else {
            xd->pkt_info.flags &= (~BCM_TX_ETHER); /* Raw mode */
        }

        if (xd->hdr_mode == ENCAP_RAW) {
            if (xd->w0 != 0) {
                xd->pkt_info.flags |= (BCM_TX_HG_READY);
            } else {
                xd->pkt_info.flags &= (~BCM_TX_HG_READY);
            }
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    if (!XD_FILE(xd) && !XD_STRING(xd) && !tagged) {
        cli_out("%s: Warning: Sending untagged packets from CPU "
                "not recommended\n",
                ARG_CMD(a));
    }

    if (!XD_FILE(xd) && !XD_STRING(xd) && xd->xd_pkt_len < min_len) {
        cli_out("%s: Warning: Length %d too small for %s packet (min %d)\n",
                ARG_CMD(a),
                xd->xd_pkt_len,
                tagged ? "tagged" : "untagged",
                min_len);
    }

    if (xd->xd_hg_cos > NUM_COS(xd->xd_unit)-1 && 
        xd->xd_hg_cos != 14 && xd->xd_hg_cos != 15 && 
        !SOC_IS_TR_VL(xd->xd_unit)) {
        cli_out("%s: Error: Invalid COS queue %d, must be < %d\n", ARG_CMD(a),
                xd->xd_hg_cos, NUM_COS(xd->xd_unit));
        return(CMD_FAIL);
    }

    if (xd->xd_untag) {
        xd->xd_vlan = 0;
    }

    return CMD_OK;
}

/*
 * Function:    _tx_first_pbm(pbmp_t pbm)
 * Purpose:     Get the first port in a bitmap.
 * Parameters:  pbm -- the port bitmap
 *              rpbm -- (OUT) port bitmap with only one port set.
 */
STATIC void
_tx_first_pbm(int unit, bcm_pbmp_t pbm, bcm_pbmp_t *rpbm)
{
    soc_port_t p, dport;

    COMPILER_REFERENCE(unit);

    BCM_PBMP_CLEAR(*rpbm);
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        LOG_INFO(BSL_LS_APPL_TX,
                 (BSL_META_U(unit,
                             "First to port %d\n"), p));
        /* coverity[overrun-local] */
        BCM_PBMP_PORT_ADD(*rpbm, p);
        return;
    }

    LOG_INFO(BSL_LS_APPL_TX,
             (BSL_META_U(unit,
                         "Warning: first pbm null\n")));
}

/*
 * Function:    int _tx_next_port(pbmp_t *newpbm, pbmp_t allpbm)
 * Purpose:     Get the first port in a bitmap.
 * Parameters:  *newpbm -- the current port bitmap (one port)
 *                         set to next port
 *              allpbm  -- all ports being iterated over.
 * Returns:     0 if okay.  1 if reset to first port in allpbm
 */
STATIC int
_tx_next_port(int unit, bcm_pbmp_t *newpbm, bcm_pbmp_t allpbm)
{
    soc_port_t p, dport;
    soc_port_t np, ndport;
    int found = FALSE;
    char pfmt[SOC_PBMP_FMT_LEN];

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, *newpbm, ndport, np) {
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, allpbm, dport, p) {
            if (found) {
                LOG_INFO(BSL_LS_APPL_TX,
                         (BSL_META_U(unit,
                                     "Next to port %d\n"), p));
                BCM_PBMP_CLEAR(*newpbm);
                BCM_PBMP_PORT_ADD(*newpbm, p);
                return 0;
            }
            if (np == p) found = TRUE;
        }
    }

    /* If we get here, must be resetting. */
    _tx_first_pbm(unit, allpbm, newpbm);
    LOG_INFO(BSL_LS_APPL_TX,
             (BSL_META_U(unit,
                         "Resetting to pbm %s\n"), SOC_PBMP_FMT(*newpbm, pfmt)));
    return 1;
}

#define TX_LOAD_MAX             4096

#ifndef NO_FILEIO

/* Discard the rest of the line */
STATIC int
_discard_line(FILE *fp)
{
    int c;
    do {
        if ((c = sal_fgetc(fp)) == EOF) {
	    return c;
	}
    } while (c != '\n');
    return c;
}

STATIC int
tx_load_byte(FILE *fp, uint8 *byte)
{
    int         c, d;

    do {
        if ((c = sal_fgetc(fp)) == EOF) {
            return -1;
	}
	else if (c == '#') {
	    if((c = _discard_line(fp)) == EOF) {
	        return -1;
	    }
	}
    } while (!isxdigit(c));

    do {
        if ((d = sal_fgetc(fp)) == EOF) {
            return -1;
	}
	else if (d == '#') {
	    if((d = _discard_line(fp)) == EOF) {
	        return -1;
	    }
	}
    } while (!isxdigit(d));

    *byte = (xdigit2i(c) << 4) | xdigit2i(d);

    return 0;
}

STATIC uint8 *
tx_load_packet(int unit, char *fname, int *length)
{
    uint8               *p;
    FILE                *fp;
    int                 i;

    if ((p = soc_cm_salloc(unit, TX_LOAD_MAX, "tx_packet")) == NULL)
        return p;

    if ((fp = sal_fopen(fname, "r")) == NULL) {
        soc_cm_sfree(unit, p);
        return NULL;
    }

    for (i = 0; i < TX_LOAD_MAX; i++) {
        if (tx_load_byte(fp, &p[i]) < 0)
            break;
    }

    *length = i;

    sal_fclose(fp);

    return p;
}
#endif /* NO_FILEIO */

/* If data was read from file, extract to XD structure */
STATIC void
check_pkt_fields(xd_t *xd)
{
    enet_hdr_t        *ep;              /* Ethernet packet header */
    bcm_pkt_t      *pkt_info = &xd->pkt_info;

    ep = (enet_hdr_t *)BCM_PKT_IEEE(pkt_info);

    if (XD_FILE(xd) || XD_STRING(xd)) { /* Loaded from file or string */
        /* Also set parameters to file data so incrementing works */

        ENET_COPY_MACADDR(&ep->en_dhost, xd->xd_mac_dst);
        ENET_COPY_MACADDR(&ep->en_shost, xd->xd_mac_src);

        if (!ENET_TAGGED(ep)) {
            cli_out("Warning:  Untagged packet read from file for tx.\n");
            xd->xd_vlan = VLAN_ID_NONE;
        } else {
            xd->xd_vlan = VLAN_CTRL_ID(bcm_ntohs(ep->en_tag_ctrl));
            xd->xd_prio = VLAN_CTRL_PRIO(bcm_ntohs(ep->en_tag_ctrl));
        }
    } else {
        if (xd->xd_vlan) {                      /* Tagged format */
            ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(xd->xd_prio, 0, xd->xd_vlan));
            ep->en_tag_len	= bcm_htons(xd->xd_pkt_len - ENET_TAGGED_HDR_LEN - ENET_CHECKSUM_LEN);
            ep->en_tag_tpid = bcm_htons(0x8100);
        } else {                                /* Untagged format */
            cli_out("Warning:  Sending untagged packet.\n");
		ep->en_untagged_len = bcm_htons(xd->xd_pkt_len - ENET_UNTAGGED_HDR_LEN - ENET_CHECKSUM_LEN);
        }
        ENET_SET_MACADDR(ep->en_dhost, xd->xd_mac_dst);
        ENET_SET_MACADDR(ep->en_shost, xd->xd_mac_src);
    }
}

#ifdef  BCM_XGS_SUPPORT
STATIC int
_tx_hghdr_setup(bcm_pkt_t *pkt, int sop)
{
    soc_higig_hdr_t *xgh = (soc_higig_hdr_t *)pkt->_higig;
    int unit = pkt->unit;
    uint32 vlan_val;

    sal_memset(xgh, 0, sizeof(pkt->_higig));
    soc_higig_field_set(unit, xgh, HG_start, sop);
    soc_higig_field_set(unit, xgh, HG_hgi, SOC_HIGIG_HGI);
    soc_higig_field_set(unit, xgh, HG_opcode, pkt->opcode);
    soc_higig_field_set(unit, xgh, HG_hdr_format,
                        SOC_HIGIG_HDR_DEFAULT);

    vlan_val = BCM_PKT_VLAN_CONTROL(pkt);
    soc_higig_field_set(unit, xgh, HG_vlan_tag, vlan_val);
    soc_higig_field_set(unit, xgh, HG_dst_mod, pkt->dest_mod);
    soc_higig_field_set(unit, xgh, HG_dst_port, pkt->dest_port);
    soc_higig_field_set(unit, xgh, HG_src_mod, pkt->src_mod);
    soc_higig_field_set(unit, xgh, HG_src_port, pkt->src_port);
    soc_higig_field_set(unit, xgh, HG_pfm, pkt->pfm);
    soc_higig_field_set(unit, xgh, HG_cos, pkt->prio_int);

    return BCM_E_NONE;
}

#if defined(BCM_HIGIG2_SUPPORT)

STATIC int
_tx_hg2hdr_setup(bcm_pkt_t *pkt, int sop, 
    uint32      xd_hg_mgid,             /* Higig2 Multicast Group ID */
    uint32      xd_hg_lbid,             /* Higig2 Load Balancing ID */
    uint32      xd_hg_dp                /* Higig2 Drop Precendece */
)
{
    soc_higig2_hdr_t *xgh = (soc_higig2_hdr_t *)pkt->_higig;
    int unit = pkt->unit, multicast = 0, mc_index_offset = 0, mc_index = 0;
    uint32 vlan_val;
#if defined(BCM_BRADLEY_SUPPORT)
    int bcast_size, mcast_size, ipmc_size;
#endif

    sal_memset(xgh, 0, sizeof(pkt->_higig));
    soc_higig2_field_set(unit, xgh, HG_start, sop);
    soc_higig2_field_set(unit, xgh, HG_hgi, SOC_HIGIG_HGI);
    soc_higig2_field_set(unit, xgh, HG_opcode, pkt->opcode);
    soc_higig2_field_set(unit, xgh, HG_hdr_format,
                        SOC_HIGIG_HDR_DEFAULT);

    vlan_val = BCM_PKT_VLAN_CONTROL(pkt);
    soc_higig2_field_set(unit, xgh, HG_vlan_tag, vlan_val);
    soc_higig2_field_set(unit, xgh, HG_dst_mod, pkt->dest_mod);
    soc_higig2_field_set(unit, xgh, HG_dst_port, pkt->dest_port);
    soc_higig2_field_set(unit, xgh, HG_src_mod, pkt->src_mod);
    soc_higig2_field_set(unit, xgh, HG_src_port, pkt->src_port);
    soc_higig2_field_set(unit, xgh, HG_pfm, pkt->pfm);
    soc_higig2_field_set(unit, xgh, HG_cos, pkt->prio_int);

    multicast = ((pkt->opcode == SOC_HIGIG_OP_CPU) ||
                 (pkt->opcode == SOC_HIGIG_OP_UC)) ? 0 : pkt->opcode;
    if (multicast) {
        soc_higig2_field_set(unit, xgh, HG_mcst, 1);


        mc_index_offset = 0;

#if defined(BCM_BRADLEY_SUPPORT)
        /* Higig2 has different ranges for different multicast types */
        BCM_IF_ERROR_RETURN
            (soc_hbx_higig2_mcast_sizes_get(unit, &bcast_size, &mcast_size,
                                            &ipmc_size));
        switch (multicast) {
        case SOC_HIGIG_OP_IPMC:
            mc_index_offset += mcast_size;
            /* Fall thru */
        case SOC_HIGIG_OP_MC:
            mc_index_offset += bcast_size;
            /* Fall thru */
        case SOC_HIGIG_OP_BC:
            break;
        default:
            /* Unknown opcode */
            return BCM_E_PARAM;
        }
#endif
        mc_index = _BCM_MULTICAST_ID_GET(xd_hg_mgid);
        soc_higig2_field_set(unit, xgh, HG_mgid,
                             mc_index + mc_index_offset);
 
    }

    soc_higig2_field_set(unit, xgh, HG_lbid, xd_hg_lbid);
    soc_higig2_field_set(unit, xgh, HG_dp,   xd_hg_dp);

    return BCM_E_NONE;
}
#endif /* BCM_HIGIG2_SUPPORT */

STATIC void
add_higig_hdr(xd_t *xd, bcm_pkt_t *pkt_info)
{
    soc_bcm5632_hdr_t *b5632h;          /* BCM5632 header */
    uint32 temp_raw;
    uint8 *ptr;
    uint8 *ext_ptr;

    switch(xd->hdr_mode) {
    case ENCAP_HIGIG:       /* HiGig header */
        _tx_hghdr_setup(pkt_info, SOC_HIGIG_START);
        break;

#ifdef BCM_HIGIG2_SUPPORT
    case ENCAP_HIGIG2:      /* HiGig2 header */
        if (xd->hdr_mode == ENCAP_HIGIG2 &&
            soc_feature(xd->xd_unit, soc_feature_higig2)) {
            _tx_hg2hdr_setup(pkt_info, SOC_HIGIG2_START,
                xd->xd_hg_mgid,
                xd->xd_hg_lbid,
                xd->xd_hg_dp);
        } else {
            cli_out("Warning:  Higig2 not supported by device\n");
        }
        break;
#endif /* BCM_HIGIG2_SUPPORT */

    case ENCAP_B5632:      /* 5632 header */
        b5632h = (soc_bcm5632_hdr_t*)BCM_PKT_HG_HDR(pkt_info);

        sal_memset(b5632h, 0, sizeof (soc_bcm5632_hdr_t));
        b5632h->overlay1.s_portid = xd->xd_s_portid;
        b5632h->overlay1.d_portid = xd->xd_d_portid;
        b5632h->overlay1.start = xd->xd_start;
        b5632h->overlay1.length_hi = xd->xd_length >> 8;
        b5632h->overlay1.length_lo = xd->xd_length & 0xff;
        break;

    case ENCAP_RAW:        /* Raw HIGIG */
        ptr = (uint8 *)BCM_PKT_HG_HDR(pkt_info);
        ext_ptr = (uint8 *)BCM_PKT_EXT_HG_HDR(pkt_info);

        /* Convert to network byte order */
        temp_raw = soc_htonl(xd->w0);
        sal_memcpy(ptr,   (void*)&temp_raw, sizeof(uint32));
        temp_raw = soc_htonl(xd->w1);
        sal_memcpy(ptr+4, (void*)&temp_raw, sizeof(uint32));
        temp_raw = soc_htonl(xd->w2);
        sal_memcpy(ptr+8, (void*)&temp_raw, sizeof(uint32));
#ifdef BCM_HIGIG2_SUPPORT
        if (soc_feature(xd->xd_unit, soc_feature_higig2)) {
            temp_raw = soc_htonl(xd->w3);
            sal_memcpy(ptr+12, (void*)&temp_raw, sizeof(uint32));
            temp_raw = soc_htonl(xd->w4);
            sal_memcpy(ext_ptr, (void*)&temp_raw, sizeof(uint32));
        }
#endif /* BCM_HIGIG2_SUPPORT */
        break;

    default:          /* Error */
        cli_out("Warning:  Unknown HG header format: %d\n", xd->hdr_mode);
        break;
    }
}

/* Set up the XGS information needed for the DCB */
STATIC void
add_xgs_info(xd_t *xd, bcm_pkt_t *pkt)
{
    pkt->dest_mod = xd->xd_hg_dmod;
    pkt->dest_port = xd->xd_hg_dport;
    pkt->src_mod = xd->xd_hg_smod;
    pkt->src_port = xd->xd_hg_sport;
    pkt->flags &= ~(BCM_TX_SRC_MOD | BCM_TX_SRC_PORT);
    if (xd->xd_link_down_tx) {
        pkt->flags |= BCM_TX_LINKDOWN_TRANSMIT;
    } else {
        pkt->flags &= ~BCM_TX_LINKDOWN_TRANSMIT;
    }
    if (!xd->ether_pkt) {
        pkt->flags |= BCM_TX_SRC_PORT;
        pkt->flags |= BCM_TX_SRC_MOD;
    }
    pkt->opcode = xd->xd_hg_opcode;
    pkt->cos = xd->xd_hg_cos;
    pkt->rqe_q_num = xd->xd_rqe_q_num;
    pkt->spid_override = xd->xd_spid_override;
    pkt->spid = xd->xd_spid;
    pkt->flags &= ~BCM_TX_PRIO_INT;
    if (xd->xd_prio_int != XD_PRIO_INT_INVALID) {
        pkt->prio_int = xd->xd_prio_int;
        pkt->flags |= BCM_TX_PRIO_INT;
    } else {
        /* For now, internal priority matches COS (or v.v.) */
        pkt->prio_int = xd->xd_hg_cos;
    }
    pkt->pfm = xd->xd_hg_pfm;
}

/*
 * Function:
 *      setup_higig_tx
 * Purpose:
 *      Setup the hercules packet for tx
 * Parameters:
 *      xd         - transmit descriptor
 *      pkt_data   - Raw data to transmit
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC void
setup_higig_tx(xd_t *xd)
{
    bcm_pkt_t      *pkt_info = &xd->pkt_info;
    
    pkt_info->unit = xd->xd_unit;
    /* Use raw higig header from pkt_info structure */
    if (BCM_PKT_TX_HG_READY(pkt_info)) {
        add_xgs_info(xd, pkt_info);
        add_higig_hdr(xd, pkt_info);
    } else if (SOC_IS_XGS_SWITCH(xd->xd_unit)) { /* Set HG info members */
        add_xgs_info(xd, pkt_info);
        if (SOC_IS_XGS3_SWITCH(xd->xd_unit)) {
            /* Ethernet mode TX. Remove TX properties */
            pkt_info->flags &= (~(BCM_TX_PKT_PROP_ANY));

        }
    } else {
        /* Always get cos from higig info structure */
        pkt_info->cos = xd->xd_hg_cos;
    }
}

#ifdef BCM_CPU_TX_PROC_SUPPORT
/*
 * Function:
 *      setup_cpu_tx_proc
 * Purpose:
 *      Setup packet for cpu tx proc
 * Parameters:
 *      xd         - transmit descriptor
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC void
setup_cpu_tx_proc(xd_t *xd)
{
    bcm_pkt_t      *pkt_info = &xd->pkt_info;

    pkt_info->unit = xd->xd_unit;

    if (xd->xd_ph_destination_type <= SOC_TX_PROC_OP_VLAN_FLOOD) {
        xd->pkt_info.flags2 |= BCM_PKT_F2_CPU_TX_PROC;
        xd->pkt_info.flags &= (~BCM_TX_ETHER);
        pkt_info->txprocmh_destination_type = xd->xd_ph_destination_type;
        pkt_info->rx_port = xd->xd_ph_source_port;
    }

    if (xd->xd_ph_qos_fields_valid) {
        pkt_info->txprocmh_qos_fields_valid = xd->xd_ph_qos_fields_valid;
        pkt_info->color = xd->xd_ph_dp;
        pkt_info->flags &= ~BCM_TX_PRIO_INT;
        if (xd->xd_prio_int != XD_PRIO_INT_INVALID) {
            pkt_info->prio_int = xd->xd_prio_int;
            pkt_info->flags |= BCM_TX_PRIO_INT;
        }

        pkt_info->flags2 &= ~BCM_PKT_F2_CONG_INT;
        if (xd->xd_cong_int != XD_CONG_INT_INVALID) {
            pkt_info->txprocmh_congestion_int = xd->xd_cong_int;
            pkt_info->flags2 |= BCM_PKT_F2_CONG_INT;
        }
    }

    if (xd->xd_ph_destination_type == SOC_TX_PROC_OP_DGPP) {
        pkt_info->txprocmh_destination = xd->xd_ph_destination_port;
    } else {
        pkt_info->txprocmh_destination = xd->xd_ph_destination;
    }

    if ((xd->xd_ph_destination_type == SOC_TX_PROC_OP_ECMP_MEMBER) ||
            (xd->xd_ph_destination_type == SOC_TX_PROC_OP_ECMP)) {
        pkt_info->txprocmh_ecmp_group_index = xd->xd_ph_ecmp_group_index;
    }

    if (xd->xd_ph_destination_type == SOC_TX_PROC_OP_ECMP_MEMBER) {
        pkt_info->txprocmh_ecmp_member_index = xd->xd_ph_ecmp_member_index;
    }

    if (xd->xd_ph_routed_pkt) {
        pkt_info->flags |= BCM_PKT_F_ROUTED;
    }

    if (xd->xd_ph_mcast_lb_index_valid) {
        pkt_info->txprocmh_mcast_lb_index_valid = xd->xd_ph_mcast_lb_index_valid;
        pkt_info->txprocmh_mcast_lb_index       = xd->xd_ph_mcast_lb_index;
    }
}
#endif /* BCM_CPU_TX_PROC_SUPPORT */

#endif  /* BCM_XGS_SUPPORT */

/* parsing string given by user as packet payload */
STATIC uint8 *
parse_data_packet_payload(int unit, char *packet_data, int *length)
{
    uint8               *p;
    char                 tmp, data_iter;
    int                  data_len, i, j, pkt_len, data_base;

    /* If string data starts with 0x or 0X, skip it */
    if ((packet_data[0] == '0')
        && (packet_data[1] == 'x' || packet_data[1] == 'X')) {
        data_base = 2;
    } else {
        data_base = 0;
    }

    data_len = strlen(packet_data) - data_base;
    pkt_len = (data_len + 1) / 2;
    if (pkt_len < UNTAGGED_PACKET_LENGTH) {
        pkt_len = UNTAGGED_PACKET_LENGTH;
    }

    p = soc_cm_salloc(unit, pkt_len, "tx_string_packet");
    if (p == NULL) {
        return p;
    }
    sal_memset(p, 0, pkt_len);
    /* Convert char to value */
    i = j = 0;
    while (j < data_len) {
        data_iter = packet_data[data_base + j];
        if (('0' <= data_iter) && (data_iter <= '9')) {
            tmp = data_iter - '0';
        } else if (('a' <= data_iter) && (data_iter <= 'f')) {
            tmp = data_iter - 'a' + 10;
        } else if (('A' <= data_iter) && (data_iter <= 'F')) {
            tmp = data_iter - 'A' + 10;
        } else if (data_iter == ' ') {
            ++j;
            continue;
        } else {
            soc_cm_sfree(unit, p);
            return NULL;
        }

        /* String input is in 4b unit. Below we're filling in 8b:
           offset is /2, and we shift by 4b if the input char is odd */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
        ++j;
    }

    *length = i / 2 + 4; /* pad for crc */
    return p;
}

STATIC cmd_result_t
do_tx(xd_t *xd)
/*
 * Function:    do_tx
 * Purpose:     Perform actual work for a TX operation.
 * Parameters:  xd - pointer to XD structure already filled in.
 * Returns:     CMD_xxx
 */
{
    uint8             *pkt_data;        /* Packet */
    uint8             *payload;         /* data - packet payload */
    enet_hdr_t        *ep;              /* Ethernet packet header */
    bcm_pkt_t         *pkt_info = &xd->pkt_info;
    int                rv = BCM_E_INTERNAL;
    int                payload_len;

    pkt_data = NULL;

    /* Allocate the packet; use tx_load if reading from file */
    if (XD_FILE(xd)) {
#ifdef NO_FILEIO
        cli_out("no filesystem\n");
        return CMD_FAIL;
#else
        if ((pkt_data = tx_load_packet(xd->xd_unit,
                                xd->xd_file, &xd->xd_pkt_len)) == NULL) {
            cli_out("Unable to load packet from file %s\n", xd->xd_file);
            return CMD_FAIL;
        }
        cli_out("Packet from file: %s, length=%d\n", xd->xd_file, xd->xd_pkt_len);
#endif
    } else if (XD_STRING(xd)) {
        /* use parse_data_packet_payload if reading from user string input */
        pkt_data = parse_data_packet_payload(xd->xd_unit, 
                                             xd->xd_data, &xd->xd_pkt_len);
        if (pkt_data == NULL) {
            cli_out("Unable to allocate memory or Invalid inputs %s\n", xd->xd_data);
            return CMD_FAIL;
        }
        cli_out("Packet from data=<>, length=%d\n", xd->xd_pkt_len);
    }

    /* Make sure packet allocation size is right size */
    if (pkt_info->pkt_data[0].len != xd->xd_pkt_len) {
        soc_cm_sfree(xd->xd_unit, pkt_info->pkt_data[0].data);
        if ((pkt_info->pkt_data[0].data = (uint8 *)
             soc_cm_salloc(xd->xd_unit, xd->xd_pkt_len, "TX")) == NULL) {
            if (pkt_data) { /* free packet buffer */
                soc_cm_sfree(xd->xd_unit, pkt_data);
            }
            cli_out("Unable to allocate packet memory\n");
            return(CMD_FAIL);
        }
        pkt_info->pkt_data[0].len = xd->xd_pkt_len;
    }

    sal_memset(pkt_info->pkt_data[0].data, 0, pkt_info->pkt_data[0].len);
    if (pkt_data) { /* Data was read from file.  Copy into pkt_info */
        sal_memcpy(pkt_info->pkt_data[0].data, pkt_data, xd->xd_pkt_len);
        soc_cm_sfree(xd->xd_unit, pkt_data);
    }

    /* Setup the packet */
    pkt_info->flags &= ~BCM_TX_CRC_FLD;
    pkt_info->flags |= (xd->xd_crc==1 ? BCM_TX_CRC_REGEN : 0) |
        (xd->xd_crc==2 ? BCM_TX_CRC_APPEND: 0);

    pkt_info->flags &= ~BCM_TX_NO_PAD;
    if (xd->xd_pkt_len < (xd->xd_vlan != 0 ? 68 : 64)) {
        pkt_info->flags |= BCM_TX_NO_PAD;
    }
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    pkt_info->flags &= (~(BCM_TX_PURGE));
    if (xd->xd_purge) {
        pkt_info->flags |= BCM_TX_PURGE;
    }

    pkt_info->flags2 &= ~BCM_PKT_F2_MC_QUEUE;
    if (xd->xd_mc_qtype) {
        pkt_info->flags2 |= BCM_PKT_F2_MC_QUEUE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    check_pkt_fields(xd);

#ifdef  BCM_XGS_SUPPORT
    /* Setup special headers and DMA data */
    setup_higig_tx(xd);
#endif  /* BCM_XGS_SUPPORT */

#ifdef BCM_CPU_TX_PROC_SUPPORT
    if (soc_feature(xd->xd_unit, soc_feature_cpu_tx_proc)) {
        setup_cpu_tx_proc(xd);
    }
#endif  /* BCM_CPU_TX_PROC_SUPPORT */

    ep = (enet_hdr_t *)(pkt_info->pkt_data[0].data);

    payload = &pkt_info->pkt_data[0].data[sizeof(enet_hdr_t)];
    payload_len = pkt_info->pkt_data[0].len - sizeof(enet_hdr_t) -
        sizeof(uint32) /* CRC */;

    if (xd->xd_ppsm) { /* save base info. setup first port. */
        LOG_INFO(BSL_LS_APPL_TX,
                 (BSL_META_U(xd->xd_unit,
                             "Per port source is active\n")));
        xd->xd_ppsm_pbm = pkt_info->tx_pbmp;
        _tx_first_pbm(xd->xd_unit, pkt_info->tx_pbmp, &pkt_info->tx_pbmp);
        ENET_COPY_MACADDR(xd->xd_mac_src, xd->xd_mac_src_base);
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if(xd->xd_rpl_type) {
        pkt_info->flags2 |= BCM_PKT_F2_REPLACEMENT_TYPE;
        pkt_info->flags2 |= BCM_PKT_F2_REPLACEMENT_OFFSET;
        pkt_info->oam_replacement_type = xd->xd_rpl_type;
        pkt_info->oam_replacement_offset = xd->xd_rpl_offset;
        if(xd->xd_lm_ctr_index) {
            pkt_info->flags2 |= BCM_PKT_F2_LM_COUNTER_INDEX;
            pkt_info->oam_lm_counter_index = xd->xd_lm_ctr_index;
        }
    }
#endif
    /* XMIT all the required packets */
    for (xd->xd_cur_cnt = 0;
         xd->xd_cur_cnt < xd->xd_tot_cnt;
         xd->xd_cur_cnt++) {

        if (xd->xd_state != XD_RUNNING) {
            break;                      /* Abort */
        }

        /*
         * Generate pattern on first time through, or every time if
         * pattern is incrementing.
         */

        if (xd->xd_mac_dst_inc != 0) {
            ENET_SET_MACADDR(ep->en_dhost, xd->xd_mac_dst);
        }
        if (xd->xd_mac_src_inc != 0) {
            ENET_SET_MACADDR(ep->en_shost, xd->xd_mac_src);
        }

        /* Store pattern */
        if (!XD_FILE(xd) && !XD_STRING(xd) && xd->xd_pat_random) {
            packet_random_store(payload, payload_len);
        } else if (!XD_FILE(xd) && !XD_STRING(xd) &&
                    (xd->xd_cur_cnt == 0 || xd->xd_pat_inc != 0)) {
            xd->xd_pat =
                packet_store(payload, payload_len,
                             xd->xd_pat, xd->xd_pat_inc);
        }

#ifdef BCM_INSTRUMENTATION_SUPPORT
        if (xd->xd_vis_sport) {
            rv = appl_visibility_trace(xd->xd_unit,
                                       xd->xd_vis_options,
                                       xd->xd_vis_sport,
                                       pkt_info);
        } else
#endif /* BCM_INSTRUMENTATION_SUPPORT */
        {
            rv = bcm_tx(xd->xd_tx_unit, pkt_info, NULL);
        }
        if (rv != BCM_E_NONE) {
            cli_out("bcm_tx failed2: TX Unit %d: %s\n",
                    xd->xd_tx_unit, bcm_errmsg(rv));
            break;
        }

        increment_macaddr(xd->xd_mac_dst, xd->xd_mac_dst_inc);
        increment_macaddr(xd->xd_mac_src, xd->xd_mac_src_inc);

        if (xd->xd_ppsm) { /* per port source mac */
            /* change xd_pbm to next port. reset src mac if at base port */
            if (_tx_next_port(xd->xd_unit, &pkt_info->tx_pbmp,
                              xd->xd_ppsm_pbm)) {
                LOG_INFO(BSL_LS_APPL_TX,
                         (BSL_META_U(xd->xd_tx_unit,
                                     "resetting mac\n")));
                ENET_COPY_MACADDR(xd->xd_mac_src_base, xd->xd_mac_src);
            }
        }
    }

    if (xd->xd_ppsm) { /* Replace original params */
        ENET_COPY_MACADDR(xd->xd_mac_src_base, xd->xd_mac_src);
        pkt_info->tx_pbmp = xd->xd_ppsm_pbm;
    }

    return(rv == BCM_E_NONE ? CMD_OK : CMD_FAIL);
}

STATIC void
do_tx_start(void *xdv)
/*
 * Function:    do_tx_start
 * Purpose:     Wrapper function for a tx_start thread.
 * Parameters:  xdv - pointer to xd structure for the unit under test.
 * Returns:     Nothing, does not return.
 */
{
    cmd_result_t rv;
    xd_t        *xd = (xd_t *)xdv;

    rv = do_tx(xd);

    cli_out("TX Completed %s: TX Requested(%d) Tx Sent(%d)\n",
            rv == CMD_OK ?"successfully" : "with error",
            xd->xd_tot_cnt, xd->xd_cur_cnt);
    xd->xd_state = XD_IDLE;
    sal_thread_exit(rv == CMD_OK ? 0 : 1);
}

char cmd_rx_usage[] =
    "Parameters: <count>\n\t"
    "Wait for and received <count> packets from the CMIC. This command\n\t"
    "is mostly obsolete, and PacketWatcher should be used.\n";

cmd_result_t
cmd_rx(int u, args_t *a)
/*
 * Function:    rx
 * Purpose:     Perform simple RX
 * Parameters:  u - unit number
 *              a - arguments (expect "count");
 * Returns:     CMD_XX
 */
{
    cmd_result_t rv = CMD_OK;
    dv_t        *dv;
    void        *p;                     /* Packet */
    int         cnt, i;
    pbmp_t      empty_pbm;

    if ((1 != ARG_CNT(a)) || !(isint(ARG_CUR(a)))) {
        return(CMD_USAGE);
    } else if (!sh_check_attached(ARG_CMD(a), u)) {
        return(CMD_FAIL);
    } else if (pw_running(u)) {
        cli_out("%s: Error: Packetwatcher running\n", ARG_CMD(a));
        return(CMD_FAIL);
    }

    cnt = parse_integer(ARG_GET(a));

    if (!(dv = soc_dma_dv_alloc(u, DV_RX, 1))) {
        cli_out("%s: failed to allocate DV\n", ARG_CMD(a));
        return(CMD_FAIL);
    }
    if (!(p = soc_cm_salloc(u, 10240, "RX"))) {
        cli_out("%s: failed to allocate packet memory\n", ARG_CMD(a));
        soc_dma_dv_free(u, dv);
        return(CMD_FAIL);
    }

    SOC_PBMP_CLEAR(empty_pbm);
    for (i = 0; i < cnt; i++) {
        int     result;

        soc_dma_dv_reset(DV_RX, dv);
        soc_dma_desc_add(dv, (sal_vaddr_t)p, 10240,
                         empty_pbm, empty_pbm, empty_pbm, 0, NULL);
        soc_dma_desc_end_packet(dv);

        if (0 > (result = soc_dma_wait(u, dv))) {
            cli_out("%s: Dma Failed: %s\n", ARG_CMD(a), soc_errmsg(result));
            rv = CMD_FAIL;
            break;
        }
    }

    soc_dma_dv_free(u, dv);
    soc_cm_sfree(u, p);
    return(rv);
}

cmd_result_t
cmd_esw_tx_count(int u, args_t *a)
/*
 * Function:    tx_count
 * Purpose:     Print out status of any currently running TX command.
 * Parameters:  u - unit number.
 *              a - arguments, none expected.
 * Returns:     CMD_OK
 */
{
    xd_t        *xd;

    if (ARG_CNT(a)) {
        return(CMD_USAGE);
    }

    _XD_INIT(u, xd);
    cli_out("TX (%sRunning) Transmit Req(%d) Sent(%d)\n",
            xd->xd_state == XD_RUNNING ? "" : "Not-",
            xd->xd_tot_cnt, xd->xd_cur_cnt);
    return(CMD_OK);
}

cmd_result_t
cmd_esw_tx_start(int u, args_t *a)
/*
 * Function:    tx_start
 * Purpose:     Start off a background TX thread.
 * Parameters:  u - unit number
 *              a - arguments.
 * Returns:     CMD_XXX
 */
{
    cmd_result_t rv;
    xd_t        *xd;

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return(CMD_FAIL);
    }

    _XD_INIT(u, xd);
    if (xd->xd_state == XD_RUNNING) {
        cli_out("%s: Error: tx command running\n", ARG_CMD(a));
        return(CMD_FAIL);
    }

#ifdef BROADCOM_DEBUG
    if (ARG_CUR(a) && !sal_strcasecmp(_ARG_CUR(a), "show")) {
        bcm_tx_show(u);
        return CMD_OK;
    }
#endif /* BROADCOM_DEBUG */

    if (CMD_OK != (rv = tx_parse(u, a, xd))) {
        return(rv);
    }

    /* BLOCK_CNTL_C */
    xd->xd_state = XD_RUNNING;          /* Say GO */

    if (SAL_THREAD_ERROR == sal_thread_create("TX", SAL_THREAD_STKSZ, 100,
                                              do_tx_start, (void *)xd)) {
        cli_out("%s: Error: Failed to create background thread\n",ARG_CMD(a));
        xd->xd_state = XD_IDLE;
        rv = CMD_FAIL;
    }
    /* UN-BLOCK_CNTL_C */
    return(rv);
}

cmd_result_t
cmd_esw_tx_stop(int u, args_t *a)
/*
 * Function:    tx_stop
 * Purpose:     Stop a currently running TX command
 * Parameters:  u - unit number.
 *              a - arguments (none expected).
 * Returns:     CMD_OK/CMD_USAGE/CMD_FAIL
 */
{
    xd_t        *xd;

    if (ARG_CNT(a)) {
        return(CMD_USAGE);
    } else if (!sh_check_attached(ARG_CMD(a), u)) {
        return(CMD_FAIL);
    }
    _XD_INIT(u, xd);
    if (XD_RUNNING != xd->xd_state) {
        cli_out("%s: TX not currently running\n", ARG_CMD(a));
        return(CMD_FAIL);
    } else {
        xd->xd_state = XD_STOP;
        cli_out("%s: TX termination requested\n", ARG_CMD(a));
        return(CMD_OK);
    }
}

cmd_result_t
cmd_esw_tx(int u, args_t *a)
/*
 * Function:
 * Purpose:
 * Parameters:
 * Returns:
 */
{
    xd_t *xd;
    volatile cmd_result_t rv;
    jmp_buf ctrl_c;

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return(CMD_FAIL);
    }
    _XD_INIT(u, xd);
    if (xd->xd_state == XD_RUNNING) {
        cli_out("%s: Error: already active\n", ARG_CMD(a));
        return(CMD_FAIL);
    }

    if (CMD_OK != (rv = tx_parse(u, a, xd))) {
        return(rv);
    }

#ifndef NO_CTRL_C
    if (!setjmp(ctrl_c)) {
#endif
        sh_push_ctrl_c(&ctrl_c);
        xd->xd_state = XD_RUNNING;      /* Say GO */
        rv = do_tx(xd);
#ifndef NO_CTRL_C
    } else {
        rv = CMD_INTR;
    }
#endif

    sh_pop_ctrl_c();
    xd->xd_state = XD_IDLE;

    return(rv);
}

/****************************************************************
 *
 * RX commands
 */


cmd_result_t
cmd_esw_rx_init(int unit, args_t *args)
{
    char *ch;
    int override_unit;
    int rv;

    if ((ch = ARG_GET(args)) == NULL) {
        cli_out("RXINIT requires unit specification\n");
        return CMD_USAGE;
    }

    override_unit = strtoul(ch, NULL, 0);
    rv = bcm_rx_init(override_unit);

    if (rv < 0) {
        cli_out("ERROR:  bcm_rx_init(%d) returns %d: %s\n",
                override_unit, rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}


char cmd_hdr_mode_usage[] =
    "Parameters: [mode]\n"
    "  With no mode, shows packet header mode of current unit.\n"
    "  Although the header mode may be set for any device, \n"
    "  it only has an effect for BCM5670 and related devices\n"
    "  Supported modes are:\n"
    "      Ieee   --  no encapusulation\n"
    "      Higig  --  Use parsed HiGig header information\n"
#ifdef BCM_HIGIG2_SUPPORT
    "      Higig2 --  Use parsed HiGig2 header information\n"
#endif /* BCM_HIGIG2_SUPPORT */
    "      B5632  --  Use parsed BCM5632 header information\n"
    "      Raw    --  Use 3 words of raw data for the header\n";

cmd_result_t
cmd_hdr_mode(int unit, args_t *args)
/*
 * Function:
 * Purpose:
 * Parameters:
 * Returns:
 */
{
    xd_t *xd;
    char *arg;
    int i = -1;

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return(CMD_FAIL);
    }

    _XD_INIT(unit, xd);

    if ((arg = ARG_GET(args)) != NULL) {
        for (i = 0; i < ENCAP_COUNT; i++) {
            if (!sal_strcasecmp(arg, hdr_modes[i])) {
                xd->hdr_mode = i;
                break;
            }
        }
    }

    if (xd->hdr_mode < 0 || xd->hdr_mode >= ENCAP_COUNT) {
        cli_out("Current header mode for unit %d is invalid (%d).\n",
                unit, xd->hdr_mode);
    } else {
        cli_out("Current header mode for unit %d is%s %s (%d).\n",
                unit, (i != -1) ? " now" : "",
                hdr_modes[xd->hdr_mode], xd->hdr_mode);
    }

    return (i < ENCAP_COUNT) ? CMD_OK : CMD_USAGE;
}

/*
 * Very simple test command for the SOC DMA ROM interface
 */
char cmd_drt_usage[] =
    "Parameters: [init|detach|rx|tx]\n"
    "  init     - Calls soc_dma_rom_init() \n"
    "  detach   - Calls soc_dma_rom_detach() \n" 
    "  rx       - Calls soc_dma_rom_rx_poll() \n"
    "             Dumps packet if available \n"
    "  tx       - Simple packet transmit using soc_dma_rom_tx_start() \n"; 


cmd_result_t
cmd_drt(int unit, args_t *args)
{
    char* arg; 
    int rv; 

    if((arg = ARG_GET(args)) == NULL) {
        return CMD_USAGE; 
    }

    if(!sal_strcmp(arg, "init")) {
        if((rv = soc_dma_rom_init(unit, 1600, 1, 1)) < 0) {
            cli_out("soc_dma_rom_init() returned %d: %s\n",
                    rv, soc_errmsg(rv)); 
            return CMD_FAIL; 
        }
    }
    else if(!sal_strcmp(arg, "detach")) {
        if((rv = soc_dma_rom_detach(unit)) < 0) {
            cli_out("soc_dma_rom_detach() returnd %d: %s\n",
                    rv, soc_errmsg(rv)); 
            return CMD_FAIL;
        }
    }
    else if(!sal_strcmp(arg, "rx")) {
        dcb_t* dcb; 
        
        if((rv=soc_dma_rom_rx_poll(unit, &dcb)) < 0) {
            cli_out("soc_dma_rom_rx_poll() returned %d: %s\n",
                    rv, soc_errmsg(rv)); 
            return CMD_FAIL; 
        }
        
        if(dcb) {
            unsigned char* pkt; 
            int len; 

            pkt = (unsigned char *)SOC_DCB_ADDR_GET(unit, dcb); 
            len = SOC_DCB_XFERCOUNT_GET(unit, dcb); 
            
            soc_dma_dump_pkt(unit, "rx: ", pkt, len, TRUE); 

            soc_dma_rom_dcb_free(unit, dcb); 
        }
        else {
            cli_out("no packet\n");          
        }       
    }
    else if(!sal_strcmp(arg, "tx")) {
        unsigned char _pkt[100] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
                                    0x81, 0x00, 0x00, 0x01, 0x00, 0x00, 
        }; 
        dcb_t* dcb; 
        unsigned char* p; 
        int polls = 0; 
        int done; 
        
        dcb = soc_dma_rom_dcb_alloc(unit, sizeof(_pkt)); 
        
        p = (unsigned char*)SOC_DCB_ADDR_GET(unit, dcb); 
        sal_memcpy(p, _pkt, sizeof(_pkt)); 
        
        SOC_DCB_TX_L2PBM_SET(unit, dcb, PBMP_ALL(unit)); 
        SOC_DCB_TX_UTPBM_SET(unit, dcb, PBMP_E_ALL(unit)); 

#ifdef  BCM_XGS_SUPPORT
        if (SOC_IS_XGS_SWITCH(unit)) {
            SOC_DCB_TX_DESTPORT_SET(unit, dcb, 0);
            SOC_DCB_TX_DESTMOD_SET(unit, dcb, 1); 
            SOC_DCB_TX_OPCODE_SET(unit, dcb, SOC_HIGIG_OP_UC); 
        }
#endif

        if ((rv = soc_dma_rom_tx_start(unit, dcb)) < 0) {
            cli_out("soc_dma_rom_tx_start() returned %d: %s\n",
                    rv, soc_errmsg(rv));         
            soc_dma_rom_dcb_free(unit, dcb); 
            return CMD_FAIL; 
        }
        
        do {
            soc_dma_rom_tx_poll(unit, &done);   
            polls++;
        } while(!done); 

        cli_out("tx done in %d polls\n", polls); 
        soc_dma_rom_dcb_free(unit, dcb); 

    }
    else {
        return CMD_USAGE; 
    }

    return CMD_OK; 
}


