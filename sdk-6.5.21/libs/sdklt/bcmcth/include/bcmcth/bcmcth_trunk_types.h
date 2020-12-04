/*! \file bcmcth_trunk_types.h
 *
 * This file contains data types used by TRUNK custom handler component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_TYPES_H
#define BCMCTH_TRUNK_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_itbm.h>
#include <bcmcth/generated/trunk_ha.h>

/*! Max trunk groups supported. */
#define BCMCTH_TRUNK_MAX_TRUNK        (1024)

/*! Max resilient trunk group array size. */
#define BCMCTH_TRUNK_RH_GRP_INFO_PTR_ARRAY_SIZE    (1024)

/*!
 * Max trunk members supported in normal trunk mode.
 * Max members is fixed as 8 in fast trunk mode.
 */
#define BCMCTH_TRUNK_MAX_MEMBERS      (256)

/*!
 * Max trunk members supported in resilient trunk mode.
 * Max members is fixed as 8 in fast trunk mode.
 */
#define BCMCTH_TRUNK_RESILIENT_MAX_MEMBERS      (256)

/*! Mim trunk members supported in normal trunk mode. */
#define BCMCTH_TRUNK_MIN_MEMBERS      (1)

/*! Max trunk members supported in fast trunk mode. */
#define BCMCTH_TRUNK_FAST_MAX_MEMBERS (64)

/*! Max failover port members for a port. */
#define BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS  (8)

/*! HA subcomponent id for trunk group. */
#define BCMCTH_TRUNK_GRP_SUB_COMP_ID      (1)
/*! HA subcomponent id for trunk fast group. */
#define BCMCTH_TRUNK_FAST_GRP_SUB_COMP_ID (2)
/*! HA subcomponent id for trunk failover entry. */
#define BCMCTH_TRUNK_FAILOVER_SUB_COMP_ID (3)
/*! HA subcomponent id for trunk member bitmap. */
#define BCMCTH_TRUNK_MEMBER_BMP_SUB_COMP_ID (4)

/*! HA subcomponent id for system trunk group. */
#define BCMCTH_TRUNK_SYSTEM_GRP_SUB_COMP_ID  (5)

/*! HA subcomponent id for system trunk member bitmap. */
#define BCMCTH_TRUNK_SYSTEM_MEMBER_BMP_SUB_COMP_ID (6)

/*! HA subcomponent id for system trunk failover group. */
#define BCMCTH_TRUNK_SYSTEM_FAILOVER_SUB_COMP_ID  (7)

/*! Encap base tunnel structure version. */
#define BCMCTH_TRUNK_MEMBER_BMAP_T_VER            1

/*! Encap base tunnel structure signature. */
#define BCMCTH_TRUNK_MEMBER_BMAP_T_SIG            0x12261ee5fd9a1111

/*! Maximum system trunk supported */
#define BCMCTH_TRUNK_MAX_TRUNK_SYSTEM        (64)

/*! Max trunk members supported for system trunk. */
#define BCMCTH_TRUNK_SYSTEM_MAX_MEMBERS      (128)

/*!
 * Max trunk members supported for
 * resilient hashing system trunk group.
 */
#define BCMCTH_TRUNK_SYSTEM_RH_MAX_MEMBERS      (64)

/*! Min trunk members supported for system trunk. */
#define BCMCTH_TRUNK_SYSTEM_MIN_MEMBERS      (1)

/*! Invalid trunk identifier. */
#define BCMCTH_TRUNK_INVALID                 -1

/*! Max resilient system trunk group array size. */
#define BCMCTH_TRUNK_SYSTEM_RH_GRP_INFO_PTR_ARRAY_SIZE    (64)

/*!
 * Resilient trunk group entry flagged
 * for preconfiguration.
 */
#define BCMCTH_TRUNK_ENTRY_PRECONFIG (1 << 0)

/*!
 * Resilient system trunk group entry
 * flagged for preconfiguration.
 */
