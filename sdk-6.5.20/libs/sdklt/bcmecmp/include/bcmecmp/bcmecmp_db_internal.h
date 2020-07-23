/*! \file bcmecmp_db_internal.h
 *
 * This file contains ECMP custom table handler data structures definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_DB_INTERNAL_H
#define BCMECMP_DB_INTERNAL_H

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
#include <bcmecmp/bcmecmp_flex_db_internal.h>
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
#define BCMECMP_LT_FIELD_SET(_m, _f)    \
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
#define BCMECMP_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))

/*! \brief The max number NextHop paths. */
#define BCMECMP_NHOP_ARRAY_SIZE (4096)

/*! \brief The max number NextHop paths. */
#define BCMECMP_NHOP_WEIGHTED_OL_ARRAY_SIZE (16384)

/*! \brief The max number NextHop paths. */
#define BCMECMP_NHOP_WEIGHTED_UL_ARRAY_SIZE (16384)

/*! \brief The max number Underlay ECMP paths. */
#define BCMECMP_UNDERLAY_ECMP_ARRAY_SIZE (1024)

/*! \brief The max number RHG info point array size. */
#define BCMECMP_RHG_INFO_PTR_ARRAY_SIZE (4096)

/*! \brief The max number RHG info point array size. */
#define BCMECMP_FLEX_RHG_INFO_PTR_ARRAY_SIZE (2*4096)

/*! \brief Max bits number of member banks. */
#define BCMECMP_MEMB_BANK_MAX_BIT 32

/*!
 * \brief ECMP logical table fields information.
 */
typedef enum bcmecmp_lt_fields_e {

    /*! ECMP group identifier field is set. */
    BCMECMP_LT_FIELD_ECMP_ID = 0,

    /*! ECMP load balancing mode field is set. */
    BCMECMP_LT_FIELD_LB_MODE,

    /*! Sort ECMP next-hop interfaces status field is set. */
    BCMECMP_LT_FIELD_NHOP_SORTED,

    /*! Maximum number of ECMP paths allowed for the group field is set. */
    BCMECMP_LT_FIELD_MAX_PATHS,

    /*! Number of active ECMP paths configured for the group field is set. */
    BCMECMP_LT_FIELD_NUM_PATHS,

    /*! ECMP next-hop indicator field is set. */
    BCMECMP_LT_FIELD_ECMP_NHOP,

    /*! Underlay ECMP group identifier field is set. */
    BCMECMP_LT_FIELD_UECMP_ID,

    /*! Next-hop identifier field is set. */
    BCMECMP_LT_FIELD_NHOP_ID,

    /*! Overlay next-hop identifier field is set. */
    BCMECMP_LT_FIELD_O_NHOP_ID,

    /*! Resilient hashing group size field is set. */
    BCMECMP_LT_FIELD_RH_SIZE,

    /*!
     * Number of active ECMP paths configured for the resilient hashing group
     * field is set.
     */
    BCMECMP_LT_FIELD_RH_NUM_PATHS,

    /*!
     * The resilient hashing load balancing mode group random number
     * generator seed value.
     */
    BCMECMP_LT_FIELD_RH_RANDOM_SEED,

    /*! RH ECMP next-hop indicator field is set. */
    BCMECMP_LT_FIELD_RH_ECMP_NHOP,

    /*! RH next-hop identifier field is set. */
    BCMECMP_LT_FIELD_RH_NHOP_ID,

    /*! RH overlay next-hop identifier field is set. */
    BCMECMP_LT_FIELD_RH_O_NHOP_ID,

    /*! RH underlay ECMP group identifier field is set. */
    BCMECMP_LT_FIELD_RH_UECMP_ID,

    /*! Weighted group size field is set. */
    BCMECMP_LT_FIELD_WEIGHTED_SIZE,

    /*! Weighted group mode field is set. */
    BCMECMP_LT_FIELD_WEIGHTED_MODE,

    /*! Weighted group weight field is set. */
    BCMECMP_LT_FIELD_WEIGHT,

    /* Aggregation Group Monitor Pool for unicast packets. */
    BCMECMP_LT_FIELD_MON_AGM_POOL,

    /* Aggregation Group Monitor Identifier for unicast packets. */
    BCMECMP_LT_FIELD_MON_AGM_ID,

    /* member 0 field 0 is SET. */
    BCMECMP_LT_FIELD_MEMBER0_FIELD0,

    /* member 0 field 0 is SET. */
    BCMECMP_LT_FIELD_MEMBER0_FIELD1,

    /* member 0 field 0 is SET. */
    BCMECMP_LT_FIELD_MEMBER0_FIELD2,

    /* member 0 field 0 is SET. */
    BCMECMP_LT_FIELD_MEMBER0_FIELD3,

    /* member 1 field 0 is SET. */
    BCMECMP_LT_FIELD_MEMBER1_FIELD0,

    /* member 0 field 0 is SET. */
    BCMECMP_LT_FIELD_MEMBER1_FIELD1,

    /* member 0 field 0 is SET. */
    BCMECMP_LT_FIELD_MEMBER1_FIELD2,

    /* Enhanced flex counter action ID. */
    BCMECMP_LT_FIELD_CTR_ING_EFLEX_ACTION_ID,

    /* Enhanced flex counter object. */
    BCMECMP_LT_FIELD_CTR_ING_EFLEX_OBJECT,

    /* BASE_INDEX of the group in member table */
    BCMECMP_LT_FIELD_BASE_INDEX,

    /* AUTO assignment of BASE_INDEX of the group*/
    BCMECMP_LT_FIELD_BASE_INDEX_AUTO,

    /*! Resilient hashing group size field is set. */
    BCMECMP_LT_FIELD_RH_MODE,

    /*! Weighted group weight NHOP field is set. */
    BCMECMP_LT_FIELD_WEIGHTED_NHOP,

    /*! Must be the last one. */
    BCMECMP_LT_FIELD_COUNT

} bcmecmp_fields_t;

/*!
 * \brief Structure for ECMP group table DRD fields information.
 */
typedef struct bcmecmp_grp_drd_fields_s {

    /*! Load Balancing Mode DRD field identifier. */
    bcmdrd_fid_t lb_mode;

    /*! Member table Base Pointer DRD field identifier. */
    bcmdrd_fid_t base_ptr;

    /*! Number of members in ECMP group DRD field identifier. */
    bcmdrd_fid_t count;

    /*! Round-robin counter DRD field identifier. */
    bcmdrd_fid_t rrlb_cnt;

    /* Aggregation Group Monitor Pool for unicast packets. */
    bcmdrd_fid_t agm_pool;

    /* Aggregation Group Monitor Identifier for unicast packets. */
    bcmdrd_fid_t agm_id;
} bcmecmp_grp_drd_fields_t;

/*!
 * \brief Structure for ECMP group table LTD fields information.
 */
