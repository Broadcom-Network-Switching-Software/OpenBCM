/*! \file bcmpkt_socket.c
 *
 * SOCKET access functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMPKT_DEV

#define BCMPKT_RX_POLL_TIME_OUT     1000

static bcmpkt_socket_t uvect[BCMPKT_SOCKET_DRV_T_COUNT];
/* Per unit driver handler. */
static bcmpkt_socket_t *udrv[BCMDRD_CONFIG_MAX_UNITS];

int
bcmpkt_socket_drv_register(bcmpkt_socket_t *socket_drv)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(socket_drv, SHR_E_PARAM);

    if (socket_drv->driver_type >= BCMPKT_SOCKET_DRV_T_COUNT ||
        socket_drv->driver_type < BCMPKT_SOCKET_DRV_T_TPKT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (socket_drv->initialized != 1) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (uvect[socket_drv->driver_type].initialized) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    uvect[socket_drv->driver_type] = *socket_drv;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_socket_drv_inuse(bcmpkt_socket_drv_types_t type)
{
    int i = 0;

    if (type < BCMPKT_SOCKET_DRV_T_COUNT &&
        type >= BCMPKT_SOCKET_DRV_T_TPKT) {
        for (i = 0; i < BCMDRD_CONFIG_MAX_UNITS; i++) {
            if (udrv[i] == &uvect[type]) {
                return 1;
            }
        }
    }

    return 0;
}

int
bcmpkt_socket_drv_unregister(bcmpkt_socket_drv_types_t type)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (type >= BCMPKT_SOCKET_DRV_T_COUNT ||
        type < BCMPKT_SOCKET_DRV_T_TPKT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (bcmpkt_socket_drv_inuse(type)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (uvect[type].initialized) {
        sal_memset(&uvect[type], 0, sizeof(bcmpkt_socket_t));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_socket_drv_attach(int unit, bcmpkt_socket_drv_types_t type)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (udrv[unit] != NULL) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    switch (type) {
    case BCMPKT_SOCKET_DRV_T_AUTO:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET)) {
            if (uvect[BCMPKT_SOCKET_DRV_T_TPKT].initialized) {
                udrv[unit] = &uvect[BCMPKT_SOCKET_DRV_T_TPKT];
            }
        }
        if (udrv[unit] == NULL) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        break;
    case BCMPKT_SOCKET_DRV_T_TPKT:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET) &&
            uvect[type].initialized) {
            udrv[unit] = &uvect[type];
            break;
        }
        SHR_ERR_EXIT(SHR_E_FAIL);
    case BCMPKT_SOCKET_DRV_T_RAWSOCK:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET) &&
            uvect[type].initialized) {
            udrv[unit] = &uvect[type];
            break;
        }
        SHR_ERR_EXIT(SHR_E_FAIL);
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    LOG_VERBOSE(BSL_LS_BCMPKT_SOCKET,
                (BSL_META_U(unit, "Attach SOCKET driver succeed\n")));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_socket_drv_get(int unit, bcmpkt_socket_t **socket)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    *socket = udrv[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_socket_drv_type_get(int unit, bcmpkt_socket_drv_types_t *type)
{
    bcmpkt_socket_t *socket;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);
    *type = BCMPKT_SOCKET_DRV_T_NONE;
    SHR_IF_ERR_EXIT
        (bcmpkt_socket_drv_get(unit, &socket));
    if (socket) {
        *type = socket->driver_type;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
socket_delete(int unit, const bcmpkt_netif_t *netif, void *data)
{
    if (netif != NULL) {
        int rv;
        bool created;
        rv = bcmpkt_socket_created(unit, netif->id, &created);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get SOCKET created status failed (%d)\n"),
                       rv));
        } else if (created) {
            rv = bcmpkt_socket_destroy(unit, netif->id);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Destroy SOCKET on network interface %d failed (%d)\n"),
                                      netif->id, rv));
            }
        }
    }

    return SHR_E_NONE;
}

int
bcmpkt_socket_drv_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (udrv[unit] != NULL) {
        int rv;
        bcmpkt_dev_drv_types_t drv_type;
        rv = bcmpkt_dev_drv_type_get(unit, &drv_type);
        if (SHR_SUCCESS(rv) && drv_type == BCMPKT_DEV_DRV_T_KNET) {
            (void)bcmpkt_netif_traverse(unit, socket_delete, NULL);
        }

        udrv[unit] = NULL;
        LOG_VERBOSE(BSL_LS_BCMPKT_SOCKET,
                    (BSL_META_U(unit, "SOCKET driver was detached\n")));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_socket_create(int unit, int netif_id, bcmpkt_socket_cfg_t *cfg)
{
    bcmpkt_socket_t *socket;
    bcmpkt_socket_cfg_t socket_cfg = {
        .rx_poll_timeout = BCMPKT_RX_POLL_TIME_OUT,
        .rx_buf_size = 0,
        .tx_buf_size = 0,
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_socket_drv_get(unit, &socket));
    SHR_NULL_CHECK(socket, SHR_E_CONFIG);

    SHR_NULL_CHECK(socket->create, SHR_E_CONFIG);
    if (!cfg) {
        cfg = &socket_cfg;
    }
    SHR_IF_ERR_EXIT
        (socket->create(unit, netif_id, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_socket_destroy(int unit, int netif_id)
{
    bcmpkt_socket_t *socket;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_socket_drv_get(unit, &socket));
    SHR_NULL_CHECK(socket, SHR_E_CONFIG);

    SHR_NULL_CHECK(socket->destroy, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (socket->destroy(unit, netif_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_socket_created(int unit, int netif_id, bool *created)
{
     bcmpkt_socket_t *socket;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(created, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_socket_drv_get(unit, &socket));
    *created = false;
    if (socket != NULL && socket->created) {
        SHR_IF_ERR_EXIT
            (socket->created(unit, netif_id, created));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_socket_stats_dump(int unit, int netif_id, shr_pb_t *pb)
{
    bcmpkt_socket_t *socket;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_socket_drv_get(unit, &socket));
    SHR_NULL_CHECK(socket, SHR_E_CONFIG);
    SHR_NULL_CHECK(socket->stats_dump, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (socket->stats_dump(unit, netif_id, pb));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_socket_stats_clear(int unit, int netif_id)
{
    bcmpkt_socket_t *socket;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_socket_drv_get(unit, &socket));
    SHR_NULL_CHECK(socket, SHR_E_CONFIG);
    SHR_NULL_CHECK(socket->stats_clear, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (socket->stats_clear(unit, netif_id));

exit:
    SHR_FUNC_EXIT();
}
