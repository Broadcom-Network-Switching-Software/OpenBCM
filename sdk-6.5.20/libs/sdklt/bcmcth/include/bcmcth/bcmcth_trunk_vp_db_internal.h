/*! \file bcmcth_trunk_vp_db_internal.h
 *
 * This file contains TRUNK_VP custom table handler data structures definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_VP_DB_INTERNAL_H
#define BCMCTH_TRUNK_VP_DB_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <shr/shr_itbm.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmcth/bcmcth_trunk_vp.h>
#include <bcmcth/generated/trunk_ha.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_trunk_drv.h>
#include <bcmimm/bcmimm_int_comp.h>

/*!
 * \brief This macro is used to set a field present indicator bit status.
 *
 * Sets the field \c _f present bit indicator in the field bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMCTH_TRUNK_VP_LT_FIELD_SET(_m, _f)    \
            do {                        \
                (_m) |= (1 << _f);      \
            } while (0)

/*!
 * \brief This macro is used to get the field present indicator bit status.
 *
 * Returns the field \c _f present bit indicator status from the field
 * Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Field present indicator bit status from the field bitmap.
 */
#define BCMCTH_TRUNK_VP_LT_FIELD_GET(_m, _f)  ((_m) & (1 << (_f)))

/*! \brief The max number NextHop paths. */
#define BCMCTH_TRUNK_VP_NHOP_ARRAY_SIZE (16384)

/*!
 * \brief TRUNK_VP logical table fields information.
 */
typedef enum bcmcth_trunk_vp_lt_fields_e {

    /*! TRUNK_VP group identifier field is set. */
    BCMCTH_TRUNK_VP_LT_FIELD_TRUNK_VP_ID = 0,

    /*! TRUNK_VP load balancing mode field is set. */
    BCMCTH_TRUNK_VP_LT_FIELD_LB_MODE,

    /*! Maximum number of TRUNK_VP paths allowed for the group field is set. */
    BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS,

    /*!
     * Number of active TRUNK_VP paths
     * configured for the group field is set.
     */
    BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT,

    /*! Weighted group size field is set. */
    BCMCTH_TRUNK_VP_LT_FIELD_WEIGHTED_SIZE,

    /* member 0 field 0 is SET. */
    BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD0,

    /* member 0 field 0 is SET. */
    BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD1,

    /* member 0 field 0 is SET. */
    BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD2,

    /* member 0 field 0 is SET. */
    BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD3,

    /*! Must be the last one. */
    BCMCTH_TRUNK_VP_LT_FIELD_COUNT

} bcmcth_trunk_vp_fields_t;

/*!
 * \brief Structure for TRUNK_VP group table DRD fields information.
 */
typedef struct bcmcth_trunk_vp_grp_drd_fields_s {

    /*! Load Balancing Mode DRD field identifier. */
    bcmdrd_fid_t lb_mode;

    /*! Member table Base Pointer DRD field identifier. */
    bcmdrd_fid_t base_ptr;

    /*! Number of members in TRUNK_VP group DRD field identifier. */
    bcmdrd_fid_t count;

} bcmcth_trunk_vp_grp_drd_fields_t;

/*!
 * \brief Structure for TRUNK_VP group table LTD fields information.
 */
typedef struct bcmcth_trunk_vp_grp_ltd_fields_t_s {

    /*! TRUNK_VP group identifier LTD field identifier. */
    bcmcth_trunk_vp_ltd_fid_t trunk_vp_id_fid;

    /*! Load Balancing Mode LTD field identifier. */
    bcmcth_trunk_vp_ltd_fid_t lb_mode_fid;

    /*! Maximum TRUNK_VP paths LTD field identifier. */
    bcmcth_trunk_vp_ltd_fid_t max_members_fid;

    /*! Number of TRUNK_VP paths configured LTD field identifier. */
    bcmcth_trunk_vp_ltd_fid_t member_cnt_fid;

    /*! weighted TRUNK_VP group size LTD field identifier. */
    bcmcth_trunk_vp_ltd_fid_t weighted_size_fid;

} bcmcth_trunk_vp_grp_ltd_fields_t;

/*!
 * \brief Structure for TRUNK_VP member table DRD fields information.
 */
typedef struct bcmcth_trunk_vp_memb_drd_fields_s {
    /*! Next Hop ID or TRUNK_VP Group ID indicator DRD field identifier. */
    bcmdrd_fid_t trunk_vp_flag;

    /*! Underlay TRUNK_VP Group ID DRD field identifier. */
    bcmdrd_fid_t group_id;
} bcmcth_trunk_vp_memb_drd_fields_t;

/*!
 * \brief TRUNK_VP hardware tables resource manager operation flags.
 */
