/*! \file bcmcth_trunk_vp_types.h
 *
 * This file contains TRUNK_VP custom table handler data type definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_VP_TYPES_H
#define BCMCTH_TRUNK_VP_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_int_comp.h>

/*! Maximum number of BCM Units supported. */
#define BCMCTH_TRUNK_VP_NUM_UNITS_MAX BCMDRD_CONFIG_MAX_UNITS

/*! Invalid Index or ID. */
#define BCMCTH_TRUNK_VP_INVALID (-1)

/*! Install hardware entry in all pipe instances. */
#define BCMCTH_TRUNK_VP_ALL_PIPES_INST (-1)

/*! Logical table entry staged status indicator. */
#define BCMCTH_TRUNK_VP_ENTRY_STAGED    (1 << 0)

/*! Logical table entry defragment staged status indicator. */
#define BCMCTH_TRUNK_VP_ENTRY_DEFRAG_STAGED (1 << 2)

/*! Perform pre-configuration recovery operation post warm start. */
#define BCMCTH_TRUNK_VP_ENTRY_PRECONFIG (1 << 1)

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
#define BCMCTH_TRUNK_VP_ALLOC(_ptr, _size, _descr)      \
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
#define BCMCTH_TRUNK_VP_FREE(_ptr) SHR_FREE(_ptr)

/*! HA sub-component ID for staged TRUNK_VP groups configuration block. */
#define BCMCTH_TRUNK_VP_GRP_SUB_COMP_ID          (8)

/*! HA sub-component ID for committed TRUNK_VP groups configuration block. */
#define BCMCTH_TRUNK_VP_GRP_BK_SUB_COMP_ID       (9)

/*! HA sub-component ID for staged TRUNK_VP group table entries block. */
#define BCMCTH_TRUNK_VP_GRP_TBL_SUB_COMP_ID      (10)

/*! HA sub-component ID for committed TRUNK_VP group table entries block. */
#define BCMCTH_TRUNK_VP_GRP_TBL_BK_SUB_COMP_ID   (11)

/*! HA sub-component ID for staged TRUNK_VP member table entries block. */
#define BCMCTH_TRUNK_VP_MEMB_TBL_SUB_COMP_ID     (12)

/*! HA sub-component ID for committed TRUNK_VP member table entries block. */
#define BCMCTH_TRUNK_VP_MEMB_TBL_BK_SUB_COMP_ID  (13)

/*! TRUNK_VP Group Identifier.  */
typedef int bcmcth_trunk_vp_id_t;

/*! Logical Table Definition Field Identifier. */
typedef int bcmcth_trunk_vp_ltd_fid_t;

/*!
 * \brief Device TRUNK_VP modes.
 *
 * List of TRUNK_VP modes supported by the device.
 */
typedef enum bcmcth_trunk_vp_mode_e {
    /*! Full feature single level TRUNK_VP resolution. */
    BCMCTH_TRUNK_VP_MODE_FULL = 0,

    /*! Must be the last one. */
    BCMCTH_TRUNK_VP_MODE_COUNT
} bcmcth_trunk_vp_mode_t;

/*! \brief Device TRUNK_VP mode names. */
#define BCMCTH_TRUNK_VP_MODE_NAME { \
            "FULL",         \
            "HIERARCHICAL", \
            "FAST"          \
}

/*! \brief TRUNK_VP resilient hashing group members operation name. */
#define BCMCTH_TRUNK_VP_RH_OPER_NAME { \
            "NONE",            \
            "ADD",             \
            "DELETE",          \
            "REPLACE",         \
            "ADD_REPLACE",     \
            "DELETE_REPLACE"   \
}

/*!
 * \brief TRUNK_VP Load Balance Modes.
 */
typedef enum bcmcth_trunk_vp_lb_mode_e {
    /*! Regular Load Balance Mode. */
    BCMCTH_TRUNK_VP_LB_MODE_REGULAR = 0,

    /*! Random Load Balance Mode. */
    BCMCTH_TRUNK_VP_LB_MODE_RANDOM,

    /*! Must be the last one. */
    BCMCTH_TRUNK_VP_LB_MODE_COUNT
} bcmcth_trunk_vp_lb_mode_t;

/*! \brief TRUNK_VP load balancing mode names. */
#define BCMCTH_TRUNK_VP_LB_MODE_NAME {  \
            "REG_HASH",          \
            "RANDOM",           \
}

/*!
 * \brief weighted TRUNK_VP group size.
 */