typedef struct bcmecmp_grp_ltd_fields_t_s {

    /*! ECMP group identifier LTD field identifier. */
    bcmecmp_ltd_fid_t ecmp_id_fid;

    /*! Load Balancing Mode LTD field identifier. */
    bcmecmp_ltd_fid_t lb_mode_fid;

    /*! Next-hop identifiers sorted indicator. */
    bcmecmp_ltd_fid_t nhop_sorted_fid;

    /*! Maximum ECMP paths LTD field identifier. */
    bcmecmp_ltd_fid_t max_paths_fid;

    /*! Number of ECMP paths configured LTD field identifier. */
    bcmecmp_ltd_fid_t num_paths_fid;

    /*! Underlay ECMP next-hops indicator. */
    bcmecmp_ltd_fid_t ecmp_nhop_fid;

    /*! Underlay ECMP group identifier LTD field identifier. */
    bcmecmp_ltd_fid_t uecmp_id_fid;

    /*! Overlay next-hop identifier LTD field identifier. */
    bcmecmp_ltd_fid_t o_nhop_id_fid;

    /*! Single or underlay next-hop identifier LTD field identifier. */
    bcmecmp_ltd_fid_t nhop_id_fid;

    /*! Resilient hashing underlay ECMP next-hops indicator. */
    bcmecmp_ltd_fid_t rh_ecmp_nhop_fid;

    /*! Resilient hashing underlay ECMP group identifier LTD field identifier. */
    bcmecmp_ltd_fid_t rh_uecmp_id_fid;

    /*! Resilient hashing overlay next-hop identifier LTD field identifier. */
    bcmecmp_ltd_fid_t rh_o_nhop_id_fid;

    /*!
     * Resilient hashing single or underlay next-hop identifier LTD field
     * identifier.
     */
    bcmecmp_ltd_fid_t rh_nhop_id_fid;

    /*! Resilient hashing group size LTD field identifier. */
    bcmecmp_ltd_fid_t rh_size_fid;

    /*! Resilient hashing group seed. */
    bcmecmp_ltd_fid_t rh_seed_fid;

    /*!
     * Number of resilient hashing ECMP paths configured LTD field
     * identifier.
     */
    bcmecmp_ltd_fid_t rh_num_paths_fid;

    /*! Weighted ECMP group size LTD field identifier. */
    bcmecmp_ltd_fid_t weighted_size_fid;

    /*! Weighted ECMP group weight mode LTD field identifier. */
    bcmecmp_ltd_fid_t weighted_mode_fid;

    /*! Weighted ECMP group NHI weight LTD field identifier. */
    bcmecmp_ltd_fid_t weight_fid;

    /* Aggregation Group Monitor Pool for unicast packets. */
    bcmecmp_ltd_fid_t agm_pool_fid;

    /* Aggregation Group Monitor Identifier for unicast packets. */
    bcmecmp_ltd_fid_t agm_id_fid;

    /* Enhanced flex counter action ID. */
    bcmecmp_ltd_fid_t ctr_ing_eflex_action_id_fid;

    /* Enhanced flex counter object. */
    bcmecmp_ltd_fid_t ctr_ing_eflex_object_fid;

} bcmecmp_grp_ltd_fields_t;

/*!
 * \brief Structure for ECMP member table DRD fields information.
 */
typedef struct bcmecmp_memb_drd_fields_s {
    /*! Next Hop ID or ECMP Group ID indicator DRD field identifier. */
    bcmdrd_fid_t ecmp_flag;

    /*! Underlay ECMP Group ID DRD field identifier. */
    bcmdrd_fid_t group_id;

    /*! Next Hop ID DRD field identifier. */
    bcmdrd_fid_t nhop_id;
} bcmecmp_memb_drd_fields_t;

/*!
 * \brief ECMP hardware tables resource manager operation flags.
 */
typedef enum bcmecmp_oper_e {
    /*! No operation for Init value. */
    BCMECMP_OPER_NONE      = 0,

    /*! Entry add operation. */
    BCMECMP_OPER_ADD       = (1 << 1),

    /*! Entry delete operation. */
    BCMECMP_OPER_DEL       = (1 << 2),

    /*! Entry add operation with skip hardware index zero option. */
    BCMECMP_OPER_SKIP_ZERO = (1 << 3),

    /*! Entry copy operation. */
    BCMECMP_OPER_CPY       = (1 << 4),

    /*! Entry clear operation. */
    BCMECMP_OPER_CLR       = (1 << 5),

    /*! Entry update operation. */
    BCMECMP_OPER_UPDT      = (1 << 6),

    /*! Must be last one. */
    BCMECMP_OPER_COUNT
} bcmecmp_oper_t;

/*!
 * \brief Structure for ECMP logical table fields information.
 */
typedef struct bcmecmp_lt_field_attrs_s {
    /*! Minimum ECMP Group Identifier value supported. */
    int min_ecmp_id;

    /*! Maximum ECMP Group Identifier value supported. */
    int max_ecmp_id;

    /*! Minimum ECMP load balancing mode value supported. */
    uint32_t min_lb_mode;

    /*! Maximum ECMP load balancing mode value supported. */
    uint32_t max_lb_mode;

    /*! Load balancing modes supported for the logical table. */
    uint32_t lb_mode[BCMECMP_LB_MODE_COUNT];

    /*! Paths count (MAX_PATHS & NUM_PATHS) must be in powers-of-two. */
    bool pow_two_paths;

    /*! Minimum ECMP paths allowed for groups. */
    uint32_t min_paths;

    /*! Maximum ECMP paths supported for groups (hardware limit). */
    uint32_t max_paths;

    /*! Minimum value for configured ECMP paths allowed for groups. */
    uint32_t min_npaths;

    /*! Maximum value for configured ECMP paths allowed for groups. */
    uint32_t max_npaths;

    /*! Default ECMP next-hop field value for ECMP_OVERLAY logical table. */
    bool def_ecmp_nhop;

    /*! Minimum Underlay ECMP identifier value supported by this device. */
    int min_uecmp_id;

    /*! Maximum Underlay ECMP identifier value supported by this device. */
    int max_uecmp_id;

    /*! Minimum next-hop identifier value supported by this device. */
    int min_nhop_id;

    /*! Maximum next-hop identifier value supported by this device. */
    int max_nhop_id;

    /*! Minimum overlay next-hop identifier value supported by this device. */
    int min_o_nhop_id;

    /*! Maximum overlay next-hop identifier value supported by this device. */
    int max_o_nhop_id;

    /*! Minimum resilient hashing group size supported by the device. */
    uint32_t min_rh_size_enc;

    /*! Maximum resilient hashing group size supported by the device. */
    uint32_t max_rh_size_enc;

    /*!
     * Minimum value for configured resilient hashing ECMP paths allowed for
     * groups.
     */
    uint32_t min_rh_npaths;

    /*!
     * Maximum value for configured resilient hashing ECMP paths allowed for
     * groups.
     */
    uint32_t max_rh_npaths;

    /*!
     * Minimum value for configured resilient hashing ECMP seed allowed for
     * groups.
     */
    uint32_t min_rh_seed;

    /*!
     * Maximum value for configured resilient hashing ECMP seed allowed for
     * groups.
     */
    uint32_t max_rh_seed;

    /*! Minimum weighted ECMP group size value supported. */
    uint32_t min_weighted_size;

    /*! Maximum weighted ECMP group size value supported. */
    uint32_t max_weighted_size;

    /*! Default weighted ECMP group size value supported. */
    uint32_t def_weighted_size;

    /*! Minimum weighted ECMP group mode value supported. */
    uint32_t min_weighted_mode;

    /*! Maximum weighted ECMP group mode value supported. */
    uint32_t max_weighted_mode;

    /*! Minimum weighted ECMP group weight value supported. */
    uint32_t min_weight;

    /*! Maximum weighted ECMP group weight value supported. */
    uint32_t max_weight;

    /* Minimum Aggregation Group Monitor Pool value supported. */
    int min_agm_pool;

    /* Maximum Aggregation Group Monitor Pool value supported. */
    int max_agm_pool;

    /* Minimum Aggregation Group Monitor Identifier value supported. */
    int min_agm_id;

    /* Maximum Aggregation Group Monitor Identifier value supported. */
    int max_agm_id;

    /* Minimum enhanced flex counter action identifier. */
    int min_ctr_ing_eflex_action_id;

    /* Maimum enhanced flex counter action identifier. */
    int max_ctr_ing_eflex_action_id;

    /* Minimum enhanced flex counter object. */
    int min_ctr_ing_eflex_object;

    /* Maimum enhanced flex counter object. */
    int max_ctr_ing_eflex_object;
} bcmecmp_lt_field_attrs_t;

#define MAX_ECMP_FLDS (4)

/*!
 * \brief Structure for ECMP logical table entry information.
 */
