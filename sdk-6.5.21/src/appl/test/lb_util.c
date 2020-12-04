/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utilities for Loopback Tests, Mark 2
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/test.h>


#include <soc/debug.h>
#include <soc/hash.h>
#include <soc/higig.h>
#include <soc/drv.h>
#ifdef BCM_ESW_SUPPORT
#include <soc/firebolt.h>
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/ism.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_ESW_SUPPORT
#include <bcm_int/esw/mbcm.h>
#endif


#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/link.h>
#include <bcm/stg.h>
#include <bcm/tx.h>
#include <bcm/stat.h>
#include <bcm/mcast.h>
#include <bcm/stack.h>
#include <bcm/cosq.h>
#include <bcm/port.h>

#include <appl/test/loopback.h>
#include <appl/test/loopback2.h>
#include <appl/test/lb_util.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#endif /* PORTMOD_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif

/*
 * De-allocate any packet buffers in the RX packet pool.
 * Then clear all that memory.
 */
#define CLEAR_RX_PKTS(lw, lp)                                           \
    do {                                                                \
        int idxm;                                                       \
        for (idxm = 0; idxm < lp->ppt_end; idxm++) {                    \
            if (LB2_RX_PKT(lw, idxm)->alloc_ptr) {                      \
                bcm_rx_free((lw)->unit, LB2_RX_PKT(lw, idxm)->alloc_ptr); \
            }                                                           \
        }                                                               \
        sal_memset((lw)->rx_pkts, 0, (lp)->ppt_end * sizeof(bcm_pkt_t));\
    } while (0)


/* sal_mac_addr_t lbu_mac_default = {0x00, 0x00, 0x00, 0xde, 0xad, 0x00}; */
sal_mac_addr_t lbu_mac_src = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
sal_mac_addr_t lbu_mac_dst = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};

const char *lb2_parse_speed[] = {
    LB2_SPEED_INIT_STR,
    NULL
};


STATIC uint32 lbu_saved_srcmod[SOC_MAX_NUM_DEVICES];
STATIC uint32 lbu_saved_srcport[SOC_MAX_NUM_DEVICES];

STATIC
char *lb2_crc_mode[] = { "CpuNone", "CpuAppend", "MacRegen", NULL };

const char *lb2_parse_snake[] = {
    "INCreasing",
    "DECreasing",
    "TWOways",
    NULL
};

const char *lb2_parse_lbmode[] = {
    "MAC",
    "PHY",
    "EXTernal",
    NULL
};

static char *send_mech_list[] = {
    "Single", "Array", NULL /* "LinkList" */
};

/* Forward declarations */
STATIC void mac_params_init(int unit, int num_cos, loopback2_testdata_t *lp);
STATIC void phy_params_init(int unit, int num_cos, loopback2_testdata_t *lp);
STATIC void ext_params_init(int unit, int num_cos, loopback2_testdata_t *lp);
STATIC void snake_params_init(int unit, int num_cos, loopback2_testdata_t *lp);

STATIC  uint32
lbu_timeout_usec(int unit)
{
    uint32              defl;

    /* These values are given in seconds, then converted to us below */
    if (SAL_BOOT_QUICKTURN) {
        defl = LB2_PACKET_TIMEOUT_QT;
    } else if (SAL_BOOT_PLISIM) {
        defl = LB2_PACKET_TIMEOUT_PLI;
    } else {
        defl = LB2_PACKET_TIMEOUT;
    }
    
    return (soc_property_get(unit, spn_DIAG_LB_PACKET_TIMEOUT, defl) *
            SECOND_USEC);
}

/*
 * Loopback work structure
 */

void
lbu_pkt_param_add(int unit, parse_table_t *pt, loopback2_testdata_t *lp)
 /*
 * Returns:     0 for success, -1 for failed.
 */
{
    if (lp->test_type == LB2_TT_EXT) {
        parse_table_add(pt, "TxPortBitMap",PQ_PBMP|PQ_DFL,      0,
                        &lp->pbm_tx,    NULL);
        parse_table_add(pt, "DestinationIncrement",PQ_INT|PQ_DFL, 0,
                        &lp->dst_inc,   NULL);
    } else {
        parse_table_add(pt, "PortBitMap", PQ_PBMP | PQ_DFL, 0, 
                        &lp->pbm, NULL);
    }
    if ((lp->test_type == LB2_TT_MAC) || (lp->test_type == LB2_TT_PHY)) {
        if (!SOC_IS_XGS12_FABRIC(unit)){
            parse_table_add(pt, "UntagBitMap",  PQ_PBMP|PQ_DFL, 0,
                            &lp->ubm,   NULL);
        }
    }

    parse_table_add(pt, "HGDestMOD",     PQ_DFL|PQ_INT,  0,
                    &lp->d_mod,      NULL);
    if (lp->test_type == LB2_TT_SNAKE) {
        parse_table_add(pt, "HGDestMODInc",     PQ_DFL|PQ_INT,  0,
                        &lp->d_mod_inc,      NULL);
    }
    parse_table_add(pt, "HGDestPORT",    PQ_DFL|PQ_INT, 0,
                    &lp->d_port,     NULL);
    parse_table_add(pt, "OpCode",    PQ_DFL|PQ_INT, 0,
                    &lp->opcode,     NULL);

    parse_table_add(pt, "SourceMac",    PQ_DFL|PQ_MAC,  0,
                    &lp->mac_src,       NULL);
    parse_table_add(pt, "SourceMacInc",PQ_DFL|PQ_INT,   0,
                    &lp->mac_src_inc,NULL);
    parse_table_add(pt, "DestMac",      PQ_DFL|PQ_MAC,  0,
                    &lp->mac_dst,       NULL);
    parse_table_add(pt, "DestMacInc",   PQ_DFL|PQ_INT,  0,
                    &lp->mac_dst_inc,NULL);
    parse_table_add(pt, "Pattern", PQ_HEX | PQ_DFL, 0, &lp->pattern, NULL);
    parse_table_add(pt, "PatternIncrement", PQ_HEX | PQ_DFL, 0,
                    &lp->pattern_inc, NULL);
    parse_table_add(pt, "VLantag",      PQ_INT|PQ_DFL,  0,
                    &lp->vlan,  NULL);
    parse_table_add(pt, "LengthStart", PQ_INT | PQ_DFL, 0,
                    &lp->len_start, NULL);
    parse_table_add(pt, "LengthEnd",    PQ_INT|PQ_DFL,  0,
                    &lp->len_end,       NULL);
    parse_table_add(pt, "LengthIncrement",PQ_INT|PQ_DFL, 0,
                    &lp->len_inc,       NULL);
    parse_table_add(pt, "PktsPerTrialStart",PQ_INT|PQ_DFL, 0,
                    &lp->ppt_start,     NULL);
    parse_table_add(pt, "PktsPerTrialEnd",PQ_INT|PQ_DFL, 0,
                    &lp->ppt_end,       NULL);
    parse_table_add(pt, "PktsPerTrialInc",PQ_INT|PQ_DFL, 0,
                    &lp->ppt_inc,       NULL);
    parse_table_add(pt, "SENDMethod", PQ_DFL|PQ_MULTI, 0,
                    &lp->send_mech, send_mech_list);
    parse_table_add(pt, "Count", PQ_INT | PQ_DFL, 0, &lp->iterations, NULL);
    parse_table_add(pt, "CRC",          PQ_MULTI|PQ_DFL, 0,
                    &lp->crc_mode, lb2_crc_mode);
    parse_table_add(pt, "PortSelectMode",     PQ_INT|PQ_DFL, 0,
                    &lp->port_select_mode, NULL);
}

void
lbu_port_param_add(int unit, parse_table_t *pt, loopback2_testdata_t *lp)
{
    parse_table_add(pt, "Speed",        PQ_MULTI|PQ_DFL,0,
                    &lp->speed, lb2_parse_speed);
    parse_table_add(pt, "AutoNeg",      PQ_BOOL|PQ_DFL, 0,
                    &lp->autoneg,       NULL);
    if (lp->test_type == LB2_TT_SNAKE) {
        parse_table_add(pt, "LoopbackMode", PQ_MULTI | PQ_DFL, 0,
                        &lp->loopback, lb2_parse_lbmode);
    }
}

void
lbu_other_param_add(int unit, parse_table_t *pt, loopback2_testdata_t *lp)
{
    parse_table_add(pt, "COSStart",     PQ_INT|PQ_DFL,  0,
                    &lp->cos_start,NULL);
    parse_table_add(pt, "COSEnd",       PQ_INT|PQ_DFL,  0,
                    &lp->cos_end,NULL);
    parse_table_add(pt, "CheckData",    PQ_BOOL|PQ_DFL, 0,
                    &lp->check_data,NULL);
    parse_table_add(pt, "CheckCrc",     PQ_BOOL|PQ_DFL, 0,
                    &lp->check_crc,NULL);
    parse_table_add(pt, "InjectPackets", PQ_BOOL | PQ_DFL, 0,
                    &lp->inject, NULL);
    if (lp->test_type == LB2_TT_SNAKE) {
        parse_table_add(pt, "Duration", PQ_INT | PQ_DFL, 0,
                        &lp->duration, NULL);
        parse_table_add(pt, "Interval", PQ_INT | PQ_DFL, 0,
                        &lp->interval, NULL);
        parse_table_add(pt, "SnakePath", PQ_MULTI | PQ_DFL, 0,
                        &lp->snake_way, lb2_parse_snake);
#ifdef BCM_TOMAHAWK_SUPPORT
        parse_table_add(pt, "LineRate", PQ_BOOL | PQ_DFL, 0,
                        &lp->line_rate, NULL);
#endif
    }
}

bcm_rx_t
lbu_rx_callback(int unit, bcm_pkt_t *info, void *cookie)
/*
 * Function:    lbu_rx_callback
 * Purpose:     This is the callback handler for the packets forwarded to
 *              the CMIC.
 * Parameters:
 *              unit - device code.
 *              info - received packet description structure.
 *              cookie - lw - pointer to lb work.
 * Returns:     0 - success, -1 failed.
 * Notes:
 *       Called in INTERRUPT CONTEXT
 */
{
    loopback2_test_t     *lw = (loopback2_test_t *)cookie;
    int                  num_pkts = lw->tx_ppt;
    bcm_pkt_t            *local_info;

    if (lw->expect_pkts) {
        /* Queue the packet */
        /* Copy pkt into array at location rx_pkt_cnt */
        local_info = LB2_RX_CUR_PKT(lw);
        sal_memcpy(local_info, info, sizeof(bcm_pkt_t));
        /* Fix internal packet pointer */
        local_info->pkt_data = &local_info->_pkt_data;
        if (++lw->rx_pkt_cnt >= num_pkts) {
            lw->expect_pkts = FALSE;
            /* Wake up thread to check things out */
            if (!lw->sema_woke) {
                lw->sema_woke = TRUE;
                if (sal_sem_give(lw->sema)) {
                    cli_out("Warning: LB RX done give failed\n");
                }
            }
        }
        return BCM_RX_HANDLED_OWNED;
    } 

    /* Not accepting packets; let rx discard */
    return BCM_RX_NOT_HANDLED;

}

