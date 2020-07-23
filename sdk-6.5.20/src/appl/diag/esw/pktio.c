/*! \file pktio.c
 *
 * ESW Pktio command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_PKTIO)
#include <shared/bsl.h>
#include <shared/pbmp.h>

#include <appl/diag/system.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/decode.h>

#include <bcm/port.h>
#include <bcm/pktio.h>
#include <bcm_int/esw/pktio.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE      BSL_LS_APPL_PKTIO

#define NAME_STR_LEN_MAX            128

#define PKTIO_ENET_MIN_PKT_SIZE     60 /* Does not include CRC */
#define PKTIO_ENET_TPID             0x8100
#define PKTIO_ENET_TAG_SIZE         4

#define PKTIO_DEF_TX_DMAC           {0x00, 0xbc, 0x10, 0x00, 0x00, 0x00}
#define PKTIO_DEF_TX_SMAC           {0x00, 0x01, 0x02, 0x03, 0x04, 0x05}
#define PKTIO_DEF_VLAN              1
#define PKTIO_DEF_PKT_LEN           60  /* Does not include VLAN TAG size. */
#define PKTIO_DEF_PATTERN           0x01020304
#define PKTIO_DEF_PATTERN_INC       0x04040404

/* TX description. */
typedef struct xd_s {
    int         xd_unit;            /* Unit #. */
    enum {
        XD_IDLE,                    /* Not doing anything. */
        XD_RUNNING,                 /* Running. */
        XD_STOP                     /* Stop requested. */
    }           xd_state;

    uint32      xd_tot_cnt;         /* # to send/receive. */
    uint32      xd_pkt_len;         /* packet length. */
    bcm_pbmp_t  tx_pbmp;

    char        *xd_file;           /* File name of packet data */
    char        *xd_data;           /* packet data */

    bcm_pktio_pmd_t xd_pmd;

    /* TX Metadata */
    int         xd_txport;
    int         xd_mcq;
    uint8       xd_prio_int;

    /* Packet generation */
    uint16      xd_tpid;
    int         xd_untag;
    uint16      xd_vlan;            /* VLAN ID. */
    uint16      xd_prio;            /* VLAN ID. */
    uint32      xd_pat;             /* Specific 32-bit data pattern. */
    uint32      xd_pat_inc;         /* Value by which each word of the data
                                       pattern is incremented. */
    int         xd_pat_random;      /* use random pattern. */
    int         xd_ppsm;            /* different source mac for each source port. */
    bcm_mac_t   xd_mac_src;         /* source MAC. */
    uint32      xd_mac_src_inc;     /* source MAC increment value. */
    bcm_mac_t   xd_mac_dst;         /* destination MAC. */
    uint32      xd_mac_dst_inc;     /* destination MAC increment value. */
} xd_t;

static xd_t *xd_units[BCM_MAX_NUM_UNITS];

#define _XD_INIT(unit, xd)          \
    do {                            \
        if (xd_units[unit] == NULL) \
            _xd_init(unit);         \
        if (xd_units[unit] == NULL) \
            return CMD_FAIL;        \
        xd = xd_units[unit];        \
    } while (0)

#define XD_FILE(xd)     ((xd)->xd_file != NULL && (xd)->xd_file[0] != 0)
#define XD_STRING(xd)   ((xd)->xd_data != NULL && (xd)->xd_data[0] != 0)

#define UNTAGGED_PACKET_LENGTH      1024

#define TX_LOAD_MAX                 4096

static char pw_name[BCM_MAX_NUM_UNITS][16];

#define REPORT_COUNT    0x1
#define REPORT_DECODE   0x2
#define REPORT_RAW      0x4
#define REPORT_PMD      0x8

static const parse_pm_t pw_report_table[] = {
    {"Count", REPORT_COUNT},
    {"DECode", REPORT_DECODE},
    {"Raw", REPORT_RAW},
    {"Pmd", REPORT_PMD},
    {"@ALL", ~0},
    {"@*", ~0},
    {NULL, 0}
};

typedef struct pup_s {
    struct pup_s *pup_next, *pup_prev;
    int pup_seqno;
    bcm_pktio_pkt_t *rx_pkt;
} pup_t;

/* Packet Watcher discripter. */
typedef struct pktio_pwu_s {
#define PU_F_RUN    0x1
#define PU_F_STOP   0x2
#define PU_F_SYNC   0x4
    uint32 pu_flags;

    sal_mutex_t pu_lock;
    sal_spinlock_t pu_spinlock;

    uint32 pu_report;
    uint32 pu_dump_options;
    uint32 pu_pmd_op;
    COMPILER_DOUBLE pu_count_last;
    COMPILER_DOUBLE pu_count_time;
    sal_thread_t pu_pid;
    sal_sem_t pu_sema;
    sal_sem_t pu_sync;

    pup_t *pu_pending;
    pup_t *pu_log;
    pup_t *pu_log_free;
    pup_t *pup_list;
    int pu_log_cnt;
#define PU_LOG_CNT  64
    int pu_log_max;
    uint32 pu_packet_received;
    uint8 rx_pri;
    int last_pkt_count;

    int init_done;
} pktio_pwu_t;

static pktio_pwu_t pw_units[BCM_MAX_NUM_UNITS];

