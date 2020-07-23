/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_BST_H
#define _BCM_BST_H

#include <soc/defs.h>
#include <soc/types.h>
#include <bcm/cosq.h>

#ifdef BCM_TOMAHAWK3_SUPPORT
#define _BCM_MAX_PIPES              8
#define _BCM_MAX_THD_REGS           8
#define _BCM_MAX_PROFILES           8
#define _BCM_MAX_PROFILE_REGS       4
#define _BCM_MAX_REGS_PER_PROFILE   2
#define _BCM_BST_MAX_FIELDS         8
#else
#define _BCM_MAX_PIPES              4
#define _BCM_MAX_XPES               4
#define _BCM_MAX_THD_REGS           8
#define _BCM_MAX_PROFILES           4
#define _BCM_MAX_PROFILE_REGS       4
#define _BCM_MAX_REGS_PER_PROFILE   2
#endif

typedef enum {
    _bcmResourceDevice  = 0,
    _bcmResourceEgrPool,
    _bcmResourceEgrMCastPool,
    _bcmResourceIngPool,
    _bcmResourcePortPool,
    _bcmResourcePriGroupShared,
    _bcmResourcePriGroupHeadroom,
    _bcmResourceUcast,
    _bcmResourceMcast,
    _bcmResourceHeadroomPool,
    _bcmResourceEgrPortPoolSharedUcast,
    _bcmResourceEgrPortPoolSharedMcast,
    _bcmResourceBstStatIdQueueTotal,
    _bcmResourceRQEQueue,
    _bcmResourceRQEPool,
    _bcmResourceUCQueueGroup,
    _bcmResourceMaxCount
} _bcm_bst_resource_t;

typedef enum _bcm_bst_cb_ret_e {
    _BCM_BST_RV_OK  = 0,
    _BCM_BST_RV_RETRY,
    _BCM_BST_RV_ERROR
} _bcm_bst_cb_ret_t;

typedef enum _bcm_bst_snapshot_mode_e {
    _BCM_BST_SNAPSHOT_THDO,
    _BCM_BST_SNAPSHOT_THDI,
    _BCM_BST_SNAPSHOT_CFAP,
    _BCM_BST_SNAPSHOT_COUNT
} _bcm_bst_snapshot_mode_t;

typedef enum _bcm_bst_thread_oper_e {
    _bcmBstThreadOperExternal = 0, /* External Operation to Start/Kill
                                    * the BST Thread. */
    _bcmBstThreadOperInternal = 1  /* Internal Operation to Pause/Resume
                                    * the BST Thread */
} _bcm_bst_thread_oper_t;

/*
 * _bcm_bst_device_index_resolve callback is used if device
 * bst API uses the common implementation. This callback queries
 * the device implementation to resolve the gport, cosq to
 * bst resource, along with the index ranges. also device implementation
 * can return a reason code retry to call tresolve again if the
 * indexes are not contiguous or to map multiple resources.
 */
typedef _bcm_bst_cb_ret_t (*_bcm_bst_device_index_resolve)(int unit,
                                         bcm_gport_t gport, bcm_cos_queue_t cosq,
                                         bcm_bst_stat_id_t bid,
                                         int *pipe, int *from_hw_index, 
                                         int *to_hw_index, int *rcb_data, 
                                         void **user_data, int *bcm_rv);

typedef int (*_bcm_bst_device_index_map)(int unit,
            bcm_bst_stat_id_t bid, int port, int index, bcm_gport_t *gport, bcm_cos_t *cosq);

typedef struct _bcm_bst_device_handlers_s {
    _bcm_bst_device_index_resolve resolve_index;
    _bcm_bst_device_index_map     reverse_resolve_index;
} _bcm_bst_device_handlers_t;

extern int _bcm_bst_attach(int unit, _bcm_bst_device_handlers_t *cbs);

extern int _bcm_bst_detach(int unit);

extern int
_bcm_bst_cmn_profile_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         int xpe, bcm_bst_stat_id_t bid, 
                         bcm_cosq_bst_profile_t *profile);

