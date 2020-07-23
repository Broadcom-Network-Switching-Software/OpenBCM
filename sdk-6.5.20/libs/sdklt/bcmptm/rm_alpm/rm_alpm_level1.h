/*! \file rm_alpm_level1.h
 *
 * Main routines for RM ALPM Level-1
 *
 * This file contains routines manage ALPM Level-1 (TCAM) resouces,
 * which are internal to ALPM resource manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_LEVEL1_H
#define RM_ALPM_LEVEL1_H

/*******************************************************************************
   Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>

#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */

#define L1_DB0 0
#define L1_DB1 1
#define L1_DB2 2
#define L1_DB3 3
#define L1_DBS 4

#define MAX_L1_BLOCKS           10


#define HALF_FLAG               (1 << 30)
#define RIGHT_FLAG              (1 << 29)
#define HALF_INDEX_L(i)         (HALF_FLAG | (i))
#define HALF_INDEX_R(i)         (HALF_FLAG | RIGHT_FLAG | (i))
#define FULL_INDEX(i)           (i)
#define IS_FULL_INDEX(i)        ((i != INVALID_INDEX) && !(i & HALF_FLAG))
#define IS_HALF_INDEX(i)        ((i != INVALID_INDEX) && (i & HALF_FLAG))
#define IS_HALF_R_INDEX(i)      (IS_HALF_INDEX(i) && (i & RIGHT_FLAG))
#define IS_HALF_L_INDEX(i)      (IS_HALF_INDEX(i) && !(i & RIGHT_FLAG))
#define HW_MASK                 ~(HALF_FLAG | RIGHT_FLAG)
#define HW_INDEX(i)             (i & HW_MASK)
/*******************************************************************************
  Typedefs
 */
/*!
 * \brief Level-1 statistics
 */
typedef struct block_stats_s {
    int used_count[KEY_TYPES][VRF_TYPES];  /*!< Used count */
    int max_count[KEY_TYPES];   /*!< Max count */
    int half_count[KEY_TYPES][VRF_TYPES];  /*!< Half count */
} block_stats_t;


/*!
 * \brief Level-1 prefix state for index management
 */
typedef struct pair_state_s {
    int start;   /*!< Start index for this prefix length */
    int end;     /*!< End index for this prefix length */
    int prev;    /*!< Previous (Lo to Hi) prefix length with entries */
    int next;    /*!< Next (Hi to Lo) prefix length with entries */
    int vent;    /*!< Valid entries, IPv6 (dw based), IPv4 (sw based)*/
    int fent;    /*!< Free entries, IPv6 & IPv4 (both sw based) */
    int hent;    /*!< Half entries (0,1) */
} block_state_t;

typedef struct l1_key_input_info_s {
    /*!< PAIR TCAM range1 first index */
    int range1_first;

    /*!< PAIR TCAM range2 first index */
    int range2_first;

    /*!< PAIR TCAM range1 last index */
    int range1_last;

    /*!< PAIR TCAM range1 last prefix */
    int range1_last_pfx;

    /*!< PAIR TCAM range1 last prefix (valid entries) span onto Range2. */
    int last_pfx_span;

    /*!< PAIR TCAM max prefix */
    int max_pfx[LAYOUT_NUM];

    /*!< PAIR TCAM base prefix */
    int base_pfx[LAYOUT_NUM];

    /*!< Prefix state array  */
    block_state_t  *block_state;

    /*!< Statistics */
    block_stats_t  block_stats;

    /*!< End index in tcam block, single-view based */
    int block_end;

    /*!< PAIR TCAM size, single-view based */
    int block_size;

    /*!< Start index in pair tcam, single-view based */
    int block_start;

    /*!< Number of PAIR blocks, double blocks based */
    int blocks;

    int valid;
} l1_key_input_info_t;

typedef struct l1_db_info_s {

    l1_key_input_info_t ki[PKM_NUM];

    int valid;
} l1_db_info_t;

/*!
 * \brief Global ALPM Level-1 info
 *
 * All Level-1 related info are described in this structure
 */
typedef struct alpm_l1_info_s {
    l1_db_info_t l1_db[L1_DBS];

    /*!< Pivot info array (half-wide pt idx as subscript ) */
    alpm_pivot_t  **pivot_info;

    /*!< TCAM depth */
    int tcam_depth;

    /*!< TCAM blocks */
    int tcam_blocks;

    /*!< L1 blocks */
    int l1_blocks;

    /*!< L1 block start */
    int l1_block_start;

    /*!< L1 block end */
    int l1_block_end;

    /*!< TCAM only SID */
    bcmdrd_sid_t tcam_only_sid;

    /*!< DATA only SID */
    bcmdrd_sid_t data_only_sid;

    /*!< TCAM only SID */
    bcmdrd_sid_t wide_tcam_only_sid;

    /*!< DATA only SID */
    bcmdrd_sid_t wide_data_only_sid;

    /*!< Single-wide SID */
    bcmdrd_sid_t unpair_sid;

    /*!< Double-wide SID */
    bcmdrd_sid_t pair_sid;

    /*!< Single-wide SID */
    bcmdrd_sid_t wide_unpair_sid;

    /*!< Double-wide SID */
    bcmdrd_sid_t wide_pair_sid;

    /*!< Level1 hit SID */
    bcmdrd_sid_t l1_hit_sid;

    /*!< Dual mode (dual databases) */
    int dual_mode;

    /*!< Template wide entry */
    void *w_template[VRF_TYPES];

    /*!< Template narrow entry */
    void *n_template[VRF_TYPES];

    /*!< Template wide entry size */
    uint8_t w_size;

    /*!< Template narrow entry size */
    uint8_t n_size;

    /*!< Ser slice mode enabled? */
    uint8_t slice_mode_en;

    /*!< Key data separated */
    uint8_t key_data_separate[2];

    /*!< L1 resources are shared between RM_TCAM and RM_ALPM */
    uint8_t l1_tcams_shared;

    /*!< If set, data only index is always x2 TCAM_ONLY index */
    uint8_t data_only_x2;

    int no_pkm_s;
} alpm_l1_info_t;


