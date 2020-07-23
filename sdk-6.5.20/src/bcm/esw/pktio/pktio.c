/*! \file pktio.c
 *
 * Emulator over SDKLT PacketIO interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_PKTIO)

#include <pktio_dep.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcm/pktio.h>
#include <bcm/pktio_defs.h>

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
    uint32 rco_flags;

    /*! Callout priority 0 == lowest. */
    uint8 rco_priority;

    /*! Counter. */
    uint32 rco_pkts_handled;

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

typedef struct pktio_cb_data_s {
    void *cb_func;
    void *cb_data;
} pktio_cb_data_t;

/*!
 * \brief Packet IO information.
 */
typedef struct pktio_info_s {

    /*! Device driver type. */
    bcmpkt_dev_drv_types_t drv_type;

    /*! Device type. */
    bcmdrd_dev_type_t dev_type;

#define PKTIO_F_GIH     0x1
#define PKTIO_F_HIGIG3  0x2
    /*! */
    uint32 flags;

    /*! Packet metadata support bitmap. */
    bcm_pktio_fid_support_t support;

    /*! Callout linked list. */
    pktio_rx_callout_t *rco;

    /*! Sync for handler list. */
    sal_mutex_t mutex;

    /*! Default network interface in KNET mode. */
    int def_netif;

} pktio_info_t;

/*! Per chip packet IO information. */
static pktio_info_t *pktio_info[BCM_MAX_NUM_UNITS] = {0};

#define PKTIO_LOCK(unit)       \
    sal_mutex_take(pktio_info[unit]->mutex, SAL_MUTEX_FOREVER)

#define PKTIO_UNLOCK(unit)     \
    sal_mutex_give(pktio_info[unit]->mutex)

/*! Per chip packet IO initialize flag. */
static bool pktio_initialized[BCM_MAX_NUM_UNITS] = {0};

/*! Default netif MAC address. */
#define PKTIO_DEF_NETIF_MAC  {0x00, 0xbc, 0x00, 0x00, 0x00, 0x00}

/*! Maximum default packet size. */
#define MAX_FRAME_SIZE_DEF  (9472)

/*! Per device buffer count. */
#define PKTIO_BCOUNT_DEF    (1024)
#define PKTIO_BCOUNT_SIM    (256)

/*! Per channel ring size. */
#define PKTIO_RSIZE_DEF     (64)
#define PKTIO_RSIZE_SIM     (16)

/******************************************************************************
 * Private functions
 */
static int
bcmi_pktio_dev_dunit(int unit)
{
    return unit;
}
static int
bcmi_pktio_dev_unit(int dunit)
{
    return dunit;
}

static int
bcmi_pkt_core_cleanup(void)
{
    return SHR_E_NONE;

}

static int
bcmi_pkt_core_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_unet_attach());

exit:
    SHR_FUNC_EXIT();
}