int
lbu_port_monitor_task(int unit, bcm_rx_cb_f callback, void *cookie)
{
    int rv = BCM_E_NONE;
    bcm_rx_cfg_t rx_cfg;

#ifdef INCLUDE_RCPU
    
    if ((soc_property_get_str(unit, spn_RCPU_PORT) == NULL) && (BCM_RX_INIT_DONE(unit))) {
        cli_out("lbu monitor:  RX already running (u=%d)\n", unit);
        return BCM_E_INIT;
    }
#else
    if (BCM_RX_INIT_DONE(unit)) {
        cli_out("lbu monitor:  RX already running (u=%d)\n", unit);
        return BCM_E_INIT;
    }
#endif /* INCLUDE_RCPU */

    /* Get current configuration */
    bcm_rx_cfg_init(unit);   /* Re-init in case in bad state */
    bcm_rx_cfg_get(unit, &rx_cfg);
    rx_cfg.global_pps = BCM_RX_RATE_NOLIMIT;
    rx_cfg.chan_cfg[BCM_RX_CHAN_DFLT].rate_pps = BCM_RX_RATE_NOLIMIT;
    rv = bcm_rx_start(unit, &rx_cfg);
#ifdef INCLUDE_RCPU
#else
    if (BCM_E_NONE != rv) {
        cli_out("lbu_port_monitor_task:"
                "rx_start failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
#endif /* INCLUDE_RCPU */

    /* Allow BCM Rx thread to prepare DMA buffers */
    sal_usleep(500000);
    
    rv = bcm_rx_register(unit, "loopback test", callback,
                         BCM_RX_PRIO_MAX, cookie,
                         BCM_RCO_F_ALL_COS);
    if (BCM_E_NONE != rv) {
        cli_out("lbu_port_monitor_task:"
                "rx_register failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

STATIC int
lbu_packet_compare(loopback2_test_t *lw, bcm_pkt_t *tx_info, 
                   bcm_pkt_t *rx_info)
{
    loopback2_testdata_t        *lp = lw->cur_params;
    int         unit = lw->unit;
    int         i;
    int         rv = 0;
    int         tx_compare_len, crc_length = 0;
    uint8       *tx_data, *rx_data;

    tx_data = BCM_PKT_IEEE(tx_info);
    rx_data = BCM_PKT_IEEE(rx_info);

#ifdef BCM_XGS_SUPPORT
    /* Inspect HIGIG/XGS header if received from HG port */
    if (SOC_IS_XGS12_FABRIC(unit)) {
        soc_higig_hdr_t *tx_hdr = (soc_higig_hdr_t*)tx_info->_higig;
        soc_higig_hdr_t *rx_hdr = (soc_higig_hdr_t*)rx_info->_higig;

        if (soc_higig_field_get(unit, rx_hdr, HG_start) != 
            SOC_HIGIG_START) {
            cli_out("ERROR: HIGIG START not detected (rx=%x)\n",
                    soc_higig_field_get(unit, rx_hdr, HG_start));
            rv = -1;
        }

        if (soc_higig_field_get(unit, rx_hdr, HG_hgi) != 
            SOC_HIGIG_HGI) {
            cli_out("ERROR: HIGIG identifier not detected (rx=%x)\n",
                    soc_higig_field_get(unit, rx_hdr, HG_hgi));
            rv = -1;
        }

        if (soc_higig_field_get(unit, tx_hdr, HG_vlan_pri) != 
            soc_higig_field_get(unit, rx_hdr, HG_vlan_pri)) {
            cli_out("ERROR: HIGIG VLAN priority miscompare (tx=%x,rx=%x)\n",
                    soc_higig_field_get(unit, tx_hdr, HG_vlan_pri), 
                    soc_higig_field_get(unit, rx_hdr, HG_vlan_pri));
            rv = -1;
        }

        if (soc_higig_field_get(unit, tx_hdr, HG_vlan_cfi) != 
            soc_higig_field_get(unit, rx_hdr, HG_vlan_cfi)) {
            cli_out("ERROR: HIGIG VLAN cfi miscompare (tx=%x,rx=%x)\n",
                    soc_higig_field_get(unit, tx_hdr, HG_vlan_cfi), 
                    soc_higig_field_get(unit, rx_hdr, HG_vlan_cfi));
            rv = -1;
        }

        if (soc_higig_field_get(unit, tx_hdr, HG_vlan_id) != 
            soc_higig_field_get(unit, rx_hdr, HG_vlan_id)) {
            cli_out("ERROR: HIGIG VLAN id miscompare (tx=%x,rx=%x)\n",
                    soc_higig_field_get(unit, tx_hdr, HG_vlan_id), 
                    soc_higig_field_get(unit, rx_hdr, HG_vlan_id));
            rv = -1;
        }

        if (soc_higig_field_get(unit, tx_hdr, HG_opcode) != 
            soc_higig_field_get(unit, rx_hdr, HG_opcode)) {
            cli_out("ERROR: HIGIG opcode miscompare (tx=%x,rx=%x)\n",
                    soc_higig_field_get(unit, tx_hdr, HG_opcode), 
                    soc_higig_field_get(unit, rx_hdr, HG_opcode));
            rv = -1;
        }

        /* What about remapping the COS? */
        if (soc_higig_field_get(unit, tx_hdr, HG_cos) != 
            soc_higig_field_get(unit, rx_hdr, HG_cos)) {
            cli_out("ERROR: HIGIG cos miscompare (tx=%x,rx=%x)\n",
                    soc_higig_field_get(unit, tx_hdr, HG_cos), 
                    soc_higig_field_get(unit, rx_hdr, HG_cos));
            rv = -1;
        }

        
    }
#endif /* BCM_XGS_SUPPORT */
    
    /* Check source and destination MAC address */

    if (ENET_CMP_MACADDR(BCM_PKT_DMAC(tx_info), BCM_PKT_DMAC(rx_info)) ||
        ENET_CMP_MACADDR(BCM_PKT_DMAC(tx_info) + 6, 
                         BCM_PKT_DMAC(rx_info) + 6)) {
        char    src_mac[SAL_MACADDR_STR_LEN],
                dst_mac[SAL_MACADDR_STR_LEN],
                exp_src_mac[SAL_MACADDR_STR_LEN],
                exp_dst_mac[SAL_MACADDR_STR_LEN];
        format_macaddr(exp_src_mac, BCM_PKT_DMAC(tx_info) + 6);
        format_macaddr(exp_dst_mac, BCM_PKT_DMAC(tx_info));
        format_macaddr(src_mac, BCM_PKT_DMAC(rx_info) + 6);
        format_macaddr(dst_mac, BCM_PKT_DMAC(rx_info));
        cli_out("ERROR: MAC address miscompare:\n"
                "\tExpected src=%s dst=%s\n\tReceived src=%s dst=%s\n",
                exp_src_mac, exp_dst_mac, src_mac, dst_mac);

        rv = -1;
    }

#ifdef BCM_ESW_SUPPORT
    crc_length = ((lp->crc_mode == LB2_CRC_MODE_CPU_APPEND) ? 4 : 0);
#endif /* BCM_ESW_SUPPORT */

    /* Verify lengths, and if they match, verify payload */
    tx_compare_len = tx_info->_pkt_data.len; 
    if (lp->crc_mode != LB2_CRC_MODE_CPU_APPEND) {
        tx_compare_len -= 4;
    }

    if (rx_info->pkt_len != (tx_info->_pkt_data.len + crc_length)) {
#ifdef BCM_ESW_SUPPORT
        if (SOC_IS_ESW(unit)) {
            soc_pci_analyzer_trigger(unit);
        }
#endif /* BCM_ESW_SUPPORT */
        cli_out("ERROR: Length miscompare: TX(%d) RX(%d)\n",
                tx_info->_pkt_data.len, rx_info->pkt_len);
        rv = -1;
    } else if ((i = packet_compare(BCM_PKT_VLAN_PTR(rx_info),
                                   BCM_PKT_VLAN_PTR(tx_info),
                                   4)) >= 0) {
        cli_out("ERROR: VLAN tag miscompare: offset 0x%x\n", i + 12);
        rv = -1;
    } else if ((i = packet_compare(rx_data + 16,
                                   tx_data + 16,
                                   4)) >= 0) {
        cli_out("ERROR: Sequence # miscompare: offset 0x%x\n"
                "\tExpected 0x%08x, Received 0x%08x\n", 16,
                packet_load(tx_data + 16, 4),
                packet_load(rx_data + 16, 4));
        rv = -1;
    } else if ((i = packet_compare(rx_data + 20,
                                   tx_data + 20,
                                   tx_compare_len - 20)) >= 0) {
        cli_out("ERROR: Payload miscompare: offset 0x%x\n", i + 20);
        rv = -1;
    }

    if (lp->check_crc) {
        uint32 calc_crc = 0, rx_crc;
        if (lp->vlan) {
            if (rx_info->pkt_len > 0) {
                /* Skip Vlan Tag for CRC calculation */
                uint8 *rx_data_crc;
                rx_data_crc = sal_alloc(rx_info->pkt_len, "rx_data_crc");
                sal_memset(rx_data_crc, 0, rx_info->pkt_len);
                sal_memcpy(rx_data_crc, rx_data, 12);
                sal_memcpy(rx_data_crc + 12, rx_data + 16,
                           rx_info->pkt_len - 16);
                calc_crc = ~_shr_crc32(~0, rx_data_crc, rx_info->pkt_len - 8);
                sal_free(rx_data_crc);
            }
        } else {
            calc_crc = ~_shr_crc32(~0, rx_data, rx_info->pkt_len - 4);
        }
        rx_crc = packet_load(rx_data + rx_info->pkt_len - 4, 4);

        if (calc_crc != rx_crc) {
            cli_out("ERROR: CRC miscompare: calc=0x%08x rx=0x%08x\n",
                    calc_crc, rx_crc);
            
            rv = -1;
        }
    }

    if (rv) {
        /* On error, dump tx/rx packets, and call test_error once */

        cli_out("TX packet: len=%d\n", tx_info->_pkt_data.len);
#ifdef BCM_ESW_SUPPORT
        soc_dma_dump_pkt(unit, "  ", tx_data, tx_info->_pkt_data.len, TRUE);
        cli_out("RX packet: len=%d\n", rx_info->pkt_len);
        soc_dma_dump_pkt(unit, "  ", rx_data, rx_info->pkt_len, TRUE);
#endif
        test_error(unit,
                   "ERROR Found when verifying received packet\n");
    }

    return(rv);
}

STATIC int
lbu_snake_pkt_match(loopback2_test_t *lw, bcm_pkt_t *rx_info, int *match_idx)
/*
 * Function:    lb_isnake_pkt_match
 * Purpose:     Search for tx packet matching rx packet.
 * Parameters:  lw - pointer to lb work.
 *              rx_info - pointer to rx packet  info
 *              match_idx - return index for tx matching packet
 * Returns:     0 for match, -1 for no match, -2 for duplicate match.
 */
{
    int             tx_idx;
    bcm_pkt_t       *compare_info;

    for (tx_idx = 0; tx_idx < lw->tx_ppt; tx_idx++) {

        compare_info = lw->tx_pkts[tx_idx];

        if (ENET_CMP_MACADDR(BCM_PKT_DMAC(compare_info), 
                             BCM_PKT_DMAC(rx_info)) ||
            ENET_CMP_MACADDR(BCM_PKT_DMAC(compare_info) + 6, 
                             BCM_PKT_DMAC(rx_info) + 6)) {
            /* Wrong macs */
            continue;
        }

        if (BCM_PKT_DMAC(compare_info)[LB2_ID_POS] != 
            BCM_PKT_DMAC(rx_info)[LB2_ID_POS]) {
            /* Wrong SW identifying tag */
            continue;
        }

        *match_idx = tx_idx;

        if (lw->tx_pkt_match[tx_idx]) {
            /* Already matched this packet, duplicate arrival */
            return -2;
        }

        lw->tx_pkt_match[tx_idx]  = TRUE;

        return 0;
    }

    return -1;
}

int
lbu_snake_analysis(loopback2_test_t *lw)
{
    int             unit = lw->unit;
    int             ix, ev = 0;
    bcm_pkt_t       *tx_info, *rx_info;
    int             match_idx;

    for (ix = 0; ix < lw->rx_pkt_cnt; ix++) {
        rx_info = LB2_RX_PKT(lw, ix);
        match_idx = -1;
        if ((ev = lbu_snake_pkt_match(lw, rx_info, &match_idx)) < 0) {
            if (match_idx >=0) {
                tx_info = lw->tx_pkts[match_idx];
                cli_out("TX packet: len=%d\n", tx_info->_pkt_data.len);
                
#ifdef BCM_ESW_SUPPORT                
                soc_dma_dump_pkt(unit, "  ", BCM_PKT_IEEE(tx_info), 
                                 tx_info->_pkt_data.len, TRUE);
#endif
            } else {
                cli_out("No TX match\n");
            }
            
            if (ev == -2) {
                test_error(unit, 
                           "Duplicate packet from port %s\n",
                           SOC_PORT_NAME(unit, rx_info->rx_port));
            } else {
                test_error(unit, 
                           "Unrecognized packet from port %s\n",
                           SOC_PORT_NAME(unit, rx_info->rx_port));
            }
          
            return -1;
        }
        
        tx_info = lw->tx_pkts[match_idx];

        if (lbu_packet_compare(lw, tx_info, rx_info) < 0) {
            return -1;
       }
    }
    return 0;
}

STATIC int
lbu_set_l2_addr(int unit, bcm_port_t p, bcm_vlan_t vlan, 
                bcm_l2_addr_t *l2_addr, sal_mac_addr_t mac_addr)
/*
 * Function:    lbu_set_l2_addr
 * Purpose:     Set an L2 entry
 * Parameters:  unit - SOC unit #
 *              p - port to set address for.
 *              vlan - vlan tag to use in ARL entry.
 *              l2_addr - pointer to arl entry to use.
 * Returns:     0 - success, -1 failed.
 */
{
    int rv;

    if ((rv = bcm_l2_addr_delete(unit, mac_addr, vlan)) < 0) {
        if (rv != BCM_E_NOT_FOUND) {
            cli_out("bcm_l2_addr_delete FAILED\n");
            return rv;
        } /* else, non-existent entry */
    }

    sal_memcpy(l2_addr->mac, mac_addr, sizeof(sal_mac_addr_t));
    l2_addr->vid = vlan;
    l2_addr->flags = BCM_L2_STATIC;
    l2_addr->port = p;
    return(bcm_l2_addr_add(unit, l2_addr));
}

int
lbu_setup_arl_cmic(loopback2_test_t *lw)
/*
 * Function:    lbu_setup_arl
 * Purpose:     Setup an ARL entry for the specified port
 * Parameters:  unit - unit #
 *              dst_port - port to which packet is sent
 * Returns:     0 - success
 *              -1 - failed.
 * Notes:       Only the destination port CBIT is returned, as only
 *              it is required for the test.
 */
{
    loopback2_testdata_t *lp = lw->cur_params;
    int                  unit = lw->unit;
    int                  rv = 0;
    sal_mac_addr_t       cur_mac;
    bcm_l2_addr_t        *l2_addr = &lw->l2_addr_scratch;
    int                  pkt_cur;
#ifdef BCM_TRIUMPH_SUPPORT
    int ext_index_max = -1;
#endif /* BCM_TRIUMPH_SUPPORT */

    ENET_SET_MACADDR(cur_mac, lw->base_mac_dst);
    bcm_l2_addr_t_init(l2_addr, cur_mac, 0);

    /* Get the module ID for this unit */
    if (SOC_IS_XGS_SWITCH(unit)) {
        if ((rv = bcm_stk_my_modid_get(unit, &l2_addr->modid)) < 0) {
            test_error(unit,
                       "Modid retrieval failed: %s\n",
                       soc_errmsg(rv));
            return(-1);
        }
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
        ext_index_max = SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max;
        SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = -1;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    for (pkt_cur = 0; pkt_cur < lp->ppt_end; pkt_cur++) {
        rv |= lbu_set_l2_addr(unit, CMIC_PORT(unit), lp->vlan,
                              l2_addr, cur_mac);
        increment_macaddr(cur_mac, lp->mac_dst_inc);
    }
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
        SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = ext_index_max;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if (rv) {
        test_error(unit,
                   "Failed to set up L2 address: port %s\n",
                   SOC_PORT_NAME(unit, CMIC_PORT(unit)));
        return(-1);
    } else if (l2_addr->port != CMIC_PORT(unit)) {
        test_error(unit,
                   "Port %s: invalid destination arl port: %s\n",
                   SOC_PORT_NAME(unit, CMIC_PORT(unit)),
                   SOC_PORT_NAME(unit, l2_addr->port));
        return(-1);
    }

    return 0;
}

#ifdef BCM_HERCULES_SUPPORT 
STATIC int
lbu_connect_herc_ports(int unit, lb2s_port_connect_t *pc_info,
                       int connect)
{
    pbmp_t             uc;
    uint32             ucdata;
    int                blk;

    /* Configure UC table for packet forwarding */
    SOC_PBMP_CLEAR(uc);
    blk = SOC_PORT_BLOCK(unit, pc_info->this_port);
    if (connect) {
        SOC_PBMP_PORT_ADD(uc, pc_info->to_port);
        ucdata = SOC_PBMP_WORD_GET(uc, 0);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, MEM_UCm, blk,
                                          pc_info->src_modid, &ucdata));
    } else {
        ucdata = SOC_PBMP_WORD_GET(uc, 0);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, MEM_UCm, blk,
                                          pc_info->src_modid, &ucdata));
    }
    return 0;
}
#endif

STATIC int
lbu_connect_ether_ports(int unit, lb2s_port_connect_t *pc_info,
                        int connect)
{
    int rv;
#ifdef BCM_TRIUMPH_SUPPORT
    int ext_index_max = -1;
#endif /* BCM_TRIUMPH_SUPPORT */

    if (MAC_IS_MCAST(pc_info->dst_mac)) {
        if (connect) {
            bcm_mcast_addr_t   l2mc_addr;
            sal_memset(&l2mc_addr, 0, sizeof (bcm_mcast_addr_t));
            ENET_SET_MACADDR(l2mc_addr.mac, pc_info->dst_mac);
            l2mc_addr.vid = pc_info->added_vlan;
            SOC_PBMP_CLEAR(l2mc_addr.pbmp);
            SOC_PBMP_PORT_ADD(l2mc_addr.pbmp, pc_info->this_port);
            SOC_PBMP_PORT_ADD(l2mc_addr.pbmp, pc_info->to_port);
            SOC_PBMP_ASSIGN(l2mc_addr.ubmp, l2mc_addr.pbmp);
            BCM_IF_ERROR_RETURN(bcm_mcast_addr_add(unit, &l2mc_addr));
        } else {
            BCM_IF_ERROR_RETURN(bcm_mcast_addr_remove(unit, pc_info->dst_mac, 
                                                      pc_info->added_vlan));
        }
    } else {
        if (connect) {
            bcm_l2_addr_t   l2_addr;
            bcm_l2_addr_t_init(&l2_addr,
                               pc_info->dst_mac, pc_info->added_vlan);
            if (NUM_MODID(unit) == 3 &&
                pc_info->to_port % (SOC_PORT_ADDR_MAX(unit) + 1) > 63) {
                l2_addr.port =
                    (pc_info->to_port % (SOC_PORT_ADDR_MAX(unit) + 1)) - 64;
                l2_addr.modid = pc_info->dst_modid + 2;
            } else if (NUM_MODID(unit) >= 2 &&
                       pc_info->to_port % (SOC_PORT_ADDR_MAX(unit) + 1) > 31) {
                l2_addr.port =
                    (pc_info->to_port % (SOC_PORT_ADDR_MAX(unit) + 1)) - 32; 
                l2_addr.modid = pc_info->dst_modid + 1;
            } else {
                l2_addr.port =
                    pc_info->to_port % (SOC_PORT_ADDR_MAX(unit) + 1);
                l2_addr.modid = pc_info->dst_modid;
            }
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
                ext_index_max =
                    SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max;
                SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = -1;
            }
#endif /* BCM_TRIUMPH_SUPPORT */
            rv = bcm_l2_addr_add(unit, &l2_addr);
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
                SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max =
                    ext_index_max;
            }
#endif /* BCM_TRIUMPH_SUPPORT */
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            if ((pc_info->this_port == pc_info->to_port) &&
                (pc_info->dst_modid == pc_info->src_modid)) {
                BCM_IF_ERROR_NOT_UNAVAIL_RETURN(
                    bcm_port_control_set(unit, pc_info->this_port, 
                                         bcmPortControlBridge, 1));
            }
        } else {
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
                ext_index_max =
                    SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max;
                SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = -1;
            }
#endif /* BCM_TRIUMPH_SUPPORT */
            rv = bcm_l2_addr_delete(unit, pc_info->dst_mac,
                                    pc_info->added_vlan);
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
                SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max =
                    ext_index_max;
            }
#endif /* BCM_TRIUMPH_SUPPORT */
            if (BCM_FAILURE(rv)) {
                if (rv != BCM_E_NOT_FOUND) {
                    return rv;
                } /* else, non-existent entry */
            }

            /* Disable Port bridging if src and dest ports are same */
            if ((pc_info->this_port == pc_info->to_port) &&
                (pc_info->dst_modid == pc_info->src_modid)) {
                BCM_IF_ERROR_NOT_UNAVAIL_RETURN(
                    bcm_port_control_set(unit, pc_info->this_port, 
                                         bcmPortControlBridge, 0));
            }
        }
     }
    return 0;
}

STATIC int
lbu_set_modport(int unit, bcm_port_t src_port, int dst_port, int modid)
{
    /*      
     * A ghastly construct.  Select correct modid field, 
     * and modify dest port into field.
     */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_TR_VL(unit)) {
        return 0;
    }
    if (SOC_IS_HBX(unit)) {
        int rv;
        modport_map_entry_t uc;
        int idx = (src_port * (SOC_MODID_MAX(unit) + 1)) + modid;
        soc_mem_lock(unit, MODPORT_MAPm);
        rv = READ_MODPORT_MAPm(unit, MEM_BLOCK_ANY, idx, &uc);
        if (SOC_SUCCESS(rv)) {
            /* Use MODPORT_MAP for forwarding packets for both HG and XE ports.
             */
            soc_MODPORT_MAPm_field32_set(unit, &uc, HIGIG_PORT_BITMAPf,
                                         1 << dst_port);
            rv = WRITE_MODPORT_MAPm(unit, MEM_BLOCK_ALL, idx, &uc);
        }
        soc_mem_unlock(unit, MODPORT_MAPm);
        return rv;
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    return 0;
}

STATIC int
lbu_connect_ether_to_hg_port(int unit, lb2s_port_connect_t *pc_info, 
                             int connect)
{
    bcm_l2_addr_t   l2_addr;

    if (connect) {
        bcm_l2_addr_t_init(&l2_addr, pc_info->dst_mac, pc_info->added_vlan);
        l2_addr.modid = pc_info->dst_modid;
        l2_addr.port = pc_info->to_port % (SOC_PORT_ADDR_MAX(unit) + 1);
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
        SOC_IF_ERROR_RETURN(lbu_set_modport(unit, pc_info->this_port,
                                    pc_info->to_port, pc_info->dst_modid));
    } else {
        int rv;

        if ((rv = bcm_l2_addr_delete(unit, pc_info->dst_mac,
                                     pc_info->added_vlan)) < 0) {
            if (rv != BCM_E_NOT_FOUND) {
                return rv;
            } /* else, non-existent entry */
        }
        
    }
    return 0;
}

STATIC int
lbu_connect_hg_to_ether_port(int unit, lb2s_port_connect_t *pc_info, 
                             int connect)
{
    if (connect) {
        SOC_IF_ERROR_RETURN(lbu_set_modport(unit, pc_info->this_port,
                                    pc_info->to_port, pc_info->src_modid));
    } else {
        
    }
    return 0;
}

int
lbu_connect_ports(int unit, lb2s_port_connect_t *pc_info,
                  int connect)
{
    int rv;

    if (!pc_info->valid_info) {
        /* Nothing to do */
        return 0;
    }

    if (connect) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Port connection:\n"
                             "  RX(mod,port)=%d,%d, TX(mod,port)=%d,%d, VLAN=0x%03x,\n"
                             "  SA=%02x:%02x:%02x:%02x:%02x:%02x, "
                             "DA=%02x:%02x:%02x:%02x:%02x:%02x\n"),
                  pc_info->src_modid, pc_info->this_port,
                  pc_info->dst_modid, pc_info->to_port,
                  pc_info->added_vlan,
                  pc_info->src_mac[0], pc_info->src_mac[1], pc_info->src_mac[2],
                  pc_info->src_mac[3], pc_info->src_mac[4], pc_info->src_mac[5],
                  pc_info->dst_mac[0], pc_info->dst_mac[1], pc_info->dst_mac[2],
                  pc_info->dst_mac[3], pc_info->dst_mac[4], pc_info->dst_mac[5]));
    }
    /* Sort out things into the proper subsettings */
#ifdef BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES(unit)) {
        rv = lbu_connect_herc_ports(unit, pc_info, connect);
    } else 
#endif
    {
        if (IS_E_PORT(unit, pc_info->this_port)) {
            if (IS_E_PORT(unit, pc_info->to_port)) {
                rv = lbu_connect_ether_ports(unit, pc_info, connect);
            } else if (IS_ST_PORT(unit, pc_info->to_port)) {
                rv = lbu_connect_ether_to_hg_port(unit, pc_info, connect);
            } else { /* Should be CMIC */
                rv = lbu_connect_ether_ports(unit, pc_info, connect);
            }
        } else {
            if (IS_ST_PORT(unit, pc_info->to_port)) {
                if (SOC_IS_HBX(unit)) {
                    rv = lbu_set_modport(unit, pc_info->this_port,
                                         pc_info->to_port, pc_info->dst_modid);
                } else {
                    /* Uhhh, now what? Two HG ports */
                    /* Not supported for FB/ER */
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META_U(unit,
                                         "lbu_connect_ports: Two HG ports rv = %d\n"),
                              BCM_E_UNAVAIL));
                    return BCM_E_UNAVAIL;
                }
            } else { /* Ether or CMIC */
                rv = lbu_connect_hg_to_ether_port(unit, pc_info, connect);
            } 
        }
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "lbu_connect_ports: rv = %d\n"), rv));
    return rv;
}


void
lbu_cleanup_arl(loopback2_test_t *lw)
{
    loopback2_testdata_t *lp;
    int                  unit;
    sal_mac_addr_t       cur_mac;
    int                  pkt_cur;
#ifdef BCM_TRIUMPH_SUPPORT
    int ext_index_max = -1;
#endif /* BCM_TRIUMPH_SUPPORT */

    if (lw == NULL) {
        return;
    }
    unit = lw->unit;
    lp = lw->cur_params;
    if (lp == NULL) {
        return;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
        ext_index_max = SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max;
        SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = -1;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    /* Source MAC flush */
    ENET_SET_MACADDR(cur_mac, lw->base_mac_src);
    for (pkt_cur = 0; pkt_cur < lp->ppt_end; pkt_cur++) {
        (void)bcm_l2_addr_delete(unit, cur_mac, lp->vlan);
        increment_macaddr(cur_mac, lp->mac_src_inc);
    }

    /* Destination MAC flush */
    ENET_SET_MACADDR(cur_mac, lw->base_mac_dst);
    for (pkt_cur = 0; pkt_cur < lp->ppt_end; pkt_cur++) {
        (void)bcm_l2_addr_delete(unit, cur_mac, lp->vlan);
        increment_macaddr(cur_mac, lp->mac_dst_inc);
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
        SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = ext_index_max;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
}

STATIC int
lbu_save_port(loopback2_test_t *lw, loopback2_testdata_t *lp)
{
    int         rv;
    bcm_port_t  port;
    pbmp_t      save_ports;
    int         unit = lw->unit;

    if (SOC_IS_XGS12_FABRIC(unit)) {
        BCM_PBMP_ASSIGN(save_ports, PBMP_PORT_ALL(unit));
    } else {
        BCM_PBMP_ASSIGN(save_ports, PBMP_PORT_ALL(unit));
        BCM_PBMP_AND(save_ports, lp->pbm);
    }

    PBMP_ITER(save_ports, port) {
        if ((rv = bcm_port_info_save(unit, port,
                                    &lw->save_port[port])) < 0) {
            test_error(unit,
                       "Port %s: Could not get port info: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            return(-1);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Save original fec, phy config and link training info. Note pm8x50
         * (cd ports) need this information
         */
        if ((SOC_IS_TOMAHAWK3(unit)) && (IS_CD_PORT(unit, port))) {
            rv = bcm_port_resource_speed_get(unit, port,
                                             &lw->port_info[port]);
            if (rv < 0) {
                test_error(unit,
                           "Port %s: bcm_port_resource_speed_get failed:"
                           " %s\n", SOC_PORT_NAME(unit, port),
                           bcm_errmsg(rv));
                return(-1);
            }
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
    }

    
    if (bcm_rx_cfg_get(unit, NULL) == BCM_E_NONE) {
        test_error(unit, "BCM_RX is currently not active; cannot run test\n");
        return(-1);
    }

#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_XGS_SWITCH(unit)) {
        int     not_my_mod;
        /* Save current source port/module and change to != lp->d_mod */

        lbu_saved_srcmod[unit] = SOC_DEFAULT_DMA_SRCMOD_GET(unit);
        lbu_saved_srcport[unit] = SOC_DEFAULT_DMA_SRCPORT_GET(unit);
        not_my_mod = (lp->d_mod == 0);
        SOC_DEFAULT_DMA_SRCMOD_SET(unit, not_my_mod);
        SOC_DEFAULT_DMA_SRCPORT_SET(unit, CMIC_PORT(unit));
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit)) {
            /*
             * Should save(Not yet done here) ICONTROL_OPCODE_BITMAP before
             * the test and restore it after test is done
             * Redirect all packets with CPU control opcode
             * from HG ports to CPU.
             * Replace with API. 
             */
            int pbm_len;
            uint32 cpu_pbm;
            if (SOC_MEM_IS_VALID(unit, ICONTROL_OPCODE_BITMAPm)) {
                cpu_pbm = 0; /* init it, some compiler has false warning */
            } else if (SOC_IS_TR_VL(unit) && !SOC_IS_TRIUMPH2(unit) 
                && !SOC_IS_APOLLO(unit) && !SOC_IS_VALKYRIE2(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_xgs3_port_to_higig_bitmap(unit, CMIC_PORT(unit),
                                                    &cpu_pbm));
            } else if (CMIC_PORT(unit)) {
                pbm_len = soc_reg_field_length(unit, ICONTROL_OPCODE_BITMAPr,
                                               BITMAPf);
		/* Bad shift complaint by Coverity */
                cpu_pbm = pbm_len > 0? 1 << (pbm_len - 1): 0;
            } else {
                cpu_pbm = 1;
            }
            PBMP_ST_ITER(unit, port) {
                if (SOC_MEM_IS_VALID(unit, ICONTROL_OPCODE_BITMAPm)) {
                    icontrol_opcode_bitmap_entry_t entry;
                    sal_memset(&entry, 0, sizeof(entry));
                    soc_mem_pbmp_field_set(unit, ICONTROL_OPCODE_BITMAPm,
                                           &entry, BITMAPf, &PBMP_CMIC(unit));
                    rv = soc_mem_write(unit, ICONTROL_OPCODE_BITMAPm,
                                       MEM_BLOCK_ANY, port, &entry);
                } else if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                    SOC_IS_VALKYRIE2(unit)) {
                    uint64 cpu_pbm_64;
                    COMPILER_64_ZERO(cpu_pbm_64);
                    COMPILER_64_SET(cpu_pbm_64, 0, cpu_pbm);
                    SOC_IF_ERROR_RETURN
                        (WRITE_ICONTROL_OPCODE_BITMAP_64r(unit, port,
                                                          cpu_pbm_64));
                } else if (SOC_REG_IS_VALID(unit, ICONTROL_OPCODE_BITMAP_LOr) &&
                           SOC_REG_IS_VALID(unit, ICONTROL_OPCODE_BITMAP_HIr)) {

                    uint64      fval64, rval64;
                    uint32      t_port;
                    pbmp_t      curr_pbmp;
                    soc_reg_t   reg;

                    BCM_PBMP_CLEAR(curr_pbmp);
                    COMPILER_64_ZERO(fval64);

                    /* Set HG ingress CPU Opcode map to the CPU */
                    if (CMIC_PORT(unit) < 64) {
                        reg = ICONTROL_OPCODE_BITMAP_LOr;
                        t_port = CMIC_PORT(unit);
                    } else {
                        reg = ICONTROL_OPCODE_BITMAP_HIr;
                        t_port = CMIC_PORT(unit) - 64;
                    }

                    SOC_PBMP_PORT_ADD(curr_pbmp, t_port);

                    SOC_IF_ERROR_RETURN(
                            soc_reg_get(unit, reg, port, 0, &rval64));
                    COMPILER_64_SET(fval64,
                                    SOC_PBMP_WORD_GET(curr_pbmp, 1),
                                    SOC_PBMP_WORD_GET(curr_pbmp, 0));

                    soc_reg64_field_set(unit, 
                            reg, &rval64, BITMAPf, fval64);
                    SOC_IF_ERROR_RETURN(
                            soc_reg_set(unit, reg, port, 0, rval64));
                } else {
                    SOC_IF_ERROR_RETURN
                        (WRITE_ICONTROL_OPCODE_BITMAPr(unit, port, cpu_pbm));
                }
            }
        }
#endif
    } 
#ifdef BCM_XGS12_FABRIC_SUPPORT
    else if (SOC_IS_XGS12_FABRIC(unit)) {
        uint32      ucdata;
        pbmp_t      uc;
        int         blk;
        
        SOC_PBMP_CLEAR(uc);
        SOC_PBMP_PORT_ADD(uc, CMIC_PORT(unit));
        ucdata = SOC_PBMP_WORD_GET(uc, 0);
        PBMP_ITER(save_ports, port) {
            blk = SOC_PORT_BLOCK(unit, port);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, MEM_UCm, blk,
                                              lp->d_mod, &ucdata));
        }
    }
