/*! \file bcmecmp_flex_db_internal.h
 *
 * This file contains ECMP flexible custom table handler data
 * structure definitions for logical tables defined for flexible device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_FLEX_DB_INTERNAL_H
#define BCMECMP_FLEX_DB_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_itbm.h>
#include <sal/sal_mutex.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmecmp/bcmecmp.h>
#include <bcmecmp/generated/bcmecmp_ha.h>
#include <bcmecmp/generated/bcmecmp_member_dest_ha.h>
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
#define BCMECMP_FLEX_LT_FIELD_SET(_m, _f)    \
            do {                        \
                SHR_BITSET(_m, _f);      \
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
#define BCMECMP_FLEX_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))


/*! \brief The max number of the member fields. */
#define BCMECMP_MEMBER_FIELD_ARRAY_SIZE 16384

/*! \brief The max member count. */
#define BCMECMP_MEMBER_COUNT 4

/*!
 * \brief Structure for ECMP logical table entry information.
 */
typedef struct bcmecmp_flex_lt_entry_s {

    /*! ECMP group logial table source identifier. */
    bcmltd_sid_t glt_sid;

    /*! Operational argument. */
    const bcmltd_op_arg_t *op_arg;

    /*! Logical table entry field set/present indicator Bit Map. */
    SHR_BITDCLNAME(fbmp, BCMECMP_FLAG_BMP_MAX_BIT);

    /*! Logical table entry group type - BCMECMP_GRP_TYPE_XXX . */
    bcmecmp_grp_type_t grp_type;

    /*! ECMP group identifier - Entry Key field value. */
    bcmecmp_id_t ecmp_id;

    /*! Load Balance mode field value. */
    uint32_t lb_mode;

    /*! Maximum number of paths supported by the group field value. */
    uint32_t max_paths;

    /*! Number of paths configured for this group field value. */
    uint32_t num_paths;

    /*! Input member fields count. */
    uint32_t member_count[BCMECMP_MEMBER_COUNT];

    /*! rh mode enabled or disabled */
    bool rh_mode;

    /*!
     * Number of paths configured for this resilient hashing group field
     * value.
     */
    uint32_t rh_num_paths;

    /*! Array of resilient hashing load balanced members. */
    uint32_t *rh_entries_arr[BCMECMP_MEMBER_COUNT];

    /*! Array of RH-ECMP group entry's mapped member's array index. */
    uint32_t *rh_mindex_arr;

    /*!
     * Number of member table entries occupied by a member in a
     * resilient hashing load balancing mode group.
     */
    uint32_t *rh_memb_ecnt_arr;

    /*!
     * The resilient hashing load balancing mode group random number
     * generator seed value.
     */
    uint32_t rh_rand_seed;

    /*! Weighted ECMP group size value in member entries count. */
    uint32_t weighted_size;

   /*! Member table start index allocated for this ECMP group. */
    int mstart_idx;

   /*! Member table start index allocated for this ECMP group. */
    int mstart_auto;

    /*! IMM event for operating on this ECMP group. */
    bcmimm_entry_event_t event;

    /*! member field0 array elements. */
    uint32_t member0_field[BCMECMP_MEMBER_COUNT][BCMECMP_MEMBER_FIELD_ARRAY_SIZE];

    /*! member field0 array elements. */
    uint32_t member1_field[BCMECMP_MEMBER_COUNT][BCMECMP_MEMBER_FIELD_ARRAY_SIZE];

    /*! Enhanced flex counter action ID. */
    uint32_t ctr_ing_eflex_action_id;

    /*! Enhanced flex counter object. */
    uint32_t ctr_ing_eflex_object;

    /*! Weight mode. */
    uint32_t weighted_mode;

    /*! NHOP associated weight. */
    uint32_t weight[BCMECMP_MEMBER_FIELD_ARRAY_SIZE];

    /*! Weight count. */
    uint32_t weight_count;

    /*! Weight NHOP index. */
    bcmecmp_nhop_id_t weight_nhop_index[BCMECMP_MEMBER_FIELD_ARRAY_SIZE];

    /*! Weight NHOP index count. */
    uint32_t weight_nhop_index_count;
} bcmecmp_flex_lt_entry_t;


/*!
 * \brief Structure for Resilient hashing ECMP group information.
 */