extern int
_bcm_bst_cmn_profile_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         int xpe, bcm_bst_stat_id_t bid, 
                         bcm_cosq_bst_profile_t *profile);

extern int _bcm_bst_cmn_control_set(int unit, bcm_switch_control_t type, int arg);

extern int _bcm_bst_cmn_control_get(int unit, bcm_switch_control_t type, int *arg);

extern int
_bcm_bst_cmn_stat_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                      bcm_cosq_buffer_id_t buffer_id, bcm_bst_stat_id_t bid,
                      uint32 options, uint64 *pvalue);

extern int _bcm_bst_cmn_stat_sync(int unit, bcm_bst_stat_id_t bid);

extern int 
_bcm_bst_cmn_stat_multi_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                            uint32 options, int max_values,
                            bcm_bst_stat_id_t *id_list, uint64 *pvalues);
extern int 
_bcm_bst_cmn_stat_clear(int unit, bcm_gport_t port, bcm_cos_queue_t cosq, 
                        bcm_bst_stat_id_t bid);

#define BST_OP_GET  0x1
#define BST_OP_SET  0x2
#define BST_OP_THD  0x4
/* cfg/stat on different xpes are same */
#define BCM_BST_DUP_XPE     -1
/* cfg/stat on different xpes could be different */
#define BCM_BST_MLT_XPE     -2

typedef struct _bcm_bst_resource_info_s {
    int         valid;
    uint32      flags;
    soc_pbmp_t  pbmp;
    soc_mem_t   stat_mem[_BCM_MAX_PIPES];
    soc_reg_t   stat_reg[_BCM_MAX_PIPES];
    soc_field_t stat_field;
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_field_t stat_fields[_BCM_BST_MAX_FIELDS];
    int         num_field;
    soc_field_t thd_fields[_BCM_BST_MAX_FIELDS];
    int         num_profile_reg;
#endif
    int         threshold_adj;
    int         threshold_gran; /* Granularity of cells */
    soc_mem_t   threshold_mem[_BCM_MAX_PIPES];
    soc_mem_t   threshold_reg[_BCM_MAX_THD_REGS];
    soc_field_t threshold_field;

    soc_mem_t   profile_mem;
    soc_mem_t   profile_reg[_BCM_MAX_PROFILE_REGS];
    soc_field_t profile_field;

    soc_profile_reg_t  iprofile_r[_BCM_MAX_PROFILES];
    soc_profile_reg_t  *profile_r[_BCM_MAX_PROFILES];
    soc_profile_mem_t  iprofile_m;
    soc_profile_mem_t  *profile_m;

    int                 index_min;
    int                 index_max;

    uint32              *p_stat;
    uint32              *p_threshold;
    int                 num_instance; /* total instances of this resource */
    int                 num_stat_pp;
} _bcm_bst_resource_info_t;

typedef struct _bcm_bst_tbl_ctrl {
    soc_block_t blk;
    uint32 tindex;
    uint32 entries;
    uint8 *buff;
    uint32 size;
} _bcm_bst_tbl_ctrl_t;

typedef struct _bcm_bst_tbl_desc {
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 flags;
    int acc_type;
    uint32 blk;
    uint32 addr;
    uint32 width;
    uint32 entries;
    uint32 shift;
} _bcm_bst_tbl_desc_t;

typedef int (*_bcm_bst_byte_to_cell)(int unit, uint32 byte);

typedef int (*_bcm_bst_cell_to_byte)(int unit, uint32 cell);

typedef int (*_bcm_bst_control_set)(int unit, bcm_switch_control_t type,
                                    int arg, _bcm_bst_thread_oper_t operation);
typedef int (*_bcm_bst_control_get)(int unit, bcm_switch_control_t type,
                                    int *arg);

typedef int (*_bcm_bst_pre_sync_cb)(int unit, bcm_bst_stat_id_t bid,
             int *sync_val);
typedef int (*_bcm_bst_post_sync_cb)(int unit, bcm_bst_stat_id_t bid,
             int sync_val);

