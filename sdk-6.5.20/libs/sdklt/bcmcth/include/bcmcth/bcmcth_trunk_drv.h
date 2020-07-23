/*! \file bcmcth_trunk_drv.h
 *
 * BCMCTH Trunk driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_DRV_H
#define BCMCTH_TRUNK_DRV_H

#include <shr/shr_bitop.h>

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmcth/generated/dlb_ha.h>
#include <bcmcth/bcmcth_trunk_types.h>

/*******************************************************************************
 * Typedefs
 */
/*!
 * \brief Structure for storing information on one TRUNK_VP member table field.
 */
typedef struct bcmcth_trunk_vp_member_field_info_s {
    /*! Field name in TRUNK_VP group LT */
    uint16_t grp_fld;

    /*! Field name in TRUNK_VP member HW table */
    uint16_t mbr_fld;

    /*! Start bit of the field in TRUNK_VP HW table */
    uint16_t start_bit;

    /*! End bit of the field in TRUNK_VP HW table */
    uint16_t end_bit;
}bcmcth_trunk_vp_member_field_info_t;

/*!
 * \brief Structure for storing information on TRUNK_VP HW table fields.
 */
typedef struct bcmcth_trunk_vp_member_info_s {
    /*! Name of TRUNK_VP group LT */
    uint32_t grp_tbl;

    /*! Number of TRUNK_VP member tables */
    uint16_t mbr_tbl_count;

    /*! Array of member tables */
    const uint32_t *mbr_tbl;

    /*! Count of member fields */
    uint16_t flds_count;

    /*! Array of fields in member tables */
    const bcmcth_trunk_vp_member_field_info_t *flds;
}bcmcth_trunk_vp_member_info_t;

/*!
 * \brief Structure for storing information on TRUNK
 * driver variant lt ID's and field ID's.
 */
typedef struct bcmcth_trunk_drv_var_ids_s {
    /*! TRUNK_VP LT ID. */
    const int trunk_vp_ltid;

    /*! TRUNK_VP_ID field ID. */
    const int trunk_vp_trunk_vp_id;

    /*! LB_MDOE field ID. */
    const int trunk_vp_lb_mode_id;

    /*! MAX_MEMBERS field ID. */
    const int trunk_vp_max_members_id;

    /*! MEMBER_CNT field ID. */
    const int trunk_vp_member_cnt_id;

    /*! TRUNK_VP_WEIGHTED LT ID. */
    const int trunk_vp_weighted_ltid;

    /*! TRUNK_VP_ID field ID for TRUNK_VP_WEIGHTED LT. */
    const int trunk_vp_weighted_trunk_vp_id;

    /*! WEIGHTED SIZE field ID. */
    const int trunk_vp_weighted_wt_size_id;

    /*! MEMBER_CNT field ID for TRUNK_VP_WEIGHTED LT. */
    const int trunk_vp_weighted_member_cnt_id;
} bcmcth_trunk_drv_var_ids_t;

/*!
 * \brief Structure for TRUNK_VP device variant driver.
 */
typedef struct bcmcth_trunk_vp_drv_var_s {
    /*! Pointer to field information for TRUNK_VP LT. */
    const bcmcth_trunk_vp_member_field_info_t *flds_mem0;

    /*! Pointer to field information for TRUNK_VP_WEIGHTED LT. */
    const bcmcth_trunk_vp_member_field_info_t *wflds_mem0;

    /*! Pointer to HW field information for TRUNK_VP LT. */
    const bcmcth_trunk_vp_member_info_t *member0_info;

    /*! Pointer to HW field information for TRUNK_VP_WEIGHTED LT. */
    const bcmcth_trunk_vp_member_info_t *wmember0_info;

    /*! Pointer to HW memory information for TRUNK_VP LT. */
    const uint32_t *mem0;

    /*! Pointer to device variant LT ID's. */
    const bcmcth_trunk_drv_var_ids_t *ids;
} bcmcth_trunk_vp_drv_var_t;

/*!
 * \brief Structure for TRUNK device variant driver.
 */
typedef struct bcmcth_trunk_drv_var_s {
    /*! Pointer to TRUUNK_VP device variant information. */
    const bcmcth_trunk_vp_drv_var_t    *trunk_vp_var;
} bcmcth_trunk_drv_var_t;

