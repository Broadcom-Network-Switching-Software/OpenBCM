/*! \file bcmcth_dlb_drv.h
 *
 * BCMCTH Dynamic Load Balance (DLB) driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_DLB_DRV_H
#define BCMCTH_DLB_DRV_H

#include <shr/shr_bitop.h>

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmcth/generated/dlb_ha.h>

/*! Maximum number of BCM Units supported. */
#define BCMPORT_NUM_UNITS_MAX BCMDRD_CONFIG_MAX_UNITS

/*! Invalid Index or ID. */
#define BCMPORT_INVALID (-1)

/*! Install hardware entry in all pipe instances. */
#define BCMPORT_ALL_PIPES_INST (-1)

/*! The max number of physical ports per chip */
#define BCMPORT_NUM_PPORTS_PER_CHIP_MAX   (256)

/*! HA sub-module IDs */
typedef enum {
    BCMPTM_HA_SUBID_DLB_ECMP = 1,
    BCMPTM_HA_SUBID_DLB_ECMP_STATS = 2,
    BCMPTM_HA_SUBID_DLB_TRUNK_ENT = 3,
    BCMPTM_HA_SUBID_DLB_TRUNK_MBMP = 4,
} bcmcth_dlb_ha_sub_id_t;

/*! DLB ID.  */
typedef bcmdrd_id_t bcmcth_dlb_id_t;

/*! Port ID.  */
typedef bcmdrd_id_t bcmcth_dlb_port_id_t;

/*! Next-hop ID.  */
typedef bcmdrd_id_t bcmcth_dlb_nhop_id_t;

/*!
 * \brief This macro is used to set a Field present indicator bit status.
 *
 * Sets the Field \c _f present bit indicator in the Field Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMCTH_DLB_LT_FIELD_SET(_m, _f)    \
            do {                        \
                SHR_BITSET(_m, _f);;      \
            } while (0)

/*!
 * \brief This macro is used to get the Field present indicator bit status.
 *
 * Returns the Field \c _f present bit indicator status from the Field
 * Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Field present indicator bit status from the Field bitmap.
 */
#define BCMCTH_DLB_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))

/*!
 * \brief DLB ECMP LT fields information.
 */
typedef enum bcmcth_dlb_ecmp_lt_fields_e {
    BCMCTH_DLB_ECMP_LT_FIELD_DLB_ID = 0,
    BCMCTH_DLB_ECMP_LT_FIELD_INACTIVITY_TIME = 1,
    BCMCTH_DLB_ECMP_LT_FIELD_ASSIGNMENT_MODE = 2,
    BCMCTH_DLB_ECMP_LT_FIELD_FLOW_SET_SIZE = 3,
    BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_COST = 4,
    BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_BIAS = 5,
    BCMCTH_DLB_ECMP_LT_FIELD_PRIMARY_PATH_THRESHOLD = 6,
    BCMCTH_DLB_ECMP_LT_FIELD_NUM_PATHS = 7,
    BCMCTH_DLB_ECMP_LT_FIELD_NHOP_IDS = 8,
    BCMCTH_DLB_ECMP_LT_FIELD_PORT_IDS = 9,
    BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NUM_PATHS = 10,
    BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NHOP_IDS = 11,
    BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PORT_IDS = 12,
} bcmcth_dlb_ecmp_lt_fields_t;

/*!
 * \brief DLB_ECMP_STATS LT fields information.
 */
typedef enum bcmcth_dlb_ecmp_stats_lt_fields_e {
    BCMCTH_DLB_ECMP_STATS_LT_FIELD_DLB_ID = 0,
    BCMCTH_DLB_ECMP_STATS_LT_FIELD_FAIL_CNT = 1,
    BCMCTH_DLB_ECMP_STATS_LT_FIELD_PORT_REASSIGNMENT_CNT = 2,
    BCMCTH_DLB_ECMP_STATS_LT_FIELD_MEMBER_REASSIGNMENT_CNT = 3,
} bcmcth_dlb_ecmp_stats_lt_fields_t;

