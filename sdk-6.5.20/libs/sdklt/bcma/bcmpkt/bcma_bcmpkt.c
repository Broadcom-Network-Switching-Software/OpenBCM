/*! \file bcma_bcmpkt.c
 *
 * General functions for Packet I/O command Lines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd.h>
#include <bcma/bcmpkt/bcma_bcmpkt_rx.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_rxpmd.h>
#include <bcmpkt/bcmpkt_rxpmd_defs.h>
#include <bcmpkt/bcmpkt_hg3.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_base_t.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t.h>
#include <bcmpkt/flexhdr/bcmpkt_rxpmd_flex.h>
#include <bcmpkt/flexhdr/bcmpkt_rxpmd_flex_defs.h>
#include "cmd/bcma_bcmpktcmd_internal.h"
#include <bcma/io/bcma_io_file.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmlt/bcmlt.h>
#include <bcmlrd/bcmlrd_conf.h>

#define BSL_LOG_MODULE BSL_LS_APPL_PACKET

/*! Packet received output file name. */
static char *px_watcher_file = NULL;

/*! Handle of the packet received output file. */
static bcma_io_file_handle_t px_watcher_fh = NULL;

/*! Option of display packet on console. Enabled by default. */
static int px_watcher_console_display = 1;

/*! Convert two hex-characters to an integer */
static int
xbyte2i(int xch_hi, int xch_lo)
{
    if ((xch_hi = shr_util_xch2i(xch_hi)) < 0) {
        xch_hi = 0;
    }
    if ((xch_lo = shr_util_xch2i(xch_lo)) < 0) {
        xch_lo = 0;
    }
    return (xch_hi << 4) | xch_lo;
}

/* Ignore comments. */
static int
file_discard_comments(bcma_io_file_handle_t fh)
{
    int c;

    do {
        if ((c = bcma_io_file_getc(fh)) == -1) {
            return c;
        }
    } while ( c != '\n');

    return c;
}

/*
 * Load each byte of packet data from file:
 * c - byte high 4 bits.
 * d - byte low 4 bits.
 */
static int
file_load_one_byte(bcma_io_file_handle_t fh, uint8_t *byte)
{
    int c, d;

    /* Parse first char. */
    do {
        if ((c = bcma_io_file_getc(fh)) == -1) {
            return -1;
        }
        else if (c == '#') { /* Ignore comments. */
            if ((c = file_discard_comments(fh)) == -1) {
                return -1;
            }
        }
    } while (!sal_isxdigit(c));

    /* Parse second char. */
    do {
        if ((d = bcma_io_file_getc(fh)) == -1) {
            return -1;
        }
        else if (d == '#') {/* Ignore comments. */
            if ((d = file_discard_comments(fh)) == '\0') {
                return -1;
            }
        }
    } while (!sal_isxdigit(d));

    /* Compose a byte. */
    *byte = xbyte2i(c, d);

    return 0;
}

/*
 * Random pattern.
 */
static void
packet_payload_fill_random(uint8_t *buf, int size)
{
    int      pat_off = 24;
    /* coverity[dont_call] */
    uint32_t pat = sal_rand();

    while (size > 3) {
        *buf++ = pat >> 24;
        *buf++ = pat >> 16;
        *buf++ = pat >> 8;
        *buf++ = pat & 0xff;
        /* coverity[dont_call] */
        pat = sal_rand();
        size -= 4;
    }

    while (size > 0) {
        *buf++ = pat >> pat_off;
        pat_off -= 8;
        size--;
    }
}

/* Pattern increase by word. */
static uint32_t
packet_payload_fill_pattern(uint8_t *buf, int size, uint32_t pat, uint32_t inc)
{
    int pat_off = 24;

    while (size > 3) {
        *buf++ = pat >> 24;
        *buf++ = pat >> 16;
        *buf++ = pat >> 8;
        *buf++ = pat & 0xff;
        pat += inc;
        size-=4;
    }

    while (size > 0) {
        *buf++ = pat >> pat_off;
        pat_off -= 8;
        size--;
    }

    return pat;
}

void
bcma_bcmpkt_macaddr_inc(shr_mac_t macaddr, int amount)
{
    int i, v;
    for (i = 5; i >= 0; i--) {
        v = (int) macaddr[i] + amount;
        macaddr[i] = v;
        if (v >= 0 && v <= 255)
            break;
        amount = v >> 8;
    }
}

