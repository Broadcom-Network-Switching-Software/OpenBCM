/*! \file bcmptm_common.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_COMMON_H
#define BCMPTM_COMMON_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_ctr_flex_internal.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcmptm/bcmptm_uft_internal.h>
#include <bcmptm/bcmptm_sbr_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>

#include "bcmptm_common_ser.h"

/*******************************************************************************
 * Defines
 */
#define BCMPTM_CCI_REG_ADDR_GAP_DEFAULT     (8)

/*******************************************************************************
 * Typedefs
 */
typedef int(*fn_rsp_info_get_t)(int unit,
                                bcmptm_op_type_t,
                                bcmbd_tbl_resp_info_t *,
                                bcmptm_rsp_info_t *); /* out */

typedef int(*fn_wal_info_get_t)(int unit,
                                bcmptm_wal_info_t *); /* out */

typedef int(*fn_dev_info_get_t)(int unit,
                                const bcmptm_dev_info_t **); /* out */

typedef int(*fn_pt_attr_is_alpm_t)(int unit, bcmdrd_sid_t sid, bool *is_alpm);

typedef int(*fn_pt_overlay_info_get_t)(int unit, bcmdrd_sid_t sid,
                                       const bcmptm_overlay_info_t **oinfo);

typedef int(*fn_pt_overlay_mult_info_get_t)(int unit,
                                            bcmdrd_sid_t n_sid,
                                            bcmdrd_sid_t w_sid,
                                            const bcmptm_overlay_info_t **oinfo);

typedef int(*fn_pt_wo_aggr_info_get_t)(int unit, bcmdrd_sid_t sid,
                                       bcmptm_wo_aggr_info_t *wo_aggr_info);

typedef int(*fn_pt_tcam_km_format_t)(int unit, bcmdrd_sid_t sid,
                                     bcmptm_pt_tcam_km_format_t *km_format);

typedef int(*fn_hw_does_km_to_xy_t)(int unit, bool *hw_does_km_to_xy);

typedef int(*fn_tcam_corrupt_bits_enable_t)(int unit, bool *tc_en);

typedef int(*fn_pt_needs_ltid_t)(int unit, bcmdrd_sid_t sid, bool *ltid_en);

typedef int(*fn_pt_needs_dfid_t)(int unit, bcmdrd_sid_t sid, uint8_t *dfid_bytes);

typedef int(*fn_pt_in_must_cache_list_t)(int unit, bcmdrd_sid_t sid,
                                         bool *must_cache);

typedef int(*fn_pt_in_dont_cache_list_t)(int unit, bcmdrd_sid_t sid,
                                         bool *dont_cache);

typedef int (*fn_pt_group_index_count_set_t) (int unit,
                                              uint32_t group_id,
                                              bcmptm_index_count_set_info_t
                                              *index_count_set_info,
                                              uint8_t *bank_count,
                                              uint8_t *start_bank_enum);

typedef int (*fn_pt_index_max_get_t) (int unit, bcmdrd_sid_t sid, bool for_ser);

typedef int (*fn_pt_index_min_get_t) (int unit, bcmdrd_sid_t sid, bool for_ser);

typedef int (*fn_pt_group_info_get_t) (int unit, uint32_t group_id,
                                       uint32_t *pt_count, bool *bank_start_en,
                                       const bcmdrd_sid_t **pt_list);

typedef int(*fn_pt_rm_tcam_memreg_oper_t)(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_sid_type_t sid_type,
                              bcmptm_rm_tcam_sid_oper_t sid_oper,
                              bcmptm_rm_tcam_sid_info_t *sid_info);

typedef int(*fn_pt_rm_tcam_prio_only_hw_key_info_init_t)(int unit,
                            bcmltd_sid_t ltid,
                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                            uint8_t num_entries_per_index,
                            void *hw_field_list);

typedef int (*fn_alpm_driver_register_t) (int unit, bcmptm_rm_alpm_driver_t *drv);
typedef int (*fn_cth_alpm_driver_register_t) (int unit, bcmptm_cth_alpm_driver_t *drv);

typedef const bcmptm_ser_data_driver_t* (*fn_ser_data_driver_register_t) (int unit);