typedef enum bcmcth_trunk_vp_oper_e {
    /*! No operation for Init value. */
    BCMCTH_TRUNK_VP_OPER_NONE      = 0,

    /*! Entry add operation. */
    BCMCTH_TRUNK_VP_OPER_ADD       = (1 << 1),

    /*! Entry delete operation. */
    BCMCTH_TRUNK_VP_OPER_DEL       = (1 << 2),

    /*! Entry add operation with skip hardware index zero option. */
    BCMCTH_TRUNK_VP_OPER_SKIP_ZERO = (1 << 3),

    /*! Entry copy operation. */
    BCMCTH_TRUNK_VP_OPER_CPY       = (1 << 4),

    /*! Entry clear operation. */
    BCMCTH_TRUNK_VP_OPER_CLR       = (1 << 5),

    /*! Entry update operation. */
    BCMCTH_TRUNK_VP_OPER_UPDT      = (1 << 6),

    /*! Must be last one. */
    BCMCTH_TRUNK_VP_OPER_COUNT
} bcmcth_trunk_vp_oper_t;

/*!
 * \brief Structure for TRUNK_VP logical table fields information.
 */
typedef struct bcmcth_trunk_vp_lt_field_attrs_s {
    /*! Minimum TRUNK_VP Group Identifier value supported. */
    int min_trunk_vp_id;

    /*! Maximum TRUNK_VP Group Identifier value supported. */
    int max_trunk_vp_id;

    /*! Minimum TRUNK_VP load balancing mode value supported. */
    uint32_t min_lb_mode;

    /*! Maximum TRUNK_VP load balancing mode value supported. */
    uint32_t max_lb_mode;

    /*! Load balancing modes supported for the logical table. */
    uint32_t lb_mode[BCMCTH_TRUNK_VP_LB_MODE_COUNT];

    /*! Paths count (MAX_MEMBERS & MEMBER_CNT) must be in powers-of-two. */
    bool pow_two_paths;

    /*! Minimum TRUNK_VP paths allowed for groups. */
    uint32_t min_members;

    /*! Maximum TRUNK_VP paths supported for groups (hardware limit). */
    uint32_t max_members;

    /*! Minimum value for configured TRUNK_VP paths allowed for groups. */
    uint32_t min_nmembers;

    /*! Maximum value for configured TRUNK_VP paths allowed for groups. */
    uint32_t max_nmembers;

    /*! Minimum weighted TRUNK_VP group size value supported. */
    uint32_t min_weighted_size;

    /*! Maximum weighted TRUNK_VP group size value supported. */
    uint32_t max_weighted_size;

    /*! Weighted TRUNK_VP group size supported for the logical table. */
    uint32_t weighted_size[BCMCTH_TRUNK_VP_WEIGHTED_SIZE_COUNT];

} bcmcth_trunk_vp_lt_field_attrs_t;


/*!
 * Maximum TRUNK_VP fields value fields imported from
 * logical table ING_L3_ECMP_MEMBER_0_TABLE.
 */
#define MAX_TRUNK_VP_FLDS (4)

/*!
 * \brief Structure for TRUNK_VP logical table entry information.
 */
typedef struct bcmcth_trunk_vp_lt_entry_s {

    /*! TRUNK_VP group logial table source identifier. */
    bcmltd_sid_t glt_sid;
    /*! Operational argument. */
    const bcmltd_op_arg_t *op_arg;

    /*! Logical table entry field set/present indicator Bit Map. */
    SHR_BITDCL fbmp;

    /*! Logical table entry group type - BCMCTH_TRUNK_VP_GRP_TYPE_XXX . */
    bcmcth_trunk_vp_grp_type_t grp_type;

    /*! TRUNK_VP group identifier - Entry Key field value. */
    bcmcth_trunk_vp_id_t trunk_vp_id;

    /*! Load Balance mode field value. */
    uint32_t lb_mode;

    /*! Maximum number of paths supported by the group field value. */
    uint32_t max_members;

    /*! Numumber of paths configured for this group field value. */
    uint32_t member_cnt;

    /*! TRUNK_VP next-hop indicator field value. */
    bool trunk_vp_nhop;

    /*! Input NHOP_IDs count. */
    uint32_t nhop_ids_count;

    /*! Weighted TRUNK_VP group size value in member entries count. */
    uint32_t weighted_size;

    /*! Member table start index allocated for this TRUNK_VP group. */
    int mstart_idx;

    /*! IMM event for operating on this TRUNK_VP group. */
    bcmimm_entry_event_t event;

    /*! member field0 array elements. */
    uint32_t member0_field[MAX_TRUNK_VP_FLDS][BCMCTH_TRUNK_VP_NHOP_ARRAY_SIZE];
} bcmcth_trunk_vp_lt_entry_t;

/*!
 * \brief Structure for TRUNK_VP hardware table entry information.
 */
typedef struct bcmcth_trunk_vp_hw_entry_info_s {

    /*! Entry flags information. */
    uint8_t flags;
} bcmcth_trunk_vp_hw_entry_info_t;

/*!
 * \brief Structure for TRUNK_VP hardware table information.
 */
