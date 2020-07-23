/*! \file bcmlu_knet.c
 *
 * Linux user mode KNET driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>

#include <bcmcnet/bcmcnet_types.h>
#include <lkm/ngknet_dev.h>

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <unistd.h>

#include <bcmlu/bcmlu_ngknet.h>
#include <bcmlu/bcmpkt/bcmlu_knet.h>

#define BSL_LOG_MODULE              BSL_LS_SYS_KNET

#define DEV_CTRL_NETIF_ID           0

#define TXPMD_START_IHEADER         2
#define TXPMD_HEADER_TYPE_FROM_CPU  1
#define TXPMD_UNICAST               1

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

static bool dev_initialized[BCMDRD_CONFIG_MAX_UNITS];

static int
txpmd_encap(int unit, int port, int queue, uint32_t *txpmd)
{
    uint32_t dev_type;
    bcmpkt_txpmd_fid_support_t support_status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_START, TXPMD_START_IHEADER));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_HEADER_TYPE,
                                TXPMD_HEADER_TYPE_FROM_CPU));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_LOCAL_DEST_PORT, port));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_COS, queue));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_UNICAST, TXPMD_UNICAST));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_fid_support_get(dev_type, &support_status));
    if (SHR_BITGET(support_status.fbits, BCMPKT_TXPMD_UNICAST_PKT)) {
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                    BCMPKT_TXPMD_UNICAST_PKT,
                                    TXPMD_UNICAST));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * This is for HIGIG encapsulation for sending from linux protocol
 * stack. Leave for future.
 */
static int
higig_encap(int port, uint8_t *mh)
{
    return SHR_E_UNAVAIL;
}

/*!
 * This is for HIGIG2 encapsulation for sending from linux protocol
 * stack. Leave for future.
 */
static int
higig2_encap(int port, uint8_t *mh)
{
    return SHR_E_UNAVAIL;
}

static bool
socket_dev_initialized(int unit) {
    if (bcmdrd_dev_exists(unit) && dev_initialized[unit]) {
        return true;
    }
    return false;
}

