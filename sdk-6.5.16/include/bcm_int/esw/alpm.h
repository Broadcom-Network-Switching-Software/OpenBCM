/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        alpm.h
 * Purpose:     Function declarations for ALPM Internal functions.
 */

#ifndef _BCM_INT_ALPM_H_
#define _BCM_INT_ALPM_H_

#include <soc/defs.h>

#ifdef ALPM_ENABLE

/* ALPM */

#include <bcm_int/esw/alpm_lib_trie.h>

#define ALPM_DEBUG          0

/* #define PSEUDO_ALPM_FMT13 1 */
#if 1
#define ALPM_FMT_TP_CNT     13
#endif

/* max bank per bucket */
#if defined(BCM_TRIDENT3_SUPPORT)
#define ALPM_BPB_MAX        16
#else
#define ALPM_BPB_MAX        8
#endif
#define ALPM_CB_CNT         2
#define ALPM_ENT_WORDS_MAX  15  /* 480 bits */

/* Maximum TCAM/ALPM memory entry size in words (640 bits) */
#define ALPM_MEM_ENT_MAX    20

/* Max entry per bank */
#define ALPM_EPB_MAX        15
/* max prefix per bucket */
#define ALPM_PPB_MAX        (ALPM_EPB_MAX * ALPM_BPB_MAX)

#define ALPM_PPG_INSERT     0
#define ALPM_PPG_DELETE     1
#define ALPM_PPG_HBP        3

#define ALPM_MERGE_THRESHOLD_MAX      10

/* ALPM Packing Mode definitions */
#define ALPM_PKM_32B        (0)
#define ALPM_PKM_64B        (1)
#define ALPM_PKM_128        (2)
#define ALPM_PKM_CNT        (3)
/* IP database type V4 V6 */
#define ALPM_IPT_V4         (ALPM_PKM_32B)
#define ALPM_IPT_V6         (ALPM_PKM_64B)
#define ALPM_IPT_CNT        (2)

#define ALPM_PKM2IPT(pkm)   (!!(pkm))

#define ALPM_BKT_PRT_PID    0
#define ALPM_BKT_GLB_PID    1
#define ALPM_BKT_PID_CNT    2

#define ALPM_TRACE_OP_ADD      0
#define ALPM_TRACE_OP_DELETE   1
#define ALPM_TRACE_OP_DELETE_ALL    2

/* ref: esw/dump.c
 #define DUMP_TABLE_RAW       0x01
 #define DUMP_TABLE_HEX       0x02
 #define DUMP_TABLE_ALL       0x04
 #define DUMP_TABLE_CHANGED   0x08
 #define DUMP_TABLE_EXT       0x10
 #define DUMP_TABLE_PIPE_X    0x20
 #define DUMP_TABLE_PIPE_Y    0x40
 #define DUMP_TABLE_PIPE_0    0x80
 #define DUMP_TABLE_PIPE_1    0x100
 #define DUMP_TABLE_PIPE_2    0x200
 #define DUMP_TABLE_PIPE_3    0x400
 #define DUMP_TABLE_NO_CACHE  0x800
 #define DUMP_TABLE_PHY_INDEX 0x1000
 #define DUMP_TABLE_SIMPLIFIED 0x2000
 #define DUMP_TABLE_FORMAT 0x4000
 #define DUMP_TABLE_ALL_DMA_THRESHOLD 8
 #define DUMP_TABLE_ITER_BEFORE_YIELD 8
 */
#define ALPM_DUMP_TABLE_RAW          0x01
#define ALPM_DUMP_TABLE_HEX          0x02
#define ALPM_DUMP_TABLE_ALL          0x04
#define ALPM_DUMP_TABLE_CHANGED      0x08
#define ALPM_DUMP_TABLE_NOCACHE      0x800

/* ALPM Distributed Hitbits */
#define ALPM_HIT_INTERVAL_DEF 1000000   /* 1s */
#define ALPM_HIT_INTERVAL_MIN 100000    /* 100ms  */
#define ALPM_HIT_INTERVAL_MAX 60000000  /* 1 min  */
#define ALPM_HIT_PRIORITY_DEF 200       /* Dist hitbit thread task priority */
#define ALPM_MAX_HIT_ENT_MASK 0xffff    /* max 16 bits of HIT table entry */

/* ALPM mode */
#define BCM_ALPM_MODE_INVALID       -1
#define BCM_ALPM_MODE_COMBINED      0
#define BCM_ALPM_MODE_PARALLEL      1
#define BCM_ALPM_MODE_TCAM_ALPM     2

/* SOC_ALPM_LOOKUP_HIT */
#define BCM_ALPM_LOOKUP_HIT             (1 << 31)
/* SOC_ALPM_COOKIE_FLEX */
#define ALPM_ROUTE_FULL_MD              (1 << 29)
/* SOC_ALPM_LPM_LOOKUP_HIT */
#define BCM_ALPM_LPM_LOOKUP_HIT         (1 << 28)
/* SOC_ALPM_DEBUG_SHOW_FLAG_BKT */
/* for ALPM debug show */
#define BCM_ALPM_DEBUG_SHOW_FLAG_PVT    (1U << 0)
#define BCM_ALPM_DEBUG_SHOW_FLAG_BKT    (1U << 1)
#define BCM_ALPM_DEBUG_SHOW_FLAG_BKTUSG (1U << 2)
#define BCM_ALPM_DEBUG_SHOW_FLAG_CNT    (1U << 3)
#define BCM_ALPM_DEBUG_SHOW_FLAG_INTDBG (1U << 4)
#define BCM_ALPM_DEBUG_SHOW_FLAG_ALL    (~0U)

/* Hit table control for bucket levels
   (TCAM level hitbit access uses direct soc_mem cache/hw interface) */
typedef struct _alpm_hit_tbl_ctrl_s {
    soc_mem_t mem;          /* Hit table mem */
    int     index_cnt;      /* Hit table index count */
    int     size;           /* Hit Table size (in bytes) */
    int     ent_moved;      /* Entry moved (TRUE or FALSE) */
    int     ent_moved_cnt;  /* Entry moved counter */

    void    *cache;         /* Hit cache table */
    void    *move;          /* Hit move table */
    void    *move_mask;     /* Hit move mask table */
} _alpm_hit_tbl_ctrl_t;

/* ALPM distributed hitbit control structure */
typedef struct _alpm_dist_hitbit_s {
    /* Hit tbl ctrl for Level2 & Level3 per PID */
    _alpm_hit_tbl_ctrl_t *hit_tbl[ALPM_CB_CNT][ALPM_BKT_PID_CNT];
    char              taskname[16];       /* Dist hitbit thread name */
    VOL sal_thread_t  thread;             /* Dist hitbit thread id */
    VOL int           interval;           /* Time between dist hitbit thread (us) */
    int               priority;           /* task priority */
    sal_sem_t         sema;               /* Dist hitbit semaphore */
} _alpm_dist_hitbit_t;

extern _alpm_dist_hitbit_t *alpm_dist_hitbit[];

/* threshold in number of HITBIT entries moved (>= threshold use DMA_write) */
#define ALPM_HIT_DMA_THRESHOLD          500

#define ALPMDH(u)                       (alpm_dist_hitbit[u])
#define ALPM_HTBL(u, i, p)              (ALPMDH(u)->hit_tbl[i][p])
#define ALPM_HTBL_MEM(u, i, p)          (ALPM_HTBL(u, i, p)->mem)
#define ALPM_HTBL_IDX_CNT(u, i, p)      (ALPM_HTBL(u, i, p)->index_cnt)
#define ALPM_HTBL_SIZE(u, i, p)         (ALPM_HTBL(u, i, p)->size)
#define ALPM_HTBL_CACHE(u, i, p)        (ALPM_HTBL(u, i, p)->cache)
#define ALPM_HTBL_MOVE(u, i, p)         (ALPM_HTBL(u, i, p)->move)
#define ALPM_HTBL_MOVE_MASK(u, i, p)    (ALPM_HTBL(u, i, p)->move_mask)
#define ALPM_HTBL_ENT_MOVED(u, i, p)    (ALPM_HTBL(u, i, p)->ent_moved)
#define ALPM_HTBL_ENT_MOVED_CNT(u, i, p) (ALPM_HTBL(u, i, p)->ent_moved_cnt)

#define ALPM_HIT_LOCK(u)          L3_LOCK(u)
#define ALPM_HIT_UNLOCK(u)        L3_UNLOCK(u)

/* ALPM control block */

/* Propagation data structure */
typedef struct _alpm_ppg_data_s {
    uint32 ipt;
    uint32 key_len;
    uint32 bpm_len;
    uint32 vrf_id;
    uint32 key[5];
    void   *bkt_def;
} _alpm_ppg_data_t;

