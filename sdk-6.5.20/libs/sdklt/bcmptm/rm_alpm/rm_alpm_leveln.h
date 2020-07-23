/*! \file rm_alpm_leveln.h
 *
 * Main routines for RM ALPM Level-N
 *
 * This file contains routines manage ALPM Level-N (SRAM) resouces,
 * which are internal to ALPM resource manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_LEVELN_H
#define RM_ALPM_LEVELN_H


/*******************************************************************************
   Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/generated/bcmptm_rm_alpm_ha.h>

#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */

#define ALPM_MAX_BANKS      8

/* LN INDEX format: [RBKT | SUB_BKT | ENT_OFS] */
#define ENT_OFS_BITS    5
#define ENT_OFS_MASK    0x1F
#define SUB_BKT_BITS    2
#define SUB_BKT_MASK    0x3
#define SUB_BKT_SHIFT   ENT_OFS_BITS
#define RBKT_SHIFT      7
#define ENT_OFS_FROM_IDX(index)   ((index) & ENT_OFS_MASK)
#define SUB_BKT_FROM_IDX(index)   (((index) >> SUB_BKT_SHIFT) & SUB_BKT_MASK)
#define RBKT_FROM_INDEX(index)    ((index) >> RBKT_SHIFT)
#define ALPM_INDEX_ENCODE_VER1(_rbkt, _ofs)   \
    (((_rbkt) << RBKT_SHIFT) | (_ofs))
#define ALPM_INDEX_ENCODE_VER0(_rbkt, _sub, _ofs)   \
    (((_rbkt) << RBKT_SHIFT) | ((_sub) << SUB_BKT_SHIFT) | (_ofs))

#define LN_HIT_INDEX(hw_idx, ent_ofs ) ((((hw_idx) << 2) + ((hw_idx) << 4)) + (ent_ofs))
#define MAX_LN_HIT_BITS  20

/*******************************************************************************
  Typedefs
 */

/*!
 * \brief ALPM Level Handle
 *
 * ALPM level handle contains the info describing Level-N
 */
typedef struct lvl_hdl_s {
    /*!< Level index */
    uint8_t lvl_idx;

    /*!< Level bucket handle */
    void *bkt_hdl; /* bkt_hdl_t */
} lvl_hdl_t;

/*!
 * \brief ALPM Level N info
 *
 * ALPM level-n info contains the info describing level-n sub-module
 */
typedef struct alpm_ln_info_s {
    /*!< Level handles */
    lvl_hdl_t *lvl_hdl[DBS][LEVELS];

    /*!< SID for LN level */
    bcmdrd_sid_t ln_sid[LEVELS];

    /*!< SID for LN level hit table */
    bcmdrd_sid_t ln_hit_sid[LEVELS];

    /*!< ALPM bucket mgmt version */
    uint8_t alpm_ver;
} alpm_ln_info_t;
/*******************************************************************************
   Function prototypes
 */
/*!
 * \brief Get alpm level-n info
 *
 * \param [in] u Device u.
 * \param [out] linfo Level-n info
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND.
 */
extern int
bcmptm_rm_alpm_ln_info_get(int u, int m, alpm_ln_info_t *linfo);

/*!
 * \brief Get alpm level-n sid
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 *
 * \return Symbol id
 */
extern bcmdrd_sid_t
bcmptm_rm_alpm_ln_sid_get(int u, int m, int8_t ln);

/*!
 * \brief Get alpm level handle for level-n
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] lvl_hdl Level handle
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND.
 */
extern int
bcmptm_rm_alpm_lvl_hdl_get(int u, int m, int db, int8_t ln,
                              lvl_hdl_t **lvl_hdl);


