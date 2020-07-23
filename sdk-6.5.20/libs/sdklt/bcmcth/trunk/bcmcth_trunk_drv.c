/*! \file bcmcth_trunk_drv.c
 *
 * BCMCTH Trunk Driver APIs.
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
#include <bcmcth/bcmcth_trunk_drv.h>

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type. */
typedef bcmcth_trunk_drv_t *(*bcmcth_trunk_drv_get_f)(int unit);

/*! Device variant specific attach function type. */
typedef bcmcth_trunk_drv_var_t *(*bcmcth_trunk_drv_var_attach_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_trunk_drv_get },
static struct {
    bcmcth_trunk_drv_get_f drv_get;
} trunk_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*! Array of device variant specific TRUNK_VP table init function pointer. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_trunk_drv_var_attach},
static struct {
    bcmcth_trunk_drv_var_attach_f attach;
} trunk_drv_var_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

static bcmcth_trunk_drv_t *trunk_drv[BCMDRD_CONFIG_MAX_UNITS];

static bcmcth_trunk_drv_var_t *trunk_drv_var[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmcth_trunk_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    trunk_drv[unit] = trunk_drv_get[dev_type].drv_get(unit);

    /* Perform trunk variant-specific driver attach. */
    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        return SHR_E_UNIT;
    }
    trunk_drv_var[unit] = trunk_drv_var_get[variant].attach(unit);

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_mode_set(int unit, bool fast)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_trunk_mode_set) {
        return trunk_drv[unit]->fn_trunk_mode_set(unit, fast);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_grp_add(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t lt_id,
                         bcmcth_trunk_grp_bk_t *grp_bk,
                         bcmcth_trunk_group_param_t *param)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_grp_add) {
        return trunk_drv[unit]->fn_grp_add(unit, op_arg, lt_id,
                                           grp_bk, param);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_grp_update(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t lt_id,
                            bcmcth_trunk_grp_bk_t *grp_bk,
                            bcmcth_trunk_group_param_t *param)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_grp_update) {
        return trunk_drv[unit]->fn_grp_update(unit, op_arg, lt_id,
                                              grp_bk, param);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_grp_del(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t lt_id,
                         bcmcth_trunk_grp_bk_t *grp_bk,
                         bcmcth_trunk_group_param_t *param)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_grp_del) {
        return trunk_drv[unit]->fn_grp_del(unit, op_arg, lt_id,
                                           grp_bk, param);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_grp_init(int unit, bcmcth_trunk_grp_bk_t *grp_bk)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_grp_init) {
        return trunk_drv[unit]->fn_grp_init(unit, grp_bk);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_fast_grp_add(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_fast_group_param_t *param)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_fast_grp_add) {
        return trunk_drv[unit]->fn_fast_grp_add(unit, op_arg, lt_id, param);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_fast_grp_update(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_fast_group_param_t *param,
                                 bcmcth_trunk_fast_group_t *grp)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_fast_grp_update) {
        return trunk_drv[unit]->fn_fast_grp_update(unit, op_arg, lt_id,
                                                   param, grp);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_fast_grp_del(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_fast_group_param_t *param,
                              bcmcth_trunk_fast_group_t *grp)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_fast_grp_del) {
        return trunk_drv[unit]->fn_fast_grp_del(unit, op_arg, lt_id,
                                                param, grp);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_fast_grp_init(int unit, bcmcth_trunk_fgrp_bk_t *fgrp_bk)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_fast_grp_init) {
        return trunk_drv[unit]->fn_fast_grp_init(unit, fgrp_bk);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_failover_add(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t lt_id,
                          bcmcth_trunk_failover_param_t *param)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_failover_add) {
        return trunk_drv[unit]->fn_failover_add(unit, op_arg, lt_id, param);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_failover_update(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_failover_param_t *param,
                                 bcmcth_trunk_failover_t *entry)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_failover_update) {
        return trunk_drv[unit]->fn_failover_update(unit, op_arg, lt_id,
                                                   param, entry);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_failover_del(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_failover_param_t *param)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_failover_del) {
        return trunk_drv[unit]->fn_failover_del(unit, op_arg, lt_id, param);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_failover_init(int unit,
                               bcmcth_trunk_failover_bk_t *failover_bk)
{
    if (trunk_drv[unit] && trunk_drv[unit]->fn_failover_init) {
        return trunk_drv[unit]->fn_failover_init(unit, failover_bk);
    }

    return SHR_E_NONE;
}

int
bcmcth_trunk_drv_sys_grp_add(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_sys_grp_bk_t *grp_bk,
                             bcmcth_trunk_sys_grp_param_t *param,
                             uint32_t new_base)
{
    fn_sys_grp_add_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_sys_grp_add) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, op_arg, lt_id, grp_bk, param, new_base);
    }
}

int
bcmcth_trunk_drv_sys_grp_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                bcmcth_trunk_sys_grp_bk_t *grp_bk,
                                bcmcth_trunk_sys_grp_param_t *param,
                                uint32_t new_base)
{
    fn_sys_grp_update_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_sys_grp_update) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, op_arg, lt_id, grp_bk, param, new_base);
    }
}

int
bcmcth_trunk_drv_sys_grp_move(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_sys_grp_bk_t *grp_bk,
                              bcmcth_trunk_grp_defrag_t *src,
                              bcmcth_trunk_grp_defrag_t *dest)
{
    fn_sys_grp_move_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_sys_grp_move) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, op_arg, lt_id, grp_bk, src, dest);
    }
}