/* Defines Logical ALPM table ID */
typedef enum _alpm_tbl_e {
    alpmTblInvalid = 0,
    alpmTblPvtCmodeL1,
    alpmTblPvtPmodeL1,
    alpmTblPvtCmodeL1P128,
    alpmTblPvtPmodeL1P128,
    alpmTblBktCmodeL2,
    alpmTblBktPmodeL2,
    alpmTblBktPmodeGblL2,
    alpmTblBktPmodePrtL2,
    alpmTblBktCmodeL3,
    alpmTblBktPmodeGblL3,
    alpmTblBktPmodePrtL3,
    alpmTblCount
} _alpm_tbl_t;

typedef struct _alpm_fmt_info_s {
    int16       fmt_type;   /* ALPM_FMT_PVT/RTE */
    int16       fmt_count;
    int16       fmt_ent_max[ALPM_FMT_TP_CNT];
    int16       fmt_pfx_len[ALPM_FMT_TP_CNT];
    int         fmt_bnk[ALPM_FMT_TP_CNT];
    int         fmt_ent[ALPM_FMT_TP_CNT];
    int         fmt_ftype[ALPM_FMT_TP_CNT];
} _alpm_fmt_info_t;

typedef struct _alpm_bnk_info_s {
    /* Support max to 131072 (4096 * 32) banks */
    SHR_BITDCL      bnk_btmp[4096];
    int             bnk_rsvd;
    int             bnk_used;
} _alpm_bnk_info_t;

typedef struct _alpm_bkt_pool_conf_s {
    _alpm_bnk_info_t *bnk_info[ALPM_IPT_CNT];
    int             bnk_total;
    int             bnk_per_bkt;

    /* Indicating the logical bucket table */
    _alpm_tbl_t     bkt_tbl;
    int             bkt_wra;
    int             bkt_fixed_fmt;
    /* Table entry array to store pivot pointer */
    void            **pvt_ptr;

    /* Bank fmt configuration */
    _alpm_fmt_info_t *fmt_info;
} _alpm_bkt_pool_conf_t;

typedef struct _alpm_bnk_conf_s {
    /* Bank pool config:
     * We have 2 bank pool pointer,
     * They can point to the same logical pool (Combined Mode)
     * or point to the different pools (Parallel & Mixed Mode)
     *
     * Private routes use pool 0.
     * Global routes use pool 1.
     */
    _alpm_bkt_pool_conf_t *bkt_pool[ALPM_BKT_PID_CNT];

    /* Bucket config */
    int16 bkt_bits;
    int16 bkt_dw;
    int16 phy_bnks_per_bkt;
    int16 bnk_bits;
} _alpm_bnk_conf_t;

typedef struct _alpm_bkt_info_s {
    uint8  bnk_fmt[ALPM_BPB_MAX];
    uint32 vet_bmp[ALPM_BPB_MAX];
    uint16 sub_bkt_idx;
    uint16 bkt_idx;
    uint8  rofs; /* ROFS */
    uint8  reverse;
} _alpm_bkt_info_t;

typedef struct _alpm_bkt_adata_s {
    uint32  defip_flags;
    int     defip_ecmp_index;
    int     defip_prio;
    int     defip_lookup_class;
    int     defip_flex_ctr_pool;
    int     defip_flex_ctr_mode;
    int     defip_flex_ctr_base_id;
} _alpm_bkt_adata_t;

/* Used for bucket trie node */
typedef struct _alpm_bkt_node_s {
    alpm_lib_trie_node_t node; /* trie node */
    struct _alpm_bkt_node_s *bkt_ptr;   /* pfx_node -> bkt_node */
    struct _alpm_bkt_adata_s adata;
    uint32 key[5];
    uint32 key_len;
    /* ent_idx format: ent | bnk | bkt @ALPM_IDX_MAKE */
    uint32 ent_idx;             /* HW memory loc */
} _alpm_bkt_node_t;

/* Used for prefix trie node, same as _alpm_bkt_node_t */
typedef struct _alpm_bkt_node_s _alpm_pfx_node_t;

typedef struct _alpm_bkt_pfx_arr_s {
    _alpm_bkt_node_t *pfx[ALPM_PPB_MAX];
    int pfx_cnt;
} _alpm_bkt_pfx_arr_t;

typedef struct _alpm_bnk_idx_arr_s {
    int idx[ALPM_BPB_MAX];
    int idx_cnt;
} _alpm_bnk_idx_arr_t;

typedef struct _alpm_bkt_split_cb_data_s {
    struct _alpm_cb_s *acb;
    int pvt_len;
    int bkt_cur_cnt;
    int bkt_max_cnt;
} _alpm_bkt_split_cb_data_t;

typedef struct _alpm_tcam_write_s {
    int rv;
    int hw_idx; /* -1 means invalid or already updated */
    uint32 ent[SOC_MAX_MEM_FIELD_WORDS];
} _alpm_tcam_write_t;

/* Pivot Structure */
typedef struct _alpm_pvt_node_s {
    alpm_lib_trie_node_t node;           /* trie node */
    alpm_lib_trie_t *bkt_trie;           /* Link to bucket trie -> _alpm_bkt_node_t */

    uint32 vrf_id;
    uint32 bkt_pkm;             /* V4, V6 */
    uint32 key[5];              /* pivot */
    uint32 key_len;             /* pivot length, aka kshift */
    uint32 bpm_len;             /* pivot bpm_len */
    uint32 has_def;             /* has default route */

    uint32 tcam_idx;            /* TCAM index in 1st level */
    uint32 pvt_flags;

    struct _alpm_bkt_node_s *def_pfx; /* Link to bkt_node for bkt def route */
    struct _alpm_bkt_info_s bkt_info;    /* Bucket info */
} _alpm_pvt_node_t;

typedef struct _alpm_pvt_ctrl_s {
    alpm_lib_trie_t *pvt_trie;           /* -> _alpm_pvt_node_t */
    uint32 route_cnt;
    uint32 pvt_len_cnt[128];
    /* spl_len only valid for acb 1 */
    uint32 spl_len_diff1;
    uint32 spl_len_diff2;

    /* Debug counter */
    uint32 cnt_add;
    uint32 cnt_del;

    uint8 dr_inited; /* to indicate direct route inited first */
    uint8 db_inited;
    /* database type: FULL | REDUCED
     * FULL : supports Destination AND
     *        flex counter, pri, rpe, dst_discard, class_id
     * REDUCED : only supports Destination
     */
    uint8 db_type;
} _alpm_pvt_ctrl_t;

typedef struct _alpm_cb_stat_s {
    uint32      c_mem[alpmTblCount][2]; /* RD 0, WR 1 */
    uint32      c_bulk_mem[alpmTblCount][2]; /* RD 0, WR 1 */

    uint32      c_defrag;
    uint32      c_merge;
    uint32      c_expand;
    uint32      c_split;
    uint32      c_ripple;
    uint32      c_bnkshrk;
    uint32      c_bnkfree;
    uint32      c_full;

    /* Split misplaced case counter */
    uint32      c_spl_mp_cs1;
    uint32      c_spl_mp_cs21;
    uint32      c_spl_mp_cs221;
    uint32      c_spl_mp_cs222;
} _alpm_cb_stat_t;

#define ALPM_FMT_PVT        (0)
#define ALPM_FMT_RTE        (1)
#define ALPM_FMT_CNT        (2)

typedef struct _alpm_cb_s {
    int         unit;
    _alpm_pvt_ctrl_t *pvt_ctl[ALPM_IPT_CNT];
    _alpm_tbl_t pvt_tbl[ALPM_PKM_CNT];

    struct _alpm_bnk_conf_s bnk_conf;
    uint32      acb_idx;    /* Current Control Block index */

    struct _alpm_cb_stat_s acb_cnt;
} _alpm_cb_t;

typedef struct _alpm_pfx_ctrl_s {
    alpm_lib_trie_t *pfx_trie[ALPM_IPT_CNT];
} _alpm_pfx_ctrl_t;

