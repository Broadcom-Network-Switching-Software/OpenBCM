/*! \file bcmfp_chip.c
 *
 * APIs to dispatch the request to chip specific functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_cth_range_check.h>
#include <bcmfp/bcmfp_cth_ctr.h>
#include <bcmfp/bcmfp_chip_internal.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlrd/bcmlrd_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

typedef int (*bcmfp_device_attach_f)(int unit);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_device_attach },
static struct {
    bcmfp_device_attach_f attach;
} fp_device_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_device_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_device_attach[dev_type].attach(unit));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_compress_fid_hw_info_init_f)(int unit);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_compress_fid_hw_info_init },
static struct {
    bcmfp_compress_fid_hw_info_init_f init;
} fp_compress_fid_hw_info_init[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_compress_fid_hw_info_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_compress_fid_hw_info_init[dev_type].init(unit));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_variant_attach_f)(int unit);
/* Array of device variant specific FP tables init function pointers. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_di_fp_variant_attach },
static struct {
    bcmfp_variant_attach_f attach;
} fp_variant_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

int
bcmfp_variant_attach(int unit)
{
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_variant_attach[variant].attach(unit));

exit:
    SHR_FUNC_EXIT();
}

typedef int (*bcmfp_stage_device_init_f)(int unit,
                                         bcmfp_stage_t *stage,
                                         bool warm);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_stage_device_init },
static struct {
    bcmfp_stage_device_init_f stage_device_init;
} fp_stage_device_init[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_stage_init(int unit, bcmfp_stage_t *stage, bool warm)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_stage_device_init[dev_type].stage_device_init(unit, stage, warm));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_action_conflict_check_f)(int unit,
                                             bcmfp_stage_t *stage,
                                             bcmfp_action_t action1,
                                             bcmfp_action_t action2);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_action_conflict_check },
static struct {
    bcmfp_action_conflict_check_f action_conflict_check;
} fp_action_conflict_check[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_action_conflict_check(int unit,
                            bcmfp_stage_t *stage,
                            bcmfp_action_t action1,
                            bcmfp_action_t action2)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_action_conflict_check[dev_type].action_conflict_check(unit, stage, action1, action2));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_qualifier_enum_set_f)(int unit,
                                          bcmfp_stage_id_t stage_id,
                                          bcmfp_qualifier_t qual,
                                          bool is_presel,
                                          int value,
                                          uint32_t *data,
                                          uint32_t *mask);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_qualifier_enum_set },
static struct {
    bcmfp_qualifier_enum_set_f qualifier_enum_set;
} fp_qualifier_enum_set[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_qualifier_enum_set(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qualifier_t qual,
                         bool is_presel,
                         int value,
                         uint32_t *data,
                         uint32_t *mask)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_qualifier_enum_set[dev_type].qualifier_enum_set(unit, stage_id, qual, is_presel, value, data, mask));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_qualifier_enum_get_f)(int unit,
                                          bcmfp_stage_id_t stage_id,
                                          bcmfp_qualifier_t qual,
                                          bool is_presel,
                                          uint32_t data,
                                          uint32_t mask,
                                          int *value);


#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_qualifier_enum_get },
static struct {
    bcmfp_qualifier_enum_get_f qualifier_enum_get;
} fp_qualifier_enum_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_qualifier_enum_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qualifier_t qual,
                         bool is_presel,
                         uint32_t data,
                         uint32_t mask,
                         int *value)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_qualifier_enum_get[dev_type].qualifier_enum_get(unit, stage_id, qual, is_presel, data, mask, value));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_profiles_install_f)(int unit,
                                        const bcmltd_op_arg_t *op_arg,
                                        uint32_t flags,
                                        bcmfp_stage_t *stage,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_group_oper_info_t *opinfo,
                                        bcmfp_buffers_t *buffers);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_profiles_install },
static struct {
    bcmfp_profiles_install_f profiles_install;
} fp_profiles_install[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_profiles_install(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       uint32_t flags,
                       bcmfp_stage_t *stage,
                       bcmfp_group_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_buffers_t *buffers)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_profiles_install[dev_type].profiles_install(unit, op_arg, flags, stage, group_id, opinfo, buffers));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_profiles_uninstall_f)(int unit,
                                        const bcmltd_op_arg_t *op_arg,
                                        uint32_t flags,
                                        bcmfp_stage_t *stage,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_group_oper_info_t *opinfo);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_profiles_uninstall },
static struct {
    bcmfp_profiles_uninstall_f profiles_uninstall;
} fp_profiles_uninstall[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_profiles_uninstall(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       uint32_t flags,
                       bcmfp_stage_t *stage,
                       bcmfp_group_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_profiles_uninstall[dev_type].profiles_uninstall(unit, op_arg, flags, stage, group_id, opinfo));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_profiles_copy_f)(int unit,
                                     uint32_t trans_id,
                                     bcmfp_stage_t *stage);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_profiles_copy },
static struct {
    bcmfp_profiles_copy_f profiles_copy;
} fp_profiles_copy[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_profiles_copy(int unit,
                    uint32_t trans_id,
                    bcmfp_stage_t *stage)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_profiles_copy[dev_type].profiles_copy(unit, trans_id, stage));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_presel_edw_get_f)(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      bcmfp_group_id_t group_id,
                                      bcmfp_group_oper_info_t *opinfo,
                                      uint32_t **edw);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_presel_edw_get },
static struct {
    bcmfp_presel_edw_get_f presel_edw_get;
} fp_presel_edw_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_presel_edw_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_group_id_t group_id,
                     bcmfp_group_oper_info_t *opinfo,
                     uint32_t **edw)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_presel_edw_get[dev_type].presel_edw_get(unit, stage_id, group_id, opinfo, edw));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_presel_group_info_get_f)(int unit,
                                             bcmfp_stage_id_t stage_id,
                                             bcmfp_group_id_t group_id,
                                             uint32_t **presel_group_info);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_presel_group_info_get },
static struct {
    bcmfp_presel_group_info_get_f presel_group_info_get;
} fp_presel_group_info_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_presel_group_info_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            uint32_t **presel_group_info)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(presel_group_info, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_presel_group_info_get[dev_type].presel_group_info_get(unit, stage_id, group_id, presel_group_info));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_group_selcode_key_get_f)(int unit, bcmfp_stage_t *stage, bcmfp_group_t *fg);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_group_selcode_key_get },
static struct {
    bcmfp_group_selcode_key_get_f group_selcode_key_get;
} fp_group_selcode_key_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_group_selcode_key_get(int unit, bcmfp_stage_t *stage, bcmfp_group_t *fg)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_group_selcode_key_get[dev_type].group_selcode_key_get(unit, stage, fg));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_entry_key_match_type_set_f)(int unit,
                                                bcmfp_stage_t *stage,
                                                bcmfp_group_id_t group_id,
                                                bcmfp_group_oper_info_t *opinfo,
                                                bcmfp_group_slice_mode_t slice_mode,
                                                bcmfp_group_type_t port_pkt_type,
                                                uint32_t **ekw);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_entry_key_match_type_set },
static struct {
    bcmfp_entry_key_match_type_set_f entry_key_match_type_set;
} fp_entry_key_match_type_set[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_entry_key_match_type_set(int unit,
                               bcmfp_stage_t *stage,
                               bcmfp_group_id_t group_id,
                               bcmfp_group_oper_info_t *opinfo,
                               bcmfp_group_slice_mode_t slice_mode,
                               bcmfp_group_type_t port_pkt_type,
                               uint32_t **ekw)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_entry_key_match_type_set[dev_type].entry_key_match_type_set(unit,
                                                                        stage,
                                                                        group_id,
                                                                        opinfo,
                                                                        slice_mode,
                                                                        port_pkt_type,
                                                                        ekw));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_src_class_mode_set_f)(int unit,
                                          const bcmltd_op_arg_t *op_arg,
                                          bcmfp_stage_id_t stage_id,
                                          bcmltd_sid_t sid,
                                          int pipe_id,
                                          uint32_t mode);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_src_class_mode_set },
static struct {
    bcmfp_src_class_mode_set_f src_class_mode_set;
} fp_src_class_mode_set[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_src_class_mode_set(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_sid_t sid,
                         int pipe_id,
                         uint32_t mode)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_src_class_mode_set[dev_type].src_class_mode_set(unit,
                                                            op_arg,
                                                            stage_id,
                                                            sid,
                                                            pipe_id,
                                                            mode));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_buffers_customize_f)(int unit,
                                         const bcmltd_op_arg_t *op_arg,
                                         uint32_t flags,
                                         bcmfp_stage_t *stage,
                                         bcmfp_group_oper_info_t *opinfo,
                                         bcmfp_pdd_config_t *pdd_config,
                                         bcmfp_key_ids_t *key_ids,
                                         bcmfp_buffers_t *buffers);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_buffers_customize },
static struct {
    bcmfp_buffers_customize_f buffers_customize;
} fp_buffers_customize[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_buffers_customize(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        uint32_t flags,
                        bcmfp_stage_t *stage,
                        bcmfp_group_oper_info_t *opinfo,
                        bcmfp_pdd_config_t *pdd_config,
                        bcmfp_key_ids_t *key_ids,
                        bcmfp_buffers_t *buffers)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_buffers_customize[dev_type].buffers_customize(unit,
                                                          op_arg,
                                                          flags,
                                                          stage,
                                                          opinfo,
                                                          pdd_config,
                                                          key_ids,
                                                          buffers));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_pdd_profile_entry_get_f)(int unit,
                                             const bcmltd_op_arg_t *op_arg,
                                             bcmfp_stage_t *stage,
                                             bcmfp_pdd_id_t pdd_id,
                                             bcmfp_group_id_t group_id,
                                             bcmfp_group_oper_info_t *opinfo,
                                             bcmfp_pdd_oper_type_t pdd_type,
                                             uint32_t **pdd_profile,
                                             bcmfp_pdd_config_t *pdd_config);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_pdd_profile_entry_get },
static struct {
    bcmfp_pdd_profile_entry_get_f pdd_profile_entry_get;
} fp_pdd_profile_entry_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_pdd_profile_entry_get(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_t *stage,
                            bcmfp_pdd_id_t pdd_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_group_oper_info_t *opinfo,
                            bcmfp_pdd_oper_type_t pdd_type,
                            uint32_t **pdd_profile,
                            bcmfp_pdd_config_t *pdd_config)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_pdd_profile_entry_get[dev_type].pdd_profile_entry_get(unit,
                                                                  op_arg,
                                                                  stage,
                                                                  pdd_id,
                                                                  group_id,
                                                                  opinfo,
                                                                  pdd_type,
                                                                  pdd_profile,
                                                                  pdd_config));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_range_check_grp_set_f)(int unit,
                                                 const bcmltd_op_arg_t *op_arg,
                                                 bcmfp_stage_id_t stage_id,
                                                 bcmltd_sid_t sid,
                                                 int pipe_id,
                                                 int range_check_grp_id,
                                                 bcmfp_range_check_id_bmp_t bmp);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_range_check_grp_set },
static struct {
    bcmfp_range_check_grp_set_f range_check_grp_set;
} fp_range_check_grp_set[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_range_check_grp_set(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmltd_sid_t sid,
                          int pipe_id,
                          int range_check_grp_id,
                          bcmfp_range_check_id_bmp_t bmp)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_range_check_grp_set[dev_type].range_check_grp_set(unit,
                                                              op_arg,
                                                              stage_id,
                                                              sid,
                                                              pipe_id,
                                                              range_check_grp_id,
                                                              bmp));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_meter_actions_set_f)(int unit,
                                         bcmfp_stage_t *stage,
                                         bcmfp_group_id_t group_id,
                                         bcmfp_pdd_id_t pdd_id,
                                         bcmfp_group_oper_info_t *opinfo,
                                         bcmcth_meter_fp_policy_fields_t *meter_entry,
                                         uint32_t **edw);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_meter_actions_set },
static struct {
    bcmfp_meter_actions_set_f meter_actions_set;
} fp_meter_actions_set[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_meter_actions_set(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_id_t group_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_group_oper_info_t *opinfo,
                        bcmcth_meter_fp_policy_fields_t *meter_entry,
                        uint32_t **edw)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_meter_actions_set[dev_type].meter_actions_set(unit,
                                                          stage,
                                                          group_id,
                                                          pdd_id,
                                                          opinfo,
                                                          meter_entry,
                                                          edw));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_counter_actions_set_f)(int unit,
                                           bcmfp_stage_t *stage,
                                           bcmfp_group_id_t group_id,
                                           bcmfp_pdd_id_t pdd_id,
                                           bcmfp_group_oper_info_t *opinfo,
                                           bcmfp_ctr_params_t *ctr_params,
                                           uint32_t **edw);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_counter_actions_set },
static struct {
    bcmfp_counter_actions_set_f counter_actions_set;
} fp_counter_actions_set[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_counter_actions_set(int unit,
                          bcmfp_stage_t *stage,
                          bcmfp_group_id_t group_id,
                          bcmfp_pdd_id_t pdd_id,
                          bcmfp_group_oper_info_t *opinfo,
                          bcmfp_ctr_params_t *ctr_params,
                          uint32_t **edw)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_counter_actions_set[dev_type].counter_actions_set(unit,
                                                              stage,
                                                              group_id,
                                                              pdd_id,
                                                              opinfo,
                                                              ctr_params,
                                                              edw));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_counter_hw_idx_set_f)(int unit,
                                            bcmfp_stage_t *stage,
                                            uint32_t ctr_id,
                                            uint32_t pipe_id,
                                            uint32_t pool_id,
                                            int color,
                                            bcmimm_entry_event_t event_reason);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_counter_hw_idx_set },
static struct {
    bcmfp_counter_hw_idx_set_f counter_hw_idx_set;
} fp_counter_hw_idx_set[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_counter_hw_idx_set(int unit,
                         bcmfp_stage_t *stage,
                         uint32_t ctr_id,
                         uint32_t pipe_id,
                         uint32_t pool_id,
                         int color,
                         bcmimm_entry_event_t event_reason)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_counter_hw_idx_set[dev_type].counter_hw_idx_set(unit,
                                                            stage,
                                                            ctr_id,
                                                            pipe_id,
                                                            pool_id,
                                                            color,
                                                            event_reason));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_bank_sids_get_f)(int unit,
                                     bcmfp_stage_t *stage,
                                     bcmdrd_sid_t bank_sid,
                                     uint32_t *ptinfo);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_bank_sids_get },
static struct {
    bcmfp_bank_sids_get_f bank_sids_get;
} fp_bank_sids_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_bank_sids_get(int unit,
                    bcmfp_stage_t *stage,
                    uint32_t bank_sid,
                    uint32_t *ptinfo)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_bank_sids_get[dev_type].bank_sids_get(unit,
                                                  stage,
                                                  bank_sid,
                                                  ptinfo));
exit:
   SHR_FUNC_EXIT();
}

typedef int (*bcmfp_bank_info_set_f)(int unit,
                                   bcmfp_stage_t *stage);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_fp_bank_info_set },
static struct {
    bcmfp_bank_info_set_f bank_info_set;
} fp_bank_info_set[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

int
bcmfp_bank_info_set(int unit,
                    bcmfp_stage_t *stage)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    /* Perform device-specific software setup */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_bank_info_set[dev_type].bank_info_set(unit, stage));
exit:
   SHR_FUNC_EXIT();
}