typedef struct bcmcth_trunk_vp_tbl_prop_s {

    /*! Device Resouce Database table Symbol ID */
    bcmdrd_sid_t drd_sid;

    /*! Logical table Database Symbol ID. */
    bcmltd_sid_t ltd_sid;

    /*! Table minimum valid index value. */
    int imin;

    /*! Table maximum valid index value. */
    int imax;

    /*! Max used index. */
    int imax_used;

    /*! ECM group table size. */
    int tbl_size;

    /*! Table Entry Size in words. */
    uint32_t ent_size;

    /*!
     *  Pointer to logical table DRD field ID -
     *  bcmcth_trunk_vp_grp_drd_fields_t.
     */
    void *drd_fids;

    /*!
     * Pointer to logical table LTD field ID -
     * bcmcth_trunk_vp_grp_ltd_fields_t.
     */
    void *ltd_fids;

    /*!
     * Pointer to logical table field attributes -
     * bcmcth_trunk_vp_lt_field_attrs_t.
     */
    void *lt_fattrs;

    /*! Entry information array. */
    bcmcth_trunk_vp_hw_entry_attr_t *ent_arr;

    /*! Table entry backup array elements base pointer.  */
    bcmcth_trunk_vp_hw_entry_attr_t *ent_bk_arr;

    /*! Table list pointer.  */
    shr_itbm_list_hdl_t list_hdl;
} bcmcth_trunk_vp_tbl_prop_t;

/*!
 * \brief Structure for TRUNK_VP hardware table information.
 */
typedef struct bcmcth_trunk_vp_reg_prop_s {

    /*! Device Resouce Database register Symbol ID */
    bcmdrd_sid_t drd_sid;

    /*! Logical table Database Symbol ID. */
    bcmltd_sid_t ltd_sid;

    /*! Register entry size in words. */
    uint32_t ent_size;

    /*! TRUNK_VP resolution mode DRD field identifier. */
    bcmdrd_fid_t mode_fid;

    /*! Device TRUNK_VP resolution mode hardware encoding value.  */
    uint32_t mode_hw_val[BCMCTH_TRUNK_VP_MODE_COUNT];
} bcmcth_trunk_vp_reg_prop_t;

/*!
 * \brief Structure for TRUNK_VP hardware table information.
 */
typedef struct bcmcth_trunk_vp_tbl_op_s {

    /*! Table pointer. */
    bcmcth_trunk_vp_tbl_prop_t *tbl_ptr;

    /*! Type of Operation. */
    uint32_t oper_flags;

    /*! Number of entries to be allocated. */
    uint32_t num_ent;

    /*! Allocated entry index. */
    int free_idx;
} bcmcth_trunk_vp_tbl_op_t;

/*!
 * \brief Structure for TRUNK_VP group information.
 */
typedef struct bcmcth_trunk_vp_grp_info_s {

    /*! Transaction identifer related to this group. */
    uint32_t trans_id;

    /*! Logical table entry group type - BCMCTH_TRUNK_VP_GRP_TYPE_XXX . */
    bcmcth_trunk_vp_grp_type_t grp_type;

    /*! Group logial table source identifier. */
    bcmltd_sid_t glt_sid;

    /*! Load balancing mode configured for this group. */
    uint32_t lb_mode;

    /*! Maximum TRUNK_VP paths supported by this group. */
    uint32_t max_members;

    /*! Number of TRUNK_VP paths configured for this group. */
    uint32_t member_cnt;

    /*! Member table start index for this group. */
    int mstart_idx;
} bcmcth_trunk_vp_grp_info_t;

/*!
 * \brief Structure for TRUNK_VP group and hardware tables information.
 */
typedef struct bcmcth_trunk_vp_info_s {

    /*! Device TRUNK_VP mode. */
    uint32_t trunk_vp_mode;

    /*! Device TRUNK_VP config register information. */
    bcmcth_trunk_vp_reg_prop_t config;

    /*! Post-IFP TRUNK_VP group table information. */
    bcmcth_trunk_vp_tbl_prop_t group[BCMCTH_TRUNK_VP_GRP_TYPE_COUNT];

    /*! Post-IFP TRUNK_VP member table information. */
    bcmcth_trunk_vp_tbl_prop_t member[BCMCTH_TRUNK_VP_GRP_TYPE_COUNT];

    /*! Post-IFP TRUNK_VP member 2 table information. */
    bcmcth_trunk_vp_tbl_prop_t member2[BCMCTH_TRUNK_VP_GRP_TYPE_COUNT];

    /*! Widest entry size among all TRUNK_VP hardware tables. */
    uint32_t ent_wsize_max;

    /*! TRUNK_VP entries queued in WAL information.  */
    bcmcth_trunk_vp_wal_info_t *wali;

    /*! TRUNK_VP logical table entry information. */
    bcmcth_trunk_vp_lt_entry_t *lt_ent;

    /*! TRUNK_VP current logical table entry information. */
    bcmcth_trunk_vp_lt_entry_t *current_lt_ent;

    /*! TRUNK_VP group information. */
    bcmcth_trunk_vp_grp_attr_t *grp_arr;

    /*! TRUNK_VP group entires backup array. */
    bcmcth_trunk_vp_grp_attr_t *grp_bk_arr;

} bcmcth_trunk_vp_info_t;