/*******************************************************************************
   Function prototypes
 */
extern void
bcmptm_rm_alpm_l1_pfx_decode(int u, int m, int pfx,
                             int *ipv,
                             alpm_vrf_type_t *vt,
                             int *len,
                             uint8_t *pfx_type,
                             uint8_t *app);

extern int
bcmptm_rm_alpm_l1_pfx_encode(int u, int m, uint8_t app, int ipv,
                             alpm_vrf_type_t vt,
                             int len);


/*!
 * \brief Sync up level-1 ha memory.
 *
 * \param [in] u Device unit.
 *
 * \return nothing.
 */
void
bcmptm_rm_alpm_l1_ha_sync(int u, int m);

/*!
 * \brief Insert level-1 entry
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_insert(int u, int m, alpm_arg_t *arg);


/*!
 * \brief Delete level-1 entry
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_delete(int u, int m, alpm_arg_t *arg);

/*!
 * \brief Read and decode level-1 entry
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_read(int u, int m, alpm_arg_t *arg);

/*!
 * \brief Lookup (Exact match) and decode level-1 entry
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_match(int u, int m, alpm_arg_t *arg);

/*!
 * \brief Lookup (LPM match) and decode level-1 entry
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_find(int u, int m, alpm_arg_t *arg);

/*!
 * \brief Calculate free count for given key type & vrf type
 *
 * Can be used as a reference for insert decision
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg.
 *
 * \return Free count.
 */
extern int
bcmptm_rm_alpm_l1_can_insert(int u, int m, alpm_arg_t *arg);

/*!
 * \brief Cleanup ALPM level-1 module.
 *
 * \param [in] u Device u.
 * \param [in] warm Warm cleanup
 *
 * \return SHR_E_XXX.
 */
extern void
bcmptm_rm_alpm_l1_cleanup(int u, int m, bool graceful);

/*!
 * \brief Initiliaze ALPM level-1 module.
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 * \param [in] boot_type Boot type.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_l1_init(int u, int m, bool recover, uint8_t boot_type);


/*!
 * \brief Get pointer of ALPM level-1 info
 *
 * \return ALPM Level-1 info ptr
 */
extern alpm_l1_info_t *
bcmptm_rm_alpm_l1_info_get(int u, int m);


/*!
 * \brief Decode Level-1 entry (both key and data parts).
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 * \param [in] l1v Level-1 view type
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_arg_fill(int u, int m, alpm_arg_t *arg, void *de, void *se, uint8_t l1v);

/*!
 * \brief Get level-1 sid & hw index
 *
 * \param [in] u Device u.
 * \param [in] index Entry index
 * \param [in] kt Key type.
 * \param [out] sid Symbol ID
 * \param [out] hw_index Hardware index
 * \param [out] sid_data Symbol ID
 * \param [out] hw_index_data Hardware index
 * \param [out] sid_cnt Symbol ID count.
 * \param [out] sid_cnt_dta Symbol ID count.
 *
 * \return Level1 symbol id
 */
extern int
bcmptm_rm_alpm_l1_sid_hidx_get(int u, int m, ln_index_t index, alpm_key_type_t kt,
                               bcmdrd_sid_t *sid, uint32_t *hw_index,
                               bcmdrd_sid_t *sid_data, uint32_t *hw_index_data,
                               uint8_t *sid_cnt,
                               uint8_t *sid_cnt_data
                               );


extern int
bcmptm_rm_alpm_l1_max_count(int u, int m, uint8_t ldb, alpm_key_type_t kt,
                            alpm_vrf_type_t vt);

extern alpm_key_type_t
bcmptm_rm_alpm_key_type_get(int u, int m, int ipv6, int len, uint8_t app, alpm_vrf_type_t vt);

extern int
bcmptm_rm_alpm_l1_recover(int u, int m, bool full);
extern bool
bcmptm_rm_alpm_pkm_is_pair(int u, int m, int pkm);

extern int
bcmptm_rm_alpm_l1_dump(int u, int m, alpm_arg_t *arg, int idx, int lvl_cnt);

extern void
bcmptm_rm_alpm_l1_usage_get(int u, int m,
                            bcmptm_cth_alpm_usage_info_t *info);
#endif /* RM_ALPM_LEVEL1_H */

