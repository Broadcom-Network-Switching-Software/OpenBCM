/*! \file l3_fib.c
 *
 * BCM L3 FIB module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/l3_fib.h>
#include <bcm_int/ltsw/uft.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

static mbcm_ltsw_l3_fib_drv_t mbcm_ltsw_l3_fib_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_l3_fib_drv_set(int unit, mbcm_ltsw_l3_fib_drv_t *drv)
{
    mbcm_ltsw_l3_fib_drv_t *local = &mbcm_ltsw_l3_fib_drv[unit];

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    return SHR_E_NONE;
}

int
mbcm_ltsw_l3_fib_init(int unit)
{

    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_init(unit);
}

int
mbcm_ltsw_l3_fib_deinit(int unit)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_deinit(unit);
}

int
mbcm_ltsw_l3_fib_vrf_max_get(int unit, uint32_t *max_vrf)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_vrf_max_get(unit, max_vrf);
}

int
mbcm_ltsw_l3_host_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_uc_add(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_uc_delete(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv4_uc_del_by_prefix(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_uc_del_by_prefix(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv4_uc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_uc_find(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv4_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_host_traverse_cb cb, void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_uc_traverse(unit, flags,
                                                               start, end, cb,
                                                               user_data);
}

int
mbcm_ltsw_l3_host_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_add(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_delete(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv4_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_find(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv4_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_traverse(unit, attrib,
                                                               trav_fn, cb,
                                                               user_data);
}

int
mbcm_ltsw_l3_host_ipv4_mc_flexctr_attach(int unit,
                                         bcmi_ltsw_l3_fib_t *fib,
                                         bcmi_ltsw_flexctr_counter_info_t *ci)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_flexctr_attach(unit,
                                                                     fib, ci);
}

int
mbcm_ltsw_l3_host_ipv4_mc_flexctr_detach(int unit,
                                         bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_flexctr_detach(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv4_mc_flexctr_info_get(int unit,
                                           bcmi_ltsw_l3_fib_t *fib,
                                           bcmi_ltsw_flexctr_counter_info_t *ci)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_flexctr_info_get(unit,
                                                                       fib, ci);
}

int
mbcm_ltsw_l3_host_ipv4_mc_flexctr_object_set(int unit,
                                             bcmi_ltsw_l3_fib_t *fib,
                                             uint32_t value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_flexctr_object_set(unit,
                                                                         fib,
                                                                         value);
}

int
mbcm_ltsw_l3_host_ipv4_mc_flexctr_object_get(int unit,
                                             bcmi_ltsw_l3_fib_t *fib,
                                             uint32_t *value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv4_mc_flexctr_object_get(unit,
                                                                         fib,
                                                                         value);
}

int
mbcm_ltsw_l3_host_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_uc_add(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_uc_delete(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv6_uc_del_by_prefix(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_uc_del_by_prefix(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv6_uc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_uc_find(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv6_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_host_traverse_cb cb, void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_uc_traverse(unit, flags,
                                                               start, end, cb,
                                                               user_data);
}

int
mbcm_ltsw_l3_host_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_add(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_delete(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv6_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_find(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv6_mc_traverse(int unit, uint32_t attrib,
                                   bcm_ipmc_traverse_cb trav_fn,
                                   bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                   void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_traverse(unit, attrib,
                                                               trav_fn, cb,
                                                               user_data);
}

int
mbcm_ltsw_l3_host_ipv6_mc_flexctr_attach(int unit,
                                         bcmi_ltsw_l3_fib_t *fib,
                                         bcmi_ltsw_flexctr_counter_info_t *ci)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_flexctr_attach(unit,
                                                                     fib, ci);
}

int
mbcm_ltsw_l3_host_ipv6_mc_flexctr_detach(int unit,
                                         bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_flexctr_detach(unit, fib);
}

int
mbcm_ltsw_l3_host_ipv6_mc_flexctr_info_get(int unit,
                                           bcmi_ltsw_l3_fib_t *fib,
                                           bcmi_ltsw_flexctr_counter_info_t *ci)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_flexctr_info_get(unit,
                                                                       fib, ci);
}

int
mbcm_ltsw_l3_host_ipv6_mc_flexctr_object_set(int unit,
                                             bcmi_ltsw_l3_fib_t *fib,
                                             uint32_t value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_flexctr_object_set(unit,
                                                                         fib,
                                                                         value);
}

int
mbcm_ltsw_l3_host_ipv6_mc_flexctr_object_get(int unit,
                                             bcmi_ltsw_l3_fib_t *fib,
                                             uint32_t *value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_ipv6_mc_flexctr_object_get(unit,
                                                                         fib,
                                                                         value);
}

int
mbcm_ltsw_l3_host_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_del_by_intf(unit, fib, negate);
}

int
mbcm_ltsw_l3_host_del_all(int unit, uint32_t attrib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_del_all(unit, attrib);
}

int
mbcm_ltsw_l3_host_age(int unit, bcm_l3_host_traverse_cb age_out,
                      void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_age(unit, age_out, user_data);
}

int
mbcm_ltsw_l3_route_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_uc_add(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_uc_delete(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv4_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_uc_find(unit, lpm, fib);
}

int
mbcm_ltsw_l3_route_ipv4_uc_traverse(int unit, uint32_t flags,
                                    uint32_t start, uint32_t end,
                                    bcm_l3_route_traverse_cb cb,
                                    void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_uc_traverse(unit, flags,
                                                                start, end, cb,
                                                                user_data);
}

int
mbcm_ltsw_l3_route_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_add(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_delete(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv4_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_find(unit, lpm, fib);
}

int
mbcm_ltsw_l3_route_ipv4_mc_traverse(int unit, uint32_t attrib,
                                    bcm_ipmc_traverse_cb trav_fn,
                                    bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                    void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_traverse(unit, attrib,
                                                                trav_fn, cb,
                                                                user_data);
}

int
mbcm_ltsw_l3_route_ipv4_mc_flexctr_attach(int unit,
                                          bcmi_ltsw_l3_fib_t *fib,
                                          bcmi_ltsw_flexctr_counter_info_t *ci)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_flexctr_attach(unit,
                                                                      fib, ci);
}

int
mbcm_ltsw_l3_route_ipv4_mc_flexctr_detach(int unit,
                                          bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_flexctr_detach(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv4_mc_flexctr_info_get(int unit,
                                        bcmi_ltsw_l3_fib_t *fib,
                                        bcmi_ltsw_flexctr_counter_info_t *ci)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_flexctr_info_get(unit,
                                                                        fib, ci);
}

int
mbcm_ltsw_l3_route_ipv4_mc_flexctr_object_set(int unit,
                                              bcmi_ltsw_l3_fib_t *fib,
                                              uint32_t value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_flexctr_object_set(unit,
                                                                          fib,
                                                                          value);
}

int
mbcm_ltsw_l3_route_ipv4_mc_flexctr_object_get(int unit,
                                              bcmi_ltsw_l3_fib_t *fib,
                                              uint32_t *value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv4_mc_flexctr_object_get(unit,
                                                                          fib,
                                                                          value);
}

int
mbcm_ltsw_l3_route_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_uc_add(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_uc_delete(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv6_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_uc_find(unit, lpm, fib);
}

int
mbcm_ltsw_l3_route_ipv6_uc_traverse(int unit, uint32_t flags,
                                    uint32_t start, uint32_t end,
                                    bcm_l3_route_traverse_cb cb,
                                    void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_uc_traverse(unit, flags,
                                                                start, end, cb,
                                                                user_data);
}

int
mbcm_ltsw_l3_route_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_add(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_delete(unit, fib);
}

int
mbcm_ltsw_l3_route_ipv6_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_find(unit, lpm, fib);
}

int
mbcm_ltsw_l3_route_ipv6_mc_flexctr_attach(int unit,
                                          bcmi_ltsw_l3_fib_t *fib,
                                          bcmi_ltsw_flexctr_counter_info_t *ci)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_flexctr_attach(unit,
                                                                      fib, ci);
}

int
mbcm_ltsw_l3_route_ipv6_mc_flexctr_detach(int unit,
                                          bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_flexctr_detach(unit,
                                                                      fib);
}

int
mbcm_ltsw_l3_route_ipv6_mc_flexctr_info_get(int unit,
                                        bcmi_ltsw_l3_fib_t *fib,
                                        bcmi_ltsw_flexctr_counter_info_t *ci)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_flexctr_info_get(unit,
                                                                        fib, ci);
}

int
mbcm_ltsw_l3_route_ipv6_mc_flexctr_object_set(int unit,
                                              bcmi_ltsw_l3_fib_t *fib,
                                              uint32_t value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_flexctr_object_set(unit,
                                                                          fib,
                                                                          value);
}

int
mbcm_ltsw_l3_route_ipv6_mc_flexctr_object_get(int unit,
                                              bcmi_ltsw_l3_fib_t *fib,
                                              uint32_t *value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_flexctr_object_get(unit,
                                                                          fib,
                                                                          value);
}

int
mbcm_ltsw_l3_route_ipv6_mc_traverse(int unit, uint32_t attrib,
                                    bcm_ipmc_traverse_cb trav_fn,
                                    bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                    void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_ipv6_mc_traverse(unit, attrib,
                                                                trav_fn, cb,
                                                                user_data);
}

int
mbcm_ltsw_l3_route_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_del_by_intf(unit, fib, negate);
}

int
mbcm_ltsw_l3_route_del_all(int unit, uint32_t attrib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_del_all(unit, attrib);
}

int
mbcm_ltsw_l3_route_age(int unit, bcm_l3_route_traverse_cb age_out,
                       void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_age(unit, age_out, user_data);

}

int
mbcm_ltsw_l3_vrf_flexctr_attach(int unit, bcm_vrf_t vrf,
                                bcmi_ltsw_flexctr_counter_info_t *info)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_vrf_flexctr_attach(unit, vrf, info);
}

int
mbcm_ltsw_l3_vrf_flexctr_detach(int unit, bcm_vrf_t vrf)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_vrf_flexctr_detach(unit, vrf);
}

int
mbcm_ltsw_l3_vrf_flexctr_info_get(int unit, bcm_vrf_t vrf,
                                  bcmi_ltsw_flexctr_counter_info_t *info)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_vrf_flexctr_info_get(unit, vrf, info);
}

int
mbcm_ltsw_l3_vrf_route_cnt_get(int unit, bcm_vrf_t vrf,
                               bool ipv6, uint32_t *cnt)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_vrf_route_cnt_get(unit, vrf, ipv6, cnt);
}

int
mbcm_ltsw_l3_nat_host_traverse(int unit, uint32_t flags,
                               bcm_l3_nat_ingress_traverse_cb trav_fn,
                               bcmi_ltsw_l3_fib_to_nat_cb cb,
                               void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_nat_host_traverse(unit, flags, trav_fn,
                                                           cb, user_data);
}

int
mbcm_ltsw_l3_route_flexctr_attach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_flexctr_attach(unit, fib, info);
}

int
mbcm_ltsw_l3_route_flexctr_detach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_flexctr_detach(unit, fib);
}

int
mbcm_ltsw_l3_route_flexctr_info_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_flexctr_info_get(unit, fib,
                                                                info);
}

int
mbcm_ltsw_l3_route_flexctr_object_set(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_flexctr_object_set(unit, fib,
                                                                  value);
}

int
mbcm_ltsw_l3_route_flexctr_object_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_flexctr_object_get(unit, fib,
                                                                  value);
}

int
mbcm_ltsw_l3_ipmc_flexctr_object_set(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_ipmc_flexctr_object_set(unit, fib,
                                                                 value);
}

int
mbcm_ltsw_l3_ipmc_flexctr_object_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_ipmc_flexctr_object_get(unit, fib,
                                                                 value);
}

int
mbcm_ltsw_l3_fib_info_get(
    int unit,
    bcmi_ltsw_l3_fib_info_t *info)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_info_get(unit, info);
}


int
mbcm_ltsw_l3_route_hit_clear(int unit)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_hit_clear(unit);
}

int
mbcm_ltsw_l3_host_hit_clear(int unit)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_hit_clear(unit);
}

int
mbcm_ltsw_l3_ipmc_age(int unit, uint32_t attrib,
                      bcm_ipmc_traverse_cb age_cb,
                      bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                      void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_ipmc_age(unit, attrib, age_cb, cb,
                                                  user_data);
}

int
mbcm_ltsw_l3_fib_tbl_db_get(int unit, bcmint_ltsw_l3_fib_tbl_db_t *tbl_db)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_tbl_db_get(unit, tbl_db);
}

int
mbcm_ltsw_l3_same_intf_drop_set(int unit, int enable)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_same_intf_drop_set(unit, enable);
}

int
mbcm_ltsw_l3_same_intf_drop_get(int unit, int *enable)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_same_intf_drop_get(unit, enable);
}

int
mbcm_ltsw_l3_alpm_control_set(int unit, int alpm_temp,
                              bcmi_ltsw_uft_mode_t uft_mode)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_alpm_control_set(unit, alpm_temp,
                                                          uft_mode);
}

int
mbcm_ltsw_l3_fib_aacl_add(
    int unit,
    uint32_t options,
    bcm_l3_aacl_t *aacl)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_aacl_add(unit, options, aacl);
}

int
mbcm_ltsw_l3_fib_aacl_delete(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_aacl_delete(unit, aacl);
}

int
mbcm_ltsw_l3_fib_aacl_delete_all(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_aacl_delete_all(unit, aacl);
}

int
mbcm_ltsw_l3_fib_aacl_find(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_aacl_find(unit, aacl);
}

int
mbcm_ltsw_l3_fib_aacl_traverse(
    int unit,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_aacl_traverse(unit, trav_fn,
                                                           user_data);
}

int
mbcm_ltsw_l3_fib_aacl_matched_traverse(
    int unit,
    uint32_t flags,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_aacl_matched_traverse(unit, flags,
                                                                   trav_fn,
                                                                   user_data);
}

int
mbcm_ltsw_l3_alpm_grp_prj_db_get(int unit, bcm_l3_route_group_t grp,
                                 int *prj_cnt, int *max_usage_lvl,
                                 int *max_usage)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_alpm_grp_prj_db_get(unit, grp,
                                                             prj_cnt,
                                                             max_usage_lvl,
                                                             max_usage);
}

int
mbcm_ltsw_l3_route_perf_v4_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint32 ip,
    uint32 ip_mask,
    int intf,
    int data_type,
    bool mtop)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_perf_v4_lt_op(unit, op,
                                                             vrf,
                                                             ip, ip_mask,
                                                             intf, data_type,
                                                             mtop);
}

int
mbcm_ltsw_l3_route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int data_type,
    bool mtop)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_route_perf_v6_lt_op(unit, op,
                                                             vrf,
                                                             v6, v6_mask,
                                                             intf, data_type,
                                                             mtop);
}

int
mbcm_ltsw_l3_fib_sw_dump(int unit)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_fib_sw_dump(unit);
}

int
mbcm_ltsw_l3_host_flexctr_attach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_flexctr_attach(unit, fib, info);
}

int
mbcm_ltsw_l3_host_flexctr_detach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_flexctr_detach(unit, fib);
}

int
mbcm_ltsw_l3_host_flexctr_info_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_flexctr_info_get(unit, fib,
                                                               info);
}

int
mbcm_ltsw_l3_host_flexctr_object_set(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_flexctr_object_set(unit, fib,
                                                                 value);
}

int
mbcm_ltsw_l3_host_flexctr_object_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value)
{
    return mbcm_ltsw_l3_fib_drv[unit].l3_host_flexctr_object_get(unit, fib,
                                                                 value);
}