static int
netif_to_bnetif_translate(int unit, bcmpkt_netif_t *netif, ngknet_netif_t *bnetif)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(bnetif, 0, sizeof(*bnetif));
    sal_memcpy(bnetif->macaddr, netif->mac_addr, sizeof(bnetif->macaddr));
    sal_strlcpy(bnetif->name, netif->name, sizeof(bnetif->name));
    bnetif->vlan = netif->vlan;
    bnetif->mtu = netif->max_frame_size;
    sal_memcpy(bnetif->user_data, netif->user_data, sizeof(bnetif->user_data));

    if (netif->flags & BCMPKT_NETIF_F_RCPU_ENCAP) {
        bnetif->flags |= NGKNET_NETIF_F_RCPU_ENCAP;
        bnetif->mtu += BCMPKT_RCPU_MAX_ENCAP_SIZE;
    }

    if (netif->flags & BCMPKT_NETIF_F_ADD_TAG) {
        bnetif->flags |= NGKNET_NETIF_F_ADD_TAG;
    }

    if (netif->flags & BCMPKT_NETIF_F_BIND_RX_CH) {
        bnetif->chan = netif->dma_chan_id;
        bnetif->flags |= NGKNET_NETIF_F_BIND_CHAN;
    }
    if (netif->flags & BCMPKT_NETIF_F_BIND_TX_PORT) {
        uint32_t *txpmd = NULL;

        bnetif->type = NGKNET_NETIF_T_PORT;
        txpmd = (uint32_t *)bnetif->meta_data;
        SHR_IF_ERR_EXIT
            (txpmd_encap(unit, netif->port, netif->port_queue, txpmd));

        bnetif->meta_len = NGKNET_NETIF_META_MAX;
        /* KNET driver doesn't support module header, leave for future. */
        if (!sal_strcasecmp("higig2", netif->port_encap)) {
            SHR_IF_ERR_EXIT
                (higig2_encap(netif->port, NULL));
        } else if (!sal_strcasecmp("higig", netif->port_encap)) {
            SHR_IF_ERR_EXIT
                (higig_encap(netif->port, NULL));
        } else if (sal_strcasecmp("ieee", netif->port_encap)) {/* Non-IEEE. */
            if (sal_strlen(netif->port_encap)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bnetif_to_netif_translate(int unit, ngknet_netif_t *bnetif, bcmpkt_netif_t *netif)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(netif, 0, sizeof(*netif));
    netif->id = bnetif->id;
    sal_memcpy(netif->mac_addr, bnetif->macaddr, sizeof(netif->mac_addr));
    sal_strlcpy(netif->name, bnetif->name, sizeof(netif->name));
    netif->vlan = bnetif->vlan;
    netif->max_frame_size = bnetif->mtu;
    sal_memcpy(netif->user_data, bnetif->user_data, sizeof(netif->user_data));

    if (bnetif->flags & NGKNET_NETIF_F_RCPU_ENCAP) {
        netif->flags |= BCMPKT_NETIF_F_RCPU_ENCAP;
        if (netif->max_frame_size > BCMPKT_RCPU_MAX_ENCAP_SIZE) {
            netif->max_frame_size -= BCMPKT_RCPU_MAX_ENCAP_SIZE;
        }
    }
    if (bnetif->flags & NGKNET_NETIF_F_ADD_TAG) {
        netif->flags |= BCMPKT_NETIF_F_ADD_TAG;
    }

    if (bnetif->flags & NGKNET_NETIF_F_BIND_CHAN) {
        netif->dma_chan_id = bnetif->chan;
        netif->flags |= BCMPKT_NETIF_F_BIND_RX_CH;
    }
    if (bnetif->type & NGKNET_NETIF_T_PORT) {
        uint32_t txpmd[BCMPKT_TXPMD_SIZE_WORDS];
        uint32_t dev_type;

        SHR_IF_ERR_EXIT
            (bcmpkt_dev_type_get(unit, &dev_type));

        sal_memset(txpmd, 0, BCMPKT_TXPMD_SIZE_BYTES);
        sal_memcpy(txpmd, bnetif->meta_data, bnetif->meta_len);

        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_get(dev_type, txpmd,
                                    BCMPKT_TXPMD_LOCAL_DEST_PORT,
                                    (uint32_t *)&netif->port));
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_get(dev_type, txpmd,
                                    BCMPKT_TXPMD_COS,
                                    (uint32_t *)&netif->port_queue));
        /* Current support IEEE only. */
        sal_strcpy(netif->port_encap, "IEEE");
        netif->flags |= BCMPKT_NETIF_F_BIND_TX_PORT;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_create(int unit, bcmpkt_netif_t *netif)
{
    ngknet_netif_t bnetif;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(netif, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (netif_to_bnetif_translate(unit, netif, &bnetif));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_create(unit, &bnetif));

    netif->id = bnetif.id;
    sal_strcpy(netif->name, bnetif.name);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_get(int unit, int netif_id, bcmpkt_netif_t *netif)
{
    ngknet_netif_t bnetif;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }
    SHR_NULL_CHECK(netif, SHR_E_PARAM);

    sal_memset(&bnetif, 0, sizeof(bnetif));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_get(unit, netif_id, &bnetif));

    SHR_IF_ERR_EXIT
        (bnetif_to_netif_translate(unit, &bnetif, netif));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_traverse(int unit, bcmpkt_netif_traverse_cb_f cb_func, void *cb_data)
{
    ngknet_netif_t bnetif;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_EXIT();
    }
    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    /* Only handle other than id == 0. */
    sal_memset(&bnetif, 0, sizeof(bnetif));
    do {
        SHR_IF_ERR_EXIT
            (bcmlu_ngknet_netif_get_next(unit, &bnetif));

        if (bnetif.id == 0) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bnetif_to_netif_translate(unit, &bnetif, &netif));

        SHR_IF_ERR_EXIT
            (cb_func(unit, &netif, cb_data));
    } while (bnetif.next != 0);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_destroy(int unit, int netif_id)
{
    int rv;
    bool created;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    rv = bcmpkt_socket_created(unit, netif_id, &created);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get SOCKET created status failed (%d)\n"),
                   rv));
        SHR_ERR_EXIT(rv);
    } else if (created) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_RESOURCE,
             (BSL_META_U(unit, "Destroy a SOCKET netif (netif %d) is prohibit\n"),
              netif_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_destroy(unit, netif_id));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_link_update(int unit, int netif_id, bool linkup)
{
    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_link_set(unit, netif_id, linkup));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_max_get(int unit, int *max_num)
{
    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(max_num, SHR_E_PARAM);

    *max_num = BCMLU_NETIF_MAX_IDS;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_dev_ctrl_intf_get(int unit, bcmpkt_netif_t *netif)
{
    ngknet_netif_t bnetif;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(netif, SHR_E_PARAM);

    sal_memset(&bnetif, 0, sizeof(bnetif));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_get(unit, DEV_CTRL_NETIF_ID, &bnetif));

    SHR_IF_ERR_EXIT
        (bnetif_to_netif_translate(unit, &bnetif, netif));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_chan_set(int unit, bcmpkt_dma_chan_t *chan)
{
    ngknet_chan_cfg_t chan_cfg;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(chan, SHR_E_PARAM);

    sal_memset(&chan_cfg, 0, sizeof(chan_cfg));
    chan_cfg.chan = chan->id;
    chan_cfg.dir = chan->dir;
    chan_cfg.nb_desc = chan->ring_size;
    chan_cfg.rx_buf_size = chan->max_frame_size;

    if (bcmdrd_dev_byte_swap_packet_dma_get(unit)) {
        chan_cfg.chan_ctrl |= NGKNET_PKT_BYTE_SWAP;
    }
    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        chan_cfg.chan_ctrl |= NGKNET_OTH_BYTE_SWAP | NGKNET_HDR_BYTE_SWAP;
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_chan_config(unit, &chan_cfg));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Setup DMA channel %d: %s, ring size %"PRIu32" frame size %"PRIu32"\n"),
                 chan_cfg.chan, (chan_cfg.dir) ? "TX" : "RX",
                 chan_cfg.nb_desc, chan_cfg.rx_buf_size));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_chan_get(int unit, int chan_id, bcmpkt_dma_chan_t *chan)
{
    ngknet_chan_cfg_t chan_cfg;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(chan, SHR_E_PARAM);

    sal_memset(&chan_cfg, 0, sizeof(chan_cfg));
    chan_cfg.chan = chan_id;
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_chan_query(unit, &chan_cfg));

    chan->dir = chan_cfg.dir;
    chan->id = chan_cfg.chan;
    chan->ring_size = chan_cfg.nb_desc;
    chan->max_frame_size = chan_cfg.rx_buf_size;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief DMA channel list retrieve function.
 *
 * Retrieve a list of DMA channels' configurations.
 *
 * \param [in] unit Switch unit number.
 * \param [in] size  The maximum number of \ref bcmpkt_dma_chan_t elements
 *                   can be held.
 * \param [out] chans The \ref bcmpkt_dma_chan_t elements array.
 * \param [out] num_elements The number of \ref bcmpkt_dma_chan_t elements
 *                           copied to 'chans', if size > 0; total number of
 *                           DMA channel in the unit can be used by packet
 *                           APIs, if size = 0.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM A parameter was invalid.
 */