typedef struct bcmecmp_lt_entry_s {

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

    /*!
     * Field value indicates next-hops must be sorted prior to installation in
     * member table in hardware.
     */
    bool nhop_sorted;

    /*! Maximum number of paths supported by the group field value. */
    uint32_t max_paths;

    /*! Numumber of paths configured for this group field value. */
    uint32_t num_paths;

    /*! ECMP next-hop indicator field value. */
    bool ecmp_nhop;

    /*! Underlay member ECMP or next-hop indicator field value. */
    uint8_t u_ecmp_nhop[BCMECMP_NHOP_ARRAY_SIZE];

    /*! Input underlay indicator fields count. */
    uint32_t u_ecmp_nhop_count;

    /*! Underlay ECMP group identifier field array elements. */
    bcmecmp_id_t uecmp_id[BCMECMP_UNDERLAY_ECMP_ARRAY_SIZE];

    /*! Input ECMP_UNDERLAY_IDs count. */
    uint32_t uecmp_ids_count;

    /*! Next-hop indices field array elements. */
    bcmecmp_nhop_id_t nhop_id[BCMECMP_NHOP_ARRAY_SIZE];

     /*! Overlay next-hop indices field array elements. */
    bcmecmp_nhop_id_t o_nhop_id[BCMECMP_NHOP_ARRAY_SIZE];

    /*! Input NHOP_IDs count. */
    uint32_t nhop_ids_count;

    /*! Input overlay NHOP_IDs count. */
    uint32_t o_nhop_ids_count;

    /*! Resilient hashing group size hardware encoding value. */
    uint32_t rh_size_enc;

    /*!
     * Numumber of paths configured for this resilient hashing group field
     * value.
     */
    uint32_t rh_num_paths;

    /*! RH next-hop indices field array elements. */
    bcmecmp_nhop_id_t rh_nhop_id[BCMECMP_NHOP_ARRAY_SIZE];

    /*! RH input NHOP_IDs count. */
    uint32_t rh_nhop_ids_count;

    /*! Resilient hashing group size value in member entries count. */
    uint32_t rh_entries_cnt;

    /*! Weighted ECMP group size value in member entries count. */
    uint32_t weighted_size;

    /*! Weighted ECMP group mode value in member entries count. */
    uint32_t weighted_mode;

    /*! Weighted ECMP group NHI weight value in member entries count. */
    uint32_t weight[BCMECMP_NHOP_ARRAY_SIZE];

    /*! Input weight count. */
    uint32_t weight_count;

    /* Aggregation Group Monitor Pool for unicast packets. */
    uint32_t agm_pool;

    /* Aggregation Group Monitor Identifier for unicast packets. */
    uint32_t agm_id;

    /*! Array of resilient hashing load balanced members. */
    int *rh_entries_arr;

    /*! Array of resilient hashing load balanced overlay members. */
    int *rh_o_entries_arr;

    /*! Array of resilient hashing load balanced underlay members indicator.
     * Enable to indicate ECMP group, else next hop.
     */
    bool *rh_u_ecmp_nhop_arr;

    /*! Array of RH-ECMP group entry's mapped member's array index. */
    uint32_t *rh_mindex_arr;

    /*!
     * Number of member table entries occupied by a next-hop member in a
     * resilient hashing load balancing mode group.
     */
    uint32_t *rh_memb_ecnt_arr;

    /*!
     * The resilient hashing load balancing mode group random number
     * generator seed value.
     */
    uint32_t rh_rand_seed;

    /*! Member table start index allocated for this ECMP group. */
    int mstart_idx;

    /*! IMM event for operating on this ECMP group. */
    bcmimm_entry_event_t event;

    /* Enhanced flex counter action ID. */
    uint32_t ctr_ing_eflex_action_id;

    /* Enhanced flex counter object. */
    uint32_t ctr_ing_eflex_object;
} bcmecmp_lt_entry_t;

/*!
 * \brief Structure for ECMP hardware table entry information.
 */
typedef struct bcmecmp_hw_entry_info_s {

    /*! Entry reference count information. */
    uint16_t ref_cnt;

    /*! Entry flags information. */
    uint8_t flags;
} bcmecmp_hw_entry_info_t;

/*!
 * \brief Structure for ECMP hardware table information.
 */
typedef struct bcmecmp_tbl_prop_s {

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

    /*! Pointer to logical table DRD field ID - bcmecmp_grp_drd_fields_t. */
    void *drd_fids;

    /*! Pointer to logical table LTD field ID - bcmecmp_grp_ltd_fields_t. */
    void *ltd_fids;

    /*! Pointer to logical table field attributes - bcmecmp_lt_field_attrs_t. */
    void *lt_fattrs;

    /*! Entry information array. */
    void *ent_arr;

    /*! Table entry backup array elements base pointer.  */
    void *ent_bk_arr;

    /*! Table list pointer.  */
    shr_itbm_list_hdl_t list_hdl;

    /*! Second table list pointer in case two lists for one LT. */
    shr_itbm_list_hdl_t list_hdl_1;
} bcmecmp_tbl_prop_t;

/*!
 * \brief Structure for ECMP hardware table information.
 */
typedef struct bcmecmp_reg_prop_s {

    /*! Device Resouce Database register Symbol ID */
    bcmdrd_sid_t drd_sid;

    /*! Logical table Database Symbol ID. */
    bcmltd_sid_t ltd_sid;

    /*! Register entry size in words. */
    uint32_t ent_size;

    /*! ECMP resolution mode DRD field identifier. */
    bcmdrd_fid_t mode_fid;

    /*! Device ECMP resolution mode hardware encoding value.  */
    uint32_t mode_hw_val[BCMECMP_MODE_COUNT];
} bcmecmp_reg_prop_t;

/*!
 * \brief Structure for ECMP hardware table information.
 */
typedef struct bcmecmp_tbl_op_s {

    /*! Table pointer. */
    bcmecmp_tbl_prop_t *tbl_ptr;

    /*! Type of Operation. */
    uint32_t oper_flags;

    /*! Number of entries to be allocated. */
    uint32_t num_ent;

    /*! Allocated entry index. */
    int free_idx;
} bcmecmp_tbl_op_t;

/*!
 * \brief Structure for Resilient hashing ECMP group information.
 */
typedef struct bcmecmp_rh_grp_info_s {

    /*! Array of resilient hashing load balanced members. */
    int *rh_entries_arr;

    /*! Array of resilient hashing load balanced overlay members. */
    int *rh_o_entries_arr;

    /*! Array of resilient hashing load balanced underlay indicator members. */
    bool *rh_u_ecmp_nhop_arr;

    /*! Array of RH-ECMP group entry's mapped member's array index. */
    uint32_t *rh_mindex_arr;

    /*!
     * Number of member table entries occupied by a next-hop member in a
     * resilient hashing load balancing mode group.
     */
    uint32_t *rh_memb_ecnt_arr;
} bcmecmp_rh_grp_info_t;

/*!
 * \brief Structure for ECMP group information.
 */
typedef struct bcmecmp_grp_info_s {

    /*! Transaction identifer related to this group. */
    uint32_t trans_id;

    /*! Logical table entry group type - BCMECMP_GRP_TYPE_XXX . */
    bcmecmp_grp_type_t grp_type;

    /*! Group logial table source identifier. */
    bcmltd_sid_t glt_sid;

    /*! Load balancing mode configured for this group. */
    uint32_t lb_mode;

    /*! Group next-hops sorted status indicator. */
    bool nhop_sorted;

    /*! Maximum ECMP paths supported by this group. */
    uint32_t max_paths;

    /*! Number of ECMP paths configured for this group. */
    uint32_t num_paths;

    /*! Next-hop is ECMP next hop. */
    bool ecmp_nhop;

    /*! Resilient hashing group size configured for this group - HW encoding. */
    uint32_t rh_size_enc;

    /*! Maximum resilient hashing group size by this group. */
    uint32_t rh_entries_cnt;

    /* Group next-hop members array. */
    bcmecmp_nhop_id_t *nhop_arr;

    /* Group underlay ECMP_ID members array. */
    bcmecmp_id_t *uecmp_arr;

    /*! Member table start index for this group. */
    int mstart_idx;
} bcmecmp_grp_info_t;

/*! ECMP group node in the list. */
typedef struct bcmecmp_grp_node_s {
    /*! Group id. */
    bcmecmp_id_t grp_id;

    /*! The pointer to the next group in the list. */
    struct bcmecmp_grp_node_s *next;
} bcmecmp_grp_node_t;

/*! ECMP group list. */
typedef struct bcmecmp_grp_list_s {
    /*! The head node in the list. */
    bcmecmp_grp_node_t *head;
} bcmecmp_grp_list_t;