/*!
 * \brief DLB ECMP LT fields information.
 */
typedef enum bcmcth_dlb_ecmp_pctl_lt_fields_e {
    BCMCTH_DLB_ECMP_PCTL_LT_FIELD_PORT_ID = 0,
    BCMCTH_DLB_ECMP_PCTL_LT_FIELD_OVERRIDE = 1,
    BCMCTH_DLB_ECMP_PCTL_LT_FIELD_FORCE_LINK_STATUS = 2,
    BCMCTH_DLB_ECMP_PCTL_LT_FIELD_COUNT
} bcmcth_dlb_ecmp_pctl_lt_fields_t;

/*! Maximum number of members in an ECMP group. */
#define BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS       64

/*! Maximum field count of DLB IMM LTs. */
#define DLB_FIELD_COUNT_MAX     265

/*! Maximum field ID of DLB LTs. */
#define DLB_FIELD_ID_MAX        13

/*! Invlalid flowset base ptr. */
#define DLB_INVALID_FLOWSET_BASE        0xFFFF

/*!
 * \brief Structure for DLB ECMP LT entry information.
 */
typedef struct bcmcth_dlb_ecmp_s {

    /*! DLB group LT symbol ID. */
    bcmltd_sid_t glt_sid;

    /*! LT operation argument. */
    const bcmltd_op_arg_t *op_arg;

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, DLB_FIELD_ID_MAX);

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(pri_nh_array_bmp, BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS);

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(pri_port_array_bmp, BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS);

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(alt_nh_array_bmp, BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS);

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(alt_port_array_bmp, BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS);

    /*! Store LT entry logical DLB group ID key field value. */
    bcmcth_dlb_id_t dlb_id;

    /*! Inactivity duration in microseconds. */
    uint16_t inactivity_time;
    /*! DLB operational mode. */

    uint8_t assignment_mode;

    /*! Encoding defining the number of flow sets for an aggregation group. */
    uint8_t flow_set_size;

    /*! Cost to switch over to an alternative path. */
    uint8_t alternate_path_cost;

    /*! Bias to switch over to an alternative path. */
    uint8_t alternate_path_bias;

    /*!
     * Indicates the primary path is always selected when
     * primary optimal quality band is above this threshold.
     */
    uint8_t primary_path_threshold;

    /*! Primary ECMP DLB group size. */
    uint8_t num_paths;

    /*! L3_UC_NHOP LT indices for primary next hops. */
    bcmcth_dlb_nhop_id_t nhop_id[BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS];

    /*! DLB PORT ID Array for primary next hops. */
    shr_port_t port_id[BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS];

    /*! Alternate ECMP DLB group size. */
    uint8_t num_alternate_paths;

   /*! L3_UC_NHOP LT indices for alternate next hops. */
    bcmcth_dlb_nhop_id_t alternate_nhop_id[BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS];

    /*! DLB PORT ID Array for alternate next hops. */
    shr_port_t alternate_port_id[BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS];

    /*! DLB flow set block base. */
    uint16_t flow_set_base;

    /*! Previous DLB flow set block size. */
    uint8_t prev_flow_set_size;

    /*! Previous DLB flow set base. */
    uint16_t prev_flow_set_base;
} bcmcth_dlb_ecmp_t;

/*!
 * \brief Structure for DLB_ECMP_STATS LT entry information.
 */
typedef struct bcmcth_dlb_ecmp_stats_s {

    /*! LT operation argument. */
    const bcmltd_op_arg_t *op_arg;

    /*! Bitmap of fields to be operated. */
    SHR_BITDCLNAME(fbmp, DLB_FIELD_ID_MAX);

    /*! LT key. */
    bcmcth_dlb_id_t dlb_id;

    /*! Number of packets failed due to DLB mechanism. */
    uint64_t fail_cnt;

    /*! Total aggregate port member assignments. */
    uint64_t port_reassignment_cnt;

    /*! Total aggregate member assignments. */
    uint64_t member_reassignment_cnt;
} bcmcth_dlb_ecmp_stats_t;

