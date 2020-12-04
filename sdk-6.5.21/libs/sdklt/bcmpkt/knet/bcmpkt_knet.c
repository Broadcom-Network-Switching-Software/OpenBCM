/*! \file bcmpkt_knet.c
 *
 * KNET configuration access interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMPKT_DEV

static bcmpkt_knet_t kvect;

int
bcmpkt_knet_drv_register(bcmpkt_knet_t *knet_drv)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(knet_drv, SHR_E_PARAM);

    if (knet_drv->initialized != 1) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (kvect.initialized) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    kvect = *knet_drv;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_knet_drv_unregister(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (bcmpkt_dev_drv_inuse(BCMPKT_DEV_DRV_T_KNET)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (kvect.initialized) {
        sal_memset(&kvect, 0, sizeof(bcmpkt_knet_t));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_knet_drv_get(int unit, bcmpkt_knet_t **knet)
{
    bcmpkt_dev_drv_types_t dev_drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &dev_drv_type));
    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        *knet = &kvect;
    } else {
        *knet = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_netif_create(int unit, bcmpkt_netif_t *netif)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->netif_ops.create, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->netif_ops.create(unit, netif));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_netif_get(int unit, int netif_id, bcmpkt_netif_t *netif)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->netif_ops.get, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->netif_ops.get(unit, netif_id, netif));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_netif_traverse(int unit, bcmpkt_netif_traverse_cb_f cb_func,
                      void *cb_data)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->netif_ops.traverse, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->netif_ops.traverse(unit, cb_func, cb_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_netif_destroy(int unit, int netif_id)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->netif_ops.destroy, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->netif_ops.destroy(unit, netif_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_netif_link_update(int unit, int netif_id, bool linkup)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->netif_ops.link_update, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->netif_ops.link_update(unit, netif_id, linkup));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_netif_max_get(int unit, int *max_num)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->netif_ops.netif_max_get, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->netif_ops.netif_max_get(unit, max_num));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_filter_create(int unit, bcmpkt_filter_t *filter)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->filter_ops.create, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->filter_ops.create(unit, filter));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_filter_get(int unit, int filter_id, bcmpkt_filter_t *filter)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->filter_ops.get, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->filter_ops.get(unit, filter_id, filter));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_filter_traverse(int unit, bcmpkt_filter_traverse_cb_f cb_func,
                       void *cb_data)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->filter_ops.traverse, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->filter_ops.traverse(unit, cb_func, cb_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_filter_destroy(int unit, int filter_id)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->filter_ops.destroy, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->filter_ops.destroy(unit, filter_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_filter_max_get(int unit, int *max_num)
{
    bcmpkt_knet_t *knet;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_get(unit, &knet));
    SHR_NULL_CHECK(knet, SHR_E_CONFIG);
    SHR_NULL_CHECK(knet->filter_ops.filter_max_get, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (knet->filter_ops.filter_max_get(unit, max_num));

exit:
    SHR_FUNC_EXIT();
}