int
bcma_bcmpkt_packet_payload_fill(bcma_bcmpkt_pktgen_cfg_t *cfg,
                                bcmpkt_data_buf_t *dbuf)
{
    int payload_len;
    int offset = 0;
    uint8_t *filladdr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf->data, SHR_E_PARAM);

    offset = ENET_MAC_SIZE * 2 + 2 /* Ethertype. */;

    /* Tagged. */
    if (!cfg->untagged) {
        offset += 4;
    }

    payload_len = dbuf->data_len - offset;
    filladdr = dbuf->data + offset;
    /* Fill pattern */
    if (cfg->pat_random) {
        packet_payload_fill_random(filladdr, payload_len);
    }
    else {
        cfg->pattern = packet_payload_fill_pattern(filladdr, payload_len,
                                                   cfg->pattern, cfg->pat_inc);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Load data from input string. */
int
bcma_bcmpkt_load_data_from_istr(char *istr, uint8_t *buf, uint32_t buf_size,
                                uint32_t *data_size)
{
    uint32_t byte, chr, len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(istr, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);
    SHR_NULL_CHECK(data_size, SHR_E_PARAM);

    len = sal_strlen(istr);
    byte = 0;
    for (chr = 0; chr < (len - 1); chr += 2) {
        if (chr == 0 && istr[chr] == '0' &&
            (istr[chr + 1] == 'x' || istr[chr + 1] == 'X')) {
            chr += 2;
            if (chr > (len - 2)) {
                break;
            }
        } else if (istr[chr] == '_') {
            chr++;
            if (chr > (len - 2)) {
                break;
            }
        }
        if (!sal_isxdigit(istr[chr]) || !sal_isxdigit(istr[chr + 1])) {
            LOG_ERROR(BSL_LS_APPL_PACKET,
                      (BSL_META("input data error %c%c\n"), istr[chr],
                       istr[chr + 1]));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        buf[byte++] = xbyte2i(istr[chr], istr[chr + 1]);

        if (byte == buf_size) {
            chr += 2;
            break;
        }
    }
    if (len != chr) {
        LOG_ERROR(BSL_LS_APPL_PACKET,
                  (BSL_META("Input data error at character %d\n"), chr + 1));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *data_size = byte;
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_load_data_from_file(char *fname, bcmpkt_data_buf_t *dbuf)
{
    bcma_io_file_handle_t fh;
    uint32_t i, size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(fname, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf->data, SHR_E_PARAM);

    if ((fh = bcma_io_file_open(fname, "r")) == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET,
                  (BSL_META("Open file %s failed\n"), fname));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    size = bcmpkt_tailroom(dbuf);

    for (i = 0; i < size; i++) {
        if (file_load_one_byte(fh, &dbuf->data[i]) < 0)
            break;
    }

    dbuf->data_len = i;

    bcma_io_file_close(fh);

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_packet_generate(bcma_bcmpkt_pktgen_cfg_t *cfg,
                            bcmpkt_data_buf_t *dbuf)
{
    uint8_t *filladdr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf->data, SHR_E_PARAM);

    if (bcmpkt_tailroom(dbuf) < cfg->len) {
        LOG_ERROR(BSL_LS_APPL_PACKET,
                  (BSL_META("Configure length is too big\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    if(bcmpkt_put(dbuf, cfg->len) == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    sal_memset(dbuf->data, 0, cfg->len);
    filladdr = dbuf->data;
    sal_memcpy(filladdr, cfg->dmac, ENET_MAC_SIZE);
    filladdr += ENET_MAC_SIZE;
    sal_memcpy(filladdr, cfg->smac, ENET_MAC_SIZE);
    filladdr += ENET_MAC_SIZE;

    /* Tagged. */
    if (!cfg->untagged) {
        *filladdr++ = cfg->tpid >> 8;
        *filladdr++ = cfg->tpid & 0xff;
        *filladdr++ = (cfg->dei << 4) | (cfg->pcp << 5) | (cfg->vlan >> 8);
        *filladdr++ = cfg->vlan & 0xff;
    }
    *filladdr++ = cfg->ethertype >> 8;
    *filladdr++ = cfg->ethertype & 0xff;

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_packet_payload_fill(cfg, dbuf));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_lmatch_check(const char *dst, const char *src, int size)
{
    int len;
    char *name = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(dst, SHR_E_PARAM);
    SHR_NULL_CHECK(src, SHR_E_PARAM);

    len = sal_strlen(dst);
    if (len >= size) {
        SHR_ALLOC(name, size + 1, "bcmaBcmpktLmatchCheck");
        if (name != NULL) {
            sal_memcpy(name, dst, size);
            name[size] = '\0';
            if (!sal_strcasecmp(name, src)) {
                SHR_EXIT();
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_FAIL);
exit:
    SHR_FREE(name);
    SHR_FUNC_EXIT();
}

void
bcma_bcmpkt_data_dump(shr_pb_t *pb, const uint8_t *data, int size)
{
    int idx;

    if (data == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer\n")));
        return;
    }

    if (size > 256) {
        size = 256;
    }

    for (idx = 0; idx < size; idx++) {
        if ((idx & 0xf) == 0) {
            shr_pb_printf(pb, "%04x: ", idx);
        }
        if ((idx & 0xf) == 8) {
            shr_pb_printf(pb, "- ");
        }
        shr_pb_printf(pb, "%02x ", data[idx]);
        if ((idx & 0xf) == 0xf) {
            shr_pb_printf(pb, "\n");
        }
    }
    if ((idx & 0xf) != 0) {
        shr_pb_printf(pb, "\n");
    }
}

static int
watcher_info_show(int unit, int netif_id, bcmpkt_packet_t *packet,
                  watcher_data_t *wdata)
{
    uint32_t *rxpmd;
    uint32_t dev_type;
    shr_pb_t *pb = NULL;
    int locked = FALSE;
    uint32_t *flexdata = NULL;
    uint32_t flexdata_len = 0;
    uint32_t flex_profile = 0;
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    if (!(wdata->debug_mode & (WATCHER_DEBUG_SHOW_PACKET_DATA |
                                WATCHER_DEBUG_SHOW_META_DATA |
                                WATCHER_DEBUG_SHOW_RX_REASON))) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_EXIT(bcmpkt_rxpmd_get(packet, &rxpmd));

    sal_mutex_take(wdata->pb_mutex, SAL_MUTEX_FOREVER);
    locked = TRUE;

    pb = shr_pb_create();

    shr_pb_printf(pb, "\nWatch information: (interface %d)\n", netif_id);

    SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd, &flexdata, &flexdata_len),
            SHR_E_UNAVAIL);

    if (wdata->debug_mode & WATCHER_DEBUG_SHOW_META_DATA) {
        shr_pb_printf(pb, "[RX metadata information]\n");
        bcmpkt_rxpmd_dump(dev_type, rxpmd, BCMPKT_RXPMD_DUMP_F_NONE_ZERO, pb);
        if (flexdata_len) {
            variant = bcmlrd_variant_get(unit);
            if (variant == BCMLRD_VARIANT_T_NONE) {
                SHR_IF_ERR_EXIT(SHR_E_UNIT);
            }

            bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                   BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE, &flex_profile);
            shr_pb_printf(pb, "[FLEX fields]\n");
            bcmpkt_rxpmd_flex_dump(variant, flexdata, flex_profile,
                                   BCMPKT_RXPMD_FLEX_DUMP_F_NONE_ZERO, pb);
        }
    }

    if (wdata->debug_mode & WATCHER_DEBUG_SHOW_RX_REASON) {
        shr_pb_printf(pb, "[RX reasons]\n");
        if (flexdata_len) {
            variant = bcmlrd_variant_get(unit);
            if (variant == BCMLRD_VARIANT_T_NONE) {
                SHR_IF_ERR_EXIT(SHR_E_UNIT);
            }

            bcmpkt_rxpmd_flex_reason_dump(variant, flexdata, pb);
        } else {
            bcmpkt_rx_reason_dump(dev_type, rxpmd, pb);
        }
    }

    if (wdata->debug_mode & WATCHER_DEBUG_SHOW_PACKET_DATA) {
        shr_pb_printf(pb, "Packet raw data (%d):\n", BCMPKT_PACKET_LEN(packet));
        bcma_bcmpkt_data_dump(pb, BCMPKT_PACKET_DATA(packet),
                              BCMPKT_PACKET_LEN(packet));
    }

    if (px_watcher_console_display) {
        cli_out("%s\n", shr_pb_str(pb));
    }

    /* Log the recevied packet to specified file. */
    if (px_watcher_fh) {
        bcma_io_file_puts(px_watcher_fh, shr_pb_str(pb));
    }

exit:
    if (locked) {
        sal_mutex_give(wdata->pb_mutex);
    }
    shr_pb_destroy(pb);
    SHR_FUNC_EXIT();
}

static int
watcher_packet_lpbk_tx(int unit, int netif_id, bcmpkt_packet_t *packet)
{
    int port;
    shr_pb_t *pb;
    uint32_t *rxpmd;
    uint32_t dev_type;
    bcmpkt_packet_t *txpkt = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_EXIT(bcmpkt_rxpmd_get(packet, &rxpmd));

    SHR_IF_ERR_EXIT(bcmpkt_packet_claim(packet->unit, packet, &txpkt));

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_field_get(dev_type, rxpmd, BCMPKT_RXPMD_SRC_PORT_NUM,
                                (uint32_t *)&port));

    SHR_IF_ERR_EXIT(bcmpkt_fwd_port_set(dev_type, port, txpkt));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb, "Data buf: (Line %d)\n", __LINE__);
        bcmpkt_data_buf_dump(txpkt->data_buf, pb);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    SHR_IF_ERR_EXIT(bcmpkt_tx(unit, netif_id, txpkt));

exit:
    if (txpkt != NULL) {
        bcmpkt_free(unit, txpkt);
        txpkt = NULL;
    }
    SHR_FUNC_EXIT();
}

static void
watcher_rx_rate_show(int netif_id, watcher_data_t *wdata)
{
    if (!(wdata->debug_mode & WATCHER_DEBUG_SHOW_RX_RATE)) {
        return;
    }

    wdata->rx_packets++;
    if (wdata->rx_packets == 1) {
        wdata->start_time = sal_time_usecs();
    }
    if (wdata->rx_packets == 100000) {
        sal_usecs_t delt_ms = (sal_time_usecs() - wdata->start_time) / 1000;
         /* Show rate when speed no less than 1k pps. */
        if (delt_ms < 100000) {
            cli_out("Network interface %d receive rate is %"PRIu32" PPS \n",
                    netif_id, 100000000/delt_ms);
        }
        wdata->rx_packets = 0;
    }
}

static int
watcher_packet_terminate(int unit, int netif_id, bcmpkt_packet_t *packet,
                         watcher_data_t *wdata, bcma_bcmpkt_rx_result_t *result)
{
    int rv;
    uint32_t *rxpmd;
    uint32_t dev_type;
    int vlan_id = 0;
    int match_id_low = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_EXIT(bcmpkt_rxpmd_get(packet, &rxpmd));

    if (wdata->term_netif == netif_id) {
        *result = BCMA_BCMPKT_RX_HANDLED;
        SHR_EXIT();
    }

    if (wdata->term_vlan >= 0) {
        rv = bcmpkt_rxpmd_field_get(dev_type, rxpmd, BCMPKT_RXPMD_OUTER_VID,
                                    (uint32_t *)&vlan_id);
        if (SHR_SUCCESS(rv) && wdata->term_vlan == vlan_id) {
            *result = BCMA_BCMPKT_RX_HANDLED;
            SHR_EXIT();
        }
    }

    if (wdata->term_match_id >= 0) {
        rv = bcmpkt_rxpmd_field_get(dev_type, rxpmd, BCMPKT_RXPMD_MATCH_ID_LO,
                                    (uint32_t *)&match_id_low);
        if (SHR_SUCCESS(rv) && wdata->term_match_id == match_id_low) {
            *result = BCMA_BCMPKT_RX_HANDLED;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_watcher(int unit, int netif_id, bcmpkt_packet_t *packet,
                    void *cookie)
{
    int rv;
    bcma_bcmpkt_rx_result_t rx_result = BCMA_BCMPKT_RX_NOT_HANDLED;
    watcher_data_t *wdata = (watcher_data_t *)cookie;

    if (wdata == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Application contex check failed.\n")));
        return rx_result;
    }

    /* Display packet Info. */
    rv = watcher_info_show(unit, netif_id, packet, wdata);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Dump packet info failed (%d).\n"),
                   rv));
        return rx_result;
    }

    /* Loopback packet Data to TX. */
    if (wdata->debug_mode & WATCHER_DEBUG_LPBK_PACKET) {
        rx_result = BCMA_BCMPKT_RX_HANDLED;
        rv = watcher_packet_lpbk_tx(unit, netif_id, packet);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Loopback packet to TX failed (%d).\n"),
                       rv));
            return rx_result;
        }
    }

    /* Display RX rate for every 100k packets. */
    watcher_rx_rate_show(netif_id, wdata);

    rv = watcher_packet_terminate(unit, netif_id, packet, wdata, &rx_result);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Terminate packet failed (%d).\n"),
                   rv));
        return rx_result;
    }

    return rx_result;
}