/*!
 * \brief Structure for DLB_ECMP_PORT_CONTROL LT entry information.
 */
typedef struct bcmcth_dlb_ecmp_pctl_s {

    /*! DLB Group LT symbol ID. */
    bcmltd_sid_t glt_sid;

    /*! LT operation argument. */
    const bcmltd_op_arg_t *op_arg;

    /*! Bitmap of fields to be operated. */
    SHR_BITDCLNAME(fbmp, DLB_FIELD_ID_MAX);

    /*! Store LT entry logical port ID key field value. */
    bcmcth_dlb_port_id_t port_id;

    /*! Enable override mode. */
    bool override;

    /*! Link status of the port. */
    bool force_link_status;

} bcmcth_dlb_ecmp_pctl_t;


/*!
  * \brief Structure for DLB_ECMP LTs and hardware tables information.
  */
typedef struct bcmcth_dlb_ecmp_info_s {

    /*! DLB_ECMP LT entry local info. */
    bcmcth_dlb_ecmp_entry_info_t *ent_arr;

    /*! DLB_ECMP LT entry HA info. */
    bcmcth_dlb_ecmp_entry_info_t *ha_ent_arr;

} bcmcth_dlb_ecmp_info_t;

/*!
  * \brief Structure for DLB_ECMP_STATS LTs and hardware tables information.
  */
typedef struct bcmcth_dlb_ecmp_stats_info_s {
    /*! DLB_ECMP_STATS LT entry HA info. */
    bcmcth_dlb_ecmp_stats_entry_info_t *ha_ent_arr;

} bcmcth_dlb_ecmp_stats_info_t;

/*! Maximum number of members in an DLB TRUNK group. */
#define BCMCTH_DLB_TRUNK_GROUP_MAX_MEMBERS 64

/*! Maximum quantized value. */
#define BCMCTH_DLB_MAX_QUANTIZED_VALUE  7

/*!
 * \brief DLB TRUNK LT fields information.
 */
typedef enum bcmcth_dlb_trunk_lt_fields_e {
    BCMCTH_DLB_TRUNK_LT_FIELD_DLB_ID = 0,
    BCMCTH_DLB_TRUNK_LT_FIELD_INACTIVITY_TIME = 1,
    BCMCTH_DLB_TRUNK_LT_FIELD_ASSIGNMENT_MODE = 2,
    BCMCTH_DLB_TRUNK_LT_FIELD_FLOW_SET_SIZE = 3,
    BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_PATH_COST = 4,
    BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_PATH_BIAS = 5,
    BCMCTH_DLB_TRUNK_LT_FIELD_PRIMARY_PATH_THRESHOLD = 6,
    BCMCTH_DLB_TRUNK_LT_FIELD_NUM_PATHS = 7,
    BCMCTH_DLB_TRUNK_LT_FIELD_NHOP_IDS = 8,
    BCMCTH_DLB_TRUNK_LT_FIELD_PORT_IDS = 9,
    BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_NUM_PATHS = 10,
    BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_NHOP_IDS = 11,
    BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_PORT_IDS = 12,
    BCMCTH_DLB_TRUNK_LT_FIELD_COUNT
} bcmcth_dlb_trunk_lt_fields_t;

/*!
 * \brief DLB TRUNK port LT fields information.
 */
typedef enum bcmcth_dlb_trunk_pctl_lt_fields_e {
    BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_PORT_ID = 0,
    BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_OVERRIDE = 1,
    BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_FORCE_LINK_STATUS = 2,
    BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_COUNT
} bcmcth_dlb_trunk_pctl_lt_fields_t;

/*!
  * \brief Structure for DLB_TRUNK LT book keeping.
  */