static int
bcmlu_chan_get_list(int unit, uint32_t size, bcmpkt_dma_chan_t *chans,
                    uint32_t *num_elements)
{
    uint32_t chan_num = 0;
    uint32_t chan_id;
    uint32_t chans_bmp;
    struct bcmcnet_dev_info info;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(num_elements, SHR_E_PARAM);

    sal_memset(&info, 0, sizeof(info));
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_info_get(unit, &info));

    chan_num = info.nb_rx_queues + info.nb_tx_queues;
    if (size == 0) {
        *num_elements = chan_num;
        SHR_EXIT();
    }

    SHR_NULL_CHECK(chans, SHR_E_PARAM);

    if (chan_num > size) {
        chan_num = size;
    }
    chans_bmp = info.bm_rx_queues | info.bm_tx_queues;

    *num_elements = 0;
    for (chan_id = 0; chan_id < 32; chan_id++) {
        if (*num_elements >= chan_num) {
            break;
        }

        if (chans_bmp & (0x1 << chan_id)) {
            SHR_IF_ERR_EXIT
                (bcmlu_chan_get(unit, chan_id, &chans[*num_elements]));
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
bcmlu_socket_dev_info_dump(int unit, shr_pb_t *pb)
{
    struct bcmcnet_dev_info info;
    bcmpkt_dma_chan_t *chans = NULL;
    uint32_t i, chan_num;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    sal_memset(&info, 0, sizeof(info) );
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_info_get(unit, &info));

    packet_device_info_dump(&info, pb);

    SHR_ALLOC(chans, sizeof(*chans) * info.max_queues, "bcmpktDevInfoDump");
    SHR_NULL_CHECK(chans, SHR_E_MEMORY);

    /* Dump channel configurations. */
    SHR_IF_ERR_EXIT
        (bcmlu_chan_get_list(unit, info.max_queues, chans, &chan_num));

    shr_pb_printf(pb, "\tDMA Channels:\n");
    for (i = 0; i < chan_num; i++) {
        shr_pb_printf(pb, "\t\tID: %d\n", chans[i].id);
        if (chans[i].dir) {
            shr_pb_printf(pb, "\t\tDirection: TX\n");
        } else {
            shr_pb_printf(pb, "\t\tDirection: RX\n");
            shr_pb_printf(pb, "\t\tMaxFrameSize: %"PRIu32"\n", chans[i].max_frame_size);
        }
        shr_pb_printf(pb, "\t\tRing Size: %"PRIu32"\n", chans[i].ring_size);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_dev_ctrl_intf_get(unit, &netif));
    shr_pb_printf(pb, "\tManagement interface:\n");
    shr_pb_printf(pb, "\t\tID: %d\n", netif.id);
    shr_pb_printf(pb, "\t\tName: %s\n", netif.name);
    shr_pb_printf(pb, "\t\tMACaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  netif.mac_addr[0], netif.mac_addr[1], netif.mac_addr[2],
                  netif.mac_addr[3], netif.mac_addr[4], netif.mac_addr[5]);

exit:
    SHR_FREE(chans);
    SHR_FUNC_EXIT();
}

static int
bcmlu_socket_dev_stats_get(int unit, bcmpkt_dev_stat_t *stats)
{
    struct bcmcnet_dev_stats drv_stats;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(stats, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_stats_get(unit, &drv_stats));

    sal_memset(stats, 0, sizeof(*stats));
    stats->rx_packets = drv_stats.rx_packets;
    stats->rx_dropped = drv_stats.rx_dropped;
    stats->tx_packets = drv_stats.tx_packets;
    stats->tx_dropped = drv_stats.tx_dropped;
    stats->intrs = drv_stats.intrs;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_socket_dev_stats_dump(int unit, shr_pb_t *pb)
{
    struct bcmcnet_dev_stats stats;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    sal_memset(&stats, 0, sizeof(stats));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_stats_get(unit, &stats));

    packet_device_stats_dump(&stats, pb);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_socket_dev_stats_clear(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_stats_reset(unit));
exit:
    SHR_FUNC_EXIT();
}

static int
netif_enable(int unit, bool enable)
{
    int sock_fd = -1;
    struct ifreq ifr;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlu_dev_ctrl_intf_get(unit, &netif));

    /* Create socket. */
    sock_fd = socket(PF_PACKET, SOCK_RAW, 0);
    if (sock_fd == -1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Create socket failed (%s)\n"), strerror(errno)));

        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strlcpy(ifr.ifr_name, netif.name, sizeof(ifr.ifr_name));
    if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
        close(sock_fd);
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Ioctl error\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Set interface configuration. */
    if (enable) {
        ifr.ifr_flags |= IFF_UP;
    }
    else {
        ifr.ifr_flags &= ~IFF_UP;
    }
    if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
        close(sock_fd);
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Ioctl error\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s packet device %s\n"),
                 (enable) ? "Open" : "Close", netif.name));
    close(sock_fd);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_socket_dev_enable(int unit)
{
    struct bcmcnet_dev_info dev_info;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    sal_memset(&dev_info, 0, sizeof(dev_info));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_info_get(unit, &dev_info));
    if (dev_info.nb_rx_queues == 0 || dev_info.nb_tx_queues == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Not config Rx or Tx queue yet\n")));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (netif_enable(unit, 1));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_socket_dev_disable(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (netif_enable(unit, 0));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_socket_dev_enabled(int unit, bool *enabled)
{
    int sock_fd = -1;
    struct ifreq ifr;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(enabled, SHR_E_PARAM);

    if (!socket_dev_initialized(unit)) {
        *enabled = false;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlu_dev_ctrl_intf_get(unit, &netif));

    /* Create socket. */
    sock_fd = socket(PF_PACKET, SOCK_RAW, 0);
    if (sock_fd == -1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Create socket failed (%s)\n"), strerror(errno)));

        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strlcpy(ifr.ifr_name, netif.name, sizeof(ifr.ifr_name));
    if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
        close(sock_fd);
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Ioctl error\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    *enabled = (ifr.ifr_flags & IFF_UP);
    close(sock_fd);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_socket_dev_init(int unit, const bcmpkt_dev_init_t *cfg)
{
    uint32_t i, meta_len;
    ngknet_dev_cfg_t dev_cfg;
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
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    if (dev_initialized[unit]) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "Network device has already been initialized\n")));
    }

    sal_memset(&dev_cfg, 0, sizeof(dev_cfg));
    if (sal_strlen(cfg->name) > 0) {
        sal_strlcpy(dev_cfg.name, cfg->name, sizeof(dev_cfg.name));
    }
    else {
        sal_sprintf(dev_cfg.name, "%s%d", "bcm", unit);
    }

    dev_name = bcmdrd_dev_type_str(unit);
    sal_strncpy(dev_cfg.type_str, dev_name, sizeof(dev_cfg.type_str) - 1);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_id_get(unit, &dev_cfg.dev_id));
    if (dev_cfg.dev_id == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_len_get(dev_type, &meta_len));
    if (meta_len > BCMPKT_RCPU_TX_MH_SIZE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
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
             (BSL_META_U(unit, "Error: cgrp_bmp(0x%x) and cgrp_size(%"PRIu32")\n"),
              cfg->cgrp_bmp, cfg->cgrp_size));
    }

    sal_memcpy(dev_cfg.base_netif.macaddr, cfg->mac_addr, 6);
    sal_strlcpy(dev_cfg.base_netif.name, dev_cfg.name,
        sizeof(dev_cfg.base_netif.name));
    dev_cfg.base_netif.vlan = 1;
    dev_cfg.base_netif.id = -1;
    dev_cfg.base_netif.mtu = 1500;
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_init(unit, &dev_cfg));

    dev_initialized[unit] = true;

    /* Configure RCPU header. */
    SHR_IF_ERR_EXIT
        (bcmpkt_rcpu_hdr_set(unit, &rhdr));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "KNET device initialization succeeded: %s, groupbitmap 0x%x\n"),
                 dev_cfg.name, dev_cfg.bm_grp));

