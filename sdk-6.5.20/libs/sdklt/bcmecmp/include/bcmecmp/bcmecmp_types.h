/*! \file bcmecmp_types.h
 *
 * This file contains ECMP custom table handler data type definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_TYPES_H
#define BCMECMP_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_int_comp.h>

/*! Maximum number of BCM Units supported. */
#define BCMECMP_NUM_UNITS_MAX BCMDRD_CONFIG_MAX_UNITS

/*! Invalid Index or ID. */
#define BCMECMP_INVALID (-1)

/*! Install hardware entry in all pipe instances. */
#define BCMECMP_ALL_PIPES_INST (-1)

/*! Logical table entry staged status indicator. */
#define BCMECMP_ENTRY_STAGED    (1 << 0)

/*! Perform pre-configuration recovery operation post warm start. */
#define BCMECMP_ENTRY_PRECONFIG (1 << 1)

/*! Logical table entry defragment staged status indicator. */
#define BCMECMP_ENTRY_DEFRAG_STAGED (1 << 2)

/*!
 * \brief ECMP Memory allocation macro.
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
#define BCMECMP_ALLOC(_ptr, _size, _descr)      \
    do {                                        \
        SHR_ALLOC((_ptr), (_size), _descr);     \
        SHR_NULL_CHECK((_ptr), SHR_E_MEMORY);   \
        sal_memset((_ptr), 0, (_size));         \
    } while(0)

/*!
 * \brief ECMP Memory free macro.
 *
 * Frees the memory space pointed by \c _ptr, which must have been returned
 * by a previous call to BCMECMP_ALLOC() macro.
 *
 * \param [in] _ptr Pointer to the memory space to be freed.
 *
 * \returns Nothing.
 */
#define BCMECMP_FREE(_ptr) SHR_FREE(_ptr)

/*! HA sub-component ID for staged ECMP groups configuration block. */
#define BCMECMP_GRP_SUB_COMP_ID               (1)

/*! HA sub-component ID for committed ECMP groups configuration block. */
#define BCMECMP_GRP_BK_SUB_COMP_ID            (2)

/*! HA sub-component ID for staged ECMP group table entries block. */
#define BCMECMP_GRP_TBL_SUB_COMP_ID           (3)

/*! HA sub-component ID for committed ECMP group table entries block. */
#define BCMECMP_GRP_TBL_BK_SUB_COMP_ID        (4)

/*! HA sub-component ID for staged ECMP member table entries block. */
#define BCMECMP_MEMB_TBL_SUB_COMP_ID          (5)

/*! HA sub-component ID for committed ECMP member table entries block. */
#define BCMECMP_MEMB_TBL_BK_SUB_COMP_ID       (6)

/*! HA sub-component ID for staged ECMP flex group table entries block. */
#define BCMECMP_FLEX_GRP_TBL_SUB_COMP_ID      (7)

/*! HA sub-component ID for committed ECMP flex group table entries block. */
#define BCMECMP_FLEX_GRP_TBL_BK_SUB_COMP_ID   (8)

/*! HA sub-component ID for staged ECMP flex member table entries block. */
#define BCMECMP_FLEX_MEMB_TBL_SUB_COMP_ID     (9)

/*! HA sub-component ID for committed ECMP flex member table entries block. */
#define BCMECMP_FLEX_MEMB_TBL_BK_SUB_COMP_ID  (10)

/*! HA sub-component ID for staged ECMP flex level0 member table entries block.*/
#define BCMECMP_FLEX_MEMB0_SUB_COMP_ID     (11)

/*! HA sub-component ID for committed ECMP flex level0 member table
  entries block.*/
#define BCMECMP_FLEX_MEMB0_BK_SUB_COMP_ID  (12)

/*! HA sub-component ID for staged ECMP flex level1 member table entries block.*/
#define BCMECMP_FLEX_MEMB1_SUB_COMP_ID     (13)

/*! HA sub-component ID for committed ECMP flex level1 member table
  entries block. */
#define BCMECMP_FLEX_MEMB1_BK_SUB_COMP_ID  (14)

/*! HA sub-component ID for staged ECMP flex RH group table entries block.*/
#define BCMECMP_FLEX_RH_SUB_COMP_ID     (15)