typedef struct _alpm_ctrl_s {
    _alpm_pfx_ctrl_t *_vrf_pfx_hdl;
    _alpm_cb_t *alpm_cb[ALPM_CB_CNT];   /* ALPM Control Block */
#define _ALPM_MERGE_CHANGE      0
#define _ALPM_MERGE_UNCHANGE    1
    /* UNCHANGE -> CHANGE: split/delete
     * CHANGE -> UNCHANGE: unsuccessful merge
     */
    int *_alpm_merge_state;             /* Per VRF merge state */
    int _alpm_cb_cnt;                   /* Numbers of ALPM Control Block */
    int _alpm_btm_cb_idx[ALPM_BKT_PID_CNT];
    int _alpm_no_hit_bit;
    int _alpm_tcam_hb_is_ptr;
    int _alpm_mode;
    int _alpm_tcam_zoned;
    int _alpm_half_banks;
    int _alpm_128b;
    int _alpm_128b_paired_blk_cnt;
    int _alpm_128b_bkt_rsvd;
    int _alpm_128b_bkt_rsvd_cnt;
    int _alpm_inited;
    int _alpm_bulk_wr_threshold;
    int _alpm_bnk_threshold;
    int _alpm_max_vrf_id;
    int _alpm_spl[ALPM_PKM_CNT];
    int _alpm_tcam_tbl_skip[ALPM_PKM_CNT];

    /* properties for debug purpose */
#define _ALPM_DBG_PVT_SANITY        (1 << 0)    /* 0x1   */
#define _ALPM_DBG_PVT_SANITY_LEN    (1 << 1)    /* 0x2   */
#define _ALPM_DBG_PVT_DEFRAG        (1 << 2)    /* 0x4   */
#define _ALPM_DBG_PVT_SANITY_NODE   (1 << 3)    /* 0x8   */
#define _ALPM_DBG_PVT_SANITY_COUNT  (1 << 4)    /* 0x10  */
#define _ALPM_DBG_WRITE_CACHE_ONLY  (1 << 5)    /* 0x20  */
#define _ALPM_DBG_BNK_CONF          (1 << 6)    /* 0x40  */
#define _ALPM_DBG_PVT_UPDATE_SKIP   (1 << 7)    /* 0x80  */
#define _ALPM_DBG_HITBIT            (1 << 8)    /* 0x100 */
#define _ALPM_DBG_PVT_PTR_CHK       (1 << 9)    /* 0x200 */
#define _ALPM_DBG_PVT_BKT_BNK_CONT  (1 << 10)   /* 0x400 */
#define _ALPM_DBG_PVT_NODE_BKT      (1 << 11)   /* 0x800 */
#define _ALPM_DBG_BNK_CONF_NULL_PVT (1 << 12)   /* 0x1000*/

    uint32 _alpm_dbg_bmp;

    /* properties for debug info purpose */
#define _ALPM_DBG_INFO_PVT  (1 << 0)
#define _ALPM_DBG_INFO_RTE  (1 << 1)
#define _ALPM_DBG_INFO_CNT  (1 << 2)
#define _ALPM_DBG_INFO_HIT  (1 << 3)
#define _ALPM_DBG_INFO_MAX_NUM    4
#define _ALPM_DBG_INFO_ALL  0xffffffff
    uint32 _alpm_dbg_info_bmp;

#define _ALPM_ERR_MSG_BUF_CNT       128
#define _ALPM_ERR_MSG_BUF_ENT_SZ    128
    char *_alpm_err_msg_buf;

    uint32 _alpm_schan_fifo_handle;

    struct alpm_functions_s *alpm_driver;

} _alpm_ctrl_t;

#define _ALPM_ERR_MSG_ENT(u, idx)  (ALPMC(u)->_alpm_err_msg_buf+1+((idx)*_ALPM_ERR_MSG_BUF_ENT_SZ))
/* First char stores the current index */
#define _ALPM_ERR_MSG_CUR_IDX(u) ((ALPMC(u)->_alpm_err_msg_buf[0]))
#define _ALPM_ERR_MSG_INSERT(u, msg) \
    do { \
        uint8 cur_idx = _ALPM_ERR_MSG_CUR_IDX(u);\
        if (cur_idx < _ALPM_ERR_MSG_BUF_CNT - 1) { \
            sal_memcpy(_ALPM_ERR_MSG_ENT(u, cur_idx), msg, _ALPM_ERR_MSG_BUF_ENT_SZ); \
            _ALPM_ERR_MSG_CUR_IDX(u) = cur_idx + 1; \
        } \
    } while(0)

extern _alpm_ctrl_t *alpm_control[SOC_MAX_NUM_DEVICES];

/* bcm_esw_alpm_pvt_trav_cb */
typedef int (*bcm_esw_alpm_pvt_trav_cb) (
    int                 u,
    _alpm_cb_t          *acb,
    _alpm_pvt_node_t    *pvt_node,
    void                *user_data);

typedef struct _alpm_pvt_trav_s {
    _alpm_cb_t                  *acb;
    bcm_esw_alpm_pvt_trav_cb    user_cb;
    void                        *user_data;
} _alpm_pvt_trav_t;

/* bcm_esw_alpm_pfx_trav_cb */
typedef int (*bcm_esw_alpm_pfx_trav_cb)(
    _alpm_pfx_node_t *pfx_node,
    void *user_data);

typedef struct _alpm_pfx_trav_s {
    bcm_esw_alpm_pfx_trav_cb    user_cb;
    void                        *user_data;
} _alpm_pfx_trav_t;

/* bcm_esw_alpm_bkt_trav_cb */
typedef int (*bcm_esw_alpm_bkt_trav_cb)(
    _alpm_bkt_node_t *bkt_node,
    void *user_data);

typedef struct _alpm_bkt_trav_s {
    bcm_esw_alpm_bkt_trav_cb    user_cb;
    void                        *user_data;
} _alpm_bkt_trav_t;

typedef struct _alpm_ppg_cb_user_data_s {
    int  unit;
    int  ppg_op;
    int  ppg_cnt;
    void *ppg_acb;
    void *ppg_data;
} _alpm_ppg_cb_user_data_t;

#define ALPM_INFO_MASK_VALID             0x00000001
#define ALPM_INFO_MASK_KEYLEN            0x00000002
#define ALPM_INFO_MASK_ASSOC_DATA        0x00000004
#define ALPM_INFO_MASK_ALPM_DATA         0x00000008
#define ALPM_INFO_MASK_ALPM_DATA_RAW     0x00000010
#define ALPM_INFO_MASK_FIXED_DATA        0x00000020

/* Used for retrieving alpm info from given entry,
 * Entry can be a TCAM entry or a entry entry, not bank entry */
typedef struct _alpm_ent_info_s {
    /* Input */
    uint32  action_mask;
    /* ent_fmt or sub_idx */
    int     ent_fmt;
    int     vrf_id;
    int     pvt_pkm;
    int     ipt;

    /* KEY & LENGTH */
    uint32  ent_valid;
    uint32  key[5];
    int     key_len;

    /* ALPM_DATA */
    void    *alpm_data_raw;
    int     default_miss;
    int     kshift;
    _alpm_bkt_info_t    bkt_info;

    /* FIXED_DATA */
    int     route_pri; /* Global Hi/Lo, Private */

    /* ASSOC_DATA */
    _alpm_bkt_adata_t   adata;

} _alpm_ent_info_t;

typedef struct _alpm_cb_merge_ctrl_s {
    int unit;
    int merge_count;
    int vrf_id;
    int ipt;
    int bnk_pbkt;
    int ent_pbnk;

    _alpm_cb_t *acb;

    int max_cnt;
} _alpm_cb_merge_ctrl_t;

typedef enum _alpm_cb_merge_type_e {
    ACB_MERGE_INVALID = 0,
    ACB_MERGE_CHILD_TO_PARENT,
    ACB_MERGE_PARENT_TO_CHILD
} _alpm_cb_merge_type_t;

#define ACB_REPART_THRESHOLD 8
#define ACB_MERGE_THRESHOLD  8

#define ALPM_MERGE_REQD(u, vrf_id)              \
        (ALPMC(u)->_alpm_merge_state[vrf_id] != _ALPM_MERGE_UNCHANGE)
#define ALPM_MERGE_STATE_CHKED(u, vrf_id)        \
        (ALPMC(u)->_alpm_merge_state[vrf_id] = _ALPM_MERGE_UNCHANGE)
#define ALPM_MERGE_STATE_CHANGED(u, vrf_id)     \
        (ALPMC(u)->_alpm_merge_state[vrf_id] = _ALPM_MERGE_CHANGE)

#define ALPM_PVT_UPDATE_ALPM_DATA    (0xffffffff)
#define ALPM_PVT_UPDATE_ASSOC_DATA   (0xffffffff)
#define ALPM_PVT_UPDATE_PREFIX       (0xffffffff)
#define ALPM_PVT_UPDATE_ALL          (0xffffffff)

/* ALPM functions */
typedef int (*alpm_ctrl_init_f)(int);
typedef void (*alpm_ctrl_deinit_f)(int);
typedef int (*alpm_ctrl_cleanup_f)(int);
typedef int (*alpm_bkt_bnk_copy_f)(int u, _alpm_cb_t *acb,
                                   _alpm_pvt_node_t *pvt_node,
                                   _alpm_bkt_info_t *src_bkt,
                                   _alpm_bkt_info_t *dst_bkt,
                                   int src_idx, int dst_idx);
typedef int (*alpm_bkt_bnk_clear_f)(int u, _alpm_cb_t *acb,
                                   _alpm_pvt_node_t *pvt_node,
                                   int tab_idx);

typedef int (*alpm_bkt_pfx_copy_f)(int u, _alpm_cb_t *acb,
                                   _alpm_bkt_pfx_arr_t *pfx_arr,
                                   _alpm_pvt_node_t *opvt_node,
                                   _alpm_pvt_node_t *npvt_node);
typedef int (*alpm_bkt_pfx_clean_f)(int u, _alpm_cb_t *acb,
                                    _alpm_pvt_node_t *pvt_node,
                                    int count, uint32 *ent_idx);
typedef int (*alpm_bkt_pfx_shrink_f)(int u, _alpm_cb_t *acb,
                                     _alpm_pvt_node_t *pvt_node,
                                     _alpm_bkt_pfx_arr_t *pfx_arr,
                                     int shrink_empty_only);