#define BCMCTH_TRUNK_SYSTEM_ENTRY_PRECONFIG (1 << 0)

/*!
 * Random number 1 used in the calculation of final
 * random seed value useed by the resilient hashing load
 * balancing mode group for the member redistribution
 * into the member table.
 */
#define BCMCTH_TRUNK_RESILIENT_MAGIC_NUM_1 1103515245

/*!
 * Random number 2 used in the calculation of final
 * random seed value useed by the resilient hashing load
 * balancing mode group for the member redistribution
 * into the member table.
 */
#define BCMCTH_TRUNK_RESILIENT_MAGIC_NUM_2 12345

/*!
 * \brief TRUNK_VP Memory allocation macro.
 *
 * Allocates \c _size bytes, clears the memory and returns a pointer to the
 * allocated memory \c in _ptr.
 *
 * \param [in] _ptr Pointer to the allocated memory.
 * \param [in] _size Size in bytes.
 * \param [in] _descr Description for the allocated memory.
 *
 * \returns Pointer to allocated memory.
 */
#define BCMCTH_TRUNK_ALLOC(_ptr, _size, _descr)      \
    do {                                        \
        SHR_ALLOC((_ptr), (_size), _descr);     \
        SHR_NULL_CHECK((_ptr), SHR_E_MEMORY);   \
        sal_memset((_ptr), 0, (_size));         \
    } while(0)

/*!
 * \brief TRUNK_VP Memory free macro.
 *
 * Frees the memory space pointed by \c _ptr, which must have been returned
 * by a previous call to BCMCTH_TRUNK_VP_ALLOC() macro.
 *
 * \param [in] _ptr Pointer to the memory space to be freed.
 *
 * \returns Nothing.
 */
#define BCMCTH_TRUNK_FREE(_ptr) SHR_FREE(_ptr)

/*! Trunk Hg LB mode. */
typedef enum bcmcth_trunk_lb_mode_e {
    /*! Regular hash mode. */
    BCMCTH_TRUNK_LB_HASH = 0,

    /*! Random mode. */
    BCMCTH_TRUNK_LB_RANDOM = 1,

    /*! Round robin mode. */
    BCMCTH_TRUNK_LB_RR = 2,

    BCMCTH_TRUNK_LB_INVALID = 4
} bcmcth_trunk_lb_mode_t;

/*!
 * \brief TRUNK_RESILIENT group size.
 */
typedef enum bcmcth_trunk_rh_size_e {
    BCMCTH_TRUNK_RH_SIZE_64 = 0,
    BCMCTH_TRUNK_RH_SIZE_128,
    BCMCTH_TRUNK_RH_SIZE_COUNT
} bcmcth_trunk_rh_size_t;

/*!
 * \brief TRUNK_SYSTEM_RESILIENT group size.
 */
typedef enum bcmcth_trunk_sys_rh_size_e {
    BCMCTH_TRUNK_SYSTEM_RH_SIZE_64 = 0,
    BCMCTH_TRUNK_SYSTEM_RH_SIZE_COUNT
} bcmcth_trunk_sys_rh_size_t;


/*!
 * \brief System trunk  group lookup types.
 */
typedef enum bcmcth_trunk_sys_trunk_gtype_e {
    /*! Regular system trunk group. */
    BCMCTH_TRUNK_SYSTEM_GRP_TYPE_REGULAR = 0,

    /*! Resilient system trunk group. */
    BCMCTH_TRUNK_SYSTEM_GRP_TYPE_RESILIENT,

    BCMCTH_TRUNK_SYSTEM_GRP_TYPE_COUNT,
} bcmcth_trunk_sys_trunk_gtype_t;

/*! Bookkeeping info for TRUNK LT. */
typedef struct bcmcth_trunk_grp_bk_s {
    /*! Trunk group info. */
    bcmcth_trunk_group_t    *grp;

    /*! Total member bitmap for TRUNK_MEMBER resource. */
    SHR_BITDCL          *mbmp;

    /*! Size of TRUNK_MEMBER table. */
    int                 member_size;
} bcmcth_trunk_grp_bk_t;

