/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON DRV H
 */
 
#ifndef _SOC_RAMON_DRV_H_
#define _SOC_RAMON_DRV_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif





#include <soc/error.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_defs.h>





#define SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_0        (0x00000fff)
#define SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_1        (0x00000000)
#define SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_2        (0xfff00000)
#define SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_3        (0x00ffffff)





int soc_ramon_reset_device(int unit);
int soc_ramon_drv_rings_map_set(int unit);
int soc_ramon_drv_pll_config_set(int unit);
int soc_ramon_drv_pvt_monitor_enable(int unit);
int soc_ramon_drv_mac_broadcast_id_get(int unit, int block_num,int *broadcast_id);
int soc_ramon_bist_all(const int unit, const int skip_errors);
int soc_ramon_drv_soft_init(int unit, uint32 soft_reset_mode_flags);
int soc_ramon_drv_reg_access_only_reset(int unit);
int soc_ramon_drv_sbus_broadcast_config(int unit);
int soc_ramon_drv_blocks_reset(int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap);
int soc_ramon_drv_temperature_monitor_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count);
int soc_ramon_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances);
int soc_ramon_drv_mbist(int unit, int skip_errors);
int soc_ramon_drv_block_pbmp_get(int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp);
int soc_ramon_tbl_is_dynamic(int unit, soc_mem_t mem) ;
int soc_ramon_drv_block_valid_get(int unit, int blktype, int blkid, char *valid);
int soc_ramon_drv_link_to_block_mapping(int unit, int link, int* block_id,int* inner_link,int block_type);
int soc_ramon_set_mesh_topology_config(int unit);
int soc_bist_all_ramon(int unit, int skip_errors);







#endif 