typedef int (*alpm_bkt_ent_write_f)(int u, _alpm_cb_t *acb,
                                    _alpm_pvt_node_t *pvt_node,
                                    _bcm_defip_cfg_t *lpm_cfg,
                                    int ent_idx, uint32 write_flags);
typedef uint8 (*alpm_bkt_def_fmt_get_f)(int u, _alpm_cb_t *acb,
                                        int vrf_id, int pkm);
typedef int16 (*alpm_bkt_fmt_type_get_f)(int u, int vrf_id, _alpm_cb_t *acb, uint8 fmt);
typedef int (*alpm_ent_ent_get_f)(int u, int vrf_id, _alpm_cb_t *acb,
                                  void *e, uint32 fmt, int eid, void *ftmp);
typedef int (*alpm_ent_pfx_len_get_f)(int u, _alpm_cb_t *acb,
                                   _alpm_pvt_node_t *pvt_node, int ent_idx);
typedef int (*alpm_ent_selective_get_f)(int u, _alpm_cb_t *acb, void *e,
                                   _alpm_ent_info_t *info);

/* TCAM function pointer */
typedef int (*tcam_tab_sz_f)(int, int);
typedef int (*tcam_hw_cleanup_f)(int);
typedef int (*tcam_ent_x_to_y_f)(int, int, void *, void *, int, int, int);
typedef int (*tcam_ent_from_cfg_f)(int, int, _bcm_defip_cfg_t *, void *, int, uint32 write_flags);
typedef int (*tcam_ent_to_cfg_f)(int, int, void *, int, _bcm_defip_cfg_t *);
typedef int (*tcam_ent_valid_f)(int, int, void *, int);
typedef int (*tcam_ent_valid_set_f)(int, int, void *, int, int);
typedef int (*tcam_ent_vrf_id_get_f)(int, int, void *, int, int *);
typedef int (*tcam_ent_pfx_len_get_f)(int, int, void *, int, int *);
typedef int (*tcam_ent_to_key_f)(int, int, void *, int, uint32 *);

typedef int (*tcam_ent_mode_get_f)(int u, int pk, void *e,
             int *step_size, int *pkm, int *ipv6, int *key_mode, int sub_idx);
typedef int (*tcam_ent_adata_get_f)(int u, int pkm, void *e, int sub_idx,
                                    _alpm_bkt_adata_t *adata);
typedef int (*tcam_ent_bdata_get_f)(int u, int pkm, void *e,
                                    int sub_idx,  void *fent);

/* HITBIT function pointer */
typedef int (*alpm_hit_init_f)(int);
typedef void (*alpm_hit_deinit_f)(int);
typedef int (*alpm_hit_cleanup_f)(int);
typedef int (*alpm_bkt_hit_get_f)(int u, int vrf_id, _alpm_cb_t *acb,
                                  _alpm_tbl_t tbl, int idx, int ent);
typedef int (*alpm_bkt_hit_set_f)(int u, int vrf_id, _alpm_cb_t *acb,
                                  _alpm_tbl_t tbl, int idx, int ent, int hit_val);
typedef int (*alpm_bkt_hit_write_f)(int u, int vrf_id, _alpm_cb_t *acb,
                                    _alpm_tbl_t tbl, int ent_idx, int hit_val);
typedef int (*alpm_hit_move_update_f)(int u, int i, int pid);
typedef int (*alpm_hit_cache_sync_f)(int u, int i, int pid);
typedef int (*alpm_hit_hw_write_f)(int u, int i, int pid);
typedef int (*tcam_ent_hit_get_f)(int u, int pkm, void *e, int sub_idx);
typedef int (*tcam_cache_hit_get_f)(int u, int pkm, int tcam_idx);
typedef int (*tcam_hit_cache_sync_f)(int u);
typedef int (*alpm_table_dump_f)(int u, int vrf_id, soc_mem_t mem, int copyno,
                                 int index, int fmt, int ent_idx, int flags);
typedef int (*alpm_cap_get_f)(int u, soc_mem_t mem, int *max, int *min);

/* TCAM/ALPM function pointer */
typedef int (*mem_ent_read_f)(int unit, _alpm_cb_t *acb, _alpm_tbl_t tbl,
                              int index, void *entry_data,
                              int no_cache);
typedef int (*mem_ent_write_f)(int unit, _alpm_cb_t *acb,
                               _alpm_pvt_node_t *pvt_node, _alpm_tbl_t tbl,
                               int index, void *entry_data);
typedef int (*mem_ent_phy_idx_get_f)(int u, _alpm_cb_t *acb,
                                     _alpm_tbl_t tbl, int index);
typedef struct alpm_functions_s {
    /* ALPM */
    alpm_ctrl_init_f        alpm_ctrl_init;
    alpm_ctrl_deinit_f      alpm_ctrl_deinit;
    alpm_ctrl_cleanup_f     alpm_ctrl_cleanup;
    alpm_bkt_bnk_copy_f     alpm_bkt_bnk_copy;
    alpm_bkt_bnk_clear_f    alpm_bkt_bnk_clear;
    alpm_bkt_pfx_copy_f     alpm_bkt_pfx_copy;
    alpm_bkt_pfx_clean_f    alpm_bkt_pfx_clean;
    alpm_bkt_pfx_shrink_f   alpm_bkt_pfx_shrink;
    alpm_bkt_ent_write_f    alpm_bkt_ent_write;
    alpm_bkt_def_fmt_get_f  alpm_bkt_def_fmt_get;
    alpm_bkt_fmt_type_get_f alpm_bkt_fmt_type_get;

    alpm_ent_ent_get_f      alpm_ent_ent_get;
    alpm_ent_pfx_len_get_f  alpm_ent_pfx_len_get;
    alpm_ent_selective_get_f alpm_ent_selective_get;

    /* TCAM */
    tcam_tab_sz_f           tcam_table_sz;
    tcam_hw_cleanup_f       tcam_hw_cleanup;
    tcam_ent_x_to_y_f       tcam_entry_x_to_y;
    tcam_ent_from_cfg_f     tcam_entry_from_cfg;
    tcam_ent_to_cfg_f       tcam_entry_to_cfg;
    tcam_ent_valid_f        tcam_entry_valid;
    tcam_ent_valid_set_f    tcam_entry_valid_set;
    tcam_ent_vrf_id_get_f   tcam_entry_vrf_id_get;
    tcam_ent_pfx_len_get_f  tcam_entry_pfx_len_get;
    tcam_ent_to_key_f       tcam_entry_to_key;

    tcam_ent_mode_get_f     tcam_entry_mode_get;
    tcam_ent_adata_get_f    tcam_entry_adata_get;
    tcam_ent_bdata_get_f    tcam_entry_bdata_get;

    /* TCAM/ALPM */
    mem_ent_read_f          mem_entry_read;
    mem_ent_write_f         mem_entry_write;

    /* HITBIT */
    alpm_hit_init_f         alpm_hit_init;
    alpm_hit_deinit_f       alpm_hit_deinit;
    alpm_hit_cleanup_f      alpm_hit_cleanup;
    alpm_bkt_hit_get_f      alpm_bkt_hit_get;
    alpm_bkt_hit_set_f      alpm_bkt_hit_set;
    alpm_bkt_hit_write_f    alpm_bkt_hit_write;
    alpm_hit_move_update_f  alpm_hit_move_update;
    alpm_hit_cache_sync_f   alpm_hit_cache_sync;
    alpm_hit_hw_write_f     alpm_hit_hw_write;
    tcam_ent_hit_get_f      tcam_entry_hit_get;
    tcam_cache_hit_get_f    tcam_cache_hit_get;
    tcam_hit_cache_sync_f   tcam_hit_cache_sync;

    /* dump table */
    alpm_table_dump_f       alpm_table_dump;
    alpm_cap_get_f          alpm_cap_get;

    /* Logical idx to physical idx mapping */
    mem_ent_phy_idx_get_f   mem_ent_phy_idx_get;
} alpm_functions_t;

extern alpm_functions_t th3_alpm_driver;
extern alpm_functions_t th_alpm_driver;

#define _SHIFT_LEFT(val, count) \
    (((count) == 32) ? 0 : (val) << (count))

#define _SHIFT_RIGHT(val, count) \
    (((count) == 32) ? 0 : (val) >> (count))