int
bcmcth_trunk_drv_sys_grp_del(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_sys_grp_bk_t *grp_bk,
                             bcmcth_trunk_sys_grp_param_t *param)
{
    fn_sys_grp_del_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_sys_grp_del) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, op_arg, lt_id, grp_bk, param);
    }
}

int
bcmcth_trunk_drv_sys_grp_init(int unit,
                              bcmcth_trunk_sys_grp_bk_t *grp_bk)
{
    fn_sys_grp_init_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_sys_grp_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, grp_bk);
    }
}

int
bcmcth_trunk_drv_imm_grp_add(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_imm_grp_bk_t *grp_bk,
                             bcmcth_trunk_group_param_t *param,
                             uint32_t new_base)
{
    fn_imm_grp_add_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_imm_grp_add) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, op_arg, lt_id, grp_bk, param, new_base);
    }
}

int
bcmcth_trunk_drv_imm_grp_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                bcmcth_trunk_imm_grp_bk_t *grp_bk,
                                bcmcth_trunk_group_param_t *param,
                                uint32_t new_base)
{
    fn_imm_grp_update_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_imm_grp_update) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, op_arg, lt_id, grp_bk, param, new_base);
    }
}

int
bcmcth_trunk_drv_imm_grp_move(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_imm_grp_bk_t *grp_bk,
                              bcmcth_trunk_grp_defrag_t *src,
                              bcmcth_trunk_grp_defrag_t *dest)
{
    fn_imm_grp_move_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_imm_grp_move) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, op_arg, lt_id, grp_bk, src, dest);
    }
}

int
bcmcth_trunk_drv_imm_grp_del(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_imm_grp_bk_t *grp_bk,
                             bcmcth_trunk_group_param_t *param)
{
    fn_imm_grp_del_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_imm_grp_del) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, op_arg, lt_id, grp_bk, param);
    }
}

int
bcmcth_trunk_drv_imm_grp_init(int unit,
                              bcmcth_trunk_imm_grp_bk_t *grp_bk)
{
    fn_imm_grp_init_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_imm_grp_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, grp_bk);
    }
}

/*!
 * \brief Intialize an TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in] warm Warm boot flag.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
int
bcmcth_trunk_drv_vp_info_init(int unit, bool warm)
{
    fn_vp_info_init_t my_fn;
    bcmcth_trunk_drv_var_t *drv_var;

    drv_var = trunk_drv_var[unit];

    if ((my_fn = trunk_drv[unit]->info_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, warm, drv_var);
    }
}

/*!
 * \brief Clean up TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
int
bcmcth_trunk_drv_vp_cleanup(int unit)
{
    fn_vp_info_cleanup_t my_fn;

    if ((my_fn = trunk_drv[unit]->info_cleanup) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit);
    }
}

/*!
 * \brief Group insert TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in,out] info lt entry information
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
int
bcmcth_trunk_drv_vp_grp_insert(int unit, void *info)
{
    fn_vp_grp_ins_t my_fn;
    bcmcth_trunk_drv_var_t *drv_var;

    drv_var = trunk_drv_var[unit];

    if ((my_fn = trunk_drv[unit]->grp_ins) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, info, drv_var);
    }
}

/*!
 * \brief Group delete TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in,out] info lt entry information
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
int
bcmcth_trunk_drv_vp_grp_delete(int unit, void *info)
{
    fn_vp_grp_del_t my_fn;

    if ((my_fn = trunk_drv[unit]->grp_del) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, info);
    }
}

/*!
 * \brief Member block copy TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in] mdest member destination block information.
 * \param [in] msrc member source block information.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
int
bcmcth_trunk_drv_vp_memb_cpy(int unit,
                           const void *mdest,
                           const void *msrc)
{
    fn_vp_memb_cpy_t my_fn;

    if ((my_fn = trunk_drv[unit]->memb_cpy) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, mdest, msrc);
    }
}

/*!
 * \brief Member block clear TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in,out] mfrag member block information.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
int
bcmcth_trunk_drv_vp_memb_clr(int unit,
                         const void *mfrag)
{
    fn_vp_memb_clr_t my_fn;

    if ((my_fn = trunk_drv[unit]->memb_clr) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, mfrag);
    }
}

/*!
 * \brief Group member block update TRUNK_VP driver function.
 *        Update group's member table start
 *        index for the group in ginfo.
 *
 * \param [in] unit BCM unit number.
 * \param [in,out] ginfo group information.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
int
bcmcth_trunk_drv_vp_gmstart_updt(int unit,
                             const void *ginfo)
{
    fn_vp_gmstart_updt_t my_fn;

    if ((my_fn = trunk_drv[unit]->gmstart_updt) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, ginfo);
    }
}
/*!
 * \brief Populate the TRUNK_VP LT entry.
 *
 * \param [in]     unit BCM unit number.
 * \param [in]     field imm field informtion.
 * \param [in,out] lt_entry LT entry.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
int
bcmcth_trunk_drv_vp_lt_fields_fill(int unit,
                                   const void *field,
                                   void *lt_entry)
{
    fn_vp_lt_fields_fill_t my_fn;
    bcmcth_trunk_drv_var_t *drv_var;

    drv_var = trunk_drv_var[unit];

    if ((my_fn = trunk_drv[unit]->lt_fields_fill) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, field, drv_var, lt_entry);
    }
}


/*!
 * \brief Get pointer to TRUNK base driver device variant structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device variant structure, or NULL if not found.
 */
bcmcth_trunk_drv_var_t *
bcmcth_trunk_drv_var_get(int unit)
{
    return trunk_drv_var[unit];
}