/*!
 * \brief Set trunk mode for a unit
 * \n Must be called every time a unit is detached.
 *
 * \param [in] unit Logical device id.
 * \param [in] fast Fast trunk mode enable.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int(*fn_trunk_mode_set_t)(int unit, bool fast);

/*!
 * \brief Add trunk group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_grp_add_t)(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t lt_id,
                            bcmcth_trunk_grp_bk_t *grp_bk,
                            bcmcth_trunk_group_param_t *param);

/*!
 * \brief Update trunk group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_grp_update_t)(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               bcmcth_trunk_grp_bk_t *grp_bk,
                               bcmcth_trunk_group_param_t *param);

/*!
 * \brief Del trunk group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeeping structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_grp_del_t)(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t lt_id,
                            bcmcth_trunk_grp_bk_t *grp_bk,
                            bcmcth_trunk_group_param_t *param);

/*!
 * \brief trunk group init
 * \n Must be called every time a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_grp_init_t)(int unit, bcmcth_trunk_grp_bk_t *grp_bk);

/*!
 * \brief Add trunk fast group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_fast_grp_add_t)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_fast_group_param_t *param);

/*!
 * \brief Update trunk fast group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of group info from users.
 * \param [in] grp Old trunk group info.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_fast_grp_update_t)(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmltd_sid_t lt_id,
                                    bcmcth_trunk_fast_group_param_t *param,
                                    bcmcth_trunk_fast_group_t *grp);

/*!
 * \brief Del trunk fast group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of group info from users.
 * \param [in] grp Old trunk group info.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_fast_grp_del_t)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_fast_group_param_t *param,
                                 bcmcth_trunk_fast_group_t *grp);

/*!
 * \brief trunk fast group init
 * \n Must be called every time when a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] fgrp_bk Pointer to fast grp bookkeep structure.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_fast_grp_init_t)(int unit, bcmcth_trunk_fgrp_bk_t *fgrp_bk);

/*!
 * \brief Add trunk failover entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of failover info from users.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_failover_add_t)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_failover_param_t *param);

/*!
 * \brief Update trunk failover entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of failover info from users.
 * \param [in] entry Pointer to old bcmcth_trunk_failover_t entry.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_failover_update_t)(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmltd_sid_t lt_id,
                                    bcmcth_trunk_failover_param_t *param,
                                    bcmcth_trunk_failover_t *entry);

/*!
 * \brief Del trunk failover entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of failover info from users.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_failover_del_t)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_failover_param_t *param);

/*!
 * \brief trunk failover init
 * \n Must be called every time a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] failover_bk Pointer to failover bookkeeping.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_failover_init_t)(int unit,
                                  bcmcth_trunk_failover_bk_t *failover_bk);

/*!
 * \brief Add system trunk group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 * \param [in] new_base member group start offset.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_sys_grp_add_t)(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                bcmcth_trunk_sys_grp_bk_t *grp_bk,
                                bcmcth_trunk_sys_grp_param_t *param,
                                uint32_t new_base);

/*!
 * \brief Update system trunk group.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 * \param [in] new_base member group start offset.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_sys_grp_update_t)(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t lt_id,
                                   bcmcth_trunk_sys_grp_bk_t *grp_bk,
                                   bcmcth_trunk_sys_grp_param_t *param,
                                   uint32_t new_base);

/*!
 * \brief Move sys trunk group to another location.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] src Source block information.
 * \param [in] dest Destination block information.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_sys_grp_move_t)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_sys_grp_bk_t *grp_bk,
                                 bcmcth_trunk_grp_defrag_t *src,
                                 bcmcth_trunk_grp_defrag_t *dest);

/*!
 * \brief Del system trunk entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of failover info from users.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_sys_grp_del_t)(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                bcmcth_trunk_sys_grp_bk_t *grp_bk,
                                bcmcth_trunk_sys_grp_param_t *param);

/*!
 * \brief System trunk group init
 * \n Must be called every time when a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_sys_grp_init_t)(int unit,
                                 bcmcth_trunk_sys_grp_bk_t *grp_bk);

/*!
 * \brief Add imm trunk entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 * \param [in] new_base member group start offset.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_imm_grp_add_t)(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                bcmcth_trunk_imm_grp_bk_t *grp_bk,
                                bcmcth_trunk_group_param_t *param,
                                uint32_t new_base);

/*!
 * \brief Update imm trunk entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 * \param [in] new_base member group start offset.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_imm_grp_update_t)(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t lt_id,
                                   bcmcth_trunk_imm_grp_bk_t *grp_bk,
                                   bcmcth_trunk_group_param_t *param,
                                   uint32_t new_base);

/*!
 * \brief Move imm trunk group to another location.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] src Source block information.
 * \param [in] desc Destination block information.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_imm_grp_move_t)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_imm_grp_bk_t *grp_bk,
                                 bcmcth_trunk_grp_defrag_t *src,
                                 bcmcth_trunk_grp_defrag_t *dest);


/*!
 * \brief Del imm trunk entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_imm_grp_del_t)(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                bcmcth_trunk_imm_grp_bk_t *grp_bk,
                                bcmcth_trunk_group_param_t *param);

/*!
 * \brief Initialize routine for imm trunk group
 * \n Must be called every time when a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] fgrp_bk Pointer to fast grp bookkeep structure.
 *
 * \retval SHR_E_NONE Success.
 */