#define PVT_BKT_TRIE(pvt_node)          ((pvt_node)->bkt_trie)
#define PVT_BKT_DEF(pvt_node)           ((pvt_node)->def_pfx)
#define PVT_BKT_INFO(pvt_node)          ((pvt_node)->bkt_info)
#define PVT_ROFS(pvt_node)              (PVT_BKT_INFO(pvt_node).rofs)
#define PVT_BKT_IDX(pvt_node)           (PVT_BKT_INFO(pvt_node).bkt_idx)
#define PVT_SUB_BKT_IDX(pvt_node)       (PVT_BKT_INFO(pvt_node).sub_bkt_idx)
#define PVT_BKT_VRF(pvt_node)           ((pvt_node)->vrf_id)
#define PVT_FLAGS(pvt_node)             ((pvt_node)->pvt_flags)
#define PVT_IS_IPMC(pvt_node)           (PVT_FLAGS(pvt_node) & BCM_L3_IPMC)
#define PVT_BKT_PKM(pvt_node)           ((pvt_node)->bkt_pkm)
#define PVT_BKT_IPT(pvt_node)           (ALPM_PKM2IPT((pvt_node)->bkt_pkm))
#define PVT_BPM_LEN(pvt_node)           ((pvt_node)->bpm_len)
#define PVT_KEY_LEN(pvt_node)           ((pvt_node)->key_len)
#define PVT_KEY_CPY(pvt_node, _key)     (sal_memcpy(pvt_node->key, _key, 4 * 5))
#define PVT_IDX(pvt_node)               ((pvt_node)->tcam_idx)

/* Bucket Info Macros */
#define BI_BNK_IS_USED(bkt_info, bnk)   ((bkt_info)->bnk_fmt[(bnk)] != 0)
#define BI_BNK_FREE(bkt_info, bnk)      ((bkt_info)->bnk_fmt[(bnk)] = 0)
#define BI_SUB_BKT_IDX(bkt_info)        ((bkt_info)->sub_bkt_idx)
#define BI_BKT_IDX(bkt_info)            ((bkt_info)->bkt_idx)
#define BI_ROFS(bkt_info)               ((bkt_info)->rofs)

/* ALPM Macros */
#define ALPMC(u)                    (alpm_control[u])
#define ALPM_MODE(u)                (ALPMC(u)->_alpm_mode)
#define ALPM_MODE_CHK(u, md)        (ALPM_MODE(u) == md)
#define ALPM_TCAM_ZONED(u)          (ALPMC(u)->_alpm_tcam_zoned)
#define ALPM_128B(u)                (ALPMC(u)->_alpm_128b)
#define ALPM_TCAM_PAIR_BLK_CNT(u)   (ALPMC(u)->_alpm_128b_paired_blk_cnt)
#define ALPM_BKT_RSVD(u)            (ALPMC(u)->_alpm_128b_bkt_rsvd)
#define ALPM_BKT_RSVD_CNT(u)        (ALPMC(u)->_alpm_128b_bkt_rsvd_cnt)
#define ALPM_DRV(u)                 (ALPMC(u)->alpm_driver)
#define ALPM_HIT_SKIP(u)            (ALPMC(u)->_alpm_no_hit_bit)
#define ALPM_TCAM_HB_IS_PTR(u)      (ALPMC(u)->_alpm_tcam_hb_is_ptr)
#define ALPM_TCAM_TBL_SKIP(u, pk)   (ALPMC(u)->_alpm_tcam_tbl_skip[pk])
#define ALPM_SCHAN_FIFO_HDL(u)      (ALPMC(u)->_alpm_schan_fifo_handle)
#define ALPM_HLF_BNKS(u)            (ALPMC(u)->_alpm_half_banks)
#define ALPM_PKM_RECOVER(u, ipt) \
    (((ipt) == ALPM_IPT_V4) ? ALPM_PKM_32B : ((ALPM_128B(u)) ? ALPM_PKM_128 : ALPM_PKM_64B))

/* ALPM VRF/VRF_ID Macros */
#define ALPM_VRF_ID_GHI(u)    (ALPMC(u)->_alpm_max_vrf_id + 2)
#define ALPM_VRF_ID_GLO(u)    (ALPMC(u)->_alpm_max_vrf_id + 1)
#define ALPM_VRF_ID_MAX(u)    (ALPMC(u)->_alpm_max_vrf_id + 2)
#define ALPM_VRF_ID_CNT(u)    (ALPMC(u)->_alpm_max_vrf_id + 3)

#define ALPM_VRF_ID_TO_VRF(u, vrf_id)                           \
    ((vrf_id) == ALPM_VRF_ID_GHI(u) ? BCM_L3_VRF_OVERRIDE :     \
     (vrf_id) == ALPM_VRF_ID_GLO(u) ? BCM_L3_VRF_GLOBAL :       \
     (vrf_id))

#define ALPM_VRF_IS_GBL(u, vrf)                                 \
    ((vrf) == BCM_L3_VRF_OVERRIDE || (vrf) == BCM_L3_VRF_GLOBAL)

#define ALPM_VRF_ID_IS_GBL(u, vrf_id)                           \
    ((vrf_id) == ALPM_VRF_ID_GHI(u) || (vrf_id) == ALPM_VRF_ID_GLO(u))
#define ALPM_VRF_ID_TO_BPC_PID(u, vrf_id)                       \
    (ALPM_VRF_ID_IS_GBL(u, (vrf_id)) ? 1 : 0)
#define ALPM_BPC_PID_TO_VRF_ID(u, pid)                          \
    (pid == ALPM_BKT_GLB_PID ? ALPM_VRF_ID_GLO(u) : 1)
#define ALPM_VRF_ID_HAS_BKT(u, vrf_id)                          \
    ((vrf_id) == ALPM_VRF_ID_GHI(u) ? 0 :                       \
     (vrf_id) == ALPM_VRF_ID_GLO(u) ?                           \
     (ALPM_MODE_CHK(u, BCM_ALPM_MODE_TCAM_ALPM) ? 0 : 1) : 1)

#define ALPM_VRF_TO_VRFID(u, vrf)                               \
    ((vrf) == BCM_L3_VRF_OVERRIDE ? ALPM_VRF_ID_GHI(u) :        \
     (vrf) == BCM_L3_VRF_GLOBAL ? ALPM_VRF_ID_GLO(u) :          \
     (vrf))

#define ALPM_VRF_TRIE(u, vrf_id, ipt)                           \
        (ALPMC(u)->_vrf_pfx_hdl[vrf_id].pfx_trie[!!ipt])
#define ALPM_VRF_DATA_MODE(u, vrf_id)                           \
        (ALPMC(u)->_vrf_pfx_hdl[vrf_id].data_mode)

#define ALPM_GLO_VRF_IN_HLF_PMODE(u, vrf_id)                    \
        (ALPM_HLF_BNKS(u) && ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL) && \
         vrf_id == ALPM_VRF_ID_GLO(u))

/* ALPM BPC(BUCKET POOL CONFIG) Macros */
#define BPC_BNK_PER_BKT(bp_conf)        ((bp_conf)->bnk_per_bkt)
#define BPC_BNK_INFO(bp_conf, ipt)      ((bp_conf)->bnk_info[ipt])
#define BPC_BNK_CNT(bp_conf)            ((bp_conf)->bnk_total)
#define BPC_BNK_BMP(bp_conf, ipt)       (BPC_BNK_INFO(bp_conf, ipt)->bnk_btmp)
#define BPC_BNK_RSVD(bp_conf, ipt)      (BPC_BNK_INFO(bp_conf, ipt)->bnk_rsvd)
#define BPC_BNK_USED(bp_conf, ipt)      (BPC_BNK_INFO(bp_conf, ipt)->bnk_used)
#define BPC_BKT_TBL(bp_conf)            ((bp_conf)->bkt_tbl)
#define BPC_BKT_WRA(bp_conf)            ((bp_conf)->bkt_wra)
#define BPC_BKT_FIXED_FMT(bp_conf)      ((bp_conf)->bkt_fixed_fmt)
#define BPC_FMT_INFO(bp_conf)           ((bp_conf)->fmt_info)
#define BPC_FMT_CNT(bp_conf)            (BPC_FMT_INFO(bp_conf)->fmt_count)
#define BPC_FMT_TYPE(bp_conf)           (BPC_FMT_INFO(bp_conf)->fmt_type)
#define BPC_FMT_PFX_LEN(bp_conf, fmt)   (BPC_FMT_INFO(bp_conf)->fmt_pfx_len[fmt])
#define BPC_FMT_ENT_MAX(bp_conf, fmt)   (BPC_FMT_INFO(bp_conf)->fmt_ent_max[fmt])
#define BPC_FMT_BNK(bp_conf, fmt)       (BPC_FMT_INFO(bp_conf)->fmt_bnk[fmt])
#define BPC_FMT_ENT(bp_conf, fmt)       (BPC_FMT_INFO(bp_conf)->fmt_ent[fmt])
#define BPC_FMT_FULL_TYPE(bp_conf, fmt) (BPC_FMT_INFO(bp_conf)->fmt_ftype[fmt])

/* ALPM ACB Macros */
#define ACB_IDX(acb)                ((acb)->acb_idx)
#define ACB_HAS_TCAM(acb)           (ACB_IDX(acb) == 0)
#define ACB(u, i)                   (ALPMC(u)->alpm_cb[i])
#define ACB_UPR(u, acb)             (ACB(u, ACB_IDX(acb) - 1))
#define ACB_DWN(u, acb)             (ACB(u, ACB_IDX(acb) + 1))
#define ACB_TOP(u)                  (ACB(u, 0))
#define ACB_CNT(u)                  (ALPMC(u)->_alpm_cb_cnt)
#define ACB_BTM(u)                  (ACB(u, ACB_CNT(u) - 1))
#define ACB_VRF_BTM_IDX(u, vrf_id)  (ALPMC(u)->_alpm_btm_cb_idx[ALPM_VRF_ID_TO_BPC_PID(u, vrf_id)])
#define ACB_VRF_BTM(u, vrf_id)      (ACB(u, ACB_VRF_BTM_IDX(u, vrf_id)))

