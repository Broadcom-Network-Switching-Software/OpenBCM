/*! \file pktio.c
 *
 * Emulator over SDKLT PacketIO interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <sal/core/boot.h>

#include <bcm/pktio.h>
#include <bcm/pktio_defs.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>

#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/pktio.h>
#include <bcm_int/ltsw/pktio_int.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/mbcm/pktio.h>

#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_trace.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <sal/sal_assert.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_PKTIO

/*!
 * \brief Describes a registered callout for RX.
 */
typedef struct pktio_rx_callout_s {

    /*! Next in sorted list. */
    struct pktio_rx_callout_s *rco_next;

    /*! Name for handler. */
    const char *rco_name;

    /*! Cookie for call out. */
    void *rco_cookie;

    /*! Callout function. */
    bcm_pktio_rx_cb_f rco_function;

    /*! Flags. */
    uint32_t rco_flags;

    /*! Callout priority 0 == lowest. */
    uint8_t rco_priority;

    /*! Counter. */
    uint32_t rco_pkts_handled;

} pktio_rx_callout_t;

/*! Setup Rx callout. */
#define SETUP_RCO(rco, name, fun, pri, cookie, next, flags)   \
    do {                                                 \
        (rco)->rco_name = (name);                        \
        (rco)->rco_function = (fun);                     \
        (rco)->rco_priority = (pri);                     \
        (rco)->rco_cookie = (cookie);                    \
        (rco)->rco_next = (next);                        \
        (rco)->rco_flags = (flags);                      \
        (rco)->rco_pkts_handled = 0;                     \
    } while (0)

/*! SDKLT callback data. */
typedef struct pktio_cb_data_s {

    /*! Packet IO callback function. */
    void *cb_func;

    /*! Packet IO callback data. */
    void *cb_data;
} pktio_cb_data_t;

/*! Packet IO UNET mode. */
#define PKTIO_MODE_UNET     0

/*! Packet IO KNET mode. */
#define PKTIO_MODE_KNET     1

/*!
 * \brief Packet IO information.
 */
typedef struct ltsw_pktio_info_s {

    /*! Device driver type. */
    bcmpkt_dev_drv_types_t drv_type;

    /*! Device type. */
    bcmdrd_dev_type_t dev_type;

    /*! Device variant. */
    bcmlrd_variant_t variant;

#define PKTIO_F_GIH     0x1
#define PKTIO_F_RXFLEX  0x2
#define PKTIO_F_HIGIG3  0x4
    /*! Packet IO flags. */
    uint32_t flags;

    /*! Packet metadata support bitmap. */
    bcm_pktio_fid_support_t support;

    /*! Callout linked list. */
    pktio_rx_callout_t *rco;

    /*! Sync for handler list. */
    sal_mutex_t mutex;

    /*! Default network interface in KNET mode. */
    int def_netif;

} ltsw_pktio_info_t;

/*! Per chip packet IO information. */
static ltsw_pktio_info_t *pktio_info[BCM_MAX_NUM_UNITS] = {0};

#define LTSW_PKTIO_LOCK(unit)       \
    sal_mutex_take(pktio_info[unit]->mutex, SAL_MUTEX_FOREVER)

#define LTSW_PKTIO_UNLOCK(unit)     \
    sal_mutex_give(pktio_info[unit]->mutex)

/*! Per chip packet IO initialize flag. */
static bool pktio_initialized[BCM_MAX_NUM_UNITS] = {0};

/*!
 * \brief Packet IO ha information.
 */
typedef struct ltsw_pktio_ha_info_s {

    /* Packet IO working mode. */
    bcmpkt_dev_drv_types_t drv_type;

    /* API network interface id. */
    int netif_id;
} ltsw_pktio_ha_info_t;

/*! Default netif MAC address. */
#define LTSW_DEF_NETIF_MAC  {0x00, 0xbc, 0x00, 0x00, 0x00, 0x00}

/*! Maximum default packet size. */
#define MAX_FRAME_SIZE_DEF  (9472)

/*! Per device buffer count. */
#define PKTIO_BCOUNT_SHARED (1024) /* Match with SDKLT init value. */
#define PKTIO_BCOUNT_DEF    (512) /* WAR for TD4 BU. */
#define PKTIO_BCOUNT_SIM    (256)

/*! Per channel ring size. */
#define PKTIO_RSIZE_DEF     (32) /* WAR for TD4 BU. */
#define PKTIO_RSIZE_SIM     (16)

/*! Match id array size. */
#define PKTIO_MATCH_ID_SIZE (2)

/*! Packet IO enumeration map. */
typedef struct ltsw_pktio_enum_map_s {

    /* Packet IO enumeration. */
    int api_enum;

    /* SDKLT enumeration. */
    int lt_enum;
} ltsw_pktio_enum_map_t;

/*! Packet IO invalid enumeeration. */
#define PKTIO_INVALID_ENUM  (-1)

/*! Network interface flags. */
static const
ltsw_pktio_enum_map_t netif_flags[] = {
    {BCM_PKTIO_NETIF_F_RCPU_ENCAP, BCMPKT_NETIF_F_RCPU_ENCAP},
    {BCM_PKTIO_NETIF_F_BIND_RX_CH, BCMPKT_NETIF_F_BIND_RX_CH},
    {BCM_PKTIO_NETIF_F_BIND_TX_PORT, BCMPKT_NETIF_F_BIND_TX_PORT},
    {BCM_PKTIO_NETIF_F_ADD_TAG, BCMPKT_NETIF_F_ADD_TAG},
    {PKTIO_INVALID_ENUM, PKTIO_INVALID_ENUM} /* Always last one */
};

/*! Filter type. */
static const
ltsw_pktio_enum_map_t filter_type[] = {
    {BCM_PKTIO_FILTER_T_RX_PKT, BCMPKT_FILTER_T_RX_PKT},
    {PKTIO_INVALID_ENUM, PKTIO_INVALID_ENUM} /* Always last one */
};

/*! Filter flags. */
static const
ltsw_pktio_enum_map_t filter_flags[] = {
    {BCM_PKTIO_FILTER_F_STRIP_TAG ,BCMPKT_FILTER_F_STRIP_TAG},
    {BCM_PKTIO_FILTER_F_ANY_DATA, BCMPKT_FILTER_F_ANY_DATA},
    {PKTIO_INVALID_ENUM, PKTIO_INVALID_ENUM} /* Always last one */
};

/*! Filter dest type. */
static const
ltsw_pktio_enum_map_t filter_dest_type[] = {
    {BCM_PKTIO_DEST_T_NULL, BCMPKT_DEST_T_NULL},
    {BCM_PKTIO_DEST_T_NETIF, BCMPKT_DEST_T_NETIF},
    {BCM_PKTIO_DEST_T_BCM_RX_API, BCMPKT_DEST_T_BCM_RX_API},
    {BCM_PKTIO_DEST_T_CALLBACK, BCMPKT_DEST_T_CALLBACK},
    {PKTIO_INVALID_ENUM, PKTIO_INVALID_ENUM} /* Always last one */
};

/*! Filter match flags. */
static const
ltsw_pktio_enum_map_t filter_match_flags[] = {
    {BCM_PKTIO_FILTER_M_RAW, BCMPKT_FILTER_M_RAW},
    {BCM_PKTIO_FILTER_M_VLAN, BCMPKT_FILTER_M_VLAN},
    {BCM_PKTIO_FILTER_M_INGPORT, BCMPKT_FILTER_M_INGPORT},
    {BCM_PKTIO_FILTER_M_SRC_MODPORT, BCMPKT_FILTER_M_SRC_MODPORT},
    {BCM_PKTIO_FILTER_M_SRC_MODID, BCMPKT_FILTER_M_SRC_MODID},
    {BCM_PKTIO_FILTER_M_REASON, BCMPKT_FILTER_M_REASON},
    {BCM_PKTIO_FILTER_M_FP_RULE, BCMPKT_FILTER_M_FP_RULE},
    {BCM_PKTIO_FILTER_M_ERROR, BCMPKT_FILTER_M_ERROR},
    {BCM_PKTIO_FILTER_M_CPU_QUEUE, BCMPKT_FILTER_M_CPU_QUEUE},
    {BCM_PKTIO_FILTER_M_PMD, BCMPKT_FILTER_M_RAW_METADATA},
    {PKTIO_INVALID_ENUM, PKTIO_INVALID_ENUM} /* Always last one */
};

static const shr_enum_map_t dop_id_names[] = {
    BCMPKT_TRACE_DOP_ID_NAME_MAP_INIT
};

static const shr_enum_map_t dop_field_names[] = {
    BCMPKT_TRACE_DOP_FIELD_NAME_MAP_INIT
};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Packet IO enumeration set helper function.
 *
 * \param [in] emap Enumbertaion map.
 * \param [in] prefix Debug prefix.
 * \param [in] api Check api or lt.
 * \param [in] val_in Input value.
 * \param [out] val_out Output value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