exit:
    /* Configure RCPU header failed, cleanup. */
    if (SHR_FUNC_ERR() &&
        (!SHR_FUNC_VAL_IS(SHR_E_EXISTS)) && dev_initialized[unit]) {
        bcmlu_ngknet_dev_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_socket_dev_initialized(int unit, bool *initialized)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(initialized, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    *initialized = dev_initialized[unit];

exit:
    SHR_FUNC_EXIT();
}

/*
 * Parse data, ignore front and 0 bytes
 * data -  data for parsing.
 * data_len - data maximum size.
 * offset - the first non 0 byte position.
 * size - the size from the first !0 byte to the last !0 byte.
 */
static void
parse_data_range(const uint8_t *data, int data_len, uint16_t *offset, uint16_t *size)
{
    uint16_t i;
    int found = 0;

    for (i = 0; i < data_len; i++) {
        if (data[i] != 0) {
            if (found == 0) {
                *offset = i;
                break;
            }
        }
    }
    *size = data_len - *offset;
}

/*
 * bfilter->oob_data is full 16 words dcb.
 * bfilter->oob_data_offset - dcb[0]
 */
static int
filter_to_bfilter_translate(int unit, bcmpkt_filter_t *filter,
                            ngknet_filter_t *bfilter)
{
    uint32_t *rxpmd = NULL;
    uint32_t *rxpmd_mask = NULL;
    uint32_t *rxpmd_ref = NULL;
    bcmlrd_variant_t variant;
    bool matching_raw_metadata = 0;
    bool matching_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(bfilter, 0, sizeof(*bfilter));
    bfilter->id = filter->id;
    bfilter->type = filter->type;
    bfilter->priority = filter->priority;
    sal_strcpy(bfilter->desc, filter->desc);
    if (filter->flags & BCMPKT_FILTER_F_ANY_DATA) {
        bfilter->flags |= NGKNET_FILTER_F_ANY_DATA;
    }
    if (filter->flags & BCMPKT_FILTER_F_STRIP_TAG) {
        bfilter->flags |= NGKNET_FILTER_F_STRIP_TAG;
    }
    if (filter->flags & BCMPKT_FILTER_F_MATCH_DMA_CHAN) {
        bfilter->flags |= NGKNET_FILTER_F_MATCH_CHAN;
    }
    bfilter->dest_type = filter->dest_type;
    bfilter->dest_id = filter->dest_id;
    bfilter->dest_proto = filter->dest_proto;
    bfilter->mirror_type = filter->mirror_type;
    bfilter->mirror_id = filter->mirror_id;
    bfilter->mirror_proto = filter->mirror_proto;
    sal_memcpy(bfilter->user_data, filter->user_data,
               sizeof(bfilter->user_data));

    if (filter->match_flags & BCMPKT_FILTER_M_RAW_METADATA) {
        matching_raw_metadata = 1;
    }

    if (filter->match_flags &
        (BCMPKT_FILTER_M_VLAN
        | BCMPKT_FILTER_M_INGPORT
        | BCMPKT_FILTER_M_SRC_MODPORT
        | BCMPKT_FILTER_M_SRC_MODID
        | BCMPKT_FILTER_M_REASON
        | BCMPKT_FILTER_M_FP_RULE
        | BCMPKT_FILTER_M_CPU_QUEUE)) {
        matching_flags = 1;
    }

    if (matching_raw_metadata && matching_flags) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Cannot set filter of raw metadata \n"
                              "and metadata matching flags at the "
                              "same time.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (matching_raw_metadata ||
        ((filter->match_flags & BCMPKT_FILTER_M_ERROR) &&
         !matching_flags)) {
        /*
         * For the cases of
         * - Raw metadata matching only.
         * - Error flag matching only.
         * - Raw metadata and error flag matching.
         */
        uint32_t rxmeta_size_words;
        uint32_t rxmeta_size_bytes;
        uint32_t dev_type;

        SHR_IF_ERR_EXIT
            (bcmpkt_dev_type_get(unit, &dev_type));

        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_len_get(dev_type, &rxmeta_size_bytes));
        /*
         * Additional buffer of a word for recording filter flag
         * BCMPKT_FILTER_M_ERROR.
         */
        rxmeta_size_bytes += 4;
        /* Ensure word alignment. */
        rxmeta_size_words = NGKNET_BYTES2WORDS(rxmeta_size_bytes);
        rxmeta_size_bytes = rxmeta_size_words * sizeof(uint32_t);

        rxpmd = sal_alloc(rxmeta_size_bytes, "bcmpktKnetFilterData");
        SHR_NULL_CHECK(rxpmd, SHR_E_MEMORY);
        rxpmd_mask = sal_alloc(rxmeta_size_bytes, "bcmpktKnetFilterMask");
        SHR_NULL_CHECK(rxpmd_mask, SHR_E_MEMORY);

        sal_memset(rxpmd, 0, rxmeta_size_bytes);
        sal_memset(rxpmd_mask, 0, rxmeta_size_bytes);

        if (matching_raw_metadata) {
            /*
             * Copy RXPMD raw data only. The last 32-bits of rxpmd are for
             * error flags.
             */
            sal_memcpy(rxpmd, filter->m_raw_metadata,
                       rxmeta_size_bytes - 4);
            sal_memcpy(rxpmd_mask, filter->m_raw_metadata_mask,
                       rxmeta_size_bytes - 4);
            /* Specify the filter is created with raw metadata. */
            bfilter->flags |= NGKNET_FILTER_F_RAW_PMD;
        }

        if (filter->match_flags & BCMPKT_FILTER_M_ERROR) {
            rxpmd[rxmeta_size_words - 1] |= BCMPKT_RXMETA_ST_CELL_ERROR;
            rxpmd_mask[rxmeta_size_words - 1] |= BCMPKT_RXMETA_ST_CELL_ERROR;
        }

        /* Search offset and data size */
        parse_data_range((uint8_t *)rxpmd_mask, rxmeta_size_bytes,
                         &bfilter->oob_data_offset, &bfilter->oob_data_size);
        sal_memcpy(bfilter->data.b,
                   &((uint8_t *)rxpmd)[bfilter->oob_data_offset],
                   bfilter->oob_data_size);
        sal_memcpy(bfilter->mask.b,
                   &((uint8_t *)rxpmd_mask)[bfilter->oob_data_offset],
                   bfilter->oob_data_size);
    } else if (matching_flags) {
        /*
         * For the cases of
         * - Individual flag matching only.
         * - Individual flag and error flag matching.
         */
        uint32_t mask_val;
        HIGIG_t *hg = NULL, *hg_mask = NULL;
        uint32_t *u32p = NULL;
        uint32_t rxmeta_size_words;
        uint32_t rxmeta_size_bytes;
        uint32_t dev_type;
        int rv = SHR_E_NONE;
        int mh_support = 0;

        SHR_IF_ERR_EXIT
            (bcmpkt_dev_type_get(unit, &dev_type));

        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_len_get(dev_type, &rxmeta_size_bytes));
        /*
         * Additional buffer of a word for recording filter flag
         * BCMPKT_FILTER_M_ERROR.
         */
        rxmeta_size_bytes += 4;
        /* Ensure word alignment. */
        rxmeta_size_words = NGKNET_BYTES2WORDS(rxmeta_size_bytes);
        rxmeta_size_bytes = rxmeta_size_words * sizeof(uint32_t);

        rxpmd = sal_alloc(rxmeta_size_bytes, "bcmpktKnetFilterData");
        SHR_NULL_CHECK(rxpmd, SHR_E_MEMORY);
        rxpmd_mask = sal_alloc(rxmeta_size_bytes, "bcmpktKnetFilterMask");
        SHR_NULL_CHECK(rxpmd_mask, SHR_E_MEMORY);
        rxpmd_ref = sal_alloc(rxmeta_size_bytes, "bcmpktKnetFilterRef");
        SHR_NULL_CHECK(rxpmd_ref, SHR_E_MEMORY);

        sal_memset(rxpmd, 0, rxmeta_size_bytes);
        sal_memset(rxpmd_mask, 0, rxmeta_size_bytes);
        /* This is for create field mask. */
        sal_memset(rxpmd_ref, 0xff, rxmeta_size_bytes);

        rv = bcmpkt_rxpmd_mh_get(dev_type, rxpmd, &u32p);
        if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
            SHR_IF_ERR_EXIT(rv);
        }
        if (SHR_SUCCESS(rv)) {
            hg = (HIGIG_t *)u32p;
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_mh_get(dev_type, rxpmd_mask, &u32p));
            hg_mask = (HIGIG_t *)u32p;
            mh_support = 1;
        }

        if (filter->match_flags & BCMPKT_FILTER_M_VLAN) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd_ref,
                                        BCMPKT_RXPMD_OUTER_VID, &mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd_mask,
                                        BCMPKT_RXPMD_OUTER_VID, mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd,
                                        BCMPKT_RXPMD_OUTER_VID, filter->m_vlan));
        }
        if (filter->match_flags & BCMPKT_FILTER_M_INGPORT) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd_ref,
                                        BCMPKT_RXPMD_SRC_PORT_NUM, &mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd_mask,
                                        BCMPKT_RXPMD_SRC_PORT_NUM, mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd,
                                        BCMPKT_RXPMD_SRC_PORT_NUM,
                                        filter->m_ingport));
        }

        if (filter->match_flags & BCMPKT_FILTER_M_SRC_MODPORT) {
            if (!mh_support) {
                SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
            }
            HIGIG_SRC_PORT_TGIDf_SET(*hg, filter->m_src_modport);
            HIGIG_SRC_PORT_TGIDf_SET(*hg_mask, 0x3f);
        }
        if (filter->match_flags & BCMPKT_FILTER_M_SRC_MODID) {
            if (!mh_support) {
                SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
            }
            HIGIG_SRC_MODID_LSf_SET(*hg, filter->m_src_modid & 0x1f);
            HIGIG_SRC_MODID_LSf_SET(*hg_mask, 0x1f);
            HIGIG_SRC_MODID_5f_SET(*hg, ((filter->m_src_modid & 0x20) != 0));
            HIGIG_SRC_MODID_5f_SET(*hg_mask, 0x1);
            HIGIG_SRC_MODID_6f_SET(*hg, ((filter->m_src_modid & 0x40) != 0));
            HIGIG_SRC_MODID_6f_SET(*hg_mask, 0x1);
        }

        if (filter->match_flags & BCMPKT_FILTER_M_REASON) {
            bcmpkt_rx_reasons_t reasons;
            bcmpkt_rxpmd_flex_reasons_t flex_reasons;
            uint32_t fdata_len, fdata_ref_len, fdata_mask_len;
            uint32_t *fdata = NULL, *fdata_ref = NULL, *fdata_mask = NULL;

            SHR_IF_ERR_EXIT_EXCEPT_IF(
                bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd,
                                          &fdata, &fdata_len),
                SHR_E_UNAVAIL);

            if (fdata_len) {
                variant = bcmlrd_variant_get(unit);
                if (variant == BCMLRD_VARIANT_T_NONE) {
                    SHR_IF_ERR_EXIT(SHR_E_UNIT);
                }

                /* Rx reason from flex data */
                SHR_IF_ERR_EXIT(
                    bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd_ref,
                                              &fdata_ref, &fdata_ref_len));
                SHR_IF_ERR_EXIT(
                    bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd_mask,
                                              &fdata_mask, &fdata_mask_len));

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "fdata_len (%d), fdata_ref_len(%d) "
                                        "fdata_mask_len (%d)\n"),
                             fdata_len,
                             fdata_ref_len,
                             fdata_mask_len));

                SHR_IF_ERR_EXIT
                    (bcmpkt_rxpmd_flex_reasons_get(variant, fdata_ref,
                                                   &flex_reasons));
                SHR_IF_ERR_EXIT
                    (bcmpkt_rxpmd_flex_reasons_set(variant,
                                                   &flex_reasons, fdata_mask));
                SHR_IF_ERR_EXIT
                    (bcmpkt_rxpmd_flex_reasons_set(variant,
                                                   &filter->m_flex_reason,
                                                   fdata));
            } else {
                SHR_IF_ERR_EXIT
                    (bcmpkt_rxpmd_reasons_get(dev_type, rxpmd_ref, &reasons));
                SHR_IF_ERR_EXIT
                    (bcmpkt_rxpmd_reasons_set(dev_type, &reasons, rxpmd_mask));
                SHR_IF_ERR_EXIT
                    (bcmpkt_rxpmd_reasons_set(dev_type,
                                              &filter->m_reason, rxpmd));
            }

        }
        if (filter->match_flags & BCMPKT_FILTER_M_FP_RULE) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd_ref,
                                        BCMPKT_RXPMD_MATCHED_RULE, &mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd_mask,
                                        BCMPKT_RXPMD_MATCHED_RULE, mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd,
                                        BCMPKT_RXPMD_MATCHED_RULE,
                                        filter->m_fp_rule));
        }
        if (filter->match_flags & BCMPKT_FILTER_M_CPU_QUEUE) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd_ref,
                                        BCMPKT_RXPMD_CPU_COS, &mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd_mask,
                                        BCMPKT_RXPMD_CPU_COS, mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd,
                                        BCMPKT_RXPMD_CPU_COS,
                                        filter->m_cpu_queue));
        }

        if (filter->match_flags & BCMPKT_FILTER_M_ERROR) {
            rxpmd[rxmeta_size_words - 1] = BCMPKT_RXMETA_ST_CELL_ERROR;
            rxpmd_mask[rxmeta_size_words - 1] = BCMPKT_RXMETA_ST_CELL_ERROR;
        }

        /* Search offset and data size */
        parse_data_range((uint8_t *)rxpmd_mask, rxmeta_size_bytes,
                         &bfilter->oob_data_offset, &bfilter->oob_data_size);
        sal_memcpy(bfilter->data.b,
                   &((uint8_t *)rxpmd)[bfilter->oob_data_offset],
                   bfilter->oob_data_size);
        sal_memcpy(bfilter->mask.b,
                   &((uint8_t *)rxpmd_mask)[bfilter->oob_data_offset],
                   bfilter->oob_data_size);
    }


    if (filter->match_flags & BCMPKT_FILTER_M_RAW) {
        uint32_t i;
        for (i = 0; i < filter->raw_size; i++) {
            if (!filter->m_raw_mask[i]) {
                filter->m_raw_data[i] = 0;
            }
        }
        parse_data_range(filter->m_raw_mask, filter->raw_size,
                         &bfilter->pkt_data_offset, &bfilter->pkt_data_size);
        if (bfilter->oob_data_size + bfilter->pkt_data_size >
            NGKNET_FILTER_BYTES_MAX) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Exceed max filter size (%d).\n"
                                  "bfilter->oob_data_size (%d), "
                                  "bfilter->pkt_data_size (%d)\n"),
                      NGKNET_FILTER_BYTES_MAX,
                      bfilter->oob_data_size,
                      bfilter->pkt_data_size));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        sal_memcpy(&bfilter->data.b[bfilter->oob_data_size],
                   &filter->m_raw_data[bfilter->pkt_data_offset],
                   bfilter->pkt_data_size);
        sal_memcpy(&bfilter->mask.b[bfilter->oob_data_size],
                   &filter->m_raw_mask[bfilter->pkt_data_offset],
                   bfilter->pkt_data_size);
    }
    bfilter->chan = filter->dma_chan;