typedef int (*fn_imm_grp_init_t)(int unit,
                                 bcmcth_trunk_imm_grp_bk_t *grp_bk);

/*! TRUNK_VP device specific data initialization function. */
typedef int (*fn_vp_info_init_t)(int unit,
                                 bool warm,
                                 const bcmcth_trunk_drv_var_t *drv_var);

/*! TRUNK_VP device specific data cleanup function. */
typedef int (*fn_vp_info_cleanup_t)(int unit);

/*! Install an TRUNK_VP group LT entry into the hardware tables. */
typedef int (*fn_vp_grp_ins_t)(int unit,
                               void *info,
                               const bcmcth_trunk_drv_var_t *drv_var);

/*! Delete an TRUNK_VP group LT entry from the hardware tables. */
typedef int (*fn_vp_grp_del_t)(int unit, void *info);

/*! Copy TRUNK_VP member table entries pointed by msrc to mdest region. */
typedef int (*fn_vp_memb_cpy_t)(int unit, const void *mdest, const void *msrc);

/*! Clear TRUNK_VP member table entries pointed by mfrag. */
typedef int (*fn_vp_memb_clr_t)(int unit, const void *mfrag);

/*! Update TRUNK_VP group's member table start index for the group in ginfo. */
typedef int (*fn_vp_gmstart_updt_t)(int unit, const void *ginfo);

/*! Populate the TRUNK_VP LT entry. */
typedef int (*fn_vp_lt_fields_fill_t)(int unit,
                                     const void *field,
                                     const bcmcth_trunk_drv_var_t *drv_var,
                                     void *lt_entry);

/*!
 * \brief TRUNK driver object
 *
 * TRUNK driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH TRUNK module by drv
 * init from the top layer. BCMCTH TRUNK internally will use this interface
 * to get the corresponding information.
 */
typedef struct bcmcth_trunk_drv_s {
    /*! Set trunk mode. */
    fn_trunk_mode_set_t             fn_trunk_mode_set;

    /*! Add trunk group. */
    fn_grp_add_t                    fn_grp_add;

    /*! Update trunk group. */
    fn_grp_update_t                 fn_grp_update;

    /*! Delete trunk group. */
    fn_grp_del_t                    fn_grp_del;

    /*! Initialize device-specific trunk driver. */
    fn_grp_init_t                   fn_grp_init;

    /*! Add fast trunk group. */
    fn_fast_grp_add_t               fn_fast_grp_add;

    /*! Update fast trunk group. */
    fn_fast_grp_update_t            fn_fast_grp_update;
    /*! Delete fast trunk group. */
    fn_fast_grp_del_t               fn_fast_grp_del;

    /*! Initialize device-specific fast trunk driver. */
    fn_fast_grp_init_t              fn_fast_grp_init;

    /*! Add failover LT entry. */
    fn_failover_add_t               fn_failover_add;

    /*! Update failover LT entry. */
    fn_failover_update_t            fn_failover_update;

    /*! Update failover LT entry. */
    fn_failover_del_t               fn_failover_del;

    /*! Initialize device-specific trunk failover driver. */
    fn_failover_init_t              fn_failover_init;

    /*! Add system trunk group LT entry. */
    fn_sys_grp_add_t                fn_sys_grp_add;

    /*! Add system trunk group LT entry. */
    fn_sys_grp_move_t               fn_sys_grp_move;

    /*! Update system trunk group LT entry. */
    fn_sys_grp_update_t             fn_sys_grp_update;

    /*! Delete system trunk group LT entry. */
    fn_sys_grp_del_t                fn_sys_grp_del;

    /*! Initialize device-specific system trunk driver. */
    fn_sys_grp_init_t               fn_sys_grp_init;

    /*! Add imm trunk group LT entry. */
    fn_imm_grp_add_t                fn_imm_grp_add;

    /*! Update imm trunk group LT entry. */
    fn_imm_grp_update_t             fn_imm_grp_update;

    /*! Move imm trunk group LT entry. */
    fn_imm_grp_move_t               fn_imm_grp_move;

    /*! Delete imm trunk group LT entry. */
    fn_imm_grp_del_t                fn_imm_grp_del;

    /*! Initialize device-specific trunk driver. */
    fn_imm_grp_init_t               fn_imm_grp_init;

    /*! TRUNK_VP device specific data initialization function. */
    fn_vp_info_init_t               info_init;

    /*! TRUNK_VP device specific data cleanup function. */
    fn_vp_info_cleanup_t            info_cleanup;

    /*! Install an TRUNK_VP group LT entry into the hardware tables. */
    fn_vp_grp_ins_t                 grp_ins;

    /*! Delete an TRUNK_VP group LT entry from the hardware tables. */
    fn_vp_grp_del_t                 grp_del;

    /*! Copy TRUNK_VP member table entries from src block to dest block. */
    fn_vp_memb_cpy_t                memb_cpy;

    /*! Clear TRUNK_VP member table entries block pointed by memb_blk. */
    fn_vp_memb_clr_t                memb_clr;

    /*! Update TRUNK_VP group member table start/base index. */
    fn_vp_gmstart_updt_t            gmstart_updt;

    /*! Populate the LT entry fields. */
    fn_vp_lt_fields_fill_t          lt_fields_fill;
} bcmcth_trunk_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern bcmcth_trunk_drv_t *_bc##_cth_trunk_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Install the device-specific TRUNK driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_trunk_drv_init(int unit);


