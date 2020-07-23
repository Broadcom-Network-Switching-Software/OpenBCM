/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: Regex API for flow tracking
 */
#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/defs.h>
#if defined(BCM_TRIUMPH3_SUPPORT)

#if defined(INCLUDE_REGEX)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>

#include <soc/triumph3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm/error.h>
#include <bcm/bregex.h>
#include <bcm_int/regex_api.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/policer.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/common/multicast.h>

#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/bregex.h>
#include <bcm_int/esw/fb4regex.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define BCM_TR3_MAX_ENGINE                  32
#define BCM_TR3_MAX_MATCH_MEM               4
#define BCM_TR3_MAX_NUM_MATCH               256
#define BCM_TR3_MAX_SLOTS                   512
#define BCM_TR3_MAX_TILES                   8

#define BCM_TR3_LINE_BYTE_SIZE              32
#define BCM_TR3_NUM_RV_PER_LINE             8
#define BCM_TR3_RV_STATE_SIZE               (BCM_TR3_LINE_BYTE_SIZE / BCM_TR3_NUM_RV_PER_LINE)

/* Maximum number of flow */
#define BCM_TR3_MAX_FLOWS   8192
#define BCM_TR3_IN_USE_MASK          0x00003FFF
#define BCM_TR3_IN_USE_MASK_SIZE     14
#define BCM_TR3_MAX_IN_USE_MASK      0x0FFFC000
#define BCM_TR3_MAX_IN_USE_MASK_SIZE 14

/* maximum payload depth */
#define BCM_TR3_MAX_PAYLOAD_DEPTH   16383

/* maximum packets to SME */
#define BCM_TR3_MAX_PKT_TO_SME   15

#define BCM_REGEX_ENG2MATCH_MEMIDX(u,e)    ((e)/8)

#define BCM_REGEX_NUM_SLOT_PER_CPS 16

#define BCM_TR3_SPECIAL_SLOT 0x2012

typedef struct _bcm_regex_cps_obj_s {
    uint32    flags;
    int             foff;
    int             alloc_map[BCM_REGEX_NUM_SLOT_PER_CPS];
    int             refcnt[BCM_REGEX_NUM_SLOT_PER_CPS];
    int             from_line;
    int             req_lsz;
} _bcm_regex_cps_obj_t;

#define BCM_REGEX_CPS_F_ALLOC     1

#define BCM_REGEX_CPS_ALLOCED(pc) ((pc)->flags & BCM_REGEX_CPS_F_ALLOC)

#define BCM_REGEX_CPS_SET_ALLOCED(pc) (pc)->flags |= BCM_REGEX_CPS_F_ALLOC

#define BCM_REGEX_CPS_FREE_ALLOCED(pc) (pc)->flags &= ~BCM_REGEX_CPS_F_ALLOC

#define BCM_TR3_MAX_SIG_ID  1024

#define BCM_TR3_SIG_ID_MAP_BSZ   ((1024+31)/32)

#define BCM_TR3_POLICER_NUM     256

STATIC int _bcm_tr3_free_cps(int unit, _bcm_regex_cps_obj_t *pcps);

typedef struct _bcm_regex_engine_obj_s {
    int                     hw_idx;
    uint32            flags;
    int                     total_lsz;  
    /* HW mapping */
    int                     tile;
    int                     from_line;
    int                     req_lsz;
    _bcm_regex_cps_obj_t    cps;
} _bcm_regex_engine_obj_t;

#define BCM_TR3_REGEX_ENGINE_ALLOC          0x01
#define BCM_REGEX_ENGINE_ALLOCATED(eng) \
                        ((eng)->flags & BCM_TR3_REGEX_ENGINE_ALLOC)

#define BCM_REGEX_ENGINE_SET_ALLOCATED(eng) \
                         (eng)->flags |= BCM_TR3_REGEX_ENGINE_ALLOC

#define BCM_REGEX_ENGINE_SET_UNALLOCATED(eng) \
                         (eng)->flags &= ~BCM_TR3_REGEX_ENGINE_ALLOC

typedef struct _bcm_regex_lengine_s {
    bcm_regex_engine_t      id;
    uint32                  flags;
    bcm_regex_engine_config_t config;
    _bcm_regex_engine_obj_t *physical_engine;
} _bcm_regex_lengine_t;

typedef int (*reset_engine)(int unit, int engine_idx);
typedef int (*enable_engine)(int unit, _bcm_regex_engine_obj_t *pengine);
typedef int (*get_engine_info)(int unit, int engine_idx, 
                                            _bcm_regex_engine_obj_t *e);
typedef int (*disable_engine)(int unit, int engine_idx);
typedef uint32 (*l2pf)(int line, int off);

typedef struct bcm_regex_desc_s {
    int                 num_tiles;
    int                 num_engine_per_tile;
    int                 num_match_mem;
    int                 line_sz;
    uint32              flags;
    reset_engine        reset_engine;
    get_engine_info     get_engine_info;
    enable_engine       enable_engine;
    disable_engine      disable_engine;
} bcm_regex_desc_t;

#define BCM_REGEX_RESET_ENGINE(d, u, e) (d)->info->reset_engine((u), (e))

#define BCM_REGEX_GET_ENGINE_INFO(d, u, pe, po) \
                                    (d)->info->get_engine_info((u), (pe), (po))

#define BCM_REGEX_ENABLE_ENGINE(d, u, pe) (d)->info->enable_engine((u), (pe))
#define BCM_REGEX_DISABLE_ENGINE(d, u, e) (d)->info->disable_engine((u), (e))

#define BCM_TR3_MATCH_BMAP_BSZ  ((BCM_TR3_MAX_NUM_MATCH+31)/32)

#define BCM_REGEX_DB_BUCKET_MAX 32

STATIC int _bcm_tr3_alloc_regex_engine(int unit, int clsz, int extra_lsz,
                            _bcm_regex_engine_obj_t **ppeng, int intile);

struct bcm_regexdb_entry_s;

typedef struct _bcm_tr3_regex_device_info_s {
    int max_engines;

    bcm_regex_desc_t *info;

    _bcm_regex_engine_obj_t physical_engines[BCM_TR3_MAX_ENGINE];

    /* logical engines table */
    _bcm_regex_lengine_t engines[BCM_TR3_MAX_ENGINE];

    uint32      match_alloc_bmap[BCM_TR3_MAX_MATCH_MEM][BCM_TR3_MATCH_BMAP_BSZ];

    struct bcm_regexdb_entry_s *l[BCM_REGEX_DB_BUCKET_MAX];

    sal_mutex_t regex_mutex;

    uint32      sigid[BCM_TR3_SIG_ID_MAP_BSZ];    

    bcm_stat_flex_pool_attribute_t flex_pools[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    uint32      policer_bmp[BCM_TR3_POLICER_NUM/sizeof(uint32)];
    bcm_policer_config_t policer_cfg[BCM_TR3_POLICER_NUM];

    _regex_policy_t **policies;
    int               num_policies;
} _bcm_tr3_regex_device_info_t;

#define _BCM_TR3_FLEX_CTR_POOL_VALID(p) (((p) == -1) ? 0 : 1)

static _bcm_tr3_regex_device_info_t *_bcm_tr3_regex_info[BCM_MAX_NUM_UNITS];

static soc_profile_mem_t *_bcm_tr3_ft_policy_profile[BCM_MAX_NUM_UNITS];

#define REGEX_INFO(u) _bcm_tr3_regex_info[(u)]

#define BCM_TR3_NUM_ENGINE(d)   ((d)->max_engines)

#define BCM_TR3_LOGICAL_ENGINE(d,leid)  \
  ((leid) >= BCM_TR3_NUM_ENGINE((d))) ? NULL : &(d)->engines[(leid)]

#define BCM_TR3_PHYSICAL_ENGINE(d,peid) \
  ((peid) >= (BCM_TR3_NUM_ENGINE((d)))) ? NULL : &(d)->physical_engines[(peid)]

#define BCM_TR3_ENGINE_PER_TILE(d)  ((d)->info->num_engine_per_tile)

#define BCM_TR3_ENGINE_TILE(d,peid) \
  ((peid) >= (BCM_TR3_NUM_ENGINE((d)))) ? 0 : ((peid) / BCM_TR3_ENGINE_PER_TILE(d))

#define BCM_TR3_CPS_INFO(d,peid)    &((d)->physical_engines[(peid)].cps)

#define BCM_TR3_MATCH_BMAP_PTR(u,d,peid)    \
            (d)->match_alloc_bmap[BCM_REGEX_ENG2MATCH_MEMIDX((u),(peid))]

typedef struct bcm_regexdb_entry_s {
    int         sigid;
    int         engid;
    int         match_id;
    uint16      match_index;
    int         provides;
    int         requires;
    short int   match_set_id;
    short int   match_chk_id;
    struct bcm_regexdb_entry_s *next;
} _bcm_regexdb_entry_t;

typedef struct _bcm_tr3_regex_data_s {
    int                     unit;
    uint32                  flags;
    int                     compile_size;
    /* HW engine information */
    _bcm_regex_engine_obj_t *hw_eng;
    bcm_regex_match_t       *matches;
    int                     num_match;
    int                     *match_set_ids;
    int                     *match_check_ids;
    int                     *sig_ids;
    int                     *hw_match_idx;

    /* temp information to compile to HW */
    int                     last_state;
    uint32                  last_flags;
    uint8                   cmap[REGEX_MAX_CHARACTER_CLASS_BYTES];
    soc_mem_t               mem;
    axp_sm_state_table_mem0_entry_t line;
   
    uint16                  *per_state_data;
    uint16                  wr_off;
    uint16                  line_off;
    int                     line_dirty;
} _bcm_tr3_regex_data_t;

typedef struct _bcm_regex_resource_s {
    _bcm_regex_engine_obj_t *pres;
    int     eng_idx;
    int     lsize;
} _bcm_regex_resource_t;

STATIC int
_bcm_regex_sort_resources_by_size(int unit,
                                  _bcm_regex_resource_t *res, int num_res);

STATIC int
_bcm_regex_sort_resources_by_offset(int unit, 
                                  _bcm_regex_resource_t *res, int num_res);

typedef struct _bcm_regex_tile_s {
    int valid;
    int freep;
    int avail;
    int oidx[BCM_TR3_MAX_ENGINE]; 
    int num_o;
    axp_sm_match_table_mem0_entry_t *match_mem;
} _bcm_regex_tile_t;

STATIC int
_bcm_tr3_arrange_resources_in_tiles(int unit, _bcm_regex_resource_t *res, 
                                    int num_res, int to_tiles, int to_tilee,
                                    _bcm_regex_tile_t *_tr3tile, int num_tile);

STATIC int
_bcm_tr3_rearrange_engines_in_hw(int unit, _bcm_regex_resource_t *res, 
                           int num_res, int to_tiles, int to_tilee, 
                           _bcm_regex_tile_t *_tr3tile);

STATIC int
_bcm_tr3_regex_alloc_set_ids(int unit, int *match_set_ids, bcm_regex_match_t *matches,
                            int count, int pengine_hwidx);

STATIC int
_bcm_regexdb_handle_setid_avail(int unit, int avail, int engid,
                                int provides, int setid);

STATIC int _regex_tr3_policy_index_min_get(int unit);
STATIC int _regex_tr3_policy_index_max_get(int unit);
STATIC int _regex_tr3_policy_action_support_check(int                 unit,
                                                  bcm_field_action_t  action);
STATIC int _regex_tr3_policy_action_conflict_check(int                unit,
                                                   bcm_field_action_t action1,
                                                   bcm_field_action_t action);
STATIC int _regex_tr3_policy_action_params_check(int                     unit,
                                                 _regex_policy_t        *p_ent,
                                                 _regex_policy_action_t *pa);
STATIC int _regex_tr3_policy_install(int unit, _regex_policy_t *p_ent);
STATIC int _regex_tr3_policy_remove(int unit, _regex_policy_t *p_ent);
STATIC int _regex_tr3_policer_install(int unit, _field_policer_t *f_pl);
STATIC int _regex_tr3_policy_redirect_profile_alloc(int unit, _regex_policy_t *p_ent,
                                                    _regex_policy_action_t *pa);
STATIC int _regex_tr3_policy_redirect_profile_ref_count_get(int unit, int index, int *ref_count);
STATIC int _regex_tr3_policy_redirect_profile_delete(int unit, int index);

STATIC _bcm_esw_regex_functions_t _regex_tr3_functions =
{
    _regex_tr3_policy_index_min_get,
    _regex_tr3_policy_index_max_get,
    _regex_tr3_policy_action_support_check,
    _regex_tr3_policy_action_conflict_check,
    _regex_tr3_policy_action_params_check,
    _regex_tr3_policy_install,
    _regex_tr3_policy_remove,
    _regex_tr3_policer_install,
    _regex_tr3_policy_redirect_profile_alloc,
    _regex_tr3_policy_redirect_profile_delete,
    _regex_tr3_policy_redirect_profile_ref_count_get,
};

STATIC int _regex_tr3_policer_action_set(int              unit,
                                         _regex_policy_t *p_ent,
                                         uint32          *policy_buf);

STATIC int _bcm_tr3_alloc_sigid(int unit, bcm_regex_match_t *match)
{
    _bcm_tr3_regex_device_info_t *device;
    int     i, j;

    device = REGEX_INFO(unit);

    for (i=0; i<BCM_TR3_SIG_ID_MAP_BSZ; i++) {
        if (device->sigid[i] == 0) {
            continue;
        }
        for (j = 0; j < 32; j++) {
            if (device->sigid[i] & (1 << j)) {
                device->sigid[i] &= ~(1 << j);
                return ((i*32) + j);
            }
        }
    }
    return -1;
}

STATIC int
_bcm_tr3_free_sigid(int unit, int sigid)
{
    _bcm_tr3_regex_device_info_t *device;

    if ((sigid < 0) || (sigid >= BCM_TR3_MAX_SIG_ID)) {
        return BCM_E_PARAM;
    }

    device = REGEX_INFO(unit);
    device->sigid[sigid/32] |= 1 << (sigid % 32);
    return BCM_E_NONE;
}

typedef struct _bcm_regexdb_cb_ua_s {
    int avail;
    int setid;
} _bcm_regexdb_cb_ua_t;

/* locking */
STATIC int _bcm_tr3_regex_lock (int unit)
{
    _bcm_tr3_regex_device_info_t *device;

    if ((device = REGEX_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    
    sal_mutex_take(device->regex_mutex, sal_mutex_FOREVER);
    return BCM_E_NONE; 
}

STATIC void _bcm_tr3_regex_unlock(int unit)
{
    _bcm_tr3_regex_device_info_t *device;

    device = REGEX_INFO(unit);
    sal_mutex_give(device->regex_mutex);
}

#define BCM_REGEX_LOCK(u) _bcm_tr3_regex_lock((u))

#define BCM_REGEX_UNLOCK(u) _bcm_tr3_regex_unlock((u))


/********** db *******/

#define BCM_REGEX_DB_MATCH_BUCKET(matchidx) \
                            ((matchidx) % BCM_REGEX_DB_BUCKET_MAX)

#define BCM_REGEXDB_MATCH_SIG_ID            0x1
#define BCM_REGEXDB_MATCH_ENGINE_ID         0x2
#define BCM_REGEXDB_MATCH_MATCH_ID          0x4
#define BCM_REGEXDB_MATCH_PROVIDES          0x10
#define BCM_REGEXDB_MATCH_REQUIRES          0x20

STATIC _bcm_regexdb_entry_t**
_bcm_regexdb_find(int unit, uint32 match_flags, _bcm_regexdb_entry_t *a)
{
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regexdb_entry_t **p, **res =  NULL;
    int i, from, to, found = 0;
    uint32 tmpf = match_flags;

    if ((match_flags & BCM_REGEXDB_MATCH_SIG_ID)) {
        from = to = BCM_REGEX_DB_MATCH_BUCKET(a->sigid);
    } else {
        from = 0;
        to = BCM_REGEX_DB_BUCKET_MAX - 1;
    }

    device = REGEX_INFO(unit);

    for (i = from; !found && (i <= to); i++) {
        p = &device->l[i];
        while (*p) {
            tmpf = match_flags;
            if ((match_flags & BCM_REGEXDB_MATCH_SIG_ID) &&
                (a->sigid == (*p)->sigid)) {
                tmpf &= ~BCM_REGEXDB_MATCH_SIG_ID;
            }
            if ((match_flags & BCM_REGEXDB_MATCH_ENGINE_ID) &&
                (a->engid == (*p)->engid)) {
                tmpf &= ~BCM_REGEXDB_MATCH_ENGINE_ID;
            }
        #if 0
            if ((match_flags & BCM_REGEXDB_MATCH_MATCH_INDEX) &&
                (a->match_index == (*p)->match_index)) {
                tmpf &= ~BCM_REGEXDB_MATCH_MATCH_INDEX;
            }
        #endif
            if ((match_flags & BCM_REGEXDB_MATCH_MATCH_ID) &&
                (a->match_id == (*p)->match_id)) {
                tmpf &= ~BCM_REGEXDB_MATCH_MATCH_ID;
            }
            if ((match_flags & BCM_REGEXDB_MATCH_PROVIDES) &&
                (a->provides == (*p)->provides)) {
                tmpf &= ~BCM_REGEXDB_MATCH_PROVIDES;
            }
            if ((match_flags & BCM_REGEXDB_MATCH_REQUIRES) &&
                (a->requires == (*p)->requires)) {
                tmpf &= ~BCM_REGEXDB_MATCH_REQUIRES;
            }

            /* match */
            if (tmpf == 0) {
                found = 1;
                res = p;
                break;
            }
            p = &((*p)->next);
        }
    }

    return found ? res : NULL;
}

typedef int (*regexdb_traverse)(int unit, _bcm_regexdb_entry_t *e, void *ua);

STATIC int
_bcm_regexdb_traverse(int unit, uint32 match_flags, _bcm_regexdb_entry_t *a,
                      regexdb_traverse cbf, void *ua)
{
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regexdb_entry_t **p;
    int i, from, to, skip;

    if ((match_flags & BCM_REGEXDB_MATCH_SIG_ID)) {
        from = to = BCM_REGEX_DB_MATCH_BUCKET(a->sigid);
    } else {
        from = 0;
        to = BCM_REGEX_DB_BUCKET_MAX - 1;
    }
    
    device = REGEX_INFO(unit);

    for (i = from; i <= to; i++) {
        p = &device->l[i];
        while (*p) {
            skip = 0;
            if (!skip && (match_flags & BCM_REGEXDB_MATCH_SIG_ID) &&
                (a->sigid != (*p)->sigid)) {
                skip = 1;
            }
            if (!skip && (match_flags & BCM_REGEXDB_MATCH_ENGINE_ID) &&
                (a->engid != (*p)->engid)) {
                skip = 1;
            }
            #if 0
            if (!skip && (match_flags & BCM_REGEXDB_MATCH_MATCH_INDEX) &&
                (a->match_index != (*p)->match_index)) {
                skip = 1;
            }
            #endif
            if (!skip && (match_flags & BCM_REGEXDB_MATCH_MATCH_ID) &&
                (a->match_id != (*p)->match_id)) {
                skip = 1;
            }
            if (!skip && (match_flags & BCM_REGEXDB_MATCH_PROVIDES) &&
                (a->provides != (*p)->provides)) {
                skip = 1;
            }
            if (!skip && (match_flags & BCM_REGEXDB_MATCH_REQUIRES) &&
                (a->requires != (*p)->requires)) {
                skip = 1;
            }

            /* match */
            if (!skip) {
                cbf(unit, *p, ua);
            }
            p = &((*p)->next);
        }    
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_regexdb_delete_cmn(int unit, uint32 match_flags, _bcm_regexdb_entry_t *a)
{
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regexdb_entry_t **p, *pn;
    int i, from, to, del, num_del = 0;

    if ((match_flags & BCM_REGEXDB_MATCH_SIG_ID)) {
        from = to = BCM_REGEX_DB_MATCH_BUCKET(a->match_index);
    } else {
        from = 0;
        to = BCM_REGEX_DB_BUCKET_MAX - 1;
    }
    
    device = REGEX_INFO(unit);

    for (i = from; i <= to; i++) {
        p = &device->l[i];
        while (*p) {
            del = 1;
            if (del && (match_flags & BCM_REGEXDB_MATCH_SIG_ID) &&
                (a->sigid != (*p)->sigid)) {
                del = 0;
            }
            if (del && (match_flags & BCM_REGEXDB_MATCH_ENGINE_ID) &&
                (a->engid != (*p)->engid)) {
                del = 0;
            }
            #if 0
            if ((match_flags & BCM_REGEXDB_MATCH_MATCH_INDEX) &&
                (a->match_index != (*p)->match_index)) {
                del = 0;
            }
            #endif
            if (del && (match_flags & BCM_REGEXDB_MATCH_MATCH_ID) &&
                (a->match_id != (*p)->match_id)) {
                del = 0;
            }
            if (del & (match_flags & BCM_REGEXDB_MATCH_PROVIDES) &&
                (a->provides != (*p)->provides)) {
                del = 0;
            }
            if (del && (match_flags & BCM_REGEXDB_MATCH_REQUIRES) &&
                (a->requires != (*p)->requires)) {
                del = 0;
            }

            if (del) {
                pn = *p;
                *p = pn->next;
                sal_free(pn);
                num_del++;
            } else {
                p = &((*p)->next);
            }
        }    
    }
    return !num_del;
}

/*
 * Mark all the match entries that wre in pending state because on
 * unavailability of setid, to enable state.
 */
STATIC int
_bcm_regexdb_setid_avail_cb(int unit, _bcm_regexdb_entry_t *en, void *vua) 
{
    soc_mem_t mem;
    axp_sm_match_table_mem0_entry_t match_entry;
    _bcm_regexdb_cb_ua_t *ua = (_bcm_regexdb_cb_ua_t*) vua;

    mem = AXP_SM_MATCH_TABLE_MEM0m + BCM_REGEX_ENG2MATCH_MEMIDX(unit,en->engid);

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                          MEM_BLOCK_ALL, en->match_index, &match_entry));
    if (soc_mem_field32_get(unit, mem, &match_entry, MATCH_ENTRY_VALIDf) == 0) {
        return BCM_E_NONE;
    }
    if (ua->avail) {
        soc_mem_field32_set(unit, mem, 
                        &match_entry, CHECK_CROSS_SIG_FLAG_IS_SETf, 1);
        soc_mem_field32_set(unit, mem, 
                        &match_entry, CHECK_CROSS_SIG_FLAG_INDEXf, ua->setid);
    } else {
        soc_mem_field32_set(unit, mem, 
                        &match_entry, CHECK_CROSS_SIG_FLAG_IS_SETf, 0);
        en->match_chk_id = BCM_TR3_SPECIAL_SLOT;
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, 
                              MEM_BLOCK_ALL, en->match_index, &match_entry));
    return BCM_E_NONE;
}

STATIC int
_bcm_regexdb_handle_setid_avail(int unit, int avail, int engid,
                                int provides, int setid)
{
    _bcm_regexdb_entry_t en;
    _bcm_regexdb_cb_ua_t ua;

    en.requires = provides;
    ua.avail = avail;
    ua.setid = setid;

    return _bcm_regexdb_traverse(unit, BCM_REGEXDB_MATCH_REQUIRES, 
                                 &en, _bcm_regexdb_setid_avail_cb, (void*) &ua);
}

STATIC int
_bcm_regexdb_engine_unavail_cb(int unit, _bcm_regexdb_entry_t *en, void *vua) 
{ 
    _bcm_tr3_regex_device_info_t *device;
    axp_sm_match_table_mem0_entry_t match_entry;
    soc_mem_t mem;
    uint32 *alloc_bmap;

    device = REGEX_INFO(unit);

    if (en->match_set_id >= 0) {
        _bcm_regexdb_handle_setid_avail(unit, 0, en->engid, 
                                        en->provides, en->match_set_id);
    }

    /* delete the match index */
    mem = AXP_SM_MATCH_TABLE_MEM0m + BCM_REGEX_ENG2MATCH_MEMIDX(unit,en->engid);

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                          MEM_BLOCK_ALL, en->match_index, &match_entry));
    soc_mem_field32_set(unit, mem, &match_entry, MATCH_ENTRY_VALIDf, 0);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, 
                              MEM_BLOCK_ALL, en->match_index, &match_entry));

    /* clear the alloc bitmap */
    alloc_bmap = BCM_TR3_MATCH_BMAP_PTR(unit,device, en->engid);

    alloc_bmap[en->match_index/32] &= ~(1 << (en->match_index % 32));

    _bcm_tr3_free_sigid(unit, en->sigid);

    return BCM_E_NONE;
}

STATIC int
bcm_regexdb_find_chkid_for_requires(int unit, int requires, int *slot)
{
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_cps_obj_t *pcps;
    int i, j;

    device = REGEX_INFO(unit);
    for (i = 0; i < BCM_TR3_NUM_ENGINE(device); i++) {
        pcps = BCM_TR3_CPS_INFO(device, i);
        if (!BCM_REGEX_CPS_ALLOCED(pcps)) {
            continue;
        }

        for (j=0; j<BCM_REGEX_NUM_SLOT_PER_CPS; j++) {
            if (pcps->alloc_map[j] == requires) {
                *slot = pcps->foff + j;
                return BCM_E_NONE;
            }
        }
    }
    return BCM_E_NOT_FOUND;
}

STATIC _bcm_regexdb_entry_t*
bcm_regexdb_find_match_entry_by_sig_id(int unit, int sigid)
{
    _bcm_regexdb_entry_t e, **ppe;
    e.sigid = sigid;
    ppe = _bcm_regexdb_find(unit, BCM_REGEXDB_MATCH_SIG_ID, &e);
    return ppe ? *ppe : NULL;
}

STATIC _bcm_regexdb_entry_t*
bcm_regexdb_find_match_entry_by_match_id(int unit, int match_id)
{
    _bcm_regexdb_entry_t e, **ppe;
    e.match_id = match_id;
    ppe = _bcm_regexdb_find(unit, BCM_REGEXDB_MATCH_MATCH_ID, &e);
    return ppe ? *ppe : NULL;
}

#if 0
STATIC int
bcm_regexdb_delete_by_match_id(int unit, int match_id)
{
    _bcm_regexdb_entry_t en;
    en.match_id = match_id;

    return _bcm_regexdb_delete_cmn(unit, BCM_REGEXDB_MATCH_MATCH_ID, &en);
}
#endif

STATIC int
bcm_regexdb_delete_by_engine_id(int unit, int engid)
{
    _bcm_regexdb_entry_t en;
    en.engid = engid;
    return _bcm_regexdb_delete_cmn(unit, BCM_REGEXDB_MATCH_ENGINE_ID, &en);
}

STATIC int
bcm_regexdb_add_entry(int unit, int match_idx, int eng_idx, 
                      int match_id, int sigid, int provides, int requires,
                      int match_set_id, int match_chk_id)
{
    _bcm_regexdb_entry_t *en, **ppen;
    _bcm_tr3_regex_device_info_t *device;

    en = bcm_regexdb_find_match_entry_by_match_id(unit, match_id);
    if (en) {
        return BCM_E_EXISTS;
    }

    en = sal_alloc(sizeof(_bcm_regexdb_entry_t), "regx_db-entry");
    en->sigid = sigid;
    en->match_index = match_idx;
    en->match_id = match_id;
    en->provides = provides;
    en->requires = requires;
    en->match_set_id = match_set_id;
    en->match_chk_id = match_chk_id;
    en->engid = eng_idx;
    en->next = NULL;

    device = REGEX_INFO(unit);

    ppen = &device->l[BCM_REGEX_DB_MATCH_BUCKET(sigid)];
    while(*ppen) {
        ppen = &(*ppen)->next;
    }

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "signature %d added (match_index=%d engid=%d provides=%d \
                         requires=%d match_set_id=%d match_chk_id=%d)\n"),
              sigid, match_idx, eng_idx, provides, requires, match_set_id, match_chk_id));

    *ppen = en;
    return 0;
}

/*
 * Compare the objects.
 */
STATIC int _bcm_regex_object_cmp_size(void*a, void*b)
{
    _bcm_regex_resource_t *oa, *ob;

    oa = (_bcm_regex_resource_t*)a;
    ob = (_bcm_regex_resource_t*)b;

    return ob->pres->total_lsz - oa->pres->total_lsz;
}

STATIC int _bcm_regex_object_cmp_offset(void*a, void*b)
{
    _bcm_regex_resource_t *oa, *ob;
    _bcm_regex_engine_obj_t *eng_a, *eng_b;

    oa = (_bcm_regex_resource_t*)a;
    ob = (_bcm_regex_resource_t*)b;

    eng_a = (_bcm_regex_engine_obj_t*)oa->pres;
    eng_b = (_bcm_regex_engine_obj_t*)ob->pres;

    if (eng_a->tile != eng_b->tile) {
        /* if not in same tile, return a big bumber */
        return (eng_a->tile - eng_b->tile)*0x072011;
    }

    return eng_a->from_line - eng_b->from_line;
}

STATIC int
_bcm_regex_sort_resources_by_size(int unit,
                                  _bcm_regex_resource_t *res, int num_res)
{
    _shr_sort(res, num_res, sizeof(_bcm_regex_resource_t), 
                        _bcm_regex_object_cmp_size);
    return BCM_E_NONE;
}

STATIC int
_bcm_regex_sort_resources_by_offset(int unit, 
                                  _bcm_regex_resource_t *res, int num_res)
{
    _shr_sort(res, num_res, sizeof(_bcm_regex_resource_t), 
                        _bcm_regex_object_cmp_offset);
    return BCM_E_NONE;
}

STATIC int
_bcm_tr3_make_resource_objs(int unit, _bcm_regex_resource_t **ppres,
                                    int *pnum_res, int tiles, int tilee)
{
    _bcm_tr3_regex_device_info_t *device;
    int i, j, nept, count;
    _bcm_regex_resource_t *pres;
    _bcm_regex_engine_obj_t *pengine = NULL;
    
    *ppres = NULL;
    *pnum_res = 0;

    device = REGEX_INFO(unit);

    nept = BCM_TR3_ENGINE_PER_TILE(device);

    for (count=0, i=tiles; i<=tilee; i++) {
        for (j=0; j<nept; j++) {
            pengine = BCM_TR3_PHYSICAL_ENGINE(device, (i*nept) + j);
            if (!BCM_REGEX_ENGINE_ALLOCATED(pengine)) {
                continue;
            }
            count++;
        }
    }

    if (count == 0) {
        return BCM_E_NONE;
    }
    
    pres = sal_alloc(sizeof(_bcm_regex_resource_t)*count, "res_objs");
    
    for (count=0, i=tiles; i<=tilee; i++) {
        for (j=0; j<nept; j++) {
            pengine = BCM_TR3_PHYSICAL_ENGINE(device, (i*nept) + j);
            if (!BCM_REGEX_ENGINE_ALLOCATED(pengine)) {
                continue;
            }
            pres[count].pres = pengine;
            pres[count].eng_idx = pengine->hw_idx;
            pres[count].lsize = pengine->total_lsz;
            count++;
        }
    }

    *pnum_res = count;
    *ppres = pres;

    return BCM_E_NONE;
}

STATIC int
_bcm_tr3_free_resource_objs(int unit, _bcm_regex_resource_t **ppres, 
                            int *num_res)
{
    if (*ppres) {
        sal_free(*ppres);
        *ppres = NULL;
        *num_res = 0;
    }
    return BCM_E_NONE;
}

STATIC int _encode_max_flow(int max_flow)
{
    int enc;

    if (max_flow <= 1024) {
        enc = 0;
    } else if (max_flow <= 2048) {
        enc = 1;
    } else if (max_flow <= 4096) {
        enc = 2;
    } else if (max_flow <= 8192) {
        enc = 3;
    } else {
        enc = -1;
    }
    return enc;
}

STATIC int _decode_max_flow(int hw_enc)
{
    int max_flow;

    if (hw_enc == 0) {
        max_flow = 1024;
    } else if (hw_enc == 1) {
        max_flow = 2048;
    } else if (hw_enc == 2) {
        max_flow = 4096;
    } else if (hw_enc == 3) {
        max_flow = 8192;
    } else {
        max_flow = -1;
    }
    return max_flow;
}

static int is_printable(unsigned char c)
{
    const unsigned int printable_bmap[] = {
        0x00000000,
        0xffffffff,
        0xffffffff,
        0x7fffffff,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000
    };

    if (printable_bmap[c/32] & (1 << (c % 32))) {
        return 1;
    }
    return 0;
}

typedef enum _bcm_tr3_ctr_mode_e {
    _CTR_MODE_NONE  = 0,
    _CTR_MODE_PER_LEG,
    _CTR_MODE_BOTH_LEG
} _bcm_tr3_ctr_mode_t;

STATIC int
_bcm_tr3_ctr_mode_set(int unit, _bcm_tr3_ctr_mode_t mode)
{
    uint32 regval, fval;

    SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit, &regval));
    switch (mode) {
        case _CTR_MODE_NONE:
            fval = 0;
        break;
        case _CTR_MODE_BOTH_LEG:
            fval = 1;
        break;
        case _CTR_MODE_PER_LEG:
            fval = 2;
        break;
        default:
            return BCM_E_PARAM;
    }

    soc_reg_field_set(unit, FT_CONFIGr, &regval, CNT_MODEf, fval);
    SOC_IF_ERROR_RETURN(WRITE_FT_CONFIGr(unit, regval));
    
    return BCM_E_NONE;
}

STATIC 
_bcm_tr3_ctr_mode_t _bcm_tr3_ctr_mode_get(int unit)
{
    uint32 regval, fval;

    SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit, &regval));
    fval = soc_reg_field_get(unit, FT_CONFIGr, regval, CNT_MODEf);
    
    switch (fval) {
        case 1:
            return _CTR_MODE_BOTH_LEG;
        case 2:
            return _CTR_MODE_PER_LEG;
        default:
        case 0:
            return _CTR_MODE_NONE;
    }
}

STATIC int _bcm_tr3_flex_ctr_pool_id_unmap(int unit, int pool_id)
{
    _bcm_tr3_regex_device_info_t *device;
    int i;
    int rv;

    if ((device = REGEX_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    for (i = bcmStatFlexDirectionEgress; i >= bcmStatFlexDirectionIngress; i--) {
        device->flex_pools[i].pool_id = pool_id;
        if (!_BCM_TR3_FLEX_CTR_POOL_VALID(device->flex_pools[i].pool_id)) {
            continue;
        }
        device->flex_pools[i].flags = BCM_FLEX_DEALLOCATE_POOL;
        device->flex_pools[i].flags |= (i == bcmStatFlexDirectionIngress ?
                                             BCM_FLEX_INGRESS_POOL : 
                                             BCM_FLEX_EGRESS_POOL);
        rv = _bcm_esw_stat_flex_pool_operation(unit, &device->flex_pools[i]);
        if (rv != 0) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                    (BSL_META_U(unit,
                                "unmap: _bcm_esw_stat_flex_pool_operation failed: i = %d, id = %d, flags = 0x%04x, size = %d\n"),
                     i, device->flex_pools[i].pool_id,
                     device->flex_pools[i].flags,
                     device->flex_pools[i].pool_size));
            return rv;
        } else {
            LOG_INFO(BSL_LS_BCM_REGEX,
                    (BSL_META_U(unit,
                                "unmap: _bcm_esw_stat_flex_pool_operation succeeded: i = %d, id = %d, flags = 0x%04x, size = %d\n"),
                     i, device->flex_pools[i].pool_id,
                     device->flex_pools[i].flags,
                     device->flex_pools[i].pool_size));
        }
        device->flex_pools[i].pool_id = -1;
    }
    return BCM_E_NONE;
}

STATIC int _bcm_tr3_flex_ctr_pool_map(int unit, _bcm_tr3_ctr_mode_t mode, int max_flows)
{
    _bcm_tr3_regex_device_info_t *device;
    int dir;
    int rgn, poff, poolr;
    uint64 fval;
    uint64 regval;
    int rv;
    soc_field_t poolfTR3[] = { FWD_00f, FWD_01f, FWD_10f, FWD_11f,
                            REV_00f,  REV_01f, REV_10f, REV_11f };
    soc_field_t poolfH4[] = { FWD_000f, FWD_001f, FWD_010f, FWD_011f, FWD_100f, FWD_101f, FWD_110f, FWD_111f,
                            REV_000f, REV_001f, REV_010f, REV_011f, REV_100f, REV_101f, REV_110f, REV_111f };
    uint64 reg_update_per_dir[] = { 0,1,2,3,0,0,0,0,4,5,6,7,0,0,0,0 };    
    uint64 reg_update_shared[] = { 0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7 };    
    int poolf_size;

    if (SOC_IS_HELIX4(unit)) {
        poolf_size = sizeof(poolfH4)/sizeof(poolfH4[0]);
    }
    else {
        poolf_size = sizeof(poolfTR3)/sizeof(poolfTR3[0]);
    }

    if ((device = REGEX_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (mode == _CTR_MODE_NONE) {
        return BCM_E_NONE;
    }


    /* Alloc pools */

    /*
    Note that stat direction can be ingress or egress depending on the flow
    direction.  However, flow tracker stats are always on the ingress side,
    no matter whether the flow is coming from the client or the server.  So
    the size of the pool used will always be the size of an ingress pool.

    Since Triumph3 and Helix4 support 8192 entries in a flex_ingress_pool,
    device->flex_pools is only ever of size two, so there will be only
    two pools available for the FT stats, and the second one will only be
    used when traffic must be counted separately for each direction.
    */
    poolr = max_flows/ (SOC_INFO(unit).size_flex_ingress_pool);
    poolr += (max_flows % (SOC_INFO(unit).size_flex_ingress_pool)) ? 1 : 0;

    if(poolr > SOC_INFO(unit).num_flex_ingress_pools) {
        return BCM_E_RESOURCE;
    }

    for (dir = bcmStatFlexDirectionIngress; dir <=
          ((mode == _CTR_MODE_PER_LEG) ? bcmStatFlexDirectionEgress :
           bcmStatFlexDirectionIngress); dir++) {
        for( rgn =0; rgn < poolr; rgn++) {
            device->flex_pools[dir].flags = BCM_FLEX_ALLOCATE_POOL;
            device->flex_pools[dir].pool_size =
                (dir == bcmStatFlexDirectionIngress ?
                         SOC_INFO(unit).size_flex_ingress_pool:
                         SOC_INFO(unit).size_flex_egress_pool);
            device->flex_pools[dir].flags |= (dir == bcmStatFlexDirectionIngress ?
                    BCM_FLEX_INGRESS_POOL : 
                    BCM_FLEX_EGRESS_POOL);
            if (SOC_IS_HELIX4(unit)) {
                if(mode == _CTR_MODE_PER_LEG) {
                    device->flex_pools[dir].pool_id = reg_update_per_dir[rgn + dir*8];
                } else {
                    device->flex_pools[dir].pool_id = reg_update_shared[rgn + dir*8];
                }
            } else {
                device->flex_pools[dir].pool_id = dir;
            }
            rv = _bcm_esw_stat_flex_pool_operation(unit, &device->flex_pools[dir]);
            if (rv != 0) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                        (BSL_META_U(unit,
                                    "map: _bcm_esw_stat_flex_pool_operation failed: dir = %d, id = %d, flags = 0x%04x, size = %d\n"),
                         dir, device->flex_pools[dir].pool_id,
                         device->flex_pools[dir].flags,
                         device->flex_pools[dir].pool_size));
                /*Clean up*/
                if(device->flex_pools[dir].pool_id == 0) {
                    device->flex_pools[dir].pool_id = -1;
                    return rv;
                }
                for(rgn = device->flex_pools[dir].pool_id - 1; rgn >= 0; rgn--) {
                    _bcm_tr3_flex_ctr_pool_id_unmap(unit,rgn); 
                }
                return rv;
            } else {
                LOG_INFO(BSL_LS_BCM_REGEX,
                        (BSL_META_U(unit,
                                    "map: _bcm_esw_stat_flex_pool_operation succeeded: dir = %d, id = %d, flags = 0x%04x, size = %d\n"),
                         dir, device->flex_pools[dir].pool_id,
                         device->flex_pools[dir].flags,
                         device->flex_pools[dir].pool_size));
            }
        }
    }

    SOC_IF_ERROR_RETURN(READ_FT_CTR_POOLr(unit, &regval));
    /*
    Assign 2 pools of 8K entries for counting traffic in separate directions,
    and just 1 pool of 8K entries for counting bidirectional traffic.
    */
    if (SOC_IS_HELIX4(unit)) {
        for (rgn = 0; rgn < poolf_size; rgn++) {
             if (mode == _CTR_MODE_PER_LEG) {
                 soc_reg64_field_set(unit, FT_CTR_POOLr, &regval, poolfH4[rgn], reg_update_per_dir[rgn]);
             } else  {
                 soc_reg64_field_set(unit, FT_CTR_POOLr, &regval, poolfH4[rgn], reg_update_shared[rgn]);
             }
        }
    } else {
        for (rgn = 0; rgn < poolf_size; rgn++) {
            poff = (mode == _CTR_MODE_PER_LEG) ? rgn/(poolf_size/2) : 0;
            COMPILER_64_SET(fval, 0, device->flex_pools[poff].pool_id);
            soc_reg64_field_set(unit, FT_CTR_POOLr, &regval, poolfTR3[rgn], fval);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_FT_CTR_POOLr(unit, regval));

    return BCM_E_NONE;
}

STATIC int _bcm_tr3_flex_ctr_pool_unmap(int unit)
{
    _bcm_tr3_regex_device_info_t *device;
    int i;
    int rv, max_flows, poolr, rgn;
    uint64 reg_update_shared[] = { 0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7 };

    if ((device = REGEX_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    max_flows = BCM_TR3_MAX_FLOWS;
    poolr = max_flows/ (SOC_INFO(unit).size_flex_ingress_pool);
    poolr += (max_flows % (SOC_INFO(unit).size_flex_ingress_pool)) ? 1 : 0;

    if(poolr > SOC_INFO(unit).num_flex_ingress_pools){
        return BCM_E_RESOURCE;
    }
    for (i = bcmStatFlexDirectionIngress; 
            i <= bcmStatFlexDirectionEgress ; i++) {
        for(rgn =0; rgn < poolr; rgn++) {
            device->flex_pools[i].pool_id = reg_update_shared[rgn + i*8]; 
            if (!_BCM_TR3_FLEX_CTR_POOL_VALID(device->flex_pools[i].pool_id)) {
                continue;
            }
            device->flex_pools[i].flags = BCM_FLEX_DEALLOCATE_POOL;
            device->flex_pools[i].flags |= (i == bcmStatFlexDirectionIngress ?
                    BCM_FLEX_INGRESS_POOL : 
                    BCM_FLEX_EGRESS_POOL);
            rv = _bcm_esw_stat_flex_pool_operation(unit, &device->flex_pools[i]);

            if (rv != 0) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                        (BSL_META_U(unit,
                                    "unmap: _bcm_esw_stat_flex_pool_operation failed: i = %d, id = %d, flags = 0x%04x, size = %d\n"),
                         i, device->flex_pools[i].pool_id,
                         device->flex_pools[i].flags,
                         device->flex_pools[i].pool_size));
            }
            else
            {
                LOG_INFO(BSL_LS_BCM_REGEX,
                        (BSL_META_U(unit,
                                    "unmap: _bcm_esw_stat_flex_pool_operation succeeded: i = %d, id = %d, flags = 0x%04x, size = %d\n"),
                         i, device->flex_pools[i].pool_id,
                         device->flex_pools[i].flags,
                         device->flex_pools[i].pool_size));
            }
        }
    }
    device->flex_pools[bcmStatFlexDirectionIngress].pool_id = -1;
    device->flex_pools[bcmStatFlexDirectionEgress].pool_id = -1;
    return BCM_E_NONE;
}

int 
_bcm_tr3_regex_dump_axp(int unit)
{
    uint32 regval;

    LOG_CLI((BSL_META_U(unit,
                        "Regex AXP status register values:\n")));
    SOC_IF_ERROR_RETURN
        (READ_AXP_SM_SIGNATURE_MATCH_ACTIVE_STATUS1r(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_SIGNATURE_MATCH_ACTIVE_STATUS1 = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN
        (READ_AXP_SM_SIGNATURE_MATCH_ACTIVE_STATUS0r(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_SIGNATURE_MATCH_ACTIVE_STATUS0 = 0x%08x\n\n"), regval));
    LOG_CLI((BSL_META_U(unit,
                        "Regex AXP counter register values:\n")));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_PACKETS_RECEIVED_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_PACKETS_RECEIVED_COUNTER       = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_PACKETS_SENT_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_PACKETS_SENT_COUNTER           = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_PACKETS_DROPPED_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_PACKETS_DROPPED_COUNTER        = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_BYTES_MATCHED_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_BYTES_MATCHED_COUNTER          = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_MATCHED_FLOWS_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_MATCHED_FLOWS_COUNTER          = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_UNMATCHED_FLOWS_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_UNMATCHED_FLOWS_COUNTER        = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_TOTAL_MATCH_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_TOTAL_MATCH_COUNTER            = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN
        (READ_AXP_SM_TOTAL_CROSS_SIG_FLAGS_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_TOTAL_CROSS_SIG_FLAGS_COUNTER  = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_FRAGMENTS_RECEIVED_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_FRAGMENTS_RECEIVED_COUNTER     = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_IN_PACKET_ERROR_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_IN_PACKET_ERROR_COUNTER        = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_FLOW_TRACKER_ERROR_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_FLOW_TRACKER_ERROR_COUNTER     = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN
        (READ_AXP_SM_PACKET_LENGTH_ERROR_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_PACKET_LENGTH_ERROR_COUNTER    = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_L4_CHECKSUM_ERROR_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_L4_CHECKSUM_ERROR_COUNTER      = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN
        (READ_AXP_SM_FLOW_DONE_PACKET_DROP_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_FLOW_DONE_PACKET_DROP_COUNTER  = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN
        (READ_AXP_SM_FLOW_TIMESTAMP_ERROR_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_FLOW_TIMESTAMP_ERROR_COUNTER   = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN
        (READ_AXP_SM_FLOW_PACKET_NUM_ERROR_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_FLOW_PACKET_NUM_ERROR_COUNTER  = 0x%08x\n"), regval));
    SOC_IF_ERROR_RETURN(READ_AXP_SM_ECC_ERROR_COUNTERr(unit, &regval));
    LOG_CLI((BSL_META_U(unit,
                        " AXP_SM_ECC_ERROR_COUNTER              = 0x%08x\n"), regval));

    return BCM_E_NONE;
}

int
_bcm_tr3_regex_config_set(int unit, bcm_regex_config_t *config)
{
    int max_flows, payload_depth, inspect_num_pkt, rv;
    _bcm_tr3_regex_device_info_t *device;
    _bcm_tr3_ctr_mode_t ctr_mode;
    uint32 field_val;
    uint32 regval;

    if ((device = REGEX_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (!config) {
        return BCM_E_PARAM;
    }

    /*
    Set mode for REGEX usage
    */

    if (config->flags & BCM_REGEX_USE_POLICY_ID) {
        device->info->flags |= BCM_REGEX_USE_POLICY_ID;
    }
    else {
        device->info->flags &= ~BCM_REGEX_USE_POLICY_ID;
    }


    /*
     * Adjust encodings.
     */
    max_flows = ((config->max_flows < 0) || 
                 (_encode_max_flow(config->max_flows) == -1)) ? 
                BCM_TR3_MAX_FLOWS : config->max_flows;

    if (config->flags & BCM_REGEX_PER_DIRECTION_COUNTER_ENABLE) {
        if (config->max_flows < 0) {
            max_flows = BCM_TR3_MAX_FLOWS/2;
        } else {
            if (max_flows > (BCM_TR3_MAX_FLOWS/2)) {
                return BCM_E_RESOURCE;
            } 
        } 
    }

    payload_depth = (config->payload_depth <= 0) ?
                BCM_TR3_MAX_PAYLOAD_DEPTH : config->payload_depth;

    inspect_num_pkt = (config->inspect_num_pkt <= 0) ?
                BCM_TR3_MAX_PKT_TO_SME : config->inspect_num_pkt;

    /* validate params */
    if ((config->max_flows > BCM_TR3_MAX_FLOWS) ||
        (config->payload_depth > BCM_TR3_MAX_PAYLOAD_DEPTH) ||
        (config->inspect_num_pkt > BCM_TR3_MAX_PKT_TO_SME)) {
        return BCM_E_PARAM;
    }

    /* set MACSA/DA */
    if (config->flags & BCM_REGEX_CONFIG_ENABLE) {
        field_val = (config->dst_mac[0] << 8) |    /* MAC-DA[47:40] */
                    (config->dst_mac[1]);          /* MAC-DA[39:32] */
        SOC_IF_ERROR_RETURN(READ_FT_RESULT_DA_MSr(unit, &regval));
        soc_reg_field_set(unit, FT_RESULT_DA_MSr, &regval, DAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_FT_RESULT_DA_MSr(unit, regval));

        field_val = (config->dst_mac[2] << 24) |    /* MAC-DA[31:24] */
                    (config->dst_mac[3] << 16) |    /* MAC-DA[23:16] */
                    (config->dst_mac[4] << 8) |     /* MAC-DA[15:8] */
                    (config->dst_mac[5]);           /* MAC-DA[7:0] */

        regval = 0;
        soc_reg_field_set(unit, FT_RESULT_DA_LSr, &regval, DAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_FT_RESULT_DA_LSr(unit, regval));

        regval = 0;
        field_val = (config->dst_mac[0] << 24) |    /* MAC-DA[47:40] */
                    (config->dst_mac[1] << 16) |    /* MAC-DA[39:32] */
                    (config->dst_mac[2] << 8) |     /* MAC-DA[31:24] */
                    (config->dst_mac[3]);           /* MAC-DA[23:16] */
        soc_reg_field_set(unit, AXP_SM_REPORT_PACKET_CONTROL0r, &regval, 
                            UPPER_MACDAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REPORT_PACKET_CONTROL0r(unit, regval));

        regval = 0;
        field_val = (config->dst_mac[4] << 8) | (config->dst_mac[5]);
        soc_reg_field_set(unit, AXP_SM_REPORT_PACKET_CONTROL1r, &regval, 
                            LOWER_MACDAf, field_val);
        field_val = (config->src_mac[0] << 8) | (config->src_mac[1]);
        soc_reg_field_set(unit, AXP_SM_REPORT_PACKET_CONTROL1r, &regval, 
                            UPPER_MACSAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REPORT_PACKET_CONTROL1r(unit, regval));

        regval = 0;
        field_val = ((config->src_mac[2] <<24) | 
                     (config->src_mac[3] << 16) |
                     (config->src_mac[4] << 8) |
                     (config->src_mac[5]));
        soc_reg_field_set(unit, AXP_SM_REPORT_PACKET_CONTROL2r, &regval, 
                            LOWER_MACSAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REPORT_PACKET_CONTROL2r(unit, regval));

        regval = 0;
        soc_reg_field_set(unit, FT_RESULT_LENGTH_TYPEr, &regval, ENABLEf, 1);
        soc_reg_field_set(unit, FT_RESULT_LENGTH_TYPEr, &regval, 
                          LENGTH_TYPEf, config->ethertype);
        SOC_IF_ERROR_RETURN(WRITE_FT_RESULT_LENGTH_TYPEr(unit, regval));

        regval = 0;
        soc_reg_field_set(unit, AXP_SM_REPORT_PACKET_CONTROL3r, &regval, 
                          VLANf, 1);
        soc_reg_field_set(unit, AXP_SM_REPORT_PACKET_CONTROL3r, &regval, 
                          ETHER_TYPEf, config->ethertype);
        SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REPORT_PACKET_CONTROL3r(unit, regval));
    }

    /* Set Reports */
    SOC_IF_ERROR_RETURN(READ_FT_EVENT_CONFIGr(unit, &regval));
    if (config->flags & BCM_REGEX_CONFIG_ENABLE) {
        soc_reg_field_set(unit, FT_EVENT_CONFIGr, &regval, NEW_FLOWf, 
                          ((config->report_flags & (BCM_REGEX_REPORT_NEW | BCM_REGEX_REPORT_ALL)) ? 1 : 0));
        soc_reg_field_set(unit, FT_EVENT_CONFIGr, &regval, RESf,
                          ((config->report_flags & (BCM_REGEX_REPORT_MATCHED | BCM_REGEX_REPORT_ALL)) ? 1 : 0));
        soc_reg_field_set(unit, FT_EVENT_CONFIGr, &regval, RETIREDf,
                          ((config->report_flags & (BCM_REGEX_REPORT_END | BCM_REGEX_REPORT_ALL)) ? 1 : 0));
    } else {
        soc_reg_field_set(unit, FT_EVENT_CONFIGr, &regval, NEW_FLOWf, 0);
        soc_reg_field_set(unit, FT_EVENT_CONFIGr, &regval, RESf, 0);
        soc_reg_field_set(unit, FT_EVENT_CONFIGr, &regval, RETIREDf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_FT_EVENT_CONFIGr(unit, regval));
   
    SOC_IF_ERROR_RETURN(READ_FT_AGE_CONTROLr(unit,&regval));
    if (config->flags & BCM_REGEX_CONFIG_AGING) {
        soc_reg_field_set(unit, FT_AGE_CONTROLr, &regval, AGE_ENABLEf, 
                    ((config->inactivity_timeout_usec >= 0) ? 1 : 0));
    }
    else {
        soc_reg_field_set(unit, FT_AGE_CONTROLr, &regval, AGE_ENABLEf, 0);
    }
    soc_reg_field_set(unit, FT_AGE_CONTROLr, &regval, INDEXf, 0);
    SOC_IF_ERROR_RETURN(WRITE_FT_AGE_CONTROLr(unit, regval));

    SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit,&regval));
    soc_reg_field_set(unit, FT_CONFIGr, &regval, MAX_BYTESf, 
                                                     payload_depth);
    soc_reg_field_set(unit, FT_CONFIGr, &regval, MAX_PKTSf, 
                                                    inspect_num_pkt);
    soc_reg_field_set(unit, FT_CONFIGr, &regval, TABLE_SIZEf, 
                                        _encode_max_flow(max_flows));
    /*
     * Initializing INT_SME_ENABLEf to 0 so that the flow tracker does
     * not direct packets to Signature Matching block even before any 
     * signatures are configured.
     */
    soc_reg_field_set(unit, FT_CONFIGr, &regval, INT_SME_ENABLEf, 0);
    soc_reg_field_set(unit, FT_CONFIGr, &regval, EXT_SME_ENABLEf, 0);

    soc_reg_field_set(unit, FT_CONFIGr, &regval, FT_DISABLEf, 
                (config->flags & BCM_REGEX_CONFIG_ENABLE) ? 0 : 1);

    soc_reg_field_set(unit, FT_CONFIGr, &regval, TCP_END_DETECTf, 
          (config->flags & BCM_REGEX_CONFIG_TCP_SESSION_DETECT) ? 1 : 0);

    soc_reg_field_set(unit, FT_CONFIGr, &regval, NORMALIZEf, 1);

    soc_reg_field_set(unit, FT_CONFIGr, &regval, IPV4_CREATE_ENf,
                    (config->flags & BCM_REGEX_CONFIG_IP4) ? 1 : 0);

    soc_reg_field_set(unit, FT_CONFIGr, &regval, IPV6_CREATE_ENf,
                        (config->flags & BCM_REGEX_CONFIG_IP6) ? 1 : 0);

    SOC_IF_ERROR_RETURN(WRITE_FT_CONFIGr(unit,regval));

    field_val = (config->flags & BCM_REGEX_CONFIG_ENABLE) ? 1 : 0;
    SOC_IF_ERROR_RETURN(
                READ_AXP_SM_SIGNATURE_MATCH_CONTROLr(unit, &regval));
    soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                                             &regval, ENABLEf, field_val);
    soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                                                &regval, RESETf, 0);
    if (field_val) {
        soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                          &regval, STATE_TABLE_ACCESS_LIMIT_ENABLEf, 1);
        soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                         &regval, DROP_ON_L4_CHECKSUM_ERROR_ENABLEf, 1);
        soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                       &regval, DROP_ON_PACKET_LENGTH_ERROR_ENABLEf, 0);
        soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                      &regval, DROP_ON_FLOW_TIMESTAMP_ERROR_ENABLEf, 1);
        soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                                &regval, MAX_BYTE_INSPECTED_ENABLEf, 1);
        soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                                &regval, MAX_BYTE_INSPECTEDf, payload_depth);
        soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                        &regval, DROP_ON_FLOW_PKT_NUM_ERROR_ENABLEf, 1);
        soc_reg_field_set(unit, AXP_SM_SIGNATURE_MATCH_CONTROLr, 
                      &regval, DROP_ON_FLOW_TABLE_ECC_ERROR_ENABLEf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_SIGNATURE_MATCH_CONTROLr(unit, 
                                                            regval));

    SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROL_2r(unit, &regval));
    soc_reg_field_set(unit, AUX_ARB_CONTROL_2r, &regval, 
                            FT_REFRESH_ENABLEf, field_val);
    SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROL_2r(unit, regval));


    ctr_mode = _bcm_tr3_ctr_mode_get(unit);
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "Counter mode = %d, config flags = 0x%x\n"),
              ctr_mode, config->flags));
    if (config->flags & (BCM_REGEX_COUNTER_ENABLE | 
                         BCM_REGEX_PER_DIRECTION_COUNTER_ENABLE)) {
        /*
        if ((ctr_mode != _CTR_MODE_NONE) &&
            (((config->flags & BCM_REGEX_PER_DIRECTION_COUNTER_ENABLE) &&
              (ctr_mode != _CTR_MODE_PER_LEG)) || 
             (ctr_mode != _CTR_MODE_BOTH_LEG))) {
            */
        if (ctr_mode != _CTR_MODE_NONE) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "Setting counter mode to none and unmapping\n")));
            _bcm_tr3_ctr_mode_set(unit, _CTR_MODE_NONE);
            _bcm_tr3_flex_ctr_pool_unmap(unit);
        }
        ctr_mode = (config->flags & BCM_REGEX_PER_DIRECTION_COUNTER_ENABLE) ? 
                                    _CTR_MODE_PER_LEG : _CTR_MODE_BOTH_LEG;
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "Counter mode = %d, setting and mapping flex counters\n"),
                  ctr_mode));
        rv = _bcm_tr3_flex_ctr_pool_map(unit, ctr_mode, max_flows);
        if(rv != BCM_E_NONE)
            return rv;
        _bcm_tr3_ctr_mode_set(unit, ctr_mode);
    } else {
        /* free up the pools */
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "No counters enabled, setting counter mode to none and unmapping\n")));
        _bcm_tr3_ctr_mode_set(unit, _CTR_MODE_NONE);
        _bcm_tr3_flex_ctr_pool_unmap(unit);
    }

    return BCM_E_NONE;
}

int 
_bcm_tr3_regex_config_get(int unit, bcm_regex_config_t *config)
{
    _bcm_tr3_ctr_mode_t ctr_mode;
    uint32 field_val;
    uint32 regval;
    _bcm_tr3_regex_device_info_t *device;

    if (!config) {
        return BCM_E_PARAM;
    }

    if ((device = REGEX_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    /* Set Reports */
    SOC_IF_ERROR_RETURN(READ_FT_EVENT_CONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, FT_EVENT_CONFIGr, regval, NEW_FLOWf)) {
        config->report_flags |= BCM_REGEX_REPORT_NEW;
    }
    if (soc_reg_field_get(unit, FT_EVENT_CONFIGr, regval, RESf)) {
        config->report_flags |= BCM_REGEX_REPORT_MATCHED;
    }
    if (soc_reg_field_get(unit, FT_EVENT_CONFIGr, regval, RETIREDf)) {
        config->report_flags |= BCM_REGEX_REPORT_END;
    }
    

    SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit,&regval));
    config->payload_depth = 
            soc_reg_field_get(unit, FT_CONFIGr, regval, MAX_BYTESf);
    config->inspect_num_pkt = 
            soc_reg_field_get(unit, FT_CONFIGr, regval, MAX_PKTSf);
    config->max_flows = _decode_max_flow(soc_reg_field_get(unit, 
                                        FT_CONFIGr, regval, TABLE_SIZEf));

    config->flags = 0;
    if (soc_reg_field_get(unit, FT_CONFIGr, regval, FT_DISABLEf) == 0) {
        config->flags |= BCM_REGEX_CONFIG_ENABLE;
    }

    if (soc_reg_field_get(unit, FT_CONFIGr, regval, TCP_END_DETECTf)) {
        config->flags |= BCM_REGEX_CONFIG_TCP_SESSION_DETECT;
    }

    if (soc_reg_field_get(unit, FT_CONFIGr, regval, IPV4_CREATE_ENf)) {
        config->flags |= BCM_REGEX_CONFIG_IP4;
    }

    if (soc_reg_field_get(unit, FT_CONFIGr, regval, IPV6_CREATE_ENf)) {
        config->flags |= BCM_REGEX_CONFIG_IP6;
    }

    /* get MACSA/DA */
    SOC_IF_ERROR_RETURN(READ_AXP_SM_REPORT_PACKET_CONTROL0r(unit, &regval));
    field_val = soc_reg_field_get(unit, AXP_SM_REPORT_PACKET_CONTROL0r, 
                                    regval, UPPER_MACDAf);
    config->dst_mac[0] = ((field_val >> 24) & 0xff);
    config->dst_mac[1] = ((field_val >> 16) & 0xff);
    config->dst_mac[2] = ((field_val >> 8) & 0xff);
    config->dst_mac[3] = (field_val & 0xff);

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REPORT_PACKET_CONTROL2r(unit, &regval));
    field_val = soc_reg_field_get(unit, AXP_SM_REPORT_PACKET_CONTROL2r, 
                                  regval, LOWER_MACSAf);
    config->src_mac[2] = ((field_val >> 24) & 0xff);
    config->src_mac[3] = ((field_val >> 16) & 0xff);
    config->src_mac[4] = ((field_val >> 8) & 0xff);
    config->src_mac[5] = (field_val & 0xff);

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REPORT_PACKET_CONTROL1r(unit, &regval));
    field_val = soc_reg_field_get(unit, AXP_SM_REPORT_PACKET_CONTROL1r, regval, 
                                  UPPER_MACSAf);
    config->src_mac[0] = ((field_val >> 8) & 0xff);
    config->src_mac[1] = (field_val & 0xff);

    field_val = soc_reg_field_get(unit, AXP_SM_REPORT_PACKET_CONTROL1r, 
                                 regval, LOWER_MACDAf);
    config->dst_mac[4] = ((field_val >> 8) & 0xff);
    config->dst_mac[5] = (field_val & 0xff);

    SOC_IF_ERROR_RETURN(READ_FT_RESULT_LENGTH_TYPEr(unit, &regval));
    config->ethertype = soc_reg_field_get(unit, 
                     FT_RESULT_LENGTH_TYPEr, regval, LENGTH_TYPEf);

    ctr_mode = _bcm_tr3_ctr_mode_get(unit);
    if (ctr_mode == _CTR_MODE_PER_LEG) {
        config->flags |= BCM_REGEX_PER_DIRECTION_COUNTER_ENABLE;
    } else if (ctr_mode == _CTR_MODE_BOTH_LEG) {
        config->flags |= BCM_REGEX_COUNTER_ENABLE;
    }

    /*
    Get REGEX usage mode
    */

    if (device->info->flags & BCM_REGEX_USE_POLICY_ID) {
        config->flags |= BCM_REGEX_USE_POLICY_ID;
    }

    return BCM_E_NONE;
}

int _bcm_tr3_regex_exclude_delete_all(int unit)
{
    int rv;

    BCM_REGEX_LOCK(unit);
    rv = soc_mem_clear(unit, FT_L4PORTm, COPYNO_ALL, TRUE);
    BCM_REGEX_UNLOCK(unit);
    return rv;
}

#define BCM_TR3_PROTOCOL_TCP        6
#define BCM_TR3_PROTOCOL_UDP        17

STATIC
int _bcm_tr3_valid_protocol(int unit, uint16 protocol)
{
    if ((protocol == BCM_TR3_PROTOCOL_TCP) || 
        (protocol == BCM_TR3_PROTOCOL_UDP)) {
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

#define BCM_TR3_EXCLUDE_OP_INSERT   0
#define BCM_TR3_EXCLUDE_OP_DELETE   1

#define BCM_TR3_NUM_PROCOL          2

#define BCM_TR3_MAX_EXCLUDE_RANGE   8

typedef struct _bcm_tr3_l4_range_s {
    uint16 start;
    uint16 end;
    int    tcp;
    int    any;
} _bcm_tr3_l4_range_t;

STATIC int
_bcm_tr3_regex_sort_ranges(void *a, void *b)
{
    _bcm_tr3_l4_range_t *ia, *ib;
    ia = (_bcm_tr3_l4_range_t*) a;
    ib = (_bcm_tr3_l4_range_t*) b;
    return ia->start - ib->start; 
}

#define _TR3_REGEX_E_RANGE(s,t,a,b)     \
            ((s))->tcp = (t);             \
            ((s))->any = 0;               \
            ((s))->start = (a);           \
            ((s))->end = (b);

#define _TR3_REGEX_E_RANGE_START(r)     (r)->start
#define _TR3_REGEX_E_RANGE_END(r)     (r)->end
#define _TR3_REGEX_E_RANGE_PROTOCOL(r)     (r)->tcp
                

STATIC int
_bcm_tr3_merge_ranges(_bcm_tr3_l4_range_t *range, int *count)
{
    int in_cnt = *count, i, j, o_cnt = in_cnt, pr1, pr2;

    _shr_sort(range, in_cnt, sizeof(_bcm_tr3_l4_range_t), 
                             _bcm_tr3_regex_sort_ranges);

    for (i = 0; i < o_cnt - 1;) {
        pr1 = _TR3_REGEX_E_RANGE_PROTOCOL(&range[i]);
        pr2 = _TR3_REGEX_E_RANGE_PROTOCOL(&range[i + 1]);

        if ((_TR3_REGEX_E_RANGE_START(&range[i]) == 
                            _TR3_REGEX_E_RANGE_START(&range[i+1])) &&
            (_TR3_REGEX_E_RANGE_END(&range[i]) == 
                            _TR3_REGEX_E_RANGE_END(&range[i+1]))) {
            if (pr1 != pr2) {
                range[i].any = 1;
            }

            o_cnt -= 1;
            for (j = i+1; j < o_cnt; j++) {
                range[j] = range[j+1];
            }
        } else if (pr1 != pr2) {
            i++;
            continue;
        } else if (_TR3_REGEX_E_RANGE_END(&range[i]) >= 
                            _TR3_REGEX_E_RANGE_START(&range[i+1])) {
            if (_TR3_REGEX_E_RANGE_END(&range[i+1]) > 
                            _TR3_REGEX_E_RANGE_END(&range[i])) {
                _TR3_REGEX_E_RANGE(&range[i],
                                   _TR3_REGEX_E_RANGE_PROTOCOL(&range[i]),
                                   _TR3_REGEX_E_RANGE_START(&range[i]),
                                   _TR3_REGEX_E_RANGE_END(&range[i+1]));
            } else {
                _TR3_REGEX_E_RANGE(&range[i],
                                   _TR3_REGEX_E_RANGE_PROTOCOL(&range[i]),
                                   _TR3_REGEX_E_RANGE_START(&range[i]),
                                   _TR3_REGEX_E_RANGE_END(&range[i]));
            }

            o_cnt -= 1;
            for (j = i+1; j < o_cnt; j++) {
                range[j] = range[j+1];
            }
        } else {
            i++;
        }
    }
    *count = o_cnt;
    return 0;
}

STATIC
int _bcm_tr3_exclude_op(int unit, int op, int tcp,
                        uint16 l4start, uint16 l4end)
{
    _bcm_tr3_l4_range_t *tcomp, *ucomp, *fcomp, *prange;
    uint16 this_start, this_end, num_te;
    int     rv = BCM_E_NONE, *pnum;
    int     i, j, tcpi=0, udpi = 0, num_entry=0;
    ft_l4port_entry_t   ftl4m;

    num_entry = soc_mem_index_count(unit, FT_L4PORTm);
    num_te = (num_entry * 2) + 1;
    tcomp = sal_alloc(sizeof(_bcm_tr3_l4_range_t) * num_te, "sortee");
    ucomp = sal_alloc(sizeof(_bcm_tr3_l4_range_t) * num_te, "sorteeu");
    fcomp = sal_alloc(sizeof(_bcm_tr3_l4_range_t) * 2 * num_te, "sorteef");

    /* read all the entries so that we can rebalnace the entries ie
     * merge/split based on ranges. */
    for (i=0; i < num_entry; i++) {
        if (READ_FT_L4PORTm(unit, MEM_BLOCK_ANY, i, &ftl4m)) {
            goto done;
        }

        if (soc_mem_field32_get(unit, FT_L4PORTm, &ftl4m, VALIDf) == 0) {
            break;
        }
        
        this_start = soc_mem_field32_get(unit, FT_L4PORTm, 
                                         &ftl4m, L4_PORT_LOWERf);
        this_end = soc_mem_field32_get(unit, FT_L4PORTm, 
                                         &ftl4m, L4_PORT_UPPERf);

        if (soc_mem_field32_get(unit, FT_L4PORTm, &ftl4m, TCPf)) {
            _TR3_REGEX_E_RANGE(&tcomp[tcpi], 1, this_start, this_end);
            tcpi++;
        }
        if (soc_mem_field32_get(unit, FT_L4PORTm, &ftl4m, UDPf)) {
            _TR3_REGEX_E_RANGE(&ucomp[udpi], 0, this_start, this_end);
            udpi++;
        }
    }

    if (tcp) {
        prange = tcomp;
        pnum = &tcpi;
    } else {
        prange = ucomp;
        pnum = &udpi;
    }

    if (op == BCM_TR3_EXCLUDE_OP_INSERT) {
        /* insert the new range at the end */
        _TR3_REGEX_E_RANGE(&prange[*pnum], tcp, l4start, l4end);
        *pnum += 1;
    }

    _bcm_tr3_merge_ranges(tcomp, &tcpi);
    _bcm_tr3_merge_ranges(ucomp, &udpi);


    if (op == BCM_TR3_EXCLUDE_OP_DELETE) {
        for (i = 0; i < *pnum; i++) {
            if ((l4start <= _TR3_REGEX_E_RANGE_START(&prange[i])) &&
                (l4end >= _TR3_REGEX_E_RANGE_START(&prange[i]))) {
                for (j = i+1; j < *pnum; j++) {
                    prange[j - 1] = prange[j];
                }
                *pnum -= 1;
                i -= 1;
            } else if ((l4start > _TR3_REGEX_E_RANGE_START(&prange[i])) &&
                       (l4end >= _TR3_REGEX_E_RANGE_END(&prange[i]))) {
                _TR3_REGEX_E_RANGE(&prange[i], tcp, 
                    _TR3_REGEX_E_RANGE_START(&prange[i]), l4start - 1);
            } else if ((l4end >= _TR3_REGEX_E_RANGE_START(&prange[i])) &&
                       (l4end < _TR3_REGEX_E_RANGE_END(&prange[i]))) {
                _TR3_REGEX_E_RANGE(&prange[i], tcp, l4end + 1,
                                    _TR3_REGEX_E_RANGE_END(&prange[i]));
            } else if ((l4start > _TR3_REGEX_E_RANGE_START(&prange[i])) &&
                       (l4end < _TR3_REGEX_E_RANGE_END(&prange[i]))) {
                /* make space */
                for (j = *pnum - 1; j > i; j--) {
                    prange[j+1] = prange[j];
                }
                _TR3_REGEX_E_RANGE(&prange[i+1], tcp, l4end + 1, 
                                _TR3_REGEX_E_RANGE_END(&prange[i]));
                _TR3_REGEX_E_RANGE(&prange[i], tcp, 
                        _TR3_REGEX_E_RANGE_START(&prange[i]), l4start - 1);

                *pnum += 1;
                i += 1;
            }
        }
    }

    /* merge tcp and  udp entries */

    if ((tcpi + udpi) == 0) {
        rv = _bcm_tr3_regex_exclude_delete_all(unit);
        goto done;
    }

    j = 0;
    for (i = 0; i < tcpi; i++) {
        fcomp[j++] = tcomp[i];
    }

    for (i = 0; i < udpi; i++) {
        fcomp[j++] = ucomp[i];
    }
    
    _bcm_tr3_merge_ranges(fcomp, &j);
    if (j > num_entry) {
        return BCM_E_RESOURCE;
    }

    for (i = 0; i < j; i++) {
        sal_memset(&ftl4m, 0, sizeof(ft_l4port_entry_t));
        soc_mem_field32_set(unit, FT_L4PORTm, &ftl4m, VALIDf, 1);
        soc_mem_field32_set(unit, FT_L4PORTm, &ftl4m, 
               L4_PORT_LOWERf, _TR3_REGEX_E_RANGE_START(&fcomp[i]));
        soc_mem_field32_set(unit, FT_L4PORTm, &ftl4m, 
               L4_PORT_UPPERf, _TR3_REGEX_E_RANGE_END(&fcomp[i]));

        if (fcomp[i].any == 1) {
            soc_mem_field32_set(unit, FT_L4PORTm, &ftl4m, TCPf, 1);
            soc_mem_field32_set(unit, FT_L4PORTm, &ftl4m, UDPf, 1);
        } else if (_TR3_REGEX_E_RANGE_PROTOCOL(&fcomp[i])) {
            soc_mem_field32_set(unit, FT_L4PORTm, &ftl4m, TCPf, 1);
        } else {
            soc_mem_field32_set(unit, FT_L4PORTm, &ftl4m, UDPf, 1);
        }
        rv = WRITE_FT_L4PORTm(unit, MEM_BLOCK_ANY, i, &ftl4m);
        if (BCM_FAILURE(rv)) {
            goto done;
        }
    }

    for (; i < num_entry; i++) {
        sal_memset(&ftl4m, 0, sizeof(ft_l4port_entry_t));
        rv = WRITE_FT_L4PORTm(unit, MEM_BLOCK_ANY, i, &ftl4m);
        if (BCM_FAILURE(rv)) {
            goto done;
        }
    }

done:
    sal_free(tcomp);
    sal_free(ucomp);
    sal_free(fcomp);

    return rv;
}

int _bcm_tr3_regex_exclude_add(int unit, uint8 protocol, 
                              uint16 l4_start, uint16 l4_end)
{
    int rv;
    if (_bcm_tr3_valid_protocol(unit, protocol)) {
        return BCM_E_PARAM;
    }
    
    BCM_REGEX_LOCK(unit);
    rv = _bcm_tr3_exclude_op(unit, BCM_TR3_EXCLUDE_OP_INSERT, 
                        (protocol == BCM_TR3_PROTOCOL_TCP) ? 1 : 0,
                        l4_start, l4_end);
    BCM_REGEX_UNLOCK(unit);
    return rv;
}

int _bcm_tr3_regex_exclude_delete(int unit, uint8 protocol, 
                              uint16 l4_start, uint16 l4_end)
{
    int rv;

    if (_bcm_tr3_valid_protocol(unit, protocol)) {
        return BCM_E_PARAM;
    }
    
    BCM_REGEX_LOCK(unit);
    rv = _bcm_tr3_exclude_op(unit, BCM_TR3_EXCLUDE_OP_DELETE, 
                        (protocol == BCM_TR3_PROTOCOL_TCP) ? 1 : 0,
                        l4_start, l4_end);
    BCM_REGEX_UNLOCK(unit);
    return rv;
}

int _bcm_tr3_regex_exclude_get(int unit, int array_size, uint8 *protocol, 
                        uint16 *l4low, uint16 *l4high, int *array_count)
{
    int i, valid, nume = 0;
    uint16  this_start, this_end;
    ft_l4port_entry_t   ftl4m;

    *array_count = 0;

    BCM_REGEX_LOCK(unit);
    for (i=0; (i < soc_mem_index_count(unit, FT_L4PORTm)) && 
                                  (nume < array_size); i++) {
        SOC_IF_ERROR_RETURN(READ_FT_L4PORTm(unit, MEM_BLOCK_ANY, i, &ftl4m));
        valid = soc_mem_field32_get(unit, FT_L4PORTm, &ftl4m, VALIDf);
        if (!valid) {
            continue;
        }
        
        this_start = soc_mem_field32_get(unit, FT_L4PORTm, 
                                         &ftl4m, L4_PORT_LOWERf);
        this_end = soc_mem_field32_get(unit, FT_L4PORTm, 
                                         &ftl4m, L4_PORT_UPPERf);

        if (soc_mem_field32_get(unit, FT_L4PORTm, &ftl4m, TCPf)) {
            l4low[nume] = this_start;
            l4high[nume] = this_end;
            protocol[nume] = BCM_TR3_PROTOCOL_TCP;
            nume++;
            if (nume == array_size) {
                break;
            }
        }
        if (soc_mem_field32_get(unit, FT_L4PORTm, &ftl4m, UDPf)) {
            l4low[nume] = this_start;
            l4high[nume] = this_end;
            protocol[nume] = BCM_TR3_PROTOCOL_UDP;
            nume++;
            if (nume == array_size) {
                break;
            }
        }
    }

    BCM_REGEX_UNLOCK(unit);

    *array_count = nume;
    return BCM_E_NONE;
}

/* remap tables */
static uint8 *cmap_lower_tbl = NULL;

/* tr3 engine */
STATIC int _bcm_tr3_engine_reset(int unit, int eng_idx)
{
    uint32  regval;

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, &regval));

    /* reset the regex engines and clear out the state memories. */
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, 
                      &regval, REGEX_RESETf, 1);
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, 
                      &regval, REGEX_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, regval));
    return BCM_E_NONE;
}

/* CHECK : this just returns start of dfa , change name */
STATIC int _bcm_tr3_get_engine_info(int unit, int eng_idx, 
                                    _bcm_regex_engine_obj_t *pinfo)
{
    uint32  regval;

    pinfo->flags = 0;
    pinfo->req_lsz = 0;
    pinfo->from_line = -1;

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, &regval));
    if (soc_reg_field_get(unit, AXP_SM_REGEX_CONTROL0r, regval, REGEX_ENABLEf)) {
        pinfo->flags = BCM_TR3_REGEX_ENGINE_ALLOC;
        /* read the regex code pointers */
        SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL1r(unit, eng_idx, &regval));
        pinfo->from_line = soc_reg_field_get(unit, AXP_SM_REGEX_CONTROL1r, 
                                             regval, REGEX_START_PTR0f);
    }
    return BCM_E_NONE;
}

#define BCM_REGEX_SM_LINE2_OFFSET(l) ((l) * BCM_TR3_NUM_RV_PER_LINE)

STATIC int 
_bcm_tr3_engine_enable(int unit, _bcm_regex_engine_obj_t *pengine)
{
    uint32  regval, regval1, start_addr;
    int     eng_idx;

    eng_idx = pengine->hw_idx;

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, &regval));
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, &regval, REGEX_RESETf, 0);
    if (soc_reg_field_get(unit, AXP_SM_REGEX_CONTROL0r, regval, REGEX_ENABLEf)) {
    
        soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, &regval, REGEX_ENABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, regval));
    }

    /* program the regex program/state offsets. */
    start_addr = BCM_REGEX_SM_LINE2_OFFSET(pengine->from_line);
    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL1r(unit, eng_idx, &regval1));
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL1r, &regval1, REGEX_START_PTR0f,
                      start_addr);
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL1r, &regval1, REGEX_START_PTR1f,
                      start_addr);
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REGEX_CONTROL1r(unit, eng_idx, regval1));

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL2r(unit, eng_idx, &regval1));
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL2r, &regval1, REGEX_START_PTR3f,
                      start_addr);
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL2r, &regval1, REGEX_START_PTR2f,
                      start_addr);
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REGEX_CONTROL2r(unit, eng_idx, regval1));

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL3r(unit, eng_idx, &regval1));
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL3r, &regval1, REGEX_START_PTR4f,
                      start_addr);
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL3r, &regval1, REGEX_START_PTR5f,
                      start_addr);
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REGEX_CONTROL3r(unit, eng_idx, regval1));

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL4r(unit, eng_idx, &regval1));
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL4r, &regval1, REGEX_START_PTR7f,
                      start_addr);
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL4r, &regval1, REGEX_START_PTR6f,
                      start_addr);
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REGEX_CONTROL4r(unit, eng_idx, regval1));

    /* enable the engine */
    if (BCM_REGEX_CPS_ALLOCED(&pengine->cps)) {
        start_addr = BCM_REGEX_SM_LINE2_OFFSET(pengine->cps.from_line);
        soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, &regval, 
                            CONTEXT_BASE_PTRf, start_addr);
    } else {
        soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, &regval, 
                            CONTEXT_BASE_PTRf, 0);
    }
    
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, &regval, REGEX_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, regval));
    return BCM_E_NONE;
}

STATIC int _bcm_tr3_engine_disable(int unit, int eng_idx)
{
    uint32  regval;

    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, &regval));
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, &regval, REGEX_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, regval));
    return BCM_E_NONE;
}

static bcm_regex_desc_t tr3_regex_info = { 8, 
                                           4, 
                                           4,
                                           32,
                                           0x0,
                                           _bcm_tr3_engine_reset, 
                                           _bcm_tr3_get_engine_info,
                                           _bcm_tr3_engine_enable,
                                           _bcm_tr3_engine_disable,
                                         };

#define BCM_REGEX_ENGINE_ID(u,i)  (((u)<<16) | (i))
#define BCM_REGEX_ENGINE_INDEX_FROM_ID(u,id)  ((id) & 0xffff)

STATIC int
_bcm_tr3_find_engine(int unit, bcm_regex_engine_t engid,
                        _bcm_regex_lengine_t** plengine)
{
    int idx;
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_lengine_t *lengine = NULL;
    
    device = REGEX_INFO(unit);

    if (!device) {
        return BCM_E_UNAVAIL;
    }

    idx = BCM_REGEX_ENGINE_INDEX_FROM_ID(unit, engid);

    if (idx >= BCM_TR3_NUM_ENGINE(device)) {
        return BCM_E_PARAM;
    }

    lengine = BCM_TR3_LOGICAL_ENGINE(device, idx);
    if (!BCM_REGEX_ENGINE_ALLOCATED(lengine)) {
        return BCM_E_NOT_FOUND;
    }
    *plengine = lengine;
    return BCM_E_NONE;
}

int 
_bcm_tr3_regex_engine_create(int unit, bcm_regex_engine_config_t *config, 
                            bcm_regex_engine_t *engid)
{
    int i;
    _bcm_regex_lengine_t *lengine = NULL, *tmp;
    _bcm_tr3_regex_device_info_t *device;
   
    device = REGEX_INFO(unit);
    if (!device) {
        return BCM_E_INIT;
    }
    
    BCM_REGEX_LOCK(unit);
    /* 
     * When only Engine 0 is enabled the CSF table will not be enabled
     * causing failure in chaining. Enabling from engine 1 and
     * Engine 0 enabled at last
     */  
    for (i = 1; i < BCM_TR3_NUM_ENGINE(device); i++) {
        tmp = BCM_TR3_LOGICAL_ENGINE(device, i);
        if (BCM_REGEX_ENGINE_ALLOCATED(tmp)) {
            continue;
        }
        BCM_REGEX_ENGINE_SET_ALLOCATED(tmp);
        sal_memcpy(&tmp->config, config, sizeof(bcm_regex_engine_config_t));
        lengine = tmp;
        break;
    }
    if (i == BCM_TR3_NUM_ENGINE(device)) {
        tmp = BCM_TR3_LOGICAL_ENGINE(device, 0);
        if (!(BCM_REGEX_ENGINE_ALLOCATED(tmp))) {
            BCM_REGEX_ENGINE_SET_ALLOCATED(tmp);
            sal_memcpy(&tmp->config, config, sizeof(bcm_regex_engine_config_t));
            lengine = tmp;
        }
    }
    BCM_REGEX_UNLOCK(unit);
    if (lengine) {
        *engid = lengine->id;
    }
    return (lengine) ? BCM_E_NONE : BCM_E_RESOURCE;
}

STATIC int
_bcm_tr3_regex_hw_engine_detach(int unit, _bcm_regex_engine_obj_t *pengine)
{
    int num_alloc = 0, rv = BCM_E_NONE, i;
    uint32 regval;
    _bcm_regex_cps_obj_t *pcps;
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_engine_obj_t *temp_pengine = NULL;
#if 0
    _bcm_regexdb_cb_ua_t ua;
#endif
    _bcm_regexdb_entry_t en;

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "resetting engine..")));

    device = REGEX_INFO(unit);
    if (!pengine || !BCM_REGEX_ENGINE_ALLOCATED(pengine)) {
        return BCM_E_NONE;
    }

    for (i = 0; i < BCM_TR3_NUM_ENGINE(device); i++) {
        temp_pengine = BCM_TR3_PHYSICAL_ENGINE(device, i);
        if (BCM_REGEX_ENGINE_ALLOCATED(temp_pengine)) {
            num_alloc++;
        }
    }

    if (num_alloc == 1) {
        /*
         * Detaching the last engine. INT_SME_ENABLEf is set to 0 here 
         * so that the flow tracker does not direct packets to
         * Signature Matching block
         */
        SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit,&regval));
        soc_reg_field_set(unit, FT_CONFIGr, &regval, INT_SME_ENABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_FT_CONFIGr(unit,regval));
    }


    rv = BCM_REGEX_DISABLE_ENGINE(device, unit, pengine->hw_idx);

    if (SOC_FAILURE(rv)) {
        if (num_alloc == 1) {
            /*
             * Detaching the last engine failed. INT_SME_ENABLEf is set
             * back to 1 to let flow tracker direct packets to SME
             */
            SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit,&regval));
            soc_reg_field_set(unit, FT_CONFIGr, &regval, INT_SME_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_FT_CONFIGr(unit,regval));
        }
        return rv;
    }

    /*
     * unavail any check slots which depend on signatures in this engine.
     */
#if 0
    ua.avail = 0;
    ua.setid = BCM_TR3_SPECIAL_SLOT;
#endif
    en.engid = pengine->hw_idx;
    _bcm_regexdb_traverse(unit, BCM_REGEXDB_MATCH_ENGINE_ID, &en, 
                           _bcm_regexdb_engine_unavail_cb, NULL);

    bcm_regexdb_delete_by_engine_id(unit, pengine->hw_idx);

    pcps = BCM_TR3_CPS_INFO(device, pengine->hw_idx);
    _bcm_tr3_free_cps(unit, pcps);

    BCM_REGEX_ENGINE_SET_UNALLOCATED(pengine);
    pengine->from_line = -1;
    pengine->flags = 0;
    pengine->req_lsz = 0;

    return BCM_E_NONE;
}

int 
_bcm_tr3_regex_engine_destroy(int unit, bcm_regex_engine_t engid)
{
    int     idx, rv = BCM_E_NONE;
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_lengine_t *lengine = NULL;
    _bcm_regex_engine_obj_t *pengine = NULL;

    device = REGEX_INFO(unit);

    if (!device) {
        return BCM_E_UNAVAIL;
    }

    BCM_REGEX_LOCK(unit);

    idx = BCM_REGEX_ENGINE_INDEX_FROM_ID(unit, engid);
    lengine = BCM_TR3_LOGICAL_ENGINE(device, idx);
    if (!lengine) {
        return BCM_E_PARAM;
    }

    if (!BCM_REGEX_ENGINE_ALLOCATED(lengine)) {
        BCM_REGEX_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    pengine = lengine->physical_engine;

    /*
     * Disable the HW dfa engine and detach from logical engine 
 */
    rv = _bcm_tr3_regex_hw_engine_detach(unit, pengine);
    if (SOC_FAILURE(rv)) {
        BCM_REGEX_UNLOCK(unit);
        return rv;
    }
    lengine->physical_engine = NULL;

    BCM_REGEX_ENGINE_SET_UNALLOCATED(lengine);
    lengine->physical_engine = NULL;

    return rv;
}

int 
_bcm_tr3_regex_engine_traverse(int unit, bcm_regex_engine_traverse_cb cb, 
                                void *user_data)
{
    _bcm_tr3_regex_device_info_t *device;
    bcm_regex_engine_config_t     config;
    _bcm_regex_lengine_t *lengine = NULL;
    int i, rv = BCM_E_NONE, eng_idx;
    uint32  regval, fval;

    if ((device = REGEX_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    BCM_REGEX_LOCK(unit);

    for (i = 0; i < BCM_TR3_NUM_ENGINE(device); i++) {
        sal_memset(&config, 0, sizeof(config));
        lengine = BCM_TR3_LOGICAL_ENGINE(device, i);
        if (!BCM_REGEX_ENGINE_ALLOCATED(lengine)) {
            continue;
        }

        /* continue if no physical engine attached. */
        if (lengine->physical_engine == NULL) {
            rv = cb(unit, lengine->id, &config, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (rv) {
                break;
            }
#endif
            continue;
        }
        

        /* read the config from HW */
        eng_idx = lengine->physical_engine->hw_idx;
        rv = READ_AXP_SM_REGEX_CONTROL0r(unit, eng_idx, &regval);
        if (SOC_FAILURE(rv)) {
            goto fail;
        }
        fval = soc_reg_field_get(unit, AXP_SM_REGEX_CONTROL0r,
                                    regval, CONTEXT_STORAGE_MODEf);
        if (fval == 1) {
            config.flags |= BCM_REGEX_ENGINE_CONFIG_MULTI_PACKET;
        }
        rv = cb(unit, lengine->id, &config, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (rv) {
            break;
        }
#endif
    }

fail:
    BCM_REGEX_UNLOCK(unit);
    return rv;
}

int 
_bcm_tr3_regex_engine_get(int unit, bcm_regex_engine_t engid, 
                            bcm_regex_engine_config_t *config)
{
    _bcm_regex_lengine_t *lengine = NULL;
    uint32  regval, fval;

    SOC_IF_ERROR_RETURN(_bcm_tr3_find_engine(unit, engid, &lengine));

    sal_memcpy(config, &lengine->config, sizeof(bcm_regex_engine_config_t));

    if (lengine->physical_engine) {
        /* read the config from HW */
        SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL0r(unit, 
                                lengine->physical_engine->hw_idx, &regval));
        fval = soc_reg_field_get(unit, AXP_SM_REGEX_CONTROL0r,
                                 regval, CONTEXT_STORAGE_MODEf);
        if (fval == 1) {
            config->flags |= BCM_REGEX_ENGINE_CONFIG_MULTI_PACKET;
        }
    }
    return BCM_E_NONE;
}

STATIC soc_mem_t
_bcm_tr3_get_axp_sm_memory(int unit, int tile)
{
    soc_mem_t mem = -1;

    switch (tile) {
        case 0: mem = AXP_SM_STATE_TABLE_MEM0m; break;
        case 1: mem = AXP_SM_STATE_TABLE_MEM1m; break;
        case 2: mem = AXP_SM_STATE_TABLE_MEM2m; break;
        case 3: mem = AXP_SM_STATE_TABLE_MEM3m; break;
        case 4: mem = AXP_SM_STATE_TABLE_MEM4m; break;
        case 5: mem = AXP_SM_STATE_TABLE_MEM5m; break;
        case 6: mem = AXP_SM_STATE_TABLE_MEM6m; break;
        case 7: mem = AXP_SM_STATE_TABLE_MEM7m; break;
        default: break;
    }
    return mem;
}

/* CHECK */
#define BCM_TR3_ADD_TRANSITION_TO_STATE(t,s,c)    \
            (t)[(s)] = (((t)[(s)] & 0xc000) | (((t)[(s)] & 0x3fff) + (c)))

#define BCM_TR3_SET_STATE_TRANSITION_OFFSET(t,s,o)    \
            (t)[(s)] = (((t)[(s)] & 0xc000) | ((o) & 0x3fff))

#define BCM_TR3_NUM_STATE_TRANSITION(t,s)   ((t)[(s)] & 0x3fff)

#define BCM_TR3_NUM_STATE_OFFSET(t,s)   ((t)[(s)] & 0x3fff)

#define BCM_TR3_STATE_OFFSET(t,s)   ((t)[(s)] & 0x3fff)

#define BCM_TR3_DFA_STATE_F_FINAL   0x1
#define BCM_TR3_SET_STATE_FLAGS(t,s,f)    \
            (t)[(s)] = ((t)[(s)] & 0x3fff) | (((f) & 3) << 14)

#define BCM_TR3_GET_STATE_FLAGS(t,s,f)  (((t)[(s)] >> 14) & 3)

#define _BCM_TR3_SET_DEFAULT_POLICY 0x01

extern int _regex_policers_hw_alloc(int unit, _regex_policy_t *p_ent);

STATIC int
_bcm_tr3_install_action_policy(int unit, bcm_regex_match_t *match, 
                            uint32 flags, int *policy_index)
{
    int rv;
    uint32 hw_index;
    ft_policy_entry_t policy;
    void *entries[1];
    _regex_policy_t policy_ent;
    _field_policer_t *f_pl;
    _field_entry_policer_t *f_ent_pl;

    sal_memset(&policy, 0, sizeof(ft_policy_entry_t));

    if (match->action_flags & BCM_REGEX_MATCH_ACTION_DROP) {
        soc_mem_field32_set(unit, FT_POLICYm, &policy, R_DROPf, 1);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, Y_DROPf, 1);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, G_DROPf, 1);
    }

    if (match->action_flags & BCM_REGEX_MATCH_ACTION_COPY_TO_CPU) {
        soc_mem_field32_set(unit, FT_POLICYm, &policy, R_COPY_TO_CPUf, 1);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, Y_COPY_TO_CPUf, 1);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, G_COPY_TO_CPUf, 1);
    }

    if (match->action_flags & BCM_REGEX_MATCH_ACTION_INT_PRI) {
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                            R_COS_INT_PRIf, match->new_int_pri);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                            Y_COS_INT_PRIf, match->new_int_pri);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                            G_COS_INT_PRIf, match->new_int_pri);
    }

    if (match->action_flags & BCM_REGEX_MATCH_ACTION_DSCP) {
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                                    R_NEW_DSCPf, match->new_dscp);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                                    Y_NEW_DSCPf, match->new_dscp);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                                    G_NEW_DSCP_TOSf, match->new_dscp);
    }

    if (match->action_flags & BCM_REGEX_MATCH_ACTION_PKT_PRI) {
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                                    G_NEW_PKT_PRIf, match->new_pkt_pri);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, G_CHANGE_PKT_PRIf, 1);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                                    Y_NEW_PKT_PRIf, match->new_pkt_pri);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, Y_CHANGE_PKT_PRIf, 1);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, 
                                    R_NEW_PKT_PRIf, match->new_pkt_pri);
        soc_mem_field32_set(unit, FT_POLICYm, &policy, R_CHANGE_PKT_PRIf, 1);
    }

    if (match->policer_id > 0) {
        /* Create a dummy policy entry which is needed for the policer APIs below. */
        memset(&policy_ent, 0, sizeof(policy_ent));
        f_ent_pl = &policy_ent.policer[0];
        f_ent_pl->pid = match->policer_id;
        rv = _bcm_regex_policer_get(unit, f_ent_pl->pid, &f_pl);
        BCM_IF_ERROR_RETURN(rv);
        f_ent_pl->flags = f_pl->hw_flags;
        /* Normally this flag will be set when the policer is attached to a policy. */
        f_ent_pl->flags |= _FP_POLICER_VALID;
        /* Calculate the HW index and install the policer in the meter table. */
        _regex_policers_hw_alloc(unit, &policy_ent);
        BCM_IF_ERROR_RETURN(rv);
        /* Update the policy memory with the policer info. */
        rv = _regex_tr3_policer_action_set(unit, &policy_ent, policy.entry_data);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (flags & _BCM_TR3_SET_DEFAULT_POLICY) {
        rv = soc_mem_write(unit, FT_POLICYm, MEM_BLOCK_ALL, 0, &policy);
    } else {
        entries[0] = &policy;
        rv = soc_profile_mem_add(unit, _bcm_tr3_ft_policy_profile[unit], entries,
                                 1, &hw_index);
        if (!rv) {
            *policy_index = hw_index;
        }
    }
    return rv;
}

STATIC int
_bcm_tr3_set_match_action(int unit, int eng_idx, bcm_regex_match_t *match, 
                          int *match_idx, int match_set_id, int match_chk_id, int sigid)
{
    uint32 *alloc_bmap, fval;
    _bcm_tr3_regex_device_info_t *device;
    int     i, j, idx, policy_index = -1, rv = BCM_E_NONE;
    axp_sm_match_table_mem0_entry_t match_entry;
    int     defer_enable = 0;
    ft_age_profile_entry_t ftpm;
    int     age_profile = -1, ticks, max_index, anc_end = 0, ptlen;
    soc_mem_t mem;

    device = REGEX_INFO(unit);

    ptlen = sal_strlen(match->pattern);
    if (ptlen && (match->pattern[ptlen - 1] == '$') &&
        ((ptlen > 1) ? (match->pattern[ptlen - 2] != '\\') : 1)) {
        anc_end = 1;
    }

    /*
     * defer enabling the entry if check slot is not determined yet,
     * either the corresponding setid signature is not programmed.
     */
     if (match_chk_id == BCM_TR3_SPECIAL_SLOT) {
        defer_enable = 1;
     }

    /*
     * Alloc action index entry.
     */
    if ((match->action_flags & BCM_REGEX_MATCH_ACTION_IGNORE) == 0) {
        if (device->info->flags & BCM_REGEX_USE_POLICY_ID) {
            /*
            TBD: Check that the policy_id actually exists and is installed
            (getting the hardware ID would be sufficient, similar to what
            is already returned by _bcm_tr3_install_action_policy().  If
            not, return BCM_E_PARAM
            */
        }
        else {
        SOC_IF_ERROR_RETURN(_bcm_tr3_install_action_policy(unit, match, 0,
                                                        &policy_index));
        }
        /* if the flow is just ignore type, program the entry and enable
         * it now even if check slot is pernding */
         defer_enable = 0;
    }

    alloc_bmap = BCM_TR3_MATCH_BMAP_PTR(unit, device, eng_idx);
    mem = AXP_SM_MATCH_TABLE_MEM0m + BCM_REGEX_ENG2MATCH_MEMIDX(unit,eng_idx);
    idx = -1;
    for (i = 0; i < BCM_TR3_MATCH_BMAP_BSZ; i++) {
        if (alloc_bmap[i] == 0xffffffff) {
            continue;
        }
        for (j = 0; j < 32; j++) {
            if (alloc_bmap[i] & (1 << j)) {
                continue;
            }
            idx = (i*32) + j;
            break;
        }
        break;
    }

    if (idx < 0) {
        rv = BCM_E_NOT_FOUND;
        goto fail;
    }

    /* mark the entry as alloced */
    alloc_bmap[idx/32] |= (1 << (idx % 32));

    sal_memset(&match_entry, 0, sizeof(match_entry));

    soc_mem_field32_set(unit, mem, &match_entry, CHECK_LAST_BYTE_ANCHORf, anc_end);

    soc_mem_field32_set(unit, mem, &match_entry, REPORT_SIGNATURE_IDf, sigid);

    soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PROTOCOLf, 3);

    if (match->action_flags & BCM_REGEX_MATCH_ACTION_IGNORE) {
        soc_mem_field32_set(unit, mem, &match_entry, REPORT_KEEP_FLOWf, 0);
        soc_mem_field32_set(unit, mem, &match_entry, MATCH_REPORT_LEVELf, 2);
    } else {
        soc_mem_field32_set(unit, mem, &match_entry, MATCH_REPORT_LEVELf, 3);
        soc_mem_field32_set(unit, mem, &match_entry, REPORT_KEEP_FLOWf, 1);
        soc_mem_field32_set(unit, mem, &match_entry, REPORT_APPLY_ACTIONf, 1);
        if ((device->info->flags & BCM_REGEX_USE_POLICY_ID) == 0) {
        soc_mem_field32_set(unit, mem, &match_entry, REPORT_ACTION_INDEXf, 
                                                                policy_index);
    }
        else
        {
            soc_mem_field32_set(unit, mem, &match_entry, REPORT_ACTION_INDEXf,
                                                                    match->policy_id);
        }
    }

    if (match_set_id >= 0) {
        soc_mem_field32_set(unit, mem, 
                          &match_entry, MATCH_CROSS_SIG_FLAG_SETf, 1);
        soc_mem_field32_set(unit, mem, 
                        &match_entry, MATCH_CROSS_SIG_FLAG_INDEXf, match_set_id);
        soc_mem_field32_set(unit, mem, &match_entry, MATCH_REPORT_LEVELf, 0);
    }

    if ((match_chk_id >= 0) && (match_chk_id != BCM_TR3_SPECIAL_SLOT)) {
        soc_mem_field32_set(unit, mem, 
                            &match_entry, CHECK_CROSS_SIG_FLAG_IS_SETf, 1);

        soc_mem_field32_set(unit, mem, 
                            &match_entry, CHECK_CROSS_SIG_FLAG_INDEXf, match_chk_id);
    }

    if (match->action_flags & BCM_REGEX_MATCH_ACTION_NOTIFY_HEADER_ONLY) {
        soc_mem_field32_set(unit, mem, &match_entry, REPORT_PACKET_HEADER_ONLYf, 1);
    }

    if (match->action_flags & BCM_REGEX_MATCH_ACTION_NOTIFY_COPY_TO_CPU) {
        soc_mem_field32_set(unit, mem, &match_entry, REPORT_COPY_TO_CPUf, 1);
    }

    if (match->payload_length_min > 0) {
        soc_mem_field32_set(unit, mem, &match_entry, 
                            CHECK_L4_PAYLOAD_LENGTH_MINf, 
                            (match->payload_length_min & 0x3fff));
    } else {
        soc_mem_field32_set(unit, mem, &match_entry, 
                            CHECK_L4_PAYLOAD_LENGTH_MINf, 0);
    }

    if (match->payload_length_max >= 0) {
        soc_mem_field32_set(unit, mem, &match_entry, 
                            CHECK_L4_PAYLOAD_LENGTH_MAXf, 
                            (match->payload_length_max & 0x3fff));
    } else {
        soc_mem_field32_set(unit, mem, &match_entry, 
                            CHECK_L4_PAYLOAD_LENGTH_MAXf, 0x3fff);
    }

    if (match->flags & BCM_REGEX_MATCH_PKT_SEQUENCE) {
        soc_mem_field32_set(unit, mem, &match_entry, CHECK_CONTIGUOUS_FLOWf, 1);
    } else {
        soc_mem_field32_set(unit, mem, &match_entry, CHECK_CONTIGUOUS_FLOWf, 0);
    }

    fval = 3;
    if (match->flags & (BCM_REGEX_MATCH_TO_SERVER | BCM_REGEX_MATCH_TO_CLIENT)) {
        fval = (match->flags & (BCM_REGEX_MATCH_TO_SERVER | BCM_REGEX_MATCH_TO_CLIENT));
    }
    soc_mem_field32_set(unit, mem, &match_entry, CHECK_PKT_DIRECTIONf, fval);

    soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PORT_MAXf, 0xffff);
    if (match->flags & BCM_REGEX_MATCH_L4_SRC_PORT) {
        soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PORT_MINf, 
                                                        match->l4_src_port_low);
        soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PORT_MAXf, 
                                                        match->l4_src_port_high);
    } 
    if (match->flags & BCM_REGEX_MATCH_L4_DST_PORT) {
        soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PORT_MINf, 
                                                        match->l4_dst_port_low);
        soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PORT_MAXf, 
                                                        match->l4_dst_port_high);
    }
    soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PORT_VALUEf, 0xffff);
    soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PORT_MASKf, 0);
    if ((match->flags & BCM_REGEX_MATCH_L4_DST_PORT) && 
        (match->flags & BCM_REGEX_MATCH_L4_SRC_PORT)) {
        fval = 3;
    } else if (match->flags & BCM_REGEX_MATCH_L4_DST_PORT) {
        fval = 2;
    } else if (match->flags & BCM_REGEX_MATCH_L4_SRC_PORT) {
        fval = 1;
    } else { 
        fval = 3;
    }
    soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PORT_SELECTf, fval);

    fval = (match->flags & BCM_REGEX_MATCH_MULTI_FLOW) ? 1 : 0;
    soc_mem_field32_set(unit, mem, &match_entry, REPORT_HTTP_PERSISTENTf, fval);

    soc_mem_field32_set(unit, mem, &match_entry, REPORT_INACTIVITY_INDEXf, 0);

    soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PAYLOAD_LENGTH_MAXf, 
                                                                        0x3fff);

    soc_mem_field32_set(unit, mem, &match_entry, CHECK_L4_PAYLOAD_LENGTH_MINf, 0);

    soc_mem_field32_set(unit, mem, &match_entry, MATCH_PRIORITYf, match->priority);
    soc_mem_field32_set(unit, mem, &match_entry, CHECK_FIRST_PACKETf, 
                                                (match->sequence ? 0 : 1));

    soc_mem_field32_set(unit, mem, &match_entry, MATCH_ENTRY_VALIDf, 
                                                            !defer_enable);
   
    if (match->inactivity_timeout_usec >= 0)  {
        ticks = match->inactivity_timeout_usec/_REGEX_SESSION_AGE_PROFILE_TICKS_US;
        max_index = soc_mem_index_max(unit, FT_AGE_PROFILEm);
        for (i = max_index; i >= 0; i--) {
            SOC_IF_ERROR_RETURN(READ_FT_AGE_PROFILEm(unit, MEM_BLOCK_ALL,
                                                        i, &ftpm));
            if (ticks >= soc_mem_field32_get(unit, 
                                    FT_AGE_PROFILEm, &ftpm, TIMEOUTf)) {
                age_profile = (i == max_index) ? i : i + 1;
                break;
            }
        }
    }
    if (age_profile == -1) {
        age_profile = soc_mem_index_max(unit, FT_AGE_PROFILEm);
    }
   
    soc_mem_field32_set(unit, mem, &match_entry, 
                                    REPORT_INACTIVITY_INDEXf, age_profile);

    *match_idx = idx;

    SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, &match_entry));

    /* add to db */
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "Add entry to DB, sig=%d match=%d p=%d r=%d ms=%d mc=%d\n"),
              sigid, match->match_id, match->provides, 
              match->requires, match_set_id, match_chk_id));
    BCM_IF_ERROR_RETURN(bcm_regexdb_add_entry(unit, idx, eng_idx, match->match_id, 
                    sigid, match->provides, match->requires, match_set_id, match_chk_id));

    return BCM_E_NONE;

fail:
    return rv;
}

/*
cmap is used to keep track of all character classes used for
transitions.  If all 0-255 characters are used in a transition,
then no final "default" transition is necessary
*/

STATIC void _regex_cmap_init(_bcm_tr3_regex_data_t *cd)
{
    memset(cd->cmap, 0, sizeof(cd->cmap));
}

STATIC void _regex_cmap_add(_bcm_tr3_regex_data_t *cd, int from_c, int to_c)
{
    int i;

    for (i = from_c; i <= to_c; i++) {
        cd->cmap[i / BYTES2BITS(1)] |= 1 << (i % BYTES2BITS(1));
    }
}

STATIC int _regex_cmap_is_full(_bcm_tr3_regex_data_t *cd)
{
    int i;

    /* Check to see if there are transitions for all characters */
    for (i = 0; i < sizeof(cd->cmap); i++) {
        if (cd->cmap[i] != 0xff) {
            return 0;
        }
    }

    return 1;
}

STATIC regex_cb_error_t 
_bcm_regex_dfa_cb_calc_size(uint32 flags, int match_idx, int in_state, 
                           int from_c, int to_c, int to_state, 
                           int num_dfa_state, void *user_data)
{
    _bcm_tr3_regex_data_t *cd = (_bcm_tr3_regex_data_t*)user_data;

    if (flags & DFA_TRAVERSE_START) {
        cd->compile_size = 0;
        cd->last_state = 0;
        cd->last_flags = 0;
        _regex_cmap_init(cd);
        return REGEX_CB_OK;
    }

    if (flags & DFA_TRAVERSE_END) {
        return REGEX_CB_OK;
    }

    /* if last state is not same as this state, insert goto IDLE state if necessary*/
    if (cd->last_state != in_state) {
        /*
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META("State %s %d:\n"),
                  (match_idx>=0)? "[FINAL]" : ""),
                  in_state));
        */
        /*
        If last state is not same as this state, insert goto IDLE state
        for non-final states, and only if all characters don't already
        have a transition
        */
        if ((cd->last_flags & DFA_STATE_FINAL) == 0) {
            if (0 == _regex_cmap_is_full(cd)) {
        cd->compile_size++;
    }
        }
        _regex_cmap_init(cd);
    }

    /*
    if (to_state >= 0)
    {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META("\t %d -> %d [%d-%d];\n"),
                  in_state, to_state, from_c, to_c));
    }
    */
    cd->compile_size++;
    cd->last_state = in_state;
    cd->last_flags = flags;
    _regex_cmap_add(cd, from_c, to_c);
    return REGEX_CB_OK;
}

STATIC int
_bcm_tr3_regex_rv_fields_get(int unit, int eoff, soc_field_t *minf,
                             soc_field_t *maxf, soc_field_t *matchf,
                             soc_field_t *ptrf)
{
    *minf = RV_MIN0f + eoff;
    *maxf = RV_MAX0f + eoff;
    *matchf = RV_MATCHID0f + eoff;
    *ptrf = RV_PTR0f + eoff;
    return 0;
}

STATIC int
_bcm_tr3_regex_rv_set(int unit, soc_mem_t mem,
                      axp_sm_state_table_mem0_entry_t *line,
                      uint8 fromc, uint8 toc, int eoff,
                      uint32 jump_off, int match_index)
{
    soc_field_t minf, maxf, matchf, ptrf;

    _bcm_tr3_regex_rv_fields_get(unit, eoff, &minf, &maxf, &matchf, &ptrf);
    
    soc_mem_field32_set(unit, mem, line, minf, fromc);
    soc_mem_field32_set(unit, mem, line, maxf, toc);
    if (match_index >= 0) {
        soc_mem_field32_set(unit, mem, line, matchf, match_index);
    } else {
        soc_mem_field32_set(unit, mem, line, ptrf, jump_off);
    }
    return 0;
}

STATIC int
_bcm_tr3_regex_rv_get(int unit, soc_mem_t mem,
                      axp_sm_state_table_mem0_entry_t *line,
                      uint8 *fromc, uint8 *toc, int eoff,
                      uint32 *jump_off, int *match_index)
{
    soc_field_t minf, maxf, matchf, ptrf;

    _bcm_tr3_regex_rv_fields_get(unit, eoff, &minf, &maxf, &matchf, &ptrf);
    
    *match_index = -1;
    *jump_off = 0xffff;
    *fromc = soc_mem_field32_get(unit, mem, line, minf);
    *toc = soc_mem_field32_get(unit, mem, line, maxf);
    if ((*fromc == 255) && ((*toc == 1) || (*toc == 2))) {
        *match_index = soc_mem_field32_get(unit, mem, line, matchf);
    } else {
        *jump_off = soc_mem_field32_get(unit, mem, line, ptrf);
    }
    return 0;
}

STATIC regex_cb_error_t 
_bcm_tr3_regex_compiler_preprocess_cb(uint32 flags, int match_idx, 
                            int in_state, int from_c, int to_c, int to_state, 
                           int num_dfa_state, void *user_data)
{
    _bcm_tr3_regex_data_t *cd = (_bcm_tr3_regex_data_t*)user_data;
    int insert_fail = 0, i, offset, size, num_state;

    if (flags & DFA_TRAVERSE_START) {
        cd->last_state = 0;
        cd->last_flags = 0;
        _regex_cmap_init(cd);
        return REGEX_CB_OK;
    } else if (flags & DFA_TRAVERSE_END) {
        /* adjust the per state table data to make generate the
         * offset table. */
        offset = BCM_TR3_NUM_STATE_TRANSITION(cd->per_state_data, 0);
        BCM_TR3_SET_STATE_TRANSITION_OFFSET(cd->per_state_data,0,0);
        for (i=1; i<num_dfa_state; i++) {
            num_state = BCM_TR3_NUM_STATE_TRANSITION(cd->per_state_data, i);
            BCM_TR3_SET_STATE_TRANSITION_OFFSET(cd->per_state_data,i,offset);
            offset += num_state;
        }
        return REGEX_CB_OK;
    }

    /*
    If last state is not same as this state, insert goto IDLE state
    for non-final states, and only if all characters don't already
    have a transition
    */
    if (cd->last_state != in_state) {
        if ((cd->last_flags & DFA_STATE_FINAL) == 0) {
            if (0 == _regex_cmap_is_full(cd)) {
        insert_fail = 1;
    }
        }
        _regex_cmap_init(cd);
    }

    if (cd->per_state_data == NULL) {
        size = sizeof(cd->per_state_data[0]) * num_dfa_state;
        cd->per_state_data = sal_alloc(size, "dfa_state_tbl");
        sal_memset(cd->per_state_data, 0, size);
    }
    if (insert_fail) {
        BCM_TR3_ADD_TRANSITION_TO_STATE(cd->per_state_data, cd->last_state,1);
    }
    BCM_TR3_ADD_TRANSITION_TO_STATE(cd->per_state_data, in_state, 1);
    if (flags & DFA_STATE_FINAL) {
        BCM_TR3_SET_STATE_FLAGS(cd->per_state_data, 
                                in_state, BCM_TR3_DFA_STATE_F_FINAL);
    }
    cd->last_state = in_state;
    cd->last_flags = flags;
    _regex_cmap_add(cd, from_c, to_c);
    return REGEX_CB_OK;
}

STATIC regex_cb_error_t 
_bcm_tr3_regex_compiler_install_cb(uint32 flags, int match_idx, int in_state, 
                           int from_c, int to_c, int to_state, 
                           int num_dfa_state, void *user_data)
{
    _bcm_tr3_regex_data_t *cd = (_bcm_tr3_regex_data_t*)user_data;
    int insert_fail = 0, i, j, unit, jf, hw_match_idx;
    int match_set_id, match_chk_id, sigid;
    uint16      ptr;

    unit = cd->unit;
    
    if (flags & DFA_TRAVERSE_START) {
        cd->last_state = 0;
        cd->last_flags = 0;
        _regex_cmap_init(cd);
        cd->line_dirty = 0;
        cd->wr_off = 0;
        cd->line_off = cd->hw_eng->from_line;
        cd->hw_match_idx = sal_alloc(sizeof(int)*cd->num_match, "hw_match_idx");
        for (i=0; i<cd->num_match; i++) {
            cd->hw_match_idx[i] = -1;
        }
        cd->mem = _bcm_tr3_get_axp_sm_memory(unit, cd->hw_eng->tile);
        if (cd->mem < 0) {
            return REGEX_CB_ABORT;
        }
        _regex_cmap_init(cd);
        return REGEX_CB_OK;
    } else if (flags & DFA_TRAVERSE_END) {
        if (cd->line_dirty) {
            /* fill rest of the line with fail statements */
            for (i=cd->wr_off; i<BCM_TR3_NUM_RV_PER_LINE; i++) {
                _bcm_tr3_regex_rv_set(unit, cd->mem, &cd->line, 0, 
                                        255, i, 0xffff, -1);
            }
	    soc_mem_write(unit, cd->mem, MEM_BLOCK_ALL, cd->line_off, &cd->line);
        }
        if (cd->per_state_data) {
            sal_free(cd->per_state_data);
            cd->per_state_data = NULL;
        }
        if (cd->hw_match_idx) {
            sal_free(cd->hw_match_idx);
            cd->hw_match_idx = NULL;
        }
        return REGEX_CB_OK;
    }

    /*
    If last state is not same as this state, insert goto IDLE state
    for non-final states, and only if all characters don't already
    have a transition
    */
    if (cd->last_state != in_state) {
        if ((cd->last_flags & DFA_STATE_FINAL) == 0) {
            if (0 == _regex_cmap_is_full(cd)) {
        insert_fail = 1;
    }
        }
        _regex_cmap_init(cd);
    }

    /* INSTALL */
    jf = 2 | (insert_fail ? 1 : 0);
    for (j=0; j<2; j++) {
        if ((jf & (1 << j)) == 0) {
            continue;
        }

        if ((cd->wr_off % BCM_TR3_NUM_RV_PER_LINE) == 0) {
            if (cd->line_dirty) {
                soc_mem_write(unit, cd->mem, 
                                MEM_BLOCK_ALL, cd->line_off, &cd->line);
                cd->line_off++;
                cd->line_dirty = 0;
            }
            sal_memset(&cd->line, 0, sizeof(axp_sm_state_table_mem0_entry_t));
            soc_mem_field32_set(unit, cd->mem, &cd->line, STATE_DATA_TYPEf, 1);
            cd->wr_off = 0;
        }

        if (j == 1) {
            /* encode an rv entry */
            if (match_idx >= 0) {
                /*
                Note that the following code will be run multiple times for a FINAL
                state that has transitions.  To avoid installing multiple 255/2
                RVs, FINAL states should have all transitions removed first
                */
                match_set_id = cd->match_set_ids ? cd->match_set_ids[match_idx] : -1;
                match_chk_id = cd->match_check_ids ? cd->match_check_ids[match_idx] : -1;
                sigid = cd->sig_ids[match_idx];
                hw_match_idx = cd->hw_match_idx[match_idx];
                if (hw_match_idx == -1) {
                    if (_bcm_tr3_set_match_action(unit, cd->hw_eng->hw_idx, 
                                              &cd->matches[match_idx], 
                                              &hw_match_idx, 
                                              match_set_id, match_chk_id, sigid)) {
                        LOG_INFO(BSL_LS_BCM_REGEX,
                                 (BSL_META("error\n")));
                        return REGEX_CB_ABORT;
                    }
                    cd->hw_match_idx[match_idx] = hw_match_idx;
                }

                _bcm_tr3_regex_rv_set(unit, cd->mem, &cd->line, 255, 2, 
                                      cd->wr_off, 0, hw_match_idx);

            } else {
                ptr = (cd->hw_eng->from_line * BCM_TR3_NUM_RV_PER_LINE) +
                        BCM_TR3_STATE_OFFSET(cd->per_state_data, to_state);
                _bcm_tr3_regex_rv_set(unit, cd->mem, &cd->line, from_c, 
                                        to_c, cd->wr_off, ptr, -1);
            }
        } else {
            _bcm_tr3_regex_rv_set(unit, cd->mem, &cd->line, 0, 
                                        255, cd->wr_off, 0xffff, -1);
        }
        cd->line_dirty = 1;
        cd->wr_off++;
    }
    cd->last_state = in_state;
    cd->last_flags = flags;
    _regex_cmap_add(cd, from_c, to_c);
    return REGEX_CB_OK;
}

/* Note: the caller should allocate enough state line to read the dfa */
STATIC int
_bcm_tr3_regex_read_hw_dfa(int unit, _bcm_regex_engine_obj_t *pengine,
                           axp_sm_state_table_mem0_entry_t *pmem)
{
    int    i;
    int    rv;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;

    mem = _bcm_tr3_get_axp_sm_memory(unit, pengine->tile);

    for (i = pengine->from_line; i < pengine->from_line + pengine->req_lsz; i++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, i, entry);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_BCM_REGEX,
                      (BSL_META_U(unit,
                                  "Read ERROR: table %s[%d]: %s\n"),
                       SOC_MEM_UFNAME(unit, mem),
                       i, soc_errmsg(i)));
            break;
        }
        memcpy(pmem[i - pengine->from_line].entry_data, entry, sizeof(pmem[0].entry_data));
    }

    return BCM_E_NONE;
}

/*
 * Rearrange the engines and optimize the use of tile resources,
 * so that all the states in the tiles are utilized to fullest.
 */
STATIC int
_bcm_tr3_rearrage_engines(int unit)
{
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_resource_t *res;
    int rv = BCM_E_NONE;
    int tiles, tilee, num_res;
    _bcm_regex_tile_t  tmp_tiles[BCM_TR3_MAX_TILES];

    device = REGEX_INFO(unit);

    tiles = 0;
    tilee = device->info->num_tiles - 1;

    _bcm_tr3_make_resource_objs(unit, &res, &num_res, tiles, tilee);

    if (num_res == 0) {
        goto done;
    }

    _bcm_regex_sort_resources_by_size(unit, res, num_res);

    _bcm_tr3_arrange_resources_in_tiles(unit, res, num_res, tiles, tilee,
                                            tmp_tiles, BCM_TR3_MAX_TILES);

    rv = _bcm_tr3_rearrange_engines_in_hw(unit, res, num_res, 
                                               tiles, tilee, tmp_tiles);
done:
    return rv;
}

STATIC int _bcm_tr3_get_tile_line_size(int unit)
{
    return soc_mem_index_count(unit, AXP_SM_STATE_TABLE_MEM0m);
}

STATIC int _bcm_tr3_get_cps_line_size(int unit)
{
    uint32  regval, fval, cslsize;

    SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit,&regval));
    fval = soc_reg_field_get(unit, FT_CONFIGr, regval, TABLE_SIZEf);
    cslsize = (_decode_max_flow(fval) * 2)/BCM_TR3_LINE_BYTE_SIZE;
    return cslsize;
}

STATIC int
_bcm_tr3_calc_dfa_memsize(int unit, char **re, uint32 *re_flags, 
                          int num_pattern, uint32 flags, 
                         int *hw_states, void **ppdfa)
{
    _bcm_tr3_regex_data_t cdata;
    void    *pdfa;
    int                    rv;

    rv = bcm_regex_compile(re, re_flags, num_pattern, 0, &pdfa);
    if (rv < 0) {
        switch (rv) {
            case REGEX_ERROR_INVALID_CLASS:
                LOG_WARN(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "Regex: Invalid class.\n")));
                break;
            case REGEX_ERROR_EXPANSION_FAIL:
                LOG_WARN(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "Regex: Expansion failed.\n")));
                break;
            case REGEX_ERROR_NO_POST:
                LOG_WARN(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "Regex: No post.\n")));
                break;
            case REGEX_ERROR_NO_DFA:
                LOG_WARN(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "Regex: No DFA.\n")));
                break;
            case REGEX_ERROR_NO_NFA:
                LOG_WARN(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "Regex: No NFA.\n")));
                break;
            case REGEX_ERROR_NO_MEMORY:
                LOG_WARN(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "Regex: No memory.\n")));
                break;
            default:
                LOG_WARN(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "Regex: Error.\n")));
                break;
        }
        return BCM_E_FAIL;
    }

    sal_memset(&cdata, 0, sizeof(_bcm_tr3_regex_data_t));
    cdata.unit = unit;

    if (bcm_regex_dfa_traverse(pdfa, 
                                _bcm_regex_dfa_cb_calc_size, &cdata)) {
        return BCM_E_INTERNAL;
    }

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "DFA num hw_states/transitions = %d, %d lines\n"),
              cdata.compile_size,
              cdata.compile_size/BCM_TR3_NUM_RV_PER_LINE +
              (((cdata.compile_size/BCM_TR3_NUM_RV_PER_LINE) % BCM_TR3_NUM_RV_PER_LINE) ? 1 : 0)));

    *hw_states = cdata.compile_size;
    if (ppdfa != NULL) {
       *ppdfa = pdfa;
    } else {
        bcm_regex_dfa_free(pdfa);    
    }
    return BCM_E_NONE;
}

int _bcm_tr3_get_engine_size_info(int unit, int engine_id,
                                  bcm_regex_engine_info_t *engine_info)
{
    uint32  regval;
    _bcm_regex_lengine_t *ltmp = NULL;
    _bcm_regex_engine_obj_t *ptmp = NULL;
    _bcm_tr3_regex_device_info_t *device;
    int i, in_use, tile;
    int max_tile_free = 0, tile_free_size[BCM_TR3_MAX_TILES];
    int tile_eng[BCM_TR3_MAX_TILES] = {0};

    device = REGEX_INFO(unit);

    if (engine_id == BCM_REGEX_ENGINE_ALL) {
        engine_info->size = _bcm_tr3_get_tile_line_size(unit) * BCM_TR3_MAX_TILES * BCM_TR3_LINE_BYTE_SIZE;
        in_use = 0;

        for (i = 0; i < BCM_TR3_NUM_ENGINE(device); i++) {
            ltmp = BCM_TR3_LOGICAL_ENGINE(device, i);
            if (ltmp == NULL) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "i = %d, logical engine NULL\n"),
                          i));
                continue;
            }
            if (!BCM_REGEX_ENGINE_ALLOCATED(ltmp)) {
                continue;
            }
            ptmp = ltmp->physical_engine;
            if (ptmp == NULL) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "i = %d, Engine id %d : physical engine NULL\n"),
                          i, ltmp->id));
                continue;
            }
            in_use += ptmp->total_lsz;
        }

        engine_info->free_size = ((_bcm_tr3_get_tile_line_size(unit) * BCM_TR3_MAX_TILES) - in_use)
                                    * BCM_TR3_LINE_BYTE_SIZE;
    }
    else
    {
        if (engine_id < 0 || engine_id >= BCM_TR3_MAX_ENGINE) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "engine_id = %d, INVALID\n"),
                      engine_id));
            return BCM_E_PARAM;
        }

        if (_bcm_tr3_find_engine(unit, engine_id, &ltmp) ||
            ((ptmp = ltmp->physical_engine) == NULL)) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "engine_id = %d, Engine logical id %d : physical engine NULL\n"),
                      engine_id, ltmp->id));

            /*
            Search for tile with largest free size available
            */

            for (i = 0; i < BCM_TR3_MAX_TILES; i++) {
                tile_free_size[i] = _bcm_tr3_get_tile_line_size(unit) * BCM_TR3_LINE_BYTE_SIZE;
            }

            for (i = 0; i < BCM_TR3_NUM_ENGINE(device); i++) {
                ltmp = BCM_TR3_LOGICAL_ENGINE(device, i);
                if (ltmp == NULL) {
                    continue;
                }
                if (!BCM_REGEX_ENGINE_ALLOCATED(ltmp)) {
                    continue;
                }
                ptmp = ltmp->physical_engine;
                if (ptmp == NULL) {
                    LOG_INFO(BSL_LS_BCM_REGEX,
                             (BSL_META_U(unit,
                                         "i = %d, Engine id %d, pid NULL\n"),
                              i, ltmp->id));
                    continue;
                }
                tile = BCM_TR3_ENGINE_TILE(device, ptmp->hw_idx);
                tile_free_size[tile] -= (ptmp->total_lsz*BCM_TR3_LINE_BYTE_SIZE);
                tile_eng[tile]++;
            }

            for (i = 0; i < BCM_TR3_MAX_TILES; i++) {
                if ((tile_free_size[i] > max_tile_free) &&
                    tile_eng[i] < BCM_TR3_ENGINE_PER_TILE(device)) {
                    max_tile_free = tile_free_size[i];
                }
            }

            engine_info->size = _bcm_tr3_get_tile_line_size(unit) * BCM_TR3_LINE_BYTE_SIZE;
            engine_info->free_size = max_tile_free;
            LOG_CLI((BSL_META_U(unit,
                                "Engine %d not allocated, max size in tile would be engine size:\n"),
                     engine_id));
            return BCM_E_NONE;
        }

        SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL0r(unit, ptmp->hw_idx, &regval));

        if (!soc_reg_field_get(unit, AXP_SM_REGEX_CONTROL0r, regval, REGEX_ENABLEf)) {
            return BCM_E_NONE;
        }

        engine_info->size = _bcm_tr3_get_tile_line_size(unit) * BCM_TR3_LINE_BYTE_SIZE;
        in_use = 0;
        tile = BCM_TR3_ENGINE_TILE(device, ptmp->hw_idx);

        for (i = 0; i < BCM_TR3_NUM_ENGINE(device); i++) {
            ltmp = BCM_TR3_LOGICAL_ENGINE(device, i);
            if (!BCM_REGEX_ENGINE_ALLOCATED(ltmp)) {
                continue;
            }
            ptmp = ltmp->physical_engine;
            if (ptmp == NULL) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "i = %d, Engine id %d, pid NULL\n"),
                          i, ltmp->id));
                continue;
            }
            if (ptmp->tile != tile) {
                continue;
            }
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "i = %d, Engine id %d, pid %d : tile %d, size %d\n"),
                      i,
                      ltmp->id, ptmp->hw_idx, ptmp->tile, ptmp->req_lsz * BCM_TR3_LINE_BYTE_SIZE));
            in_use += ptmp->total_lsz;
        }

        engine_info->free_size = (_bcm_tr3_get_tile_line_size(unit) - in_use)
                                    * BCM_TR3_LINE_BYTE_SIZE;
    }

    return BCM_E_NONE;
}
        
STATIC int
_bcm_tr3_set_remap_table(int unit, int hw_idx, uint8 *cmap_tbl)
{
    int i;
    soc_mem_t remap_mem = AXP_SM_CHAR_REMAP0m + hw_idx;
    axp_sm_char_remap0_entry_t remap_en;
    uint32  regval;
   
    if (cmap_tbl) {
        for (i=0; i<256; i+=4) {
            soc_mem_field32_set(unit, remap_mem, &remap_en, 
                              REMAP_VAL0f, *(cmap_tbl+i));
            soc_mem_field32_set(unit, remap_mem, &remap_en, 
                              REMAP_VAL1f, *(cmap_tbl + i+1));
            soc_mem_field32_set(unit, remap_mem, &remap_en, 
                              REMAP_VAL2f, *(cmap_tbl + i+2));
            soc_mem_field32_set(unit, remap_mem, &remap_en, 
                              REMAP_VAL3f, *(cmap_tbl + i+3));
            soc_mem_write(unit, remap_mem, MEM_BLOCK_ALL, i/4, &remap_en);
        }
    }
    SOC_IF_ERROR_RETURN(READ_AXP_SM_REGEX_CONTROL0r(unit, hw_idx, &regval));
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, 
                                  &regval, CHAR_REMAP_ENABLEf, cmap_tbl?1:0);
    SOC_IF_ERROR_RETURN(
                    WRITE_AXP_SM_REGEX_CONTROL0r(unit, hw_idx, regval));
    return 0;
}

STATIC int
_bcm_tr3_install_dfa(int unit, void *dfa, bcm_regex_match_t *matches,
                     int *match_set_ids, int *match_check_ids, int *sig_ids,
                     int count, _bcm_regex_engine_obj_t* pengine)
{
    _bcm_tr3_regex_data_t user_data;

    /* Install is done in 2 stages, first stage, preprocess stage
     * calculate the enteries required per state including any fail states.
     * this will create a map :
     *  stateA-> num_hw_state, offset in state_table
     *  stateB-> num_hw_state, offset in state_table
     *  using this its easy to generate the jump pointers to next state.
     */

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "installing DFA\n")));
    /*
     * Traverse al the DFA with flags preprocess */
    sal_memset(&user_data, 0, sizeof(user_data));
    user_data.matches = matches;
    user_data.num_match = count;
    user_data.match_set_ids = match_set_ids;
    user_data.match_check_ids = match_check_ids;
    user_data.sig_ids = sig_ids;
    user_data.unit = unit;
    user_data.hw_eng = pengine;

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "preprocessing ..")));
    if (bcm_regex_dfa_traverse(dfa, _bcm_tr3_regex_compiler_preprocess_cb,
                                    &user_data)) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "Error\n")));
        return BCM_E_INTERNAL;
    }

    /* traverse again and install it */
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "HW Install ..")));
    if (bcm_regex_dfa_traverse(dfa, _bcm_tr3_regex_compiler_install_cb,
                                    &user_data)) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "Error\n")));
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_tr3_disable_tile(int unit, int tile)
{
    _bcm_tr3_regex_device_info_t *device;
    int e, eng_idx;
    
    device = REGEX_INFO(unit);
    for (e = 0; e < BCM_TR3_ENGINE_PER_TILE(device); e++) {
        eng_idx = (tile*BCM_TR3_ENGINE_PER_TILE(device)) + e;
        BCM_REGEX_DISABLE_ENGINE(device, unit, eng_idx);
    }

    return 0;
}

STATIC int
_bcm_tr3_enable_tile(int unit, int tile)
{
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_engine_obj_t      *pengine;
    int e, eng_idx;
    
    device = REGEX_INFO(unit);
    for (e = 0; e < BCM_TR3_ENGINE_PER_TILE(device); e++) {
        eng_idx = (tile*BCM_TR3_ENGINE_PER_TILE(device)) + e;
        pengine = BCM_TR3_PHYSICAL_ENGINE(device, eng_idx); 
        if (!BCM_REGEX_ENGINE_ALLOCATED(pengine)) {
            continue;
        }
        BCM_REGEX_ENABLE_ENGINE(device, unit, pengine);
    }

    return 0;
}

STATIC int 
_bcm_tr3_init_cps_block(int unit, _bcm_regex_cps_obj_t *pcps)
{
    int     i;

    for (i = 0; i < BCM_REGEX_NUM_SLOT_PER_CPS; i++) {
        pcps->alloc_map[i] = -1;
        pcps->refcnt[i] = 0;
    }
    return BCM_E_NONE;
}

STATIC int _bcm_tr3_free_cps(int unit, _bcm_regex_cps_obj_t *pcps)
{
    int i;

    for (i = 0; i < BCM_REGEX_NUM_SLOT_PER_CPS; i++) {
        pcps->alloc_map[i] = -1;
        pcps->refcnt[i] = 0;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_regex_alloc_one_cps_slot(int unit, int linkid, 
                              _bcm_regex_cps_obj_t *pcps, int *slot)
{
    int j;

    if (linkid < 0) {
        return BCM_E_PARAM;
    }

    for (j=0; j<BCM_REGEX_NUM_SLOT_PER_CPS; j++) {
        if (pcps->alloc_map[j] == linkid) {
            *slot = pcps->foff + j;
            pcps->alloc_map[j] = linkid;
            pcps->refcnt[j]++;
            return BCM_E_NONE;
        }
    }

    for (j=0; j<BCM_REGEX_NUM_SLOT_PER_CPS; j++) {
        if (pcps->alloc_map[j] == -1) {
            *slot = pcps->foff + j;
            pcps->alloc_map[j] = linkid;
            pcps->refcnt[j] = 1;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

STATIC int
_bcm_regex_free_one_cps_slot(int unit, _bcm_regex_cps_obj_t *pcps, int slot)
{
    int b, idx;
    
    b = (slot/BCM_REGEX_NUM_SLOT_PER_CPS)*BCM_REGEX_NUM_SLOT_PER_CPS;
    if (pcps->foff != b) {
        return BCM_E_PARAM;
    }
   
    idx = slot % BCM_REGEX_NUM_SLOT_PER_CPS;
    if (pcps->refcnt[idx]) {
        pcps->refcnt[idx]--;
        if (pcps->refcnt[idx] == 0) {
            pcps->alloc_map[idx] = -1;
        }
    }
    
    return BCM_E_RESOURCE;
}

STATIC int
_bcm_tr3_regex_alloc_set_ids(int unit, int *match_set_ids, bcm_regex_match_t *matches,
                            int count, int pengine_hwidx)
{
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_cps_obj_t *pcps;
    int si, rv = BCM_E_NONE;

    device = REGEX_INFO(unit);
    pcps = BCM_TR3_CPS_INFO(device, pengine_hwidx);

    for (si = 0; si < count; si++) {
        if (matches[si].provides < 0) { /* setids */
            match_set_ids[si] = -1;
            continue;
        }

        /* Multiple signature might set the same flag, in which case 
         * reuse the flag, ie 
         * S{a} -> sets f1
         * S{b} -> sets f1
         * S{c} -> checks for f1, something similar to 
         * match = (S{a} || S{bb}) && S{c}
         */
        rv = _bcm_regex_alloc_one_cps_slot(unit, matches[si].provides,
                                                pcps, &match_set_ids[si]);
        if (rv) {
            break;
        }
    }

    if (rv) {
        for (si = 0; si < count; si++) {
            if (match_set_ids[si] == -1) {
                continue;
            }
            _bcm_regex_free_one_cps_slot(unit, pcps, match_set_ids[si]);
        }
        _bcm_tr3_free_cps(unit, pcps);
    }
    return rv;
}

STATIC int
_bcm_tr3_regex_alloc_check_ids(int unit, int *match_check_ids, bcm_regex_match_t *matches,
                            int count, int pengine_hwidx, int defer)
{
    int rv = BCM_E_NONE, i;

    for (i = 0; i < count; i++) {
        if (matches[i].requires < 0) { /* chkids */
            match_check_ids[i] = -1;
            continue;
        }
        rv = bcm_regexdb_find_chkid_for_requires(unit, 
                                        matches[i].requires, &match_check_ids[i]);
        if (rv) {
            if (defer) {
                match_check_ids[i] = BCM_TR3_SPECIAL_SLOT;
            } else {
                return BCM_E_RESOURCE;
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_tr3_attach_hw_engine(int unit, _bcm_regex_lengine_t *lengine,
                          int req_lsz, int *match_set_ids, int *match_check_ids,
                          bcm_regex_match_t *matches, int *sigids, 
                          int count)
{
    int rv = BCM_E_NONE, try, engidx, i;
    bcm_regex_engine_config_t config;
    _bcm_regex_engine_obj_t *pengine = NULL;
    int  extra_lsz = 0;

    engidx = BCM_REGEX_ENGINE_INDEX_FROM_ID(unit, lengine->id);

    /*
     * Cannot do both multipacket and cross packet mode 
     */
    SOC_IF_ERROR_RETURN(_bcm_tr3_regex_engine_get(unit, engidx, &config));
    if ((config.flags & BCM_REGEX_ENGINE_CONFIG_MULTI_PACKET) && 
        (match_set_ids || match_check_ids)) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "Can't use multi-packet with chaining\n")));
        return BCM_E_CONFIG;
    }

    if ((config.flags & BCM_REGEX_ENGINE_CONFIG_MULTI_PACKET) || match_set_ids ||
         match_check_ids) {
        extra_lsz = _bcm_tr3_get_cps_line_size(unit);
    }
    
    for(pengine=NULL, try=0; (try<2) && !pengine; try++) {
        rv = _bcm_tr3_alloc_regex_engine(unit, req_lsz, extra_lsz, &pengine, -1);
        if (rv && (try == 0)) {
            /* first try and alloc failed, try to rearrange engines
             * across tiles and optimize resources to see if we can still
             * fit things.
             */
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "Alloc of regex engine failed.. rearrange engines\n")));
            rv = _bcm_tr3_rearrage_engines(unit);
            if (rv) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "rearrange engines failed.\n")));
                goto fail;
            }
        }
    }

    /* allocate signature IDs, this is internal ID for a pattern and maps
     * 1 to 1 to match_id provided by the application. The reason for the
     * remapping is , internal signature ID space is limited whereas 
     * application can pick arbitary values for the match-id.
     */
    for (i = 0; i < count; i++) {
        sigids[i] = _bcm_tr3_alloc_sigid(unit, &matches[i]);
    }

    /*
     * Allocate set slots if required 
     */
    if (match_set_ids) {
        rv = _bcm_tr3_regex_alloc_set_ids(unit, match_set_ids,
                                           matches, count, pengine->hw_idx);
        if (rv) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "Alloc of set_ids failed\n")));
            goto fail;
        }
    }

    /*
     * allocate check slots if required 
     */
    if (match_check_ids) {
        rv = _bcm_tr3_regex_alloc_check_ids(unit, match_check_ids,
                                            matches, count, pengine->hw_idx, 1);
        if (rv) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "Alloc of check_ids failed\n")));
            goto fail;
        }
    }

    lengine->physical_engine = pengine;

    return BCM_E_NONE;
    
fail:

    if (pengine) {
        _bcm_tr3_regex_hw_engine_detach(unit, pengine);
    }

    return rv;
}

int
_bcm_tr3_regex_match_check(int unit, bcm_regex_match_t *matches,
                           int count, int *metric)
{
    char **res;
    uint32   *res_flags;
    int     i, hw_states, ncs = 0, nncs = 0, rv = BCM_E_NONE;
    int need_check_ids = 0, need_set_ids = 0, extra_lsz=0;

    if ((count <= 0) || (!matches) || (!metric))  {
        return BCM_E_PARAM;
    }

    res = sal_alloc(count * sizeof(char*), "tmp_regex_patterns");
    res_flags = sal_alloc(count * sizeof(uint32), "tmp_regex_ptr");

    for (i=0; i<count; i++) {
        res[i] = matches[i].pattern;
        res_flags[i] = 0;
        if (matches[i].flags & BCM_REGEX_MATCH_CASE_INSENSITIVE) {
            nncs++;
        } else {
            ncs++;
        }
        if (matches[i].provides > 0) {
            need_set_ids += 1;
        }
        if (matches[i].requires > 0) {
            need_check_ids += 1;
        }
    }

#if 1
    if (ncs && nncs) {
        for (i = 0; i < count; i++) {
            if (matches[i].flags & BCM_REGEX_MATCH_CASE_INSENSITIVE) {
                res_flags[i] = BCM_TR3_REGEX_CFLAG_EXPAND_LCUC;
            }
        }
    }
#else
    if (nncs) {
        for (i = 0; i < count; i++) {
            if (matches[i].flags & BCM_REGEX_MATCH_CASE_INSENSITIVE) {
                res_flags[i] = BCM_TR3_REGEX_CFLAG_EXPAND_LCUC;
            }
        }
    }
#endif

    if (!ncs && nncs) {
        for (i = 0; i < count; i++) {
            if (matches[i].flags & BCM_REGEX_MATCH_CASE_INSENSITIVE) {
                res_flags[i] = BCM_TR3_REGEX_CFLAG_EXPAND_LC;
            }
        }
    }
    
    BCM_REGEX_LOCK(unit);

    rv = _bcm_tr3_calc_dfa_memsize(unit, res, res_flags, count, 
                                    0, &hw_states, NULL);
    if (!rv && (hw_states >= 0)) {
        *metric = ((hw_states / BCM_TR3_NUM_RV_PER_LINE) +
            (((hw_states % BCM_TR3_NUM_RV_PER_LINE) == 0) ? 0 : 1)) *
            BCM_TR3_LINE_BYTE_SIZE;
    }
    if(need_set_ids || need_check_ids) {
        /*Extra lsz for saving CSF*/
        extra_lsz = _bcm_tr3_get_cps_line_size(unit);
        *metric += (extra_lsz*BCM_TR3_LINE_BYTE_SIZE);
    }

    BCM_REGEX_UNLOCK(unit);

    sal_free(res);
    sal_free(res_flags);
    return rv;
}

int
_bcm_tr3_regex_match_set(int unit, bcm_regex_engine_t engid, 
                         bcm_regex_match_t *matches, int count)
{
    char                         **res        = NULL;
    uint32                        *res_flags  = NULL;
    uint32                         cflags     = 0, regval, fval;
    void    *dfa = NULL;
    int  i, hw_states, *cps_match_set_ids=NULL, *cps_match_check_ids = NULL;
    int  ncs = 0;                       /* number of case sensitive signatures */
    int  nncs = 0;                      /* number of non-case sensitive signatures */
    int  rv = SOC_E_NONE, req_lsz, need_set_ids = 0, need_check_ids = 0;
    _bcm_tr3_regex_device_info_t  *device;
    _bcm_regex_lengine_t          *lengine    = NULL;
    _bcm_regex_engine_obj_t       *pengine    = NULL;
    int                           *sig_ids    = NULL;
    int                            tot_length = 0;

    if (!(device = REGEX_INFO(unit))) {
        return BCM_E_INIT;
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "unit %d, device not initialized\n"),
                  unit));
    }

    if ((engid == -1) && (count > 0)) {
        return _bcm_tr3_install_action_policy(unit, &matches[0], 
                                     _BCM_TR3_SET_DEFAULT_POLICY, NULL);
    }

    /* Ensure that at least one regex has non-zero length. Otherwise, no DFA can be
     * generated. Making the check here so that the engine is not disturbed if invalid
     * parameters are supplied. Note that it is valid to supply a count of zero which
     * means to delete the engine. */
    if (count > 0) {
        for (i = 0; i < count; i++) {
            if (NULL != matches[i].pattern) {
                tot_length += strlen(matches[i].pattern);
            }
        }
        if (tot_length <= 0) {
            return BCM_E_PARAM;
        }
    }

    rv = _bcm_tr3_find_engine(unit, engid, &lengine);
    if (rv) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "can't find engine %d\n"),
                  engid));
        return BCM_E_PARAM;
    }
   
    BCM_REGEX_LOCK(unit);

    /* if engine already has patterns, delete them */
    pengine = lengine->physical_engine;
    rv = _bcm_tr3_regex_hw_engine_detach(unit, pengine);
    if (rv) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "can't detach hw_engine %d, rv %d\n"),
                  engid, rv));
        goto fail;
    }
    lengine->physical_engine = NULL;

    if (count <= 0) {
        BCM_REGEX_UNLOCK(unit);
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "no patterns to install\n")));
        return BCM_E_NONE;
    }

    res = sal_alloc(count * sizeof(char*), "tmp_regex_ptr");
    res_flags = sal_alloc(count * sizeof(uint32), "tmp_regex_ptr");
    sig_ids = sal_alloc(count * sizeof(int), "tmp_sigids");

    /* iterate over all the patterns and see if we can enable
     * case insensitive feature in HW vs in software by doing [xX] */
    for (i = 0; i < count; i++) {
        /* Ensure that the regex is not too long.  It must be a null terminated string. */
        if (sal_strlen(matches[i].pattern) >= sizeof(matches[i].pattern)) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "regex %d too long\n"),
                      i));
            rv = BCM_E_PARAM;
            goto fail;
        }
        if (device->info->flags & BCM_REGEX_USE_POLICY_ID) {
            if (matches[i].action_flags & BCM_REGEX_MATCH_ACTION_NON_POLICY_ACTIONS) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "non-policy action flags set with policy-ID flag\n")));
                rv = BCM_E_PARAM;
                goto fail;
            }

            /*
            The esw-level function checks for the existence and installation
            of the policy ID that is being requested for this match, so at
            this point, all that is checked is that the correct policy-type
            (create w/ regex or use existing) is being requested.  If not,
            return BCM_E_PARAM.
            */
        }
        else {
            if (matches[i].policy_id != -1) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "attempting to use policy_id with no policy-ID flag\n")));
                rv = BCM_E_PARAM;
                goto fail;
            }
        }
        res[i] = matches[i].pattern;
        sig_ids[i] = -1;
        res_flags[i] = 0;
        if (matches[i].flags & BCM_REGEX_MATCH_CASE_INSENSITIVE) {
            nncs++;
        } else {
            ncs++;
        }
        if (matches[i].provides > 0) {
            need_set_ids += 1;
        }
        if (matches[i].requires > 0) {
            need_check_ids += 1;
        }
    }

    /* if the matches are mixed that is, it has both case sensitive and 
     * non-case sensitive matches mixes, we need to expand the case 
     * case insensitive patterns using [xX] */
    if (ncs && nncs) {
        for (i = 0; i < count; i++) {
            if (matches[i].flags & BCM_REGEX_MATCH_CASE_INSENSITIVE) {
                res_flags[i] = BCM_TR3_REGEX_CFLAG_EXPAND_LCUC;
            }
        }
    }

    if (!ncs && nncs) {
        for (i = 0; i < count; i++) {
            if (matches[i].flags & BCM_REGEX_MATCH_CASE_INSENSITIVE) {
                res_flags[i] = BCM_TR3_REGEX_CFLAG_EXPAND_LC;
            }
        }
    }
    
    dfa = NULL;
    rv = _bcm_tr3_calc_dfa_memsize(unit, res, res_flags, count, 
                                    cflags, &hw_states, &dfa);
    if (rv) {
        goto fail;
    }

    req_lsz = (hw_states + BCM_TR3_NUM_RV_PER_LINE-1)/BCM_TR3_NUM_RV_PER_LINE;
    if (req_lsz > _bcm_tr3_get_tile_line_size(unit)) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "required lines %d too large to install\n"),
                  req_lsz));
        rv = BCM_E_RESOURCE;
        goto fail;
    }

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "%d RV entry, %d lines needed\n"),
              hw_states, req_lsz));

    /*
     * Find the new cps slots needs, eliminate the ones for which one leg
     * already exist.
     */
    if (need_set_ids) {
        cps_match_set_ids = sal_alloc(sizeof(int)*count, "cps_set_ids");
        if (!cps_match_set_ids) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "no memory for match set IDs\n")));
            rv = BCM_E_MEMORY;
            goto fail;
        }
        for (i = 0; i < count; i++) {
            cps_match_set_ids[i] = -1;
        }
    }

    if (need_check_ids) {
        cps_match_check_ids = sal_alloc(sizeof(int)*count, "cps_check_ids");
        if (!cps_match_check_ids) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "no memory for match check IDs\n")));
            rv = BCM_E_MEMORY;
            goto fail;
        }
        for (i = 0; i < count; i++) {
            cps_match_check_ids[i] = -1;
        }
    }

    /* 
     * find the best tile/physical regex engine where this should be installed
     */
    rv = _bcm_tr3_attach_hw_engine(unit, lengine, req_lsz, 
                     cps_match_set_ids, cps_match_check_ids, matches, sig_ids, count);
    if (rv) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "could not attach hw_engine, rv %d\n"),
                  rv));
        goto fail;
    }

    rv = _bcm_tr3_install_dfa(unit, dfa, matches, cps_match_set_ids, cps_match_check_ids,
                              sig_ids, count, lengine->physical_engine);
    if (rv) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "could not install DFA, rv %d\n"),
                  rv));
        goto fail;
    }

    /*
    Return the signature ID, if requested
    */
    for (i = 0; i < count; i++) {
        if (matches[i].flags & BCM_REGEX_MATCH_SIG_ID_RETURN) {
            matches[i].sig_id = sig_ids[i];
        }
    }

    /* program the mapping table:
     * If any case-sensitive matches, do not remap characters.
     * Else, remap upper case to lower case characters.
     */
    if (ncs) {
        _bcm_tr3_set_remap_table(unit, lengine->physical_engine->hw_idx, NULL);
    } else {
        _bcm_tr3_set_remap_table(unit, lengine->physical_engine->hw_idx,
                                 cmap_lower_tbl);
    }
    
    rv = READ_AXP_SM_REGEX_CONTROL0r(unit, 
                            lengine->physical_engine->hw_idx, &regval);
    if (SOC_FAILURE(rv)) {
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "could not REGEX_CONTROL0 register, rv %d\n"),
                  rv));
        goto fail;
    }
    fval = 0;
    if (cps_match_set_ids || need_check_ids) {
        fval = 2;
    } else if (lengine->config.flags & BCM_REGEX_ENGINE_CONFIG_MULTI_PACKET) {
        fval = 1;
    }
    soc_reg_field_set(unit, AXP_SM_REGEX_CONTROL0r, &regval,
                             CONTEXT_STORAGE_MODEf, fval);
    WRITE_AXP_SM_REGEX_CONTROL0r(unit, lengine->physical_engine->hw_idx, regval);

    /*
     * resolve any pending matches.
     */
     if (cps_match_set_ids) {
        for (i = 0; i < count; i++) {
            if (cps_match_set_ids[i] == -1) {
                continue;
            }
            _bcm_regexdb_handle_setid_avail(unit, 1, 
                                            lengine->physical_engine->hw_idx,
                                            matches[i].provides, cps_match_set_ids[i]);
        }
     }

    BCM_REGEX_ENABLE_ENGINE(device, unit, lengine->physical_engine);

    /*
     * A signature is successfully configured. INT_SME_ENABLEf is to be SET here
     * so as to let the flow tracker direct the packets to Signature Matching
     * block for pattern matching
     */
    SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit,&regval));
    soc_reg_field_set(unit, FT_CONFIGr, &regval, INT_SME_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_FT_CONFIGr(unit,regval));

fail:
    if (res) {
        sal_free(res);
    }

    if (res_flags) {
        sal_free(res_flags);
    }

    if (dfa) {
        bcm_regex_dfa_free(dfa);
    }

    if (cps_match_set_ids) {
        sal_free(cps_match_set_ids);
    }

    if (cps_match_check_ids) {
        sal_free(cps_match_check_ids);
    }

    if (rv) {
        if (pengine) {
            _bcm_tr3_regex_hw_engine_detach(unit, pengine);
            lengine->physical_engine = NULL;
        }
        if (sig_ids) {
            for (i = 0; i < count; i++) {
                _bcm_tr3_free_sigid(unit, sig_ids[i]);
            }
        }
    }

    if (sig_ids) {
        sal_free(sig_ids);
    }

    BCM_REGEX_UNLOCK(unit);

    return rv;
}

STATIC int
_bcm_regex_init_cmn(int unit, bcm_regex_desc_t *d, int cold)
{
    _bcm_tr3_regex_device_info_t *device;
    int     i, j, count;
    soc_mem_t mem;
    int entry_words[1], min_index_array[1];
    _bcm_regex_cps_obj_t *pcps;
    ft_age_profile_entry_t ftpm;
    uint32          regval, fval;
    bcm_regex_match_t def_mp;

    if (!cmap_lower_tbl) {
        cmap_lower_tbl = sal_alloc(sizeof(uint8)*256, "remap_table");
        for (i=0; i<256; i++) {
            cmap_lower_tbl[i] = ((i >= 65) && (i <= 90)) ? i + 32 : i;
        }
    }

    /* Init logical engines.  */
    if ((device = REGEX_INFO(unit)) == NULL) {
        device = REGEX_INFO(unit) = sal_alloc(sizeof(_bcm_tr3_regex_device_info_t), 
                              "_bcm_tr3_regex_device_info");
        if (!device) {
            return BCM_E_MEMORY;
        }
        sal_memset(device, 0, sizeof(_bcm_tr3_regex_device_info_t));
        device->info = d;
        BCM_TR3_NUM_ENGINE(device) = d->num_tiles * d->num_engine_per_tile;

        for (i = 0; i < 2; i++) {
            device->flex_pools[i].pool_id = -1;
        }
    }

    sal_memset(device->sigid, 0xff, BCM_TR3_SIG_ID_MAP_BSZ*sizeof(uint32));
    sal_memset(device->policer_bmp, 0, BCM_TR3_POLICER_NUM/sizeof(uint32));
    device->regex_mutex = sal_mutex_create("regex_mutex");

    for (i = 0; i < BCM_TR3_NUM_ENGINE(device); i++) {
        sal_memset(&device->engines[i], 0, sizeof(_bcm_regex_lengine_t));
        sal_memset(&device->physical_engines[i], 0, sizeof(_bcm_regex_engine_obj_t));
        device->physical_engines[i].hw_idx = i;
        device->physical_engines[i].tile = (int)i/d->num_engine_per_tile;
        device->physical_engines[i].from_line = -1;
        device->physical_engines[i].req_lsz = 0;
        device->physical_engines[i].flags = 0;
        device->engines[i].id = BCM_REGEX_ENGINE_ID(unit, i);
        if (cold) {
            SOC_IF_ERROR_RETURN(BCM_REGEX_RESET_ENGINE(device, unit, i));
            bcm_regexdb_delete_by_engine_id(unit, i);
        }
        /* reset cps entries */
        pcps = BCM_TR3_CPS_INFO(device, i);
        pcps->foff = i*BCM_REGEX_NUM_SLOT_PER_CPS;
        pcps->flags = 0;
    }

    for (i=0; i<d->num_match_mem; i++) {
        sal_memset(device->match_alloc_bmap[i], 0, 
                                BCM_TR3_MATCH_BMAP_BSZ*sizeof(uint32));
        if (cold) {
            /* clear match memory */
            SOC_IF_ERROR_RETURN(soc_mem_clear(unit, 
                    AXP_SM_MATCH_TABLE_MEM0m + i, COPYNO_ALL, TRUE));
        }
    }

    if (_bcm_tr3_ft_policy_profile[unit] == NULL) {
        _bcm_tr3_ft_policy_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t), "FT_Policy Profile Mem");
        if (_bcm_tr3_ft_policy_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_tr3_ft_policy_profile[unit]);
    }
    mem = FT_POLICYm;
    entry_words[0] = sizeof(ft_policy_entry_t) / sizeof(uint32);
    min_index_array[0] = 1; /* reserve entry Zero */
    SOC_IF_ERROR_RETURN(soc_profile_mem_index_create(unit, &mem, entry_words, 
             &min_index_array[0], NULL, NULL, 1, _bcm_tr3_ft_policy_profile[unit]));

    if (cold) {
        count = soc_mem_index_count(unit, FT_AGE_PROFILEm);
        sal_memset(&ftpm, 0, sizeof(ftpm));
        for (i = 0; i < count; i++) {
            if (i == (count - 1)) {
                fval = 0x3fff;
            } else {
                fval = (i <= 8) ? (16 << i) : 0x1000 + (0x400 * (i - 8)) - 1;
            }
            soc_mem_field32_set(unit, FT_AGE_PROFILEm, &ftpm, TIMEOUTf, fval);
            SOC_IF_ERROR_RETURN(WRITE_FT_AGE_PROFILEm(unit, MEM_BLOCK_ALL, 
                                                                    i, &ftpm));
        }

        SOC_IF_ERROR_RETURN(READ_FT_REFRESH_CFGr(unit, &regval));
        soc_reg_field_set(unit, FT_REFRESH_CFGr, &regval, ENABLEf, 1);
        soc_reg_field_set(unit, FT_REFRESH_CFGr, &regval, MIN_TIMEf, 0x3fff);
        SOC_IF_ERROR_RETURN(WRITE_FT_REFRESH_CFGr(unit, regval));

        /*
        For now, just use defaults for these timeouts

        SOC_IF_ERROR_RETURN(READ_FT_NEW_TIMEOUTr(unit, &regval));
        soc_reg_field_set(unit, FT_NEW_TIMEOUTr, &regval, UDPf, 0xfff);
        soc_reg_field_set(unit, FT_NEW_TIMEOUTr, &regval, TCPf, 0xfff);
        SOC_IF_ERROR_RETURN(WRITE_FT_NEW_TIMEOUTr(unit, regval));

        SOC_IF_ERROR_RETURN(READ_FT_HOLDWAIT_TIMEOUTr(unit, &regval));
        soc_reg_field_set(unit, FT_HOLDWAIT_TIMEOUTr, &regval, UDPf, 0x7ff);
        soc_reg_field_set(unit, FT_HOLDWAIT_TIMEOUTr, &regval, TCPf, 0x7ff);
        SOC_IF_ERROR_RETURN(WRITE_FT_HOLDWAIT_TIMEOUTr(unit, regval));

        SOC_IF_ERROR_RETURN(READ_FT_CMDWAIT_TIMEOUTr(unit, &regval));
        soc_reg_field_set(unit, FT_CMDWAIT_TIMEOUTr, &regval, MAX_TIMEf, 0x3ff);
        SOC_IF_ERROR_RETURN(WRITE_FT_CMDWAIT_TIMEOUTr(unit, regval));
        */

        SOC_IF_ERROR_RETURN(READ_FT_HASH_CONTROLr(unit, &regval));
        soc_reg_field_set(unit, FT_HASH_CONTROLr, &regval, HASH_SELECT_Af, 1);
        soc_reg_field_set(unit, FT_HASH_CONTROLr, &regval, HASH_SELECT_Bf, 1);
        SOC_IF_ERROR_RETURN(WRITE_FT_HASH_CONTROLr(unit, regval));

        regval = 0;
        SOC_IF_ERROR_RETURN(WRITE_AXP_SM_MEM_ECC_GEN_CONTROLr(unit, regval));

        for (j = 0; j < 8; j++) {
            mem = _bcm_tr3_get_axp_sm_memory(unit, j);
            SOC_IF_ERROR_RETURN(soc_mem_clear(unit, mem, MEM_BLOCK_ALL, 1));
        }
        BCM_IF_ERROR_RETURN(_bcm_tr3_regex_exclude_delete_all(unit));

        /* create a default policy profile */
        sal_memset(&def_mp, 0, sizeof(bcm_regex_match_t));
        BCM_IF_ERROR_RETURN(_bcm_tr3_install_action_policy(unit, &def_mp, 
                                        _BCM_TR3_SET_DEFAULT_POLICY, NULL));
    
	BCM_IF_ERROR_RETURN(_bcm_esw_regex_report_reset(unit));
    }
    
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

typedef struct _bcm_regex_dal_s {
    int (*dal_op_u32)(uint8 **buf, uint32 *data, int in);
    int (*dal_op_u16)(uint8 **buf, uint16 *data, int in);
    int (*dal_op_u8)(uint8 **buf, uint8 *data, int in);
} _bcm_regex_dal_t;

STATIC int _bcm_regex_dal_op_u32(uint8 **buf, uint32 *data, int in)
{
    uint8 *pbuf = *buf;
    int i, bshf = (sizeof(uint32) - 1) * 8;
    uint32 v;

    if (in) {
        for (i = 0; i < sizeof(uint32); i++, bshf -= 8) {
            *pbuf++ = (uint8)((*data) >> bshf) & 0xff;
        }
    } else {
        v = 0;
        for (i = 0; i < sizeof(uint32); i++) {
            v |= (uint32)((*pbuf) << bshf);
            pbuf++;
        }
        *data = v;
    }
    *buf = pbuf;
    return 0;
}

STATIC int _bcm_regex_dal_op_u16(uint8 **buf, uint16 *data, int in)
{
    uint8 *pbuf = *buf;
    int i, bshf = (sizeof(uint16) - 1) * 8;
    uint16 v;

    if (in) {
        for (i = 0; i < sizeof(uint16); i++, bshf -= 8) {
            *pbuf++ = (uint8)((*data) >> bshf) & 0xff;
        }
    } else {
        v = 0;
        for (i = 0; i < sizeof(uint16); i++) {
            v |= (uint16)((*pbuf) << bshf);
            pbuf++;
        }
        *data = v;
    }
    *buf = pbuf;
    return 0;
}

STATIC int _bcm_regex_dal_op_u8(uint8 **buf, uint8 *data, int in)
{
    uint8 *pbuf = *buf;

    if (in) {
        *pbuf++ = *data;
    } else {
        *data = *pbuf++;
    }
    *buf = pbuf;
    return 0;
}

static _bcm_regex_dal_t bcm_regex_dal = {
                        _bcm_regex_dal_op_u32,
                        _bcm_regex_dal_op_u16,
                        _bcm_regex_dal_op_u8,
                                    };

#define BCM_REGEX_DAL_RD_U32(b,d) bcm_regex_dal.dal_op_u32((b),(d),0)
#define BCM_REGEX_DAL_RD_U16(b,d) bcm_regex_dal.dal_op_u16((b),(d),0)
#define BCM_REGEX_DAL_RD_U8(b,d) bcm_regex_dal.dal_op_u8((b),(d),0)

#define BCM_REGEX_DAL_WR_U32(b,d) bcm_regex_dal.dal_op_u32((b),(d),1)
#define BCM_REGEX_DAL_WR_U16(b,d) bcm_regex_dal.dal_op_u16((b),(d),1)
#define BCM_REGEX_DAL_WR_U8(b,d) bcm_regex_dal.dal_op_u8((b),(d),1)

#define BCM_REGEX_WARMBOOT_ENGINE_SIG   0x07062005
#define BCM_REGEX_WARMBOOT_MATCH_SIG    0x07062012

#if 0
STATIC int _bcm_regex_engine_reinit(int unit, uint8 **regex_data)
{
    uint32 v, i, leid, peid, req_lsz, cps_lsz;
    uint16 v16;
    uint8  v8, tv8;
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_engine_obj_t      engine_info, *pengine;
    _bcm_regex_lengine_t         *lengine;

    device = REGEX_INFO(unit);

    /* read marker */
    BCM_REGEX_DAL_RD_U32(regex_data, &v);
    if (v != BCM_REGEX_WARMBOOT_ENGINE_SIG) {
        return BCM_E_MEMORY;
    }

    BCM_REGEX_DAL_RD_U8(regex_data, &v8);

    for (i=0; i<v8; i++) {
        /* logicval engine id */
        BCM_REGEX_DAL_WR_U8(regex_data, &tv8);
        leid = (uint32) tv8;
        /* physical engine id */
        BCM_REGEX_DAL_WR_U8(regex_data, &tv8);
        peid = (uint32) tv8;
        /* engine dfa length */
        BCM_REGEX_DAL_WR_U16(regex_data, &v16);
        req_lsz = (uint16) v16;
        /* engine cps length */
        BCM_REGEX_DAL_WR_U16(regex_data, &v16);
        cps_lsz = (uint16) v16;

        BCM_REGEX_GET_ENGINE_INFO(device, unit, peid, &engine_info);
        if (!BCM_REGEX_ENGINE_ALLOCATED(&engine_info)) {
            continue;
        }
        pengine = BCM_TR3_PHYSICAL_ENGINE(device, peid); 
        lengine = BCM_TR3_LOGICAL_ENGINE(device, leid);
        BCM_REGEX_ENGINE_SET_ALLOCATED(lengine);
        BCM_REGEX_ENGINE_SET_ALLOCATED(pengine);
        pengine->from_line = engine_info.from_line;
        pengine->req_lsz = req_lsz;
        pengine->cps.req_lsz = cps_lsz;
        pengine->total_lsz = cps_lsz + req_lsz;
        /* set physical engine mapping */
        lengine->physical_engine = pengine;
    }
    
    BCM_REGEX_DAL_RD_U32(regex_data, &v);
    if (v != ~BCM_REGEX_WARMBOOT_ENGINE_SIG) {
        return BCM_E_MEMORY;
    }
    return BCM_E_NONE;
}
#endif

STATIC int _bcm_regex_engine_sync(int unit, uint8 **regex_data)
{
    uint32 i, v32;
    uint8  v8;
    uint16 v16;
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regex_lengine_t         *lengine;

    device = REGEX_INFO(unit);

    /* read marker */
    v32 = BCM_REGEX_WARMBOOT_ENGINE_SIG;
    BCM_REGEX_DAL_WR_U32(regex_data, &v32);

    for (i=0,v8=0; i<BCM_TR3_NUM_ENGINE(device); i++) {
        lengine = BCM_TR3_LOGICAL_ENGINE(device, i);
        if (BCM_REGEX_ENGINE_ALLOCATED(lengine)) {
            v8++;
        }
    }
    
    BCM_REGEX_DAL_WR_U8(regex_data, &v8);

    /*
     * write engine data.
     */
    for (i=0; i<BCM_TR3_NUM_ENGINE(device); i++) {
        lengine = BCM_TR3_LOGICAL_ENGINE(device, i);
        if (!BCM_REGEX_ENGINE_ALLOCATED(lengine)) {
            continue;
        }

        /* engine data:
         *  engine logical index (u8), engine physical index(u8),
         *  dfa length (u16)
         */
        /* engine logical index */
        v8 = (uint8)i;
        BCM_REGEX_DAL_WR_U8(regex_data, &v8);
        /* engine physical index */
        v8 = (uint8) lengine->physical_engine->hw_idx;
        BCM_REGEX_DAL_WR_U8(regex_data, &v8);
        /* engine dfa size */
        v16 = lengine->physical_engine->req_lsz;
        BCM_REGEX_DAL_WR_U16(regex_data, &v16);
        /* engine cps size */
        v16 = lengine->physical_engine->cps.req_lsz;
        BCM_REGEX_DAL_WR_U16(regex_data, &v16);
    }

    v32 = ~BCM_REGEX_WARMBOOT_ENGINE_SIG;
    BCM_REGEX_DAL_WR_U32(regex_data, &v32);
    return 0;
}

#if 0
STATIC int _bcm_regex_match_reinit(int unit, uint8 **regex_data)
{
    uint32 i, v32;
    uint16 f, num_match = 0;
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regexdb_entry_t pe;
    axp_sm_match_table_mem0_entry_t match_entry;
    soc_mem_t mem;
    int match_set_id = -1, match_chk_id = -1, hw_match_idx;
    uint32 *alloc_bmap;
    _bcm_regex_engine_obj_t *pengine = NULL;

    device = REGEX_INFO(unit);

    BCM_REGEX_DAL_RD_U32(regex_data, &v32);
    if (v32 != BCM_REGEX_WARMBOOT_MATCH_SIG) {
        return BCM_E_MEMORY;
    }

    /* read the number of match entries */
    BCM_REGEX_DAL_RD_U16(regex_data, &num_match);

    for (i=0; i<num_match; i++) {
        sal_memset(&pe, 0, sizeof(_bcm_regexdb_entry_t));

        /* read 2 byte flag */
        BCM_REGEX_DAL_RD_U16(regex_data, &f);

        /* read match id */
        BCM_REGEX_DAL_RD_U32(regex_data, (uint32*)&pe.engid);

        pengine = BCM_TR3_PHYSICAL_ENGINE(device, pe.engid);

        /* read signature id */
        BCM_REGEX_DAL_RD_U32(regex_data, (uint32*)&pe.sigid);

        device->sigid[pe.sigid/32] |= 1 << (pe.sigid % 32);

        /* read match id */
        BCM_REGEX_DAL_RD_U32(regex_data, (uint32*)&pe.match_id);

        /* read match index */
        BCM_REGEX_DAL_RD_U32(regex_data, (uint32*)&pe.match_index);

        /* read provides and requires ID if valid */
        if (f & BCM_REGEXDB_MATCH_PROVIDES) {
            BCM_REGEX_DAL_RD_U32(regex_data, (uint32*)&pe.provides);
        }

        if (f & BCM_REGEXDB_MATCH_REQUIRES) {
            BCM_REGEX_DAL_RD_U32(regex_data, (uint32*)&pe.requires);
        }

        /* read match entry and find the match_set_id and match_chk_id */
        mem = AXP_SM_MATCH_TABLE_MEM0m + 
                        BCM_REGEX_ENG2MATCH_MEMIDX(unit,pe.engid);

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                    MEM_BLOCK_ALL, pe.match_index, &match_entry));

        if (soc_mem_field32_get(unit, mem, &match_entry, 
                                            MATCH_CROSS_SIG_FLAG_SETf)) {
            /* set the new setid */
            match_set_id = soc_mem_field32_get(unit, mem, &match_entry,
                                            MATCH_CROSS_SIG_FLAG_INDEXf);
            pengine->cps.alloc_map[match_set_id % BCM_REGEX_NUM_SLOT_PER_CPS] = match_set_id;
            pengine->cps.alloc_map[match_set_id % BCM_REGEX_NUM_SLOT_PER_CPS]++;
        }

        if (soc_mem_field32_get(unit, mem, &match_entry,
                                CHECK_CROSS_SIG_FLAG_IS_SETf)) {
            /* set the new chkid */
            match_chk_id = soc_mem_field32_get(unit, mem, &match_entry,
                                             MATCH_CROSS_SIG_FLAG_INDEXf);
        }

        alloc_bmap = BCM_TR3_MATCH_BMAP_PTR(unit, device, pe.engid);
        alloc_bmap[pe.match_index/32] |= 1 << (pe.match_index % 32);

        /* 
         * also ref count for match profiles.
         * also cps allocation and reference count 
         */
        hw_match_idx = (pe.match_index % 1024);

        alloc_bmap = BCM_TR3_MATCH_BMAP_PTR(unit, device, pe.engid);
        alloc_bmap[hw_match_idx/32] &= ~(1 << (hw_match_idx % 32));
        BCM_IF_ERROR_RETURN(bcm_regexdb_add_entry(unit, pe.match_index, pe.engid, 
               pe.match_id, pe.sigid, pe.provides, pe.requires, match_set_id, match_chk_id));
    }

    /* end marker */
    BCM_REGEX_DAL_WR_U32(regex_data, &v32);
    if (v32 != ~BCM_REGEX_WARMBOOT_MATCH_SIG) {
        return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}
#endif

STATIC int _bcm_regex_match_sync(int unit, uint8 **regex_data)
{
    uint32 i, v32;
    uint8  *num_match_loc;
    uint16 f, num_match = 0;
    _bcm_tr3_regex_device_info_t *device;
    _bcm_regexdb_entry_t *pe;

    device = REGEX_INFO(unit);

    v32 = BCM_REGEX_WARMBOOT_MATCH_SIG;
    BCM_REGEX_DAL_WR_U32(regex_data, &v32);

    /* save the location to write the number of match entries */
    num_match_loc = *regex_data;
    BCM_REGEX_DAL_WR_U16(regex_data, &num_match);

    for (i=0; i<BCM_REGEX_DB_BUCKET_MAX; i++) {
        pe = device->l[i];
        while (pe) {
            f = 0;
            if (pe->provides > 0) {
                f |= BCM_REGEXDB_MATCH_PROVIDES;
            }
            if (pe->requires > 0) {
                f |= BCM_REGEXDB_MATCH_REQUIRES;
            }
            /* write 2 byte flag */
            BCM_REGEX_DAL_WR_U16(regex_data, &f);

            /* write physical engine id */
            v32 = (uint32) pe->engid;
            BCM_REGEX_DAL_WR_U32(regex_data, &v32);

            /* write signature id */
            v32 = (uint32) pe->sigid;
            BCM_REGEX_DAL_WR_U32(regex_data, &v32);

            /* write match id */
            v32 = (uint32) pe->match_id;
            BCM_REGEX_DAL_WR_U32(regex_data, &v32);

            /* write match index */
            v32 = (uint32) pe->match_index;
            BCM_REGEX_DAL_WR_U32(regex_data, &v32);

            /* write provides and requires ID if valid */
            if (f & BCM_REGEXDB_MATCH_PROVIDES) {
                v32 = (uint32) pe->provides;
                BCM_REGEX_DAL_WR_U32(regex_data, &v32);
            }

            if (f & BCM_REGEXDB_MATCH_REQUIRES) {
                v32 = (uint32) pe->requires;
                BCM_REGEX_DAL_WR_U32(regex_data, &v32);
            }
            num_match++;
            pe = pe->next;
        }
    }

    BCM_REGEX_DAL_WR_U16(&num_match_loc, &num_match);

    /* end marker */
    v32 = ~BCM_REGEX_WARMBOOT_MATCH_SIG;
    BCM_REGEX_DAL_WR_U32(regex_data, &v32);

    return BCM_E_NONE;
}

#define REGEX_WB_CACHE_SZ       4096

#if 0
STATIC int 
_bcm_tr3_regex_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    int     rv = BCM_E_NONE;
    int     stable_size;
    uint8   *regex_data;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_REGEX, 0);

    if (!SOC_WARM_BOOT(unit)) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                     REGEX_WB_CACHE_SZ, &regex_data, 
                                     BCM_WB_DEFAULT_VERSION, NULL);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
        return BCM_E_NONE;
    } else {

        SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                     stable_size, &regex_data,
                                     BCM_WB_DEFAULT_VERSION, NULL);

        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }

        if (NULL == regex_data) {
            return BCM_E_MEMORY;
        }

        SOC_IF_ERROR_RETURN(_bcm_regex_engine_reinit(unit, &regex_data));

        SOC_IF_ERROR_RETURN(_bcm_regex_match_reinit(unit, &regex_data));
    }

    return rv;
}
#endif

int _bcm_esw_tr3_regex_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    int       stable_size, rv = BCM_E_NONE;
    uint8     *regex_data, *data_start;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_REGEX, 0);
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    
    rv = _bcm_esw_scache_ptr_get(unit,
                                 scache_handle, FALSE,
                                 REGEX_WB_CACHE_SZ, &data_start,
                                 BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
        return rv;
    }

    if (NULL == data_start) {
        return BCM_E_MEMORY;
    }
    regex_data = data_start;
    rv = _bcm_regex_engine_sync(unit, &regex_data);
    if (SOC_FAILURE(rv)) {
        goto fail;
    }

    rv = _bcm_regex_match_sync(unit, &regex_data);

fail:
    return rv;
}
#endif  /* BCM_WARM_BOOT_SUPPORT */

int _bcm_tr3_regex_init(int unit, _bcm_esw_regex_functions_t **functions)
{
    bcm_regex_desc_t *d = NULL;
    int cold = 1, rv = BCM_E_NONE;

    if (SOC_IS_TRIUMPH3(unit)) {
        d = &tr3_regex_info;
    }

    if (!d) {
        return BCM_E_UNAVAIL;
    }

    BCM_REGEX_LOCK(unit);

    rv = _bcm_regex_init_cmn(unit, d, cold);
    if (SOC_FAILURE(rv)) {
        goto fail;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* No WARMBOOT support for REGEX */ 
#endif

    *functions = &_regex_tr3_functions;

fail:
    return rv;
}

extern int _bcm_ft_key_field_set(uint32 *fld, int bp, int len, uint32 *kfld);
extern int _bcm_ft_key_field_get(uint32 *fld, int bp, int len, uint32 *kfld);
extern int _bcm_ft_key_field_get_char(uint32 *fld, int bp, int len, uint8 *kfld);
extern int _bcm_ft_key_field_set_char(uint32 *fld, int bp, int len, uint8 *kfld);

/*
 * Function:
 *      _bcm_tr3_ft_session_to_hw
 * Purpose:
 *      Convert a hardware-independent FT session entry to a TR3 FT
 *      session HW entry
 * Parameters:
 *      unit - Unit number
 *      hw_entry - (OUT) Triumph 3 HW entry
 *      flags - FT session flags (e.g. static entry)
 *      key - Hardware-independent FT session key
 *      session - Hardware-independent FT session info
 */

STATIC int
_bcm_tr3_ft_session_to_hw(int unit, ft_session_entry_t *hw_entry, int flags,
                          bcm_regex_session_key_t *key,
                          bcm_regex_session_t *session)
{
    uint32 key_field[_REGEX_KEY_FIELD_LENGTH/32 + 1], direction;
    uint32 ip1, ip2, port1, port2, protocol;
    int i, age_profile = -1, max_index, ticks;
    ft_age_profile_entry_t ftpm;
    uint32 regval, flow_state;
    uint8 *p8;

    soc_FT_SESSIONm_field32_set(unit, hw_entry, VALIDf, 1);
    soc_FT_SESSIONm_field32_set(unit, hw_entry, KEY_TYPEf,
                                _REGEX_SESSION_KEY_TYPE_IPV4);

    if (session != NULL) {
        soc_FT_SESSIONm_field32_set(unit, hw_entry, POLICY_INDEXf,
                                    session->policy);

        /*
        Granularity of AGE_PROFILE entries is 10ms, and inactivity_timeout
        is in usecs.
        */
        ticks = session->inactivity_timeout/_REGEX_SESSION_AGE_PROFILE_TICKS_US;
        max_index = soc_mem_index_max(unit, FT_AGE_PROFILEm);
        for (i = max_index; i >= 0; i--) {
            SOC_IF_ERROR_RETURN(READ_FT_AGE_PROFILEm(unit, MEM_BLOCK_ALL,
                                                        i, &ftpm));
            if (ticks >= soc_mem_field32_get(unit,
                                    FT_AGE_PROFILEm, &ftpm, TIMEOUTf)) {
                age_profile = (i == max_index) ? i : i + 1;
                break;
            }
        }

        if (age_profile == -1) {
            age_profile = soc_mem_index_max(unit, FT_AGE_PROFILEm);
        }

        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "ticks = %d (0x%x), age_profile = 0x%x\n"),
                  ticks, ticks, age_profile));

        soc_FT_SESSIONm_field32_set(unit, hw_entry, AGE_PROFILEf,
                                    age_profile);

        if (session->flags & BCM_REGEX_SESSION_STATIC) {
            soc_FT_SESSIONm_field32_set(unit, hw_entry, STATIC_BITf, 1);
        }

        /* Entries that are added will be set to flow state ACTIVE */

        flow_state = _REGEX_SESSION_FLOW_STATE_ACTIVE;

        soc_FT_SESSIONm_field32_set(unit, hw_entry, FLOW_STATEf,
                                    flow_state);

        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "flow_state: 0x%x\n"),
                  flow_state));
    }

    /*
    Current time and flow timestamp are both in units of 10ms
    */
    SOC_IF_ERROR_RETURN(READ_FT_CURRENT_TIMEr(unit, &regval));
    soc_FT_SESSIONm_field32_set(unit, hw_entry, ID_TIMESTAMPf, regval);

    if (key->dip < key->sip) {
        direction = _REGEX_SESSION_DIRECTION_REVERSE;
    } else {
        direction = _REGEX_SESSION_DIRECTION_FORWARD;
    }

    protocol = key->protocol;

    if (direction == _REGEX_SESSION_DIRECTION_REVERSE) {
      ip1 = key->sip;
      ip2 = key->dip;
      port1 = key->src_port;
      port2 = key->dst_port;
    } else {
      ip1 = key->dip;
      ip2 = key->sip;
      port1 = key->dst_port;
      port2 = key->src_port;
    }

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "ip1 = 0x%08x\n"),
              ip1));
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "ip2 = 0x%08x\n"),
              ip2));
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "port1 = 0x%04x\n"),
              port1));
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "port2 = 0x%04x\n"),
              port2));
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "protocol = 0x%02x\n"),
              protocol));

    /*
    _bcm_ft_key_field_set uses bit or to set the various bits in
    the field, so the fields must be set to 0 before use
    */
    for (i = 0; i < (_REGEX_KEY_FIELD_LENGTH/32 + 1); i++) {
        key_field[i] = 0;
    }

    _bcm_ft_key_field_set(key_field, _REGEX_KEY_FIELD_V4_OFFSET_PORT_1,
                          _REGEX_KEY_FIELD_V4_LENGTH_PORT_1, &port1);
    _bcm_ft_key_field_set(key_field, _REGEX_KEY_FIELD_V4_OFFSET_PORT_2,
                          _REGEX_KEY_FIELD_V4_LENGTH_PORT_2, &port2);
    _bcm_ft_key_field_set(key_field, _REGEX_KEY_FIELD_V4_OFFSET_PROTOCOL,
                          _REGEX_KEY_FIELD_V4_LENGTH_PROTOCOL, &protocol);
    _bcm_ft_key_field_set(key_field, _REGEX_KEY_FIELD_V4_OFFSET_IP_1,
                          _REGEX_KEY_FIELD_V4_LENGTH_IP_1, &ip1);
    _bcm_ft_key_field_set(key_field, _REGEX_KEY_FIELD_V4_OFFSET_IP_2,
                          _REGEX_KEY_FIELD_V4_LENGTH_IP_2, &ip2);

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "direction = 0x%x\n"),
              direction));
    soc_FT_SESSIONm_field32_set(unit, hw_entry, KEY_DIRECTIONf, direction);
    p8 = (uint8*) key_field;
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "key_field = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n"),
              *p8, *(p8+1), *(p8+2), *(p8+3), *(p8+4), *(p8+5), *(p8+6), *(p8+7), *(p8+8), *(p8+9), *(p8+10), *(p8+11), *(p8+12)));
    soc_FT_SESSIONm_field_set(unit, hw_entry, KEYf, key_field);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ft_session_ipv6_to_hw
 * Purpose:
 *      Convert a hardware-independent FT session IPV6 entry to a TR3 FT
 *      session IPV6 HW entry
 * Parameters:
 *      unit - Unit number
 *      hw_entry6 - (OUT) Triumph 3 HW entry
 *      flags - FT session flags (e.g. static entry)
 *      key - Hardware-independent FT session key
 *      session - Hardware-independent FT session info
 */

STATIC int
_bcm_tr3_ft_session_ipv6_to_hw(int unit, ft_session_ipv6_entry_t *hw_entry6,
                               int flags, bcm_regex_session_key_t *key,
                               bcm_regex_session_t *session)
{
    uint32 key_upper[_REGEX_KEY_UPPER_LENGTH/32 + 1];
    uint32 key_lower[_REGEX_KEY_LOWER_LENGTH/32 + 1], direction;
    bcm_ip6_t sip6, dip6;
    uint32 port1, port2, protocol;
    int i, age_profile = -1, max_index, ticks;
    ft_age_profile_entry_t ftpm;
    uint32 regval, flow_state;

    soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, VALIDf, 1);
    soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, VALID_0f, 1);
    soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, KEY_TYPEf,
                                     _REGEX_SESSION_KEY_TYPE_IPV6_UPPER);
    soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, KEY_TYPE_0f,
                                     _REGEX_SESSION_KEY_TYPE_IPV6_LOWER);
    if (session != NULL) {
        soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, POLICY_INDEXf,
                                         session->policy);

        /*
        Granularity of AGE_PROFILE entries is 10ms, and inactivity_timeout
        is in usecs.
        */
        ticks = session->inactivity_timeout/_REGEX_SESSION_AGE_PROFILE_TICKS_US;
        max_index = soc_mem_index_max(unit, FT_AGE_PROFILEm);
        for (i = max_index; i >= 0; i--) {
            SOC_IF_ERROR_RETURN(READ_FT_AGE_PROFILEm(unit, MEM_BLOCK_ALL,
                                                        i, &ftpm));
            if (ticks >= soc_mem_field32_get(unit,
                                    FT_AGE_PROFILEm, &ftpm, TIMEOUTf)) {
                age_profile = (i == max_index) ? i : i + 1;
                break;
            }
        }

        if (age_profile == -1) {
            age_profile = soc_mem_index_max(unit, FT_AGE_PROFILEm);
        }

        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "ticks = %d (0x%x), age_profile = 0x%x\n"),
                  ticks, ticks, age_profile));

        soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, AGE_PROFILEf,
                                         age_profile);

        if (session->flags & BCM_REGEX_SESSION_STATIC) {
            soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, STATIC_BITf, 1);
        }

        /* Entries that are added will be set to flow state ACTIVE */

        flow_state = _REGEX_SESSION_FLOW_STATE_ACTIVE;

        soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, FLOW_STATEf,
                                         flow_state);

        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "flow_state: 0x%x\n"),
                  flow_state));
    }

    /*
    Current time and flow timestamp are both in units of 10ms
    */
    SOC_IF_ERROR_RETURN(READ_FT_CURRENT_TIMEr(unit, &regval));
    soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, ID_TIMESTAMPf, regval);

    direction = 0;
    for (i = 0; i < _SHR_L3_IP6_ADDRLEN; i++) {
        if (key->dip6[i] < key->sip6[i]) {
            direction = _REGEX_SESSION_DIRECTION_REVERSE;
            break;
        } else if (key->sip6[i] < key->dip6[i]) {
            direction = _REGEX_SESSION_DIRECTION_FORWARD;
            break;
        } else if (i == _SHR_L3_IP6_ADDRLEN) {
            direction = _REGEX_SESSION_DIRECTION_FORWARD;
            break;
        } else {
            continue;
        }
    }

    protocol = key->protocol;

    if (direction == _REGEX_SESSION_DIRECTION_REVERSE) {
      port1 = key->src_port;
      port2 = key->dst_port;
    } else {
      port1 = key->dst_port;
      port2 = key->src_port;
    }

    /*
    _bcm_ft_key_field_set uses bit or to set the various bits in
    the field, so the fields must be set to 0 before use
    */
    for (i = 0; i < (_REGEX_KEY_UPPER_LENGTH/32 + 1); i++) {
        key_upper[i] = 0;
    }

    for (i = 0; i < (_REGEX_KEY_LOWER_LENGTH/32 + 1); i++) {
        key_lower[i] = 0;
    }

    _bcm_ft_key_field_set(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_PORT_1,
                          _REGEX_KEY_LOWER_V6_LENGTH_PORT_1, &port1);
    _bcm_ft_key_field_set(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_PORT_2,
                          _REGEX_KEY_LOWER_V6_LENGTH_PORT_2, &port2);
    _bcm_ft_key_field_set(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_PROTOCOL,
                          _REGEX_KEY_LOWER_V6_LENGTH_PROTOCOL, &protocol);
    /*
    IPv6 addresses will go into hardware in reverse order, so reverse
    the addresses here
    */

    for (i = 0; i < _SHR_L3_IP6_ADDRLEN; i++) {
        sip6[i] = key->sip6[_SHR_L3_IP6_ADDRLEN - i - 1];
        dip6[i] = key->dip6[_SHR_L3_IP6_ADDRLEN - i - 1];
    }

    if (direction == _REGEX_SESSION_DIRECTION_FORWARD) {
        _bcm_ft_key_field_set_char(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_IP_1,
                              _REGEX_KEY_LOWER_V6_LENGTH_IP_1, dip6);
    } else {
        _bcm_ft_key_field_set_char(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_IP_1,
                              _REGEX_KEY_LOWER_V6_LENGTH_IP_1, sip6);
    }
    if (direction == _REGEX_SESSION_DIRECTION_FORWARD) {
        _bcm_ft_key_field_set_char(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_IP_2,
                              _REGEX_KEY_LOWER_V6_LENGTH_IP_2, sip6);
    } else {
        _bcm_ft_key_field_set_char(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_IP_2,
                              _REGEX_KEY_LOWER_V6_LENGTH_IP_2, dip6);
    }
    if (direction == _REGEX_SESSION_DIRECTION_FORWARD) {
        _bcm_ft_key_field_set_char(key_upper, _REGEX_KEY_UPPER_V6_OFFSET_IP_2,
                                   _REGEX_KEY_UPPER_V6_LENGTH_IP_2,
                                   &sip6[_REGEX_KEY_LOWER_V6_LENGTH_IP_2/8]);
    } else {
        _bcm_ft_key_field_set_char(key_upper, _REGEX_KEY_UPPER_V6_OFFSET_IP_2,
                                   _REGEX_KEY_UPPER_V6_LENGTH_IP_2,
                                   &dip6[_REGEX_KEY_LOWER_V6_LENGTH_IP_2/8]);
    }

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "direction = 0x%x\n"),
              direction));
    soc_FT_SESSION_IPV6m_field32_set(unit, hw_entry6, KEY_DIRECTIONf, direction);
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "set mem %s field %s\n"),
              soc_mem_name[FT_SESSION_IPV6m], soc_fieldnames[KEYf]));
    soc_FT_SESSION_IPV6m_field_set(unit, hw_entry6, KEYf, key_upper);
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "set mem %s field %s\n"),
              soc_mem_name[FT_SESSION_IPV6m], soc_fieldnames[IPV6_LOWER_KEYf]));
    soc_FT_SESSION_IPV6m_field_set(unit, hw_entry6, IPV6_LOWER_KEYf, key_lower);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ft_session_from_hw
 * Purpose:
 *      Convert a TR3 FT session HW entry to a hardware-independent
 *      FT session entry
 * Parameters:
 *      unit - Unit number
 *      flags - (OUT) FT session flags (e.g. static entry)
 *      key - (OUT) Hardware-independent FT session key
 *      session - (OUT) Hardware-independent FT session info
 *      hw_entry - Triumph 3 HW entry
 */

STATIC int
_bcm_tr3_ft_session_from_hw(int unit, int flags,
                            bcm_regex_session_t *session,
                            bcm_regex_session_key_t *key,
                            ft_session_entry_t *ft_hw_entry)
{
    uint32 static_bit, flow_state;
    uint32 key_field[_REGEX_KEY_FIELD_LENGTH/32 + 1], direction;
    uint32 type, valid, ip1, ip2, port1, port2, protocol;

    soc_mem_t mem = FT_SESSIONm;
    const void *hw_entry = (const void *) ft_hw_entry;
    int age_profile, ticks;
    ft_age_profile_entry_t ftpm;

    soc_mem_field_get(unit, mem, hw_entry, KEY_TYPEf, &type);
    if (type != _REGEX_SESSION_KEY_TYPE_IPV4) {
        return BCM_E_UNAVAIL;
    }

    soc_mem_field_get(unit, mem, hw_entry, VALIDf, &valid);
    if (valid != 1) {
        return BCM_E_UNAVAIL;
    }

    soc_mem_field_get(unit, mem, hw_entry, POLICY_INDEXf, (uint32 *) &session->policy);

    /*
    Granularity of AGE_PROFILE entries is 10ms, and inactivity_timeout
    is in usecs.
    */

    age_profile = soc_FT_SESSIONm_field32_get(unit, hw_entry, AGE_PROFILEf);
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "age_profile = 0x%x\n"),
              age_profile));
    SOC_IF_ERROR_RETURN(READ_FT_AGE_PROFILEm(unit, MEM_BLOCK_ALL, age_profile, &ftpm));
    ticks = soc_mem_field32_get(unit, FT_AGE_PROFILEm, &ftpm, TIMEOUTf);
    session->inactivity_timeout = ticks * _REGEX_SESSION_AGE_PROFILE_TICKS_US;
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "ticks = 0x%x, timeout = %d\n"),
              ticks, session->inactivity_timeout));

    soc_mem_field_get(unit, mem, hw_entry, ID_TIMESTAMPf, &session->timestamp);
    soc_mem_field_get(unit, mem, hw_entry, STATIC_BITf, &static_bit);

    session->flags = 0;
    if (static_bit) {
        session->flags |= BCM_REGEX_SESSION_STATIC;
    }

    soc_mem_field_get(unit, mem, hw_entry, FLOW_STATEf, &flow_state);
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "flow_state = 0x%08x\n"),
              flow_state));

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "session->flags = 0x%x\n"),
              session->flags));

    if (flags & _REGEX_GET_KEY_FROM_HW_ENTRY) {
        direction = soc_mem_field32_get(unit, mem, hw_entry, KEY_DIRECTIONf);
        soc_mem_field_get(unit, mem, hw_entry, KEYf, key_field);

        _bcm_ft_key_field_get(key_field, _REGEX_KEY_FIELD_V4_OFFSET_PORT_1,
                              _REGEX_KEY_FIELD_V4_LENGTH_PORT_1, &port1);
        _bcm_ft_key_field_get(key_field, _REGEX_KEY_FIELD_V4_OFFSET_PORT_2,
                              _REGEX_KEY_FIELD_V4_LENGTH_PORT_2, &port2);
        _bcm_ft_key_field_get(key_field, _REGEX_KEY_FIELD_V4_OFFSET_PROTOCOL,
                              _REGEX_KEY_FIELD_V4_LENGTH_PROTOCOL, &protocol);
        _bcm_ft_key_field_get(key_field, _REGEX_KEY_FIELD_V4_OFFSET_IP_1,
                              _REGEX_KEY_FIELD_V4_LENGTH_IP_1, &ip1);
        _bcm_ft_key_field_get(key_field, _REGEX_KEY_FIELD_V4_OFFSET_IP_2,
                              _REGEX_KEY_FIELD_V4_LENGTH_IP_2, &ip2);

        key->protocol = protocol;

        key->flags = 0;

        if (direction == _REGEX_SESSION_DIRECTION_REVERSE) {
          key->sip = ip1;
          key->dip = ip2;
          key->src_port = port1;
          key->dst_port = port2;
        } else {
          key->dip = ip1;
          key->sip = ip2;
          key->dst_port = port1;
          key->src_port = port2;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ft_session_ipv6_from_hw
 * Purpose:
 *      Convert a TR3 FT session IPV6 HW entry to a hardware-independent
 *      FT session IPV6 entry
 * Parameters:
 *      unit - Unit number
 *      flags - (OUT) FT session flags (e.g. static entry)
 *      key - (OUT) Hardware-independent FT session key
 *      session - (OUT) Hardware-independent FT session info
 *      hw_entry6 - Triumph 3 HW entry
 */

STATIC int
_bcm_tr3_ft_session_ipv6_from_hw(int unit, int flags,
                            bcm_regex_session_t *session,
                            bcm_regex_session_key_t *key,
                            ft_session_ipv6_entry_t *ft_hw_entry6)
{
    uint32 key_upper[_REGEX_KEY_UPPER_LENGTH/32 + 1], static_bit, flow_state;
    uint32 key_lower[_REGEX_KEY_LOWER_LENGTH/32 + 1], direction;
    bcm_ip6_t sip6, dip6;
    uint32 type, type_0, valid, port1, port2, protocol;
    soc_mem_t mem = FT_SESSION_IPV6m;
    const void *hw_entry6 = (const void *) ft_hw_entry6;
    int i, age_profile, ticks;
    ft_age_profile_entry_t ftpm;

    /* Check for both key types in IPV6 entry */
    soc_mem_field_get(unit, mem, hw_entry6, KEY_TYPEf, &type);
    if (type != _REGEX_SESSION_KEY_TYPE_IPV6_UPPER) {
        return BCM_E_UNAVAIL;
    }

    soc_mem_field_get(unit, mem, hw_entry6, KEY_TYPE_0f, &type_0);
    if (type_0 != _REGEX_SESSION_KEY_TYPE_IPV6_LOWER) {
        return BCM_E_UNAVAIL;
    }

    /* Check validity of upper and lower keys */
    soc_mem_field_get(unit, mem, hw_entry6, VALIDf, &valid);
    if (valid != 1) {
        return BCM_E_UNAVAIL;
    }

    soc_mem_field_get(unit, mem, hw_entry6, VALID_0f, &valid);
    if (valid != 1) {
        return BCM_E_UNAVAIL;
    }

    soc_mem_field_get(unit, mem, hw_entry6, POLICY_INDEXf, (uint32 *) &session->policy);

    /*
    Granularity of AGE_PROFILE entries is 10ms, and inactivity_timeout
    is in usecs.
    */

    age_profile = soc_FT_SESSION_IPV6m_field32_get(unit, hw_entry6, AGE_PROFILEf);
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "age_profile = 0x%x\n"),
              age_profile));
    SOC_IF_ERROR_RETURN(READ_FT_AGE_PROFILEm(unit, MEM_BLOCK_ALL, age_profile, &ftpm));
    ticks = soc_mem_field32_get(unit, FT_AGE_PROFILEm, &ftpm, TIMEOUTf);
    session->inactivity_timeout = ticks * _REGEX_SESSION_AGE_PROFILE_TICKS_US;
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "ticks = 0x%x, timeout = %d\n"),
              ticks, session->inactivity_timeout));

    soc_mem_field_get(unit, mem, hw_entry6, ID_TIMESTAMPf, &session->timestamp);
    soc_mem_field_get(unit, mem, hw_entry6, STATIC_BITf, &static_bit);

    session->flags = 0;
    if (static_bit) {
        session->flags |= BCM_REGEX_SESSION_STATIC;
    }

    soc_mem_field_get(unit, mem, hw_entry6, FLOW_STATEf, &flow_state);
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "flow_state = 0x%08x\n"),
              flow_state));

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "session->flags = 0x%x\n"),
              session->flags));

    if (flags & _REGEX_GET_KEY_FROM_HW_ENTRY) {
        /*
        IPV6 addresses are stored in two different fields -- the KEY field,
        and the IPV6_LOWER_KEY field.  To reconstruct the addresses, both
        fields must be used, especially as 104 bits of one IPV6 address is
        in  the upper key field, and the other 24 bits are in the lower key
        field, while one IPV6 address is stored entirely in the lower key
        field.
        */

        direction = soc_mem_field32_get(unit, mem, hw_entry6, KEY_DIRECTIONf);
        soc_mem_field_get(unit, mem, hw_entry6, KEYf, key_upper);
        soc_mem_field_get(unit, mem, hw_entry6, IPV6_LOWER_KEYf, key_lower);

        _bcm_ft_key_field_get(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_PORT_1,
                              _REGEX_KEY_LOWER_V6_LENGTH_PORT_1, &port1);
        _bcm_ft_key_field_get(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_PORT_2,
                              _REGEX_KEY_LOWER_V6_LENGTH_PORT_2, &port2);
        _bcm_ft_key_field_get(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_PROTOCOL,
                              _REGEX_KEY_LOWER_V6_LENGTH_PROTOCOL, &protocol);

        key->protocol = protocol;

        key->flags = BCM_REGEX_SESSION_KEY_IPV6;

        if (direction == _REGEX_SESSION_DIRECTION_REVERSE) {
          key->src_port = port1;
          key->dst_port = port2;
        } else {
          key->dst_port = port1;
          key->src_port = port2;
        }

        if (direction == _REGEX_SESSION_DIRECTION_FORWARD) {
            _bcm_ft_key_field_get_char(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_IP_1,
                                  _REGEX_KEY_LOWER_V6_LENGTH_IP_1, dip6);
        } else {
            _bcm_ft_key_field_get_char(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_IP_1,
                                  _REGEX_KEY_LOWER_V6_LENGTH_IP_1, sip6);
        }
        if (direction == _REGEX_SESSION_DIRECTION_FORWARD) {
            _bcm_ft_key_field_get_char(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_IP_2,
                                  _REGEX_KEY_LOWER_V6_LENGTH_IP_2, sip6);
        } else {
            _bcm_ft_key_field_get_char(key_lower, _REGEX_KEY_LOWER_V6_OFFSET_IP_2,
                                  _REGEX_KEY_LOWER_V6_LENGTH_IP_2, dip6);
        }
        if (direction == _REGEX_SESSION_DIRECTION_FORWARD) {
            _bcm_ft_key_field_get_char(key_upper, _REGEX_KEY_UPPER_V6_OFFSET_IP_2,
                                       _REGEX_KEY_UPPER_V6_LENGTH_IP_2,
                                       &sip6[_REGEX_KEY_LOWER_V6_LENGTH_IP_2/8]);
        } else {
            _bcm_ft_key_field_get_char(key_upper, _REGEX_KEY_UPPER_V6_OFFSET_IP_2,
                                       _REGEX_KEY_UPPER_V6_LENGTH_IP_2,
                                       &dip6[_REGEX_KEY_LOWER_V6_LENGTH_IP_2/8]);
        }

        /*
        IPv6 addresses will go into hardware in reverse order, so reverse
        the addresses here before returning them
        */

        for (i = 0; i < _SHR_L3_IP6_ADDRLEN; i++) {
            key->sip6[i] = sip6[_SHR_L3_IP6_ADDRLEN - i - 1];
            key->dip6[i] = dip6[_SHR_L3_IP6_ADDRLEN - i - 1];
        }
    }

    return BCM_E_NONE;
}

int _bcm_tr3_regex_session_add(int unit,
                              int flags,
                              bcm_regex_session_key_t *key,
                              bcm_regex_session_t *session)
{
    int index, rv = BCM_E_UNAVAIL;
    ft_session_entry_t session_entry, session_entry_lookup;
    ft_session_ipv6_entry_t session_entry6, session_entry_lookup6;
    soc_mem_t mem;

    BCM_REGEX_LOCK(unit);
    if (key->flags & BCM_REGEX_SESSION_KEY_IPV6) {
        sal_memset(&session_entry6, 0, sizeof(ft_session_ipv6_entry_t));
        mem = FT_SESSION_IPV6m;
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ft_session_ipv6_to_hw(unit, &session_entry6, flags, key, session));
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            (void *)&session_entry6,
                            (void *)&session_entry_lookup6, 0);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            goto end;
        } else if (rv == BCM_E_NOT_FOUND) {
            if ((key->flags & BCM_REGEX_SESSION_KEY_REPLACE) != 0) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "soc_mem_insert ipv6 rv: %d, can't replace non-existent entry\n"),
                          rv));
                goto end;
            }
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "session_add ipv6\n")));
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, (void *)&session_entry6);
        } else if (rv == BCM_E_NONE) {
            if ((key->flags & BCM_REGEX_SESSION_KEY_REPLACE) == 0) {
                rv = BCM_E_EXISTS;
                goto end;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_ft_session_ipv6_to_hw(unit, &session_entry_lookup6, flags, key, session));
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "session_modify ipv6\n")));
                rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, (void *)&session_entry_lookup6);
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "soc_mem_insert ipv6 rv: %d, OK for modify\n"),
                          rv));
                if (rv == BCM_E_EXISTS) {
                    rv = BCM_E_NONE; /* OK to get BCM_E_EXISTS in update case, insert still happens */
                }
            }
        }
    } else {
        sal_memset(&session_entry, 0, sizeof(ft_session_entry_t));
        mem = FT_SESSIONm;
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ft_session_to_hw(unit, &session_entry, flags, key, session));
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            (void *)&session_entry,
                            (void *)&session_entry_lookup, 0);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            goto end;
        } else if (rv == BCM_E_NOT_FOUND) {
            if ((key->flags & BCM_REGEX_SESSION_KEY_REPLACE) != 0) {
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "soc_mem_insert ipv4 rv: %d, can't replace non-existent entry\n"),
                          rv));
                goto end;
            }
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "session_add ipv4\n")));
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, (void *)&session_entry);
        } else if (rv == BCM_E_NONE) {
            if ((key->flags & BCM_REGEX_SESSION_KEY_REPLACE) == 0) {
                rv = BCM_E_EXISTS;
                goto end;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_ft_session_to_hw(unit, &session_entry_lookup, flags, key, session));
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "session_modify ipv4\n")));
                rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, (void *)&session_entry_lookup);
                LOG_INFO(BSL_LS_BCM_REGEX,
                         (BSL_META_U(unit,
                                     "soc_mem_insert ipv4 rv: %d, OK for modify\n"),
                          rv));
                if (rv == BCM_E_EXISTS) {
                    rv = BCM_E_NONE; /* OK to get BCM_E_EXISTS in update case, insert still happens */
                }
            }
        }
    }

end:
    BCM_REGEX_UNLOCK(unit);
    return rv;
}


int _bcm_tr3_regex_session_policy_update(int unit, int flags, int flow_index,
                                         bcm_regex_policy_t policy)
{
    int rv = BCM_E_UNAVAIL;
    ft_session_entry_t session_entry;
    ft_session_ipv6_entry_t session_entry6;
    soc_mem_t mem;

    BCM_REGEX_LOCK(unit);
    if (flags & BCM_REGEX_SESSION_KEY_IPV6) {
        sal_memset(&session_entry6, 0, sizeof(ft_session_ipv6_entry_t));
        mem = FT_SESSION_IPV6m;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, flow_index, &session_entry6);
        if (BCM_FAILURE(rv)) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "soc_mem_read ipv6 rv: %d, can't update non-existent entry\n"),
                      rv));
            goto end;
        }
        soc_FT_SESSION_IPV6m_field32_set(unit, &session_entry6, POLICY_INDEXf,
                                         policy);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, flow_index, &session_entry6);
        if (BCM_FAILURE(rv)) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "soc_mem_write ipv6 rv: %d, can't write entry\n"),
                      rv));
            goto end;
        }
    } else {
        sal_memset(&session_entry, 0, sizeof(ft_session_entry_t));
        mem = FT_SESSIONm;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, flow_index, &session_entry);
        if (BCM_FAILURE(rv)) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "soc_mem_read ipv4 rv: %d, can't update non-existent entry\n"),
                      rv));
            goto end;
        }
        soc_FT_SESSIONm_field32_set(unit, &session_entry, POLICY_INDEXf,
                                         policy);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, flow_index, &session_entry);
        if (BCM_FAILURE(rv)) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "soc_mem_write ipv4 rv: %d, can't write entry\n"),
                      rv));
            goto end;
        }
    }

end:
    BCM_REGEX_UNLOCK(unit);
    return rv;
}

int _bcm_tr3_regex_session_delete_all(int unit)
{
    int rv = BCM_E_NONE;

    BCM_REGEX_LOCK(unit);
    /*
     * Session table is common for IPv4 and IPv6 entries
     * Clearing FT_SESSIONm will remove all entries
     */
    rv = soc_mem_clear(unit, FT_SESSIONm, MEM_BLOCK_ALL, TRUE);
    BCM_REGEX_UNLOCK(unit);

    return rv;
}

int _bcm_tr3_regex_session_delete(int unit,
                              bcm_regex_session_key_t *key)
{
    int index, rv = BCM_E_UNAVAIL;
    int flags = 0;
    ft_session_entry_t session_entry, session_entry_lookup;
    ft_session_ipv6_entry_t session_entry6, session_entry_lookup6;
    soc_mem_t mem;

    BCM_REGEX_LOCK(unit);
    if (key->flags & BCM_REGEX_SESSION_KEY_IPV6) {
        sal_memset(&session_entry6, 0, sizeof(ft_session_ipv6_entry_t));
        mem = FT_SESSION_IPV6m;
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ft_session_ipv6_to_hw(unit, &session_entry6, flags, key, NULL));
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            (void *)&session_entry6,
                            (void *)&session_entry_lookup6, 0);
        if (BCM_FAILURE(rv)) {
            goto end;
        } else if (rv == BCM_E_NONE) {
            rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, (void *)&session_entry6);
        }
    } else {
        sal_memset(&session_entry, 0, sizeof(ft_session_entry_t));
        mem = FT_SESSIONm;
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ft_session_to_hw(unit, &session_entry, flags, key, NULL));
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            (void *)&session_entry,
                            (void *)&session_entry_lookup, 0);
        if (BCM_FAILURE(rv)) {
            goto end;
        } else if (rv == BCM_E_NONE) {
            rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, (void *)&session_entry);
        }
    }

end:
    BCM_REGEX_UNLOCK(unit);
    return rv;
}

int _bcm_tr3_regex_session_get(int unit,
                              int flags,
                              bcm_regex_session_key_t *key,
                              bcm_regex_session_t *session)
{
    int index, rv = BCM_E_UNAVAIL;
    ft_session_entry_t session_entry, session_entry_lookup;
    ft_session_ipv6_entry_t session_entry6, session_entry_lookup6;
    soc_mem_t mem;

    if (key->flags & BCM_REGEX_SESSION_KEY_IPV6) {
        sal_memset(&session_entry6, 0, sizeof(ft_session_ipv6_entry_t));
        mem = FT_SESSION_IPV6m;
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ft_session_ipv6_to_hw(unit, &session_entry6, flags, key, NULL));
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            (void *)&session_entry6,
                            (void *)&session_entry_lookup6, 0);
        if (BCM_FAILURE(rv)) {
            return rv;
        } else if (rv == BCM_E_NONE) {
            return _bcm_tr3_ft_session_ipv6_from_hw(unit, flags, session, key,
                                                    &session_entry_lookup6);
        }
    } else {
        sal_memset(&session_entry, 0, sizeof(ft_session_entry_t));
        mem = FT_SESSIONm;
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ft_session_to_hw(unit, &session_entry, flags, key, NULL));
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            (void *)&session_entry,
                            (void *)&session_entry_lookup, 0);
        if (BCM_FAILURE(rv)) {
            return rv;
        } else if (rv == BCM_E_NONE) {
            return _bcm_tr3_ft_session_from_hw(unit, flags, session, key,
                                                    &session_entry_lookup);
        }
    }

    return rv;
}

int _bcm_tr3_regex_session_get_by_index(int unit, int flags, int index,
                                        bcm_regex_session_key_t *key,
                                        bcm_regex_session_t *session)
{
    int rv = BCM_E_UNAVAIL;
    ft_session_entry_t session_entry;
    ft_session_ipv6_entry_t session_entry6;
    soc_mem_t mem;

    if (flags & _REGEX_ENTRY_TYPE_IS_IPV6) {
        sal_memset(&session_entry6, 0, sizeof(ft_session_ipv6_entry_t));
        mem = FT_SESSION_IPV6m;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &session_entry6);
        if (BCM_FAILURE(rv)) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "soc_mem_read ipv6 rv: %d, can't get non-existent entry\n"),
                      rv));
            goto end;
        } else if (rv == BCM_E_NONE) {
            return _bcm_tr3_ft_session_ipv6_from_hw(unit, flags, session, key,
                                                    &session_entry6);
        }
    } else {
        sal_memset(&session_entry, 0, sizeof(ft_session_entry_t));
        mem = FT_SESSIONm;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &session_entry);
        if (BCM_FAILURE(rv)) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 "soc_mem_read ipv4 rv: %d, can't get non-existent entry\n"),
                      rv));
            goto end;
        } else if (rv == BCM_E_NONE) {
            return _bcm_tr3_ft_session_from_hw(unit, flags, session, key,
                                                    &session_entry);
        }
    }

end:
    return rv;
}

int _bcm_tr3_regex_session_traverse(int unit,
                                    int flags,
                                    bcm_regex_session_traverse_cb cb,
                                    void *user_data)
{
    int i, index_min4, index_max4, rv = BCM_E_NONE;
    int index_min6, index_max6;
    int valid, type, *buffer4 = NULL, *buffer6 = NULL;
    ft_session_entry_t *hw_entry4 = NULL;
    ft_session_ipv6_entry_t *hw_entry6 = NULL;
    bcm_regex_session_t info;
    bcm_regex_session_key_t key;
    uint32 static_bit, flow_state;

    index_min4 = soc_mem_index_min(unit, FT_SESSIONm);
    index_max4 = soc_mem_index_max(unit, FT_SESSIONm);
    index_min6 = soc_mem_index_min(unit, FT_SESSION_IPV6m);
    index_max6 = soc_mem_index_max(unit, FT_SESSION_IPV6m);

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "index_min4: %d, index_max4: %d, index_min6: %d, index_max6: %d\n"),
              index_min4, index_max4, index_min6, index_max6));

    BCM_REGEX_LOCK(unit);
    buffer4 = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, FT_SESSIONm),
                           "flow tracker session traverse ipv4");
    if (NULL == buffer4) {
        rv = BCM_E_MEMORY;
        goto end;
    }

    if ((rv = soc_mem_read_range(unit, FT_SESSIONm, MEM_BLOCK_ALL,
                                 index_min4, index_max4, buffer4)) < 0 ) {
        soc_cm_sfree(unit, buffer4);
        goto end;
    }

    buffer6 = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, FT_SESSION_IPV6m),
                           "flow tracker session traverse ipv6");
    if (NULL == buffer6) {
        soc_cm_sfree(unit, buffer4);
        rv = BCM_E_MEMORY;
        goto end;
    }

    if ((rv = soc_mem_read_range(unit, FT_SESSION_IPV6m, MEM_BLOCK_ALL,
                                 index_min6, index_max6, buffer6)) < 0 ) {
        goto free_end;
    }

    for (i = index_min4; i <= index_max4; i++) {
        hw_entry4 = soc_mem_table_idx_to_pointer(unit, FT_SESSIONm,
                                         ft_session_entry_t *, buffer4, i);
        valid = soc_FT_SESSIONm_field32_get(unit, hw_entry4, VALIDf);
        if (!valid) {
            continue;
        }

        /*
        Only look at entries of type IPV4 or IPV6 lower (which will
        account for the IPV6 upper half when taken as a whole
        */
        type = soc_FT_SESSIONm_field32_get(unit, hw_entry4, KEY_TYPEf);
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "entry: %d, key_type = %d\n"),
                  i, type));

        if (type == _REGEX_SESSION_KEY_TYPE_RESERVED ||
            type == _REGEX_SESSION_KEY_TYPE_IPV6_UPPER) {
            continue;
        }

        if (type != _REGEX_SESSION_KEY_TYPE_IPV4) {
            hw_entry6 = soc_mem_table_idx_to_pointer(unit, FT_SESSION_IPV6m,
                                             ft_session_ipv6_entry_t *, buffer6, i/2);

            valid = soc_FT_SESSION_IPV6m_field32_get(unit, hw_entry6, VALID_0f);
            if (!valid) {
                continue;
            }

            static_bit = soc_FT_SESSION_IPV6m_field32_get(unit, hw_entry6, STATIC_BITf);
            flow_state = soc_FT_SESSION_IPV6m_field32_get(unit, hw_entry6, FLOW_STATEf);
        }
        else {
            static_bit = soc_FT_SESSIONm_field32_get(unit, hw_entry4, STATIC_BITf);
            flow_state = soc_FT_SESSIONm_field32_get(unit, hw_entry4, FLOW_STATEf);
        }

        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "flow_state = 0x%08x, static_bit = 0x%x\n"),
                  flow_state, static_bit));

        if ((flags & BCM_REGEX_SESSION_TRAVERSE_ALL) == 0 && static_bit == 0) {
            continue;
        }

        if (flow_state == _REGEX_SESSION_FLOW_STATE_INVALID ||
            flow_state == _REGEX_SESSION_FLOW_STATE_RETIRED ||
            flow_state == _REGEX_SESSION_FLOW_STATE_RESERVED) {
            continue;
        }

        bcm_regex_session_t_init(&info);
        bcm_regex_session_key_t_init(&key);

        if (type != _REGEX_SESSION_KEY_TYPE_IPV4) {
            rv = _bcm_tr3_ft_session_ipv6_from_hw(unit, _REGEX_GET_KEY_FROM_HW_ENTRY, &info, &key, hw_entry6);
        }
        else
        {
            rv = _bcm_tr3_ft_session_from_hw(unit, _REGEX_GET_KEY_FROM_HW_ENTRY, &info, &key, hw_entry4);
        }
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "rv = %d (0x%08x)\n"),
                  rv, rv));

        if (BCM_FAILURE(rv)) {
            goto free_end;
        }

        rv = cb(unit, &key, &info, user_data);

        if (BCM_FAILURE(rv)) {
            goto free_end;
        }
    }

free_end:
    soc_cm_sfree(unit, buffer4);
    soc_cm_sfree(unit, buffer6);

end:
    BCM_REGEX_UNLOCK(unit);
    return rv;
}

STATIC int
_bcm_tr3_regex_hw_dfa_relocate(int unit, 
                                soc_mem_t state_mem,
                                int num_lines, int prev_offset, int new_offset,
                                axp_sm_state_table_mem0_entry_t *state_membuf,
                                soc_mem_t to_match_mem, 
                                soc_mem_t from_match_mem,
                                axp_sm_match_table_mem0_entry_t *match_membuf, 
                                int *to_match_alloc_base,
                                int *setid_remap, int setid_tblsz,
                                int update_db)
{
    int i, rvo, adj, sigid, ptrv;
    soc_field_t minf, maxf, matchf, ptrf;
    uint32  minv, maxv, matchv;
    axp_sm_state_table_mem0_entry_t *pstmem;
    axp_sm_match_table_mem0_entry_t *pmtmem;
    int old_chkid, old_setid;
    int next_index = *to_match_alloc_base;
    _bcm_regexdb_entry_t* dbe = NULL;

    adj = (new_offset - prev_offset) * BCM_TR3_NUM_RV_PER_LINE;
    for (i = 0; i < num_lines; i++) {
        pstmem = state_membuf + i;
        for (rvo = 0; rvo < BCM_TR3_NUM_RV_PER_LINE; rvo++) {
            _bcm_tr3_regex_rv_fields_get(unit, rvo, &minf, &maxf, &matchf, &ptrf);
            
            minv = soc_mem_field32_get(unit, state_mem, (void*)pstmem, minf);
            maxv = soc_mem_field32_get(unit, state_mem, (void*)pstmem, maxf);
            if ((minv == 255) && ((maxv == 2) || (maxv == 1))) {
                /* match state */
                matchv = soc_mem_field32_get(unit, state_mem, 
                                             (void*)pstmem, matchf);
                /* get the match entry */
                pmtmem = match_membuf + matchv;
                
                sigid = soc_mem_field32_get(unit, from_match_mem, pmtmem, 
                                                           REPORT_SIGNATURE_IDf);

                if (update_db) {
                    dbe = bcm_regexdb_find_match_entry_by_sig_id(unit, sigid);
                    if (!dbe) {
                        return BCM_E_INTERNAL;
                    }
                }

                /* check if setid or checkid needs relocation. */
                if (soc_mem_field32_get(unit, from_match_mem, pmtmem, 
                                                    MATCH_ENTRY_VALIDf)) {
                    if (soc_mem_field32_get(unit, from_match_mem, 
                                        pmtmem, MATCH_CROSS_SIG_FLAG_SETf)) {
                        /* set the new setid */
                        old_chkid = soc_mem_field32_get(unit, from_match_mem, 
                                          pmtmem, MATCH_CROSS_SIG_FLAG_INDEXf);
                        
                        soc_mem_field32_set(unit, from_match_mem, 
                                        pmtmem, MATCH_CROSS_SIG_FLAG_INDEXf, 
                                        setid_remap[old_chkid]);
                        dbe->match_set_id = setid_remap[old_chkid];
                    }
                    
                    if (soc_mem_field32_get(unit, from_match_mem, 
                                        pmtmem, CHECK_CROSS_SIG_FLAG_IS_SETf)) {
                        /* set the new chkid */
                        old_setid  = soc_mem_field32_get(unit, from_match_mem, 
                                          pmtmem, CHECK_CROSS_SIG_FLAG_INDEXf);
                        
                        soc_mem_field32_set(unit, from_match_mem, 
                                        pmtmem, CHECK_CROSS_SIG_FLAG_INDEXf, 
                                        setid_remap[old_setid]);
                        dbe->match_chk_id = setid_remap[old_setid];
                    }
                }
                
                /* write the entry to new match memory */
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, to_match_mem, 
                                    MEM_BLOCK_ALL, next_index, pmtmem));
                next_index++;

            } else {
                ptrv = soc_mem_field32_get(unit, state_mem, (void*)pstmem, ptrf);
                if (ptrv != 0xffff) {
                    ptrv += adj;
                    if (ptrv >= 0xffff) {
                        ptrv=0xffff; 
                    }
                }
                soc_mem_field32_set(unit, state_mem, (void*)pstmem, ptrf, ptrv);
            }
        }
	soc_mem_write(unit, state_mem, MEM_BLOCK_ALL, new_offset+i, pstmem);
    }

    *to_match_alloc_base = next_index;
    return 0;
}

STATIC int
_bcm_tr3_arrange_resources_in_tiles(int unit, _bcm_regex_resource_t *res, 
                                    int num_res, int to_tiles, int to_tilee,
                                    _bcm_regex_tile_t *_tr3tile, int num_tile)
{
    _bcm_tr3_regex_device_info_t *device;
    int i, t, tile_lsz, placed;
    int max_per_tile;

    if (to_tilee >= num_tile) {
        return BCM_E_PARAM;
    }

    device = REGEX_INFO(unit);

    max_per_tile = BCM_TR3_ENGINE_PER_TILE(device);
    tile_lsz = _bcm_tr3_get_tile_line_size(unit);
    sal_memset(_tr3tile, 0, sizeof(_bcm_regex_tile_t)*num_tile);
    for (i = to_tiles; i <= to_tilee; i++) {
        _tr3tile[i].valid = 1;
        _tr3tile[i].avail = tile_lsz;
        _tr3tile[i].num_o = 0;
    }

    for (i = 0; i < num_res; i++) {
        placed = 0;
        for (t = to_tiles; t <= to_tilee; t++) {
            if (!_tr3tile[t].valid) {
                continue;
            }

            if ((_tr3tile[t].avail >= res[i].lsize) && 
                (_tr3tile[t].num_o < max_per_tile)) {
                _tr3tile[t].oidx[_tr3tile[t].num_o++] = i;
                _tr3tile[t].avail -= res[i].lsize;
                placed = 1;
                break;
            }
        }
        if (!placed) {
            return BCM_E_RESOURCE;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_tr3_rearrange_engines_in_hw(int unit, _bcm_regex_resource_t *res, 
                           int num_res, int to_tiles, int to_tilee, 
                           _bcm_regex_tile_t *_tr3tile)
{
    _bcm_tr3_regex_device_info_t *device;
    int i, t, j, k, last_match_offset, off;
    int max_per_tile, maxm, old_tile;
    axp_sm_state_table_mem0_entry_t *pmem;
    _bcm_regex_resource_t *pres;
    _bcm_regex_engine_obj_t physical_engines[BCM_TR3_MAX_ENGINE];
    int *old2new, *new2old, *oldleid2peid, *old_setid2new;
    int new_peid, old_peid, new_setidb, old_setidb;
    soc_mem_t mem, from_match_mem, to_match_mem;
    _bcm_regex_engine_obj_t *pengine = NULL, *old_pe, *new_pe;
    int rv = BCM_E_NONE, match_off;
    _bcm_regex_lengine_t *lengine;
    uint32 *alloc_bmap;
    axp_sm_state_table_mem0_entry_t *dfa_cp[BCM_TR3_MAX_ENGINE], *pdfa; 

    if (!num_res) {
        return BCM_E_NONE;
    }

    device = REGEX_INFO(unit);

    max_per_tile = BCM_TR3_ENGINE_PER_TILE(device);
    for (t=to_tiles; t <= to_tilee; t++) {
        _tr3tile[t].match_mem =  NULL;
    }
   
    old2new = sal_alloc(sizeof(int)*BCM_TR3_MAX_ENGINE, "old2new_emap");
    new2old = sal_alloc(sizeof(int)*BCM_TR3_MAX_ENGINE, "new2old_emap");
    oldleid2peid = sal_alloc(sizeof(int)*BCM_TR3_MAX_ENGINE, "old2new_peid");
    old_setid2new = sal_alloc(sizeof(int)*BCM_TR3_MAX_SLOTS, "setid_map");
    sal_memset(dfa_cp, 0, 
               sizeof(axp_sm_state_table_mem0_entry_t*)*BCM_TR3_MAX_ENGINE);

    if (!(old2new && new2old && oldleid2peid && old_setid2new)) {
        rv = BCM_E_MEMORY;
        goto fail;
    }

    for (i = 0; i < BCM_TR3_MAX_ENGINE; i++) {
        old2new[i] = -1;
        new2old[i] = -1;
        oldleid2peid[i] = -1;
    }

    for (i = 0; i < BCM_TR3_MAX_SLOTS; i++) {
        old_setid2new[i] = -1;
    }

    /*
     * Read the dfa states from HW.
     */
    for (t=to_tiles; t<=to_tilee; t++) {
        if (_tr3tile[t].num_o == 0) {
            continue;
        }

        /* disable the tile */
        _bcm_tr3_disable_tile(unit, t);

        for (i = 0; i < _tr3tile[t].num_o; i++) {
            new_peid = (t * max_per_tile) + i;
            pres = res + _tr3tile[t].oidx[i];
            pengine = pres->pres;
            old_peid = pengine->hw_idx;
            /* add to old2new map table */
            old2new[old_peid] = new_peid;
            new2old[new_peid] = old_peid;
            pmem = sal_alloc(sizeof(axp_sm_state_table_mem0_entry_t)*pengine->req_lsz, 
                                "dfa_cp");
            dfa_cp[old_peid] = pmem;
            _bcm_tr3_regex_read_hw_dfa(unit, pengine, pmem);
        }
        if (_tr3tile[t].match_mem == NULL) {
            mem = AXP_SM_MATCH_TABLE_MEM0m + 
                BCM_REGEX_ENG2MATCH_MEMIDX(unit,pengine->hw_idx);
            maxm = soc_mem_index_max(unit, mem);
            _tr3tile[t].match_mem = sal_alloc(
                sizeof(axp_sm_match_table_mem0_entry_t)*(maxm+1), "match_mem_cp");
    
            /* read the match memory */
            soc_mem_read_range(unit, mem, MEM_BLOCK_ALL, 0, 
                                maxm, _tr3tile[t].match_mem);

            /* clear out the match memory */
            soc_mem_clear(unit, mem, MEM_BLOCK_ALL, 1);
        }
    }

    /*
     * Adjust the logical to physical engine mapping.
     */
    sal_memcpy(physical_engines, device->physical_engines, 
                sizeof(physical_engines));
   
    /* remap logical to physcial mapping */
    for (i=0; i<BCM_TR3_NUM_ENGINE(device); i++) {
        lengine = &device->engines[i];
        if ((pengine = lengine->physical_engine) == NULL) {
            continue;
        }
        old_peid = pengine->hw_idx;
        oldleid2peid[i] = old_peid;
        lengine->physical_engine = NULL;

        pengine->total_lsz = 0;
        pengine->flags = 0;
        pengine->from_line = -1;
        pengine->req_lsz = 0;
        pengine->cps.flags = 0;
        pengine->cps.from_line = -1;
        pengine->cps.req_lsz = 0;
        pengine->total_lsz = 0;
    }

    /* remap logical engines to new physical engines */
    for (i=0; i<BCM_TR3_NUM_ENGINE(device); i++) {
        if (oldleid2peid[i] == -1) {
            continue;
        }
        old_peid = oldleid2peid[i];
        old_pe = &physical_engines[old_peid];
        new_peid = old2new[old_peid];
        new_pe = &device->physical_engines[new_peid];
        device->engines[i].physical_engine = new_pe;

        /* assign engine info/params to new engine from old engine */

        /* keep the from offset since we might have to adjust the 
         * jump offsets in the DFA code. */
        new_pe->from_line   = old_pe->from_line;
        new_pe->req_lsz     = old_pe->req_lsz;
        new_pe->total_lsz   = old_pe->total_lsz;
        new_pe->flags       = old_pe->flags;
        sal_memcpy(&new_pe->cps, &old_pe->cps, sizeof(_bcm_regex_cps_obj_t));
        new_setidb = new_peid * BCM_REGEX_NUM_SLOT_PER_CPS;
        new_pe->cps.foff = new_setidb;
        old_setidb = old_peid * BCM_REGEX_NUM_SLOT_PER_CPS;
        for (k = 0; k < BCM_REGEX_NUM_SLOT_PER_CPS; k++) {
            old_setid2new[old_setidb + k] = new_setidb + k;
        }
    }

    /* flush the DFA to new location/tiles */
    for (t = to_tiles; t <= to_tilee; t++) {
        sal_memset(device->match_alloc_bmap[t], 0, 
                                        BCM_TR3_MATCH_BMAP_BSZ*sizeof(uint32));
        off = 0;
        match_off = 0;
        pengine = &device->physical_engines[t*max_per_tile];
        mem = _bcm_tr3_get_axp_sm_memory(unit, t);
        for (i = 0; i < BCM_TR3_ENGINE_PER_TILE(device); i++, pengine++) {
            if (!BCM_REGEX_ENGINE_ALLOCATED(pengine)) {
                continue;
            }
            pdfa = dfa_cp[new2old[pengine->hw_idx]];   
            if (pdfa == NULL) {
                continue;
            }

            to_match_mem = AXP_SM_MATCH_TABLE_MEM0m + 
                    BCM_REGEX_ENG2MATCH_MEMIDX(unit,pengine->hw_idx);

            from_match_mem = AXP_SM_MATCH_TABLE_MEM0m + 
                    BCM_REGEX_ENG2MATCH_MEMIDX(unit,new2old[pengine->hw_idx]);

            old_tile = new2old[pengine->hw_idx]/max_per_tile;

            last_match_offset = match_off;

            /* adjust the jump offsets */
            _bcm_tr3_regex_hw_dfa_relocate(unit, mem, pengine->req_lsz, 
                                           pengine->from_line, off,
                                           pdfa,
                                           to_match_mem, 
                                           from_match_mem, 
                                           _tr3tile[old_tile].match_mem,
                                           &match_off,
                                           old_setid2new, 512, 1);

            /* set the base for DFA in this engine */
            pengine->from_line = off;
            off += pengine->total_lsz;

            if (pengine->cps.from_line >= 0) {
                
            }

            /* update the alloc map */
            alloc_bmap = &device->match_alloc_bmap[t][0];
            for (j = last_match_offset; j < match_off; j++) {
                alloc_bmap[j/32] |= (1 << (j % 32));
            }

            sal_free(pdfa);
            dfa_cp[new2old[pengine->hw_idx]] = NULL;
        }
    }

    /* free up the alloced resources */
    for (t = to_tiles; t <= to_tilee; t++) {
        if (_tr3tile[t].num_o) {
            _bcm_tr3_enable_tile(unit, t);
        }
    }

fail:
    if (old2new) {
        sal_free(old2new);
    }

    if (new2old) {
        sal_free(new2old);
    }

    if (oldleid2peid) {
        sal_free(oldleid2peid);
    }

    if (old_setid2new) {
        sal_free(old_setid2new);
    }

    for (t = to_tiles; t <= to_tilee; t++) {
        if (_tr3tile[t].match_mem) {
            sal_free(_tr3tile[t].match_mem);
            _tr3tile[t].match_mem = NULL;
        }
    }
    return rv;
}

STATIC int 
_bcm_tr3_alloc_lines_in_tile(int unit, int tile, int require_lsz,
                             int *start_line, int compress)
{
    _bcm_tr3_regex_device_info_t *device;
    int maxlines, i, num_res, as, ae, top = 0;
    _bcm_regex_resource_t *pres, *a;
    _bcm_regex_engine_obj_t *peng = NULL;
    int rv = BCM_E_NONE, lines_avail, hole;
    _bcm_regex_tile_t  tmp_tiles[BCM_TR3_MAX_TILES];

    *start_line = -1;

    device = REGEX_INFO(unit);

    maxlines = lines_avail = _bcm_tr3_get_tile_line_size(unit);
    peng = BCM_TR3_PHYSICAL_ENGINE(device, 
                                    tile*BCM_TR3_ENGINE_PER_TILE(device));
    for (i=0; i<BCM_TR3_ENGINE_PER_TILE(device); i++, peng++) {
        if (!BCM_REGEX_ENGINE_ALLOCATED(peng)) {
            continue;
        }
        lines_avail -= peng->total_lsz;
    }

    if (lines_avail < require_lsz) {
        return BCM_E_RESOURCE;
    }
    
    _bcm_tr3_make_resource_objs(unit, &pres, &num_res, tile, tile);

    if (compress) {
        /* Arrange the objects by size. */
        _bcm_regex_sort_resources_by_size(unit, pres, num_res);

        _bcm_tr3_arrange_resources_in_tiles(unit, pres, num_res, tile, tile,
                                            tmp_tiles, BCM_TR3_MAX_TILES);
        /* compress */
        if (_bcm_tr3_rearrange_engines_in_hw(unit, pres, num_res, 
                                             tile, tile, tmp_tiles)) {
            return BCM_E_INTERNAL;
        }
    } else {
        _bcm_regex_sort_resources_by_offset(unit, pres, num_res);
    }

    top = 0; 
    *start_line = -1;
    for (i=0; i<num_res; i++) {
        a = pres + i;
        as = a->pres->from_line;
        hole = as - top;
        if (hole >= require_lsz) {
            *start_line = top;
            break;
        }
        ae = as + a->pres->total_lsz - 1;
        top = ae+1;
    }

    if ((*start_line < 0) && (maxlines - top) >= require_lsz) {
        *start_line = top;
    }

    if (pres) {
        _bcm_tr3_free_resource_objs(unit, &pres, &num_res);
    }
    return rv;
}

STATIC int _bcm_tr3_alloc_regex_engine(int unit, int clsz, int extra_lsz,
                            _bcm_regex_engine_obj_t **ppeng, int tile)
{
    _bcm_tr3_regex_device_info_t *device;
    int i, j, try, alloc_lsz;
    int tiles, tilee, start_line, rv;
    _bcm_regex_engine_obj_t *peng = NULL;

    device = REGEX_INFO(unit);

    if (tile >= 0) {
        tiles = tilee = tile;
    } else {
        tiles = 0;
        tilee = device->info->num_tiles - 1;
    }

    *ppeng = NULL;
    alloc_lsz = clsz + extra_lsz;

    /*
     * 2 stage allocation, first try find memory where contingious
     * memory available, if not found, in second try, compress the
     * state memory to allocate CPS memory.
     */
    for (try=0; (try<2); try++) {
        /* iterate all the tiles and alocate cps resource */
        for (i = tiles; i <= tilee; i++) {
            peng = BCM_TR3_PHYSICAL_ENGINE(device, 
                                i*BCM_TR3_ENGINE_PER_TILE(device));
            for (j=0; j<BCM_TR3_ENGINE_PER_TILE(device); j++, peng++) {
                if (BCM_REGEX_ENGINE_ALLOCATED(peng)) {
                    continue;
                }
                /* allocate state mem. */
                rv = _bcm_tr3_alloc_lines_in_tile(unit, peng->tile,
                                        alloc_lsz, &start_line, try);
                /* unable to alloc lines in this tile, try next tile. */
                if (rv) {
                    break;
                }
                peng->from_line = start_line;
                peng->req_lsz = clsz;
                *ppeng = peng;
                BCM_REGEX_ENGINE_SET_ALLOCATED(peng);
                peng->cps.from_line = peng->from_line + peng->req_lsz;
                peng->cps.req_lsz = extra_lsz;
                peng->total_lsz = clsz + extra_lsz;
                if (extra_lsz) {
                    _bcm_tr3_init_cps_block(unit, &peng->cps);
                    BCM_REGEX_CPS_SET_ALLOCED(&peng->cps);
                }
                goto phy_engine_alloced;
            }
        }
    }

phy_engine_alloced:
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "engine engine %d allocated\n"),
              peng->hw_idx));
    return (*ppeng) ? BCM_E_NONE : BCM_E_RESOURCE;
}

int 
_bcm_tr3_get_match_id(int unit, int signature_id, int *match_id)
{
    _bcm_regexdb_entry_t *pe;
    
    pe = bcm_regexdb_find_match_entry_by_sig_id(unit, signature_id);
    if (!pe) {
        return BCM_E_NOT_FOUND;
    }

    *match_id = pe->match_id;

    return BCM_E_NONE;
}

int
_bcm_tr3_get_sig_id(int unit, int match_id, int *signature_id)
{
    _bcm_regexdb_entry_t *pe;

    pe = bcm_regexdb_find_match_entry_by_match_id(unit, match_id);
    if (!pe) {
        return BCM_E_NOT_FOUND;
    }

    *signature_id = pe->sigid;

    return BCM_E_NONE;
}

int
_bcm_tr3_regex_info_get(int unit, bcm_regex_info_t *regex_info)
{
    uint32  regval, fval;
    soc_profile_mem_table_t *table;  /* Table info array */
    int ref_count = 0, i;
    _bcm_tr3_regex_device_info_t *device;

    /*
    Session size
    */

    SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit, &regval));
    fval = soc_reg_field_get(unit, FT_CONFIGr, regval, TABLE_SIZEf);
    regex_info->session_size = _decode_max_flow(fval);

    /*
    Session free sizes
    */

    READ_FT_IN_USEr(unit, &fval);
    fval &= BCM_TR3_IN_USE_MASK;

    regex_info->session_free_size_ipv4 = regex_info->session_size - fval;
    regex_info->session_free_size_ipv6 = regex_info->session_free_size_ipv4 / 2;

    /*
    Policy sizes
    */

    device = REGEX_INFO(unit);

    if ((device->info->flags & BCM_REGEX_USE_POLICY_ID) == 0) {
        if (_bcm_tr3_ft_policy_profile[unit] != NULL &&
            _bcm_tr3_ft_policy_profile[unit]->tables != NULL) {
            table = &_bcm_tr3_ft_policy_profile[unit]->tables[0];
            regex_info->policy_size = table->index_max + 1;
            for (i = table->index_min; i <= table->index_max; i++) {
                ref_count += table->entries[i-1].ref_count ? 1 : 0;
            }
            regex_info->policy_free_size = table->index_max - ref_count;
        }
        else{
            regex_info->policy_size = 0;
            regex_info->policy_free_size = 0;
        }
    }
    
    return BCM_E_NONE;
        }


int
_bcm_tr3_regex_stat_get(int unit, bcm_regex_stat_t type, uint64 *val)
{
    uint32 statval;

    if (val == NULL) {
        return BCM_E_PARAM;
    }

    switch(type) {
        case bcmStatRegexSessionEntriesInUse:
            READ_FT_IN_USEr(unit, &statval);
            statval &= BCM_TR3_IN_USE_MASK;
            *val = (uint64) statval;
            break;

        case bcmStatRegexSessionFlowsCreated:
            READ_FT_FLOWS_CREATEDr(unit, &statval);
            break;

        case bcmStatRegexSessionMaxFlowsInUse:
            READ_FT_IN_USEr(unit, &statval);
            statval &= BCM_TR3_MAX_IN_USE_MASK;
            /*
            Shift the counter down to remove the "in-use" part of the register
            and leave just the "max in-use"
            */
            statval >>= BCM_TR3_IN_USE_MASK_SIZE;
            break;

        case bcmStatRegexSessionFlowsMissedTCP:
            READ_FT_FLOWS_MISSED_TCPr(unit, &statval);
            break;

        case bcmStatRegexSessionFlowsMissedUDP:
            READ_FT_FLOWS_MISSED_UDPr(unit, &statval);
            break;

        case bcmStatRegexSessionCmdWaitTimeouts:
            READ_FT_CMDWAIT_TIMEOUTr(unit, &statval);
            break;

        case bcmStatRegexSessionUnusedResults:
            READ_FT_UPDATE_ERRr(unit, &statval);
            break;

        case bcmStatRegexSessionSuppressedActions:
            READ_FT_ACTION_INHIBITEDr(unit, &statval);
            break;

        case bcmStatRegexSessionTcpSynData:
            READ_FT_SYN_DATAr(unit, &statval);
            break;

        case bcmStatRegexSessionL4Invalid:
            READ_FT_L4_INVALIDr(unit, &statval);
            break;

        case bcmStatRegexSessionL4PortsExcluded:
            READ_FT_L4_PORTS_EXCLUDEDr(unit, &statval);
            break;

        /*
        The following stats are for signature match
        */

        case bcmStatRegexSigMatchPacketsReceived:
            READ_AXP_SM_PACKETS_RECEIVED_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchPacketsSent:
            READ_AXP_SM_PACKETS_SENT_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchPacketsDropped:
            READ_AXP_SM_PACKETS_DROPPED_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchBytesMatched:
            READ_AXP_SM_BYTES_MATCHED_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchMatchedFlows:
            READ_AXP_SM_MATCHED_FLOWS_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchUnmatchedFlows:
            READ_AXP_SM_UNMATCHED_FLOWS_COUNTERr(unit, &statval);
            break;
            
        case bcmStatRegexSigMatchTotalMatch:
            READ_AXP_SM_TOTAL_MATCH_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchCrossSigFlags:
            READ_AXP_SM_TOTAL_CROSS_SIG_FLAGS_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchFragmentsReceived:
            READ_AXP_SM_FRAGMENTS_RECEIVED_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchInPacketError:
            READ_AXP_SM_IN_PACKET_ERROR_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchFlowTrackerError:
            READ_AXP_SM_FLOW_TRACKER_ERROR_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchPacketLengthError:
            READ_AXP_SM_PACKET_LENGTH_ERROR_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchL4ChecksumError:
            READ_AXP_SM_L4_CHECKSUM_ERROR_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchFlowDonePacketDrop:
            READ_AXP_SM_FLOW_DONE_PACKET_DROP_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchFlowTimestampError:
            READ_AXP_SM_FLOW_TIMESTAMP_ERROR_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchFlowPacketNumError:
            READ_AXP_SM_FLOW_PACKET_NUM_ERROR_COUNTERr(unit, &statval);
            break;

        case bcmStatRegexSigMatchECCError:
            READ_AXP_SM_ECC_ERROR_COUNTERr(unit, &statval);
            break;

        default:
            return BCM_E_PARAM;
            break;
    }

    *val = (uint64) statval;

    return BCM_E_NONE;
}

int
_bcm_tr3_regex_stat_set(int unit, bcm_regex_stat_t type, uint64 val)
{
    uint32 statval = (uint32) val;
    uint32 statval2;

    switch(type) {
        case bcmStatRegexSessionEntriesInUse:
            READ_FT_IN_USEr(unit, &statval2);
            statval2 &= BCM_TR3_MAX_IN_USE_MASK;
            statval &= BCM_TR3_IN_USE_MASK;
            statval |= statval2;
            WRITE_FT_IN_USEr(unit, statval);
            break;

        case bcmStatRegexSessionFlowsCreated:
            WRITE_FT_FLOWS_CREATEDr(unit, statval);
            break;

        case bcmStatRegexSessionMaxFlowsInUse:
            READ_FT_IN_USEr(unit, &statval2);
            /*
            Save the "in-use" part of the register before adding the
            new "max in-use" value
            */
            statval2 &= BCM_TR3_IN_USE_MASK;
            statval <<= BCM_TR3_IN_USE_MASK_SIZE;
            statval &= BCM_TR3_MAX_IN_USE_MASK;
            statval |= statval2;
            WRITE_FT_IN_USEr(unit, statval);
            break;

        case bcmStatRegexSessionFlowsMissedTCP:
            WRITE_FT_FLOWS_MISSED_TCPr(unit, statval);
            break;

        case bcmStatRegexSessionFlowsMissedUDP:
            WRITE_FT_FLOWS_MISSED_UDPr(unit, statval);
            break;

        case bcmStatRegexSessionCmdWaitTimeouts:
            WRITE_FT_CMDWAIT_TIMEOUTr(unit, statval);
            break;

        case bcmStatRegexSessionUnusedResults:
            WRITE_FT_UPDATE_ERRr(unit, statval);
            break;

        case bcmStatRegexSessionSuppressedActions:
            WRITE_FT_ACTION_INHIBITEDr(unit, statval);
            break;

        case bcmStatRegexSessionTcpSynData:
            WRITE_FT_SYN_DATAr(unit, statval);
            break;

        case bcmStatRegexSessionL4Invalid:
            WRITE_FT_L4_INVALIDr(unit, statval);
            break;

        case bcmStatRegexSessionL4PortsExcluded:
            WRITE_FT_L4_PORTS_EXCLUDEDr(unit, statval);
            break;

        /*
        The following stats are for signature match
        */

        case bcmStatRegexSigMatchPacketsReceived:
            WRITE_AXP_SM_PACKETS_RECEIVED_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchPacketsSent:
            WRITE_AXP_SM_PACKETS_SENT_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchPacketsDropped:
            WRITE_AXP_SM_PACKETS_DROPPED_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchBytesMatched:
            WRITE_AXP_SM_BYTES_MATCHED_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchMatchedFlows:
            WRITE_AXP_SM_MATCHED_FLOWS_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchUnmatchedFlows:
            WRITE_AXP_SM_UNMATCHED_FLOWS_COUNTERr(unit, statval);
            break;
            
        case bcmStatRegexSigMatchTotalMatch:
            WRITE_AXP_SM_TOTAL_MATCH_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchCrossSigFlags:
            WRITE_AXP_SM_TOTAL_CROSS_SIG_FLAGS_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchFragmentsReceived:
            WRITE_AXP_SM_FRAGMENTS_RECEIVED_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchInPacketError:
            WRITE_AXP_SM_IN_PACKET_ERROR_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchFlowTrackerError:
            WRITE_AXP_SM_FLOW_TRACKER_ERROR_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchPacketLengthError:
            WRITE_AXP_SM_PACKET_LENGTH_ERROR_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchL4ChecksumError:
            WRITE_AXP_SM_L4_CHECKSUM_ERROR_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchFlowDonePacketDrop:
            WRITE_AXP_SM_FLOW_DONE_PACKET_DROP_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchFlowTimestampError:
            WRITE_AXP_SM_FLOW_TIMESTAMP_ERROR_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchFlowPacketNumError:
            WRITE_AXP_SM_FLOW_PACKET_NUM_ERROR_COUNTERr(unit, statval);
            break;

        case bcmStatRegexSigMatchECCError:
            WRITE_AXP_SM_ECC_ERROR_COUNTERr(unit, statval);
            break;

        default:
            return BCM_E_PARAM;
            break;
    }
    
    return BCM_E_NONE;
    }

/*
 * Function:
 *     _regex_tr3_policer_hw_update
 * Purpose:
 *     Update (Install) the policer parameters
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     f_ent         - (IN) Field entry policer attached to.
 *     index_mtr     - (IN) Peak/Committed
 *     bucket_size   - (IN) Encoded bucket size.
 *     refresh_rate  - (IN) Tokens refresh rate.
 *     granularity   - (IN) Tokens granularity.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policer_hw_update(int unit,
                                        _field_policer_t *f_pl, uint8 index_mtr,
                                        uint32 bucket_size, uint32 refresh_rate,
                                        uint32 granularity, soc_mem_t meter_table)
{
    _field_stage_t  *stage_fc;               /* Field stage control.         */
    uint32          meter_entry[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.*/
    int             bucket_cnt_bitsize;      /* Bucket count field bit size. */
    int             bucket_max_bitsize;      /* Bucket size field bit size.  */
    int             meter_hw_idx;            /* Meter table index.           */
    uint32          bucketcount;             /* Bucket count initial value.  */
    int             meter_offset;            /* Peak/Comm meter selection.   */
    int             rv;                      /* Operation return status.     */

    if ((NULL == f_pl) || (INVALIDm == meter_table)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Calculate initial backet count.
     * bucket size << (bit count diff - 1(sign) -1 (overflow))  - 1
     */
    if (bucket_size) {
        bucket_cnt_bitsize = soc_mem_field_length(unit, meter_table,
                                                  BUCKETCOUNTf);
        bucket_max_bitsize = soc_mem_field_length(unit, meter_table,
                                                  BUCKETSIZEf);
        bucketcount = (bucket_size
                        << (bucket_cnt_bitsize - bucket_max_bitsize - 2))  - 1;
        bucketcount &= ((1 << bucket_cnt_bitsize) - 1);
    } else {
        bucketcount = 0;
    }

    meter_offset = (BCM_FIELD_METER_PEAK == index_mtr) ? 0 : 1;

    meter_hw_idx = (f_pl->pool_index * stage_fc->meter_pool[_FP_DEF_INST][0]->pool_size) + 
                                                      (2 * f_pl->hw_index) + meter_offset;

    /* Update meter config in hw. */
    rv = soc_mem_read(unit, meter_table, MEM_BLOCK_ANY,
                      meter_hw_idx, &meter_entry);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field32_set(unit, meter_table, &meter_entry, REFRESHCOUNTf,
                        refresh_rate);
    soc_mem_field32_set(unit, meter_table, &meter_entry, BUCKETSIZEf,
                        bucket_size);
    soc_mem_field32_set(unit, meter_table, &meter_entry, METER_GRANf,
                        granularity);
    soc_mem_field32_set(unit, meter_table, &meter_entry, BUCKETCOUNTf,
                        bucketcount);

    /* Refresh mode is only set to 1 for Single Rate. Other modes get 0 */
    if (f_pl->cfg.mode  == bcmPolicerModeSrTcm) {
        soc_mem_field32_set(unit, meter_table, &meter_entry, REFRESH_MODEf, 1);
    } else if (f_pl->cfg.mode  == bcmPolicerModeCoupledTrTcmDs) {
        soc_mem_field32_set(unit, meter_table, &meter_entry, REFRESH_MODEf, 2);
    } else {
        soc_mem_field32_set(unit, meter_table, &meter_entry, REFRESH_MODEf, 0);
    }

    if ((soc_feature(unit, soc_feature_field_packet_based_metering)) &&
        (soc_mem_field_valid(unit, meter_table, PKTS_BYTESf))) {
        if (f_pl->cfg.flags & BCM_POLICER_MODE_PACKETS) {
            soc_mem_field32_set(unit, meter_table, &meter_entry, PKTS_BYTESf, 1);
        } else {
            soc_mem_field32_set(unit, meter_table, &meter_entry, PKTS_BYTESf, 0);
        }
    }

    rv = soc_mem_write(unit, meter_table, MEM_BLOCK_ALL,
                       meter_hw_idx, meter_entry);

    return (rv);
    }

STATIC int _regex_tr3_policer_action_set(int              unit,
                                         _regex_policy_t *p_ent,
                                         uint32          *policy_buf)
{
    _field_entry_policer_t *f_ent_pl;
    unsigned               api_mode, api_flags, pool_idx, hw_idx;
    uint8                  hw_mode, modifier, sh_mode;
    _field_policer_t       *f_pl = NULL;

    /* Level 0 */

    f_ent_pl = &p_ent->policer[0];
    if (f_ent_pl->flags & _FP_POLICER_INSTALLED) {
        BCM_IF_ERROR_RETURN(_bcm_regex_policer_get(unit, f_ent_pl->pid, &f_pl));
        api_mode  = f_pl->cfg.mode;
        api_flags = f_pl->cfg.flags;
        pool_idx  = f_pl->pool_index;
        hw_idx    = f_pl->hw_index;
    } else {
        api_mode = bcmPolicerModePassThrough;
        api_flags = pool_idx = hw_idx = 0;
    }

    switch (api_mode) {
        case bcmPolicerModeGreen:
        case bcmPolicerModePassThrough:
            hw_mode = 0;
            modifier = (api_mode == bcmPolicerModePassThrough) ? 1 : 0;
            break;
        case bcmPolicerModeCommitted:
            hw_mode  = 1;
            modifier = (_FP_POLICER_EXCESS_HW_METER(f_pl)) ? 0 : 1;
            break;
        case bcmPolicerModeSrTcm:
        case bcmPolicerModeSrTcmModified:
            hw_mode  = (api_flags & BCM_POLICER_COLOR_BLIND) ? 6 : 7;
            modifier = (api_mode == bcmPolicerModeSrTcmModified) ? 1 : 0;
            break;
        case bcmPolicerModeTrTcm:
            hw_mode = (api_flags & BCM_POLICER_COLOR_BLIND) ? 2 : 3;
            modifier = 0;
            break;
        case bcmPolicerModeTrTcmDs:
        case bcmPolicerModeCoupledTrTcmDs:
            hw_mode = (api_flags & BCM_POLICER_COLOR_BLIND) ? 4 : 5;
            modifier = 0;
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        METER_PAIR_POOL_NUMBERf,
                        pool_idx);
    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        METER_PAIR_POOL_INDEXf,
                        hw_idx);
    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        METER_PAIR_MODEf,
                        hw_mode);
    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        METER_PAIR_MODE_MODIFIERf,
                        modifier);

    /* Level 1 */
#if defined(ESW_REGEX_HIERARCHICAL_METERS)
    f_ent_pl = &p_ent->policer[1];
    if (f_ent_pl->flags & _FP_POLICER_INSTALLED) {
        BCM_IF_ERROR_RETURN(_bcm_regex_policer_get(unit, f_ent_pl->pid, &f_pl));
        api_mode  = f_pl->cfg.mode;
        api_flags = f_pl->cfg.flags;
        pool_idx  = f_pl->pool_index;
        hw_idx    = f_pl->hw_index;
    } else
#endif
    {
        api_mode = bcmPolicerModePassThrough;
        api_flags = pool_idx = hw_idx = 0;
    }

    if (api_mode == bcmPolicerModePassThrough) {
        hw_mode = sh_mode = modifier = 0;
    } else {
        switch (api_mode) {
            case bcmPolicerModeCommitted:
                hw_mode = 0;
                modifier = (_FP_POLICER_EXCESS_HW_METER(f_pl)) ? 0 : 1;
                break;
            case bcmPolicerModeTrTcmDs:
            case bcmPolicerModeCoupledTrTcmDs:
                hw_mode = 1;
                modifier = (api_flags & BCM_POLICER_COLOR_BLIND) ? 0 : 1;
                break;
            default:
                return (BCM_E_INTERNAL);
    }

        switch (api_flags & BCM_POLICER_COLOR_MERGE_MASK) {
            case BCM_POLICER_COLOR_MERGE_AND:
                sh_mode = 2;
                break;
            case BCM_POLICER_COLOR_MERGE_OR:
                sh_mode = 1;
                break;
            case BCM_POLICER_COLOR_MERGE_DUAL:
                sh_mode = 3;
                break;
            default:
                return (BCM_E_INTERNAL);
        }
    }

    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        SHARED_METER_PAIR_POOL_NUMBERf,
                        pool_idx);
    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        SHARED_METER_PAIR_POOL_INDEXf,
                        hw_idx);
    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        SHARED_METER_PAIR_MODEf,
                        hw_mode);
    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        METER_SHARING_MODEf,
                        sh_mode);
    soc_mem_field32_set(unit,
                        FT_POLICYm,
                        policy_buf,
                        METER_SHARING_MODE_MODIFIERf,
                        modifier);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_redirect_profile_get
 * Purpose:
 *     Get the redirect profile for the unit
 * Parameters:
 *     unit             - BCM device number
 *     redirect_profile - (OUT) redirect profile
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     For triumph, External TCAM and IFP refer to same IFP_REDIRECTION_PROFILEm
 */
STATIC int _regex_tr3_policy_redirect_profile_get(int unit, soc_profile_mem_t **redirect_profile)
{
    _field_control_t *fc = NULL;
 
    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
 
    *redirect_profile = &fc->redirect_profile;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_redirect_profile_ref_count_get
 * Purpose:
 *     Get redirect profile entry use count.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     index     - (IN) Profile entry index.
 *     ref_count - (OUT) redirect profile use count.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_redirect_profile_ref_count_get(int unit, int index, int *ref_count)
{
    soc_profile_mem_t *redirect_profile;

    if (NULL == ref_count) {
        return (BCM_E_PARAM);
    }

    /* Get the redirect profile */
    BCM_IF_ERROR_RETURN(_regex_tr3_policy_redirect_profile_get(unit, &redirect_profile));

    BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit, redirect_profile,
                                                      index, ref_count));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_redirect_profile_delete
 * Purpose:
 *     Delete redirect profile entry.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     index     - (IN) Profile entry index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_redirect_profile_delete(int unit, int index)
{
    soc_profile_mem_t *redirect_profile;

    /* Get the redirect profile */
    BCM_IF_ERROR_RETURN(_regex_tr3_policy_redirect_profile_get(unit, &redirect_profile));

    BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit, redirect_profile, index));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_redirect_profile_alloc
 * Purpose:
 *     Allocate redirect profile index
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     fa       - (IN) Field action.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_redirect_profile_alloc(int unit, _regex_policy_t *p_ent,
                                                    _regex_policy_action_t *pa)
{
    ifp_redirection_profile_entry_t  entry_arr[2];
    uint32                          *entry_ptr[2];
    soc_profile_mem_t               *redirect_profile;
    int                              rv;
#ifdef INCLUDE_L3
    bcm_pbmp_t                       ipmc_l2_pbmp, ipmc_l3_pbmp;
    int                              ipmc_index;
    int                              entry_count;
#endif /* INCLUDE_L3 */
    bcm_mcast_addr_t                 mcaddr;
    bcm_pbmp_t                       pbmp;
    void                            *entries[1];
    soc_mem_t                        profile_mem = IFP_REDIRECTION_PROFILEm;

    entry_ptr[0] = (uint32 *)entry_arr;
    entry_ptr[1] =  entry_ptr[0] + soc_mem_entry_words(unit, profile_mem);
    entries[0] = (void *)&entry_arr;

    if ((NULL == p_ent) || (NULL == pa)) {
        return (BCM_E_PARAM);
    }

    /* Reset redirection profile entry. */
    sal_memcpy(entry_ptr[0], soc_mem_entry_null(unit, profile_mem),
               soc_mem_entry_words(unit, profile_mem) * sizeof(uint32));
    sal_memcpy(entry_ptr[1], soc_mem_entry_null(unit, profile_mem),
               soc_mem_entry_words(unit, profile_mem) * sizeof(uint32));

    /* Get the redirect profile */
    rv = _regex_tr3_policy_redirect_profile_get(unit, &redirect_profile);
    BCM_IF_ERROR_RETURN(rv);

    switch (pa->action) {
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionRedirectBcastPbmp:
        case bcmFieldActionEgressMask:
        case bcmFieldActionEgressPortsAdd:
            SOC_PBMP_CLEAR(pbmp);
            SOC_PBMP_WORD_SET(pbmp, 0, pa->param[0]);
            SOC_PBMP_WORD_SET(pbmp, 1, pa->param[1]);
            /* Trident family could support more than 64-ports on a device*/
            if (SOC_IS_TD_TT(unit)) {
                SOC_PBMP_WORD_SET(pbmp, 2, pa->param[2]);
            }
            /* Trident2 device can support more than 96-ports */
            if (SOC_IS_TD2_TT2(unit)) {
                SOC_PBMP_WORD_SET(pbmp, 3, pa->param[3]);
            }
            soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], BITMAPf, &pbmp);
            rv = soc_profile_mem_add(unit, redirect_profile, entries,
                                     1, (uint32 *)&pa->hw_index);
            BCM_IF_ERROR_RETURN(rv);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectIpmc:
            if (_BCM_MULTICAST_IS_SET(pa->param[0])) {
                if ((0 == _BCM_MULTICAST_IS_L3(pa->param[0])) &&
                    (0 == _BCM_MULTICAST_IS_WLAN(pa->param[0]))) {
                    return (BCM_E_PARAM);
                }
                ipmc_index = _BCM_MULTICAST_ID_GET(pa->param[0]);
            } else {
                ipmc_index = pa->param[0];
            }
            rv = _bcm_esw_multicast_ipmc_read(unit, ipmc_index,
                                              &ipmc_l2_pbmp, &ipmc_l3_pbmp);
            BCM_IF_ERROR_RETURN(rv);
            entry_count = 2;
            soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], BITMAPf,
                                   &ipmc_l3_pbmp);
            soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[1], BITMAPf,
                                   &ipmc_l2_pbmp);
            soc_mem_field32_set(unit, profile_mem, entry_ptr[0], MC_INDEXf, ipmc_index);
            /* MTU profile index overlayed on MC_INDEXf. */
            rv = soc_profile_mem_add(unit, redirect_profile,
                                     entries, entry_count, (uint32 *)&pa->hw_index);
            BCM_IF_ERROR_RETURN(rv);
            break;
#endif /* INCLUDE_L3 */
        case bcmFieldActionRedirectMcast:
            rv = _bcm_xgs3_mcast_index_port_get(unit, pa->param[0], &mcaddr);
            BCM_IF_ERROR_RETURN(rv);
            soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], BITMAPf,
                                   &mcaddr.pbmp);
            soc_mem_field32_set(unit, profile_mem, entry_ptr[0], MC_INDEXf, pa->param[0]);
            rv = soc_profile_mem_add(unit, redirect_profile, entries,
                                     1, (uint32 *)&pa->hw_index);
            BCM_IF_ERROR_RETURN(rv);
            break;
        default:
            return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_action_redirect
 * Purpose:
 *     Install redirect action in policy table.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     mem      - (IN) Policy table memory.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     fa       - (IN) Field action.
 *     buf      - (OUT) Field Policy table entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_action_redirect(int unit, soc_mem_t mem, _regex_policy_t *p_ent,
                                             _regex_policy_action_t *pa, uint32 *buf)
{
    uint32          redir_field = 0;
    int             shift_val;
    soc_field_t     redir_field_type = INVALIDf;

    if (NULL == p_ent || NULL == pa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (pa->action) {
        case bcmFieldActionOffloadRedirect:
            redir_field = ((pa->param[0] & 0x7f) << 6);
            redir_field |= (pa->param[1] & 0x3f);
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x1);
            PolicySet(unit, mem, buf, REDIRECTIONf, redir_field);

            PolicySet(unit, mem, buf, HI_PRI_ACTION_CONTROLf, 0x1);
            PolicySet(unit, mem, buf, HI_PRI_RESOLVEf, 0x1);
            PolicySet(unit, mem, buf, SUPPRESS_COLOR_SENSITIVE_ACTIONSf, 0x1);
            PolicySet(unit, mem, buf, DEFER_QOS_MARKINGSf, 0x1);
            PolicySet(unit, mem, buf, SUPPRESS_SW_ACTIONSf, 0x1);
            PolicySet(unit, mem, buf, SUPPRESS_VXLTf, 0x1);
            break;
        case bcmFieldActionRedirect: /* param0 = modid, param1 = port*/
            redir_field_type = REDIRECTIONf;
            redir_field_type = REDIRECT_DVPf;
            if (BCM_GPORT_IS_MPLS_PORT(pa->param[1])) {
                redir_field = BCM_GPORT_MPLS_PORT_ID_GET((int)pa->param[1]);
                PolicySet(unit, mem, buf, UNICAST_REDIRECT_CONTROLf, 0x6);
            } else if (BCM_GPORT_IS_MIM_PORT(pa->param[1])) {
                redir_field = BCM_GPORT_MIM_PORT_ID_GET((int)pa->param[1]);
                PolicySet(unit, mem, buf, UNICAST_REDIRECT_CONTROLf, 0x6);
            } else if (BCM_GPORT_IS_NIV_PORT(pa->param[1])) {
                redir_field = BCM_GPORT_NIV_PORT_ID_GET((int)pa->param[1]);
                PolicySet(unit, mem, buf, UNICAST_REDIRECT_CONTROLf, 0x6);
            } else if (BCM_GPORT_IS_TRILL_PORT(pa->param[1])) {
                redir_field = BCM_GPORT_TRILL_PORT_ID_GET((int)pa->param[1]);
                PolicySet(unit, mem, buf, UNICAST_REDIRECT_CONTROLf, 0x6);
            } else if (BCM_GPORT_IS_L2GRE_PORT(pa->param[1])) {
                redir_field = BCM_GPORT_L2GRE_PORT_ID_GET((int)pa->param[1]);
                PolicySet(unit, mem, buf, UNICAST_REDIRECT_CONTROLf, 0x6);
            } else {
                redir_field_type = REDIRECTIONf;
                redir_field = ((pa->param[0] & 0xff) << 7);
                redir_field |= (pa->param[1] & 0x7f);
            }
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x1);
            PolicySet(unit, mem, buf, redir_field_type, redir_field);
            break;
        case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
            shift_val = 9;
            redir_field |= (0x40 << shift_val);  /* Trunk indicator. */
            redir_field |= pa->param[0];

            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x1);
            PolicySet(unit, mem, buf, REDIRECTIONf, redir_field);
            break;
        case bcmFieldActionRedirectCancel:
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 2);
            break;
        case bcmFieldActionRedirectPbmp:
            PolicySet(unit, mem, buf, REDIRECTIONf, pa->hw_index);
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x3);
            p_ent->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionEgressMask:
            PolicySet(unit, mem, buf, REDIRECTIONf, pa->hw_index);
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x4);
            break;
        case bcmFieldActionEgressPortsAdd:
            PolicySet(unit, mem, buf, REDIRECTIONf, pa->hw_index);
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x5);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectIpmc:
            if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
                if (_BCM_MULTICAST_IS_SET(pa->param[0])) {
                    if ((0 == _BCM_MULTICAST_IS_L3(pa->param[0])) &&
                        (0 == _BCM_MULTICAST_IS_WLAN(pa->param[0]))) {
                        return (BCM_E_PARAM);
                    }
                    redir_field = _BCM_MULTICAST_ID_GET(pa->param[0]);
                } else {
                    redir_field = pa->param[0];
                }
            } else {
                redir_field = pa->hw_index;
            }
            /* Assign IPMC action to redirect profile index. */
            shift_val = 16;

            PolicySet(unit, mem, buf, REDIRECTIONf,
                      (redir_field | (3 << shift_val)));
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x3);
            break;
#endif /* INCLUDE_L3 */
        case bcmFieldActionRedirectMcast:
            if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
                if (_BCM_MULTICAST_IS_SET(pa->param[0])) {
                    if (0 == _BCM_MULTICAST_IS_L2(pa->param[0])) {
                        return (BCM_E_PARAM);
                    }
                    redir_field = _BCM_MULTICAST_ID_GET(pa->param[0]);
                } else {
                    redir_field = pa->param[0];
                }
            } else {
                redir_field = pa->hw_index;
            }

            /* Assign MCAST action to redirect profile index. */
            shift_val = 16;
            PolicySet(unit, mem, buf, REDIRECTIONf,
                      (redir_field | (2 << shift_val)));
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x3);
            break;
        case bcmFieldActionRedirectVlan:
            shift_val = 16;
            PolicySet(unit, mem, buf, REDIRECTIONf, (1 << shift_val));
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x3);
            break;
        case bcmFieldActionRedirectBcastPbmp:
            shift_val = 16;
            redir_field = pa->hw_index;
            PolicySet(unit, mem, buf, REDIRECTIONf,
                      (redir_field | (1 << 11) | (1 << shift_val)));
            PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x3);
            break;
        default:
            return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

STATIC int _regex_tr3_policy_index_min_get(int unit)
{
    return soc_mem_index_min(unit, FT_POLICYm);
}

STATIC int _regex_tr3_policy_index_max_get(int unit)
{
    return soc_mem_index_max(unit, FT_POLICYm);
}

STATIC int _regex_tr3_policy_action_support_check(int                 unit,
                                                  bcm_field_action_t  action)
{
    switch (action) {
/* Following cases from _field_tr3_stage_action_support_check */
        case bcmFieldActionNewClassId:
        case bcmFieldActionTrunkLoadBalanceCancel:
        case bcmFieldActionEgressClassSelect:
        case bcmFieldActionHiGigClassSelect:
        case bcmFieldActionOamDmTimeFormat:
        case bcmFieldActionOamLmDmSampleEnable:
        case bcmFieldActionOamTagStatusCheck:
        case bcmFieldActionOamTunnelControl:
            return (BCM_E_NONE);

        case bcmFieldActionEcmpLoadBalanceCancel:
            if (soc_feature(unit, soc_feature_ecmp_dlb)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionPortPrioIntCosQNew:
        case bcmFieldActionRpPortPrioIntCosQNew:
        case bcmFieldActionYpPortPrioIntCosQNew:
        case bcmFieldActionGpPortPrioIntCosQNew:
            if (soc_feature(unit, soc_feature_field_ingress_cosq_override)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionGpCopyToCpu:
            return (BCM_E_NONE);

        case bcmFieldActionRedirectEgrNextHop:
#if defined(INCLUDE_L3)
            if (soc_feature(unit, soc_feature_field_action_redirect_nexthop) ||
                soc_feature(unit, soc_feature_field_action_redirect_ecmp)) {
                return (BCM_E_NONE);
            }
#endif
            break;

/* Following cases from _bcm_field_trx_stage_action_support_check */
        case bcmFieldActionAddClassTag:
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionDrop:
        case bcmFieldActionDropCancel:
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionColorIndependent:
        case bcmFieldActionL3ChangeVlan:
        case bcmFieldActionL3ChangeVlanCancel:
        case bcmFieldActionL3ChangeMacDa:
        case bcmFieldActionL3ChangeMacDaCancel:
        case bcmFieldActionL3Switch:
        case bcmFieldActionL3SwitchCancel:
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionPrioPktCancel:
        case bcmFieldActionEcnNew:
        case bcmFieldActionDscpNew:
        case bcmFieldActionDscpCancel:
        case bcmFieldActionCosQNew:
        case bcmFieldActionCosQCpuNew:
        case bcmFieldActionVlanCosQNew:
        case bcmFieldActionMirrorOverride:
        case bcmFieldActionSwitchToCpuCancel:
        case bcmFieldActionSwitchToCpuReinstate:
        case bcmFieldActionRedirect:
        case bcmFieldActionRedirectTrunk:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionRedirectIpmc:
        case bcmFieldActionRedirectMcast:
            return (BCM_E_NONE);
            break;

        case bcmFieldActionRedirectBcastPbmp:
            return (BCM_E_NONE);
            break;

        case bcmFieldActionRedirectVlan:
            if (SOC_IS_TR_VL(unit)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionEgressMask:
        case bcmFieldActionEgressPortsAdd:
        case bcmFieldActionDoNotChangeTtl:
            return (BCM_E_NONE);
            break;

        case bcmFieldActionDoNotCheckUrpf:
            if (SOC_MEM_FIELD_VALID(unit, FT_POLICYm, DO_NOT_URPFf)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionPrioPktAndIntCopy:
        case bcmFieldActionPrioPktAndIntNew:
        case bcmFieldActionPrioPktAndIntTos:
        case bcmFieldActionPrioPktAndIntCancel:
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionMirrorIngress:
        case bcmFieldActionMirrorEgress:
        case bcmFieldActionRpDrop:
        case bcmFieldActionRpDropCancel:
        case bcmFieldActionRpDropPrecedence:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionRpPrioPktCopy:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionRpPrioPktTos:
        case bcmFieldActionRpPrioPktCancel:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionRpDscpCancel:
        case bcmFieldActionRpEcnNew:
        case bcmFieldActionRpCosQNew:
        case bcmFieldActionRpVlanCosQNew:
        case bcmFieldActionRpPrioPktAndIntCopy:
        case bcmFieldActionRpPrioPktAndIntNew:
        case bcmFieldActionRpPrioPktAndIntTos:
        case bcmFieldActionRpPrioPktAndIntCancel:
        case bcmFieldActionRpSwitchToCpuCancel:
        case bcmFieldActionRpSwitchToCpuReinstate:
        case bcmFieldActionRpPrioIntCopy:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionRpPrioIntTos:
        case bcmFieldActionRpPrioIntCancel:

        case bcmFieldActionYpDrop:
        case bcmFieldActionYpDropCancel:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionYpPrioPktCopy:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionYpPrioPktTos:
        case bcmFieldActionYpPrioPktCancel:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionYpDscpCancel:
        case bcmFieldActionYpEcnNew:
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionYpVlanCosQNew:
        case bcmFieldActionYpPrioPktAndIntCopy:
        case bcmFieldActionYpPrioPktAndIntNew:
        case bcmFieldActionYpPrioPktAndIntTos:
        case bcmFieldActionYpPrioPktAndIntCancel:
        case bcmFieldActionYpSwitchToCpuCancel:
        case bcmFieldActionYpSwitchToCpuReinstate:
        case bcmFieldActionYpPrioIntCopy:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionYpPrioIntTos:
        case bcmFieldActionYpPrioIntCancel:

        case bcmFieldActionGpDrop:
        case bcmFieldActionGpDropCancel:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionGpPrioPktCopy:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionGpPrioPktTos:
        case bcmFieldActionGpPrioPktCancel:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionGpTosPrecedenceNew:
        case bcmFieldActionGpTosPrecedenceCopy:
        case bcmFieldActionGpDscpCancel:
        case bcmFieldActionGpEcnNew:
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionGpVlanCosQNew:
        case bcmFieldActionGpPrioPktAndIntCopy:
        case bcmFieldActionGpPrioPktAndIntNew:
        case bcmFieldActionGpPrioPktAndIntTos:
        case bcmFieldActionGpPrioPktAndIntCancel:
        case bcmFieldActionGpSwitchToCpuCancel:
        case bcmFieldActionGpSwitchToCpuReinstate:
        case bcmFieldActionGpPrioIntCopy:
        case bcmFieldActionGpPrioIntNew:
        case bcmFieldActionGpPrioIntTos:
        case bcmFieldActionGpPrioIntCancel:
            return (BCM_E_NONE);
            break;

        case bcmFieldActionMultipathHash:
            if (SOC_MEM_FIELD_VALID(unit, FT_POLICYm, ECMP_HASH_SELf)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionIpFix:
        case bcmFieldActionIpFixCancel:
            if (soc_feature(unit, soc_feature_ipfix)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionOamUpMep:
        case bcmFieldActionOamTx:
        case bcmFieldActionOamLmepMdl:
        case bcmFieldActionOamServicePriMappingPtr:
        case bcmFieldActionOamLmBasePtr:
        case bcmFieldActionOamDmEnable:
        case bcmFieldActionOamLmEnable:
        case bcmFieldActionOamLmepEnable:
            if (soc_feature(unit, soc_feature_field_oam_actions)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionTimeStampToCpu:
        case bcmFieldActionRpTimeStampToCpu:
        case bcmFieldActionYpTimeStampToCpu:
        case bcmFieldActionGpTimeStampToCpu:
        case bcmFieldActionTimeStampToCpuCancel:
        case bcmFieldActionRpTimeStampToCpuCancel:
        case bcmFieldActionYpTimeStampToCpuCancel:
        case bcmFieldActionGpTimeStampToCpuCancel:
            if (soc_feature(unit, soc_feature_field_action_timestamp)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionOffloadRedirect:
        case bcmFieldActionOffloadClassSet:
        case bcmFieldActionOffloadDropIndication:
            if (SOC_MEM_FIELD_VALID(unit, FT_POLICYm, HI_PRI_ACTION_CONTROLf)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionFabricQueue:
            if (soc_feature(unit, soc_feature_field_action_fabric_queue)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionSrcMacNew:
#if defined(INCLUDE_L3)
            if (soc_feature(unit, soc_feature_field_action_l2_change) &&
                soc_feature(unit, soc_feature_l3)) {
                return (BCM_E_NONE);
            }
#endif /* !INCLUDE_L3 */
            break;

        case bcmFieldActionDstMacNew:
        case bcmFieldActionOuterVlanNew:
#if defined(INCLUDE_L3)
            if (soc_feature(unit, soc_feature_field_action_l2_change)) {
                return (BCM_E_NONE);
            }
#endif /* !INCLUDE_L3 */
            break;

        case bcmFieldActionVnTagNew:
        case bcmFieldActionVnTagDelete:
            if (soc_feature(unit, soc_feature_niv)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionCnmCancel:
            if (SOC_MEM_FIELD_VALID(unit, FT_POLICYm, DO_NOT_GENERATE_CNMf)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionDynamicHgTrunkCancel:
            if (SOC_MEM_FIELD_VALID(unit, FT_POLICYm, DISABLE_DYNAMIC_LOAD_BALANCINGf)) {
                return (BCM_E_NONE);
            }
            break;
        case bcmFieldActionUcastCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionYpUcastCosQNew:
        case bcmFieldActionRpUcastCosQNew:
            if (soc_feature(unit, soc_feature_ets)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionMcastCosQNew:
        case bcmFieldActionGpMcastCosQNew:
        case bcmFieldActionYpMcastCosQNew:
        case bcmFieldActionRpMcastCosQNew:
            if (soc_feature(unit, soc_feature_ets)) {
                return (BCM_E_NONE);
            }
            break;

        case bcmFieldActionUseGlobalMeterColor:
            if (soc_feature(unit, soc_feature_global_meter)) {
                return (BCM_E_NONE);
            }
            break;

        default:
            ;
    }

        return BCM_E_PARAM;
    }

/*
 * Function:
 *     _regex_tr3_policy_action_conflict_check
 *
 * Purpose:
 *     Check if two actions conflict (occupy the same field in FT policy table)
 *
 * Parameters:
 *     unit    - (IN) BCM device number
 *     action  - (IN) Action to check(bcmFieldActionXXX)
 *     action1 - (IN) Action to check(bcmFieldActionXXX)
 *
 * Returns:
 *     BCM_E_CONFIG - if actions do conflict
 *     BCM_E_NONE   - if there is no conflict
 */
STATIC int _regex_tr3_policy_action_conflict_check(int                unit,
                                                   bcm_field_action_t action1,
                                                   bcm_field_action_t action)
{
    /* Two identical actions are forbidden. */
    if (action1 == action) {
        if ((action != bcmFieldActionMirrorIngress) &&
            (action != bcmFieldActionMirrorEgress)) {
            return (BCM_E_CONFIG);
        }
    }

    switch (action1) {
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionSwitchToCpuCancel:
        case bcmFieldActionSwitchToCpuReinstate:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuReinstate);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuReinstate);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpSwitchToCpuReinstate);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpSwitchToCpuReinstate);
            break;
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionRpSwitchToCpuCancel:
        case bcmFieldActionRpSwitchToCpuReinstate:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuReinstate);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuReinstate);
            break;
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionYpSwitchToCpuCancel:
        case bcmFieldActionYpSwitchToCpuReinstate:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuReinstate);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpSwitchToCpuReinstate);
            break;
        case bcmFieldActionGpCopyToCpu:
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionGpSwitchToCpuCancel:
        case bcmFieldActionGpSwitchToCpuReinstate:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuReinstate);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpSwitchToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpSwitchToCpuReinstate);
            break;
        case bcmFieldActionDrop:
        case bcmFieldActionDropCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
            break;
        case bcmFieldActionRpDrop:
        case bcmFieldActionRpDropCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
            break;
        case bcmFieldActionYpDrop:
        case bcmFieldActionYpDropCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
            break;
        case bcmFieldActionGpDrop:
        case bcmFieldActionGpDropCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
            break;
        case bcmFieldActionL3ChangeVlan:
        case bcmFieldActionL3ChangeVlanCancel:
        case bcmFieldActionL3ChangeMacDa:
        case bcmFieldActionL3ChangeMacDaCancel:
        case bcmFieldActionL3Switch:
        case bcmFieldActionL3SwitchCancel:
        case bcmFieldActionAddClassTag:
        case bcmFieldActionFabricQueue:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSrcMacNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDstMacNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionOuterVlanNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionFabricQueue);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVnTagNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVnTagDelete);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectEgrNextHop);
            break;
        case bcmFieldActionOuterVlanNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionOuterVlanNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionFabricQueue);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectEgrNextHop);
            break;
        case bcmFieldActionVnTagDelete:
        case bcmFieldActionVnTagNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVnTagDelete);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVnTagNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionFabricQueue);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectEgrNextHop);
            break;
        case bcmFieldActionSrcMacNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSrcMacNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionFabricQueue);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectEgrNextHop);
            break;
        case bcmFieldActionDstMacNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDstMacNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionFabricQueue);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectEgrNextHop);
            break;
        case bcmFieldActionRedirect:
        case bcmFieldActionRedirectTrunk:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionRedirectIpmc:
        case bcmFieldActionRedirectMcast:
        case bcmFieldActionRedirectVlan:
        case bcmFieldActionRedirectBcastPbmp:
        case bcmFieldActionOffloadRedirect:
        case bcmFieldActionEgressMask:
        case bcmFieldActionEgressPortsAdd:
        case bcmFieldActionRedirectEgrNextHop:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionOffloadRedirect);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionEgressMask);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionEgressPortsAdd);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirect);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectTrunk);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectPbmp);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectIpmc);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectMcast);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectVlan);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectBcastPbmp);
            break;
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionPrioPktCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
            break;
        case bcmFieldActionPrioPktAndIntCopy:
        case bcmFieldActionPrioPktAndIntNew:
        case bcmFieldActionPrioPktAndIntTos:
        case bcmFieldActionPrioPktAndIntCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionPrioIntNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
            /*
             * coverity[fallthrough]
             * This is intentional as the rest of the statements should be
             * executed for this case also
             */
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionVlanCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionUcastCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionMcastCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionRpPrioPktCopy:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionRpPrioPktTos:
        case bcmFieldActionRpPrioPktCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
            break;
        case bcmFieldActionRpPrioPktAndIntCopy:
        case bcmFieldActionRpPrioPktAndIntNew:
        case bcmFieldActionRpPrioPktAndIntTos:
        case bcmFieldActionRpPrioPktAndIntCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            break;
        case bcmFieldActionRpPrioIntCopy:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionRpPrioIntTos:
        case bcmFieldActionRpPrioIntCancel:
        case bcmFieldActionRpCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            break;
        case bcmFieldActionRpUcastCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            break;
        case bcmFieldActionRpMcastCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPortPrioIntCosQNew);
            break;
        case bcmFieldActionYpPrioPktCopy:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionYpPrioPktTos:
        case bcmFieldActionYpPrioPktCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
            break;
        case bcmFieldActionYpPrioPktAndIntCopy:
        case bcmFieldActionYpPrioPktAndIntNew:
        case bcmFieldActionYpPrioPktAndIntTos:
        case bcmFieldActionYpPrioPktAndIntCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            break;
        case bcmFieldActionYpPrioIntCopy:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionYpPrioIntTos:
        case bcmFieldActionYpPrioIntCancel:
        case bcmFieldActionYpCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            break;
        case bcmFieldActionYpUcastCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            break;
        case bcmFieldActionYpMcastCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            break;
        case bcmFieldActionGpPrioPktCopy:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionGpPrioPktTos:
        case bcmFieldActionGpPrioPktCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
            break;
        case bcmFieldActionGpPrioPktAndIntCopy:
        case bcmFieldActionGpPrioPktAndIntNew:
        case bcmFieldActionGpPrioPktAndIntTos:
        case bcmFieldActionGpPrioPktAndIntCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionGpPrioIntCopy:
        case bcmFieldActionGpPrioIntNew:
        case bcmFieldActionGpPrioIntTos:
        case bcmFieldActionGpPrioIntCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionGpCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionGpUcastCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionGpMcastCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPortPrioIntCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPortPrioIntCosQNew);
            break;
        case bcmFieldActionDropPrecedence:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDropPrecedence);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDropPrecedence);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDropPrecedence);
            break;
        case bcmFieldActionRpDropPrecedence:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDropPrecedence);
            break;
        case bcmFieldActionYpDropPrecedence:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDropPrecedence);
            break;
        case bcmFieldActionGpDropPrecedence:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDropPrecedence);
            break;
        case bcmFieldActionTimeStampToCpu:
        case bcmFieldActionTimeStampToCpuCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpTimeStampToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpTimeStampToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpTimeStampToCpuCancel);
            break;
        case bcmFieldActionRpTimeStampToCpu:
        case bcmFieldActionRpTimeStampToCpuCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpTimeStampToCpuCancel);
            break;
        case bcmFieldActionYpTimeStampToCpu:
        case bcmFieldActionYpTimeStampToCpuCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpTimeStampToCpuCancel);
            break;
        case bcmFieldActionGpTimeStampToCpu:
        case bcmFieldActionGpTimeStampToCpuCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpuCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpTimeStampToCpu);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpTimeStampToCpuCancel);
            break;
        case bcmFieldActionIpFix:
        case bcmFieldActionIpFixCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionIpFix);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionIpFixCancel);
            break;
        case bcmFieldActionDscpNew:
        case bcmFieldActionDscpCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDscpCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDscpCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpTosPrecedenceNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpTosPrecedenceCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDscpCancel);
            break;
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionRpDscpCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpDscpCancel);
            break;
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionYpDscpCancel:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpDscpCancel);
            break;
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionGpDscpCancel:
        case bcmFieldActionGpTosPrecedenceNew:
        case bcmFieldActionGpTosPrecedenceCopy:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpTosPrecedenceNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpTosPrecedenceCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpDscpCancel);
            break;
        case bcmFieldActionEcnNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpEcnNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpEcnNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpEcnNew);
            break;
        case bcmFieldActionRpEcnNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpEcnNew);
            break;
        case bcmFieldActionYpEcnNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpEcnNew);
            break;
        case bcmFieldActionGpEcnNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpEcnNew);
            break;
        case bcmFieldActionMirrorIngress:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
            break;
        case bcmFieldActionMirrorEgress:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
            break;
        case bcmFieldActionMirrorOverride:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMirrorIngress);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMirrorEgress);
            break;
        case bcmFieldActionPortPrioIntCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            break;
        case bcmFieldActionRpPortPrioIntCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRpMcastCosQNew);
            break;
        case bcmFieldActionYpPortPrioIntCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionYpMcastCosQNew);
            break;
        case bcmFieldActionGpPortPrioIntCosQNew:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpUcastCosQNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionGpMcastCosQNew);
            break;
        case bcmFieldActionColorIndependent:
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionColorIndependent);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionSrcMacNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionDstMacNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionOuterVlanNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectVlan);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectMcast);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectIpmc);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionEgressPortsAdd);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionEgressMask);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectCancel);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirect);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectTrunk);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectEgrNextHop);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVnTagNew);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionVnTagDelete);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectPbmp);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionOffloadRedirect);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionRedirectBcastPbmp);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionFabricQueue);
            _REGEX_POLICY_ACTIONS_CONFLICT(bcmFieldActionMirrorEgress);
            break;
        case bcmFieldActionDoNotChangeTtl:
        case bcmFieldActionDoNotCheckUrpf:
        case bcmFieldActionMultipathHash:
        case bcmFieldActionOamUpMep:
        case bcmFieldActionOamTx:
        case bcmFieldActionOamLmepMdl:
        case bcmFieldActionOamServicePriMappingPtr:
        case bcmFieldActionOamLmBasePtr:
        case bcmFieldActionOamDmEnable:
        case bcmFieldActionOamLmEnable:
        case bcmFieldActionOamLmepEnable:
        case bcmFieldActionOffloadClassSet:
        case bcmFieldActionOffloadDropIndication:
        case bcmFieldActionCnmCancel:
        case bcmFieldActionDynamicHgTrunkCancel:
            break;
        default:
            break;
    }
    return (BCM_E_NONE);
}

STATIC int _regex_tr3_policy_action_params_check(int                     unit,
                                                 _regex_policy_t        *p_ent,
                                                 _regex_policy_action_t *pa)
{
    uint32     arg  = pa->param[0];     /* Action param value      */
    uint32     arg2 = pa->param[1];     /* Action param value      */
    soc_mem_t  mem  = FT_POLICYm;       /* Policy table memory id. */
    bcm_port_t local_port;
    int        index;
    int        rv;
    int        shift_val;
    uint32     redir_field = 0;
#if defined(INCLUDE_L3)
    uint32     flags;            /* L3 forwarding flags.    */
    int        nh_ecmp_id;          /* Next hop/Ecmp group id. */
#endif /* INCLUDE_L3 */

    switch (pa->action) {
        case bcmFieldActionNewClassId:
            RegexPolicyCheck(unit, mem, I2E_CLASSIDf, arg);
            return (BCM_E_NONE);

        case bcmFieldActionEgressClassSelect:
            return (arg <= BCM_FIELD_EGRESS_CLASS_SELECT_NEW ? BCM_E_NONE : BCM_E_PARAM);

        case bcmFieldActionHiGigClassSelect:
            return (arg <= BCM_FIELD_HIGIG_CLASS_SELECT_PORT ? BCM_E_NONE : BCM_E_PARAM);

        case bcmFieldActionOamDmTimeFormat:
            return (arg <= BCM_FIELD_OAM_DM_TIME_FORMAT_NTP ? BCM_E_NONE : BCM_E_PARAM);

        case bcmFieldActionOamTagStatusCheck:
            return (arg < (BCM_FIELD_OAM_TAG_STATUS_INNER_L2_INNER_MASK << 1) ? BCM_E_NONE : BCM_E_PARAM);

        case bcmFieldActionOamTunnelControl:
            return (arg <= BCM_FIELD_OAM_TUNNEL_CONTROL_NOT_TUNNELDED ? BCM_E_NONE : BCM_E_PARAM);

        case bcmFieldActionEcnNew:
        case bcmFieldActionRpEcnNew:
        case bcmFieldActionYpEcnNew:
        case bcmFieldActionGpEcnNew:
            return (arg <= 3 ? BCM_E_NONE : BCM_E_PARAM);

        case bcmFieldActionCosQNew:
        case bcmFieldActionUcastCosQNew:
            BCM_IF_ERROR_RETURN(_bcm_tr3_cosq_index_resolve(unit, pa->param[0], 0,
                                                            _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                            &local_port, &index, NULL));

            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, index);
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, index);
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, index);
            return (BCM_E_NONE);

        case bcmFieldActionGpCosQNew:
        case bcmFieldActionGpUcastCosQNew:
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, arg);
            return (BCM_E_NONE);

        case bcmFieldActionYpCosQNew:
        case bcmFieldActionYpUcastCosQNew:
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, arg);
            return (BCM_E_NONE);

        case bcmFieldActionRpCosQNew:
        case bcmFieldActionRpUcastCosQNew:
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, arg);
            return (BCM_E_NONE);

        case bcmFieldActionMcastCosQNew:
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, (arg << 4));
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, (arg << 4));
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, (arg << 4));
            return (BCM_E_NONE);

        case bcmFieldActionGpMcastCosQNew:
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, (arg << 4));
            return (BCM_E_NONE);

        case bcmFieldActionYpMcastCosQNew:
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, (arg << 4));
            return (BCM_E_NONE);

        case bcmFieldActionRpMcastCosQNew:
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, (arg << 4));
            return (BCM_E_NONE);

        case bcmFieldActionPortPrioIntCosQNew:
        case bcmFieldActionRpPortPrioIntCosQNew:
            if (arg2 & (0x1 << 9)) {
                return (BCM_E_PARAM);
            }
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, arg2);
            return (BCM_E_NONE);
        case bcmFieldActionYpPortPrioIntCosQNew:
            if (arg2 & (0x1 << 9)) {
                return (BCM_E_PARAM);
            }
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, arg2);
            return (BCM_E_NONE);
        case bcmFieldActionGpPortPrioIntCosQNew:
            if (arg2 & (0x1 << 9)) {
                return (BCM_E_PARAM);
            }
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, arg2);
            return (BCM_E_NONE);
        case bcmFieldActionMultipathHash:
            RegexPolicyCheck(unit, mem, ECMP_HASH_SELf, arg);
            break;
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionGpCopyToCpu:
        case bcmFieldActionTimeStampToCpu:
        case bcmFieldActionRpTimeStampToCpu:
        case bcmFieldActionYpTimeStampToCpu:
        case bcmFieldActionGpTimeStampToCpu:
            if (arg != 0) {
                RegexPolicyCheck(unit, mem, MATCHED_RULEf, arg2);
            }
            break;
        case bcmFieldActionDropPrecedence:
            RegexPolicyCheck(unit, mem, R_DROP_PRECEDENCEf, arg);
            RegexPolicyCheck(unit, mem, Y_DROP_PRECEDENCEf, arg);
            RegexPolicyCheck(unit, mem, G_DROP_PRECEDENCEf, arg);
            break;
        case bcmFieldActionPrioPktNew:
            RegexPolicyCheck(unit, mem, R_NEW_PKT_PRIf, arg);
            RegexPolicyCheck(unit, mem, Y_NEW_PKT_PRIf, arg);
            RegexPolicyCheck(unit, mem, G_NEW_PKT_PRIf, arg);
            break;
        case bcmFieldActionDscpNew:
            RegexPolicyCheck(unit, mem, R_NEW_DSCPf, arg);
            RegexPolicyCheck(unit, mem, Y_NEW_DSCPf, arg);
            RegexPolicyCheck(unit, mem, G_NEW_DSCP_TOSf, arg);
            break;
    
        case bcmFieldActionVlanCosQNew:
            /* Add 8 to the value since VLAN shaping queues are 8..23 */
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, arg + 8);
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, arg + 8);
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, arg + 8);
            break;

        case bcmFieldActionPrioIntNew:
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, (0xf & arg));
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, (0xf & arg));
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, (0xf & arg));
            break;

        case bcmFieldActionPrioPktAndIntNew:
            RegexPolicyCheck(unit, mem, R_NEW_PKT_PRIf, arg);
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, (0xf & arg));
            RegexPolicyCheck(unit, mem, Y_NEW_PKT_PRIf, arg);
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, (0xf & arg));
            RegexPolicyCheck(unit, mem, G_NEW_PKT_PRIf, arg);
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, (0xf & arg));
            break;

        case bcmFieldActionCosQCpuNew:
            RegexPolicyCheck(unit, mem, CPU_COSf, arg);
            break;

        case bcmFieldActionOuterVlanNew:
            RegexPolicyCheck(unit, EGR_L3_NEXT_HOPm, INTF_NUMf,
                             arg);
            break;

        case bcmFieldActionMirrorIngress:
        case bcmFieldActionMirrorEgress:
            rv = _bcm_regex_policy_action_dest_check(unit, pa);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionOffloadRedirect:
        case bcmFieldActionRedirect:
            rv = _bcm_regex_policy_action_dest_check(unit, pa);
            BCM_IF_ERROR_RETURN(rv);
            if (BCM_GPORT_IS_MPLS_PORT(arg2)) {
                redir_field = BCM_GPORT_MPLS_PORT_ID_GET((int)arg2);
                redir_field |= (6 << 18);
            } else if(BCM_GPORT_IS_MIM_PORT(arg2)) {
                redir_field = BCM_GPORT_MIM_PORT_ID_GET((int)arg2);
                redir_field |= (6 << 18);
            } else if (BCM_GPORT_IS_NIV_PORT(arg2)) {
                redir_field = BCM_GPORT_NIV_PORT_ID_GET((int)arg2);
                redir_field |= (6 << 18);
            } else if (BCM_GPORT_IS_TRILL_PORT(arg2)) {
                redir_field = BCM_GPORT_TRILL_PORT_ID_GET((int)arg2);
                redir_field |= (6 << 18);
            } else if (BCM_GPORT_IS_L2GRE_PORT(arg2)) {
                redir_field = BCM_GPORT_L2GRE_PORT_ID_GET((int)arg2);
                redir_field |= (6 << 18);
            } else {
                redir_field = ((arg & 0xff) << 7);
                redir_field |= (arg2 & 0x7f);
            }
            RegexPolicyCheck(unit, mem, REDIRECTIONf, redir_field);
            break;
        case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
            shift_val = SOC_IS_TR_VL(unit) ? 7 : 8;
            redir_field |= (0x40 << shift_val);  /* Trunk indicator. */
            redir_field |= arg;

            RegexPolicyCheck(unit, mem, REDIRECTIONf, redir_field);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectIpmc:
            if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
                if (_BCM_MULTICAST_IS_SET(arg)) {
                    if ((0 == _BCM_MULTICAST_IS_L3(arg)) &&
                        (0 == _BCM_MULTICAST_IS_WLAN(arg))) {
                        return (BCM_E_PARAM);
                    }
                    redir_field = _BCM_MULTICAST_ID_GET(arg);
                } else {
                    redir_field = arg;
                }
            } else {
                redir_field = 0;
            }
            /* Assign IPMC action to redirect profile index. */
            shift_val = SOC_IS_TR_VL(unit) ? 12 : 13;
            RegexPolicyCheck(unit, mem, REDIRECTIONf,
                             (redir_field | (3 << shift_val)));
            break;
#endif /* INCLUDE_L3 */
        case bcmFieldActionRedirectMcast:
            if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
                if (_BCM_MULTICAST_IS_SET(arg)) {
                    if (0 == _BCM_MULTICAST_IS_L2(arg)) {
                        return (BCM_E_PARAM);
                    }
                    redir_field = _BCM_MULTICAST_ID_GET(arg);
                } else {
                    redir_field = arg;
                }
            } else {
                redir_field = 0;
}

            /* Assign MCAST action to redirect profile index. */
            shift_val = SOC_IS_TR_VL(unit) ? 12 : 13;
            RegexPolicyCheck(unit, mem, REDIRECTIONf,
                             (redir_field | (2 << shift_val)));
            break;
        case bcmFieldActionOffloadClassSet:
            RegexPolicyCheck(unit, mem, PPD3_CLASS_TAGf, arg);
            break;
        case bcmFieldActionRpDropPrecedence:
            RegexPolicyCheck(unit, mem, R_DROP_PRECEDENCEf, arg);
            break;
        case bcmFieldActionRpPrioPktNew:
            RegexPolicyCheck(unit, mem, R_NEW_PKT_PRIf, arg);
            break;
        case bcmFieldActionRpDscpNew:
            RegexPolicyCheck(unit, mem, R_NEW_DSCPf, arg);
            break;
        case bcmFieldActionRpPrioPktAndIntNew:
            RegexPolicyCheck(unit, mem, R_NEW_PKT_PRIf, arg);
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, (0xf & arg));
            break;
        case bcmFieldActionRpPrioIntNew:
            RegexPolicyCheck(unit, mem, R_COS_INT_PRIf, (0xf & arg));
            break;
        case bcmFieldActionYpDropPrecedence:
            RegexPolicyCheck(unit, mem, Y_DROP_PRECEDENCEf, arg);
            break;
        case bcmFieldActionYpPrioPktNew:
            RegexPolicyCheck(unit, mem, Y_NEW_PKT_PRIf, arg);
            break;
        case bcmFieldActionYpDscpNew:
            RegexPolicyCheck(unit, mem, Y_NEW_DSCPf, arg);
            break;
        case bcmFieldActionYpVlanCosQNew:
            /* Add 8 to the value since VLAN shaping queues are 8..23 */
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, arg + 8);
            break;
        case bcmFieldActionYpPrioPktAndIntNew:
            RegexPolicyCheck(unit, mem, Y_NEW_PKT_PRIf, arg);
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, (0xf & arg));
            break;
        case bcmFieldActionYpPrioIntNew:
            RegexPolicyCheck(unit, mem, Y_COS_INT_PRIf, (0xf & arg));
            break;
        case bcmFieldActionGpDropPrecedence:
            RegexPolicyCheck(unit, mem, G_DROP_PRECEDENCEf, arg);
            break;
        case bcmFieldActionGpPrioPktNew:
            RegexPolicyCheck(unit, mem, G_NEW_PKT_PRIf, arg);
            break;
        case bcmFieldActionGpDscpNew:
            RegexPolicyCheck(unit, mem, G_NEW_DSCP_TOSf, arg);
            break;
        case bcmFieldActionGpTosPrecedenceNew:
            RegexPolicyCheck(unit, mem, G_NEW_DSCP_TOSf, arg);
            break;
        case bcmFieldActionGpVlanCosQNew:
            /* Add 8 to the value since VLAN shaping queues are 8..23 */
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, arg + 8);
            break;
        case bcmFieldActionGpPrioPktAndIntNew:
            RegexPolicyCheck(unit, mem, G_NEW_PKT_PRIf, arg);
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, (0xf & arg));
            break;
        case bcmFieldActionGpPrioIntNew:
            RegexPolicyCheck(unit, mem, G_COS_INT_PRIf, (0xf & arg));
            break;
        case bcmFieldActionAddClassTag:
            /*
             * The NEXT_HOP_INDEXf is 14-bits wide in FT_POLICYm view.
             * But only 13-bits are valid for this action
             */
            if (SOC_IS_TR_VL(unit)) {
                if (!(arg <= ((1 << 13) - 1))) {
                    return BCM_E_PARAM;
                }
            } else
{
                RegexPolicyCheck(unit, mem, NEXT_HOP_INDEXf, arg);
            }
            break;
        case bcmFieldActionFabricQueue:
            if ((arg & BCM_FABRIC_QUEUE_QOS_BASE)  &&
                (arg & BCM_FABRIC_QUEUE_DEST_OFFSET)) {
                return (BCM_E_PARAM);
            }

            if (arg &
                ~(BCM_FABRIC_QUEUE_CUSTOMER |
                  BCM_FABRIC_QUEUE_QOS_BASE |
                  BCM_FABRIC_QUEUE_DEST_OFFSET |
                  0xffff)) {
                return (BCM_E_PARAM);
            }

            break;

#ifdef INCLUDE_L3
        case bcmFieldActionL3ChangeVlan:
        case bcmFieldActionL3ChangeMacDa:
        case bcmFieldActionL3Switch:
            rv = _bcm_field_policy_set_l3_nh_resolve(unit,  arg,
                                                     &flags, &nh_ecmp_id);
            BCM_IF_ERROR_RETURN(rv);
            if (flags & BCM_L3_MULTIPATH) {
                RegexPolicyCheck(unit, mem, ECMP_PTRf, nh_ecmp_id);
            } else {
                RegexPolicyCheck(unit, mem, NEXT_HOP_INDEXf, nh_ecmp_id);
        }
            break;
#endif /* INCLUDE_L3 */
        case bcmFieldActionOamUpMep:
            RegexPolicyCheck(unit, mem, OAM_UP_MEPf, arg);
            break;
        case bcmFieldActionOamTx:
            RegexPolicyCheck(unit, mem, OAM_TXf, arg);
            break;
        case bcmFieldActionOamLmepMdl:
            RegexPolicyCheck(unit, mem, OAM_LMEP_MDLf, arg);
            break;
        case bcmFieldActionOamServicePriMappingPtr:
            RegexPolicyCheck(unit, mem, OAM_SERVICE_PRI_MAPPING_PTRf, arg);
            break;
        case bcmFieldActionOamLmBasePtr:
            RegexPolicyCheck(unit, mem, OAM_LM_BASE_PTRf, arg);
            break;
        case bcmFieldActionOamDmEnable:
            RegexPolicyCheck(unit, mem, OAM_DM_ENf, arg);
            break;
        case bcmFieldActionOamLmEnable:
            RegexPolicyCheck(unit, mem, OAM_LM_ENf, arg);
            break;
        case bcmFieldActionOamLmepEnable:
            RegexPolicyCheck(unit, mem, OAM_LMEP_ENf, arg);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectEgrNextHop:
            /* Get next hop info from Egress Object ID param */
            BCM_IF_ERROR_RETURN(_bcm_field_policy_set_l3_nh_resolve(unit,
                                                                    arg, &flags, &nh_ecmp_id));

            if (flags & BCM_L3_MULTIPATH) {
                /* Param0 - ECMP next hop */
                if (0 == soc_feature(unit, soc_feature_field_action_redirect_ecmp)) {
                    return (BCM_E_PARAM);
                }
                RegexPolicyCheck(unit, mem, REDIRECTIONf, (nh_ecmp_id | (0x3 << 16)));
            } else {
                /* Param0 - Regular next hop */
                RegexPolicyCheck(unit, mem, REDIRECTIONf, (nh_ecmp_id | (0x1 << 14)));
    }
            break;
#endif
        default:
    return BCM_E_NONE;
}
    return (BCM_E_NONE);
}

#ifdef INCLUDE_L3
/*
 * Function:
 *     _regex_tr3_policy_set_l3_info
 * Purpose:
 *     Install l3 forwarding policy entry.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     mem       - (IN) Policy table memory.
 *     value     - (IN) Egress object id or combined next hop information.
 *     buf       - (IN/OUT) Hw entry buffer to write.
 * Returns:
 *     BCM_E_XXX
 */

STATIC int _regex_tr3_policy_set_l3_info(int unit, soc_mem_t mem, int value, uint32 *buf)
{
    uint32 flags;         /* L3 forwarding flags           */
    int nh_ecmp_id;       /* Next hop/Ecmp group id.       */
    int retval;           /* Operation return value.       */

    /* Resove next hop /ecmp group id. */
    retval = _bcm_regex_policy_set_l3_nh_resolve(unit,  value,
                                                 &flags, &nh_ecmp_id);
    BCM_IF_ERROR_RETURN(retval);

    if (flags & BCM_L3_MULTIPATH) {
        LOG_DEBUG(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) vverb: Install mpath L3 policy "
                              "(Ecmp_group: %d)))"),
                   unit, nh_ecmp_id));
        RegexPolicySet(unit, mem, buf, ECMPf, 1);
        RegexPolicySet(unit, mem, buf, ECMP_PTRf, nh_ecmp_id);
    } else {
        LOG_DEBUG(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) vverb: Install unipath L3 policy"
                              "(Next hop id: %d)))"),
                   unit, nh_ecmp_id));
        if (SOC_MEM_FIELD_VALID(unit, mem, ECMPf)) {
            RegexPolicySet(unit, mem, buf, ECMPf, 0);
        }
        RegexPolicySet(unit, mem, buf, NEXT_HOP_INDEXf, nh_ecmp_id);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_egr_nexthop_info_set
 * Purpose:
 *     Set next hop redirection value in the policy table entry.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     mem       - (IN) Policy table memory.
 *     value     - (IN) Egress object id or combined next hop information.
 *     buf       - (IN/OUT) Hw entry buffer to write.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_egr_nexthop_info_set(int unit, soc_mem_t mem,
                                                  int value, uint32 *buf)
{
    uint32 redirect_to_nhi = 0; /* Hw config + (ECMP or NH Info)    */
    uint32 flags;               /* L3 forwarding flags.             */
    int nh_ecmp_id;             /* Next hop/Ecmp group id.          */

    /* Resove next hop /ecmp group id. */
    BCM_IF_ERROR_RETURN(_bcm_regex_policy_set_l3_nh_resolve(unit,  value,
                                                            &flags, &nh_ecmp_id));

    /* Check if NextHop is ECMP or regular Next Hop */
    if (flags & BCM_L3_MULTIPATH) {
        /* ECMP next hop */
        if (soc_feature(unit, soc_feature_field_action_redirect_ecmp)) {
            redirect_to_nhi = (0x3 << 16) | nh_ecmp_id;
        } else {
            /* Non-Trident devices do not support redirect to ECMP action */
            return (BCM_E_UNAVAIL);
        }
        LOG_DEBUG(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) vverb: Set ECMP (Group id: %d\n)))"),
                   unit, nh_ecmp_id));
    } else {
        /* Regular next hop */
        redirect_to_nhi = (0x1 << 14) | nh_ecmp_id;
        LOG_DEBUG(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) vverb: Set unipath (Nexthop index: %d\n)))"),
                   unit, nh_ecmp_id));
    }

    /* Set policy table redirection fields */
    RegexPolicySet(unit, mem, buf, REDIRECTIONf, redirect_to_nhi);
    RegexPolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 1);
    return (BCM_E_NONE);
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *     _regex_tr3_policy_action_fabricQueue_actions_set
 * Purpose:
 *     Install add fabric tag action
 * Parameters:
 *     unit        - (IN) BCM device number
 *     mem         - (IN) Policy table memory
 *     cosq_idx    - (IN) cosq index or (ucast_queue_group/ucast_subscriber) cosq gport
 *     profile_idx - (IN) QoS map profile index
 *     buf         - (IN/OUT) Hw entry buffer to write.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_action_fabricQueue_actions_set(int unit, soc_mem_t mem,
                                                            int cosq_idx, int profile_idx, uint32 *buf)
{
    int traffic_manager;  /* Indicates the intended application for the
                             SBX_QUEUE_TAG.0x0-Fabric Interface Chip.
                             0x1 Traffic Manager.*/
    int tag_type;        /*  SBX tag type.                    */
    uint32 param0 = 0;
    uint32 in_flags = 0;

    if (NULL == buf) {
        return (BCM_E_PARAM);
    }

    param0 = cosq_idx;

    in_flags = param0;

    /* Tag type resolution. */
    if (in_flags & BCM_FABRIC_QUEUE_QOS_BASE) {
        tag_type = 0x3;  /* Offset to base queue number from the
                            QUEUE_MAP Table. Index into QUEUE_MAP
                            Table is {DST_MODID, DST_PID} */
    } else if (in_flags & BCM_FABRIC_QUEUE_DEST_OFFSET) {
        tag_type = 0x2;  /* Index into QUEUE_MAP Table used for lookup. */
    } else {
        tag_type = 0x1;  /* Explicit queue number. */
    }

    RegexPolicySet(unit, mem, buf, EH_TAG_TYPEf, tag_type);

    /* Traffic manager vs Fabric chip queue selection. */
    if (soc_mem_field_valid (unit, mem, EH_TMf)) {
        traffic_manager = (cosq_idx & BCM_FABRIC_QUEUE_CUSTOMER) ? 0x1 : 0x0;
        RegexPolicySet(unit, mem, buf, EH_TMf, traffic_manager);
    }

    /* Set queue number. */
    RegexPolicySet(unit, mem, buf, EH_QUEUE_TAGf, cosq_idx & 0xffff);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_action_copy_to_cpu
 * Purpose:
 *     Install copy to cpu action in policy table.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     p_ent    - (IN) Regex policy structure to get policy info from
 *     pa       - (IN  Policy action
 *     buf      - (OUT) Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
int _regex_tr3_policy_action_copy_to_cpu(int unit, soc_mem_t mem,
                                         _regex_policy_t *p_ent,
                                         _regex_policy_action_t *pa,
                                         uint32 *buf)
{
    if (NULL == p_ent || NULL == pa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (pa->action) {
        case bcmFieldActionTimeStampToCpu:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x5);
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x5);
            RegexPolicySet(unit, mem, buf, R_DROPf, 0x1);
            RegexPolicySet(unit, mem, buf, Y_DROPf, 0x1);
            RegexPolicySet(unit, mem, buf, G_DROPf, 0x1);
            break;
        case bcmFieldActionRpTimeStampToCpu:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x5);
            RegexPolicySet(unit, mem, buf, R_DROPf, 0x1);
            break;
        case bcmFieldActionYpTimeStampToCpu:
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_DROPf, 0x1);
            break;
        case bcmFieldActionGpTimeStampToCpu:
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x5);
            RegexPolicySet(unit, mem, buf, G_DROPf, 0x1);
            break;
        case bcmFieldActionCopyToCpu:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x1);
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x1);
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x1);
            break;
        case bcmFieldActionRpCopyToCpu:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x1);
            break;
        case bcmFieldActionYpCopyToCpu:
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x1);
            break;
        case bcmFieldActionGpCopyToCpu:
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x1);
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    if (pa->param[0] != 0) {
        RegexPolicySet(unit, mem, buf, MATCHED_RULEf, pa->param[1]);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_action_copy_to_cpu_cancel
 * Purpose:
 *     Override copy to cpu action in policy table.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     p_ent    - (IN) Regex policy structure to get policy info from
 *     pa       - (IN  Policy action
 *     buf      - (OUT) Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_action_copy_to_cpu_cancel(int unit, soc_mem_t mem,
                                                       _regex_policy_t *p_ent,
                                                       _regex_policy_action_t *pa,
                                                       uint32 *buf)
{
    if (NULL == p_ent || NULL == pa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (pa->action) {
        case bcmFieldActionTimeStampToCpuCancel:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
            RegexPolicySet(unit, mem, buf, R_DROPf, 0x2);
            RegexPolicySet(unit, mem, buf, Y_DROPf, 0x2);
            RegexPolicySet(unit, mem, buf, G_DROPf, 0x2);
            break;
        case bcmFieldActionRpTimeStampToCpuCancel:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
            RegexPolicySet(unit, mem, buf, R_DROPf, 0x2);
            break;
        case bcmFieldActionYpTimeStampToCpuCancel:
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
            RegexPolicySet(unit, mem, buf, Y_DROPf, 0x2);
            break;
        case bcmFieldActionGpTimeStampToCpuCancel:
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
            RegexPolicySet(unit, mem, buf, G_DROPf, 0x2);
            break;
        case bcmFieldActionCopyToCpuCancel:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
            break;
        case bcmFieldActionRpCopyToCpuCancel:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
            break;
        case bcmFieldActionYpCopyToCpuCancel:
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
            break;
        case bcmFieldActionGpCopyToCpuCancel:
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_cosq_action_param_get
 * Purpose:
 *     Returns CosQ action's param value if action is set for the
 *     entry.
 * Parameters:
 *     unit     - BCM device number
 *     p_ent    - pointer to policy entry structure
 *     action   - CosQ action for which param value needs to be retrieved
 *     cosq     - (OUT) CoS value
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_cosq_action_param_get(int unit,
                                                   _regex_policy_t *p_ent,
                                                   bcm_field_action_t action,
                                                   int *cosq)
{
    _regex_policy_action_t *pa_iter = NULL;    /* Policy action */

    if (NULL == cosq || NULL == p_ent) {
        return (BCM_E_INTERNAL);
    }

    /* Get the requested action param. */
    pa_iter = p_ent->actions;
    while (pa_iter != NULL) {
        if (action == pa_iter->action) {
            *cosq = pa_iter->param[0];
            return (BCM_E_NONE);
        }
        pa_iter = pa_iter->next;
    }

    /* Specified action not found in policy action list. */
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     _regex_tr3_policy_ucast_mcast_action_set
 * Purpose:
 *     Set Unicast and Multicast CoS action in the policy buffer
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     p_ent    - policy structure to get policy info from
 *     pa       - policy action
 *     buf      - (OUT) Regex Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_ucast_mcast_action_set(int unit,
                                                    soc_mem_t mem,
                                                    _regex_policy_t *p_ent,
                                                    _regex_policy_action_t *pa,
                                                    uint32 *buf)
{
    int m_cos = BCM_COS_INVALID;
    int u_cos = BCM_COS_INVALID;
    int cos = BCM_COS_INVALID;
    int rv;

    if (NULL == p_ent || NULL == pa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    cos = (int)pa->param[0];

    switch (pa->action) {
        case bcmFieldActionMcastCosQNew:
            m_cos = cos;
            rv = _regex_tr3_policy_cosq_action_param_get(unit,
                                                         p_ent,
                                                         bcmFieldActionUcastCosQNew,
                                                         &u_cos);
            if (BCM_SUCCESS(rv)) {
                RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
                RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
                RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
                RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
                RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_NEW_MODE);
                RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_NEW_MODE);
                RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_SET(m_cos));
                RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_SET(m_cos));
                RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_SET(m_cos));
            }
            break;

        case bcmFieldActionGpMcastCosQNew:
            m_cos = cos;
            rv = _regex_tr3_policy_cosq_action_param_get(unit,
                                                         p_ent, bcmFieldActionGpUcastCosQNew, &u_cos);
            if (BCM_SUCCESS(rv)) {
                RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_SET(m_cos));
            }
            break;

        case bcmFieldActionYpMcastCosQNew:
            m_cos = cos;
            rv = _regex_tr3_policy_cosq_action_param_get(unit,
                                                         p_ent, bcmFieldActionYpUcastCosQNew, &u_cos);
            if (BCM_SUCCESS(rv)) {
                RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_SET(m_cos));
            }
            break;

        case bcmFieldActionRpMcastCosQNew:
            m_cos = cos;
            rv = _regex_tr3_policy_cosq_action_param_get(unit,
                                                         p_ent, bcmFieldActionRpUcastCosQNew, &u_cos);
            if (BCM_SUCCESS(rv)) {
                RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                               _REGEX_ACTION_MCAST_QUEUE_SET(m_cos));
            }
            break;

        case bcmFieldActionUcastCosQNew:
            u_cos = cos;
            rv = _regex_tr3_policy_cosq_action_param_get(unit,
                                                         p_ent, bcmFieldActionMcastCosQNew, &m_cos);
            if (BCM_SUCCESS(rv)) {
                RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
                RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
                RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
                RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
                RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);
                RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);
                RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_SET(u_cos));
                RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_SET(u_cos));
                RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_SET(u_cos));
            }
            break;

        case bcmFieldActionGpUcastCosQNew:
            u_cos = cos;
            rv = _regex_tr3_policy_cosq_action_param_get(unit,
                                                         p_ent, bcmFieldActionGpMcastCosQNew, &m_cos);
            if (BCM_SUCCESS(rv)) {
                RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_SET(u_cos));
            }
            break;

        case bcmFieldActionYpUcastCosQNew:
            u_cos = cos;
            rv = _regex_tr3_policy_cosq_action_param_get(unit,
                                                         p_ent, bcmFieldActionYpMcastCosQNew, &m_cos);
            if (BCM_SUCCESS(rv)) {
                RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_SET(u_cos));
            }
            break;

        case bcmFieldActionRpUcastCosQNew:
            u_cos = cos;
            rv = _regex_tr3_policy_cosq_action_param_get(unit,
                                                         p_ent, bcmFieldActionRpMcastCosQNew, &m_cos);
            if (BCM_SUCCESS(rv)) {
                RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

                RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                               _REGEX_ACTION_UCAST_QUEUE_SET(u_cos));
            }
            break;

        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_mirror_add
 * Purpose:
 *     Set mirroring destination & enable mirroring for rule matching
 *     packets.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     f_ent    - (IN) Entry structure to get policy info from
 *     fa       - (IN) field action
 *     buf      - (IN/OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policy_mirror_add(int unit, soc_mem_t mem, _regex_policy_t *p_ent,
                                        _regex_policy_action_t *pa, uint32 *buf)
{
    int    mtp_index;
    uint32 enable;
    int    index;

    soc_field_t mtp_field[] = {
        MTP_INDEX0f, MTP_INDEX1f,
        MTP_INDEX2f, MTP_INDEX3f};

    /* Input parameters check. */
    if (NULL == p_ent || NULL == pa || NULL == buf) {
        return (BCM_E_PARAM);
    }
    mtp_index = pa->hw_index;
    index     = pa->hw_index;
    if (index >= COUNTOF(mtp_field)) {
        return (BCM_E_INTERNAL);
    }

    enable = PolicyGet(unit, mem, buf, MIRRORf);
    PolicySet(unit, mem, buf, mtp_field[index], mtp_index);
    PolicySet(unit, mem, buf, MIRRORf, (enable | (0x1 << index)));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policy_action_get
 * Purpose:
 *     Get the actions to be written
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     p_ent    - policy structure to get policy info from
 *     pa       - policy action
 *     buf      - (OUT) Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     Regex unit lock should be held by calling function.
 */
STATIC int _regex_tr3_policy_action_get(int unit, soc_mem_t mem, _regex_policy_t *p_ent,
                                        _regex_policy_action_t *pa, uint32 *buf)
{
    int         rv;
    soc_field_t l3sw_or_change_l2 = INVALIDf;
    uint32      arg;
    uint32      arg2;

    if (NULL == p_ent || NULL == pa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: BEGIN %s(eid=%d)\n"),
               unit, FUNCTION_NAME(), p_ent->eid));

    arg  = pa->param[0];
    arg2 = pa->param[1];

    if (SOC_MEM_FIELD_VALID(unit, mem, G_L3SW_CHANGE_MACDA_OR_VLANf)) {
        l3sw_or_change_l2 = G_L3SW_CHANGE_MACDA_OR_VLANf;
    } else {
        l3sw_or_change_l2 = G_L3SW_CHANGE_L2_FIELDSf;
    }

    switch (pa->action) {
        case bcmFieldActionNewClassId:
            RegexPolicySet(unit, mem, buf, I2E_CLASSIDf, arg);
            RegexPolicySet(unit, mem, buf, G_L3SW_CHANGE_L2_FIELDSf, 0x8);
            break;
        case bcmFieldActionTrunkLoadBalanceCancel:
            RegexPolicySet(unit, mem, buf, LAG_DLB_DISABLEf, 1);
            RegexPolicySet(unit, mem, buf, HGT_DLB_DISABLEf, 1);
            break;

        case bcmFieldActionEcmpLoadBalanceCancel:
            RegexPolicySet(unit, mem, buf, ECMP_DLB_DISABLEf, 1);
            break;

        case bcmFieldActionEgressClassSelect:
        {
            unsigned i2e_cl_sel;

            switch (arg) {
                case BCM_FIELD_EGRESS_CLASS_SELECT_PORT:
                    i2e_cl_sel = 0x1;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_SVP:
                    i2e_cl_sel = 0x2;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF:
                    i2e_cl_sel = 0x3;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_SRC:
                    i2e_cl_sel = 0x4; /* VFP hi */
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_DST:
                    i2e_cl_sel = 0x5; /* VFP lo */
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L2_SRC:
                    i2e_cl_sel = 0x6;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L2_DST:
                    i2e_cl_sel = 0x7;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC:
                    i2e_cl_sel = 0x8;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST:
                    i2e_cl_sel = 0x9;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_VLAN:
                    i2e_cl_sel = 0xa;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_VRF:
                    i2e_cl_sel = 0xb;
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_NEW:
                    i2e_cl_sel = 0xf;
                    break;
                default:
                    /* Invalid parameter should have been caught earlier */
                    return (BCM_E_INTERNAL);
            }

            RegexPolicySet(unit, mem, buf, I2E_CLASSID_SELf, i2e_cl_sel);
        }
        break;

        case bcmFieldActionHiGigClassSelect:
        {
            unsigned hg_cl_sel;

            switch (arg) {
                case BCM_FIELD_HIGIG_CLASS_SELECT_EGRESS:
                    hg_cl_sel = 1;
                    break;
                case BCM_FIELD_HIGIG_CLASS_SELECT_EGR_DVP:
                    hg_cl_sel = 4;
                    break;
                case BCM_FIELD_HIGIG_CLASS_SELECT_EGR_L3_INTERFACE:
                    hg_cl_sel = 3;
                    break;
                case BCM_FIELD_HIGIG_CLASS_SELECT_EGR_NEXT_HOP:
                    hg_cl_sel = 2;
                    break;
                case BCM_FIELD_HIGIG_CLASS_SELECT_PORT:
                    hg_cl_sel = 7;
                    break;
                default:
                    /* Invalid parameter should have been caught earlier */
                    return (BCM_E_INTERNAL);
            }

            RegexPolicySet(unit, mem, buf, HG_CLASSID_SELf, hg_cl_sel);
        }
        break;

        case bcmFieldActionOamDmTimeFormat:
        {
            unsigned oam_dm_type;

            switch (arg) {
                case BCM_FIELD_OAM_DM_TIME_FORMAT_IEEE1588:
                    oam_dm_type = 0;
                    break;
                case BCM_FIELD_OAM_DM_TIME_FORMAT_NTP:
                    oam_dm_type = 1;
                    break;
                default:
                    /* Invalid parameter should have been caught earlier */
                    return (BCM_E_INTERNAL);
            }

            RegexPolicySet(unit, mem, buf, OAM_DM_TYPEf, oam_dm_type);
        }
        break;

        case bcmFieldActionOamLmDmSampleEnable:
            RegexPolicySet(unit, mem, buf, OAM_ENABLE_LM_DM_SAMPLEf, arg ? 1 : 0);
            break;

        case bcmFieldActionOamTagStatusCheck:
            RegexPolicySet(unit, mem, buf, OAM_TAG_STATUS_CHECK_CONTROLf, arg);
            break;

        case bcmFieldActionOamTunnelControl:
        {
            unsigned oam_tnl_cntl;

            switch (arg) {
                case BCM_FIELD_OAM_TUNNEL_CONTROL_ANY:
                    oam_tnl_cntl = 0;
                    break;
                case BCM_FIELD_OAM_TUNNEL_CONTROL_NOT_TUNNELDED:
                    oam_tnl_cntl = 1;
                    break;
                default:
                    /* Invalid parameter should have been caught earlier */
                    return (BCM_E_INTERNAL);
            }

            RegexPolicySet(unit, mem, buf, OAM_TUNNEL_CONTROLf, oam_tnl_cntl);
        }
        break;

        case bcmFieldActionEcnNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_ECNf, 1);
            RegexPolicySet(unit, mem, buf, R_NEW_ECNf, arg);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_ECNf, 1);
            RegexPolicySet(unit, mem, buf, Y_NEW_ECNf, arg);
            RegexPolicySet(unit, mem, buf, G_CHANGE_ECNf, 1);
            RegexPolicySet(unit, mem, buf, G_NEW_ECNf, arg);
            break;

        case bcmFieldActionRpEcnNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_ECNf, 1);
            RegexPolicySet(unit, mem, buf, R_NEW_ECNf, arg);
            break;

        case bcmFieldActionYpEcnNew:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_ECNf, 1);
            RegexPolicySet(unit, mem, buf, Y_NEW_ECNf, arg);
            break;

        case bcmFieldActionGpEcnNew:
            RegexPolicySet(unit, mem, buf, G_CHANGE_ECNf, 1);
            RegexPolicySet(unit, mem, buf, G_NEW_ECNf, arg);
            break;

        case bcmFieldActionCosQNew:
            /* Apply this action for Gp/Yp/Rp unicast packets. */
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

            /* Set new CoSQ/hardware queue value. */
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_SET(arg));

            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_SET(arg));

            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_SET(arg));
            break;

        case bcmFieldActionGpCosQNew:
            /* Apply this action for Gp unicast packets. */
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

            /* Set new CoSQ/hardware queue value. */
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_SET(arg));

            break;

        case bcmFieldActionYpCosQNew:
            /* Apply this action for Yp unicast packets. */
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

            /* Set new CoSQ/hardware queue value. */
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_SET(arg));
            break;

        case bcmFieldActionRpCosQNew:
            /* Apply this action for Rp unicast packets. */
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_NEW_MODE);

            /* Set new CoSQ/hardware queue value. */
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                           _REGEX_ACTION_UCAST_QUEUE_SET(arg));
            break;

        case bcmFieldActionUcastCosQNew:
        case bcmFieldActionMcastCosQNew:
        case bcmFieldActionGpMcastCosQNew:
        case bcmFieldActionYpMcastCosQNew:
        case bcmFieldActionRpMcastCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionYpUcastCosQNew:
        case bcmFieldActionRpUcastCosQNew:
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_ucast_mcast_action_set(unit, mem, p_ent,
                                                                         pa, buf));
            break;
        case bcmFieldActionPortPrioIntCosQNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 10);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 10);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 10);
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                           (_BCM_COSQ_CLASSIFIER_FIELD_GET(arg) << 9 | arg2));
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                           (_BCM_COSQ_CLASSIFIER_FIELD_GET(arg) << 9 | arg2));
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                           (_BCM_COSQ_CLASSIFIER_FIELD_GET(arg) << 9 | arg2));
            break;
        case bcmFieldActionRpPortPrioIntCosQNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 10);
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf,
                           (_BCM_COSQ_CLASSIFIER_FIELD_GET(arg) << 9 | arg2));
            break;
        case bcmFieldActionYpPortPrioIntCosQNew:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 10);
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                           (_BCM_COSQ_CLASSIFIER_FIELD_GET(arg) << 9 | arg2));
            break;
        case bcmFieldActionGpPortPrioIntCosQNew:
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 10);
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf,
                           (_BCM_COSQ_CLASSIFIER_FIELD_GET(arg) << 9 | arg2));
            break;
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionGpCopyToCpu:
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_action_copy_to_cpu(unit, mem, p_ent,
                                                                     pa, buf));
            break;






        case bcmFieldActionMultipathHash:
            RegexPolicySet(unit, mem, buf, ECMP_HASH_SELf, pa->param[0]);
            break;
        case bcmFieldActionTimeStampToCpu:
        case bcmFieldActionRpTimeStampToCpu:
        case bcmFieldActionYpTimeStampToCpu:
        case bcmFieldActionGpTimeStampToCpu:
            rv = _regex_tr3_policy_action_copy_to_cpu(unit, mem, p_ent, pa, buf);
            BCM_IF_ERROR_RETURN(rv);
            break;

        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionTimeStampToCpuCancel:
        case bcmFieldActionRpTimeStampToCpuCancel:
        case bcmFieldActionYpTimeStampToCpuCancel:
        case bcmFieldActionGpTimeStampToCpuCancel:
            rv = _regex_tr3_policy_action_copy_to_cpu_cancel(unit, mem, p_ent, pa, buf);
            BCM_IF_ERROR_RETURN(rv);
            break;

        case bcmFieldActionDrop:
            RegexPolicySet(unit, mem, buf, R_DROPf, 0x1);
            RegexPolicySet(unit, mem, buf, Y_DROPf, 0x1);
            RegexPolicySet(unit, mem, buf, G_DROPf, 0x1);
            break;

        case bcmFieldActionDropCancel:
            RegexPolicySet(unit, mem, buf, R_DROPf, 0x2);
            RegexPolicySet(unit, mem, buf, Y_DROPf, 0x2);
            RegexPolicySet(unit, mem, buf, G_DROPf, 0x2);
            break;

        case bcmFieldActionDropPrecedence:
            RegexPolicySet(unit, mem, buf, R_DROP_PRECEDENCEf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, Y_DROP_PRECEDENCEf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, G_DROP_PRECEDENCEf, pa->param[0]);
            break;

        case bcmFieldActionPrioPktCopy:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x4);
            break;

        case bcmFieldActionPrioPktNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, R_NEW_PKT_PRIf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_NEW_PKT_PRIf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, G_NEW_PKT_PRIf, pa->param[0]);
            break;

        case bcmFieldActionPrioPktTos:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x6);
            break;

        case bcmFieldActionPrioPktCancel:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x7);
            break;

        case bcmFieldActionDscpNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_DSCPf, 0x1);
            RegexPolicySet(unit, mem, buf, R_NEW_DSCPf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_DSCPf, 0x1);
            RegexPolicySet(unit, mem, buf, Y_NEW_DSCPf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x3);
            RegexPolicySet(unit, mem, buf, G_NEW_DSCP_TOSf, pa->param[0]);
            break;

        case bcmFieldActionDscpCancel:
            RegexPolicySet(unit, mem, buf, R_CHANGE_DSCPf, 0x2);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_DSCPf, 0x2);
            RegexPolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x4);
            break;

        case bcmFieldActionVlanCosQNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x2);
            /* Add 8 to the value since VLAN shaping queues are 8..23 */
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf, pa->param[0] + 8);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x2);
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf, pa->param[0] + 8);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x2);
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf, pa->param[0] + 8);
            break;

        case bcmFieldActionPrioIntCopy:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x4);
            break;

        case bcmFieldActionPrioIntNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf, (0xf & pa->param[0]));
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf, (0xf & pa->param[0]));
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf, (0xf & pa->param[0]));
            break;

        case bcmFieldActionPrioIntTos:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x6);
            break;

        case bcmFieldActionPrioIntCancel:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x7);
            break;

        case bcmFieldActionPrioPktAndIntNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, R_NEW_PKT_PRIf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf, (0xf & pa->param[0]));
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_NEW_PKT_PRIf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf, (0xf & pa->param[0]));
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, G_NEW_PKT_PRIf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf, (0xf & pa->param[0]));
            break;

        case bcmFieldActionPrioPktAndIntCopy:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x4);
            break;

        case bcmFieldActionPrioPktAndIntTos:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x6);
            break;

        case bcmFieldActionPrioPktAndIntCancel:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x7);
            break;

        case bcmFieldActionCosQCpuNew:
            RegexPolicySet(unit, mem, buf, CHANGE_CPU_COSf, 0x1);
            RegexPolicySet(unit, mem, buf, CPU_COSf, pa->param[0]);
            break;

        case bcmFieldActionSwitchToCpuCancel:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x3);
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x3);
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x3);
            break;

        case bcmFieldActionSwitchToCpuReinstate:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x4);
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x4);
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x4);
            break;

        case bcmFieldActionIpFix:
            RegexPolicySet(unit, mem, buf, IPFIX_CONTROLf, 0x1);
            break;

        case bcmFieldActionIpFixCancel:
            RegexPolicySet(unit, mem, buf, IPFIX_CONTROLf, 0x2);
            break;

        case bcmFieldActionOuterVlanNew:
            RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x1);
            if (SOC_MEM_FIELD_VALID(unit, mem, ECMPf)) {
                RegexPolicySet(unit, mem, buf, ECMPf, 0);
            }
            RegexPolicySet(unit, mem, buf, NEXT_HOP_INDEXf, pa->hw_index);
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;

        case bcmFieldActionSrcMacNew:
        case bcmFieldActionDstMacNew:
            RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x1);
            if (SOC_MEM_FIELD_VALID(unit, mem, ECMPf)) {
                RegexPolicySet(unit, mem, buf, ECMPf, 0);
            }
            RegexPolicySet(unit, mem, buf, NEXT_HOP_INDEXf, pa->hw_index);
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionVnTagNew:
        case bcmFieldActionVnTagDelete:
            RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x1);
            if (SOC_MEM_FIELD_VALID(unit, mem, ECMPf)) {
                RegexPolicySet(unit, mem, buf, ECMPf, 0);
            }
            RegexPolicySet(unit, mem, buf, NEXT_HOP_INDEXf, pa->hw_index);
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;

        case bcmFieldActionMirrorOverride:
            RegexPolicySet(unit, mem, buf, MIRROR_OVERRIDEf, 1);
            break;

        case bcmFieldActionDoNotChangeTtl:
            RegexPolicySet(unit, mem, buf, DO_NOT_CHANGE_TTLf, 1);
            break;

        case bcmFieldActionDoNotCheckUrpf:
            RegexPolicySet(unit, mem, buf, DO_NOT_URPFf, 1);
            break;

        case bcmFieldActionCnmCancel:
            RegexPolicySet(unit, mem, buf, DO_NOT_GENERATE_CNMf, 1);
            break;

        case bcmFieldActionDynamicHgTrunkCancel:
            RegexPolicySet(unit, mem, buf, DISABLE_DYNAMIC_LOAD_BALANCINGf, 1);
            break;
        case bcmFieldActionMirrorIngress:    /* param0=modid, param1=port/tgid */
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_mirror_add(unit, mem, p_ent, pa, buf));
            break;
        case bcmFieldActionMirrorEgress:     /* param0=modid, param1=port/tgid */
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_mirror_add(unit, mem, p_ent, pa, buf));
            break;
        case bcmFieldActionOffloadRedirect:
        case bcmFieldActionRedirect:
        case bcmFieldActionRedirectTrunk:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionRedirectPbmp:
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectIpmc:
#endif /* INCLUDE_L3 */
        case bcmFieldActionRedirectMcast:
        case bcmFieldActionRedirectVlan:
        case bcmFieldActionRedirectBcastPbmp:
        case bcmFieldActionEgressMask:
        case bcmFieldActionEgressPortsAdd:
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_action_redirect(unit, mem, p_ent, pa, buf));
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionOffloadDropIndication:
            RegexPolicySet(unit, mem, buf, SPLIT_DROP_RESOLVEf, 0x1);
            break;
        case bcmFieldActionOffloadClassSet:
            RegexPolicySet(unit, mem, buf, PPD3_CLASS_TAGf, pa->param[0]);
            break;
        case bcmFieldActionRpDrop:
            RegexPolicySet(unit, mem, buf, R_DROPf, 0x1);
            break;
        case bcmFieldActionRpDropCancel:
            RegexPolicySet(unit, mem, buf, R_DROPf, 0x2);
            break;
        case bcmFieldActionRpDropPrecedence:
            RegexPolicySet(unit, mem, buf, R_DROP_PRECEDENCEf, pa->param[0]);
            break;
        case bcmFieldActionRpSwitchToCpuCancel:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x3);
            break;
        case bcmFieldActionRpSwitchToCpuReinstate:
            RegexPolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x4);
            break;
        case bcmFieldActionRpPrioPktCopy:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x4);
            break;
        case bcmFieldActionRpPrioPktNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, R_NEW_PKT_PRIf, pa->param[0]);
            break;
        case bcmFieldActionRpPrioPktTos:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x6);
            break;
        case bcmFieldActionRpPrioPktCancel:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x7);
            break;
        case bcmFieldActionRpDscpNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_DSCPf, 0x1);
            RegexPolicySet(unit, mem, buf, R_NEW_DSCPf, pa->param[0]);
            break;
        case bcmFieldActionRpDscpCancel:
            RegexPolicySet(unit, mem, buf, R_CHANGE_DSCPf, 0x2);
            break;
        case bcmFieldActionRpVlanCosQNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x2);
            /* Add 8 to the value since VLAN shaping queues are 8..23 */
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf, pa->param[0] + 8);
            break;
        case bcmFieldActionRpPrioPktAndIntCopy:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x4);
            break;
        case bcmFieldActionRpPrioPktAndIntNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, R_NEW_PKT_PRIf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf, (0xf & pa->param[0]));
            break;
        case bcmFieldActionRpPrioPktAndIntTos:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x6);
            break;
        case bcmFieldActionRpPrioPktAndIntCancel:
            RegexPolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x7);
            break;
        case bcmFieldActionRpPrioIntCopy:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x4);
            break;
        case bcmFieldActionRpPrioIntNew:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, R_COS_INT_PRIf, (0xf & pa->param[0]));
            break;
        case bcmFieldActionRpPrioIntTos:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x6);
            break;
        case bcmFieldActionRpPrioIntCancel:
            RegexPolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x7);
            break;
        case bcmFieldActionYpDrop:
            RegexPolicySet(unit, mem, buf, Y_DROPf, 0x1);
            break;
        case bcmFieldActionYpDropCancel:
            RegexPolicySet(unit, mem, buf, Y_DROPf, 0x2);
            break;
        case bcmFieldActionYpDropPrecedence:
            RegexPolicySet(unit, mem, buf, Y_DROP_PRECEDENCEf, pa->param[0]);
            break;
        case bcmFieldActionYpSwitchToCpuCancel:
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x3);
            break;
        case bcmFieldActionYpSwitchToCpuReinstate:
            RegexPolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x4);
            break;
        case bcmFieldActionYpPrioPktCopy:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x4);
            break;
        case bcmFieldActionYpPrioPktNew:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_NEW_PKT_PRIf, pa->param[0]);
            break;
        case bcmFieldActionYpPrioPktTos:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x6);
            break;
        case bcmFieldActionYpPrioPktCancel:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x7);
            break;
        case bcmFieldActionYpDscpNew:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_DSCPf, 0x1);
            RegexPolicySet(unit, mem, buf, Y_NEW_DSCPf, pa->param[0]);
            break;
        case bcmFieldActionYpDscpCancel:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_DSCPf, 0x2);
            break;
        case bcmFieldActionYpVlanCosQNew:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x2);
            /* Add 8 to the value since VLAN shaping queues are 8..23 */
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf, pa->param[0] + 8);
            break;
        case bcmFieldActionYpPrioPktAndIntCopy:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x4);
            break;
        case bcmFieldActionYpPrioPktAndIntNew:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_NEW_PKT_PRIf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf, (0xf & pa->param[0]));
            break;
        case bcmFieldActionYpPrioPktAndIntTos:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x6);
            break;
        case bcmFieldActionYpPrioPktAndIntCancel:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x7);
            break;
        case bcmFieldActionYpPrioIntCopy:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x4);
            break;
        case bcmFieldActionYpPrioIntNew:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, Y_COS_INT_PRIf, (0xf & pa->param[0]));
            break;
        case bcmFieldActionYpPrioIntTos:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x6);
            break;
        case bcmFieldActionYpPrioIntCancel:
            RegexPolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x7);
            break;
        case bcmFieldActionGpDrop:
            RegexPolicySet(unit, mem, buf, G_DROPf, 0x1);
            break;
        case bcmFieldActionGpDropCancel:
            RegexPolicySet(unit, mem, buf, G_DROPf, 0x2);
            break;
        case bcmFieldActionGpDropPrecedence:
            RegexPolicySet(unit, mem, buf, G_DROP_PRECEDENCEf, pa->param[0]);
            break;
        case bcmFieldActionGpSwitchToCpuCancel:
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x3);
            break;
        case bcmFieldActionGpSwitchToCpuReinstate:
            RegexPolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x4);
            break;
        case bcmFieldActionGpPrioPktCopy:
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x4);
            break;
        case bcmFieldActionGpPrioPktNew:
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, G_NEW_PKT_PRIf, pa->param[0]);
            break;
        case bcmFieldActionGpPrioPktTos:
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x6);
            break;
        case bcmFieldActionGpPrioPktCancel:
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x7);
            break;
        case bcmFieldActionGpTosPrecedenceNew:
            RegexPolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x1);
            RegexPolicySet(unit, mem, buf, G_NEW_DSCP_TOSf, pa->param[0]);
            break;
        case bcmFieldActionGpTosPrecedenceCopy:
            RegexPolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x2);
            break;
        case bcmFieldActionGpDscpNew:
            RegexPolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x3);
            RegexPolicySet(unit, mem, buf, G_NEW_DSCP_TOSf, pa->param[0]);
            break;
        case bcmFieldActionGpDscpCancel:
            RegexPolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x4);
            break;
        case bcmFieldActionGpVlanCosQNew:
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x2);
            /* Add 8 to the value since VLAN shaping queues are 8..23 */
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf, pa->param[0] + 8);
            break;
        case bcmFieldActionGpPrioPktAndIntCopy:
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x4);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x4);
            break;
        case bcmFieldActionGpPrioPktAndIntNew:
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, G_NEW_PKT_PRIf, pa->param[0]);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf, (0xf & pa->param[0]));
            break;
        case bcmFieldActionGpPrioPktAndIntTos:
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x6);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x6);
            break;
        case bcmFieldActionGpPrioPktAndIntCancel:
            RegexPolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x7);
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x7);
            break;
        case bcmFieldActionGpPrioIntCopy:
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x4);
            break;
        case bcmFieldActionGpPrioIntNew:
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x5);
            RegexPolicySet(unit, mem, buf, G_COS_INT_PRIf, (0xf & pa->param[0]));
            break;
        case bcmFieldActionGpPrioIntTos:
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x6);
            break;
        case bcmFieldActionGpPrioIntCancel:
            RegexPolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x7);
            break;
        case bcmFieldActionAddClassTag:
            if (SOC_MEM_FIELD_VALID(unit, mem, G_L3SW_CHANGE_L2_FIELDSf)) {
                RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x4);
            } else {
                RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x3);
            }
            RegexPolicySet(unit, mem, buf, NEXT_HOP_INDEXf, pa->param[0]);
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionFabricQueue:
            RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x3);
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_action_fabricQueue_actions_set(unit, mem, pa->param[0],
                                                                                 pa->param[1], buf));
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionL3ChangeVlan:
            RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x1);
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_set_l3_info(unit, mem, pa->param[0], buf));
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionL3ChangeVlanCancel:
            RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x2);
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionL3ChangeMacDa:
            if (SOC_MEM_FIELD_VALID(unit, mem, G_L3SW_CHANGE_L2_FIELDSf)) {
                RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x1);
            } else {
                RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x4);
            }
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_set_l3_info(unit, mem,  pa->param[0], buf));
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionL3ChangeMacDaCancel:
            if (SOC_MEM_FIELD_VALID(unit, mem, G_L3SW_CHANGE_L2_FIELDSf)) {
                RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x2);
            } else {
                RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x5);
            }
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionL3Switch:
            RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x6);
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_set_l3_info(unit, mem, pa->param[0], buf));
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
        case bcmFieldActionL3SwitchCancel:
            RegexPolicySet(unit, mem, buf, l3sw_or_change_l2, 0x7);
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
            break;
#endif /* INCLUDE_L3 */
        case bcmFieldActionOamUpMep:
            RegexPolicySet(unit, mem, buf, OAM_UP_MEPf, pa->param[0]);
            break;
        case bcmFieldActionOamTx:
            RegexPolicySet(unit, mem, buf, OAM_TXf, pa->param[0]);
            break;
        case bcmFieldActionOamLmepMdl:
            RegexPolicySet(unit, mem, buf, OAM_LMEP_MDLf, pa->param[0]);
            break;
        case bcmFieldActionOamServicePriMappingPtr:
            RegexPolicySet(unit, mem, buf, OAM_SERVICE_PRI_MAPPING_PTRf, pa->param[0]);
            break;
        case bcmFieldActionOamLmBasePtr:
            RegexPolicySet(unit, mem, buf, OAM_LM_BASE_PTRf, pa->param[0]);
            break;
        case bcmFieldActionOamDmEnable:
            RegexPolicySet(unit, mem, buf, OAM_DM_ENf, pa->param[0]);
            break;
        case bcmFieldActionOamLmEnable:
            RegexPolicySet(unit, mem, buf, OAM_LM_ENf, pa->param[0]);
            break;
        case bcmFieldActionOamLmepEnable:
            RegexPolicySet(unit, mem, buf, OAM_LMEP_ENf, pa->param[0]);
            break;
        case bcmFieldActionUseGlobalMeterColor:
            if (soc_feature(unit, soc_feature_global_meter)) {
                RegexPolicySet(unit, mem, buf, USE_SVC_METER_COLORf,1);
            }
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectEgrNextHop:
            BCM_IF_ERROR_RETURN(_regex_tr3_policy_egr_nexthop_info_set(unit, mem,
                                                                       pa->param[0], buf));
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
                break;
#endif
        default:
            return (BCM_E_UNAVAIL);
            }

    pa->flags &= ~_REGEX_ACTION_DIRTY; /* Mark action as installed. */

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: END %s()\n"),
               unit, FUNCTION_NAME()));
    return (BCM_E_NONE);
        }

/*
 * Function:
 *     _regex_tr3_policy_install
 * Purpose:
 *     Write policy into the chip's memory.
 * Parameters:
 *     unit     -   BCM Unit
 *     p_ent    -   Physical policy structure to be installed
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int _regex_tr3_policy_install(int unit, _regex_policy_t *p_ent)
{
    uint32                  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to fill Policy entry. */
    soc_mem_t               policy_mem;                 /* Policy table memory id.      */
    int                     rv;                         /* Operation return status.     */
    _regex_policy_action_t *pa = NULL;

    /* Input parameters check. */
    if (NULL == p_ent) {
        return (BCM_E_PARAM);
    }

    policy_mem = FT_POLICYm;

    /* Start with an empty POLICY entry */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    /* Extract the policy info from the entry structure. */
    for (pa = p_ent->actions; ((pa != NULL) && (_REGEX_ACTION_VALID & pa->flags)); pa = pa->next) {
        rv = _regex_tr3_policy_action_get(unit, policy_mem, p_ent,
                                          pa, e);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Handle color dependence/independence */
    if (soc_mem_field_valid(unit, policy_mem, GREEN_TO_PIDf)) {
        soc_mem_field32_set(unit, policy_mem, e, GREEN_TO_PIDf,
                            (p_ent->flags & _REGEX_POLICY_COLOR_INDEPENDENT) ? 1 : 0);
    }

    /* Extract meter related policy fields */
    rv = _regex_tr3_policer_action_set(unit, p_ent, e);
    BCM_IF_ERROR_RETURN(rv);

#ifdef ESW_REGEX_STAT_SUPPORT
    /* Extract counter related policy fields */
    rv = _bcm_field_tr3_stat_action_set(unit,
                                        p_ent,
                                        policy_mem,
                                        tcam_idx,
                                        e);
    BCM_IF_ERROR_RETURN(rv);
#endif  /* ESW_REGEX_STAT_SUPPORT */

    /* Write the POLICY Table */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, p_ent->eid, e);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _regex_tr3_policy_remove
 * Purpose:
 *     Remove a previously installed physical entry.
 * Parameters:
 *     unit   - BCM device number
 *     p_ent  - Physical policy structure to be removed
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     Regex unit lock should be held by calling function.
 */
STATIC int _regex_tr3_policy_remove(int unit, _regex_policy_t *p_ent)
{
    soc_mem_t policy_mem = FT_POLICYm;  /* Policy table memory id.           */

    if (NULL == p_ent) {
        return (BCM_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, p_ent->eid,
                                      soc_mem_entry_null(unit, policy_mem)));
    p_ent->flags |= _REGEX_POLICY_DIRTY; /* Mark entry as not installed */

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_tr3_policer_install
 * Purpose:
 *     Install a meter pair into the hardware tables.
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     f_ent  - (IN) Field entry.
 *     f_pl   - (IN) Field policer descriptor.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_tr3_policer_install(int unit, _field_policer_t *f_pl)
{
    uint32    bucketsize_peak = 0;      /* Bucket size.             */
    uint32    refresh_rate_peak = 0;    /* Policer refresh rate.    */
    uint32    granularity_peak = 0;     /* Policer granularity.     */
    uint32    bucketsize_commit = 0;    /* Bucket size.             */
    uint32    refresh_rate_commit = 0;  /* Policer refresh rate.    */
    uint32    granularity_commit = 0;   /* Policer granularity.     */
    int       refresh_bitsize;          /* Number of bits for the
                                           refresh rate field.      */
    int       bucket_max_bitsize;       /* Number of bits for the
                                           bucket max field.        */
    int       rv = BCM_E_NONE;          /* Operation return status. */
    soc_mem_t meter_table;              /* Meter table name.        */
    uint32    flags;                    /* Policer flags.           */

    /* Input parameters check. */
    if (NULL == f_pl) {
        return (BCM_E_PARAM);
    }

    if (0 == (f_pl->hw_flags & _FP_POLICER_DIRTY)) {
        return (BCM_E_NONE);
    }

    /* Resolve meter table name. */
    meter_table = FP_METER_TABLEm;

    refresh_bitsize = soc_mem_field_length(unit, meter_table, REFRESHCOUNTf);
    bucket_max_bitsize = soc_mem_field_length(unit, meter_table, BUCKETSIZEf);

    /* lookup bucket size from tables */
    flags = _BCM_XGS_METER_FLAG_GRANULARITY | _BCM_XGS_METER_FLAG_FP_POLICER;

    /* Set packet mode flags setting */
    if (f_pl->cfg.flags & BCM_POLICER_MODE_PACKETS) {
        flags |= _BCM_XGS_METER_FLAG_PACKET_MODE;
    } else {
        flags &= ~_BCM_XGS_METER_FLAG_PACKET_MODE;
    }

    if (f_pl->cfg.mode  != bcmPolicerModeSrTcm) {
        if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.ckbits_sec,
                                                   f_pl->cfg.ckbits_burst,
                                                   flags, refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_commit,
                                                   &bucketsize_commit,
                                                   &granularity_commit);
            /* Program policer parameters into hw. */
            rv =  _regex_tr3_policer_hw_update(unit, f_pl,
                                               BCM_FIELD_METER_COMMITTED,
                                               bucketsize_commit,
                                               refresh_rate_commit,
                                               granularity_commit, meter_table);

            f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;
        }

        if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {

            if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                /*
                 * Rates are always set in committed variables,
                 * for flow meters.
                 */
                f_pl->cfg.pkbits_sec = f_pl->cfg.ckbits_sec;
                f_pl->cfg.pkbits_burst = f_pl->cfg.ckbits_burst;
            }

            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.pkbits_sec,
                                                   f_pl->cfg.pkbits_burst,
                                                   flags,
                                                   refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_peak,
                                                   &bucketsize_peak,
                                                   &granularity_peak);
            /* Programm policer parameters into hw. */
            rv =  _regex_tr3_policer_hw_update(unit, f_pl,
                                               BCM_FIELD_METER_PEAK,
                                               bucketsize_peak, refresh_rate_peak,
                                               granularity_peak, meter_table);

            f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY;

            if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                /* Reset peak meter rates. */
                f_pl->cfg.pkbits_sec = 0;
                f_pl->cfg.pkbits_burst = 0;
            }
        }
    } else {
        if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.ckbits_sec,
                                                   f_pl->cfg.ckbits_burst,
                                                   flags, refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_commit,
                                                   &bucketsize_commit,
                                                   &granularity_commit);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.pkbits_sec,
                                                   f_pl->cfg.pkbits_burst,
                                                   flags,
                                                   refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_peak,
                                                   &bucketsize_peak,
                                                   &granularity_peak);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (granularity_commit != granularity_peak) {
            if (granularity_commit < granularity_peak) {
                rv = _bcm_xgs_kbits_to_dual_bucket_encoding(f_pl->cfg.ckbits_sec,
                                                            f_pl->cfg.ckbits_burst,
                                                            flags,
                                                            refresh_bitsize,
                                                            bucket_max_bitsize,
                                                            granularity_peak,
                                                            &refresh_rate_commit,
                                                            &bucketsize_commit,
                                                            &granularity_commit);
            } else if (granularity_commit > granularity_peak) {
                rv = _bcm_xgs_kbits_to_dual_bucket_encoding(f_pl->cfg.pkbits_sec,
                                                            f_pl->cfg.pkbits_burst,
                                                            flags,
                                                            refresh_bitsize,
                                                            bucket_max_bitsize,
                                                            granularity_commit,
                                                            &refresh_rate_peak,
                                                            &bucketsize_peak,
                                                            &granularity_peak);
            }
            BCM_IF_ERROR_RETURN(rv);
        }
        /* Program policer parameters into hw. */
        rv =  _regex_tr3_policer_hw_update(unit, f_pl,
                                           BCM_FIELD_METER_COMMITTED,
                                           bucketsize_commit,
                                           refresh_rate_commit,
                                           granularity_commit, meter_table);

        f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;

        /* Program policer parameters into hw. */
        rv =  _regex_tr3_policer_hw_update(unit, f_pl,
                                           BCM_FIELD_METER_PEAK,
                                           bucketsize_peak, refresh_rate_peak,
                                           granularity_peak, meter_table);

        f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY;
    }
    return rv;
}

#define TR3_REGEX_DUMP_DFA_MEM_OFFSET 1

void _bcm_regex_dump_dfa(int unit, bcm_regex_engine_t engid)
{
    _bcm_regex_lengine_t *lengine = NULL;
    _bcm_regex_engine_obj_t *pengine = NULL;
    axp_sm_state_table_mem0_entry_t *state_storage;
    soc_mem_t   mem;
    int     i, j, num_states, num_final;
    uint8 fc, tc;
    int match_index;
    uint32 jump_offset, jump_offset_last;
    int *jump_table, this_state, base, print_id;
    int jump_table_size;

    if (_bcm_tr3_find_engine(unit, engid, &lengine) || 
        ((pengine = lengine->physical_engine) == NULL)) {
        return;
    }

    if (pengine->from_line < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "Invalid DFA state memory offset of allocated engine.\n")));
        return;
    }

    base = pengine->from_line*BCM_TR3_NUM_RV_PER_LINE;
    jump_table_size = pengine->req_lsz * BCM_TR3_NUM_RV_PER_LINE;

    mem = _bcm_tr3_get_axp_sm_memory(unit, pengine->tile);
    state_storage = sal_alloc(sizeof(axp_sm_state_table_mem0_entry_t) * pengine->req_lsz,
                        "dfa_cpd");
    if (NULL == state_storage) {
        LOG_CLI((BSL_META_U(unit,
                            "Unable to allocate memory storage.\n")));
        return;
    }

    _bcm_tr3_regex_read_hw_dfa(unit, pengine, state_storage);
 
    jump_table = sal_alloc(sizeof(jump_table[0]) * jump_table_size,
                     "re_jump_table");
    if (NULL == jump_table) {
        LOG_CLI((BSL_META_U(unit,
                            "Unable to allocate jump table storage.\n")));
        return;
    }

    for (i = 0; i < jump_table_size; i++) {
        jump_table[i] = -1;
    }

    /*
     * Initialize the jump table.  The jump table will hold the state number of each
     * destination of a jump.  This table is processed in two steps.  Step 1 is to mark
     * all the locations that are targets of jumps.  Step 2 is to assign state numbers.
     * This is done in two steps since it is possible that state numbers will be assigned
     * out of order if a single pass is used.
     */
    /*
     * First, mark all locations that are targets of jumps.
     */
    jump_offset_last = 0;
    for (i = 0; i < pengine->req_lsz; i++) {
        for (j = 0; j < BCM_TR3_NUM_RV_PER_LINE; j++) {
            _bcm_tr3_regex_rv_get(unit, mem, &state_storage[i], &fc, &tc,
                                  j, &jump_offset, &match_index);
            if ((match_index < 0) && (jump_offset != 0xffff)) {
                if ((jump_offset - base) >= jump_table_size) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Jump offset %d (0x%x) out of range.\n"),
                             jump_offset, jump_offset));
                } else if (jump_table[jump_offset - base] == -1) {
                    jump_table[jump_offset - base] = 0;
                }
            }

            /*
            Show all states, even if not jumped-to.  This code should
            not be necessary, especially with the changes to remove
            transitions from final states.  Also, when removing the
            extra jump to 0xffff from every state that already has
            transitions for every possible character, this code will
            not work for states following non-final states anyway.
            */
            if (jump_offset_last == 0xffff) {
                if (jump_table[(i * BCM_TR3_NUM_RV_PER_LINE)+ j] == -1){
                    if (jump_offset != 0xffff || match_index >= 0) {
                        jump_table[(i * BCM_TR3_NUM_RV_PER_LINE)+ j] = 0;
                    }
                }
            }

            jump_offset_last = jump_offset;
        }
    }

    /* Now assign state numbers. */
    /* Always begin the DFA with state 0. */
    jump_table[0] = 0;
    num_states = 1;
    for (i = 1; i < jump_table_size; i++) {
        if (-1 != jump_table[i]) {
            jump_table[i] = num_states;
            num_states++;
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n------------------DFA-(Tile:%d)---------------\n\n"),
             pengine->tile));
    LOG_CLI((BSL_META_U(unit,
                        "Number of lines = %d\nNumber of states = %d\n"),
             pengine->req_lsz, num_states));
    this_state = 0;
    print_id = 0;
    num_final = 0;
    for (i = 0; i < pengine->req_lsz; i++) {
        for (j=0; j<BCM_TR3_NUM_RV_PER_LINE; j++) {
            /*
             * If this location is the target of a jump, set the flag to print the state number.
             */
            if (jump_table[(i * BCM_TR3_NUM_RV_PER_LINE) + j] != -1) {
                this_state = jump_table[(i * BCM_TR3_NUM_RV_PER_LINE) + j];
                print_id = 1;
            }
            
            _bcm_tr3_regex_rv_get(unit, mem, &state_storage[i], &fc, &tc,
                                  j, &jump_offset, &match_index);

            if (print_id) {
#if TR3_REGEX_DUMP_DFA_MEM_OFFSET==0
                LOG_CLI((BSL_META_U(unit,
                                    "\nState %d %s\n"),
                         (match_index >= 0) ? this_state, "[FINAL]" : ""));
#else
                LOG_CLI((BSL_META_U(unit,
                                    "\nState %d %s @line=%d, line offset=%d "
                                    "full offset (0x%x)\n"),
                         this_state, (match_index >= 0) ? "[FINAL]" : "",
                         pengine->from_line + i, j,
                         (pengine->from_line + i) *
                         BCM_TR3_NUM_RV_PER_LINE + j));
#endif
                if (match_index >= 0) {
                    num_final++;
                    LOG_CLI((BSL_META_U(unit,
                                        "\tMatch ID = %d, %s\n"),
                             match_index,
                             ((tc == 2) ? "IDLE STOP" :
                             ((tc == 1) ? "IDLE CONT" : ""))));
                }
                print_id = 0;
            }
            if (jump_offset != 0xffff) {
                LOG_CLI((BSL_META_U(unit,
                                    "\t %4d -> %4d : "),
                         this_state, jump_table[jump_offset - base]));
                if ((tc == 0) && (fc == 255)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "[\\%-3d-\\%-3d], "), fc, tc));
                    LOG_CLI((BSL_META_U(unit,
                                        "Same data offset;\n")));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "[\\%-3d-\\%-3d]"), fc, tc));
                    if (is_printable(fc) && is_printable(tc)) {
                        LOG_CLI((BSL_META_U(unit,
                                            " (%2c-%-2c)"), fc, tc));
                    } else if (is_printable(fc)) {
                        LOG_CLI((BSL_META_U(unit,
                                            " (%2c-'')"), fc));
                    } else if (is_printable(tc)) {
                        LOG_CLI((BSL_META_U(unit,
                                            " (''-%-2c)"), tc));
                    }

                    LOG_CLI((BSL_META_U(unit,
                                        ";\n")));
            }
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "\t %4d -> 0xffff (IDLE) : "), this_state));
                LOG_CLI((BSL_META_U(unit,
                                    "[\\%-3d-\\%-3d]"), fc, tc));
                if ((fc == 255) && ((tc == 1) || (tc == 2))) {
                    LOG_CLI((BSL_META_U(unit,
                                        " Match Id %d"), match_index));
                }
                LOG_CLI((BSL_META_U(unit,
                                    ";\n")));
            }
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n--------------Number of final states %d-----------\n\n"),
             num_final));
    sal_free(state_storage);
    sal_free(jump_table);
    return; 
}

void _bcm_regex_sw_dump_engine(int unit, bcm_regex_engine_t engid,
                                uint32 flags)
{
    _bcm_regex_lengine_t *lengine = NULL;
    _bcm_regex_engine_obj_t *pengine = NULL;

    if (_bcm_tr3_find_engine(unit, engid, &lengine) || 
        ((pengine = lengine->physical_engine) == NULL)) {
        return;
    }

    /* dump engine info */
    LOG_CLI((BSL_META_U(unit,
                        "Regex Tile %d, Engine %d : "
                        "Start offset:%04d Size (lines):%04d/%04d\n"),
             pengine->tile, pengine->hw_idx,
             pengine->from_line, pengine->req_lsz, pengine->total_lsz));
    LOG_CLI((BSL_META_U(unit,
                        "          CPS   :%s\n"), 
             BCM_REGEX_CPS_ALLOCED(&pengine->cps) ? "Yes" : "No"));
    if (BCM_REGEX_CPS_ALLOCED(&pengine->cps)) {
        LOG_CLI((BSL_META_U(unit,
                            " From line  : %04d Size (lines):%04d\n"),
                 pengine->cps.from_line, pengine->cps.req_lsz));
    }
    if (flags) {
        _bcm_regex_dump_dfa(unit, engid);
    }
}

void _bcm_regex_sw_load_engine(int unit, bcm_regex_engine_t engid,
                                uint32 flags, int *eng_hwidx)
{
    _bcm_regex_lengine_t *lengine = NULL;
    _bcm_regex_engine_obj_t *pengine = NULL;

    if (engid == BCM_REGEX_ENGINE_ALL) {
        return;
    }

    if (_bcm_tr3_find_engine(unit, engid, &lengine) ||
        ((pengine = lengine->physical_engine) == NULL)) {
        return;
    }

    if (eng_hwidx) {
        *eng_hwidx = pengine->hw_idx;
    }
}

void _bcm_regex_sw_engine_hwid(int unit, bcm_regex_engine_t engid,
                                int *enghwid)
{
    _bcm_regex_lengine_t *lengine = NULL;
    _bcm_regex_engine_obj_t *pengine = NULL;

    if (_bcm_tr3_find_engine(unit, engid, &lengine) ||
        ((pengine = lengine->physical_engine) == NULL)) {
        return;
    }

    if (enghwid != NULL) {
        *enghwid = pengine->hw_idx;
    }
}

void _bcm_regex_sw_dump(int unit)
{
    _bcm_tr3_regex_device_info_t *device;
    int i, j, nept, count;
    _bcm_regex_engine_obj_t *pengine = NULL;
    _bcm_regex_lengine_t *tmp;
    
    device = REGEX_INFO(unit);

    nept = BCM_TR3_ENGINE_PER_TILE(device);

    for (count = 0, i=0; i<device->info->num_tiles; i++, count = 0) {
        for (j=0; j<nept; j++) {
            pengine = BCM_TR3_PHYSICAL_ENGINE(device, (i*nept) + j);
            if (!BCM_REGEX_ENGINE_ALLOCATED(pengine)) {
                continue;
            }
            count++;
        }
        if (count == 0) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "Tile %02d: %0d Engines in use.\n"), i, count));
    }

    for (i = 0; i < BCM_TR3_NUM_ENGINE(device); i++) {
        tmp = BCM_TR3_LOGICAL_ENGINE(device, i);
        if (!BCM_REGEX_ENGINE_ALLOCATED(tmp)) {
            continue;
        }
        _bcm_regex_sw_dump_engine(unit, tmp->id, 0);
    }
}

#endif /* INCLUDE_REGEX */

#endif  /* BCM_TRIUMPH3_SUPPORT */