typedef const bcmptm_ctr_flex_data_driver_t* (*fn_ctr_flex_data_driver_register_t) (int unit);

typedef int (*fn_bcmlrd_lt_mreq_info_init_t) (int unit);
typedef int (*fn_bcmlrd_lt_mreq_info_cleanup_t) (int unit);
typedef int (*fn_bcmlrd_lt_mreq_info_get_t) (int unit, bcmltd_sid_t req_ltid,
                                             const lt_mreq_info_t **lt_mreq_info);

/*CCI */
typedef int (*fn_pt_cci_ctrtype_get_t)(int unit, bcmdrd_sid_t sid);
typedef int (*fn_pt_cci_ctr_sym_map_t)(int unit, bcmptm_cci_ctr_info_t *ctr_info);
typedef int (*fn_pt_cci_frmt_sym_get_t)(int unit, int index, int field,
                                        bcmdrd_sid_t *sid, size_t *width);
typedef int (*fn_pt_cci_frmt_index_from_port_t)(int unit, bcmdrd_sid_t sid, int port,
                                                      int *index_min, int *index_max);
typedef bool (*fn_pt_cci_index_valid_t)(int unit, bcmdrd_sid_t sid, int tbl_inst, int index);
typedef int (*fn_pt_cci_ctr_evict_control_get_t)(int unit, bcmdrd_sid_t sid,
                                                 bcmdrd_sid_t *ctrl_sid);
typedef int (*fn_pt_cci_ctr_evict_enable_field_get_t)(int unit,
                                                      bcmdrd_fid_t *fid);
typedef int (*fn_pt_cci_ctr_evict_clr_on_read_field_get_t)(int unit,
                                                           bcmdrd_fid_t *fid);
typedef int (*fn_pt_cci_ctr_evict_default_enable_list_get_t)(int unit,
                                                             size_t *size,
                                                             bcmdrd_sid_t *sid);
typedef bool (*fn_pt_cci_ctr_is_bypass_t)(int unit, bcmdrd_sid_t sid);
typedef int (*fn_pt_cci_fifodma_chan_get_t)(int unit);
typedef int (*fn_pt_cci_ctr_evict_fifo_enable_sym_get_t)(int unit,
                                                         bcmdrd_sid_t *sid,
                                                         bcmdrd_fid_t *fid);
typedef int (*fn_pt_cci_ctr_evict_fifo_sym_get_t)(int unit, bcmdrd_sid_t *sid);
typedef int (*fn_pt_cci_ctr_evict_fifo_entry_get_t)(int unit,
                                                    uint32_t *evict,
                                                    uint32_t *pool,
                                                    bcmptm_cci_ctr_info_t *info);
typedef uint32_t (*fn_pt_cci_ctr_evict_fifo_buf_size_get_t)(int unit);
typedef int (*fn_pt_cci_ctr_evict_field_num_get_t)(int unit,
                                                   bcmptm_cci_ctr_mode_t ctr_mode,
                                                   size_t *num);
typedef int (*fn_pt_cci_ctr_evict_field_width_get_t)(int unit,
                                                   bcmptm_cci_ctr_mode_t ctr_mode,
                                                   size_t *width);
typedef int (*fn_pt_cci_ctr_evict_normal_val_get_t)(int unit,
                                                    size_t fld_width,
                                                    uint32_t *entry,
                                                    size_t size_e,
                                                    uint64_t *ctr,
                                                    size_t size_ctr);
typedef int (*fn_pt_cci_ctr_evict_wide_val_get_t)(int unit,
                                                  size_t fld_width,
                                                  uint32_t *entry,
                                                  size_t size_w,
                                                  uint64_t *ctr,
                                                  size_t size_ctr);
typedef int (*fn_pt_cci_ctr_evict_slim_val_get_t)(int unit,
                                                  size_t fld_width,
                                                  uint32_t *entry,
                                                  size_t size_e,
                                                  uint32_t *ctr,
                                                  size_t size_ctr);
typedef int (*fn_pt_cci_ctr_evict_normal_double_val_get_t)(int unit,
                                                  size_t fld_width,
                                                  uint32_t *entry,
                                                  size_t size_e,
                                                  uint64_t *ctr,
                                                  size_t size_ctr);