typedef enum bcmcth_trunk_vp_weighted_size_e {
    BCMCTH_TRUNK_VP_WEIGHTED_SIZE_256 = 2,
    BCMCTH_TRUNK_VP_WEIGHTED_SIZE_512,
    BCMCTH_TRUNK_VP_WEIGHTED_SIZE_1K,
    BCMCTH_TRUNK_VP_WEIGHTED_SIZE_2K,
    BCMCTH_TRUNK_VP_WEIGHTED_SIZE_4K,
    BCMCTH_TRUNK_VP_WEIGHTED_SIZE_8K,
    BCMCTH_TRUNK_VP_WEIGHTED_SIZE_16K,
    BCMCTH_TRUNK_VP_WEIGHTED_SIZE_COUNT
} bcmcth_trunk_vp_weighted_size_t;

/*! \brief weighted TRUNK_VP group size names. */
#define BCMCTH_TRUNK_VP_WEIGHTED_SIZE_NAME {  \
            "WEIGHTED_SIZE_256",      \
            "WEIGHTED_SIZE_512",      \
            "WEIGHTED_SIZE_1K",       \
            "WEIGHTED_SIZE_2K",       \
            "WEIGHTED_SIZE_4K",       \
            "WEIGHTED_SIZE_8K",       \
            "WEIGHTED_SIZE_16K"       \
}

/*!
 * \brief TRUNK_VP group lookup types.
 */

typedef enum bcmcth_trunk_vp_grp_type_e {
    BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY,
    BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0,
    BCMCTH_TRUNK_VP_GRP_TYPE_COUNT
} bcmcth_trunk_vp_grp_type_t;

/*!
 * \brief Verify if group type is weighted.
 *
 * Given a group type check if the group is weighted group.
 *
 * \param [in] _grp_type Group type.
 *
 * \returns True if group is weighted else false.
 */
/*! Check if the group is weighted. */
#define BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(_grp_type) \
            ((_grp_type) == BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)

/*! \brief TRUNK_VP group level names. */
#define BCMCTH_TRUNK_VP_LEVEL_NAME { \
            "OVERLAY",       \
            "WCMP LEVEL0"           \
}

/*!
 * \brief TRUNK_VP group defragmentation structure.
 */
typedef struct bcmcth_trunk_vp_grp_defrag_s {

    /*! Defragment operations of bcmcth_trunk_vp_oper_t type. */
    uint32_t oper_flags;

    /*! Group logial table source identifier. */
    bcmltd_sid_t glt_sid;

    /*! TRUNK_VP group identifier. */
    bcmcth_trunk_vp_id_t trunk_vp_id;

    /*! Logical table entry group type - BCMCTH_TRUNK_VP_GRP_TYPE_XXX . */
    bcmcth_trunk_vp_grp_type_t gtype;

    /*! Member table start index of this group. */
    int mstart_idx;

    /*! Number of member table entries used by this group. */
    uint32_t gsize;

    /*! Operational argument. */
    const bcmltd_op_arg_t *op_arg;
} bcmcth_trunk_vp_grp_defrag_t;

/*!
 * \brief TRUNK_VP table entries in WAL information structure.
 */
typedef struct bcmcth_trunk_vp_wal_info_s {

    /*! INITIAL_L3_TRUNK_VP member table entry size in words. */
    uint32_t imwsize;

    /*! Total INITIAL_L3_TRUNK_VP member table entries in WAL for READ. */
    uint32_t imr_ecount;

    /*! Total INITIAL_L3_TRUNK_VP member table entries in WAL for WRITE. */
    uint32_t imw_ecount;

    /*!
     * Total INITIAL_L3_TRUNK_VP member
     * table entries in WAL for WRITE (CLR).
     */
    uint32_t imclr_ecount;

    /*! L3_TRUNK_VP member table entry size in words. */
    uint32_t mwsize;

    /*! Total L3_TRUNK_VP member table entries in WAL for READ. */
    uint32_t mr_ecount;

    /*! Total L3_TRUNK_VP member table entries in WAL for WRITE. */
    uint32_t mw_ecount;

    /*! Total L3_TRUNK_VP member  table entries in WAL for WRITE (CLR). */
    uint32_t mclr_ecount;

    /*! INITIAL_L3_TRUNK_VP_GROUP member table entry size in words. */
    uint32_t igwsize;

    /*! Total INITIAL_L3_TRUNK_VP_GROUP table entries in WAL for READ. */
    uint32_t igr_ecount;

    /*! Total INITIAL_L3_TRUNK_VP_GROUP table entries in WAL for WRITE. */
    uint32_t igw_ecount;

    /*! L3_TRUNK_VP_COUNT member table entry size in words. */
    uint32_t gwsize;

    /*! Total L3_TRUNK_VP_COUNT table entries in WAL for READ. */
    uint32_t gr_ecount;

    /*! Total L3_TRUNK_VP_COUNT table entries in WAL for WRITE. */
    uint32_t gw_ecount;
} bcmcth_trunk_vp_wal_info_t;
#endif /* BCMCTH_TRUNK_VP_TYPES_H */