#define PW_LOCK(_u)     sal_mutex_take(pw_units[_u].pu_lock, sal_mutex_FOREVER)
#define PW_UNLOCK(_u)   sal_mutex_give(pw_units[_u].pu_lock)
#define PW_SPIN_LOCK(_u)    sal_spinlock_lock(pw_units[_u].pu_spinlock)
#define PW_SPIN_UNLOCK(_u)  sal_spinlock_unlock(pw_units[_u].pu_spinlock)

#define DP_FMT  "%sdata[%04x]: "    /* dump line start format */
#define DP_BPL  16          /* dumped bytes per line */

/******************************************************************************
 * Private functions
 */

static void
_xd_init(int unit)
{
    xd_t *xd;
    bcm_mac_t default_mac_dst = PKTIO_DEF_TX_DMAC;
    bcm_mac_t default_mac_src = PKTIO_DEF_TX_SMAC;

    xd = xd_units[unit];
    if (xd == NULL) {
        xd = sal_alloc(sizeof(xd_t), "xd");
        if (xd == NULL) {
            cli_out("WARNING: xd memory allocation failed\n");
            return;
        }
        sal_memset(xd, 0, sizeof(xd_t));
    }

    xd->xd_unit         = unit;
    xd->xd_state        = XD_IDLE;
    xd->xd_file         = NULL;
    xd->xd_data         = NULL;

    xd->xd_pkt_len      = PKTIO_DEF_PKT_LEN + PKTIO_ENET_TAG_SIZE;
    xd->xd_tpid         = PKTIO_ENET_TPID;
    xd->xd_vlan         = PKTIO_DEF_VLAN;
    xd->xd_pat          = PKTIO_DEF_PATTERN;
    xd->xd_pat_inc      = PKTIO_DEF_PATTERN_INC;
    ENET_SET_MACADDR(xd->xd_mac_dst, default_mac_dst);
    ENET_SET_MACADDR(xd->xd_mac_src, default_mac_src);

    xd_units[unit] = xd;
}

static cmd_result_t
tx_parse(int u, args_t *a, xd_t *xd)
{
    parse_table_t       pt;
    int                 min_len, tagged;

    /* First arg is count */

    if (!ARG_CNT(a) || !isint(ARG_CUR(a))) {
        return(CMD_USAGE);
    }

    _XD_INIT(u, xd);

    xd->xd_tot_cnt = parse_integer(ARG_GET(a));

    parse_table_init(u, &pt);

    parse_table_add(&pt, "PortBitMap",  PQ_DFL|PQ_PBMP|PQ_BCM, 0,
                    &xd->tx_pbmp,       NULL);
    parse_table_add(&pt, "Filename",    PQ_DFL|PQ_STRING, 0,
                    &xd->xd_file,       NULL);
    parse_table_add(&pt, "DATA",        PQ_DFL|PQ_STRING, 0,
                    &xd->xd_data,       NULL);

    parse_table_add(&pt, "Length",      PQ_DFL|PQ_INT,  0,
                    &xd->xd_pkt_len,    NULL);
    parse_table_add(&pt, "Untagged",    PQ_DFL|PQ_BOOL, 0,
                    &xd->xd_untag,      NULL);
    parse_table_add(&pt, "VLantag",     PQ_DFL|PQ_HEX,  0,
                    &xd->xd_vlan,       NULL);
    parse_table_add(&pt, "VlanPrio",    PQ_DFL|PQ_INT,  0,
                    &xd->xd_prio,       NULL);
    parse_table_add(&pt, "Pattern",     PQ_DFL|PQ_HEX,  0,
                    &xd->xd_pat,        NULL);
    parse_table_add(&pt, "PatternInc",  PQ_DFL|PQ_INT,  0,
                    &xd->xd_pat_inc,    NULL);
    parse_table_add(&pt, "PatternRandom", PQ_DFL|PQ_BOOL, 0,
                    &xd->xd_pat_random, NULL);
    parse_table_add(&pt, "PerPortSrcMac", PQ_DFL|PQ_BOOL, 0,
                    &xd->xd_ppsm,       NULL);
    parse_table_add(&pt, "SourceMac",   PQ_DFL|PQ_MAC,  0,
                    &xd->xd_mac_src,    NULL);
    parse_table_add(&pt, "SourceMacInc", PQ_DFL|PQ_INT,  0,
                    &xd->xd_mac_src_inc, NULL);
    parse_table_add(&pt, "DestMac",     PQ_DFL|PQ_MAC,  0,
                    &xd->xd_mac_dst,    NULL);
    parse_table_add(&pt, "DestMacInc",  PQ_DFL|PQ_INT,  0,
                    &xd->xd_mac_dst_inc, NULL);
    parse_table_add(&pt, "PrioInt",     PQ_DFL|PQ_INT,  0,
                    &xd->xd_prio_int,   NULL);
    parse_table_add(&pt, "McQType",     PQ_DFL|PQ_BOOL, 0,
                    &xd->xd_mcq,        NULL);

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(a, &pt)) {
        cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    } else {
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

    tagged = (xd->xd_vlan != 0);
    min_len = tagged ? (PKTIO_ENET_MIN_PKT_SIZE + PKTIO_ENET_TAG_SIZE) :
                       PKTIO_ENET_MIN_PKT_SIZE;

    if (!XD_FILE(xd) && !XD_STRING(xd) && !tagged) {
        cli_out("%s: Warning: Sending untagged packets from CPU "
                "not recommended\n", ARG_CMD(a));
    }

    if (!XD_FILE(xd) && !XD_STRING(xd) && xd->xd_pkt_len < min_len) {
        cli_out("%s: Warning: Length %d too small for %s packet (min %d)\n",
                ARG_CMD(a),
                xd->xd_pkt_len,
                tagged ? "tagged" : "untagged",
                min_len);
        return CMD_FAIL;
    }

    if (xd->xd_untag) {
        xd->xd_vlan = 0;
    }

    return CMD_OK;
}