typedef int (*_bcm_bst_intr_to_resources_cb)(int unit, uint32 *flags);
typedef int (*_bcm_bst_hw_trigger_set)(int unit, bcm_bst_stat_id_t bid);
typedef int (*_bcm_bst_hw_trigger_reset)(int unit, bcm_bst_stat_id_t bid);
typedef int (*_bcm_bst_hw_intr_cb)(int unit);
typedef int (*_bcm_bst_hw_sbusdma_desc_init)(int unit);
typedef int (*_bcm_bst_hw_sbusdma_desc_deinit)(int unit);
typedef int (*_bcm_bst_hw_sbusdma_desc_sync)(int unit);
typedef int (*_bcm_bst_intr_enable_set)(int unit, int enable);
typedef int (*_bcm_bst_hw_stat_snapshot)(int unit, bcm_bst_stat_id_t bid,
                                         int port, int index);
typedef int (*_bcm_bst_hw_stat_clear_t)(int unit,
                                        _bcm_bst_resource_info_t *resInfo,
                                        bcm_bst_stat_id_t bid, int port,
                                        int index);
typedef int (*_bcm_bst_port_mmu_inst_map_t)(int unit, bcm_bst_stat_id_t bid,
                                            bcm_gport_t gport,
                                            uint32 *mmu_inst_map);
typedef int (*_bcm_bst_cmn_io_op_t)(int unit, int op, int pipe, int hw_index,
                                    bcm_bst_stat_id_t bid, soc_mem_t mem,
                                    soc_reg_t reg, soc_field_t field,
                                    uint32 *resval);
typedef int (*_bcm_bst_profile_init)(int unit);
typedef int (*_bcm_bst_thd_get)(int unit, int pipe, int xpe, int hw_index, 
                bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile);
typedef int (*_bcm_bst_thd_set)(int unit, int pipe, int xpe, int hw_index, 
                bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile, 
                uint32 *p_profile);

#define _BCM_BST_SYNC_F_ENABLE          0x1 /* Flag to Pause/Run the thread */
#define _BCM_BST_SYNC_F_THREAD_VALID    0x2 /* Flag for existence of thread */

typedef struct _bcm_bst_cmn_unit_info_s {
    uint32                          max_bytes;
    _bcm_bst_byte_to_cell           to_cell;
    _bcm_bst_cell_to_byte           to_byte;
    _bcm_bst_control_set            control_set;
    _bcm_bst_control_get            control_get;
    _bcm_bst_resource_info_t        resource_tbl[bcmBstStatIdMaxCount];
    _bcm_bst_device_handlers_t      handlers;
    _bcm_bst_pre_sync_cb            pre_sync;
    _bcm_bst_post_sync_cb           post_sync;
    _bcm_bst_intr_to_resources_cb   intr_to_resources;
    _bcm_bst_hw_trigger_set         hw_trigger_set;
    _bcm_bst_hw_trigger_reset       hw_trigger_reset;
    _bcm_bst_hw_intr_cb             hw_intr_cb;
    _bcm_bst_hw_sbusdma_desc_init   hw_sbusdma_desc_init;
    _bcm_bst_hw_sbusdma_desc_deinit hw_sbusdma_desc_deinit;
    _bcm_bst_hw_sbusdma_desc_sync   hw_sbusdma_desc_sync;
    _bcm_bst_intr_enable_set        intr_enable_set;
    _bcm_bst_hw_stat_snapshot       hw_stat_snapshot;
    _bcm_bst_hw_stat_clear_t        hw_stat_clear;
    _bcm_bst_port_mmu_inst_map_t    port_to_mmu_inst_map; /* Maps a pipe for
                                                           * given resource to
                                                           * MMU instances */
    _bcm_bst_cmn_io_op_t            bst_cmn_io_op;
    _bcm_bst_profile_init           profile_init;
    _bcm_bst_thd_get                thd_get;
    _bcm_bst_thd_set                thd_set;

    sal_sem_t                       bst_trigger;
    sal_sem_t                       bst_thread;
    int                             sync_flags;
    sal_thread_t                    bst_thread_id;
    int                             interval;
    sal_mutex_t                     bst_reslock;
    int                             mode;
    /*
     * mode = 0, 1 SHARED and 8 HEADROOM
     * mode = 1, 8 SHARED and 1 HEADROOM 
     */
    int                             track_mode;
    int                             snapshot_mode;
    int                             hw_cor_in_max_use_count;
    int                             bst_tbl_size;
    int                             bst_tbl_num;
    uint8                           *bst_tbl_buf;
    _bcm_bst_tbl_desc_t             *bst_tbl_desc;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int                             bst_tracking_enable;
#endif
} _bcm_bst_cmn_unit_info_t;