int
bcma_bcmpkt_txpmd_init(uint32_t dev_type, uint32_t *txpmd)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_START,
                                BCMPKT_TXPMD_START_IHEADER));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_HEADER_TYPE,
                                BCMPKT_TXPMD_HEADER_TYPE_FROM_CPU));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd, BCMPKT_TXPMD_UNICAST, 1));
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_lbhdr_init(uint32_t dev_type, uint32_t *lbhdr)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                BCMPKT_LBHDR_START,
                                BCMPKT_LBHDR_START_CODE));
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_generic_loopback_init(uint32_t variant, uint32_t *generic_loopback)
{
    int fid = BCMPKT_GENERIC_LOOPBACK_T_FID_INVALID;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_field_id_get("START_BYTE", &fid));

    SHR_IF_ERR_EXIT
        (bcmpkt_generic_loopback_t_field_set(variant, generic_loopback,
                                             fid,
                                             0x82));
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_hg3hdr_init(uint32_t variant, uint32_t *hg3hdr)
{
    int fid = BCMPKT_HG3_BASE_T_FID_INVALID;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(bcmpkt_hg3_base_t_field_id_get("RESERVED_ETYPE", &fid));
    SHR_IF_ERR_EXIT
        (bcmpkt_hg3_base_t_field_set(variant, hg3hdr,
                                     fid, BCMPKT_HG3_ETHER_TYPE));

exit:
    SHR_FUNC_EXIT();
}