/*!
 * \brief Structure for ECMP group and hardware tables information.
 */
typedef struct bcmecmp_info_s {

    /*! Device ECMP mode. */
    uint32_t ecmp_mode;

    /*! Resilient hashing load balancing mode pseudo-random number generator. */
    uint32_t rh_rand_seed;

    /*!
     * Enable index remap feature.
     * There are two member index spaces coexist in some devices.
     * One member index space has to be remapped into another one
     * for free index management.
     */
    bool use_index_remap;

    /*! Enable to use itbm lib. */
    bool itbm;

    /*! Overlay ECMP member count in the LTs. */
    uint32_t ovlay_memb_count;

    /*! Member weight mode group list. */
    bcmecmp_grp_list_t member_weight_list;

    /*! Non-member weight mode group list. */
    bcmecmp_grp_list_t non_member_weight_list;

    /*! Device ECMP config register information. */
    bcmecmp_reg_prop_t config;

    /*! Pre-IFP ECMP group table information. */
    bcmecmp_tbl_prop_t init_group[BCMECMP_GRP_TYPE_COUNT];

    /*! Pre-IFP ECMP member table information. */
    bcmecmp_tbl_prop_t init_member[BCMECMP_GRP_TYPE_COUNT];

    /*! Post-IFP ECMP group table information. */
    bcmecmp_tbl_prop_t group[BCMECMP_GRP_TYPE_COUNT];

    /*! Post-IFP ECMP member table information. */
    bcmecmp_tbl_prop_t member[BCMECMP_GRP_TYPE_COUNT];

    /*! Post-IFP ECMP member 2 table information. */
    bcmecmp_tbl_prop_t member2[BCMECMP_GRP_TYPE_COUNT];

    /*! ECMP Round-Robin load balancing count table information. */
    bcmecmp_tbl_prop_t rrlb_cnt[BCMECMP_GRP_TYPE_COUNT];

    /*! Widest entry size among all ECMP hardware tables. */
    uint32_t ent_wsize_max;

    /*! ECMP entries queued in WAL information.  */
    bcmecmp_wal_info_t *wali;

    /*! ECMP logical table entry information. */
    void *lt_ent;

    /*! ECMP current logical table entry information. */
    void *current_lt_ent;

    /*! ECMP group information array size. */
    uint32_t grp_arr_sz;

    /*! ECMP group information. */
    void *grp_arr;

    /*! ECMP group entires backup array. */
    void *grp_bk_arr;

    /*! ECMP group information. */
    void *rh_grp_arr;

    /*! ECMP group entires backup array. */
    void *rh_grp_bk_arr;

    /*! ECMP Member0 information. */
    bcmecmp_member_dest_info_t dest_info0;

    /*! ECMP Member1 information. */
    bcmecmp_member_dest_info_t dest_info1;

    /*! Resilient hashing group mode details. */
    bcmecmp_rh_grp_info_t **rhg;

    /*! Resilient hashing group mode details. */
    bcmecmp_rh_grp_info_t **rhg_bk;

    /*! Resilient hashing group mode details. */
    void *rhg_flex[BCMECMP_FLEX_RHG_INFO_PTR_ARRAY_SIZE];

    /*! Resilient hashing group mode details. */
    void *rhg_flex_bk[BCMECMP_FLEX_RHG_INFO_PTR_ARRAY_SIZE];
} bcmecmp_info_t;

/*!
 * \brief Structure for ECMP control information.
 */
typedef struct bcmecmp_control_s {

    /*! ECMP feature initalized status indicator. */
    bool init;

    /*! Use HA memory for storing ECMP CTH software state control. */
    bool use_ha_mem;

    /*! Use DMA engine for ECMP hardware tables programming control. */
    bool use_dma;

    /*! ECMP atomic transactions support feature control. */
    bool atomic_trans;

    /*! Mutex lock for synchronization. */
    sal_mutex_t ecmp_lock;

    /*! ECMP module information. */
    bcmecmp_info_t ecmp_info;

    /*! ECMP device specific functions. */
    bcmecmp_drv_t *ecmp_drv;
} bcmecmp_control_t;

/*!
 * ECMP limit control lt structure
 */
typedef struct bcmecmp_limit_s {
    /*! Number of overlay next hops. */
    uint8_t    num_o_memb_bank;
    /*! Validity of num_o_nhop field. */
    bool       num_o_memb_bank_valid;
    /*! Next hops banks bitmap. */
    SHR_BITDCLNAME(memb_bank_bitmap, BCMECMP_MEMB_BANK_MAX_BIT);
} bcmecmp_limit_t;

/*!
 * \brief Get ECMP Per-BCM unit control structure.
 */
bcmecmp_control_t *bcmecmp_get_ecmp_control(int unit);

/*!
 * \brief Get BCM unit ECMP control structure pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Pointer to BCM unit's \c _u ECMP Control Structure.
 */
#define BCMECMP_CTRL(_u) (bcmecmp_get_ecmp_control(_u))

/*!
 * \brief Get ECMP feature initialized status.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP feature status on the BCM unit.
 */
#define BCMECMP_INIT(_u)                         \
            if (NULL == BCMECMP_CTRL(_u) ||      \
                !(BCMECMP_CTRL(_u)->init)) {     \
                SHR_ERR_EXIT(SHR_E_INIT); \
            }

/*!
 * \brief Set or get the use HA memory for storing ECMP software state control.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Status of use HA memory for storing ECMP software state control.
 */
#define BCMECMP_USE_HA_MEM(_u) (BCMECMP_CTRL(_u)->use_ha_mem)

/*!
 * \brief Set or get use DMA engine for programming ECMP tables control.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Status of use DMA engine for programming ECMP tables control.
 */
#define BCMECMP_USE_DMA(_u) (BCMECMP_CTRL(_u)->use_dma)

/*!
 * \brief Set or get ECMP atomic transaction feature support status.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP atomic transaction feature support status.
 */
#define BCMECMP_ATOMIC_TRANS(_u) (BCMECMP_CTRL(_u)->atomic_trans)

/*!
 * \brief Take the ECMP control structure lock of this BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns None.
 */
#define BCMECMP_LOCK(_u) \
            (sal_mutex_take(BCMECMP_CTRL(_u)->ecmp_lock, SAL_MUTEX_FOREVER))

/*!
 * \brief Give ECMP control structure lock of this BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns None.
 */
#define BCMECMP_UNLOCK(_u) \
            (sal_mutex_give(BCMECMP_CTRL(_u)->ecmp_lock))

/*!
 * \brief Get ECMP information structure for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP information structure of the unit.
 */
#define BCMECMP_INFO(_u) (BCMECMP_CTRL(_u)->ecmp_info)

/*!
 * \brief Get ECMP information structure pointer for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP information structure pointer of the unit.
 */
#define BCMECMP_INFO_PTR(_u) (&(BCMECMP_CTRL(_u)->ecmp_info))

/*!
 * \brief Get ECMP LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_LT_ENT_PTR_VOID(_u) \
        ((BCMECMP_INFO_PTR(_u))->lt_ent)

/*!
 * \brief Get ECMP current LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_CURRENT_LT_ENT_PTR_VOID(_u) \
       ((BCMECMP_INFO_PTR(_u))->current_lt_ent)


/*!
 * \brief Get ECMP LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_LT_ENT_PTR(_u) \
        ((bcmecmp_lt_entry_t *) ((BCMECMP_INFO_PTR(_u))->lt_ent))

/*!
 * \brief Get ECMP current LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_CURRENT_LT_ENT_PTR(_u) \
       ((bcmecmp_lt_entry_t *) ((BCMECMP_INFO_PTR(_u))->current_lt_ent))


/*!
 * \brief Get ECMP entries in WAL information.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP entries in WAL details.
 */
#define BCMECMP_WALI_PTR(_u) ((BCMECMP_INFO_PTR(_u))->wali)

/*!
 * \brief Get ECMP table structure pointer for a given table name.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Table name.
 *
 * \returns Pointer to the ECMP table structure.
 */