typedef struct bcmcth_dlb_trunk_blk_s {
  /*! DLB_TRUNK LT entry info. */
  bcmcth_dlb_trunk_entry_ha_blk_t *ent_arr;

  /*! DLB_TRUNK LT flow set block bitmap. */
  bcmcth_dlb_trunk_mbmp_ha_blk_t  *mbmp_arr;
} bcmcth_dlb_trunk_blk_t;

/*!
  * \brief Structure for DLB_TRUNK LT.
  */
typedef struct bcmcth_dlb_trunk_info_s {
    /*! DLB_TRUNK LT entry local info. */
    bcmcth_dlb_trunk_blk_t *blk_ptr;

    /*! DLB_TRUNK LT entry HA info. */
    bcmcth_dlb_trunk_blk_t *ha_blk_ptr;
} bcmcth_dlb_trunk_info_t;


/*!
 * \brief Structure for DLB TRUNK LT entry.
 */
typedef struct bcmcth_dlb_trunk_s {
    /*! DLB Trunk Logial Table Symbol ID. */
    bcmltd_sid_t       glt_sid;

    /*! LT operation argument. */
    const bcmltd_op_arg_t *op_arg;

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, BCMCTH_DLB_TRUNK_LT_FIELD_COUNT);

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(pri_port_array_bmp, BCMCTH_DLB_TRUNK_GROUP_MAX_MEMBERS);

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(alt_port_array_bmp, BCMCTH_DLB_TRUNK_GROUP_MAX_MEMBERS);

    /*! Store LT entry Logical DLB group Identifier key field value. */
    bcmcth_dlb_id_t        dlb_id;

    /*! Inactivity duration in microseconds. */
    uint16_t          inactivity_time;
    /*! DLB operational mode. */

    uint8_t            assignment_mode;

    /*! Cost to switch over to an alternative path. */
    uint8_t            alternate_path_cost;
    /*! Bias to switch over to an alternative path. */
    uint8_t            alternate_path_bias;

    /*!
     * Indicates the primary path is always selected when
     * primary optimal quality band is above this threshold.
     */
    uint8_t            primary_path_threshold;

    /*! Primary ECMP DLB group size. */
    uint8_t            num_paths;

    /*! DLB PORT ID Array for primary next hops. */
    uint8_t            port_id[BCMCTH_DLB_TRUNK_GROUP_MAX_MEMBERS];

    /*! Alternate ECMP DLB group size. */
    uint8_t            num_alternate_paths;

    /*! DLB PORT ID Array for alternate next hops. */
    uint8_t            alternate_port_id[BCMCTH_DLB_TRUNK_GROUP_MAX_MEMBERS];

    /*! Encoding defining the number of flow sets for an aggregation group. */
    uint8_t            flow_set_size;

    /*! DLB flow set block base. */
    uint16_t           flow_set_base;

    /*! Previous DLB flow set block size. */
    uint8_t            prev_flow_set_size;

    /*! Previous DLB flow set base. */
    uint16_t           prev_flow_set_base;
} bcmcth_dlb_trunk_t;

/*!
 * \brief Structure for
 *  DLB_TRUNK_PORT_CONTROL LT entry.
 */
typedef struct bcmcth_dlb_trunk_pctl_s
{
   /*! DLB Trunk Logial Table Symbol ID. */
   bcmltd_sid_t       glt_sid;

    /*! Operation argument. */
    const bcmltd_op_arg_t *op_arg;

   /*! Bitmap of fields to be operated. */
   SHR_BITDCLNAME(fbmp, BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_COUNT);

   /*! Logical Port ID. */
   uint8_t port_id;

   /*! SW override. */
   bool override;

   /*! SW post status. */
   bool port_st;

} bcmcth_dlb_trunk_pctl_t;

/*!
 * \brief Initialize device-specific DLB driver.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot flag.
 * \param [in] entry_info HA info of DLB ECMP entry.
 *
 * \return SHR_E_NONE No errors.
 */
typedef int (*bcmcth_dlb_ecmp_init_f)(
    int unit,
    bool warm,
    bcmcth_dlb_ecmp_entry_info_t *entry_info);