/*!
 * \brief Structure for TRUNK_VP control information.
 */
typedef struct bcmcth_trunk_vp_control_s {

    /*! TRUNK_VP feature initalized status indicator. */
    bool init;

    /*! Use HA memory for storing TRUNK_VP CTH software state control. */
    bool use_ha_mem;

    /*! Use DMA engine for TRUNK_VP hardware tables programming control. */
    bool use_dma;

    /*! TRUNK_VP atomic transactions support feature control. */
    bool atomic_trans;

    /*! Mutex lock for synchronization. */
    sal_mutex_t trunk_vp_lock;

    /*! TRUNK_VP module information. */
    bcmcth_trunk_vp_info_t trunk_vp_info;
} bcmcth_trunk_vp_control_t;

/*!
 * \brief Get per-BCM unit TRUNK_VP control structure.
 */
bcmcth_trunk_vp_control_t *bcmcth_trunk_vp_ctrl_get(int unit);

/*!
 * \brief Get BCM unit TRUNK_VP control structure pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Pointer to BCM unit's \c _u TRUNK_VP Control Structure.
 */
#define BCMCTH_TRUNK_VP_CTRL(_u) (bcmcth_trunk_vp_ctrl_get(_u))

/*!
 * \brief Get TRUNK_VP feature initialized status.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP feature status on the BCM unit.
 */
#define BCMCTH_TRUNK_VP_INIT(_u)                         \
            if ((BCMCTH_TRUNK_VP_CTRL(_u)) == NULL ||      \
                !((BCMCTH_TRUNK_VP_CTRL(_u))->init)) {     \
                SHR_ERR_EXIT(SHR_E_INIT); \
            }

/*!
 * \brief Set or get the use HA memory
 * for storing TRUNK_VP software state control.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Status of use HA memory for storing TRUNK_VP software state control.
 */
#define BCMCTH_TRUNK_VP_USE_HA_MEM(_u) ((BCMCTH_TRUNK_VP_CTRL(_u))->use_ha_mem)

/*!
 * \brief Set or get use DMA engine for programming TRUNK_VP tables control.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Status of use DMA engine for programming TRUNK_VP tables control.
 */
#define BCMCTH_TRUNK_VP_USE_DMA(_u) ((BCMCTH_TRUNK_VP_CTRL(_u))->use_dma)

/*!
 * \brief Set or get TRUNK_VP atomic transaction feature support status.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP atomic transaction feature support status.
 */
#define BCMCTH_TRUNK_VP_ATOMIC_TRANS(_u) \
        ((BCMCTH_TRUNK_VP_CTRL(_u))->atomic_trans)

/*!
 * \brief Take the TRUNK_VP control structure lock of this BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns None.
 */
#define BCMCTH_TRUNK_VP_LOCK(_u) \
        (sal_mutex_take(BCMCTH_TRUNK_VP_CTRL(_u)->trunk_vp_lock, \
         SAL_MUTEX_FOREVER))

/*!
 * \brief Give TRUNK_VP control structure lock of this BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns None.
 */
#define BCMCTH_TRUNK_VP_UNLOCK(_u) \
            (sal_mutex_give(BCMCTH_TRUNK_VP_CTRL(_u)->trunk_vp_lock))

/*!
 * \brief Get TRUNK_VP information structure for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP information structure of the unit.
 */
#define BCMCTH_TRUNK_VP_INFO(_u) (BCMCTH_TRUNK_VP_CTRL(_u)->trunk_vp_info)

/*!
 * \brief Get TRUNK_VP information structure pointer for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP information structure pointer of the unit.
 */
#define BCMCTH_TRUNK_VP_INFO_PTR(_u) \
        (&(BCMCTH_TRUNK_VP_CTRL(_u)->trunk_vp_info))

/*!
 * \brief Get TRUNK_VP LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP LT Entry buffer pointer.
 */
#define BCMCTH_TRUNK_VP_LT_ENT_PTR(_u) ((BCMCTH_TRUNK_VP_INFO_PTR(_u))->lt_ent)

/*!
 * \brief Get TRUNK_VP current LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP LT Entry buffer pointer.
 */
#define BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(_u) \
           ((BCMCTH_TRUNK_VP_INFO_PTR(_u))->current_lt_ent)


/*!
 * \brief Get TRUNK_VP entries in WAL information.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP entries in WAL details.
 */
#define BCMCTH_TRUNK_VP_WALI_PTR(_u) ((BCMCTH_TRUNK_VP_INFO_PTR(_u))->wali)