/*!
 * \brief Initialize trunk driver for a unit
 * \n Must be called every time a new unit is attached.
 *
 * \param [in] unit Logical device id.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_init(int unit);

/*!
 * \brief Cleanup trunk driver for a unit
 * \n Must be called every time a unit is detached.
 *
 * \param [in] unit Logical device id.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_cleanup(int unit);

/*!
 * \brief Set trunk mode for a unit
 * \n Must be called every time a unit is detached.
 *
 * \param [in] unit Logical device id.
 * \param [in] fast Fast trunk mode enable.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_mode_set(int unit, bool fast);

/*!
 * \brief Add trunk group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeeping structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_grp_add(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t lt_id,
                         bcmcth_trunk_grp_bk_t *grp_bk,
                         bcmcth_trunk_group_param_t *param);

/*!
 * \brief Update trunk group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeeping structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_grp_update(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t lt_id,
                            bcmcth_trunk_grp_bk_t *grp_bk,
                            bcmcth_trunk_group_param_t *param);

/*!
 * \brief Del trunk group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeeping structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_grp_del(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t lt_id,
                         bcmcth_trunk_grp_bk_t *grp_bk,
                         bcmcth_trunk_group_param_t *param);

/*!
 * \brief trunk group init
 * \n Must be called every time a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_grp_init(int unit, bcmcth_trunk_grp_bk_t *grp_bk);

/*!
 * \brief Add trunk fast group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_fast_grp_add(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_fast_group_param_t *param);

/*!
 * \brief Update trunk fast group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of group info from users.
 * \param [in] grp Old trunk group info.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_fast_grp_update(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_fast_group_param_t *param,
                                 bcmcth_trunk_fast_group_t *grp);

/*!
 * \brief Del trunk fast group
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of group info from users.
 * \param [in] grp Old trunk group info.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_fast_grp_del(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_fast_group_param_t *param,
                              bcmcth_trunk_fast_group_t *grp);

/*!
 * \brief trunk fast group init
 * \n Must be called every time when a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] fgrp_bk Pointer to fast grp bookkeep structure.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_fast_grp_init(int unit, bcmcth_trunk_fgrp_bk_t *fgrp_bk);

/*!
 * \brief Add trunk failover entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of failover info from users.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_failover_add(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_failover_param_t *param);

/*!
 * \brief Update trunk failover entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of failover info from users.
 * \param [in] entry Pointer to old bcmcth_trunk_failover_t entry.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_failover_update(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_failover_param_t *param,
                                 bcmcth_trunk_failover_t *entry);

/*!
 * \brief Del trunk failover entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] param Parameter of failover info from users.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_failover_del(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_failover_param_t *param);

/*!
 * \brief trunk failover init
 * \n Must be called every time a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] failover_bk Pointer to failover bookkeeping.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_failover_init(int unit,
                               bcmcth_trunk_failover_bk_t* failover_bk);

/*!
 * \brief Add system trunk group.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 * \param [in] new_base Member group start offset.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_sys_grp_add(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_sys_grp_bk_t *grp_bk,
                             bcmcth_trunk_sys_grp_param_t *param,
                             uint32_t new_base);

/*!
 * \brief Update system trunk group.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 * \param [in] new_base Member group start offset.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_sys_grp_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                bcmcth_trunk_sys_grp_bk_t *grp_bk,
                                bcmcth_trunk_sys_grp_param_t *param,
                                uint32_t new_base);


/*!
 * \brief Move system trunk member group.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] src Information of source block.
 * \param [in] dest Information of destination block.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_sys_grp_move(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_sys_grp_bk_t *grp_bk,
                              bcmcth_trunk_grp_defrag_t *src,
                              bcmcth_trunk_grp_defrag_t *dest);

/*!
 * \brief Delete system trunk group.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_sys_grp_del(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_sys_grp_bk_t *grp_bk,
                             bcmcth_trunk_sys_grp_param_t *param);

/*!
 * \brief Initialize system trunk group.
 * \n Must be called every time a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_sys_grp_init(int unit,
                              bcmcth_trunk_sys_grp_bk_t *grp_bk);

/*!
 * \brief Add trunk group managed by imm.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 * \param [in] new_base Member group start offset.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_imm_grp_add(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_imm_grp_bk_t *grp_bk,
                             bcmcth_trunk_group_param_t *param,
                             uint32_t new_base);

/*!
 * \brief Update trunk group managed by imm.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 * \param [in] new_base Member group start offset.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_imm_grp_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                bcmcth_trunk_imm_grp_bk_t *grp_bk,
                                bcmcth_trunk_group_param_t *param,
                                uint32_t new_base);

/*!
 * \brief Move trunk member group.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] src Information of source block.
 * \param [in] dest Information of destination block.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_imm_grp_move(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_imm_grp_bk_t *grp_bk,
                              bcmcth_trunk_grp_defrag_t *src,
                              bcmcth_trunk_grp_defrag_t *dest);

/*!
 * \brief Del trunk group managed by imm.
 *
 * \param [in] unit Logical device id.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 * \param [in] param Parameter of group info from users.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_imm_grp_del(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_imm_grp_bk_t *grp_bk,
                             bcmcth_trunk_group_param_t *param);

/*!
 * \brief Initialize trunk group.
 * \n Must be called every time a unit is attached.
 *
 * \param [in] unit Logical device id.
 * \param [in] grp_bk Pointer to grp bookkeep structure.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmcth_trunk_drv_imm_grp_init(int unit,
                              bcmcth_trunk_imm_grp_bk_t *grp_bk);

/*!
 * \brief Intialize an TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in] warm Warm boot flag.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
extern int
bcmcth_trunk_drv_vp_info_init(int unit, bool warm);

/*!
 * \brief Clean up TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
extern int
bcmcth_trunk_drv_vp_cleanup(int unit);

/*!
 * \brief Group insert TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in,out] info lt entry information
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
extern int
bcmcth_trunk_drv_vp_grp_insert(int unit, void *info);

/*!
 * \brief Group delete TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in,out] info lt entry information
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
extern int
bcmcth_trunk_drv_vp_grp_delete(int unit, void *info);

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
extern int
bcmcth_trunk_drv_vp_memb_cpy(int unit,
                             const void *mdest,
                             const void *msrc);

/*!
 * \brief Member block clear TRUNK_VP driver function.
 *
 * \param [in] unit BCM unit number.
 * \param [in,out] mfrag member block information.
 *
 * \returns SHR_E_ERROR on failure.
 *          SHR_E_SUCCESS on success.
 */
extern int
bcmcth_trunk_drv_vp_memb_clr(int unit,
                             const void *mfrag);

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
extern int
bcmcth_trunk_drv_vp_gmstart_updt(int unit,
                                 const void *ginfo);


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
extern int
bcmcth_trunk_drv_vp_lt_fields_fill(int unit,
                                  const void *field,
                                  void *lt_entry);

/*!
 * \brief Get pointer to TRUNK base driver device variant structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device variant structure, or NULL if not found.
 */
extern bcmcth_trunk_drv_var_t *
bcmcth_trunk_drv_var_get(int unit);

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcmcth_trunk_drv_var_t *_bd##_vu##_va##_cth_trunk_drv_var_attach(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMCTH_TRUNK_DRV_H */