#define ACB_VRF_INITED(u, acb, vrf_id, ipt)                     \
        ((acb)->pvt_ctl[!!ipt] &&                               \
         ACB_PVT_CTRL(acb, vrf_id, ipt).db_inited)
#define ACB_VRF_INIT_SET(u, acb, vrf_id, ipt)                   \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).db_inited = TRUE)
#define ACB_VRF_INIT_CLEAR(u, acb, vrf_id, ipt)                 \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).db_inited = FALSE)
#define ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt)                    \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).db_type)
#define ACB_VRF_DB_TYPE_SET(u, acb, vrf_id, ipt, rt)            \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).db_type = rt)
#define ACB_VRF_SPLEN_DIFF1(u, acb, vrf_id, ipt)                \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).spl_len_diff1)
#define ACB_VRF_SPLEN_DIFF2(u, acb, vrf_id, ipt)                \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).spl_len_diff2)

#define ACB_DR_INITED(u, acb, vrf_id, ipt)                      \
        ((acb)->pvt_ctl[!!ipt] &&                               \
         ACB_PVT_CTRL(acb, vrf_id, ipt).dr_inited)
#define ACB_DR_INIT_SET(u, acb, vrf_id, ipt)                    \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).dr_inited = TRUE)
#define ACB_DR_INIT_CLEAR(u, acb, vrf_id, ipt)                  \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).dr_inited = FALSE)

#define ACB_BNK_CONF(acb)               ((acb)->bnk_conf)
#define ACB_BNK_BMP(acb, vrf_id, ipt)                           \
        (BPC_BNK_BMP(ACB_BKT_VRF_POOL(acb, vrf_id), ipt))
#define ACB_BNK_CNT(acb, vrf_id)                                \
        (BPC_BNK_CNT(ACB_BKT_VRF_POOL(acb, vrf_id)))
#define ACB_PID_BNK_CNT(acb, pid)                               \
        (BPC_BNK_CNT(ACB_BKT_POOL(acb, pid)))
#define ACB_BNK_PER_BKT(acb, vrf_id)                            \
        (BPC_BNK_PER_BKT(ACB_BKT_VRF_POOL(acb, vrf_id)))
#define ACB_BKT_POOL(acb, pid)          (ACB_BNK_CONF(acb).bkt_pool[pid])
#define ACB_BKT_VRF_POOL(acb, vrf_id)                           \
        (ACB_BKT_POOL(acb, ALPM_VRF_ID_TO_BPC_PID(acb->unit, vrf_id)))
#define ACB_POOL_FMT_INFO(acb, pid)     (BPC_FMT_INFO(ACB_BKT_POOL(acb, pid)))
#define ACB_VRF_POOL_FMT_INFO(acb, vrf_id)                      \
        (BPC_FMT_INFO(ACB_BKT_POOL(acb, ALPM_VRF_ID_TO_BPC_PID(acb->unit, vrf_id))))
#define ACB_BKT_TBL(acb, vrf_id)                                \
        (BPC_BKT_TBL(ACB_BKT_VRF_POOL(acb, vrf_id)))
#define ACB_BKT_WRA(acb, vrf_id)                                \
        (BPC_BKT_WRA(ACB_BKT_VRF_POOL(acb, vrf_id)))
#define ACB_BKT_FIXED_FMT(acb, vrf_id)                          \
        (BPC_BKT_FIXED_FMT(ACB_BKT_VRF_POOL(acb, vrf_id)))
#define ACB_PVT_CTRL(acb, vrf_id, ipt)  (acb->pvt_ctl[!!ipt][vrf_id])
#define ACB_PVT_TRIE(acb, vrf_id, ipt)                          \
        (ACB_PVT_CTRL(acb, vrf_id, ipt).pvt_trie)
#define ACB_VRF_PVT_PTR(acb, vrf_id, idx)                       \
        (ACB_BKT_VRF_POOL(acb, vrf_id)->pvt_ptr[idx])
#define ACB_PVT_PTR(acb, pvt_node, idx)                         \
        (ACB_VRF_PVT_PTR(acb, PVT_BKT_VRF(pvt_node), idx))

#define ACB_PID_CNT(acb)                                        \
        (ACB_BKT_POOL(acb, 0) == ACB_BKT_POOL(acb, 1) ? 1 : 2)
#define ACB_BKT_VRF_PID(acb, vrf_id)                            \
        (ACB_BKT_POOL(acb, 0) == ACB_BKT_POOL(acb, 1) ? 0 :     \
        ALPM_VRF_ID_TO_BPC_PID(acb->unit, vrf_id))

#define ACB_BKT_BITS(acb)               (ACB_BNK_CONF(acb).bkt_bits)
#define ACB_BKT_CNT(acb)                (1 << ACB_BKT_BITS(acb))
#define ACB_BKT_DW(acb)                 (ACB_BNK_CONF(acb).bkt_dw)
#define ACB_BNK_BITS(acb)               (ACB_BNK_CONF(acb).bnk_bits)
#define ACB_PHY_BNK_PER_BKT(acb)        (ACB_BNK_CONF(acb).phy_bnks_per_bkt)
#define ACB_FMT_PFX_LEN(acb, vrf_id, fmt)  (ACB_VRF_POOL_FMT_INFO(acb, vrf_id)->fmt_pfx_len[fmt])
#define ACB_FMT_ENT_MAX(acb, vrf_id, fmt)  (ACB_VRF_POOL_FMT_INFO(acb, vrf_id)->fmt_ent_max[fmt])
#define ACB_FMT_BNK(acb, vrf_id, fmt)   (ACB_VRF_POOL_FMT_INFO(acb, vrf_id)->fmt_bnk[fmt])
#define ACB_FMT_ENT(acb, vrf_id, fmt)   (ACB_VRF_POOL_FMT_INFO(acb, vrf_id)->fmt_ent[fmt])
#define ACB_FMT_FULL_TYPE(acb, vrf_id, fmt)  (ACB_VRF_POOL_FMT_INFO(acb, vrf_id)->fmt_ftype[fmt])
#define ACB_FMT_CNT(acb, vrf_id)        (ACB_VRF_POOL_FMT_INFO(acb, vrf_id)->fmt_count)
#define ACB_FMT_TYPE(acb, vrf_id)        (ACB_VRF_POOL_FMT_INFO(acb, vrf_id)->fmt_type)
#define ACB_HLF_BNK_MD(acb)             (ACB_BNK_BITS(acb) == 2)

#define ACB_HAS_RTE(acb, vrf_id)    (ACB_FMT_TYPE(acb, vrf_id) == ALPM_FMT_RTE)
#define ACB_HAS_PVT(acb, vrf_id)    (ACB_FMT_TYPE(acb, vrf_id) == ALPM_FMT_PVT)
#define ACB_VRF_EMPTY(acb, vrf_id)  (ACB_FMT_TYPE(acb, vrf_id) == ALPM_FMT_CNT)

/* ALPM LPM Macros */
#define ALPM_LPM_DIRECT_RTE(u, lpm_cfg)                         \
    ((lpm_cfg->defip_flags & BCM_L3_IPMC) ||                    \
    (((lpm_cfg)->defip_vrf == BCM_L3_VRF_OVERRIDE) ? 1 :        \
     ((lpm_cfg)->defip_vrf == BCM_L3_VRF_GLOBAL) ?              \
     (ALPM_MODE_CHK(u, BCM_ALPM_MODE_TCAM_ALPM) ? 1 : 0) : 0))

#define ALPM_LPM_PKM(u, lpm_cfg)                                \
    ((lpm_cfg)->defip_flags & BCM_L3_IP6 ?                      \
     (ALPM_128B(u) ? ALPM_PKM_128 : ALPM_PKM_64B) :             \
     ALPM_PKM_32B)

#define ALPM_LPM_IPT(u, lpm_cfg)                                \
    (((lpm_cfg)->defip_flags & BCM_L3_IP6) ? ALPM_IPT_V6 : ALPM_IPT_V4)

#define ALPM_LPM_VRF_ID(u, lpm_cfg)                             \
    ((lpm_cfg)->defip_vrf >= 0 ?    (lpm_cfg)->defip_vrf :      \
     (lpm_cfg)->defip_vrf == BCM_L3_VRF_GLOBAL ?                \
                                    (ALPM_VRF_ID_GLO(u)) :      \
                                    (ALPM_VRF_ID_GHI(u)))

/*  -- ALPM entry IDX Format --
 * IDX:
 *   <ent> | <bnk> | <bkt>
 * TAB_IDX:
 *   <bnk> | <bkt>
 */