/*!
 * \brief Clean up device-specific DLB driver.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
typedef int (*bcmcth_dlb_ecmp_cleanup_f)(int unit);

/*!
 * \brief Get DLB group count.
 *
 * \param [in] unit Unit number.
 * \param [out] grp_cnt Number of DLB groups.
 *
 * \return SHR_E_NONE No errors.
 */
typedef int (*bcmcth_dlb_grp_cnt_get_f)(int unit, int *grp_cnt);

/*!
 * \brief Add DLB_ECMP LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 * \param [in] flow_set_init Flag indicating allocation
 *             of flow set resources to DLB group.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_dlb_ecmp_set_f)(int unit,
                                     bcmcth_dlb_ecmp_t *lt_entry,
                                     bool flow_set_init);

/*!
 * \brief Delete DLB_ECMP LT entry from hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for delete not found.
 */
typedef int (*bcmcth_dlb_ecmp_del_f)(int unit,
                                     bcmcth_dlb_ecmp_t *lt_entry);

/*!
 * \brief Add DLB_ECMP_PORT_CONTROL LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_dlb_ecmp_pctl_set_f)(int unit,
                                          bcmcth_dlb_ecmp_pctl_t *lt_entry);

/*!
 * \brief Delete DLB_ECMP_PORT_CONTROL LT entry from hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for delete not found.
 */
typedef int (*bcmcth_dlb_ecmp_pctl_del_f)(int unit,
                                          bcmcth_dlb_ecmp_pctl_t *lt_entry);

/*!
 * \brief Recover chip-level DLB ECMP info.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_info HA info of DLB ECMP entry.
 *
 * \return SHR_E_NONE No errors.
 */
typedef int (*bcmcth_dlb_ecmp_info_recover_f)(
    int unit,
    bcmcth_dlb_ecmp_entry_info_t *entry_info);

/*!
 * \brief Add DLB_TRUNK LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] blk_ptr DLB_TRUNK LT block pointer.
 * \param [in] lt_entry Pointer to LT entry data.
 * \param [in] flow_set_init Flag indicating allocation
 *             of flow set resources to DLB group.
 *
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_dlb_trunk_set_f)(
                          int unit,
                          bcmcth_dlb_trunk_blk_t *blk_ptr,
                          bcmcth_dlb_trunk_t *lt_entry,
                          bool flow_set_init);

/*!
 * \brief Delete DLB_TRUNK LT entry from hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] blk_ptr DLB_TRUNK LT block pointer.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for delete not found.
 */
typedef int (*bcmcth_dlb_trunk_del_f)(
                           int unit,
                           bcmcth_dlb_trunk_blk_t *blk_ptr,
                           bcmcth_dlb_trunk_t *lt_entry);

/*!
 * \brief Add DLB_TRUNK_PORT_CONTROL LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
typedef int (*bcmcth_dlb_trunk_pctl_set_f)(
                           int unit,
                           bcmcth_dlb_trunk_pctl_t *lt_entry);

/*!
 * \brief Get DLB trunk group parameters .
 *
 * \param [in] unit Unit number.
 * \param [out] grp_cnt Number of DLB trunk groups.
 * \param [out] flow_set_blk_cnt Flow set block count of DLB trunk groups.
 *
 * \return SHR_E_NONE No errors.
 */
typedef int (*bcmcth_dlb_trunk_grp_param_f)(
                                 int unit,
                                 int *grp_cnt,
                                 int *flow_set_blk_cnt);