exit:
    if (rxpmd != NULL) {
        sal_free(rxpmd);
    }
    if (rxpmd_mask != NULL) {
        sal_free(rxpmd_mask);
    }
    if (rxpmd_ref != NULL) {
        sal_free(rxpmd_ref);
    }
    SHR_FUNC_EXIT();
}

static int
bfilter_to_filter_translate(int unit, ngknet_filter_t *bfilter,
                            bcmpkt_filter_t *filter)
{
    HIGIG_t *hg = NULL, *hg_mask = NULL;
    uint32_t *u32p = NULL;
    uint32_t mask_val = 0;
    bcmpkt_rx_reasons_t reason_mask;
    bcmpkt_rxpmd_flex_reasons_t flex_reason_mask;
    int reason_count = 0;
    uint32_t *rxpmd = NULL, *fdata = NULL;
    uint32_t *rxpmd_mask = NULL, *fdata_mask = NULL;
    uint32_t fdata_len, fdata_mask_len;
    uint32_t rxmeta_size_words;
    uint32_t rxmeta_size_bytes;
    uint32_t dev_type;
    uint32_t val;
    int rv = SHR_E_NONE;
    int mh_support = 0;
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_len_get(dev_type, &rxmeta_size_bytes));
    /* Additional buffer of a word for recording filter flag BCMPKT_FILTER_M_ERROR. */
    rxmeta_size_bytes += 4;
    /* Ensure word alignment. */
    rxmeta_size_words = NGKNET_BYTES2WORDS(rxmeta_size_bytes);
    rxmeta_size_bytes = rxmeta_size_words * sizeof(uint32_t);

    rxpmd = sal_alloc(rxmeta_size_bytes, "bcmpktKnetFilterData");
    SHR_NULL_CHECK(rxpmd, SHR_E_MEMORY);
    rxpmd_mask = sal_alloc(rxmeta_size_bytes, "bcmpktKnetFilterMask");
    SHR_NULL_CHECK(rxpmd_mask, SHR_E_MEMORY);
    sal_memset(rxpmd, 0, rxmeta_size_bytes);
    sal_memset(rxpmd_mask, 0, rxmeta_size_bytes);

    sal_memset(filter, 0, sizeof(*filter));
    filter->id = bfilter->id;
    filter->type = bfilter->type;
    filter->priority = bfilter->priority;
    sal_strcpy(filter->desc, bfilter->desc);
    if (bfilter->flags & NGKNET_FILTER_F_ANY_DATA) {
        filter->flags |= BCMPKT_FILTER_F_ANY_DATA;
    }
    if (bfilter->flags & NGKNET_FILTER_F_STRIP_TAG) {
        filter->flags |= BCMPKT_FILTER_F_STRIP_TAG;
    }
    if (bfilter->flags & NGKNET_FILTER_F_MATCH_CHAN) {
        filter->flags |= BCMPKT_FILTER_F_MATCH_DMA_CHAN;
    }
    filter->dest_type = bfilter->dest_type;
    filter->dest_id = bfilter->dest_id;
    filter->dest_proto = bfilter->dest_proto;
    filter->mirror_type = bfilter->mirror_type;
    filter->mirror_id = bfilter->mirror_id;
    filter->mirror_proto = bfilter->mirror_proto;
    filter->dma_chan = bfilter->chan;
    sal_memcpy(filter->user_data, bfilter->user_data, sizeof(filter->user_data));

    if (bfilter->pkt_data_size > 0) {
        filter->raw_size = bfilter->pkt_data_size + bfilter->pkt_data_offset;
        sal_memcpy(&filter->m_raw_data[bfilter->pkt_data_offset],
                   &bfilter->data.b[bfilter->oob_data_size], bfilter->pkt_data_size);
        sal_memcpy(&filter->m_raw_mask[bfilter->pkt_data_offset],
                   &bfilter->mask.b[bfilter->oob_data_size], bfilter->pkt_data_size);
        filter->match_flags |= BCMPKT_FILTER_M_RAW;
    }

    sal_memcpy(&((uint8_t *)rxpmd)[bfilter->oob_data_offset],
               bfilter->data.b,
               bfilter->oob_data_size);
    sal_memcpy(&((uint8_t *)rxpmd_mask)[bfilter->oob_data_offset],
               bfilter->mask.b,
               bfilter->oob_data_size);

    if (bfilter->flags & NGKNET_FILTER_F_RAW_PMD) {
        /* The filter is created with raw metadata. */
        if (bfilter->oob_data_size > 0) {
            if (rxmeta_size_words - 1 > BCMPKT_RXPMD_SIZE_WORDS) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Internal error. RXPMD size is invalid.\n"
                                      "Max array size supported (%d) 32-bit "
                                      "words, device reported size (%d) 32-bit "
                                      "words\n"),
                                      BCMPKT_RXPMD_SIZE_WORDS,
                                      rxmeta_size_words - 1));
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Copy RXPMD raw data only. The last 32-bits of rxpmd are for error flags. */
            sal_memcpy(filter->m_raw_metadata, rxpmd,
                       rxmeta_size_bytes - 4);
            sal_memcpy(filter->m_raw_metadata_mask, rxpmd_mask,
                       rxmeta_size_bytes - 4);
            filter->match_flags |= BCMPKT_FILTER_M_RAW_METADATA;
        }
    } else {
        rv = bcmpkt_rxpmd_mh_get(dev_type, rxpmd, &u32p);
        if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
            SHR_IF_ERR_EXIT(rv);
        }
        if (SHR_SUCCESS(rv)) {
            hg = (HIGIG_t *)u32p;
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_mh_get(dev_type, rxpmd_mask, &u32p));
            hg_mask = (HIGIG_t *)u32p;
            mh_support = 1;
        }

        mask_val = 0;
        rv = bcmpkt_rxpmd_field_get(dev_type, rxpmd_mask,
                                    BCMPKT_RXPMD_OUTER_VID, &mask_val);
        if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
            SHR_IF_ERR_EXIT(rv);
        }
        if (SHR_SUCCESS(rv) && mask_val) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                        BCMPKT_RXPMD_OUTER_VID, &val));
            filter->m_vlan = val & 0xffff;
            filter->match_flags |= BCMPKT_FILTER_M_VLAN;
        }

        mask_val = 0;
        rv = bcmpkt_rxpmd_field_get(dev_type, rxpmd_mask,
                                    BCMPKT_RXPMD_SRC_PORT_NUM, &mask_val);
        if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
            SHR_IF_ERR_EXIT(rv);
        }
        if (SHR_SUCCESS(rv) && mask_val) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                        BCMPKT_RXPMD_SRC_PORT_NUM, &val));
            filter->m_ingport = (int)val;
            filter->match_flags |= BCMPKT_FILTER_M_INGPORT;
        }

        mask_val = 0;
        rv = bcmpkt_rxpmd_field_get(dev_type, rxpmd_mask,
                                    BCMPKT_RXPMD_CPU_COS, &mask_val);
        if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
            SHR_IF_ERR_EXIT(rv);
        }
        if (SHR_SUCCESS(rv) && mask_val) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                        BCMPKT_RXPMD_CPU_COS, &val));
            filter->m_cpu_queue = (int)val;
            filter->match_flags |= BCMPKT_FILTER_M_CPU_QUEUE;
        }

        mask_val = 0;
        rv = bcmpkt_rxpmd_field_get(dev_type, rxpmd_mask,
                                    BCMPKT_RXPMD_MATCHED_RULE, &mask_val);
        if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
            SHR_IF_ERR_EXIT(rv);
        }
        if (SHR_SUCCESS(rv) && mask_val) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                        BCMPKT_RXPMD_MATCHED_RULE, &val));
            filter->m_fp_rule = val;
            filter->match_flags |= BCMPKT_FILTER_M_FP_RULE;
        }

        if (mh_support) {
            if (HIGIG_SRC_PORT_TGIDf_GET(*hg_mask) > 0) {
                filter->m_src_modport = HIGIG_SRC_PORT_TGIDf_GET(*hg);
                filter->match_flags |= BCMPKT_FILTER_M_SRC_MODPORT;
            }

            if ((HIGIG_SRC_MODID_LSf_GET(*hg_mask) & 0x1f) > 0) {
                filter->m_src_modid = (HIGIG_SRC_MODID_LSf_GET(*hg) & 0x1f)
                                    | (HIGIG_SRC_MODID_5f_GET(*hg) << 5)
                                    | (HIGIG_SRC_MODID_6f_GET(*hg) << 6);
                filter->match_flags |= BCMPKT_FILTER_M_SRC_MODID;
            }
        }

        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd, &fdata, &fdata_len),
            SHR_E_UNAVAIL);

        if (fdata_len) {
            variant = bcmlrd_variant_get(unit);
            if (variant == BCMLRD_VARIANT_T_NONE) {
                SHR_IF_ERR_EXIT(SHR_E_UNIT);
            }

            /* Rx reason from flex data */
            SHR_IF_ERR_EXIT(
                bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd_mask,
                                          &fdata_mask, &fdata_mask_len));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "fdata_len (%d), "
                                    "fdata_mask_len (%d)\n"),
                         fdata_len,
                         fdata_mask_len));

            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_flex_reasons_get(variant, fdata_mask,
                                               &flex_reason_mask));
            SHR_BITCOUNT_RANGE(flex_reason_mask.pbits, reason_count,
                               0, BCMPKT_RXPMD_FLEX_REASON_COUNT);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "reason_count (%d)\n"),
                        reason_count));
            if (reason_count > 0) {
                SHR_IF_ERR_EXIT
                    (bcmpkt_rxpmd_flex_reasons_get(variant, fdata,
                                                   &filter->m_flex_reason));
                filter->match_flags |= BCMPKT_FILTER_M_REASON;
            }
        } else {
            rv = bcmpkt_rxpmd_reasons_get(dev_type, rxpmd_mask, &reason_mask);
            if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
                SHR_IF_ERR_EXIT(rv);
            }
            if (SHR_SUCCESS(rv)) {
                SHR_BITCOUNT_RANGE(reason_mask.pbits, reason_count,
                                   0, BCMPKT_RX_REASON_COUNT);
                if (reason_count > 0) {
                    SHR_IF_ERR_EXIT
                        (bcmpkt_rxpmd_reasons_get(dev_type, rxpmd,
                                                  &filter->m_reason));
                    filter->match_flags |= BCMPKT_FILTER_M_REASON;
                }
            }
        }
    }

    /* Support error match, but not support ~error match.
     * To support ~error match, need to add a variable.
     */
    if (*(rxpmd + (rxmeta_size_words - 1))  &
        *(rxpmd_mask + (rxmeta_size_words - 1)) &
        BCMPKT_RXMETA_ST_CELL_ERROR) {
        filter->match_flags |= BCMPKT_FILTER_M_ERROR;
    }
