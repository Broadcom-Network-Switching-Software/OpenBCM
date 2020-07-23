/*! \file bcmpkt_net.c
 *
 * NET access.
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
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpkt/bcmpkt_buf.h>

#define TXPMD_START_IHEADER           2
#define TXPMD_HEADER_TYPE_FROM_CPU    1
#define TXPMD_UNICAST                 1

#define BSL_LOG_MODULE BSL_LS_BCMPKT_NET

static bcmpkt_net_t nvect[BCMPKT_NET_DRV_T_COUNT];

int
bcmpkt_net_drv_register(bcmpkt_net_t *net_drv)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(net_drv, SHR_E_PARAM);

    if (net_drv->driver_type >= BCMPKT_NET_DRV_T_COUNT ||
        net_drv->driver_type <= BCMPKT_NET_DRV_T_AUTO) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (net_drv->initialized != 1) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (nvect[net_drv->driver_type].initialized) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    nvect[net_drv->driver_type] = *net_drv;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_net_drv_unregister(bcmpkt_net_drv_types_t type)
{
    bcmpkt_dev_drv_types_t dev_drv_type = type - BCMPKT_NET_DRV_T_NONE;
    bcmpkt_socket_drv_types_t socket_drv_type = type - BCMPKT_NET_DRV_T_NONE;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (type >= BCMPKT_NET_DRV_T_COUNT ||
        type <= BCMPKT_NET_DRV_T_AUTO) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((type < BCMPKT_NET_DRV_T_TPKT && bcmpkt_dev_drv_inuse(dev_drv_type)) ||
        (type >= BCMPKT_NET_DRV_T_TPKT && bcmpkt_socket_drv_inuse(socket_drv_type)))
        {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (nvect[type].initialized) {
        sal_memset(&nvect[type], 0, sizeof(bcmpkt_net_t));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_net_drv_get(int unit, bcmpkt_net_t **net)
{
    uint32_t drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &drv_type));

    if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_EXIT
            (bcmpkt_socket_drv_type_get(unit, &drv_type));
    }

    *net = &nvect[drv_type];
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_fwd_port_set(bcmdrd_dev_type_t dev_type, int port,
                    bcmpkt_packet_t *packet)
{
    uint32_t *txpmd;
    uint32_t val;
    bcmpkt_txpmd_fid_support_t support_status;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_fid_support_get(dev_type, &support_status));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_get(packet, &txpmd));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_get(dev_type, txpmd, BCMPKT_TXPMD_START, &val));

    /* If not initialized. */
    if (val == 0) {
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                    BCMPKT_TXPMD_START, TXPMD_START_IHEADER));
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                    BCMPKT_TXPMD_HEADER_TYPE,
                                    TXPMD_HEADER_TYPE_FROM_CPU));
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                    BCMPKT_TXPMD_UNICAST, TXPMD_UNICAST));

        if (SHR_BITGET(support_status.fbits, BCMPKT_TXPMD_UNICAST_PKT)) {
            SHR_IF_ERR_EXIT
                (bcmpkt_txpmd_field_set(dev_type,
                                        txpmd,
                                        BCMPKT_TXPMD_UNICAST_PKT,
                                        TXPMD_UNICAST));
        }
    }

    if (SHR_BITGET(support_status.fbits, BCMPKT_TXPMD_LOCAL_DEST_PORT)) {
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                    BCMPKT_TXPMD_LOCAL_DEST_PORT, port));
    }
    if (SHR_BITGET(support_status.fbits, BCMPKT_TXPMD_DESTINATION_TYPE) &&
        SHR_BITGET(support_status.fbits, BCMPKT_TXPMD_DESTINATION)) {
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                    BCMPKT_TXPMD_DESTINATION_TYPE, 0));
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                    BCMPKT_TXPMD_DESTINATION, port));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_fwd_type_set(bcmpkt_fwd_types_t type, bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    if (type >= BCMPKT_FWD_T_COUNT) {
        LOG_ERROR(BSL_LS_BCMPKT_NET, (BSL_META("Unknown type %08x\n"), type));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    packet->type = type;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_register(int unit, int netif_id, uint32_t flags,
                   bcmpkt_rx_cb_f cb_func, void *cb_data)
{
    bcmpkt_net_t *net;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_get(unit, &net));
    SHR_NULL_CHECK(net, SHR_E_CONFIG);
    SHR_NULL_CHECK(net->rx_register, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (net->rx_register(unit, netif_id, flags, cb_func, cb_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_unregister(int unit, int netif_id, bcmpkt_rx_cb_f cb_func,
                     void *cb_data)
{
    bcmpkt_net_t *net;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_get(unit, &net));
    SHR_NULL_CHECK(net, SHR_E_CONFIG);
    SHR_NULL_CHECK(net->rx_unregister, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (net->rx_unregister(unit, netif_id, cb_func, cb_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_tx(int unit, int netif_id, bcmpkt_packet_t *packet)
{
    bcmpkt_net_t *net;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_get(unit, &net));
    SHR_NULL_CHECK(net, SHR_E_CONFIG);
    SHR_NULL_CHECK(net->tx, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (net->tx(unit, netif_id, packet));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_net_drv_type_get(int unit, bcmpkt_net_drv_types_t *type)
{
    bcmpkt_net_t *net;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_get(unit, &net));
    SHR_NULL_CHECK(net, SHR_E_CONFIG);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    *type = net->driver_type;
exit:
    SHR_FUNC_EXIT();

}