#define BCMECMP_TBL_PTR(_u, _var, _t) (&(BCMECMP_INFO(_u)._var[_t]))

/*!
 * \brief Set/Get the minimum entry index value supported for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Minimum entry index value supported by the table.
 */
#define BCMECMP_TBL_IDX_MIN(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).imin)

/*!
 * \brief Set/Get the maximum entry index value supported for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Maximum entry index value supported by the table.
 */
#define BCMECMP_TBL_IDX_MAX(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).imax)

/*!
 * \brief Set/Get the maximum entry index value in use for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Maximum entry index value in use for the table.
 */
#define BCMECMP_TBL_IDX_MAX_USED(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).imax_used)

/*!
 * \brief Set/Get the maximum entry ID value supported by a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Maximum entry ID value supported by the table.
 */
#define BCMECMP_TBL_SIZE(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[_t]).tbl_size)

/*!
 * \brief Set/Get DRD symbol identifier value of a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns DRD symbol identifier of the table.
 */
#define BCMECMP_TBL_DRD_SID(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).drd_sid)

/*!
 * \brief Set/Get DRD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table Name of bcmecmp_tbl_prop_t type.
 *
 * \returns DRD symbol identifier values of fields in the table.
 */
#define BCMECMP_TBL_DRD_FIELDS(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).drd_fids)

/*!
 * \brief Set/Get the LTD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table Name of bcmecmp_tbl_prop_t type.
 *
 * \returns LTD symbol identifier values of fields in the table.
 */
#define BCMECMP_TBL_LTD_FIELDS(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).ltd_fids)

/*!
 * \brief Set/Get the LTD source identifier value of a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns LTD source identifier value of the table.
 */
#define BCMECMP_TBL_LTD_SID(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).ltd_sid)

/*!
 * \brief Set/Get the entry size in words for a given physical table/memory.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Entry size in words of the table/memory.
 */
#define BCMECMP_TBL_ENT_SIZE(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).ent_size)

/*!
 * \brief Set/Get maximum entry size in words among all ECMP table entries.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum entry size in words among all ECMP table entries.
 */
#define BCMECMP_TBL_ENT_SIZE_MAX(_u) (BCMECMP_INFO(_u).ent_wsize_max)

/*!
 * \brief ECMP group logical table field attributes structure void * pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Void * pointer to logical table field attributes structure.
 */
#define BCMECMP_LT_FIELD_ATTRS_VOID_PTR(_u, _g, _t) \
            ((BCMECMP_INFO(_u)._g[(_t)]).lt_fattrs)

/*!
 * \brief ECMP group logical table field attributes structure pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Pointer to ECMP Group logical table field attributes structure.
 */
#define BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t) \
            ((bcmecmp_lt_field_attrs_t*)((BCMECMP_INFO(_u)._g[(_t)]).lt_fattrs))

/*!
 * \brief Minimum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum ECMP ID value supported by the logical table.
 */
#define BCMECMP_LT_MIN_ECMP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_ecmp_id)

/*!
 * \brief Maximum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum ECMP ID value supported by the logical table.
 */
#define BCMECMP_LT_MAX_ECMP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_ecmp_id)

/*!
 * \brief Minimum value supported for LB_MODE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum LB_MODE value supported by the logical table.
 */
#define BCMECMP_LT_MIN_LB_MODE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_lb_mode)

/*!
 * \brief Maximum value supported for LB_MODE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum LB_MODE value supported by the logical table.
 */
#define BCMECMP_LT_MAX_LB_MODE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_lb_mode)

/*!
 * \brief LB_MODE name for a given LB_MODE enum value of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns LB_MODE name for the given LB_MODE enum value.
 */
#define BCMECMP_LT_LB_MODE(_u, _g, _t, _lbm) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->lb_mode[(_lbm)])

/*!
 * \brief Paths count (MAX_PATHS & NUM_PATHS) must be in powers-of-two.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Powers-of-two paths count requirement status.
 */
#define BCMECMP_LT_POW_TWO_PATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->pow_two_paths)

/*!
 * \brief Minimum value supported for MAX_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_LT_MIN_PATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_paths)

/*!
 * \brief Maximum value supported for MAX_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_LT_MAX_PATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_paths)

/*!
 * \brief Minimum value supported for RH_NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for RH_NUM_PATHS field of the logical table.
 */
#define BCMECMP_LT_MIN_RH_NPATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_rh_npaths)

/*!
 * \brief Maximum value supported for RH_NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for RH_NUM_PATHS field of the logical table.
 */
#define BCMECMP_LT_MAX_RH_NPATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_rh_npaths)

/*!
 * \brief Minimum value supported for NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_LT_MIN_NPATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_npaths)

/*!
 * \brief Maximum value supported for NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_LT_MAX_NPATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_npaths)

/*!
 * \brief Default ECMP_NHOP field value for ECMP_OVERLAY LT entries.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Default value of ECMP_NHOP field for ECMP_OVERLAY LT entries.
 */
#define BCMECMP_LT_DEF_ECMP_NHOP(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->def_ecmp_nhop)

/*!
 * \brief Minimum value supported for ECMP_UNDERLAY_ID logical field.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for ECMP_UNDERLAY_ID logical field.
 */
#define BCMECMP_LT_MIN_UECMP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_uecmp_id)

/*!
 * \brief Maximum value supported for ECMP_UNDERLAY_ID logical field.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for ECMP_UNDERLAY_ID logical field.
 */
#define BCMECMP_LT_MAX_UECMP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_uecmp_id)

/*!
 * \brief Minimum value supported for NHOP_ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for NHOP_ID field of the logical table.
 */
#define BCMECMP_LT_MIN_NHOP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_nhop_id)

/*!
 * \brief Maximum value supported for NHOP_ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for NHOP_ID field of the logical table.
 */
#define BCMECMP_LT_MAX_NHOP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_nhop_id)

/*!
 * \brief Minimum value supported for RH_SIZE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for RH_SIZE field of the logical table.
 */
#define BCMECMP_LT_MIN_RH_SIZE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_rh_size_enc)

/*!
 * \brief Maximum value supported for RH_SIZE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for RH_SIZE field of the logical table.
 */
#define BCMECMP_LT_MAX_RH_SIZE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_rh_size_enc)

/*!
 * \brief ECMP group array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_GRP_INFO_PTR(_u) \
            ((BCMECMP_INFO_PTR(_u))->grp_arr)
/*!
 * \brief ECMP group array entry pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_GRP_INFO_ENT_PTR(_u,_id) \
           (((bcmecmp_grp_attr_t *) \
            ((BCMECMP_INFO_PTR(_u))->grp_arr)) + _id)

/*!
 * \brief ECMP backup group array entry pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id) \
           (((bcmecmp_grp_attr_t *) \
            ((BCMECMP_INFO_PTR(_u))->grp_bk_arr)) + _id)

/*!
 * \brief Group ECMP resolution type.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP resolution type of this group.
 */
#define BCMECMP_GRP_TYPE(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->grp_type)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_GRP_LT_SID(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->glt_sid)

/*!
 * \brief Get the load balancing mode of a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the ECMP group.
 */
#define BCMECMP_GRP_LB_MODE(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->lb_mode)

/*!
 * \brief Get the load balancing mode of a given weighted ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the weighted ECMP group.
 */
#define BCMECMP_GRP_WEIGHTED_MODE(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->weighted_mode)

/*!
 * \brief Get next-hop identifiers sorted status for given group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Status of next-hop identifiers sorted status of the group.
 */
#define BCMECMP_GRP_NHOP_SORTED(_u, _id) \
          ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->nhop_sorted)

/*!
 * \brief Maximum ECMP paths supported by this ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Maximum ECMP paths supported by the group.
 */
#define BCMECMP_GRP_MAX_PATHS(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->max_paths)

/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_GRP_NUM_PATHS(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->num_paths)

/*!
 * \brief Next-hop is Underlay (Level 2) ECMP Group indicator.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Status of Underlay ECMP Group ID as Next-hop.
 */
#define BCMECMP_GRP_ECMP_NHOP(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->ecmp_nhop