typedef uint32_t (*fn_pt_cci_reg_addr_gap_get_t)(int unit,
                                                 bcmdrd_sid_t sid);
typedef bool (*fn_pt_cci_reg_new_block_t)(int unit,
                                          bcmdrd_sid_t sid);
typedef bool (*fn_pt_cci_ctr_evict_cor_supported_t)(int unit, bcmdrd_sid_t sid);
typedef bool (*fn_pt_cci_ctr_col_dma_supported_t)(int unit);
typedef int (*fn_pt_cci_ctr_evict_update_mode_num_get_t)(int unit,
                                                         size_t *num);

/* UFT */
typedef int (*fn_uft_driver_register_t) (int unit, uft_driver_t *drv);
typedef int (*fn_uft_be_driver_register_t) (int unit, uft_be_driver_t *drv);
/* SBR. */
typedef int (*fn_sbr_driver_register_t) (int unit, sbr_driver_t *drv);

/* RM hash */
typedef int (*fn_rm_hash_def_bank_info_get_t) (int unit,
                                               uint32_t mask,
                                               bcmptm_rm_hash_def_bank_info_t *banks_info);

typedef int (*fn_rm_hash_table_info_get_t) (int unit,
                                            const bcmptm_rm_hash_table_info_t **tbl_info,
                                            uint8_t *num_tbl_info);

/* FP EM */
typedef int (*fn_pt_dyn_lt_mreq_info_init_t)(int unit,
                                             uint32_t trans_id,
                                             bcmltd_sid_t ltid,
                                             bool warm);
typedef int (*fn_pt_dynamic_lt_validate_t)(int unit, bcmltd_sid_t ltid);