typedef struct bcmecmp_flex_rh_grp_info_s {

    /*! Array of RH-ECMP group entry's mapped member's array index. */
    uint32_t *member_field[BCMECMP_MEMBER_COUNT];

    /*! Array of RH-ECMP group entry's mapped member's array index. */
    uint32_t *rh_mindex_arr;

    /*!
     * Number of member table entries occupied by a next-hop member in a
     * resilient hashing load balancing mode group.
     */
    uint32_t *rh_memb_ecnt_arr;
} bcmecmp_flex_rh_grp_info_t;


/*!
 * \brief Structure for ECMP hardware table entry information.
 */
typedef struct bcmecmp_flex_hw_entry_info_s {
    /*! Entry flags information. */
    uint8_t flags;
} bcmecmp_flex_hw_entry_info_t;

/*!
 * \brief Structure for ECMP member_dest information.
 */
typedef struct bcmecmp_member_dest_info_s {
    /*! ECMP _member dest group information. */
    bcmecmp_member_dest_grp_attr_t *grp_member_dest_arr;

    /*! ECMP _member dest group entires backup array. */
    bcmecmp_member_dest_grp_attr_t *grp_member_dest_bk_arr;

    /*! Pointer to logical table LTD field ID - bcmecmp_grp_ltd_fields_t. */
    void *ltd_fids;

    /*! Pointer to logical table field attributes - bcmecmp_lt_field_attrs_t. */
    void *lt_fattrs;

    /*! Group flags. */
    uint8_t flags;

    /*! Group flags. */
    uint8_t bk_flags;

    /*! Member Table list pointer.  */
    shr_itbm_list_hdl_t list_hdl;
}bcmecmp_member_dest_info_t;

/*!
 * \brief Get ECMP LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_FLEX_LT_ENT_PTR_VOID(_u) \
        ((BCMECMP_INFO_PTR(_u))->lt_ent)

/*!
 * \brief Get ECMP LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_FLEX_LT_ENT_PTR(_u) \
        ((bcmecmp_flex_lt_entry_t *) ((BCMECMP_INFO_PTR(_u))->lt_ent))

/*!
 * \brief Get ECMP current LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_CURRENT_FLEX_LT_ENT_PTR(_u) \
       ((bcmecmp_flex_lt_entry_t *) ((BCMECMP_INFO_PTR(_u))->current_lt_ent))


/*!
 * \brief Resilient hashing group total number of members count.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group total number of members count.
 */
#define BCMECMP_FLEX_GRP_RH_ENTRIES_CNT(_u, _id) \
            ((BCMECMP_FLEX_GRP_RH_INFO_ENT_PTR(_u,_id))->rh_entries_cnt)

/*!
 * \brief Pointer to resilient hashing group data.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hashing group data.
 */
#define BCMECMP_FLEX_GRP_RHG_PTR(_u, _id) \
             ((bcmecmp_flex_rh_grp_info_t *)(BCMECMP_INFO_PTR(_u)->rhg_flex[(_id)]))

/*!
 * \brief Pointer to resilient hashing group data.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hashing group data.
 */
#define BCMECMP_FLEX_GRP_VOID_RHG_PTR(_u, _id) \
             (BCMECMP_INFO_PTR(_u)->rhg_flex[(_id)])
/*!
 * \brief Pointer to resilient hash load balanced members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 * \param [in] _fld_idx field identifier.
 *
 * \returns Pointer to resilient hash load balanced members array.
 */
#define BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(_u, _id, _fld_idx) \
            (BCMECMP_FLEX_GRP_RHG_PTR(_u, _id)->member_field[_fld_idx])

/*!
 * \brief Pointer to resilient hash load balanced member indices array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced member indices array.
 */
#define BCMECMP_FLEX_GRP_RH_MINDEX_PTR(_u, _id) \
            (BCMECMP_FLEX_GRP_RHG_PTR(_u, _id)->rh_mindex_arr)

/*!
 * \brief Number of member table entries occupied by a RH group member.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of member table entries occupied by a RH group member.
 */
#define BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(_u, _id) \
            (BCMECMP_FLEX_GRP_RHG_PTR(_u, _id)->rh_memb_ecnt_arr)

/*!
 * \brief Pointer to resilient hashing group data.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hashing group data.
 */
#define BCMECMP_FLEX_GRP_RHG_BK_PTR(_u, _id) \
            ((bcmecmp_flex_rh_grp_info_t *)(BCMECMP_INFO_PTR(_u)->rhg_flex_bk[(_id)]))

