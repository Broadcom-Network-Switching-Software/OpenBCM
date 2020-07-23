/*! \file bcmptm_common_ser.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_COMMON_SER_H
#define BCMPTM_COMMON_SER_H

#include <bcmptm/bcmptm_ser_chip_internal.h>

typedef int
(*bcmptm_ser_mem_clr_list_get_f)(int unit,
                                 bcmptm_ser_mem_clr_list_t **mem_list,
                                 int *entry_num);

typedef int
(*bcmptm_ser_clear_mem_after_ser_enable_f)(int unit,
                                           bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                           int *reg_num);

typedef int
(*bcmptm_ser_clear_mem_before_ser_enable_f)(int unit,
                                            bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                            int *reg_num);

typedef int
(*bcmptm_ser_tcam_hw_scan_reg_info_get_f)(int unit, int enable,
                                          bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                          int *reg_num);

typedef int
(*bcmptm_ser_acc_type_map_info_get_f)(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **ser_acc_type_map,
                                      int *reg_num);

typedef int
(*bcmptm_ser_tcam_info_get_f)(int unit,
                              bcmptm_ser_tcam_generic_info_t **tcam_ser_info,
                              int *tcam_info_num);

typedef int
(*bcmptm_ser_mute_mem_list_get_f)(int unit,
                                  bcmdrd_sid_t **mem_ser_mute_list,
                                  int *mem_ser_mute_num);

typedef int
(*bcmptm_ser_blk_ctrl_regs_get_f)(int unit,
                                  bcmptm_ser_ctrl_en_t **ser_blks_ctrl_regs,
                                  int *ctrl_item_num);

typedef int
(*bcmptm_ser_intr_map_get_f)(int unit,
                             int *cdmac_inst,
                             bcmptm_ser_intr_route_info_t **intr_map,
                             int *intr_map_num);

typedef int
(*bcmptm_ser_blk_type_map_f)(int unit,
                             int blk_type, int *blk_type_overlay,
                             int *ser_blk_type);

typedef void
(*bcmptm_ser_intr_enable_f)(int unit, int sync, int intr_num, int enable);


typedef int
(*bcmptm_ser_intr_func_set_f)(int unit, unsigned int cmic_intr_num,
                              bcmbd_ser_intr_f intr_func,
                              uint32_t intr_param);

typedef int
(*bcmptm_ser_cmic_ser_engine_regs_get_f)(int unit,
                                         bcmdrd_sid_t *range_cfg_list,
                                         bcmdrd_sid_t *range_start_list,
                                         bcmdrd_sid_t *range_end_list,
                                         bcmdrd_sid_t *range_result_list,
                                         bcmdrd_sid_t **prot_list,
                                         bcmdrd_sid_t *range_addr_bits_list,
                                         bcmdrd_sid_t *range_disable_list,
                                         int *group_num,
                                         bcmdrd_fid_t  **range_cfg_fid_list,
                                         bcmdrd_fid_t  **prot_fid_list,
                                         bcmdrd_sid_t  *ser_memory_sid);

typedef int
(*bcmptm_ser_cmic_ser_engine_enable_f)(int unit, uint32_t ser_range_enable,
                                       bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                       int *reg_num, int *intr_num);

typedef int
(*bcmptm_ser_cmic_ser_result_get_f)(int unit,
                                    bcmptm_ser_ctrl_reg_info_t **ser_error_lst,
                                    bcmptm_ser_ctrl_reg_info_t **ser_result_0,
                                    bcmptm_ser_ctrl_reg_info_t **ser_result_1,
                                    int *err_num);

typedef int
(*bcmptm_ser_cmic_adext_get_f)(int unit, bcmptm_cmic_adext_info_t adext_info,
                               uint32_t *adext);

typedef int
(*bcmptm_ser_pt_acctype_get_f)(int unit, bcmdrd_sid_t sid, uint32_t *acc_type,
                               const char **acctype_str);

typedef bool
(*bcmptm_ser_pt_inst_remap_f)(int unit, bcmdrd_sid_t sid,
                              int *tbl_inst, int *tbl_copy);

typedef int
(*bcmptm_ser_cmic_schan_opcode_get_f)(int unit, int cmd, int dst_blk,
                                      int blk_types, int tbl_inst, int tbl_copy,
                                      int data_len, int dma, uint32_t ignore_mask,
                                      uint32_t *opcode);

typedef int
(*bcmptm_ser_hwmem_base_info_get_f)(int unit,
                                    int membase, int stage_id,
                                    char **bd_bb_nm, char **base_info, bool *mem);

typedef int
(*bcmptm_ser_ipep_hw_info_remap_f)(int unit, uint32_t address, int blk_type,
                                   uint32_t membase, uint32_t instruction,
                                   uint32_t *address_mapped, int *non_sbus);

typedef int
(*bcmptm_ser_mmu_mem_remap_f)(int unit, bcmptm_ser_correct_info_t *spci,
                              uint32_t adext, int blk_inst);

typedef int
(*bcmptm_ser_ipep_mem_remap_f)(int unit, bcmdrd_sid_t sid,
                               bcmdrd_sid_t *remap_sid);

typedef int
(*bcmptm_ser_data_split_mem_test_fid_get_f)(int unit, bcmdrd_sid_t sid,
                                            int acc_type, bcmdrd_fid_t *test_fid);

typedef int
(*bcmptm_ser_mem_ecc_info_get_f)(int unit, bcmdrd_sid_t sid_fv, int index_fv,
                                 bcmdrd_sid_t *sid, int *index, int *ecc_checking);

typedef int
(*bcmptm_ser_tcam_remap_f)(int unit, bcmdrd_sid_t tcam_sid,
                           bcmdrd_sid_t *remap_sid,
                           int *inject_err_to_key_fld);

typedef int
(*bcmptm_ser_tcam_sticky_bit_clr_get_f)(int unit, bcmdrd_sid_t tcam_sid,
                                        bcmdrd_sid_t *ctrl_sid,
                                        bcmdrd_fid_t *ctrl_fid);

typedef int
(*bcmptm_ser_refresh_regs_info_get_f)(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **mem_refresh_info,
                                      uint32_t *info_num);

typedef int
(*bcmptm_ser_status_reg_map_f)(int unit,
                               bcmptm_ser_correct_info_t *spci,
                               const bcmdrd_sid_t **corrected_sids,
                               uint32_t *index,
                               uint32_t *sid_num);

typedef int
(*bcmptm_ser_sram_ctrl_reg_get_f)(int unit, bcmdrd_sid_t sid, uint32_t index,
                                  bcmptm_ser_control_reg_info_t *sram_ctrl_reg_info);

typedef int
(*bcmptm_ser_xor_lp_mem_info_get_f)(int unit, bcmdrd_sid_t sid, uint32_t index,
                                    bcmptm_ser_sram_info_t *sram_info);

typedef int
(*bcmptm_ser_sram_scan_mem_skip_f)(int unit, bcmdrd_sid_t sid, bool *skip);

typedef int
(*bcmptm_ser_copy_to_cpu_regs_info_get_f)(int unit,
                            bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                            int *reg_num);

typedef int
(*bcmptm_ser_expected_value_get_f)(int unit, bcmdrd_sid_t sid,
                                   bcmdrd_fid_t fid, uint32_t *value);

typedef int
(*bcmptm_ser_test_num_bit_interleave_get_f)(int unit, bcmdrd_sid_t sid,
                                   bcmdrd_fid_t fid, uint32_t *num_interleave);

typedef int
(*bcmptm_ser_status_field_parse_f)(int unit, bcmdrd_sid_t sid, int fid_count,
                                   int *fval);
typedef int
(*bcmptm_ser_force_error_value_get_f)(int unit, bcmdrd_sid_t sid,
                                      bcmdrd_fid_t fid, uint32_t *value);

typedef int
(*bcmptm_ser_tcam_ctrl_reg_list_get_f)(int unit, bcmdrd_sid_t **mem_list,
                                       int *entry_num, bcmdrd_fid_t *fid);

/*!\brief Routines in chip specific code */
typedef struct bcmptm_ser_per_chip_data_driver_s {
    bcmptm_ser_mem_clr_list_get_f mem_clr_list_get;

    bcmptm_ser_clear_mem_after_ser_enable_f clear_mem_after_ser_enable;

    bcmptm_ser_clear_mem_before_ser_enable_f clear_mem_before_ser_enable;

    bcmptm_ser_tcam_hw_scan_reg_info_get_f tcam_ser_hw_scan_reg_info_get;

    bcmptm_ser_acc_type_map_info_get_f acc_type_map_info_get;

    bcmptm_ser_tcam_info_get_f tcam_cmic_ser_info_get;

    bcmptm_ser_mute_mem_list_get_f mute_mem_list_get;

    bcmptm_ser_mute_mem_list_get_f mute_ctrl_reg_list_get;

    bcmptm_ser_blk_ctrl_regs_get_f blk_ctrl_regs_get;

    bcmptm_ser_intr_map_get_f intr_map_get;

    bcmptm_ser_blk_type_map_f blk_type_map;

    bcmptm_ser_intr_enable_f ser_intr_enable;

    bcmptm_ser_intr_func_set_f intr_cb_func_set;

    bcmptm_ser_cmic_ser_engine_regs_get_f cmic_ser_engine_regs_get;

    bcmptm_ser_cmic_ser_engine_enable_f cmic_ser_engine_enable;

    bcmptm_ser_cmic_ser_result_get_f cmic_ser_result_get;

    bcmptm_ser_cmic_adext_get_f cmic_adext_get;

    bcmptm_ser_pt_acctype_get_f pt_acctype_get;

    bcmptm_ser_pt_inst_remap_f  pt_inst_remap;

    bcmptm_ser_cmic_schan_opcode_get_f cmic_schan_opcode_get;

    bcmptm_ser_hwmem_base_info_get_f hwmem_base_info_get;

    bcmptm_ser_ipep_hw_info_remap_f ip_ep_hw_info_remap;

    bcmptm_ser_mmu_mem_remap_f mmu_mem_remap;

    bcmptm_ser_ipep_mem_remap_f ip_ep_mem_remap;

    bcmptm_ser_data_split_mem_test_fid_get_f data_split_mem_test_fid_get;

    bcmptm_ser_mem_ecc_info_get_f mem_ecc_info_get;

    bcmptm_ser_tcam_remap_f tcam_remap;

    bcmptm_ser_tcam_sticky_bit_clr_get_f tcam_sticky_bit_clr_get;

    bcmptm_ser_refresh_regs_info_get_f refresh_regs_info_get;

    bcmptm_ser_status_reg_map_f ser_status_reg_translate;

    bcmptm_ser_sram_ctrl_reg_get_f sram_ctrl_reg_get;

    bcmptm_ser_xor_lp_mem_info_get_f xor_lp_mem_info_get;

    bcmptm_ser_sram_scan_mem_skip_f sram_scan_mem_skip;

    bcmptm_ser_copy_to_cpu_regs_info_get_f copy_to_cpu_regs_info_get;

    bcmptm_ser_expected_value_get_f expected_value_get;

    bcmptm_ser_test_num_bit_interleave_get_f num_bit_interleave_get;

    bcmptm_ser_status_field_parse_f ser_status_field_parse;

    bcmptm_ser_force_error_value_get_f force_error_value_get;

    bcmptm_ser_tcam_ctrl_reg_list_get_f tcam_ctrl_reg_list_get;
} bcmptm_ser_data_driver_t;

/*******************************************************************************
 * Public Functions
 */
/*!
* \brief Get data driver from chip specific code.
*
* \param [in] unit     Logical device id.
*
* \return Data driver routines.
*/
extern const bcmptm_ser_data_driver_t*
bcmptm_ser_drv_get(int unit);

#endif /* BCMPTM_COMMON_SER_H */