#endif /* BCM_XGS12_FABRIC_SUPPORT */
#endif /* BCM_ESW_SUPPORT */

    return(0);
}

int
lbu_setup_port(int unit, bcm_port_t port, int req_speed, int autoneg)
{
    int                    speed = 0, duplex = 0, max_speed;
    bcm_port_ability_t     req_ability;
    bcm_port_ability_t     actual_ability;
    bcm_port_medium_t      medium;

    sal_memset(&req_ability, 0, sizeof(bcm_port_ability_t));

    BCM_IF_ERROR_RETURN(
        bcm_port_speed_max(unit, port, &max_speed));
    if (max_speed > SOC_INFO(unit).port_speed_max[port]) {
        max_speed = SOC_INFO(unit).port_speed_max[port];
    }

    switch (req_speed) {
        case LB_SPEED_10FD:
            speed = 10 ; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_10MB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_100FD:
            speed = 100; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_100MB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_1000FD:
            speed = 1000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_1000MB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_2500FD:
            speed = 2500; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_2500MB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_3000FD:
            speed = 3000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_3000MB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_5000FD:
            speed = 5000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_5000MB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_10G_FD:
            speed = 10000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_10GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_11G_FD:
            speed = 11000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_11GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_12G_FD:
            speed = 12000;
            req_ability.speed_full_duplex = SOC_PA_SPEED_12GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_13G_FD:
            speed = 13000;
            req_ability.speed_full_duplex = SOC_PA_SPEED_13GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_15G_FD:
            speed = 15000;
            req_ability.speed_full_duplex = SOC_PA_SPEED_15GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_16G_FD:
            speed = 16000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_16GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_20G_FD:
            speed = 20000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_20GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_21G_FD:
            speed = 21000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_21GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_23G_FD:
            speed = 23000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_23GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_24G_FD:
            speed = 24000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_24GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_25G_FD:
            speed = 25000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_25GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_30G_FD:
            speed = 30000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_30GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_32G_FD:
            speed = 32000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_32GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_40G_FD:
            speed = 40000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_40GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_42G_FD:
            speed = 42000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_42GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_50G_FD:
            speed = 50000;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_100G_FD:
            speed = 100000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_100GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_106G_FD:
            speed = 106000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_106GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_120G_FD:
            speed = 120000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_120GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_127G_FD:
            speed = 127000; 
            req_ability.speed_full_duplex = SOC_PA_SPEED_127GB;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_200G_FD:
            speed = 200000;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_400G_FD:
            speed = 400000;
            duplex = SOC_PORT_DUPLEX_FULL;
            break;
        case LB_SPEED_MAX:
            /* PM8x50 does not support port_ability_local_get. Use max speed */
            if (!IS_CD_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                bcm_port_ability_local_get(unit, port, &req_ability)); 
            }
            duplex = SOC_PORT_DUPLEX_FULL;
            speed = max_speed;
            break;
        default:
            return(BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN( 
        bcm_port_enable_set(unit, port, TRUE));
    BCM_IF_ERROR_RETURN(
        bcm_port_medium_get(unit, port, &medium));
    if ((medium == BCM_PORT_MEDIUM_FIBER) && 
        (duplex != SOC_PORT_DUPLEX_FULL)) {
        LOG_ERROR(BSL_LS_APPL_TESTS,
                  (BSL_META_U(unit,
                              "unit %d Port %s is in Fiber mode. "
                              "Forcing FullDuplex\n"),
                   unit, SOC_PORT_NAME(unit, port)));
        duplex = SOC_PORT_DUPLEX_FULL;
    }

    if (autoneg) {
        BCM_IF_ERROR_RETURN(
            bcm_port_ability_advert_set(unit, port, &req_ability));
        BCM_IF_ERROR_RETURN(
            bcm_port_autoneg_set(unit, port, TRUE));
    } else {
        /* PM8x50 does not support port_ability_local_get. Use max speed */
        if (!IS_CD_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(
            bcm_port_ability_local_get(unit, port, &actual_ability));

        if (!(actual_ability.speed_full_duplex & req_ability.speed_full_duplex)) {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                  (BSL_META_U(unit,
                              "unit %d Port %s: Speed %uMbps is unsupported, "
                              "Forcing %uMbps\n"),
                   unit, SOC_PORT_NAME(unit, port), speed, max_speed));
            speed = max_speed;
        }
        }

        BCM_IF_ERROR_RETURN(
            bcm_port_autoneg_set(unit, port, FALSE));
       if (IS_CD_PORT(unit, port)) {
           bcm_port_resource_t pr;
           BCM_IF_ERROR_RETURN(bcm_port_resource_speed_get(unit, port,
                                                           &pr));
#ifdef BCM_TOMAHAWK3_SUPPORT
           if (SOC_IS_TOMAHAWK3(unit)) {
               int lanes_match = 1;
               bcm_port_phy_fec_t fec_type = bcmPortPhyFecNone;
               int phy_lane_config;
               int link_training;

               /* We will proceed only if speed specified by the user matches
                * (for a given number of lanes) one of the speeds with equal
                * number of lanes
                */
               switch(pr.lanes) {
                   case 8:
                       if (speed != 400000) {
                           lanes_match = 0;
                       }

                       fec_type = bcmPortPhyFecRs544_2xN;
                       break;
                   case 4:
                       if ((speed != 200000) &&
                           (speed != 100000) &&
                           (speed != 40000)) {
                           lanes_match = 0;
                       }
                       switch (speed) {
                           case 200000:
                               fec_type = bcmPortPhyFecRs544;
                               break;
                           case 100000:
                           case 40000:
                           default:
                               fec_type = bcmPortPhyFecNone;
                               break;
                       }
                       break;
                   case 2:
                       if ((speed != 100000) &&
                           (speed != 50000) &&
                           (speed != 40000)) {
                           lanes_match = 0;
                       }

                       switch (speed) {
                           case 100000:
                           case 50000:
                               fec_type = bcmPortPhyFecRs544;
                               break;
                           case 40000:
                           default:
                               fec_type = bcmPortPhyFecNone;
                               break;
                       }
                       break;
                   case 1:
                       if ((speed != 50000) &&
                           (speed != 25000) &&
                           (speed != 10000)) {
                           lanes_match = 0;
                       }

                       switch (speed) {
                           case 50000:
                               fec_type = bcmPortPhyFecRs544;
                               break;
                           case 25000:
                           case 10000:
                           default:
                               fec_type = bcmPortPhyFecNone;
                               break;
                       }
                       break;

                   default:
                       lanes_match = 0;
                       break;
               }

               phy_lane_config = -1;
               link_training = 0;

                if (!lanes_match) {
                    LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "unit %d"
                                  " Port %s: Speed %uMbps is not supported for %d"
                                  " lane(s)\n"), unit,
                                  SOC_PORT_NAME(unit, port), speed, pr.lanes));
                    return(BCM_E_PARAM);
                }

                pr.fec_type = fec_type;
                pr.phy_lane_config = phy_lane_config;
                pr.link_training = link_training;
           }
#endif /* BCM_TOMAHAWK3_SUPPORT */
           if (pr.speed != speed) {
               LOG_INFO(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                         "port speed (%d) does NOT match "
                         " current speed (%d).\n"),
                         pr.speed, speed));
           }
           pr.speed = speed;
           BCM_IF_ERROR_RETURN(bcm_port_resource_speed_set(unit, port, &pr));
       } else {
        BCM_IF_ERROR_RETURN(
            bcm_port_speed_set(unit, port, speed));
       }
        BCM_IF_ERROR_RETURN(
            bcm_port_duplex_set(unit, port, duplex));
    }

    return (BCM_E_NONE);
}

STATIC int
lbu_port_init(loopback2_test_t *lw, loopback2_testdata_t *lp)
{
    char                 pfmt[SOC_PBMP_FMT_LEN];
    bcm_port_t           p;
    int                  rv, unit = lw->unit;
    int                  rcpu;

    /* Save all the current port states */

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Saving ports\n")));
    if (lbu_save_port(lw, lp) < 0) {
        return (-1);
    }
    
    /* Port setup */
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Port setup, bitmap %s\n"),
              SOC_PBMP_FMT(lp->pbm, pfmt)));
    if (lp->loopback == LB2_MODE_EXT) {
        PBMP_ITER(lp->pbm, p) {
            /* Forwarding, egress enable, epc_link */
            if ((rv = bcm_port_stp_set(unit, p,
                                       BCM_STG_STP_FORWARD)) < 0) {
                test_error(unit,
                           "Unable to set port %s in forwarding state: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                return -1;
            }
        }
    }

    if (lp->loopback == LB2_MODE_MAC) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("MAC loopback at speed %s\n"), 
                  lb2_parse_speed[lp->speed]));

        PBMP_ITER(lp->pbm, p) {
            if (SOC_IS_HAWKEYE(unit)||SOC_IS_GREYHOUND2(unit)) {
                if ((rv = lbu_setup_port(unit, p, lp->speed, FALSE)) < 0) {
                    test_error(unit,
                               "Port %s: Port setup failed: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);
                }
                if ((rv = bcm_port_loopback_set(unit, p, 
                                                BCM_PORT_LOOPBACK_MAC)) < 0) {
                    test_error(unit,
                               "Port %s: Failed to set MAC loopback: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);
                }
            } else {
                /*
                 * Set loopback mode before speed, since it may affect
                 * the allowable speeds.
                 */

                if ((rv = bcm_port_loopback_set(unit, p, 
                                                BCM_PORT_LOOPBACK_MAC)) < 0) {
                    test_error(unit,
                               "Port %s: Failed to set MAC loopback: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);
                }

                if (IS_HG_PORT(unit, p)) {
                    /*
                     *      Disabling pause reuiqred on IPIC/XPORT
                     *      HG ports. Should already be disabled on
                     *      GE (stack) ports during init.
                     */
                    bcm_port_pause_set(unit, p, FALSE, FALSE);
                }

                if ((rv = lbu_setup_port(unit, p, lp->speed, FALSE)) < 0) {
                    test_error(unit,
                               "Port %s: Port setup failed: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);
                }
            }
        }

        if (SOC_IS_XGS12_FABRIC(unit)) {
            pbmp_t               pbm;

            /*
             * If there is an external cable looped back, 5670 will
             * receive packets on ports that may not be in the test
             * and these packets will go to the CPU and interfere.
             */

            BCM_PBMP_ASSIGN(pbm, lp->pbm);
            BCM_PBMP_XOR(pbm, PBMP_PORT_ALL(unit));
            PBMP_ITER(pbm, p) {
                if ((rv = bcm_port_enable_set(unit, p, FALSE)) < 0) {
                    test_error(unit,
                               "Port %s: Failed to set MAC enable: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);
                }
            }
        }
    } else if (lp->loopback == LB2_MODE_PHY) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("PHY loopback at speed %s\n"), 
                  lb2_parse_speed[lp->speed]));

        rcpu = soc_property_get(unit, spn_RCPU_PORT, -1);
        PBMP_ITER(lp->pbm, p) {
            if (rcpu !=0 && rcpu != -1 && SOC_IS_RAVEN(unit)) {
                if ((rv = bcm_port_loopback_set(unit, p,
                                                BCM_PORT_LOOPBACK_PHY)) < 0) {
                    test_error(unit,
                               "Port %s: Failed to set PHY loopback: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);                     /* In case they continue */
                }

                if ((rv = lbu_setup_port(unit, p, lp->speed, FALSE)) < 0) {
                    test_error(unit,
                               "Port %s: Port setup failed: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);
                }

            } else {
                if ((rv = lbu_setup_port(unit, p, lp->speed, FALSE)) < 0) {
                    test_error(unit,
                               "Port %s: Port setup failed: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);
                }

                if ((rv = bcm_port_loopback_set(unit, p,
                                                BCM_PORT_LOOPBACK_PHY)) < 0) {
                    test_error(unit,
                               "Port %s: Failed to set PHY loopback: %s\n",
                               SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return(-1);                     /* In case they continue */
                }
            }
        }
    } else { /* External */

        PBMP_ITER(lp->pbm, p) {
            if ((rv = lbu_setup_port(unit, p, lp->speed, FALSE)) < 0) {
                test_error(unit,
                           "Port %s: Port setup failed: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                return(-1);
            }
        }
    }
    if(SOC_IS_ESW(unit)){       
        if (lp->test_type == LB2_TT_SNAKE) {
            PBMP_ITER(lp->pbm, p) {
                if ((rv = bcm_port_learn_set(unit, p, BCM_PORT_LEARN_FWD)) < 0) {
                    test_error(unit,
                           "Unable to set learn state: port %s: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return (-1);
                }
            }
        }
    }
#ifdef BCM_TRIUMPH_SUPPORT
    if(SOC_IS_TRX(unit)){       
        PBMP_ITER(lp->pbm, p) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (SOC_MEM_IS_VALID(unit, EGR_VLAN_CONTROL_1m)) {
                if (soc_feature(unit, soc_feature_egr_all_profile)) {
                    BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit,
                        p, EGR_VLAN_CONTROL_1m, REMARK_OUTER_DOT1Pf, 0));
                } else {
                    egr_vlan_control_1_entry_t entry;
                    BCM_IF_ERROR_RETURN
                        (READ_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, p, &entry));
                    soc_EGR_VLAN_CONTROL_1m_field32_set(unit, &entry,
                                                        REMARK_OUTER_DOT1Pf, 0);
                    BCM_IF_ERROR_RETURN
                        (WRITE_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, p, &entry));
                }
            } else
#endif
            if ((rv = soc_reg_field32_modify(unit, EGR_VLAN_CONTROL_1r, p,
                                             REMARK_OUTER_DOT1Pf, 0x0)) < 0) {
                test_error(unit,
                       "Unable to disable outer dot1p remarking: port %s: %s\n",
                       SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                return (-1);
            }
        }
    }
#endif
    {
        bcm_pbmp_t lw_pbm;
        BCM_PBMP_ASSIGN(lw_pbm, lp->pbm);
        rv = bcm_link_wait(unit, &lw_pbm, 1000000);
        if (rv < 0) {
            return -1;
        }
    }


    return 0;
}

STATIC int
lbu_vlan_init(loopback2_test_t *lw, loopback2_testdata_t *lp)
{
    int                 rv, unit = lw->unit;
    pbmp_t              empty_pbm, vlan_pbm;
    bcm_port_t          p;
    bcm_vlan_t          vlan;

    if (!lp->inject) {
        if ((rv = bcm_vlan_destroy_all(unit)) < 0) {
            test_error(unit,
                       "Could not destroy existing VLANs: %s\n",
                       bcm_errmsg(rv));
            return (-1);
        }
    }

    if (lp->test_type == LB2_TT_SNAKE) {
        PBMP_ITER(lp->pbm, p) {
            /* Each port in all VLANs for test */
            vlan = lp->vlan + p + 1;
            if ( ((rv = bcm_vlan_create(unit, vlan)) < 0) &&
                 (rv != BCM_E_EXISTS) ) {
                test_error(unit,
                           "Could not create VLAN %d: %s\n",
                           vlan, bcm_errmsg(rv));
                return (-1);
            }
        
            /* Untagged! */
            BCM_PBMP_ASSIGN(vlan_pbm, lp->pbm);
            BCM_PBMP_PORT_ADD(vlan_pbm, CMIC_PORT(unit));
            if ((rv = bcm_vlan_port_add(unit, vlan, vlan_pbm, vlan_pbm)) < 0) {
                test_error(unit,
                           "Could not add test ports to VLAN %d: %s\n",
                           vlan, bcm_errmsg(rv));
                return (-1);
            }

            /* Set port to tag with vlan */
            if (IS_E_PORT(unit, p)) {
                if ((rv = bcm_port_untagged_vlan_set(unit, p, vlan)) < 0) {
                    test_error(unit,
                               "Could not set port %s to "
                               "tag packet with VLAN %d: %s\n",
                               SOC_PORT_NAME(unit, p), vlan, bcm_errmsg(rv));
                    return (-1);
                }
            }
        }
    } else {
        if ((rv = bcm_vlan_create(unit, lp->vlan)) < 0 &&
            rv != BCM_E_EXISTS) {
            test_error(unit,
                       "Could not create VLAN %d: %s\n",
                       lp->vlan, bcm_errmsg(rv));
            return (-1);
        }

        SOC_PBMP_CLEAR(empty_pbm);
        SOC_PBMP_ASSIGN(vlan_pbm, lp->pbm);
        SOC_PBMP_PORT_ADD(vlan_pbm, CMIC_PORT(unit));
        if ((rv = bcm_vlan_port_add(unit, lp->vlan, vlan_pbm,
                                    empty_pbm)) < 0) {
            test_error(unit,
                       "Could not add selected ports to VLAN %d: %s\n",
                       lp->vlan, bcm_errmsg(rv));
            return (-1);
        }

        SOC_PBMP_ASSIGN(vlan_pbm, lp->pbm);
        SOC_PBMP_AND(vlan_pbm, PBMP_E_ALL(unit));
        PBMP_ITER(vlan_pbm, p) {
            if ((rv = bcm_port_untagged_vlan_set(unit, p, lp->vlan)) < 0) {
                test_error(unit,
                     "Could not set port %s to tag packet with VLAN %d: %s\n",
                           SOC_PORT_NAME(unit, p), lp->vlan, bcm_errmsg(rv));
                return (-1);
            }
        }
    }

    return 0;
}

STATIC int
lbu_stack_check(int unit)
{
    if (SOC_SL_MODE(unit)) {
        return -1;
    }
    return 0;
}

int
lbu_init(loopback2_test_t *lw, loopback2_testdata_t *lp)
{
    int                 unit = lw->unit;
    uint32              flags = BCM_TX_NO_PAD;
    
    if (NULL == (lw->sema = sal_sem_create("lb2-sema", sal_sem_BINARY, 0))) {
        return (-1);
    }

    lw->sema_woke = FALSE;
    lw->timeout_usec = lbu_timeout_usec(unit);

    lw->tx_seq = 0;
    lw->tx_len = 0;

    /* Don't want any stack ports lying about */
    if (lbu_stack_check(unit) < 0) {
        return (-1);
    }

    if (lbu_vlan_init(lw, lp) < 0) {
        return (-1);
    }

    /* Initialize ports */
    if (lbu_port_init(lw, lp) < 0) {
        return (-1);
    }

    /* Allocate the tx packets */
    /* Set up TX pkts once, for max length */
    if (lp->crc_mode == LB2_CRC_MODE_MAC_REGEN) {
        flags |= BCM_TX_CRC_REGEN;
    } else if (lp->crc_mode == LB2_CRC_MODE_CPU_APPEND) {
        flags |= BCM_TX_CRC_APPEND;
    } /* else, do not add a flag */
    bcm_pkt_blk_alloc(unit, lp->ppt_end, lp->len_end, flags, &(lw->tx_pkts));
    if (!lw->tx_pkts) {
        test_error(unit, "Unable to allocate tx packet memory\n");
        return -1;
    }
    lw->rx_pkts = sal_alloc(lp->ppt_end * sizeof(bcm_pkt_t), "lb2 rx pkts");
    if (!lw->rx_pkts) {
        bcm_pkt_blk_free(unit, lw->tx_pkts, lp->ppt_end);
        test_error(unit, "Unable to allocate rx packet memory\n");
        return -1;
    }
    sal_memset(lw->rx_pkts, 0, lp->ppt_end * sizeof(bcm_pkt_t));

    /* Now that all mems are safely created, fire off the handler */
    if (!lp->inject) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Port monitor init\n")));
        /* Let rx handler ignore packets until testing */
        lw->expect_pkts = FALSE; 
        lw->rx_pkt_cnt = 0;
        if (lbu_port_monitor_task(unit, lbu_rx_callback, lw) < 0) {
            return (-1);
        }
    }

    lw->cur_params = lp;

    return 0;
}

int
lbu_snake_init(loopback2_test_t *lw, loopback2_testdata_t *lp)
/*
 * Function:    lbu_snake_init
 * Purpose:     Link adjacent ports to forward packets.
 * Parameters:  lw - pointer to current work structure.
 * Returns:     0 - OK
 *              -1 - failed.
 */
{
    int         unit = lw->unit;
    int         snake_way = lp->snake_way;
    bcm_port_t  port, first_port, last_port;
    int         forward, reverse;
    lb2s_port_connect_t *work_info, *f_info, *r_info;
    int         modid = 0, modid_hi = 0, ipic_my_modid, ipic_my_modid_hi;
    int         rv, pix; /* port index */
    int         port_count, install_count;
    int         close_loop = (lp->duration != 0);
#ifdef BCM_TOMAHAWK_SUPPORT
    int         line_rate = lp->line_rate;
    int         speed_linerate = 0, speed = 0;
#endif
    /* Get the module ID for this unit */
    if (SOC_IS_XGS_SWITCH(unit)) {
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &modid));
    }
