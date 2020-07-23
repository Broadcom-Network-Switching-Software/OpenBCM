/*! \file bcmecmp.h
 *
 * Top-level ECMP device functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_H
#define BCMECMP_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmecmp/bcmecmp_types.h>
#include <bcmimm/bcmimm_int_comp.h>


/*! \brief Max bits number of flag BMP. */
#define BCMECMP_FLAG_BMP_MAX_BIT 64

/*!
 * \brief Structure for ECMP physical table operation.
 *
 * This structure is used to supply PTM API attributes used during ECMP logical
 * table entry Read/Write operations to hardware/physical tables.
 */
typedef struct bcmecmp_pt_op_info_s {

    /*! Device Resource Database Symbol ID. */
    bcmdrd_sid_t drd_sid;

    /*! Request Logical Table Source ID information. */
    bcmltd_sid_t req_lt_sid;

    /*! Response Logical Table Source ID information. */
    bcmltd_sid_t rsp_lt_sid;

    /*! Logical table operation transaction identifier. */
    uint32_t trans_id;

    /*! Request PTM API option flags. */
    uint64_t req_flags;

    /*! Response PTM API option flags. */
    uint32_t rsp_flags;

    /*! PTM operation type. */
    bcmptm_op_type_t op;

    /*! Physical Table Entry Size in number of words. */
    size_t wsize;

    /*! Enable DMA. */
    bool dma;

    /*! Total number of entries for DMA (SLAM/write or TDMA/read) operation. */
    uint32_t ecount;

    /*!
     * Dynamic address information of the physical table.
     * Physical Table Entry Hardware Index and Pipe Instances to Install Info.
     */
    bcmbd_pt_dyn_info_t dyn_info;

    /*!
     * Operational argument.
     */
    const bcmltd_op_arg_t *op_arg;
} bcmecmp_pt_op_info_t;

/*! ECMP device specific data initialization function. */
typedef int (*fn_info_init_t)(int unit, bool warm);

/*! ECMP device specific data cleanup function. */
typedef int (*fn_info_cleanup_t)(int unit);

/*! Configures device ECMP resolution mode. */
typedef int (*fn_mode_config_t)(int unit);

/*! Configures device ECMP resolution mode. */
typedef int (*fn_rh_pre_config_t)(int unit);

/*! Install an ECMP group LT entry into the hardware tables. */
typedef int (*fn_grp_ins_t)(int unit, void *info);

/*! Delete an ECMP group LT entry from the hardware tables. */
typedef int (*fn_grp_del_t)(int unit, void *info);

/*! Get an ECMP group LT entry field values from the hardware tables. */
typedef int (*fn_grp_get_t)(int unit, void *info);

/*! Install a RH-ECMP group LT entry into the hardware tables. */
typedef int (*fn_rh_grp_ins_t)(int unit, void *info);

/*! Delete a RH-ECMP group LT entry from the hardware tables. */
typedef int (*fn_rh_grp_del_t)(int unit, void *info);

/*! Get a RH-ECMP group LT entry field values from the hardware tables. */
typedef int (*fn_rh_grp_get_t)(int unit, void *info);

/*! Copy ECMP member table entries pointed by msrc to mdest region. */
typedef int (*fn_memb_cpy_t)(int unit, const void *mdest, const void *msrc);

/*! Clear ECMP member table entries pointed by mfrag. */
typedef int (*fn_memb_clr_t)(int unit, const void *mfrag);

/*! Update ECMP group's member table start index for the group in ginfo. */
typedef int (*fn_gmstart_updt_t)(int unit, const void *ginfo);

/*! Index width of member weight group. */
typedef int (*fn_member_weight_index_width_t)(void);

typedef int (*fn_lt_fields_fill_t)(int unit,
                                   const void *field,
                                   void *lt_entry);
/*! Get count of member fields in LT. */
typedef int (*fn_lt_fields_count_t)(int unit,
                                    void *lt_entry);
/*! Get the group type of shared HW mem partition. */
typedef bcmecmp_grp_type_t (*fn_shr_grp_type_get_t)(bcmecmp_grp_type_t gtype);
/*! Get the max paths from weighted size. */
typedef uint32_t (*fn_weight_size_to_max_paths_t)(uint32_t weight_size);
/*! Update the itbm member lists. */
typedef int (*fn_itbm_memb_lists_update_t)(int unit, uint8_t num_o_memb_bank);
/*! Set the ECMP member bank config. */
typedef int (*fn_memb_bank_set_t)(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmltd_sid_t lt_id,
                                    void *l3_bank);