pktio_enum_set(const ltsw_pktio_enum_map_t *emap, char *prefix,
               bool api, uint32_t val_in, uint32_t *val_out)
{
    int rv = SHR_E_NONE, i = 0;
    bool found = FALSE;

    *val_out = 0;
    while (emap[i].api_enum != PKTIO_INVALID_ENUM) {
        if (api) {
            if (emap[i].api_enum == (int)val_in) {
                *val_out = (uint32_t) emap[i].lt_enum;
                found = TRUE;
                break;
            }
        } else {
            if (emap[i].lt_enum == (int)val_in) {
                *val_out = (uint32_t) emap[i].api_enum;
                found = TRUE;
                break;
            }
        }

        i++;
    }

    /* Unsupported flag. */
    if (!found) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Unsupported %s flag %u\n"), prefix, val_in));

        rv = SHR_E_UNAVAIL;
    }

    return rv;
}

/*!
 * \brief Packet IO enumeration bitmap set helper function.
 *
 * \param [in] emap Enumbertaion map.
 * \param [in] prefix Debug prefix.
 * \param [in] api Check api or lt.
 * \param [in] bits_in input bitmap.
 * \param [out] bits_out output bitmap.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
pktio_enum_bitmap_set(const ltsw_pktio_enum_map_t *emap, char *prefix,
                      bool api, uint32_t bits_in, uint32_t *bits_out)
{
    int rv = SHR_E_NONE, i = 0;
    uint32_t bits_chk = bits_in;

    *bits_out = 0;
    while (emap[i].api_enum != PKTIO_INVALID_ENUM) {
        if (api) {
            if (bits_in & emap[i].api_enum) {
                *bits_out |= (uint32_t)emap[i].lt_enum;
                bits_chk &= (uint32_t)~(emap[i].api_enum);
            }
        } else {
            if (bits_in & emap[i].lt_enum) {
                *bits_out |= (uint32_t)emap[i].api_enum;
                bits_chk &= (uint32_t)~(emap[i].lt_enum);
            }
        }

        i++;
    }

    /* Have unsupported bit. */
    if (bits_chk) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Unsupported %s bitmap %x in %x\n"),
                   prefix, bits_chk, bits_in));

        rv = SHR_E_UNAVAIL;
    }

    return rv;
}

/*!
 * \brief Lowest priority registered handler that discards packet.
 *
 * \param [in] unit StrataSwitch Unit number.
 * \param [in] packet The packet to handle.
 * \param [in] cookie (Not used).
 *
 * \retval BCM_RX_HANDLED Packet handled.
 */
static bcm_pktio_rx_t
pktio_rx_discard_packet(int unit, bcm_pktio_pkt_t *packet, void *cookie)
{
    COMPILER_REFERENCE(packet);
    COMPILER_REFERENCE(cookie);

    LOG_VERBOSE(BSL_LOG_MODULE,
             (BSL_META_U(unit, "PKTIO RX: discard packet: %d\n"), unit));

    return (BCM_PKTIO_RX_HANDLED);
}

/*!
 * \brief Setup RX discard handler.
 *
 * \param [in] unit StrataSwitch Unit number.
 * \param [in] pi Pointer of pktio info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_pktio_discard_handler_setup(int unit, ltsw_pktio_info_t *pi)
{
    pktio_rx_callout_t *rco = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(rco, sizeof(pktio_rx_callout_t), "bcmPktioRXCallout");
    SHR_NULL_CHECK(rco, SHR_E_MEMORY);

    SETUP_RCO(rco, "Discard", pktio_rx_discard_packet, 0, NULL, NULL, 0);

    pi->rco = rco;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SDKLT RX handler.
 *
 * \param [in] dunit BCMDRD Unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] packet Pointer of SDKLT RX packet structure.
 * \param [in] cookie (Not used).
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
pktio_rx_handler(int dunit, int netif_id, bcmpkt_packet_t *packet, void *cookie)
{
    int unit = bcmi_ltsw_dev_unit(dunit);
    ltsw_pktio_info_t *pi = NULL;
    pktio_rx_callout_t *rco = NULL;
    bcm_pktio_rx_t handler_rv;
    int handled = FALSE;

    SHR_FUNC_ENTER(unit);

    if (unit < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pi = pktio_info[unit];

    /* BCMPKT API only supports one callback. */
    for (rco = pi->rco; rco; rco = rco->rco_next) {

        handler_rv = rco->rco_function(unit, (bcm_pktio_pkt_t *)packet,
                                       rco->rco_cookie);

        switch(handler_rv) {
            case BCM_PKTIO_RX_NOT_HANDLED:
                break;
            case BCM_PKTIO_RX_HANDLED:
                handled = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "PKTIO RX: packet handled by %s\n"),
                             rco->rco_name));
                rco->rco_pkts_handled++;
                break;
            default:
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "ltsw_rx_handler: unit=%d: "
                                     "Invalid callback return value=%d\n"),
                          unit, handler_rv));
                break;
        }

        if (handled) {
            break;
        }
    }

    if (!handled) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "No handler processed packet: ")));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pktio_netif_to_lt_netif(bcm_pktio_netif_t *netif, bcmpkt_netif_t *dst)
{
    int rv = SHR_E_NONE;

    sal_memset(dst, 0, sizeof(bcmpkt_netif_t));

    dst->id = netif->id;
    rv = pktio_enum_bitmap_set(netif_flags, "netif flags", TRUE,
                               netif->flags, &dst->flags);
    sal_memcpy(dst->mac_addr, netif->mac_addr, sizeof(shr_mac_t));
    dst->vlan = netif->vlan;
    dst->port = netif->port;
    sal_memcpy(dst->port_encap, netif->port_encap, 10);
    dst->port_queue = netif->port_queue;
    dst->dma_chan_id = netif->dma_chan_id;
    dst->max_frame_size = netif->max_frame_size;
    sal_memcpy(dst->name, netif->name, BCMPKT_NETIF_NAME_MAX);
    sal_memcpy(dst->user_data, netif->user_data, BCMPKT_NETIF_USER_DATA_SIZE);

    return rv;
}

static int
ltsw_pktio_lt_netif_to_netif(bcmpkt_netif_t *netif, bcm_pktio_netif_t *dst)
{
    int rv = SHR_E_NONE;

    sal_memset(dst, 0, sizeof(bcm_pktio_netif_t));

    dst->id = netif->id;
    rv = pktio_enum_bitmap_set(netif_flags, "netif flags", FALSE,
                               netif->flags, &dst->flags);
    sal_memcpy(dst->mac_addr, netif->mac_addr, sizeof(shr_mac_t));
    dst->vlan = netif->vlan;
    dst->port = netif->port;
    sal_memcpy(dst->port_encap, netif->port_encap, 10);
    dst->port_queue = netif->port_queue;
    dst->dma_chan_id = netif->dma_chan_id;
    dst->max_frame_size = netif->max_frame_size;
    sal_memcpy(dst->name, netif->name, BCMPKT_NETIF_NAME_MAX);
    sal_memcpy(dst->user_data, netif->user_data, BCMPKT_NETIF_USER_DATA_SIZE);

     return rv;
}