#ifndef NO_FILEIO

/* Discard the rest of the line */
static int
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

static int
tx_load_byte(FILE *fp, uint8 *byte)
{
    int c, d;

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

static uint8 *
tx_load_packet(int unit, char *fname, uint32 *length)
{
    uint8 *p;
    FILE *fp;
    int i;

    p = (uint8 *)sal_alloc(TX_LOAD_MAX, "tx_packet");
    if (p == NULL) {
        return NULL;
    }

    if ((fp = sal_fopen(fname, "r")) == NULL) {
        sal_free((void *)p);
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

/* Parsing string given by user as packet payload */
static uint8 *
parse_data_packet_payload(int unit, char *packet_data, uint32 *length)
{
    uint8 *p;
    char tmp, data_iter;
    int data_len, i, j, pkt_len, data_base;

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

    p = (uint8 *)sal_alloc(pkt_len, "tx_string_packet");
    if (p == NULL) {
        return NULL;
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
            sal_free((void *)p);
            return NULL;
        }

        /* String input is in 4b unit. Below we're filling in 8b:
           offset is /2, and we shift by 4b if the input char is odd */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
        ++j;
    }

    *length = i / 2;
    return p;
}

/* If data was read from file, extract to XD structure */
static void
check_pkt_fields(xd_t *xd, uint8 *packet_data)
{
    enet_hdr_t *ep;              /* Ethernet packet header */
    uint8 *payload, payload_len;

    ep = (enet_hdr_t *)(packet_data);
    payload = packet_data + sizeof(enet_hdr_t);
    payload_len = xd->xd_pkt_len - sizeof(enet_hdr_t);

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
            ep->en_tag_len = bcm_htons(xd->xd_pkt_len - ENET_TAGGED_HDR_LEN);
            ep->en_tag_tpid = bcm_htons(xd->xd_tpid);
        } else {                                /* Untagged format */
            cli_out("Warning:  Sending untagged packet.\n");
            ep->en_untagged_len =
                bcm_htons(xd->xd_pkt_len - ENET_UNTAGGED_HDR_LEN);
        }
        ENET_SET_MACADDR(ep->en_dhost, xd->xd_mac_dst);
        ENET_SET_MACADDR(ep->en_shost, xd->xd_mac_src);

        if (xd->xd_pat_random) {
            packet_random_store(payload, payload_len);
        } else {
            xd->xd_pat = packet_store(payload, payload_len, xd->xd_pat, 0);
        }
    }
}

static int
tx_send(xd_t *xd, uint8 *pkt_data, uint32 count)
{
    int rv = BCM_E_NONE;
    enet_hdr_t *ep;
    uint8 *payload;
    uint32 payload_len;
    bcm_pktio_pkt_t *txpkt = NULL;
    uint8 *data;

    ep = (enet_hdr_t *)pkt_data;
    payload = pkt_data + sizeof(enet_hdr_t);
    payload_len = xd->xd_pkt_len - sizeof(enet_hdr_t);

    while (count--) {
        if (xd->xd_state != XD_RUNNING) {
            /* Abort */
            break;
        }

        rv = bcm_pktio_alloc(xd->xd_unit, xd->xd_pkt_len, BCM_PKTIO_BUF_F_TX,
                             &txpkt);
        if (BCM_FAILURE(rv)) {
            cli_out("Packet alloc failed.\n");
            break;
        }

        rv = bcm_pktio_put(xd->xd_unit, txpkt, xd->xd_pkt_len, (void *)&data);
        if (BCM_FAILURE(rv)) {
            break;
        }

        rv = bcm_pktio_pkt_data_get(xd->xd_unit, txpkt, (void *)&data, NULL);
        if (BCM_FAILURE(rv)) {
            break;
        }

        sal_memcpy(txpkt->pmd.data, xd->xd_pmd.data,
                   BCM_PKTIO_PMD_SIZE_WORDS * 4);
        sal_memcpy(data, pkt_data, xd->xd_pkt_len);

        if (xd->xd_txport) {
            bcm_pktio_txpmd_t pmd = {0};
            pmd.tx_port = xd->xd_txport;
            pmd.prio_int = xd->xd_prio_int;
            if (xd->xd_mcq) {
                pmd.flags |= BCM_PKTIO_TX_MC_QUEUE;
            }

            rv = bcm_pktio_pmd_set(xd->xd_unit, txpkt, &pmd);
            if (BCM_FAILURE(rv)) {
                cli_out("Tx pmd setup failed.\n");
                break;
            }
        }

        rv = bcm_pktio_tx(xd->xd_unit, txpkt);
        if (BCM_FAILURE(rv)) {
            cli_out("Send packet failed.\n");
            break;
        }

        /* Update payload and MAC for next packet. */
        increment_macaddr(ep->en_dhost, xd->xd_mac_dst_inc);
        increment_macaddr(ep->en_shost, xd->xd_mac_src_inc);

        /* Store pattern */
        if (!XD_FILE(xd) && !XD_STRING(xd) && xd->xd_pat_random) {
            packet_random_store(payload, payload_len);
        } else if (!XD_FILE(xd) && !XD_STRING(xd) && (xd->xd_pat_inc != 0)) {
            xd->xd_pat = packet_store(payload, payload_len,
                                      xd->xd_pat, xd->xd_pat_inc);
        }

        rv = bcm_pktio_free(xd->xd_unit, txpkt);
        if (BCM_FAILURE(rv)) {
            cli_out("Free packet failed.\n");
            break;
        }
        txpkt = NULL;
    }

    if (txpkt) {
        bcm_pktio_free(xd->xd_unit, txpkt);
    }

    return rv;
}

