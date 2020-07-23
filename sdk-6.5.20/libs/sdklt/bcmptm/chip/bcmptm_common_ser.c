/*! \file bcmptm_common_ser.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "bcmptm_common_ser.h"

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_mem_clr_list_get(int unit,
                            bcmptm_ser_mem_clr_list_t **mem_list,
                            int *entry_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->mem_clr_list_get(unit, mem_list, entry_num);
}

int
bcmptm_ser_clear_mem_after_ser_enable(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                      int *reg_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->clear_mem_after_ser_enable(unit, reg_ctrl_lst, reg_num);
}

int
bcmptm_ser_clear_mem_before_ser_enable(int unit,
                                       bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                       int *reg_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->clear_mem_before_ser_enable(unit, reg_ctrl_lst, reg_num);
}

int
bcmptm_ser_tcam_hw_scan_reg_info_get(int unit, int enable,
                                     bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                     int *reg_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->tcam_ser_hw_scan_reg_info_get(unit, enable, reg_ctrl_lst, reg_num);
}

int
bcmptm_ser_acc_type_map_info_get(int unit,
                                 bcmptm_ser_ctrl_reg_info_t **ser_acc_type_map,
                                 int *reg_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->acc_type_map_info_get == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->acc_type_map_info_get(unit, ser_acc_type_map, reg_num);
    }
}

int
bcmptm_ser_tcam_info_get(int unit,
                         bcmptm_ser_tcam_generic_info_t **tcam_ser_info,
                         int *tcam_info_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->tcam_cmic_ser_info_get == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->tcam_cmic_ser_info_get(unit, tcam_ser_info, tcam_info_num);
    }
}

int
bcmptm_ser_mute_mem_list_get(int unit,
                             bcmdrd_sid_t **mem_ser_mute_list,
                             int *mem_ser_mute_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->mute_mem_list_get(unit, mem_ser_mute_list, mem_ser_mute_num);
}

int
bcmptm_ser_mute_drop_pkt_reg_list_get(int unit,
                                      bcmdrd_sid_t **ctrl_reg_mute_list,
                                      int *ctrl_reg_mute_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->mute_ctrl_reg_list_get == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->mute_ctrl_reg_list_get(unit, ctrl_reg_mute_list, ctrl_reg_mute_num);
    }
}

int
bcmptm_ser_blk_ctrl_regs_get(int unit,
                             bcmptm_ser_ctrl_en_t **ser_blks_ctrl_regs,
                             int *ctrl_item_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->blk_ctrl_regs_get(unit, ser_blks_ctrl_regs, ctrl_item_num);
}

int
bcmptm_ser_intr_map_get(int unit,
                        int *mac_inst,
                        bcmptm_ser_intr_route_info_t **intr_map,
                        int *intr_map_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->intr_map_get(unit, mac_inst, intr_map, intr_map_num);
}

int
bcmptm_ser_blk_type_map(int unit, int blk_type, int *blk_type_overlay,
                        int *ser_blk_type)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->blk_type_map(unit, blk_type, blk_type_overlay, ser_blk_type);
}

void
bcmptm_ser_lp_intr_enable(int unit, int sync, int intr_num, int enable)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    sd->ser_intr_enable(unit, sync, intr_num, enable);
    return;
}

int
bcmptm_ser_lp_intr_func_set(int unit, unsigned int intr_num,
                         bcmbd_ser_intr_f intr_func, uint32_t intr_param)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->intr_cb_func_set(unit, intr_num, intr_func, intr_param);
}

int
bcmptm_ser_cmic_ser_engine_regs_get(int unit,
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
                                    bcmdrd_sid_t  *ser_memory_sid)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->cmic_ser_engine_regs_get == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->cmic_ser_engine_regs_get(unit,
                                            range_cfg_list,
                                            range_start_list,
                                            range_end_list,
                                            range_result_list,
                                            prot_list,
                                            range_addr_bits_list,
                                            range_disable_list,
                                            group_num,
                                            range_cfg_fid_list,
                                            prot_fid_list,
                                            ser_memory_sid);
    }
}

int
bcmptm_ser_cmic_ser_engine_enable(int unit, uint32_t ser_range_enable,
                                  bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                  int *reg_num, int *intr_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->cmic_ser_engine_enable == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->cmic_ser_engine_enable(unit, ser_range_enable,
                                          reg_ctrl_lst, reg_num, intr_num);
    }
}

int
bcmptm_ser_cmic_ser_result_get(int unit,
                               bcmptm_ser_ctrl_reg_info_t **ser_error_lst,
                               bcmptm_ser_ctrl_reg_info_t **ser_result_0,
                               bcmptm_ser_ctrl_reg_info_t **ser_result_1,
                               int *err_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->cmic_ser_result_get == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->cmic_ser_result_get(unit, ser_error_lst, ser_result_0,
                                       ser_result_1, err_num);
    }
}

int
bcmptm_ser_cmic_adext_get(int unit, bcmptm_cmic_adext_info_t adext_info,
                          uint32_t *adext)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->cmic_adext_get(unit, adext_info, adext);
}

int
bcmptm_ser_pt_acctype_get(int unit, bcmdrd_sid_t sid, uint32_t *acc_type,
                          const char **acctype_str)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->pt_acctype_get(unit, sid, acc_type, acctype_str);
}

bool
bcmptm_ser_pt_inst_remap(int unit, bcmdrd_sid_t sid,
                         int *tbl_inst, int *tbl_copy)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->pt_inst_remap(unit, sid, tbl_inst, tbl_copy);
}

int
bcmptm_ser_cmic_schan_opcode_get(int unit, int cmd, int dst_blk,
                                 int blk_types, int tbl_inst, int tbl_copy,
                                 int data_len, int dma, uint32_t ignore_mask,
                                 uint32_t *opcode)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->cmic_schan_opcode_get(unit, cmd, dst_blk, blk_types,
                                     tbl_inst, tbl_copy, data_len, dma,
                                     ignore_mask, opcode);
}

int
bcmptm_ser_hwmem_base_info_get(int unit,
                               int membase, int stage_id,
                               char **bd_bb_nm, char **base_info, bool *mem)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->hwmem_base_info_get(unit, membase, stage_id,
                                   bd_bb_nm, base_info, mem);
}

int
bcmptm_ser_ipep_hw_info_remap(int unit, uint32_t address, int blk_type,
                              uint32_t membase, uint32_t instruction,
                              uint32_t *address_mapped, int *non_sbus)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->ip_ep_hw_info_remap(unit, address, blk_type,
                                   membase, instruction,
                                   address_mapped, non_sbus);
}

int
bcmptm_ser_mmu_mem_remap(int unit, bcmptm_ser_correct_info_t *spci,
                         uint32_t adext, int blk_inst)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->mmu_mem_remap(unit, spci, adext, blk_inst);
}

int
bcmptm_ser_ipep_mem_remap(int unit, bcmdrd_sid_t sid,
                          bcmdrd_sid_t *remap_sid)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->ip_ep_mem_remap(unit, sid, remap_sid);
}

int
bcmptm_ser_data_split_mem_test_fid_get(int unit, bcmdrd_sid_t sid,
                                       int acc_type, bcmdrd_fid_t *test_fid)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->data_split_mem_test_fid_get == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->data_split_mem_test_fid_get(unit, sid, acc_type, test_fid);
    }
}

int
bcmptm_ser_mem_ecc_info_get(int unit, bcmdrd_sid_t sid_fv, int index_fv,
                            bcmdrd_sid_t *sid, int *index, int *ecc_checking)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->mem_ecc_info_get(unit, sid_fv, index_fv,
                                sid, index, ecc_checking);
}

int
bcmptm_ser_tcam_remap(int unit, bcmdrd_sid_t tcam_sid,
                      bcmdrd_sid_t *remap_sid,
                      int *inject_err_to_key_fld)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->tcam_remap(unit, tcam_sid, remap_sid, inject_err_to_key_fld);
}

int
bcmptm_ser_tcam_sticky_bit_clr_get(int unit, bcmdrd_sid_t tcam_sid,
                                   bcmdrd_sid_t *ctrl_sid,
                                   bcmdrd_fid_t *ctrl_fid)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->tcam_sticky_bit_clr_get == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->tcam_sticky_bit_clr_get(unit, tcam_sid, ctrl_sid, ctrl_fid);
    }
}

int
bcmptm_ser_refresh_regs_info_get(int unit,
                                 bcmptm_ser_ctrl_reg_info_t **mem_refresh_info,
                                 uint32_t *info_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->refresh_regs_info_get(unit, mem_refresh_info, info_num);
}

int
bcmptm_ser_status_reg_map(int unit,
                          bcmptm_ser_correct_info_t *spci,
                          const bcmdrd_sid_t **corrected_sids,
                          uint32_t *index, uint32_t *sid_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->ser_status_reg_translate(unit, spci, corrected_sids,
                                        index, sid_num);
}

int
bcmptm_ser_sram_ctrl_reg_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                             bcmptm_ser_control_reg_info_t *sram_ctrl_reg_info)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->sram_ctrl_reg_get(unit, sid, index, sram_ctrl_reg_info);
}

int
bcmptm_ser_xor_lp_mem_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                               bcmptm_ser_sram_info_t *sram_info)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->xor_lp_mem_info_get(unit, sid, index, sram_info);
}

int
bcmptm_ser_sram_scan_mem_skip(int unit, bcmdrd_sid_t sid, bool *skip)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->sram_scan_mem_skip(unit, sid, skip);
}

int
bcmptm_ser_copy_to_cpu_regs_info_get(int unit,
                                     bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                     int *reg_num)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->copy_to_cpu_regs_info_get == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return sd->copy_to_cpu_regs_info_get(unit, reg_ctrl_lst, reg_num);
    }
}

int
bcmptm_ser_expected_value_get(int unit, bcmdrd_sid_t sid,
                              bcmdrd_fid_t fid, uint32_t *value)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->expected_value_get(unit, sid, fid, value);
}

int
bcmptm_ser_test_num_interleave_get(int unit, bcmdrd_sid_t sid,
                         bcmdrd_fid_t fid, uint32_t *num_interleave)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->num_bit_interleave_get(unit, sid, fid, num_interleave);
}

int
bcmptm_ser_status_field_parse(int unit, bcmdrd_sid_t sid,
                              int fid_count, int *fval)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->ser_status_field_parse(unit, sid, fid_count, fval);
}

int
bcmptm_ser_force_error_value_get(int unit, bcmdrd_sid_t sid,
                                 bcmdrd_fid_t fid, uint32_t *value)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    return sd->force_error_value_get(unit, sid, fid, value);
}

int
bcmptm_ser_tcam_ctrl_reg_list_get(int unit, bcmdrd_sid_t **mem_list,
                                  int *entry_num, bcmdrd_fid_t *fid)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd->tcam_ctrl_reg_list_get == NULL) {
        return SHR_E_NONE;
    } else {
        return sd->tcam_ctrl_reg_list_get(unit, mem_list, entry_num, fid);
    }
}

bool
bcmptm_ser_drv_populated(int unit)
{
    const bcmptm_ser_data_driver_t *sd = bcmptm_ser_drv_get(unit);

    if (sd == NULL) {
        return FALSE;
    } else {
        return TRUE;
    }
}