/*!
 * \brief Set DLB_ECMP_STATS LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
typedef int (*bcmcth_dlb_ecmp_stats_set_f)(int unit,
                                           bcmcth_dlb_ecmp_stats_t *lt_entry);

/*!
 * \brief Get DLB_ECMP_STATS LT entry from hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
typedef int (*bcmcth_dlb_ecmp_stats_get_f)(int unit,
                                           bcmcth_dlb_ecmp_stats_t *lt_entry);

/*!
 * \brief DLB driver object
 *
 * DLB driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH DLB module by
 * \ref bcmcth_dlb_drv_init from the top layer. BCMCTH DLB internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_dlb_drv_s {

    /*! Initialize device-specific DLB driver. */
    bcmcth_dlb_ecmp_init_f ecmp_init;

    /*! Clean up device-specific DLB driver. */
    bcmcth_dlb_ecmp_cleanup_f ecmp_cleanup;

    /*! Get DLB group count. */
    bcmcth_dlb_grp_cnt_get_f grp_cnt_get;

    /*! Add DLB_ECMP LT entry to hardware tables. */
    bcmcth_dlb_ecmp_set_f ecmp_set;

    /*! Delete DLB_ECMP LT entry from hardware tables. */
    bcmcth_dlb_ecmp_del_f ecmp_del;

    /*! Add DLB_ECMP_PORT_CONTROL LT entry to hardware tables. */
    bcmcth_dlb_ecmp_pctl_set_f ecmp_pctl_set;

    /*! Delete DLB_ECMP_PORT_CONTROL LT entry from hardware tables. */
    bcmcth_dlb_ecmp_pctl_del_f ecmp_pctl_del;

    /*! Recover chip-level DLB ECMP info. */
    bcmcth_dlb_ecmp_info_recover_f ecmp_recover;

    /*! Get DLB group parameters. */
    bcmcth_dlb_trunk_grp_param_f trunk_grp_param_get;

    /*! Add DLB_TRUNK LT entry to hardware tables. */
    bcmcth_dlb_trunk_set_f trunk_set;

    /*! Delete DLB_TRUNK LT entry from hardware tables. */
    bcmcth_dlb_trunk_del_f trunk_del;

    /*! Add DLB_TRUNK_PORT_CONTROL LT entry to hardware tables. */
    bcmcth_dlb_trunk_pctl_set_f trunk_pctl_set;

    /*! Set DLB_ECMP_STATS LT entry to hardware tables. */
    bcmcth_dlb_ecmp_stats_set_f ecmp_stats_set;

    /*! Get DLB_ECMP_STATS LT entry from hardware tables. */
    bcmcth_dlb_ecmp_stats_get_f ecmp_stats_get;

} bcmcth_dlb_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern bcmcth_dlb_drv_t *_bc##_cth_dlb_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Install the device-specific DLB driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_dlb_drv_init(int unit);

/*!
 * \brief Initialize device-specific DLB ECMP driver.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot flag.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmcth_dlb_ecmp_init(int unit, bool warm);

/*!
 * \brief Get DLB group count.
 *
 * \param [in] unit Unit number.
 * \param [out] grp_cnt Number of DLB groups.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmcth_dlb_grp_cnt_get(int unit, int *grp_cnt);

/*!
 * \brief Clean up device-specific DLB ECMP driver.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmcth_dlb_ecmp_cleanup(int unit);

/*!
 * \brief Add DLB_ECMP LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 * \param [in] flow_set_init Flag indicating allocation
 *             of flow set resources to DLB group.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_dlb_ecmp_set(int unit,
                    bcmcth_dlb_ecmp_t *lt_entry,
                    bool flow_set_init);

/*!
 * \brief Delete DLB_ECMP LT entry from hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for delete not found.
 */
extern int
bcmcth_dlb_ecmp_del(int unit,
                    bcmcth_dlb_ecmp_t *lt_entry);

/*!
 * \brief Add DLB_ECMP_PORT_CONTROL LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_dlb_ecmp_pctl_set(int unit,
                         bcmcth_dlb_ecmp_pctl_t *lt_entry);

/*!
 * \brief Delete DLB_ECMP_PORT_CONTROL LT entry from hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for delete not found.
 */
extern int
bcmcth_dlb_ecmp_pctl_del(int unit,
                         bcmcth_dlb_ecmp_pctl_t *lt_entry);