typedef struct fn_ptr_all_s {
    fn_rsp_info_get_t             fn_rsp_info_get;
    fn_wal_info_get_t             fn_wal_info_get;
    fn_dev_info_get_t             fn_dev_info_get;
    fn_pt_attr_is_alpm_t          fn_pt_attr_is_alpm;
    fn_pt_overlay_info_get_t      fn_pt_overlay_info_get;
    fn_pt_overlay_mult_info_get_t fn_pt_overlay_mult_info_get;
    fn_pt_wo_aggr_info_get_t      fn_pt_wo_aggr_info_get;
    fn_pt_tcam_km_format_t        fn_pt_tcam_km_format;
    fn_hw_does_km_to_xy_t         fn_hw_does_km_to_xy;
    fn_tcam_corrupt_bits_enable_t fn_tcam_corrupt_bits_enable;
    fn_pt_needs_ltid_t            fn_pt_needs_ltid;
    fn_pt_needs_dfid_t            fn_pt_needs_dfid;
    fn_pt_in_must_cache_list_t    fn_pt_in_must_cache_list;
    fn_pt_in_dont_cache_list_t    fn_pt_in_dont_cache_list;
    fn_pt_group_index_count_set_t fn_pt_group_index_count_set;
    fn_pt_index_max_get_t         fn_pt_index_max_get;
    fn_pt_index_min_get_t         fn_pt_index_min_get;
    fn_pt_group_info_get_t        fn_pt_group_info_get;
    fn_pt_rm_tcam_memreg_oper_t   fn_pt_rm_tcam_memreg_oper;

    fn_alpm_driver_register_t     fn_alpm_driver_register;

    fn_ser_data_driver_register_t fn_ser_data_driver_register;

    fn_ctr_flex_data_driver_register_t fn_ctr_flex_data_driver_register;

    /* CCI */
    fn_pt_cci_ctrtype_get_t fn_pt_cci_ctrtype_get;
    fn_pt_cci_ctr_sym_map_t fn_pt_cci_ctr_sym_map;
    fn_pt_cci_frmt_sym_get_t fn_pt_cci_frmt_sym_get;
    fn_pt_cci_frmt_index_from_port_t fn_pt_cci_frmt_index_from_port;
    fn_pt_cci_index_valid_t fn_pt_cci_index_valid;
    fn_pt_cci_ctr_evict_control_get_t fn_pt_cci_ctr_evict_control_get;
    fn_pt_cci_ctr_evict_enable_field_get_t fn_pt_cci_ctr_evict_enable_field_get;
    fn_pt_cci_ctr_evict_clr_on_read_field_get_t fn_pt_cci_ctr_evict_clr_on_read_field_get;
    fn_pt_cci_ctr_evict_default_enable_list_get_t fn_pt_cci_ctr_evict_default_enable_list_get;
    fn_pt_cci_ctr_is_bypass_t fn_pt_cci_ctr_is_bypass;
    fn_pt_cci_fifodma_chan_get_t fn_pt_cci_fifodma_chan_get;
    fn_pt_cci_ctr_evict_fifo_enable_sym_get_t fn_pt_cci_ctr_evict_fifo_enable_sym_get;
    fn_pt_cci_ctr_evict_fifo_sym_get_t fn_pt_cci_ctr_evict_fifo_sym_get;
    fn_pt_cci_ctr_evict_fifo_entry_get_t fn_pt_cci_ctr_evict_fifo_entry_get;
    fn_pt_cci_ctr_evict_fifo_buf_size_get_t fn_pt_cci_ctr_evict_fifo_buf_size_get;
    fn_pt_cci_ctr_evict_field_num_get_t fn_pt_cci_ctr_evict_field_num_get;
    fn_pt_cci_ctr_evict_field_width_get_t fn_pt_cci_ctr_evict_field_width_get;
    fn_pt_cci_ctr_evict_normal_val_get_t fn_pt_cci_ctr_evict_normal_val_get;
    fn_pt_cci_ctr_evict_wide_val_get_t fn_pt_cci_ctr_evict_wide_val_get;
    fn_pt_cci_ctr_evict_slim_val_get_t fn_pt_cci_ctr_evict_slim_val_get;
    fn_pt_cci_ctr_evict_normal_double_val_get_t fn_pt_cci_ctr_evict_normal_double_val_get;
    fn_pt_cci_reg_addr_gap_get_t fn_pt_cci_reg_addr_gap_get;
    fn_pt_cci_reg_new_block_t fn_pt_cci_reg_new_block;
    fn_pt_cci_ctr_evict_cor_supported_t fn_pt_cci_ctr_evict_cor_supported;
    fn_pt_cci_ctr_col_dma_supported_t fn_pt_cci_ctr_col_dma_supported;
    fn_pt_cci_ctr_evict_update_mode_num_get_t fn_pt_cci_ctr_evict_update_mode_num_get;

    /* UFT */
    fn_uft_driver_register_t         fn_uft_driver_register;
    fn_uft_be_driver_register_t      fn_uft_be_driver_register;
    /* SBR. */
    fn_sbr_driver_register_t         fn_sbr_driver_register;

    /* CTH ALPM */
    fn_cth_alpm_driver_register_t     fn_cth_alpm_driver_register;

    /* RM HASH */
    fn_rm_hash_def_bank_info_get_t   fn_rm_hash_def_bank_info_get;
    fn_rm_hash_table_info_get_t      fn_rm_hash_table_info_get;

    /* RM TCAM */
    fn_pt_rm_tcam_prio_only_hw_key_info_init_t fn_pt_rm_tcam_prio_only_hw_key_info_init;
    /* FP EM */
    fn_pt_dyn_lt_mreq_info_init_t       fn_pt_dyn_lt_mreq_info_init;
    fn_pt_dynamic_lt_validate_t    fn_pt_dynamic_lt_validate;

    /* add more members here as needed */
} fn_ptr_all_t;

typedef int (*fnptr_attach_f)(int unit, fn_ptr_all_t *fn_ptr_all); /* signature of attach function */
typedef int (*fnptr_detach_f)(int unit); /* signature of detach function */

/*! Function type for ptm variant driver attach */
typedef int (*ptm_var_attach_f)(int unit);

/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * Public Functions
 */
/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int bcmptm_##_bc##_fnptr_attach(int unit, fn_ptr_all_t *fn_ptr_all);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int bcmptm_##_bc##_fnptr_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_ptm_var_attach(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMPTM_COMMON_H */