#ifdef BCM_ESW_SUPPORT
    /* Why does this make the snake work? */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_ism_memory)) {
        
        uint8 banks[_SOC_ISM_MAX_BANKS], count, bank_ix;
        uint32 bank_size[_SOC_ISM_MAX_BANKS];
        rv = soc_ism_get_banks_for_mem(unit, L2_ENTRY_1m,
                                       banks, bank_size, &count);
        if (rv  < 0) {
            test_error(unit, "ISM L2 bank enumeration retrieval failed\n");
            return rv;
        }
        
        for (bank_ix = 0; bank_ix < count; bank_ix++) {
            /* Offset 0 is CRC32 low */
            rv = soc_ism_hash_offset_config(unit, banks[bank_ix], 0);
            if (rv  < 0) {
                test_error(unit, "ISM L2 bank %d hash offset set failure\n",
                           banks[bank_ix]);
                return rv;
            }
        }
    } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
    if (SOC_IS_XGS3_SWITCH(unit) && !(SOC_IS_TOMAHAWK3(unit))) {
#ifdef BCM_XGS3_SWITCH_SUPPORT
        uint32  hc;
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Hash select reset to CRC32 low\n")));
        rv = READ_HASH_CONTROLr(unit, &hc);
        if (rv >= 0) {
            soc_reg_field_set(unit, HASH_CONTROLr, &hc,
                              L2_AND_VLAN_MAC_HASH_SELECTf,
                              FB_HASH_CRC32_LOWER);
            rv = WRITE_HASH_CONTROLr(unit, hc);
        }
        if (rv  < 0) {
            test_error(unit, "Hash select setting failed\n");
            return rv;
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    } else if (SOC_IS_XGS_SWITCH(unit)) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Hash select reset to CRC32 low\n")));
        if ((rv = WRITE_HASH_CONTROLr(unit, XGS_HASH_CRC32_LOWER)) < 0) {
            test_error(unit, "Hash select setting failed\n");
            return rv;
        }
    }
#endif

    ipic_my_modid = lp->d_mod + lp->d_mod_inc;
    if ((ipic_my_modid == (lp->d_mod == 0)) || (ipic_my_modid == modid)) {
        ipic_my_modid += lp->d_mod_inc;
    }
    if ((ipic_my_modid == (lp->d_mod == 0)) || (ipic_my_modid == modid)) {
        ipic_my_modid += lp->d_mod_inc;
    }

    ipic_my_modid_hi = ipic_my_modid;

    forward = (snake_way == LB2_SNAKE_BOTH) || (snake_way == LB2_SNAKE_INCR);
    reverse = (snake_way == LB2_SNAKE_BOTH) || (snake_way == LB2_SNAKE_DECR);

    first_port = last_port = -1;
    PBMP_ITER(lp->pbm, port) {
        last_port = port;
        if (first_port < 0) {
            first_port = port;
        }
    }

    if (first_port < 0) {
        return BCM_E_NOT_FOUND;
    }

    BCM_PBMP_COUNT(lp->pbm, port_count);
    lw->port_count = port_count;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (line_rate) {
        PBMP_ITER(lp->pbm, port) {
            BCM_IF_ERROR_RETURN(bcm_port_speed_get(unit,port,&speed));
            if (speed_linerate && speed && (speed != speed_linerate)){
                cli_out("line rate snake test requires all tested ports have same speed\n");
                return BCM_E_PORT;
            }
            speed_linerate = speed;
        }
        /*For each one 10G port, we send 10 packets to achieve line rate in snake test.
         *This is not a precise value but should be enough.
         */
        lp->line_rate_count= speed_linerate / 1000 * lw->port_count;
    }
#endif

    if (forward) {
        if (!(lw->port_cnct_f = 
              sal_alloc((port_count + 1) * sizeof(lb2s_port_connect_t), 
                        "Forward snake port info"))) {
            return BCM_E_MEMORY;
        }
        sal_memset(lw->port_cnct_f, 0, (port_count + 1)* 
                   sizeof(lb2s_port_connect_t));
    } else {
        lw->port_cnct_f = NULL;
    }

    if (reverse) {
        if (!(lw->port_cnct_r = 
              sal_alloc((port_count + 1) * sizeof(lb2s_port_connect_t), 
                        "Reverse snake port info"))) {
            return BCM_E_MEMORY;
        }
        sal_memset(lw->port_cnct_r, 0, (port_count + 1) * 
                   sizeof(lb2s_port_connect_t));
    } else {
        lw->port_cnct_r = NULL;
    }

    if (!(lw->tx_pkt_match = sal_alloc(lp->ppt_end * sizeof(int), 
                                       "Snake TX match markers"))) {
        return BCM_E_MEMORY;
    }
    sal_memset(lw->tx_pkt_match, 0, lp->ppt_end * sizeof(int));
 
    f_info = lw->port_cnct_f;
    r_info = lw->port_cnct_r;
    work_info = forward ? f_info : r_info;

    work_info->this_port = work_info->to_port = first_port;
    work_info->added_vlan = lp->vlan + first_port + 1;
    ENET_SET_MACADDR(work_info->src_mac, lp->mac_src);
    ENET_SET_MACADDR(work_info->dst_mac, lp->mac_dst);

    if (!IS_E_PORT(unit, first_port)) {
        work_info->src_modid = ipic_my_modid_hi + lp->d_mod_inc;
    } else {
        work_info->src_modid = 
            SOC_PORT_MOD1(unit, first_port) ? modid_hi : modid;
    }
    work_info->dst_modid = work_info->src_modid;

    if (snake_way == LB2_SNAKE_BOTH) {
        r_info->to_port = f_info->this_port;
        r_info->dst_modid = f_info->src_modid;
        ENET_SET_MACADDR(r_info->src_mac, f_info->src_mac);
        increment_macaddr(r_info->src_mac, lp->mac_src_inc);
        ENET_SET_MACADDR(r_info->dst_mac, f_info->dst_mac);
        increment_macaddr(r_info->dst_mac, lp->mac_dst_inc);
    }

    if (snake_way == LB2_SNAKE_BOTH || snake_way == LB2_SNAKE_DECR) {
        if (SOC_IS_XGS12_FABRIC(unit) || 
            (SOC_IS_HBX(unit) && IS_ST_PORT(unit, first_port))) {
            r_info->dst_modid += lp->d_mod_inc;
        }
    }

    
     
    /* Generate the port connection info */
    pix = 0;
    PBMP_ITER(lp->pbm, port) {
        if (pix > 0) {
            if (forward) {
                f_info[pix].this_port = f_info[pix - 1].to_port = port;

                if (IS_ST_PORT(unit, port) && SOC_IS_XGS_SWITCH(unit)) {
                    f_info[pix].added_vlan = f_info[pix - 1].added_vlan;
                    f_info[pix].src_modid = 
                        f_info[pix - 1].dst_modid = 
                        ipic_my_modid_hi + lp->d_mod_inc;
                } else {
                    f_info[pix].added_vlan = lp->vlan + port + 1;
                    if (IS_E_PORT(unit, port)) {
                        f_info[pix].src_modid = 
                            f_info[pix - 1].dst_modid = 
                            SOC_PORT_MOD1(unit, port) ? 
                            modid_hi : modid;
                    } else {
                        f_info[pix].src_modid = 
                            f_info[pix - 1].dst_modid = 
                            f_info[pix - 1].src_modid;
                    }

                }
                ENET_SET_MACADDR(f_info[pix].src_mac, f_info[pix - 1].src_mac);
                ENET_SET_MACADDR(f_info[pix].dst_mac, f_info[pix - 1].dst_mac);

                f_info[pix - 1].valid_info = TRUE;
            }

            if (reverse) {
                r_info[pix].to_port = r_info[pix - 1].this_port = port;
                if (!IS_E_PORT(unit, port) && SOC_IS_XGS_SWITCH(unit)) {
                    r_info[pix - 1].added_vlan = r_info[pix - 2].added_vlan;
                    r_info[pix].dst_modid = 
                        r_info[pix - 1].src_modid = 
                        ipic_my_modid_hi + (2 * lp->d_mod_inc);
                } else {
                    r_info[pix - 1].added_vlan = lp->vlan + port + 1;
                    if (IS_E_PORT(unit, port)) {
                        r_info[pix].dst_modid = 
                            r_info[pix - 1].src_modid = 
                            SOC_PORT_MOD1(unit, port) ? 
                            modid_hi : modid;
                    } else {
                        r_info[pix].dst_modid = 
                            r_info[pix - 1].src_modid = 
                            r_info[pix - 1].dst_modid;
                    }
                }
                ENET_SET_MACADDR(r_info[pix].src_mac, r_info[pix - 1].src_mac);
                ENET_SET_MACADDR(r_info[pix].dst_mac, r_info[pix - 1].dst_mac);
                r_info[pix - 1].valid_info = TRUE;
            }
        }

#ifdef BCM_ESW_SUPPORT
        if (IS_ST_PORT(unit, port) && SOC_IS_XGS_SWITCH(unit)) {
            if (SOC_IS_FBX(unit)) {
#ifdef BCM_FIREBOLT_SUPPORT
                port_tab_entry_t pte;
                int     hg_port;
                uint32  config = 0;

                
                if (soc_mem_field_valid(unit, PORT_TABm, MY_MODIDf)) {
                    PBMP_ST_ITER(unit, hg_port) {
                        SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                                MEM_BLOCK_ANY, hg_port, &pte));
                        soc_PORT_TABm_field32_set(unit, &pte, MY_MODIDf,
                                                      ipic_my_modid);
                        SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm,
                                                MEM_BLOCK_ALL, hg_port, &pte));
                    }
                } else if (soc_reg_field_valid(unit, XPORT_CONFIGr, MY_MODIDf)) {
                    PBMP_ST_ITER(unit, hg_port) {
                        if (SOC_REG_PORT_VALID(unit, XPORT_CONFIGr, hg_port )) {
                            BCM_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, hg_port,
                                                    &config));
                            soc_reg_field_set(unit, XPORT_CONFIGr, &config,
                                                  MY_MODIDf, ipic_my_modid);
                            BCM_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit,
                                                    hg_port, config));
                        }
                    }
                }
#endif /* BCM_FIREBOLT_SUPPORT */
            }
        }
#endif /* BCM_ESW_SUPPORT */
        pix++;
    }

    assert(pix == port_count);
    if (forward) {
        /* Close loop */
        f_info[pix - 1].to_port = f_info[0].this_port;
        f_info[pix - 1].dst_modid = f_info[0].src_modid;
        f_info[pix - 1].valid_info = close_loop;

        /* Record CPU redirect */
        f_info[pix].this_port = f_info[pix - 1].this_port;
        f_info[pix].added_vlan = f_info[pix - 1].added_vlan;
        f_info[pix].src_modid = f_info[pix - 1].src_modid;
        ENET_SET_MACADDR(f_info[pix].src_mac, f_info[pix - 1].src_mac);
        ENET_SET_MACADDR(f_info[pix].dst_mac, f_info[pix - 1].dst_mac);
        f_info[pix].to_port = CMIC_PORT(unit);
        f_info[pix].dst_modid = modid;
        f_info[pix].valid_info = TRUE;
    }
    if (reverse) {
        /* Close loop */
        r_info[pix - 1].this_port = r_info[0].to_port;
        r_info[pix - 1].added_vlan = 
            lp->vlan + r_info[pix - 1].this_port + 1;
        r_info[pix - 1].src_modid = r_info[0].dst_modid;
        r_info[pix - 1].valid_info = close_loop;

        /* Record CPU redirect */
        r_info[pix].this_port = r_info[pix - 1].this_port;
        r_info[pix].added_vlan = r_info[pix - 1].added_vlan;
        r_info[pix].src_modid = r_info[pix - 1].src_modid;
        ENET_SET_MACADDR(r_info[pix].src_mac, r_info[pix - 1].src_mac);
        ENET_SET_MACADDR(r_info[pix].dst_mac, r_info[pix - 1].dst_mac);
        r_info[pix].to_port = CMIC_PORT(unit);
        r_info[pix].dst_modid = modid;
        r_info[pix].valid_info = TRUE;
    }

    /* If not closing loop, install the CPU return path also. */
    install_count = port_count + (close_loop ? 0 : 1);

    for (pix = 0; pix < install_count; pix++) {
        if (forward) {
            if ((rv = lbu_connect_ports(unit, &f_info[pix], TRUE)) < 0 ) {
                return rv;
            }
        }
        if (reverse) {
            if ((rv = lbu_connect_ports(unit, &r_info[pix], TRUE)) < 0 ) {
                return rv;
            }
        }
    }

    return 0;
}

int
lbu_snake_done(loopback2_test_t *lw)
/*
 * Function:    lbu_snake_done
 * Purpose:     Link adjacent ports to forward packets.
 * Parameters:  lw - pointer to current work structure.
 * Returns:     0 - OK
 *              -1 - failed.
 */
{
    int         unit = lw->unit;
    int         pix, rv;
    lb2s_port_connect_t *f_info, *r_info;

    f_info = lw->port_cnct_f;
    r_info = lw->port_cnct_r;
    for (pix = 0; pix < lw->port_count; pix++) {
        if (f_info) {
            if ((rv = lbu_connect_ports(unit, &f_info[pix], FALSE)) < 0 ) {
                return rv;
            }
        }
        if (r_info) {
            if ((rv = lbu_connect_ports(unit, &r_info[pix], FALSE)) < 0 ) {
                return rv;
            }
        }
    }
    
    if (lw->port_cnct_f) {
        sal_free(lw->port_cnct_f);
    }
    lw->port_cnct_f = NULL;
    if (lw->port_cnct_r) {
        sal_free(lw->port_cnct_r);
    }
    lw->port_cnct_r = NULL;
    if (lw->tx_pkt_match) {
        sal_free(lw->tx_pkt_match);
    }
    lw->tx_pkt_match = NULL;

    return 0;
}


/* These values are used to keep the number of packets per test within the
 * limits that the MMU COS settings can handle. */
static int _lb_ppte_initval(int unit, int *lene, int *ppte)
{
    int max_cells, num_cells, num_ports, num_cos, mmu_cell_size;
    int len = *lene;
#if defined(BCM_HERCULES_SUPPORT)
    int port = 0, cos = 0;
    uint32 num_cells_per_port_cos = 0;
#endif
    int ppte_max = SOC_DV_PKTS_MAX;
    

    *ppte = ppte_max;

    if (SOC_IS_XGS_SWITCH(unit) || SOC_IS_XGS12_FABRIC(unit)) {
        /* Check that we don't overload the MMU HOL cell limits */
        /*  (This should be from #defines somewhere else)  */
        mmu_cell_size = 128;
        BCM_IF_ERROR_RETURN(bcm_cosq_config_get(unit, &num_cos));
        max_cells = (len + mmu_cell_size - 1) / mmu_cell_size;
#if defined(BCM_FIREBOLT_SUPPORT)
        if (SOC_IS_FBX(unit) && !SOC_IS_KATANAX(unit)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                num_cells = _TH3_MMU_TOTAL_CELLS_PER_ITM;
            } else
#endif
            {
            num_cells = soc_mem_index_count(unit, MMU_CBPDATA0m);
            }
        } else
#endif /* BCM_FIREBOLT_SUPPORT */        
        {
            num_cells = 8192;
        }
        num_ports = NUM_ALL_PORT(unit);
        *ppte = (num_cells / (num_cos * num_ports * max_cells)) - 1;

#if defined(BCM_HERCULES_SUPPORT)
        if (SOC_IS_HERCULES(unit)) {
            /* look at what is configured for any port/cos 0,0 */
            SOC_IF_ERROR_RETURN(READ_MMU_CELLLMTCOS_LOWERr(unit, port, cos, 
                                                           &num_cells_per_port_cos));
            /* overwrite ppte */
            *ppte = (num_cells_per_port_cos / max_cells) - 1; 
        }
#endif /* BCM_HERCULES_SUPPORT */

        if (*ppte < 1) {
            *ppte = 1;
        }
        if (*ppte > ppte_max) {
            *ppte = ppte_max;
        }
        /*
         * Number of cells allocated to this port 
         * Assume DYNCELL_PERCENT 50
         */
        len = num_cells / num_ports / 2;

        /*
         * Number of cells usable by a cos queue
         */
        len += (len / num_cos);
        /*
         * Max packet size supported
         */
        len *= mmu_cell_size;
        if (*lene > len) {
            *lene = len;
        }
    }
    return(SOC_E_NONE);
}