/*!
 * \brief Recover chip-level DLB ECMP info.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_info HA info of DLB ECMP entry.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmcth_dlb_ecmp_info_recover(int unit,
                             bcmcth_dlb_ecmp_entry_info_t *entry_info);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The DLB custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_dlb_ecmp_imm_init(int unit);

/*!
 * \brief Get DLB ECMP entry info.
 *
 * \param [in] unit
 * \param [out] ecmp_info Returned DLB ecmp info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_dlb_ecmp_info_get(int unit, bcmcth_dlb_ecmp_info_t **ecmp_info);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The DLB custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_dlb_ecmp_pctl_imm_init(int unit);

/*!
 * \brief DLB_ECMP logical table resources alloc.
 *
 * \param [in] unit Unit number.
 * \param [in] warm True if warm boot.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_dlb_ecmp_resources_alloc(int unit, bool warm);

/*!
 * \brief DLB_ECMP logical table resources free.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_dlb_ecmp_resources_free(int unit);

/*!
 * \brief DLB_TRUNK information resources alloc.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] warm flag indicating cold boot or warm boot
 * enable for warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
extern int
bcmcth_dlb_trunk_resources_alloc(int unit, bool warm);

/*!
 * \brief DLB_TRUNK information resources free.
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
extern int
bcmcth_dlb_trunk_resources_free(int unit);

/*!
 * \brief DLB_TRUNK LT imm registration
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to register the imm callbacks.
 */
extern int
bcmcth_dlb_trunk_imm_init(int unit);

/*!
 * \brief DLB_TRUNK_PORT_CONTROL LT imm registration
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to register the imm callbacks.
 */
extern int
bcmcth_dlb_trunk_pctl_imm_init(int unit);

/*!
 * \brief Add DLB_TRUNK LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 * \param [in] blk_ptr Pointer to dlb trunk group entry.
 * \param [in] flow_set_init Flag indicating allocation
 *             of flow set resources to DLB group.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_dlb_trunk_set(int unit,
                    bcmcth_dlb_trunk_blk_t *blk_ptr,
                    bcmcth_dlb_trunk_t *lt_entry,
                    bool flow_set_init);

/*!
 * \brief Delete DLB_TRUNK LT entry from hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] blk_ptr Pointer to dlb trunk group entry.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for delete not found.
 */
extern int
bcmcth_dlb_trunk_del(int unit,
                    bcmcth_dlb_trunk_blk_t *blk_ptr,
                    bcmcth_dlb_trunk_t *lt_entry);

/*!
 * \brief Add DLB_TRUNK_PORT_CONTROL LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
extern int
bcmcth_dlb_trunk_pctl_set(int unit,
                         bcmcth_dlb_trunk_pctl_t *lt_entry);

/*!
 * \brief Set the DLB Trunk group count.
 *
 * \param [in] unit Unit number.
 * \param [out] grp_cnt Group count.
 * \param [out] flow_set_blk_cnt Flow set block count.
 *
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if call fails.
 */
extern int
bcmcth_dlb_trunk_param_get(int unit,
                           int *grp_cnt,
                           int *flow_set_blk_cnt);

/*!
 * \brief DLB_ECMP_STATS logical table resources alloc.
 *
 * \param [in] unit Unit number.
 * \param [in] warm True if warm boot.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_dlb_ecmp_stats_resources_alloc(int unit, bool warm);

/*!
 * \brief DLB_ECMP_STATS logical table resources free.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_dlb_ecmp_stats_resources_free(int unit);

/*!
 * \brief Set DLB_ECMP_STATS LT entry to hardware tables.
 *
 * Reserves hardware table resources and installs LT entry fields to
 * the corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
extern int
bcmcth_dlb_ecmp_stats_set(int unit,
                          bcmcth_dlb_ecmp_stats_t *lt_entry);

/*!
 * \brief Get DLB_ECMP_STATS LT entry from hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
extern int
bcmcth_dlb_ecmp_stats_get(int unit,
                          bcmcth_dlb_ecmp_stats_t *lt_entry);

#endif /* BCMCTH_DLB_DRV_H */