/*!
 * \brief Pointer to resilient hashing group data.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hashing group data.
 */
#define BCMECMP_FLEX_GRP_VOID_RHG_BK_PTR(_u, _id) \
            (BCMECMP_INFO_PTR(_u)->rhg_flex_bk[(_id)])

/*!
 * \brief Resilient hashing group total number of members count.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group total number of members count.
 */
#define BCMECMP_FLEX_GRP_RH_ENTRIES_CNT_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_RH_INFO_ENT_BK_PTR(_u,_id))->rh_entries_cnt)


/*!
 * \brief Pointer to resilient hash load balanced members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 * \param [in] _idx field identifier.
 *
 * \returns Pointer to resilient hash load balanced members array.
 */
#define BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(_u, _id, _idx) \
            (BCMECMP_FLEX_GRP_RHG_BK_PTR(_u, _id)->member_field[_idx])

/*!
 * \brief Pointer to resilient hash load balanced member indices array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced member indices array.
 */
#define BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(_u, _id) \
            (BCMECMP_FLEX_GRP_RHG_BK_PTR(_u, _id)->rh_mindex_arr)

/*!
 * \brief Number of member table entries occupied by a RH group member.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of member table entries occupied by a RH group member.
 */
#define BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(_u, _id) \
            (BCMECMP_FLEX_GRP_RHG_BK_PTR(_u, _id)->rh_memb_ecnt_arr)


/*!
 * \brief ECMP flex group array entry pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id) \
           (((bcmecmp_flex_grp_attr_t *) \
            ((BCMECMP_INFO_PTR(_u))->grp_arr)) + _id)

/*!
 * \brief ECMP flex backup group array entry pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id) \
           (((bcmecmp_flex_grp_attr_t *) \
            ((BCMECMP_INFO_PTR(_u))->grp_bk_arr)) + _id)
/*!
 * \brief ECMP flex group array entry pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_FLEX_GRP_RH_INFO_ENT_PTR(_u,_id) \
           (((bcmecmp_flex_grp_rh_attr_t *) \
            ((BCMECMP_INFO_PTR(_u))->rh_grp_arr)) + _id)

/*!
 * \brief ECMP flex backup group array entry pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_FLEX_GRP_RH_INFO_ENT_BK_PTR(_u,_id) \
           (((bcmecmp_flex_grp_rh_attr_t *) \
            ((BCMECMP_INFO_PTR(_u))->rh_grp_bk_arr)) + _id)
/*!
 * \brief ECMP group array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_FLEX_GRP_INFO_PTR(_u) \
            ((BCMECMP_INFO_PTR(_u))->grp_arr)

/*!
 * \brief RH ECMP group array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns RH ECMP group array base pointer.
 */
#define BCMECMP_FLEX_GRP_RH_INFO_PTR(_u) \
            ((BCMECMP_INFO_PTR(_u))->rh_grp_arr)

/*!
 * \brief RH ECMP group backup array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns RH ECMP group backup array base pointer.
 */
#define BCMECMP_FLEX_GRP_RH_INFO_BK_PTR(_u) \
            ((BCMECMP_INFO_PTR(_u))->rh_grp_bk_arr)

/*!
 * \brief Group ECMP resolution type.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP resolution type of this group.
 */
#define BCMECMP_FLEX_GRP_TYPE(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->grp_type)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_FLEX_GRP_LT_SID(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->glt_sid)

/*!
 * \brief Get the load balancing mode of a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the ECMP group.
 */
#define BCMECMP_FLEX_GRP_LB_MODE(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->lb_mode)

/*!
 * \brief Get the RH mode of a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the ECMP group.
 */
#define BCMECMP_FLEX_GRP_RH_MODE(_u, _id) \
            ((BCMECMP_FLEX_GRP_RH_INFO_ENT_PTR(_u,_id))->rh_mode)

/*!
 * \brief Device resilient hashing pseudo-random number generator seed value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Device resilient hashing pseudo-random number generator seed value.
 */
#define BCMECMP_FLEX_GRP_RH_RAND_SEED(_u, _id) \
            ((BCMECMP_FLEX_GRP_RH_INFO_ENT_PTR(_u,_id))->rh_rand)

/*!
 * \brief Device resilient hashing pseudo-random number generator seed value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Device resilient hashing pseudo-random number generator seed value.
 */
