/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 DRV H
 */
 
#ifndef _SOC_FE1600_DRV_H_
#define _SOC_FE1600_DRV_H_

int soc_fe1600_interrupt_init(int unit);
int soc_fe1600_interrupt_deinit(int unit);
int soc_fe1600_tbl_is_dynamic(int unit, soc_mem_t mem);
int soc_fe1600_ser_init(int unit);
int soc_fe1600_reset_device(int unit);
int soc_fe1600_drv_soft_init(int unit, uint32 soft_reset_mode_flags);
int soc_fe1600_drv_blocks_reset(int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap);
int soc_fe1600_TDM_fragment_validate(int unit, uint32 tdm_frag);
int soc_fe1600_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances);
int soc_fe1600_avs_value_get(int unit, uint32 *avs_val);
int soc_fe1600_drv_graceful_shutdown_set(int unit, soc_pbmp_t active_links, int shutdown, soc_pbmp_t unisolated_links, int isolate_device);
int soc_fe1600_drv_fe13_graceful_shutdown_set(int unit, soc_pbmp_t active_links,  soc_pbmp_t unisolated_links, int shutdown);
int soc_fe1600_drv_fe13_isolate_set(int unit, soc_pbmp_t unisolated_links, int isolate);
int soc_fe1600_drv_soc_properties_validate(int unit);
int soc_fe1600_drv_temperature_monitor_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count);
int soc_fe1600_set_mesh_topology_config(int unit);

int soc_fe1600_drv_sw_ver_set(int unit);
int soc_fe1600_drv_test_reg_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe1600_drv_test_reg_default_val_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe1600_drv_test_mem_filter(int unit, soc_mem_t mem, int *is_filtered);
int soc_fe1600_drv_test_brdc_blk_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe1600_drv_test_brdc_blk_info_get(int unit, int max_size, soc_reg_brdc_block_info_t *brdc_info, int *actual_size);
int soc_fe1600_drv_link_to_block_mapping(int unit, int link, int* block_id,int* inner_link,int block_type);
int soc_fe1600_drv_block_pbmp_get(int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp);
int soc_fe1600_drv_block_valid_get(int unit, int blktype, int blkid, char *valid);


int soc_fe1600_drv_mbist(int unit, int skip_errors);
int soc_bist_all_fe1600(const int unit, const int skip_errors);
int soc_bist_all_fe1600_bcm88754(const int unit, const int skip_errors);
int soc_bist_fe1600_ser_test(const int unit, const int skip_errors, uint32 nof_repeats, uint32 time_to_wait);

#ifdef BCM_88750_B0
int soc_fe1600_bo_feature_fixes_enable(int unit, int enable);
#endif


int soc_fe1600_reduced_support_set(int unit);

#ifdef BCM_88754_A0

int soc_fe1600_drv_bcm88754_async_fifo_set(int unit, int fmac_index, int lane, soc_field_t field, uint32 data);
int soc_fe1600_drv_bcm88754_async_fifo_get(int unit, int fmac_index, int lane, soc_field_t field, uint32 *data);
#endif
#endif 