/*
 +-----------------------------------+
 | 8 bits    | bnk bits | bkt bits   |
 +-----------------------------------+
 | EntryID   | BankID   | BucketID   |
 +-----------------------------------+
 */

#define ALPM_IDX_ENT_SHIFT          24
#define ALPM_IDX_ENT_MASK           0xff
#define ALPM_IDX_BNK_SHIFT(acb)     ACB_BKT_BITS(acb)
#define ALPM_IDX_BNK_MASK           0xff
#define ALPM_IDX_BKT_MASK(acb)      ((1 << ALPM_IDX_BNK_SHIFT(acb)) - 1)
#define ALPM_TAB_IDX_MASK           ((1 << ALPM_IDX_ENT_SHIFT) - 1) /* 0xffffff */

#define ALPM_IDX_TO_BKT(acb, idx)                            \
    ((idx) & ALPM_IDX_BKT_MASK(acb))

#define ALPM_IDX_TO_BNK(acb, idx)                            \
    (((idx) >> ALPM_IDX_BNK_SHIFT(acb)) & ALPM_IDX_BNK_MASK)

#define ALPM_IDX_TO_ENT(idx)                                 \
    (((idx) >> ALPM_IDX_ENT_SHIFT) & ALPM_IDX_ENT_MASK)

#define ALPM_IDX_MAKE(acb, bi, bnk, ent)                     \
    (((ent) & ALPM_IDX_ENT_MASK) << ALPM_IDX_ENT_SHIFT |     \
     (bnk) << ALPM_IDX_BNK_SHIFT(acb) |                      \
     ((BI_BKT_IDX(bi) + ((bnk) < BI_ROFS(bi))) & ALPM_IDX_BKT_MASK(acb)))

/*  -- ALPM Table IDX construct --
 * TAB_IDX:
 *   <bnk> | <bkt>
 */
#define ALPM_TAB_IDX_GET_BKT_BNK(acb, rofs, bkt, bnk)       \
    ((bnk << ALPM_IDX_BNK_SHIFT(acb)) | (bkt + (rofs > bnk)))

#define ALPM_TAB_IDX_GET(idx)   (idx & ALPM_TAB_IDX_MASK)

#define ALPM_TAB_IDX_GET_BI_BNK(acb, bi, bnk)               \
    ALPM_TAB_IDX_GET_BKT_BNK(acb, BI_ROFS(bi), BI_BKT_IDX(bi), bnk)

/* VRF Based Counter */
#define VRF_PVT_LEN_CNT_ADD(acb, vrf_id, ipt, pvt_len)  \
do {                                                    \
    ACB_PVT_CTRL(acb, vrf_id, ipt).pvt_len_cnt[pvt_len] ++;      \
} while (0)

#define VRF_PVT_LEN_CNT_DEC(acb, vrf_id, ipt, pvt_len)  \
do {                                                    \
    ACB_PVT_CTRL(acb, vrf_id, ipt).pvt_len_cnt[pvt_len] --;      \
} while (0)

#define VRF_PVT_LEN_CNT_GET(acb, vrf_id, ipt, pvt_len)  \
    (ACB_PVT_CTRL(acb, vrf_id, ipt).pvt_len_cnt[pvt_len])

#define VRF_PVT_LEN_CNT_PTR(acb, vrf_id, ipt)           \
    (ACB_PVT_CTRL(acb, vrf_id, ipt).pvt_len_cnt)

#define VRF_ROUTE_ADD(acb, vrf_id, ipt)               \
do {                                                  \
    ACB_PVT_CTRL(acb, vrf_id, ipt).route_cnt ++;      \
    ACB_PVT_CTRL(acb, vrf_id, ipt).cnt_add ++;        \
} while (0)

#define VRF_ROUTE_DEC(acb, vrf_id, ipt)               \
do {                                                  \
    ACB_PVT_CTRL(acb, vrf_id, ipt).route_cnt --;      \
    ACB_PVT_CTRL(acb, vrf_id, ipt).cnt_del ++;        \
} while (0)

#define VRF_ROUTE_CNT(acb, vrf_id, ipt)               \
    (ACB_PVT_CTRL(acb, vrf_id, ipt).route_cnt)

/* ALPM logs */
/* Macro for invoking "fast" checker */
#define ALPM_LOG(chk_, stuff_) do {         \
        if (bsl_fast_check(chk_)) {         \
            bsl_printf stuff_;              \
        }                                   \
    } while (0)

extern char alpm_tmpbuf[_ALPM_ERR_MSG_BUF_ENT_SZ];
extern void alpm_util_snprintf(const char *fmt, ...);

#define ALPM_ERR_MSG_LOG(stuff_) do {       \
        if (ALPMTR(u)) ALPM_LOG(BSL_LS_BCM_ALPM|BSL_ERROR, ("#%d: ", ALPMTR_CNT(u)));\
        ALPM_LOG(BSL_LS_BCM_ALPM|BSL_ERROR, stuff_);\
        if (ALPMC(u) != NULL) {\
            sal_memset(alpm_tmpbuf, 0, sizeof(alpm_tmpbuf));\
            if (ALPMTR(u)) sal_sprintf(alpm_tmpbuf, "#%d: ", ALPMTR_CNT(u));\
            alpm_util_snprintf stuff_;\
            _ALPM_ERR_MSG_INSERT(u, alpm_tmpbuf);  \
        }\
    } while (0)

/* Any layer log macros */
#define ALPM_FATAL(stuff_)          ALPM_LOG(BSL_LS_BCM_ALPM|BSL_FATAL, stuff_)
#define ALPM_ERR(stuff_)            ALPM_ERR_MSG_LOG(stuff_)
#define ALPM_WARN(stuff_)           ALPM_LOG(BSL_LS_BCM_ALPM|BSL_WARN, stuff_)
#define ALPM_INFO(stuff_)           ALPM_LOG(BSL_LS_BCM_ALPM|BSL_INFO, stuff_)
#define ALPM_VERB(stuff_)           ALPM_LOG(BSL_LS_BCM_ALPM|BSL_VERBOSE, stuff_)
#define ALPM_DBG(stuff_)            ALPM_LOG(BSL_LS_BCM_ALPM|BSL_DEBUG, stuff_)
#define ALPM_LOG_CHECK(stuff_)      LOG_CHECK(BSL_LS_BCM_ALPM|(stuff_))

/* ALPM ERROR condition check */

#define ALPM_IER_PRT_EXCEPT(op, except_err)            \
    do { int __rv__; if ((__rv__ = (op)) < 0) { \
        if (__rv__ != (except_err) && __rv__ != BCM_E_BUSY) \
        {ALPM_ERR(("ALPMerr: %s : %d rv %d\n", FUNCTION_NAME(), __LINE__, __rv__));} return(__rv__); } } while(0)

/* If error return */
#define ALPM_IER(op)            \
    do { int __rv__; if ((__rv__ = (op)) < 0) { \
        if (__rv__ != BCM_E_BUSY) \
        {ALPM_ERR(("ALPMerr: %s : %d rv %d\n", FUNCTION_NAME(), __LINE__, __rv__));} return(__rv__); } } while(0)

#define ALPM_IEG_PRT_EXCEPT(op, except_err)            \
    do { if ((rv = (op)) < 0) { \
        if (rv != (except_err) && rv != BCM_E_BUSY) \
        {ALPM_ERR(("ALPMerr: %s : %d rv %d\n", FUNCTION_NAME(), __LINE__, rv));} goto bad; } } while(0)

/* If error goto */
#define ALPM_IEG(op)            \
    do { if ((rv = (op)) < 0) { \
        if (rv != BCM_E_BUSY) \
        {ALPM_ERR(("ALPMerr: %s : %d rv %d\n", FUNCTION_NAME(), __LINE__, rv));} goto bad; } } while(0)

#define ALPM_REALLOC_EG(_ptr, _size, _str)  \
    do {                                    \
        if (_ptr == NULL) {                 \
            _ptr = alpm_util_alloc(_size, _str);\
        }                                   \
        if (_ptr == NULL) {                 \
            rv = BCM_E_MEMORY;              \
            ALPM_ERR(("ALPMerr: out of memory at %s : %d\n", FUNCTION_NAME(), __LINE__)); \
            goto bad;                       \
        }                                   \
        sal_memset(_ptr, 0, _size);         \
    } while (0)

#define ALPM_ALLOC_EG(_ptr, _size, _str)    \
    do {                                    \
        _ptr = alpm_util_alloc(_size, _str);\
        if (_ptr == NULL) {                 \
            rv = BCM_E_MEMORY;              \
            ALPM_ERR(("ALPMerr: out of memory at %s : %d\n", FUNCTION_NAME(), __LINE__)); \
            goto bad;                       \
        }                                   \
        sal_memset(_ptr, 0, _size);         \
    } while (0)