extern _bcm_bst_cmn_unit_info_t *_bcm_bst_unit_info[BCM_MAX_NUM_UNITS];

#define _BCM_UNIT_BST_INFO(u) _bcm_bst_unit_info[(u)]

#define _BCM_BST_RESOURCE(u,r) (&((_bcm_bst_unit_info[(u)])->resource_tbl[(r)]))

extern sbusdma_desc_handle_t bst_tbl_handles[BCM_MAX_NUM_UNITS];
#define _BCM_UNIT_TBL_HANDLE(u) bst_tbl_handles[(u)]

extern _bcm_bst_tbl_ctrl_t **bst_tbl_ctrl[BCM_MAX_NUM_UNITS];
#define _BCM_UNIT_TBL_CTRL(u) bst_tbl_ctrl[(u)]

#define _BCM_BST_RESOURCE_VALID(pr)     (pr)->valid

#define _BCM_BST_STAT_THRESHOLD_INFO_INIT(pe)        \
    do {                                             \
        int i = 0;                                   \
        for (i = 0; i < _BCM_MAX_PIPES; i++) {       \
            (pe)->stat_mem[i] = (INVALIDm);          \
            (pe)->stat_reg[i] = (INVALIDr);          \
            (pe)->threshold_mem[i] = (INVALIDm);     \
        }                                            \
        for (i = 0; i < _BCM_MAX_THD_REGS; i++) {    \
            (pe)->threshold_reg[i] = (INVALIDr);     \
        }                                            \
        (pe)->stat_field = (INVALIDf);               \
        (pe)->threshold_field = (INVALIDf);          \
        (pe)->threshold_gran = (1);                  \
    } while(0);

#ifdef BCM_TOMAHAWK3_SUPPORT
#define _BCM_BST_STAT_FIELD_INFO(pe, sf, i)\
            (pe)->stat_fields[(i)] = (sf);\
            ((pe)->num_field)++;

#define _BCM_BST_THD_FIELD_INFO(pe, tf, i)\
            (pe)->thd_fields[(i)] = (tf);
#endif
#define _BCM_BST_STAT_INFO(pe,smem,sreg,sf)     \
            (pe)->stat_mem[0] = (smem);         \
            (pe)->stat_reg[0] = (sreg);         \
            (pe)->stat_mem[1] = INVALIDm;       \
            (pe)->stat_reg[1] = INVALIDr;       \
            (pe)->stat_field = (sf);

#define _BCM_BST_STAT_INFO2(pe,smem1,smem2,sreg1,sreg2,sf)     \
            (pe)->stat_mem[0] = (smem1);         \
            (pe)->stat_reg[0] = (sreg1);         \
            (pe)->stat_mem[1] = (smem2);         \
            (pe)->stat_reg[1] = (sreg2);         \
            (pe)->stat_field = (sf);

#define _BCM_BST_STAT_INFO_N(pe,smem,sreg,sf,n) \
            (pe)->stat_mem[n] = (smem);         \
            (pe)->stat_reg[n] = (sreg);         \
            (pe)->stat_field = (sf);

#define _BCM_BST_THRESHOLD_INFO(pe,pmem,preg,pf,gran)       \
            (pe)->threshold_mem[0] = (pmem);                \
            (pe)->threshold_reg[0] = (preg);                \
            (pe)->threshold_mem[1] = INVALIDm;              \
            (pe)->threshold_reg[1] = INVALIDr;              \
            (pe)->threshold_field = (pf);                   \
            (pe)->threshold_gran = (gran);

