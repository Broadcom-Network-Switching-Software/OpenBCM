/*! \file l3_egress.c
 *
 * BCM L3 Egress module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/l3_egress.h>
#include <bcm_int/ltsw/l3_egress.h>

#include <shr/shr_debug.h>


/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

static mbcm_ltsw_l3_egress_drv_t mbcm_ltsw_l3_egress_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_l3_egress_drv_set(int unit, mbcm_ltsw_l3_egress_drv_t *drv)
{
    mbcm_ltsw_l3_egress_drv_t *local = &mbcm_ltsw_l3_egress_drv[unit];

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                        (mbcm_ltsw_drv_t *)drv,
                        sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    return SHR_E_NONE;
}

int
mbcm_ltsw_l3_egress_l2_rsv(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_l2_rsv(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_id_range_get(int unit, int type, int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_id_range_get(unit, type,
                                                              min, max));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_create(int unit, bcm_l3_egress_t *egr, int *idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_create(unit, egr, idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_destroy(int unit, int idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_destroy(unit, idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_get(int unit, int idx, bcm_l3_egress_t *egr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_get(unit, idx, egr));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_find(int unit, bcm_l3_egress_t *egr, int *idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_find(unit, egr, idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_traverse(int unit,
                             bcm_l3_egress_traverse_cb trav_fn,
                             void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_traverse(unit, trav_fn,
                                                          user_data));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_obj_id_resolve(int unit,
                                   bcm_if_t intf,
                                   int *nh_ecmp_idx,
                                   bcmi_ltsw_l3_egr_obj_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_obj_id_resolve(unit, intf,
                                                                nh_ecmp_idx,
                                                                type));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_obj_id_construct(int unit,
                                     int nh_ecmp_idx,
                                     bcmi_ltsw_l3_egr_obj_type_t type,
                                     bcm_if_t *intf)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_obj_id_construct(unit,
                                                                  nh_ecmp_idx,
                                                                  type, intf));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_obj_info_get(int unit, bcm_if_t intf,
                                 bcmi_ltsw_l3_egr_obj_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_obj_info_get(unit, intf,
                                                              info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_obj_cnt_get(int unit, bcmi_ltsw_l3_egr_obj_type_t type,
                                int *max, int *used)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_obj_cnt_get(unit, type,
                                                             max, used));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_dvp_set(int unit, bcm_if_t intf, int dvp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_dvp_set(unit, intf, dvp));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_flexctr_attach(int unit, bcm_if_t intf_id,
                                   bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_flexctr_attach(unit, intf_id,
                                                                info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_flexctr_detach(int unit, bcm_vrf_t intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_flexctr_detach(unit, intf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_flexctr_info_get(int unit, bcm_if_t intf_id,
                                     bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_flexctr_info_get(unit, intf_id,
                                                                  info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_flexctr_object_set(unit,
                                                                    intf_id,
                                                                    value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_flexctr_object_get(unit,
                                                                    intf_id,
                                                                    value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_ifa_create(
    int unit,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa,
    int *idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_ifa_create(unit, egr_ifa,
                                                            idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_ifa_destroy(int unit, int idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_ifa_destroy(unit, idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_ifa_get(int unit, int idx, bcmi_ltsw_l3_egr_ifa_t *egr_ifa)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_ifa_get(unit, idx, egr_ifa));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_ifa_traverse(
    int unit,
    bcm_ifa_header_traverse_cb trav_fn,
    bcmi_ltsw_l3_egr_to_ifa_header_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_ifa_traverse(unit, trav_fn, cb,
                                                              user_data));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_create(int unit,
                         bcm_l3_egress_ecmp_t *ecmp_info,
                         int ecmp_member_count,
                         bcm_l3_ecmp_member_t *ecmp_member_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_create(unit, ecmp_info,
                                                      ecmp_member_count,
                                                      ecmp_member_array));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_destroy(int unit, bcm_if_t ecmp_group_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_destroy(unit, ecmp_group_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_get(int unit,
                      bcm_l3_egress_ecmp_t *ecmp_info,
                      int ecmp_member_size,
                      bcm_l3_ecmp_member_t *ecmp_member_array,
                      int *ecmp_member_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_get(unit, ecmp_info,
                                                   ecmp_member_size,
                                                   ecmp_member_array,
                                                   ecmp_member_count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_member_add(int unit, bcm_if_t ecmp_group_id,
                             bcm_l3_ecmp_member_t *ecmp_member)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_member_add(unit, ecmp_group_id,
                                                          ecmp_member));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_member_del(int unit, bcm_if_t ecmp_group_id,
                             bcm_l3_ecmp_member_t *ecmp_member)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_member_del(unit, ecmp_group_id,
                                                          ecmp_member));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_member_del_all(int unit, bcm_if_t ecmp_group_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_member_del_all(unit,
                                                              ecmp_group_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_find(int unit, int ecmp_member_count,
                       bcm_l3_ecmp_member_t *ecmp_member_array,
                       bcm_l3_egress_ecmp_t *ecmp_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_find(unit, ecmp_member_count,
                                                    ecmp_member_array,
                                                    ecmp_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_trav(int unit, bcm_l3_ecmp_traverse_cb trav_fn,
                       void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_trav(unit, trav_fn, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_max_paths_get(int unit, uint32_t attr, int *max_paths)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_max_paths_get(unit,
                                                             attr, max_paths));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_hash_ctrl_set(int unit, int underlay, int flow_based)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_hash_ctrl_set(unit, underlay,
                                                             flow_based));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_hash_ctrl_get(int unit, int underlay, int *flow_based)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_hash_ctrl_get(unit, underlay,
                                                             flow_based));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_flow_hash_set(int unit,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_flow_hash_set(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_flow_hash_get(int unit,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_flow_hash_get(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_port_hash_set(int unit,
                                int port,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_port_hash_set(unit, port, type,
                                                             arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_port_hash_get(int unit,
                                int port,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_port_hash_get(unit, port, type,
                                                             arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_member_status_set(int unit, bcm_if_t intf, int status)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_member_status_set(unit, intf,
                                                                 status));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_member_status_get(int unit, bcm_if_t intf, int *status)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_member_status_get(unit, intf,
                                                                 status));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_ethertype_set(
    int unit,
    uint32_t flags,
    int ethertype_count,
    int *ethertype_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_ethertype_set(unit, flags,
                                                             ethertype_count,
                                                             ethertype_array));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_ethertype_get(int unit, int ethertype_max, uint32_t *flags,
                                int *ethertype_array, int *ethertype_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_ethertype_get(unit, ethertype_max, flags,
                                                             ethertype_array, ethertype_count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_dlb_mon_config_set(int unit,
                                     bcm_if_t ecmp_group_id,
                                     bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_dlb_mon_config_set(unit, ecmp_group_id,
                                                                  dlb_mon_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_dlb_mon_config_get(int unit,
                                     bcm_if_t ecmp_group_id,
                                     bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_dlb_mon_config_get(unit, ecmp_group_id,
                                                                  dlb_mon_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_ecmp_member_dest_alloc(int unit,
                                 bcmi_ltsw_ecmp_member_dest_info_t *dest_info,
                                 int *dest_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].ecmp_member_dest_alloc(unit, dest_info,
                                                              dest_index));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_ecmp_member_dest_get(int unit, int dest_index,
                               bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].ecmp_member_dest_get(unit, dest_index,
                                                            dest_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_ecmp_member_dest_free(int unit, int dest_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].ecmp_member_dest_free(unit, dest_index));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_ecmp_member_dest_update(int unit,
                                  bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].ecmp_member_dest_update(unit,
                                                               dest_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_size_get(int unit, int *size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_size_get(unit, size));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_count_get(int unit, int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_count_get(unit, count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_size_get(int unit, int *size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_size_get(unit, size));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_count_get(int unit, int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_count_get(unit, count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_flexctr_attach(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_flexctr_attach(unit,
                                                              intf_id,
                                                              info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_flexctr_detach(
    int unit,
    bcm_vrf_t intf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_flexctr_detach(unit,
                                                              intf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_flexctr_info_get(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_flexctr_info_get(unit,
                                                                intf_id,
                                                                info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t obj_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_flexctr_object_set(unit,
                                                                  intf_id,
                                                                  obj_val));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *obj_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_flexctr_object_get(unit,
                                                                  intf_id,
                                                                  obj_val));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_tbl_db_get(unit, tbl_db));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_tbl_db_get(unit, tbl_db));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_flexctr_action_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_flexctr_action_tbl_db_get(unit, tbl_db));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_overlay_table_init(
    int unit,
    bcmint_ltsw_l3_ol_tbl_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_overlay_table_init(unit, info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_dlb_stat_set(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_dlb_stat_set(unit, ecmp_intf, type, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_dlb_stat_get(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_dlb_stat_get(unit, ecmp_intf, type, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ecmp_dlb_stat_sync_get(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_ecmp_dlb_stat_sync_get(unit, ecmp_intf, type, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_add_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_add_l2tag(unit, nhidx, l2_tag_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_get_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_get_l2tag(unit, nhidx, l2_tag_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_delete_l2tag(
    int unit,
    int nhidx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_delete_l2tag(unit, nhidx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_php_action_add(
    int unit,
    bcm_if_t intf)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_php_action_add(unit, intf));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_php_action_delete(
    int unit,
    bcm_if_t intf)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_php_action_del(unit, intf));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_secmp_clear(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_secmp_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_secmp_set(
    int unit,
    int idx,
    bcm_l3_egress_ecmp_t *ecmp_info,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_secmp_set(
            unit, idx, ecmp_info, member, count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_secmp_del(
    int unit,
    int idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_secmp_del(unit, idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_secmp_member_add(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_secmp_member_add(unit, idx, member));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_secmp_member_del(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count,
    bcmi_ltsw_l3_ecmp_member_info_t *del_member)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_secmp_member_del(
            unit, idx, member, count, del_member));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_secmp_member_del_all(
    int unit,
    int idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_secmp_member_del_all(unit, idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
                                        int member_cnt, bcm_port_t *member_arr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_trunk_member_del(unit,
                                                                  trunk_id,
                                                                  member_cnt,
                                                                  member_arr));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_egress_trunk_member_add(int unit, bcm_trunk_t trunk_id,
                                     int member_cnt, bcm_port_t *member_arr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv[unit].l3_egress_trunk_member_add(unit,
                                                                  trunk_id,
                                                                  member_cnt,
                                                                  member_arr));

exit:
    SHR_FUNC_EXIT();
}