#define ALPM_DMA_ALLOC_EG(_u, _ptr, _size, _str)    \
    do {                                            \
        _ptr = soc_cm_salloc(_u, _size, _str);      \
        if (_ptr == NULL) {                 \
            rv = BCM_E_MEMORY;              \
            ALPM_ERR(("ALPMerr: out of memory at %s : %d\n", FUNCTION_NAME(), __LINE__)); \
            goto bad;                       \
        }                                   \
        sal_memset(_ptr, 0, _size);         \
    } while (0)

/* ALPM externs */
extern int
alpm_trie_ent_to_pfx(int u, int pkm, void *e, int, int, uint32 *key);
extern void
alpm_trie_pfx_to_cfg(int u, uint32 *pfx, int plen, _bcm_defip_cfg_t *cfg);

extern int
alpm_bkt_free_bnk_range_get(int u, SHR_BITDCL *bnk_bmp, int bnk_sz,
                            int req_cnt, int *rng_left, int *rng_right,
                            int *rng_mid);
extern int
alpm_bkt_bnk_nearest_get(int u, SHR_BITDCL *bnk_bmp, int bnk_sz,
                         int left_bnk, int right_bnk, int *nst_bnk);
extern int
alpm_bkt_ent_get_attempt(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                 _alpm_bkt_info_t *bkt_info, uint8 def_fmt, int pfx_len,
                 uint32 *ent_idx, int *fmt_update, int bkt_test);

extern int alpm_cb_merge(int u, _alpm_cb_t *acb, int vrf_id, int ipt);
extern int alpm_cb_delete(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg);
extern int alpm_cb_insert(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *cfg);
extern int alpm_cb_defrag(int u, _alpm_cb_t *acb, int vrf_id, int ipt);
extern int alpm_cb_expand(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *, int, int *);
extern int alpm_cb_expand_in_bkt(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *, int );
extern int alpm_cb_pvt_sanity_cb(int, _alpm_cb_t *, _alpm_pvt_node_t *, void *);
extern int alpm_cb_sanity(int u, int acb_bmp, int vrf_id, int);
extern int alpm_cb_find(int, _alpm_cb_t *, _bcm_defip_cfg_t *,
                        _alpm_pvt_node_t **, _alpm_bkt_node_t **);
extern void alpm_cb_pvt_dump(int u, int acb_bmp, int bkt_idx);
extern void alpm_cb_fmt_dump(int u, int acb_bmp);
extern void alpm_cb_pvtlen_dump(int u, int acb_bmp);
extern void alpm_cb_bkt_dump(int u, int acb_bmp);
extern void alpm_cb_stat_dump(int u, int acb_bmp);
extern void alpm_cb_conf_dump(int u, int acb_bmp);

extern int alpm_bkt_ent_write(int, _alpm_cb_t *, _alpm_pvt_node_t *,
                              _bcm_defip_cfg_t *, int, uint32 write_flags);

extern int  alpm_table_dump(int u, int vrf_id, soc_mem_t mem, int copyno, int index,
                            int fmt, int ent_idx, int flags);
extern void th_alpm_table_ent_dump(int u, int vrf_id, soc_mem_t mem, void *entry,
                int copyno, int index, int fmt,
                int ent_idx, int flags, char *pfx);

extern int soc_alpm_cmn_mode_get(int unit);
extern int soc_alpm_cmn_banks_get(int unit);

/* ESW externs */
extern int bcm_esw_alpm_init(int u);
extern int bcm_esw_alpm_cleanup(int u, _bcm_l3_trvrs_data_t *trv_data,
                                _bcm_l3_trvrs_data_t *trv_data1);
extern int bcm_esw_alpm_fast_delete_all(int u, _bcm_l3_trvrs_data_t *trv_data,
                                        _bcm_l3_trvrs_data_t *trv_data1);
extern int bcm_esw_alpm_lookup(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_insert(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_delete(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_update(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_find(int u, _bcm_defip_cfg_t *lpm_cfg,
                             int *nh_ecmp_idx);
extern int bcm_esw_alpm_update_match(int u, _bcm_l3_trvrs_data_t *trv_data);
extern int bcm_esw_alpm_pvt_traverse(int u, int lvlp, int vrf_id, int ipt,
                bcm_esw_alpm_pvt_trav_cb trav_fn, void *user_data);
extern int bcm_esw_alpm_pfx_traverse(int u, int vrf_id, int ipt,
                bcm_esw_alpm_pfx_trav_cb trav_fn, void *user_data);
extern int bcm_esw_alpm_bkt_traverse(alpm_lib_trie_t *bkt_trie,
                bcm_esw_alpm_bkt_trav_cb trav_fn, void *user_data);
extern int bcm_esw_alpm_pvt_count_get(int u, _alpm_pvt_node_t *pvt_node, int *);
extern int bcm_esw_alpm_vrf_data_mode_get(int u, bcm_vrf_t vrf, uint32 flags,
                                          bcm_l3_vrf_route_data_mode_t *mode);
extern int bcm_esw_alpm_vrf_data_mode_set(int u, bcm_vrf_t vrf, uint32 flags,
                                          bcm_l3_vrf_route_data_mode_t mode);
extern int tcam_entry_to_key(int u, int pkm, void *e, int sub_idx, uint32 *key);

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int bcm_th3_alpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                            int nh_ecmp_idx);
extern int bcm_th3_alpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_th3_alpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int
                            *nh_ecmp_idx);
extern int bcm_th3_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg,
                             int *nh_ecmp_idx);
extern int bcm_th3_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
extern int bcm_th_alpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                             int nh_ecmp_idx);
extern int bcm_th_alpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_th_alpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int
                            *nh_ecmp_idx);
extern int bcm_th_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg,
                             int *nh_ecmp_idx);
extern int bcm_th_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
#endif

/* Extern functions of alpm_tcam.c */
extern int bcm_esw_alpm_tcam_insert(int u, _bcm_defip_cfg_t *lpm_cfg, uint32 write_flags);
extern int bcm_esw_alpm_tcam_match(int u, _bcm_defip_cfg_t *lpm_cfg, int *idx);
extern int bcm_esw_alpm_tcam_delete(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_esw_alpm_tcam_init(int u);
extern int bcm_esw_alpm_tcam_deinit(int u);
extern int bcm_esw_alpm_tcam_cleanup(int u);
extern int bcm_esw_alpm_tcam_state_free_get(int, int zn, int *fcnt, int *vcnt);
extern void bcm_esw_alpm_tcam_zone_dump(int u, char *pfx_str);
extern void bcm_esw_alpm_tcam_zone_state_dump(int u);
extern int bcm_esw_alpm_tcam_avail(int u, int vrf_id, int ipt, int key_len, int mc);
extern int alpm_pvt_trie_init(int u, int vrf_id, int ipt);
extern int alpm_pvt_trie_insert(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int alpm_pvt_trie_delete(int u, _bcm_defip_cfg_t *lpm_cfg);

/* APIs distributed hitbits */
extern int bcm_esw_alpm_hit_init(int u);
extern int bcm_esw_alpm_hit_deinit(int u);
extern int bcm_esw_alpm_clear_hit(int u, _bcm_defip_cfg_t *lpm_cfg);
extern int alpm_dist_hitbit_enable_set(int u, int us);
extern int alpm_dist_hitbit_interval_get(int u);
extern int alpm_rte_hit_get(int u, int vrf_id, _alpm_tbl_t tbl, int idx, int ent);
extern int alpm_pvt_ent_idx_get(int u, _alpm_cb_t *acb,
                                _alpm_pvt_node_t *pvt_node, int *ent_idx);
/* Extern fucntions for alpm_tr.c */
extern int bcm_esw_alpm_trace_init(int u);
extern int bcm_esw_alpm_trace_deinit(int u);
extern int alpm_trace_set(int u, int val);
extern int alpm_trace_clear(int u);
extern int alpm_trace_sanity(int u, int enable, int start, int end);
extern int alpm_trace_wrap_set(int u, int val);
extern int alpm_trace_log(int u, int op, _bcm_defip_cfg_t *cfg, int nh_idx, int rc);

/* ALPM UnitTest definitions */

#define ALPM_UT_FLAG_IP_INCR    (1U << 1)
#define ALPM_UT_FLAG_LEN_INCR   (1U << 2)
#define ALPM_UT_FLAG_VRF_INCR   (1U << 3)
#define ALPM_UT_FLAG_SPEED      (1U << 4)
#define ALPM_UT_FLAG_NOMSG      (1U << 5)

#define ALPM_UT_V4 (1 << 0)
#define ALPM_UT_V6 (1 << 1)

#define ALPM_UT_OPC_ADD     0
#define ALPM_UT_OPC_DEL     1

typedef struct alpm_ut_route_db_s {
    int         type;
    int         vrf;
    char        *base_addr;
    uint32      pfx_len;
    int         count;
    int         opc;
} alpm_ut_route_db_t;


/* dbg_info */
extern int alpm_dbg_info_bmp_show(int u);
extern int alpm_dbg_info_bmp_set(int u, uint32 flags);

#endif /* ALPM_ENABLE */

#endif /* _BCM_INT_ALPM_H_ */