/*!
 * \brief Get TRUNK_VP table structure pointer for a given table name.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Table name.
 * \param [in] _var Table name.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Pointer to the TRUNK_VP table structure.
 */
#define BCMCTH_TRUNK_VP_TBL_PTR(_u, _var, _t)  \
      (&(BCMCTH_TRUNK_VP_INFO(_u)._var[_t]))
/*!
 * \brief Set/Get the minimum entry index value supported for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Minimum entry index value supported by the table.
 */
#define BCMCTH_TRUNK_VP_TBL_IDX_MIN(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[(_t)]).imin)

/*!
 * \brief Set/Get the maximum entry index value supported for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Maximum entry index value supported by the table.
 */
#define BCMCTH_TRUNK_VP_TBL_IDX_MAX(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[(_t)]).imax)

/*!
 * \brief Set/Get the maximum entry index value in use for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Maximum entry index value in use for the table.
 */
#define BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[(_t)]).imax_used)

/*!
 * \brief Set/Get the maximum entry ID value supported by a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Maximum entry ID value supported by the table.
 */
#define BCMCTH_TRUNK_VP_TBL_SIZE(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[_t]).tbl_size)

/*!
 * \brief Set/Get DRD symbol identifier value of a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns DRD symbol identifier of the table.
 */
#define BCMCTH_TRUNK_VP_TBL_DRD_SID(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[(_t)]).drd_sid)

/*!
 * \brief Set/Get DRD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table Name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns DRD symbol identifier values of fields in the table.
 */
#define BCMCTH_TRUNK_VP_TBL_DRD_FIELDS(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[(_t)]).drd_fids)

/*!
 * \brief Set/Get the LTD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table Name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns LTD symbol identifier values of fields in the table.
 */
#define BCMCTH_TRUNK_VP_TBL_LTD_FIELDS(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[(_t)]).ltd_fids)

/*!
 * \brief Set/Get the LTD source identifier value of a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns LTD source identifier value of the table.
 */
#define BCMCTH_TRUNK_VP_TBL_LTD_SID(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[(_t)]).ltd_sid)

/*!
 * \brief Set/Get the entry size in words for a given physical table/memory.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Entry size in words of the table/memory.
 */
#define BCMCTH_TRUNK_VP_TBL_ENT_SIZE(_u, _var, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._var[(_t)]).ent_size)

/*!
 * \brief Set/Get maximum entry size in words among all TRUNK_VP table entries.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum entry size in words among all TRUNK_VP table entries.
 */
#define BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(_u) \
       (BCMCTH_TRUNK_VP_INFO(_u).ent_wsize_max)

/*!
 * \brief TRUNK_VP group logical table field
 * attributes structure void * pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Void * pointer to logical table field attributes structure.
 */
#define BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_VOID_PTR(_u, _g, _t) \
            ((BCMCTH_TRUNK_VP_INFO(_u)._g[(_t)]).lt_fattrs)

/*!
 * \brief TRUNK_VP group logical table field attributes structure pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Pointer to TRUNK_VP Group logical table field attributes structure.
 */
#define BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t) \
     ((bcmcth_trunk_vp_lt_field_attrs_t*) \
      ((BCMCTH_TRUNK_VP_INFO(_u)._g[(_t)]).lt_fattrs))

/*!
 * \brief Minimum value supported for TRUNK_VP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Minimum TRUNK_VP ID value supported by the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_trunk_vp_id)

/*!
 * \brief Maximum value supported for TRUNK_VP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Maximum TRUNK_VP ID value supported by the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_trunk_vp_id)

/*!
 * \brief Minimum value supported for LB_MODE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Minimum LB_MODE value supported by the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MIN_LB_MODE(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_lb_mode)

/*!
 * \brief Maximum value supported for LB_MODE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Maximum LB_MODE value supported by the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MAX_LB_MODE(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_lb_mode)

/*!
 * \brief LB_MODE name for a given LB_MODE enum value of the logical table.
 *
 * \param [in] _u   BCM unit number.
 * \param [in] _g   TRUNK_VP Group table array of
 *                  bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t   TRUNK_VP Group type.
 * \param [in] _lbm Load balancing enum value.
 *
 * \returns LB_MODE name for the given LB_MODE enum value.
 */
#define BCMCTH_TRUNK_VP_LT_LB_MODE(_u, _g, _t, _lbm) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->lb_mode[(_lbm)])
/*!
 * \brief Paths count (MAX_MEMBERS & MEMBER_CNT) must be in powers-of-two.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Powers-of-two paths count requirement status.
 */
#define BCMCTH_TRUNK_VP_LT_POW_TWO_PATHS(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->pow_two_paths)