/*! Get the ECMP member bank config. */
typedef int (*fn_memb_bank_get_t)(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmltd_sid_t lt_id,
                                    void *l3_bank);

/*!
 * \brief Structure for ECMP device specific function list.
 */
typedef struct bcmecmp_drv_s {

    /*! Device type. */
    bcmdrd_dev_type_t dev_type;

    /*! ECMP device specific data initialization function. */
    fn_info_init_t info_init;

    /*! ECMP device specific data cleanup function. */
    fn_info_cleanup_t info_cleanup;

    /*! ECMP device specific data cleanup function. */
    fn_rh_pre_config_t rh_pre_config;

    /*! Configures device ECMP resolution mode. */
    fn_mode_config_t mode_config;

    /*! Install an ECMP group LT entry into the hardware tables. */
    fn_grp_ins_t grp_ins;

    /*! Delete an ECMP group LT entry from the hardware tables. */
    fn_grp_del_t grp_del;

    /*! Get an ECMP group LT entry field values from the hardware tables. */
    fn_grp_get_t grp_get;

    /*! Install a RH-ECMP group LT entry into the hardware tables. */
    fn_rh_grp_ins_t rh_grp_ins;

    /*! Delete a RH-ECMP group LT entry from the hardware tables. */
    fn_rh_grp_del_t rh_grp_del;

    /*! Get a RH-ECMP group LT entry field values from the hardware tables. */
    fn_rh_grp_get_t rh_grp_get;

    /*! Copy ECMP member table entries from src block to dest block. */
    fn_memb_cpy_t memb_cpy;

    /*! Clear ECMP member table entries block pointed by memb_blk. */
    fn_memb_clr_t memb_clr;

    /*! Update ECMP group member table start/base index. */
    fn_gmstart_updt_t gmstart_updt;

    /*! Index width of member weight group. */
    fn_member_weight_index_width_t member_weight_index_width;

    /*! Fill lt fields for overlay LT. */
    fn_lt_fields_fill_t grp_ol_lt_fields_fill;

    /*! Fill lt fields for underlay LT. */
    fn_lt_fields_fill_t grp_ul_lt_fields_fill;

    /*! Fill lt fields for member_dest LT. */
    fn_lt_fields_fill_t grp_member_dest_lt_fields_fill;

    /*! Get count of member fields in LT. */
    fn_lt_fields_count_t grp_member_fields_count_get;

    /*! Get the group types of ITBM lists sharing a HW mem partition. */
    fn_shr_grp_type_get_t shr_grp_type_get;

    /*! Get the max paths from weighted size. */
    fn_weight_size_to_max_paths_t weight_size_to_max_paths;

    /*! Update the itbm member lists. */
    fn_itbm_memb_lists_update_t itbm_memb_lists_update;

    /*! Set the ECMP member bank config. */
    fn_memb_bank_set_t memb_bank_set;

    /*! Get the ECMP member bank config. */
    fn_memb_bank_get_t memb_bank_get;
} bcmecmp_drv_t;

/*!
 * \brief Structure for storing information on one ECMP member table field.
 */
typedef struct bcmecmp_member_field_info_s {
    /*! Field name in ECMP group LT */
    uint16_t grp_fld;

    /*! Field name in ECMP member HW table */
    uint16_t mbr_fld;

    /*! Start bit of the field in ECMP HW table */
    uint16_t start_bit;

    /*! End bit of the field in ECMP HW table */
    uint16_t end_bit;
}bcmecmp_member_field_info_t;

/*!
 * \brief Structure for storing information on ECMP HW table fields.
 */
typedef struct bcmecmp_member_info_s {
    /*! Name of ECMP group LT */
    uint32_t grp_tbl;

    /*! Number of ECMP member tables */
    uint16_t mbr_tbl_count;

    /*! Array of member tables */
    const uint32_t *mbr_tbl;

    /*! Count of member fields */
    uint16_t flds_count;
    /*! Array of fields in member tables */
    const bcmecmp_member_field_info_t *flds;
}bcmecmp_member_info_t;

/*!
 * \brief Structure for ECMP device variant ID's.
 */