exit:
    if (rxpmd != NULL) {
        sal_free(rxpmd);
    }
    if (rxpmd_mask != NULL) {
        sal_free(rxpmd_mask);
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_create(int unit, bcmpkt_filter_t *filter)
{
    ngknet_filter_t bfilter;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (filter_to_bfilter_translate(unit, filter, &bfilter));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_filter_create(unit, &bfilter));

    filter->id = bfilter.id;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_destroy(int unit, int filter_id)
{
    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_filter_destroy(unit, filter_id));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_get(int unit, int filter_id, bcmpkt_filter_t *filter)
{
    ngknet_filter_t bfilter;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    sal_memset(&bfilter, 0, sizeof(bfilter));
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_filter_get(unit, filter_id, &bfilter));

    SHR_IF_ERR_EXIT
        (bfilter_to_filter_translate(unit, &bfilter, filter));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_traverse(int unit, bcmpkt_filter_traverse_cb_f cb_func,
                      void *cb_data)
{
    ngknet_filter_t bfilter;
    bcmpkt_filter_t filter;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    sal_memset(&bfilter, 0, sizeof(bfilter));
    do {
        int rv = bcmlu_ngknet_filter_get_next(unit, &bfilter);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (rv, SHR_E_NOT_FOUND);

        /* No entry, return. */
        if (rv == SHR_E_NOT_FOUND) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "No filter entry\n")));
            break;
        }

        SHR_IF_ERR_EXIT
            (bfilter_to_filter_translate(unit, &bfilter, &filter));

        cb_func(unit, &filter, cb_data);
    } while (bfilter.next != 0);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_max_get(int unit, int *max_num)
{
    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(max_num, SHR_E_PARAM);

    *max_num = BCMLU_FILTER_MAX;

exit:
    SHR_FUNC_EXIT();
}