/*!
 * \brief Minimum value supported for MAX_MEMBERS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Minimum value supported for MAX_MEMBERS field of the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MIN_MEMBERS(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_members)

/*!
 * \brief Maximum value supported for MAX_MEMBERS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Maximum value supported for MAX_MEMBERS field of the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MAX_MEMBERS(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_members)
/*!
 * \brief Minimum value supported for MEMBER_CNT field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Minimum value supported for MAX_MEMBERS field of the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MIN_NMEMBERS(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_nmembers)

/*!
 * \brief Maximum value supported for MEMBER_CNT field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Maximum value supported for MAX_MEMBERS field of the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MAX_NMEMBERS(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_nmembers)

/*!
 * \brief TRUNK_VP group array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP group array base pointer.
 */
#define BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u) \
        ((BCMCTH_TRUNK_VP_INFO_PTR(_u))->grp_arr)

/*!
 * \brief Group TRUNK_VP resolution type.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns TRUNK_VP resolution type of this group.
 */
#define BCMCTH_TRUNK_VP_GRP_TYPE(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u))[(_id)].grp_type)

/*!
 * \brief TRUNK_VP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns TRUNK_VP group logical table source identifier value.
 */
#define BCMCTH_TRUNK_VP_GRP_LT_SID(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief Get the load balancing mode of a given TRUNK_VP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Load balancing mode of the TRUNK_VP group.
 */
#define BCMCTH_TRUNK_VP_GRP_LB_MODE(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u))[(_id)].lb_mode)

/*!
 * \brief Maximum TRUNK_VP paths supported by this TRUNK_VP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Maximum TRUNK_VP paths supported by the group.
 */
#define BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u))[(_id)].max_members)

/*!
 * \brief Number of TRUNK_VP Paths configured for a given TRUNK_VP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Number of TRUNK_VP paths configured for the group.
 */
#define BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u))[(_id)].member_cnt)

/*!
 * \brief Start index of a given group in TRUNK_VP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Start index of the TRUNK_VP group in TRUNK_VP member table.
 */
#define BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u))[(_id)].mstart_idx)

/*!
 * \brief Itbm use flag for TRUNK_VP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Number of TRUNK_VP paths configured for the group.
 */
#define BCMCTH_TRUNK_VP_GRP_ITBM_USE(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u))[(_id)].in_use_itbm)


/*!
 * \brief TRUNK_VP group entries backup array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns TRUNK_VP group high availability data array base pointer.
 */
#define BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u) \
           ((BCMCTH_TRUNK_VP_INFO_PTR(_u))->grp_bk_arr)

/*!
 * \brief TRUNK_VP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns TRUNK_VP group logical table transaction identifier value.
 */
#define BCMCTH_TRUNK_VP_GRP_TRANS_ID(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_PTR(_u))[(_id)].trans_id)

/*!
 * \brief TRUNK_VP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns TRUNK_VP group logical table transaction identifier value.
 */
#define BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u))[(_id)].trans_id)
/*!
 * \brief Group TRUNK_VP resolution type.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns TRUNK_VP resolution type of this group.
 */
#define BCMCTH_TRUNK_VP_GRP_TYPE_BK(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u))[(_id)].grp_type)

/*!
 * \brief TRUNK_VP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns TRUNK_VP group logical table source identifier value.
 */
#define BCMCTH_TRUNK_VP_GRP_LT_SID_BK(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief Get the load balancing mode of a given TRUNK_VP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Load balancing mode of the TRUNK_VP group.
 */
#define BCMCTH_TRUNK_VP_GRP_LB_MODE_BK(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u))[(_id)].lb_mode)

/*!
 * \brief Maximum TRUNK_VP paths supported by this TRUNK_VP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Maximum TRUNK_VP paths supported by the group.
 */
#define BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS_BK(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u))[(_id)].max_members)

/*!
 * \brief Number of TRUNK_VP Paths configured for a given TRUNK_VP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Number of TRUNK_VP paths configured for the group.
 */
#define BCMCTH_TRUNK_VP_GRP_MEMBER_CNT_BK(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u))[(_id)].member_cnt)

/*!
 * \brief Start index of a given group in TRUNK_VP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Start index of the TRUNK_VP group in TRUNK_VP member table.
 */
#define BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX_BK(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u))[(_id)].mstart_idx)

/*!
 * \brief Itbm use flag for TRUNK_VP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id TRUNK_VP group identifier.
 *
 * \returns Number of TRUNK_VP paths configured for the group.
 */
#define BCMCTH_TRUNK_VP_GRP_ITBM_USE_BK(_u, _id) \
            ((BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(_u))[(_id)].in_use_itbm)