static int
bcmi_pkt_dev_cleanup(int unit)
{
    int rv;
    int dunit = bcmi_pktio_dev_dunit(unit);
    int actual_rv = SHR_E_NONE;
    bool initialized = false;

    rv = bcmpkt_dev_initialized(dunit, &initialized);
    if (actual_rv == SHR_E_NONE) {
        actual_rv = rv;
    }
    if (initialized) {
        rv = bcmpkt_dev_cleanup(dunit);
        if (actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
    }
    rv = bcmpkt_dev_drv_detach(dunit);
    if (actual_rv == SHR_E_NONE) {
        actual_rv = rv;
    }

    return actual_rv;
}

static int
bcmi_pkt_dev_init(int unit)
{
    int dunit = bcmi_pktio_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if  (bcmpkt_dev_drv_attached(dunit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_attach(dunit, BCMPKT_DEV_DRV_T_UNET));

exit:
    SHR_FUNC_EXIT();
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
pktio_discard_handler_setup(int unit, pktio_info_t *pi)
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
    int unit = bcmi_pktio_dev_unit(dunit);
    pktio_info_t *pi = NULL;
    pktio_rx_callout_t *rco = NULL;
    bcm_pktio_rx_t handler_rv;
    int handled = FALSE;

    SHR_FUNC_ENTER(unit);

    if (unit < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
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
                                     "pktio_rx_handler: unit=%d: "
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
esw_pktio_init(int unit)
{
    pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_pktio_dev_dunit(unit);
    bool initialized;
    bcmpkt_txpmd_fid_support_t txpmd_support;
    bcmpkt_rxpmd_fid_support_t rxpmd_support;
    bcmpkt_gih_fid_support_t gih_support;
    bcmpkt_higig3_fid_support_t higig3_support;
    /* */
    bcmpkt_dev_init_t cfg = {
        .cgrp_bmp = 0x1,
        .cgrp_size = 4,
        .mac_addr = PKTIO_DEF_NETIF_MAC,
    };
    int rv = SHR_E_NONE, netif_id = -1, i;

    assert(sizeof(bcm_pktio_pkt_t) == sizeof(bcmpkt_packet_t));

    SHR_FUNC_ENTER(unit);

    if (pi != NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(pi, sizeof(pktio_info_t), "eswPktioInfo");
        SHR_NULL_CHECK(pi, SHR_E_MEMORY);
        sal_memset(pi, 0, sizeof(pktio_info_t));
        pktio_info[unit] = pi;
    }

    /* Create mutex. */
    pi->mutex = sal_mutex_create("eswPktioInfo");
    SHR_NULL_CHECK(pi->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_type_get(dunit, &(pi->dev_type)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_fid_support_get(pi->dev_type, &txpmd_support));
    SHR_BITCOPY_RANGE(pi->support.txpmd_support.pbits, 0, txpmd_support.fbits,
                      0, BCMPKT_TXPMD_FID_COUNT);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_rxpmd_fid_support_get(pi->dev_type, &rxpmd_support));
    SHR_BITCOPY_RANGE(pi->support.rxpmd_support.pbits, 0, rxpmd_support.fbits,
                      0, BCMPKT_RXPMD_FID_COUNT);

    rv = bcmpkt_gih_fid_support_get(dunit, pi->dev_type, &gih_support);
    if (SHR_SUCCESS(rv)) {
        pi->flags |= PKTIO_F_GIH;
        SHR_BITCOPY_RANGE(pi->support.gih_support.pbits, 0, gih_support.fbits,
                          0, BCMPKT_GIH_FID_COUNT);
    } else if (rv != SHR_E_UNAVAIL) {
        SHR_RETURN_VAL_EXIT(rv);
    }

    rv = bcmpkt_higig3_fid_support_get(dunit, pi->dev_type, &higig3_support);
    if (SHR_SUCCESS(rv)) {
        pi->flags |= PKTIO_F_HIGIG3;
        SHR_BITCOPY_RANGE(pi->support.higig3_support.pbits, 0,
                          higig3_support.fbits, 0, BCMPKT_HIGIG3_FID_COUNT);
    } else if (rv != SHR_E_UNAVAIL) {
        SHR_RETURN_VAL_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_drv_type_get(dunit, &(pi->drv_type)));

    if (pi->drv_type == BCMPKT_DEV_DRV_T_NONE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    if (pi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_bpool_create(dunit,
                                 MAX_FRAME_SIZE_DEF, PKTIO_BCOUNT_DEF));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_initialized(dunit, &initialized));

    if (initialized) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_init(dunit, &cfg));

    if (pi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        bcmpkt_dma_chan_t chan = {
            .ring_size = PKTIO_RSIZE_DEF,
            .max_frame_size = MAX_FRAME_SIZE_DEF,
        };

        netif_id = pi->def_netif = 1;

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
    }

    /* Register RX dispatch callback. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_rx_register(dunit, netif_id, 0, pktio_rx_handler, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (pktio_discard_handler_setup(unit, pi));

exit:
    SHR_FUNC_EXIT();
}

static int
esw_pktio_cleanup(int unit)
{
    pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_pktio_dev_dunit(unit);
    int netif_id;
    pktio_rx_callout_t *rco = NULL, *next_rco = NULL;

    SHR_FUNC_ENTER(unit);

    if (pi == NULL) {
        SHR_EXIT();
    }

    netif_id = pi->def_netif;

    (void)bcmpkt_rx_unregister(dunit, netif_id, pktio_rx_handler, NULL);

    (void)bcmpkt_dev_cleanup(dunit);

    if (pi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        bcmpkt_bpool_destroy(dunit);
    }

    PKTIO_LOCK(unit);

    /* Destroy all RX callouts. */
    rco = pi->rco;
    while (rco) {
        next_rco = rco->rco_next;
        SHR_FREE(rco);
        rco = next_rco;
    }
    pi->rco = NULL;

    PKTIO_UNLOCK(unit);

    if (pi->mutex) {
        sal_mutex_destroy(pi->mutex);
    }

    SHR_FREE(pi);
    pktio_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

int
bcmi_esw_pktio_alloc(int unit, uint32 size, uint32 flags,
                    bcm_pktio_pkt_t **packet)
{
    int dunit = bcmi_pktio_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_alloc(dunit, (size < 64 ? 64 : size), flags,
                      (bcmpkt_packet_t **)packet));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_free(int unit, bcm_pktio_pkt_t *packet)
{
    int dunit = bcmi_pktio_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_free(dunit, (bcmpkt_packet_t *)packet));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_claim(int unit, bcm_pktio_pkt_t *pkt, bcm_pktio_pkt_t **new_pkt)
{
    int dunit = bcmi_pktio_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(new_pkt, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_packet_claim(dunit, (bcmpkt_packet_t *)pkt,
                             (bcmpkt_packet_t **)new_pkt));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_pkt_data_get(int unit, bcm_pktio_pkt_t *packet, void **data,
                        uint32 *len)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
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
bcmi_esw_pktio_reserve(int unit, bcm_pktio_pkt_t *packet, uint32 len,
                       void **data)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    *data = (void *)bcmpkt_reserve(((bcmpkt_packet_t *)packet)->data_buf, len);

    if (*data == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_push(int unit, bcm_pktio_pkt_t *packet, uint32 len,
                    void **data)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    *data = (void *)bcmpkt_push(((bcmpkt_packet_t *)packet)->data_buf, len);

    if (*data == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_put(int unit, bcm_pktio_pkt_t *packet, uint32 len,
                   void **data)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    *data = (void *)bcmpkt_put(((bcmpkt_packet_t *)packet)->data_buf, len);

    if (*data == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_pull(int unit, bcm_pktio_pkt_t *packet, uint32 len,
                    void **data)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    *data = (void *)bcmpkt_pull(((bcmpkt_packet_t *)packet)->data_buf, len);

    if (*data == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_trim(int unit, bcm_pktio_pkt_t *packet, uint32 len)
{
    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_trim(packet->data_buf, len));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_pmd_fid_support_get(int unit, bcm_pktio_fid_support_t *support)
{
    pktio_info_t *pi = pktio_info[unit];

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(support, SHR_E_PARAM);

    sal_memcpy(support, &(pi->support), sizeof(bcm_pktio_fid_support_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_pmd_field_set(int unit, bcm_pktio_pkt_t *packet,
                             bcm_pktio_pmd_type_t type,
                             int fid, uint32_t val)
{
    pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_pktio_dev_dunit(unit);
    uint32 *txpmd = NULL, *lbhdr = NULL, *hghdr = NULL;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    switch (type) {
        case bcmPktioPmdTypeTx:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_get((bcmpkt_packet_t *)packet, &txpmd));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_txpmd_field_set(pi->dev_type, txpmd, fid, val));

            break;
        case bcmPktioPmdTypeGih:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_lbhdr_get((bcmpkt_packet_t *)packet, &lbhdr));

            if ((pi->flags & PKTIO_F_GIH)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_gih_field_set(dunit, pi->dev_type, lbhdr, fid, val));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpkt_lbhdr_field_set(pi->dev_type, lbhdr, fid, val));
            }

            break;

        case bcmPktioPmdTypeHigig3:
            if (!(pi->flags & PKTIO_F_HIGIG3)) {
                SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_higig_get((bcmpkt_packet_t *)packet, &hghdr));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpkt_higig3_field_set(dunit, pi->dev_type, hghdr, fid, val));

            break;

        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_pmd_field_get(int unit, bcm_pktio_pkt_t *packet,
                             bcm_pktio_pmd_type_t type,
                             int fid, uint32 *val)
{
    pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_pktio_dev_dunit(unit);
    uint32_t *txpmd = NULL, *lbhdr = NULL, *hghdr = NULL;
    uint32 *rxpmd = NULL;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    switch (type) {
    case bcmPktioPmdTypeTx:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_txpmd_get((bcmpkt_packet_t *)packet, &txpmd));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_txpmd_field_get(pi->dev_type, txpmd, fid, val));

        break;
    case bcmPktioPmdTypeGih:
        if (!(pi->flags & PKTIO_F_GIH)) {
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_lbhdr_get((bcmpkt_packet_t *)packet, &lbhdr));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_gih_field_get(dunit, pi->dev_type, lbhdr, fid, val));

        break;
    case bcmPktioPmdTypeHigig3:
        if (!(pi->flags & PKTIO_F_HIGIG3)) {
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_higig_get((bcmpkt_packet_t *)packet, &hghdr));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_higig3_field_get(dunit, pi->dev_type, hghdr, fid, val));

        break;
    case bcmPktioPmdTypeRx:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_get((bcmpkt_packet_t *)packet, &rxpmd));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_rxpmd_field_get(pi->dev_type, rxpmd, fid, val));

        break;
    case bcmPktioPmdTypeRxFlex:
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_pmd_reasons_get(int unit, bcm_pktio_pkt_t *packet,
                               bcm_pktio_reasons_t *reasons)
{
    pktio_info_t *pi = pktio_info[unit];
    uint32 *rxpmd = NULL;
    bcmpkt_rx_reasons_t rx_reasons;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_rxpmd_get((bcmpkt_packet_t *)packet, &rxpmd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_rxpmd_reasons_get(pi->dev_type, rxpmd, &rx_reasons));

    SHR_BITCOPY_RANGE(reasons->rx_reasons.pbits, 0, rx_reasons.pbits, 0,
                      BCMPKT_RX_REASON_COUNT);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_pmd_set(int unit, bcm_pktio_pkt_t *packet,
                       bcm_pktio_txpmd_t *pmd)
{
    pktio_info_t *pi = pktio_info[unit];
    uint32 *txpmd = NULL, val;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(pmd, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_get((bcmpkt_packet_t *)packet, &txpmd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_field_set(pi->dev_type, txpmd, BCMPKT_TXPMD_START, 2));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                BCMPKT_TXPMD_HEADER_TYPE, 1));

    val = (pmd->flags & BCM_PKTIO_TX_MC_QUEUE) ? 0 : 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                BCMPKT_TXPMD_UNICAST, val));

    if (SHR_BITGET(pi->support.txpmd_support.pbits,
                   BCMPKT_TXPMD_UNICAST_PKT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_txpmd_field_set(pi->dev_type,
                                    txpmd,
                                    BCMPKT_TXPMD_UNICAST_PKT,
                                    val));
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
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_txpmd_field_set(pi->dev_type, txpmd,
                                BCMPKT_TXPMD_LOCAL_DEST_PORT, val));
exit:
    SHR_FUNC_EXIT();
}


int
bcmi_esw_pktio_tx(int unit, bcm_pktio_pkt_t *packet)
{
    pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_pktio_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_tx(dunit, pi->def_netif, (bcmpkt_packet_t *)packet));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_rx_register(int unit, const char *name,
                           bcm_pktio_rx_cb_f callback, uint8 priority,
                           void *cookie, uint32 flags)
{
    pktio_info_t *pi = pktio_info[unit];
    pktio_rx_callout_t *rco = NULL, *list_rco = NULL, *prev_rco = NULL;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(callback, SHR_E_PARAM);

    PKTIO_LOCK(unit);

    /* First check if already registered */
    for (list_rco = pi->rco; list_rco; list_rco = list_rco->rco_next) {
        if (list_rco->rco_function == callback &&
            list_rco->rco_priority == priority) {
            if (list_rco->rco_flags == flags &&
                list_rco->rco_cookie == cookie) {
                PKTIO_UNLOCK(unit);
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "PKTIO RX: %s registered with diff params\n"),
                         name));

            PKTIO_UNLOCK(unit);
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
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

    PKTIO_UNLOCK(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "PKTIO RX: %s registered %s%s.\n"),
                 name,
                 prev_rco ? "after " : "first",
                 prev_rco ? prev_rco->rco_name : ""));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_rx_unregister(int unit,
                             bcm_pktio_rx_cb_f callback, uint8 priority)
{
    pktio_info_t *pi = pktio_info[unit];
    pktio_rx_callout_t *rco = NULL, *prev_rco = NULL;
    const char *name;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    PKTIO_LOCK(unit);

    for (rco = pi->rco; rco; rco = rco->rco_next) {
        if (rco->rco_function == callback && rco->rco_priority == priority) {
            break;
        }
        prev_rco = rco;
    }

    if (!rco) {
        PKTIO_UNLOCK(unit);
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    name = rco->rco_name;

    if (!prev_rco) {
        pi->rco = rco->rco_next;
    } else {
        prev_rco->rco_next = rco->rco_next;
    }

    PKTIO_UNLOCK(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "PKTIO RX: Unregistered %s on %d\n"), name, unit));

    SHR_FREE(rco);

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_esw_pktio_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    pktio_initialized[unit] = FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (esw_pktio_cleanup(unit));

    bcmi_pkt_dev_cleanup(unit);
    bcmi_pkt_core_cleanup();

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_esw_pktio_cleanup(unit));

    bcmi_pkt_core_init(unit);
    bcmi_pkt_dev_init(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (esw_pktio_init(unit));

    pktio_initialized[unit] = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        bcmi_esw_pktio_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}