/*!
 * \brief TRUNK  group lookup types.
 */
typedef enum bcmcth_trunk_imm_trunk_gtype_e {
    /*! Regular trunk group. */
    BCMCTH_TRUNK_GRP_TYPE_REGULAR = 0,

    /*! Resilient trunk group. */
    BCMCTH_TRUNK_GRP_TYPE_RESILIENT,

    BCMCTH_TRUNK_GRP_TYPE_COUNT,
} bcmcth_trunk_imm_trunk_gtype_t;


/*!
 * It is used to save parameters from LTM while doing parsing LT fields.
 * It is used for TRUNK LT.
 */
typedef struct bcmcth_trunk_group_param_s {
    /*! Trunk id. */
    uint32_t      id;

    /*! Trunk id valid. */
    bool          id_valid;

    /*! Trunk load balancing mode(regualr hash, random or roud robin). */
    uint8_t       lb_mode;

    /*! Load balancing valid */
    bool          lb_mode_valid;

    /*! Hash rule tag for member selection. */
    uint8_t       uc_rtag;

    /*! Uc_rtag valid. */
    bool          uc_rtag_valid;

    /*!
     * Max possible members of this trunk assumed by end-users.
     * SDK will reserve TRUNK_MEMBER resource for this trunk group.
     * It can improve performance to processing trunk change, but may
     * waste some TRUNK_MEMBER resources.
     * It is not supported in fast trunk mode.
     * Max members for fast trunk group is fixed as 8.
     */
    uint16_t      uc_max_members;

    /*! Uc_max_members valid. */
    bool          uc_max_members_valid;

    /*! Actual egress member count. */
    uint16_t      uc_cnt;

    /*! Uc_cnt valid. */
    bool          uc_cnt_valid;

    /*! Module ID array of egress members for unicast packets. */
    uint8_t       uc_modid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Uc_modid valid. */
    bool          uc_modid_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Port ID array of egress members for unicast packets. */
    uint16_t      uc_modport[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Uc_modport valid. */
    bool          uc_modport_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! AGM pool. */
    uint8_t       uc_agm_pool;

    /*! Mon_agm_pool valid. */
    bool          uc_agm_pool_valid;

    /*! AGM id. */
    uint8_t       uc_agm_id;

    /*! Mon_agm_id valid. */
    bool          uc_agm_id_valid;

    /*! Egress member count for non-unicast packets. */
    uint16_t      nonuc_cnt;

    /*! Nonuc_cnt valid. */
    bool          nonuc_cnt_valid;

    /*! Module ID array of egress members for non-unicast packets. */
    uint8_t       nonuc_modid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Nonuc_modid valid. */
    bool          nonuc_modid_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Port ID array of egress members for non-unicast packets. */
    uint16_t      nonuc_modport[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Nonuc_modport valid. */
    bool          nonuc_modport_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! System port ID array for unicast system ports. */
    uint16_t      uc_system_port[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! System port ID array for unicast system ports. */
    bool          uc_system_port_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! System port ID array for non-unicast system ports. */
    uint16_t      nonuc_system_port[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Valid for non-unicast system ports. */
    bool          nonuc_system_port_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! l2_oif. */
    uint32_t      l2_oif[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! l2_oif_valid. */
    bool          l2_oif_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Egress disable flag for unicast system ports. */
    bool          uc_egr_block[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Valid for egress disable flag for unicast system ports. */
    bool          uc_egr_block_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Egress disable flag for unicast system ports. */
    bool          nonuc_egr_block[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Valid for egress disable flag for non unicast system ports. */
    bool          nonuc_egr_block_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Group type. */
    bcmcth_trunk_imm_trunk_gtype_t   \
                  gtype;

    /*! Resilient group size. */
     bcmcth_trunk_rh_size_t          \
                  rh_size;

    /*! Reslient random seed. */
    uint32_t      rh_uc_random_seed;

    /*! Valid reslient random seed.*/
    uint32_t      rh_uc_random_seed_valid;

    /*!
     * Numumber of paths configured for this TRUNK
     * resilient hashing group field value.
     */
    uint32_t      rh_member_cnt;

    /*!
     * Array of RH TRUNK group entry's
     * mapped member's array index.
     */
    uint32_t      *rh_mindex_arr;

    /*!
     * Number of member table entries occupied
     * by TRUNK member in a
     * resilient hashing load balancing mode group.
     */
    uint32_t     *rh_memb_ecnt_arr;

    /*!
     * Array of resilient hashing TRUNK
     * group load balanced members.
     */
    uint16_t     *rh_system_port;

    /*!
     * Array of resilient hashing TRUNK
     * group load balanced members.
     */
    uint32_t     *rh_l2_oif;

    /*! System port ID array for unicast system ports. */
    uint16_t     unblk_rh_uc_system_port[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! l2_oif. */
    uint32_t     unblk_rh_l2_oif[BCMCTH_TRUNK_MAX_MEMBERS];

} bcmcth_trunk_group_param_t;

/*! Bookkeeping info for TRUNK FAST LT. */
typedef struct bcmcth_trunk_fgrp_bk_s {
    /*! Trunk group info. */
    bcmcth_trunk_fast_group_t    *grp;

    /*! Different device has different max members. */
    uint32_t max_members;

} bcmcth_trunk_fgrp_bk_t;

/*!
 * It is used to save parameters from LTM while doing parsing LT fields.
 * It is used for TRUNK_FAST LT.
 */
typedef struct bcmcth_trunk_fast_group_param_s {
    /*! Trunk id. */
    uint32_t      id;

    /*! Trunk id valid. */
    bool          id_valid;

    /*! Trunk load balancing mode(regualr hash, random or roud robin). */
    uint8_t       lb_mode;

    /*! Load balancing valid */
    bool          lb_mode_valid;

    /*! Hash rule tag for member selection. */
    uint8_t       uc_rtag;

    /*! Uc_rtag valid. */
    bool          uc_rtag_valid;

    /*! Actual egress member count. */
    uint16_t      uc_cnt;

    /*! Uc_cnt valid. */
    bool          uc_cnt_valid;

    /*! Module ID array of egress members for unicast packets. */
    uint8_t       uc_modid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Uc_modid valid. */
    bool          uc_modid_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Port ID array of egress members for unicast packets. */
    uint16_t      uc_modport[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! Uc_modport valid. */
    bool          uc_modport_valid[BCMCTH_TRUNK_MAX_MEMBERS];

    /*! AGM pool. */
    uint8_t       uc_agm_pool;

    /*! Mon_agm_pool valid. */
    bool          uc_agm_pool_valid;

    /*! AGM id. */
    uint8_t       uc_agm_id;

    /*! Mon_agm_id valid. */
    bool          uc_agm_id_valid;

    /*! Egress member count for non-unicast packets. */
    uint16_t      nonuc_cnt;

    /*! Nonuc_cnt valid. */
    bool          nonuc_cnt_valid;

    /*! Module ID array of egress members for non-unicast packets. */
    uint8_t       nonuc_modid[BCMCTH_TRUNK_FAST_MAX_MEMBERS];

    /*! Nonuc_modid valid. */
    bool          nonuc_modid_valid[BCMCTH_TRUNK_FAST_MAX_MEMBERS];

    /*! Port ID array of egress members for non-unicast packets. */
    uint16_t      nonuc_modport[BCMCTH_TRUNK_FAST_MAX_MEMBERS];

    /*! Nonuc_modport valid. */
    bool          nonuc_modport_valid[BCMCTH_TRUNK_FAST_MAX_MEMBERS];

    /*! Ing_eflex_action_sel valid. */
    bool          ing_eflex_action_id_valid;

    /*!
     * Select one enhanced flex counter action from
     * TRUNK_CTR_ING_EFLEX_ACTION logical table.
     */
    uint8_t       ing_eflex_action_id;

    /*! Ing_eflex_object valid. */
    bool          ing_eflex_object_valid;

    /*! Enhanced flex counter object. */
    uint16_t      ing_eflex_object;

} bcmcth_trunk_fast_group_param_t;


/*! Save parameters from LTM while parsing TRUNK_FAILOVER lt fields. */
typedef struct bcmcth_trunk_failover_param_s {
    /*! Failover port id. */
    uint32_t      id;

    /*! Failover port id valid */
    bool          id_valid;

    /*! Hash rule tag for failover. */
    uint8_t       rtag;

    /*! Rtag valid. */
    bool          rtag_valid;

    /*! Actual failover list count. */
    uint16_t      failover_cnt;

    /*! Failover cnt valid. */
    bool          failover_cnt_valid;

    /*! Module ID array of failover ports. */
    uint8_t       failover_modid[BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS];

    /*! Module id valid. */
    bool          failover_modid_valid[BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS];

    /*! Port ID array of failover ports. */
    uint16_t      failover_modport[BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS];

    /*! Modport id valid. */
    bool          failover_modport_valid[BCMCTH_TRUNK_FAILOVER_MAX_MEMBERS];

} bcmcth_trunk_failover_param_t;

/*! Bookkeeping info for TRUNK_FALIOVER LT. */
typedef struct bcmcth_trunk_failover_bk_s {
    /*! Trunk failover info. */
    bcmcth_trunk_failover_t    *entry;

    /*! Maximum number of logical ports. */
    uint16_t                max_ports;

} bcmcth_trunk_failover_bk_t;

/*!
 * \brief TRUNK_SYSTEM  group lookup types.
 */
typedef enum bcmcth_trunk_sys_grp_gtype_e {

    /*! Regular system trunk group. */
    BCMCTH_TRUNK_SYS_GRP_TYPE_REGULAR = 0,

    /*! Resilient system trunk group. */
    BCMCTH_TRUNK_SYS_GRP_TYPE_RESILIENT,

    BCMCTH_TRUNK_SYS_GRP_TYPE_COUNT,
} bcmcth_trunk_sys_grp_gtype_t;

/*!
 * \brief Structure for Resilient hashing TRUNK group information.
 */
typedef struct bcmcth_trunk_sys_grp_rh_s {

    /*! Port ID array for unicast system ports. */
    uint16_t      *rh_port;

    /*! Array of RH TRUNK group entry's mapped member's array index. */
    uint32_t      *rh_mindex_arr;

    /*!
     * Number of member table entries occupied by
     * a member entry in a resilient hashing
     * load balancing mode group.
     */
    uint32_t      *rh_memb_ecnt_arr;
} bcmcth_trunk_sys_grp_rh_t;

/*! Bookkeeping info for TRUNK_SYSTEM LT imm. */
typedef struct bcmcth_trunk_sys_grp_bk_s {
    /*! System Trunk group info. */
    bcmcth_trunk_imm_sys_group_ha_blk_t    *grp_ptr;

    /*! System trunk group bitmap mbmp. */
    bcmcth_trunk_imm_sys_group_mbmp_ha_blk_t    *mbmp_ptr;

    /*! Maximum system trunk member count. */
    uint32_t                    max_member_cnt;

    /*! Maximum system trunk group count. */
    uint32_t                    max_group_cnt;

    /*! ITBM handle for defragmentation management.  */
    shr_itbm_list_hdl_t     itbm_hdl;

    /*! True to support ITBM defragmentation. */
    bool                    itbm_support;

    /*! Resilient system trunk group information. */
    bcmcth_trunk_sys_grp_rh_t
        *rhg[BCMCTH_TRUNK_SYSTEM_RH_GRP_INFO_PTR_ARRAY_SIZE];
} bcmcth_trunk_sys_grp_bk_t;

/*!
 * It is used to save parameters from LTM while doing parsing LT fields.
 * It is used for TRUNK LT.
 */
typedef struct bcmcth_trunk_sys_grp_param_s {
    /*! Trunk id. */
    uint32_t      id;

    /*! Trunk load balancing mode(regualr hash, random or roud robin). */
    uint8_t       lb_mode;

    /*!Trunk load balancing mode valid. */
    bool          lb_mode_valid;

    /*!
     * Max possible members of this system trunk assumed by end-users.
     * SDK will reserve TRUNK_SYSTEM_MEMBER resource for this trunk group.
     * It can improve performance to processing trunk change, but may
     * waste some TRUNK_SYSTEM_MEMBER resources.
     */
    uint16_t      max_members;

    /*! max_members_valid. */
    bool          max_members_valid;

    /*! Actual egress member count. */
    uint16_t      cnt;

    /*! cnt_valid. */
    bool          cnt_valid;

    /*! Port ID array for unicast system ports. */
    uint16_t       port[BCMCTH_TRUNK_SYSTEM_MAX_MEMBERS];

    /*! port_valid. */
    bool          port_valid[BCMCTH_TRUNK_SYSTEM_MAX_MEMBERS];

    /*! Resolution disable. */
    bool          disable;

    /*! Resolution disable valid. */
    bool          disable_valid;

    /*! Group type. */
    bcmcth_trunk_sys_trunk_gtype_t   \
                  gtype;

    /*! Resilient group size. */
     bcmcth_trunk_sys_rh_size_t          \
                  rh_size;

    /*! Reslient random seed. */
    uint32_t      rh_random_seed;

    /*! Valid reslient random seed.*/
    uint32_t      rh_random_seed_valid;

    /*!
     * Array of RH TRUNK_SYSTEM group entry's
     * mapped member's array index.
     */
    uint32_t      *rh_mindex_arr;

    /*!
     * Number of member table entries occupied
     * by TRUNK_SYSTEM member in a
     * resilient hashing load balancing mode group.
     */
    uint32_t     *rh_memb_ecnt_arr;

    /*!
     * Array of resilient hashing TRUNK_SYSTEM
     * group load balanced members.
     */
    uint16_t     *rh_port;
} bcmcth_trunk_sys_grp_param_t;

/*!
 * \brief Structure for Resilient hashing TRUNK group information.
 */
typedef struct bcmcth_trunk_imm_group_rh_s {

    /*! Resilient system port ID array for unicast system ports. */
    uint16_t      *rh_system_port;

    /*! rh_l2_oif. */
    uint32_t      *rh_l2_oif;


    /*! Array of RH TRUNK group entry's mapped member's array index. */
    uint32_t      *rh_mindex_arr;

    /*!
     * Number of member table entries occupied by
     * a member entry in a resilient hashing
     * load balancing mode group.
     */
    uint32_t      *rh_memb_ecnt_arr;
} bcmcth_trunk_imm_group_rh_t;

/*! Bookkeeping info for TRUNK LT IMM. */
typedef struct bcmcth_trunk_imm_grp_bk_s {
    /*! Trunk group array. */
    bcmcth_trunk_imm_group_ha_blk_t    *grp_ptr;

    /*! Trunk group mbmp array. */
    bcmcth_trunk_imm_group_mbmp_ha_blk_t    *mbmp_ptr;

    /*! Maximum trunk group member count. */
    uint32_t                max_member_cnt;

    /*! Maximum trunk group count. */
    uint32_t                max_group_cnt;

    /*! ITBM handle for defragmentation management.  */
    shr_itbm_list_hdl_t     itbm_hdl;

    /*! True to support ITBM defragmentation. */
    bool                    itbm_support;

    /*! Resilient group information. */
    bcmcth_trunk_imm_group_rh_t
            *rhg[BCMCTH_TRUNK_RH_GRP_INFO_PTR_ARRAY_SIZE];
} bcmcth_trunk_imm_grp_bk_t;

/*!
 * \brief TRUNK group defragmentation structure.
 */
typedef struct bcmcth_trunk_grp_defrag_s {
    /*! Trunk group identifier. */
    uint32_t     trunk_id;

    /*! Member table start index of this group. */
    int group_base;

    /*! Number of member table entries used by this group. */
    uint32_t group_size;

    /*! Operational argument. */
    const bcmltd_op_arg_t *op_arg;
} bcmcth_trunk_grp_defrag_t;

/*!
 * \brief TRUNK resilient hashing group members operation type enum.
 */
typedef enum bcmcth_trunk_rh_oper_e {
    /*! No operation to be performed on RH group members. */
    BCMCTH_TRUNK_RH_OPER_NONE = 0,

    /*! RH group members add operation. */
    BCMCTH_TRUNK_RH_OPER_ADD = 1,

    /*! RH group members delete operation. */
    BCMCTH_TRUNK_RH_OPER_DELETE = 2,

    /*! RH group members replace operation. */
    BCMCTH_TRUNK_RH_OPER_REPLACE = 3,

    /*! RH group members add and replace operation. */
    BCMCTH_TRUNK_RH_OPER_ADD_REPLACE = 4,

    /*! RH group members delete and replace operation. */
    BCMCTH_TRUNK_RH_OPER_DELETE_REPLACE = 5,

    /*! Must be last one. */
    BCMCTH_TRUNK_RH_OPER_COUNT = 6
} bcmcth_trunk_rh_oper_t;

/*! \brief TRUNK resilient hashing group members operation name. */
#define BCMCTH_TRUNK_RH_OPER_NAME { \
            "NONE",            \
            "ADD",             \
            "DELETE",          \
            "REPLACE",         \
            "ADD_REPLACE",     \
            "DELETE_REPLACE"   \
}

static inline uint32_t bcmcth_trunk_grp_max_members_to_rh_size
                           (uint32_t max_members) {
    uint32_t rh_size;

    switch (max_members) {
        case 64:
            rh_size = BCMCTH_TRUNK_RH_SIZE_64;
            break;
        case 128:
            rh_size = BCMCTH_TRUNK_RH_SIZE_128;
            break;
        default:
            rh_size = BCMCTH_TRUNK_RH_SIZE_COUNT;
            break;
    }

    return rh_size;
}

static inline uint32_t bcmcth_trunk_grp_rh_size_to_max_members
                           (uint32_t rh_size) {
    uint32_t max_members;

    switch (rh_size) {
        case BCMCTH_TRUNK_RH_SIZE_64:
            max_members = 64;
            break;
        case BCMCTH_TRUNK_RH_SIZE_128:
            max_members = 128;
            break;
        default:
            max_members = 0;
            break;
    }

    return max_members;
}

static inline uint32_t bcmcth_trunk_sys_grp_max_members_to_rh_size
                           (uint32_t max_members) {
    uint32_t rh_size;

    switch (max_members) {
        case 64:
            rh_size = BCMCTH_TRUNK_SYSTEM_RH_SIZE_64;
            break;
        default:
            rh_size = BCMCTH_TRUNK_SYSTEM_RH_SIZE_COUNT;
            break;
    }

    return rh_size;
}

static inline uint32_t bcmcth_trunk_sys_grp_rh_size_to_max_members
                           (uint32_t rh_size) {
    uint32_t max_members;

    switch (rh_size) {
        case BCMCTH_TRUNK_SYSTEM_RH_SIZE_64:
            max_members = 64;
            break;
        default:
            max_members = 0;
            break;
    }

    return max_members;
}
#endif /* BCMCTH_TRUNK_TYPES_H */