/*!
 * \brief Set the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_ENT_STAGED_SET(_tptr, _i)    \
            (((_tptr)->ent_arr[(_i)]).flags |= (BCMCTH_TRUNK_VP_ENTRY_STAGED))

/*!
 * \brief Clear the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_ENT_STAGED_CLEAR(_tptr, _i)    \
            (((_tptr)->ent_arr[(_i)]).flags &= (~BCMCTH_TRUNK_VP_ENTRY_STAGED))

/*!
 * \brief Get entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_ENT_STAGED_GET(_tptr, _i)    \
            (((_tptr)->ent_arr[(_i)]).flags & (BCMCTH_TRUNK_VP_ENTRY_STAGED))

/*!
 * \brief Set the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_SET(_tptr, _i)    \
     (((_tptr)->ent_bk_arr[(_i)]).flags |= (BCMCTH_TRUNK_VP_ENTRY_STAGED))

/*!
 * \brief Clear the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_CLEAR(_tptr, _i)    \
     (((_tptr)->ent_bk_arr[(_i)]).flags &= (~BCMCTH_TRUNK_VP_ENTRY_STAGED))

/*!
 * \brief Get entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_BK_ENT_STAGED_GET(_tptr, _i)    \
            (((_tptr)->ent_bk_arr[(_i)]).flags & (BCMCTH_TRUNK_VP_ENTRY_STAGED))

/*!
 * \brief Set the entry defragment staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry defragment staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_ENT_DEFRAG_STAGED_SET(_tptr, _i)    \
            (((_tptr)->ent_arr[(_i)]).flags |= (BCMCTH_TRUNK_VP_ENTRY_DEFRAG_STAGED))

/*!
 * \brief Clear the entry defragment staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry defragment staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_ENT_DEFRAG_STAGED_CLEAR(_tptr, _i)    \
            (((_tptr)->ent_arr[(_i)]).flags &= (~BCMCTH_TRUNK_VP_ENTRY_DEFRAG_STAGED))

/*!
 * \brief Get entry defragment staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry degragment staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_ENT_DEFRAG_STAGED_GET(_tptr, _i)    \
            (((_tptr)->ent_arr[(_i)]).flags & (BCMCTH_TRUNK_VP_ENTRY_DEFRAG_STAGED))

/*!
 * \brief Set the entry defragment staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry defragment staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_SET(_tptr, _i)    \
     (((_tptr)->ent_bk_arr[(_i)]).flags |= (BCMCTH_TRUNK_VP_ENTRY_DEFRAG_STAGED))

/*!
 * \brief Clear the entry defragment staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry defragment staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_CLEAR(_tptr, _i)    \
     (((_tptr)->ent_bk_arr[(_i)]).flags &= (~BCMCTH_TRUNK_VP_ENTRY_DEFRAG_STAGED))

/*!
 * \brief Get entry defragment staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry defragment staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_GET(_tptr, _i)    \
            (((_tptr)->ent_bk_arr[(_i)]).flags & (BCMCTH_TRUNK_VP_ENTRY_DEFRAG_STAGED))

/*!
 * \brief Set the entry pre-config status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry pre-config status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_ENT_PRECONFIG_SET(_tptr, _i)    \
     (((_tptr)->ent_arr[(_i)]).flags |= (BCMCTH_TRUNK_VP_ENTRY_PRECONFIG))

/*!
 * \brief Clear the entry pre-config status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry pre-config status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMCTH_TRUNK_VP_ENT_PRECONFIG_CLEAR(_tptr, _i)    \
     (((_tptr)->ent_arr[(_i)]).flags &= (~BCMCTH_TRUNK_VP_ENTRY_PRECONFIG))

/*!
 * \brief Get entry pre-config status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry pre-config status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry pre-config status.
 * table.
 */
#define BCMCTH_TRUNK_VP_ENT_PRECONFIG_GET(_tptr, _i)    \
            (((_tptr)->ent_arr[(_i)]).flags & (BCMCTH_TRUNK_VP_ENTRY_PRECONFIG))


/*!
 * \brief Get entry pre-config status.
 *
 * Given a table base pointer and the unit, table type,
 * get the list corresponding to the group / member table.
 *
 * \param [in] _tptr Table base pointer.
 *
 * \returns list corresponding to the group / member table.
 * table.
 */
#define BCMCTH_TRUNK_VP_TBL_ITBM_LIST(_tptr)    \
            (((_tptr)->list_hdl))

/*!
 * \brief Set/Get the entry size in words for a given TRUNK_VP register.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware register name of bcmcth_trunk_vp_reg_prop_t type.
 *
 * \returns Given TRUNK_VP register entry size in words.
 */
#define BCMCTH_TRUNK_VP_REG_ENT_SIZE(_u, _var) \
            (BCMCTH_TRUNK_VP_INFO(_u)._var.ent_size)

/*!
 * \brief Set/Get TRUNK_VP config register DRD symbol identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var TRUNK_VP config register
 *  name of bcmcth_trunk_vp_reg_prop_t type.
 *
 * \returns TRUNK_VP config register DRD symbol identifier value.
 */
#define BCMCTH_TRUNK_VP_REG_DRD_SID(_u, _var) \
            (BCMCTH_TRUNK_VP_INFO(_u)._var.drd_sid)