static int
ltsw_pktio_netif_traverse_cb(int dunit,
                             const bcmpkt_netif_t *nif,
                             void *cb_data)
{
    int unit = bcmi_ltsw_dev_unit(dunit);
    pktio_cb_data_t *cbd = NULL;
    bcm_pktio_netif_traverse_cb_f cb_func = NULL;
    bcm_pktio_netif_t netif = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(nif, SHR_E_PARAM);
    SHR_NULL_CHECK(cb_data, SHR_E_PARAM);

    cbd = (pktio_cb_data_t *)cb_data;
    cb_func = (bcm_pktio_netif_traverse_cb_f)cbd->cb_func;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_lt_netif_to_netif((bcmpkt_netif_t *)nif, &netif));

    SHR_IF_ERR_VERBOSE_EXIT
        (cb_func(unit, &netif, cbd->cb_data));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pktio_filter_to_lt_filter(int unit,
                               bcm_pktio_filter_t *filter,
                               bcmpkt_filter_t *dst)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int reason;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    dst->id = filter->id;

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_set(filter_type, "filter type", TRUE,
                        filter->type, &dst->type));

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_bitmap_set(filter_flags, "filter flags", TRUE,
                               filter->flags, &dst->flags));

    dst->priority = filter->priority;

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_set(filter_dest_type, "filter dest type", TRUE,
                        filter->dest_type, &dst->dest_type));

    dst->dest_id = filter->dest_id;
    dst->dest_proto = filter->dest_proto;

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_set(filter_dest_type, "filter mirror type", TRUE,
                        filter->mirror_type, &dst->mirror_type));

    dst->mirror_id = filter->mirror_id;
    dst->mirror_proto = filter->mirror_proto;
    dst->dma_chan = filter->dma_chan;

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_bitmap_set(filter_match_flags, "filter match flags", TRUE,
                               filter->match_flags, &dst->match_flags));

    sal_memcpy(dst->desc, filter->desc, BCMPKT_FILTER_DESC_MAX);
    dst->m_vlan = filter->m_vlan;
    dst->m_ingport = filter->m_ingport;
    dst->m_src_modport = filter->m_src_modport;
    dst->m_src_modid = filter->m_src_modid;
    dst->m_cpu_queue = filter->m_cpu_queue;
    if (pi->flags & PKTIO_F_RXFLEX) {
        /* Convert HSDK reason to SDKLT reason. */
        SHR_BIT_ITER(filter->m_flex_reason.pbits,
                     BCM_PKTIO_REASON_COUNTS,
                     reason) {
            hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
            hdr_info.fid = reason;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_lookup(unit, BCMINT_PKTIO_OP_API2LT,
                                                 &hdr_info, &fid_info));

            SHR_BITSET(dst->m_flex_reason.pbits, fid_info->lt_fid);
        }
    } else {
        SHR_BITCOPY_RANGE(dst->m_reason.pbits, 0, filter->m_reason.pbits, 0,
                          BCMPKT_RX_REASON_COUNT);
    }
    dst->m_fp_rule = filter->m_fp_rule;
    dst->raw_size = filter->raw_size;
    sal_memcpy(dst->m_raw_data, filter->m_raw_data, BCMPKT_FILTER_SIZE_MAX);
    sal_memcpy(dst->m_raw_mask, filter->m_raw_mask, BCMPKT_FILTER_SIZE_MAX);
    sal_memcpy(dst->user_data, filter->user_data, BCMPKT_FILTER_USER_DATA_SIZE);

    SHR_NULL_CHECK(filter->m_pmd_data.rxpmd, SHR_E_PARAM);
    sal_memcpy(dst->m_raw_metadata, filter->m_pmd_data.rxpmd,
               BCMPKT_RXPMD_SIZE_BYTES);

    SHR_NULL_CHECK(filter->m_pmd_mask.rxpmd, SHR_E_PARAM);
    sal_memcpy(dst->m_raw_metadata_mask, filter->m_pmd_mask.rxpmd,
               BCMPKT_RXPMD_SIZE_BYTES);

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pktio_lt_filter_to_filter(int unit,
                               bcmpkt_filter_t *filter,
                               bcm_pktio_filter_t *dst)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int reason;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    dst->id = filter->id;
    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_set(filter_type, "filter type", FALSE,
                        filter->type, &dst->type));

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_bitmap_set(filter_flags, "filter flags", FALSE,
                               filter->flags, &dst->flags));

    dst->priority = filter->priority;

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_set(filter_dest_type, "filter dest type", FALSE,
                        filter->dest_type, &dst->dest_type));

    dst->dest_id = filter->dest_id;
    dst->dest_proto = filter->dest_proto;

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_set(filter_dest_type, "filter mirror type", FALSE,
                        filter->mirror_type, &dst->mirror_type));

    dst->mirror_id = filter->mirror_id;
    dst->mirror_proto = filter->mirror_proto;
    dst->dma_chan = filter->dma_chan;

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_enum_bitmap_set(filter_match_flags, "filter match flags", FALSE,
                               filter->match_flags, &dst->match_flags));

    sal_memcpy(dst->desc, filter->desc, BCMPKT_FILTER_DESC_MAX);
    dst->m_vlan = filter->m_vlan;
    dst->m_ingport = filter->m_ingport;
    dst->m_src_modport = filter->m_src_modport;
    dst->m_src_modid = filter->m_src_modid;
    dst->m_cpu_queue = filter->m_cpu_queue;
    if (pi->flags & PKTIO_F_RXFLEX) {
        /* Convert SDKLT reason to HSDK reason. */
        SHR_BIT_ITER(filter->m_flex_reason.pbits,
                     BCMPKT_RXPMD_FLEX_REASON_COUNT,
                     reason) {
            hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
            hdr_info.fid = reason;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_lookup(unit, BCMINT_PKTIO_OP_LT2API,
                                                 &hdr_info, &fid_info));

            SHR_BITSET(dst->m_flex_reason.pbits, fid_info->fid);
        }
    } else {
        SHR_BITCOPY_RANGE(dst->m_reason.pbits, 0, filter->m_reason.pbits, 0,
                          BCMPKT_RX_REASON_COUNT);
    }
    dst->m_fp_rule = filter->m_fp_rule;
    dst->raw_size = filter->raw_size;
    sal_memcpy(dst->m_raw_data, filter->m_raw_data, BCMPKT_FILTER_SIZE_MAX);
    sal_memcpy(dst->m_raw_mask, filter->m_raw_mask, BCMPKT_FILTER_SIZE_MAX);
    sal_memcpy(dst->user_data, filter->user_data, BCMPKT_FILTER_USER_DATA_SIZE);

    SHR_NULL_CHECK(dst->m_pmd_data.rxpmd, SHR_E_PARAM);
    sal_memcpy(dst->m_pmd_data.rxpmd, filter->m_raw_metadata,
               BCMPKT_RXPMD_SIZE_BYTES);

    SHR_NULL_CHECK(dst->m_pmd_mask.rxpmd, SHR_E_PARAM);
    sal_memcpy(dst->m_pmd_mask.rxpmd, filter->m_raw_metadata_mask,
               BCMPKT_RXPMD_SIZE_BYTES);

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pktio_filter_traverse_cb(int dunit,
                              const bcmpkt_filter_t *flt,
                              void *cb_data)
{
    int unit = bcmi_ltsw_dev_unit(dunit);
    pktio_cb_data_t *cbd = NULL;
    bcm_pktio_filter_traverse_cb_f cb_func = NULL;
    bcm_pktio_filter_t filter = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(flt, SHR_E_PARAM);
    SHR_NULL_CHECK(cb_data, SHR_E_PARAM);

    cbd = (pktio_cb_data_t *)cb_data;
    cb_func = (bcm_pktio_filter_traverse_cb_f)(cbd->cb_func);

    bcm_pktio_filter_t_init(&filter);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_lt_filter_to_filter(unit, (bcmpkt_filter_t*)flt, &filter));

    SHR_IF_ERR_VERBOSE_EXIT
        (cb_func(unit, &filter, cbd->cb_data));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pktio_init(int unit)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int warm = bcmi_warmboot_get(unit);
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    void *alloc_ptr = NULL;
    ltsw_pktio_ha_info_t *hi = NULL;
    bcmpkt_txpmd_fid_support_t txpmd_support;
    bcmpkt_rxpmd_fid_support_t rxpmd_support;
    bcmpkt_rxpmd_flex_fid_support_t flex_support;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmpkt_bpool_status_t bpool_status = {0};
    /* */
    bcmpkt_dev_init_t cfg = {
        .cgrp_bmp = 0x1,
        .cgrp_size = 4,
        .mac_addr = LTSW_DEF_NETIF_MAC,
    };
    int rv = SHR_E_NONE, i, drv_type;
    bool initialized = TRUE, is_supported;
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    assert(sizeof(bcm_pktio_pkt_t) == sizeof(bcmpkt_packet_t));

    if (pi != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(pi, sizeof(ltsw_pktio_info_t), "ltswPktioInfo");
        SHR_NULL_CHECK(pi, SHR_E_MEMORY);
        sal_memset(pi, 0, sizeof(ltsw_pktio_info_t));
        pktio_info[unit] = pi;
    }

    /* Create mutex. */
    pi->mutex = sal_mutex_create("ltswPktioInfo");
    SHR_NULL_CHECK(pi->mutex, SHR_E_MEMORY);

    ha_req_size = sizeof(ltsw_pktio_ha_info_t);
    ha_alloc_size = ha_req_size;
    alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_PKTIO, 0,
                                       "bcmPktIOInfo",
                                       &ha_alloc_size);
    SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        sal_memset(alloc_ptr, 0, ha_alloc_size);
    }

    hi = (ltsw_pktio_ha_info_t *)alloc_ptr;

    /* Attach packet IO driver. */
    if (bcmpkt_dev_drv_attached(dunit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_dev_drv_detach(dunit));
    }

    if (!warm) {
        drv_type = bcmi_ltsw_property_get(unit, BCMI_CPN_PKTIO_MODE, 0);

        switch (drv_type) {
            case PKTIO_MODE_UNET:
                hi->drv_type = BCMPKT_DEV_DRV_T_UNET;
                break;

            case PKTIO_MODE_KNET:
                hi->drv_type = BCMPKT_DEV_DRV_T_KNET;
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_drv_attach(dunit, hi->drv_type));

    variant = bcmlrd_variant_get(dunit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNIT);
    }
    pi->variant = variant;

    /* Initialize chip pmd information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_type_get(dunit, &(pi->dev_type)));

    /* TXPMD */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_fid_support_get(pi->dev_type, &txpmd_support));
    SHR_BITCOPY_RANGE(pi->support.txpmd_support.pbits, 0, txpmd_support.fbits,
                      0, BCMPKT_TXPMD_FID_COUNT);

    /* RXPMD */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_rxpmd_fid_support_get(pi->dev_type, &rxpmd_support));
    SHR_BITCOPY_RANGE(pi->support.rxpmd_support.pbits, 0, rxpmd_support.fbits,
                      0, BCMPKT_RXPMD_FID_COUNT);

    /* RXPMD FLEX */
    rv = bcmpkt_rxpmd_flex_fid_support_get(variant, &flex_support);
    if (SHR_SUCCESS(rv)) {
        hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_pktio_fid_support_get(unit, &hdr_info,
                                             &(pi->support.rxflex_support)));

        /* Set RXPMD FLEX into RXPMD. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_pktio_fid_support_get(unit, &hdr_info,
                                             &(pi->support.rxpmd_support)));

        pi->flags |= PKTIO_F_RXFLEX;
    } else if (rv != SHR_E_UNAVAIL) {
        SHR_ERR_EXIT(rv);
    }

    /* GENERIC LOOPBACK */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_generic_loopback_t_is_supported(variant, &is_supported));
    if (is_supported) {
        hdr_info.pmd_type = bcmPktioPmdTypeGih;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_pktio_fid_support_get(unit, &hdr_info,
                                             &(pi->support.gih_support)));

        pi->flags |= PKTIO_F_GIH;
    }

    /* HG3 */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_hg3_base_t_is_supported(variant, &is_supported));

    if (is_supported) {
        hdr_info.pmd_type = bcmPktioPmdTypeHigig3;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_pktio_fid_support_get(unit, &hdr_info,
                                             &(pi->support.higig3_support)));

        pi->flags |= PKTIO_F_HIGIG3;
    }

    /* Create shared buffer pool if needed. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));
    if (!bpool_status.active) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_bpool_create(BCMPKT_BPOOL_SHARED_ID,
                                 MAX_FRAME_SIZE_DEF, PKTIO_BCOUNT_SHARED));
    }

    /* Create per unit dma buffer pool for UNET. */
    if (hi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        if (SAL_BOOT_BCMSIM) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_bpool_create(dunit,
                                     MAX_FRAME_SIZE_DEF, PKTIO_BCOUNT_SIM));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_bpool_create(dunit,
                                     MAX_FRAME_SIZE_DEF, PKTIO_BCOUNT_DEF));
        }
    }

    /* WAR
     * HSDK exit does not cleanup packet device.
     * Thus configuration is left in KNET mode, need sequence below to do cleanup.
     */
    if (!warm) {
        (void)bcmpkt_dev_cleanup(dunit);
        (void)bcmpkt_dev_init(dunit, &cfg);
        (void)bcmpkt_dev_cleanup(dunit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_init(dunit, &cfg));

    if (hi->drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* KNET initialization. */
        if (warm) {
            bcmpkt_netif_t netif = {0};

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_netif_get(dunit, hi->netif_id, &netif));

            /* Bringup network device. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_dev_enable(dunit));
        } else {
            bcmpkt_dma_chan_t chan = {
                .ring_size = (SAL_BOOT_BCMSIM ? PKTIO_RSIZE_SIM :
                                                PKTIO_RSIZE_DEF),
                .max_frame_size = MAX_FRAME_SIZE_DEF,
            };

            bcmpkt_netif_t netif = {
                .mac_addr = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
                .max_frame_size = MAX_FRAME_SIZE_DEF,
                .flags = BCMPKT_NETIF_F_RCPU_ENCAP,
            };

            bcmpkt_filter_t filter = {
                .type = BCMPKT_FILTER_T_RX_PKT,
                .priority = 255,
                .dma_chan = 1,
                .dest_type = BCMPKT_DEST_T_NETIF,
                .dest_id = 1,
                .match_flags = 0,
            };

            /* Configure TX channel. */
            chan.id = 0;
            chan.dir = BCMPKT_DMA_CH_DIR_TX;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_dma_chan_set(dunit, &chan));

            /* Configure RX channels. */
            for (i = 1; i < cfg.cgrp_size; i++) {
                chan.id = i;
                chan.dir = BCMPKT_DMA_CH_DIR_RX;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_dma_chan_set(dunit, &chan));
            }

            /* Bringup network device. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_dev_enable(dunit));

            /* Create network interface. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_netif_create(dunit, &netif));

            /* Create filter to forward all packet to the netif. */
            filter.dest_id = netif.id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_filter_create(unit, &filter));

            hi->netif_id = netif.id;
        }

        /* Setup socket for API. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_socket_create(dunit, hi->netif_id, NULL));

    } else if (hi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        /* UNET initialization. */
        bcmpkt_dma_chan_t chan = {
            .ring_size = (SAL_BOOT_BCMSIM ? PKTIO_RSIZE_SIM :
                                            PKTIO_RSIZE_DEF),
            .max_frame_size = MAX_FRAME_SIZE_DEF,
        };

        /* Configure TX channel. */
        chan.id = 0;
        chan.dir = BCMPKT_DMA_CH_DIR_TX;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_dma_chan_set(dunit, &chan));

        /* Configure RX channels. */
        for (i = 1; i < cfg.cgrp_size; i++) {
            chan.id = i;
            chan.dir = BCMPKT_DMA_CH_DIR_RX;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_dma_chan_set(dunit, &chan));
        }

        /* Bringup network device. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_dev_enable(dunit));

        /* Netif id for UNET is (Tx chan id + 1). */
        hi->netif_id = 1;
    }

    /* Register RX dispatch callback. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_rx_register(dunit, hi->netif_id, 0, pktio_rx_handler, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_discard_handler_setup(unit, pi));

    pi->def_netif = hi->netif_id;
    pi->drv_type = hi->drv_type;

exit:
    if (SHR_FUNC_ERR()) {
        if (alloc_ptr && !warm) {
            (void)bcmi_ltsw_ha_mem_free(unit, alloc_ptr);
        }

        (void)bcmpkt_dev_initialized(dunit, &initialized);

        if (initialized) {
            (void)bcmpkt_dev_cleanup(dunit);
        }
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_pktio_cleanup(int unit)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int i, netif_id;
    bool initialized = FALSE;
    bcmpkt_bpool_status_t bpool_status = {0};
    pktio_rx_callout_t *rco = NULL, *next_rco = NULL;

    SHR_FUNC_ENTER(unit);

    if (pi == NULL) {
        SHR_EXIT();
    }

    netif_id = pi->def_netif;

    /* Attach packet IO driver again. */
    if (bcmpkt_dev_drv_attached(dunit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_dev_drv_detach(dunit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_drv_attach(dunit, pi->drv_type));

    if (pi->drv_type == BCMPKT_DEV_DRV_T_KNET) {
        (void)bcmpkt_socket_created(dunit, netif_id, &initialized);
        if (initialized) {
            (void)bcmpkt_rx_unregister(dunit, netif_id, pktio_rx_handler, NULL);
            (void)bcmpkt_socket_destroy(dunit, netif_id);
        }
    } else if (pi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        /* It should be reset in bcmpkt_dev_cleanup. */
        (void)bcmpkt_rx_unregister(dunit, netif_id, pktio_rx_handler, NULL);

        (void)bcmpkt_dev_initialized(dunit, &initialized);
        if (initialized) {
            (void)bcmpkt_dev_cleanup(dunit);
        }

        (void)bcmpkt_bpool_status_get(dunit, &bpool_status);
        if (bpool_status.active) {
            (void)bcmpkt_bpool_destroy(dunit);
        }
    }

    /*
     * Check if this is the last unit being detached
     */
    for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
        if (i == unit) {
            continue;
        } else if (pktio_info[i] != NULL) {
            break;
        }
    }
    /*
     * This is the last unit. Cleanup shared buffer pool.
     */
    if (i == BCM_MAX_NUM_UNITS) {
        (void)bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status);
        if (bpool_status.active) {
            (void)bcmpkt_bpool_destroy(BCMPKT_BPOOL_SHARED_ID);
        }
    }

    LTSW_PKTIO_LOCK(unit);

    /* Destroy all Rx callouts. */
    rco = pi->rco;
    while (rco) {
        next_rco = rco->rco_next;
        SHR_FREE(rco);
        rco = next_rco;
    }
    pi->rco = NULL;

    LTSW_PKTIO_UNLOCK(unit);

    if (pi->mutex) {
        sal_mutex_destroy(pi->mutex);
    }

    SHR_FREE(pi);
    pktio_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