static int
filter_delete(int unit, const bcmpkt_filter_t *filter, void *data)
{
    if (filter != NULL) {
        int rv;
        rv = bcmlu_filter_destroy(unit, filter->id);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Destroy filter %d failed: (%d)\n"),
                                  filter->id, rv));
        }
    }

    return SHR_E_NONE;
}

static int
netif_delete(int unit, const bcmpkt_netif_t *netif, void *data)
{
    if (netif != NULL) {
        int rv;
        bool created;
        rv = bcmpkt_socket_created(unit, netif->id, &created);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit,
                                  "Get SOCKET created status failed: (%d)\n"),
                       rv));
        } else if (created) {
            rv = bcmpkt_socket_destroy(unit, netif->id);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Destroy SOCKET on network interface %d "
                                      "failed: (%d)\n"),
                           netif->id, rv));
            }
        }
        rv = bcmlu_netif_destroy(unit, netif->id);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Destroy network interface %d failed: "
                                  "(%d)\n"),
                       netif->id, rv));
        }
    }

    return SHR_E_NONE;
}

static int
bcmlu_socket_dev_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (socket_dev_initialized(unit)) {

        (void)bcmlu_filter_traverse(unit, filter_delete, NULL);

        (void)bcmlu_netif_traverse(unit, netif_delete, NULL);

        (void)bcmlu_socket_dev_disable(unit);

        dev_initialized[unit] = false;

        SHR_IF_ERR_EXIT
            (bcmlu_ngknet_dev_cleanup(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rx_rate_limit_set(int unit, int rate_limit)
{

    SHR_FUNC_ENTER(unit);

    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_rx_rate_limit(&rate_limit, true));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rx_rate_limit_get(int unit, int *rate_limit)
{

    SHR_FUNC_ENTER(unit);

    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_rx_rate_limit(rate_limit, false));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rcpu_hdr_set(int unit, bcmpkt_rcpu_hdr_t *hdr)
{
    struct ngknet_rcpu_hdr nhdr;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(hdr, SHR_E_PARAM);

    sal_memcpy(&nhdr, hdr, sizeof(nhdr));
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_rcpu_config(unit, &nhdr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rcpu_hdr_get(int unit, bcmpkt_rcpu_hdr_t *hdr)
{
    struct ngknet_rcpu_hdr nhdr;

    SHR_FUNC_ENTER(unit);
    if (!socket_dev_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(hdr, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_rcpu_get(unit, &nhdr));

    sal_memcpy(hdr, &nhdr, sizeof(nhdr));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_knet_attach(void)
{
    int rv = SHR_E_NONE;

    bcmpkt_knet_t knet_vect = {
        .initialized = 1,
        .driver_name = "KNET",
        .netif_ops.create = bcmlu_netif_create,
        .netif_ops.get = bcmlu_netif_get,
        .netif_ops.traverse = bcmlu_netif_traverse,
        .netif_ops.destroy = bcmlu_netif_destroy,
        .netif_ops.link_update = bcmlu_netif_link_update,
        .netif_ops.netif_max_get = bcmlu_netif_max_get,
        .filter_ops.create = bcmlu_filter_create,
        .filter_ops.get = bcmlu_filter_get,
        .filter_ops.traverse = bcmlu_filter_traverse,
        .filter_ops.destroy = bcmlu_filter_destroy,
        .filter_ops.filter_max_get = bcmlu_filter_max_get,
    };

    bcmpkt_dev_t dev_vect = {
        .initialized = 1,
        .driver_name = "KNET",
        .driver_type = BCMPKT_DEV_DRV_T_KNET,

        .init = bcmlu_socket_dev_init,
        .dev_initialized = bcmlu_socket_dev_initialized,
        .cleanup = bcmlu_socket_dev_cleanup,
        .enable = bcmlu_socket_dev_enable,
        .disable = bcmlu_socket_dev_disable,
        .enabled = bcmlu_socket_dev_enabled,
        .dma_chan_ops.set = bcmlu_chan_set,
        .dma_chan_ops.get = bcmlu_chan_get,
        .dma_chan_ops.get_list = bcmlu_chan_get_list,
        .info_dump = bcmlu_socket_dev_info_dump,
        .stats_get = bcmlu_socket_dev_stats_get,
        .stats_dump = bcmlu_socket_dev_stats_dump,
        .stats_clear = bcmlu_socket_dev_stats_clear,
        .rcpu_hdr_set = bcmlu_rcpu_hdr_set,
        .rcpu_hdr_get = bcmlu_rcpu_hdr_get,
        .rx_rate_limit_ops.set = bcmlu_rx_rate_limit_set,
        .rx_rate_limit_ops.get = bcmlu_rx_rate_limit_get
    };

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    rv = bcmlu_ngknet_dev_check();
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_dev_drv_register(&dev_vect), SHR_E_EXISTS);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_knet_drv_register(&knet_vect), SHR_E_EXISTS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_knet_detach(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_unregister());

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_unregister(BCMPKT_DEV_DRV_T_KNET));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_close());

exit:
    SHR_FUNC_EXIT();
}