int
lbu_check_parms(loopback2_test_t *lw, loopback2_testdata_t *lp)
/*
 * Function:    lbu_check_parms
 * Purpose:     Check parameters for reasonable values.
 * Parameters:  lw - pointer to current work structure.
 * Returns:     0 - OK
 *              -1 - failed.
 */
{
    int         nLen, nCOS, nPort;
    int         unit = lw->unit;
    pbmp_t      pbm;
    char        pfmt[SOC_PBMP_FMT_LEN];
    int         max_pkt;
    int         max_adj = 0;
    int         jumbo_adj = 0;
    int         num_ports, num_cos;
    int         ppt_max = SOC_DV_PKTS_MAX;
#ifdef BCM_ESW_SUPPORT
    int         max_set;
    bcm_port_t  port;
#endif
    int                p;
    bcm_port_abil_t    ability;

    if (lp->len_start < 64) {
        test_error(unit, "Packet start length %d too small\n", lp->len_start);
        return(-1);
    }

    /*
     * Adjust FE_MAXFr, MAXFRr, and GTH_MAXFr as necessary, to
     * accommodate requested packet size.
     */
    max_pkt = lp->len_start;

    if (max_pkt < lp->len_end) {
        max_pkt = lp->len_end;
    }

    if (max_pkt > LB_JUMBO_MAXSZ) {
        max_pkt = LB_JUMBO_MAXSZ;
    }

    if (lp->ppt_start > ppt_max || lp->ppt_end > ppt_max) {
        test_error(unit, "Too many packets per trial (%d, %d).  Max is %d.\n",
               lp->ppt_start, lp->ppt_end, ppt_max);
        lp->ppt_end = lp->ppt_start = ppt_max;
        return -1;
    }
#ifdef BCM_ESW_SUPPORT
    {
        BCM_PBMP_ASSIGN(pbm, PBMP_E_ALL(unit));
        BCM_PBMP_AND(pbm, lp->pbm);
        BCM_PBMP_ITER(pbm, port) {
            BCM_IF_ERROR_RETURN
                (bcm_port_frame_max_get(unit, port, &max_set));
            if (max_set < max_pkt) {
                BCM_IF_ERROR_RETURN
                    (bcm_port_frame_max_set(unit, port, max_pkt));
                max_adj = 1;
            }
        }
    
        BCM_PBMP_ASSIGN(pbm, PBMP_GE_ALL(unit));
        BCM_PBMP_AND(pbm, lp->pbm);
        PBMP_ITER(pbm, port) {
            if (max_pkt > 1536) {
                if (soc_feature(unit, soc_feature_trimac)) {
                   uint32              gmacc1;
 
                   SOC_IF_ERROR_RETURN
                       (READ_GMACC1r(unit, port, &gmacc1));
 
                   if (! soc_reg_field_get(unit, GMACC1r, gmacc1, JUMBOf)) {
                       soc_reg_field_set(unit, GMACC1r, &gmacc1, JUMBOf, 1);
                        SOC_IF_ERROR_RETURN
                            (WRITE_GMACC1r(unit, port, gmacc1));
                        jumbo_adj = 1;
                    }
                }
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
                else if (soc_feature(unit, soc_feature_unimac)) {
                    SOC_IF_ERROR_RETURN(WRITE_FRM_LENGTHr(unit, port, LB_JUMBO_MAXSZ));
                }
#endif
            }
        }
    }
#endif

    if (max_adj) {
        cli_out("NOTICE: Increased max frame size for %d-byte packets\n",
                max_pkt);
    }

    if (jumbo_adj) {
        cli_out("NOTICE: Enabled JUMBO frames on GE\n");
    }

    if (lp->len_inc <= 0) {
        test_error(unit, "Increments must be > 0\n");
        return(-1);
    }

    if (SOC_IS_XGS(unit) && (lp->test_type == LB2_TT_SNAKE)) {
        if (lp->d_mod_inc <= 0) {
            test_error(unit, "Module increments must be > 0\n");
            return(-1);
        }
    }

    BCM_PBMP_ASSIGN(pbm, lp->pbm);
    BCM_PBMP_REMOVE(pbm, PBMP_PORT_ALL(unit));
    if (BCM_PBMP_NOT_NULL(pbm)) {
        test_error(unit,
                   "Invalid bits in port bitmap.\n"
                   "Allowable ports are %s\n",
                   SOC_PBMP_FMT(PBMP_PORT_ALL(unit), pfmt));
        return(-1);
    }

    if (lp->cos_start < 0 ||
        lp->cos_end < lp->cos_start ||
        lp->cos_end >= NUM_COS(unit)) {
        test_error(unit,
                   "Invalid COS values: start=%d end=%d\n",
                   lp->cos_start, lp->cos_end);
        return(-1);
    }

    if (lp->check_crc) {
        if (lp->crc_mode == LB2_CRC_MODE_CPU_NONE) {
            test_error(unit, "Can't check CRC if not generating it\n");
            return(-1);
        }
        if (lp->loopback == LB2_MODE_MAC) { 
            /* CRC not generated in MAC loopback */
            test_error(unit, "Can't check CRC (not generated in MAC loopback mode)\n");
            return(-1);
        }
    }

    nLen = (lp->len_end - lp->len_start) / lp->len_inc + 1;
    nCOS = (lp->cos_end - lp->cos_start) / 1 + 1;
    BCM_PBMP_COUNT(lp->pbm, nPort);

    if (lp->test_type == LB2_TT_EXT) {
        assert(nPort % 2 == 0);
        nPort /= 2;
    }

    BCM_PBMP_ASSIGN(pbm, lp->pbm);
    BCM_PBMP_AND(pbm, PBMP_HG_ALL(unit));

    /*
     *  Packet accumulaion and discard possible due to speed mismatch.
     *  Check if MMU bufering is capable of absorbing this.
     */
    if (SOC_IS_XGS_SWITCH(unit) && 
        ((lp->speed == LB_SPEED_10FD) ||
         (SOC_IS_RAPTOR(unit)) ||
         BCM_PBMP_NOT_NULL(pbm))) {
        int ppte;
        int lene = lp->len_end;
        /* Check that we don't overload the MMU HOL cell limits */
        BCM_IF_ERROR_RETURN(bcm_cosq_config_get(unit, &num_cos));
        num_ports = NUM_ALL_PORT(unit);
        SOC_IF_ERROR_RETURN(_lb_ppte_initval(unit, &lene, &ppte));
        if (lp->len_end > lene) {
            test_error(unit,
                   "Packet length (%d) too Large. Try (%d)\n"
                   "\twith current configuration of %d COS, %d ports.\n",
                   lp->len_end, lene, num_cos, num_ports);
            return (-1);
        }
        if ((lp->ppt_end > ppte) &&
            ((lp->speed == LB_SPEED_100FD) ||
             (lp->speed == LB_SPEED_10FD))) {
            test_error(unit,
                   "Too many packets per chain (%d) for maximum length: %d\n"
                    "\twith current configuration of %d COS, %d ports.\n",
                   lp->ppt_end, lp->len_end, num_cos, num_ports);
            return (-1);
        }
    }

    if (lp->test_type == LB2_TT_SNAKE) {
        int icnt, pkt;
        lw->tx_total = 0;
        pkt = lp->ppt_start;
        for (icnt = 0; icnt < lp->iterations; icnt++ ) {
            lw->tx_total += pkt;

            if ( lp->ppt_end != lp->ppt_start ) {
                pkt += lp->ppt_inc;
                while ( pkt > lp->ppt_end ) {
                    pkt -= lp->ppt_end - lp->ppt_start;
                }
            }
        }
        test_msg("LB: total %d pkt over %d runs\n",
                 lw->tx_total, lp->iterations);
    } else {
        int ppt, nPkt = 0;

        /* Count up total number of pkts per run */
        for (ppt = lp->ppt_start; ppt <= lp->ppt_end; ppt += lp->ppt_inc) {
            nPkt += ppt;
        }
        lw->tx_total = nLen * nCOS * nPort * nPkt * lp->iterations;
        test_msg("LB: total %d pkt (%d len "
                 "* %d COS * %d port * %d packets * %d count)\n",
                 lw->tx_total,
                 nLen, nCOS, nPort, nPkt, lp->iterations);
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (!SOC_IS_TOMAHAWKX(unit) && lp->line_rate){
        test_error(unit,
                   "Line rate snake test is only supported on TH/TH2\n");
        lp->line_rate = 0;
        lp->line_rate_count = 1;
        return (-1);
    }
    if (SOC_IS_TOMAHAWKX(unit) && lp->line_rate && SOC_INFO(unit).mmu_lossless){
        test_error(unit, "Line rate snake test is not supported when mmu_lossless = 1\n");
        lp->line_rate = 0;
        lp->line_rate_count = 1;
        return (-1);
    }
#endif

    if (lp->loopback == LB2_MODE_PHY) {
        PBMP_ITER(lp->pbm, p) {
            if (!IS_CD_PORT(unit, p)) {
                if ( bcm_port_ability_get(unit, p, &ability) != BCM_E_NONE) {
                    test_error(unit,"======> ERROR : fail to get ability Port %s\n", SOC_PORT_NAME(unit, p));
                }
                if (!(ability & BCM_PORT_ABIL_LB_PHY)) {
                    SOC_PBMP_PORT_REMOVE(lp->pbm, p);
                }
            }
        }
    }

    return(0);
}

/*
 * Loopback Statistics Report
 */

void
lbu_stats_init(loopback2_test_t *lw)
{
    lw->tx_count = 0;
    lw->rx_count = 0;

    lw->tx_bytes = 0;
    lw->rx_bytes = 0;

    lw->tx_stime = sal_time();
    lw->tx_rtime = lw->tx_stime + sh_set_report_time;

    lw->stats_init = TRUE;
}

STATIC void
lbu_stats_report(loopback2_test_t *lw)
{
    sal_time_t          now;
    int                 secs;

    if (!sh_set_report_status) {
        return;
    }

    /*
     * Feel free to call this routine after every packet sent.
     * It will only report stats every "sh_set_report_time" seconds.
     */

    now = sal_time();

    if ((secs = (now - lw->tx_stime)) == 0) {
        secs = 1;
    }

    if (now >= lw->tx_rtime) {
        test_msg("LB: xmit %d pkt (%d%%, %d pkt/s, %d KB/s avg), "
                 "recv %d pkt (%d sec)\n", lw->tx_count,
                 100 * lw->rx_count / lw->tx_total,
                 lw->tx_count / secs,
                 lw->tx_bytes / 1024 / secs,
                 lw->rx_count, secs);

        lw->tx_rtime += sh_set_report_time;
    }
}

void
lbu_stats_done(loopback2_test_t *lw)
{
    if (lw->stats_init && lw->tx_count > 0) {
        lw->tx_rtime = 0;               /* Force output now */
        lbu_stats_report(lw);
    }
}


/*
 * Print bytes as %d,%2.2fK, %2.2fG, etc.
 */
static void
snake_show_number(uint64 size)
{
    uint32 value;
    char *s;

    s = _shr_scale_uint64(size, 1024, 100, &value);
    if (s) {
        cli_out("%d.%02d%s", value / 100, value % 100, s);
    } else {
        value = COMPILER_64_LO(size);
        cli_out("%d", value);
    }
}

/*
 * Print rate as %d,%2.2fG, etc.
 */
static void
snake_show_rate(uint32 size)
{
    uint64 size64;
    uint32 value;
    char *s;

    COMPILER_64_SET(size64, 0, size);
    s = _shr_scale_uint64(size64, 1024, 100, &value);
    if (s) {
        cli_out("%d.%02d%s", value / 100, value % 100, s);
    } else {
        cli_out("%d", size);
    }
}

int
lbu_snake_stats(int unit, bcm_pbmp_t sel_pbm, int lb_mode, 
                lb2_port_stat_t *stats)
/*
 * Function:    lbu_snake_stats
 * Purpose:     Check error/pkt stats during run to see if packets still
 *              running smoothly.  Notify if not.
 *              Most of this routine is blantanly stolen from the
 *              traffic test.
 * Parameters:  unit - device number.
 *              sel_pbm - port bitmap of selected ports.
 *              mac_lb - (BOOL) TRUE if MAC loopback mode.
 *              stats - pointer to stats records.
 * Returns:     0 - success, -1 error detected.
 */
{
    int             port;
    int             port_error, error = 0;
    int             stopped;
    pbmp_t          pbm;
    uint32          tolerance;

    uint64 rbyt, tbyt, rpkt, tpkt, rerr,raln, rdisc, dropped;
    uint64 drbyt, dtbyt, drpkt, dtpkt, drerr, draln, drdisc, ddropped;

    COMPILER_64_ZERO(rbyt);    COMPILER_REFERENCE(&rbyt);
    COMPILER_64_ZERO(tbyt);    COMPILER_REFERENCE(&tbyt);   
    COMPILER_64_ZERO(rpkt);    COMPILER_REFERENCE(&rpkt);   
    COMPILER_64_ZERO(tpkt);    COMPILER_REFERENCE(&tpkt);   
    COMPILER_64_ZERO(rerr);    COMPILER_REFERENCE(&rerr);   
    COMPILER_64_ZERO(raln);    COMPILER_REFERENCE(&raln);   
    COMPILER_64_ZERO(rdisc);   COMPILER_REFERENCE(&rdisc);    
    COMPILER_64_ZERO(dropped); COMPILER_REFERENCE(&dropped);
    COMPILER_64_ZERO(drbyt);   COMPILER_REFERENCE(&drbyt); 
    COMPILER_64_ZERO(dtbyt);   COMPILER_REFERENCE(&dtbyt);   
    COMPILER_64_ZERO(drpkt);   COMPILER_REFERENCE(&drpkt);   
    COMPILER_64_ZERO(dtpkt);   COMPILER_REFERENCE(&dtpkt);    
    COMPILER_64_ZERO(drerr);   COMPILER_REFERENCE(&drerr);   
    COMPILER_64_ZERO(draln);   COMPILER_REFERENCE(&draln);    
    COMPILER_64_ZERO(drdisc);  COMPILER_REFERENCE(&drdisc);    
    COMPILER_64_ZERO(ddropped);COMPILER_REFERENCE(&ddropped);

    bcm_stat_sync(unit);

    PBMP_ITER(sel_pbm, port) {
        /*
         *      To account for counters such as IRDISC
         */
        tolerance = IS_HG_PORT(unit, port) ? 1 : 0;
        port_error = 0;
        stopped = 0;

        bcm_stat_get(unit, port, snmpIfInOctets, &rbyt);
        bcm_stat_get(unit, port, snmpIfOutOctets,&tbyt);
        bcm_stat_get(unit, port, snmpDot1dTpPortOutFrames, &tpkt);
        bcm_stat_get(unit, port, snmpDot1dTpPortInFrames,  &rpkt);
        bcm_stat_get(unit, port, snmpDot3StatsAlignmentErrors, 
                     &raln);
        bcm_stat_get(unit, port, 
                     snmpDot3StatsInternalMacReceiveErrors, &rdisc);
        bcm_stat_get(unit, port, snmpIfInDiscards, &dropped);

        if (!stats[port].initialized) {
            stats[port].rbyt = rbyt;
            stats[port].tbyt = tbyt;
            stats[port].rpkt = rpkt;
            stats[port].tpkt = tpkt;            

            stats[port].raln = raln;            
            stats[port].rdisc = rdisc;
            stats[port].dropped = dropped; 
            stats[port].error = 0;
#ifdef BCM_TOMAHAWK_SUPPORT
            COMPILER_64_ZERO(stats[port].rate);
#endif
            stats[port].initialized = TRUE;
        } else {
            int speed, duplex;
            uint32 dtpktl, dtbytl, drpktl, drbytl;
            uint32 dralnl, drdiscl, ddroppedl;

            COMPILER_64_DELTA(drbyt, stats[port].rbyt, rbyt);
            COMPILER_64_DELTA(dtbyt, stats[port].tbyt, tbyt);
            COMPILER_64_DELTA(drpkt, stats[port].rpkt, rpkt);
            COMPILER_64_DELTA(dtpkt, stats[port].tpkt, tpkt);
            COMPILER_64_DELTA(draln, stats[port].raln, raln);
            COMPILER_64_DELTA(drdisc, stats[port].rdisc,
                             rdisc);
            COMPILER_64_DELTA(ddropped, stats[port].dropped,
                             dropped);

            COMPILER_64_TO_32_LO(drbytl, drbyt);
            COMPILER_64_TO_32_LO(dtbytl, dtbyt);
            COMPILER_64_TO_32_LO(drpktl, drpkt);
            COMPILER_64_TO_32_LO(dtpktl, dtpkt);
            COMPILER_64_TO_32_LO(dralnl, draln);
            COMPILER_64_TO_32_LO(drdiscl, drdisc);
            COMPILER_64_TO_32_LO(ddroppedl, ddropped);

            switch (lb_mode) {
            case LB2_MODE_MAC:
                if (dtpktl == 0 || dtbytl == 0 ||
                    ((drpktl == 0 || drbytl == 0) &&
                     (IS_FE_PORT(unit, port) || IS_GE_PORT(unit, port)))) {
                    stopped = -1;
                }
                break;
            case LB2_MODE_PHY:
                if (dtpktl == 0 || dtbytl == 0 || 
                    drpktl == 0 || drbytl == 0) {
                    stopped = -1;
                }
                break;
            case LB2_MODE_EXT:
                /* Don't check for stopped yet */
                break;
            default:
                assert(0);
            }

            if ((drdiscl > tolerance) || (ddroppedl > 0) || 
                (stopped < 0) || 
                ((dralnl > tolerance) && 
                 (IS_FE_PORT(unit, port) || IS_GE_PORT(unit, port)))) {
                port_error = -1;
                error = -1;
            }

            bcm_port_speed_get(unit, port, &speed);
            bcm_port_duplex_get(unit, port, &duplex);

            if (port_error < 0) {
                cli_out("Port %s (%d) (",
                        SOC_PORT_NAME(unit, port), port);
                snake_show_rate(speed * 1000000);
                cli_out("b %s): ",
                        duplex ? "FD" : "HD");
                if ((dralnl > tolerance) && 
                    (IS_FE_PORT(unit, port) || IS_GE_PORT(unit, port))) {
                    cli_out("ALN Error Detected\n");
                } else if (drdiscl > tolerance) {
                    cli_out("Discard Error Detected\n");
                } else if (ddroppedl > 0) {
                    cli_out("Dropped Error Detected\n");
                } else if (stopped < 0) {
                    cli_out("FAIL->NO TRAFFIC\n");
                } else {
                    cli_out("PASSED\n");
                }

                cli_out("\tTX packets ");
                snake_show_number(tpkt);
                cli_out(" +");
                snake_show_number(dtpkt);
                cli_out("/s, bytes ");
                snake_show_number(tbyt);
                cli_out("B +");
                snake_show_number(dtbyt);
                cli_out("B/s [");
                snake_show_rate(dtbytl*8);
                cli_out("b/s]\n");               
                cli_out("\tRX packets ");
                snake_show_number(rpkt);
                cli_out(" +");
                snake_show_number(drpkt);
                cli_out("/s, bytes ");
                snake_show_number(rbyt);
                cli_out("B +");
                snake_show_number(drbyt);
                cli_out("B/s [");
                snake_show_rate(drbytl*8);
                cli_out("b/s]\n");
            }

            /* Zero packet rate increase indicates failure */

            if (stopped < 0) {
                test_error(unit, 
                           "ERROR: traffic stopped on port %s (%d)\n",
                           SOC_PORT_NAME(unit, port), port);
 
                /* Give us a clue .. */
                cli_out("Port %s (%d) statistics:\n",
                        SOC_PORT_NAME(unit, port), port);
                SOC_PBMP_CLEAR(pbm);
                SOC_PBMP_PORT_ADD(pbm, port);
                if (SOC_IS_ESW(unit)) {
#ifdef BCM_ESW_SUPPORT
                        do_show_counters(unit,
                                     INVALIDr,
                                     pbm,
                                 SHOW_CTR_CHANGED|
                                 SHOW_CTR_SAME|
                                 SHOW_CTR_NZ);              
#endif
                    } else {
                    }               
            }

            stats[port].rbyt = rbyt;
            stats[port].tbyt = tbyt;
            stats[port].rpkt = rpkt;
            stats[port].tpkt = tpkt;            

            stats[port].raln = raln;            
            stats[port].rdisc = rdisc;
            stats[port].dropped = dropped;      
            stats[port].error = port_error;
        }
    }

    return error;
}

STATIC int
lbu_snake_diag(loopback2_test_t *lw, lb2_port_stat_t *stats, 
               lb2s_port_connect_t *connect_info)
/*
 * Function:    lbu_snake_diag
 * Purpose:     Attempt to determing snake failure cause
 * Parameters:  lw - pointer to lb work.
 *              stats - pointer to port statistics.
 *              connect_info - port interlink data.
 * Returns:     0 - success, -1 failed.
 */
{
    loopback2_testdata_t  *lp = lw->cur_params;
    int             unit = lw->unit;
    soc_port_t      thisPort, toPort;
    int             ix, rv = 0;
    uint32          tempStat, tolerance = 0;
    uint64          tempDiff;

    COMPILER_64_ZERO(tempDiff);

    for (ix = 0; ix < lw->port_count; ix++) {
        thisPort = connect_info[ix].this_port;
        toPort = connect_info[ix].to_port;

        COMPILER_64_TO_32_LO(tempStat, stats[thisPort].tpkt);
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Port %s Tx Count = 0x%x\n"), 
                  SOC_PORT_NAME(unit, thisPort), tempStat));

        if (tempStat == 0) {
            test_error(unit, "Port %s failed to send packets\n", 
                       SOC_PORT_NAME(unit, thisPort));
            rv = -1;
        }

        /* BigMac receive counters don't work in MAC loopback */
        if (((IS_FE_PORT(unit, thisPort) || IS_GE_PORT(unit, thisPort)) &&
              (lp->loopback == LB2_MODE_MAC))) {
            COMPILER_64_TO_32_LO(tempStat, stats[thisPort].rpkt);
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Port %s Rx Count = 0x%x\n"), 
                      SOC_PORT_NAME(unit, thisPort), tempStat));

            if (tempStat == 0) {
                test_error(unit, "Port %s failed to receive packets\n", 
                           SOC_PORT_NAME(unit, thisPort));
                rv = -1;
            }

            COMPILER_64_DELTA(tempDiff, stats[thisPort].rpkt, 
                             stats[thisPort].tpkt);
            COMPILER_64_TO_32_LO(tempStat, tempDiff);
            if ((tempStat > tolerance) && (tempStat < (~tolerance + 1))) {
                COMPILER_64_TO_32_LO(tempStat, stats[thisPort].tpkt);
                cli_out("\t0x%x packets transmitted by port %s\n",
                        tempStat, SOC_PORT_NAME(unit, thisPort));
                COMPILER_64_TO_32_LO(tempStat, stats[thisPort].rpkt);
                cli_out("\t0x%x packets received by port %s\n", 
                        tempStat, SOC_PORT_NAME(unit, thisPort));
                test_error(unit, "Port %s loopback failure\n", 
                           SOC_PORT_NAME(unit, thisPort));
                rv = -1;
            }
        }

        /* BigMac receive counters don't work in MAC loopback */
        if (((IS_FE_PORT(unit, toPort) || IS_GE_PORT(unit, toPort)) &&
              (lp->loopback == LB2_MODE_MAC))) {
            COMPILER_64_DELTA(tempDiff, stats[toPort].tpkt, 
                             stats[toPort].rpkt);
            COMPILER_64_TO_32_LO(tempStat, tempDiff);

            tolerance = (IS_HG_PORT(unit, toPort) || 
                         IS_HG_PORT(unit, thisPort)) ? 
                lw->tx_ppt : 0;
            if ((tempStat > tolerance) && (tempStat < (~tolerance + 1))) {
                COMPILER_64_TO_32_LO(tempStat, stats[toPort].rpkt);
                cli_out("\t0x%x packets received by port %s\n", 
                        tempStat, SOC_PORT_NAME(unit, toPort));
                COMPILER_64_TO_32_LO(tempStat, stats[thisPort].tpkt);
                cli_out("\t0x%x packets transmitted by port %s\n",
                        tempStat, SOC_PORT_NAME(unit, thisPort));
                test_error(unit, 
                           "Port %s failed to switch packets to port %s\n",
                           SOC_PORT_NAME(unit, toPort),
                           SOC_PORT_NAME(unit, thisPort));
                rv = -1;
            }
        }

        
    }

    return rv;
}