ltsw_pktio_match_id_dump(int unit, bcmlrd_variant_t variant,
                         uint32_t *match_id, shr_pb_t *pb)
{
    const bcmlrd_match_id_db_t *info;
    uint32_t id, lsb, msb, match_data;
    int start, right_shift;
    bcmint_pktio_hdr_info_t hdr_info;
    bcmint_pktio_fid_info_t *fid_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (id = BCM_PKTIO_MATCH_ID_NONE; id < BCM_PKTIO_MATCH_ID_COUNT; id++) {
        hdr_info.pmd_type = bcmintPktioPmdTypeMatchId;
        hdr_info.fid = id;

        rv = mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info);
        if (rv == SHR_E_UNAVAIL) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_match_id_data_get(variant, fid_info->lt_str,
                                            &info));

        start = info->match_minbit / 32;
        right_shift = info->match_minbit % 32;
        lsb = match_id[start] >> right_shift;
        /* msb part */
        if (start == 1) {
            msb = 0;
        } else {
            msb = match_id[start + 1] & ((1 << right_shift) - 1);
            msb = match_id[start + 1] << (32 - right_shift);
        }
        match_data = msb | lsb;
        match_data &= info->match_mask;
        if (match_data == info->match) {
            shr_pb_printf(pb, "\t%s\n", fid_info->lt_str);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

int
bcm_ltsw_pktio_alloc(int unit, uint32_t size, uint32_t flags,
                     bcm_pktio_pkt_t **packet)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    if (pi->drv_type == BCMPKT_DEV_DRV_T_KNET) {
        dunit = BCMPKT_BPOOL_SHARED_ID;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_alloc(dunit, (size < 64 ? 64 : size), flags,
                      (bcmpkt_packet_t **)packet));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_free(int unit, bcm_pktio_pkt_t *packet)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    if (pi->drv_type == BCMPKT_DEV_DRV_T_KNET) {
        dunit = BCMPKT_BPOOL_SHARED_ID;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_free(dunit, (bcmpkt_packet_t *)packet));

    packet = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_claim(int unit, bcm_pktio_pkt_t *pkt, bcm_pktio_pkt_t **new_pkt)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(new_pkt, SHR_E_PARAM);

    if (pi->drv_type == BCMPKT_DEV_DRV_T_KNET) {
        dunit = BCMPKT_BPOOL_SHARED_ID;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_packet_claim(dunit, (bcmpkt_packet_t *)pkt,
                             (bcmpkt_packet_t **)new_pkt));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_pkt_data_get(int unit, bcm_pktio_pkt_t *packet, void **data,
                            uint32_t *len)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    if (data) {
        *data = BCMPKT_PACKET_DATA((bcmpkt_packet_t *)packet);
    }

    if (len) {
        *len = BCMPKT_PACKET_LEN((bcmpkt_packet_t *)packet);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_reserve(int unit, bcm_pktio_pkt_t *packet, uint32_t len,
                       void **data)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    *data = (void *)bcmpkt_reserve(((bcmpkt_packet_t *)packet)->data_buf, len);

    if (*data == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_push(int unit, bcm_pktio_pkt_t *packet, uint32_t len,
                    void **data)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    *data = (void *)bcmpkt_push(((bcmpkt_packet_t *)packet)->data_buf, len);

    if (*data == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_put(int unit, bcm_pktio_pkt_t *packet, uint32_t len,
                   void **data)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    *data = (void *)bcmpkt_put(((bcmpkt_packet_t *)packet)->data_buf, len);

    if (*data == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_pull(int unit, bcm_pktio_pkt_t *packet, uint32_t len,
                    void **data)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    *data = (void *)bcmpkt_pull(((bcmpkt_packet_t *)packet)->data_buf, len);

    if (*data == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_trim(int unit, bcm_pktio_pkt_t *packet, uint32_t len)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trim(((bcmpkt_packet_t *)packet)->data_buf, len));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_pmd_fid_support_get(int unit, bcm_pktio_fid_support_t *support)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(support, SHR_E_PARAM);

    sal_memcpy(support, &(pi->support), sizeof(bcm_pktio_fid_support_t));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_pktio_raw_pmd_fid_support_get(int unit, bcm_pktio_pmd_t *pmd,
                                       bcm_pktio_fid_support_t *support)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    uint32_t *rxpmd = NULL, flex_profile = 0;
    bcmint_pktio_hdr_info_t hdr_info = {0};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(pmd, SHR_E_PARAM);
    SHR_NULL_CHECK(support, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(pmd);

    sal_memcpy(support, &(pi->support), sizeof(bcm_pktio_fid_support_t));

    /* Get per pmd support list. */
    if (pi->flags |= PKTIO_F_RXFLEX) {
        rxpmd = pmd->rxpmd;

        sal_memset(&(support->rxpmd_support), 0, sizeof(bcm_pktio_bitmap_t));
        sal_memset(&(support->rxflex_support), 0, sizeof(bcm_pktio_bitmap_t));

        SHR_BITCOPY_RANGE(support->rxpmd_support.pbits, 0,
                          pi->support.rxpmd_support.pbits, 0, BCMPKT_RXPMD_FID_COUNT);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd,
                                    BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE,
                                    &flex_profile));

        hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
        hdr_info.profile_chk = TRUE;
        hdr_info.profile = flex_profile;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_pktio_fid_support_get(unit, &hdr_info,
                                             &(support->rxflex_support)));

        /* Set RXPMD FLEX into RXPMD. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_pktio_fid_support_get(unit, &hdr_info,
                                             &(support->rxpmd_support)));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_pmd_field_set(int unit, bcm_pktio_pkt_t *packet,
                             bcm_pktio_pmd_type_t type,
                             int fid, uint32_t val)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    uint32_t *txpmd = NULL, *lbhdr = NULL, *hghdr = NULL;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(&(packet->pmd));

    switch (type) {
        case bcmPktioPmdTypeTx:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_get((bcmpkt_packet_t *)packet, &txpmd));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_field_set(pi->dev_type, txpmd, fid, val));

            break;
        case bcmPktioPmdTypeGih:
            if (!(pi->flags & PKTIO_F_GIH)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            hdr_info.pmd_type = type;
            hdr_info.fid = fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

            fid = fid_info->lt_fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_lbhdr_get((bcmpkt_packet_t *)packet, &lbhdr));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_generic_loopback_t_field_set(pi->variant, lbhdr,
                                                     fid, val));

            break;
        case bcmPktioPmdTypeHigig3:
            if (!(pi->flags & PKTIO_F_HIGIG3)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            hdr_info.pmd_type = type;
            hdr_info.fid = fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_higig_get((bcmpkt_packet_t *)packet, &hghdr));

            if (fid >= BCM_PKTIO_HG3_EXT0_FID_OFFSET) {
                hghdr += BCMPKT_HG3_BASE_HEADER_SIZE_WORDS;
                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_hg3_extension_0_t_field_set(pi->variant, hghdr,
                                                        fid, val));
            } else {
                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_hg3_base_t_field_set(pi->variant, hghdr,
                                                 fid, val));
            }

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_pmd_field_get(int unit, bcm_pktio_pkt_t *packet,
                             bcm_pktio_pmd_type_t type,
                             int fid, uint32_t *val)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    uint32_t *txpmd = NULL, *lbhdr = NULL, *hghdr = NULL;
    uint32_t *rxpmd = NULL, *flexpmd = NULL, flex_profile = 0, len = 0;
    bool rx_dump = FALSE;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(&(packet->pmd));

    switch (type) {
        case bcmPktioPmdTypeTx:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_get((bcmpkt_packet_t *)packet, &txpmd));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_field_get(pi->dev_type, txpmd, fid, val));

            break;
        case bcmPktioPmdTypeGih:
            if (!(pi->flags & PKTIO_F_GIH)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            hdr_info.pmd_type = type;
            hdr_info.fid = fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

            fid = fid_info->lt_fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_lbhdr_get((bcmpkt_packet_t *)packet, &lbhdr));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_generic_loopback_t_field_get(pi->variant, lbhdr,
                                                     fid, val));

            break;
        case bcmPktioPmdTypeHigig3:
            if (!(pi->flags & PKTIO_F_HIGIG3)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            hdr_info.pmd_type = type;
            hdr_info.fid = fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_higig_get((bcmpkt_packet_t *)packet, &hghdr));

            if (fid >= BCM_PKTIO_HG3_EXT0_FID_OFFSET) {
                hghdr += BCMPKT_HG3_BASE_HEADER_SIZE_WORDS;
                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_hg3_extension_0_t_field_get(pi->variant, hghdr,
                                                        fid, val));
            } else {
                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_hg3_base_t_field_get(pi->variant, hghdr, fid, val));
            }

            break;
        case bcmPktioPmdTypeRx:
        case bcmPktioPmdTypeRxFlex:
            if (fid >= BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) {

                if (!(pi->flags & PKTIO_F_RXFLEX)) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }

                hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
                hdr_info.fid = fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_get((bcmpkt_packet_t *)packet, &rxpmd));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd,
                                            BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE,
                                            &flex_profile));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_flexdata_get(pi->dev_type, rxpmd, &flexpmd, &len));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_flex_field_get(pi->variant, flexpmd,
                                                 flex_profile, fid, val));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_get((bcmpkt_packet_t *)packet, &rxpmd));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd, fid, val));
            }

            rx_dump = TRUE;

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE) && rx_dump) {
        (void)bcmi_ltsw_pktio_rx_dump(unit, packet, 1/* NONE_ZERO */);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_raw_pmd_field_set(int unit, bcm_pktio_pmd_t *pmd,
                                 bcm_pktio_pmd_type_t type,
                                 int fid, uint32_t val)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    uint32_t *txpmd = NULL, *lbhdr = NULL, *hghdr = NULL;
    uint32_t *rxpmd = NULL, *flexpmd = NULL, flex_profile = 0, len = 0;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(pmd, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(pmd);

    switch (type) {
        case bcmPktioPmdTypeTx:
            txpmd = pmd->txpmd;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_field_set(pi->dev_type, txpmd, fid, val));

            break;
        case bcmPktioPmdTypeGih:
            if (!(pi->flags & PKTIO_F_GIH)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            hdr_info.pmd_type = type;
            hdr_info.fid = fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

            fid = fid_info->lt_fid;

            lbhdr = pmd->lbhdr;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_generic_loopback_t_field_set(pi->variant, lbhdr,
                                                     fid, val));

            break;
        case bcmPktioPmdTypeHigig3:
            if (!(pi->flags & PKTIO_F_HIGIG3)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            hdr_info.pmd_type = type;
            hdr_info.fid = fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

            hghdr = pmd->higig;

            if (fid >= BCM_PKTIO_HG3_EXT0_FID_OFFSET) {
                hghdr += BCMPKT_HG3_BASE_HEADER_SIZE_WORDS;
                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_hg3_extension_0_t_field_set(pi->variant, hghdr,
                                                        fid, val));
            } else {
                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_hg3_base_t_field_set(pi->variant, hghdr,
                                                 fid, val));
            }

            break;
        case bcmPktioPmdTypeRx:
        case bcmPktioPmdTypeRxFlex:
            if (fid >= BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) {

                if (!(pi->flags & PKTIO_F_RXFLEX)) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }

                hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
                hdr_info.fid = fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

                fid = fid_info->lt_fid;

                rxpmd = pmd->rxpmd;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd,
                                            BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE,
                                            &flex_profile));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_flexdata_get(pi->dev_type, rxpmd, &flexpmd, &len));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_flex_field_set(pi->variant, flexpmd,
                                                 flex_profile, fid, val));
            } else {
                rxpmd = pmd->rxpmd;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_field_set(pi->dev_type, rxpmd, fid, val));
            }

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_raw_pmd_field_get(int unit, bcm_pktio_pmd_t *pmd,
                                 bcm_pktio_pmd_type_t type,
                                 int fid, uint32_t *val)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    uint32_t *txpmd = NULL, *lbhdr = NULL, *hghdr = NULL;
    uint32_t *rxpmd = NULL, *flexpmd = NULL, flex_profile = 0, len = 0;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(pmd, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(pmd);

    switch (type) {
        case bcmPktioPmdTypeTx:
            txpmd = pmd->txpmd;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_field_get(pi->dev_type, txpmd, fid, val));

            break;
        case bcmPktioPmdTypeGih:
            if (!(pi->flags & PKTIO_F_GIH)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            hdr_info.pmd_type = type;
            hdr_info.fid = fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

            fid = fid_info->lt_fid;

            lbhdr = pmd->lbhdr;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_generic_loopback_t_field_get(pi->variant, lbhdr,
                                                     fid, val));

            break;
        case bcmPktioPmdTypeHigig3:
            if (!(pi->flags & PKTIO_F_HIGIG3)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            hdr_info.pmd_type = type;
            hdr_info.fid = fid;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

            hghdr = pmd->higig;

            if (fid >= BCM_PKTIO_HG3_EXT0_FID_OFFSET) {
                hghdr += BCMPKT_HG3_BASE_HEADER_SIZE_WORDS;
                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_hg3_extension_0_t_field_get(pi->variant, hghdr,
                                                        fid, val));
            } else {
                fid = fid_info->lt_fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_hg3_base_t_field_get(pi->variant, hghdr, fid, val));
            }

            break;
        case bcmPktioPmdTypeRx:
        case bcmPktioPmdTypeRxFlex:
            if (fid >= BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) {

                if (!(pi->flags & PKTIO_F_RXFLEX)) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }

                hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
                hdr_info.fid = fid;

                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info));

                fid = fid_info->lt_fid;

                rxpmd = pmd->rxpmd;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd,
                                            BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE,
                                            &flex_profile));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_flexdata_get(pi->dev_type, rxpmd, &flexpmd, &len));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_flex_field_get(pi->variant, flexpmd,
                                                 flex_profile, fid, val));
            } else {
                rxpmd = pmd->rxpmd;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd, fid, val));
            }

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_pmd_reasons_get(int unit, bcm_pktio_pkt_t *packet,
                               bcm_pktio_reasons_t *reasons)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int reason;
    uint32_t *rxpmd = NULL, *flexpmd = NULL, len = 0;
    bcmpkt_rx_reasons_t rx_reasons;
    bcmpkt_rxpmd_flex_reasons_t flex_reasons;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(&(packet->pmd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_rxpmd_get((bcmpkt_packet_t *)packet, &rxpmd));

    if (pi->flags & PKTIO_F_RXFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_flexdata_get(pi->dev_type, rxpmd, &flexpmd, &len));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_flex_reasons_get(pi->variant, flexpmd, &flex_reasons));
        /* Convert SDKLT reason to HSDK reason. */
        SHR_BIT_ITER(flex_reasons.pbits, BCMPKT_RXPMD_FLEX_REASON_COUNT, reason) {
            hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
            hdr_info.fid = reason;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_lookup(unit, BCMINT_PKTIO_OP_LT2API,
                                                 &hdr_info, &fid_info));

            SHR_BITSET(reasons->flex_reasons.pbits, fid_info->fid);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_reasons_get(pi->dev_type, rxpmd, &rx_reasons));

        SHR_BITCOPY_RANGE(reasons->rx_reasons.pbits, 0, rx_reasons.pbits, 0,
                          BCMPKT_RX_REASON_COUNT);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_raw_pmd_reasons_set(int unit, bcm_pktio_pmd_t *pmd,
                                   bcm_pktio_reasons_t *reasons)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int reason;
    uint32_t *rxpmd = NULL, *flexpmd = NULL, len = 0;
    bcmpkt_rx_reasons_t rx_reasons = {{0}};
    bcmpkt_rxpmd_flex_reasons_t flex_reasons = {{0}};
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(pmd, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(pmd);
    rxpmd = pmd->rxpmd;

    if (pi->flags & PKTIO_F_RXFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_flexdata_get(pi->dev_type, rxpmd, &flexpmd, &len));

        /* Convert HSDK reason to SDKLT reason. */
        SHR_BIT_ITER(reasons->flex_reasons.pbits,
                     BCM_PKTIO_REASON_COUNTS,
                     reason) {
            hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
            hdr_info.fid = reason;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_lookup(unit, BCMINT_PKTIO_OP_API2LT,
                                                 &hdr_info, &fid_info));

            SHR_BITSET(flex_reasons.pbits, fid_info->lt_fid);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_flex_reasons_set(pi->variant, &flex_reasons, flexpmd));
    } else {
        SHR_BITCOPY_RANGE(rx_reasons.pbits, 0, reasons->rx_reasons.pbits, 0,
                          BCMPKT_RX_REASON_COUNT);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_reasons_set(pi->dev_type, &rx_reasons, rxpmd));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_raw_pmd_reasons_get(int unit, bcm_pktio_pmd_t *pmd,
                                   bcm_pktio_reasons_t *reasons)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int reason;
    uint32_t *rxpmd = NULL, *flexpmd = NULL, len = 0;
    bcmpkt_rx_reasons_t rx_reasons;
    bcmpkt_rxpmd_flex_reasons_t flex_reasons;
    bcmint_pktio_hdr_info_t hdr_info = {0};
    bcmint_pktio_fid_info_t *fid_info;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(pmd, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(pmd);
    rxpmd = pmd->rxpmd;

    if (pi->flags & PKTIO_F_RXFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_flexdata_get(pi->dev_type, rxpmd, &flexpmd, &len));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_flex_reasons_get(pi->variant, flexpmd, &flex_reasons));
        /* Convert SDKLT reason to HSDK reason. */
        SHR_BIT_ITER(flex_reasons.pbits, BCMPKT_RXPMD_FLEX_REASON_COUNT, reason) {
            hdr_info.pmd_type = bcmPktioPmdTypeRxFlex;
            hdr_info.fid = reason;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_pktio_fid_info_lookup(unit, BCMINT_PKTIO_OP_LT2API,
                                                 &hdr_info, &fid_info));

            SHR_BITSET(reasons->flex_reasons.pbits, fid_info->fid);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_reasons_get(pi->dev_type, rxpmd, &rx_reasons));

        SHR_BITCOPY_RANGE(reasons->rx_reasons.pbits, 0, rx_reasons.pbits, 0,
                          BCMPKT_RX_REASON_COUNT);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_pmd_pkt_format_get(int unit, int array_size, uint32_t *match_id_array,
                                  int *count, bcm_pktio_bitmap_t *pkt_format)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    const bcmlrd_match_id_db_t *info;
    uint32_t id, lsb, msb, match_data;
    int start, right_shift;
    bcmint_pktio_hdr_info_t hdr_info;
    bcmint_pktio_fid_info_t *fid_info;
    int rv = SHR_E_NONE;


    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(count, SHR_E_PARAM);

    if (array_size == 0) {
        /* Look up required array size. */
        *count = PKTIO_MATCH_ID_SIZE;
        SHR_EXIT();
    } else if (array_size != PKTIO_MATCH_ID_SIZE) {
        *count = PKTIO_MATCH_ID_SIZE;
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else {
        *count = PKTIO_MATCH_ID_SIZE;
    }

    SHR_NULL_CHECK(match_id_array, SHR_E_PARAM);
    SHR_NULL_CHECK(pkt_format, SHR_E_PARAM);

    if (BCM_PKTIO_FID_SUPPORT_GET(pi->support.rxpmd_support, BCM_PKTIO_RXPMD_MATCH_ID_LO) &&
        BCM_PKTIO_FID_SUPPORT_GET(pi->support.rxpmd_support, BCM_PKTIO_RXPMD_MATCH_ID_HI)) {
        if (pi->variant <= BCMLRD_VARIANT_T_NONE ||
            pi->variant >= BCMLRD_VARIANT_T_COUNT) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        for (id = BCM_PKTIO_MATCH_ID_NONE; id < BCM_PKTIO_MATCH_ID_COUNT; id++) {
            hdr_info.pmd_type = bcmintPktioPmdTypeMatchId;
            hdr_info.fid = id;

            rv = mbcm_ltsw_pktio_fid_info_get(unit, &hdr_info, &fid_info);
            if (rv == SHR_E_UNAVAIL) {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_rxpmd_match_id_data_get(pi->variant, fid_info->lt_str,
                                                &info));

            start = info->match_minbit / 32;
            right_shift = info->match_minbit % 32;
            lsb = match_id_array[start] >> right_shift;
            /* msb part */
            if (start == 1) {
                msb = 0;
            } else {
                msb = match_id_array[start + 1] & ((1 << right_shift) - 1);
                msb = match_id_array[start + 1] << (32 - right_shift);
            }
            match_data = msb | lsb;
            match_data &= info->match_mask;
            if (match_data == info->match) {
                SHR_BITSET(pkt_format->pbits, id);
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_pmd_set(int unit, bcm_pktio_pkt_t *packet,
                       bcm_pktio_txpmd_t *pmd)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    uint32_t *txpmd = NULL, val;
    uint32_t dest_op = 0, dest_type = 0;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(pmd, SHR_E_PARAM);

    if (SHR_BITGET(pi->support.txpmd_support.pbits,
                   BCMPKT_TXPMD_DESTINATION_TYPE) &&
        SHR_BITGET(pi->support.txpmd_support.pbits,
                   BCMPKT_TXPMD_DESTINATION)) {
        dest_op = 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_get((bcmpkt_packet_t *)packet, &txpmd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_field_set(pi->dev_type, txpmd, BCMPKT_TXPMD_START, 2));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                BCMPKT_TXPMD_HEADER_TYPE, 1));

    /* Set sobmh uc or mc flow. */
    if (pmd->flags & BCM_PKTIO_TX_MC_QUEUE) {
        if (dest_op) {
            dest_type = BCMPKT_TXPMD_DESTINATION_T_L2MC;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                        BCMPKT_TXPMD_SET_L2BM, 1));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                    BCMPKT_TXPMD_UNICAST, 1));

        if (SHR_BITGET(pi->support.txpmd_support.pbits,
                       BCMPKT_TXPMD_UNICAST_PKT)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                        BCMPKT_TXPMD_UNICAST_PKT, 1));
        }
    }

    val = pmd->prio_int;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                BCMPKT_TXPMD_INPUT_PRI, val));

    val = pmd->cos;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                BCMPKT_TXPMD_COS, val));

    val = pmd->tx_port;
    if (dest_op) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                    BCMPKT_TXPMD_DESTINATION_TYPE, dest_type));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                    BCMPKT_TXPMD_DESTINATION, val));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                    BCMPKT_TXPMD_LOCAL_DEST_PORT, val));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_tx(int unit, bcm_pktio_pkt_t *packet)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_tx(dunit, pi->def_netif, (bcmpkt_packet_t *)packet));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_rx_register(int unit, const char *name,
                           bcm_pktio_rx_cb_f callback, uint8_t priority,
                           void *cookie, uint32_t flags)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    pktio_rx_callout_t *rco = NULL, *list_rco = NULL, *prev_rco = NULL;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(callback, SHR_E_PARAM);

    LTSW_PKTIO_LOCK(unit);

    /* First check if already registered */
    for (list_rco = pi->rco; list_rco; list_rco = list_rco->rco_next) {
        if (list_rco->rco_function == callback &&
            list_rco->rco_priority == priority) {
            if (list_rco->rco_flags == flags &&
                list_rco->rco_cookie == cookie) {
                LTSW_PKTIO_UNLOCK(unit);
                SHR_EXIT();
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "PKTIO RX: %s registered with diff params\n"),
                         name));

            LTSW_PKTIO_UNLOCK(unit);
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_ALLOC(rco, sizeof(pktio_rx_callout_t), "bcmPktioRXCallout");
    SHR_NULL_CHECK(rco, SHR_E_MEMORY);

    SETUP_RCO(rco, name, callback, priority, cookie, NULL, flags);

    /*
     * Find correct place to insert handler, this code assumes that
     * the discard handler has been registered on init.  Handlers
     * of the same priority are placed in the list in the order
     * they are registered
     */
    prev_rco = NULL;
    for (list_rco = pi->rco; list_rco; list_rco = list_rco->rco_next) {
        if (list_rco->rco_priority < priority) {
            break;
        }

        prev_rco = list_rco;
    }

    if (prev_rco) {                     /* Insert after prev_rco */
        rco->rco_next = prev_rco->rco_next;
        prev_rco->rco_next = rco;
    } else {                            /* Insert first */
        rco->rco_next = list_rco;
        pi->rco = rco;
    }

    LTSW_PKTIO_UNLOCK(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "PKTIO RX: %s registered %s%s.\n"),
                 name,
                 prev_rco ? "after " : "first",
                 prev_rco ? prev_rco->rco_name : ""));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_rx_unregister(int unit,
                             bcm_pktio_rx_cb_f callback, uint8_t priority)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    pktio_rx_callout_t *rco = NULL, *prev_rco = NULL;
    const char *name;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    LTSW_PKTIO_LOCK(unit);

    for (rco = pi->rco; rco; rco = rco->rco_next) {
        if (rco->rco_function == callback && rco->rco_priority == priority) {
            break;
        }
        prev_rco = rco;
    }

    if (!rco) {
        LTSW_PKTIO_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    name = rco->rco_name;

    if (!prev_rco) {
        pi->rco = rco->rco_next;
    } else {
        prev_rco->rco_next = rco->rco_next;
    }

    LTSW_PKTIO_UNLOCK(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "PKTIO RX: Unregistered %s on %d\n"), name, unit));

    SHR_FREE(rco);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_netif_create(int unit, bcm_pktio_netif_t *netif)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmpkt_netif_t nif = {0};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(netif, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_netif_to_lt_netif(netif, &nif));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_netif_create(dunit, &nif));

    netif->id = nif.id;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_netif_get(int unit, int netif_id, bcm_pktio_netif_t *netif)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmpkt_netif_t nif = {0};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(netif, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_netif_get(dunit, netif_id, &nif));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_lt_netif_to_netif(&nif, netif));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_netif_destroy(int unit, int netif_id)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_netif_destroy(dunit, netif_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_netif_link_status_set(int unit, int netif_id, int status)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_netif_link_update(dunit, netif_id, (bool)status));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_netif_traverse(int unit, bcm_pktio_netif_traverse_cb_f cb_func,
                              void *cb_data)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmpkt_netif_traverse_cb_f cb = ltsw_pktio_netif_traverse_cb;
    pktio_cb_data_t cbd = {(void *)cb_func, cb_data};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_netif_traverse(dunit, cb, (void *)&cbd));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_filter_create(int unit, bcm_pktio_filter_t *filter)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmpkt_filter_t flt = {0};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    bcm_pktio_pmd_t_init(&(filter->m_pmd_data));
    bcm_pktio_pmd_t_init(&(filter->m_pmd_mask));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_filter_to_lt_filter(unit, filter, &flt));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_filter_create(dunit, &flt));

    filter->id = flt.id;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_filter_get(int unit, int filter_id, bcm_pktio_filter_t *filter)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmpkt_filter_t flt = {0};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    bcm_pktio_filter_t_init(filter);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_filter_get(dunit, filter_id, &flt));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_lt_filter_to_filter(unit, &flt, filter));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_filter_destroy(int unit, int filter_id)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_filter_destroy(dunit, filter_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_filter_traverse(int unit, bcm_pktio_filter_traverse_cb_f cb_func,
                               void *cb_data)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmpkt_filter_traverse_cb_f cb = ltsw_pktio_filter_traverse_cb;
    pktio_cb_data_t cbd = {(void *)cb_func, cb_data};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_filter_traverse(dunit, cb, (void *)&cbd));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_trace_data_collect(int unit, bcm_pktio_trace_pkt_t *trace_pkt,
                                  bcm_pktio_trace_data_t *trace_data)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmpkt_trace_pkt_t tpkt = {0};
    bcmpkt_trace_data_t tdata = {0};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trace_pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(trace_pkt->pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(trace_data, SHR_E_PARAM);

    if (ltsw_feature(unit, LTSW_FT_TRACE_DOP)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    tpkt.options = trace_pkt->flags;
    tpkt.port = trace_pkt->port;
    tpkt.len = trace_pkt->len;
    tpkt.netif_id = pi->def_netif;
    tpkt.pkt = trace_pkt->pkt;

    tdata.buf = trace_data->buf;
    tdata.buf_size = BCM_PKTIO_TRACE_SIZE_BYTES;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_data_collect(dunit, &tpkt, &tdata));

    trace_data->len = tdata.len;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_trace_field_get(int unit, bcm_pktio_trace_data_t *trace_data,
                               int fid, uint32_t *val)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trace_data, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    if (ltsw_feature(unit, LTSW_FT_TRACE_DOP)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_field_get(dunit, trace_data->buf, fid, val));

    if (fid == BCMPKT_TRACE_INGRESS_STG_STATE) {
        switch (*val) {
        case 0: /* Disabled */
            *val = BCM_STG_STP_DISABLE;
            break;
        case 1: /* Blocking/Listening */
            *val = BCM_STG_STP_BLOCK;
            break;
        case 2: /* Learning */
            *val = BCM_STG_STP_LEARN;
            break;
        case 3: /* Forwarding */
            *val = BCM_STG_STP_FORWARD;
            break;
        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_trace_drop_reasons_get(int unit,
                                      bcm_pktio_trace_data_t *trace_data,
                                      bcm_pktio_bitmap_t *bitmap)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trace_data, SHR_E_PARAM);
    SHR_NULL_CHECK(bitmap, SHR_E_PARAM);

    if (ltsw_feature(unit, LTSW_FT_TRACE_DOP)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_drop_reasons_get(dunit, trace_data->buf,
                                       (bcmpkt_trace_drop_reasons_t *)bitmap));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_trace_counter_get(int unit,
                                 bcm_pktio_trace_data_t *trace_data,
                                 bcm_pktio_bitmap_t *bitmap)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trace_data, SHR_E_PARAM);
    SHR_NULL_CHECK(bitmap, SHR_E_PARAM);

    if (ltsw_feature(unit, LTSW_FT_TRACE_DOP)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_counter_get(dunit, trace_data->buf,
                                  (bcmpkt_trace_counters_t *)bitmap));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_trace_tx(int unit, bcm_pktio_trace_pkt_t *trace_pkt)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcm_port_t port;
    bcmpkt_trace_pkt_t tpkt = {0};
    bcmpkt_trace_data_t tdata = {0};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trace_pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(trace_pkt->pkt, SHR_E_PARAM);

    if (!ltsw_feature(unit, LTSW_FT_TRACE_DOP)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, trace_pkt->port, &port));

    tpkt.options = trace_pkt->flags;
    tpkt.port = (uint8_t)port;
    tpkt.len = trace_pkt->len;
    tpkt.netif_id = pi->def_netif;
    tpkt.pkt = trace_pkt->pkt;

    tdata.len = 0;
    tdata.buf = NULL;
    tdata.buf_size = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_data_collect(dunit, &tpkt, &tdata));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_trace_dop_data_collect(int unit, bcm_port_t port, uint32_t dop_id,
                                      bcm_pktio_trace_data_t *trace_data)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmpkt_trace_data_t tdata = {0};

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trace_data, SHR_E_PARAM);

    if (!ltsw_feature(unit, LTSW_FT_TRACE_DOP)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, (bcm_port_t)port,
                                       (bcm_port_t *)&port));

    tdata.buf = trace_data->buf;
    tdata.buf_size = BCM_PKTIO_TRACE_SIZE_BYTES;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_dop_data_collect(dunit, port, dop_id, &tdata));

    trace_data->len = tdata.len;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    pktio_initialized[unit] = FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_pktio_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_pktio_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pktio_init(unit));

    pktio_initialized[unit] = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        bcm_ltsw_pktio_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_pktio_sw_dump(int unit)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);
    shr_pb_t *pb = NULL;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    LOG_CLI((BSL_META_U(unit, "Packet device information:\n")));
    LOG_CLI((BSL_META_U(unit, "  Driver: ")));
    if (pi->drv_type == BCMPKT_DEV_DRV_T_KNET) {
        LOG_CLI((BSL_META_U(unit, "KNET\n")));
    } else if (pi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        LOG_CLI((BSL_META_U(unit, "UNET\n")));
    } else {
        LOG_CLI((BSL_META_U(unit, "Unknown - %d\n"), pi->drv_type));
    }

    pb = shr_pb_create();
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_info_dump(dunit, pb));
    LOG_CLI((BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    pb = NULL;

    LOG_CLI((BSL_META_U(unit, "Packet buffer pool information:\n")));
    pb = shr_pb_create();
    if (pi->drv_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_bpool_info_dump(BCMPKT_BPOOL_SHARED_ID, pb));
    } else if (pi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_bpool_info_dump(dunit, pb));
    }
    LOG_CLI((BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    pb = NULL;

exit:
    if (pb) {
        shr_pb_destroy(pb);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_pktio_rx_dump(int unit, bcm_pktio_pkt_t *packet, int flags)
{
    ltsw_pktio_info_t *pi = pktio_info[unit];
    uint32_t *rxpmd;
    uint32_t *flexdata = NULL, flex_profile = 0;
    uint32_t len = 0;
    uint32_t match_id[2] = {0};
    shr_pb_t *pb = NULL;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmpkt_rxpmd_get((bcmpkt_packet_t *)packet, &rxpmd));

    pb = shr_pb_create();

    shr_pb_printf(pb, "[RX metadata information]\n");
    bcmpkt_rxpmd_dump(pi->dev_type, rxpmd, flags, pb);

    if (pi->flags & PKTIO_F_RXFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd,
                                    BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE,
                                    &flex_profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_flexdata_get(pi->dev_type, rxpmd, &flexdata, &len));

        shr_pb_printf(pb, "[FLEX fields]\n");

        bcmpkt_rxpmd_flex_dump(pi->variant, flexdata, flex_profile,
                               flags, pb);
    }

    shr_pb_printf(pb, "[RX reasons]\n");

    if (pi->flags & PKTIO_F_RXFLEX) {
        bcmpkt_rxpmd_flex_reason_dump(pi->variant, flexdata, pb);
    } else {
        bcmpkt_rx_reason_dump(pi->dev_type, rxpmd, pb);
    }

    if (BCM_PKTIO_FID_SUPPORT_GET(pi->support.rxpmd_support, BCM_PKTIO_RXPMD_MATCH_ID_LO) &&
        BCM_PKTIO_FID_SUPPORT_GET(pi->support.rxpmd_support, BCM_PKTIO_RXPMD_MATCH_ID_HI)) {
        shr_pb_printf(pb, "[Decode match id]\n");
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd,
                                    BCMPKT_RXPMD_MATCH_ID_LO,
                                    &match_id[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd,
                                    BCMPKT_RXPMD_MATCH_ID_HI,
                                    &match_id[1]));

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pktio_match_id_dump(unit, pi->variant, match_id, pb));
    }

    LOG_CLI((BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    pb = NULL;

exit:
    if (pb) {
        shr_pb_destroy(pb);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_pktio_trace_data_dump(int unit, uint32_t dop_id,
                                bcm_pktio_trace_data_t *trace_data)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    uint32_t idx;
    uint32_t *fid_list = NULL;
    uint32_t fid_max_count;
    uint8_t fid_count;
    uint32_t alloc_sz;
    uint32_t max_dop_phase;
    uint32_t *field_data = NULL;
    uint8_t field_data_len;
    int i, j;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trace_data, SHR_E_PARAM);

    if (!ltsw_feature(unit, LTSW_FT_TRACE_DOP)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!trace_data->len) {
        LOG_CLI((BSL_META_U(unit, "DOP_NAME:%s dop_id:%d invalid dop data\n"),
                dop_id_names[dop_id].name, dop_id));

        SHR_EXIT();
    } else {
        LOG_CLI((BSL_META_U(unit, "DOP_NAME:%s dop_id:%d len:%d\n"),
                dop_id_names[dop_id].name, dop_id, trace_data->len));
    }

    for (idx = 0;  idx < trace_data->len; idx++) {
        if ((idx & 0xf) == 0) {
            LOG_CLI((BSL_META_U(unit, "%04x: "), idx));
        }
        if ((idx & 0xf) == 8) {
            LOG_CLI((BSL_META_U(unit, "- ")));
        }
        LOG_CLI((BSL_META_U(unit, "%02x "), trace_data->buf[idx]));

        if ((idx & 0xf) == 0xf) {
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n")));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_max_dop_field_count_get(dunit, &fid_max_count));

    alloc_sz = sizeof(uint32_t) * fid_max_count;
    SHR_ALLOC(fid_list, alloc_sz, "DOPFieldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);

    fid_count = (uint8_t)fid_max_count;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_dop_fids_get(dunit, dop_id, fid_list, &fid_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trace_max_dop_phase_get(dunit, &max_dop_phase));

    field_data_len = sizeof(uint32_t) * max_dop_phase;
    SHR_ALLOC(field_data, field_data_len, "DOPFieldData");
    SHR_NULL_CHECK(field_data, SHR_E_MEMORY);

    for (i = 0; i < fid_count; i++) {
        field_data_len = max_dop_phase * sizeof(uint32_t);
        sal_memset(field_data, 0, field_data_len);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_trace_dop_field_get(unit, trace_data->buf, fid_list[i],
                                        field_data, &field_data_len));
        LOG_CLI((BSL_META_U(unit, "%s: "), dop_field_names[fid_list[i]].name));
        for (j = 0; j < field_data_len; j++) {
            LOG_CLI((BSL_META_U(unit, "0x%x "), field_data[j]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

exit:
    if (fid_list) {
        SHR_FREE(fid_list);
    }

    if (field_data) {
        SHR_FREE(field_data);
    }

    SHR_FUNC_EXIT();
}