#define _BCM_BST_THRESHOLD_INFO2(pe,pmem1,pmem2,preg1,preg2,pf,gran) \
            (pe)->threshold_mem[0] = (pmem1);                        \
            (pe)->threshold_reg[0] = (preg1);                        \
            (pe)->threshold_mem[1] = (pmem2);                        \
            (pe)->threshold_reg[1] = (preg2);                        \
            (pe)->threshold_field = (pf);                            \
            (pe)->threshold_gran = (gran);

#define _BCM_BST_THRESHOLD_INFO_N(pe,pmem,preg,pf,gran,n)       \
            (pe)->threshold_mem[n%_BCM_MAX_PIPES] = (pmem);     \
            (pe)->threshold_reg[n]  = (preg);                   \
            (pe)->threshold_gran    = (gran);                   \
            (pe)->threshold_field   = (pf);

#define _BCM_BST_PROFILE_INFO(pe,pmem,preg,pf)       \
            (pe)->profile_mem    = (pmem);           \
            (pe)->profile_reg[0] = (preg);           \
            (pe)->profile_field  = (pf);

#define _BCM_BST_PROFILE_INFO_N(pe,pmem,preg,pf,n)   \
            (pe)->profile_mem    = (pmem);           \
            (pe)->profile_reg[n] = (preg);           \
            (pe)->profile_field  = (pf);

#define _BCM_BST_CMN_RES_F_PROFILEID    0x01
#define _BCM_BST_CMN_RES_F_PIPED        0x02
/* Following flags for Resource Classification (Dev/Ing/Egr) */
#define _BCM_BST_CMN_RES_F_RES_DEV      0x04
#define _BCM_BST_CMN_RES_F_RES_ING      0x08
#define _BCM_BST_CMN_RES_F_RES_EGR      0x10
/* Following flags for special bst process */
#define _BCM_BST_CMN_RES_F_PORTED       0x20

#define _BCM_BST_RESOURCE_FLAG_SET(pe,rflags)        \
            (pe)->flags = (rflags);                  \
            (pe)->valid = 1;

#define _BCM_BST_RESOURCE_STAT_INIT(pres, num, rname)                       \
    do {                                                                    \
        if ((pres)->p_stat == NULL) {                                       \
            (pres)->p_stat = sal_alloc((num)*sizeof(uint32), (rname));      \
            if ((pres)->p_stat == NULL) {                                   \
                return (BCM_E_MEMORY);                                      \
            }                                                               \
        }                                                                   \
        sal_memset((pres)->p_stat, 0, sizeof(uint32)*(num));                \
        (pres)->num_stat_pp = (num);                                        \
    } while (0);

#define _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, num, rname)                  \
    do {                                                                    \
        if ((pres)->p_threshold == NULL) {                                  \
            (pres)->p_threshold = sal_alloc((num)*sizeof(uint32), (rname)); \
            if ((pres)->p_threshold == NULL) {                              \
                return (BCM_E_PARAM);                                       \
            }                                                               \
        }                                                                   \
        sal_memset((pres)->p_threshold, 0, (num)*sizeof(uint32));           \
    } while (0);

#define _BCM_BST_RESOURCE_PBMP_SET(pe,pbm)      (pe)->pbmp = (pbm)

#define _BCM_BST_RESOURCE_MIN       bcmBstStatIdDevice
#define _BCM_BST_RESOURCE_MAX       bcmBstStatIdMaxCount

#define _BCM_BST_RESOURCE_ITER(bi, bid)             \
        for (bid = _BCM_BST_RESOURCE_MIN;           \
             bid < _BCM_BST_RESOURCE_MAX; bid++)

#define _BCM_BST_SYNC_ENABLED(bi)   \
        (((bi)->sync_flags & _BCM_BST_SYNC_F_ENABLE) ? 1 : 0)

#define _BCM_BST_SYNC_THREAD_VALID(bi)   \
        (((bi)->sync_flags & _BCM_BST_SYNC_F_THREAD_VALID) ? 1 : 0)