/*!
 * \brief Set/Get TRUNK_VP config register LTD source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var TRUNK_VP config register
 * name of bcmcth_trunk_vp_reg_prop_t type.
 *
 * \returns TRUNK_VP config register LTD source identifier value.
 */
#define BCMCTH_TRUNK_VP_REG_LTD_SID(_u, _var) \
            (BCMCTH_TRUNK_VP_INFO(_u)._var.ltd_sid)

/*!
 * \brief Set/Get device TRUNK_VP mode field DRD field identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var TRUNK_VP config register
 * name of bcmcth_trunk_vp_reg_prop_t type.
 *
 * \returns TRUNK_VP config register LTD source identifier value.
 */
#define BCMCTH_TRUNK_VP_MODE_DRD_FID(_u, _var) \
            (BCMCTH_TRUNK_VP_INFO(_u)._var.mode_fid)

/*!
 * \brief Set/Get device TRUNK_VP resolution mode hardware encoding value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var TRUNK_VP config register
 * name of bcmcth_trunk_vp_reg_prop_t type.
 * \param [in] _m Software TRUNK_VP mode value.
 *
 * \returns Device TRUNK_VP resolution mode hardware encoding value.
 */
#define BCMCTH_TRUNK_VP_MODE_HW_VAL(_u, _var, _m) \
            (BCMCTH_TRUNK_VP_INFO(_u)._var.mode_hw_val[(_m)])

/*!
 * \brief Set/Get device current TRUNK_VP resolution mode software value.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Device TRUNK_VP resolution mode software value.
 */
#define BCMCTH_TRUNK_VP_MODE(_u) \
            (BCMCTH_TRUNK_VP_INFO(_u).trunk_vp_mode)

/*!
 * \brief Minimum value supported for weighted TRUNK_VP group size field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Minimum weighted TRUNK_VP group size value
 * supported by the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MIN_WEIGHTED_SIZE(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_weighted_size)

/*!
 * \brief Maximum value supported for weighted TRUNK_VP group size field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g TRUNK_VP Group table array of bcmcth_trunk_vp_tbl_prop_t type.
 * \param [in] _t TRUNK_VP Group type.
 *
 * \returns Maximum weighted TRUNK_VP group size value
 * supported by the logical table.
 */
#define BCMCTH_TRUNK_VP_LT_MAX_WEIGHTED_SIZE(_u, _g, _t) \
            (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_weighted_size)

static inline uint32_t bcmcth_trunk_vp_member_cnt_convert_to_weighted_size
                           (uint32_t member_cnt) {
    uint32_t weighted_size;

    switch (member_cnt) {
        case 256:
            weighted_size = BCMCTH_TRUNK_VP_WEIGHTED_SIZE_256;
            break;
        case 512:
            weighted_size = BCMCTH_TRUNK_VP_WEIGHTED_SIZE_512;
            break;
        case 1024:
            weighted_size = BCMCTH_TRUNK_VP_WEIGHTED_SIZE_1K;
            break;
        case 2048:
            weighted_size = BCMCTH_TRUNK_VP_WEIGHTED_SIZE_2K;
            break;
        case 4096:
            weighted_size = BCMCTH_TRUNK_VP_WEIGHTED_SIZE_4K;
            break;
        case 8192:
            weighted_size = BCMCTH_TRUNK_VP_WEIGHTED_SIZE_8K;
            break;
        case 16384:
            weighted_size = BCMCTH_TRUNK_VP_WEIGHTED_SIZE_16K;
            break;
        default:
            weighted_size = BCMCTH_TRUNK_VP_WEIGHTED_SIZE_COUNT;
            break;
    }

    return weighted_size;
}

static inline uint32_t bcmcth_trunk_vp_weighted_size_convert_to_member_cnt
                           (uint32_t weighted_size) {
    uint32_t member_cnt;

    switch (weighted_size) {
        case BCMCTH_TRUNK_VP_WEIGHTED_SIZE_256:
            member_cnt = 256;
            break;
        case BCMCTH_TRUNK_VP_WEIGHTED_SIZE_512:
            member_cnt = 512;
            break;
        case BCMCTH_TRUNK_VP_WEIGHTED_SIZE_1K:
            member_cnt = 1024;
            break;
        case BCMCTH_TRUNK_VP_WEIGHTED_SIZE_2K:
            member_cnt = 2048;
            break;
        case BCMCTH_TRUNK_VP_WEIGHTED_SIZE_4K:
            member_cnt = 4096;
            break;
        case BCMCTH_TRUNK_VP_WEIGHTED_SIZE_8K:
            member_cnt = 8192;
            break;
        case BCMCTH_TRUNK_VP_WEIGHTED_SIZE_16K:
            member_cnt = 16*1024;
            break;
        default:
            member_cnt = 0;
            break;
    }

    return member_cnt;
}
#endif /* BCMCTH_TRUNK_VP_DB_INTERNAL_H */
