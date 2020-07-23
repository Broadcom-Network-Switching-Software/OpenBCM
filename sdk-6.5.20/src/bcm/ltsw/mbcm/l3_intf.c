/*! \file l3_intf.c
 *
 * BCM L3 interface module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/l3_intf.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

static mbcm_ltsw_l3_intf_drv_t mbcm_ltsw_l3_intf_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_l3_intf_drv_set(int unit, mbcm_ltsw_l3_intf_drv_t *drv)
{
    mbcm_ltsw_l3_intf_drv_t *local = &mbcm_ltsw_l3_intf_drv[unit];

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    return SHR_E_NONE;
}

int
mbcm_ltsw_l3_intf_tbl_db_get(int unit, bcmint_ltsw_l3_intf_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_tbl_db_get(unit, tbl_db));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_intf_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_intf_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_id_range_get(int unit, int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_id_range_get(unit, min, max));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_class_id_range_get(int unit, int *min, int *max, int *width)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_class_id_range_get(unit, min, max,
                                                               width));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_create(int unit, bcm_l3_ingress_t *intf_info,
                             bcm_if_t *intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_create(unit, intf_info,
                                                        intf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_destroy(int unit, bcm_if_t intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_destroy(unit, intf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_get(int unit, bcm_if_t intf_id,
                          bcm_l3_ingress_t *ing_intf)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_get(unit, intf_id, ing_intf));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_find(int unit, bcm_l3_ingress_t *ing_intf,
                           bcm_if_t *intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_find(unit, ing_intf, intf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_traverse(int unit,
                               bcm_l3_ingress_traverse_cb trav_fn,
                               void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_traverse(unit, trav_fn,
                                                          user_data));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_flexctr_attach(int unit,
                                     bcm_if_t intf_id,
                                     bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_flexctr_attach(unit, intf_id,
                                                                info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_flexctr_detach(int unit, bcm_if_t intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_flexctr_detach(unit, intf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ing_intf_flexctr_info_get(int unit,
                                       bcm_if_t intf_id,
                                       bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_flexctr_info_get(unit, intf_id,
                                                                  info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_l2cpu_intf_id_get(int unit, int *cpu_intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_l2cpu_intf_id_get(unit,
                                                              cpu_intf_id));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_id_range_get(int unit,
                                   bcmi_ltsw_l3_egr_intf_type_t type,
                                   int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_id_range_get(unit, type,
                                                              min, max));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_to_index(int unit, bcm_if_t intf_id, int *index,
                               bcmi_ltsw_l3_egr_intf_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_to_index(unit, intf_id,
                                                          index, type));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_index_to_intf(int unit, int index,
                               bcmi_ltsw_l3_egr_intf_type_t type,
                               bcm_if_t *intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_index_to_intf(unit, index,
                                                          type, intf_id));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_create(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_create(unit, intf_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_get(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_get(unit, intf_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_find(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_find(unit, intf_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_delete(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_delete(unit, intf_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_delete_all(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_flexctr_attach(int unit,
                                     bcm_if_t intf_id,
                                     bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_flexctr_attach(unit, intf_id,
                                                                info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_flexctr_detach(int unit, bcm_if_t intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_flexctr_detach(unit, intf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egr_intf_flexctr_info_get(int unit,
                                       bcm_if_t intf_id,
                                       bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_egr_intf_flexctr_info_get(unit, intf_id,
                                                                  info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_intf_tnl_init_set(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t tnl_type,
    int tnl_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_tnl_init_set(unit, intf_id,
                                                          tnl_type, tnl_idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_intf_tnl_init_get(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_tnl_init_get(unit, intf_id,
                                                          tnl_type, tnl_idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_intf_tnl_init_reset(int unit, int intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_tnl_init_reset(unit, intf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_intf_vfi_update(int unit, int vid,
                             bcmi_ltsw_l3_intf_vfi_t *vfi_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_vfi_update(unit, vid, vfi_info));

exit:
    SHR_FUNC_EXIT()
}

int
mbcm_ltsw_l3_ing_intf_rp_op(int unit, int intf_id, int rp_id, int set)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_rp_op(unit, intf_id,
                                                       rp_id, set));

exit:
    SHR_FUNC_EXIT()
}

int
mbcm_ltsw_l3_ing_intf_rp_get(int unit, int intf_id, uint32_t *rp_bmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_ing_intf_rp_get(unit, intf_id, rp_bmp));

exit:
    SHR_FUNC_EXIT()
}

int
mbcm_ltsw_l3_intf_mpls_recover_all(int unit,
                                   bcmi_ltsw_l3_intf_mpls_recover_cb cb,
                                   void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_mpls_recover_all(unit, cb,
                                                              user_data));

exit:
    SHR_FUNC_EXIT()
}

int
mbcm_ltsw_l3_intf_adapt_lookup_key_set(
    int unit,
    int intf_id,
    int key_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv[unit].l3_intf_adapt_lookup_key_set(unit,
                                                                  intf_id,
                                                                  key_type));

exit:
    SHR_FUNC_EXIT();
}