typedef struct bcmecmp_drv_var_ids_s {
    /* ECMP_LEVEL0 LT ID. */
    const uint32_t ecmp_level0_ltid;

    /* ECMP_LEVEL0 LT Field - ECMP_ID. */
    const uint32_t ecmp_level0_ecmp_id;

    /* ECMP_LEVEL0 LT Field - LB_MODE. */
    const uint32_t ecmp_level0_lb_mode_id;

    /* ECMP_LEVEL0 LT Field - MAX_PATH. */
    const uint32_t ecmp_level0_max_paths_id;

    /* ECMP_LEVEL0 LT Field - NUM_PATHS */
    const uint32_t ecmp_level0_num_paths_id;

    /* ECMP_LEVEL0_WEIGHTED LT ID. */
    const uint32_t ecmp_wlevel0_ltid;

    /* ECMP_LEVEL0_WEIGHTED LT Field - ECMP_ID. */
    const uint32_t ecmp_wlevel0_ecmp_id;

    /* ECMP_LEVEL0_WEIGHTED LT Field - WEIGHTED_SIZE. */
    const uint32_t ecmp_wlevel0_wt_size_id;

    /* ECMP_LEVEL0_WEIGHTED LT Field - NUM_PATHS. */
    const uint32_t ecmp_wlevel0_num_paths_id;

    /* ECMP_LEVEL0_WEIGHTED LT Field - RH_MODE. */
    const uint32_t ecmp_wlevel0_rh_mode_id;

    /* ECMP_LEVEL0_WEIGHTED LT Field - RH_RANDOM_SEED. */
    const uint32_t ecmp_wlevel0_rh_rand_seed_id;

    /* ECMP_LEVEL0_WEIGHTED LT Field - RH_NUM_PATHS. */
    const uint32_t ecmp_wlevel0_rh_num_paths_id;

    /* ECMP_MEMBER0 LT ID. */
    const uint32_t ecmp_mlevel0_ltid;

    /* ECMP_MEMBER0 LT Field - GROUP_ID. */
    const uint32_t ecmp_mlevel0_group_id;

    /* ECMP_MEMBER0 LT Field - NUM_ENTRIES. */
    const uint32_t ecmp_mlevel0_num_entries_id;

    /* ECMP_MEMBER0 LT Field - BASE_INDEX_AUTO. */
    const uint32_t ecmp_mlevel0_base_index_auto_id;

    /* ECMP_MEMBER0 LT Field - BASE_INDEX_OPER. */
    const uint32_t ecmp_mlevel0_base_index_oper_id;

    /* ECMP_MEMBER0 LT Field - BASE_INDEX. */
    const uint32_t ecmp_mlevel0_base_index_id;

    /* ECMP_MEMBER1 LT ID. */
    const uint32_t ecmp_mlevel1_ltid;

    /* ECMP_MEMBER1 LT Field - GROUP_ID. */
    const uint32_t ecmp_mlevel1_group_id;

    /* ECMP_MEMBER1 LT Field - NUM_ENTRIES. */
    const uint32_t ecmp_mlevel1_num_entries_id;

    /* ECMP_MEMBER1 LT Field - BASE_INDEX_AUTO. */
    const uint32_t ecmp_mlevel1_base_index_auto_id;

    /* ECMP_MEMBER1 LT Field - BASE_INDEX_OPER. */
    const uint32_t ecmp_mlevel1_base_index_oper_id;

    /* ECMP_MEMBER1 LT Field - BASE_INDEX. */
    const uint32_t ecmp_mlevel1_base_index_id;

    /* ECMP_LEVEL1 LT ID. */
    const uint32_t ecmp_level1_ltid;

    /* ECMP_LEVEL1 LT Field - ECMP_ID. */
    const uint32_t ecmp_level1_ecmp_id;

    /* ECMP_LEVEL1 LT Field - LB_MODE. */
    const uint32_t ecmp_level1_lb_mode_id;

    /* ECMP_LEVEL1 LT Field - MAX_PATHS. */
    const uint32_t ecmp_level1_max_paths_id;

    /* ECMP_LEVEL1 LT Field - NUM_PATHS. */
    const uint32_t ecmp_level1_num_paths_id;

    /* ECMP_LEVEL1_WEIGHTED LT ID. */
    const uint32_t ecmp_wlevel1_ltid;

    /* ECMP_LEVEL1_WEIGHTED LT Field - ECMP_ID. */
    const uint32_t ecmp_wlevel1_ecmp_id;

    /* ECMP_LEVEL1_WEIGHTED LT Field - WEIGHTED_SIZE. */
    const uint32_t ecmp_wlevel1_wt_size_id;

    /* ECMP_LEVEL1_WEIGHTED LT Field - NUM_PATHS. */
    const uint32_t ecmp_wlevel1_num_paths_id;

    /* ECMP_LEVEL0_WEIGHTED LT Field - RH_MODE. */
    const uint32_t ecmp_wlevel1_rh_mode_id;

    /* ECMP_LEVEL0_WEIGHTED LT Field - RH_RANDOM_SEED. */
    const uint32_t ecmp_wlevel1_rh_rand_seed_id;

    /* ECMP_LEVEL0_WEIGHTED LT Field - RH_NUM_PATHS. */
    const uint32_t ecmp_wlevel1_rh_num_paths_id;

    /* ECMP_LEVEL1_WEIGHTED LT Field - WEIGHT_MODE. */
    const uint32_t ecmp_wlevel1_wt_mode_id;

    /* ECMP_LEVEL1_WEIGHTED LT Field - WEIGHT. */
    const uint32_t ecmp_wlevel1_wt_id;

    /* ECMP_LEVEL1_WEIGHTED LT Field - NHOP. */
    const uint32_t ecmp_wlevel1_wt_nhop_index_id;
} bcmecmp_drv_var_ids_t;