/*! HA sub-component ID for committed ECMP flex RH group table entries block.*/
#define BCMECMP_FLEX_RH_BK_SUB_COMP_ID  (16)

/*! ECMP Group Identifier.  */
typedef int bcmecmp_id_t;

/*! NextHop index. */
typedef int bcmecmp_nhop_id_t;

/*! Logical Table Definition Field Identifier. */
typedef int bcmecmp_ltd_fid_t;

/*!
 * \brief Device ECMP modes.
 *
 * List of ECMP modes supported by the device.
 */
typedef enum bcmecmp_mode_e {
    /*! Full feature single level ECMP resolution. */
    BCMECMP_MODE_FULL = 0,

    /*! Two level hierarchical ECMP resolution. */
    BCMECMP_MODE_HIER = 1,

    /*! Fast single level ECMP resolution. */
    BCMECMP_MODE_FAST = 2,

    /*! Must be the last one. */
    BCMECMP_MODE_COUNT
} bcmecmp_mode_t;

/*! \brief Device ECMP mode names. */
#define BCMECMP_MODE_NAME { \
            "FULL",         \
            "HIERARCHICAL", \
            "FAST"          \
}

/*!
 * \brief ECMP resilient hashing group members operation type enum.
 */
typedef enum bcmecmp_rh_oper_e {
    /*! No operation to be performed on RH group members. */
    BCMECMP_RH_OPER_NONE = 0,

    /*! RH group members add operation. */
    BCMECMP_RH_OPER_ADD = 1,

    /*! RH group members delete operation. */
    BCMECMP_RH_OPER_DELETE = 2,

    /*! RH group members replace operation. */
    BCMECMP_RH_OPER_REPLACE = 3,

    /*! RH group members add and replace operation. */
    BCMECMP_RH_OPER_ADD_REPLACE = 4,

    /*! RH group members delete and replace operation. */
    BCMECMP_RH_OPER_DELETE_REPLACE = 5,

    /*! Must be last one. */
    BCMECMP_RH_OPER_COUNT = 6
} bcmecmp_rh_oper_t;

/*! \brief ECMP resilient hashing group members operation name. */
#define BCMECMP_RH_OPER_NAME { \
            "NONE",            \
            "ADD",             \
            "DELETE",          \
            "REPLACE",         \
            "ADD_REPLACE",     \
            "DELETE_REPLACE"   \
}

/*!
 * \brief ECMP Load Balance Modes.
 */
typedef enum bcmecmp_lb_mode_e {
    /*! Regular Load Balance Mode. */
    BCMECMP_LB_MODE_REGULAR = 0,

    /*! Resilient Hash Load Balance Mode. */
    BCMECMP_LB_MODE_RESILIENT,

    /*! Random Load Balance Mode. */
    BCMECMP_LB_MODE_RANDOM,

    /*! Round Robin Load Balance Mode. */
    BCMECMP_LB_MODE_ROUNDROBIN,

    /*! Must be the last one. */
    BCMECMP_LB_MODE_COUNT
} bcmecmp_lb_mode_t;

/*! \brief ECMP load balancing mode names. */
#define BCMECMP_LB_MODE_NAME {  \
            "REGULAR",          \
            "RESILIENT",        \
            "RANDOM",           \
            "ROUNDROBIN",       \
}

/*!
 * \brief weighted ECMP group size.
 */
typedef enum bcmecmp_weighted_size_e {
    BCMECMP_WEIGHTED_SIZE_0_127 = 2,
    BCMECMP_WEIGHTED_SIZE_256,
    BCMECMP_WEIGHTED_SIZE_512,
    BCMECMP_WEIGHTED_SIZE_1K,
    BCMECMP_WEIGHTED_SIZE_2K,
    BCMECMP_WEIGHTED_SIZE_4K,
    BCMECMP_WEIGHTED_SIZE_8K,
    BCMECMP_WEIGHTED_SIZE_16K,
    BCMECMP_WEIGHTED_SIZE_COUNT
} bcmecmp_weighted_size_t;

/*!
 * \brief weighted ECMP group size.
 */