/*!
 * \brief Resilient hashing group size hardware encoding value of the group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group size hardware encoding value of the group.
 */
#define BCMECMP_GRP_RH_SIZE(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->rh_size_enc  \

/*!
 * \brief Resilient hashing group total number of members count.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group total number of members count.
 */
#define BCMECMP_GRP_RH_ENTRIES_CNT(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->rh_entries_cnt)

/*!
 * \brief Pointer to resilient hashing group data.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hashing group data.
 */
#define BCMECMP_GRP_RHG_PTR(_u, _id) \
             (BCMECMP_INFO_PTR(_u)->rhg[(_id)])

/*!
 * \brief Pointer to resilient hash load balanced members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced members array.
 */
#define BCMECMP_GRP_RH_ENTRIES_PTR(_u, _id) \
            (BCMECMP_GRP_RHG_PTR(_u, _id)->rh_entries_arr)

/*!
 * \brief Pointer to resilient hash load balanced member indices array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced member indices array.
 */
#define BCMECMP_GRP_RH_MINDEX_PTR(_u, _id) \
            (BCMECMP_GRP_RHG_PTR(_u, _id)->rh_mindex_arr)

/*!
 * \brief Number of member table entries occupied by a RH group member.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of member table entries occupied by a RH group member.
 */
#define BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(_u, _id) \
            (BCMECMP_GRP_RHG_PTR(_u, _id)->rh_memb_ecnt_arr)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_GRP_MEMB_TBL_START_IDX(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->mstart_idx)

/*!
 * \brief Aggregation group monitor pool in ECMP group table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Aggregation group monitor pool in ECMP group table.
 */
#define BCMECMP_GRP_MON_AGM_POOL(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->agm_pool

/*!
 * \brief Aggregation group monitor ID in ECMP group table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Aggregation group monitor ID in ECMP group table.
 */
#define BCMECMP_GRP_MON_AGM_ID(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->agm_id

/*!
 * \brief Enhanced flex counter action ID in ECMP group table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Enhanced flex counter action ID in ECMP group table.
 */
#define BCMECMP_GRP_CTR_ING_EFLEX_ACTION_ID(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->ctr_ing_eflex_action_id

/*!
 * \brief Enhanced flex counter object in ECMP group table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Enhanced flex counter object in ECMP group table.
 */
#define BCMECMP_GRP_CTR_ING_EFLEX_OBJECT(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->ctr_ing_eflex_object

/*!
 * \brief Pointer to group next-hop members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group next-hop members array.
 */
#define BCMECMP_GRP_NHOP_PTR(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->nhop_id

/*!
 * \brief Pointer to group weight array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group weight array.
 */
#define BCMECMP_GRP_WEIGHT_PTR(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->weight

/*!
 * \brief Pointer to group underlay next-hop members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group underlay next-hop members array.
 */
#define BCMECMP_GRP_UECMP_PTR(_u, _id) \
            BCMECMP_CURRENT_LT_ENT_PTR(_u)->uecmp_id
/*!
 * \brief ECMP group entries backup array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group high availability data array base pointer.
 */
#define BCMECMP_GRP_INFO_BK_PTR(_u) ((BCMECMP_INFO_PTR(_u))->grp_bk_arr)

/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_GRP_TRANS_ID(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->trans_id)

/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_GRP_TRANS_ID_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->trans_id)
/*!
 * \brief Group ECMP resolution type.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP resolution type of this group.
 */
#define BCMECMP_GRP_TYPE_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->grp_type)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_GRP_LT_SID_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->glt_sid)

/*!
 * \brief Get the load balancing mode of a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the ECMP group.
 */
#define BCMECMP_GRP_LB_MODE_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->lb_mode)

/*!
 * \brief Get the load balancing mode of a given weighted ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the weighted ECMP group.
 */
#define BCMECMP_GRP_WEIGHTED_MODE_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->weighted_mode)

/*!
 * \brief Get next-hop identifiers sorted status for given group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Status of next-hop identifiers sorted status of the group.
 */
#define BCMECMP_GRP_NHOP_SORTED_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->nhop_sorted)

/*!
 * \brief Maximum ECMP paths supported by this ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Maximum ECMP paths supported by the group.
 */
#define BCMECMP_GRP_MAX_PATHS_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->max_paths)

/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_GRP_NUM_PATHS_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->num_paths)

/*!
 * \brief Next-hop is Underlay (Level 2) ECMP Group indicator.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Status of Underlay ECMP Group ID as Next-hop.
 */
#define BCMECMP_GRP_ECMP_NHOP_BK(_u, _id) \
            (BCMECMP_CURRENT_LT_ENT_PTR(_u)->ecmp_nhop)

/*!
 * \brief Resilient hashing group size hardware encoding value of the group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group size hardware encoding value of the group.
 */
#define BCMECMP_GRP_RH_SIZE_BK(_u, _id) \
            (BCMECMP_CURRENT_LT_ENT_PTR(_u)->rh_size_enc)

/*!
 * \brief Resilient hashing group total number of members count.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group total number of members count.
 */
#define BCMECMP_GRP_RH_ENTRIES_CNT_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->rh_entries_cnt)

/*!
 * \brief Pointer to resilient hashing group data.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hashing group data.
 */
#define BCMECMP_GRP_RHG_BK_PTR(_u, _id) \
            (BCMECMP_INFO_PTR(_u)->rhg_bk[(_id)])

/*!
 * \brief Pointer to resilient hash load balanced members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced members array.
 */
#define BCMECMP_GRP_RH_ENTRIES_BK_PTR(_u, _id) \
            (BCMECMP_GRP_RHG_BK_PTR(_u, _id)->rh_entries_arr)

/*!
 * \brief Pointer to resilient hash load balanced member indices array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced member indices array.
 */
#define BCMECMP_GRP_RH_MINDEX_BK_PTR(_u, _id) \
            (BCMECMP_GRP_RHG_BK_PTR(_u, _id)->rh_mindex_arr)

/*!
 * \brief Number of member table entries occupied by a RH group member.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of member table entries occupied by a RH group member.
 */
#define BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(_u, _id) \
            (BCMECMP_GRP_RHG_BK_PTR(_u, _id)->rh_memb_ecnt_arr)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_GRP_MEMB_TBL_START_IDX_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->mstart_idx)

/*!
 * \brief Aggregation group monitor pool in ECMP group table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Aggregation group monitor pool in ECMP group table.
 */
#define BCMECMP_GRP_MON_AGM_POOL_BK(_u, _id) \
            (BCMECMP_CURRENT_LT_ENT_PTR(_u)->agm_pool)

/*!
 * \brief Aggregation group monitor ID in ECMP group table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Aggregation group monitor ID in ECMP group table.
 */
#define BCMECMP_GRP_MON_AGM_ID_BK(_u, _id) \
            (BCMECMP_CURRENT_LT_ENT_PTR(_u)->agm_id)

/*!
 * \brief Pointer to group next-hop members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group next-hop members array.
 */
#define BCMECMP_GRP_NHOP_BK_PTR(_u, _id) \
            (BCMECMP_CURRENT_LT_ENT_PTR(_u)->nhop_id)

/*!
 * \brief Pointer to group underlay next-hop members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group underlay next-hop members array.
 */
#define BCMECMP_GRP_UECMP_BK_PTR(_u, _id) \
            (BCMECMP_CURRENT_LT_ENT_PTR(_u)->uecmp_id)

/*!
 * \brief Pointer to the ECMP functions of a BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Pointer to the ECMP functions of the unit.
 */
#define BCMECMP_FNCALL_PTR(_u) (BCMECMP_CTRL(_u)->ecmp_drv)

/*!
 * \brief Function pointer of the specified ECMP function and BCM unit.
 *
 * \param [in] _u BCM unit number.
 * \param [in] FN ECMP function name.
 *
 * \returns Pointer to the requested ECMP function of the given BCM unit.
 */
#define BCMECMP_FNCALL(_u, FN) (BCMECMP_FNCALL_PTR(_u)->FN)