static cmd_result_t
do_tx(xd_t *xd)
{
    uint8 *pkt_data = NULL;
    int rv = BCM_E_INTERNAL;

    /* Allocate the packet; use tx_load if reading from file */
    if (XD_FILE(xd)) {
#ifdef NO_FILEIO
        cli_out("no filesystem\n");
        goto error;
#else
        pkt_data = tx_load_packet(xd->xd_unit, xd->xd_file, &xd->xd_pkt_len);
        if (pkt_data == NULL) {
            cli_out("Unable to load packet from file %s\n", xd->xd_file);
            goto error;
        }
        cli_out("Packet from file: %s, length=%d\n", xd->xd_file,
                xd->xd_pkt_len);
#endif
    } else if (XD_STRING(xd)) {
        /* use parse_data_packet_payload if reading from user string input */
        pkt_data = parse_data_packet_payload(xd->xd_unit,
                                             xd->xd_data, &xd->xd_pkt_len);
        if (pkt_data == NULL) {
            cli_out("Unable to allocate memory or Invalid inputs %s\n",
                    xd->xd_data);
            goto error;
        }
        cli_out("Packet from data=<>, length=%d\n", xd->xd_pkt_len);
    } else {
        pkt_data = (uint8 *)sal_alloc(xd->xd_pkt_len, "tx_string_packet");

        if (pkt_data == NULL) {
            cli_out("Unable to allocate memory\n");
            goto error;
        }
        cli_out("Packet generate, length=%d\n", xd->xd_pkt_len);
    }

    check_pkt_fields(xd, pkt_data);

    /* XMIT all the required packets */
    if (BCM_PBMP_IS_NULL(xd->tx_pbmp)) {
        xd->xd_txport = 0;
        rv = tx_send(xd, pkt_data, xd->xd_tot_cnt);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
    } else {
        int tx_port;
        BCM_PBMP_ITER(xd->tx_pbmp, tx_port) {
            xd->xd_txport = tx_port;
            rv = tx_send(xd, pkt_data, xd->xd_tot_cnt);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
        }
    }

error:
    if (pkt_data) {
        sal_free(pkt_data);
    }

    return(rv == BCM_E_NONE ? CMD_OK : CMD_FAIL);
}

static void
pw_init(int unit)
{
    pktio_pwu_t *pu = &pw_units[unit];

    if (pu->init_done) {
        return;
    }

    sal_snprintf(pw_name[unit], sizeof(pw_name[unit]), "bcmPWN.%d", unit);

    if (!pu->pu_sync) {
        pu->pu_sync = sal_sem_create("pw_sync", sal_sem_BINARY, 0);
    }

    if (!pu->pu_lock) {
        pu->pu_lock = sal_mutex_create("pw_lock");
    }

    if (!pu->pu_spinlock) {
        pu->pu_spinlock = sal_spinlock_create("pw_spinlock");
    }

    if (!pu->pu_sync || !pu->pu_lock || !pu->pu_spinlock) {
        cli_out("%s ERROR:  Could not allocate sync/lock\n", pw_name[unit]);
    }

    pu->pu_pmd_op = 1;/* BCMPKT_RXPMD_FLEX_DUMP_F_NONE_ZERO */
    pu->rx_pri = 100;
    pu->init_done = TRUE;
}

static pup_t *
pktio_pw_pup_alloc(int unit)
{
    pktio_pwu_t *pu = &pw_units[unit];
    pup_t *pup;

    PW_LOCK(unit);
    pup = pu->pu_log_free;
    if (pup) {
        pu->pu_log_free = pup->pup_next;
    }
    PW_UNLOCK(unit);

    return (pup);
}

static void
pktio_pw_pup_free(int unit, pup_t *pup)
{
    pktio_pwu_t *pu = &pw_units[unit];

    pup->pup_next = pu->pu_log_free;
    pu->pu_log_free = pup;

    if (pup->rx_pkt) {
        bcm_pktio_free(unit, pup->rx_pkt);
        pup->rx_pkt = NULL;
    }
}

static bcm_pktio_rx_t
pw_rx_callback(int unit, bcm_pktio_pkt_t *pkt, void *cookie)
{
    pup_t *pup;
    pktio_pwu_t *pu = &pw_units[unit];
    int rv;

    if (pu->pu_report == 0 && pu->pu_log_max == 1) {
        /* No post-processing - just count packet */
        return BCM_PKTIO_RX_HANDLED;
    }

    pup = pktio_pw_pup_alloc(unit);
    if (!pup) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "PW: Failed to allocate pup struct.\n")));
        return BCM_PKTIO_RX_NOT_HANDLED;
    }

    rv = bcm_pktio_claim(unit, pkt, &pup->rx_pkt);
    if (rv < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "PW: Failed to claim rx packet.\n")));
        return BCM_PKTIO_RX_NOT_HANDLED;
    }

    PW_SPIN_LOCK(unit);
    pup->pup_next = NULL;
    if (pu->pu_pending == NULL) {
        pup->pup_prev = pup;
        pu->pu_pending = pup;
    } else {
        pu->pu_pending->pup_prev->pup_next = pup;
        pu->pu_pending->pup_prev = pup;
    }
    PW_SPIN_UNLOCK(unit);

    sal_sem_give(pw_units[unit].pu_sema);

    return BCM_PKTIO_RX_HANDLED;
}

