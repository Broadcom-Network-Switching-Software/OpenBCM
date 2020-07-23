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

/*!
 * Max trunk members supported in normal trunk mode.
 * Max members is fixed as 8 in fast trunk mode.
 */
#define BCMCTH_TRUNK_MAX_MEMBERS      (256)


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

/*! Min trunk members supported for system trunk. */
#define BCMCTH_TRUNK_SYSTEM_MIN_MEMBERS      (1)

/*! Invalid trunk identifier. */
#define BCMCTH_TRUNK_INVALID                 -1
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
    uint8_t       port[BCMCTH_TRUNK_SYSTEM_MAX_MEMBERS];

    /*! port_valid. */
    bool          port_valid[BCMCTH_TRUNK_SYSTEM_MAX_MEMBERS];

    /*! Resolution disable. */
    bool          disable;

    /*! Resolution disable valid. */
    bool          disable_valid;
} bcmcth_trunk_sys_grp_param_t;

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


#endif /* BCMCTH_TRUNK_TYPES_H */