#define BCMECMP_FLEX_INIT_RH_RAND_SEED(_u, _id) \
            (BCMECMP_FLEX_LT_ENT_PTR(_u)->rh_rand_seed)

/*!
 * \brief Device resilient hashing pseudo-random number generator seed value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Device resilient hashing pseudo-random number generator seed value.
 */
#define BCMECMP_FLEX_RH_RAND_SEED_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_RH_INFO_ENT_BK_PTR(_u,_id))->rh_rand)

/*!
 * \brief Get the load balancing mode of a given weighted ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the weighted ECMP group.
 */
#define BCMECMP_FLEX_GRP_WEIGHTED_MODE(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->weighted_mode)

/*!
 * \brief Maximum ECMP paths supported by this ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Maximum ECMP paths supported by the group.
 */
#define BCMECMP_FLEX_GRP_MAX_PATHS(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->max_paths)

/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_FLEX_GRP_NUM_PATHS(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->num_paths)
/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->mstart_idx)

/*!
 * \brief ECMP group entries backup array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group high availability data array base pointer.
 */
#define BCMECMP_FLEX_GRP_INFO_BK_PTR(_u) ((BCMECMP_INFO_PTR(_u))->grp_bk_arr)

/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_FLEX_GRP_TRANS_ID(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->trans_id)

/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_FLEX_GRP_TRANS_ID_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->trans_id)
/*!
 * \brief Group ECMP resolution type.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP resolution type of this group.
 */
#define BCMECMP_FLEX_GRP_TYPE_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->grp_type)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_FLEX_GRP_LT_SID_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->glt_sid)

/*!
 * \brief Get the load balancing mode of a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the ECMP group.
 */
#define BCMECMP_FLEX_GRP_LB_MODE_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->lb_mode)

/*!
 * \brief Get the RH mode of a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the ECMP group.
 */
#define BCMECMP_FLEX_GRP_RH_MODE_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_RH_INFO_ENT_BK_PTR(_u,_id))->rh_mode)

/*!
 * \brief Get the load balancing mode of a given weighted ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the weighted ECMP group.
 */
#define BCMECMP_FLEX_GRP_WEIGHTED_MODE_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->weighted_mode)

/*!
 * \brief Maximum ECMP paths supported by this ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Maximum ECMP paths supported by the group.
 */
#define BCMECMP_FLEX_GRP_MAX_PATHS_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->max_paths)

/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_FLEX_GRP_NUM_PATHS_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->num_paths)
/*!
 * \brief Itbm use flag for the ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_FLEX_GRP_ITBM_USE(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_PTR(_u,_id))->in_use_itbm)


/*!
 * \brief Itbm use flag for ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_FLEX_GRP_ITBM_USE_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->in_use_itbm)

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
#define BCMECMP_FLEX_TBL_ENT_DEFRAG_STAGED_SET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            |= (BCMECMP_ENTRY_DEFRAG_STAGED))
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
#define BCMECMP_FLEX_TBL_ENT_DEFRAG_STAGED_CLEAR(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            &= (~BCMECMP_ENTRY_DEFRAG_STAGED))

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
#define BCMECMP_FLEX_TBL_ENT_DEFRAG_STAGED_GET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            &(BCMECMP_ENTRY_DEFRAG_STAGED))

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
#define BCMECMP_FLEX_TBL_BK_ENT_DEFRAG_STAGED_SET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
            |= (BCMECMP_ENTRY_DEFRAG_STAGED))

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
#define BCMECMP_FLEX_TBL_BK_ENT_DEFRAG_STAGED_CLEAR(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
            &= (~BCMECMP_ENTRY_DEFRAG_STAGED))

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
#define BCMECMP_FLEX_TBL_BK_ENT_DEFRAG_STAGED_GET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
            &(BCMECMP_ENTRY_DEFRAG_STAGED))

/*!
 * \brief Get the pointer to hw entry from ecmp info structure.
 *
 * \param [in] _p Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Returns pointer to hardware entry.
 */
#define BCMECMP_FLEX_HW_ENTRY_PTR(_p, _i) \
            (((bcmecmp_flex_hw_entry_attr_t *) \
            ((_p)->ent_arr)) + _i)


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
#define BCMECMP_FLEX_ENT_PRECONFIG_SET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            |= (BCMECMP_ENTRY_PRECONFIG))

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
#define BCMECMP_FLEX_ENT_PRECONFIG_CLEAR(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            &= (~BCMECMP_ENTRY_PRECONFIG))

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
#define BCMECMP_FLEX_ENT_PRECONFIG_GET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            & (BCMECMP_ENTRY_PRECONFIG))


