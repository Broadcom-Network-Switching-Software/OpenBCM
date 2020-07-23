/*! \file bcmpkt_unet.c
 *
 * Interfaces for hooking up UNET driver onto BCMPKT API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#endif
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmcnet/bcmcnet_types.h>
#include <bcmcnet/bcmcnet_main.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#define BSL_LOG_MODULE              BSL_LS_BCMPKT_DMA

#define TXPMD_START_HIGIG           3

/*! Print structure integer member. */
#define PRINT_ST_I(_pb, _s, _m) \
    shr_pb_printf(_pb, "\t%s: %d\n", #_m, _s->_m)
#define PRINT_ST_H(_pb, _s, _m) \
        shr_pb_printf(_pb, "\t%s: 0x%x\n", #_m, _s->_m)
/*! Print queues. */
#define PRINT_ST_I_PER_QUEUE(_pb, _s, _m) \
    { \
        int i; \
        shr_pb_printf(_pb, "\t%s:\n", #_m); \
        for (i = 0; i < NUM_QUE_MAX; i++) { \
            if (_s->_m[i]) \
                shr_pb_printf(_pb, "\t\tqueue_%d: %d\n", i, _s->_m[i]); \
        } \
    }

/*! Print structure uint64 member. */
#define PRINT_ST_U64(_pb, _s, _m) \
    shr_pb_printf(_pb, "\t%s: %" PRIu64 "\n" , #_m, _s->_m)

#define PRINT_ST_U64_PER_QUEUE(_pb, _s, _m) \
    { \
        int i; \
        shr_pb_printf(_pb, "\t%s:\n", #_m); \
        for (i = 0; i < NUM_QUE_MAX; i++) { \
            if (_s->_m[i]) \
                shr_pb_printf(_pb, "\t\tqueue_%d: %" PRIu64 "\n" , i, \
                              _s->_m[i]); \
        } \
    }

static int dev_initialized[BCMDRD_CONFIG_MAX_UNITS];

/*
 * UNET doesn't support multiple disable.
 * Log per-unit enable status.
 */
static bool dev_enabled[BCMDRD_CONFIG_MAX_UNITS];

/* Each unit supports one callback. */
static bcmpkt_rx_cb_info_t rx_cb_info[BCMDRD_CONFIG_MAX_UNITS];

static bool
unet_dev_initialized(int unit) {
    if (bcmdrd_dev_exists(unit) && dev_initialized[unit]) {
        return true;
    }
    return false;
}

static int
rx_packet_handle(int unit, int queue, bcmpkt_data_buf_t *dbuf, void *ck)
{
    bcmpkt_packet_t pkt;
    bcmpkt_packet_t *packet = &pkt;
    bcmpkt_rcpu_hdr_t *rhdr;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf->data, SHR_E_PARAM);

    sal_memset(packet, 0, sizeof(*packet));
    bcmpkt_pmd_format(packet);
    packet->data_buf = dbuf;
    packet->unit = unit;

    /* Copy RXPMD data. */
    rhdr = (bcmpkt_rcpu_hdr_t *)dbuf->data;
    sal_memcpy(packet->pmd.rxpmd, dbuf->data + sizeof(*rhdr), rhdr->meta_len);

    /* Remove RCPU header and meta data from head of packet. */
    if (!bcmpkt_pull(dbuf, sizeof(*rhdr) + rhdr->meta_len)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* BCMPKT API only supports one callback. */
    if (rx_cb_info[unit].cb_func && !rx_cb_info[unit].cb_pending) {
        rx_cb_info[unit].cb_func(unit, queue + 1, packet,
                                 rx_cb_info[unit].cb_data);
    }

exit:
    if (!SHR_FUNC_ERR() && packet->data_buf) {
        bcmpkt_data_buf_free(packet->unit, packet->data_buf);
    }
    SHR_FUNC_EXIT();
}

/*!
 * The netif_id is network interface ID. For CNET driver, DMA queue (channel)
 * ID is the network interface ID. (ID is start from 1.)
 *
 * CNET driver doesn't support RX DMA channel base callback register.
 */
static int
bcmpkt_unet_rx_register(int unit, int netif_id, uint32_t flags,
                        bcmpkt_rx_cb_f cb_func, void *cb_data)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    if (rx_cb_info[unit].cb_func) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "Exists: Support only one callback function\n")));
    }

    rx_cb_info[unit].cb_func = cb_func;
    rx_cb_info[unit].cb_data = cb_data;
    rx_cb_info[unit].flags = flags;
    rx_cb_info[unit].cb_pending = FALSE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * The netif_id is network interface ID. For CNET driver, DMA queue (channel)
 * ID is the network interface ID. (ID is start from 1.)
 *
 * CNET driver doesn't support RX DMA channel base callback register.
 */
static int
bcmpkt_unet_rx_unregister(int unit, int netif_id, bcmpkt_rx_cb_f cb_func,
                          void *cb_data)
{
    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (rx_cb_info[unit].cb_func == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "RX callback not registered\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (rx_cb_info[unit].cb_func != cb_func) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback doesn't match\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    rx_cb_info[unit].cb_pending = TRUE;
    sal_usleep(100000);
    rx_cb_info[unit].cb_func = NULL;
    rx_cb_info[unit].cb_data = NULL;
    rx_cb_info[unit].flags = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_init(int unit, const bcmpkt_dev_init_t *cfg)
{
    uint32_t i, meta_len;
    bcmcnet_dev_cfg_t dev_cfg;
    bcmdrd_dev_type_t dev_type;
    const char *dev_name = NULL;
    bcmpkt_rcpu_hdr_t rhdr = {
        .tpid       = BCMPKT_RCPU_TPID,
        .vlan       = BCMPKT_RCPU_VLAN,
        .ethertype  = BCMPKT_RCPU_ETYPE,
        .flags      = BCMPKT_RCPU_F_MODHDR,
    };

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    if (dev_initialized[unit] == 1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "Network device has already been initialized\n")));
    }

    sal_memset(&dev_cfg, 0, sizeof(dev_cfg));
    if (sal_strlen(cfg->name) > 0) {
        sal_strncpy(dev_cfg.name, cfg->name, sizeof(dev_cfg.name) - 1);
    }
    else {
        sal_sprintf(dev_cfg.name, "%s%d", "bcm", unit);
    }

    dev_name = bcmdrd_dev_type_str(unit);
    sal_strncpy(dev_cfg.type_str, dev_name, sizeof(dev_cfg.type_str) - 1);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_id_get(unit, &dev_cfg.dev_id));
    if (dev_cfg.dev_id == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_len_get(dev_type, &meta_len));
    if (meta_len > BCMPKT_RCPU_TX_MH_SIZE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    rhdr.meta_len = meta_len & 0xff;

    /* Set RCPU header signature with device ID. */
    rhdr.signature = dev_cfg.dev_id;
    dev_cfg.bm_grp = cfg->cgrp_bmp;
    dev_cfg.nb_grp = 0;
    i = dev_cfg.bm_grp;
    while (i != 0) {
        dev_cfg.nb_grp++;
        i &= (i - 1);
    }

    if (dev_cfg.nb_grp == 0){
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Error: cgrp_bmp(0x%x) and cgrp_size(%d)\n"),
              cfg->cgrp_bmp, cfg->cgrp_size));
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_init(unit, &dev_cfg));

    dev_initialized[unit] = 1;

    SHR_IF_ERR_EXIT
        (bcmpkt_rcpu_hdr_set(unit, &rhdr));

    SHR_IF_ERR_EXIT
        (bcmcnet_rx_cb_register(unit, rx_packet_handle, NULL));

    LOG_VERBOSE(BSL_LS_SYS_KNET,
                (BSL_META_U(unit,
                            "Knet device initial succeed: %s, groupbitmap 0x%x\n"),
                 dev_cfg.name, dev_cfg.bm_grp));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_initialized(int unit, bool *initialized)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(initialized, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    *initialized = dev_initialized[unit];

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_enable(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_start(unit));

    dev_enabled[unit] = true;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_disable(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    if (dev_enabled[unit]) {
        SHR_IF_ERR_EXIT
            (bcmcnet_dev_stop(unit));
        dev_enabled[unit] = false;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_enabled(int unit, bool *enabled)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(enabled, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    *enabled = dev_enabled[unit];

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_cleanup(int unit)
{
    if (unet_dev_initialized(unit)) {
        (void)bcmcnet_rx_cb_unregister(unit, rx_packet_handle, NULL);

        (void)bcmpkt_unet_rx_unregister(unit, 0,
                                        rx_cb_info[unit].cb_func, NULL);

        (void)bcmpkt_unet_dev_disable(unit);
        dev_initialized[unit] = 0;
        return bcmcnet_dev_cleanup(unit);
    }

    return SHR_E_NONE;
}

static int
bcmpkt_unet_chan_set(int unit, bcmpkt_dma_chan_t *chan)
{
    bcmcnet_chan_cfg_t chan_cfg;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(chan, SHR_E_PARAM);

    sal_memset(&chan_cfg, 0, sizeof(chan_cfg));
    chan_cfg.chan = chan->id;
    chan_cfg.dir = chan->dir;
    chan_cfg.nb_desc = chan->ring_size;
    chan_cfg.rx_buf_size = chan->max_frame_size;

    if (bcmdrd_dev_byte_swap_packet_dma_get(unit)) {
        chan_cfg.chan_ctrl |= BCMCNET_PKT_BYTE_SWAP;
    }
    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        chan_cfg.chan_ctrl |= BCMCNET_OTH_BYTE_SWAP | BCMCNET_HDR_BYTE_SWAP;
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_chan_config(unit, &chan_cfg));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Setup DMA channel %d: %s, ring size %d frame size %d\n"),
                 chan_cfg.chan, (chan_cfg.dir) ? "TX" : "RX",
                 chan_cfg.nb_desc, chan_cfg.rx_buf_size));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_chan_get(int unit, int chan_id, bcmpkt_dma_chan_t *chan)
{
    bcmcnet_chan_cfg_t chan_cfg;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(chan, SHR_E_PARAM);

    chan_cfg.chan = chan_id;
    SHR_IF_ERR_EXIT
        (bcmcnet_dev_chan_query(unit, &chan_cfg));

    sal_memset(chan, 0, sizeof(*chan));
    chan->dir = chan_cfg.dir;
    chan->id = chan_cfg.chan;
    chan->ring_size = chan_cfg.nb_desc;
    chan->max_frame_size = chan_cfg.rx_buf_size;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_chan_get_list(int unit, uint32_t size, bcmpkt_dma_chan_t *chans,
                           uint32_t *num_elements)
{
    uint32_t chan_num = 0;
    uint32_t chan_id;
    uint32_t chans_bmp;
    struct bcmcnet_dev_info *info;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(num_elements, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_info_get(unit, &info));

    SHR_NULL_CHECK(info, SHR_E_CONFIG);

    chan_num = info->nb_rx_queues + info->nb_tx_queues;
    if (size == 0) {
        *num_elements = chan_num;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_NULL_CHECK(chans, SHR_E_PARAM);

    if (chan_num > size) {
        chan_num = size;
    }
    chans_bmp = info->bm_rx_queues | info->bm_tx_queues;

    *num_elements = 0;
    for (chan_id = 0; chan_id < 32; chan_id++) {
        if (*num_elements >= chan_num) {
            break;
        }

        if (chans_bmp & (0x1 << chan_id)) {
            SHR_IF_ERR_EXIT
                (bcmpkt_unet_chan_get(unit, chan_id, &chans[*num_elements]));
            (*num_elements)++;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
packet_device_info_dump(struct bcmcnet_dev_info *info, shr_pb_t *pb)
{
    shr_pb_printf(pb, "\tdev_name: %s\n", info->dev_name);
    PRINT_ST_I(pb, info, max_groups);
    PRINT_ST_I(pb, info, max_queues);
    PRINT_ST_H(pb, info, bm_groups);
    PRINT_ST_H(pb, info, bm_rx_queues);
    PRINT_ST_H(pb, info, bm_tx_queues);
    PRINT_ST_I(pb, info, nb_groups);
    PRINT_ST_I(pb, info, nb_rx_queues);
    PRINT_ST_I(pb, info, nb_tx_queues);
    PRINT_ST_I(pb, info, rx_desc_size);
    PRINT_ST_I(pb, info, tx_desc_size);
    PRINT_ST_I(pb, info, rx_buf_dflt);
    PRINT_ST_I(pb, info, nb_desc_dflt);

    PRINT_ST_I_PER_QUEUE(pb, info, rx_buf_size);
    PRINT_ST_I_PER_QUEUE(pb, info, nb_rx_desc);
    PRINT_ST_I_PER_QUEUE(pb, info, nb_tx_desc);
}

static void
packet_device_stats_dump(struct bcmcnet_dev_stats *stats, shr_pb_t *pb)
{
    PRINT_ST_U64(pb, stats, rx_packets);
    PRINT_ST_U64(pb, stats, rx_bytes);
    PRINT_ST_U64(pb, stats, rx_dropped);
    PRINT_ST_U64(pb, stats, rx_errors);
    PRINT_ST_U64(pb, stats, rx_head_errors);
    PRINT_ST_U64(pb, stats, rx_data_errors);
    PRINT_ST_U64(pb, stats, rx_cell_errors);
    PRINT_ST_U64(pb, stats, rx_nomems);
    PRINT_ST_U64(pb, stats, tx_packets);
    PRINT_ST_U64(pb, stats, tx_bytes);
    PRINT_ST_U64(pb, stats, tx_dropped);
    PRINT_ST_U64(pb, stats, tx_errors);
    PRINT_ST_U64(pb, stats, tx_xoffs);
    PRINT_ST_U64(pb, stats, intrs);

    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_packets);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_bytes);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_dropped);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_head_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_data_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_cell_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_nomems);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_packets);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_bytes);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_dropped);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_xoffs);
}

