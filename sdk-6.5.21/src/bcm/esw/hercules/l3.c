/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	l3.c
 * Purpose: 	Tracks and manages l3 routing tables and interfaces.
 */

#ifdef INCLUDE_L3

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/port.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>

int
bcm_hercules_l3_tables_init(int unit)
{    
    return BCM_E_NONE;
}

int
bcm_hercules_l3_tables_cleanup(int unit)
{    
    return BCM_E_NONE;
}

int
bcm_hercules_l3_enable(int unit, int enable)
{
    return BCM_E_NONE;
}

int
bcm_hercules_l3_intf_get(int unit, _bcm_l3_intf_cfg_t *intf_info)
{    
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_intf_get_by_vid(int unit, _bcm_l3_intf_cfg_t *intf_info)
{    
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_intf_create(int unit, _bcm_l3_intf_cfg_t *intf_info)
{    
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_intf_id_create(int unit, _bcm_l3_intf_cfg_t *intf_info)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_intf_lookup(int unit, _bcm_l3_intf_cfg_t *l3i)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_intf_del(int unit, _bcm_l3_intf_cfg_t *intf_info)
{    
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_intf_del_all(int unit)
{    
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_get(int unit, int flag, _bcm_l3_cfg_t *l3cfg)
{    
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_add(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_del_prefix(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_del_intf(int unit, _bcm_l3_cfg_t *l3cfg, int negate)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_del_all(int unit)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_replace(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_conflict_get(int unit, bcm_l3_key_t *ipkey,
			     bcm_l3_key_t *cf_array, int cf_max, int *cf_count)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_defip_cfg_get(int unit, _bcm_defip_cfg_t *defip)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_defip_ecmp_get_all(int unit, _bcm_defip_cfg_t *defip,
           bcm_l3_route_t *path_array, int max_path, int *path_count)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_defip_add(int unit, _bcm_defip_cfg_t *defip)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_defip_del(int unit, _bcm_defip_cfg_t *defip)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_defip_del_intf(int unit, _bcm_defip_cfg_t *defip, int negate)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_defip_del_all(int unit)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_age(int unit, uint32 flags, bcm_l3_host_traverse_cb age_out,
                    void *user_data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_traverse(int unit, uint32 flags, uint32 start,
                         uint32 end, bcm_l3_host_traverse_cb cb,
                         void *user_data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_info(int unit, bcm_l3_info_t *l3info)
{
    l3info->l3info_max_intf = 0;
    l3info->l3info_max_host = 0;
    l3info->l3info_max_route = 0;
    l3info->l3info_occupied_intf = 0;
    l3info->l3info_occupied_host = 0;
    l3info->l3info_occupied_route = 0;
    l3info->l3info_max_lpm_block = 0;
    l3info->l3info_used_lpm_block = 0;

    /* Superseded values for backward compatibility */

    l3info->l3info_max_l3         = l3info->l3info_max_host;
    l3info->l3info_max_defip      = l3info->l3info_max_route;
    l3info->l3info_occupied_l3    = l3info->l3info_occupied_host;
    l3info->l3info_occupied_defip = l3info->l3info_occupied_route;

    return (BCM_E_NONE);
}

int
bcm_hercules_defip_age(int unit, bcm_l3_route_traverse_cb age_out,
                       void *user_data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_defip_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_route_traverse_cb trav_fn, void *user_data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l3_ip6_get(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_l3_ip6_add(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_l3_ip6_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_l3_ip6_del_prefix(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_l3_ip6_replace(int unit, _bcm_l3_cfg_t *l3cfg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_l3_ip6_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_host_traverse_cb cb, void *user_data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_ip6_defip_cfg_get(int unit, _bcm_defip_cfg_t *defip)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ip6_defip_ecmp_get_all(int unit, _bcm_defip_cfg_t *defip,
           bcm_l3_route_t *path_array, int max_path, int *path_count)
{ 
    return (BCM_E_UNAVAIL);
} 


int
bcm_hercules_ip6_defip_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_route_traverse_cb trav_fn, void *user_data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_ip6_defip_add(int unit, _bcm_defip_cfg_t *defip)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ip6_defip_del(int unit, _bcm_defip_cfg_t *defip)
{
    return BCM_E_UNAVAIL;
}

#endif	/* INCLUDE_L3 */

typedef int _bcm_esw_hercules_l3_not_empty; /* Make ISO compilers happy. */