int
bcmi_esw_pktio_sw_dump(int unit)
{
    pktio_info_t *pi = pktio_info[unit];
    int dunit = bcmi_pktio_dev_dunit(unit);
    shr_pb_t *pb = NULL;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    cli_out("Packet device information:\n");
    cli_out("  Driver: ");
    if (pi->drv_type == BCMPKT_DEV_DRV_T_KNET) {
        cli_out("KNET\n");
    } else if (pi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        cli_out("UNET\n");
    } else {
        cli_out("Unknown - %d\n", pi->drv_type);
    }

    pb = shr_pb_create();
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_dev_info_dump(dunit, pb));
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);
    pb = NULL;

    if (pi->drv_type == BCMPKT_DEV_DRV_T_UNET) {
        cli_out("Packet buffer pool information:\n");
        pb = shr_pb_create();
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_bpool_info_dump(dunit, pb));
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);
        pb = NULL;
    }

exit:
    if (pb) {
        shr_pb_destroy(pb);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_esw_pktio_rx_dump(int unit, bcm_pktio_pkt_t *packet, int flags)
{
    pktio_info_t *pi = pktio_info[unit];
    uint32 *rxpmd;
    shr_pb_t *pb = NULL;

    SHR_FUNC_ENTER(unit);

    if (pktio_initialized[unit] == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmpkt_rxpmd_get((bcmpkt_packet_t *)packet, &rxpmd));

    pb = shr_pb_create();

    shr_pb_printf(pb, "[RX metadata information]\n");
    bcmpkt_rxpmd_dump(pi->dev_type, rxpmd, BCMPKT_RXPMD_DUMP_F_NONE_ZERO, pb);

    shr_pb_printf(pb, "[RX reasons]\n");
    bcmpkt_rx_reason_dump(pi->dev_type, rxpmd, pb);

    LOG_CLI((BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    pb = NULL;

exit:
    if (pb) {
        shr_pb_destroy(pb);
    }

    SHR_FUNC_EXIT();
}


#endif /* INCLUDE_PKTIO */