typedef enum bcmecmp_flex_weighted_size_e {
    BCMECMP_FLEX_WEIGHTED_SIZE_256 = 2,
    BCMECMP_FLEX_WEIGHTED_SIZE_512,
    BCMECMP_FLEX_WEIGHTED_SIZE_1K,
    BCMECMP_FLEX_WEIGHTED_SIZE_2K,
    BCMECMP_FLEX_WEIGHTED_SIZE_4K,
    BCMECMP_FLEX_WEIGHTED_SIZE_8K,
    BCMECMP_FLEX_WEIGHTED_SIZE_16K,
    BCMECMP_FLEX_WEIGHTED_SIZE_COUNT
} bcmecmp_flex_weighted_size_t;

/*!
 * \brief ECMP Load Balance Modes.
 */
typedef enum bcmecmp_flex_lb_mode_e {
    /*! Regular Load Balance Mode. */
    BCMECMP_FLEX_LB_MODE_REGULAR = 0,

    /*! Random Load Balance Mode. */
    BCMECMP_FLEX_LB_MODE_RANDOM,

    /*! Must be the last one. */
    BCMECMP_FLEX_LB_MODE_COUNT
} bcmecmp_flex_lb_mode_t;

/*! \brief ECMP load balancing mode names. */
#define BCMECMP_FLEX_LB_MODE_NAME {  \
            "REGULAR",          \
            "RANDOM",           \
}

/*! \brief weighted ECMP group size names. */
#define BCMECMP_FLEX_WEIGHTED_SIZE_NAME {  \
                 "WEIGHTED_SIZE_256",      \
                 "WEIGHTED_SIZE_512",      \
                 "WEIGHTED_SIZE_1K",       \
                 "WEIGHTED_SIZE_2K",       \
                 "WEIGHTED_SIZE_4K",       \
                 "WEIGHTED_SIZE_8K",       \
                 "WEIGHTED_SIZE_16K"       \
}

/*! \brief weighted ECMP group size names. */
#define BCMECMP_WEIGHTED_SIZE_NAME {  \
            "WEIGHTED_SIZE_0_127",    \
            "WEIGHTED_SIZE_256",      \
            "WEIGHTED_SIZE_512",      \
            "WEIGHTED_SIZE_1K",       \
            "WEIGHTED_SIZE_2K",       \
            "WEIGHTED_SIZE_4K",       \
            "WEIGHTED_SIZE_8K",       \
            "WEIGHTED_SIZE_16K"       \
}

/*! \brief underlay weighted ECMP group size names. */
#define BCMECMP_UNLAY_WEIGHTED_SIZE_NAME {  \
            "WEIGHTED_SIZE_0_511",    \
            "",                       \
            "",                       \
            "WEIGHTED_SIZE_1K",       \
            "WEIGHTED_SIZE_2K",       \
            "WEIGHTED_SIZE_4K",       \
}

typedef enum bcmecmp_weighted_mode_e {
    BCMECMP_WEIGHTED_MODE_MEMBER_NONE = 0,
    BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION = 1,
    BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT = 2,
    BCMECMP_WEIGHTED_MODE_COUNT
} bcmecmp_weighted_mode_t;

/*! \brief weighted ECMP group mode names. */
#define BCMECMP_WEIGHTED_MODE_NAME {  \
            "MEMBER_NONE",     \
            "MEMBER_REPLICATION",     \
            "MEMBER_WEIGHT",          \
}

/*!
 * \brief ECMP group lookup types.
 */

typedef enum bcmecmp_grp_type_e {
    BCMECMP_GRP_TYPE_SINGLE = 0,
    BCMECMP_GRP_TYPE_OVERLAY,
    BCMECMP_GRP_TYPE_UNDERLAY,
    BCMECMP_GRP_TYPE_WEIGHTED,
    BCMECMP_GRP_TYPE_WEIGHTED_0,
    BCMECMP_GRP_TYPE_LEVEL0,
    BCMECMP_GRP_TYPE_LEVEL1,
    BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED,
    BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED,
    /* Fixed overlay weighted. */
    BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED,
    /* Fixed underlay weighted. */
    BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED,
    BCMECMP_GRP_TYPE_COUNT
} bcmecmp_grp_type_t;

#define BCMECMP_GRP_IS_WEIGHTED(_grp_type) \
            (((_grp_type) == BCMECMP_GRP_TYPE_WEIGHTED) || \
             ((_grp_type) == BCMECMP_GRP_TYPE_WEIGHTED_0) || \
             ((_grp_type) == BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) || \
             ((_grp_type) == BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED))

