/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 DRV H
 */
 
#ifndef _SOC_FE3200_DRV_H_
#define _SOC_FE3200_DRV_H_





#include <soc/error.h>
#include <soc/drv.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_defs.h>





#define SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_0        (0xffffffff)
#define SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_1        (0x03ffffff)
#define SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_2        (0x00000000)





int soc_fe3200_reset_device(int unit);
int soc_fe3200_drv_soc_properties_validate(int unit);
int soc_fe3200_drv_rings_map_set(int unit);
int soc_fe3200_drv_pll_config_set(int unit);
int soc_fe3200_drv_mdio_config_set(int unit);
int soc_fe3200_drv_pvt_monitor_enable(int unit);
int soc_fe3200_drv_mac_broadcast_id_get(int unit, int block_num,int *broadcast_id);
int soc_fe3200_bist_all(const int unit, const int skip_errors);
int soc_fe3200_drv_soft_init(int unit, uint32 soft_reset_mode_flags);
int soc_fe3200_drv_reg_access_only_reset(int unit);
int soc_fe3200_drv_sbus_broadcast_config(int unit);
int soc_fe3200_drv_blocks_reset(int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap);
int soc_fe3200_drv_sw_ver_set(int unit);
int soc_fe3200_drv_temperature_monitor_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count);
int soc_fe3200_drv_test_reg_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe3200_drv_test_reg_default_val_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe3200_drv_test_mem_filter(int unit, soc_mem_t reg, int *is_filtered);
int soc_fe3200_drv_test_brdc_blk_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe3200_drv_test_brdc_blk_info_get(int unit, int max_size, soc_reg_brdc_block_info_t *brdc_info, int *actual_size);
int soc_fe3200_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances);
int soc_fe3200_drv_asymmetrical_quad_get(int unit, int link, int *asymmetrical_quad);
int soc_fe3200_drv_fe13_isolate_set(int unit, soc_pbmp_t unisolated_links_pbmp, int isolate);
int soc_fe3200_drv_fe13_graceful_shutdown_set(int unit, soc_pbmp_t active_links, soc_pbmp_t unisolated_links, int shutdown);
int soc_fe3200_drv_graceful_shutdown_set(int unit, soc_pbmp_t active_links, int shutdown, soc_pbmp_t unisolated_links, int isolate_device);
int soc_fe3200_drv_mbist(int unit, int skip_errors);
int soc_fe3200_drv_block_pbmp_get(int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp);
int soc_fe3200_tbl_is_dynamic(int unit, soc_mem_t mem) ;
int soc_fe3200_drv_block_valid_get(int unit, int blktype, int blkid, char *valid);
int soc_fe3200_avs_value_get(int unit, uint32 *avs_val);







#endif 