/*!
 * \brief ALPM find operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_ln_find(int u, int m, alpm_arg_t *arg);

/*!
 * \brief ALPM match operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_ln_match(int u, int m, alpm_arg_t *arg);

/*!
 * \brief ALPM insert operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_ln_insert(int u, int m, alpm_arg_t *arg);


/*!
 * \brief ALPM delete operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_ln_delete(int u, int m, alpm_arg_t *arg);

/*!
 * \brief Cleanup ALPM level-n module.
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_ln_cleanup(int u, int m, bool graceful);

/*!
 * \brief Initiliaze ALPM level-n module.
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 * \param [in] boot_type Boot type.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_init(int u, int m, bool recover, uint8_t boot_type);

/*!
 * \brief And a new pivot which just split from a parent pivot.
 *
 * Basically, this is to add a new elem for a level.
 * The elem should not exist.
 * This will also set bpm ad to split_arg.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] split_arg Split arg
 * \param [out] split_pivot Split pivot
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_split_pivot_add(int u, int m, int db, int8_t ln,
                               alpm_arg_t *split_arg,
                               alpm_pivot_t **split_pivot,
                               bkt_split_aux_t *split_aux);

/*!
 * \brief Read dip/sip entry from cmdproc for a given index.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 * \param [out] de dip entry
 * \param [out] dt_bitmap Data type bitmap.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_read_ln_raw(int u, int m, int db, int8_t ln, int hw_idx, uint32_t *de,
                           uint32_t *dt_bitmap);

/*!
 * \brief Read hit entry for given index.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 * \param [out] e Hit entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_read_ln_hit(int u, int m, int8_t ln, int hw_idx, uint32_t *e);

/*!
 * \brief Write hit entry for a given index.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 * \param [in] de dip entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_write_ln_hit(int u, int m, int8_t ln, int hw_idx, uint32_t *e);

/*!
 * \brief Write dip/sip entry to cmdproc for a given index.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 * \param [in] de dip entry
 * \param [in] hit_op hit operation
 * \param [in] modify_bitmap Modify bitmap
 * \param [in] dt_bitmap Data type bitmap
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_write_ln_raw(int u, int m, int db, int8_t ln, int hw_idx, uint32_t *de,
                            uint8_t hit_op, uint32_t modify_bitmap,
                            uint32_t dt_bitmap);

/*!
 * \brief Update elem ad or alpm_data for a given level.
 *
 * The elem should exist, and the key part should be same as before.
 * Everything that needs updated are in the elem, need encode to arg first.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pivot Elem(pivot)
 * \param [in] arg ALPM arg for previous level, honour this if set.
 * \param [in] update_type Update type.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_pivot_update(int u, int m, int db, int8_t ln,
                            alpm_pivot_t *pivot,
                            alpm_arg_t *arg,
                            uint8_t update_type);
/*!
 * \brief Wrapper for adding entry on any level but last level
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for new entry
 * \param [in] split_aux Auxilliary data for split
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_pivot_insert(int u, int m, int8_t ln, alpm_arg_t *arg,
                            bkt_split_aux_t *split_aux);

/*!
 * \brief Wrapper for deleting entry on any level but last level.
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for deleting entry
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_pivot_delete(int u, int m, int8_t ln, alpm_arg_t *arg);

/*!
 * \brief Initialize alpm arg for split pivot.
 *
 * \param [in] db Database
 * \param [in] ln Previous level
 * \param [in] index Index of parent_pivot
 * \param [in] key Key of parent_pivot
 * \param [in] pivot_key Split pivot key (trie format).
 * \param [in] pivot_len Split pivot length.
 * \param [in] log_bkt Logical bucket.
 * \param [in] arg ALPM arg.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_split_arg_init(int u, int m, int8_t db, int8_t ln,
                              ln_index_t index,
                              elem_key_t *key,
                              trie_ip_t *pivot_key,
                              uint32_t pivot_len,
                              log_bkt_t log_bkt,
                              alpm_arg_t *arg);

/*!
 * \brief Initialize alpm arg for normal pivot.
 *
 * \param [in] db Database
 * \param [in] ln Previous level
 * \param [in] pivot Elem(pivot)
 * \param [in] update_type Update type.
 * \param [in] arg ALPM arg.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_pivot_arg_init(int u, int m, int8_t db, int8_t ln,
                              alpm_pivot_t *pivot,
                              uint8_t update_type,
                              alpm_arg_t *arg);
extern int
bcmptm_rm_alpm_bucket_clean(int u, int m, int db, int8_t ln, alpm_pivot_t **pivot);

extern int
bcmptm_rm_alpm_ln_pivot_recover(int u, int m, int8_t ln, alpm_arg_t *arg);
extern int
bcmptm_rm_alpm_ln_recover(int u, int m, int8_t ln, alpm_arg_t *arg);
extern int
bcmptm_rm_alpm_ln_recover_done(int u, int m, bool full);

extern bcmptm_rm_alpm_ln_stat_t *
bcmptm_rm_alpm_ln_stats_get(int u, int m, int8_t db);

extern void
bcmptm_rm_alpm_ln_usage_get(int u, int m, int ln,
                            bcmptm_cth_alpm_usage_info_t *info);
#endif /* RM_ALPM_LEVELN_H */