static void
pktio_pw_exit(int unit, int stat)
{
    pktio_pwu_t *pu = &pw_units[unit];
    int rv;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t thread;

    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof(thread_name));

    rv = bcm_pktio_rx_unregister(unit, pw_rx_callback, pu->rx_pri);
    if (rv < 0) {
        cli_out("PW stop error: Cannot unregister handler: %s.\n",
                bcm_errmsg(rv));
    }

    /* pw_exit() is called holding the lock. Give it up
     * momentarily while we stop the RX task.
     */
    PW_UNLOCK(unit);
    PW_LOCK(unit);

    if (pu->pu_sema) {
        sal_sem_destroy(pu->pu_sema);
        pu->pu_sema = 0;
    }

    pu->pu_pid = SAL_THREAD_ERROR;

    if (pu->pu_log != NULL) {
        pu->pu_log->pup_prev->pup_next = NULL;
        while (pu->pu_log != NULL) {
            pup_t *pup = pu->pu_log->pup_next;
            pktio_pw_pup_free(unit, pu->pu_log);
            pu->pu_log = pup;
        }
    }

    if (pu->pup_list) {
        sal_free(pu->pup_list);
    }

    if (pu->pu_flags & PU_F_SYNC) {
        sal_sem_give(pu->pu_sync);
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "PW-Daemon[%d]: Exiting\n"), unit));
    }

    pu->pu_flags = 0;

    PW_UNLOCK(unit);

    if (stat < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "AbnormalThreadExit:%s\n"), thread_name));
    }

    sal_thread_exit(stat);
}