/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX_BK(_u, _id) \
            ((BCMECMP_FLEX_GRP_INFO_ENT_BK_PTR(_u,_id))->mstart_idx)

/*!
 * \brief Get the pointer to backup flex hw entry from
 *  ecmp info structure.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Returns pointer to backup hardware entry.
 */
#define BCMECMP_FLEX_HW_ENTRY_BK_PTR(_tptr, _i) \
            (((bcmecmp_flex_hw_entry_attr_t *) \
            ((_tptr)->ent_bk_arr)) + _i)

/*!
 * \brief Set the flex entry staged status.
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
#define BCMECMP_FLEX_TBL_ENT_STAGED_SET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            |= (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Clear the flex_entry staged status.
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
#define BCMECMP_FLEX_TBL_ENT_STAGED_CLEAR(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            &= (~BCMECMP_ENTRY_STAGED))
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
#define BCMECMP_FLEX_TBL_ENT_STAGED_GET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_PTR(_tptr, _i))->flags) \
            &(BCMECMP_ENTRY_STAGED))

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
#define BCMECMP_FLEX_TBL_BK_ENT_STAGED_SET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
            |= (BCMECMP_ENTRY_STAGED))
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
#define BCMECMP_FLEX_TBL_BK_ENT_STAGED_CLEAR(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
            &= (~BCMECMP_ENTRY_STAGED))

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
#define BCMECMP_FLEX_TBL_BK_ENT_STAGED_GET(_tptr, _i)    \
            (((BCMECMP_FLEX_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
            &(BCMECMP_ENTRY_STAGED))

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
#define BCMECMP_TBL_ITBM_LIST(_tptr)    \
            (((_tptr)->list_hdl))

static inline uint32_t bcmecmp_flex_num_paths_convert_to_weighted_size
                           (uint32_t num_paths) {
    uint32_t weighted_size;

    switch (num_paths) {
        case 256:
            weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_256;
            break;
        case 512:
            weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_512;
            break;
        case 1024:
            weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_1K;
            break;
        case 2048:
            weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_2K;
            break;
        case 4096:
            weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_4K;
            break;
        case 8192:
            weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_8K;
            break;
        case 16384:
            weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_16K;
            break;
        default:
            weighted_size = BCMECMP_FLEX_WEIGHTED_SIZE_COUNT;
            break;
    }

    return weighted_size;
}

static inline uint32_t bcmecmp_flex_weighted_size_convert_to_num_paths
                           (uint32_t weighted_size) {
    uint32_t num_paths;

    switch (weighted_size) {
        case BCMECMP_FLEX_WEIGHTED_SIZE_256:
            num_paths = 256;
            break;
        case BCMECMP_FLEX_WEIGHTED_SIZE_512:
            num_paths = 512;
            break;
        case BCMECMP_FLEX_WEIGHTED_SIZE_1K:
            num_paths = 1024;
            break;
        case BCMECMP_FLEX_WEIGHTED_SIZE_2K:
            num_paths = 2048;
            break;
        case BCMECMP_FLEX_WEIGHTED_SIZE_4K:
            num_paths = 4096;
            break;
        case BCMECMP_FLEX_WEIGHTED_SIZE_8K:
            num_paths = 8192;
            break;
        case BCMECMP_FLEX_WEIGHTED_SIZE_16K:
            num_paths = 16*1024;
            break;
        default:
            num_paths = 0;
            break;
    }

    return num_paths;
}

/*!
 * \brief Get maximum index in the member table.
 *
 * Given a group type, return the maximum index in the
 * corresponding member table.
 *
 * \param [in] unit BCM unit number.
 * \param [in] gtype group type.
 *
 * \returns maximum index in the table.
 * table.
 */
uint32_t
bcmecmp_flex_mbr_tbl_max(int unit, bcmecmp_grp_type_t gtype);

/*!
 * \brief Get unspecified fields from current saved LT values.
 *
 * \param [in] unit BCM unit number.
 * \param [in] lt_entry pointer to lt_entry.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
int
bcmecmp_flex_group_update_entry_set(int unit,
                                    bcmecmp_flex_lt_entry_t *lt_entry);

#endif /* BCMECMP_FLEX_DB_INTERNAL_H */