/*!
 * \brief Structure for ECMP device variant specific function list.
 */
typedef struct bcmecmp_drv_var_s {
    /* Pointer to ECMP_LEVEL0 LT field's information. */
    const bcmecmp_member_field_info_t *flds_mem0;

    /* Pointer to ECMP_LEVEL1 LT field's information. */
    const bcmecmp_member_field_info_t *flds_mem1;

    /* Pointer to ECMP_LEVEL0_WEIGHTED LT field's information. */
    const bcmecmp_member_field_info_t *wflds_mem0;

    /* Pointer to ECMP_LEVEL1_WEIGHTED LT field's information. */
    const bcmecmp_member_field_info_t *wflds_mem1;

    /* Pointer to ECMP_MEMBER0 LT field's information. */
    const bcmecmp_member_field_info_t *dest_flds_mem0;

    /* Pointer to ECMP_MEMBER1 LT field's information. */
    const bcmecmp_member_field_info_t *dest_flds_mem1;

    /* Pointer to ECMP_LEVEL0 LT HW information. */
    const bcmecmp_member_info_t *member0_info;

    /* Pointer to ECMP_LEVEL1 LT HW information. */
    const bcmecmp_member_info_t *member1_info;

    /* Pointer to ECMP_LEVEL0_WEIGHTED LT HW information. */
    const bcmecmp_member_info_t *wmember0_info;

    /* Pointer to ECMP_LEVEL1_WEIGHTED LT HW information. */
    const bcmecmp_member_info_t *wmember1_info;

    /* Pointer to ECMP_MEMBER0 LT HW information. */
    const bcmecmp_member_info_t *dest_member0_info;

    /* Pointer to ECMP_MEMBER1 LT HW information. */
    const bcmecmp_member_info_t *dest_member1_info;

    /* Pointer to ECMP memory HW information. */
    const uint32_t *ecmp_mem0;

    /* Pointer to ECMP memory HW information. */
    const uint32_t *ecmp_mem1;

    /* Pointer to ECMP memory HW information. */
    const uint32_t *dest_ecmp_mem0;

    /* Pointer to ECMP memory HW information. */
    const uint32_t *dest_ecmp_mem1;

    /* Pointer to ECMP device variant IDS's. */
    const bcmecmp_drv_var_ids_t *ids;
} bcmecmp_drv_var_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_ecmp_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmecmp_attach(int unit);

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_ecmp_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated by \ref bcmecmp_attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmecmp_detach(int unit);

/*!
 * \brief Get pointer to ECMP base driver device structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern bcmecmp_drv_t *
bcmecmp_drv_get(int unit);

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_ecmp_drv_var_attach(int unit, bcmecmp_drv_var_t *drv_var);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Get pointer to ECMP base driver device structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern bcmecmp_drv_var_t *
bcmecmp_drv_var_get(int unit);

#endif /* BCMECMP_H */