#define BCMECMP_FLEX_GRP_IS_WEIGHTED(_grp_type) \
            (((_grp_type) == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) || \
             ((_grp_type) == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))

#define BCMECMP_FLEX_GRP_TYPE_IS_FLEX(_grp_type) \
            (((_grp_type) == BCMECMP_GRP_TYPE_LEVEL0) || \
             ((_grp_type) == BCMECMP_GRP_TYPE_LEVEL1) || \
             ((_grp_type) == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) || \
             ((_grp_type) == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED))

/*! \brief ECMP group level names. */
#define BCMECMP_LEVEL_NAME { \
            "SINGLE",        \
            "OVERLAY",       \
            "UNDERLAY",      \
            "WCMP"           \
            "WCMP LEVEL0"           \
            "LEVEL0",      \
            "LEVEL1",      \
            "WCMP LEVEL0"           \
            "WCMP LEVEL1"           \
}

/*!
 * \brief ECMP group defragmentation structure.
 */
typedef struct bcmecmp_grp_defrag_s {

    /*! Defragment operations of bcmecmp_oper_t type. */
    uint32_t oper_flags;

    /*! Group logial table source identifier. */
    bcmltd_sid_t glt_sid;

    /*! ECMP group identifier. */
    bcmecmp_id_t ecmp_id;

    /*! Logical table entry group type - BCMECMP_GRP_TYPE_XXX . */
    bcmecmp_grp_type_t gtype;

    /*! Member table start index of this group. */
    int mstart_idx;

    /*! Number of member table entries used by this group. */
    uint32_t gsize;

    /*! Operational argument. */
    const bcmltd_op_arg_t *op_arg;
} bcmecmp_grp_defrag_t;

/*!
 * \brief ECMP table entries in WAL information structure.
 */
typedef struct bcmecmp_wal_info_s {

    /*! INITIAL_L3_ECMP member table entry size in words. */
    uint32_t imwsize;

    /*! Total INITIAL_L3_ECMP member table entries in WAL for READ. */
    uint32_t imr_ecount;

    /*! Total INITIAL_L3_ECMP member table entries in WAL for WRITE. */
    uint32_t imw_ecount;

    /*! Total INITIAL_L3_ECMP member  table entries in WAL for WRITE (CLR). */
    uint32_t imclr_ecount;

    /*! L3_ECMP member table entry size in words. */
    uint32_t mwsize;

    /*! Total L3_ECMP member table entries in WAL for READ. */
    uint32_t mr_ecount;

    /*! Total L3_ECMP member table entries in WAL for WRITE. */
    uint32_t mw_ecount;

    /*! Total L3_ECMP member  table entries in WAL for WRITE (CLR). */
    uint32_t mclr_ecount;

    /*! INITIAL_L3_ECMP_GROUP member table entry size in words. */
    uint32_t igwsize;

    /*! Total INITIAL_L3_ECMP_GROUP table entries in WAL for READ. */
    uint32_t igr_ecount;

    /*! Total INITIAL_L3_ECMP_GROUP table entries in WAL for WRITE. */
    uint32_t igw_ecount;

    /*! L3_ECMP_COUNT member table entry size in words. */
    uint32_t gwsize;

    /*! Total L3_ECMP_COUNT table entries in WAL for READ. */
    uint32_t gr_ecount;

    /*! Total L3_ECMP_COUNT table entries in WAL for WRITE. */
    uint32_t gw_ecount;
} bcmecmp_wal_info_t;

/*!
 * \brief ECMP group RH size.
 */
typedef enum bcmecmp_rh_size_e {
    BCMECMP_RH_SIZE_64 = 6,
    BCMECMP_RH_SIZE_128,
    BCMECMP_RH_SIZE_256,
    BCMECMP_RH_SIZE_512,
    BCMECMP_RH_SIZE_1K,
    BCMECMP_RH_SIZE_2K,
    BCMECMP_RH_SIZE_4K,
    BCMECMP_RH_SIZE_8K,
    BCMECMP_RH_SIZE_16K,
    BCMECMP_RH_SIZE_32K,
    BCMECMP_RH_SIZE_COUNT
} bcmecmp_rh_size_t;
#endif /* BCMECMP_TYPES_H */
