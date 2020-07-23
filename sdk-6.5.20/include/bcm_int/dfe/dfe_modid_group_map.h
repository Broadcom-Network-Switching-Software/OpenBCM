/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE MODID GROUP MAP H
 */
 
#ifndef _BCM_DFE_MODID_GROUP_MAP_H_
#define _BCM_DFE_MODID_GROUP_MAP_H_

#include <bcm/types.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>

#define DFE_MODID_GROUP_MAP_ROWS(unit) (SOC_DFE_MODID_GROUP_NOF(unit))

#define DFE_MODID_GROUP_IS_LEGAL(unit, group) (group < DFE_MODID_GROUP_MAP_ROWS(unit))
#define DFE_MODID_GROUP_INVALID -1

typedef  soc_dfe_modid_group_map_t _bcm_dfe_modid_group_map_t;

int bcm_dfe_modid_group_map_clear(int unit);
int bcm_dfe_modid_group_map_clear_group(int unit, bcm_module_t group);
int bcm_dfe_modid_group_map_add(int unit, bcm_module_t group, bcm_module_t module); 
int bcm_dfe_modid_group_map_get_list(int unit, bcm_module_t group, int modid_max_count, bcm_module_t *moidid_array, int *modid_count);
int bcm_dfe_modid_group_map_get_group(int unit, bcm_module_t module, bcm_module_t* group);


#endif /*_BCM_DFE_MODID_GROUP_MAP_H_*/
