/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF MODID GROUP MAP H
 */
 
#ifndef _BCM_DNXF_MODID_GROUP_MAP_H_
#define _BCM_DNXF_MODID_GROUP_MAP_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <bcm/types.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>

#define DNXF_MODID_GROUP_MAP_ROWS(unit) (SOC_DNXF_MODID_GROUP_NOF(unit))

#define DNXF_MODID_GROUP_IS_LEGAL(unit, group) (group < DNXF_MODID_GROUP_MAP_ROWS(unit))
#define DNXF_MODID_GROUP_INVALID -1

typedef  soc_dnxf_modid_group_map_t _bcm_dnxf_modid_group_map_t;

int bcm_dnxf_modid_group_map_clear(int unit);
int bcm_dnxf_modid_group_map_clear_group(int unit, bcm_module_t group);
int bcm_dnxf_modid_group_map_add(int unit, bcm_module_t group, bcm_module_t module); 
int bcm_dnxf_modid_group_map_get_list(int unit, bcm_module_t group, int modid_max_count, bcm_module_t *moidid_array, int *modid_count);
int bcm_dnxf_modid_group_map_get_group(int unit, bcm_module_t module, bcm_module_t* group);


#endif /*_BCM_DNXF_MODID_GROUP_MAP_H_*/