void
lbu_snake_dump_stats(int unit, bcm_pbmp_t sel_pbm, lb2_port_stat_t *stats)
/*
 * Function:    lbu_snake_dump_stats
 * Purpose:     Display port stats for diagnosis purposes
 * Parameters:  unit - device number.
 *              sel_pbm - port bitmap of selected ports.
 *              stats - pointer to stats records.
 */
{
    soc_port_t      port;
#ifdef BCM_TOMAHAWK_SUPPORT
    char            buf_rate[32];
    int             commachr = soc_property_get(unit, spn_DIAG_COMMA, ',');
#endif
    COMPILER_REFERENCE(unit);

    PBMP_ITER(sel_pbm, port) {
        cli_out("Port %s", SOC_PORT_NAME(unit, port));
        cli_out("\tTX ");
        snake_show_number(stats[port].tpkt);
        cli_out("pkt ");
        snake_show_number(stats[port].tbyt);
        cli_out("B,\tRX ");
        snake_show_number(stats[port].rpkt);
        cli_out("pkt ");
        snake_show_number(stats[port].rbyt);
        cli_out("B");
#ifdef BCM_TOMAHAWK_SUPPORT
        if (!COMPILER_64_IS_ZERO(stats[port].rate)){
            format_uint64_decimal(buf_rate, stats[port].rate, commachr);
            cli_out("  Rate %16sB/s", buf_rate);
        }
#endif
        cli_out("\n");
    }
}


/* Set up the basic packet data that can vary with each transmit */

STATIC void
lbu_serial_higig_setup(int unit, loopback2_testdata_t *lp, bcm_pkt_t *pkt)
{
    pkt->opcode = lp->opcode;
    pkt->dest_port = lp->d_port;
    pkt->dest_mod = lp->d_mod;
    bcm_tx_pkt_setup(unit, pkt);
}

STATIC void
lbu_snake_higig_setup(int unit, loopback2_testdata_t *lp,
                      lb2s_port_connect_t *port_info, bcm_pkt_t *pkt)
{
    pkt->opcode = lp->opcode;
    pkt->dest_port = port_info->to_port;
    pkt->dest_mod = port_info->dst_modid;
    bcm_tx_pkt_setup(unit, pkt);
}

STATIC void
lbu_pkt_data_setup(loopback2_test_t *lw, bcm_pkt_t *pkt, bcm_vlan_t vlan,
                   sal_mac_addr_t mac_dst, sal_mac_addr_t mac_src, int seq_no)
{
    loopback2_testdata_t *lp = lw->cur_params;
    enet_hdr_t           *eh = NULL;
    int tx_len, payload_len;
    uint8                *sequence_p;
    int cur_offset;

    tx_len = pkt->pkt_data[0].len;
    if (pkt->flags & BCM_TX_CRC_ALLOC) {
        payload_len = tx_len - 18;
    } else {
        payload_len = tx_len - 22;
    }

    eh = (enet_hdr_t *)BCM_PKT_DMAC(pkt);

    /* Write source/destination MAC address */
    ENET_SET_MACADDR(&eh->en_dhost, mac_dst);
    ENET_SET_MACADDR(&eh->en_shost, mac_src);

    /* Write VLAN tag (16 + 16 bits) and sequence number (32 bits) */
    packet_store((uint8 *)&eh->en_tag_tpid, 2, 0x8100 << 16, 0);
    packet_store((uint8 *)&eh->en_tag_ctrl, 2,
                 VLAN_CTRL(0, 0, vlan) << 16, 0);
    packet_store((uint8*)eh + 16, 4, lw->tx_seq++, 0);
    packet_store((uint8*)&eh->en_tag_len, 2, payload_len << 16, 0);

    /*
     * Fill data from after header & seq # to end of packet.  Byte data
     * starts with the pattern MSByte through LSByte and recycles.
     * Optionally overwrite last 4 bytes of packet with correct CRC.
     */
    cur_offset = 20;
    lp->pattern = packet_store((uint8*)eh + cur_offset, tx_len - cur_offset,
                                  lp->pattern, lp->pattern_inc);

    /* Store the sequence number of the packet */
    sequence_p = (uint8 *)eh + LB2_ID_POS;
    *sequence_p = (uint8)seq_no;

    if (lp->crc_mode == LB2_CRC_MODE_CPU_APPEND) {
        packet_store((uint8*)eh + tx_len - 4, 4,
                     ~_shr_crc32(~0, (uint8*)eh, tx_len - 4), 0);
    } 

    if (SOC_IS_XGS12_FABRIC(lw->unit)) { 
        if (lp->crc_mode == LB2_CRC_MODE_MAC_REGEN) { 
            /* hercules always requires CRC field to be zero */
            packet_store(((uint8*)eh + (tx_len) - 4), 4, 0, 0);
        }
    }
}

/*
 * This routine is called by
 * The dest ports only need to be set up once */

STATIC void
lbu_pkt_init(bcm_pkt_t **pkts, int max_ppt, int tx_port, int tagged)
{
    int i;
    bcm_pkt_t *pkt;

    for (i = 0; i < max_ppt; i++) {
        pkt = pkts[i];

        SOC_PBMP_CLEAR(pkt->tx_pbmp);
        SOC_PBMP_PORT_ADD(pkt->tx_pbmp, tx_port);
        SOC_PBMP_CLEAR(pkt->tx_upbmp);
        if (!tagged) {
            SOC_PBMP_PORT_ADD(pkt->tx_upbmp, tx_port);
        }
    }
}

/* Set up a chain of packets according to current settings */

STATIC int
lbu_pkt_chain_setup(bcm_pkt_t **pkts, int ppt, int len, int cos,
                    loopback2_test_t *lw)
{
    loopback2_testdata_t *lp = lw->cur_params;
    int                  i;
    sal_mac_addr_t       cur_mac_dst, cur_mac_src;
    bcm_pkt_t            *pkt;

    /* Init the src/dest mac addresses each chain */
    ENET_SET_MACADDR(cur_mac_dst, lw->base_mac_dst);
    ENET_SET_MACADDR(cur_mac_src, lw->base_mac_src);

    for (i = 0; i < ppt; i++) {

        pkt = pkts[i];
        pkt->cos = cos;
        /* For now, internal priority matches COS (or v.v.) */
        pkt->prio_int = cos;
        pkt->pkt_data[0].len = len; /* Set length properly for pkt */

        lbu_pkt_data_setup(lw, pkt, lp->vlan, cur_mac_dst, cur_mac_src, i);
        if (SOC_IS_XGS(lw->unit)) {
            lbu_serial_higig_setup(lw->unit, lp, pkt);
        }
#ifdef BCM_XGS3_SWITCH_SUPPORT
        /*
         * Setup the Higig header for XGS3 devices
         */
        if (SOC_IS_XGS3_SWITCH(lw->unit) && !(SOC_IS_TOMAHAWK3(lw->unit))) {
            int         dmod;
            uint32      vlan;
            int         unit = lw->unit;
            soc_higig_hdr_t  *xgh = (soc_higig_hdr_t*)pkt->_higig;
            BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &dmod));
            if (SOC_IS_HAWKEYE(unit) || (SOC_IS_RAPTOR(unit) && IS_E_PORT(unit, lw->tx_port))) {
            } else {
                /* Fabric mapped mode TX */
                pkt->flags |= (BCM_TX_ETHER | BCM_TX_HG_READY);
            }
            pkt->dest_mod = dmod;
            pkt->src_mod = dmod + 1;
            pkt->src_port =  CMIC_PORT(unit);
            pkt->dest_port = lw->tx_port;
            if (IS_ST_PORT(unit,  pkt->dest_port)) {
                pkt->opcode = 0; /* CPU control for HG ports */
            } else {
                pkt->opcode = 1; /* UNICAST  for E ports */
            }
            sal_memset(xgh, 0, sizeof(pkt->_higig));
#ifdef BCM_HIGIG2_SUPPORT
            if (soc_feature(unit, soc_feature_higig2)) {
                soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG2_START);
            } else {
                soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG_START);
            }
#else
            soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG_START);
#endif
            soc_higig_field_set(unit, xgh, HG_hgi, SOC_HIGIG_HGI);
            soc_higig_field_set(unit, xgh, HG_opcode, pkt->opcode);
            soc_higig_field_set(unit, xgh, HG_hdr_format, SOC_HIGIG_HDR_DEFAULT);
            vlan = BCM_PKT_VLAN_CONTROL(pkt);
            soc_higig_field_set(unit, xgh, HG_vlan_tag, vlan);
            soc_higig_field_set(unit, xgh, HG_dst_mod, pkt->dest_mod);
            soc_higig_field_set(unit, xgh, HG_dst_port, pkt->dest_port);
            soc_higig_field_set(unit, xgh, HG_src_mod, pkt->src_mod);
            soc_higig_field_set(unit, xgh, HG_src_port, pkt->src_port);
            soc_higig_field_set(unit, xgh, HG_cos, pkt->cos);
            soc_higig_field_set(unit, xgh, HG_ingress_tagged, 1);
        }
#endif
        increment_macaddr(cur_mac_dst, lp->mac_dst_inc);
        increment_macaddr(cur_mac_src, lp->mac_src_inc);
    }

    return BCM_E_NONE;
}

/* Set up a chain of packets according to current settings */

STATIC int
lbu_snake_pkt_chain_setup(bcm_pkt_t **pkts, int ppt, int len, int cos,
                    loopback2_test_t *lw)
{
    loopback2_testdata_t *lp = lw->cur_params;
    int                  i;
    bcm_pkt_t           *pkt;
    lb2s_port_connect_t *work_info, *f_info, *r_info;
    int                 snake_way = lp->snake_way;
    int                 forward, both;

    both = (snake_way == LB2_SNAKE_BOTH);
    forward = (snake_way == LB2_SNAKE_BOTH) || (snake_way == LB2_SNAKE_INCR);

    f_info = lw->port_cnct_f;
    r_info = lw->port_cnct_r;
    work_info = forward ? f_info : r_info;

    for (i = 0; i < ppt; i++) {

        pkt = pkts[i];
        pkt->cos = cos;
        /* For now, internal priority matches COS (or v.v.) */
        pkt->prio_int = cos;
        pkt->pkt_data[0].len = len; /* Set length properly for pkt */
        SOC_PBMP_CLEAR(pkt->tx_pbmp);
        SOC_PBMP_PORT_ADD(pkt->tx_pbmp, work_info->this_port);
        SOC_PBMP_CLEAR(pkt->tx_upbmp);
        SOC_PBMP_PORT_ADD(pkt->tx_upbmp, work_info->this_port);

        if (both) {
            work_info = (i % 2) ? r_info : f_info;
        }
        
        lbu_pkt_data_setup(lw, pkt, work_info[lw->port_count].added_vlan, 
                           work_info->dst_mac, work_info->src_mac, i);
        if (SOC_IS_XGS(lw->unit)) {
            lbu_snake_higig_setup(lw->unit, lp, work_info, pkt);
        }
#ifdef BCM_XGS3_SWITCH_SUPPORT
        /*
         * Setup the Higig header for XGS3 devices
         */
        if (SOC_IS_XGS3_SWITCH(lw->unit) && 
            !IS_ST_PORT(lw->unit, work_info->this_port) &&
            !SOC_IS_TOMAHAWK3(lw->unit)) {
            int         dmod;
            uint32      vlan;
            int         unit = lw->unit;
            soc_higig_hdr_t  *xgh = (soc_higig_hdr_t*)pkt->_higig;
            BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &dmod));
            if (SOC_IS_HAWKEYE(unit) || (SOC_IS_RAPTOR(unit) && IS_E_PORT(unit, work_info->this_port))) {
            } else {
                /* Fabric mapped mode TX */
                pkt->flags |= (BCM_TX_ETHER | BCM_TX_HG_READY);
            }
            pkt->src_mod = pkt->dest_mod = dmod;
            pkt->src_port =  CMIC_PORT(unit);
            pkt->dest_port = work_info->this_port;
            pkt->opcode = lp->opcode;
            sal_memset(xgh, 0, sizeof(pkt->_higig));
#ifdef BCM_HIGIG2_SUPPORT
            if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || SOC_IS_TR_VL(unit)) {
                soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG2_START);
            } else {
                soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG_START);
            }
#else
            soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG_START);
#endif
            soc_higig_field_set(unit, xgh, HG_hgi, SOC_HIGIG_HGI);
            soc_higig_field_set(unit, xgh, HG_opcode, pkt->opcode);
            soc_higig_field_set(unit, xgh, HG_hdr_format, SOC_HIGIG_HDR_DEFAULT);
            vlan = BCM_PKT_VLAN_CONTROL(pkt);
            soc_higig_field_set(unit, xgh, HG_vlan_tag, vlan);
            soc_higig_field_set(unit, xgh, HG_dst_mod, pkt->dest_mod);
            soc_higig_field_set(unit, xgh, HG_dst_port, pkt->dest_port);
            soc_higig_field_set(unit, xgh, HG_src_mod, pkt->src_mod);
            soc_higig_field_set(unit, xgh, HG_src_port, pkt->src_port);
            soc_higig_field_set(unit, xgh, HG_cos, pkt->cos);
            soc_higig_field_set(unit, xgh, HG_ingress_tagged, 1);
        }
#endif
    }

    return BCM_E_NONE;
}

STATIC int
lbu_send_packets(int unit, bcm_pkt_t **pkts, int ppt, loopback2_test_t *lw)
{
    int rv = BCM_E_NONE, i;
    int try_cnt = 0;
#ifdef BCM_TOMAHAWK_SUPPORT
    int j;
#endif

    lw->expect_pkts = TRUE; /* Let RX handler accept packets */

    switch (lw->cur_params->send_mech) {
        case LBD_SEND_SINGLE:
#ifdef BCM_TOMAHAWK_SUPPORT
            if (!lw->cur_params->line_rate) {
                lw->cur_params->line_rate_count = 1;
            }
            for (j = 0; j < lw->cur_params->line_rate_count; j++) {
#endif
                for (i = 0; i < ppt; i++ ) {
                    /*Need try until the memor was free and then send next one*/
                    for (try_cnt = 50; try_cnt > 0; try_cnt--) {
                        rv = bcm_tx(unit, pkts[i], NULL);
                        if (rv == BCM_E_MEMORY) {
                            sal_usleep(1);
                            continue;
                        }
                        break;
                    }

                    if (rv < 0) {
                        cli_out("lbu_send_packets: ERROR: port%d: bcm_tx:%s\n",
                                 lw->tx_port, bcm_errmsg(rv));
                    }
                    lw->tx_count++;
                    lw->tx_bytes += lw->tx_len;
                }
#ifdef BCM_TOMAHAWK_SUPPORT
            }
#endif
        break;
        case LBD_SEND_ARRAY:
        if ((rv = bcm_tx_array(unit, pkts, ppt, NULL, NULL)) < 0) {
            cli_out("lbu_send_packets (array): ERROR: port %d; ppt %d; "
                    "bcm_tx: %s\n", lw->tx_port, ppt, bcm_errmsg(rv));
        }
        lw->tx_count += ppt;
        lw->tx_bytes += lw->tx_len * ppt;
        break;
    case LBD_SEND_LLIST:
    default:
        cli_out("lbu_send_packets: Unsupported send mechanism\n");
        return BCM_E_UNAVAIL;
        break;
    }

    return rv;
}

STATIC int
lbu_serial_wait(loopback2_test_t *lw)
{
    int unit = lw->unit;
    int ix;
    bcm_pkt_t *next_rx;

    if (sal_sem_take(lw->sema, lw->timeout_usec)) {
        cli_out("Time-out waiting for completion Tx(%s) Rx(%s)\n",
                SOC_PORT_NAME(unit, lw->tx_port),
                SOC_PORT_NAME(unit, lw->rx_port));
        cli_out("Receive count is %d; expecting %d pkts.\n",
                lw->rx_pkt_cnt, lw->tx_ppt);
        lbu_stats_report(lw);
        lw->sema_woke = FALSE;
        return (-1);
    }

    lw->sema_woke = FALSE;    
    /*
        The reason for this change is some strange SW synchronization 
        issue that is observed only on NSX CPU with Linux 2.6 OS 
        Seems that an RX handler releases the semaphor but not all 
        packets are updated in the SW structure.
    */
#if 0 
    lw->expect_pkts = FALSE; /* Let RX handler ignore packets */
#endif

    for (ix = 0; ix < lw->tx_ppt; ix++) {
        next_rx = LB2_RX_PKT(lw, ix);
        if (!next_rx->_pkt_data.data) {
            /* This change is intentional to avoid a strange miss 
            synchronization on NSX CPU with Linux 2.6 OS */
            ix--;
            continue;
        }
        if (lbu_packet_compare(lw, lw->tx_pkts[ix], next_rx) < 0) {
            cli_out("Compare error on packet %d in chain\n", ix);
            return (-1);
        }
        lw->rx_bytes += next_rx->pkt_len;
        lw->rx_count++;
    }

    return 0;
}

/* Dump TX packets on error */
STATIC void
lbu_dump_tx_packets(loopback2_test_t *lw)
{
    int ix;

    cli_out("Transmitted packets\n");
    for (ix = 0; ix < lw->tx_ppt; ix++) {
        cli_out("TX packet %d: len=%d\n",
                ix, lw->tx_pkts[ix]->_pkt_data.len);
    
#ifdef BCM_ESW_SUPPORT
        soc_dma_dump_pkt(lw->unit, "  ", lw->tx_pkts[ix]->_pkt_data.data, 
                         lw->tx_pkts[ix]->_pkt_data.len, TRUE);
#endif
    }
}

/* Setup a chain of packets, tx it and wait for packets back */
STATIC int
lbu_do_chain(int unit, int len, int ppt, int cos, int i,
             loopback2_test_t *lw)
{
    loopback2_testdata_t        *lp = lw->cur_params;
    int rv = BCM_E_NONE;

    if ((rv = lbu_pkt_chain_setup(lw->tx_pkts, ppt, len, cos, lw)) < 0) {
        test_error(unit, "Could not setup chain\n\tPort TX(%s) RX(%s)\n"
                   "\tPacket Length=%d bytes\n"
                   "\tPPT=%d\n"
                   "\tCurrent COS=%d\n"
                   "\tIteration %d of %d\n",
                   SOC_PORT_NAME(unit, lw->tx_port),
                   SOC_PORT_NAME(unit, lw->rx_port),
                   len, ppt, cos, i + 1, lp->iterations);
        return rv;
    }

    if ((rv = lbu_send_packets(unit, lw->tx_pkts, ppt, lw)) < 0) {
        test_error(unit, "Packet TX failed: ERROR: %s\n",
                   bcm_errmsg(rv));
        lbu_dump_tx_packets(lw);
        return rv;
    }
    if (lp->inject) { /* Diagnostic */
        return 0;
    }

    if ((rv = lbu_serial_wait(lw)) < 0) {
        test_error(unit, "See error description\n\tPort TX(%s) RX(%s)\n"
                   "\tPacket Length=%d bytes\n"
                   "\tPPT=%d\n"
                   "\tCurrent COS=%d\n"
                   "\tIteration %d of %d\n",
                   SOC_PORT_NAME(unit, lw->tx_port),
                   SOC_PORT_NAME(unit, lw->rx_port),
                   len, ppt, cos, i + 1, lp->iterations);
        lbu_dump_tx_packets(lw);
        return rv;
    }

    lbu_stats_report(lw);
    CLEAR_RX_PKTS(lw, lp);

    return 0;
}