static void
pktio_ether_dump(int unit, char *pfx, uint8 *addr, int len, int offset)
{
    int i = 0, j;

    if (addr == 0) {
        LOG_CLI((BSL_META_U(unit, "Bad packet ADDR!!\n")));
        return;
    }

    if (len > DP_BPL && (DP_BPL & 1) == 0) {
        char    linebuf[128], *s;
        /* Show first line with MAC addresses in curly braces */
        s = linebuf;
        sal_sprintf(s, DP_FMT "{", pfx, i);
        while (*s != 0) s++;
        for (i = offset; i < offset + 6; i++) {
            sal_sprintf(s, "%02x", addr[i]);
            while (*s != 0) s++;
        }
        sal_sprintf(s, "} {");
        while (*s != 0) s++;
        for (; i < offset + 12; i++) {
            sal_sprintf(s, "%02x", addr[i]);
            while (*s != 0) s++;
        }
        sal_sprintf(s, "}");
        while (*s != 0) s++;
        for (; i < offset + DP_BPL; i += 2) {
            sal_sprintf(s, " %02x%02x", addr[i], addr[i + 1]);
            while (*s != 0) s++;
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s\n"), linebuf));
    }

    for (; i < len; i += DP_BPL) {
        char    linebuf[128], *s;
        s = linebuf;
        sal_sprintf(s, DP_FMT, pfx, i);
        while (*s != 0) s++;
        for (j = i; j < i + DP_BPL && j < len; j++) {
            sal_sprintf(s, "%02x%s", addr[j], j & 1 ? " " : "");
            while (*s != 0) s++;
        }
        LOG_CLI((BSL_META_U(unit, "%s\n"), linebuf));
    }
}

static void
pktio_pw_dump_packet(int unit, pup_t *pup, uint32 report)
{
    int rv = BCM_E_NONE;
    bcm_pktio_pkt_t *pkt;
    uint8 *data;
    uint32 len;
    char pfx[64];
    pktio_pwu_t *pu = &pw_units[unit];

    pkt = pup->rx_pkt;

    if (!report) {
        return;
    }

    cli_out("\n");

    sal_sprintf(pfx, "Packet[%d]: ", pup->pup_seqno);

    if ((report & REPORT_COUNT)) {
        cli_out("%sTotal %d\n", pfx, pu->pu_packet_received);
    }

    rv = bcm_pktio_pkt_data_get(unit, pkt, (void *)&data, &len);
    if (BCM_FAILURE(rv)) {
        return;
    }

    if (report & REPORT_RAW) {
        pktio_ether_dump(unit, pfx, data, len, 0);
    }

    cli_out("\n");

    if (report & REPORT_PMD) {
        (void)bcmi_esw_pktio_rx_dump(unit, pkt, pu->pu_pmd_op);
    }

#ifndef NO_SAL_APPL
#ifndef __STRICT_ANSI__
#ifndef __KERNEL__
    if (report & REPORT_DECODE) {
        d_packet_format(pfx, DECODE_ETHER, data, len, NULL);
    }
#endif /* __KERNEL__ */
#endif /* __STRICT_ANSI__ */
#endif /* NO_SAL_APPL */

}

static void
pktio_pw_log_packet(int unit, pup_t *pup)
{
    pktio_pwu_t *pu = &pw_units[unit];

    /*
     * There are 2 cases:
     * 1) First entry
     * 2) Log is full so the LRU packet is dropped (common case)
     * 3) Log is not full, no packet is dropped.
     */
    if (pu->pu_log_cnt == 0) { /* 1) First entry */
        pu->pu_log = pup->pup_next = pup->pup_prev = pup;
        pu->pu_log_cnt = 1;
    } else if (pu->pu_log_cnt == pu->pu_log_max) { /* 2) Full log, drop one */
        pup_t *tp;

        tp = pu->pu_log->pup_prev; /* Entry to delete */
        pup->pup_next = pu->pu_log; /* Link new one in at head */
        pup->pup_prev = tp->pup_prev;
        pup->pup_next->pup_prev = pup;
        pup->pup_prev->pup_next = pup;

        pktio_pw_pup_free(unit, tp);
    } else { /* 3) Just add to chain */
        /*
         * Case 2 - add new entry and increment log count.
         */
        pu->pu_log_cnt++;
        pup->pup_next = pu->pu_log;
        pup->pup_prev = pu->pu_log->pup_prev;
        pup->pup_next->pup_prev = pup;
        pup->pup_prev->pup_next = pup;
    }
    pu->pu_log = pup; /* Set new list */
}

static void
pktio_pw_process_pending(int unit)
{
    pktio_pwu_t *pu = &pw_units[unit];
    pup_t *pup;

    /*
     * Pick up current done list and process in order.
     */
    PW_SPIN_LOCK(unit);
    pup = (pup_t *)pu->pu_pending;
    pu->pu_pending = NULL;
    PW_SPIN_UNLOCK(unit);

    pu->last_pkt_count = 0;
    while (pup) {
        pup_t *tp = pup->pup_next;
        pup->pup_seqno = ++pu->pu_packet_received;
        pktio_pw_log_packet(unit, (pup_t *)pup);
        pktio_pw_dump_packet(unit, (pup_t *)pup, pu->pu_report);
        pup = tp;
        pu->last_pkt_count++;
    }
}

static void
pktio_pw_thread(void *up)
{
    int unit = PTR_TO_INT(up);
    pktio_pwu_t *pu = &pw_units[unit];
    int i;
    int rv;
    int pup_count;
    pup_t *pup;

    PW_LOCK(unit);

    if (0 == pu->pu_log_max) {
        pu->pu_log_max = 64;
    }

    pup_count = pu->pu_log_max * 2;
    pu->pup_list = sal_alloc(sizeof(pup_t) * pup_count, "PW-pup");
    if (pu->pup_list == NULL) {
        pktio_pw_exit(unit, -1);
    }
    /*
     * No forwarding null.
     * If the allocation failed, the call to pktio_pw_exit will end the process
     * and we should never reach here.
     */
    /* coverity[var_deref_model:FALSE] */
    sal_memset(pu->pup_list, 0, sizeof(pup_t) * pup_count);

    for (i = 0; i < pup_count; i++) {
        pup = &pu->pup_list[i];
        /* Put on free list */
        pktio_pw_pup_free(unit, pup);
    }

    if (pu->pu_flags & PU_F_SYNC) {
        pu->pu_flags &= ~PU_F_SYNC;
        sal_sem_give(pu->pu_sync);
    } else {
        cli_out("PW-daemon[%d] -- Started\n", unit);
    }

    /* Kick Start channels */
    rv = bcm_pktio_rx_register(unit, "RX CMD", pw_rx_callback, pu->rx_pri,
                               NULL, 0);
    if (rv < 0) {
        cli_out("PW: Cannot start %s.\n", bcm_errmsg(rv));
        pktio_pw_exit(unit, -1);
    }

    PW_UNLOCK(unit);

    /*
     * Check if there are any actions for us.
     */
    while (1) {
        int     sem_rv;

        sem_rv = sal_sem_take(pw_units[unit].pu_sema, sal_sem_FOREVER);

        if (sem_rv < 0) {
            cli_out("Failed sem_take, exiting\n");
            pktio_pw_exit(unit, -1);
        }

        PW_LOCK(unit);                  /* Block log dumps */

        if (pu->pu_flags & PU_F_STOP) { /* Time to exit */
            pktio_pw_exit(unit, 0);
        }

        pktio_pw_process_pending(unit);

        PW_UNLOCK(unit);
    }

   pktio_pw_exit(unit, 0);
}

static int
pktio_pw_start(int unit, int sync)
{
    pktio_pwu_t *pu = &pw_units[unit];

    pw_init(unit);
    if (pu->pu_sync == NULL || pu->pu_lock == NULL || pu->pu_spinlock == NULL) {
        return -1;
    }

    pu->pu_pending = NULL;
    pu->pu_log = NULL;
    pu->pu_log_free = NULL;
    pu->pu_log_cnt = 0;

    pu->pu_sema = sal_sem_create("pw_thread", sal_sem_BINARY, 0);
    if (pu->pu_sema == 0) {
        return -1;
    }

    pu->pu_flags |= PU_F_RUN;
    if (sync) {
        pu->pu_flags |= PU_F_SYNC;
    }

    pu->pu_pid = sal_thread_create(pw_name[unit], SAL_THREAD_STKSZ, 100,
                                   pktio_pw_thread, INT_TO_PTR(unit));

    if (pu->pu_pid == SAL_THREAD_ERROR) {
        cli_out("%s: Unable to start task\n", pw_name[unit]);
        sal_sem_destroy(pu->pu_sema);
        pu->pu_sema = 0;
        pu->pu_flags &= PU_F_RUN|PU_F_SYNC;
        return -1;
    }

    pu->pu_count_last = 0;
    pu->pu_count_time = SAL_TIME_DOUBLE();

    pu->pu_packet_received = 0;

    if (sync) {
        if (sal_sem_take(pu->pu_sync, sal_sem_FOREVER)) {
            cli_out("%s: Failed to wait for start\n", pw_name[unit]);
            return -1;
        }
    }

    return 0;
}

static int
pktio_pw_stop(int unit, int sync)
{
    pktio_pwu_t *pu = &pw_units[unit];
    int pw_retry = 1000;

    pu->pu_flags |= PU_F_STOP;
    if (sync) {
        pu->pu_flags |= PU_F_SYNC;
    }
    sal_sem_give(pu->pu_sema);

    while (pu->pu_pid != SAL_THREAD_ERROR) {
        if (pw_retry-- == 0) {
            cli_out("pktio_pw_stop: pw_thread did not exit\n");
            pu->pu_pid = SAL_THREAD_ERROR;
            break;
        }
        sal_usleep(10000);
    }

    if  (pu->pu_pid == SAL_THREAD_ERROR) {
        /* Abnormal thread exit: Free semaphore resource */
        if (pu->pu_sema) {
            sal_sem_destroy(pu->pu_sema);
            pu->pu_sema = 0;
        }
    }

    if (sync) {
        (void)sal_sem_take(pu->pu_sync, sal_sem_FOREVER);
    } else {
        cli_out("%s: Termination requested...\n", pw_name[unit]);
    }

    return 0;
}

static cmd_result_t
pktio_pw_dump_log(int unit, int n)
{
    pktio_pwu_t *pu = &pw_units[unit];
    pup_t *pup;
    int abs_n;

    PW_LOCK(unit);

    if ((pu->pu_log == NULL) || (n == 0)) {
        PW_UNLOCK(unit);
        cli_out("pw_dump_log[%d]: Warning: no packets logged "
                "or selected to dump\n", unit);
        return (CMD_OK);
    }

    /*
     * Loop through list of packets, stop when we have dumped the correct
     * number - we know there is at least one logged since we checked above.
     * If dumping the last n packets, move backwards, otherwise move forwards.
     * Anything on this list is complete, so the PW_LOCK protects this
     * operation.
     */
    abs_n = n < 0 ? -n : n;
    abs_n = abs_n > pu->pu_log_cnt ? pu->pu_log_cnt : abs_n;

    if (n < 0) {/* Dump last N packets */
        for (pup = pu->pu_log; abs_n-- != 0; pup = pup->pup_next) {
            pktio_pw_dump_packet(unit, pup, pu->pu_dump_options);
        }
    } else {/* Dump first N packets */
        for (pup = pu->pu_log->pup_prev; abs_n-- != 0; pup = pup->pup_prev) {
            pktio_pw_dump_packet(unit, pup, pu->pu_dump_options);
        }
    }
    PW_UNLOCK(unit);

    return (CMD_OK);
}

/******************************************************************************
 * Public Functions
 */

cmd_result_t
cmd_pktio_txn(int unit, args_t *a)
{
    xd_t *xd;
    cmd_result_t rv;

    _XD_INIT(unit, xd);
    if (xd->xd_state == XD_RUNNING) {
        cli_out("%s: Error: already active\n", ARG_CMD(a));
        return(CMD_FAIL);
    }

    if (CMD_OK != (rv = tx_parse(unit, a, xd))) {
        return(rv);
    }

    xd->xd_state = XD_RUNNING;      /* Say GO */
    rv = do_tx(xd);
    xd->xd_state = XD_IDLE;

    return(rv);
}

cmd_result_t
cmd_pktio_pcktwatch(int unit, args_t *a)
{
    char *c;
    int n;
    uint32 on = 0;
    pktio_pwu_t *pu = &pw_units[unit];

    if (!pu->init_done) {
        pw_init(unit);
    }

    if (!ARG_CNT(a)) {
        cli_out("%s: Status: %s. Buffering up to %d packets.%s\n",
                pw_name[unit],
                (pu->pu_flags & PU_F_RUN) ? "Running" : "Not Running",
                pu->pu_log_max,
                (pu->pu_flags & PU_F_STOP) ? " STOP Requested." : "");

        cli_out("Reporting is enabled for: ");
        parse_mask_format(80, pw_report_table, pu->pu_report);
        cli_out("Reporting is disabled for: ");
        parse_mask_format(80, pw_report_table, pu->pu_report ^ ~0);

        cli_out("Dump options are enabled for: ");
        parse_mask_format(80, pw_report_table, pu->pu_dump_options);
        cli_out("Dump options are disabled for: ");
        parse_mask_format(80, pw_report_table, pu->pu_dump_options ^ ~0);

        return (CMD_OK);
    }

    while ((c = ARG_GET(a)) != NULL) {
        if (!sal_strcasecmp(c, "start")) {
            if (pu->pu_flags & PU_F_RUN) {
                cli_out("%s: WARNING: Already running\n", pw_name[unit]);
            } else {
                on |= PU_F_RUN;
            }
        } else if (!sal_strcasecmp(c, "stop")) {
            if (!(pu->pu_flags & PU_F_RUN)) {
                cli_out("%s: WARNING: Not running\n", pw_name[unit]);
                return (CMD_OK);
            }
            on |= PU_F_STOP;
        } else if (!sal_strcasecmp(c, "reset")) {
            if (pu->pu_flags & PU_F_RUN) {
                cli_out("%s: Unable to reset configuration "
                        "while running\n", pw_name[unit]);
            } else {
                pu->init_done = FALSE;
                pw_init(unit);
                return (CMD_OK);
            }
        } else if (!sal_strcasecmp(c, "quiet")) {
            pu->pu_report = 0;
        } else if (!sal_strcasecmp(c, "report")) {
            if (ARG_CNT(a)) {
                while ((c = ARG_CUR(a)) != NULL &&
                       !parse_mask(c, pw_report_table, &pu->pu_report)) {
                    ARG_NEXT(a);
                }
            } else {
                cli_out("%s: Reporting on for: ", pw_name[unit]);
                parse_mask_format(50, pw_report_table, pu->pu_report);
                cli_out("%s: Reporting off for: ", pw_name[unit]);
                parse_mask_format(50, pw_report_table, ~pu->pu_report);
            }
        } else if (!sal_strcasecmp(c, "log")) {
            c = ARG_GET(a);
            if (c) {
                if (pu->pu_flags & PU_F_RUN) {
                    cli_out("%s: Can not change \"log\" "
                            "while PW-daemon running\n", pw_name[unit]);
                    return (CMD_FAIL);
                }
                if (!sal_strcasecmp(c, "on")) {
                    n = PU_LOG_CNT;
                } else if (!sal_strcasecmp(c, "off")) {
                    n = 1;
                } else if (isint(c)) {
                    n = parse_integer(c);
                    if (n < 1) {
                        cli_out("%s: What is %s?\n", pw_name[unit], c);
                        n = 1;
                    }
                } else {
                    cli_out("%s: Invalid count \"%s\"\n", pw_name[unit], c);
                    return (CMD_FAIL);
                }
                pu->pu_log_max = n;
            }
            cli_out("%s: Logging(%d-packets)\n",
                    pw_name[unit], pu->pu_log_max);
            if (!c || !*c) {
                return (CMD_OK);
            }
        } else if (!sal_strcasecmp(c, "dump")) {
            c = ARG_GET(a);
            if (!c) {
                n = pu->pu_log_cnt;
            } else if (!isint(c)) {
                if (!sal_strcasecmp(c, "options")) {
                    if (ARG_CNT(a)) {
                        while ((c = ARG_CUR(a)) != NULL &&
                               !parse_mask(c, pw_report_table,
                                           &pu->pu_dump_options)) {
                            ARG_NEXT(a);
                        }
                    } else {
                        cli_out("%s: Dump options on for: ", pw_name[unit]);
                        parse_mask_format(50, pw_report_table,
                                          pu->pu_dump_options);
                        cli_out("%s: Dump options off for: ", pw_name[unit]);
                        parse_mask_format(50, pw_report_table,
                                          ~pu->pu_dump_options);
                    }
                    return (CMD_OK);
                } else {
                    cli_out("%s: Invalid log count\"%s\"\n", pw_name[unit], c);
                    return (CMD_FAIL);
                }
            } else {
                n = parse_integer(c);
            }
            return (pktio_pw_dump_log(unit, n));
        } else if (!sal_strcasecmp(c, "count")) {
            COMPILER_DOUBLE cur_time, last_time;
            int cur_count, last_count;
            int rate;

            cur_time = SAL_TIME_DOUBLE();
            last_time = pu->pu_count_time;
            last_count = pu->pu_count_last;
            cur_count = pu->pu_packet_received;

            if (cur_time == last_time) {
                rate = 0;
            } else {
                rate = (int) ((cur_count - last_count) / (cur_time - last_time));
            }

            cli_out("%s: Received %d packets (%d/sec), "
                    "last %d packet(s) logged\n",
                    pw_name[unit], pu->pu_packet_received, rate, pu->pu_log_cnt);
            pu->pu_count_last = cur_count;
            pu->pu_count_time = cur_time;
        } else if (!sal_strcasecmp(c, "pmd")) {
            c = ARG_GET(a);
            if (c) {
                if (!sal_strcasecmp(c, "nonzero")) {
                    pu->pu_pmd_op = 1; /* BCMPKT_RXPMD_FLEX_DUMP_F_NONE_ZERO */
                } else if (!sal_strcasecmp(c, "all")) {
                    pu->pu_pmd_op = 0; /* BCMPKT_RXPMD_FLEX_DUMP_F_ALL */
                } else {
                    cli_out("%s: Invalid pmd \"%s\"\n", pw_name[unit], c);
                    return (CMD_FAIL);
                }
            }
            cli_out("%s: Dump pmd %s\n",
                    pw_name[unit],
                    (pu->pu_pmd_op == 0 /* BCMPKT_RXPMD_FLEX_DUMP_F_ALL */) ?
                    "all" : "nonzero");
            if (!c || !*c) {
                return (CMD_OK);
            }
        } else {
            return (CMD_USAGE);
        }
    }

    if (on & PU_F_RUN) {
        return (pktio_pw_start(unit, TRUE));
    } else if (on & PU_F_STOP) {
        return (pktio_pw_stop(unit, TRUE));
    }

    return (CMD_OK);
}

int
pktio_pw_running(int unit)
{
    return pw_units[unit].pu_flags & PU_F_RUN;
}

int
pktio_pw_clean(int unit)
{
    return pktio_pw_stop(unit, TRUE);
}

#else
typedef int bcm_appl_diag_esw_pktio_not_empty; /* Make ISO compilers happy. */
#endif