void
bcma_bcmpkt_flag_set(uint32_t *flags, uint32_t this_flag, int do_set)
{
    *flags &= ~this_flag;
    if (do_set) {
        *flags |= this_flag;
    }
}

bool
bcma_bcmpkt_flag_status_get(const uint32_t flags, uint32_t this_flag)
{
    return ((flags & this_flag) != 0);
}

int
bcma_bcmpkt_chan_qmap_set(int unit, int chan_id, SHR_BITDCL *queue_bmp,
                          uint32_t num_queues)
{
    uint32_t cos;
    uint64_t cpu_queue[MAX_CPU_COS] = {0};
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t ent_info;
    bcmlt_opcode_t opcode = BCMLT_OPCODE_LOOKUP;
    bcmlt_field_def_t fields_def[2];
    uint32_t num_of_fields;
    int status;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(queue_bmp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_table_field_defs_get(unit, DEVICE_PKT_RX_Qs, 2, fields_def,
                                    &num_of_fields));

    SHR_BIT_ITER(queue_bmp, num_queues, cos) {
        if (cos >= fields_def[0].elements) {
            break;
        }
        cpu_queue[cos] = 1;
    }

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, DEVICE_PKT_RX_Qs, &ent_hdl));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, RX_Qs, chan_id));

    status = bcmlt_entry_commit(ent_hdl, opcode, BCMLT_PRIORITY_NORMAL);

    if (status == SHR_E_NONE) {
        opcode = BCMLT_OPCODE_UPDATE;
    } else if (status == SHR_E_NOT_FOUND) {
        opcode = BCMLT_OPCODE_INSERT;
    } else {
        SHR_ERR_EXIT(status);
    }

    SHR_IF_ERR_EXIT(bcmlt_entry_clear(ent_hdl));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, RX_Qs, chan_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(ent_hdl, COSs, 0, cpu_queue,
                                     fields_def[0].elements));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, opcode, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT(bcmlt_entry_info_get(ent_hdl, &ent_info));

    SHR_ERR_EXIT(ent_info.status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_rx_watch_output_file_set(const char *fname)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (fname) {
        if (px_watcher_file) {
            if (sal_strcmp(px_watcher_file, fname)) {
                SHR_IF_ERR_EXIT(SHR_E_EXISTS);
            }
            /* Do nothing if the file names are the same. */
        }

        px_watcher_file = sal_strdup(fname);
        if (px_watcher_file == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }
    } else {
        if (px_watcher_file) {
            sal_free(px_watcher_file);
            px_watcher_file = NULL;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_rx_watch_output_enable(int file_en, int console_en)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (px_watcher_file) {
        if (file_en) {
            px_watcher_fh = bcma_io_file_open(px_watcher_file, "a");
            if (px_watcher_fh == NULL) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_FAIL,
                                    (BSL_META("Unable to open log "
                                              "file %s.\n"),
                                              px_watcher_file));
            }
        } else {
            if (px_watcher_fh) {
                if (!bcma_io_file_close(px_watcher_fh)) {
                    px_watcher_fh = NULL;
                } else {
                    SHR_IF_ERR_MSG_EXIT(SHR_E_FAIL,
                                        (BSL_META("Unable to close log "
                                                  "file %s.\n"),
                                                  px_watcher_file));
                }
            }
        }
    } else {
        if (file_en) {
            /* Log file not specified before enable the logging. */
            SHR_IF_ERR_EXIT(SHR_E_CONFIG);
        }
    }

    px_watcher_console_display = console_en;
exit:
    SHR_FUNC_EXIT();
}