int
lbu_serial_txrx(loopback2_test_t *lw)
/*
 * Function:    lbu_serial_txrx
 * Purpose:     Loop over requested lengths and chaining sizes, and
 *              transmit/receive packets.
 * Parameters:  lw - pointer to work structure
 * Returns:     0 - success
 *              -1- failed.
 */
{
    loopback2_testdata_t        *lp = lw->cur_params;
    int         len, ppt, cos, i;
    int         unit = lw->unit;
    int         tagged, rv, test_rv = 0;
    uint32      tx_count, rx_count;
    uint32      rx_count0, rx_count1, rx_count2, rx_count3;

    /* Clear port rx/tx counters */
    if ((rv = bcm_stat_clear(unit, lw->tx_port)) < 0) {
        test_error(unit, "Could not clear port %s counters: %s\n",
                   SOC_PORT_NAME(unit, lw->tx_port),
                   soc_errmsg(rv));
        test_rv = -1;
        goto error;
    }

    if ((rv = bcm_stat_clear(unit, lw->rx_port)) < 0) {
        test_error(unit, "Could not clear port %s counters: %s\n",
                   SOC_PORT_NAME(unit, lw->rx_port),
                   soc_errmsg(rv));
        test_rv = -1;
        goto error;
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Testing Port %s --> %s cnt(%d) pkt/trial(%d,%d += %d) "
                         "l(%d,%d += %d) cos(%d,%d)\n"),
              SOC_PORT_NAME(unit, lw->tx_port),
              SOC_PORT_NAME(unit, lw->rx_port),
              lp->iterations,
              lp->ppt_start, lp->ppt_end, lp->ppt_inc,
              lp->len_start, lp->len_end, lp->len_inc,
              lp->cos_start, lp->cos_end));

    /* One time setup of packets */
    tagged = !SOC_PBMP_MEMBER(lp->ubm, lw->tx_port);
    lbu_pkt_init(lw->tx_pkts, lp->ppt_end, lw->tx_port, tagged);

    for (i = 0; i < lp->iterations; i++) {
        for (len = lp->len_start; len <= lp->len_end; len += lp->len_inc) {
            for (cos = lp->cos_start; cos <= lp->cos_end; cos++) {
                for (ppt = lp->ppt_start; ppt <= lp->ppt_end;
                     ppt += lp->ppt_inc) {
                    lw->tx_ppt = ppt;
                    lw->tx_len = len;
                    lw->rx_pkt_cnt = 0;
                    if (lbu_do_chain(unit, len, ppt, cos, i, lw) < 0) {
                        test_rv = -1;
                        goto done;
                    }
                }
            }
        }
    } /* Test iterations */

done:
    /* Verify port rx/tx stats */
    bcm_stat_sync(unit);
    if ((IS_FE_PORT(unit, lw->rx_port) || IS_GE_PORT(unit, lw->rx_port))) {
        bcm_stat_val_t rx_stat;
        bcm_stat_val_t tx_stat;
        if ((SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) && 
            IS_ST_PORT(unit, lw->tx_port)) {
            /* WAR for stacking ports of Raptor/Raven, where Higig unicast
               packets are accounted as multicast by the MAC */
            tx_stat = snmpIfOutMulticastPkts;
            rx_stat = snmpIfInMulticastPkts;
        } else {
            tx_stat = snmpIfOutUcastPkts;
            rx_stat = snmpIfInUcastPkts;
        }
        if ((rv = bcm_stat_get32(unit, lw->tx_port,
                                 tx_stat, &tx_count)) < 0) {
            test_error(unit,
                       "Could not get port %s tx %s stats : %s\n",
                       SOC_PORT_NAME(unit, lw->tx_port),
                       "snmpIfOutUcastPkts",
                       soc_errmsg(rv));
            test_rv = -1;
            goto error;
        }
        if ((rv = bcm_stat_get32(unit, lw->rx_port,
                                 rx_stat, &rx_count)) < 0) {
            test_error(unit,
                       "Could not get port %s rx %s stats : %s\n",
                       SOC_PORT_NAME(unit, lw->rx_port),
                       "snmpIfInUcastPkts",
                       soc_errmsg(rv));
            test_rv = -1;
            goto error;
        }
        if (tx_count != rx_count) {
            test_error(unit,
                       "TX/RX packet stats mismatch: (%s) TX=%d, (%s) RX=%d\n",
                       SOC_PORT_NAME(unit, lw->tx_port),
                       (int) tx_count,
                       SOC_PORT_NAME(unit, lw->rx_port),
                       (int) rx_count);
            test_rv = -1;
        }
    }
    
    /* Check for port TX error statistics */
    if ((rv = bcm_stat_get32(unit, lw->tx_port,
                             snmpIfOutErrors, &tx_count)) < 0) {
        test_error(unit,
                   "Could not get port %s tx error %s stats : %s\n",
                   SOC_PORT_NAME(unit, lw->tx_port),
                   "snmpIfOutErrors", 
                   soc_errmsg(rv));
        test_rv = -1;
        goto error;
    }
    if (tx_count) {
        test_error(unit,
                   "TX error stats found: TX (%s) error count=%d\n",
                   SOC_PORT_NAME(unit, lw->tx_port), (int) tx_count);
        test_rv = -1;
    }

    /* Check for port RX error statistics
     * Use individual stat to count rx errors instead of snmpIfInErrors
     * which includes good oversize packets.
     */
    if ((rv = bcm_stat_get32(unit, lw->rx_port,
                             snmpEtherStatsUndersizePkts, &rx_count0)) < 0) {
        test_error(unit,
                   "Could not get port %s rx %s stats : %s\n",
                   SOC_PORT_NAME(unit, lw->rx_port),
                   "snmpEtherStatsUndersizePkts",
                   soc_errmsg(rv));
        test_rv = -1;
        goto error;
    }
    if ((rv = bcm_stat_get32(unit, lw->rx_port,
                             snmpEtherStatsFragments, &rx_count1)) < 0) {
        test_error(unit,
                   "Could not get port %s rx %s stats : %s\n",
                   SOC_PORT_NAME(unit, lw->rx_port),
                   "snmpEtherStatsFragments",
                   soc_errmsg(rv));
        test_rv = -1;
        goto error;
    }
    if ((rv = bcm_stat_get32(unit, lw->rx_port,
                             snmpEtherStatsCRCAlignErrors, &rx_count2)) < 0) {
        test_error(unit,
                   "Could not get port %s rx %s stats : %s\n",
                   SOC_PORT_NAME(unit, lw->rx_port),
                   "snmpEtherStatsCRCAlignErrors",
                   soc_errmsg(rv));
        test_rv = -1;
        goto error;
    }
    if ((rv = bcm_stat_get32(unit, lw->rx_port,
                             snmpEtherStatsJabbers, &rx_count3)) < 0) {
        test_error(unit,
                   "Could not get port %s rx %s stats : %s\n",
                   SOC_PORT_NAME(unit, lw->rx_port),
                   "snmpEtherStatsJabbers",
                   soc_errmsg(rv));
        test_rv = -1;
        goto error;
    }
    rx_count = rx_count0 + rx_count1 + rx_count2 + rx_count3;
    if (rx_count) {
        test_error(unit,
                   "RX error stats found: RX (%s) error count=%d\n",
                   SOC_PORT_NAME(unit, lw->rx_port), (int) rx_count);
        test_rv = -1;
    }

    if (test_rv) {
        goto error;
    }
    return(0);

error:
    CLEAR_RX_PKTS(lw, lp);
    return (test_rv);
}

int
lbu_snake_tx(loopback2_test_t *lw)
/*
 * Function:    lbu_snake_tx
 * Purpose:     Inject packets into snake sources.
 * Parameters:  lw - pointer to work structure
 * Returns:     0 - success
 *              -1- failed.
 */
{
    int rv = BCM_E_NONE;

    if ((rv = lbu_snake_pkt_chain_setup(lw->tx_pkts, lw->tx_ppt, lw->tx_len, 
                                        lw->tx_cos, lw)) < 0) {
        test_error(lw->unit, "Could not setup snake\n"
                   "\tPacket Length=%d bytes\n"
                   "\tPPT=%d\n"
                   "\tCurrent COS=%d\n",
                   lw->tx_len, lw->tx_ppt, lw->tx_cos);
        return rv;
    }

    if ((rv = lbu_send_packets(lw->unit, lw->tx_pkts, lw->tx_ppt, lw)) < 0) {
        test_error(lw->unit, "Packet TX failed: ERROR: %s\n",
                   bcm_errmsg(rv));
        lbu_dump_tx_packets(lw);
        return rv;
    }
    return 0;
}
#ifdef BCM_TOMAHAWK_SUPPORT
/*
 * Function:
 *      lbu_snake_check_mib_counters
 * Purpose:
 *      Checks Error counters in MAC.
 */

static int
lbu_snake_check_mib_counters(int unit, loopback2_testdata_t *lp)
{
    uint32 j, column;
    uint64 rdata;
    int rv;
    int pblk;
    bcm_port_t   pt;

    soc_reg_t error_counters[][4] = {
                                    {RFLRr,   XLMIB_RFLRr,   CLMIB_RFLRr,   GRFLRr},
                                    {RFCSr,   XLMIB_RFCSr,   CLMIB_RFCSr,   GRFCSr},
                                    {RJBRr,   XLMIB_RJBRr,   CLMIB_RJBRr,   GRJBRr},
                                    {RMTUEr,  XLMIB_RMTUEr,  CLMIB_RMTUEr,  GRMTUEr},
                                    {RTRFUr,  XLMIB_RTRFUr,  CLMIB_RTRFUr,  INVALIDr},
                                    {RERPKTr, XLMIB_RERPKTr, CLMIB_RERPKTr, GRCDEr},
                                    {RRPKTr,  XLMIB_RRPKTr,  CLMIB_RRPKTr,  GRRPKTr},
                                    {RUNDr,   XLMIB_RUNDr,   CLMIB_RUNDr,   GRUNDr},
                                    {RFRGr,   XLMIB_RFRGr,   CLMIB_RFRGr,   GRFRGr},
                                    {RRBYTr,  XLMIB_RRBYTr,  CLMIB_RRBYTr,  GRRBYTr},
                                    {TJBRr,   XLMIB_TJBRr,   CLMIB_TJBRr,   GTJBRr},
                                    {TFCSr,   XLMIB_TFCSr,   CLMIB_TFCSr,   GTFCSr},
                                    {TERRr,   XLMIB_TERRr,   CLMIB_TERRr,   GTXCLr},
                                    {TFRGr,   XLMIB_TFRGr,   CLMIB_TFRGr,   GTFRGr},
                                    {TRPKTr,  XLMIB_TRPKTr,  CLMIB_TRPKTr,  INVALIDr},
                                    {TUFLr,   XLMIB_TUFLr,   CLMIB_TUFLr,   INVALIDr},
                                    {TPCEr,   TPCE_64r,      TPCE_64r,      TPCE_64r},
                                    {RDISCr,  RDISC_64r,     RDISC_64r,     RDISC_64r},
                                    {RIPHE4r, RIPHE4_64r,    RIPHE4_64r,    RIPHE4_64r},
                                    {RIPHE6r, RIPHE6_64r,    RIPHE6_64r,    RIPHE6_64r},
                                    {RIPD4r,  RIPD4_64r,     RIPD4_64r,     RIPD4_64r},
                                    {RIPD6r,  RIPD6_64r,     RIPD6_64r,     RIPD6_64r},
                                    {RPORTDr, RPORTD_64r,    RPORTD_64r,    RPORTD_64r}
                                 };

    PBMP_ITER(lp->pbm, pt) {
        column = 0;
        if (soc_feature(unit, soc_feature_cxl_mib)) {
            pblk = SOC_PORT_BLOCK(unit, SOC_INFO(unit).port_l2p_mapping[pt]);
            if (SOC_BLOCK_IS_CMP(unit, pblk, SOC_BLK_CLPORT)) {
                column = 2;
            } else if (IS_QSGMII_PORT(unit, pt) ||
                       IS_EGPHY_PORT(unit, pt)) {
                column = 3;
            } else if (SOC_BLOCK_IS_CMP(unit, pblk, SOC_BLK_XLPORT)) {
                column = 1;
            }
        }
        for (j = 0; j < (sizeof(error_counters) / sizeof(error_counters[0])); j++) {
            if (SOC_REG_IS_VALID(unit, error_counters[j][column])) {
                rv = soc_reg_get(unit, error_counters[j][column], pt, 0, &rdata);

                if (SOC_FAILURE(rv)) {
                    test_error(unit, "\nError reading MIB counter %s",
                                            SOC_REG_NAME(unit, error_counters[j][column]));
                    return -1;
                } else if (!(COMPILER_64_IS_ZERO(rdata))) {
                    test_error (unit, "\n*ERROR: Error counter %s has a non zero value "
                                "for device port %0d",
                                SOC_REG_NAME(unit, error_counters[j][column]), pt);
                    return -1;
                }
            }
        }
    }
    return 0;
}

/*
 * Function:
 *      lbu_snake_check_rate
 * Purpose:
 *      check rate in snake test.
 */
static int
lbu_snake_check_rate(int unit, loopback2_test_t *lw, lb2_port_stat_t  *stats)
{
    bcm_port_t   pt;
    int speed, first = TRUE, cell_num, pipe, dpp_ratio = 10;
    uint32 rate_core, rate_pipe, rate_limit;
    uint64 rate_tolerance, act_rate;
    loopback2_testdata_t   *lp = lw->cur_params;
    soc_reg_t reg = INVALIDr;
    soc_info_t *si;
    int port_c[4] = {0,0,0,0}, max_port_num = 0;

    si = &SOC_INFO(unit);
    COMPILER_64_ZERO(rate_tolerance);
    cell_num = lw->tx_len <= 144 ? 1: (1 + (lw->tx_len - 144 + 207) / 208);
    rate_core = si->frequency * (lw->tx_len + 20) * 8 / cell_num;
    if (SOC_IS_TOMAHAWK2(unit)){
        dpp_ratio = si->dpp_clk_ratio_x10;
    }
    rate_pipe = si->frequency * (lw->tx_len + 20) * 8 * 10 / dpp_ratio;

    PBMP_ITER(lp->pbm, pt) {
        BCM_IF_ERROR_RETURN(soc_port_pipe_get(unit,pt,&pipe));
        if (pipe >= 0 && pipe < 4){
            port_c[pipe]++;
            if (port_c[pipe] > max_port_num) {
                max_port_num = port_c[pipe];
            }
        }
    }
    rate_limit = rate_core < rate_pipe ?
                 rate_core / max_port_num : rate_pipe / max_port_num;

    PBMP_ITER(lp->pbm, pt) {
        if (first) {
            if (soc_feature(unit, soc_feature_cxl_mib)) {
                if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit,
                    SOC_INFO(unit).port_l2p_mapping[pt]), SOC_BLK_CLPORT)) {
                    reg = CLMIB_TBYTr;
                } else if (IS_QSGMII_PORT(unit, pt) ||
                           IS_EGPHY_PORT(unit, pt)) {
                    reg = GTBYTr;
                } else {
                    reg = XLMIB_TBYTr;
                }
            } else {
                reg = TBYTr;
            }
            if (!SOC_REG_IS_VALID(unit, reg)) {
                cli_out("reg %s is not valid", SOC_REG_NAME(unit,reg));
                return -1;
            }
            BCM_IF_ERROR_RETURN(bcm_port_speed_get(unit, pt, &speed));
            if (speed > rate_limit) {
                speed = rate_limit;
            }
            cli_out("port max line rate is  %d MB/s for packet length %d Byte \n",
                     speed, lw->tx_len);
            /*give a 5% margin, rate_tolerance = speed * 1000000 * 95 / 100 */
            COMPILER_64_SET(rate_tolerance, 0, speed);
            COMPILER_64_UMUL_32(rate_tolerance, 950000);
            first = FALSE;
        }

        soc_counter_get_rate(unit, pt, reg, 0, &(stats[pt].rate));
        /*change to real rate in bit.
         *act_rate = stats[pt].rate * 8 * (lw->tx_len + 20) / (lw->tx_len)
         */
        COMPILER_64_COPY(act_rate, stats[pt].rate);
        COMPILER_64_UMUL_32(act_rate, (8 * (lw->tx_len + 20)));
        COMPILER_64_UDIV_32(act_rate, lw->tx_len);

        if (COMPILER_64_LT(act_rate, rate_tolerance)) {
            cli_out("Cannot reach line rate for port %s\n",
                     SOC_PORT_NAME(unit, pt));
            return -1;
        }
    }
    return 0;
}

/*
 * Function:
 *      lbu_snake_check_drop_pkt
 * Purpose:
 *      check packet loss in snake test.
 */

static int
lbu_snake_check_drop_pkt(int unit, loopback2_testdata_t *lp, lb2_port_stat_t  *stats)
{
    bcm_port_t pt;
    int first = TRUE;
    uint64 byte, pkt;
    COMPILER_64_ZERO(byte);
    COMPILER_64_ZERO(pkt);

    PBMP_ITER(lp->pbm, pt) {
        if (first) {
            COMPILER_64_COPY(byte, stats[pt].tbyt);
            COMPILER_64_COPY(pkt, stats[pt].tpkt);
            first = FALSE;
        }
        if (!(COMPILER_64_EQ(byte, stats[pt].rbyt) && COMPILER_64_EQ(byte, stats[pt].tbyt) &&
              COMPILER_64_EQ(pkt, stats[pt].rpkt) && COMPILER_64_EQ(pkt, stats[pt].tpkt))) {
            cli_out("pkt counter mismatch for port %s\n", SOC_PORT_NAME(unit, pt));
            return -1;
        }
    }
    return 0;
}
#endif /*BCM_TOMAHAWK_SUPPORT */