/*!
 * \brief Verify ECMP base function pointer and a specified function pointer are
 * initialized for a given BCM unit.
 *
 * \param [in] _u BCM unit number.
 * \param [in] FN ECMP function name.
 *
 * \returns TRUE if base function pointer and specified function's pointer are
 * initialized.
 * \returns FALSE if base function pointer and specified function's pointer are
 *  not initialized.
 */
#define BCMECMP_FNCALL_CHECK(_u, FN)                    \
            (BCMECMP_CTRL(_u) && BCMECMP_FNCALL_PTR(_u) \
             && BCMECMP_FNCALL(_u, FN))

/*!
 * \brief Execute an ECMP function already initialized for a BCM unit.
 *
 * \param [in] _u BCM unit number.
 * \param [in] FN ECMP function name.
 *
 * \returns Executes \c FN already initialized for the BCM unit \c _u.
 */
#define BCMECMP_FNCALL_EXEC(_u, FN) \
            (*(BCMECMP_FNCALL_PTR(_u)->FN))

/*!
 * \brief Get the pointer to hw entry from ecmp info structure.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Returns pointer to hardware entry.
 */
#define BCMECMP_HW_ENTRY_PTR(_p, _i) \
            (((bcmecmp_hw_entry_attr_t *) \
            ((_p)->ent_arr)) + _i)

/*!
 * \brief Get the pointer to backup hw entry from
 *  ecmp info structure.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Returns pointer to backup hardware entry.
 */
#define BCMECMP_HW_ENTRY_BK_PTR(_p, _i) \
            (((bcmecmp_hw_entry_attr_t *) \
            ((_p)->ent_bk_arr)) + _i)

/*!
 * \brief Get the reference count of an entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the reference count of the entry.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Reference count of the entry at offset index \c _i in \c _tptr
 * table.
 */
#define BCMECMP_TBL_REF_CNT(_tptr, _i)    \
            ((BCMECMP_HW_ENTRY_PTR(_tptr, _i))->ref_cnt)

/*!
 * \brief Get the reference count of the entry from the backup array.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the reference count of the entry from the backup array.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Reference count of the entry at offset index \c _i in \c _tptr
 * table.
 */
#define BCMECMP_TBL_BK_REF_CNT(_tptr, _i)    \
          ((BCMECMP_HW_ENTRY_BK_PTR(_tptr, _i))->ref_cnt)

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
#define BCMECMP_TBL_ENT_STAGED_SET(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_PTR(_tptr, _i))->flags) \
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
#define BCMECMP_TBL_ENT_STAGED_CLEAR(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_PTR(_tptr, _i))->flags) \
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
#define BCMECMP_TBL_ENT_STAGED_GET(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_PTR(_tptr, _i))->flags) \
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
#define BCMECMP_TBL_BK_ENT_STAGED_SET(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
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
#define BCMECMP_TBL_BK_ENT_STAGED_CLEAR(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
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
#define BCMECMP_TBL_BK_ENT_STAGED_GET(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_BK_PTR(_tptr, _i))->flags) \
            &(BCMECMP_ENTRY_STAGED))

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
#define BCMECMP_ENT_PRECONFIG_SET(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_PTR(_tptr, _i))->flags) \
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
#define BCMECMP_ENT_PRECONFIG_CLEAR(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_PTR(_tptr, _i))->flags) \
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
#define BCMECMP_ENT_PRECONFIG_GET(_tptr, _i)    \
            (((BCMECMP_HW_ENTRY_PTR(_tptr, _i))->flags) \
            & (BCMECMP_ENTRY_PRECONFIG))


/*!
 * \brief Increment the reference count for a range of entries in member table.
 *
 * Increment the reference count for a range of entries in ECMP Member table,
 * given the table poiner \c tbl_ptr, the start index \c start_idx of the range
 * and the count of entries \c count belonging to the group.
 *
 * \param [in] tbl_ptr Table pointer.
 * \param [in] start_idx Group start index.
 * \param [in] count Count of entries belonging to the group.
 * \param [in] idx_width Width of the index.
 *
 * \returns SHR_E_NONE Success.
 * \returns !SHR_E_NONE Failure.
 */
static inline int bcmecmp_tbl_ref_cnt_incr(const bcmecmp_tbl_prop_t *tbl_ptr,
                                           int start_idx,
                                           uint32_t count,
                                           uint32_t idx_width) {
    uint32_t offset;
    bcmecmp_hw_entry_attr_t *ent;

    if (!tbl_ptr) {
        return (SHR_E_INTERNAL);
    }

    count *= idx_width;

    for (offset = 0; offset < count; offset++) {
        ent = (((bcmecmp_hw_entry_attr_t *)
               (tbl_ptr->ent_arr)) + (start_idx + (int)offset));
        if (ent->ref_cnt == 0) {
            ent->ref_cnt++;
        }
    }
    return (SHR_E_NONE);
}

/*!
 * \brief Decrement the reference count for a range of entries.
 *
 * Decrement the reference count for a range of entries belonging to a table,
 * given the range start index \c _s_, the width of the range \c _w_ and the
 * table pointer.
 *
 * \param [in] tbl_ptr Table pointer.
 * \param [in] start_idx Group start index.
 * \param [in] count Count of entries belonging to the group.
 * \param [in] idx_width Width of the index.
 *
 * \returns SHR_E_NONE Success.
 * \returns !SHR_E_NONE Failure.
 */
static inline int bcmecmp_tbl_ref_cnt_decr(const bcmecmp_tbl_prop_t *tbl_ptr,
                                           int start_idx,
                                           uint32_t count,
                                           uint32_t idx_width) {
    uint32_t offset;
    bcmecmp_hw_entry_attr_t *ent;

    if (!tbl_ptr) {
        return (SHR_E_INTERNAL);
    }

    count *= idx_width;

    for (offset = 0; offset < count; offset++) {
        ent = (((bcmecmp_hw_entry_attr_t *)
              (tbl_ptr->ent_arr)) + (start_idx + (int)offset));
        if ((ent->ref_cnt) > 0) {
            ent->ref_cnt--;
        }
    }
    return (SHR_E_NONE);
}

/*!
 * \brief Set/Get the entry size in words for a given ECMP register.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware register name of bcmecmp_reg_prop_t type.
 *
 * \returns Given ECMP register entry size in words.
 */
#define BCMECMP_REG_ENT_SIZE(_u, _var) \
            (BCMECMP_INFO(_u)._var.ent_size)

/*!
 * \brief Set/Get ECMP config register DRD symbol identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var ECMP config register name of bcmecmp_reg_prop_t type.
 *
 * \returns ECMP config register DRD symbol identifier value.
 */
#define BCMECMP_REG_DRD_SID(_u, _var) \
            (BCMECMP_INFO(_u)._var.drd_sid)

/*!
 * \brief Set/Get ECMP config register LTD source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var ECMP config register name of bcmecmp_reg_prop_t type.
 *
 * \returns ECMP config register LTD source identifier value.
 */
#define BCMECMP_REG_LTD_SID(_u, _var) \
            (BCMECMP_INFO(_u)._var.ltd_sid)

/*!
 * \brief Set/Get device ECMP mode field DRD field identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var ECMP config register name of bcmecmp_reg_prop_t type.
 *
 * \returns ECMP config register LTD source identifier value.
 */
#define BCMECMP_MODE_DRD_FID(_u, _var) \
            (BCMECMP_INFO(_u)._var.mode_fid)

/*!
 * \brief Set/Get device ECMP resolution mode hardware encoding value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var ECMP config register name of bcmecmp_reg_prop_t type.
 * \param [in] _m Software ECMP mode value.
 *
 * \returns Device ECMP resolution mode hardware encoding value.
 */
#define BCMECMP_MODE_HW_VAL(_u, _var, _m) \
            (BCMECMP_INFO(_u)._var.mode_hw_val[(_m)])

/*!
 * \brief Set/Get device current ECMP resolution mode software value.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Device ECMP resolution mode software value.
 */
#define BCMECMP_MODE(_u) \
            (BCMECMP_INFO(_u).ecmp_mode)