static int
bcmpkt_unet_dev_info_dump(int unit, shr_pb_t *pb)
{
    struct bcmcnet_dev_info *info;
    bcmpkt_dma_chan_t *chans = NULL;
    uint32_t i, chan_num;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (bcmcnet_dev_info_get(unit, &info));

    SHR_NULL_CHECK(info, SHR_E_CONFIG);
    packet_device_info_dump(info, pb);

    SHR_ALLOC(chans, sizeof(*chans) * info->max_queues, "bcmpktDevInfoDump");
    SHR_NULL_CHECK(chans, SHR_E_MEMORY);

    /* Dump channel configurations. */
    SHR_IF_ERR_EXIT
        (bcmpkt_unet_chan_get_list(unit, info->max_queues, chans, &chan_num));

    shr_pb_printf(pb, "\tdev_name: %s\n", info->dev_name);
    shr_pb_printf(pb, "\tDMA Channels:\n");
    for (i = 0; i < chan_num; i++) {
        shr_pb_printf(pb, "\t\tID: %d\n", chans[i].id);
        if (chans[i].dir) {
            shr_pb_printf(pb, "\t\tDirection: TX\n");
        } else {
            shr_pb_printf(pb, "\t\tDirection: RX\n");
            shr_pb_printf(pb, "\t\tMaxFrameSize: %d\n", chans[i].max_frame_size);
        }
        shr_pb_printf(pb, "\t\tRing Size: %d\n", chans[i].ring_size);
    }

exit:
    SHR_FREE(chans);
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_stats_get(int unit, bcmpkt_dev_stat_t *stats)
{
    struct bcmcnet_dev_stats *drv_stats;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(stats, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_stats_get(unit, &drv_stats));

    SHR_NULL_CHECK(drv_stats, SHR_E_INTERNAL);

    sal_memset(stats, 0, sizeof(*stats));
    stats->rx_packets = drv_stats->rx_packets;
    stats->rx_dropped = drv_stats->rx_dropped;
    stats->tx_packets = drv_stats->tx_packets;
    stats->tx_dropped = drv_stats->tx_dropped;
    stats->intrs = drv_stats->intrs;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_stats_dump(int unit, shr_pb_t *pb)
{
    struct bcmcnet_dev_stats *stats;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (bcmcnet_dev_stats_get(unit, &stats));

    SHR_NULL_CHECK(stats, SHR_E_INTERNAL);

    packet_device_stats_dump(stats, pb);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_dev_stats_clear(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_initialized(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_stats_reset(unit));
exit:
    SHR_FUNC_EXIT();
}

/* This function inserts higig3 header at the correct location within the packet
 * header. Higig3 packet header is specified by caller (hg_size, in bytes)
 */
static int
higig3_header_insert(int unit, bcmpkt_packet_t *packet, int hg_size)
{
    uint8_t *data_ptr;
    uint8_t *mac_ptr;
    uint32_t *hdr_ptr;

    SHR_FUNC_ENTER(unit);

    if (hg_size <= 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    /* Start of packet has eth header (dm-sm-ethtype) */
    mac_ptr = BCMPKT_PACKET_DATA(packet);

    /* Adjust data ptr and data len to accomodate hg3 header bytes (currently
     * 8 or 16)
     */
    if (!bcmpkt_push(packet->data_buf, hg_size)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Get data ptr which points to a new location */
    data_ptr = BCMPKT_PACKET_DATA(packet);
    /* Copy dmac and smac first */
    sal_memmove(data_ptr, mac_ptr, 12);

    /* Next copy higig3 header. See d2 calculation */
    hdr_ptr = (uint32_t *)(BCMPKT_PACKET_DATA(packet) + 12); /* d2 */
    shr_util_host_to_be32(hdr_ptr, packet->pmd.higig, hg_size / 4);

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT()
}

static int
header_generate(int unit, bcmpkt_packet_t *packet)
{
    uint32_t hg_size = 0, pstart = 0, hdr_size = 0;
    uint32_t lbhdr_size = 0, lbstart = 0;
    bcmpkt_data_buf_t *dbuf = NULL;
    HIGIG_t *hghdr = NULL;
    uint32_t dev_type;
    uint8_t *data;
    uint32_t *hdr = NULL;
    uint32_t data_size, meta_len = 0;
    bcmpkt_rcpu_hdr_t *rhdr;
    uint32_t is_higig3_hdr = FALSE;
    /* New var used for Higig3 since hg_size is used for module header
     * calculation/processing. Higig3 is seen as a regular ethernet packet
     */
    uint32_t hg3_size = 0;
    bool higig3_is_supported = false;
    bool gih_is_supported = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));

    rhdr = bcmpkt_rcpu_hdr(unit);
    hghdr = (HIGIG_t *)packet->pmd.higig;

    if ((hghdr != NULL) && (packet->pmd.higig[0] != 0)) {
        uint32_t hstart = HIGIG_STARTf_GET(*hghdr);
        if (hstart == BCMPKT_HIGIG2_SOF) {
            hg_size = HIGIG2_SIZE;
        } else if (hstart == BCMPKT_HIGIG_SOF) {
            hg_size = HIGIG_SIZE;
        }
    }

    SHR_IF_ERR_EXIT(bcmpkt_higig3_is_supported(unit, dev_type,
                                               &higig3_is_supported));
    /* If not HG/HG2, check for HG3 */
    if ((higig3_is_supported == true) && (packet->pmd.higig != NULL) &&
        (hg_size == 0)) {
        uint32_t hstart;
        uint32_t ext_hdrs;

        SHR_IF_ERR_EXIT
            (bcmpkt_higig3_field_get(unit, dev_type, packet->pmd.higig,
                                     BCMPKT_HIGIG3_RESERVED_ETYPE, &hstart));
        /* Extract 2-bit field for determining presence of ext hdr */
        SHR_IF_ERR_EXIT
            (bcmpkt_higig3_field_get(unit, dev_type, packet->pmd.higig,
                                     BCMPKT_HIGIG3_EXT_HDR_PRESENT,
                                     &ext_hdrs));

        if (hstart == BCMPKT_HIGIG3_ETHER_TYPE) {
            is_higig3_hdr = TRUE;
            hg3_size = 0;

            switch (ext_hdrs) {
                case 0x1:
                    /* Only base header present */
                    hg3_size = BCMPKT_HIGIG3_BASE_HEADER_SIZE_BYTES;
                    break;

                case 0x2:
                    /* Base header and extension header 0 present */
                    hg3_size = BCMPKT_HIGIG3_BASE_HEADER_SIZE_BYTES +
                               BCMPKT_HIGIG3_EXT0_HEADER_SIZE_BYTES;
                    break;

                case 0x3:
                    /* Extension header 1 is currently not supported.
                     * This code should be updated when extension header 1
                     * is used by any future device
                     */
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Error: Unsupported Higig3"
                              " version %u\n"), ext_hdrs));
                    return (SHR_E_UNAVAIL);

                case 0x0:
                    /* Reserved value */
                default:
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Error: Reserved/Invalid"
                              " Higig3 version passed %u\n"), ext_hdrs));
                    return (SHR_E_CONFIG);
            }
        } else {
            /* If hstart is 0, it means a normal(non-higig) packet
             * is being sent. Any other value means an invalid value was
             * passed
             */
            if (hstart != 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Error: Invalid"
                          " Higig3 ethertype 0x%X\n"), hstart));
                return (SHR_E_CONFIG);
            }
        }
    }

    /* If Higig3 header is present, insert it before any other pmds/headers are
     * processed. So rest of the code sees the packet as a regular
     * ethernet packet
     */
    if ((higig3_is_supported == true) && (is_higig3_hdr) && (hg3_size > 0)) {
        SHR_IF_ERR_EXIT(higig3_header_insert(unit, packet, hg3_size));
    }

    if ((packet->pmd.txpmd != NULL) && (packet->pmd.txpmd[0] != 0)) {
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_get(dev_type, packet->pmd.txpmd,
                                    BCMPKT_TXPMD_START, &pstart));

        if ((packet->pmd.lbhdr != NULL) && (packet->pmd.lbhdr[0] != 0)) {
            SHR_IF_ERR_EXIT
                (bcmpkt_gih_is_supported(unit, dev_type, &gih_is_supported));
            if (gih_is_supported == true) {
                SHR_IF_ERR_EXIT
                    (bcmpkt_gih_field_get(unit, dev_type, packet->pmd.lbhdr,
                                          BCMPKT_GIH_START, &lbstart));
            } else {
                SHR_IF_ERR_EXIT
                    (bcmpkt_lbhdr_field_get(dev_type, packet->pmd.lbhdr,
                                            BCMPKT_LBHDR_START, &lbstart));
            }

            if (lbstart > 0) {
                lbhdr_size = BCMPKT_LBHDR_SIZE_BYTES;
            }
        }
    }

    /* Calculate request space for RCPU, TXPMD and Higig header. */
    if (pstart > 0) {
        hdr_size = sizeof(*rhdr) + rhdr->meta_len + hg_size + lbhdr_size;
    } else if (hg_size > 0) {
        hdr_size = sizeof(*rhdr) + rhdr->meta_len;
    } else {
        hdr_size = sizeof(*rhdr);
    }

    /* If the head room is not enough, try to re-allocate data buffer. */
    if (BCMPKT_PACKET_DATA(packet) < packet->data_buf->head + hdr_size ||
        unit != packet->unit) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Not enough headroom, re-allocate packet buffer\n")));
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_alloc(unit,
                                   BCMPKT_PACKET_LEN(packet) + hdr_size,
                                   &dbuf));
        if (!bcmpkt_reserve(dbuf, hdr_size)) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        if (!bcmpkt_put(dbuf, BCMPKT_PACKET_LEN(packet))) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        sal_memcpy(dbuf->data, BCMPKT_PACKET_DATA(packet),
                   BCMPKT_PACKET_LEN(packet));

        SHR_IF_ERR_CONT
            (bcmpkt_data_buf_free(packet->unit, packet->data_buf));
        packet->data_buf = dbuf;
        packet->unit = unit;
    }

    data_size = BCMPKT_PACKET_LEN(packet);

    /* Encapsulate TXPMD and Higig header. */
    if (pstart > 0) {
        /* Encapsulate Higig header. Continue only for HG/HG2 hdr
         * insertion since this step is not valid for HG3
         */
        if ((!is_higig3_hdr) && (hg_size > 0)) {
            if (!bcmpkt_push(packet->data_buf, hg_size)) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
            shr_util_host_to_be32(hdr, packet->pmd.higig, hg_size / 4);
        }

        /* Encapsulate Loopback header. */
        if (lbhdr_size > 0) {
            if (!bcmpkt_push(packet->data_buf, lbhdr_size)) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
            shr_util_host_to_be32(hdr, packet->pmd.lbhdr, lbhdr_size / 4);
        }
        data_size = BCMPKT_PACKET_LEN(packet);

        /* Set TXPMD metadata. */
        if ((!is_higig3_hdr) && (hg_size > 0)) {
            /* Set Higig flag. */
            SHR_IF_ERR_EXIT
                (bcmpkt_txpmd_field_set(dev_type, packet->pmd.txpmd,
                                        BCMPKT_TXPMD_START, TXPMD_START_HIGIG));
        }
        if (!bcmpkt_push(packet->data_buf, rhdr->meta_len)) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
            shr_util_host_to_be32(hdr, packet->pmd.txpmd, rhdr->meta_len / 4);
        } else {
            sal_memcpy(hdr, packet->pmd.txpmd, rhdr->meta_len);
        }
        meta_len = rhdr->meta_len;
    } else if ((!is_higig3_hdr) && (hg_size > 0)) {
        /* The code under this 'else-if' is used for Higig/Higig2 only */
        if (rhdr->meta_len < hg_size ||
            (!bcmpkt_push(packet->data_buf, hg_size))) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
            shr_util_host_to_be32(hdr, packet->pmd.higig, hg_size / 4);
        } else {
            sal_memcpy(hdr, packet->pmd.higig, hg_size);
        }
        meta_len = rhdr->meta_len;
    }

    /* Encapsulate RCPU header. */
    if (!bcmpkt_push(packet->data_buf, sizeof(*rhdr))) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    data = BCMPKT_PACKET_DATA(packet);
    sal_memset(data, 0, sizeof(*rhdr));
    data[20] = BCMPKT_RCPU_OP_TX;
    if (meta_len > 0) {
        data[21] = BCMPKT_RCPU_F_MODHDR;
    }
    data[24] = data_size >> 8;
    data[25] = data_size & 0xFF;
    data[28] = meta_len;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_unet_tx(int unit, int netif_id, bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->data_buf, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMPKT_PACKET_DATA(packet), SHR_E_PARAM);

    if (BCMPKT_PACKET_LEN(packet) < bcmpkt_framesize_min_get()) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit,
                         "Packet size %"PRIu32" is underrun\n"),
              BCMPKT_PACKET_LEN(packet)));
    }

    if (packet->type == BCMPKT_FWD_T_NORMAL) {
        SHR_IF_ERR_EXIT
            (header_generate(unit, packet));
    }

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb;
        pb = shr_pb_create();
        shr_pb_printf(pb, "Data buf:\n");
        bcmpkt_data_buf_dump(packet->data_buf, pb);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_tx(unit, netif_id - 1, packet->data_buf));

    packet->data_buf = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_unet_attach(void)
{
    bcmpkt_dev_t dev_vect = {
        .initialized = 1,
        .driver_name = "UNET",
        .driver_type = BCMPKT_DEV_DRV_T_UNET,

        .init = bcmpkt_unet_dev_init,
        .dev_initialized = bcmpkt_unet_dev_initialized,
        .cleanup = bcmpkt_unet_dev_cleanup,
        .enable = bcmpkt_unet_dev_enable,
        .disable = bcmpkt_unet_dev_disable,
        .enabled = bcmpkt_unet_dev_enabled,
        .dma_chan_ops.set = bcmpkt_unet_chan_set,
        .dma_chan_ops.get = bcmpkt_unet_chan_get,
        .dma_chan_ops.get_list = bcmpkt_unet_chan_get_list,
        .info_dump = bcmpkt_unet_dev_info_dump,
        .stats_get = bcmpkt_unet_dev_stats_get,
        .stats_dump = bcmpkt_unet_dev_stats_dump,
        .stats_clear = bcmpkt_unet_dev_stats_clear
    };

    bcmpkt_net_t net_vect = {
        .initialized = 1,
        .driver_name = "UNET",
        .driver_type = BCMPKT_NET_DRV_T_UNET,
        .rx_register = bcmpkt_unet_rx_register,
        .rx_unregister = bcmpkt_unet_rx_unregister,
        .tx = bcmpkt_unet_tx
    };

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_dev_drv_register(&dev_vect), SHR_E_EXISTS);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_net_drv_register(&net_vect), SHR_E_EXISTS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_unet_detach(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_unregister(BCMPKT_NET_DRV_T_UNET));

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_unregister(BCMPKT_DEV_DRV_T_UNET));

exit:
    SHR_FUNC_EXIT();
}