int
lbu_snake_txrx(loopback2_test_t *lw, lb2_port_stat_t  *stats)
/*
 * Function:    lbu_snake_txrx
 * Purpose:     Transmit and receive packets from snake.
 * Parameters:  lw - pointer to work structure
 * Returns:     0 - success
 *              -1- failed.
 */
{
    loopback2_testdata_t   *lp = lw->cur_params;
    int             unit = lw->unit;
    int             rv = BCM_E_NONE;
    int             ix, timer, sleep_time;
    lb2s_port_connect_t *work_info;

    /* This is the init call */
    if (lbu_snake_stats(unit, lp->pbm, lp->loopback, stats) < 0) {
        lbu_snake_dump_stats(unit, lp->pbm, stats);
        return (-1);
    }

    lw->rx_pkt_cnt = 0;
    lbu_snake_tx(lw);

    timer = 0;
    while (lp->duration > timer) {
        sleep_time = lp->interval;

        if (lp->interval > (lp->duration - timer)) {
            sleep_time = (lp->duration - timer);
        }

        sal_sleep(sleep_time);
        timer += sleep_time;
        cli_out("Time elapsed:  %d seconds\n", timer);

        if (!SAL_BOOT_QUICKTURN) {
            if (lbu_snake_stats(unit, lp->pbm, lp->loopback, stats) < 0) {
                lbu_snake_dump_stats(unit, lp->pbm, stats);
                return (-1);
            }
        } else {
            /* JUMBO: */
            uint64 tpkt;
            uint32 tempStat;
            bcm_port_t port;

            work_info = (lw->port_cnct_f != NULL) ? 
                lw->port_cnct_f : lw->port_cnct_r;

            port = work_info[0].this_port;

            COMPILER_64_ZERO(tpkt);    COMPILER_REFERENCE(&tpkt);   
            bcm_stat_get(unit, port, 
                         snmpDot1dTpPortOutFrames, &tpkt);
            COMPILER_64_TO_32_LO(tempStat, tpkt);
            cli_out("Port %s Tx Count = 0x%x\n", 
                    SOC_PORT_NAME(unit, port), 
                    tempStat);
        }
    }
#ifdef BCM_TOMAHAWK_SUPPORT
    /*check line rate*/
    if (lp->line_rate){
        if(lbu_snake_check_rate(unit, lw, stats) < 0) {
           lbu_snake_dump_stats(unit, lp->pbm, stats);
            return -1;
        }
    }
#endif
    /* Redirect packets to CPU if loop was closed */
    if (lp->duration) {
        lw->expect_pkts = TRUE;
        if (lw->port_cnct_f != NULL) {
            work_info = &(lw->port_cnct_f[lw->port_count]);
            if ((rv = lbu_connect_ports(unit, work_info, TRUE)) < 0) {
                test_error(unit,
                           "CPU redirect failure on forward loop: %s\n",
                           soc_errmsg(rv));
                return -1;
            }
        }

        if (lw->port_cnct_r != NULL) {
            work_info = &(lw->port_cnct_r[lw->port_count]);
            if ((rv = lbu_connect_ports(unit, work_info, TRUE)) < 0) {
                test_error(unit,
                           "CPU redirect failure on reverse loop: %s\n",
                           soc_errmsg(rv));
                return -1;
            }
        }
    }

    bcm_stat_sync(unit);

    if (sal_sem_take(lw->sema, lw->timeout_usec)) {
        cli_out("Time-out waiting for snake completion\n");
        cli_out("Receive count is %d; expecting %d pkts.\n",
                lw->rx_pkt_cnt, lw->tx_ppt);
        lbu_snake_stats(unit, lp->pbm, lp->loopback, stats);
        lbu_snake_dump_stats(unit, lp->pbm, stats);
        lw->sema_woke = FALSE;
        return (-1);
    }

    lw->sema_woke = FALSE;    
    lw->expect_pkts = FALSE; /* Let RX handler ignore packets */

    if (lbu_snake_analysis(lw) < 0) {
        if (lbu_snake_stats(unit, lp->pbm, lp->loopback, stats) < 0) {
            if (lw->port_cnct_f != NULL) {
                lbu_snake_diag(lw, stats, lw->port_cnct_f);
            }
            if (lw->port_cnct_r != NULL) {
                lbu_snake_diag(lw, stats, lw->port_cnct_r);
            }
        }
        lbu_snake_dump_stats(unit, lp->pbm, stats);
        return (-1);
    }

    rv = 0;
    for (ix = 0; ix < lw->tx_ppt; ix++) {
        if (lw->tx_pkt_match[ix] == 0) {
            rv = -1;
            test_error(unit,
                       "TX packet %d did not return to CPU\n", (ix + 1));
        }
    }

    if (lbu_snake_stats(unit, lp->pbm, lp->loopback, stats) < 0) {
        rv = -1;
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (lp->line_rate){
        if (lbu_snake_check_mib_counters(unit, lp) < 0) {
            return -1;
        }
        /*skip packet drop check for small packets*/
        if (lw->tx_len > 352){
            if (lbu_snake_check_drop_pkt(unit, lp, stats) < 0) {
                rv = -1;
            }
        }
    }
#endif

    lbu_snake_dump_stats(unit, lp->pbm, stats);

    if (rv < 0) {
        return rv;
    }

    /* Close loop again */
    if (lp->duration) {
        if (lw->port_cnct_f != NULL) {
            work_info = &(lw->port_cnct_f[lw->port_count]);
            if ((rv = lbu_connect_ports(unit, work_info, FALSE)) < 0) {
                test_error(unit,
                           "CPU undirect failure on forward loop: %s\n",
                           soc_errmsg(rv));
                return -1;
            }
           
            work_info = &(lw->port_cnct_f[lw->port_count - 1]);
            if ((rv = lbu_connect_ports(unit, work_info, TRUE)) < 0) {
                test_error(unit,
                           "Forward loop closure failed: %s\n",
                           soc_errmsg(rv));
                return -1;
            }
        }

        if (lw->port_cnct_r != NULL) {
            work_info = &(lw->port_cnct_r[lw->port_count]);
            if ((rv = lbu_connect_ports(unit, work_info, FALSE)) < 0) {
                test_error(unit,
                           "CPU redirect failure on reverse loop: %s\n",
                           soc_errmsg(rv));
                return -1;
            }
           
            work_info = &(lw->port_cnct_r[lw->port_count - 1]);
            if ((rv = lbu_connect_ports(unit, work_info, TRUE)) < 0) {
                test_error(unit,
                           "Reverse loop closure failed: %s\n",
                           soc_errmsg(rv));
                return -1;
            }
        }
    }

    /* Clean up packet data */
    CLEAR_RX_PKTS(lw, lp);
    sal_memset(lw->tx_pkt_match, 0, lp->ppt_end * sizeof(int));

    return 0;
}

void
lbu_setup(int unit, loopback2_test_t *lw)
/*
 * Function:    lbu_setup
 * Purpose:     Fill in defaults for the parameters.
 * Parameters:  unit - unit #.
 *              lw - pointer to work struct.
 * Returns:     Nothing
 */
{
    int                  num_cos;

    /* Keep statistics from printing if user ^C's before test starts */
    lw->stats_init = FALSE;

    if (lw->set_up) {
        return;
    }

    if (bcm_cosq_config_get(unit, &num_cos) < 0 || num_cos == 0) {
        num_cos = 1;
    }

    lw->unit = unit;

    /* Set up parameters for different tests */
    mac_params_init(unit, num_cos, &lw->params[LB2_TT_MAC]);
    phy_params_init(unit, num_cos, &lw->params[LB2_TT_PHY]);
    ext_params_init(unit, num_cos, &lw->params[LB2_TT_EXT]);
    snake_params_init(unit, num_cos, &lw->params[LB2_TT_SNAKE]);

    lw->set_up = TRUE;
}

STATIC int
lbu_restore_port(loopback2_test_t *lw)
{
    loopback2_testdata_t        *lp = lw->cur_params;
    int         rv;
    int         unit = lw->unit;
    soc_port_t  port;
    pbmp_t      save_ports;
    int         ms;

    if (SOC_IS_XGS12_FABRIC(unit)) {
        SOC_PBMP_ASSIGN(save_ports, PBMP_PORT_ALL(unit));
    } else {
        SOC_PBMP_ASSIGN(save_ports, PBMP_PORT_ALL(unit));
        SOC_PBMP_AND(save_ports, lp->pbm);
    }

    PBMP_ITER(save_ports, port) {
       /* If port is FE port, remove phy_master attributes controlled by it */
       if(IS_FE_PORT(unit, port)) {
          lw->save_port[port].action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
       }
        /* If the Master mode is not changed, remove phy_master attributes from action_mask*/
        ms = 0;
        if ((rv = bcm_port_master_get(unit, port, &ms)) != BCM_E_NONE) {
            test_error(unit,
                       "Port %s: Could not get port Master mode: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            return (-1);
        }
        if (ms == lw->save_port[port].phy_master) {
            lw->save_port[port].action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
        }
        if ((rv = bcm_port_info_restore(unit, port,
                                    &lw->save_port[port])) < 0) {
            test_error(unit,
                       "Port %s: Could not restore port info: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            return(-1);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Restore original fec, phy config and link training info. Note pm8x50
         * (cd ports) have this additional configuration information
         */
        if ((SOC_IS_TOMAHAWK3(unit)) && (IS_CD_PORT(unit, port))) {
            rv = bcm_port_resource_speed_set(unit, port,
                                             &lw->port_info[port]);
            if (rv < 0) {
                test_error(unit,
                           "Port %s: bcm_port_resource_set failed: %s\n",
                           SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                return(-1);
            }
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
    }

    if (SOC_IS_XGS_SWITCH(unit)) {
        /* Restore source port/module for ipic */
        SOC_DEFAULT_DMA_SRCMOD_SET(unit, lbu_saved_srcmod[unit]);
        SOC_DEFAULT_DMA_SRCPORT_SET(unit, lbu_saved_srcport[unit]);
    }
#ifdef BCM_TRIUMPH_SUPPORT
    if(SOC_IS_TRX(unit)){
        PBMP_ITER(save_ports, port) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (SOC_MEM_IS_VALID(unit, EGR_VLAN_CONTROL_1m)) {
                if (soc_feature(unit, soc_feature_egr_all_profile)) {
                    BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit,
                        port, EGR_VLAN_CONTROL_1m, REMARK_OUTER_DOT1Pf, 1));
                } else {
                    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        EGR_VLAN_CONTROL_1m, port, REMARK_OUTER_DOT1Pf, 1));
                }
            } else
#endif
            if ((rv = soc_reg_field32_modify(unit, EGR_VLAN_CONTROL_1r, port,
                                             REMARK_OUTER_DOT1Pf, 0x1)) < 0) {
                test_error(unit,
                       "Unable to re-enable outer dot1p remarking: port %s: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                return (-1);
            }
        }
    }
#endif
    return(0);
}

int
lbu_done(loopback2_test_t *lw)
/*
 * Function:    lbu_done
 * Purpose:     Generic loopback done function
 * Parameters:  lw - pointer to lw to clean up.
 * Returns:
 */
{
    int         rv = 0;
    loopback2_testdata_t        *lp = lw->cur_params;
    int unit = lw->unit;

    if (lp) {
        if (!lp->inject) {
            /* Unregister the callback */
            if ((rv = bcm_rx_unregister(lw->unit, lbu_rx_callback,
                                        BCM_RX_PRIO_MAX)) < 0) {
                test_error(lw->unit,
                           "Failed to unregister RX handler.\n");
            }
#if defined(__KERNEL__) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT))
            (void)soc_cmicm_cmcx_intr0_disable(unit, 0, 0xff00);
            sal_usleep(500);
#endif
            /* lbu_restore_port resets initial state below */
            if ((rv = bcm_rx_stop(lw->unit, NULL)) < 0) {
                cli_out("lbu_done: could not stop packet driver: %s\n",
                        bcm_errmsg(rv));
            }

            if (lw->tx_pkts) {
                bcm_pkt_blk_free(unit, lw->tx_pkts, lp->ppt_end);
                lw->tx_pkts = NULL;
            }
            if (lw->rx_pkts) {
                CLEAR_RX_PKTS(lw, lp);
                sal_free(lw->rx_pkts);
                lw->rx_pkts = NULL;
            }
        }

    }


#if defined(__KERNEL__) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT))
        sal_sem_destroy(lw->sema);
        lw->sema = NULL;
#endif

    if (lw->sema) {
        sal_sem_destroy(lw->sema);
        lw->sema = NULL;
    }

    /* Clean up ARL if interrupted in middle */
    lbu_cleanup_arl(lw);

    /* ENABLE ALL PORTS */
    if (lp) {
        rv |= lbu_restore_port(lw);
    }

    lp = NULL;

#if defined(__KERNEL__) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT))
    bcm_vlan_destroy_all(unit);
#endif

    return(rv);
}

/* Test parameter init routines */

STATIC void
mac_params_init(int unit, int num_cos, loopback2_testdata_t *lp)
{
    int                 lene_ignore = 0;

    lp->test_type       = LB2_TT_MAC;

    SOC_PBMP_ASSIGN(lp->pbm, PBMP_PORT_ALL(unit));
    lp->dst_inc         = 0;
    SOC_PBMP_CLEAR(lp->ubm);
    lp->pattern         = 0xa5a4a3a2;
    lp->pattern_inc     = 0;
    lp->vlan            = LB2_VLAN_ID;
    lp->d_port          = CMIC_PORT(unit);
    if (SOC_IS_XGS_SWITCH(unit)) {
        if (bcm_stk_my_modid_get(unit, &lp->d_mod) < 0) {
            lp->d_mod = 0;
        }
    } else {
#ifdef BCM_XGS_SUPPORT
        lp->d_mod       = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_DST_MOD_CPU : 0;
#else
        lp->d_mod       = 0;
#endif
    }
#ifdef BCM_XGS_SUPPORT
    lp->opcode          = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_OP_UC : 0;
#else
    lp->opcode          = 0;
#endif
    lp->len_start       = 68;
    lp->len_end         = 1522;
    lp->len_inc         = 512;
    lp->ppt_start       = 1;
    lene_ignore         = lp->len_end;
    (void)_lb_ppte_initval(unit, &lene_ignore, &lp->ppt_end);
    lp->ppt_inc         = lp->ppt_end - lp->ppt_start;
#ifdef BCM_TOMAHAWK_SUPPORT
    lp->line_rate       = 0;
    lp->line_rate_count = 1;
#endif
    lp->cos_start       = 0;
    lp->cos_end         = num_cos - 1;
    lp->iterations      = 10;
    lp->crc_mode        = LB2_CRC_MODE_MAC_REGEN;
    lp->check_data      = TRUE;
    lp->check_crc       = FALSE;
    lp->loopback        = LB2_MODE_MAC;
    lp->inject          = FALSE;
    lp->port_select_mode = LB2_TH_ASCEND_PORT_MODE;
    if (lp->ppt_inc == 0) {
        lp->ppt_inc = 1;
    }

    ENET_SET_MACADDR(lp->mac_src, lbu_mac_src);
    lp->mac_src_inc     = 0;
    ENET_SET_MACADDR(lp->mac_dst, lbu_mac_dst);
    lp->mac_dst_inc     = 0;
}

STATIC void
phy_params_init(int unit, int num_cos, loopback2_testdata_t *lp)
{
    int                 lene_ignore = 0;
    int                 p;
    bcm_port_abil_t     ability;

    lp->test_type       = LB2_TT_PHY;

    SOC_PBMP_ASSIGN(lp->pbm, PBMP_PORT_ALL(unit));

    PBMP_ITER(lp->pbm, p) {
        if (!(IS_CD_PORT(unit, p))) {
            if ( bcm_port_ability_get(unit, p, &ability) != BCM_E_NONE) {
                test_error(unit,"======> ERROR : fail to get ability Port %s\n", SOC_PORT_NAME(unit, p));
            }
            if (!(ability & BCM_PORT_ABIL_LB_PHY)) {
                SOC_PBMP_PORT_REMOVE(lp->pbm, p);
                test_msg("Port %s: Doesn't support PHY loopback. Skip it\n", SOC_PORT_NAME(unit, p));
            }
        }
    }

    lp->dst_inc         = 0;
    SOC_PBMP_CLEAR(lp->ubm);
    lp->pattern         = 0xa5a4a3a2;
    lp->pattern_inc     = 0;
    lp->vlan            = LB2_VLAN_ID;
    lp->d_port          = CMIC_PORT(unit);
    if (SOC_IS_XGS_SWITCH(unit)) {
        if (bcm_stk_my_modid_get(unit, &lp->d_mod) < 0) {
            lp->d_mod = 0;
        }
    } else {
#ifdef BCM_XGS_SUPPORT
        lp->d_mod       = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_DST_MOD_CPU : 0;
#else
        lp->d_mod       = 0;
#endif
    }
#ifdef BCM_XGS_SUPPORT
    lp->opcode          = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_OP_UC : 0;
#else
    lp->opcode          = 0;
#endif
    lp->len_start       = 68;
    lp->len_end         = 1522;
    lp->len_inc         = 512;
    lp->ppt_start       = 1;
    lene_ignore         = lp->len_end;
    (void)_lb_ppte_initval(unit, &lene_ignore, &lp->ppt_end);
    lp->ppt_inc         = lp->ppt_end - lp->ppt_start;
#ifdef BCM_TOMAHAWK_SUPPORT
    lp->line_rate       = 0;
    lp->line_rate_count = 1;
#endif
    lp->cos_start       = 0;
    lp->cos_end         = num_cos - 1;
    lp->iterations      = 10;
    lp->crc_mode        = LB2_CRC_MODE_MAC_REGEN;
    lp->check_data      = TRUE;
    lp->check_crc       = FALSE;
    lp->loopback        = LB2_MODE_PHY;
    lp->inject          = FALSE;
    lp->port_select_mode = LB2_TH_ASCEND_PORT_MODE;

    if (lp->ppt_inc == 0) {
        lp->ppt_inc = 1;
    }

    ENET_SET_MACADDR(lp->mac_src, lbu_mac_src);
    lp->mac_src_inc     = 0;
    ENET_SET_MACADDR(lp->mac_dst, lbu_mac_dst);
    lp->mac_dst_inc     = 0;
}

STATIC void
ext_params_init(int unit, int num_cos, loopback2_testdata_t *lp)
{
    int setport;
    int p;
    int                 lene_ignore = 0;

    lp->test_type       = LB2_TT_EXT;

    SOC_PBMP_CLEAR(lp->pbm);
    /* set up pairs of transmitting and receiving ports */
    SOC_PBMP_CLEAR(lp->pbm_tx);
    setport = 1;
    PBMP_PORT_ITER(unit, p) {
        if (setport) {
            SOC_PBMP_PORT_ADD(lp->pbm_tx, p);
        }
        setport = !setport;
    }
    lp->dst_inc         = 1;
    SOC_PBMP_CLEAR(lp->ubm);
    lp->pattern         = 0xa5a4a3a2;
    lp->pattern_inc     = 0;
    lp->vlan            = LB2_VLAN_ID;
    lp->d_port          = CMIC_PORT(unit);
    if (SOC_IS_XGS_SWITCH(unit)) {
        if (bcm_stk_my_modid_get(unit, &lp->d_mod) < 0) {
            lp->d_mod = 0;
        }
    } else {
#ifdef BCM_XGS_SUPPORT
        lp->d_mod       = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_DST_MOD_CPU : 0;
#else
        lp->d_mod       = 0;
#endif
    }
#ifdef BCM_XGS_SUPPORT
    lp->opcode          = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_OP_UC : 0;
#else
    lp->opcode          = 0;
#endif
    lp->len_start       = 68;
    lp->len_end         = 1522;
    lp->len_inc         = 512;
    lp->ppt_start       = 1;
    lene_ignore         = lp->len_end;
    (void)_lb_ppte_initval(unit, &lene_ignore, &lp->ppt_end);
    lp->ppt_inc         = lp->ppt_end - lp->ppt_start;
#ifdef BCM_TOMAHAWK_SUPPORT
    lp->line_rate       = 0;
    lp->line_rate_count = 1;
#endif
    lp->cos_start       = 0;
    lp->cos_end         = num_cos - 1;
    lp->iterations      = 10;
    lp->crc_mode        = LB2_CRC_MODE_MAC_REGEN;
    lp->check_data      = TRUE;
    lp->check_crc       = FALSE;
    lp->loopback        = LB2_MODE_EXT;
    lp->inject          = FALSE;
    lp->port_select_mode = LB2_TH_ASCEND_PORT_MODE;

    if (lp->ppt_inc == 0) {
        lp->ppt_inc = 1;
    }

    ENET_SET_MACADDR(lp->mac_src, lbu_mac_src);
    lp->mac_src_inc     = 0;
    ENET_SET_MACADDR(lp->mac_dst, lbu_mac_dst);
    lp->mac_dst_inc     = 0;
}

STATIC void
snake_params_init(int unit, int num_cos, loopback2_testdata_t *lp)
{
    int                 lene_ignore = 0;
    int                p;
    bcm_port_abil_t    ability;

    lp->test_type       = LB2_TT_SNAKE;

    SOC_PBMP_ASSIGN(lp->pbm, PBMP_PORT_ALL(unit));

    /* Snake test supported only on Ethernet ports on FB */
    if (SOC_IS_FB_FX_HX(unit)) {
        SOC_PBMP_AND(lp->pbm, PBMP_E_ALL(unit));
    }

    if (lp->loopback == LB2_MODE_PHY) {
        PBMP_ITER(lp->pbm, p) {
            if (!(IS_CD_PORT(unit, p))) {
                if ( bcm_port_ability_get(unit, p, &ability) != BCM_E_NONE) {
                    test_error(unit,"======> ERROR : fail to get ability Port %s\n", SOC_PORT_NAME(unit, p));
                }
                if (!(ability & BCM_PORT_ABIL_LB_PHY)) {
                    SOC_PBMP_PORT_REMOVE(lp->pbm, p);
                    test_msg("Port %s: Doesn't support PHY loopback. Skip it\n", SOC_PORT_NAME(unit, p));
                }
            }
        }
    }

    lp->dst_inc         = 1;
    SOC_PBMP_CLEAR(lp->ubm);
    lp->pattern         = 0xa5a4a3a2;
    lp->pattern_inc     = 0;
    lp->vlan            = LB2_VLAN_ID;
    lp->d_port          = CMIC_PORT(unit);
    if (SOC_IS_XGS_SWITCH(unit)) {
        if (bcm_stk_my_modid_get(unit, &lp->d_mod) < 0) {
            lp->d_mod = 0;
        }
    } else {
#ifdef BCM_XGS_SUPPORT
        lp->d_mod       = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_DST_MOD_CPU : 0;
#else
        lp->d_mod       = 0;
#endif
    }
#ifdef BCM_XGS_SUPPORT
    lp->opcode          = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_OP_UC : 0;
#else
    lp->opcode          = 0;
#endif
    lp->d_mod_inc       = 1;
    lp->len_start       = 68;
    lp->len_end         = 1522;
    lp->len_inc         = 512;
    lp->ppt_start       = 1;
    lene_ignore         = lp->len_end;
    (void)_lb_ppte_initval(unit, &lene_ignore, &lp->ppt_end);
    lp->ppt_inc         = 6;
#ifdef BCM_TOMAHAWK_SUPPORT
    lp->line_rate       = 0;
    lp->line_rate_count = 1;
#endif
    lp->cos_start       = 0;
    lp->cos_end         = num_cos - 1;
    lp->iterations      = 5;
    lp->crc_mode        = SOC_IS_XGS12_FABRIC(unit) ? LB2_CRC_MODE_MAC_REGEN :
                             LB2_CRC_MODE_CPU_APPEND;
    lp->check_data      = FALSE;
    lp->check_crc       = FALSE;
    lp->duration        = 10;
    lp->interval        = sh_set_report_time;
    lp->snake_way       = LB2_SNAKE_INCR;
    lp->port_select_mode = LB2_TH_ASCEND_PORT_MODE;

    lp->loopback        = LB2_MODE_MAC;
    lp->inject          = FALSE;

    if (lp->ppt_inc == 0) {
        lp->ppt_inc = 1;
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_HBX(unit)) {
        int port;
        PBMP_ITER(lp->pbm, port) {
            if (IS_ST_PORT(unit, port)) {
                lp->opcode = SOC_HIGIG_OP_UC;
            }
        }
    }
#endif

    ENET_SET_MACADDR(lp->mac_src, lbu_mac_src);
    lp->mac_src_inc     = 0;
    ENET_SET_MACADDR(lp->mac_dst, lbu_mac_dst);
    lp->mac_dst_inc     = 1;
}

