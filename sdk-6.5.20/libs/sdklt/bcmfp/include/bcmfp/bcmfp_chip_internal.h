/*! \file bcmfp_chip_internal.h
 *
 * Chip specific public functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmfp/bcmfp_internal.h>

#ifndef BCMFP_CHIP_INTERNAL_H
#define BCMFP_CHIP_INTERNAL_H

/*! \cond  Externs for the fp attach functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_device_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp stage init functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_stage_device_init(int unit, bcmfp_stage_t *stage, bool warm);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp action conflict check functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_action_conflict_check(int unit, bcmfp_stage_t *stage, bcmfp_action_t action1, bcmfp_action_t action2);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp qualifier enum set functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_qualifier_enum_set(int unit,\
                                       bcmfp_stage_id_t stage_id,\
                                       bcmfp_qualifier_t qual,\
                                       bool is_presel,\
                                       int value,\
                                       uint32_t *data,\
                                       uint32_t *mask);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp qualifier enum get functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_qualifier_enum_get(int unit,\
                                       bcmfp_stage_id_t stage_id,\
                                       bcmfp_qualifier_t qual,\
                                       bool is_presel,\
                                       uint32_t data,\
                                       uint32_t mask,\
                                       int *value);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp profiles install functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_profiles_install(int unit,\
                                     const bcmltd_op_arg_t *op_arg,\
                                     uint32_t flags,\
                                     bcmfp_stage_t *stage,\
                                     bcmfp_group_id_t group_id,\
                                     bcmfp_group_oper_info_t *opinfo,\
                                     bcmfp_buffers_t *buffers);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp profiles uninstall functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_profiles_uninstall(int unit,\
                                       const bcmltd_op_arg_t *op_arg,\
                                       uint32_t flags,\
                                       bcmfp_stage_t *stage,\
                                       bcmfp_group_id_t group_id,\
                                       bcmfp_group_oper_info_t *opinfo);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp profiles copy functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_profiles_copy(int unit, uint32_t trans_id, bcmfp_stage_t *stage);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp presel edw get functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_presel_edw_get(int unit,\
                                   bcmfp_stage_id_t stage_id,\
                                   bcmfp_group_id_t group_id,\
                                   bcmfp_group_oper_info_t *opinfo,\
                                   uint32_t **edw);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp presel group info get functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_presel_group_info_get(int unit,\
                                          bcmfp_stage_id_t stage_id,\
                                          bcmfp_group_id_t group_id,\
                                          uint32_t **presel_group_info);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp presel group selcode key get functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_group_selcode_key_get(int unit, bcmfp_stage_t *stage, bcmfp_group_t *fg);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp entry key match type set functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_entry_key_match_type_set(int unit,\
                                             bcmfp_stage_t *stage,\
                                             bcmfp_group_id_t group_id,\
                                             bcmfp_group_oper_info_t *opinfo,\
                                             bcmfp_group_slice_mode_t slice_mode,\
                                             bcmfp_group_type_t port_pkt_type,\
                                             uint32_t **ekw);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp src class mode set functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_src_class_mode_set(int unit,\
                                       const bcmltd_op_arg_t *op_arg,\
                                       bcmfp_stage_id_t stage_id,\
                                       bcmltd_sid_t sid,\
                                       int pipe_id,\
                                       uint32_t mode);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp buffers customize functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_buffers_customize(int unit,\
                                      const bcmltd_op_arg_t *op_arg,\
                                      uint32_t flags,\
                                      bcmfp_stage_t *stage,\
                                      bcmfp_group_oper_info_t *opinfo,\
                                      bcmfp_pdd_config_t *pdd_config,\
                                      bcmfp_key_ids_t *key_ids,\
                                      bcmfp_buffers_t *buffers);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp pdd profile entry get functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_pdd_profile_entry_get(int unit,\
                                          const bcmltd_op_arg_t *op_arg,\
                                          bcmfp_stage_t *stage,\
                                          bcmfp_pdd_id_t pdd_id,\
                                          bcmfp_group_id_t group_id,\
                                          bcmfp_group_oper_info_t *opinfo,\
                                          bcmfp_pdd_oper_type_t pdd_type,\
                                          uint32_t **pdd_profile, \
                                          bcmfp_pdd_config_t *pdd_config);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp range check group set functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_range_check_grp_set(int unit,\
                                        const bcmltd_op_arg_t *op_arg,\
                                        bcmfp_stage_id_t stage_id,\
                                        bcmltd_sid_t sid,\
                                        int pipe_id,\
                                        int range_check_grp_id,\
                                        bcmfp_range_check_id_bmp_t bmp);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp meter actions set functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_meter_actions_set(int unit,\
                                      bcmfp_stage_t *stage,\
                                      bcmfp_group_id_t group_id,\
                                      bcmfp_pdd_id_t pdd_id,\
                                      bcmfp_group_oper_info_t *opinfo,\
                                      bcmcth_meter_fp_policy_fields_t *meter_entry,\
                                      uint32_t **edw);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp counter actions set functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_counter_actions_set(int unit,\
                                        bcmfp_stage_t *stage,\
                                        bcmfp_group_id_t group_id,\
                                        bcmfp_pdd_id_t pdd_id,\
                                        bcmfp_group_oper_info_t *opinfo,\
                                        bcmfp_ctr_params_t *ctr_params,\
                                        uint32_t **edw);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp counter hw idx set functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_counter_hw_idx_set(int unit,\
                                       bcmfp_stage_t *stage,\
                                       uint32_t ctr_id,\
                                       uint32_t pipe_id,\
                                       uint32_t pool_id,\
                                       int color,\
                                       bcmimm_entry_event_t event_reason);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp em bank sids get functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_bank_sids_get(int unit,\
                                  bcmfp_stage_t *stage,\
                                  bcmdrd_sid_t bank_sid,\
                                  uint32_t *ptinfo);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp attach functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_compress_fid_hw_info_init(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp em pbank sid get functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_bank_info_set(int unit,\
                                  bcmfp_stage_t *stage);

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the fp tbls init functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_di_fp_variant_attach(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMFP_CHIP_INTERNAL_H */