#define _BCM_BST_SYNC_THREAD_ENABLE_SET(u,status)        \
    if (status == 1) {                               \
        _bcm_bst_unit_info[(u)]->sync_flags |= _BCM_BST_SYNC_F_ENABLE; \
    } else { \
        _bcm_bst_unit_info[(u)]->sync_flags &= ~_BCM_BST_SYNC_F_ENABLE; \
    }
    
/* Set BST threshold to Max High(0x1ffff) when BST function is disabled */
#define BST_THRESHOLD_MAX_HIGH       0x1ffff
#define BST_THRESHOLD_INVALID       0xffffffff

extern int _bcm_bst_sync_thread_start(int unit);
extern int _bcm_bst_sync_thread_stop(int unit);
extern int _bst_intr_enable_set(int unit, int enable);
extern int _bcm_bst_hw_event_cb(int unit);
extern int _bst_intr_status_reset(int unit);
extern int _bcm_bst_sync_hw_trigger(int unit, bcm_bst_stat_id_t bid,
                                    int port, int index);
extern int _bcm_bst_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                                     int port, int index);
extern int _bcm_bst_hw_stat_clear(int unit,
                                  _bcm_bst_resource_info_t *resInfo,
                                  bcm_bst_stat_id_t bid, int port, int index);
extern int soc_bst_sbusdma_desc_setup(int unit);
extern int soc_bst_sbusdma_desc_free(int unit);
extern int soc_bst_hw_sbusdma_desc_sync(int unit);


#if defined(BCM_TOMAHAWK_SUPPORT)
#define _BCM_BST_TH_CPU_QUEUE_START 330
#define _BCM_BST_TH_IS_CPU_QUEUE(q) \
            (330 <= q && q <= 377)

extern int bcm_bst_th_init(int unit);
extern int _bcm_th_bst_sync_thread_restore(int unit);
extern int _bcm_th_bst_info_restore(int unit);
extern int _bcm_th_bst_resource_threshold_restore(int unit);
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
extern int bcm_bst_td3_init(int unit);
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
extern int bcm_bst_th3_init(int unit);
extern _bcm_bst_cb_ret_t
bcm_bst_th3_index_resolve(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_bst_stat_id_t bid, int *pipe, int *start_hw_index,
                          int *end_hw_index, int *rcb_data, void **cb_data, int *bcm_rv);

int bcm_th3_cosq_headroom_pool_get(int unit, bcm_gport_t gport,
                                                 int priority, int *pool_id);

int
bcm_th3_cosq_ing_service_pool_get(int unit, bcm_port_t port,
                                                 int pg, int *pool_id);
int bcm_th3_cosq_bst_stat_clear(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq, bcm_cosq_buffer_id_t buffer,
                               bcm_bst_stat_id_t bid);


int
bcm_th3_cosq_bst_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_bst_stat_id_t bid,
    uint32 options,
    uint64 *pvalue);

int
bcm_th3_cosq_bst_stat_multi_get(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               uint32 options,
                               int max_values,
                               bcm_bst_stat_id_t *id_list,
                               uint64 *pvalues);
int
bcm_bst_th3_sw_stat_clear(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_cosq_buffer_id_t buffer, bcm_bst_stat_id_t bid);
int
bcm_bst_th3_hw_stat_clear(int unit, _bcm_bst_resource_info_t *resInfo,
                bcm_bst_stat_id_t bid, bcm_cosq_buffer_id_t buffer,
                                                    int port, int index,
                                                    int cosq);

extern int
_bcm_th3_bst_cmn_profile_set (int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        int buffer, bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile);

extern int
_bcm_th3_bst_cmn_profile_get (int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        int buffer, bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile);

extern int
_bcm_th3_cosq_bst_map_resource_to_gport_cos(int unit,
    bcm_bst_stat_id_t bid, int port, int index, bcm_gport_t *gport,
    bcm_cos_t *cosq);
 
extern int _bcm_th3_bst_info_restore(int unit);
#endif /* BCM_TOMAHAWK3_SUPPORT */

#endif /* _BCM_BST_H */

