/*! \file bcmcth_ctr_eflex_drv.c
 *
 * BCMCTH Enhanced Flex Counter Driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>

#include <bcmcth/bcmcth_ctr_eflex_drv.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type. */
typedef bcmcth_ctr_eflex_drv_t *(*bcmcth_ctr_eflex_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,\
                             _cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_ctr_eflex_drv_get },
static struct {
    bcmcth_ctr_eflex_drv_get_f drv_get;
} ctr_eflex_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/* Array of device variant specific dispatch for egress cmd bus. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_ctr_egr_eflex_cmd_bus_lts_bit_offset_get },
static struct {
    ctr_egr_eflex_cmd_bus_lts_bit_offset_get_f bit_offset_get;
} ctr_egr_eflex_cmd_bus_lts_bit_offset_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

/* Array of device variant specific dispatch for egress obj bus. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_ctr_egr_eflex_obj_bus_bit_offset_get },
static struct {
    ctr_egr_eflex_obj_bus_bit_offset_get_f bit_offset_get;
} ctr_egr_eflex_obj_bus_bit_offset_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

/* Array of device variant specific dispatch for ingress obj bus. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_ctr_ing_eflex_obj_bus_bit_offset_get },
static struct {
    ctr_ing_eflex_obj_bus_bit_offset_get_f bit_offset_get;
} ctr_ing_eflex_obj_bus_bit_offset_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

static bcmcth_ctr_eflex_drv_t *ctr_eflex_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmcth_ctr_eflex_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    ctr_eflex_drv[unit] = ctr_eflex_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_drv_cleanup(int unit)
{
    if (ctr_eflex_drv[unit]) {
        ctr_eflex_drv[unit] = NULL;
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_device_info_get(int unit,
                                 bcmcth_ctr_eflex_device_info_t **device_info)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->device_info_get_fn) {
        return ctr_eflex_drv[unit]->device_info_get_fn(unit, device_info);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_ctr_table_get(int unit,
                               bool ingress,
                               bcmdrd_sid_t **ctr_table_list,
                               uint32_t *num)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->ctr_table_get_fn) {
        return ctr_eflex_drv[unit]->ctr_table_get_fn(unit,
                                                     ingress,
                                                     ctr_table_list,
                                                     num);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_action_reg_set(int unit,
                                bool enable,
                                ctr_eflex_action_profile_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->action_reg_set_fn) {
        return ctr_eflex_drv[unit]->action_reg_set_fn(unit,
                                                      enable,
                                                      entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_action_table_write(int unit,
                                    ctr_eflex_action_profile_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->action_table_write_fn) {
        return ctr_eflex_drv[unit]->action_table_write_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_bitp_profile_set(int unit)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->bitp_profile_set_fn) {
        return ctr_eflex_drv[unit]->bitp_profile_set_fn(unit);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_op_profile_write(int unit,
                                  ctr_eflex_op_profile_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->op_profile_write_fn) {
        return ctr_eflex_drv[unit]->op_profile_write_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_range_chk_write(int unit,
                                  ctr_eflex_range_chk_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->range_chk_write_fn) {
        return ctr_eflex_drv[unit]->range_chk_write_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_trigger_write(int unit,
                               ctr_eflex_trigger_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->trigger_write_fn) {
        return ctr_eflex_drv[unit]->trigger_write_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_trigger_read(int unit,
                              ctr_eflex_trigger_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->trigger_read_fn) {
        return ctr_eflex_drv[unit]->trigger_read_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_group_action_write(int unit,
                                    ctr_eflex_group_action_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->group_action_write_fn) {
        return ctr_eflex_drv[unit]->group_action_write_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_obj_id_get(int unit, int select, uint64_t data)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->object_id_get_fn) {
        return ctr_eflex_drv[unit]->object_id_get_fn(unit, select, data);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_logical_obj_id_get(int unit, int select, uint64_t data)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->logical_object_id_get_fn) {
        return ctr_eflex_drv[unit]->logical_object_id_get_fn(unit,
                                                             select,
                                                             data);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_obj_idx_get(int unit, int select, int data)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->object_idx_get_fn) {
        return ctr_eflex_drv[unit]->object_idx_get_fn(unit, select, data);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_action_zone_get(int unit, int select, uint32_t data)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->action_zone_get_fn) {
        return ctr_eflex_drv[unit]->action_zone_get_fn(unit, select, data);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_error_stats_write(int unit,
                                   ctr_eflex_error_stats_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->error_stats_write_fn) {
        return ctr_eflex_drv[unit]->error_stats_write_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_error_stats_read(int unit,
                                  ctr_eflex_error_stats_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->error_stats_read_fn) {
        return ctr_eflex_drv[unit]->error_stats_read_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_pipe_valid_check(int unit,
                                  bool ingress,
                                  uint32_t pipe,
                                  bool *valid)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->pipe_valid_check_fn) {
        return ctr_eflex_drv[unit]->pipe_valid_check_fn(unit,
                                                        ingress,
                                                        pipe,
                                                        valid);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_update_control_get(
    int unit,
    bool ingress,
    bcmdrd_sid_t **update_control_list,
    uint32_t *num,
    bcmdrd_fid_t *counter_pool_enable_fid)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->update_control_get_fn) {
        return ctr_eflex_drv[unit]->update_control_get_fn(
            unit,
            ingress,
            update_control_list,
            num,
            counter_pool_enable_fid);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_hitbit_sid_map_init(int unit, bool warm)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->hitbit_sid_map_init_fn) {
        return ctr_eflex_drv[unit]->hitbit_sid_map_init_fn(unit, warm);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_hitbit_sid_map_deinit(int unit)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->hitbit_sid_map_deinit_fn) {
        return ctr_eflex_drv[unit]->hitbit_sid_map_deinit_fn(unit);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_shadow_pool_set(int unit,
                                 bool enable,
                                 ctr_eflex_action_profile_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->shadow_pool_set_fn) {
        return ctr_eflex_drv[unit]->shadow_pool_set_fn(unit, enable, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_timestamp_init(int unit)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->timestamp_init_fn) {
        return ctr_eflex_drv[unit]->timestamp_init_fn(unit);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_obj_quant_write(int unit,
                                 ctr_eflex_obj_quant_data_t *entry)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->obj_quant_write_fn) {
        return ctr_eflex_drv[unit]->obj_quant_write_fn(unit, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_obj_quant_imm_reg(int unit, bool *imm_reg)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->obj_quant_imm_reg_fn) {
        return ctr_eflex_drv[unit]->obj_quant_imm_reg_fn(unit, imm_reg);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_egr_eflex_cmd_bus_lts_bit_offset_get(int unit,
                             int index,
                             bcmcth_ctr_eflex_obj_bit_offset_t *bit_offset)
{
    bcmlrd_variant_t variant;
    const bcmcth_ctr_eflex_obj_bit_offset_t *offset;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    offset =
        ctr_egr_eflex_cmd_bus_lts_bit_offset_get[variant].bit_offset_get(index);
    SHR_NULL_CHECK(offset, SHR_E_PARAM);
    bit_offset->minbit = offset->minbit;
    bit_offset->maxbit = offset->maxbit;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_egr_eflex_bus_bit_offset_get(int unit,
                             int index,
                             bcmcth_ctr_eflex_obj_bit_offset_t *bit_offset)
{
    bcmlrd_variant_t variant;
    const bcmcth_ctr_eflex_obj_bit_offset_t *offset;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    offset =
        ctr_egr_eflex_obj_bus_bit_offset_get[variant].bit_offset_get(index);
    SHR_NULL_CHECK(offset, SHR_E_PARAM);
    bit_offset->minbit = offset->minbit;
    bit_offset->maxbit = offset->maxbit;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_ing_eflex_bus_bit_offset_get(int unit,
                             int index,
                             bcmcth_ctr_eflex_obj_bit_offset_t *bit_offset)
{
    bcmlrd_variant_t variant;
    const bcmcth_ctr_eflex_obj_bit_offset_t *offset;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    offset =
        ctr_ing_eflex_obj_bus_bit_offset_get[variant].bit_offset_get(index);
    SHR_NULL_CHECK(offset, SHR_E_PARAM);
    bit_offset->minbit = offset->minbit;
    bit_offset->maxbit = offset->maxbit;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_obj_info_get(int unit,
                              bool ingress,
                              uint32_t *num_obj,
                              uint32_t *obj)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->obj_info_get_fn) {
        return ctr_eflex_drv[unit]->obj_info_get_fn(unit,
                                                    ingress,
                                                    num_obj,
                                                    obj);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_hitbit_ctrl_info_add(
    int unit,
    bool notify,
    uint32_t skip_index,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_group_action_data_t *grp_action_entry,
    ctr_eflex_hitbit_lt_info_t **hitbit_lt_info)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->hitbit_ctrl_info_add_fn) {
        return ctr_eflex_drv[unit]->hitbit_ctrl_info_add_fn(unit,
                                                            notify,
                                                            skip_index,
                                                            ctrl,
                                                            entry,
                                                            grp_action_entry,
                                                            hitbit_lt_info);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_hitbit_ctrl_skip_idx_get(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_hitbit_ctrl_data_t *cur_entry,
    uint32_t *skip_index)
{
    if (ctr_eflex_drv[unit] &&
        ctr_eflex_drv[unit]->hitbit_ctrl_skip_idx_get_fn) {
        return ctr_eflex_drv[unit]->hitbit_ctrl_skip_idx_get_fn(unit,
                                                                entry,
                                                                cur_entry,
                                                                skip_index);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_hitbit_ctrl_lookup_help(int unit,
                                         ctr_eflex_hitbit_ctrl_data_t *entry)

{
    if (ctr_eflex_drv[unit] &&
        ctr_eflex_drv[unit]->hitbit_ctrl_skip_idx_get_fn) {
        return ctr_eflex_drv[unit]->hitbit_ctrl_lookup_help_fn(unit,
                                                               entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_ctr_eflex_hash_entry_index_calc(
    int unit,
    bcmltd_sid_t ltid,
    bcmdrd_sid_t sid,
    bcmptm_rm_hash_hit_context_t *hit_context,
    int tbl_inst,
    uint32_t bkt_num,
    uint32_t bkt_entry,
    void *context,
    uint32_t *entry_idx)
{
    if (ctr_eflex_drv[unit] && ctr_eflex_drv[unit]->hash_entry_index_calc_fn) {
        return ctr_eflex_drv[unit]->hash_entry_index_calc_fn(unit,
                                                             ltid,
                                                             sid,
                                                             hit_context,
                                                             tbl_inst,
                                                             bkt_num,
                                                             bkt_entry,
                                                             context,
                                                             entry_idx);
    }

    return SHR_E_NONE;
}