/*!
 * \brief Device resilient hashing pseudo-random number generator seed value.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Device resilient hashing pseudo-random number generator seed value.
 */
#define BCMECMP_RH_RAND_SEED(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_PTR(_u,_id))->rh_rand)

/*!
 * \brief Set/Get the index remap feature.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Status of the index remap feature.
 */
#define BCMECMP_USE_INDEX_REMAP(_u) \
            (BCMECMP_INFO(_u).use_index_remap)

/*!
 * \brief Set/Get the member weight group list.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns The member weight group list.
 */
#define BCMECMP_MEMBER_WEIGHT_LIST(_u) \
            (&(BCMECMP_INFO(_u).member_weight_list))

/*!
 * \brief Set/Get the non-member weight group list.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns The non-member weight group list.
 */
#define BCMECMP_NON_MEMBER_WEIGHT_LIST(_u) \
            (&(BCMECMP_INFO(_u).non_member_weight_list))

/*!
 * \brief Device resilient hashing pseudo-random number generator seed value.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Device resilient hashing pseudo-random number generator seed value.
 */
#define BCMECMP_RH_RAND_SEED_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_ENT_BK_PTR(_u,_id))->rh_rand)

/*!
 * \brief Device resilient hashing pseudo-random number generator seed value.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Device resilient hashing pseudo-random number generator seed value.
 */
#define BCMECMP_INIT_RH_RAND_SEED(_u, _id) \
            (BCMECMP_CURRENT_LT_ENT_PTR(_u)->rh_rand_seed)

/*!
 * \brief Minimum value supported for weighted ECMP group size field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum weighted ECMP group size value supported by the logical table.
 */
#define BCMECMP_LT_MIN_WEIGHTED_SIZE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_weighted_size)

/*!
 * \brief Maximum value supported for weighted ECMP group size field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum weighted ECMP group size value supported by the logical table.
 */
#define BCMECMP_LT_MAX_WEIGHTED_SIZE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_weighted_size)

/*!
 * \brief Minimum value supported for weighted ECMP group mode field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum weighted ECMP group mode value supported by the logical table.
 */
#define BCMECMP_LT_MIN_WEIGHTED_MODE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_weighted_mode)

/*!
 * \brief Maximum value supported for weighted ECMP group mode field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum weighted ECMP group mode value supported by the logical table.
 */
#define BCMECMP_LT_MAX_WEIGHTED_MODE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_weighted_mode)

/*!
 * \brief Minimum value supported for weighted ECMP group weight field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum weighted ECMP group weight value supported by the logical table.
 */
#define BCMECMP_LT_MIN_WEIGHT(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_weight)

/*!
 * \brief Maximum value supported for weighted ECMP group weight field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum weighted ECMP group weight value supported by the logical table.
 */
#define BCMECMP_LT_MAX_WEIGHT(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_weight)

/*!
 * \brief Minimum value supported for MON_AGM_POOL field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for MON_AGM_POOL field of the logical table.
 */
#define BCMECMP_LT_MIN_MON_AGM_POOL(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_agm_pool)

/*!
 * \brief Maximum value supported for MON_AGM_POOL field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for MON_AGM_POOL field of the logical table.
 */
#define BCMECMP_LT_MAX_MON_AGM_POOL(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_agm_pool)

/*!
 * \brief Minimum value supported for MON_AGM_ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for MON_AGM_ID field of the logical table.
 */
#define BCMECMP_LT_MIN_MON_AGM_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_agm_id)

/*!
 * \brief Maximum value supported for MON_AGM_ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for MON_AGM_ID field of the logical table.
 */
#define BCMECMP_LT_MAX_MON_AGM_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_agm_id)

/*!
 * \brief Minimum value supported for CTR_ING_EFLEX_ACTION_ID field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for CTR_ING_EFLEX_ACTION_ID field of the
 * logical table.
 */
#define BCMECMP_LT_MIN_CTR_ING_EFLEX_ACTION_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_ctr_ing_eflex_action_id)

/*!
 * \brief Maximum value supported for CTR_ING_EFLEX_ACTION_ID field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for CTR_ING_EFLEX_ACTION_ID field of the
 * logical table.
 */
#define BCMECMP_LT_MAX_CTR_ING_EFLEX_ACTION_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_ctr_ing_eflex_action_id)

/*!
 * \brief Minimum value supported for CTR_ING_EFLEX_OBJECT field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for CTR_ING_EFLEX_OBJECT field of the
 * logical table.
 */
#define BCMECMP_LT_MIN_CTR_ING_EFLEX_OBJECT(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_ctr_ing_eflex_object)

/*!
 * \brief Maximum value supported for CTR_ING_EFLEX_OBJECT field of the
 * logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for CTR_ING_EFLEX_OBJECT field of the
 * logical table.
 */
#define BCMECMP_LT_MAX_CTR_ING_EFLEX_OBJECT(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_ctr_ing_eflex_object)

#define BCMECMP_GRP_IS_MEMBER_WEIGHT(_u, _id) \
    (BCMECMP_GRP_IS_WEIGHTED(BCMECMP_GRP_TYPE(_u, _id)) && \
     BCMECMP_GRP_WEIGHTED_MODE(_u, _id) == BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT)

#define BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(_u, _e) \
    (BCMECMP_GRP_IS_WEIGHTED(_e->grp_type) && \
     _e->weighted_mode == BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT)

static inline bool bcmecmp_grp_is_member_replication
                           (uint32_t gtype, uint32_t weight_mode) {
    if ((gtype == BCMECMP_GRP_TYPE_WEIGHTED ||
         gtype == BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED ||
         gtype == BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) &&
       weight_mode == BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION) {
        return true;
    }

    return false;
}

static inline bool bcmecmp_grp_is_overlay(uint32_t gtype) {
    if (gtype == BCMECMP_GRP_TYPE_OVERLAY ||
         gtype == BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) {
        return true;
    }

    return false;
}

static inline uint32_t bcmecmp_num_paths_convert_to_weighted_size
                           (uint32_t num_paths) {
    uint32_t weighted_size;

    switch (num_paths) {
        case 128:
            weighted_size = BCMECMP_WEIGHTED_SIZE_0_127;
            break;
        case 256:
            weighted_size = BCMECMP_WEIGHTED_SIZE_256;
            break;
        case 512:
            weighted_size = BCMECMP_WEIGHTED_SIZE_512;
            break;
        case 1024:
            weighted_size = BCMECMP_WEIGHTED_SIZE_1K;
            break;
        case 2048:
            weighted_size = BCMECMP_WEIGHTED_SIZE_2K;
            break;
        case 4096:
            weighted_size = BCMECMP_WEIGHTED_SIZE_4K;
            break;
        case 8192:
            weighted_size = BCMECMP_WEIGHTED_SIZE_8K;
            break;
        case 16384:
            weighted_size = BCMECMP_WEIGHTED_SIZE_16K;
            break;
        default:
            weighted_size = BCMECMP_WEIGHTED_SIZE_COUNT;
            break;
    }

    return weighted_size;
}

static inline uint32_t bcmecmp_weighted_size_convert_to_num_paths
                           (uint32_t weighted_size) {
    uint32_t num_paths;

    switch (weighted_size) {
        case BCMECMP_WEIGHTED_SIZE_0_127:
            num_paths = 128;
            break;
        case BCMECMP_WEIGHTED_SIZE_256:
            num_paths = 256;
            break;
        case BCMECMP_WEIGHTED_SIZE_512:
            num_paths = 512;
            break;
        case BCMECMP_WEIGHTED_SIZE_1K:
            num_paths = 1024;
            break;
        case BCMECMP_WEIGHTED_SIZE_2K:
            num_paths = 2048;
            break;
        case BCMECMP_WEIGHTED_SIZE_4K:
            num_paths = 4096;
            break;
        case BCMECMP_WEIGHTED_SIZE_8K:
            num_paths = 8192;
            break;
        case BCMECMP_WEIGHTED_SIZE_16K:
            num_paths = 16*1024;
            break;
        default:
            num_paths = 0;
            break;
    }

    return num_paths;
}
#endif /* BCMECMP_DB_INTERNAL_H */
